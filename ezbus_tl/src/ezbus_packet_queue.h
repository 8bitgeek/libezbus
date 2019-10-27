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
#ifndef EZBUS_PACKET_QUEUE_H_
#define EZBUS_PACKET_QUEUE_H_

#include "ezbus_platform.h"
#include "ezbus_packet.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	uint32_t					retries;	/* Retry count */
	ezbus_ms_tick_t				timestamp;	/* Timestamp last touched (age) */
	ezbus_packet_t				packet;		/* An ezbus packet */
} ezbus_packet_queue_item_t;

typedef struct
{
	uint32_t					count;		/* How many are there. */
	uint32_t					limit;		/* How many can be there */
	ezbus_packet_queue_item_t**	items;		/* FIFO list of packets. */
} ezbus_packet_queue_t;

extern ezbus_packet_queue_t* ezbus_packet_queue_init(uint32_t size);
extern void ezbus_packet_queue_deinit			( ezbus_packet_queue_t* queue );
extern EZBUS_ERR ezbus_packet_queue_append		( ezbus_packet_queue_t* queue, const ezbus_packet_t* packet );
extern EZBUS_ERR ezbus_packet_queue_take_last	( ezbus_packet_queue_t* queue, ezbus_packet_t* packet );
extern EZBUS_ERR ezbus_packet_queue_take_first	( ezbus_packet_queue_t* queue, ezbus_packet_t* packet );
extern EZBUS_ERR ezbus_packet_queue_take_at		( ezbus_packet_queue_t* queue, ezbus_packet_t* packet, int index );
extern EZBUS_ERR ezbus_packet_queue_touch_at	( ezbus_packet_queue_t* queue, int index );
extern ezbus_ms_tick_t ezbus_packet_queue_age_at( ezbus_packet_queue_t* queue, int index );
extern EZBUS_ERR ezbus_packet_queue_can_tx		( ezbus_packet_queue_t* queue, ezbus_packet_t* packet, int index );
extern int	ezbus_packet_queue_index_of_seq		( ezbus_packet_queue_t* queue, uint8_t seq );
extern int	ezbus_packet_queue_count			( ezbus_packet_queue_t* queue );
extern int	ezbus_packet_queue_limit			( ezbus_packet_queue_t* queue );
extern int	ezbus_packet_queue_full				( ezbus_packet_queue_t* queue );
extern int	ezbus_packet_queue_empty			( ezbus_packet_queue_t* queue );
extern void ezbus_packet_queue_dump             ( ezbus_packet_queue_t* queue, const char* prefix);

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_PACKET_QUEUE_H_ */
