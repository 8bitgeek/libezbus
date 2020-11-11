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


extern void ezbus_mac_coldboot_init( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_t* boot = ezbus_mac_get_coldboot( mac );

    ezbus_platform_memset( boot, 0 , sizeof( ezbus_mac_coldboot_t) );

    ezbus_timer_init( &boot->major_timer );
    ezbus_timer_set_key( &boot->major_timer, "coldboot_major_timer" );    
    ezbus_timer_set_callback( &boot->major_timer, ezbus_mac_coldboot_major_timer_callback, mac );

    ezbus_timer_init( &boot->minor_timer );
    ezbus_timer_set_key( &boot->minor_timer, "coldboot_minor_timer" );
    ezbus_timer_set_callback( &boot->minor_timer, ezbus_mac_coldboot_minor_timer_callback, mac );
}


extern void ezbus_mac_coldboot_run( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_t* boot = ezbus_mac_get_coldboot( mac );

    ezbus_timer_run( &boot->major_timer );
    ezbus_timer_run( &boot->minor_timer );

    switch ( ezbus_mac_coldboot_get_state( mac ) )
    {
        case state_coldboot_minor_start:    do_state_coldboot_minor_start    ( mac );  break;
        case state_coldboot_minor_active:   do_state_coldboot_minor_active   ( mac );  break;
        case state_coldboot_minor_stop:     do_state_coldboot_minor_stop     ( mac );  break;
        case state_coldboot_major_start:    do_state_coldboot_major_start    ( mac );  break;
        case state_coldboot_major_active:    do_state_coldboot_major_active    ( mac );  break;
        case state_coldboot_major_dominant: do_state_coldboot_major_dominant ( mac );  break;
    }
}

extern const char* ezbus_mac_coldboot_get_state_str( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_t* coldboot = ezbus_mac_get_coldboot( mac );

    switch(coldboot->state)
    {
        case state_coldboot_minor_start:    return "state_coldboot_minor_start";     break;
        case state_coldboot_minor_active:   return "state_coldboot_minor_active";  break;
        case state_coldboot_minor_stop:     return "state_coldboot_minor_stop";      break;
        case state_coldboot_major_start:    return "state_coldboot_major_start";     break;
        case state_coldboot_major_active:    return "state_coldboot_major_active";  break;
        case state_coldboot_major_dominant: return "state_coldboot_major_dominant";  break;
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
