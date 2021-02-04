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
#ifndef EZBUS_MAC_ARBITER_PAUSE_H_
#define EZBUS_MAC_ARBITER_PAUSE_H_

#include <ezbus_platform.h>
#include <ezbus_mac.h>
#include <ezbus_mac_timer.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    mac_arbiter_state_pause_stopping=0,
    mac_arbiter_state_pause_stopped,

    mac_arbiter_state_pause_start,
    mac_arbiter_state_pause_wait_send,
    mac_arbiter_state_pause_continue,
    mac_arbiter_state_pause_finish,
    
} ezbus_mac_arbiter_pause_state_t;

typedef bool (*ezbus_mac_arbiter_pause_callback_t)( ezbus_mac_t* );

typedef struct _ezbus_mac_arbiter_pause_t
{
    ezbus_mac_arbiter_pause_state_t     state;
    ezbus_mac_arbiter_pause_callback_t  callback;
    bool                                sender;
} ezbus_mac_arbiter_pause_t;

extern void             ezbus_mac_arbiter_pause_init            ( ezbus_mac_t* mac );
extern void             ezbus_mac_arbiter_pause_setup           ( 
                                                                    ezbus_mac_t*               mac, 
                                                                    ezbus_ms_tick_t            duration, 
                                                                    ezbus_ms_tick_t            period, 
                                                                    ezbus_mac_arbiter_pause_callback_t callback 
                                                                );
extern void             ezbus_mac_arbiter_pause_run             ( ezbus_mac_t* mac );
extern bool             ezbus_mac_arbiter_pause                 ( ezbus_mac_t* mac );
extern bool             ezbus_mac_arbiter_pause_ready           ( ezbus_mac_t* mac );

extern void             ezbus_mac_arbiter_pause_set_state       ( ezbus_mac_t* mac, ezbus_mac_arbiter_pause_state_t state );
extern ezbus_mac_arbiter_pause_state_t 
                        ezbus_mac_arbiter_pause_get_state       ( ezbus_mac_t* mac );

extern void             ezbus_mac_arbiter_pause_set_duration    ( ezbus_mac_t* mac, ezbus_ms_tick_t duration );
extern ezbus_ms_tick_t  ezbus_mac_arbiter_pause_get_duration    ( ezbus_mac_t* mac );
extern ezbus_timer_t*   ezbus_mac_arbiter_pause_get_timer       ( ezbus_mac_t* mac );
extern ezbus_timer_t*   ezbus_mac_arbiter_pause_get_half_timer  ( ezbus_mac_t* mac );
extern void             ezbus_mac_arbiter_pause_set_callback    ( ezbus_mac_t* mac, ezbus_mac_arbiter_pause_callback_t callback );


#ifdef __cplusplus
}
#endif

#endif /* EZBUS_MAC_ARBITER_PAUSE_H_ */
