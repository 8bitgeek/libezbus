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
static ezbus_socket_t ezbus_socket_cycle_next   ( void );
static ezbus_socket_t ezbus_socket_peer_is_open ( ezbus_address_t* peer_address, ezbus_socket_t peer_socket );

extern void ezbus_socket_callback_run( ezbus_mac_t* mac )
{
    for( ezbus_socket_t socket=0; socket < ezbus_socket_get_max(); socket++ )
    {
        // if ( ezbus_socket_keepalive_expired( mac, socket ) )
        // {
        //     EZBUS_LOG( EZBUS_LOG_TIMEOUT, "keepalive expired socket #%d", socket );
        //     ezbus_socket_close( socket );
        // }
    }
}


extern bool ezbus_socket_callback_transmitter_empty( ezbus_mac_t* mac )
{
    /* 
     * The mac transmitter buffer has become available.
     * attempt to give all sockets a fair shake at transmitting 
     */
    for( int n=0; n < ezbus_socket_get_max(); n++ )
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
    if ( ++next_tx_socket >= ezbus_socket_get_max() ) 
        next_tx_socket = 0;
    return next_tx_socket;
}

extern bool ezbus_socket_callback_transmitter_resend( ezbus_mac_t* mac )
{
    ezbus_packet_t* rx_packet = ezbus_mac_get_receiver_packet ( mac );
    ezbus_socket_t  socket    = ezbus_packet_dst_socket       ( rx_packet );
    ezbus_packet_t* tx_packet = ezbus_socket_get_tx_packet    ( socket );

    if ( tx_packet != NULL )
    {
        EZBUS_LOG( EZBUS_LOG_SOCKET, "%d", socket );
        ezbus_packet_t* tx_packet = ezbus_socket_get_tx_packet( socket );
        ezbus_mac_transmitter_put( mac, tx_packet );
        return true;
    }
    else
    {
        EZBUS_LOG( EZBUS_LOG_SOCKET, "??" );
        return false;
    }
}

static ezbus_socket_t ezbus_socket_peer_is_open( ezbus_address_t* peer_address, ezbus_socket_t peer_socket )
{
    /*
     * Determine of the socket has already been opened by the peer,
     * taking into acount that it may not have been reciprocated (yet?).
     */
    for( ezbus_socket_t socket=0; socket < ezbus_socket_get_max(); socket++ )
    {
        if ( ezbus_socket_get_mac( socket ) != NULL )
        {
            if ( ezbus_address_compare( peer_address, ezbus_socket_get_peer_address( socket ) ) == 0 )
            {
                if ( peer_socket == ezbus_socket_get_peer_socket( socket ) )
                {
                    return socket;
                }
            }
        } 
    }
    return EZBUS_SOCKET_ANY;
}

extern bool ezbus_socket_callback_receiver_ready( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    ezbus_packet_t* rx_packet = ezbus_mac_get_receiver_packet ( mac );
    ezbus_address_t* peer     = ezbus_packet_src              ( rx_packet );
    ezbus_socket_t dst_socket = ezbus_packet_dst_socket       ( rx_packet );
    ezbus_socket_t src_socket = ezbus_packet_src_socket       ( rx_packet );

    if ( src_socket == EZBUS_SOCKET_ANY )
    {
        EZBUS_LOG( EZBUS_LOG_SOCKET, "peer close; socket #%d", dst_socket );
        ezbus_socket_close( dst_socket );
        return true;
    }
        
    dst_socket = ezbus_socket_peer_is_open( peer, src_socket );
    
    if ( dst_socket == EZBUS_SOCKET_ANY )
    {
        EZBUS_LOG( EZBUS_LOG_SOCKET, "peer open; peer socket #%d", src_socket );
        dst_socket = ezbus_socket_open( mac, peer, src_socket );
    }

    if ( dst_socket != EZBUS_SOCKET_ANY )
    {
        EZBUS_LOG( EZBUS_LOG_SOCKET, "RX READY; peer socket #%d", dst_socket );
        ezbus_packet_copy( ezbus_socket_get_rx_packet( dst_socket ), rx_packet );
        ezbus_packet_set_dst_socket( ezbus_socket_get_rx_packet( dst_socket ), dst_socket );
        ezbus_packet_set_dst_socket( rx_packet, dst_socket );
        return ezbus_socket_callback_recv( dst_socket );
    }

    return false;
}

extern void ezbus_socket_callback_transmitter_ack( ezbus_mac_t* mac )
{
    ezbus_packet_t* rx_packet = ezbus_mac_get_receiver_packet ( mac );
    ezbus_socket_t socket     = ezbus_packet_dst_socket       ( rx_packet );
    
    EZBUS_LOG( EZBUS_LOG_SOCKET, "%d", socket );
    if ( socket != EZBUS_SOCKET_ANY && ezbus_socket_get_mac( socket ) == mac )
    {
        ezbus_socket_set_tx_seq( socket, ezbus_socket_get_tx_seq( socket ) + 1 );
        EZBUS_LOG( EZBUS_LOG_SOCKET, "seq %d", ezbus_socket_get_tx_seq( socket ) );
    }
    else
    {
        EZBUS_LOG( EZBUS_LOG_SOCKET, "??" );        
    }
}

extern void ezbus_socket_callback_transmitter_nack( ezbus_mac_t* mac )
{
    ezbus_packet_t* rx_packet = ezbus_mac_get_receiver_packet ( mac );
    ezbus_socket_t socket     = ezbus_packet_dst_socket       ( rx_packet );
    
    if ( socket != EZBUS_SOCKET_ANY && ezbus_socket_get_mac( socket ) == mac )
    {
        EZBUS_LOG( EZBUS_LOG_SOCKET, "%d", socket );
    }
    else
    {
        EZBUS_LOG( EZBUS_LOG_SOCKET, "??" );        
    }
}

extern void ezbus_socket_callback_transmitter_limit( ezbus_mac_t* mac )
{
    EZBUS_LOG( EZBUS_LOG_SOCKET, "" );
}

extern void ezbus_socket_callback_transmitter_fault( ezbus_mac_t* mac )
{
    EZBUS_LOG( EZBUS_LOG_SOCKET, "" );
}

extern void ezbus_socket_callback_receiver_fault( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    EZBUS_LOG( EZBUS_LOG_SOCKET, "" );    
}


extern void ezbus_socket_callback_peer( ezbus_mac_t* mac, ezbus_address_t* peer_address, bool peer_available )
{
    EZBUS_LOG( EZBUS_LOG_SOCKET, "Peer %s is %s", ezbus_address_string(peer_address), peer_available ? "AVAILABLE" : "UNAVAILABLE" );
    if ( !peer_available )
    {
        for( ezbus_socket_t socket=0; socket < ezbus_socket_get_max(); socket++ )
        {
            if ( ezbus_address_compare( ezbus_socket_get_peer_address( socket ), peer_address ) == 0 )
            {
                EZBUS_LOG( EZBUS_LOG_SOCKET, "peer vanished, closing socket#%d", socket );
                ezbus_socket_close( socket );
            }
        }
    }
}

extern bool ezbus_socket_callback_peer_active( ezbus_mac_t* mac, ezbus_address_t* peer_address )
{
    for( ezbus_socket_t socket=0; socket < ezbus_socket_get_max(); socket++ )
    {
        if ( ezbus_address_compare( ezbus_socket_get_peer_address( socket ), peer_address ) == 0 )
        {
            EZBUS_LOG( EZBUS_LOG_SOCKET, "Peer %s is ACTIVE", ezbus_address_string(peer_address) );
            return true;
        }
    }
    EZBUS_LOG( EZBUS_LOG_SOCKET, "Peer %s is INACTIVE", ezbus_address_string(peer_address) );
    return false;
}
