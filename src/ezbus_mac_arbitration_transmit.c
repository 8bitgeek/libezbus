/*****************************************************************************
* Copyright 2019 Mike Sharkey <mike.sharkey@mineairquality.com>              *
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
#include <ezbus_mac_arbitration_transmit.h>
#include <ezbus_mac_transmitter.h>
#include <ezbus_mac_coldboot.h>
#include <ezbus_mac_token.h>
#include <ezbus_hex.h>
#include <ezbus_log.h>

extern void ezbus_mac_arbitration_transmit_init  ( ezbus_mac_t* mac )
{
    ezbus_mac_arbitration_transmit_t* arbitration_transmit = ezbus_mac_get_arbitration_transmit( mac );

    ezbus_timer_init( &arbitration_transmit->ack_tx_timer );
    ezbus_timer_set_period( &arbitration_transmit->ack_tx_timer, ezbus_mac_token_ring_time(mac)*4 ); // FIXME *4 ??
}

extern void ezbus_mac_arbitration_transmit_run( ezbus_mac_t* mac )
{
    /* FIXME insert code here */
}



extern void ezbus_mac_transmitter_signal_empty( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_TRANSMITTER, "ezbus_mac_transmitter_signal_empty\n" );
}

extern void ezbus_mac_transmitter_signal_full( ezbus_mac_t* mac )
{   
    ezbus_log( EZBUS_LOG_TRANSMITTER, "ezbus_mac_transmitter_signal_full\n" );
}

extern void ezbus_mac_transmitter_signal_sent( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_TRANSMITTER, "ezbus_mac_transmitter_signal_sent\n" );
}

extern void ezbus_mac_transmitter_signal_wait( ezbus_mac_t* mac )
{
   
    ezbus_log( EZBUS_LOG_TRANSMITTER, "ezbus_mac_transmitter_signal_wait\n" );
}

extern void ezbus_mac_transmitter_signal_fault( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_TRANSMITTER, "ezbus_mac_transmitter_signal_fault\n" );
}





