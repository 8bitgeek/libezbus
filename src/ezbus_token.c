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
#include <ezbus_token.h>

extern void ezbus_token_init( ezbus_token_t* token )
{
    ezbus_platform_memset( token, 0, sizeof( ezbus_token_t ) );
    ezbus_token_touch( token );
}

extern void ezbus_token_set_present( ezbus_token_t* token, bool present )
{
    token->present = present;
    ezbus_token_touch( token );
}

extern bool ezbus_token_present( ezbus_token_t* token )
{
    return token->present;
}

extern void ezbus_token_touch( ezbus_token_t* token )
{
    token->touched = ezbus_platform_get_ms_ticks();
}

extern ezbus_ms_tick_t ezbus_token_touched( ezbus_token_t* token )
{
    return token->touched;
}

extern void ezbus_token_calc_timeout_period ( ezbus_token_t* token, uint32_t packet_sz, uint32_t num_peers, uint32_t baud_rate )
{
    uint32_t packets_per_round = (num_peers * 2);
    float    bit_time_sec      = 1.0f/(float)baud_rate;
    float    packet_bits       = ((float)packet_sz * 12.0f);
    float    packet_time_sec   = packet_bits * bit_time_sec;
    float    secs_per_round    = packet_time_sec * (float)packets_per_round;

    token->timeout_period = (secs_per_round * 1000.0f) + 1.0f;
}

extern ezbus_ms_tick_t ezbus_token_timeout_period( ezbus_token_t* token )
{
    if ( token->timeout_period == 0 )
    {
        token->timeout_period  = EZBUS_TOKEN_TIMEOUT_DEF;
    }
    return token->timeout_period;
}

extern bool ezbus_token_timeout( ezbus_token_t* token )
{
    if ( !ezbus_token_present( token ) )
    {
        return ezbus_platform_get_ms_ticks() - ezbus_token_touched( token ) > ezbus_token_timeout_period( token );
    }
    return false;
}

