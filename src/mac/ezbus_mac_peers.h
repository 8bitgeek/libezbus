/*****************************************************************************
* Copyright © 2019-2020 Mike Sharkey <mike.sharkey@mineairquality.com>       *
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
#ifndef EZBUS_MAC_PEERS_H_
#define EZBUS_MAC_PEERS_H_

#include <ezbus_platform.h>
#include <ezbus_mac.h>
#include <ezbus_peer.h>
#include <ezbus_crc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ezbus_mac_peers_t
{
    ezbus_peer_t        list[EZBUS_MAX_PEERS];
    uint8_t             count;
} ezbus_mac_peers_t;

extern void  ezbus_mac_peers_init   ( ezbus_mac_t* mac );
extern void  ezbus_mac_peers_deinit ( ezbus_mac_t* mac );
extern void  ezbus_mac_peers_run    ( ezbus_mac_t* mac );
extern void  ezbus_mac_peers_clear  ( ezbus_mac_t* mac );


/**
 * @brief Insert a copy of the peer into the peer list in ascending sorted order,
 *        using ezbus_platform_memcmp() as the sort comparator function.
 * @param peers A pointer to an initialized list of peers.
 * @param peer A pointer to the peer which is to be copied and insorted into the list.
 * @return A pointer to the list copy of the peer or NULL if the operation failed.
 */
extern EZBUS_ERR        ezbus_mac_peers_insort      ( ezbus_mac_t* mac, const ezbus_peer_t* peer );

extern EZBUS_ERR        ezbus_mac_peers_take        ( ezbus_mac_t* mac, int index );
extern ezbus_peer_t*    ezbus_mac_peers_at          ( ezbus_mac_t* mac, int index );
extern int              ezbus_mac_peers_count       ( ezbus_mac_t* mac );
extern int              ezbus_mac_peers_empty       ( ezbus_mac_t* mac );
extern int              ezbus_mac_peers_full        ( ezbus_mac_t* mac );
extern ezbus_peer_t*    ezbus_mac_peers_lookup      ( ezbus_mac_t* mac, const ezbus_address_t* address );
extern int              ezbus_mac_peers_index_of    ( ezbus_mac_t* mac, const ezbus_address_t* address );
extern void             ezbus_mac_peers_clean       ( ezbus_mac_t* mac, uint8_t seq );
extern bool             ezbus_mac_peers_am_dominant ( ezbus_mac_t* mac );

/**
 * @brief locate the peer in the list which follows in sort order from the given address.
 * @return A pointer to the next peer, or NULL.
 */
extern ezbus_address_t* ezbus_mac_peers_next    ( ezbus_mac_t* mac, const ezbus_address_t* address );
extern void             ezbus_mac_peers_dump    ( ezbus_mac_t* mac, const char* prefix );
extern void             ezbus_mac_peers_crc     ( ezbus_mac_t* mac, ezbus_crc_t* crc );
extern void             ezbus_mac_peers_log     ( ezbus_mac_t* mac );


#ifdef __cplusplus
}
#endif

#endif /* EZBUS_MAC_PEERS_H_ */
