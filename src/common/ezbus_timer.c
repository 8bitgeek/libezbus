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
#include <ezbus_timer.h>
#include <ezbus_log.h>

static bool ezbus_timer_timeout   ( ezbus_timer_t* timer );
static void ezbus_timer_do_pause  ( ezbus_timer_t* timer );
static void ezbus_timer_do_resume ( ezbus_timer_t* timer );

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
            if ( ezbus_timer_timeout( timer ) )
                ezbus_timer_set_state( timer, state_timer_expiring );
            break;
        case state_timer_pausing:
            ezbus_timer_do_pause( timer );
            ezbus_timer_set_state( timer, state_timer_paused );
            break;
        case state_timer_paused:
            break;
        case state_timer_resume:
            ezbus_timer_do_resume( timer );
            ezbus_timer_set_state( timer, state_timer_started );
            break;
        case state_timer_expiring:
            ezbus_timer_set_state( timer, state_timer_expired );
            break;
        case state_timer_expired:
            EZBUS_LOG( EZBUS_LOG_TIMERS, "state_timer_expired - %s", eabus_timer_get_key( timer ) );
            timer->callback( timer, timer->arg );
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

extern void ezbus_timer_set_key( ezbus_timer_t* timer, char* key )
{
    timer->key = key;
}

extern char* eabus_timer_get_key( ezbus_timer_t* timer )
{
    return timer->key;
}


extern void ezbus_timer_set_period( ezbus_timer_t* timer, ezbus_ms_tick_t period )
{
    timer->period = period;
}

extern ezbus_ms_tick_t ezbus_timer_get_period( ezbus_timer_t* timer )
{
    return timer->period;
}

extern void ezbus_timer_set_callback( ezbus_timer_t* timer, ezbus_timer_callback_t callback, void* arg )
{
    timer->callback = callback;
    timer->arg = arg;
}

extern ezbus_ms_tick_t ezbus_timer_get_ticks( ezbus_timer_t* timer )
{
    return ezbus_platform_get_ms_ticks();
}


static bool ezbus_timer_timeout( ezbus_timer_t* timer ) 
{
    return (ezbus_timer_get_ticks(timer) - (timer)->start) > timer->period;
}

static void ezbus_timer_do_pause( ezbus_timer_t* timer )
{
    timer->pause = ezbus_timer_get_ticks( timer );
}

static void ezbus_timer_do_resume( ezbus_timer_t* timer )
{
    ezbus_ms_tick_t pause_delta = (timer->pause - timer->start);
    timer->start += pause_delta;
}

