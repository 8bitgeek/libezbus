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
#ifndef EZBUS_LAYER0_TRANSCEIVER_H_
#define EZBUS_LAYER0_TRANSCEIVER_H_

#include <ezbus_platform.h>
#include <ezbus_layer0_transmitter.h>
#include <ezbus_layer0_receiver.h>
#include <ezbus_address.h>
#include <ezbus_peer_list.h>

typedef enum
{
    hello_state_idle=0,
    hello_state_init,
    hello_state_emit,
    hello_state_wait,
    hello_state_term,
} ezbus_hello_state_t;

/* callback intended to retrieve the address of the node address to the next-in-token-ring */
typedef ezbus_address_t*    (*ezbus_next_in_token_ring_callback_t)  ( ezbus_address_t* );

/* callback intended to retrieve the peer list */
typedef ezbus_address_t*    (*ezbus_peer_list_callback_t)   ( ezbus_peer_list_t* );

typedef struct _ezbus_layer0_transceiver_t
{
    ezbus_port_t*                           port;
    ezbus_layer0_transmitter_t              layer0_transmitter;
    ezbus_layer0_receiver_t                 layer0_receiver;
 
    ezbus_next_in_token_ring_callback_t     next_in_token_ring_callback;
    ezbus_peer_list_callback_t              peer_list_callback;
 
    bool                                    (*layer1_tx_callback)(struct _ezbus_layer0_transceiver_t*);
    bool                                    (*layer1_rx_callback)(struct _ezbus_layer0_transceiver_t*);

    ezbus_ms_tick_t                         transmitter_full_time;

    ezbus_ms_tick_t                         token_time;

    ezbus_hello_state_t                     hello_state;
    ezbus_ms_tick_t                         hello_time;
    ezbus_ms_tick_t                         hello_period;

    ezbus_ms_tick_t                         ack_begin;
    ezbus_packet_t                          ack_packet;
    bool                                    ack_pending;
 
 } ezbus_layer0_transceiver_t;

typedef bool (*ezbus_layer1_callback_t)( struct _ezbus_layer0_transceiver_t* );


#ifdef __cplusplus
extern "C" {
#endif

#define ezbus_layer0_transceiver_get_transmitter(layer0_transceiver)     (&(layer0_transceiver)->layer0_transmitter)
#define ezbus_layer0_transceiver_get_receiver(layer0_transceiver)        (&(layer0_transceiver)->layer0_receiver)
#define ezbus_layer0_transceiver_get_ack_packet(layer0_transceiver)      (&(layer0_transceiver)->ack_packet)
#define ezbus_layer0_transceiver_set_ack_pending(layer0_transceiver,p)   ((layer0_transceiver)->ack_pending=(p))
#define ezbus_layer0_transceiver_get_ack_pending(layer0_transceiver)     ((layer0_transceiver)->ack_pending)
#define ezbus_layer0_transceiver_set_ack_begin(layer0_transceiver,p)     ((layer0_transceiver)->ack_begin=(p))
#define ezbus_layer0_transceiver_get_ack_begin(layer0_transceiver)       ((layer0_transceiver)->ack_begin)
#define ezbus_layer0_transceiver_set_token_time(layer0_tranceiver,t)     ((layer0_transceiver)->token_time=(t))
#define ezbus_layer0_transceiver_get_token_time(layer0_tranceiver)       ((layer0_transceiver)->token_time)
#define ezbus_layer0_transceiver_get_hello_state(layer0_transceiver)     ((layer0_transceiver)->hello_state)
#define ezbus_layer0_transceiver_set_hello_state(layer0_transceiver,h)   ((layer0_transceiver)->hello_state=(h))
#define ezbus_layer0_transceiver_get_hello_time(layer0_transceiver)      ((layer0_transceiver)->hello_time)
#define ezbus_layer0_transceiver_set_hello_time(layer0_transceiver,t)    ((layer0_transceiver)->hello_time=(t))
#define ezbus_layer0_transceiver_get_hello_period(layer0_transceiver)    ((layer0_transceiver)->hello_period)
#define ezbus_layer0_transceiver_set_hello_period(layer0_transceiver,t)  ((layer0_transceiver)->hello_period=(t))

void ezbus_layer0_transceiver_init (    

                                    ezbus_layer0_transceiver_t*         layer0_transceiver, 
                                    ezbus_port_t*                       port,

                                    ezbus_next_in_token_ring_callback_t next_in_token_ring_callback, 
                                    ezbus_peer_list_callback_t          peer_list_callback,

                                    ezbus_layer1_callback_t             layer1_tx_callback,
                                    ezbus_layer1_callback_t             layer1_rx_callback

                                    );

void ezbus_layer0_transceiver_run  ( ezbus_layer0_transceiver_t* layer0_transceiver );

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_LAYER0_TRANSCEIVER_H_ */
