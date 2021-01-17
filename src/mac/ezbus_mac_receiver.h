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
#ifndef EZBUS_MAC_RECEIVER_H_
#define EZBUS_MAC_RECEIVER_H_

#include <ezbus_platform.h>
#include <ezbus_mac.h>
#include <ezbus_mac_timer.h>

typedef enum
{
    receiver_state_empty=0,
    receiver_state_full,
    receiver_state_receive_fault
} ezbus_receiver_state_t;

typedef struct _ezbus_mac_receiver_t
{
    ezbus_packet_t          packet;
    ezbus_receiver_state_t  state;
    EZBUS_ERR               err;
} ezbus_mac_receiver_t;

#ifdef __cplusplus
extern "C" {
#endif

extern void ezbus_mac_receiver_init ( ezbus_mac_t* mac );
extern void ezbus_mac_receiver_run  ( ezbus_mac_t* mac );
extern void ezbus_mac_receiver_get  ( ezbus_mac_t* mac, ezbus_packet_t* packet );
extern void ezbus_mac_receiver_push ( ezbus_mac_t* mac, uint8_t level );
extern void ezbus_mac_receiver_pop  ( ezbus_mac_t* mac, uint8_t level );

extern void         ezbus_mac_receiver_set_err( ezbus_mac_t* mac, EZBUS_ERR err );
extern EZBUS_ERR    ezbus_mac_receiver_get_err( ezbus_mac_t* mac );

extern void                   ezbus_mac_receiver_set_state( ezbus_mac_t* mac, ezbus_receiver_state_t state );
extern ezbus_receiver_state_t ezbus_mac_receiver_get_state( ezbus_mac_t* mac );

extern void ezbus_mac_receiver_signal_empty ( ezbus_mac_t* mac );
extern void ezbus_mac_receiver_signal_full  ( ezbus_mac_t* mac );
extern void ezbus_mac_receiver_signal_fault ( ezbus_mac_t* mac );

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_MAC_RECEIVER_H_ */
