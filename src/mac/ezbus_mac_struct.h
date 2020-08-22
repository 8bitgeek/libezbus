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
#ifndef EZBUS_MAC_STRUCT_H_
#define EZBUS_MAC_STRUCT_H_

#include <ezbus_platform.h>
#include <ezbus_port.h>
#include <ezbus_mac_peers.h>
#include <ezbus_mac_arbiter.h>
#include <ezbus_mac_arbiter_receive.h>
#include <ezbus_mac_arbiter_transmit.h>
#include <ezbus_mac_coldboot.h>
#include <ezbus_mac_warmboot.h>
#include <ezbus_mac_receiver.h>
#include <ezbus_mac_token.h>
#include <ezbus_mac_transmitter.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _ezbus_mac_t 
{
    ezbus_port_t*                   port;
    ezbus_mac_peers_t               peers;
    ezbus_mac_coldboot_t            coldboot;
    ezbus_mac_warmboot_t            warmboot;
    ezbus_mac_transmitter_t         transmitter;
    ezbus_mac_receiver_t            receiver;
    ezbus_mac_arbiter_t             arbiter;
    ezbus_mac_arbiter_receive_t     arbiter_receive;
    ezbus_mac_arbiter_transmit_t    arbiter_transmit;
    ezbus_mac_token_t               token;
};

typedef struct _ezbus_mac_t ezbus_mac_t;

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_MAC_STRUCT_H_ */
