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
#include <ezbus_mac_coldboot.h>
#include <ezbus_mac_peers.h>
#include <ezbus_mac_token.h>
#include <ezbus_hex.h>
#include <ezbus_log.h>
#include <ezbus_timer.h>

static void do_state_coldboot_minor_start  ( ezbus_mac_t* mac );
static void do_state_coldboot_minor_active ( ezbus_mac_t* mac );
static void do_state_coldboot_minor_stop   ( ezbus_mac_t* mac );
static void do_state_coldboot_minor_stopped( ezbus_mac_t* mac );

static void ezbus_mac_coldboot_minor_timer_callback  ( ezbus_timer_t* timer, void* arg );


extern void ezbus_mac_coldboot_init( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_minor_t* coldboot_minor = ezbus_mac_get_coldboot_minor( mac );

    ezbus_platform_memset( coldboot_minor, 0 , sizeof( ezbus_mac_coldboot_minor_t) );

    ezbus_timer_init( &coldboot_minor->timer, true );
    ezbus_timer_set_key( &coldboot_minor->timer, "coldboot_minor_timer" );
    ezbus_timer_set_callback( &coldboot_minor->timer, ezbus_mac_coldboot_minor_timer_callback, mac );
}

extern void ezbus_mac_coldboot_minor_run( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_minor_t* coldboot_minor = ezbus_mac_get_coldboot_minor( mac );

    ezbus_timer_run( &coldboot_minor->timer );

    switch ( ezbus_mac_coldboot_get_state( mac ) )
    {
        case state_coldboot_minor_start:    do_state_coldboot_minor_start    ( mac );  break;
        case state_coldboot_minor_active:   do_state_coldboot_minor_active   ( mac );  break;
        case state_coldboot_minor_stop:     do_state_coldboot_minor_stop     ( mac );  break;
        case state_coldboot_minor_stopped:  do_state_coldboot_minor_stopped  ( mac );  break;
    }
}

static void do_state_coldboot_minor_start( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_minor_t* coldboot_minor = ezbus_mac_get_coldboot_minor( mac );

    ezbus_timer_stop( &coldboot_minor->timer );

    ezbus_timer_set_period( &coldboot_minor->timer, ezbus_mac_token_ring_time(mac)+EZBUS_COLDBOOT_MINOR_TIME );
    ezbus_timer_start( &coldboot_minor->timer );
    ezbus_mac_coldboot_minor_signal_start(mac);
    ezbus_mac_coldboot_set_state( mac, state_coldboot_minor_active );
}

static void do_state_coldboot_minor_active( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_minor_signal_active(mac);
}

static void do_state_coldboot_minor_stop( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_minor_t* coldboot_minor = ezbus_mac_get_coldboot_minor( mac );
    
    ezbus_timer_stop( &coldboot_minor->timer );
    ezbus_mac_coldboot_minor_signal_stop( mac );
    ezbus_mac_coldboot_set_state( mac, state_coldboot_major_start );
    //ezbus_mac_coldboot_set_state( mac, state_coldboot_minor_stopped );
}

static void do_state_coldboot_minor_stopped( ezbus_mac_t* mac )
{
    /* do nothing */
}

extern void ezbus_mac_coldboot_minor_timer_callback( ezbus_timer_t* timer, void* arg )
{
    ezbus_mac_t* mac=(ezbus_mac_t*)arg;

    ezbus_mac_coldboot_set_state( mac, state_coldboot_minor_stop );
}

extern const char* ezbus_mac_coldboot_minor_get_state_str( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_minor_t* coldboot_minor = ezbus_mac_get_coldboot_minor( mac );

    switch(coldboot_minor->state)
    {
        case state_coldboot_minor_start:    return "state_coldboot_minor_start";    break;
        case state_coldboot_minor_active:   return "state_coldboot_minor_active";   break;
        case state_coldboot_minor_stop:     return "state_coldboot_minor_stop";     break;
        case state_coldboot_minor_stopped:  return "state_coldboot_minor_stopped";  break;
    }
    return "";
}


void ezbus_mac_coldboot_minor_set_state( ezbus_mac_t* mac, ezbus_mac_coldboot_minor_state_t state )
{
    ezbus_mac_coldboot_minor_t* coldboot_minor = ezbus_mac_get_coldboot_minor( mac );
    coldboot_minor->state = state;
}


ezbus_mac_coldboot_minor_state_t ezbus_mac_coldboot_minor_get_state( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_minor_t* coldboot_minor = ezbus_mac_get_coldboot_minor( mac );
    return coldboot_minor->state;
}
