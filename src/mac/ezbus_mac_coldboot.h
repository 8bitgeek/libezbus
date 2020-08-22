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
#ifndef EZBUS_MAC_COLDBOOT_H_
#define EZBUS_MAC_COLDBOOT_H_

#include <ezbus_platform.h>
#include <ezbus_timer.h>
#include <ezbus_packet.h>
#include <ezbus_port.h>
#include <ezbus_mac.h>
#include <ezbus_mac_coldboot_minor.h>
#include <ezbus_mac_coldboot_major.h>

typedef enum
{
    state_coldboot_minor_start=0,
    state_coldboot_minor_continue,
    state_coldboot_minor_stop,
    
    state_coldboot_major_start,
    state_coldboot_major_continue,
    state_coldboot_major_dominant

} ezbus_mac_coldboot_state_t;

typedef struct _ezbus_mac_coldboot_t
{
    ezbus_mac_coldboot_state_t state;

    ezbus_timer_t               silent_timer;
    ezbus_timer_t               coldboot_timer;

    uint32_t                    emit_count;
    uint8_t                     seq;

} ezbus_mac_coldboot_t;


#ifdef __cplusplus
extern "C" {
#endif

#define                     ezbus_mac_coldboot_reset(mac) \
                                    ezbus_mac_coldboot_set_state(mac,state_coldboot_minor_start);

extern void                 ezbus_mac_coldboot_init        ( ezbus_mac_t* mac );
extern void                 ezbus_mac_coldboot_run         ( ezbus_mac_t* mac );

void                       ezbus_mac_coldboot_set_state    ( ezbus_mac_t* mac, ezbus_mac_coldboot_state_t state );
ezbus_mac_coldboot_state_t ezbus_mac_coldboot_get_state    ( ezbus_mac_t* mac );
extern const char*         ezbus_mac_coldboot_get_state_str( ezbus_mac_t* mac );


#ifdef __cplusplus
}
#endif

#endif /* EZBUS_MAC_COLDBOOT_H_ */
