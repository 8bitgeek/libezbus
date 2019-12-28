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
#ifndef EZBUS_MAC_WARMBOOT_H_
#define EZBUS_MAC_WARMBOOT_H_

#include <ezbus_platform.h>
#include <ezbus_timer.h>
#include <ezbus_packet.h>
#include <ezbus_port.h>
#include <ezbus_mac.h>

typedef enum
{
    boot_state_warmboot_silent_start=0,
    boot_state_warmboot_silent_continue,
    boot_state_warmboot_silent_stop,

    boot_state_warmboot_tx_first,
    boot_state_warmboot_tx_start,
    boot_state_warmboot_tx_restart,
    boot_state_warmboot_tx_continue,
    boot_state_warmboot_tx_stop,

    boot_state_warmboot_rx_start,
    boot_state_warmboot_rx_continue,
    boot_state_warmboot_rx_stop,
} ezbus_mac_warmboot_state_t;

typedef struct _ezbus_mac_warmboot_t
{
    ezbus_mac_warmboot_state_t state;

    ezbus_timer_t               warmboot_reply_timer;
    ezbus_timer_t               warmboot_send_timer;

    uint8_t                     seq;
    uint8_t                     warmboot_count;
    ezbus_crc_t                 warmboot_crc;

} ezbus_mac_warmboot_t;


#ifdef __cplusplus
extern "C" {
#endif


extern void ezbus_mac_warmboot_init                     ( ezbus_mac_t* mac );
extern void ezbus_mac_warmboot_run                      ( ezbus_mac_t* mac );


extern void ezbus_mac_warmboot_signal_token_seen        ( ezbus_mac_t* mac, ezbus_packet_t* packet );
extern void ezbus_mac_warmboot_signal_peer_seen         ( ezbus_mac_t* mac, ezbus_packet_t* packet );

extern void  ezbus_mac_warmboot_signal_tx_first         ( ezbus_mac_t* mac );
extern void  ezbus_mac_warmboot_signal_tx_start         ( ezbus_mac_t* mac );
extern void  ezbus_mac_warmboot_signal_tx_restart       ( ezbus_mac_t* mac );
extern void  ezbus_mac_warmboot_signal_tx_continue      ( ezbus_mac_t* mac );
extern void  ezbus_mac_warmboot_signal_tx_stop          ( ezbus_mac_t* mac );

extern void  ezbus_mac_warmboot_signal_rx_start         ( ezbus_mac_t* mac );
extern void  ezbus_mac_warmboot_signal_rx_continue      ( ezbus_mac_t* mac );
extern void  ezbus_mac_warmboot_signal_rx_stop          ( ezbus_mac_t* mac );

void                        ezbus_mac_warmboot_set_state( ezbus_mac_t* mac, ezbus_mac_warmboot_state_t state );
ezbus_mac_warmboot_state_t ezbus_mac_warmboot_get_state ( ezbus_mac_t* mac );
    
extern const char* ezbus_mac_warmboot_get_state_str     ( ezbus_mac_t* mac );


#ifdef __cplusplus
}
#endif

#endif /* EZBUS_MAC_WARMBOOT_H_ */
