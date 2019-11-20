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
#include <ezbus_packet.h>
#include <ezbus_crc.h>

void ezbus_packet_transmitter_init( packet_transmitter_t* packet_transmitter )
{
    ezbus_platform_memset(packet_transmitter,0,sizeof(packet_transmitter_t));
}

void ezbus_packet_transmitter_run ( packet_transmitter_t* packet_transmitter )
{
    switch( ezbus_packet_transmitter_get_state( packet_transmitter ) )
    {
        case transmitter_state_empty:
            /* *FIXME - callback tx empty */
            break;
        case transmitter_state_full:
            if ( has_token )
                state = transmitter_state_send;
            break;
        case transmitter_state_send:
            break;
        case transmitter_state_wait_ack:
            break;        
    }
}

void ezbus_packet_transmitter_load( packet_transmitter_t* packet_transmitter, ezbus_packet_t* packet )
{
    ezbus_packet_copy(&packet_transmitter->packet);
    ezbus_packet_transmitter_set_state(packet_transmitter,transmitter_state_full);
}
