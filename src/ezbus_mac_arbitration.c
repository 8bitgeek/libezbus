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
#include <ezbus_mac_arbitration.h>
#include <ezbus_mac_transmitter.h>
#include <ezbus_mac_receiver.h>
#include <ezbus_hex.h>
#include <ezbus_log.h>

static void ezbus_arbitration_ack_tx_timer_callback( ezbus_timer_t* timer, void* arg );
static void ezbus_arbitration_ack_rx_timer_callback( ezbus_timer_t* timer, void* arg );

static void                          ezbus_mac_arbitration_set_state( ezbus_mac_arbitration_t* mac_arbitration, ezbus_mac_arbitration_state_t state );
static ezbus_mac_arbitration_state_t ezbus_mac_arbitration_get_state( ezbus_mac_arbitration_t* mac_arbitration );

static void do_mac_arbitration_state_offline( ezbus_mac_t* mac );
static void do_mac_arbitration_state_coldboot( ezbus_mac_t* mac );
static void do_mac_arbitration_state_warmboot( ezbus_mac_t* mac );
static void do_mac_arbitration_state_service_start( ezbus_mac_t* mac );
static void do_mac_arbitration_state_service( ezbus_mac_t* mac );
static void do_mac_arbitration_state_online( ezbus_mac_t* mac );


extern void  ezbus_mac_arbitration_init ( ezbus_mac_t* mac )
{
    ezbus_mac_arbitration_t* mac_arbitration = ezbus_mac_get_arbitration( mac );

    memset( mac_arbitration, 0 , sizeof( ezbus_mac_arbitration_t) );
    ezbus_mac_arbitration_set_state( mac_arbitration, mac_arbitration_state_offline );

    ezbus_timer_init( &mac_arbitration->ack_tx_timer );
    ezbus_timer_set_callback( &mac_arbitration->ack_tx_timer, ezbus_arbitration_ack_tx_timer_callback, mac );

    ezbus_timer_init( &mac_arbitration->ack_rx_timer );
    ezbus_timer_set_callback( &mac_arbitration->ack_rx_timer, ezbus_arbitration_ack_rx_timer_callback, mac );
}

static void ezbuz_mac_arbitration_run_timers( ezbus_mac_arbitration_t* mac_arbitration )
{   
    ezbus_timer_run( &mac_arbitration->ack_tx_timer );
    ezbus_timer_run( &mac_arbitration->ack_rx_timer );
}

extern void ezbus_mac_arbitration_run( ezbus_mac_t* mac )
{
    ezbus_mac_arbitration_t* mac_arbitration = ezbus_mac_get_arbitration( mac );
    ezbuz_mac_arbitration_run_timers( mac_arbitration );
    switch( ezbus_mac_arbitration_get_state( mac_arbitration ) )
    {
        case mac_arbitration_state_offline:
            do_mac_arbitration_state_offline( mac );
            break;
        case mac_arbitration_state_coldboot:
            do_mac_arbitration_state_coldboot( mac );
            break;
        case mac_arbitration_state_warmboot:
            do_mac_arbitration_state_warmboot( mac );
            break;                   
        case mac_arbitration_state_service_start:
            do_mac_arbitration_state_service_start( mac );
            break;
        case mac_arbitration_state_service:
            do_mac_arbitration_state_service( mac );
            break;                
        case mac_arbitration_state_online:
            do_mac_arbitration_state_online( mac );
            break;               
    }
}

static void do_mac_arbitration_state_offline( ezbus_mac_t* mac )
{
    /* FIXME - insert code here */
}

static void do_mac_arbitration_state_coldboot( ezbus_mac_t* mac )
{
    /* FIXME - insert code here */
}

static void do_mac_arbitration_state_warmboot( ezbus_mac_t* mac )
{
    /* FIXME - insert code here */
}

static void do_mac_arbitration_state_service_start( ezbus_mac_t* mac )
{
    /* FIXME - insert code here */
}

static void do_mac_arbitration_state_service( ezbus_mac_t* mac )
{
    /* FIXME - insert code here */
}

static void do_mac_arbitration_state_online( ezbus_mac_t* mac )
{
    /* FIXME - insert code here */
}



static void ezbus_mac_arbitration_set_state ( ezbus_mac_arbitration_t* mac_arbitration, ezbus_mac_arbitration_state_t state )
{
    mac_arbitration->state = state;
}

static ezbus_mac_arbitration_state_t ezbus_mac_arbitration_get_state ( ezbus_mac_arbitration_t* mac_arbitration )
{
    return mac_arbitration->state;
}




static void ezbus_arbitration_ack_tx_timer_callback( ezbus_timer_t* timer, void* arg )
{
    ezbus_log( EZBUS_LOG_ARBITRATION, "ezbus_arbitration_ack_tx_timer_callback\n" );
}

static void ezbus_arbitration_ack_rx_timer_callback( ezbus_timer_t* timer, void* arg )
{
    ezbus_log( EZBUS_LOG_ARBITRATION, "ezbus_arbitration_ack_rx_timer_callback\n" );
}




extern void ezbus_mac_transmitter_signal_empty( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_ARBITRATION, "ezbus_mac_transmitter_signal_empty\n" );
}

extern void ezbus_mac_transmitter_signal_full( ezbus_mac_t* mac )
{   
    ezbus_log( EZBUS_LOG_ARBITRATION, "ezbus_mac_transmitter_signal_full\n" );
}

extern void ezbus_mac_transmitter_signal_sent( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_ARBITRATION, "ezbus_mac_transmitter_signal_sent\n" );
}

extern void ezbus_mac_transmitter_signal_wait( ezbus_mac_t* mac )
{
   
    ezbus_log( EZBUS_LOG_ARBITRATION, "ezbus_mac_transmitter_signal_wait\n" );
}

extern void ezbus_mac_transmitter_signal_fault( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_ARBITRATION, "ezbus_mac_transmitter_signal_fault\n" );
}




extern void  ezbus_mac_bootstrap_signal_silent_start( ezbus_mac_bootstrap_t* bootstrap )
{
    ezbus_log( EZBUS_LOG_BOOTSTRAP, "ezbus_mac_bootstrap_signal_silent_start\n" );
}

extern void  ezbus_mac_bootstrap_signal_silent_continue( ezbus_mac_bootstrap_t* bootstrap )
{
    ezbus_log( EZBUS_LOG_BOOTSTRAP, "ezbus_mac_bootstrap_signal_silent_continue\n" );
}

extern void  ezbus_mac_bootstrap_signal_silent_stop( ezbus_mac_bootstrap_t* bootstrap )
{
    ezbus_log( EZBUS_LOG_BOOTSTRAP, "ezbus_mac_bootstrap_signal_silent_stop\n" );
}


extern void  ezbus_mac_bootstrap_signal_coldboot_start( ezbus_mac_bootstrap_t* bootstrap )
{
    ezbus_log( EZBUS_LOG_BOOTSTRAP, "ezbus_mac_bootstrap_signal_coldboot_start\n" );
}

extern void  ezbus_mac_bootstrap_signal_coldboot_continue( ezbus_mac_bootstrap_t* bootstrap )
{
    ezbus_log( EZBUS_LOG_BOOTSTRAP, "ezbus_mac_bootstrap_signal_coldboot_continue\n" );
}

extern void  ezbus_mac_bootstrap_signal_coldboot_stop( ezbus_mac_bootstrap_t* bootstrap )
{
    ezbus_log( EZBUS_LOG_BOOTSTRAP, "ezbus_mac_bootstrap_signal_coldboot_stop\n" );
}


extern void  ezbus_mac_bootstrap_signal_warmboot_tx_first( ezbus_mac_bootstrap_t* bootstrap )
{
    ezbus_log( EZBUS_LOG_BOOTSTRAP, "ezbus_mac_bootstrap_signal_warmboot_tx_first\n" );
}

extern void  ezbus_mac_bootstrap_signal_warmboot_tx_start( ezbus_mac_bootstrap_t* bootstrap )
{
    ezbus_log( EZBUS_LOG_BOOTSTRAP, "ezbus_mac_bootstrap_signal_warmboot_tx_start\n" );
}

extern void  ezbus_mac_bootstrap_signal_warmboot_tx_restart( ezbus_mac_bootstrap_t* bootstrap )
{
    ezbus_log( EZBUS_LOG_BOOTSTRAP, "ezbus_mac_bootstrap_signal_warmboot_tx_restart\n" );
}

extern void  ezbus_mac_bootstrap_signal_warmboot_tx_continue( ezbus_mac_bootstrap_t* bootstrap )
{
    ezbus_log( EZBUS_LOG_BOOTSTRAP, "ezbus_mac_bootstrap_signal_warmboot_tx_continue\n" );
}

extern void  ezbus_mac_bootstrap_signal_warmboot_tx_stop( ezbus_mac_bootstrap_t* bootstrap )
{
    ezbus_log( EZBUS_LOG_BOOTSTRAP, "ezbus_mac_bootstrap_signal_warmboot_tx_stop\n" );
}


extern void  ezbus_mac_bootstrap_signal_warmboot_rx_start( ezbus_mac_bootstrap_t* bootstrap )
{
    ezbus_log( EZBUS_LOG_BOOTSTRAP, "ezbus_mac_bootstrap_signal_warmboot_rx_start\n" );
}

extern void  ezbus_mac_bootstrap_signal_warmboot_rx_continue( ezbus_mac_bootstrap_t* bootstrap )
{
    ezbus_log( EZBUS_LOG_BOOTSTRAP, "ezbus_mac_bootstrap_signal_warmboot_rx_continue\n" );
}

extern void  ezbus_mac_bootstrap_signal_warmboot_rx_stop( ezbus_mac_bootstrap_t* bootstrap )
{
    ezbus_log( EZBUS_LOG_BOOTSTRAP, "ezbus_mac_bootstrap_signal_warmboot_rx_stop\n" );
}

















