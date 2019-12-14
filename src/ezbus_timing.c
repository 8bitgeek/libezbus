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
#include <ezbus_timing.h>
#include <ezbus_packet.h>

extern uint32_t ezbus_timing_ring_time( uint32_t baud_rate, uint32_t num_peers )
{
    num_peers = EZBUS_MAX_PEERS; // FIXME - hack
    uint32_t packet_sz = sizeof(ezbus_header_t);
    uint32_t packets_per_round = (num_peers * 2);
    float    bit_time_sec      = 1.0f/(float)baud_rate;
    float    packet_bits       = ((float)packet_sz * 12.0f);
    float    packet_time_sec   = packet_bits * bit_time_sec;
    float    secs_per_round    = packet_time_sec * (float)packets_per_round;
    return (secs_per_round * 1000.0f) + 1.0f;
}
