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
#include <ezbus_packet.h>
#include <ezbus_port.h>

typedef enum
{
    receiver_state_empty=0,
    receiver_state_full,
    receiver_state_receive_fault,
    receiver_state_transit_to_ack,
    receiver_state_wait_ack_sent
} ezbus_receiver_state_t;

typedef struct _ezbus_receiver_t
{
    ezbus_packet_t          packet;
    EZBUS_ERR               err;
    ezbus_receiver_state_t  state;
    ezbus_port_t*           port;
    bool                    (*callback)(struct _ezbus_receiver_t*,void*);
    void*                   arg;
} ezbus_mac_receiver_t;

typedef bool (*ezbus_receiver_callback_t) ( struct _ezbus_receiver_t*, void* );

#define ezbus_mac_receiver_set_state(mac_receiver,s)      ((mac_receiver)->state=(s))
#define ezbus_mac_receiver_get_state(mac_receiver)        ((mac_receiver)->state)
#define ezbus_mac_receiver_empty(mac_receiver)            (ezbus_mac_receiver_get_state((mac_receiver))==receiver_state_empty)
#define ezbus_mac_receiver_full(mac_receiver)             (ezbus_mac_receiver_get_state((mac_receiver))!=receiver_state_empty)
#define ezbus_mac_receiver_get_port(mac_receiver)         ((mac_receiver)->port)
#define ezbus_mac_receiver_get_packet(mac_receiver)       (&(mac_receiver)->packet)
#define ezbus_mac_receiver_set_err(mac_receiver,r)        ((mac_receiver)->err=(r))
#define ezbus_mac_receiver_get_err(mac_receiver)          ((mac_receiver)->err)

#ifdef __cplusplus
extern "C" {
#endif


void ezbus_mac_receiver_init ( ezbus_mac_receiver_t* mac_receiver, ezbus_port_t* port, ezbus_receiver_callback_t callback, void* arg );
void ezbus_mac_receiver_run  ( ezbus_mac_receiver_t* mac_receiver );
void ezbus_mac_receiver_get  ( ezbus_mac_receiver_t* mac_receiver, ezbus_packet_t* packet );


#ifdef __cplusplus
}
#endif


#endif /* EZBUS_MAC_RECEIVER_H_ */
