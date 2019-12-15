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
#include <ezbus_hello.h>
#include <ezbus_peer_list.h>
#include <ezbus_hex.h>
#include <ezbus_log.h>
#include <ezbus_timing.h>

static void ezbus_hello_timer_callback_token        ( ezbus_timer_t* timer, void* arg );
static void ezbus_hello_timer_callback_emit         ( ezbus_timer_t* timer, void* arg );
static void ezbus_hello_state_machine_run           ( ezbus_hello_t* hello );
static void ezbus_hello_peer_list_log               ( ezbus_hello_t* hello );
static void ezbus_hello_init_peer_list              ( ezbus_hello_t* hello );
static void ezbus_hello_signal_peer_seen_roll_call  ( ezbus_hello_t* hello, ezbus_address_t* address );
static void ezbus_hello_signal_peer_seen_bootstrap  ( ezbus_hello_t* hello, ezbus_address_t* address );

static void do_hello_state_silent_start       ( ezbus_hello_t* hello );
static void do_hello_state_silent_continue    ( ezbus_hello_t* hello );
static void do_hello_state_silent_stop        ( ezbus_hello_t* hello );
static void do_hello_state_bootstrap_start    ( ezbus_hello_t* hello );
static void do_hello_state_bootstrap_continue ( ezbus_hello_t* hello );
static void do_hello_state_bootstrap_stop     ( ezbus_hello_t* hello );
static void do_hello_state_token_start        ( ezbus_hello_t* hello );
static void do_hello_state_token_continue     ( ezbus_hello_t* hello );
static void do_hello_state_token_stop         ( ezbus_hello_t* hello );

extern void ezbus_hello_init(   
                                ezbus_hello_t* hello, 
                                uint32_t baud_rate, 
                                ezbus_peer_list_t* peer_list, 
                                ezbus_hello_callback_t callback, 
                                void* callback_arg 
                            )
{
    hello->baud_rate    = baud_rate;
    hello->peer_list    = peer_list;
    hello->callback     = callback;
    hello->callback_arg = callback_arg;

    ezbus_timer_init( &hello->token_timer );
    ezbus_timer_init( &hello->emit_timer );

    ezbus_timer_set_callback( &hello->token_timer, ezbus_hello_timer_callback_token, hello );
    ezbus_timer_set_callback( &hello->emit_timer,  ezbus_hello_timer_callback_emit, hello );

    ezbus_hello_init_peer_list( hello );
}

extern void ezbus_hello_run( ezbus_hello_t* hello )
{
    ezbus_hello_state_machine_run( hello );
    ezbus_timer_run( &hello->token_timer );
    ezbus_timer_run( &hello->emit_timer );
}



static void do_hello_state_silent_start( ezbus_hello_t* hello )
{
    ezbus_hello_set_peer_seen_count( hello, 0 );
    ezbus_hello_set_emit_count( hello, 0 );
    ezbus_timer_stop( &hello->emit_timer );
    ezbus_timer_set_period  ( 
                                &hello->token_timer, 
                                ezbus_timing_ring_time( hello->baud_rate, ezbus_peer_list_count( hello->peer_list ) ) + 
                                    /* ezbus_platform_random( EZBUS_TOKEN_TIMER_MIN, EZBUS_TOKEN_TIMER_MAX ) */
                                    EZBUS_TOKEN_TIMER_MAX
                            );
    ezbus_timer_start( &hello->token_timer );
    hello->callback(hello,hello->callback_arg);
    ezbus_hello_set_state( hello, hello_state_silent_continue );

}

static void do_hello_state_silent_continue( ezbus_hello_t* hello )
{
    /* continue to stay silent until token timeout */
}

static void do_hello_state_silent_stop( ezbus_hello_t* hello )
{
    ezbus_timer_stop( &hello->token_timer );
    ezbus_hello_set_state( hello, hello_state_bootstrap_start );
}

static void do_hello_state_bootstrap_start( ezbus_hello_t* hello )
{
    //ezbus_hello_init_peer_list( hello );
    ezbus_hello_set_peer_seen_count( hello, 0 );
    ezbus_timer_stop( &hello->emit_timer );
    ezbus_timer_set_period  ( 
                                &hello->emit_timer, 
                                /* ezbus_timing_ring_time( hello->baud_rate, ezbus_peer_list_count( hello->peer_list ) ) + */
                                    ezbus_platform_random( EZBUS_EMIT_TIMER_MIN, EZBUS_EMIT_TIMER_MAX ) 
                            );
    ezbus_timer_start( &hello->emit_timer );
    ezbus_hello_set_state( hello, hello_state_bootstrap_continue );
}

static void do_hello_state_bootstrap_continue( ezbus_hello_t* hello )
{
    if ( ezbus_hello_get_emit_count( hello ) > EZBUS_EMIT_CYCLES )
        ezbus_hello_set_state( hello, hello_state_token_start );

}

static void do_hello_state_bootstrap_stop( ezbus_hello_t* hello )
{
    ezbus_timer_stop( &hello->emit_timer );
    ezbus_timer_stop( &hello->token_timer );
    ezbus_hello_set_state( hello, hello_state_silent_start );
}

static void do_hello_state_token_start( ezbus_hello_t* hello )
{
    ezbus_hello_set_emit_count( hello, 0 );
    ezbus_timer_stop( &hello->emit_timer );
    ezbus_timer_set_period  ( 
                                &hello->token_timer, 
                                ezbus_timing_ring_time( hello->baud_rate, ezbus_peer_list_count( hello->peer_list ) ) + 
                                    /* ezbus_platform_random( EZBUS_TOKEN_TIMER_MIN, EZBUS_TOKEN_TIMER_MAX ) */
                                    EZBUS_TOKEN_TIMER_MAX
                            );
    ezbus_timer_start( &hello->token_timer );
    hello->callback(hello,hello->callback_arg);
    ezbus_hello_set_state( hello, hello_state_token_continue );
}

static void do_hello_state_token_continue( ezbus_hello_t* hello )
{
    /* wait for token to timeout */
}

static void do_hello_state_token_stop( ezbus_hello_t* hello )
{
    ezbus_timer_stop( &hello->token_timer );
    hello->callback(hello,hello->callback_arg);
    ezbus_hello_set_state( hello, hello_state_silent_start );
}

static void ezbus_hello_state_machine_run( ezbus_hello_t* hello )
{
    switch ( ezbus_hello_get_state( hello ) )
    {
        case hello_state_silent_start:          do_hello_state_silent_start       ( hello );  break;
        case hello_state_silent_continue:       do_hello_state_silent_continue    ( hello );  break;
        case hello_state_silent_stop:           do_hello_state_silent_stop        ( hello );  break;

        case hello_state_bootstrap_start:       do_hello_state_bootstrap_start    ( hello );  break;
        case hello_state_bootstrap_continue:    do_hello_state_bootstrap_continue ( hello );  break;
        case hello_state_bootstrap_stop:        do_hello_state_bootstrap_stop     ( hello );  break;

        case hello_state_token_start:           do_hello_state_token_start        ( hello );  break;
        case hello_state_token_continue:        do_hello_state_token_continue     ( hello );  break;
        case hello_state_token_stop:            do_hello_state_token_stop         ( hello );  break;
    }
}

static void ezbus_hello_signal_peer_seen_roll_call( ezbus_hello_t* hello, ezbus_address_t* address )
{
    switch ( ezbus_hello_get_state( hello ) )
    {
    
        case hello_state_silent_start:
        case hello_state_silent_continue:
        case hello_state_silent_stop:
            ezbus_hello_set_state( hello, hello_state_bootstrap_start );
            break;

        case hello_state_bootstrap_start:
        case hello_state_bootstrap_continue:
        case hello_state_bootstrap_stop:
            ezbus_hello_signal_peer_seen_bootstrap( hello, address );
            break;

        case hello_state_token_start:
        case hello_state_token_continue:
        case hello_state_token_stop:
            ezbus_hello_set_state( hello, hello_state_bootstrap_start );
            break;
    
    }
}

static void ezbus_hello_signal_peer_seen_bootstrap( ezbus_hello_t* hello, ezbus_address_t* address )
{
    ezbus_peer_t    peer;
    
    ezbus_peer_init( &peer, address, 0 );
    ezbus_peer_list_insort( hello->peer_list, &peer );
    ezbus_hello_peer_list_log( hello );

    if ( ezbus_address_compare( &ezbus_self_address, address ) > 0 )
    {
        ezbus_log( EZBUS_LOG_HELLO, "hello peer< %s < ", ezbus_address_string( address ) );
        ezbus_log( EZBUS_LOG_HELLO, "%s \n", ezbus_address_string( &ezbus_self_address ) );

        if ( ezbus_hello_get_state( hello ) == hello_state_bootstrap_continue )
        {
            ezbus_hello_inc_peer_seen_count( hello );
            if ( ezbus_hello_get_peer_seen_count( hello ) > EZBUS_PEER_SEEN_COUNT )
            {
                ezbus_timer_stop( &hello->emit_timer );
                ezbus_hello_set_state( hello, hello_state_silent_start );
            }
        } 
    }
}

extern void ezbus_hello_signal_peer_seen( ezbus_hello_t* hello, ezbus_address_t* address )
{
    if ( ezbus_address_compare( address, &ezbus_roll_call_address ) == 0 )
    {
        ezbus_hello_signal_peer_seen_roll_call( hello, address );
    }
    else
    {
        ezbus_hello_signal_peer_seen_bootstrap( hello, address );
    }
}

extern void ezbus_hello_signal_token_seen( ezbus_hello_t* hello, ezbus_address_t* address )
{
    ezbus_peer_t    peer;
    
    ezbus_peer_init( &peer, address, 0 );
    ezbus_peer_list_insort( hello->peer_list, &peer );

    ezbus_log( EZBUS_LOG_HELLO, "hello tok< %s\n", ezbus_address_string( address ) );
    ezbus_hello_peer_list_log( hello );

    ezbus_hello_set_state( hello, hello_state_silent_start );
}

static void ezbus_hello_timer_callback_token( ezbus_timer_t* timer, void* arg )
{
    ezbus_hello_t* hello=(ezbus_hello_t*)arg;
    if ( ezbus_timer_expired( timer ) )
    {
        switch( ezbus_hello_get_state( hello ) )
        {
            case hello_state_silent_start:
                break;
            case hello_state_silent_continue:
                ezbus_hello_set_state( hello, hello_state_silent_stop );
                break;
            case hello_state_silent_stop:
                break;
            case hello_state_bootstrap_start:
                break;
            case hello_state_bootstrap_continue:
                break;
            case hello_state_bootstrap_stop:
                break;
            case hello_state_token_start:
                break;
            case hello_state_token_continue:
                ezbus_hello_set_state( hello, hello_state_token_stop );
                break;
            case hello_state_token_stop:
                break;

        }
    }
}

static void ezbus_hello_timer_callback_emit( ezbus_timer_t* timer, void* arg )
{
    ezbus_hello_t* hello=(ezbus_hello_t*)arg;
    if ( ezbus_timer_expired( timer ) )
    {
        ezbus_hello_inc_emit_count( hello );
        ezbus_log( EZBUS_LOG_HELLO, "hello> %s %d\n", ezbus_address_string( &ezbus_self_address ), ezbus_hello_get_emit_count( hello ) );
        hello->callback(hello,hello->callback_arg);
        ezbus_hello_set_state( hello, hello_state_bootstrap_start );
    }
}

static void ezbus_hello_peer_list_log( ezbus_hello_t* hello )
{
    for(int index=0; index < ezbus_peer_list_count(hello->peer_list); index++)
    {
        ezbus_peer_t* peer = ezbus_peer_list_at(hello->peer_list,index);
        ezbus_log( EZBUS_LOG_HELLO, "%s, ", ezbus_address_string( ezbus_peer_get_address( peer ) ) );
    }
    ezbus_log( EZBUS_LOG_HELLO, "\n" );
}

static void ezbus_hello_init_peer_list( ezbus_hello_t* hello )
{
    ezbus_peer_t    self_peer;

    ezbus_peer_list_clear( hello->peer_list );
    ezbus_peer_init( &self_peer, &ezbus_self_address, 0 );
    ezbus_peer_list_insort( hello->peer_list, &self_peer );    
}
