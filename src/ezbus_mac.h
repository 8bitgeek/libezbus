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
#ifndef EZBUS_MAC_TRANSCEIVER_H_
#define EZBUS_MAC_TRANSCEIVER_H_

#include <ezbus_platform.h>
#include <ezbus_mac_transmitter.h>
#include <ezbus_mac_receiver.h>
#include <ezbus_address.h>
#include <ezbus_peer_list.h>
#include <ezbus_timer.h>
#include <ezbus_boot.h>

typedef struct _ezbus_mac_t
{
    ezbus_port_t*                           port;
    ezbus_mac_transmitter_t                 mac_transmitter;
    ezbus_mac_receiver_t                    mac_receiver;
 
    bool                                    (*layer1_tx_callback)(struct _ezbus_mac_t*);
    bool                                    (*layer1_rx_callback)(struct _ezbus_mac_t*);

    ezbus_boot_t                            boot;
    ezbus_timer_t                           ack_tx_timer;
    ezbus_timer_t                           ack_rx_timer;

    bool                                    token;
    uint8_t                                 token_packet_seq;
    uint16_t                                token_counter;

    ezbus_boot_state_t                      boot_state;
    ezbus_ms_tick_t                         boot_time;
    ezbus_ms_tick_t                         boot_period;
    uint8_t                                 coldboot_seq;
    uint8_t                                 warmboot_seq;

    ezbus_ms_tick_t                         ack_tx_begin;
    uint8_t                                 ack_tx_retry;

    ezbus_ms_tick_t                         ack_rx_begin;
    ezbus_packet_t                          ack_rx_packet;
    bool                                    ack_rx_pending;

    ezbus_peer_list_t                       peer_list;
 
 } ezbus_mac_t;

typedef bool (*ezbus_layer1_callback_t)( struct _ezbus_mac_t* );


#ifdef __cplusplus
extern "C" {
#endif

#define ezbus_mac_get_transmitter(mac)              (&(mac)->mac_transmitter)
#define ezbus_mac_get_receiver(mac)                 (&(mac)->mac_receiver)

#define ezbus_mac_set_ack_tx_begin(mac,p)           ((mac)->ack_tx_begin=(p))
#define ezbus_mac_get_ack_tx_begin(mac)             ((mac)->ack_tx_begin)
#define ezbus_mac_set_ack_tx_retry(mac,p)           ((mac)->ack_tx_retry=(p))
#define ezbus_mac_get_ack_tx_retry(mac)             ((mac)->ack_tx_retry)

#define ezbus_mac_get_ack_rx_packet(mac)            (&(mac)->ack_rx_packet)
#define ezbus_mac_set_ack_rx_pending(mac,p)         ((mac)->ack_rx_pending=(p))
#define ezbus_mac_get_ack_rx_pending(mac)           ((mac)->ack_rx_pending)
#define ezbus_mac_set_ack_rx_begin(mac,p)           ((mac)->ack_rx_begin=(p))
#define ezbus_mac_get_ack_rx_begin(mac)             ((mac)->ack_rx_begin)

#define ezbus_mac_set_token(mac_tranceiver,t)       ((mac_tranceiver)->token=(t))
#define ezbus_mac_get_token(mac_tranceiver)         ((mac_tranceiver)->token)

#define ezbus_mac_get_boot(mac)                     (&(mac)->boot)
#define ezbus_mac_get_boot_state(mac)               ((mac)->boot_state)
#define ezbus_mac_set_boot_state(mac,h)             ((mac)->boot_state=(h))
#define ezbus_mac_get_boot_time(mac)                ((mac)->boot_time)
#define ezbus_mac_set_boot_time(mac,t)              ((mac)->boot_time=(t))
#define ezbus_mac_get_boot_period(mac)              ((mac)->boot_period)
#define ezbus_mac_set_boot_period(mac,t)            ((mac)->boot_period=(t))

void ezbus_mac_init (    

                                    ezbus_mac_t*         mac, 
                                    ezbus_port_t*                       port,
                                    ezbus_layer1_callback_t             layer1_tx_callback,
                                    ezbus_layer1_callback_t             layer1_rx_callback
                                    );

void ezbus_mac_run  ( ezbus_mac_t* mac );

#define ezbus_mac_put_packet(mac,packet) ezbus_mac_transmitter_put(ezbus_mac_get_transmitter((mac),(packet)))
#define ezbus_mac_get_packet(mac,packet) ezbus_mac_receiver_get(ezbus_mac_get_receiver((mac),(packet)))


#ifdef __cplusplus
}
#endif

#endif /* EZBUS_MAC_TRANSCEIVER_H_ */
