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
#ifndef EZBUS_MAC_WARMBOOT_H_
#define EZBUS_MAC_WARMBOOT_H_

#include <ezbus_platform.h>
#include <ezbus_mac.h>
#include <ezbus_timer.h>
#include <ezbus_packet.h>
#include <ezbus_port.h>
#include <ezbus_crc.h>


typedef enum
{
    state_warmboot_idle=0,
    state_warmboot_start,
    state_warmboot_continue,
    state_warmboot_stop,
    state_warmboot_finished,

} ezbus_mac_warmboot_state_t;


typedef struct _ezbus_mac_warmboot_t
{
    ezbus_mac_warmboot_state_t  state;
    uint8_t                     seq;                /* always != 0 */
    ezbus_timer_t               warmboot_timer;
    uint8_t                     warmboot_cycles;
} ezbus_mac_warmboot_t;


#ifdef __cplusplus
extern "C" {
#endif


extern void                ezbus_mac_warmboot_init             ( ezbus_mac_t* mac );
extern void                ezbus_mac_warmboot_run              ( ezbus_mac_t* mac );

extern uint8_t             ezbus_mac_warmboot_get_seq          ( ezbus_mac_t* mac );

extern void                ezbus_mac_warmboot_receive          ( ezbus_mac_t* mac, ezbus_packet_t* packet );

void                       ezbus_mac_warmboot_set_state        ( ezbus_mac_t* mac, ezbus_mac_warmboot_state_t state );
ezbus_mac_warmboot_state_t ezbus_mac_warmboot_get_state        ( ezbus_mac_t* mac );
    
extern const char*         ezbus_mac_warmboot_get_state_str    ( ezbus_mac_t* mac );

extern void                ezbus_mac_warmboot_signal_idle      ( ezbus_mac_t* mac );
extern void                ezbus_mac_warmboot_signal_start     ( ezbus_mac_t* mac );
extern void                ezbus_mac_warmboot_signal_continue  ( ezbus_mac_t* mac );
extern void                ezbus_mac_warmboot_signal_stop      ( ezbus_mac_t* mac );
extern void                ezbus_mac_warmboot_signal_finished  ( ezbus_mac_t* mac );

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_MAC_WARMBOOT_H_ */
