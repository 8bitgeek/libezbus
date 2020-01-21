/*****************************************************************************
* Copyright © 2019-2020 Mike Sharkey <mike.sharkey@mineairquality.com>       *
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
#include <ezbus_socket_callback.h>
#include <ezbus_socket_common.h>
#include <ezbus_socket.h>
#include <ezbus_port.h>
#include <ezbus_mac_transmitter.h>
#include <ezbus_mac_token.h>
#include <ezbus_mac_peers.h>
#include <ezbus_mac.h>
#include <ezbus_packet.h>
#include <ezbus_parcel.h>
#include <ezbus_log.h>

static ezbus_socket_t next_tx_socket=0;

static ezbus_socket_t ezbus_socket_cycle_next( void );

extern bool ezbus_socket_callback_transmitter_empty( ezbus_mac_t* mac )
{
    /* 
     * The mac transmitter buffer has become available.
     * attempt to give all sockets a fair shake at transmitting 
     */
    for( int n=0; n < ezbus_socket_max(); n++ )
    {
        ezbus_socket_t socket = ezbus_socket_cycle_next();
        if ( ezbus_socket_is_open( socket ) )
        {
            if ( ezbus_socket_callback_send ( socket ) )
            {
                return true;
            }
        }
    }
    return false;
}

static ezbus_socket_t ezbus_socket_cycle_next( void )
{
    if ( ++next_tx_socket >= ezbus_socket_max() ) 
        next_tx_socket = 0;
    return next_tx_socket;
}

extern bool ezbus_socket_callback_transmitter_resend( ezbus_mac_t* mac )
{
    ezbus_packet_t* rx_packet = ezbus_mac_get_receiver_packet( mac );
    ezbus_socket_t socket = ezbus_packet_dst_socket( rx_packet );
    ezbus_packet_t* tx_packet = ezbus_socket_tx_packet( socket );

    if ( tx_packet != NULL )
    {
        ezbus_log( EZBUS_LOG_SOCKET, "ezbus_socket_callback_transmitter_resend %d\n", socket );
        ezbus_packet_t* tx_packet = ezbus_socket_tx_packet( socket );
        ezbus_mac_transmitter_put( mac, tx_packet );
        return true;        
    }
    else
    {
        ezbus_log( EZBUS_LOG_SOCKET, "ezbus_socket_callback_transmitter_resend ??\n" );
        return false;
    }
}

extern bool ezbus_socket_callback_receiver_ready( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    ezbus_packet_t* rx_packet = ezbus_mac_get_receiver_packet( mac );
    ezbus_address_t* peer = ezbus_packet_src( rx_packet );
    ezbus_socket_t socket = ezbus_packet_dst_socket( rx_packet );

    if ( socket == EZBUS_SOCKET_ANY )
    {
        socket = ezbus_socket_open( mac, peer );
    }

    if ( socket != EZBUS_SOCKET_ANY )
    {
        return ezbus_socket_callback_recv( socket );
    }

    return false;
}

extern void ezbus_socket_callback_transmitter_ack( ezbus_mac_t* mac )
{
    ezbus_packet_t* rx_packet = ezbus_mac_get_receiver_packet( mac );
    ezbus_socket_t socket = ezbus_packet_dst_socket( rx_packet );
    if ( socket != EZBUS_SOCKET_ANY && ezbus_socket_mac( socket ) == mac )
    {
        ezbus_log( EZBUS_LOG_SOCKET, "ezbus_socket_callback_transmitter_ack %d\n", socket );
        ezbus_socket_set_tx_seq( socket, ezbus_socket_tx_seq( socket ) + 1 );
    }
    else
    {
        ezbus_log( EZBUS_LOG_SOCKET, "ezbus_socket_callback_transmitter_ack ??\n" );        
    }
}

extern void ezbus_socket_callback_transmitter_limit( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_SOCKET, "ezbus_socket_callback_transmitter_limit\n" );
}

extern void ezbus_socket_callback_transmitter_fault( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_SOCKET, "ezbus_socket_callback_transmitter_fault\n" );
}

extern void ezbus_socket_callback_receiver_fault( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    ezbus_log( EZBUS_LOG_SOCKET, "ezbus_socket_callback_receiver_fault (callback)\n" );    
}

extern void ezbus_socket_callback_peer_seen( ezbus_mac_t* mac, ezbus_address_t* address )
{
    ezbus_socket_callback_peer ( mac, address );
}
