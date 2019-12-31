/*****************************************************************************
* Copyright 2019 Mike Sharkey <mike.sharkey@mineairquality.com>              *
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
#include <ezbus_mac_peers.h>
#include <ezbus_hex.h>
#include <ezbus_log.h>
#include <ezbus_timer.h>

static void do_state_warmboot_idle               ( ezbus_mac_t* mac );
static void do_state_warmboot_start              ( ezbus_mac_t* mac );
static void do_state_warmboot_continue           ( ezbus_mac_t* mac );
static void do_state_warmboot_stop               ( ezbus_mac_t* mac );

extern void ezbus_mac_warmboot_init( ezbus_mac_t* mac )
{
    ezbus_mac_warmboot_t* boot = ezbus_mac_get_warmboot( mac );

    ezbus_platform_memset( boot, 0 , sizeof( ezbus_mac_warmboot_t) );
}


extern void ezbus_mac_warmboot_run( ezbus_mac_t* mac )
{
    switch ( ezbus_mac_warmboot_get_state( mac ) )
    {
        case state_warmboot_idle:     do_state_warmboot_idle     ( mac );  break;
        case state_warmboot_start:    do_state_warmboot_start    ( mac );  break;
        case state_warmboot_continue: do_state_warmboot_continue ( mac );  break;
        case state_warmboot_stop:     do_state_warmboot_stop     ( mac );  break;
    }
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
    }
    return "";
}


void ezbus_mac_warmboot_set_state( ezbus_mac_t* mac, ezbus_mac_warmboot_state_t state )
{
    ezbus_mac_warmboot_t* boot = ezbus_mac_get_warmboot( mac );
    boot->state = state;
}


ezbus_mac_warmboot_state_t ezbus_mac_warmboot_get_state( ezbus_mac_t* mac )
{
    ezbus_mac_warmboot_t* boot = ezbus_mac_get_warmboot( mac );
    return boot->state;
}


static void do_state_warmboot_idle( ezbus_mac_t* mac )
{
    /* FIXME insert code here */
}

static void do_state_warmboot_start( ezbus_mac_t* mac )
{
    /* FIXME insert code here */
}

static void do_state_warmboot_continue( ezbus_mac_t* mac )
{
    /* FIXME insert code here */
}

static void do_state_warmboot_stop( ezbus_mac_t* mac )
{
    /* FIXME insert code here */
}



extern void ezbus_mac_arbiter_receive_signal_warmboot( ezbus_mac_t* mac, ezbus_packet_t* rx_packet )
{
    ezbus_log( EZBUS_LOG_WARMBOOT, "%cwarmboot <%s %3d | ", ezbus_mac_get_token(mac)?'*':' ', ezbus_address_string( ezbus_packet_src( rx_packet ) ), ezbus_packet_seq( rx_packet ) );
    #if EZBUS_LOG_WARMBOOT
        ezbus_mac_peers_log( mac );
    #endif
}
