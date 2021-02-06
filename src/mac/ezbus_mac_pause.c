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
#include <ezbus_mac_pause.h>
#include <ezbus_mac_timer.h>
#include <ezbus_log.h>

static void                     ezbus_mac_pause_set_state  ( ezbus_mac_t* mac, ezbus_mac_pause_state_t state );
static bool                     ezbus_mac_pause_callback   ( ezbus_mac_t* mac );

static void do_ezbus_pause_state_stopping               ( ezbus_mac_t* mac );
static void do_ezbus_pause_state_stopped                ( ezbus_mac_t* mac );
static void do_ezbus_pause_state_run                    ( ezbus_mac_t* mac );
static void do_ezbus_pause_state_start                  ( ezbus_mac_t* mac );
static void do_ezbus_pause_state_wait1                  ( ezbus_mac_t* mac );
static void do_ezbus_pause_state_half_duration_timeout  ( ezbus_mac_t* mac );
static void do_ezbus_pause_state_wait2                  ( ezbus_mac_t* mac );
static void do_ezbus_pause_state_duration_timeout       ( ezbus_mac_t* mac );
static void do_ezbus_pause_state_finish                 ( ezbus_mac_t* mac );

#define ezbus_mac_pause_period_timeout(pause)           ((ezbus_platform_get_ms_ticks()-(pause)->period_timer_start)>(pause)->period)
#define ezbus_mac_pause_duration_timeout(pause)         ((ezbus_platform_get_ms_ticks()-(pause)->duration_timer_start)>((pause)->duration))
#define ezbus_mac_pause_duration_half_timeout(pause)    ((ezbus_platform_get_ms_ticks()-(pause)->duration_timer_start)>((pause)->duration/2))

extern void ezbus_mac_pause_init( ezbus_mac_t* mac )
{
    ezbus_mac_pause_t* pause = ezbus_mac_get_pause( mac );
    memset(pause, 0, sizeof(ezbus_mac_pause_t) );
}

extern void ezbus_mac_pause_setup( ezbus_mac_t* mac, ezbus_ms_tick_t duration, ezbus_ms_tick_t period, ezbus_mac_pause_callback_t callback )
{
    ezbus_mac_pause_t* pause = ezbus_mac_get_pause( mac );

    pause->duration             = duration;
    pause->period               = period;
    pause->callback             = callback;
    pause->run_state            = ezbus_pause_state_stopping;
    pause->period_timer_start   = 0;
    pause->duration_timer_start = 0;
    pause->one_shot             = false;
}

extern void ezbus_mac_pause_run( ezbus_mac_t* mac )
{
    switch( ezbus_mac_pause_get_state( mac ) )
    {
        case ezbus_pause_state_stopping:         do_ezbus_pause_state_stopping         ( mac );   break;
        case ezbus_pause_state_stopped:          do_ezbus_pause_state_stopped          ( mac );   break;
        case ezbus_pause_state_run:              do_ezbus_pause_state_run              ( mac );   break;
        case ezbus_pause_state_start:            do_ezbus_pause_state_start            ( mac );   break;
        case ezbus_pause_state_wait1:            do_ezbus_pause_state_wait1            ( mac );   break;
        case ezbus_pause_state_half_duration_timeout:            
            do_ezbus_pause_state_half_duration_timeout( mac );   
            break;
        case ezbus_pause_state_wait2:            do_ezbus_pause_state_wait2            ( mac );   break;
        case ezbus_pause_state_duration_timeout: do_ezbus_pause_state_duration_timeout ( mac );   break;
        case ezbus_pause_state_finish:           do_ezbus_pause_state_finish           ( mac );   break;    
    }
} 

extern bool ezbus_mac_pause_active( ezbus_mac_t* mac )
{
    ezbus_mac_pause_t* pause = ezbus_mac_get_pause( mac );
    return !(pause->run_state == ezbus_pause_state_stopped || pause->run_state == ezbus_pause_state_run);
}

extern void ezbus_mac_pause_stop( ezbus_mac_t* mac )
{
    ezbus_mac_pause_set_state( mac, ezbus_pause_state_stopping );
}

/***************** state machine functions *********************/

static void do_ezbus_pause_state_stopping( ezbus_mac_t* mac )
{
    ezbus_mac_pause_callback( mac );
    ezbus_mac_pause_set_state( mac, ezbus_pause_state_stopped );
}

static void do_ezbus_pause_state_stopped( ezbus_mac_t* mac )
{
    ezbus_mac_pause_t* pause = ezbus_mac_get_pause( mac );
    pause->duration_timer_start = ezbus_platform_get_ms_ticks();
    pause->period_timer_start = ezbus_platform_get_ms_ticks();
}

/**********************************************************/

static void do_ezbus_pause_state_run( ezbus_mac_t* mac )
{
    ezbus_mac_pause_t* pause = ezbus_mac_get_pause( mac );
    if ( ezbus_mac_pause_period_timeout(pause) )
    {
        ezbus_mac_pause_set_state( mac, ezbus_pause_state_start );
    }
}

static void do_ezbus_pause_state_start( ezbus_mac_t* mac )
{
    if ( ezbus_mac_pause_callback( mac ) )
    {
        ezbus_mac_pause_t* pause = ezbus_mac_get_pause( mac );
        pause->duration_timer_start = ezbus_platform_get_ms_ticks();
        ezbus_timers_set_pause_duration( mac, ezbus_mac_arbiter_pause_get_duration( mac ) );
        ezbus_timers_set_pause_active( mac, true );
        ezbus_mac_pause_set_state( mac, ezbus_pause_state_wait1 );
    }
}

static void do_ezbus_pause_state_wait1( ezbus_mac_t* mac )
{
    ezbus_mac_pause_t* pause = ezbus_mac_get_pause( mac );
    if ( ezbus_mac_pause_duration_half_timeout(pause) )
    {
        ezbus_mac_pause_set_state( mac, ezbus_pause_state_half_duration_timeout );
    }
}

static void do_ezbus_pause_state_half_duration_timeout( ezbus_mac_t* mac )
{
    ezbus_mac_pause_callback( mac );
    ezbus_mac_pause_set_state( mac, ezbus_pause_state_wait2 );
}

static void do_ezbus_pause_state_wait2( ezbus_mac_t* mac )
{
    ezbus_mac_pause_t* pause = ezbus_mac_get_pause( mac );
    if ( ezbus_mac_pause_duration_timeout(pause) )
    {
        ezbus_mac_pause_set_state( mac, ezbus_pause_state_duration_timeout );
    }
}

static void do_ezbus_pause_state_duration_timeout( ezbus_mac_t* mac )
{
    ezbus_mac_pause_callback( mac );
    ezbus_mac_pause_set_state( mac, ezbus_pause_state_finish );
}

static void do_ezbus_pause_state_finish( ezbus_mac_t* mac )
{
    ezbus_mac_pause_t* pause = ezbus_mac_get_pause( mac );
    ezbus_mac_pause_callback( mac );
    if ( pause->one_shot )
    {
        ezbus_mac_pause_set_state( mac, ezbus_pause_state_stopping );
    }
    else
    {
        pause->period_timer_start = ezbus_platform_get_ms_ticks();
        ezbus_mac_pause_set_state( mac, ezbus_pause_state_run );
    }
}


/******************************************************************/


static void ezbus_mac_pause_set_state( ezbus_mac_t* mac, ezbus_mac_pause_state_t state )
{
    ezbus_mac_pause_t* pause = ezbus_mac_get_pause( mac );
    pause->run_state = state;
    fprintf( stderr, " %d ", pause->run_state );
}

extern ezbus_mac_pause_state_t ezbus_mac_pause_get_state( ezbus_mac_t* mac )
{
    ezbus_mac_pause_t* pause = ezbus_mac_get_pause( mac );
    return pause->run_state;
}

extern void ezbus_mac_pause_set_duration( ezbus_mac_t* mac, ezbus_ms_tick_t duration )
{
    ezbus_mac_pause_t* pause = ezbus_mac_get_pause( mac );
    pause->duration = duration;
}

extern ezbus_ms_tick_t ezbus_mac_pause_get_duration( ezbus_mac_t* mac )
{
    ezbus_mac_pause_t* pause = ezbus_mac_get_pause( mac );
    return pause->duration;
}

extern void ezbus_mac_pause_set_period( ezbus_mac_t* mac, ezbus_ms_tick_t period )
{
    ezbus_mac_pause_t* pause = ezbus_mac_get_pause( mac );
    pause->period = period;
}

extern ezbus_ms_tick_t ezbus_mac_pause_get_period( ezbus_mac_t* mac )
{
    ezbus_mac_pause_t* pause = ezbus_mac_get_pause( mac );
    return pause->period;
}

extern void ezbus_mac_pause_start( ezbus_mac_t* mac )
{
    ezbus_mac_pause_set_state( mac, ezbus_pause_state_start );
}

extern void ezbus_mac_pause_one_shot( ezbus_mac_t* mac )
{
    ezbus_mac_pause_t* pause = ezbus_mac_get_pause( mac );
    pause->one_shot = true;
    ezbus_mac_pause_set_state( mac, ezbus_pause_state_start );
}

/***************************************************************/

static bool ezbus_mac_pause_callback( ezbus_mac_t* mac )
{
    ezbus_mac_pause_t* pause = ezbus_mac_get_pause( mac );

    if ( pause->callback )
    {
        if ( !pause->callback( mac ) )
        {
            ezbus_mac_pause_stop(mac);
            return false;
        }
        return true;
    }
    return false;
}
