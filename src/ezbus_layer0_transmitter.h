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
#ifndef EZBUS_LAYER0_TRANSMITTER_H_
#define EZBUS_LAYER0_TRANSMITTER_H_

#include <ezbus_platform.h>
#include <ezbus_packet.h>
#include <ezbus_port.h>

typedef enum
{
	transmitter_state_empty=0,
	transmitter_state_full,
	transmitter_state_send,
	transmitter_state_give_token,	
	transmitter_state_wait_ack
} ezbus_layer0_transmitter_state_t;

typedef struct _ezbus_later0_transmitter_t
{
	ezbus_packet_t 						packet;
    ezbus_layer0_transmitter_state_t  	state;
    EZBUS_ERR       					err;
    ezbus_port_t*						port;
    bool                    			(*callback)(struct _ezbus_later0_transmitter_t*,void*);
    void*                       		arg;
    bool								token;
} ezbus_layer0_transmitter_t;

typedef bool (*ezbus_transmitter_callback_t) ( struct _ezbus_later0_transmitter_t*, void* );

#define ezbus_layer0_transmitter_set_state(layer0_transmitter,s) 		((layer0_transmitter)->state=(s))
#define ezbus_layer0_transmitter_get_state(layer0_transmitter) 			((layer0_transmitter)->state)
#define ezbus_layer0_transmitter_empty(layer0_transmitter)				(ezbus_layer0_transmitter_get_state((layer0_transmitter))==transmitter_state_empty)
#define ezbus_layer0_transmitter_full(layer0_transmitter) 				(ezbus_layer0_transmitter_get_state((layer0_transmitter))!=transmitter_state_empty)
#define ezbus_layer0_transmitter_get_port(layer0_transmitter) 			((layer0_transmitter)->port)
#define ezbus_layer0_transmitter_get_packet(layer0_transmitter)			((layer0_transmitter)->packet)
#define ezbus_layer0_transmitter_set_err(layer0_transmitter,r)			((layer0_transmitter)->err=(r))
#define ezbus_layer0_transmitter_get_err(layer0_transmitter)			((layer0_transmitter))
#define ezbus_layer0_transmitter_set_token(layer0_transmitter,t)		((layer0_transmitter)->token=(t))
#define ezbus_layer0_transmitter_get_token(layer0_transmitter)			((layer0_transmitter))

#ifdef __cplusplus
extern "C" {
#endif



void ezbus_layer0_transmitter_init  ( ezbus_layer0_transmitter_t* layer0_transmitter, ezbus_port_t* port, ezbus_transmitter_callback_t callback, void* arg );
void ezbus_layer0_transmitter_run   ( ezbus_layer0_transmitter_t* layer0_transmitter );
void ezbus_layer0_transmitter_put   ( ezbus_layer0_transmitter_t* layer0_transmitter, ezbus_packet_t* packet );


#ifdef __cplusplus
}
#endif

#endif /* EZBUS_LAYER0_TRANSMITTER_H_ */
