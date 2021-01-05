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
#include <ezbus_mac_coldboot_major.h>
#include <ezbus_mac_coldboot_minor.h>
#include <ezbus_log.h>
#include <ezbus_timer.h>

static void do_state_coldboot_major_stop            ( ezbus_mac_t* mac );
static void do_state_coldboot_major_stopped         ( ezbus_mac_t* mac );
static void do_state_coldboot_major_start           ( ezbus_mac_t* mac );
static void do_state_coldboot_major_active          ( ezbus_mac_t* mac );
static void do_state_coldboot_major_dominant        ( ezbus_mac_t* mac );

static void ezbus_mac_coldboot_major_timer_callback ( ezbus_timer_t* timer, void* arg );


extern void ezbus_mac_coldboot_major_init( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_major_t* coldboot_major = ezbus_mac_get_coldboot_major( mac );

    ezbus_platform_memset( coldboot_major, 0 , sizeof( ezbus_mac_coldboot_major_t) );

    ezbus_timer_init( &coldboot_major->timer, true );
    ezbus_timer_set_key( &coldboot_major->timer, "coldboot_major_timer" );    
    ezbus_timer_set_callback( &coldboot_major->timer, ezbus_mac_coldboot_major_timer_callback, mac );
}

extern void ezbus_mac_coldboot_major_run( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_major_t* coldboot_major = ezbus_mac_get_coldboot_major( mac );

    ezbus_timer_run( &coldboot_major->timer );

    switch ( ezbus_mac_coldboot_major_get_state( mac ) )
    {
        case state_coldboot_major_stop:     do_state_coldboot_major_stop     ( mac );  break;
        case state_coldboot_major_stopped:  do_state_coldboot_major_stopped  ( mac );  break;
        case state_coldboot_major_start:    do_state_coldboot_major_start    ( mac );  break;
        case state_coldboot_major_active:   do_state_coldboot_major_active   ( mac );  break;
        case state_coldboot_major_dominant: do_state_coldboot_major_dominant ( mac );  break;
    }
}

static void do_state_coldboot_major_stop( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_major_t* coldboot_major = ezbus_mac_get_coldboot_major( mac );

    ezbus_timer_stop( &coldboot_major->timer );
    ezbus_mac_coldboot_major_signal_stop( mac );
    ezbus_mac_coldboot_major_set_state( mac, state_coldboot_major_stopped );
}

static void do_state_coldboot_major_stopped( ezbus_mac_t* mac )
{
    /* wait - do nothing */
}

static void do_state_coldboot_major_start( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_major_t* coldboot_major = ezbus_mac_get_coldboot_major( mac );

    ezbus_timer_stop( &coldboot_major->timer );
    ezbus_mac_coldboot_major_signal_start( mac );
    ezbus_timer_set_period  (
                                &coldboot_major->timer,
                                ezbus_platform_random( EZBUS_COLDBOOT_TIMER_MIN, EZBUS_COLDBOOT_TIMER_MAX )
                            );
    
    ezbus_timer_start( &coldboot_major->timer );
    ezbus_mac_coldboot_major_set_state( mac, state_coldboot_major_active );
}

static void do_state_coldboot_major_active( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_major_t* coldboot_major = ezbus_mac_get_coldboot_major( mac );
    
    ++coldboot_major->seq;
    /* If I'm the "last man standing" then seize control of the bus */
    if ( ezbus_mac_coldboot_get_emit_count( coldboot_major ) > EZBUS_COLDBOOT_CYCLES )
    {
        ezbus_timer_stop( &coldboot_major->timer );
        ezbus_mac_coldboot_major_set_state( mac, state_coldboot_major_dominant );
    }
}

static void do_state_coldboot_major_dominant( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_major_t* coldboot_major = ezbus_mac_get_coldboot_major( mac );
    ++coldboot_major->seq;
    ezbus_mac_coldboot_major_signal_dominant( mac );
}

static void ezbus_mac_coldboot_major_timer_callback( ezbus_timer_t* timer, void* arg )
{
    ezbus_mac_t* mac = (ezbus_mac_t*)arg;
    ezbus_mac_coldboot_major_t* coldboot_major = ezbus_mac_get_coldboot_major( mac );
    
    ezbus_mac_coldboot_major_signal_active( mac );
    ezbus_mac_coldboot_inc_emit_count( coldboot_major );
    ezbus_mac_coldboot_major_set_state( mac, state_coldboot_major_start );
}

extern uint8_t ezbus_mac_coldboot_get_seq( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_major_t* coldboot_major = ezbus_mac_get_coldboot_major( mac );
    return coldboot_major->seq;
}

extern const char* ezbus_mac_coldboot_major_get_state_str( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_major_t* coldboot_major = ezbus_mac_get_coldboot_major( mac );

    switch(coldboot_major->state)
    {
        case state_coldboot_major_stop:     return "state_coldboot_major_stop";     break;
        case state_coldboot_major_stopped:  return "state_coldboot_major_stopped";  break;
        case state_coldboot_major_start:    return "state_coldboot_major_start";    break;
        case state_coldboot_major_active:   return "state_coldboot_major_active";   break;
        case state_coldboot_major_dominant: return "state_coldboot_major_dominant"; break;
    }
    return "";
}


void ezbus_mac_coldboot_major_set_state( ezbus_mac_t* mac, ezbus_mac_coldboot_major_state_t state )
{
    ezbus_mac_coldboot_major_t* coldboot_major = ezbus_mac_get_coldboot_major( mac );
    coldboot_major->state = state;
}


ezbus_mac_coldboot_major_state_t ezbus_mac_coldboot_major_get_state( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_major_t* coldboot_major = ezbus_mac_get_coldboot_major( mac );
    return coldboot_major->state;
}
