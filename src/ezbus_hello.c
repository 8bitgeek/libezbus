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

static void ezbus_hello_timer_callback_token( ezbus_timer_state_t* timer, void* arg );
static void ezbus_hello_timer_callback_emit( ezbus_timer_state_t* timer, void* arg );
static void ezbus_hello_state_machine_run( ezbus_timer_state_t* timer );

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

static void ezbus_hello_state_machine_run( ezbus_timer_state_t* timer )
{
    switch ( ezbus_hello_get_state( hello ) )
    {
        case hello_state_silent_start:
            ezbus_timer_set_period( &hello->token_timer, ezbus_platform_random( EZBUS_TOKEN_TIMER_MIN, EZBUS_TOKEN_TIMER_MAX ) );
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
            ezbus_timer_set_period( &hello->hello_timer, ezbus_platform_random( EZBUS_HELLO_TIMER_MIN, EZBUS_HELLO_TIMER_MAX ) );
            ezbus_timer_start( &hello->hello_timer );
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

        if ( ezbus_address_compare( &self, address ) > 0 && )
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

static void ezbus_hello_timer_callback_token( ezbus_timer_state_t* timer, void* arg )
{
    ezbus_hello_t* hello=(ezbus_hello_t*)arg;
    if ( ezbus_timer_expired( timer ) )
    {
        ezbus_hello_set_state( hello, hello_state_silent_stop );
    }
}

static void ezbus_hello_timer_callback_emit( ezbus_timer_state_t* timer, void* arg )
{
    if ( ezbus_timer_expired( timer ) )
    {
        ezbus_hello_set_state( hello, hello_state_emit_start );
    }
}















/******************************************************************************
                                HELLO
******************************************************************************/

static bool ezbus_layer0_transceiver_run_hello( ezbus_layer0_transceiver_t* layer0_transceiver )
{
    switch ( ezbus_layer0_transceiver_get_hello_state( layer0_transceiver ) )
    {
        case hello_state_idle:
            break;
        case hello_state_init:
            ezbus_layer0_transceiver_hello_init( layer0_transceiver );
            break;
        case hello_state_emit:
            ezbus_layer0_transceiver_hello_emit( layer0_transceiver );
            break;
        case hello_state_wait:
            ezbus_layer0_transceiver_hello_wait( layer0_transceiver );
            break;
        case hello_state_term:
            ezbus_layer0_transceiver_hello_term( layer0_transceiver );
            break;
    }
    return true;
}

static bool ezbus_layer0_transceiver_hello_init( ezbus_layer0_transceiver_t* layer0_transceiver )
{
    ezbus_timer_t* hello_timer = &ezbus_layer0_transceiver->hello_timer;

    ezbus_log( EZBUS_LOG_HELLO, "hello_state_init\n" );
    
    ezbus_layer0_transceiver_set_hello_state( layer0_transceiver, hello_state_wait );
    ezbus_timer_set_period( hello_timer, ezbus_platform_random( EZBUS_HELLO_TIMER_MIN, EZBUS_HELLO_TIMER_MAX ) );
    ezbus_timer_start( hello_timer );
    
    return true;
}

static bool ezbus_layer0_transceiver_hello_emit( ezbus_layer0_transceiver_t* layer0_transceiver )
{
    ezbus_log( EZBUS_LOG_HELLO, "hello_state_emit\n" );
    if ( ezbus_platform_get_ms_ticks() - layer0_transceiver->hello_time )
    {
        ezbus_packet_t  hello_packet;
        ezbus_packet_init( &hello_packet );
        ezbus_packet_set_type( &hello_packet, packet_type_hello );
        ezbus_packet_set_seq( &hello_packet, layer0_transceiver->hello_seq++ );
        ezbus_platform_address( ezbus_packet_src( &hello_packet ) );
        ezbus_port_send( ezbus_layer0_transmitter_get_port( ezbus_layer0_transceiver_get_transmitter( layer0_transceiver ) ), &hello_packet );

        ezbus_layer0_transceiver_set_hello_time   ( layer0_transceiver, ezbus_platform_get_ms_ticks() );
        ezbus_layer0_transceiver_set_hello_state  ( layer0_transceiver, hello_state_wait );
    }
    return true;
}

static bool ezbus_layer0_transceiver_hello_wait( ezbus_layer0_transceiver_t* layer0_transceiver )
{
    //ezbus_log( EZBUS_LOG_HELLO, "hello_state_wait\n" );
    if ( ezbus_platform_get_ms_ticks() - ezbus_layer0_transceiver_get_hello_time( layer0_transceiver ) >  ezbus_layer0_transceiver_get_hello_period( layer0_transceiver ) )
    {   
        ezbus_layer0_transceiver_set_hello_period ( layer0_transceiver, ezbus_platform_random(1,50) );
        ezbus_layer0_transceiver_set_hello_state( layer0_transceiver,  hello_state_emit );
    }
    return true;
}

static bool ezbus_layer0_transceiver_hello_term( ezbus_layer0_transceiver_t* layer0_transceiver )
{
    ezbus_log( EZBUS_LOG_HELLO, "hello_state_term\n" );
    ezbus_layer0_transceiver_set_hello_state( layer0_transceiver,  hello_state_idle );
    return true;
}

static ezbus_ms_tick_t ezbus_layer0_transceiver_tx_ack_timeout  ( ezbus_layer0_transceiver_t* layer0_transceiver )
{
    return ezbus_layer0_transceiver_get_token_time( layer0_transceiver ) * 2;
}
