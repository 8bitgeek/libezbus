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
#ifndef EZBUS_LAYER1_TRANSCEIVER_H_
#define EZBUS_LAYER1_TRANSCEIVER_H_

#include <ezbus_platform.h>
#include <ezbus_layer1_transmitter.h>
#include <ezbus_layer1_receiver.h>
#include <ezbus_address.h>


typedef struct _ezbus_layer1_transceiver_t
{
	ezbus_layer1_transmitter_t    			layer1_transmitter;
    ezbus_layer1_receiver_t       			layer1_receiver;
    bool                    				(*layer1_tx_callback)(struct _ezbus_layer1_transceiver_t*);
   	bool                    				(*layer1_rx_callback)(struct _ezbus_layer1_transceiver_t*);
 } ezbus_layer1_transceiver_t;

typedef bool (*ezbus_layer1_callback_t)( struct _ezbus_layer1_transceiver_t* );


#ifdef __cplusplus
extern "C" {
#endif


void ezbus_layer1_transceiver_init ( 	

									ezbus_layer1_transceiver_t* 		layer1_transceiver, 
									ezbus_port_t* 						port,

									ezbus_layer1_callback_t 			layer1_tx_callback,
									ezbus_layer1_callback_t 			layer1_rx_callback

									);

void ezbus_layer1_transceiver_run  ( ezbus_layer1_transceiver_t* layer1_transceiver );


#ifdef __cplusplus
}
#endif

#endif /* EZBUS_LAYER1_TRANSCEIVER_H_ */
