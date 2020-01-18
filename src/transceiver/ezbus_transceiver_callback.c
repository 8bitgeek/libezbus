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

extern bool ezbus_transceiver_callback_transmitter_resend( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_TRANSCEIVER, "ezbus_transceiver_callback_transmitter_resend\n" );
    ezbus_mac_transmitter_put( mac, &tx_packet );
    return true;
}

extern void ezbus_transceiver_callback_transmitter_ack( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_TRANSCEIVER, "ezbus_transceiver_callback_transmitter_ack\n" );
    ++tranceiver_seq;
}

extern void ezbus_transceiver_callback_transmitter_limit( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_TRANSCEIVER, "ezbus_transceiver_callback_transmitter_limit\n" );
}

extern void ezbus_transceiver_callback_transmitter_fault( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_TRANSCEIVER, "ezbus_transceiver_callback_transmitter_fault\n" );
}

/**** END TRANSMITTER ****/



/**** BEGIN RECEIVER ****/

extern bool ezbus_transceiver_callback_receiver_ready( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    #if EZBUS_INTEGRITY_TEST
        static int count=0;
        int new_count;
        ezbus_parcel_t* parcel = (ezbus_parcel_t*)ezbus_packet_data( packet );
        ezbus_parcel_get_string( parcel, text_buf );
        new_count = atoi(text_buf);
        if ( new_count != count )
        {
            ezbus_log( EZBUS_LOG_TRANSCEIVER, "%d != %d\n", count, new_count );
            count = new_count+1;
        }
        else
        {
            ++count;
        }

    #else
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
            //ezbus_log( EZBUS_LOG_TRANSCEIVER, "ezbus_transceiver_callback_receiver_ready (callback)\n" );
        }
    #endif

    return true;
}

extern void ezbus_transceiver_callback_receiver_fault( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    ezbus_log( EZBUS_LOG_TRANSCEIVER, "ezbus_transceiver_callback_receiver_fault (callback)\n" );    
}


/**** END RECEIVER ****/

