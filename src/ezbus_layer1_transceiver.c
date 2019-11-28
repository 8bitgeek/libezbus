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
#include <ezbus_layer1_transceiver.h>
#include <ezbus_token.h>
#include <ezbus_hex.h>

static bool ezbus_layer1_transceiver_tx_callback ( ezbus_layer1_transmitter_t* layer1_transmitter, void* arg );
static bool ezbus_layer1_transceiver_rx_callback ( ezbus_layer1_receiver_t*    layer1_receiver,    void* arg );


void ezbus_layer1_transceiver_init (    
                                        ezbus_layer1_transceiver_t*             layer1_transceiver, 
                                        ezbus_port_t*                           port,

                                        ezbus_layer1_callback_t                 layer1_tx_callback,
                                        ezbus_layer1_callback_t                 layer1_rx_callback
                                    )
{
    layer1_transceiver->layer1_tx_callback = layer1_tx_callback;
    layer1_transceiver->layer1_rx_callback = layer1_rx_callback;

    ezbus_layer1_receiver_init    ( &layer1_transceiver->layer1_receiver,    port, ezbus_layer1_transceiver_rx_callback, layer1_transceiver );
    ezbus_layer1_transmitter_init ( &layer1_transceiver->layer1_transmitter, port, ezbus_layer1_transceiver_tx_callback, layer1_transceiver );
}

void ezbus_layer1_transceiver_run  ( ezbus_layer1_transceiver_t* layer1_transceiver )
{
    ezbus_layer1_receiver_run( &layer1_transceiver->layer1_receiver );
    ezbus_layer1_transmitter_run( &layer1_transceiver->layer1_transmitter );    
}


static bool ezbus_layer1_transceiver_tx_callback( ezbus_layer1_transmitter_t* layer1_transmitter, void* arg )
{
    bool rc = false;
    ezbus_layer1_transceiver_t* layer1_transceiver = (ezbus_layer1_transceiver_t*)arg;

    switch( ezbus_layer1_transmitter_get_state( layer1_transmitter ) )
    {
        case transmitter_state_empty:
            break;
        case transmitter_state_full:
            break;
        case transmitter_state_send:
            break;
    }
    return rc;
}

static bool ezbus_layer1_transceiver_rx_callback( ezbus_layer1_receiver_t* layer1_receiver, void* arg )
{
    bool rc=false;
    ezbus_layer1_transceiver_t* layer1_transceiver = (ezbus_layer1_transceiver_t*)arg;

    switch ( ezbus_layer1_receiver_get_state( layer1_receiver ) )
    {
        case receiver_state_empty:
            break;
        case receiver_state_full:
            break;
    }
    return rc;
}




