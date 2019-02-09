/******************************************************************************
 * Copyright © 2018 by @author Mike Sharkey <mike@8bitgeek.net>
 * All Rights Reserved
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
	ezbus_packet_t				packet;		/* An ezbus packet */
	ezbus_ms_tick_t				timestamp;	/* Timestamp last touched (age) */
	uint32_t					retries;	/* Retry count */
} ezbus_packet_queue_item_t;

typedef struct
{
	ezbus_packet_queue_item_t**	items;		/* FIFO list of packets. */
	uint32_t					count;		/* How many are there. */
	uint32_t					limit;		/* How many can be there */
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

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_PACKET_QUEUE_H_ */
