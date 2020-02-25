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
#include <ezbus_mac_warmboot.h>
#include <ezbus_mac_arbiter.h>
#include <ezbus_mac_peers.h>
#include <ezbus_hex.h>
#include <ezbus_log.h>
#include <ezbus_timer.h>

static void do_state_warmboot_idle               ( ezbus_mac_t* mac );
static void do_state_warmboot_start              ( ezbus_mac_t* mac );
static void do_state_warmboot_continue           ( ezbus_mac_t* mac );
static void do_state_warmboot_stop               ( ezbus_mac_t* mac );
static void do_state_warmboot_finished           ( ezbus_mac_t* mac );

static void ezbus_mac_warmboot_period_timeout    ( ezbus_timer_t* timer, void* arg );

extern void ezbus_mac_warmboot_init( ezbus_mac_t* mac )
{
    ezbus_mac_warmboot_t* boot = ezbus_mac_get_warmboot( mac );

    ezbus_platform_memset( boot, 0 , sizeof( ezbus_mac_warmboot_t) );

    boot->seq=1;

    ezbus_timer_init        ( &boot->warmboot_timer );
    ezbus_timer_set_key     ( &boot->warmboot_timer, "warmboot_timer" );
    ezbus_timer_set_callback( &boot->warmboot_timer, ezbus_mac_warmboot_period_timeout, mac );
    ezbus_timer_set_period  ( &boot->warmboot_timer, EZBUS_WARMBOOT_TIMER_PERIOD );
}


extern void ezbus_mac_warmboot_run( ezbus_mac_t* mac )
{
    ezbus_mac_warmboot_t* boot = ezbus_mac_get_warmboot( mac );
    ezbus_timer_run( &boot->warmboot_timer );
    switch ( ezbus_mac_warmboot_get_state( mac ) )
    {
        case state_warmboot_idle:     do_state_warmboot_idle     ( mac );  break;
        case state_warmboot_start:    do_state_warmboot_start    ( mac );  break;
        case state_warmboot_continue: do_state_warmboot_continue ( mac );  break;
        case state_warmboot_stop:     do_state_warmboot_stop     ( mac );  break;
        case state_warmboot_finished: do_state_warmboot_finished ( mac );  break;
    }
}

extern uint8_t ezbus_mac_warmboot_get_seq( ezbus_mac_t* mac )
{
    ezbus_mac_warmboot_t* boot = ezbus_mac_get_warmboot(mac);
    return boot->seq;
}

extern const char* ezbus_mac_warmboot_get_state_str( ezbus_mac_t* mac )
{
    ezbus_mac_warmboot_t* boot = ezbus_mac_get_warmboot( mac );

    switch(boot->state)
    {
        case state_warmboot_idle:       return "state_warmboot_idle";     break;
        case state_warmboot_start:      return "state_warmboot_start";    break;
        case state_warmboot_continue:   return "state_warmboot_continue"; break;
        case state_warmboot_stop:       return "state_warmboot_stop";     break;
        case state_warmboot_finished:   return "state_warmboot_finished"; break;
    }
    return "";
}


extern void ezbus_mac_warmboot_set_state( ezbus_mac_t* mac, ezbus_mac_warmboot_state_t state )
{
    ezbus_mac_warmboot_t* boot = ezbus_mac_get_warmboot( mac );
    boot->state = state;
}


extern ezbus_mac_warmboot_state_t ezbus_mac_warmboot_get_state( ezbus_mac_t* mac )
{
    ezbus_mac_warmboot_t* boot = ezbus_mac_get_warmboot( mac );
    return boot->state;
}


static void do_state_warmboot_idle( ezbus_mac_t* mac )
{
    ezbus_mac_warmboot_t* boot = ezbus_mac_get_warmboot( mac );
    ezbus_timer_stop( &boot->warmboot_timer );
    ezbus_mac_warmboot_signal_idle( mac );
}

static void do_state_warmboot_start( ezbus_mac_t* mac )
{
    ezbus_mac_warmboot_t* boot = ezbus_mac_get_warmboot( mac );
    boot->warmboot_cycles = 0;
    ezbus_timer_stop( &boot->warmboot_timer );
    ezbus_mac_warmboot_set_state( mac, state_warmboot_continue );
    ezbus_timer_start( &boot->warmboot_timer );
    ezbus_mac_warmboot_signal_start( mac );
}

static void do_state_warmboot_continue( ezbus_mac_t* mac )
{
    ezbus_mac_warmboot_signal_continue( mac );
}

static void do_state_warmboot_stop( ezbus_mac_t* mac )
{
    ezbus_mac_warmboot_t* boot = ezbus_mac_get_warmboot( mac );
    ezbus_timer_stop( &boot->warmboot_timer );
    ezbus_mac_warmboot_set_state( mac, state_warmboot_start );
    ezbus_mac_warmboot_signal_stop( mac );
}

static void do_state_warmboot_finished( ezbus_mac_t* mac )
{
    ezbus_mac_warmboot_t* boot = ezbus_mac_get_warmboot( mac );
    if ( ++boot->seq == 0 )
        boot->seq=1;    /* always != 0 */
    ezbus_timer_stop( &boot->warmboot_timer );
    ezbus_mac_warmboot_set_state( mac, state_warmboot_idle );
    ezbus_mac_warmboot_signal_finished( mac );
}

static void ezbus_mac_warmboot_period_timeout( ezbus_timer_t* timer, void* arg )
{
    ezbus_mac_t* mac = (ezbus_mac_t*)arg;
    ezbus_mac_warmboot_t* boot = ezbus_mac_get_warmboot( mac );
    ezbus_timer_stop( &boot->warmboot_timer );

    ezbus_mac_arbiter_dec_warmboot_cycles( mac );
    if ( ezbus_mac_arbiter_get_warmboot_cycles( mac ) == 0 )
    {
        ezbus_mac_warmboot_set_state( mac, state_warmboot_finished );
    }
    else
    {
        EZBUS_LOG( EZBUS_LOG_WARMBOOT, "ezbus_mac_warmboot_period_timeout - state_warmboot_stop\n" );
        ezbus_mac_warmboot_set_state( mac, state_warmboot_stop );
    }
}
