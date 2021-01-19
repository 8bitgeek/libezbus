/*****************************************************************************
* Copyright © 2019-2020 Mike Sharkey <mike@8bitgeek.net>                     *
*                                                                            *
* Permission is hereby granted, free of charge, to any person obtaining a    *
* copy of this software and associated documentation files (the "Software"), *
* to deal in the Software without restriction, including without limitation  *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,   *
* and/or sell copies of the Software, and to permit persons to whom the      *
* Software is furnished to do so, subject to the following conditions:       *
*                                                                            *
* The above copyright notice and this permission notice shall be included in *
* all copies or substantial portions of the Software.                        *
*                                                                            *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    *
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    *
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        *
* DEALINGS IN THE SOFTWARE.                                                  *
*****************************************************************************/
#include <ezbus_mac_arbiter.h>
#include <ezbus_mac_arbiter_transmit.h>
#include <ezbus_mac_arbiter_receive.h>
#include <ezbus_mac_transmitter.h>
#include <ezbus_mac_receiver.h>
#include <ezbus_mac_token.h>
#include <ezbus_mac_boot0.h>
#include <ezbus_mac_boot1.h>
#include <ezbus_mac_boot2.h>
#include <ezbus_mac_peers.h>
#include <ezbus_socket_callback.h>
#include <ezbus_crc.h>
#include <ezbus_hex.h>
#include <ezbus_log.h>
#include <ezbus_pause.h>


static ezbus_mac_arbiter_t ezbus_mac_arbiter_stack[EZBUS_MAC_STACK_SIZE];

static void do_mac_arbiter_state_offline        ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_reboot_cold    ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_reboot_warm    ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_coldboot       ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_boot2          ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_service_start  ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_online         ( ezbus_mac_t* mac );

static void do_mac_arbiter_state_pause_broadcast_start     ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_pause_broadcast_continue  ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_pause_broadcast_finish    ( ezbus_mac_t* mac );

static void do_mac_arbiter_state_pause_receive_start       ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_pause_receive_continue    ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_pause_receive_finish      ( ezbus_mac_t* mac );

static void do_mac_arbiter_state_pause_start               ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_pause_continue            ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_pause_finish              ( ezbus_mac_t* mac );

static void ezbus_mac_arbiter_receive_token     ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static void ezbus_mac_arbiter_ack_parcel        ( ezbus_mac_t* mac, uint8_t seq, ezbus_address_t* address );
static void ezbus_mac_arbiter_nack_parcel       ( ezbus_mac_t* mac, uint8_t seq, ezbus_address_t* address );
static void ezbus_mac_arbiter_pause_packet      ( ezbus_mac_t* mac, ezbus_packet_t* packet, const ezbus_address_t* address );

static void ezbus_mac_pause_timer_callback      ( ezbus_timer_t* timer, void* arg);
static void ezbus_mac_pause_half_timer_callback ( ezbus_timer_t* timer, void* arg);

extern void  ezbus_mac_arbiter_init ( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );

    ezbus_platform_memset( arbiter, 0 , sizeof( ezbus_mac_arbiter_t) );
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_offline );
    ezbus_mac_arbiter_rst_boot2_cycles( mac );
}

extern void ezbus_mac_arbiter_run( ezbus_mac_t* mac )
{
    switch( ezbus_mac_arbiter_get_state( mac ) )
    {
        case mac_arbiter_state_offline:
            do_mac_arbiter_state_offline( mac );
            break;
        case mac_arbiter_state_reboot_cold:
            do_mac_arbiter_state_reboot_cold( mac );
            break;
        case mac_arbiter_state_reboot_warm:
            do_mac_arbiter_state_reboot_warm( mac );
            break;
        case mac_arbiter_state_coldboot:
            do_mac_arbiter_state_coldboot( mac );
            break;
        case mac_arbiter_state_boot2:
            do_mac_arbiter_state_boot2( mac );
            break;                   
        case mac_arbiter_state_service_start:
            do_mac_arbiter_state_service_start( mac );
            break;
        case mac_arbiter_state_online:
            ezbus_socket_callback_run( mac );
            do_mac_arbiter_state_online( mac );
            break;               

        case mac_arbiter_state_pause_broadcast_start:
            do_mac_arbiter_state_pause_broadcast_start( mac );
            break;
        case mac_arbiter_state_pause_broadcast_continue:
            do_mac_arbiter_state_pause_broadcast_continue( mac );
            break;
        case mac_arbiter_state_pause_broadcast_finish:
            do_mac_arbiter_state_pause_broadcast_finish( mac );
            break;

        case mac_arbiter_state_pause_receive_start:
            do_mac_arbiter_state_pause_receive_start( mac );
            break;
        case mac_arbiter_state_pause_receive_continue:
            do_mac_arbiter_state_pause_receive_continue( mac );
            break;
        case mac_arbiter_state_pause_receive_finish:
            do_mac_arbiter_state_pause_receive_finish( mac );
            break;

        case mac_arbiter_state_pause_start:
            do_mac_arbiter_state_pause_start( mac );
            break;
        case mac_arbiter_state_pause_continue:
            do_mac_arbiter_state_pause_continue( mac );
            break;
        case mac_arbiter_state_pause_finish:
            do_mac_arbiter_state_pause_finish( mac );
            break;
    }
}

extern bool ezbus_mac_arbiter_pause_ready( ezbus_mac_t* mac )
{
    if ( ezbus_mac_token_acquired( mac ) )
    {
        if ( ezbus_mac_transmitter_empty( mac ) )
        {
            if ( !ezbus_mac_arbiter_transmit_busy( mac ) )
            {
                return true;
            }
        }
    }
    
    return false;
}

extern void ezbus_mac_arbiter_set_callback( ezbus_mac_t* mac, ezbus_mac_arbiter_callback_t callback )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    arbiter->callback = callback;
}

static void ezbus_mac_pause_timer_callback( ezbus_timer_t* timer, void* arg)
{
    ezbus_mac_t* mac = (ezbus_mac_t*)arg;
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    
    EZBUS_LOG( EZBUS_LOG_ARBITER, "" );
    
    if ( arbiter->callback )
    {
        arbiter->callback( mac, mac_arbiter_callback_reason_pause_timer_expired );
    }
    
    ezbus_timer_stop( ezbus_mac_arbiter_get_pause_timer( mac ) );

    if ( ezbus_mac_arbiter_get_state( mac ) == mac_arbiter_state_pause_continue )
    {   
        ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_pause_finish );
    }
    else if ( ezbus_mac_arbiter_get_state( mac ) == mac_arbiter_state_pause_receive_continue )
    {
        ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_pause_receive_finish );
    }
}

static void ezbus_mac_pause_half_timer_callback( ezbus_timer_t* timer, void* arg)
{
    ezbus_mac_t* mac = (ezbus_mac_t*)arg;
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );

    EZBUS_LOG( EZBUS_LOG_ARBITER, "" );

    if ( arbiter->callback )
    {
        arbiter->callback( mac, mac_arbiter_callback_reason_pause_half_timer_expired );
    }

    ezbus_timer_stop( ezbus_mac_arbiter_get_pause_half_timer( mac ) );
}

static void do_mac_arbiter_state_pause_broadcast_start( ezbus_mac_t* mac )
{
    if ( ezbus_mac_arbiter_pause_ready( mac ) )
    {
        ezbus_packet_t packet;

        EZBUS_LOG( EZBUS_LOG_ARBITER, "" );

        ezbus_mac_arbiter_pause_packet  ( mac, &packet, &ezbus_broadcast_address );
        ezbus_pause_set_active          ( ezbus_packet_get_pause ( &packet ), true );
        ezbus_pause_set_duration        ( ezbus_packet_get_pause ( &packet ), ezbus_mac_arbiter_get_pause_duration( mac ) );
        ezbus_mac_transmitter_put       ( mac, &packet );
        ezbus_mac_arbiter_set_state     ( mac, mac_arbiter_state_pause_broadcast_continue );
    }
}

static void do_mac_arbiter_state_pause_broadcast_continue( ezbus_mac_t* mac )
{
    if ( ezbus_mac_transmitter_empty( mac ) )
    {
        ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_pause_broadcast_finish );
    }
}

static void do_mac_arbiter_state_pause_broadcast_finish( ezbus_mac_t* mac )
{
    ezbus_timer_t* timer = ezbus_mac_arbiter_get_pause_timer( mac );

    EZBUS_LOG( EZBUS_LOG_ARBITER, "" );

    ezbus_mac_timer_setup           ( mac, timer, false );
    ezbus_timer_set_key             ( timer, "pause_timer" );
    ezbus_timer_set_period          ( timer, ezbus_mac_arbiter_get_pause_duration( mac ) );
    ezbus_timer_set_callback        ( timer, ezbus_mac_pause_timer_callback, mac );
    ezbus_timer_start               ( timer );

    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_pause_start );
}

static void do_mac_arbiter_state_pause_receive_start( ezbus_mac_t* mac )
{
    ezbus_timer_t* timer = ezbus_mac_arbiter_get_pause_timer( mac );
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );

    EZBUS_LOG( EZBUS_LOG_ARBITER, "" );

    ezbus_mac_timer_setup           ( mac, timer, false );
    ezbus_timer_set_key             ( timer, "pause_timer" );
    ezbus_timer_set_period          ( timer, ezbus_mac_arbiter_get_pause_duration( mac ) );
    ezbus_timer_set_callback        ( timer, ezbus_mac_pause_timer_callback, mac );         /* FIXME */
    ezbus_timer_start               ( timer );

    if ( arbiter->callback )
    {
        arbiter->callback( mac, mac_arbiter_callback_reason_pause_timer_start );
    }

    ezbus_timers_set_pause_active( mac, true );
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_pause_receive_continue );
}

static void do_mac_arbiter_state_pause_receive_continue( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    if ( arbiter->callback )
    {
        arbiter->callback( mac, mac_arbiter_callback_reason_pause_timer_continue );
    }
}

static void do_mac_arbiter_state_pause_receive_finish( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );

    EZBUS_LOG( EZBUS_LOG_ARBITER, "" );

    if ( arbiter->callback )
    {
        arbiter->callback( mac, mac_arbiter_callback_reason_pause_timer_finish );
    }

    ezbus_timers_set_pause_active( mac, false );
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_online );
}

static void do_mac_arbiter_state_pause_start( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_timer_t* timer = ezbus_mac_arbiter_get_pause_half_timer( mac );

    EZBUS_LOG( EZBUS_LOG_ARBITER, "" );

    ezbus_mac_timer_setup   ( mac, timer, false );
    ezbus_timer_set_key     ( timer, "pause_half_timer" );
    ezbus_timer_set_period  ( timer, ezbus_mac_arbiter_get_pause_duration( mac )/2 );
    ezbus_timer_set_callback( timer, ezbus_mac_pause_half_timer_callback, mac );
    ezbus_timer_start       ( timer );

    if ( arbiter->callback )
    {
        arbiter->callback( mac, mac_arbiter_callback_reason_pause_timer_start );
    }

    ezbus_timers_set_pause_duration( mac, ezbus_mac_arbiter_get_pause_duration( mac ) );
    ezbus_timers_set_pause_active( mac, true );

    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_pause_continue );
}

static void do_mac_arbiter_state_pause_continue( ezbus_mac_t* mac )
{
    /* NOTE wait for pause callback to change state */
}

static void do_mac_arbiter_state_pause_finish( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    
    EZBUS_LOG( EZBUS_LOG_ARBITER, "" );

    if ( arbiter->callback )
    {
        arbiter->callback( mac, mac_arbiter_callback_reason_pause_timer_finish );
    }
        
    ezbus_timers_set_pause_active( mac, false );

    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_online );
}

extern void ezbus_mac_arbiter_set_pause_duration( ezbus_mac_t* mac, ezbus_ms_tick_t duration )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    arbiter->pause_duration = duration;
}

extern ezbus_ms_tick_t ezbus_mac_arbiter_get_pause_duration( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    return arbiter->pause_duration;
}

extern ezbus_timer_t* ezbus_mac_arbiter_get_pause_timer( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    return &arbiter->pause_timer;
}

extern ezbus_timer_t* ezbus_mac_arbiter_get_pause_half_timer  ( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    return &arbiter->pause_half_timer;
}

extern bool ezbus_mac_arbiter_pause( ezbus_mac_t* mac, ezbus_ms_tick_t duration )
{
    if ( ezbus_mac_arbiter_pause_ready( mac ) )
    {
        EZBUS_LOG( EZBUS_LOG_ARBITER, "state=%d", ezbus_mac_arbiter_get_state( mac ) );

        ezbus_mac_arbiter_set_pause_duration( mac, duration );
        ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_pause_broadcast_start );
        return true;
    }
    return false;
}

static void ezbus_mac_arbiter_pause_packet( ezbus_mac_t* mac, ezbus_packet_t* packet, const ezbus_address_t* address )
{
        ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );

        ezbus_packet_init           ( packet );
        ezbus_packet_set_type       ( packet, packet_type_pause );
        ezbus_packet_set_dst_socket ( packet, arbiter->rx_ack_src_socket );
        ezbus_packet_set_src_socket ( packet, arbiter->rx_ack_dst_socket );
        ezbus_packet_set_src        ( packet, &ezbus_self_address );
        ezbus_packet_set_dst        ( packet, address );
}

extern void ezbus_mac_arbiter_push( ezbus_mac_t* mac, uint8_t level )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_platform_memcpy(&ezbus_mac_arbiter_stack,arbiter,sizeof(ezbus_mac_arbiter_t));
}

extern void ezbus_mac_arbiter_pop( ezbus_mac_t* mac, uint8_t level )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_platform_memcpy(arbiter,&ezbus_mac_arbiter_stack,sizeof(ezbus_mac_arbiter_t));
}


extern uint16_t ezbus_mac_arbiter_get_token_age( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    return arbiter->token_age;
}

extern void ezbus_mac_arbiter_set_token_age( ezbus_mac_t* mac, uint16_t age )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    arbiter->token_age = age;
}


extern void ezbus_mac_arbiter_set_state ( ezbus_mac_t* mac, ezbus_mac_arbiter_state_t state )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    arbiter->state = state;
}

extern ezbus_mac_arbiter_state_t ezbus_mac_arbiter_get_state ( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    return arbiter->state;
}

extern uint8_t ezbus_mac_arbiter_get_boot2_cycles( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    return arbiter->boot2_cycles;
}

extern void ezbus_mac_arbiter_set_boot2_cycles( ezbus_mac_t* mac, uint8_t cycles )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    arbiter->boot2_cycles = cycles;
}

extern void ezbus_mac_arbiter_dec_boot2_cycles( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    if ( arbiter->boot2_cycles )
    {
        --arbiter->boot2_cycles;
    }
}


extern void ezbus_mac_arbiter_rst_boot2_cycles( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    arbiter->boot2_cycles = EZBUS_BOOT2_CYCLES;
}


static void do_mac_arbiter_state_offline( ezbus_mac_t* mac )
{
    ezbus_mac_token_relinquish( mac );
}

static void do_mac_arbiter_state_reboot_cold( ezbus_mac_t* mac )
{
   EZBUS_LOG( EZBUS_LOG_ARBITER, "" );
   ezbus_mac_token_relinquish( mac );
   ezbus_mac_boot2_set_state( mac, state_boot2_idle );
   ezbus_mac_coldboot_reset( mac );
   ezbus_mac_arbiter_set_state( mac , mac_arbiter_state_offline );
}

static void do_mac_arbiter_state_reboot_warm( ezbus_mac_t* mac )
{
   EZBUS_LOG( EZBUS_LOG_ARBITER, "" );
   ezbus_mac_peers_clear( mac );
   ezbus_mac_token_relinquish( mac );
   ezbus_mac_boot2_set_state( mac, state_boot2_start );
   ezbus_mac_coldboot_reset( mac );
   ezbus_mac_arbiter_set_state( mac , mac_arbiter_state_offline );
}

static void do_mac_arbiter_state_coldboot( ezbus_mac_t* mac )
{
    EZBUS_LOG( EZBUS_LOG_ARBITER, "" );
    ezbus_mac_boot2_set_state( mac, state_boot2_start );
    ezbus_mac_token_relinquish( mac );
}

static void do_mac_arbiter_state_boot2( ezbus_mac_t* mac )
{
    EZBUS_LOG( EZBUS_LOG_ARBITER, "" );
    ezbus_mac_token_reset( mac );
}

static void do_mac_arbiter_state_service_start( ezbus_mac_t* mac )
{
    EZBUS_LOG( EZBUS_LOG_ARBITER, "" );
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_online );
}

static void do_mac_arbiter_state_online( ezbus_mac_t* mac )
{
    if ( ezbus_mac_token_acquired( mac ) )
    {
        ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );

        EZBUS_LOG( EZBUS_LOG_ONLINE, "rx_ack_pend %d rx_nack_pend %d", arbiter->rx_ack_pend, arbiter->rx_nack_pend );

        if ( ezbus_mac_transmitter_empty( mac ) )
        {
            if ( arbiter->token_hold++ > EZBUS_TOKEN_HOLD_CYCLES )
            {
                ezbus_mac_arbiter_transmit_token( mac );
                ezbus_mac_token_relinquish( mac );
            }
        }

        if ( ezbus_mac_transmitter_empty( mac ) )
        {
            if ( arbiter->rx_ack_pend )
            {
                ezbus_mac_arbiter_ack_parcel( mac, arbiter->rx_ack_seq, &arbiter->rx_ack_address );
                arbiter->rx_ack_pend=false;
                arbiter->rx_ack_seq=0;
            }
            else
            if ( arbiter->rx_nack_pend )
            {
                ezbus_mac_arbiter_nack_parcel( mac, arbiter->rx_nack_seq, &arbiter->rx_nack_address );
                arbiter->rx_nack_pend=false;
                arbiter->rx_nack_seq=0;
            }
        }

        if ( ezbus_mac_transmitter_empty( mac ) )
        {
            if ( !ezbus_mac_arbiter_transmit_busy( mac ) )
            {
                EZBUS_LOG( EZBUS_LOG_ONLINE, "online tx parcel" );
                ezbus_socket_callback_transmitter_empty( mac ); 
            }
        }
    }
}


/**
 * @brief A give-token packet has been received
 */
extern void ezbus_mac_arbiter_receive_signal_token ( ezbus_mac_t* mac, ezbus_packet_t* packet )
{

    ezbus_mac_token_reset( mac );
    
    if ( ezbus_address_is_self( ezbus_packet_dst( packet ) ) )
    {
        ezbus_mac_arbiter_receive_token( mac, packet );
        if ( ezbus_mac_arbiter_get_state( mac ) != mac_arbiter_state_online )
        {
            ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_service_start );
        }
    }
}

static void ezbus_mac_arbiter_receive_token( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_crc_t crc;

    arbiter->token_hold=0;
    ezbus_mac_peers_crc( mac, &crc );
    ezbus_mac_arbiter_set_token_age( mac, ezbus_packet_get_token_age(packet) );
    if ( ezbus_crc_equal( &crc, ezbus_packet_get_token_crc( packet ) ) )
    {

        if ( ezbus_mac_arbiter_get_state( mac ) ==  mac_arbiter_state_pause_receive_continue )
        {
            ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_pause_receive_finish );
        }

        ezbus_mac_token_acquire( mac );

        if ( ezbus_packet_get_token_age(packet) > EZBUS_BOOT2_AGE )
        {
            ezbus_mac_arbiter_set_token_age( mac, 0 );
            ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_reboot_warm );
            //do_mac_arbiter_state_reboot_warm( mac );
            EZBUS_LOG( EZBUS_LOG_SOCKET, "initiate mac_arbiter_state_reboot_warm - token age" );
            ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_reboot_warm );
        }
        else
        {
            if ( ezbus_mac_boot2_get_state( mac ) != state_boot2_idle )
            {
                ezbus_mac_boot2_set_state( mac, state_boot2_finished );
            }
        }
        
    }
    else
    {
        EZBUS_LOG( EZBUS_LOG_SOCKET, "initiate mac_arbiter_state_reboot_warm - token crc" );
        ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_reboot_warm );
    }
}




/** BEGIN RECEIVE PARCEL **/

extern void ezbus_mac_arbiter_receive_signal_parcel( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    if ( ezbus_address_is_self( ezbus_packet_dst( packet ) ) )
    {
        ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
        ezbus_packet_t* rx_packet = ezbus_mac_get_receiver_packet( mac );

        EZBUS_LOG( EZBUS_LOG_RECEIVER, "" );
    
        if ( !arbiter->rx_ack_pend && !arbiter->rx_nack_pend )
        {
            if ( ezbus_socket_callback_receiver_ready( mac, packet ) )
            {
                arbiter->rx_ack_pend = (ezbus_packet_src_socket( rx_packet ) == EZBUS_SOCKET_INVALID) ? false : true;
                arbiter->rx_ack_seq = ezbus_packet_seq( rx_packet );
                arbiter->rx_ack_dst_socket = ezbus_packet_dst_socket( rx_packet );
                arbiter->rx_ack_src_socket = ezbus_packet_src_socket( rx_packet );
                ezbus_address_copy( &arbiter->rx_ack_address, ezbus_packet_src( rx_packet ) );
            }
            else
            {
                arbiter->rx_nack_pend = (ezbus_packet_src_socket( rx_packet ) == EZBUS_SOCKET_INVALID) ? false : true;
                arbiter->rx_nack_seq = ezbus_packet_seq( rx_packet );         
                arbiter->rx_nack_dst_socket = ezbus_packet_dst_socket( rx_packet );
                arbiter->rx_nack_src_socket = ezbus_packet_src_socket( rx_packet );
                ezbus_address_copy( &arbiter->rx_nack_address, ezbus_packet_src( rx_packet ) );
            }
        }
    }
}

static void ezbus_mac_arbiter_ack_parcel( ezbus_mac_t* mac, uint8_t seq, ezbus_address_t* address )
{
    ezbus_packet_t tx_packet;
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_packet_t* rx_packet  = ezbus_mac_get_receiver_packet( mac );

    ezbus_packet_init           ( &tx_packet );
    ezbus_packet_set_type       ( &tx_packet, packet_type_ack );
    ezbus_packet_set_dst_socket ( &tx_packet, arbiter->rx_ack_src_socket );
    ezbus_packet_set_src_socket ( &tx_packet, arbiter->rx_ack_dst_socket );
    ezbus_packet_set_seq        ( &tx_packet, ezbus_packet_seq( rx_packet ) );
    ezbus_packet_set_src        ( &tx_packet, &ezbus_self_address );
    ezbus_packet_set_dst        ( &tx_packet, ezbus_packet_src( rx_packet ) );

    ezbus_mac_transmitter_put( mac, &tx_packet );
}

static void ezbus_mac_arbiter_nack_parcel( ezbus_mac_t* mac, uint8_t seq, ezbus_address_t* address )
{
    ezbus_packet_t tx_packet;
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_packet_t* rx_packet  = ezbus_mac_get_receiver_packet( mac );

    ezbus_packet_init           ( &tx_packet );
    ezbus_packet_set_type       ( &tx_packet, packet_type_nack );
    ezbus_packet_set_dst_socket ( &tx_packet, arbiter->rx_nack_src_socket );
    ezbus_packet_set_src_socket ( &tx_packet, arbiter->rx_nack_dst_socket );
    ezbus_packet_set_seq        ( &tx_packet, ezbus_packet_seq( rx_packet ) );
    ezbus_packet_set_src        ( &tx_packet, &ezbus_self_address );
    ezbus_packet_set_dst        ( &tx_packet, ezbus_packet_src( rx_packet ) );

    ezbus_mac_transmitter_put( mac, &tx_packet );
}

/** END RECEIVE PARCEL **/



/*
 * @brief boot2 has completed, this node has the token.
 */
extern void ezbus_mac_boot2_signal_finished( ezbus_mac_t* mac )
{
    EZBUS_LOG( EZBUS_LOG_BOOT2, "" );

    ezbus_mac_arbiter_rst_boot2_cycles( mac );
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_service_start );
    ezbus_mac_token_acquire( mac );
    ezbus_mac_token_reset( mac );
}



extern void  ezbus_mac_token_signal_expired ( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_reboot_warm );
    EZBUS_LOG( EZBUS_LOG_TOKEN, "" );
}


