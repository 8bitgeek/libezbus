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
#include <ezbus_mac_transmitter.h>
#include <ezbus_mac.h>
#include <ezbus_log.h>

bool ezbus_layer_1_transceiver_tx( ezbus_mac_t* mac );
bool ezbus_layer_1_transceiver_rx( ezbus_mac_t* mac );


void ezbus_layer1_transceiver_init (    
                                        ezbus_layer1_transceiver_t*             layer1_transceiver, 
                                        ezbus_port_t*                           port
                                    )
{
    ezbus_platform_memset( layer1_transceiver, 0, sizeof(ezbus_layer1_transceiver_t) );
}


void ezbus_layer1_transceiver_run( ezbus_layer1_transceiver_t* layer1_transceiver )
{
}


bool ezbus_layer_1_transceiver_tx( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_TRANSMITTER, "ezbus_layer_1_transceiver_tx (callback) %s %d \n", 
                                        ezbus_mac_transmitter_get_state_str( mac ),
                                        ezbus_mac_get_token( mac ) );
    return false;
}

bool ezbus_layer_1_transceiver_rx( ezbus_mac_t* mac )
{
    ezbus_log( EZBUS_LOG_RECEIVER, "ezbus_layer_1_transceiver_rx (callback)\n" );
    return true;
}




