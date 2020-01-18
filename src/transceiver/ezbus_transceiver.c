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
#include <ezbus_transceiver.h>
#include <ezbus_mac_transmitter.h>
#include <ezbus_mac_token.h>
#include <ezbus_mac_peers.h>
#include <ezbus_mac.h>
#include <ezbus_packet.h>
#include <ezbus_parcel.h>
#include <ezbus_log.h>

typedef struct _ezbus_transceiver_t
{
    ezbus_mac_t*        mac;
    ezbus_address_t*    peer;


    ezbus_packet_t      tx_packet;
    uint8_t             tx_seq;

    ezbus_packet_t      rx_packet;
    uint8_t             rx_seq;

    EZBUS_ERR           err;

} ezbus_transceiver_t;

static EZBUS_ERR  global_transceiver_err=EZBUS_ERR_OKAY;

static ezbus_transceiver_t  transceivers[ EZBUS_MAX_TRANSCEIVERS ];
static size_t               transceiver_count = 0;

static int32_t              ezbus_transceiver_slot_available ( void );
static void                 ezbus_transceiver_slot_clear     ( size_t index );
static void                 ezbus_transceiver_run_one        ( ezbus_transceiver_t* tranceiver );
static size_t               ezbus_transceiver_count          ( void );
static size_t               ezbus_transceiver_max            ( void );
static ezbus_transceiver_t* ezbus_transceiver_at             ( size_t index );

extern void ezbus_transceiver_init( void )
{
    ezbus_platform_memset( transceivers, 0, sizeof(ezbus_transceiver_t*) * ezbus_transceiver_max() );
    transceiver_count=0;
}

extern void ezbus_transceiver_run( void )
{
    for(size_t n=0; n < ezbus_transceiver_max(); n++)
    {
        ezbus_transceiver_t* transceiver = ezbus_transceiver_at( n );
        ezbus_transceiver_run_one( transceiver );
    }
}


extern int32_t ezbus_transceiver_open( ezbus_mac_t* mac, ezbus_address_t* peer )
{
    int32_t rc = ezbus_transceiver_slot_available();
    if ( rc >= 0 )
    {
        ezbus_transceiver_t* transceiver = &transceivers[rc];
        ezbus_platform_memset( transceiver, 0, sizeof(ezbus_transceiver_t) );
        transceiver->mac = mac;
        transceiver->peer = peer;
        ++transceiver_count;
    }
    return rc;
}

extern void ezbus_transceiver_close( int32_t handle )
{
    if ( ezbus_transceiver_mac( handle ) != NULL )
    {
        ezbus_transceiver_slot_clear( handle );
        --transceiver_count;
    }
}

extern ezbus_mac_t* ezbus_transceiver_mac( int32_t handle )
{
    if ( handle >= 0 && handle < ezbus_transceiver_max() )
    {
        ezbus_transceiver_t* transceiver = ezbus_transceiver_at( handle );
        return transceiver->mac;
    }
    else
    {
        global_transceiver_err=EZBUS_ERR_RANGE;
        return NULL;
    }
}

extern ezbus_packet_t* ezbus_transceiver_tx_packet ( int32_t handle )
{
    if ( handle >= 0 && handle < ezbus_transceiver_max() )
    {
        ezbus_transceiver_t* transceiver = ezbus_transceiver_at( handle );
        return &transceiver->tx_packet;
    }
    else
    {
        global_transceiver_err=EZBUS_ERR_RANGE;
        return NULL;
    }

}

extern ezbus_packet_t* ezbus_transceiver_rx_packet ( int32_t handle )
{
    if ( handle >= 0 && handle < ezbus_transceiver_max() )
    {
        ezbus_transceiver_t* transceiver = ezbus_transceiver_at( handle );
        return &transceiver->rx_packet;
    }
    else
    {
        global_transceiver_err=EZBUS_ERR_RANGE;
        return NULL;
    }

}

extern EZBUS_ERR ezbus_transceiver_err( int32_t handle )
{
    if ( handle >= 0 && handle < ezbus_transceiver_max() )
    {
        ezbus_transceiver_t* transceiver = ezbus_transceiver_at( handle );
        if ( transceiver->err != EZBUS_ERR_OKAY )
        {
            return transceiver->err;
        }
    }
    else
    {
        global_transceiver_err=EZBUS_ERR_RANGE;
    }
    return global_transceiver_err;
}

extern void ezbus_transceiver_reset_err( int32_t handle )
{
    if ( handle >= 0 && handle < ezbus_transceiver_max() )
    {
        ezbus_transceiver_t* transceiver = ezbus_transceiver_at( handle );
        transceiver->err = global_transceiver_err = EZBUS_ERR_OKAY;
    }
    else
    {
        global_transceiver_err=EZBUS_ERR_RANGE;
    }
}





static void ezbus_transceiver_run_one( ezbus_transceiver_t* tranceiver )
{
    /* FIXME - insert code here */
}

static int32_t ezbus_transceiver_slot_available( void )
{
    if ( ezbus_transceiver_count() < ezbus_transceiver_max() )
    {
        for( int32_t n=0; n < ezbus_transceiver_max(); n++ )
        {
            if ( ezbus_transceiver_mac( n ) == NULL )
            {
                return n;
            } 
        }
    }
    return -1;
}

static void ezbus_transceiver_slot_clear( size_t index )
{
    ezbus_platform_memset(&transceivers[ index ], 0, sizeof(ezbus_transceiver_t) );
}

static size_t ezbus_transceiver_count( void )
{
    return transceiver_count;
}

static size_t ezbus_transceiver_max( void )
{
    return EZBUS_MAX_TRANSCEIVERS;
}

static ezbus_transceiver_t* ezbus_transceiver_at( size_t index )
{
    if ( index < ezbus_transceiver_max() )
    {
        return &transceivers[index];
    }
    return NULL;
}
