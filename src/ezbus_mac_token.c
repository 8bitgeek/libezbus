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
#include <ezbus_mac_token.h>
#include <ezbus_log.h>

#define NUM_PEERS_HACK  0       // use for debugging / testing.

#define ezbus_token_get_ring_timer(token)  (&(token)->ring_timer)

static void ezbus_token_ring_timer_callback( ezbus_timer_t* timer, void* arg );

extern void ezbus_token_init( ezbus_token_t* token, uint32_t baud_rate, uint32_t num_peers )
{
    memset( token, 0, sizeof(ezbus_token_t) );
    token->baud_rate = baud_rate;
    token->num_peer = num_peers;
    ezbus_timer_init( ezbus_token_get_ring_timer(token) );
    ezbus_timer_set_period( ezbus_token_get_ring_timer(token), ezbus_token_ring_time(token) );
    ezbus_timer_set_callback( ezbus_token_get_ring_timer(token), ezbus_token_ring_timer_callback );
}

extern void ezbus_token_run( ezbus_token_t* token )
{
    ezbus_timer_run( ezbus_token_get_ring_timer(token) );
}

extern uint32_t ezbus_token_ring_time( ezbus_token_t* token )
{
    #if NUM_PEERS_HACK
        token->num_peers = EZBUS_MAX_PEERS; 
    #endif
    uint32_t packet_sz = sizeof(ezbus_header_t);
    uint32_t packets_per_round = (token->num_peers * 2);
    float    bit_time_sec      = 1.0f/(float)token->baud_rate;
    float    packet_bits       = ((float)packet_sz * 12.0f);
    float    packet_time_sec   = packet_bits * bit_time_sec;
    float    secs_per_round    = packet_time_sec * (float)packets_per_round;
    return (secs_per_round * 1000.0f) + 1.0f;
}

extern void ezbus_token_seen( ezbus_token_t* token )
{
    ezbus_timer_restart( ezbus_token_get_ring_timer(token) );
}

extern void ezbus_token_acquire( ezbus_token_t* token )
{
    ezbus_timer_restart( ezbus_token_get_ring_timer(token) );
    token->acquired=true;
}

extern void ezbus_token_relinquish( ezbus_token_t* token )
{
    ezbus_timer_restart( ezbus_token_get_ring_timer(token) );
    token->acquired=false;
}

static void ezbus_token_ring_timer_callback( ezbus_timer_t* timer, void* arg )
{
    ezbus_timer_restart( ezbus_token_get_ring_timer(token) );
    ezbus_token_signal_expired( timer, arg );
}
