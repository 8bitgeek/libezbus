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
#ifndef EZBUS_TIMER_H_
#define EZBUS_TIMER_H_

#include <ezbus_platform.h>

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
    state_timer_resuming,
    state_timer_resume,
    state_timer_expiring,
    state_timer_expired
} ezbus_timer_state_t;

typedef struct _ezbus_timer_t
{
    ezbus_ms_tick_t     start;
    ezbus_ms_tick_t     period;
    void                (*callback)(struct _ezbus_timer_t*,void*);
    void*               arg;
    ezbus_timer_state_t state;
} ezbus_timer_t;

typedef void (*ezbus_timer_callback_t) ( ezbus_timer_state_t*, void* );

extern void                 ezbus_timer_init         ( ezbus_timer_t* timer );
extern void                 ezbus_timer_run          ( ezbus_timer_t* timer );
extern void                 ezbus_timer_set_state    ( ezbus_timer_t* timer, ezbus_timer_state_t state );
extern ezbus_timer_state_t  ezbus_timer_get_state    ( ezbus_timer_t* timer );
extern void                 ezbus_timer_set_period   ( ezbus_timer_t* timer, ezbus_ms_tick_t period );
extern ezbus_ms_tick_t      ezbus_timer_get_period   ( ezbus_timer_t* timer );
extern void                 ezbus_timer_set_callback ( ezbus_timer_t* timer, ezbus_timer_callback_t callback, void* arg );
extern ezbus_ms_tick_t      ezbus_timer_get_ticks    ( ezbus_timer_t* timer );

#define ezbus_timer_start(timer)    ezbus_timer_set_state((timer),state_timer_starting)
#define ezbus_timer_restart(timer)  ezbus_timer_set_state((timer),state_timer_starting)
#define ezbus_timer_stop(timer)     ezbus_timer_set_state((timer),state_timer_stopping)
#define ezbus_timer_pause(timer)    ezbus_timer_set_state((timer),state_timer_pausing)
#define ezbus_timer_resume(timer)   ezbus_timer_set_state((timer),state_timer_resuming)
#define ezbus_timer_expired(timer)  (ezbus_timer_get_state((timer))==state_timer_expired)

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_TIMER_H_ */
