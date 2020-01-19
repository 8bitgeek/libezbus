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
#include <ezbus_socket.h>
#include <ezbus_mac_transmitter.h>
#include <ezbus_mac_token.h>
#include <ezbus_mac_peers.h>
#include <ezbus_mac.h>
#include <ezbus_packet.h>
#include <ezbus_parcel.h>
#include <ezbus_log.h>

typedef struct _ezbus_socket_state_t
{
    ezbus_mac_t*        mac;
    ezbus_address_t*    peer;

    ezbus_packet_t      tx_packet;
    uint8_t             tx_seq;

    uint8_t             rx_seq;

    EZBUS_ERR           err;

} ezbus_socket_state_t;

static EZBUS_ERR  global_socket_err=EZBUS_ERR_OKAY;

static ezbus_socket_state_t sockets[ EZBUS_MAX_SOCKETS ];
static size_t               port_count = 0;

static ezbus_socket_t           ezbus_socket_slot_available ( void );
static void                     ezbus_socket_slot_clear     ( size_t index );
static void                     ezbus_socket_run_one        (   ezbus_socket_state_t* tranceiver );
static size_t                   ezbus_socket_count          ( void );
static size_t                   ezbus_socket_max            ( void );
static ezbus_socket_state_t*    ezbus_socket_at             ( size_t index );

extern void ezbus_socket_init( void )
{
    ezbus_platform_memset( sockets, 0, sizeof(ezbus_socket_state_t*) * ezbus_socket_max() );
    port_count=0;
}

extern void ezbus_socket_run( void )
{
    for(size_t n=0; n < ezbus_socket_max(); n++)
    {
        ezbus_socket_state_t* port = ezbus_socket_at( n );
        ezbus_socket_run_one( port );
    }
}


extern ezbus_socket_t ezbus_socket_open( ezbus_mac_t* mac, ezbus_address_t* peer )
{
    ezbus_socket_t rc = ezbus_socket_slot_available();
    if ( rc >= 0 )
    {
        ezbus_socket_state_t* port = &sockets[rc];
        ezbus_platform_memset( port, 0, sizeof(ezbus_socket_state_t) );
        port->mac = mac;
        port->peer = peer;
        ++port_count;
    }
    return rc;
}

extern void ezbus_socket_close( ezbus_socket_t port )
{
    if ( ezbus_socket_is_open( port ) )
    {
        ezbus_socket_slot_clear( port );
        --port_count;
    }
}

extern bool ezbus_socket_is_open( ezbus_socket_t port )
{
    return ( ezbus_socket_mac( port ) != NULL );
}


extern int ezbus_socket_send( ezbus_socket_t port, void* data, size_t size )
{
    
    return 0;
}

extern int ezbus_socket_recv( ezbus_socket_t port, void* data, size_t size )
{
    /* FIXME - insert code here */
    return 0;
}

static bool ezbus_socket_prepare_packet( ezbus_mac_t* mac, ezbus_address_t* dst_address, char* str )
{
    //static int count=0;
    #if EZBUS_TRANSMITTER_TEST
        // if ( ++count > 1 )
        // {
            //count=0;
            if ( ezbus_address_compare( &ezbus_self_address, dst_address ) != 0 && ezbus_address_compare( &ezbus_broadcast_address, dst_address ) != 0 )
            {

                ezbus_log( EZBUS_LOG_SOCKET, "ezbus_socket_send_packet\n" );
                ezbus_mac_peers_log( mac );

                ezbus_packet_init        ( &tx_packet );
                ezbus_packet_set_type    ( &tx_packet, packet_type_parcel );
                ezbus_packet_set_seq     ( &tx_packet, tranceiver_seq );
                ezbus_packet_set_src     ( &tx_packet, &ezbus_self_address );
                ezbus_packet_set_dst     ( &tx_packet, dst_address );
                //ezbus_packet_set_ack_req ( &tx_packet, ~PACKET_BITS_ACK_REQ );

                ezbus_parcel_init        ( &tx_parcel );
                ezbus_parcel_set_string  ( &tx_parcel, str );
                ezbus_packet_set_parcel  ( &tx_packet, &tx_parcel );

                ezbus_mac_transmitter_put( mac, &tx_packet );

                ezbus_log( EZBUS_LOG_SOCKET, "%s\n", str );
                return true;
            }
        // }
    #endif
    return false;
}





extern ezbus_mac_t* ezbus_socket_mac( ezbus_socket_t port )
{
    if ( port >= 0 && port < ezbus_socket_max() )
    {
        ezbus_socket_state_t* port = ezbus_socket_at( port );
        return port->mac;
    }
    else
    {
        global_socket_err=EZBUS_ERR_RANGE;
        return NULL;
    }
}

extern ezbus_packet_t* ezbus_socket_state_tx_packet ( ezbus_socket_t port )
{
    ezbus_socket_state_t* port = ezbus_socket_at( port );
    if ( port != NULL )
    {
        return &port.tx_packet;
    }
    global_socket_err=EZBUS_ERR_RANGE;
    return NULL;
}

extern ezbus_packet_t* ezbus_socket_rx_packet ( ezbus_socket_t port )
{
    if ( ezbus_socket_is_open(port) )
    {
        ezbus_mac_t* mac = ezbus_socket_mac( port );
        return ezbus_mac_get_receiver_packet( mac );
    }
    global_socket_err=EZBUS_ERR_NOTREADY;
    return NULL;
}

extern uint8_t ezbus_socket_state_tx_seq( ezbus_socket_t port )
{
    if ( ezbus_socket_is_open(port) )
    {
        ezbus_socket_state_t* port = ezbus_socket_at( port );
        return port->tx_seq;
    }
    global_socket_err=EZBUS_ERR_NOTREADY;
    return 0;
}

extern uint8_t ezbus_socket_rx_seq( ezbus_socket_t port )
{
    if ( ezbus_socket_is_open(port) )
    {
        ezbus_socket_state_t* port = ezbus_socket_at( port );
        return port->rx_seq;
    }
    global_socket_err=EZBUS_ERR_NOTREADY;
    return 0;
}

extern void ezbus_socket_set_tx_seq( ezbus_socket_t port, uint8_t seq)
{
    if ( ezbus_socket_is_open(port) )
    {
        ezbus_socket_state_t* port = ezbus_socket_at( port );
        port->tx_seq = seq;
    }
    global_socket_err=EZBUS_ERR_NOTREADY;
}

extern void ezbus_socket_set_rx_seq( ezbus_socket_t port, uint8_t seq)
{
    if ( ezbus_socket_is_open(port) )
    {
        ezbus_socket_state_t* port = ezbus_socket_at( port );
        port->rx_seq = seq;
    }
    global_socket_err=EZBUS_ERR_NOTREADY;
}


extern EZBUS_ERR ezbus_socket_err( ezbus_socket_t port )
{
    if ( port >= 0 && port < ezbus_socket_max() )
    {
        ezbus_socket_state_t* port = ezbus_socket_at( port );
        if ( port->err != EZBUS_ERR_OKAY )
        {
            return port->err;
        }
    }
    else
    {
        global_socket_err=EZBUS_ERR_RANGE;
    }
    return global_socket_err;
}

extern void ezbus_socket_reset_err( ezbus_socket_t port )
{
    if ( port >= 0 && port < ezbus_socket_max() )
    {
        ezbus_socket_state_t* port = ezbus_socket_at( port );
        port->err = global_socket_err = EZBUS_ERR_OKAY;
    }
    else
    {
        global_socket_err=EZBUS_ERR_RANGE;
    }
}





static void ezbus_socket_run_one( ezbus_socket_state_t* tranceiver )
{
    /* FIXME - insert code here */
}

static ezbus_socket_t ezbus_socket_slot_available( void )
{
    if ( ezbus_socket_count() < ezbus_socket_max() )
    {
        for( ezbus_socket_t n=0; n < ezbus_socket_max(); n++ )
        {
            if ( ezbus_socket_mac( n ) == NULL )
            {
                return n;
            } 
        }
    }
    return -1;
}

static void ezbus_socket_slot_clear( size_t index )
{
    ezbus_platform_memset(&sockets[ index ], 0, sizeof(ezbus_socket_state_t) );
}

static size_t ezbus_socket_count( void )
{
    return port_count;
}

static size_t ezbus_socket_max( void )
{
    return EZBUS_MAX_SOCKETS;
}

static ezbus_socket_state_t* ezbus_socket_at( size_t index )
{
    if ( index < ezbus_socket_max() )
    {
        return &sockets[index];
    }
    return NULL;
}
