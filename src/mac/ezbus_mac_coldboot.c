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
#include <ezbus_log.h>
#include <ezbus_timer.h>

static void ezbus_mac_coldboot_timer_callback          ( ezbus_timer_t* timer, void* arg );

static void do_state_coldboot_start                    ( ezbus_mac_t* mac );
static void do_state_coldboot_continue                 ( ezbus_mac_t* mac );
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
    ezbus_timer_set_callback( &boot->silent_timer, ezbus_mac_coldboot_minor_timer_callback, mac );
}


extern void ezbus_mac_coldboot_run( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_t* boot = ezbus_mac_get_coldboot( mac );

    ezbus_timer_run( &boot->coldboot_timer );
    ezbus_timer_run( &boot->silent_timer );

    switch ( ezbus_mac_coldboot_get_state( mac ) )
    {
        case state_coldboot_minor_start:    do_state_coldboot_minor_start    ( mac );  break;
        case state_coldboot_minor_continue: do_state_coldboot_minor_continue ( mac );  break;
        case state_coldboot_minor_stop:     do_state_coldboot_minor_stop     ( mac );  break;

        case state_coldboot_start:          do_state_coldboot_start          ( mac );  break;
        case state_coldboot_continue:       do_state_coldboot_continue       ( mac );  break;

        case state_coldboot_dominant:       do_state_coldboot_dominant       ( mac );  break;
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
        case state_coldboot_minor_start:    return "state_coldboot_minor_start";    break;
        case state_coldboot_minor_continue: return "state_coldboot_minor_continue"; break;
        case state_coldboot_minor_stop:     return "state_coldboot_minor_stop";     break;
        
        case state_coldboot_start:           return "state_coldboot_start";           break;
        case state_coldboot_continue:        return "state_coldboot_continue";        break;

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

