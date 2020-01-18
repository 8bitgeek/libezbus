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
    ezbus_mac_t*    mac;
    ezbus_packet_t  tx_packet;
    uint8_t         tx_seq;

} ezbus_transceiver_t;

static ezbus_transceiver_t* transceivers[ EZBUS_MAX_TRANSCEIVERS ];
static state_t              transceiver_count = 0;

static int32_t              ezbus_transceiver_slot_available ( void );
static void                 ezbus_transceiver_slot_clear     ( size_t index );
static void                 ezbus_transceiver_run_one        ( ezbus_transceiver_t* tranceiver );
static size_t               ezbus_transceiver_count          ( void );
static size_t               ezbus_transceiver_max            ( void );
static ezbus_transceiver_t* ezbus_transceiver_at             ( size_t index );

extern void ezbus_transceiver_init( void )
{
    ezbus_platform_memset( transceivers, 0, sizeof(ezbus_transceiver_t*) * ezbus_transceiver_max() )
}

extern void ezbus_transceiver_run( void )
{
    for(size_t n=0; n < ezbus_transceiver_max(); n++)
    {
        ezbus_transceiver_t* transceiver = ezbus_transceiver_at( n );
        ezbus_transceiver_run_one( transceiver );
    }
}


extern bool ezbus_transceiver_open( ezbus_transceiver_t* transceiver, ezbus_mac_t* mac )
{
    if ( ezbus_transceiver_slot_available() >= 0 )
    {}
    ezbus_platform_memset( transceiver, 0, sizeof(ezbus_transceiver_t) );
    transceiver->mac = mac;
}

extern void ezbus_transceiver_close( ezbus_transceiver_t* transceiver )
{

}



static void ezbus_transceiver_run_one( ezbus_transceiver_t* tranceiver )
{
    /* FIXME - insert code here */
}

static int32_t ezbus_transceiver_slot_available( void )
{
    for( int32_t n=0; n < ezbus_transceiver_max(); n++ )
    {
        ezbus_transceiver_t* transceiver = transceivers[ n ];
        if ( transceiver == NULL )
        {
            return n;
        } 
    }
    return -1;
}

static void ezbus_transceiver_slot_clear( size_t index )
{
    transceivers[ n ] = NULL;
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
        return trasnceiver[index];
    }
    return NULL;
}
