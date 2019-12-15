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
#ifndef EZBUS_PEER_LIST_H_
#define EZBUS_PEER_LIST_H_

#include <ezbus_platform.h>
#include <ezbus_peer.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    ezbus_peer_t        list[EZBUS_MAX_PEERS];
    uint8_t             count;
} ezbus_peer_list_t;

extern void             ezbus_peer_list_init    ( ezbus_peer_list_t* peer_list );
extern void             ezbus_peer_list_deinit  ( ezbus_peer_list_t* peer_list );

/**
 * @brief Insert a copy of the peer into the peer list in ascending sorted order,
 *        using ezbus_platform_memcmp() as the sort comparator function.
 * @param peer_list A pointer to an initialized list of peers.
 * @param peer A pointer to the peer which is to be copied and insorted into the list.
 * @return A pointer to the list copy of the peer or NULL if the operation failed.
 */
extern EZBUS_ERR        ezbus_peer_list_insort  ( ezbus_peer_list_t* peer_list, const ezbus_peer_t* peer );

extern EZBUS_ERR        ezbus_peer_list_take    ( ezbus_peer_list_t* peer_list, int index );
extern ezbus_peer_t*    ezbus_peer_list_at      ( ezbus_peer_list_t* peer_list, int index );
extern int              ezbus_peer_list_count   ( ezbus_peer_list_t* peer_list );
extern int              ezbus_peer_list_empty   ( ezbus_peer_list_t* peer_list );
extern int              ezbus_peer_list_full    ( ezbus_peer_list_t* peer_list );
extern ezbus_peer_t*    ezbus_peer_list_lookup  ( ezbus_peer_list_t* peer_list, const ezbus_address_t* address );
extern int              ezbus_peer_list_index_of( ezbus_peer_list_t* peer_list, const ezbus_address_t* address );

/**
 * @brief locate the peer in the list which follows in sort order from the given address.
 * @return A pointer to the next peer, or NULL.
 */
extern ezbus_address_t* ezbus_peer_list_next    ( ezbus_peer_list_t* peer_list, const ezbus_address_t* address );
extern void             ezbus_peer_list_dump    ( ezbus_peer_list_t* peer_list, const char* prefix );

#define ezbus_peer_list_clear(peer_list)  ezbus_peer_list_deinit(peer_list)


#ifdef __cplusplus
}
#endif

#endif /* EZBUS_PEER_LIST_H_ */
