/*****************************************************************************
* Copyright © 2019-2020 Mike Sharkey <mike@8bitgeek.net>                     *
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
#ifndef EZBUS_MAC_ARBITER_TRANSMIT_H_
#define EZBUS_MAC_ARBITER_TRANSMIT_H_

#include <ezbus_mac_arbiter.h>
#include <ezbus_mac.h>
#include <ezbus_timer.h>
#include <ezbus_packet.h>

typedef struct _ezbus_mac_arbiter_transmit_t
{
    ezbus_timer_t                   ack_tx_timer;
    uint8_t                         ack_tx_count;
} ezbus_mac_arbiter_transmit_t;


#ifdef __cplusplus
extern "C" {
#endif

extern void ezbus_mac_arbiter_transmit_init ( ezbus_mac_t* mac );
extern void ezbus_mac_arbiter_transmit_run  ( ezbus_mac_t* mac );
extern void ezbus_mac_arbiter_transmit_token( ezbus_mac_t* mac );
extern void ezbus_mac_arbiter_transmit_push ( ezbus_mac_t* mac, uint8_t level );
extern void ezbus_mac_arbiter_transmit_pop  ( ezbus_mac_t* mac, uint8_t level );

extern bool ezbus_mac_arbiter_transmit_busy ( ezbus_mac_t* mac ); /* state machine? */
extern void ezbus_mac_arbiter_transmit_reset( ezbus_mac_t* mac ); /* state machine? */

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_MAC_ARBITER_TRANSMIT_H_ */
