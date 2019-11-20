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
#ifndef EZBUS_PACKET_TRANSMITTERR_H_
#define EZBUS_PACKET_TRANSMITTERR_H_

#include <ezbus_platform.h>



typedef enum
{
	transmitter_state_empty=0,
	transmitter_state_full,
	transmitter_state_send,
	transmitter_state_wait_ack
} ezbus_transmitter_state_t;

typedef struct 
{
	ezbus_packet_t 			packet[2];
	ezbus_packet_t
    EZBUS_ERR       		err;
    ezbus_transmitter_state_t  state;
} ezbus_transmitter_t;

#define ezbus_packet_transmitter_set_state(packet_transmitter) 	(packet_transmitter->state=state)
#define ezbus_packet_transmitter_get_state(packet_transmitter) 	(packet_transmitter->state)
#define ezbus_packet_transmitter_empty(packet_transmitter)		(ezbus_packet_transmitter_get_state(packet_transmitter)==transmitter_state_empty)
#define ezbus_packet_transmitter_full(packet_transmitter) 		(ezbus_packet_transmitter_get_state(packet_transmitter)!=transmitter_state_empty)

#ifdef __cplusplus
extern "C" {
#endif

void ezbus_packet_transmitter_init( packet_transmitter_t* packet_transmitter );
void ezbus_packet_transmitter_run ( packet_transmitter_t* packet_transmitter );

void ezbus_packet_transmitter_load( packet_transmitter_t* packet_transmitter, ezbus_packet_t* packet );

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_PACKET_TRANSMITTERR_H_ */
