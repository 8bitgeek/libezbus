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
#ifndef EZBUS_MAC_H_
#define EZBUS_MAC_H_

#include <ezbus_platform.h>
#include <ezbus_port.h>
#include <ezbus_peer_list.h>

typedef struct _ezbus_mac_t                     ezbus_mac_t;
typedef struct _ezbus_mac_arbitration_t         ezbus_mac_arbitration_t;
typedef struct _ezbus_mac_arbitration_receive_t ezbus_mac_arbitration_receive_t;
typedef struct _ezbus_mac_bootstrap_t           ezbus_mac_bootstrap_t;
typedef struct _ezbus_mac_receiver_t            ezbus_mac_receiver_t;
typedef struct _ezbus_mac_token_t               ezbus_mac_token_t;
typedef struct _ezbus_mac_transmitter_t         ezbus_mac_transmitter_t;

#ifdef __cplusplus
extern "C" {
#endif


void ezbus_mac_init ( ezbus_mac_t* mac, ezbus_port_t* port );
void ezbus_mac_run  ( ezbus_mac_t* mac );


extern ezbus_port_t*                     ezbus_mac_get_port                 (ezbus_mac_t* mac);
extern ezbus_peer_list_t*                ezbus_mac_get_peer_list            (ezbus_mac_t* mac);
extern ezbus_mac_bootstrap_t*            ezbus_mac_get_bootstrap            (ezbus_mac_t* mac);
extern ezbus_mac_transmitter_t*          ezbus_mac_get_transmitter          (ezbus_mac_t* mac);
extern ezbus_mac_receiver_t*             ezbus_mac_get_receiver             (ezbus_mac_t* mac);
extern ezbus_mac_arbitration_t*          ezbus_mac_get_arbitration          (ezbus_mac_t* mac);
extern ezbus_mac_arbitration_receive_t*  ezbus_mac_get_arbitration_receive  (ezbus_mac_t* mac);
extern ezbus_mac_token_t*                ezbus_mac_get_token                (ezbus_mac_t* mac);
extern ezbus_packet_t*                   ezbus_mac_get_transmitter_packet   (ezbus_mac_t* mac);
extern ezbus_packet_t*                   ezbus_mac_get_receiver_packet      (ezbus_mac_t* mac);

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_MAC_H_ */
