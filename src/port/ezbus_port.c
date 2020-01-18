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
#include <ezbus_port.h>
#include <ezbus_mac_transmitter.h>
#include <ezbus_mac_token.h>
#include <ezbus_mac_peers.h>
#include <ezbus_mac.h>
#include <ezbus_packet.h>
#include <ezbus_parcel.h>
#include <ezbus_log.h>

typedef struct _ezbus_port_t
{
    ezbus_mac_t*        mac;
    ezbus_address_t*    peer;

    ezbus_packet_t      tx_packet;
    uint8_t             tx_seq;

    uint8_t             rx_seq;

    EZBUS_ERR           err;

} ezbus_port_t;

static EZBUS_ERR  global_port_err=EZBUS_ERR_OKAY;

static ezbus_port_t  ports[ EZBUS_MAX_PORTS ];
static size_t               port_count = 0;

static EZBUS_PORT         ezbus_port_slot_available ( void );
static void                 ezbus_port_slot_clear     ( size_t index );
static void                 ezbus_port_run_one        ( ezbus_port_t* tranceiver );
static size_t               ezbus_port_count          ( void );
static size_t               ezbus_port_max            ( void );
static ezbus_port_t* ezbus_port_at             ( size_t index );

extern void ezbus_port_init( void )
{
    ezbus_platform_memset( ports, 0, sizeof(ezbus_port_t*) * ezbus_port_max() );
    port_count=0;
}

extern void ezbus_port_run( void )
{
    for(size_t n=0; n < ezbus_port_max(); n++)
    {
        ezbus_port_t* port = ezbus_port_at( n );
        ezbus_port_run_one( port );
    }
}


extern EZBUS_PORT ezbus_port_open( ezbus_mac_t* mac, ezbus_address_t* peer )
{
    EZBUS_PORT rc = ezbus_port_slot_available();
    if ( rc >= 0 )
    {
        ezbus_port_t* port = &ports[rc];
        ezbus_platform_memset( port, 0, sizeof(ezbus_port_t) );
        port->mac = mac;
        port->peer = peer;
        ++port_count;
    }
    return rc;
}

extern void ezbus_port_close( EZBUS_PORT port )
{
    if ( ezbus_port_mac( port ) != NULL )
    {
        ezbus_port_slot_clear( port );
        --port_count;
    }
}

extern int ezbus_port_send( EZBUS_PORT port, void* data, size_t size )
{
    
    return 0;
}

extern int ezbus_port_recv( EZBUS_PORT port, void* data, size_t size )
{
    /* FIXME - insert code here */
    return 0;
}

static bool ezbus_port_prepare_packet( ezbus_mac_t* mac, ezbus_address_t* dst_address, char* str )
{
    //static int count=0;
    #if EZBUS_TRANSMITTER_TEST
        // if ( ++count > 1 )
        // {
            //count=0;
            if ( ezbus_address_compare( &ezbus_self_address, dst_address ) != 0 && ezbus_address_compare( &ezbus_broadcast_address, dst_address ) != 0 )
            {

                ezbus_log( EZBUS_LOG_TRANSCEIVER, "ezbus_port_send_packet\n" );
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

                ezbus_log( EZBUS_LOG_TRANSCEIVER, "%s\n", str );
                return true;
            }
        // }
    #endif
    return false;
}





extern ezbus_mac_t* ezbus_port_mac( EZBUS_PORT port )
{
    if ( port >= 0 && port < ezbus_port_max() )
    {
        ezbus_port_t* port = ezbus_port_at( port );
        return port->mac;
    }
    else
    {
        global_port_err=EZBUS_ERR_RANGE;
        return NULL;
    }
}

extern ezbus_packet_t* ezbus_port_tx_packet ( EZBUS_PORT port )
{
    ezbus_port_t* port = ezbus_port_at( port );
    if ( port != NULL )
    {
        return &port.tx_packet;
    }
    global_port_err=EZBUS_ERR_RANGE;
    return NULL;
}

extern ezbus_packet_t* ezbus_port_rx_packet ( EZBUS_PORT port )
{
    ezbus_mac_t* mac = ezbus_port_mac( port );
    if ( mac != NULL )
    {
        return ezbus_mac_get_receiver_packet( mac );
    }
    global_port_err=EZBUS_ERR_RANGE;
    return NULL;
}

extern uint8_t ezbus_port_tx_seq( EZBUS_PORT port )
{
    ezbus_port_t* port = ezbus_port_at( port );
    if ( port != NULL )
    {
        return port->tx_seq;
    }
    else
    {
        global_port_err=EZBUS_ERR_RANGE;
        return 0;
    }
}

extern uint8_t ezbus_port_rx_seq( EZBUS_PORT port )
{
    if ( port >= 0 && port < ezbus_port_max() )
    {
        ezbus_port_t* port = ezbus_port_at( port );
        return port->rx_seq;
    }
    else
    {
        global_port_err=EZBUS_ERR_RANGE;
        return 0;
    }
}

extern void ezbus_port_set_tx_seq( EZBUS_PORT port, uint8_t seq)
{
    if ( port >= 0 && port < ezbus_port_max() )
    {
        ezbus_port_t* port = ezbus_port_at( port );
        port->tx_seq = seq;
    }
    else
    {
        global_port_err=EZBUS_ERR_RANGE;
    }
}

extern void ezbus_port_set_rx_seq( EZBUS_PORT port, uint8_t seq)
{
    if ( port >= 0 && port < ezbus_port_max() )
    {
        ezbus_port_t* port = ezbus_port_at( port );
        port->rx_seq = seq;
    }
    else
    {
        global_port_err=EZBUS_ERR_RANGE;
    }
}


extern EZBUS_ERR ezbus_port_err( EZBUS_PORT port )
{
    if ( port >= 0 && port < ezbus_port_max() )
    {
        ezbus_port_t* port = ezbus_port_at( port );
        if ( port->err != EZBUS_ERR_OKAY )
        {
            return port->err;
        }
    }
    else
    {
        global_port_err=EZBUS_ERR_RANGE;
    }
    return global_port_err;
}

extern void ezbus_port_reset_err( EZBUS_PORT port )
{
    if ( port >= 0 && port < ezbus_port_max() )
    {
        ezbus_port_t* port = ezbus_port_at( port );
        port->err = global_port_err = EZBUS_ERR_OKAY;
    }
    else
    {
        global_port_err=EZBUS_ERR_RANGE;
    }
}





static void ezbus_port_run_one( ezbus_port_t* tranceiver )
{
    /* FIXME - insert code here */
}

static EZBUS_PORT ezbus_port_slot_available( void )
{
    if ( ezbus_port_count() < ezbus_port_max() )
    {
        for( EZBUS_PORT n=0; n < ezbus_port_max(); n++ )
        {
            if ( ezbus_port_mac( n ) == NULL )
            {
                return n;
            } 
        }
    }
    return -1;
}

static void ezbus_port_slot_clear( size_t index )
{
    ezbus_platform_memset(&ports[ index ], 0, sizeof(ezbus_port_t) );
}

static size_t ezbus_port_count( void )
{
    return port_count;
}

static size_t ezbus_port_max( void )
{
    return EZBUS_MAX_PORTS;
}

static ezbus_port_t* ezbus_port_at( size_t index )
{
    if ( index < ezbus_port_max() )
    {
        return &ports[index];
    }
    return NULL;
}
