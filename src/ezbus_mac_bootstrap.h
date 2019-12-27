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
#ifndef EZBUS_MAC_BOOTSTRAP_H_
#define EZBUS_MAC_BOOTSTRAP_H_

#include <ezbus_platform.h>
#include <ezbus_timer.h>
#include <ezbus_packet.h>
#include <ezbus_port.h>
#include <ezbus_peer_list.h>
#include <ezbus_mac_arbitration.h>
#include <ezbus_mac_transmitter.h>
#include <ezbus_mac_receiver.h>

typedef enum
{
    boot_state_silent_start=0,
    boot_state_silent_continue,
    boot_state_silent_stop,
    
    boot_state_coldboot_start,
    boot_state_coldboot_continue,
    boot_state_coldboot_stop,

    boot_state_warmboot_tx_first,
    boot_state_warmboot_tx_start,
    boot_state_warmboot_tx_restart,
    boot_state_warmboot_tx_continue,
    boot_state_warmboot_tx_stop,

    boot_state_warmboot_rx_start,
    boot_state_warmboot_rx_continue,
    boot_state_warmboot_rx_stop,
} ezbus_mac_bootstrap_state_t;

typedef struct _ezbus_mac_bootstrap_t
{
    ezbus_port_t*               port;

    ezbus_peer_list_t           peer_list;
    ezbus_mac_transmitter_t     transmitter;
    ezbus_mac_receiver_t        receiver;
    ezbus_mac_arbitration_t     arbitration;

    ezbus_timer_t               silent_timer;
    ezbus_timer_t               coldboot_timer;
    ezbus_timer_t               warmboot_reply_timer;
    ezbus_timer_t               warmboot_send_timer;

    uint32_t                    emit_count;
    ezbus_mac_bootstrap_state_t state;
    uint8_t                     seq;

    uint8_t                     warmboot_count;
    ezbus_crc_t                 warmboot_crc;

 } ezbus_mac_bootstrap_t;

#ifdef __cplusplus
extern "C" {
#endif

#define ezbus_mac_bootstrap_set_emit_count(boot,c)      ((boot)->emit_count=(c))
#define ezbus_mac_bootstrap_get_emit_count(boot)        ((boot)->emit_count)
#define ezbus_mac_bootstrap_inc_emit_count(boot)        ezbus_mac_bootstrap_set_emit_count(boot,ezbus_mac_bootstrap_get_emit_count(boot)+1)

#define ezbus_mac_bootstrap_set_emit_seq(boot,c)        ((boot)->emit_count=(c))
#define ezbus_mac_bootstrap_get_emit_seq(boot)          ((boot)->emit_count)
#define ezbus_mac_bootstrap_inc_emit_seq(boot)          ezbus_mac_bootstrap_set_emit_count(boot,ezbus_mac_bootstrap_get_emit_count(boot)+1)

#define ezbus_mac_bootstrap_set_port(boot,port)         ((boot)->port=(port))
#define ezbus_mac_bootstrap_get_port(boot)              ((boot)->port)

#define ezbus_mac_bootstrap_get_peer_list(boot)         (&(boot)->peer_list)
#define ezbus_mac_bootstrap_get_transmitter(boot)       (&(boot)->transmitter)
#define ezbus_mac_bootstrap_get_receiver(boot)          (&(boot)->receiver)
#define ezbus_mac_bootstrap_get_arbitration(boot)       (&(boot)->arbitration)

#define ezbus_mac_bootstrap_put_packet(boot,packet)     ezbus_mac_bootstrap_transmitter_put(ezbus_mac_bootstrap_get_transmitter((mac),(packet)))
#define ezbus_mac_bootstrap_get_packet(boot,packet)     ezbus_mac_bootstrap_receiver_get(ezbus_mac_bootstrap_get_receiver((mac),(packet)))

#define ezbus_mac_bootstrap_get_warmboot_crc(boot)      (&(boot)->warmboot_crc)

#if 0
    #define ezbus_mac_bootstrap_set_state(boot,s)           ((boot)->state=(s))
    #define ezbus_mac_bootstrap_get_state(boot)             ((boot)->state)
#else
    void                ezbus_mac_bootstrap_set_state( ezbus_mac_bootstrap_t* boot, ezbus_mac_bootstrap_state_t state );
    ezbus_mac_bootstrap_state_t  ezbus_mac_bootstrap_get_state( ezbus_mac_bootstrap_t* boot );
#endif
    

extern void ezbus_mac_bootstrap_init(   
                                ezbus_mac_bootstrap_t* boot, 
                                ezbus_port_t* port
                            );

extern void ezbus_mac_bootstrap_run( ezbus_mac_bootstrap_t* boot );


extern void ezbus_mac_bootstrap_signal_token_seen ( ezbus_mac_bootstrap_t* boot, ezbus_packet_t* packet );
extern void ezbus_mac_bootstrap_signal_peer_seen  ( ezbus_mac_bootstrap_t* boot, ezbus_packet_t* packet );

extern const char* ezbus_mac_bootstrap_get_state_str( ezbus_mac_bootstrap_t* boot );


extern void  ezbus_mac_bootstrap_signal_silent_start         ( ezbus_mac_bootstrap_t* boot );
extern void  ezbus_mac_bootstrap_signal_silent_continue      ( ezbus_mac_bootstrap_t* boot );
extern void  ezbus_mac_bootstrap_signal_silent_stop          ( ezbus_mac_bootstrap_t* boot );

extern void  ezbus_mac_bootstrap_signal_coldboot_start       ( ezbus_mac_bootstrap_t* boot );
extern void  ezbus_mac_bootstrap_signal_coldboot_continue    ( ezbus_mac_bootstrap_t* boot );
extern void  ezbus_mac_bootstrap_signal_coldboot_stop        ( ezbus_mac_bootstrap_t* boot );

extern void  ezbus_mac_bootstrap_signal_warmboot_tx_first    ( ezbus_mac_bootstrap_t* boot );
extern void  ezbus_mac_bootstrap_signal_warmboot_tx_start    ( ezbus_mac_bootstrap_t* boot );
extern void  ezbus_mac_bootstrap_signal_warmboot_tx_restart  ( ezbus_mac_bootstrap_t* boot );
extern void  ezbus_mac_bootstrap_signal_warmboot_tx_continue ( ezbus_mac_bootstrap_t* boot );
extern void  ezbus_mac_bootstrap_signal_warmboot_tx_stop     ( ezbus_mac_bootstrap_t* boot );

extern void  ezbus_mac_bootstrap_signal_warmboot_rx_start    ( ezbus_mac_bootstrap_t* boot );
extern void  ezbus_mac_bootstrap_signal_warmboot_rx_continue ( ezbus_mac_bootstrap_t* boot );
extern void  ezbus_mac_bootstrap_signal_warmboot_rx_stop     ( ezbus_mac_bootstrap_t* boot );


#ifdef __cplusplus
}
#endif

#endif /* EZBUS_MAC_BOOTSTRAP_H_ */
