/*****************************************************************************
* Copyright © 2019-2020 Mike Sharkey <mike.sharkey@mineairquality.com>       *
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
#include <ezbus_mac_coldboot.h>
#include <ezbus_mac_warmboot.h>
#include <ezbus_mac_peers.h>
#include <ezbus_socket_callback.h>
#include <ezbus_crc.h>
#include <ezbus_hex.h>
#include <ezbus_log.h>

static void do_mac_arbiter_state_offline       ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_reboot_cold   ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_reboot_warm   ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_coldboot      ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_warmboot      ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_service_start ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_online        ( ezbus_mac_t* mac );

static void ezbuz_mac_arbiter_receive_token    ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static void ezbus_mac_arbiter_ack_parcel       ( ezbus_mac_t* mac, uint8_t seq, ezbus_address_t* address );
static void ezbus_mac_arbiter_nack_parcel      ( ezbus_mac_t* mac, uint8_t seq, ezbus_address_t* address );


extern void  ezbus_mac_arbiter_init ( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );

    memset( arbiter, 0 , sizeof( ezbus_mac_arbiter_t) );
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_offline );
    ezbus_mac_arbiter_rst_warmboot_cycles( mac );
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
        case mac_arbiter_state_warmboot:
            do_mac_arbiter_state_warmboot( mac );
            break;                   
        case mac_arbiter_state_service_start:
            do_mac_arbiter_state_service_start( mac );
            break;
        case mac_arbiter_state_online:
            do_mac_arbiter_state_online( mac );
            break;               
    }
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

extern uint8_t ezbus_mac_arbiter_get_warmboot_cycles( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    return arbiter->warmboot_cycles;
}

extern void ezbus_mac_arbiter_set_warmboot_cycles( ezbus_mac_t* mac, uint8_t cycles )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    arbiter->warmboot_cycles = cycles;
}

extern void ezbus_mac_arbiter_dec_warmboot_cycles( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    if ( arbiter->warmboot_cycles )
    {
        --arbiter->warmboot_cycles;
    }
}


extern void ezbus_mac_arbiter_rst_warmboot_cycles( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    arbiter->warmboot_cycles = EZBUS_WARMBOOT_CYCLES;
}


static void do_mac_arbiter_state_offline( ezbus_mac_t* mac )
{
    //ezbus_log( EZBUS_LOG_ARBITER, "do_mac_arbiter_state_offline\n" );
    ezbus_mac_token_relinquish( mac );
}

static void do_mac_arbiter_state_reboot_cold( ezbus_mac_t* mac )
{
   ezbus_log( EZBUS_LOG_ARBITER, "do_mac_arbiter_state_reboot_cold\n" );
   ezbus_mac_token_relinquish( mac );
   ezbus_mac_warmboot_set_state( mac, state_warmboot_idle );
   ezbus_mac_coldboot_set_state( mac, state_coldboot_start );
   ezbus_mac_arbiter_set_state( mac , mac_arbiter_state_offline );
}

static void do_mac_arbiter_state_reboot_warm( ezbus_mac_t* mac )
{
   ezbus_log( EZBUS_LOG_ARBITER, "do_mac_arbiter_state_reboot_warm\n" );
   ezbus_mac_peers_clear( mac );
   ezbus_mac_token_relinquish( mac );
   ezbus_mac_warmboot_set_state( mac, state_warmboot_start );
   ezbus_mac_coldboot_reset( mac );
   ezbus_mac_arbiter_set_state( mac , mac_arbiter_state_offline );
}

static void do_mac_arbiter_state_coldboot( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_ARBITER, "do_mac_arbiter_state_coldboot\n" );
    ezbus_mac_warmboot_set_state( mac, state_warmboot_start );
    ezbus_mac_token_relinquish( mac );
}

static void do_mac_arbiter_state_warmboot( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_ARBITER, "do_mac_arbiter_state_warmboot\n" );
    ezbus_mac_token_reset( mac );
}

static void do_mac_arbiter_state_service_start( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_ARBITER, "do_mac_arbiter_state_service_start\n" );
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_online );
}

static void do_mac_arbiter_state_online( ezbus_mac_t* mac )
{
    if ( ezbus_mac_token_acquired( mac ) )
    {
        ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );

        ezbus_log( EZBUS_LOG_ONLINE, "rx_ack_pend %d rx_nack_pend %d \n", arbiter->rx_ack_pend, arbiter->rx_nack_pend );

        if ( ezbus_mac_transmitter_empty( mac ) )
        {
            if ( arbiter->rx_ack_pend )
            {
                ezbus_mac_arbiter_ack_parcel( mac, arbiter->rx_ack_seq, &arbiter->rx_ack_address );
                arbiter->rx_ack_pend=false;
                arbiter->rx_ack_seq=0;
            }
            if ( arbiter->rx_nack_pend )
            {
                ezbus_mac_arbiter_nack_parcel( mac, arbiter->rx_nack_seq, &arbiter->rx_nack_address );
                arbiter->rx_nack_pend=false;
                arbiter->rx_nack_seq=0;
            }
            else
            {
                if ( arbiter->token_hold++ > EZBUS_TOKEN_HOLD_CYCLES )
                {
                    ezbus_log( EZBUS_LOG_ONLINE, "online tx tok\n" );
                    ezbuz_mac_arbiter_transmit_token( mac );
                    ezbus_mac_token_relinquish( mac );
                }
            }
        }

        if ( ezbus_mac_transmitter_empty( mac ) )
        {
            if ( !ezbus_mac_arbiter_transmit_busy( mac ) )
            {
                ezbus_log( EZBUS_LOG_ONLINE, "online tx parcel\n" );
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
        ezbuz_mac_arbiter_receive_token( mac, packet );
        if ( ezbus_mac_arbiter_get_state( mac ) != mac_arbiter_state_online )
        {
            ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_service_start );
        }
    }
}

static void ezbuz_mac_arbiter_receive_token( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_crc_t crc;

    ezbus_log( EZBUS_LOG_TOKEN, "ezbuz_mac_arbiter_receive_token\n" );

    arbiter->token_hold=0;
    ezbus_mac_peers_crc( mac, &crc );
    ezbus_mac_arbiter_set_token_age( mac, ezbus_packet_get_token_age(packet) );
    if ( ezbus_crc_equal( &crc, ezbus_packet_get_token_crc( packet ) ) )
    {
        ezbus_log( EZBUS_LOG_TOKEN, "ezbus_mac_token_acquire\n" );
        ezbus_mac_token_acquire( mac );

        if ( ezbus_packet_get_token_age(packet) > EZBUS_WARMBOOT_AGE )
        {
            ezbus_log( EZBUS_LOG_TOKEN, "ezbus_packet_get_token_age\n" );
            ezbus_mac_arbiter_set_token_age( mac, 0 );
            ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_reboot_warm );
            do_mac_arbiter_state_reboot_warm( mac );
        }
    }
    else
    {
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

        ezbus_log( EZBUS_LOG_RECEIVER, "do_receiver_packet_type_parcel\n" );
    
        if ( !arbiter->rx_ack_pend && !arbiter->rx_nack_pend )
        {
            if ( ezbus_socket_callback_receiver_ready( mac, packet ) )
            {
                arbiter->rx_ack_pend = true;
                arbiter->rx_ack_seq = ezbus_packet_seq( rx_packet );
                ezbus_address_copy( &arbiter->rx_ack_address, ezbus_packet_src( rx_packet ) );
            }
            else
            {
                arbiter->rx_nack_pend = true;
                arbiter->rx_nack_seq = ezbus_packet_seq( rx_packet );         
                ezbus_address_copy( &arbiter->rx_nack_address, ezbus_packet_src( rx_packet ) );
            }
        }
    }
}

static void ezbus_mac_arbiter_ack_parcel( ezbus_mac_t* mac, uint8_t seq, ezbus_address_t* address )
{
    ezbus_packet_t tx_packet;
    ezbus_packet_t* rx_packet  = ezbus_mac_get_receiver_packet( mac );

    ezbus_packet_init           ( &tx_packet );
    ezbus_packet_set_type       ( &tx_packet, packet_type_ack );
    ezbus_packet_set_dst_socket ( &tx_packet, ezbus_packet_src_socket( rx_packet ) );
    ezbus_packet_set_src_socket ( &tx_packet, ezbus_packet_dst_socket( rx_packet ) );
    ezbus_packet_set_seq        ( &tx_packet, ezbus_packet_seq( rx_packet ) );
    ezbus_packet_set_src        ( &tx_packet, &ezbus_self_address );
    ezbus_packet_set_dst        ( &tx_packet, ezbus_packet_src( rx_packet ) );

    ezbus_mac_transmitter_put( mac, &tx_packet );
}

static void ezbus_mac_arbiter_nack_parcel( ezbus_mac_t* mac, uint8_t seq, ezbus_address_t* address )
{
    ezbus_packet_t tx_packet;
    ezbus_packet_t* rx_packet  = ezbus_mac_get_receiver_packet( mac );

    ezbus_packet_init           ( &tx_packet );
    ezbus_packet_set_type       ( &tx_packet, packet_type_nack );
    ezbus_packet_set_dst_socket ( &tx_packet, ezbus_packet_src_socket( rx_packet ) );
    ezbus_packet_set_src_socket ( &tx_packet, ezbus_packet_dst_socket( rx_packet ) );
    ezbus_packet_set_seq        ( &tx_packet, ezbus_packet_seq( rx_packet ) );
    ezbus_packet_set_src        ( &tx_packet, &ezbus_self_address );
    ezbus_packet_set_dst        ( &tx_packet, ezbus_packet_src( rx_packet ) );

    ezbus_mac_transmitter_put( mac, &tx_packet );
}

/** END RECEIVE PARCEL **/



/*
 * @brief Warmboot has completed, this node has the token.
 */
extern void ezbus_mac_warmboot_signal_finished( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_WARMBOOT, "ezbus_mac_warmboot_signal_finished\n" );

    ezbus_mac_arbiter_rst_warmboot_cycles( mac );
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_service_start );
    ezbus_mac_token_acquire( mac );
    ezbus_mac_token_reset( mac );

    #if defined(WARMBOOT_DEBUG)
        ezbus_mac_warmboot_set_state( mac, state_warmboot_start );
    #endif
}



extern void  ezbus_mac_token_signal_expired ( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_reboot_warm );
    ezbus_log( EZBUS_LOG_TOKEN, "ezbus_mac_token_signal_expired\n" );
}


