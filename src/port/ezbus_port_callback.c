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
#include <ezbus_port_callback.h>
#include <ezbus_port.h>
#include <ezbus_mac_transmitter.h>
#include <ezbus_mac_token.h>
#include <ezbus_mac_peers.h>
#include <ezbus_mac.h>
#include <ezbus_packet.h>
#include <ezbus_parcel.h>
#include <ezbus_log.h>

extern bool ezbus_port_transmitter_empty( ezbus_mac_t* mac )
{
    for( int port=0; port < )
    extern bool ezbus_port_callback_send ( EZBUS_HANDLE handle );




        ezbus_address_t* dst_address = ezbus_mac_peers_next( mac, &ezbus_self_address );

    //ezbus_log( EZBUS_LOG_TRANSCEIVER, "ezbus_port_transmitter_empty (callback)\n" );

    #if EZBUS_INTEGRITY_TEST
        static int32_t count=0;
        sprintf(text_buf,"%d",count++);
    #else
        ezbus_platform_strcpy( text_buf, "What was the person thinking when they discovered cow’s milk was fine for human consumption… and why did they do it in the first place!?" );
    #endif

    return ezbus_port_send_packet( mac, dst_address, text_buf );
}

extern bool ezbus_port_callback_transmitter_resend( ezbus_mac_t* mac )
{
    ezbus_packet_t* rx_packet = ezbus_mac_get_receiver_packet( mac );
    ezbus_packet_t* tx_packet = 



    int32_t handle = ezbus_packet_get_port( rz_packet );
    if ( handle >= 0 && ezbus_port_mac( handle ) == mac )
    {
        ezbus_log( EZBUS_LOG_TRANSCEIVER, "ezbus_port_callback_transmitter_resend %d\n", handle );
        ezbus_packet_t* tx_packet = ezbus_port_tx_packet( handle );
        ezbus_mac_transmitter_put( mac, tx_packet );
    }
    else
    {
        ezbus_log( EZBUS_LOG_TRANSCEIVER, "ezbus_port_callback_transmitter_resend ??\n" );
    }
    return true;
}

extern void ezbus_port_callback_transmitter_ack( ezbus_mac_t* mac )
{
     ezbus_packet_t* rx_packet = ezbus_mac_get_receiver_packet( mac );
    int32_t handle = ezbus_packet_get_port( rz_packet );
    if ( handle >= 0 && ezbus_port_mac( handle ) == mac )
    {
        ezbus_log( EZBUS_LOG_TRANSCEIVER, "ezbus_port_callback_transmitter_ack %d\n", handle );
        ezbus_port_set_tx_seq( handle, ezbus_port_tx_seq( handle ) + 1 );
    }
    else
    {
        ezbus_log( EZBUS_LOG_TRANSCEIVER, "ezbus_port_callback_transmitter_ack ??\n" );        
    }
}

extern void ezbus_port_callback_transmitter_limit( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_TRANSCEIVER, "ezbus_port_callback_transmitter_limit\n" );
}

extern void ezbus_port_callback_transmitter_fault( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_TRANSCEIVER, "ezbus_port_callback_transmitter_fault\n" );
}

/**** END TRANSMITTER ****/



/**** BEGIN RECEIVER ****/

extern bool ezbus_port_callback_receiver_ready( ezbus_mac_t* mac, ezbus_packet_t* packet )
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
            //ezbus_log( EZBUS_LOG_TRANSCEIVER, "ezbus_port_callback_receiver_ready (callback)\n" );
        }
    #endif

    return true;
}

extern void ezbus_port_callback_receiver_fault( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    ezbus_log( EZBUS_LOG_TRANSCEIVER, "ezbus_port_callback_receiver_fault (callback)\n" );    
}


/**** END RECEIVER ****/

