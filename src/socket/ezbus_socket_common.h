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
#ifndef _EZBUS_SOCKET_COMMON_H_
#define _EZBUS_SOCKET_COMMON_H_

#include <ezbus_platform.h>
#include <ezbus_packet.h>
#include <ezbus_mac.h>
#include <ezbus_fault.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ezbus_socket_state_t
{
    ezbus_mac_t*        mac;
    ezbus_packet_t      tx_packet;
    ezbus_packet_t      rx_packet;
    uint8_t             tx_seq;
    uint8_t             rx_seq;
    EZBUS_ERR           err;
    uint32_t            keepalive_start;
} ezbus_socket_state_t;

extern ezbus_socket_state_t ezbus_sockets[];

extern ezbus_mac_t*             ezbus_socket_get_mac            ( ezbus_socket_t socket );
extern ezbus_address_t*         ezbus_socket_get_peer_address   ( ezbus_socket_t socket );
extern ezbus_socket_t           ezbus_socket_get_peer_socket    ( ezbus_socket_t socket );
extern EZBUS_ERR                ezbus_socket_get_err            ( ezbus_socket_t socket );
extern void                     ezbus_socket_set_err            ( ezbus_socket_t socket, EZBUS_ERR err );
extern void                     ezbus_socket_reset_err          ( ezbus_socket_t socket );
extern size_t                   ezbus_socket_get_max            ( void );
extern ezbus_socket_state_t*    ezbus_socket_get_at             ( size_t index );


extern ezbus_packet_t*          ezbus_socket_get_tx_packet      ( ezbus_socket_t socket );
extern uint8_t                  ezbus_socket_get_tx_seq         ( ezbus_socket_t socket );
extern void                     ezbus_socket_set_tx_seq         ( ezbus_socket_t socket, uint8_t seq);

extern ezbus_packet_t*          ezbus_socket_get_rx_packet      ( ezbus_socket_t socket );
extern uint8_t                  ezbus_socket_get_rx_seq         ( ezbus_socket_t socket );
extern void                     ezbus_socket_set_rx_seq         ( ezbus_socket_t socket, uint8_t seq);

extern void                     ezbus_socket_keepalive_reset    ( ezbus_mac_t* mac, ezbus_socket_t socket );
extern bool                     ezbus_socket_keepalive_expired  ( ezbus_mac_t* mac, ezbus_socket_t socket );

#ifdef __cplusplus
}
#endif

#endif /* _EZBUS_SOCKET_COMMON_H_ */
