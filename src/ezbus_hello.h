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
#ifndef EZBUS_HELLO_H_
#define EZBUS_HELLO_H_

#include <ezbus_platform.h>
#include <ezbus_timer.h>
#include <ezbus_address.h>
#include <ezbus_peer_list.h>

typedef enum
{
    hello_state_silent_start=0,
    hello_state_silent_continue,
    hello_state_silent_stop,
    hello_state_emit_start,
    hello_state_emit_continue,
    hello_state_emit_stop,
} ezbus_hello_state_t;

typedef struct _ezbus_hello_t
{
    uint32_t            baud_rate;
    ezbus_peer_list_t*  peer_list;
    ezbus_timer_t       token_timer;
    ezbus_timer_t       emit_timer;
    ezbus_hello_state_t state;
    void*               callback_arg;
    void                (*callback)( struct _ezbus_hello_t*, void* arg );
 } ezbus_hello_t;

typedef void (*ezbus_hello_callback_t)( struct _ezbus_hello_t*, void* arg );

#ifdef __cplusplus
extern "C" {
#endif

#define ezbus_hello_set_state(hello,s)      ((hello)->state=(s))
#define ezbus_hello_get_state(hello)        ((hello)->state)

extern void ezbus_hello_init(   
                                ezbus_hello_t* hello, 
                                uint32_t baud_rate, 
                                ezbus_peer_list_t* peer_list, 
                                ezbus_hello_callback_t callback, 
                                void* callback_arg 
                            );

extern void ezbus_hello_run( ezbus_hello_t* hello );


extern void ezbus_hello_signal_token_seen ( ezbus_hello_t* hello );
extern void ezbus_hello_signal_peer_seen  ( ezbus_hello_t* hello, ezbus_address_t* address );

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_HELLO_H_ */
