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
#ifndef EZBUS_MAC_RECEIVER_H_
#define EZBUS_MAC_RECEIVER_H_

#include <ezbus_platform.h>
#include <ezbus_mac.h>
#include <ezbus_timer.h>

typedef enum
{
    receiver_state_empty=0,
    receiver_state_full,
    receiver_state_receive_fault,
    receiver_state_transit_to_ack,
    receiver_state_wait_ack_sent
} ezbus_receiver_state_t;

typedef struct _ezbus_mac_receiver_t
{
    ezbus_packet_t          packet;
    EZBUS_ERR               err;
    ezbus_receiver_state_t  state;
} ezbus_mac_receiver_t;

#ifdef __cplusplus
extern "C" {
#endif

void ezbus_mac_receiver_init ( ezbus_mac_t* mac );
void ezbus_mac_receiver_run  ( ezbus_mac_t* mac );
void ezbus_mac_receiver_get  ( ezbus_mac_t* mac, ezbus_packet_t* packet );

extern void ezbus_mac_receiver_signal_empty ( ezbus_mac_t* mac );
extern void ezbus_mac_receiver_signal_full  ( ezbus_mac_t* mac );
extern void ezbus_mac_receiver_signal_sent  ( ezbus_mac_t* mac );
extern void ezbus_mac_receiver_signal_wait  ( ezbus_mac_t* mac );
extern void ezbus_mac_receiver_signal_fault ( ezbus_mac_t* mac );

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_MAC_RECEIVER_H_ */
