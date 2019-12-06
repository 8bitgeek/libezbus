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
#include <ezbus_layer0_transceiver.h>
#include <ezbus_layer1_transceiver.h>
#include <ezbus_log.h>

bool ezbus_layer_1_transceiver_tx( ezbus_layer0_transceiver_t* layer0_transceiver );
bool ezbus_layer_1_transceiver_rx( ezbus_layer0_transceiver_t* layer0_transceiver );


void ezbus_layer1_transceiver_init (    
                                        ezbus_layer1_transceiver_t*             layer1_transceiver, 
                                        ezbus_port_t*                           port
                                    )
{
    ezbus_layer0_transceiver_init( &layer1_transceiver->layer0_transceiver, port, ezbus_layer_1_transceiver_tx, ezbus_layer_1_transceiver_rx );
}


void ezbus_layer1_transceiver_run( ezbus_layer1_transceiver_t* layer1_transceiver )
{
    ezbus_layer0_transceiver_run( &layer1_transceiver->layer0_transceiver );
}


bool ezbus_layer_1_transceiver_tx( ezbus_layer0_transceiver_t* layer0_transceiver )
{
    ezbus_log( EZBUS_LOG_TRANSMITTER, "ezbus_layer_1_transceiver_tx (callback)\n" );
    return false;
}

bool ezbus_layer_1_transceiver_rx( ezbus_layer0_transceiver_t* layer0_transceiver )
{
    ezbus_log( EZBUS_LOG_RECEIVER, "ezbus_layer_1_transceiver_rx (callback)\n" );
    return true;
}




