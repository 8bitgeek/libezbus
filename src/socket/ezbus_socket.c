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
#include <ezbus_socket_common.h>
#include <ezbus_mac_transmitter.h>
#include <ezbus_mac_token.h>
#include <ezbus_mac_peers.h>
#include <ezbus_mac.h>
#include <ezbus_packet.h>
#include <ezbus_parcel.h>
#include <ezbus_log.h>


ezbus_socket_state_t ezbus_sockets[ EZBUS_MAX_SOCKETS ];

static size_t           socket_count = 0;

static ezbus_socket_t   ezbus_socket_slot_available ( void );
static void             ezbus_socket_slot_clear     ( size_t index );
static size_t           ezbus_socket_count          ( void );
static size_t           ezbus_socket_prepare_packet ( ezbus_socket_t socket, ezbus_address_t* dst_address, ezbus_socket_t dst_socket, uint8_t* data, size_t size );


extern void ezbus_socket_init( void )
{
    ezbus_platform_memset( ezbus_sockets, 0, sizeof(ezbus_socket_state_t*) * ezbus_socket_max() );
    socket_count=0;
}

extern ezbus_socket_t ezbus_socket_open( ezbus_mac_t* mac, ezbus_address_t* peer )
{
    ezbus_socket_t rc = ezbus_socket_slot_available();
    if ( rc != EZBUS_SOCKET_INVALID )
    {
        ezbus_socket_state_t* socket_state = &ezbus_sockets[rc];
        ezbus_platform_memset( socket_state, 0, sizeof(ezbus_socket_state_t) );
        socket_state->mac = mac;
        socket_state->peer = peer;
        ++socket_count;
    }
    return rc;
}

extern void ezbus_socket_close( ezbus_socket_t socket )
{
    if ( ezbus_socket_is_open( socket ) )
    {
        ezbus_socket_slot_clear( socket );
        --socket_count;
    }
}

extern bool ezbus_socket_is_open( ezbus_socket_t socket )
{
    return ( ezbus_socket_mac( socket ) != NULL );
}


extern int ezbus_socket_send( ezbus_socket_t socket, void* data, size_t size )
{

    size_t parcel_data_size = ezbus_socket_prepare_packet( socket, );   // FIXME - insert code here

    ezbus_mac_transmitter_put( mac, tx_packet );

    return parcel_data_size;
}

extern int ezbus_socket_recv( ezbus_socket_t socket, void* data, size_t size )
{
    /* FIXME - insert code here */
    return 0;
}

static size_t ezbus_socket_prepare_packet   ( 
                                                ezbus_socket_t socket, 
                                                ezbus_address_t* dst_address, 
                                                ezbus_socket_t dst_socket, 
                                                uint8_t* data, 
                                                size_t size 
                                            )
{
    ezbus_packet_t* tx_packet = ezbus_socket_tx_packet( socket );
    ezbus_parcel_t* tx_parcel = ezbus_packet_get_parcel( tx_packet );

    size_t parcel_data_size = ( size > EZBUS_PARCEL_DATA_LN ) ? EZBUS_PARCEL_DATA_LN : size;

    ezbus_packet_init           ( tx_packet );
    ezbus_packet_set_type       ( tx_packet, packet_type_parcel );
    ezbus_packet_set_seq        ( tx_packet, ezbus_socket_tx_seq( socket ) );
    ezbus_packet_set_src        ( tx_packet, &ezbus_self_address );
    ezbus_packet_set_src_socket ( tx_packet, socket );
    ezbus_packet_set_dst        ( tx_packet, dst_address );
    ezbus_packet_set_dst_socket ( tx_packet, dst_socket );

    ezbus_parcel_init( tx_parcel );
    ezbus_parcel_set_data( tx_parcel, data, parcel_data_size );

    return parcel_data_size;
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
    return EZBUS_SOCKET_INVALID;
}

static void ezbus_socket_slot_clear( size_t index )
{
    ezbus_platform_memset(&ezbus_sockets[ index ], 0, sizeof(ezbus_socket_state_t) );
}

static size_t ezbus_socket_count( void )
{
    return socket_count;
}
