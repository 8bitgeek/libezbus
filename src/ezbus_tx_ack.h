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
#ifndef EZBUS_TX_ACK_H_
#define EZBUS_TX_ACK_H_

#include <ezbus_platform.h>
#include <ezbus_timer.h>
#include <ezbus_address.h>
#include <ezbus_peer_list.h>

typedef enum
{
    tx_ack_state_silent_start=0,
    tx_ack_state_silent_continue,
    tx_ack_state_silent_stop,
    tx_ack_state_emit_start,
    tx_ack_state_emit_continue,
    tx_ack_state_emit_stop,
} ezbus_tx_ack_state_t;

typedef struct _ezbus_tx_ack_t
{
    uint32_t            baud_rate;
    ezbus_peer_list_t*  peer_list;
    ezbus_timer_t       token_timer;
    ezbus_timer_t       ack_timer;
    ezbus_tx_ack_state_t state;
    void*               callback_arg;
    void                (*callback)( struct _ezbus_tx_ack_t*, void* arg );
 } ezbus_tx_ack_t;

typedef void (*ezbus_tx_ack_callback_t)( struct _ezbus_tx_ack_t*, void* arg );

#ifdef __cplusplus
extern "C" {
#endif

#define ezbus_tx_ack_set_state(tx_ack,s)      ((tx_ack)->state=(s))
#define ezbus_tx_ack_get_state(tx_ack)        ((tx_ack)->state)

extern void ezbus_tx_ack_init(   
                                ezbus_tx_ack_t* tx_ack, 
                                uint32_t baud_rate, 
                                ezbus_peer_list_t* peer_list, 
                                ezbus_tx_ack_callback_t callback, 
                                void* callback_arg 
                            );

extern void ezbus_tx_ack_run( ezbus_tx_ack_t* tx_ack );


extern void ezbus_tx_ack_signal_token_seen ( ezbus_tx_ack_t* tx_ack );
extern void ezbus_tx_ack_signal_ack_seen  ( ezbus_tx_ack_t* tx_ack, ezbus_address_t* address );

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_TX_ACK_H_ */
