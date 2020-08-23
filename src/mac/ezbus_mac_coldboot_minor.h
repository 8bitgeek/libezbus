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
#ifndef EZBUS_MAC_COLDBOOT_MINOR_H_
#define EZBUS_MAC_COLDBOOT_MINOR_H_

#include <ezbus_platform.h>
#include <ezbus_timer.h>
#include <ezbus_packet.h>
#include <ezbus_port.h>
#include <ezbus_mac.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void ezbus_mac_coldboot_minor_signal_start    ( ezbus_mac_t* mac );
extern void ezbus_mac_coldboot_minor_signal_active   ( ezbus_mac_t* mac );
extern void ezbus_mac_coldboot_minor_signal_stop     ( ezbus_mac_t* mac );

extern void do_state_coldboot_minor_start            ( ezbus_mac_t* mac );
extern void do_state_coldboot_minor_active           ( ezbus_mac_t* mac );
extern void do_state_coldboot_minor_stop             ( ezbus_mac_t* mac );

extern void ezbus_mac_coldboot_minor_timer_callback  ( ezbus_timer_t* timer, void* arg );

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_MAC_COLDBOOT_MINOR_H_ */
