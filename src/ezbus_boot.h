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
#ifndef EZBUS_BOOT_H_
#define EZBUS_BOOT_H_

#include <ezbus_platform.h>
#include <ezbus_timer.h>
#include <ezbus_address.h>
#include <ezbus_peer_list.h>
#include <ezbus_packet.h>
#include <ezbus_crc.h>

typedef enum
{
    boot_state_silent_start=0,
    boot_state_silent_continue,
    boot_state_silent_stop,
    
    boot_state_coldboot_start,
    boot_state_coldboot_continue,
    boot_state_coldboot_stop,

    boot_state_warmboot_tx_start,
    boot_state_warmboot_tx_restart,
    boot_state_warmboot_tx_continue,
    boot_state_warmboot_tx_stop,

    boot_state_warmboot_rx_start,
    boot_state_warmboot_rx_continue,
    boot_state_warmboot_rx_stop,
} ezbus_boot_state_t;

typedef struct _ezbus_boot_t
{
    
    ezbus_timer_t       silent_timer;
    ezbus_timer_t       coldboot_timer;
    ezbus_timer_t       warmboot_reply_timer;
    ezbus_timer_t       warmboot_send_timer;

    uint32_t            emit_count;
    uint32_t            baud_rate;
    ezbus_peer_list_t*  peer_list;
    ezbus_boot_state_t  state;
    void*               callback_arg;
    void                (*callback)( struct _ezbus_boot_t*, void* arg );
    uint8_t             seq;

    uint8_t             warmboot_count;
    ezbus_crc_t         warmboot_peers_crc;

 } ezbus_boot_t;

typedef void (*ezbus_boot_callback_t)( struct _ezbus_boot_t*, void* arg );

#ifdef __cplusplus
extern "C" {
#endif

#define ezbus_boot_set_emit_count(boot,c)      ((boot)->emit_count=(c))
#define ezbus_boot_get_emit_count(boot)        ((boot)->emit_count)
#define ezbus_boot_inc_emit_count(boot)        ezbus_boot_set_emit_count(boot,ezbus_boot_get_emit_count(boot)+1)

#define ezbus_boot_set_emit_seq(boot,c)        ((boot)->emit_count=(c))
#define ezbus_boot_get_emit_seq(boot)          ((boot)->emit_count)
#define ezbus_boot_inc_emit_seq(boot)          ezbus_boot_set_emit_count(boot,ezbus_boot_get_emit_count(boot)+1)

#if 0
    #define ezbus_boot_set_state(boot,s)           ((boot)->state=(s))
    #define ezbus_boot_get_state(boot)             ((boot)->state)
#else
    void                ezbus_boot_set_state( ezbus_boot_t* boot, ezbus_boot_state_t state );
    ezbus_boot_state_t  ezbus_boot_get_state( ezbus_boot_t* boot );
#endif

extern void ezbus_boot_init(   
                                ezbus_boot_t* boot, 
                                uint32_t baud_rate, 
                                ezbus_peer_list_t* peer_list, 
                                ezbus_boot_callback_t callback, 
                                void* callback_arg 
                            );

extern void ezbus_boot_run( ezbus_boot_t* boot );


extern void ezbus_boot_signal_token_seen ( ezbus_boot_t* boot, ezbus_packet_t* packet );
extern void ezbus_boot_signal_peer_seen  ( ezbus_boot_t* boot, ezbus_packet_t* packet );

extern const char* ezbus_boot_get_state_str( ezbus_boot_t* boot );

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_BOOT_H_ */
