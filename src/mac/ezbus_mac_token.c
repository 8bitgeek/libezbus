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
#include <ezbus_mac_token.h>
#include <ezbus_mac_peers.h>
#include <ezbus_log.h>

#define NUM_PEERS_HACK  500       // use for debugging / testing.

#define ezbus_mac_token_get_ring_timer(token)  (&(token)->ring_timer)

static void ezbus_mac_token_ring_timer_callback( ezbus_timer_t* timer, void* arg );

extern void ezbus_mac_token_init( ezbus_mac_t* mac )
{
    ezbus_mac_token_t* token = ezbus_mac_get_token( mac );
    ezbus_platform_memset( token, 0, sizeof(ezbus_mac_token_t) );
    ezbus_timer_init( ezbus_mac_token_get_ring_timer(token), true );
    ezbus_timer_set_key( ezbus_mac_token_get_ring_timer(token), "ring_timer" );
    ezbus_timer_set_period( ezbus_mac_token_get_ring_timer(token), 500 /* ezbus_mac_token_ring_time(mac) */ );
    ezbus_timer_set_callback( ezbus_mac_token_get_ring_timer(token), ezbus_mac_token_ring_timer_callback, mac );
}

extern void ezbus_mac_token_run( ezbus_mac_t* mac )
{
    ezbus_mac_token_t* token = ezbus_mac_get_token( mac );
    ezbus_timer_set_period( ezbus_mac_token_get_ring_timer(token), ezbus_mac_token_ring_time(mac) );
    ezbus_timer_run( ezbus_mac_token_get_ring_timer(token) );
}

extern uint32_t ezbus_mac_token_ring_count( ezbus_mac_t* mac )
{
    ezbus_mac_token_t* token = ezbus_mac_get_token( mac );
    return token->ring_count;
}

extern bool ezbus_mac_token_ring_count_timeout( ezbus_mac_t* mac, uint32_t start_count, uint32_t timeout_count )
{
    uint32_t delta_count = ezbus_mac_token_ring_count( mac ) - start_count;
    return ( delta_count > timeout_count );
}

extern uint32_t ezbus_mac_token_ring_time( ezbus_mac_t* mac )
{
    #if NUM_PEERS_HACK
        return NUM_PEERS_HACK;
    #else
        uint32_t packet_sz = sizeof(ezbus_header_t);
        #if NUM_PEERS_HACK
            uint32_t packets_per_round = EZBUS_MAX_PEERS * 2; 
        #else
            uint32_t packets_per_round = ( ezbus_mac_peers_count(mac) * 2);
        #endif
        float    bit_time_sec      = 1.0f/(float)ezbus_port_get_speed( ezbus_mac_get_port(mac) );
        float    packet_bits       = ((float)packet_sz * 12.0f);
        float    packet_time_sec   = packet_bits * bit_time_sec;
        float    secs_per_round    = packet_time_sec * (float)packets_per_round;
        return (int)((secs_per_round * 1000.0f) + 1.0f);
    #endif
}

extern uint32_t ezbus_mac_token_retransmit_time ( ezbus_mac_t* mac )
{
    return ezbus_mac_token_ring_time( mac ) * 4;
}

extern void ezbus_mac_token_reset( ezbus_mac_t* mac )
{
    ezbus_mac_token_t* token = ezbus_mac_get_token( mac );
    ezbus_timer_restart( ezbus_mac_token_get_ring_timer(token) );
}

extern void ezbus_mac_token_acquire( ezbus_mac_t* mac )
{
    ezbus_mac_token_t* token = ezbus_mac_get_token( mac );
    ++token->ring_count;
    ezbus_timer_restart( ezbus_mac_token_get_ring_timer(token) );
    token->acquired=true;
}

extern void ezbus_mac_token_relinquish( ezbus_mac_t* mac )
{
    ezbus_mac_token_t* token = ezbus_mac_get_token( mac );
    ezbus_timer_restart( ezbus_mac_token_get_ring_timer(token) );
    token->acquired=false;
}

extern bool ezbus_mac_token_acquired( ezbus_mac_t* mac )
{
    ezbus_mac_token_t* token = ezbus_mac_get_token( mac );
    return token->acquired;
}


static void ezbus_mac_token_ring_timer_callback( ezbus_timer_t* timer, void* arg )
{
    ezbus_mac_t* mac = (ezbus_mac_t*)arg;
    ezbus_mac_token_t* token = ezbus_mac_get_token( mac );

    EZBUS_LOG( EZBUS_LOG_SOCKET, "period %d", timer->period );

    ezbus_timer_restart( ezbus_mac_token_get_ring_timer(token) );
    ezbus_mac_token_signal_expired( mac );
}
