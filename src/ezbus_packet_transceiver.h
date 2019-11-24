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
#ifndef EZBUS_PACKET_TRANSCEIVER_H_
#define EZBUS_PACKET_TRANSCEIVER_H_

#include <ezbus_platform.h>
#include <ezbus_packet_transmitter.h>
#include <ezbus_packet_receiver.h>
#include <ezbus_address.h>
#include <ezbus_peer_list.h>

/* callback intended to retrieve the address of the node address to the next-in-token-ring */
typedef ezbus_address_t* 	(*ezbus_next_in_token_ring_callback_t) 	( ezbus_address_t* );

/* callback intended to retrieve the peer list */
typedef ezbus_address_t* 	(*ezbus_peer_list_callback_t) 	( ezbus_peer_list_t* );

typedef struct _ezbus_transceiver_t
{
	ezbus_port_t* 							port;
	ezbus_packet_transmitter_t    			packet_transmitter;
    ezbus_packet_receiver_t       			packet_receiver;
    ezbus_ms_tick_t              			transmitter_full_time;
    ezbus_next_in_token_ring_callback_t		token_ring_callback;
    ezbus_peer_list_callback_t				peer_list_callback;
    bool                    				(*layer1_tx_callback)(struct _ezbus_transceiver_t*);
   	bool                    				(*layer1_rx_callback)(struct _ezbus_transceiver_t*);
 } ezbus_packet_transceiver_t;

typedef bool (*ezbus_layer1_callback_t)( struct _ezbus_transceiver_t* );


#ifdef __cplusplus
extern "C" {
#endif


void ezbus_packet_transceiver_init ( 	

									ezbus_packet_transceiver_t* 		packet_transceiver, 
									ezbus_port_t* 						port,

									ezbus_next_in_token_ring_callback_t token_ring_callback, 
									ezbus_peer_list_callback_t 			peer_list_callback,

									ezbus_layer1_callback_t 			layer1_tx_callback,
									ezbus_layer1_callback_t 			layer1_rx_callback

									);

void ezbus_packet_transceiver_run  ( ezbus_packet_transceiver_t* packet_transceiver );


/**
 * @brief calculate the maximum token loop time.
 */
ezbus_ms_tick_t ezbus_packet_transceiver_token_timeout( ezbus_packet_transceiver_t* packet_transceiver );


#ifdef __cplusplus
}
#endif

#endif /* EZBUS_PACKET_TRANSCEIVER_H_ */
