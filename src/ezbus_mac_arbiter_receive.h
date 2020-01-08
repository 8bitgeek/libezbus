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
#ifndef EZBUS_MAC_ARBITER_RECEIVE_H_
#define EZBUS_MAC_ARBITER_RECEIVE_H_

#include <ezbus_mac_arbiter.h>
#include <ezbus_mac.h>
#include <ezbus_timer.h>
#include <ezbus_packet.h>

typedef struct _ezbus_mac_arbiter_receive_t
{
    ezbus_timer_t               ack_rx_timer;

    uint8_t                     warmboot_seq;
    ezbus_timer_t               warmboot_timer;

} ezbus_mac_arbiter_receive_t;


#ifdef __cplusplus
extern "C" {
#endif

extern void ezbus_mac_arbiter_receive_init ( ezbus_mac_t* mac );
extern void ezbus_mac_arbiter_receive_run  ( ezbus_mac_t* mac );

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_MAC_ARBITER_RECEIVE_H_ */
