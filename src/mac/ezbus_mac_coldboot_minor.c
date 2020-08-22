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

extern void do_state_coldboot_minor_start( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_t* boot = ezbus_mac_get_coldboot( mac );
    ezbus_mac_coldboot_set_emit_count( boot, 0 );
    ezbus_timer_stop( &boot->major_timer );
    ezbus_timer_stop( &boot->minor_timer );

    ezbus_timer_set_period( &boot->minor_timer, ezbus_mac_token_ring_time(mac)+EZBUS_COLDBOOT_MINOR_TIME );
    ezbus_timer_start( &boot->minor_timer );
    ezbus_mac_coldboot_minor_signal_start(mac);
    ezbus_mac_coldboot_set_state( mac, state_coldboot_minor_active );

}

extern void do_state_coldboot_minor_active( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_minor_signal_continue(mac);
}

extern void do_state_coldboot_minor_stop( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_t* boot = ezbus_mac_get_coldboot( mac );
    ezbus_timer_stop( &boot->major_timer );
    ezbus_timer_stop( &boot->minor_timer );
    ezbus_mac_coldboot_minor_signal_stop( mac );
    ezbus_mac_coldboot_set_state( mac, state_coldboot_major_start );
}

extern void ezbus_mac_coldboot_minor_timer_callback( ezbus_timer_t* timer, void* arg )
{
    ezbus_mac_t* mac=(ezbus_mac_t*)arg;
    if ( ezbus_timer_expired( timer ) )
    {
        ezbus_mac_coldboot_set_state( mac, state_coldboot_minor_stop );
    }
}
