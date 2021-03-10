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
    /* boot 0 (numerical order matters) */
    mac_arbiter_state_boot0_boundary_top,
    mac_arbiter_state_boot0_restart,
    mac_arbiter_state_boot0_start,
    mac_arbiter_state_boot0_active,
    mac_arbiter_state_boot0_boundary_bottom,

    /* boot 1 (numerical order matters) */
    mac_arbiter_state_boot1_boundary_top,
    mac_arbiter_state_boot1_cycle_dormant,
    mac_arbiter_state_boot1_cycle_start,
    mac_arbiter_state_boot1_cycle_active,
    mac_arbiter_state_boot1_dominant,
    mac_arbiter_state_boot1_boundary_bottom,

    /* boot 2 (numerical order matters) */
    mac_arbiter_state_boot2_boundary_top,
    mac_arbiter_state_boot2_restart,
    mac_arbiter_state_boot2_cycle_start,
    mac_arbiter_state_boot2_cycle_active,
    mac_arbiter_state_boot2_cycle_stop,
    mac_arbiter_state_boot2_finished,
    mac_arbiter_state_boot2_boundary_bottom,

    /* online */
    mac_arbiter_state_offline,
    mac_arbiter_state_service_start,
    mac_arbiter_state_online,
    mac_arbiter_state_pause,
} ezbus_mac_arbiter_state_t;

#define ezbus_mac_arbiter_in_boot0_state(mac)                                                   \
    ((int)ezbus_mac_arbiter_get_state((mac)) > (int)mac_arbiter_state_boot0_boundary_top &&     \
     (int)ezbus_mac_arbiter_get_state((mac)) < (int)mac_arbiter_state_boot0_boundary_bottom)

#define ezbus_mac_arbiter_in_boot1_state(mac)                                                   \
    ((int)ezbus_mac_arbiter_get_state((mac)) > (int)mac_arbiter_state_boot0_boundary_top &&     \
     (int)ezbus_mac_arbiter_get_state((mac)) < (int)mac_arbiter_state_boot0_boundary_bottom)

#define ezbus_mac_arbiter_in_boot2_state(mac)                                                   \
    ((int)ezbus_mac_arbiter_get_state((mac)) > (int)mac_arbiter_state_boot0_boundary_top &&     \
     (int)ezbus_mac_arbiter_get_state((mac)) <  (int)mac_arbiter_state_boot0_boundary_bottom)

typedef bool (*ezbus_mac_arbiter_token_period_callback_t)   ( ezbus_mac_t* );
typedef bool (*ezbus_mac_arbiter_pause_callback_t)          ( ezbus_mac_t* );
typedef bool (*ezbus_mac_arbiter_recieve_callback_t)        ( ezbus_mac_t* mac, ezbus_packet_t* packet );

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
    ezbus_timer_t               timeout_timer;
    ezbus_timer_t               reply_timer;
    uint8_t                     cycles;
} ezbus_mac_boot2_state_t;

typedef struct _ezbus_mac_arbiter_t
{
    ezbus_mac_boot0_state_t     boot0_state;
    ezbus_mac_boot1_state_t     boot1_state;
    ezbus_mac_boot2_state_t     boot2_state;
    ezbus_mac_arbiter_state_t   state;
    ezbus_mac_arbiter_state_t   pre_pause_state;

    ezbus_timer_t               pause_timer;
    ezbus_timer_t               pause_half_timer;
    ezbus_ms_tick_t             pause_duration;

    uint16_t                    token_period;
    uint16_t                    token_age;          
    uint16_t                    token_hold;

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

    ezbus_mac_arbiter_token_period_callback_t   token_period_callback;
    ezbus_mac_arbiter_pause_callback_t          pause_callback;
    ezbus_mac_arbiter_recieve_callback_t        receiver_filter;

} ezbus_mac_arbiter_t;

extern void                         ezbus_mac_arbiter_init                      ( ezbus_mac_t* mac );
extern void                         ezbus_mac_arbiter_run                       ( ezbus_mac_t* mac );
extern bool                         ezbus_mac_arbiter_online                    ( ezbus_mac_t* mac );
extern void                         ezbus_mac_arbiter_bootstrap                 ( ezbus_mac_t* mac );
extern void                         ezbus_mac_arbiter_warm_bootstrap            ( ezbus_mac_t* mac );
extern uint16_t                     ezbus_mac_arbiter_get_token_age             ( ezbus_mac_t* mac );
extern void                         ezbus_mac_arbiter_set_token_period_callback ( ezbus_mac_t* mac, ezbus_mac_arbiter_token_period_callback_t callback );
extern void                         ezbus_mac_arbiter_set_token_period          ( ezbus_mac_t* mac, uint16_t token_age_trigger );
extern uint16_t                     ezbus_mac_arbiter_get_token_period          ( ezbus_mac_t* mac );
extern void                         ezbus_mac_arbiter_set_state                 ( ezbus_mac_t* mac, ezbus_mac_arbiter_state_t state );
extern ezbus_mac_arbiter_state_t    ezbus_mac_arbiter_get_state                 ( ezbus_mac_t* mac );
extern const char*                  ezbus_mac_arbiter_get_state_str             ( ezbus_mac_t* mac );
extern bool                         ezbus_mac_arbiter_callback                  ( ezbus_mac_t* mac );


#ifdef __cplusplus
}
#endif

#endif /* EZBUS_MAC_ARBITER_H_ */
