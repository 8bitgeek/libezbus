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
#include <ezbus_mac_pause.h>
#include <ezbus_mac_arbiter_pause.h>
#include <ezbus_platform.h>

static ezbus_mac_arbiter_t ezbus_mac_arbiter_stack[EZBUS_MAC_STACK_SIZE];

static void do_mac_arbiter_state_offline        ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_reboot_boot0   ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_reboot_boot2   ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_boot0          ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_boot2          ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_service_start  ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_online         ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_pause          ( ezbus_mac_t* mac );

static bool ezbus_mac_arbiter_receive_token     ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static void ezbus_mac_arbiter_ack_parcel        ( ezbus_mac_t* mac, uint8_t seq, ezbus_address_t* address );
static void ezbus_mac_arbiter_nack_parcel       ( ezbus_mac_t* mac, uint8_t seq, ezbus_address_t* address );

#define ezbus_mac_arbiter_transmitter_ready(mac)    ( ezbus_mac_transmitter_empty((mac)) && !ezbus_mac_arbiter_transmit_busy((mac)) )

#define ezbus_mac_arbiter_ready_to_give_token(mac)  ( ezbus_mac_arbiter_transmitter_ready((mac)) && arbiter->token_hold++ > EZBUS_TOKEN_HOLD_CYCLES )

#define ezbus_mac_arbiter_give_token(mac)           {                                                   \
                                                        ezbus_mac_arbiter_transmit_token((mac));        \
                                                        ezbus_mac_token_relinquish((mac));              \
                                                    }  

#define ezbus_mac_arbiter_ready_to_ack(mac)         ( ezbus_mac_arbiter_transmitter_ready((mac)) && arbiter->rx_ack_pend )

#define ezbus_mac_arbiter_ready_to_nack(mac)        ( ezbus_mac_arbiter_transmitter_ready((mac)) && arbiter->rx_nack_pend )

#define ezbus_mac_arbiter_send_ack_parcel(mac)      {                                               \
                                                        ezbus_mac_arbiter_ack_parcel( (mac), arbiter->rx_ack_seq, &arbiter->rx_ack_address );       \
                                                        arbiter->rx_ack_pend=false;                 \
                                                        arbiter->rx_ack_seq=0;                      \
                                                    }

#define ezbus_mac_arbiter_send_nack_parcel(mac)     {                                               \
                                                        ezbus_mac_arbiter_nack_parcel( (mac), arbiter->rx_nack_seq, &arbiter->rx_nack_address );    \
                                                        arbiter->rx_nack_pend=false;                \
                                                        arbiter->rx_nack_seq=0;                     \
                                                    }

extern void  ezbus_mac_arbiter_init ( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );

    ezbus_platform.callback_memset( arbiter, 0 , sizeof( ezbus_mac_arbiter_t) );
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_offline );
    ezbus_mac_arbiter_rst_boot2_cycles( mac );
}

extern void ezbus_mac_arbiter_run( ezbus_mac_t* mac )
{
    switch( ezbus_mac_arbiter_get_state( mac ) )
    {
        case mac_arbiter_state_offline:         do_mac_arbiter_state_offline( mac );        break;
        case mac_arbiter_state_reboot_boot0:    do_mac_arbiter_state_reboot_boot0( mac );   break;
        case mac_arbiter_state_reboot_boot2:    do_mac_arbiter_state_reboot_boot2( mac );   break;
        case mac_arbiter_state_boot0:           do_mac_arbiter_state_boot0( mac );          break;
        case mac_arbiter_state_boot2:           do_mac_arbiter_state_boot2( mac );          break;                   
        case mac_arbiter_state_service_start:   do_mac_arbiter_state_service_start( mac );  break;
        case mac_arbiter_state_online:          do_mac_arbiter_state_online( mac );         break;               
        case mac_arbiter_state_pause:           do_mac_arbiter_state_pause( mac );          break;               
    }
}

extern bool ezbus_mac_arbiter_callback( ezbus_mac_t* mac )
{
    switch( ezbus_mac_pause_get_state( mac ) )
    {
        case ezbus_pause_state_start:
            {
                ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
                arbiter->pre_pause_state = ezbus_mac_arbiter_get_state( mac );
                arbiter->state = mac_arbiter_state_pause;
                // fprintf( stderr, "A" );
            }
            break;
        case ezbus_pause_state_finish:
            {
                ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
                arbiter->state = arbiter->pre_pause_state;
                // fprintf( stderr, "B" );
            }
            break;
        default:
            break;
    }
    return true;
}

extern bool ezbus_mac_arbiter_online( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_state_t mac_arbiter_state = ezbus_mac_arbiter_get_state( mac );
    return (mac_arbiter_state == mac_arbiter_state_online);
}

extern void ezbus_mac_arbiter_push( ezbus_mac_t* mac, uint8_t level )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_platform.callback_memcpy(&ezbus_mac_arbiter_stack,arbiter,sizeof(ezbus_mac_arbiter_t));
}

extern void ezbus_mac_arbiter_pop( ezbus_mac_t* mac, uint8_t level )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_platform.callback_memcpy(arbiter,&ezbus_mac_arbiter_stack,sizeof(ezbus_mac_arbiter_t));
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

static void do_mac_arbiter_state_reboot_boot0( ezbus_mac_t* mac )
{
    EZBUS_LOG( EZBUS_LOG_ARBITER, "" );
    ezbus_mac_peers_clear( mac );
    ezbus_mac_token_relinquish( mac );
    ezbus_mac_boot0_reset( mac );
    ezbus_mac_arbiter_set_state( mac , mac_arbiter_state_offline );
}

static void do_mac_arbiter_state_reboot_boot2( ezbus_mac_t* mac )
{
    EZBUS_LOG( EZBUS_LOG_ARBITER, "" );
    ezbus_mac_peers_clear( mac );
    ezbus_mac_token_relinquish( mac );
    ezbus_mac_boot0_reset( mac );
    ezbus_mac_arbiter_set_state( mac , mac_arbiter_state_offline );
}


static void do_mac_arbiter_state_boot0( ezbus_mac_t* mac )
{
    EZBUS_LOG( EZBUS_LOG_ARBITER, "" );
    ezbus_mac_boot2_set_state( mac, state_boot2_restart );
    ezbus_mac_token_relinquish( mac );
}

static void do_mac_arbiter_state_boot2( ezbus_mac_t* mac )
{
    EZBUS_LOG( EZBUS_LOG_ARBITER, "" );
    ezbus_mac_token_reset( mac );
}

static void do_mac_arbiter_state_service_start( ezbus_mac_t* mac )
{
    EZBUS_LOG( EZBUS_LOG_ARBITER, "%d %d", ezbus_mac_token_acquired( mac ), ezbus_mac_pause_active( mac ) );
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_online );
}

static void do_mac_arbiter_state_pause( ezbus_mac_t* mac )
{
   /* @note do nothing */
}

static void do_mac_arbiter_state_online( ezbus_mac_t* mac )
{
    ezbus_socket_callback_run( mac );
    if ( ezbus_mac_token_acquired( mac ) && !ezbus_mac_pause_active( mac ) )
    {
        ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );

        EZBUS_LOG( EZBUS_LOG_ONLINE, "rx_ack_pend %d rx_nack_pend %d", arbiter->rx_ack_pend, arbiter->rx_nack_pend );

        if ( ezbus_mac_arbiter_ready_to_ack(mac) )              ezbus_mac_arbiter_send_ack_parcel(mac)
        else if ( ezbus_mac_arbiter_ready_to_nack(mac) )        ezbus_mac_arbiter_send_nack_parcel(mac)
        else if ( ezbus_mac_arbiter_ready_to_give_token(mac) )  ezbus_mac_arbiter_give_token(mac)
        else if ( ezbus_mac_arbiter_transmitter_ready(mac) && !ezbus_socket_callback_transmitter_empty(mac) )
        {
            // fprintf( stderr, " X4 %d", ezbus_mac_arbiter_get_token_age(mac) );
            if ( ezbus_mac_arbiter_get_token_age(mac) > EZBUS_BOOT2_AGE && ezbus_mac_arbiter_transmitter_ready(mac) )
            {
                ezbus_mac_arbiter_set_token_age( mac, 0 );
                //fprintf(stderr," X5"); // invite
                EZBUS_LOG( EZBUS_LOG_ARBITER, "initiate mac_arbiter_state_reboot_boot2 - token age" );
                ezbus_mac_arbiter_warm_boot( mac );
            }
        }
    }
}

/*****************************************************************************
* @brief A give-token packet has been received                               *
*****************************************************************************/
extern void ezbus_mac_arbiter_receive_signal_token ( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    ezbus_mac_token_reset( mac );
    
    if ( ezbus_port_get_address_is_self( ezbus_mac_get_port(mac), ezbus_packet_dst( packet ) ) )
    {
        if ( ezbus_mac_arbiter_receive_token( mac, packet ) )
        {
            if ( !ezbus_mac_arbiter_online( mac ) )
            {
                ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_service_start );
            }
        }
    }
}

static bool ezbus_mac_arbiter_receive_token( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_crc_t crc;

    arbiter->token_hold=0;
    ezbus_mac_peers_crc( mac, &crc );
    // fprintf( stderr, " X3 %d", ezbus_mac_arbiter_get_token_age(mac) );
    if ( ezbus_crc_equal( &crc, ezbus_packet_get_token_crc( packet ) ) )
    {
        ezbus_mac_token_acquire( mac );
        if ( ezbus_mac_boot2_get_state( mac ) != state_boot2_idle )
        {
            ezbus_mac_boot2_set_state( mac, state_boot2_finished );
        }
        return true;
    }
    else
    {
        EZBUS_LOG( EZBUS_LOG_ARBITER, "bad token crc -> boot2" );
        ezbus_mac_arbiter_warm_boot( mac );
    }
    return false;
}


/*****************************************************************************
* parcel receive                                                             *
*****************************************************************************/

extern void ezbus_mac_arbiter_receive_signal_parcel( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    if ( ezbus_port_get_address_is_self( ezbus_mac_get_port(mac), ezbus_packet_dst( packet ) ) )
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
    ezbus_packet_set_src        ( &tx_packet, ezbus_port_get_address(ezbus_mac_get_port(mac)) );
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
    ezbus_packet_set_src        ( &tx_packet, ezbus_port_get_address(ezbus_mac_get_port(mac)) );
    ezbus_packet_set_dst        ( &tx_packet, ezbus_packet_src( rx_packet ) );

    ezbus_mac_transmitter_put( mac, &tx_packet );
}


/*****************************************************************************
* @brief boot2 has completed, this node has the token.                       *
*****************************************************************************/
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
    ezbus_mac_arbiter_warm_boot( mac );
    EZBUS_LOG( EZBUS_LOG_TOKEN, "" );
}

