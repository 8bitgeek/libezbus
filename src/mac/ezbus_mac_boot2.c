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
#include <ezbus_mac_boot2.h>
#include <ezbus_mac_arbiter.h>
#include <ezbus_mac_peers.h>
#include <ezbus_hex.h>
#include <ezbus_log.h>
#include <ezbus_mac_timer.h>
#include <ezbus_platform.h>

static void do_state_boot2_idle               ( ezbus_mac_t* mac );
static void do_state_boot2_restart            ( ezbus_mac_t* mac );
static void do_state_boot2_start              ( ezbus_mac_t* mac );
static void do_state_boot2_active             ( ezbus_mac_t* mac );
static void do_state_boot2_stop               ( ezbus_mac_t* mac );
static void do_state_boot2_finished           ( ezbus_mac_t* mac );

static void ezbus_mac_boot2_period_timeout    ( ezbus_timer_t* timer, void* arg );

extern void ezbus_mac_boot2_init( ezbus_mac_t* mac )
{
    ezbus_mac_boot2_t* boot2 = ezbus_mac_get_boot2( mac );

    ezbus_platform.callback_memset( boot2, 0 , sizeof( ezbus_mac_boot2_t) );

    boot2->seq=1;

    ezbus_mac_timer_setup   ( mac, &boot2->timer, true );
    ezbus_timer_set_key     ( &boot2->timer, "boot2_timer" );
    ezbus_timer_set_callback( &boot2->timer, ezbus_mac_boot2_period_timeout, mac );
    ezbus_timer_set_period  ( &boot2->timer, EZBUS_BOOT2_TIMER_PERIOD );
}


extern void ezbus_mac_boot2_run( ezbus_mac_t* mac )
{
    switch ( ezbus_mac_boot2_get_state( mac ) )
    {
        case state_boot2_idle:     do_state_boot2_idle     ( mac );  break;
        case state_boot2_restart:  do_state_boot2_restart  ( mac );  break;
        case state_boot2_start:    do_state_boot2_start    ( mac );  break;
        case state_boot2_active:   do_state_boot2_active   ( mac );  break;
        case state_boot2_stop:     do_state_boot2_stop     ( mac );  break;
        case state_boot2_finished: do_state_boot2_finished ( mac );  break;
    }
}

extern uint8_t ezbus_mac_boot2_get_seq( ezbus_mac_t* mac )
{
    ezbus_mac_boot2_t* boot2 = ezbus_mac_get_boot2(mac);
    return boot2->seq;
}

extern const char* ezbus_mac_boot2_get_state_str( ezbus_mac_t* mac )
{
    ezbus_mac_boot2_t* boot2 = ezbus_mac_get_boot2( mac );

    switch(boot2->state)
    {
        case state_boot2_idle:       return "state_boot2_idle";     break;
        case state_boot2_restart:    return "state_boot2_restart";  break;
        case state_boot2_start:      return "state_boot2_start";    break;
        case state_boot2_active:     return "state_boot2_active";   break;
        case state_boot2_stop:       return "state_boot2_stop";     break;
        case state_boot2_finished:   return "state_boot2_finished"; break;
    }
    return "";
}


extern void ezbus_mac_boot2_set_state( ezbus_mac_t* mac, ezbus_mac_boot2_state_t state )
{
    ezbus_mac_boot2_t* boot2 = ezbus_mac_get_boot2( mac );
    boot2->state = state;
}


extern ezbus_mac_boot2_state_t ezbus_mac_boot2_get_state( ezbus_mac_t* mac )
{
    ezbus_mac_boot2_t* boot2 = ezbus_mac_get_boot2( mac );
    return boot2->state;
}

static void do_state_boot2_idle( ezbus_mac_t* mac )
{
    ezbus_mac_boot2_t* boot2 = ezbus_mac_get_boot2( mac );
    ezbus_timer_stop( &boot2->timer );
}

static void do_state_boot2_restart( ezbus_mac_t* mac )
{
    ezbus_mac_boot2_t* boot2 = ezbus_mac_get_boot2( mac );
    ezbus_timer_stop( &boot2->timer );
    ezbus_mac_peers_clear( mac );
    ezbus_mac_boot2_set_state( mac, state_boot2_start );
}

static void do_state_boot2_start( ezbus_mac_t* mac )
{
    ezbus_mac_boot2_t* boot2 = ezbus_mac_get_boot2( mac );
    boot2->cycles = 0;
    ezbus_timer_stop( &boot2->timer );
    ezbus_mac_boot2_set_state( mac, state_boot2_active );
    ezbus_timer_start( &boot2->timer );
}

static void do_state_boot2_active( ezbus_mac_t* mac )
{
    /* wait timer(s) */
}

static void do_state_boot2_stop( ezbus_mac_t* mac )
{
    ezbus_mac_boot2_t* boot2 = ezbus_mac_get_boot2( mac );

    ezbus_timer_stop( &boot2->timer );
    ezbus_mac_boot2_set_state( mac, state_boot2_start );
    ezbus_mac_boot2_signal_stop( mac );
}

static void do_state_boot2_finished( ezbus_mac_t* mac )
{
    ezbus_mac_boot2_t* boot2 = ezbus_mac_get_boot2( mac );

    if ( ++boot2->seq == 0 )
        boot2->seq=1;    /* always != 0 */
    ezbus_timer_stop( &boot2->timer );
    ezbus_mac_boot2_set_state( mac, state_boot2_idle );
    ezbus_mac_boot2_signal_finished( mac );
}

static void ezbus_mac_boot2_period_timeout( ezbus_timer_t* timer, void* arg )
{
    ezbus_mac_t* mac = (ezbus_mac_t*)arg;

    ezbus_mac_boot2_t* boot2 = ezbus_mac_get_boot2( mac );
    ezbus_timer_stop( &boot2->timer );

    ezbus_mac_arbiter_dec_boot2_cycles( mac );
    if ( ezbus_mac_arbiter_get_boot2_cycles( mac ) == 0 )
    {
        ezbus_mac_boot2_set_state( mac, state_boot2_finished );
    }
    else
    {
        ezbus_mac_boot2_set_state( mac, state_boot2_stop );
    }
}
