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
#ifndef EZBUS_MAC_TRANSMITTER_H_
#define EZBUS_MAC_TRANSMITTER_H_

#include <ezbus_platform.h>
#include <ezbus_packet.h>
#include <ezbus_port.h>

typedef enum
{
    transmitter_state_empty=0,
    transmitter_state_transit_full,
    transmitter_state_full,
    transmitter_state_send,
    transmitter_state_give_token,   
    transmitter_state_transit_wait_ack,
    transmitter_state_wait_ack
} ezbus_mac_transmitter_state_t;

typedef struct _ezbus_later0_transmitter_t
{
    ezbus_packet_t                      packet;
    ezbus_mac_transmitter_state_t    state;
    EZBUS_ERR                           err;
    ezbus_port_t*                       port;
    bool                                (*callback)(struct _ezbus_later0_transmitter_t*,void*);
    void*                               arg;
} ezbus_mac_transmitter_t;

typedef bool (*ezbus_transmitter_callback_t) ( struct _ezbus_later0_transmitter_t*, void* );

#define ezbus_mac_transmitter_set_state(mac_transmitter,s)        ((mac_transmitter)->state=(s))
#define ezbus_mac_transmitter_get_state(mac_transmitter)          ((mac_transmitter)->state)
#define ezbus_mac_transmitter_empty(mac_transmitter)              (ezbus_mac_transmitter_get_state((mac_transmitter))==transmitter_state_empty)
#define ezbus_mac_transmitter_full(mac_transmitter)               (ezbus_mac_transmitter_get_state((mac_transmitter))!=transmitter_state_empty)
#define ezbus_mac_transmitter_get_port(mac_transmitter)           ((mac_transmitter)->port)
#define ezbus_mac_transmitter_get_packet(mac_transmitter)         (&(mac_transmitter)->packet)
#define ezbus_mac_transmitter_set_err(mac_transmitter,r)          ((mac_transmitter)->err=(r))
#define ezbus_mac_transmitter_get_err(mac_transmitter)            ((mac_transmitter))

#ifdef __cplusplus
extern "C" {
#endif



void  ezbus_mac_transmitter_init  ( ezbus_mac_transmitter_t* mac_transmitter, ezbus_port_t* port, ezbus_transmitter_callback_t callback, void* arg );
void  ezbus_mac_transmitter_run   ( ezbus_mac_transmitter_t* mac_transmitter );
void  ezbus_mac_transmitter_put   ( ezbus_mac_transmitter_t* mac_transmitter, ezbus_packet_t* packet );

const char* ezbus_mac_transmitter_get_state_str( ezbus_mac_transmitter_t* mac_transmitter );

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_MAC_TRANSMITTER_H_ */
