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
#ifndef EZBUS_MAC_TIMER_H_
#define EZBUS_MAC_TIMER_H_

#include <ezbus_platform.h>
#include <ezbus_mac.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    state_timer_stopping=0,
    state_timer_stopped,
    state_timer_starting,
    state_timer_started,
    state_timer_pausing,
    state_timer_paused,
    state_timer_resume,
    state_timer_expiring,
    state_timer_expired
} ezbus_timer_state_t;

typedef struct _ezbus_timer_t
{
    ezbus_ms_tick_t     start;
    ezbus_ms_tick_t     period;
    ezbus_ms_tick_t     pause_start;        /* start time of pause */
    ezbus_ms_tick_t     pause_duration;     /* pause duration time offset if non-zero */
    ezbus_timer_state_t pause_state;        /* state to restore after pause */
    void                (*callback)(struct _ezbus_timer_t*,void*);
    void*               arg;
    ezbus_timer_state_t state;
    char*               key;
    bool                pausable;
} ezbus_timer_t;

typedef struct _ezbus_mac_timer_t
{
    ezbus_timer_t**  ezbus_timers;
    int              ezbus_timers_count;
    bool             ezbus_timers_pause_active;
} ezbus_mac_timer_t;


typedef void (*ezbus_timer_callback_t) ( struct _ezbus_timer_t*, void* );

extern void                 ezbus_timer_init                ( ezbus_mac_t* mac ); 
extern void                 ezbus_timer_setup               ( ezbus_mac_t* mac, ezbus_timer_t* timer, bool pausable );
extern void                 ezbus_timer_run                 ( ezbus_mac_t* mac );
extern void                 ezbus_timer_set_state           ( ezbus_timer_t* timer, ezbus_timer_state_t state );
extern ezbus_timer_state_t  ezbus_timer_get_state           ( ezbus_timer_t* timer );
extern void                 ezbus_timer_set_period          ( ezbus_timer_t* timer, ezbus_ms_tick_t period );
extern ezbus_ms_tick_t      ezbus_timer_get_period          ( ezbus_timer_t* timer );
extern void                 ezbus_timer_set_callback        ( ezbus_timer_t* timer, ezbus_timer_callback_t callback, void* arg );
extern ezbus_ms_tick_t      ezbus_timer_get_ticks           ( ezbus_timer_t* timer );
extern void                 ezbus_timer_set_key             ( ezbus_timer_t* timer, char* key );
extern char*                ezbus_timer_get_key             ( ezbus_timer_t* timer );
extern void                 ezbus_timer_set_pausable        ( ezbus_timer_t* timer, bool pausable );
extern bool                 ezbus_timer_get_pausable        ( ezbus_timer_t* timer );
extern void                 ezbus_timer_set_pause_start     ( ezbus_timer_t* timer, ezbus_ms_tick_t pause_start );
extern ezbus_ms_tick_t      ezbus_timer_get_pause_start     ( ezbus_timer_t* timer );
extern void                 ezbus_timer_set_pause_duration  ( ezbus_timer_t* timer, ezbus_ms_tick_t pause_duration );
extern ezbus_ms_tick_t      ezbus_timer_get_pause_duration  ( ezbus_timer_t* timer );

extern void                 ezbus_timer_pause               ( ezbus_timer_t* timer );
extern void                 ezbus_timer_resume              ( ezbus_timer_t* timer );
extern void                 ezbus_timer_set_pause_state     ( ezbus_timer_t* timer, ezbus_timer_state_t pause_state );
extern ezbus_timer_state_t  ezbus_timer_get_pause_state     ( ezbus_timer_t* timer );

extern void                 ezbus_timers_set_pause_duration ( ezbus_mac_t* mac, ezbus_ms_tick_t pause_duration );
extern void                 ezbus_timers_set_pause_active   ( ezbus_mac_t* mac, bool active );
extern bool                 ezbus_timers_get_pause_active   ( ezbus_mac_t* mac );

#define ezbus_timer_start(timer)    ezbus_timer_set_state((timer),state_timer_starting)
#define ezbus_timer_restart(timer)  ezbus_timer_set_state((timer),state_timer_starting)
#define ezbus_timer_stop(timer)     ezbus_timer_set_state((timer),state_timer_stopping)
#define ezbus_timer_stopped(timer)  (ezbus_timer_get_state((timer))==state_timer_stopped)
#define ezbus_timer_expired(timer)  (ezbus_timer_get_state((timer))==state_timer_expired)

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_MAC_TIMER_H_ */
