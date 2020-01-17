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

static uint8_t tranceiver_seq=0;
static ezbus_packet_t tx_packet;
static ezbus_parcel_t tx_parcel;

static ezbus_ms_tick_t last_rx=0;
static uint32_t bytes_received=0;
static float rx_seconds=0.0f;

static bool ezbus_transceiver_send_packet( ezbus_mac_t* mac, ezbus_address_t* dst_address, char* str );

extern void ezbus_transceiver_init ( ezbus_port_t* port )
{

}


extern void ezbus_transceiver_run( void )
{
}


/**** BEGIN TRANSMITTER ****/

extern bool ezbus_transceiver_transmitter_empty( ezbus_mac_t* mac )
{
    ezbus_address_t* dst_address = ezbus_mac_peers_next( mac, &ezbus_self_address );

    //ezbus_log( EZBUS_LOG_TRANSCEIVER, "ezbus_transceiver_transmitter_empty (callback)\n" );

    
    return ezbus_transceiver_send_packet( mac, dst_address, "What was the person thinking when they discovered cow’s milk was fine for human consumption… and why did they do it in the first place!?" );
}

static bool ezbus_transceiver_send_packet( ezbus_mac_t* mac, ezbus_address_t* dst_address, char* str )
{
    //static int count=0;
    #if EZBUS_TRANSMITTER_TEST
        // if ( ++count > 1 )
        // {
            //count=0;
            if ( ezbus_address_compare( &ezbus_self_address, dst_address ) != 0 && ezbus_address_compare( &ezbus_broadcast_address, dst_address ) != 0 )
            {

                ezbus_log( EZBUS_LOG_TRANSCEIVER, "ezbus_transceiver_send_packet\n" );
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

                return true;
            }
        // }
    #endif
    return false;
}

extern bool ezbus_transceiver_transmitter_resend( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_TRANSCEIVER, "ezbus_transceiver_transmitter_resend\n" );
    ezbus_mac_transmitter_put( mac, &tx_packet );
    return true;
}

extern void ezbus_transceiver_transmitter_ack( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_TRANSCEIVER, "ezbus_transceiver_transmitter_ack\n" );
    ++tranceiver_seq;
}

extern void ezbus_transceiver_transmitter_limit( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_TRANSCEIVER, "ezbus_transceiver_transmitter_limit\n" );
}

extern void ezbus_transceiver_transmitter_fault( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_TRANSCEIVER, "ezbus_transceiver_transmitter_fault\n" );
}

/**** END TRANSMITTER ****/



/**** BEGIN RECEIVER ****/

extern bool ezbus_transceiver_receiver_ready( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    ezbus_ms_tick_t now = ezbus_platform_get_ms_ticks();
    ezbus_ms_tick_t delta_ticks = now - last_rx;

    last_rx = now;
    bytes_received += EZBUS_PARCEL_DATA_LN;
    rx_seconds += 0.001f * delta_ticks;

    if ( rx_seconds > 1.0f )
    {
        ezbus_log( EZBUS_LOG_TRANSCEIVER, "RX BYTES/SEC: %d \n", bytes_received );
        rx_seconds=0.0f;
        bytes_received=0;
    }
    else
    {
        //ezbus_log( EZBUS_LOG_TRANSCEIVER, "ezbus_transceiver_receiver_ready (callback)\n" );
    }

    return true;
}

extern void ezbus_transceiver_receiver_fault( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    ezbus_log( EZBUS_LOG_TRANSCEIVER, "ezbus_transceiver_receiver_fault (callback)\n" );    
}


/**** END RECEIVER ****/

