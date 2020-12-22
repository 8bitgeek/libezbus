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

static ezbus_timer_t**  ezbus_timers = NULL;
static int              ezbus_timers_count=0;

static bool ezbus_timer_timeout   ( ezbus_timer_t* timer );
static void ezbus_timer_do_pause  ( ezbus_timer_t* timer );
static void ezbus_timer_do_resume ( ezbus_timer_t* timer );

static bool ezbus_timer_append    ( ezbus_timer_t* timer );
static bool ezbus_timer_remove    ( ezbus_timer_t* timer );
static int  ezbus_timer_indexof   ( ezbus_timer_t* timer );

extern void ezbus_timer_init( ezbus_timer_t* timer, bool pausable )
{
    ezbus_platform_memset(timer,0,sizeof(ezbus_timer_t));
    ezbus_timer_append( timer );
}

extern void ezbus_timer_deinit( ezbus_timer_t* timer )
{
    ezbus_remove_timer( timer );
}

static bool ezbus_timer_append( ezbus_timer_t* timer )
{
    ezbus_timers = (ezbus_timer_t**)ezbus_platform_realloc( ezbus_timers, ( sizeof(ezbus_timer_t*) * (++ezbus_timers_count) ) );
    if ( ezbus_timers != NULL )
    {
        ezbus_timers[ezbus_timers_count-1] = timer;
        return true;
    }
    return false;
}

static bool ezbus_timer_remove( ezbus_timer_t* timer )
{
    uint32_t index = ezbus_timer_indexof( timer );
    if ( index >= 0 )
    {
        ezbus_platform_memmove( &ezbus_timers[index], &ezbus_timers[index+1], (--ezbus_timers_count)-index );
        ezbus_timers = (ezbus_timer_t**)ezbus_platform_realloc( ezbus_timers, ( sizeof(ezbus_timer_t*) * ezbus_timers_count ) );
        return true;
    }
    return false;
}

static int ezbus_timer_indexof( ezbus_timer_t* timer )
{
    if ( ezbus_timers != NULL && ezbus_timers_count > 0 )
    {
        for( uint32_t index=0; index < ezbus_timers_count; index++ )
        {
            if ( ezbus_timers[index] == timer )
            {
                return index;
            }
        }
    }
    return -1; 
}

extern void ezbus_timer_run( ezbus_timer_t* timer )
{
    switch( timer->state )
    {
        case state_timer_stopping:
            // EZBUS_LOG( EZBUS_LOG_TIMERS, "state_timer_stopping [%08X,%08X] - %s", timer->callback, timer->arg, eabus_timer_get_key( timer ) );
            ezbus_timer_set_state( timer, state_timer_stopped );
            break;
        case state_timer_stopped:
            break;
        case state_timer_starting:
            // EZBUS_LOG( EZBUS_LOG_TIMERS, "state_timer_starting [%08X,%08X] - %s", timer->callback, timer->arg, ezbus_timer_get_key( timer ) );
            timer->start = ezbus_timer_get_ticks( timer );
            ezbus_timer_set_state( timer, state_timer_started );
            break;
        case state_timer_started:
            if ( ezbus_timer_timeout( timer ) )
            {
                ezbus_timer_set_state( timer, state_timer_expiring );
            }
            break;
        case state_timer_pausing:
            EZBUS_LOG( EZBUS_LOG_TIMERS, "state_timer_pausing  [%08X,%08X] - %s", timer->callback, timer->arg, ezbus_timer_get_key( timer ) );
            ezbus_timer_do_pause( timer );
            ezbus_timer_set_state( timer, state_timer_paused );
            break;
        case state_timer_paused:
            break;
        case state_timer_resume:
            EZBUS_LOG( EZBUS_LOG_TIMERS, "state_timer_resume   [%08X,%08X] - %s", timer->callback, timer->arg, ezbus_timer_get_key( timer ) );
            ezbus_timer_do_resume( timer );
            ezbus_timer_set_state( timer, state_timer_started );
            break;
        case state_timer_expiring:
            EZBUS_LOG( EZBUS_LOG_TIMERS, "state_timer_expiring [%08X,%08X] - %s", timer->callback, timer->arg, ezbus_timer_get_key( timer ) );
            ezbus_timer_set_state( timer, state_timer_expired );
            break;
        case state_timer_expired:
            EZBUS_LOG( EZBUS_LOG_TIMERS, "state_timer_expired  [%08X,%08X] - %s", timer->callback, timer->arg, ezbus_timer_get_key( timer ) );
            timer->callback( timer, timer->arg );
            EZBUS_LOG( EZBUS_LOG_TIMERS, "state_timer_expired  [return] %s", ezbus_timer_get_key( timer ) );
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

extern char* ezbus_timer_get_key( ezbus_timer_t* timer )
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

extern void ezbus_timer_set_pausable( ezbus_timer_t* timer, bool pausable )
{
    timer->pausable = pausable;
}

extern bool ezbus_timer_get_pausable( ezbus_timer_t* timer )
{
    return timer->pausable;
}

extern void ezbus_timer_set_deadline( ezbus_timer_t* timer, ezbus_ms_tick_t deadline )
{
    timer->deadline = deadline;
}

extern ezbus_ms_tick_t ezbus_timer_get_deadline( ezbus_timer_t* timer )
{
    return timer->deadline;
}

extern void ezbus_timers_set_deadline( ezbus_ms_tick_t deadline )
{
    for( int index = 0; index < ezbus_timers_count; index++ )
    {
        ezbus_timer_set_deadline( ezbus_timers[ index ], deadline );
    }
}

extern void ezbus_timers_set_pause_active( bool active )
{
    for( int index = 0; index < ezbus_timers_count; index++ )
    {
        if ( active )
            ezbus_timer_pause( ezbus_timers[ index ] );
        else
            ezbus_timer_resume( ezbus_timers[ index ] );
    }
}

extern ezbus_ms_tick_t ezbus_timer_get_ticks( ezbus_timer_t* timer )
{
    return ezbus_platform_get_ms_ticks();
}

extern void ezbus_timer_pause( ezbus_timer_t* timer )
{
    if ( ezbus_timer_get_pausable( timer ) )
    {
        ezbus_timer_set_state((timer),state_timer_pausing);
    }
}

extern void ezbus_timer_resume( ezbus_timer_t* timer )
{
    if ( ezbus_timer_get_pausable( timer ) )
    {
        ezbus_timer_set_state((timer),state_timer_resume);
    }
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

