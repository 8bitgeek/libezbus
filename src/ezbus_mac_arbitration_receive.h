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
#ifndef EZBUS_MAC_ARBITRATION_RECEIVE_H_
#define EZBUS_MAC_ARBITRATION_RECEIVE_H_

#include <ezbus_mac_arbitration.h>
#include <ezbus_mac_transmitter.h>
#include <ezbus_mac_receiver.h>
#include <ezbus_timer.h>

typedef struct
{
    ezbus_mac_arbitration_t*    mac_arbitration;
    ezbus_mac_receiver_t*       mac_receiver;
} ezbus_mac_arbitration_receive_t;

#ifdef __cplusplus
extern "C" {
#endif


extern void ezbus_mac_arbitration_receive_init  ( 
                                                    ezbus_mac_arbitration_receive_t* mac_arbitration_receive, 
                                                    ezbus_mac_arbitration_t*         mac_arbitration,
                                                    ezbus_mac_receiver_t*            mac_receiver
                                                );

extern void ezbus_mac_arbitration_receive_packet ( ezbus_mac_arbitration_receive_t* arbitration_receive );


#ifdef __cplusplus
}
#endif

#endif /* EZBUS_MAC_ARBITRATION_RECEIVE_H_ */
