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

extern void do_state_coldboot_major_start( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_t* boot = ezbus_mac_get_coldboot( mac );
    ezbus_timer_stop( &boot->minor_timer );
    ezbus_timer_stop( &boot->major_timer );
    ezbus_mac_coldboot_major_signal_start( mac );
    ezbus_timer_set_period  (
                                &boot->major_timer,
                                ezbus_platform_random( EZBUS_COLDBOOT_TIMER_MIN, EZBUS_COLDBOOT_TIMER_MAX )
                            );
    
    //fprintf( stderr, "%d\n",ezbus_timer_get_period(&boot->major_timer));

    ezbus_timer_start( &boot->major_timer );
    ezbus_mac_coldboot_set_state( mac, state_coldboot_major_acive );
}

extern void do_state_coldboot_major_acive( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_t* boot = ezbus_mac_get_coldboot( mac );
    ++boot->seq;
    /* If I'm the "last man standing" then seize control of the bus */
    if ( ezbus_mac_coldboot_get_emit_count( boot ) > EZBUS_COLDBOOT_CYCLES )
    {
        ezbus_timer_stop( &boot->major_timer );
        ezbus_mac_coldboot_set_state( mac, state_coldboot_major_dominant );
    }
}

extern void do_state_coldboot_major_dominant( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_t* boot = ezbus_mac_get_coldboot( mac );
    ++boot->seq;
    ezbus_mac_coldboot_major_signal_dominant( mac );
}

extern void ezbus_mac_coldboot_major_timer_callback( ezbus_timer_t* timer, void* arg )
{
    ezbus_mac_t* mac = (ezbus_mac_t*)arg;
    ezbus_mac_coldboot_t* boot = ezbus_mac_get_coldboot( mac );
    if ( ezbus_timer_expired( timer ) )
    {
        ezbus_mac_coldboot_major_signal_active( mac );
        ezbus_mac_coldboot_inc_emit_count( boot );
        ezbus_mac_coldboot_set_state( mac, state_coldboot_major_start );
    }
}

extern uint8_t ezbus_mac_coldboot_get_seq( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_t* boot = ezbus_mac_get_coldboot(mac);
    return boot->seq;
}

