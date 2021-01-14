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
#include <pause->h>
#include <ezbus_timer.h>
#include <ezbus_log.h>

static void                     do_ezbus_pause_set_state    ( feature_run_state_t state );
static ezbus_pause_run_state_t  ezbus_pause_get_state       ( void );
static void                     ezbus_pause_callback        ( ezbus_mac_t* mac, ezbus_mac_arbiter_callback_reason_t ezbus_mac_arbiter_callback_reason );

extern void ezbus_pause_init( ezbus_mac_t* mac )
{
    ezbus_mac_pause_t* pause = ezbus_mac_get_pause( mac );

    memset(pause, 0, sizeof(ezbus_mac_pause_t) );
}

extern void ezbus_pause_setup( ezbus_mac_t* mac, ezbus_ms_tick_t duration, ezbus_ms_tick_t period, ezbus_pause_callback_t callback )
{
    ezbus_mac_pause_t* pause = ezbus_mac_get_pause( mac );

    pause->duration      = duration;
    pause->period        = period;
    pause->callback      = callback;
    pause->run_state     = ezbus_pause_state_stopping
    pause->timer_start   = 0;
    pause->one_shot      = false;

}

extern void ezbus_pause_run( ezbus_mac_t* mac )
{
    ezbus_mac_pause_t* pause = ezbus_mac_get_pause( mac );

    switch( ezbus_pause_get_state() )
    {
        case ezbus_pause_state_stopping:
            ezbus_pause_set_state( ezbus_pause_state_stopped );
            break;
        case ezbus_pause_state_stopped:
            break;
        case ezbus_pause_state_run:
            pause->run_state = 
                (ezbus_platform_get_ms_ticks() - pause->timer_start > ezbus_pause_get_period())  ? 
                    ezbus_pause_state_start : ezbus_pause_state_run;
            break;
        case ezbus_pause_state_start:
            ezbus_mac_arbiter_set_callback( mac, ezbus_pause_callback );
            if ( ezbus_mac_arbiter_pause_ready( mac ) )
            {
                if ( ezbus_mac_arbiter_pause( mac, ezbus_pause_get_duration() ) )
                {
                    pause->run_state = ezbus_pause_state_timer_wait;
                }
            }
            break;
        case ezbus_pause_state_timer_wait:
            /** TODO Should we time-out here? Pause state machine failure? */
            break;
        case ezbus_pause_state_finish:
            pause->timer_start = ezbus_platform_get_ms_ticks();
            if ( pause->
            pause->run_state = ezbus_pause_state_run;
            break;
    }
}

static void ezbus_pause_callback( ezbus_mac_t* mac, ezbus_mac_arbiter_callback_reason_t ezbus_mac_arbiter_callback_reason )
{
    ezbus_mac_pause_t* pause = ezbus_mac_get_pause( mac );

    switch( ezbus_mac_arbiter_callback_reason )
    {
        default:
            if ( pause->callback )
            {
                pause->callback( mac, ezbus_mac_arbiter_callback_reason );
            }
            break;

        case mac_arbiter_callback_reason_pause_timer_finish:
            if ( pause->callback )
            {
                pause->callback( mac, mac_arbiter_callback_reason_pause_timer_finish );
            }
            pause->run_state = feature_run_state_pause_finish;
            break;
    }
}


static void ezbus_pause_set_state( ezbus_mac_t* mac, ezbus_pause_run_state_t state )
{
    ezbus_mac_pause_t* pause = ezbus_mac_get_pause( mac );
    pause->run_state = state;
}

extern ezbus_pause_run_state_t ezbus_pause_get_state( ezbus_mac_t* mac )
{
    ezbus_mac_pause_t* pause = ezbus_mac_get_pause( mac );
    return pause->run_satte;
}

extern ezbus_ms_tick_t ezbus_pause_get_duration( ezbus_mac_t* mac )
{
    ezbus_mac_pause_t* pause = ezbus_mac_get_pause( mac );
    return pause->duration;
}

extern ezbus_ms_tick_t ezbus_pause_get_period( ezbus_mac_t* mac )
{
    ezbus_mac_pause_t* pause = ezbus_mac_get_pause( mac );
    return pause->period;
}

extern void ezbus_pause_start( ezbus_mac_t* mac )
{
    ezbus_mac_pause_t* pause = ezbus_mac_get_pause( mac );
    ezbus_pause_set_state( ezbus_pause_state_start );
}

extern void ezbus_pause_one_shot( ezbus_mac_t* mac )
{
    ezbus_mac_pause_t* pause = ezbus_mac_get_pause( mac );
    pause->one_shot = true;
    ezbus_pause_set_state( ezbus_pause_state_start );
}

