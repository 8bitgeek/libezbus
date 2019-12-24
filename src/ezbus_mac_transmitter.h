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
    transmitter_state_full,
    transmitter_state_send,
    transmitter_state_sent,   
    transmitter_state_wait_ack
} ezbus_mac_transmitter_state_t;

typedef struct
{
    ezbus_packet_t                      packet;
    ezbus_mac_transmitter_state_t       state;
    EZBUS_ERR                           err;
    ezbus_port_t*                       port;
    void*                               arg;
} ezbus_mac_transmitter_t;

#ifdef __cplusplus
extern "C" {
#endif

extern void  ezbus_mac_transmitter_init  ( ezbus_mac_transmitter_t* mac_transmitter, ezbus_port_t* port, extern void* callback_arg );
extern void  ezbus_mac_transmitter_run   ( ezbus_mac_transmitter_t* mac_transmitter );
extern void  ezbus_mac_transmitter_put   ( ezbus_mac_transmitter_t* mac_transmitter, ezbus_packet_t* packet );


extern void                          ezbus_mac_transmitter_set_state( ezbus_mac_transmitter_t* mac_transmitter, ezbus_mac_transmitter_state_t state );
extern ezbus_mac_transmitter_state_t ezbus_mac_transmitter_get_state( ezbus_mac_transmitter_t* mac_transmitter );
extern const char*                   ezbus_mac_transmitter_get_state_str( ezbus_mac_transmitter_t* mac_transmitter );


extern void  ezbus_mac_transmitter_empty_callback ( ezbus_mac_transmitter_t*, void* );
extern void  ezbus_mac_transmitter_full_callback  ( ezbus_mac_transmitter_t*, void* );
extern void  ezbus_mac_transmitter_sent_callback  ( ezbus_mac_transmitter_t*, void* );
extern void  ezbus_mac_transmitter_wait_callback  ( ezbus_mac_transmitter_t*, void* );
extern void  ezbus_mac_transmitter_fault_callback ( ezbus_mac_transmitter_t*, void* );


#ifdef __cplusplus
}
#endif

#endif /* EZBUS_MAC_TRANSMITTER_H_ */
