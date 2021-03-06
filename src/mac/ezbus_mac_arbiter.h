/*****************************************************************************
* Copyright © 2019-2020 Mike Sharkey <mike@8bitgeek.net>                     *
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
#ifndef EZBUS_MAC_ARBITER_H_
#define EZBUS_MAC_ARBITER_H_

#include <ezbus_types.h>
#include <ezbus_mac.h>
#include <ezbus_mac_timer.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    /* boot 0 */
    mac_arbiter_state_boot0_restart=0,
    mac_arbiter_state_boot0_start,
    mac_arbiter_state_boot0_active,
    mac_arbiter_state_boot0_stop,
    mac_arbiter_state_boot0_stopped,

    /* boot 1 */
    mac_arbiter_state_boot1_stop,
    mac_arbiter_state_boot1_stopped,
    mac_arbiter_state_boot1_start,
    mac_arbiter_state_boot1_active,
    mac_arbiter_state_boot1_dominant,

    /* boot 2 */
    mac_arbiter_state_boot2_idle,
    mac_arbiter_state_boot2_restart,
    mac_arbiter_state_boot2_start,
    mac_arbiter_state_boot2_active,
    mac_arbiter_state_boot2_stop,
    mac_arbiter_state_boot2_finished,

    /* online */
    mac_arbiter_state_offline,
    mac_arbiter_state_service_start,
    mac_arbiter_state_online,
    mac_arbiter_state_pause,
} ezbus_mac_arbiter_state_t;

typedef bool (*ezbus_mac_arbiter_pause_callback_t)( ezbus_mac_t* );

typedef struct _ezbus_mac_boot0_t
{
    ezbus_timer_t               timer;
    uint32_t                    emit_count;
} ezbus_mac_boot0_state_t;

typedef struct _ezbus_mac_boot1_t
{
    ezbus_timer_t               timer;
    uint32_t                    emit_count;
    uint8_t                     seq;
} ezbus_mac_boot1_state_t;

typedef struct _ezbus_mac_boot2_t
{
    uint8_t                     seq;   /* always != 0 */
    ezbus_timer_t               timer;
    uint8_t                     cycles;
} ezbus_mac_boot2_state_t;

typedef struct _ezbus_mac_arbiter_t
{
    ezbus_mac_boot0_state_t     boot0_state;
    ezbus_mac_boot1_state_t     boot1_state;
    ezbus_mac_boot2_state_t     boot2_state;
    ezbus_mac_arbiter_state_t   state;
    ezbus_mac_arbiter_state_t   pre_pause_state;
    uint8_t                     boot2_cycles;
    uint16_t                    token_age;
    uint16_t                    token_hold;
    ezbus_timer_t               pause_timer;
    ezbus_timer_t               pause_half_timer;
    ezbus_ms_tick_t             pause_duration;

    bool                        rx_ack_pend;
    uint8_t                     rx_ack_seq;
    ezbus_address_t             rx_ack_address;
    ezbus_socket_t              rx_ack_dst_socket;
    ezbus_socket_t              rx_ack_src_socket;

    bool                        rx_nack_pend;
    uint8_t                     rx_nack_seq;
    ezbus_address_t             rx_nack_address;
    ezbus_socket_t              rx_nack_dst_socket;
    ezbus_socket_t              rx_nack_src_socket;

    ezbus_mac_arbiter_pause_callback_t pause_callback;

} ezbus_mac_arbiter_t;

extern void                         ezbus_mac_arbiter_init                  ( ezbus_mac_t* mac );
extern void                         ezbus_mac_arbiter_run                   ( ezbus_mac_t* mac );
extern bool                         ezbus_mac_arbiter_online                ( ezbus_mac_t* mac );
extern void                         ezbus_mac_arbiter_bootstrap             ( ezbus_mac_t* mac);
extern void                         ezbus_mac_arbiter_warm_bootstrap        ( ezbus_mac_t* mac);

extern uint16_t                     ezbus_mac_arbiter_get_token_age         ( ezbus_mac_t* mac );
extern void                         ezbus_mac_arbiter_set_token_age         ( ezbus_mac_t* mac, uint16_t age );

extern void                         ezbus_mac_arbiter_set_state             ( ezbus_mac_t* mac, ezbus_mac_arbiter_state_t state );
extern ezbus_mac_arbiter_state_t    ezbus_mac_arbiter_get_state             ( ezbus_mac_t* mac );

extern uint8_t                      ezbus_mac_arbiter_get_boot2_cycles      ( ezbus_mac_t* mac );
extern void                         ezbus_mac_arbiter_set_boot2_cycles      ( ezbus_mac_t* mac, uint8_t cycles );
extern void                         ezbus_mac_arbiter_dec_boot2_cycles      ( ezbus_mac_t* mac );
extern void                         ezbus_mac_arbiter_rst_boot2_cycles      ( ezbus_mac_t* mac );

extern bool                         ezbus_mac_arbiter_callback              ( ezbus_mac_t* mac );

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_MAC_ARBITER_H_ */
