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
#include <ezbus_mac_boot1.h>
#include <ezbus_log.h>
#include <ezbus_mac_timer.h>
#include <ezbus_mac_arbiter_transmit.h>
#include <ezbus_platform.h>

static void do_state_boot1_stop            ( ezbus_mac_t* mac );
static void do_state_boot1_stopped         ( ezbus_mac_t* mac );
static void do_state_boot1_start           ( ezbus_mac_t* mac );
static void do_state_boot1_active          ( ezbus_mac_t* mac );
static void do_state_boot1_dominant        ( ezbus_mac_t* mac );

static void ezbus_mac_boot1_timer_callback ( ezbus_timer_t* timer, void* arg );


extern void ezbus_mac_boot1_init( ezbus_mac_t* mac )
{
    ezbus_mac_boot1_t* boot1 = ezbus_mac_get_boot1( mac );

    ezbus_platform.callback_memset( boot1, 0 , sizeof( ezbus_mac_boot1_t) );

    ezbus_mac_timer_setup( mac, &boot1->timer, true );
    ezbus_timer_set_key( &boot1->timer, "boot1_timer" );    
    ezbus_timer_set_callback( &boot1->timer, ezbus_mac_boot1_timer_callback, mac );
}

extern void ezbus_mac_boot1_run( ezbus_mac_t* mac )
{
    switch ( ezbus_mac_boot1_get_state( mac ) )
    {
        case state_boot1_start:    do_state_boot1_start    ( mac );  break;
        case state_boot1_active:   do_state_boot1_active   ( mac );  break;
        case state_boot1_dominant: do_state_boot1_dominant ( mac );  break;
        case state_boot1_stop:     do_state_boot1_stop     ( mac );  break;
        case state_boot1_stopped:  do_state_boot1_stopped  ( mac );  break;
    }
}

static void do_state_boot1_start( ezbus_mac_t* mac )
{
    ezbus_mac_boot1_t* boot1 = ezbus_mac_get_boot1( mac );

    ezbus_timer_stop( &boot1->timer );
    ezbus_mac_boot1_signal_start( mac );
    ezbus_timer_set_period  (
                                &boot1->timer,
                                ezbus_platform.callback_random( EZBUS_BOOT1_TIMER_MIN, EZBUS_BOOT1_TIMER_MAX )
                            );
    
    ezbus_timer_start( &boot1->timer );
    ezbus_mac_boot1_set_state( mac, state_boot1_active );
}

static void do_state_boot1_active( ezbus_mac_t* mac )
{
    ezbus_mac_boot1_t* boot1 = ezbus_mac_get_boot1( mac );
    
    ++boot1->seq;
    /* If I'm the "last man standing" then seize control of the bus */
    if ( ezbus_mac_coldboot_get_emit_count( boot1 ) > EZBUS_BOOT1_CYCLES )
    {
        ezbus_timer_stop( &boot1->timer );
        ezbus_mac_boot1_set_state( mac, state_boot1_dominant );
    }
}

static void do_state_boot1_stop( ezbus_mac_t* mac )
{
    ezbus_mac_boot1_t* boot1 = ezbus_mac_get_boot1( mac );

    ezbus_timer_stop( &boot1->timer );
    ezbus_mac_boot1_signal_stop( mac );
    ezbus_mac_boot1_set_state( mac, state_boot1_stopped );
}

static void do_state_boot1_stopped( ezbus_mac_t* mac )
{
    /* wait - do nothing */
}

static void do_state_boot1_dominant( ezbus_mac_t* mac )
{
    ezbus_mac_boot1_t* boot1 = ezbus_mac_get_boot1( mac );
    ++boot1->seq;
    ezbus_mac_boot1_signal_dominant( mac );
}

static void ezbus_mac_boot1_timer_callback( ezbus_timer_t* timer, void* arg )
{
    ezbus_mac_t* mac = (ezbus_mac_t*)arg;
    ezbus_mac_boot1_t* boot1 = ezbus_mac_get_boot1( mac );
    
    ezbus_mac_boot1_signal_active( mac );
    ezbus_mac_coldboot_inc_emit_count( boot1 );
    ezbus_mac_boot1_set_state( mac, state_boot1_start );
}

extern uint8_t ezbus_mac_coldboot_get_seq( ezbus_mac_t* mac )
{
    ezbus_mac_boot1_t* boot1 = ezbus_mac_get_boot1( mac );
    return boot1->seq;
}

extern const char* ezbus_mac_boot1_get_state_str( ezbus_mac_t* mac )
{
    ezbus_mac_boot1_t* boot1 = ezbus_mac_get_boot1( mac );

    switch(boot1->state)
    {
        case state_boot1_stop:     return "state_boot1_stop";     break;
        case state_boot1_stopped:  return "state_boot1_stopped";  break;
        case state_boot1_start:    return "state_boot1_start";    break;
        case state_boot1_active:   return "state_boot1_active";   break;
        case state_boot1_dominant: return "state_boot1_dominant"; break;
    }
    return "";
}


void ezbus_mac_boot1_set_state( ezbus_mac_t* mac, ezbus_mac_boot1_state_t state )
{
    ezbus_mac_boot1_t* boot1 = ezbus_mac_get_boot1( mac );
    boot1->state = state;
}


ezbus_mac_boot1_state_t ezbus_mac_boot1_get_state( ezbus_mac_t* mac )
{
    ezbus_mac_boot1_t* boot1 = ezbus_mac_get_boot1( mac );
    return boot1->state;
}
