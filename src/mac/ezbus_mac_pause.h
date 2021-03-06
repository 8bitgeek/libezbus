/*****************************************************************************
* Copyright © 2019-2021 Mike Sharkey <mike@8bitgeek.net>                     *
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
#ifndef EZBUS_MAC_PAUSE_H_
#define EZBUS_MAC_PAUSE_H_

#include <ezbus_mac.h>
#include <ezbus_mac_arbiter.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum 
{
    ezbus_pause_state_stopping=0,
    ezbus_pause_state_stopped,
    ezbus_pause_state_run,
    ezbus_pause_state_start,
    ezbus_pause_state_wait1,
    ezbus_pause_state_half_duration_timeout,
    ezbus_pause_state_wait2,
    ezbus_pause_state_duration_timeout,
    ezbus_pause_state_finish,

} ezbus_mac_pause_state_t;

typedef bool (*ezbus_mac_pause_callback_t)( ezbus_mac_t* );

typedef struct _ezbus_mac_pause_t
{
    ezbus_mac_pause_state_t         state;
    ezbus_mac_pause_callback_t      callback;
    ezbus_ms_tick_t                 duration;
    ezbus_ms_tick_t                 duration_timer_start;
    ezbus_ms_tick_t                 period;
    ezbus_ms_tick_t                 period_timer_start;
} ezbus_mac_pause_t;

extern void                     ezbus_mac_pause_init        ( ezbus_mac_t* mac );
extern void                     ezbus_mac_pause_setup       ( 
                                                              ezbus_mac_t*               mac, 
                                                              ezbus_ms_tick_t            duration, 
                                                              ezbus_ms_tick_t            period, 
                                                              ezbus_mac_pause_callback_t callback 
                                                            );
extern void                     ezbus_mac_pause_run         ( ezbus_mac_t* mac );
extern void                     ezbus_mac_pause_start       ( ezbus_mac_t* mac );
extern bool                     ezbus_mac_pause_active      ( ezbus_mac_t* mac );
extern bool                     ezbus_mac_pause_one_shot    ( ezbus_mac_t* mac );
extern void                     ezbus_mac_pause_stop        ( ezbus_mac_t* mac );
extern void                     ezbus_mac_pause_set_duration( ezbus_mac_t* mac, ezbus_ms_tick_t duration );
extern ezbus_ms_tick_t          ezbus_mac_pause_get_duration( ezbus_mac_t* mac );
extern void                     ezbus_mac_pause_set_period  ( ezbus_mac_t* mac, ezbus_ms_tick_t period );
extern ezbus_ms_tick_t          ezbus_mac_pause_get_period  ( ezbus_mac_t* mac );
extern ezbus_mac_pause_state_t  ezbus_mac_pause_get_state   ( ezbus_mac_t* mac );

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_MAC_PAUSE_H_ */
