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
#include <ezbus_mac_transmitter.h>
#include <ezbus_mac_receiver.h>
#include <ezbus_hex.h>
#include <ezbus_log.h>

static void ezbuz_mac_arbiter_run_timers         ( ezbus_mac_arbiter_t* arbiter );
static void ezbus_arbiter_ack_tx_timer_triggered ( ezbus_timer_t* timer, void* arg );
static void ezbus_arbiter_ack_rx_timer_triggered ( ezbus_timer_t* timer, void* arg );

static void do_mac_arbiter_state_offline       ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_coldboot      ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_warmboot      ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_service_start ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_service       ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_online        ( ezbus_mac_t* mac );


extern void  ezbus_mac_arbiter_init ( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );

    memset( arbiter, 0 , sizeof( ezbus_mac_arbiter_t) );
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_offline );

    ezbus_timer_init( &arbiter->ack_tx_timer );
    ezbus_timer_set_callback( &arbiter->ack_tx_timer, ezbus_arbiter_ack_tx_timer_triggered, mac );

    ezbus_timer_init( &arbiter->ack_rx_timer );
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




static void do_mac_arbiter_state_offline( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_ARBITRATION, "do_mac_arbiter_state_offline\n" );
}

static void do_mac_arbiter_state_coldboot( ezbus_mac_t* mac )
{
   ezbus_log( EZBUS_LOG_ARBITRATION, "do_mac_arbiter_state_coldboot\n" );
}

static void do_mac_arbiter_state_warmboot( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_ARBITRATION, "do_mac_arbiter_state_warmboot\n" );
}

static void do_mac_arbiter_state_service_start( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_ARBITRATION, "do_mac_arbiter_state_service_start\n" );
}

static void do_mac_arbiter_state_service( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_ARBITRATION, "do_mac_arbiter_state_service\n" );
}

static void do_mac_arbiter_state_online( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_ARBITRATION, "do_mac_arbiter_state_online\n" );
}




static void ezbus_arbiter_ack_tx_timer_triggered( ezbus_timer_t* timer, void* arg )
{
    ezbus_log( EZBUS_LOG_ARBITRATION, "ezbus_arbiter_ack_tx_timer_triggered\n" );
}

static void ezbus_arbiter_ack_rx_timer_triggered( ezbus_timer_t* timer, void* arg )
{
    ezbus_log( EZBUS_LOG_ARBITRATION, "ezbus_arbiter_ack_rx_timer_triggered\n" );
}





extern void  ezbus_mac_coldboot_signal_silent_start( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_COLDBOOT, "ezbus_mac_coldboot_signal_silent_start\n" );
}

extern void  ezbus_mac_coldboot_signal_silent_continue( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_COLDBOOT, "ezbus_mac_coldboot_signal_silent_continue\n" );
}

extern void  ezbus_mac_coldboot_signal_silent_stop( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_COLDBOOT, "ezbus_mac_coldboot_signal_silent_stop\n" );
}


extern void  ezbus_mac_coldboot_signal_start( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_COLDBOOT, "ezbus_mac_coldboot_signal_start\n" );
}

extern void  ezbus_mac_coldboot_signal_continue( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_COLDBOOT, "ezbus_mac_coldboot_signal_continue\n" );
}

extern void  ezbus_mac_coldboot_signal_stop( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_COLDBOOT, "ezbus_mac_coldboot_signal_stop\n" );
}

extern void  ezbus_mac_coldboot_signal_dominant( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_COLDBOOT, "ezbus_mac_coldboot_signal_dominant\n" );
}






extern void  ezbus_mac_warmboot_signal_tx_first( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_WARMBOOT, "ezbus_mac_warmboot_signal_tx_first\n" );
}

extern void  ezbus_mac_warmboot_signal_tx_start( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_WARMBOOT, "ezbus_mac_warmboot_signal_tx_start\n" );
}

extern void  ezbus_mac_warmboot_signal_tx_restart( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_WARMBOOT, "ezbus_mac_warmboot_signal_tx_restart\n" );
}

extern void  ezbus_mac_warmboot_signal_tx_continue( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_WARMBOOT, "ezbus_mac_warmboot_signal_tx_continue\n" );
}

extern void  ezbus_mac_warmboot_signal_tx_stop( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_WARMBOOT, "ezbus_mac_warmboot_signal_tx_stop\n" );
}


extern void  ezbus_mac_warmboot_signal_rx_start( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_WARMBOOT, "ezbus_mac_warmboot_signal_rx_start\n" );
}

extern void  ezbus_mac_warmboot_signal_rx_continue( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_WARMBOOT, "ezbus_mac_warmboot_signal_rx_continue\n" );
}

extern void  ezbus_mac_warmboot_signal_rx_stop( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_WARMBOOT, "ezbus_mac_warmboot_signal_rx_stop\n" );
}


extern void  ezbus_mac_token_signal_expired ( ezbus_mac_t* mac )
{
     ezbus_log( EZBUS_LOG_TOKEN, "ezbus_mac_token_signal_expired\n" );   
}














