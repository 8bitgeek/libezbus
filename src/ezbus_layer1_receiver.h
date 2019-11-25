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
#ifndef EZBUS_LAYER1_RECEIVER_H_
#define EZBUS_LAYER1_RECEIVER_H_

#include <ezbus_platform.h>
#include <ezbus_packet.h>
#include <ezbus_port.h>

typedef enum
{
	receiver_state_empty=0,
	receiver_state_full,
} ezbus_receiver_state_t;

typedef struct _ezbus_receiver_t
{
    EZBUS_ERR       		err;
    ezbus_receiver_state_t  state;
    ezbus_port_t*			port;
    bool                    (*callback)(struct _ezbus_receiver_t*,void*);
    void*                   arg;
} ezbus_layer1_receiver_t;

typedef bool (*ezbus_receiver_callback_t) ( struct _ezbus_receiver_t*, void* );

#define ezbus_layer1_receiver_set_state(layer1_receiver,s) 		((layer1_receiver)->state=(s))
#define ezbus_layer1_receiver_get_state(layer1_receiver) 		((layer1_receiver)->state)
#define ezbus_layer1_receiver_empty(layer1_receiver) 			(ezbus_layer1_receiver_get_state((layer1_receiver))==receiver_state_empty)
#define ezbus_layer1_receiver_full(layer1_receiver) 			(ezbus_layer1_receiver_get_state((layer1_receiver))!=receiver_state_empty)
#define ezbus_layer1_receiver_get_port(layer1_receiver)			((layer1_receiver)->port)
#define ezbus_layer1_receiver_get_packet(layer1_receiver)		(&(layer1_receiver)->packet)
#define ezbus_layer1_receiver_set_err(layer1_receiver,r)		((layer1_receiver)->err=(r))
#define ezbus_layer1_receiver_get_err(layer1_receiver)			((layer1_receiver))

#ifdef __cplusplus
extern "C" {
#endif


void ezbus_layer1_receiver_init ( ezbus_layer1_receiver_t* layer1_receiver, ezbus_port_t* port, ezbus_receiver_callback_t callback, void* arg );
void ezbus_layer1_receiver_run  ( ezbus_layer1_receiver_t* layer1_receiver );
void ezbus_layer1_receiver_get  ( ezbus_layer1_receiver_t* layer1_receiver, ezbus_string_t* string );


#ifdef __cplusplus
}
#endif


#endif /* EZBUS_LAYER1_RECEIVER_H_ */
