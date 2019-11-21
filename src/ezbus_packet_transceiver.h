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


typedef struct _ezbus_transceiver_t
{
	ezbus_packet_transmitter    packet_transmitter;
    ezbus_packet_receiver       packet_receiver;
} ezbus_transceiver_t;


#ifdef __cplusplus
extern "C" {
#endif



void ezbus_packet_transceiver_init ( ezbus_packet_transceiver_t* packet_transceiver, ezbus_port_t* port );
void ezbus_packet_transceiver_run  ( ezbus_packet_transceiver_t* packet_transceiver );



#ifdef __cplusplus
}
#endif

#endif /* EZBUS_PACKET_TRANSCEIVER_H_ */
