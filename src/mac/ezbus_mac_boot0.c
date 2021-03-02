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
#include <ezbus_mac_boot0.h>
#include <ezbus_mac_boot1.h>
#include <ezbus_mac_peers.h>
#include <ezbus_mac_token.h>
#include <ezbus_hex.h>
#include <ezbus_log.h>
#include <ezbus_mac_timer.h>
#include <ezbus_platform.h>

static void do_state_boot0_start  ( ezbus_mac_t* mac );
static void do_state_boot0_active ( ezbus_mac_t* mac );
static void do_state_boot0_stop   ( ezbus_mac_t* mac );
static void do_state_boot0_stopped( ezbus_mac_t* mac );

static void ezbus_mac_boot0_timer_callback  ( ezbus_timer_t* timer, void* arg );


extern void ezbus_mac_boot0_init( ezbus_mac_t* mac )
{
    ezbus_mac_boot0_t* boot0 = ezbus_mac_get_boot0( mac );

    ezbus_platform.callback_memset( boot0, 0 , sizeof( ezbus_mac_boot0_t) );

    ezbus_mac_timer_setup( mac, &boot0->timer, true );
    ezbus_timer_set_key( &boot0->timer, "boot0_timer" );
    ezbus_timer_set_callback( &boot0->timer, ezbus_mac_boot0_timer_callback, mac );
}

extern void ezbus_mac_boot0_run( ezbus_mac_t* mac )
{
    switch ( ezbus_mac_boot0_get_state( mac ) )
    {
        case state_boot0_start:    do_state_boot0_start    ( mac );  break;
        case state_boot0_active:   do_state_boot0_active   ( mac );  break;
        case state_boot0_stop:     do_state_boot0_stop     ( mac );  break;
        case state_boot0_stopped:  do_state_boot0_stopped  ( mac );  break;
    }
}

static void do_state_boot0_start( ezbus_mac_t* mac )
{
    ezbus_mac_boot0_t* boot0 = ezbus_mac_get_boot0( mac );

    ezbus_timer_stop( &boot0->timer );

    ezbus_timer_set_period( &boot0->timer, ezbus_mac_token_ring_time(mac)+EZBUS_BOOT0_TIME );
    ezbus_timer_start( &boot0->timer );
    ezbus_mac_boot0_set_state( mac, state_boot0_active );
}

static void do_state_boot0_active( ezbus_mac_t* mac )
{
    /* just wait for timer expire or forced to change boot level by rx boot1 */
}

static void do_state_boot0_stop( ezbus_mac_t* mac )
{
    ezbus_mac_boot0_t* boot0 = ezbus_mac_get_boot0( mac );
    
    ezbus_timer_stop( &boot0->timer );
    ezbus_mac_boot1_set_state( mac, state_boot1_start );
    ezbus_mac_boot0_set_state( mac, state_boot0_stopped );
}

static void do_state_boot0_stopped( ezbus_mac_t* mac )
{
    /* just wait for external event to change state */
}

extern void ezbus_mac_boot0_timer_callback( ezbus_timer_t* timer, void* arg )
{
    ezbus_mac_t* mac=(ezbus_mac_t*)arg;

    ezbus_mac_boot0_set_state( mac, state_boot0_stop );
}

extern const char* ezbus_mac_boot0_get_state_str( ezbus_mac_t* mac )
{
    ezbus_mac_boot0_t* boot0 = ezbus_mac_get_boot0( mac );

    switch(boot0->state)
    {
        case state_boot0_start:    return "state_boot0_start";    break;
        case state_boot0_active:   return "state_boot0_active";   break;
        case state_boot0_stop:     return "state_boot0_stop";     break;
        case state_boot0_stopped:  return "state_boot0_stopped";  break;
    }
    return "";
}


void ezbus_mac_boot0_set_state( ezbus_mac_t* mac, ezbus_mac_boot0_state_t state )
{
    ezbus_mac_boot0_t* boot0 = ezbus_mac_get_boot0( mac );
    boot0->state = state;
}


ezbus_mac_boot0_state_t ezbus_mac_boot0_get_state( ezbus_mac_t* mac )
{
    ezbus_mac_boot0_t* boot0 = ezbus_mac_get_boot0( mac );
    return boot0->state;
}
