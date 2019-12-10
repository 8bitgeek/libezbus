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

static void ezbus_hello_timer_callback_token( ezbus_timer_t* timer, void* arg );
static void ezbus_hello_timer_callback_emit( ezbus_timer_t* timer, void* arg );
static void ezbus_hello_state_machine_run( ezbus_hello_t* hello );

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
}

extern void ezbus_hello_run( ezbus_hello_t* hello )
{
    ezbus_timer_run( &hello->token_timer );
    ezbus_timer_run( &hello->emit_timer );
    ezbus_hello_state_machine_run( hello );
}

static void ezbus_hello_state_machine_run( ezbus_hello_t* hello )
{
    switch ( ezbus_hello_get_state( hello ) )
    {
        case hello_state_silent_start:
            ezbus_timer_set_period  ( 
                                        &hello->token_timer, 
                                        ezbus_timing_ring_time( hello->baud_rate, ezbus_peer_list_count( hello->peer_list ) ) + 
                                            ezbus_platform_random( EZBUS_TOKEN_TIMER_MIN, EZBUS_TOKEN_TIMER_MAX ) 
                                    );
            ezbus_timer_start( &hello->token_timer );
            ezbus_hello_set_state( hello, hello_state_silent_continue );
            break;
        case hello_state_silent_continue:
            /* continue to stay silent until token timeout */
            break;
        case hello_state_silent_stop:
            ezbus_timer_stop( &hello->token_timer );
            ezbus_hello_set_state( hello, hello_state_emit_start );
            break;
        case hello_state_emit_start:
            ezbus_timer_set_period  ( 
                                        &hello->emit_timer, 
                                        ezbus_timing_ring_time( hello->baud_rate, ezbus_peer_list_count( hello->peer_list ) ) +
                                            ezbus_platform_random( EZBUS_EMIT_TIMER_MIN, EZBUS_EMIT_TIMER_MAX ) 
                                    );
            ezbus_timer_start( &hello->emit_timer );
            ezbus_hello_set_state( hello, hello_state_emit_continue );
            break;
        case hello_state_emit_continue:
            /* continue to emit until higher priority peer says hello */
            break;
        case hello_state_emit_stop:
            ezbus_hello_set_state( hello, hello_state_silent_start );
            break;
    }
}

extern void ezbus_hello_signal_peer_seen( ezbus_hello_t* hello, ezbus_address_t* address )
{
    ezbus_peer_t    other;
    
    ezbus_peer_init( &other, address, 0 );
    ezbus_peer_list_insort( hello->peer_list, &other );
    
    if ( ezbus_hello_get_state( hello ) == hello_state_emit_continue )
    {
        ezbus_address_t self;
        
        ezbus_platform_address( &self );

        if ( ezbus_address_compare( &self, address ) > 0 )
        {
            // i loose - go dark...
            ezbus_hello_set_state( hello, hello_state_emit_stop );   
        }
    }
}

extern void ezbus_hello_signal_token_seen( ezbus_hello_t* hello )
{
    // token is active - go dark...
    ezbus_hello_set_state( hello, hello_state_silent_start );
}

static void ezbus_hello_timer_callback_token( ezbus_timer_t* timer, void* arg )
{
    ezbus_hello_t* hello=(ezbus_hello_t*)arg;
    if ( ezbus_timer_expired( timer ) )
    {
        ezbus_hello_set_state( hello, hello_state_silent_stop );
    }
}

static void ezbus_hello_timer_callback_emit( ezbus_timer_t* timer, void* arg )
{
    ezbus_hello_t* hello=(ezbus_hello_t*)arg;
    if ( ezbus_timer_expired( timer ) )
    {
        ezbus_hello_set_state( hello, hello_state_emit_start );
    }
}




