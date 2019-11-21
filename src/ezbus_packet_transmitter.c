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
#include <ezbus_packet_transmitter.h>
#include <ezbus_hex.h>

void ezbus_packet_transmitter_init( ezbus_packet_transmitter_t* packet_transmitter, ezbus_port_t* port, ezbus_transmitter_callback_t callback, void* arg )
{
    ezbus_platform_memset(packet_transmitter,0,sizeof(ezbus_packet_transmitter_t));
    packet_transmitter->port     = port;
    packet_transmitter->callback = callback;
    packet_transmitter->arg      = arg;
}

void ezbus_packet_transmitter_run ( ezbus_packet_transmitter_t* packet_transmitter )
{
    switch( ezbus_packet_transmitter_get_state( packet_transmitter ) )
    {
        case transmitter_state_empty:
            /*
             * In the event the callback would like to transmit, it should store a packet, and return 'true'.
             */
            if ( packet_transmitter->callback( packet_transmitter, packet_transmitter->arg ) )
            {
                ezbus_packet_transmitter_set_state( transmitter_state_full );
            }
            break;
        case transmitter_state_full:
            if ( ezbus_packet_transmitter_get_token( packet_transmitter ) )
            {
                ezbus_packet_transmitter_set_state( packet_transmitter, transmitter_state_send );
            }
            else
            {
                /**
                 * callback should return 'true' to send regardless of token state, 
                 * else 'false' and/or remedial action on timeout.
                 */
                if ( packet_transmitter->callback( packet_transmitter, packet_transmitter->arg ) )
                {
                    ezbus_packet_transmitter_set_state( packet_transmitter, transmitter_state_send );
                }
            }
            break;
        case transmitter_state_send:
            ezbus_packet_transmitter_set_err( packet_transmitter, 
                                        ezbus_port_send( ezbus_packet_transmitter_get_port( packet_transmitter ), 
                                            ezbus_packet_transmitter_get_packet( packet_transmitter ) ) );
            if ( ezbus_packet_transmitter_get_err( packet_transmitter ) == EZBUS_ERR_OKAY )
            {
                ezbus_hex_dump( "TX:", (uint8_t*)ezbus_packet_transmitter_get_packet( packet_transmitter ), sizeof(ezbus_header_t) );
                ezbus_packet_transmitter_set_state( transmitter_state_give_token );
            }
            else
            {
                /* 
                 * callback should examine fault, return true to reset fault, and/or take remedial action. 
                 */
                if ( packet_transmitter->callback( packet_transmitter, packet_transmitter->arg ) )
                {
                    ezbus_packet_transmitter_set_err( packet_transmitter, EZBUS_ERR_OKAY );
                }
            }
           break;
        case transmitter_state_give_token:
            /* 
             * callback should give up the token without disturning the contents of the transmitter.
             * i.e. it should use port directly to transmit.. 
             * callback should return 'true' upon giving up token.
             */
            if ( packet_transmitter->callback( packet_transmitter, packet_transmitter->arg ) )
            {
                ezbus_packet_transmitter_give_token( packet_transmitter );
                ezbus_packet_transmitter_set_state( packet_transmitter, transmitter_state_wait_ack );
            }
            break;
        case transmitter_state_wait_ack:
            /* 
             * callback should determine if the packet requires an acknowledge, and return 'true' when it arrives. 
             * else upon timeout or ack not required, then callback should reset transmitter state accordingly.
             */
            if ( packet_transmitter->callback( packet_transmitter, packet_transmitter->arg ) )
            {
                ezbus_packet_transmitter_set_state( transmitter_state_empty );
            }
            break;
    }
}

void ezbus_packet_transmitter_store( ezbus_packet_transmitter_t* packet_transmitter, ezbus_packet_t* packet )
{
    ezbus_packet_copy( &packet_transmitter->packet, packet );
    ezbus_packet_transmitter_set_state( packet_transmitter, transmitter_state_full );
}

