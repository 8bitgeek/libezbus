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
#include <ezbus_platform.h>
#include <ezbus_timer.h>

extern void ezbus_timer_init( ezbus_timer_t* timer )
{
    ezbus_platform_memset(timer,0,sizeof(ezbus_timer_t));
}

extern void ezbus_timer_run( ezbus_timer_t* timer )
{
    switch( timer->state )
    {
        case state_timer_stopping:
            ezbus_timer_set_state( timer, state_timer_stopped );
            break;
        case state_timer_stopped:
            break;
        case state_timer_starting:
            timer->start = ezbus_timer_get_ticks( timer );
            ezbus_timer_set_state( timer, state_timer_started );
            break;
        case state_timer_started:
            if ( ezbus_timer_expired( timer ) )
                ezbus_timer_set_state( timer, state_timer_expiring );
            break;
        case state_timer_pausing:
            ezbus_timer_set_state( timer, state_timer_paused );
            break;
        case state_timer_paused:
            break;
        case state_timer_resuming:
            ezbus_timer_set_state( timer, state_timer_resume );
            break;
        case state_timer_resume:
            ezbus_timer_set_state( timer, state_timer_started );
            break;
        case state_timer_expiring:
            ezbus_timer_set_state( timer, state_timer_expired );
            timer->callback( timer, timer->arg );
            break;
        case state_timer_expired:
            ezbus_timer_set_state( timer, state_timer_stopping );
            break;
    }
}

extern void ezbus_timer_set_state( ezbus_timer_t* timer, ezbus_timer_state_t state )
{
    timer->state = state;
}

extern ezbus_timer_state_t ezbus_timer_get_state( ezbus_timer_t* timer )
{
    return timer->state;
}

extern bool ezbus_timer_expired( ezbus_timer_t* timer )
{
    if ( timer->period )
    {
        return ezbus_timer_get_ticks( timer ) - ( timer->start + timer->period );
    }
    return false;
}

extern void ezbus_timer_set_period( ezbus_timer_t* timer, ezbus_ms_tick_t period )
{
    timer->period = period;
}

extern void ezbus_timer_set_callback( ezbus_timer_t* timer, ezbus_timer_callback_t callback, void* arg )
{
    timer->callback = callback;
    timer->arg = arg;
}

extern ezbus_ms_tick_t  ezbus_timer_get_ticks    ( ezbus_timer_t* timer )
{
    return ezbus_platform_get_ms_ticks();
}