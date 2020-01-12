/*****************************************************************************
* Copyright 2019 Mike Sharkey <mike.sharkey@mineairquality.com>              *
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
#include <ezbus_hex.h>
#include <ezbus_log.h>

static void ezbuz_mac_arbiter_run_timers         ( ezbus_mac_arbiter_t* arbiter );
static void ezbus_arbiter_ack_tx_timer_triggered ( ezbus_timer_t* timer, void* arg );
static void ezbus_arbiter_ack_rx_timer_triggered ( ezbus_timer_t* timer, void* arg );

static void do_mac_arbiter_state_offline       ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_reboot        ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_coldboot      ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_warmboot      ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_service_start ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_service       ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_online        ( ezbus_mac_t* mac );
static void ezbuz_mac_arbiter_give_token       ( ezbus_mac_t* mac );

extern void  ezbus_mac_arbiter_init ( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );

    memset( arbiter, 0 , sizeof( ezbus_mac_arbiter_t) );
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_offline );
    ezbus_mac_arbiter_rst_warmboot_cycles( mac );

    ezbus_timer_init( &arbiter->ack_tx_timer );
    ezbus_timer_set_key( &arbiter->ack_tx_timer, "ack_tx_timer" );
    ezbus_timer_set_callback( &arbiter->ack_tx_timer, ezbus_arbiter_ack_tx_timer_triggered, mac );

    ezbus_timer_init( &arbiter->ack_rx_timer );
    ezbus_timer_set_key( &arbiter->ack_rx_timer, "ack_rx_timer" );
    ezbus_timer_set_callback( &arbiter->ack_rx_timer, ezbus_arbiter_ack_rx_timer_triggered, mac );
}

static void ezbuz_mac_arbiter_run_timers( ezbus_mac_arbiter_t* arbiter )
{   
    ezbus_timer_run( &arbiter->ack_tx_timer );
    ezbus_timer_run( &arbiter->ack_rx_timer );
}

extern void ezbus_mac_arbiter_run( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbuz_mac_arbiter_run_timers( arbiter );
    switch( ezbus_mac_arbiter_get_state( mac ) )
    {
        case mac_arbiter_state_offline:
            do_mac_arbiter_state_offline( mac );
            break;
        case mac_arbiter_state_reboot:
            do_mac_arbiter_state_reboot( mac );
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
        case mac_arbiter_state_service:
            do_mac_arbiter_state_service( mac );
            break;                
        case mac_arbiter_state_online:
            do_mac_arbiter_state_online( mac );
            break;               
    }
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
    ezbus_log( EZBUS_LOG_ARBITER, "do_mac_arbiter_state_offline\n" );
    ezbus_mac_token_relinquish( mac );
}

static void do_mac_arbiter_state_reboot( ezbus_mac_t* mac )
{
   ezbus_log( EZBUS_LOG_ARBITER, "do_mac_arbiter_state_reboot\n" );
   ezbus_mac_token_relinquish( mac );
   ezbus_mac_warmboot_set_state( mac, state_warmboot_idle );
   ezbus_mac_coldboot_set_state( mac, state_warmboot_start );
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
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_service );
}

static void do_mac_arbiter_state_service( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_ARBITER, "do_mac_arbiter_state_service\n" );
    if ( ezbus_mac_token_acquired( mac ) )
    {
        ezbus_mac_arbiter_transmit_send( mac );
        ezbuz_mac_arbiter_give_token( mac );
        ezbus_mac_token_relinquish( mac );
    }
}

static void do_mac_arbiter_state_online( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_ARBITER, "do_mac_arbiter_state_online\n" );
}



static void ezbuz_mac_arbiter_give_token( ezbus_mac_t* mac )
{
    ezbus_crc_t crc;
    ezbus_packet_t tx_packet;
    ezbus_address_t* dst_address = ezbus_mac_peers_next( mac, &ezbus_self_address );

    ezbus_packet_init     ( &tx_packet );
    ezbus_packet_set_type ( &tx_packet, packet_type_give_token );
    ezbus_packet_set_seq  ( &tx_packet, 0 );                        /* FIXME seq? */
    ezbus_packet_set_src  ( &tx_packet, &ezbus_self_address );
    ezbus_packet_set_dst  ( &tx_packet, dst_address );

    ezbus_mac_peers_crc( mac, &crc );

    ezbus_packet_set_token_crc( &tx_packet, &crc );
    ezbus_packet_set_token_count( &tx_packet, 0 );                  /* FIXME count? */

    ezbus_mac_transmitter_put( mac, &tx_packet );
    ezbus_mac_transmitter_flush( mac );
}




static void ezbus_arbiter_ack_tx_timer_triggered( ezbus_timer_t* timer, void* arg )
{
    ezbus_log( EZBUS_LOG_ARBITER, "ezbus_arbiter_ack_tx_timer_triggered\n" );
}

static void ezbus_arbiter_ack_rx_timer_triggered( ezbus_timer_t* timer, void* arg )
{
    ezbus_log( EZBUS_LOG_ARBITER, "ezbus_arbiter_ack_rx_timer_triggered\n" );
}



/**
 * @brief A give-token packet has been received
 */
extern void ezbus_mac_arbiter_receive_signal_token ( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    ezbus_mac_token_reset( mac );
    if ( ezbus_address_compare( ezbus_packet_dst(packet), &ezbus_self_address ) == 0 )
    {
        ezbus_mac_token_acquire( mac );
        if ( ezbus_mac_arbiter_get_state( mac ) == mac_arbiter_state_service )
        {
            ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_service_start );
        }
    }
}



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
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_reboot );
    ezbus_log( EZBUS_LOG_TOKEN, "ezbus_mac_token_signal_expired\n" );
}


