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
#include <ezbus_mac_coldboot.h>
#include <ezbus_mac_peers.h>
#include <ezbus_mac_token.h>
#include <ezbus_hex.h>
#include <ezbus_log.h>
#include <ezbus_timer.h>

#define ezbus_mac_coldboot_set_emit_count(boot,c)      ((boot)->emit_count=(c))
#define ezbus_mac_coldboot_get_emit_count(boot)        ((boot)->emit_count)
#define ezbus_mac_coldboot_inc_emit_count(boot)        ezbus_mac_coldboot_set_emit_count(boot,ezbus_mac_coldboot_get_emit_count(boot)+1)

#define ezbus_mac_coldboot_set_emit_seq(boot,c)        ((boot)->emit_count=(c))
#define ezbus_mac_coldboot_get_emit_seq(boot)          ((boot)->emit_count)
#define ezbus_mac_coldboot_inc_emit_seq(boot)          ezbus_mac_coldboot_set_emit_count(boot,ezbus_mac_coldboot_get_emit_count(boot)+1)

static void ezbus_mac_coldboot_timer_callback_silent   ( ezbus_timer_t* timer, void* arg );
static void ezbus_mac_coldboot_timer_callback          ( ezbus_timer_t* timer, void* arg );

static void do_state_coldboot_silent_start             ( ezbus_mac_t* mac );
static void do_state_coldboot_silent_continue          ( ezbus_mac_t* mac );
static void do_state_coldboot_silent_stop              ( ezbus_mac_t* mac );

static void do_state_coldboot_start                    ( ezbus_mac_t* mac );
static void do_state_coldboot_continue                 ( ezbus_mac_t* mac );
static void do_state_coldboot_stop                     ( ezbus_mac_t* mac );
static void do_state_coldboot_dominant                 ( ezbus_mac_t* mac );


extern void ezbus_mac_coldboot_init( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_t* boot = ezbus_mac_get_coldboot( mac );

    ezbus_platform_memset( boot, 0 , sizeof( ezbus_mac_coldboot_t) );

    ezbus_timer_init( &boot->coldboot_timer );
    ezbus_timer_set_key( &boot->coldboot_timer, "coldboot_timer" );    
    ezbus_timer_set_callback( &boot->coldboot_timer, ezbus_mac_coldboot_timer_callback, mac );

    ezbus_timer_init( &boot->silent_timer );
    ezbus_timer_set_key( &boot->silent_timer, "silent_timer" );
    ezbus_timer_set_callback( &boot->silent_timer, ezbus_mac_coldboot_timer_callback_silent, mac );
}


extern void ezbus_mac_coldboot_run( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_t* boot = ezbus_mac_get_coldboot( mac );

    ezbus_timer_run( &boot->coldboot_timer );
    ezbus_timer_run( &boot->silent_timer );

    switch ( ezbus_mac_coldboot_get_state( mac ) )
    {
        case state_coldboot_silent_start:    do_state_coldboot_silent_start    ( mac );  break;
        case state_coldboot_silent_continue: do_state_coldboot_silent_continue ( mac );  break;
        case state_coldboot_silent_stop:     do_state_coldboot_silent_stop     ( mac );  break;

        case state_coldboot_start:           do_state_coldboot_start           ( mac );  break;
        case state_coldboot_continue:        do_state_coldboot_continue        ( mac );  break;
        case state_coldboot_stop:            do_state_coldboot_stop            ( mac );  break;

        case state_coldboot_dominant:        do_state_coldboot_dominant        ( mac );  break;
    }
}

extern uint8_t ezbus_mac_coldboot_get_seq( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_t* boot = ezbus_mac_get_coldboot(mac);
    return boot->seq;
}

extern const char* ezbus_mac_coldboot_get_state_str( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_t* coldboot = ezbus_mac_get_coldboot( mac );

    switch(coldboot->state)
    {
        case state_coldboot_silent_start:    return "state_coldboot_silent_start";    break;
        case state_coldboot_silent_continue: return "state_coldboot_silent_continue"; break;
        case state_coldboot_silent_stop:     return "state_coldboot_silent_stop";     break;
        
        case state_coldboot_start:           return "state_coldboot_start";           break;
        case state_coldboot_continue:        return "state_coldboot_continue";        break;
        case state_coldboot_stop:            return "state_coldboot_stop";            break;

        case state_coldboot_dominant:        return "state_coldboot_dominant";        break;
    }
    return "";
}


void ezbus_mac_coldboot_set_state( ezbus_mac_t* mac, ezbus_mac_coldboot_state_t state )
{
    ezbus_mac_coldboot_t* boot = ezbus_mac_get_coldboot( mac );
    boot->state = state;
}


ezbus_mac_coldboot_state_t ezbus_mac_coldboot_get_state( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_t* boot = ezbus_mac_get_coldboot( mac );
    return boot->state;
}


/**** SILENT BEGIN ****/

static void do_state_coldboot_silent_start( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_t* boot = ezbus_mac_get_coldboot( mac );
    ezbus_mac_coldboot_set_emit_count( boot, 0 );
    ezbus_timer_stop( &boot->coldboot_timer );
    ezbus_timer_stop( &boot->silent_timer );

    ezbus_timer_set_period( &boot->silent_timer, ezbus_mac_token_ring_time(mac)+EZBUS_COLDBOOT_SILENT_TIME );
    ezbus_timer_start( &boot->silent_timer );
    ezbus_mac_coldboot_signal_silent_start(mac);
    ezbus_mac_coldboot_set_state( mac, state_coldboot_silent_continue );

}

static void do_state_coldboot_silent_continue( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_signal_silent_continue(mac);
}

static void do_state_coldboot_silent_stop( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_t* boot = ezbus_mac_get_coldboot( mac );
    ezbus_timer_stop( &boot->coldboot_timer );
    ezbus_timer_stop( &boot->silent_timer );
    ezbus_mac_coldboot_signal_silent_stop( mac );
    ezbus_mac_coldboot_set_state( mac, state_coldboot_start );
}

static void ezbus_mac_coldboot_timer_callback_silent( ezbus_timer_t* timer, void* arg )
{
    ezbus_mac_t* mac=(ezbus_mac_t*)arg;
    if ( ezbus_timer_expired( timer ) )
    {
        ezbus_mac_coldboot_set_state( mac, state_coldboot_silent_stop );
    }
}

/**** SILENT ENDD ****/



/**** COLDBOOT BEGIN ****/

static void do_state_coldboot_start( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_t* boot = ezbus_mac_get_coldboot( mac );
    ezbus_timer_stop( &boot->silent_timer );
    ezbus_timer_stop( &boot->coldboot_timer );
    ezbus_mac_coldboot_signal_start( mac );
    ezbus_timer_set_period  (
                                &boot->coldboot_timer,
                                ezbus_platform_random( EZBUS_COLDBOOT_TIMER_MIN, EZBUS_COLDBOOT_TIMER_MAX )
                            );
    
    //fprintf( stderr, "%d\n",ezbus_timer_get_period(&boot->coldboot_timer));

    ezbus_timer_start( &boot->coldboot_timer );
    ezbus_mac_coldboot_set_state( mac, state_coldboot_continue );
}

static void do_state_coldboot_continue( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_t* boot = ezbus_mac_get_coldboot( mac );
    ++boot->seq;
    /* If I'm the "last man standing" then seize control of the bus */
    if ( ezbus_mac_coldboot_get_emit_count( boot ) > EZBUS_COLDBOOT_CYCLES )
    {
        ezbus_timer_stop( &boot->coldboot_timer );
        ezbus_mac_coldboot_set_state( mac, state_coldboot_dominant );
    }
}

static void do_state_coldboot_stop( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_t* boot = ezbus_mac_get_coldboot( mac );
    ezbus_mac_coldboot_signal_stop( mac );
    ezbus_timer_stop( &boot->coldboot_timer );
    ezbus_mac_coldboot_set_state( mac, state_coldboot_silent_start );
}

static void do_state_coldboot_dominant( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_t* boot = ezbus_mac_get_coldboot( mac );
    ++boot->seq;
    ezbus_mac_coldboot_signal_dominant( mac );
}

static void ezbus_mac_coldboot_timer_callback( ezbus_timer_t* timer, void* arg )
{
    ezbus_mac_t* mac = (ezbus_mac_t*)arg;
    ezbus_mac_coldboot_t* boot = ezbus_mac_get_coldboot( mac );
    if ( ezbus_timer_expired( timer ) )
    {
        ezbus_mac_coldboot_signal_continue( mac );
        ezbus_mac_coldboot_inc_emit_count( boot );
        ezbus_mac_coldboot_set_state( mac, state_coldboot_start );
    }
}

/**** COLDBOOT END ****/

