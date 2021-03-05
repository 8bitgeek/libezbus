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
#ifndef EZBUS_MAC_H_
#define EZBUS_MAC_H_

#include <ezbus_types.h>
#include <ezbus_port.h>

typedef struct _ezbus_mac_t                  ezbus_mac_t;
typedef struct _ezbus_mac_peers_t            ezbus_mac_peers_t;
typedef struct _ezbus_mac_boot0_t            ezbus_mac_boot0_t;
typedef struct _ezbus_mac_boot1_t            ezbus_mac_boot1_t;
typedef struct _ezbus_mac_boot2_t            ezbus_mac_boot2_t;
typedef struct _ezbus_mac_transmitter_t      ezbus_mac_transmitter_t;
typedef struct _ezbus_mac_receiver_t         ezbus_mac_receiver_t;
typedef struct _ezbus_mac_arbiter_t          ezbus_mac_arbiter_t;
typedef struct _ezbus_mac_arbiter_pause_t    ezbus_mac_arbiter_pause_t;
typedef struct _ezbus_mac_arbiter_receive_t  ezbus_mac_arbiter_receive_t;
typedef struct _ezbus_mac_arbiter_transmit_t ezbus_mac_arbiter_transmit_t;
typedef struct _ezbus_mac_token_t            ezbus_mac_token_t;
typedef struct _ezbus_mac_pause_t            ezbus_mac_pause_t;
typedef struct _ezbus_mac_timer_t            ezbus_mac_timer_t;

#ifdef __cplusplus
extern "C" {
#endif

#define  ezbus_mac_boot0_reset(mac)                                             \
            {                                                                   \
                ezbus_mac_boot0_set_state((mac),state_boot0_start);             \
                ezbus_mac_boot1_set_state((mac),state_boot1_stop);              \
                ezbus_mac_boot2_set_state((mac),state_boot2_idle);              \
                ezbus_mac_arbiter_set_state((mac),mac_arbiter_state_offline);   \
            }

extern void                          ezbus_mac_init                     (ezbus_mac_t* mac, ezbus_port_t* port);
extern void                          ezbus_mac_run                      (ezbus_mac_t* mac);
extern bool                          ezbus_mac_push                     (ezbus_mac_t* mac);
extern bool                          ezbus_mac_pop                      (ezbus_mac_t* mac);
extern ezbus_port_t*                 ezbus_mac_get_port                 (ezbus_mac_t* mac);
extern ezbus_mac_peers_t*            ezbus_mac_get_peers                (ezbus_mac_t* mac);
extern ezbus_mac_boot0_t*            ezbus_mac_get_boot0                (ezbus_mac_t* mac);
extern ezbus_mac_boot1_t*            ezbus_mac_get_boot1                (ezbus_mac_t* mac);
extern ezbus_mac_boot2_t*            ezbus_mac_get_boot2                (ezbus_mac_t* mac);
extern ezbus_mac_transmitter_t*      ezbus_mac_get_transmitter          (ezbus_mac_t* mac);
extern ezbus_mac_receiver_t*         ezbus_mac_get_receiver             (ezbus_mac_t* mac);
extern ezbus_mac_arbiter_t*          ezbus_mac_get_arbiter              (ezbus_mac_t* mac);
extern ezbus_mac_arbiter_pause_t*    ezbus_mac_get_arbiter_pause        (ezbus_mac_t* mac);
extern ezbus_mac_arbiter_receive_t*  ezbus_mac_get_arbiter_receive      (ezbus_mac_t* mac);
extern ezbus_mac_arbiter_transmit_t* ezbus_mac_get_arbiter_transmit     (ezbus_mac_t* mac);
extern ezbus_mac_token_t*            ezbus_mac_get_token                (ezbus_mac_t* mac);
extern ezbus_packet_t*               ezbus_mac_get_transmitter_packet   (ezbus_mac_t* mac);
extern ezbus_packet_t*               ezbus_mac_get_receiver_packet      (ezbus_mac_t* mac);
extern ezbus_mac_pause_t*            ezbus_mac_get_pause                (ezbus_mac_t* mac);
extern ezbus_mac_timer_t*            ezbus_mac_get_timer                (ezbus_mac_t* mac);

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_MAC_H_ */
