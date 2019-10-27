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
#include "ezbus_packet_queue.h"

/**
 * @brief Allocate a FIFO queue for queueing packets.
 * @param size A limit on the number of packets the queue can hold.
 */
ezbus_packet_queue_t* ezbus_packet_queue_init(uint32_t limit)
{
	ezbus_packet_queue_t* queue=NULL;
	if ( limit > 0 )
	{
		queue = (ezbus_packet_queue_t*)ezbus_platform_malloc(sizeof(ezbus_packet_queue_t*)*limit);
		if ( queue )
		{
			ezbus_platform_memset(queue,0,sizeof(ezbus_packet_queue_t));
		}
	}
	return queue;
}

/**
 * @brief De-initialize a previously initialized queue.
 * @param queue Psreviously initialized queue pointer.
 */
void ezbus_packet_queue_deinit(ezbus_packet_queue_t* queue)
{
	if ( queue )
	{
		ezbus_packet_t packet;
		while(ezbus_packet_queue_count(queue)>0)
			ezbus_packet_queue_take_last(queue,&packet);
		ezbus_platform_free(queue);
		ezbus_platform_memset(queue,0,sizeof(ezbus_packet_queue_t));
	}
}

/**
 * @Brief Takes a copy of packet and places on the queue.
 * @param queue The initialized packet queue.
 * @param packet Source packet that will be copied into the queue.
 * @return < 0 on failure, 0 on success.
 */
EZBUS_ERR ezbus_packet_queue_append(ezbus_packet_queue_t* queue,const ezbus_packet_t* packet)
{
	EZBUS_ERR err=EZBUS_ERR_OKAY;
	if ( queue != NULL )
	{
		if ( !ezbus_packet_queue_full(queue) )
		{
			ezbus_packet_queue_item_t** items = queue->items;
			queue->items = (ezbus_packet_queue_item_t**)ezbus_platform_realloc(queue->items,sizeof(ezbus_packet_queue_item_t**)*(queue->count+1));
			if ( queue->items != NULL )
			{
				ezbus_packet_queue_item_t* titem = ezbus_platform_malloc(sizeof(ezbus_packet_queue_item_t));
				if ( titem != NULL )
				{
					ezbus_platform_memset(titem,0,sizeof(ezbus_packet_queue_item_t));
					/* Take a copy of the packet and increment the count */
					ezbus_platform_memcpy(&titem->packet,packet,sizeof(ezbus_packet_t));
					queue->items[queue->count] = titem;
					err = ezbus_packet_queue_touch_at(queue,queue->count++);
				}
				else
				{
					/* Remove the list allocation */
					queue->items = (ezbus_packet_queue_item_t**)ezbus_platform_realloc(queue->items,sizeof(ezbus_packet_queue_item_t**)*(queue->count));
					err = EZBUS_ERR_MALLOC;
				}
			}
			else
			{
				/* realloc() failed, let's don't leak */
				queue->items = items;
				err = EZBUS_ERR_MALLOC;
			}
		}
		else
		{
			err = EZBUS_ERR_LIMIT;
		}
	}
	else
	{
		err = EZBUS_ERR_PARAM;
	}
	return err;
}

/**
 * @brief Take a copy of a packet from the first in queue and remove from the queue.
 * @param queue The initialized packet queue.
 * @param packet Source packet that will be copied into the queue.
 * @return < 0 on failure, 0 on success.
 */
EZBUS_ERR ezbus_packet_queue_take_first(ezbus_packet_queue_t* queue,ezbus_packet_t* packet)
{
	EZBUS_ERR err=EZBUS_ERR_OKAY;
	if ( queue != NULL )
	{
		if ( !ezbus_packet_queue_empty(queue) )
		{
			ezbus_packet_queue_item_t** items = queue->items;
			ezbus_packet_queue_item_t* titem = items[0];
			ezbus_platform_memmove(&items[0],&items[queue->count],queue->count);
			--queue->count;
			ezbus_platform_memcpy(packet,&titem->packet,sizeof(ezbus_packet_t));
			ezbus_platform_free(titem);
			queue->items = (ezbus_packet_queue_item_t**)ezbus_platform_realloc(queue->items,sizeof(ezbus_packet_queue_item_t**)*(queue->count));
			if ( queue->items != NULL )
			{
				err=EZBUS_ERR_OKAY;
			}
			else
			{
				/* realloc() failed, let's don't leak */
				queue->items = items;
				err = EZBUS_ERR_MALLOC;
			}
		}
		else
		{
			err = EZBUS_ERR_LIMIT;
		}
	}
	else
	{
		err = EZBUS_ERR_PARAM;
	}
	return err;
}

/**
 * @brief Take a copy of a packet from the last in queue and remove from the queue.
 * @param queue The initialized packet queue.
 * @param packet Source packet that will be copied into the queue.
 * @return < 0 on failure, 0 on success.
 */
EZBUS_ERR ezbus_packet_queue_take_last(ezbus_packet_queue_t* queue,ezbus_packet_t* packet)
{
	EZBUS_ERR err=EZBUS_ERR_OKAY;
	if ( queue != NULL )
	{
		if ( !ezbus_packet_queue_empty(queue) )
		{
			ezbus_packet_queue_item_t** items = queue->items;
			ezbus_packet_queue_item_t* titem = items[--queue->count];
			ezbus_platform_memcpy(packet,&titem->packet,sizeof(ezbus_packet_t));
			ezbus_platform_free(titem);
			queue->items = (ezbus_packet_queue_item_t**)ezbus_platform_realloc(queue->items,sizeof(ezbus_packet_queue_item_t**)*(queue->count));
			if ( queue->items != NULL )
			{
				err=EZBUS_ERR_OKAY;
			}
			else
			{
				/* realloc() failed, let's don't leak */
				queue->items = items;
				err = EZBUS_ERR_MALLOC;
			}
		}
		else
		{
			err = EZBUS_ERR_LIMIT;
		}
	}
	else
	{
		err = EZBUS_ERR_PARAM;
	}
	return err;
}

/**
 * @brief Take a copy of a packet from the index position in queue and remove from the queue.
 * @param queue The initialized packet queue.
 * @param packet Source packet that will be copied into the queue.
 * @param index The index position of the packet to take.
 * @return < 0 on failure, 0 on success.
 */
EZBUS_ERR ezbus_packet_queue_take_at ( ezbus_packet_queue_t* queue, ezbus_packet_t* packet, int index )
{
	EZBUS_ERR err=EZBUS_ERR_OKAY;
	if ( queue != NULL )
	{
		if ( !ezbus_packet_queue_empty(queue) )
		{
			ezbus_packet_queue_item_t** items = queue->items;
			ezbus_packet_queue_item_t* titem = items[index];
			ezbus_platform_memmove(&items[index],&items[queue->count],queue->count);
			--queue->count;
			ezbus_platform_memcpy(packet,&titem->packet,sizeof(ezbus_packet_t));
			ezbus_platform_free(titem);
			queue->items = (ezbus_packet_queue_item_t**)ezbus_platform_realloc(queue->items,sizeof(ezbus_packet_queue_item_t**)*(queue->count));
			if ( queue->items != NULL )
			{
				err=EZBUS_ERR_OKAY;
			}
			else
			{
				/* realloc() failed, let's don't leak */
				queue->items = items;
				err = EZBUS_ERR_MALLOC;
			}
		}
		else
		{
			err = EZBUS_ERR_LIMIT;
		}
	}
	else
	{
		err = EZBUS_ERR_PARAM;
	}
	return err;
}

/**
 * @brief In this case, we want to touch the timestamp and increment the retries.
 * @param queue The initialized packet queue.
 * @param index The index position of the packet to touch.
 */
EZBUS_ERR ezbus_packet_queue_touch_at( ezbus_packet_queue_t* queue, int index )
{
	EZBUS_ERR err = EZBUS_ERR_OKAY;
	if ( queue != NULL )
	{
		if ( index >= 0 && index < queue->count )
		{
			ezbus_packet_queue_item_t* item = queue->items[index];
			item->timestamp = ezbus_platform_get_ms_ticks();
			err = EZBUS_ERR_OKAY;
		}
		else
		{
			err = EZBUS_ERR_RANGE;
		}
	}
	else
	{
		err = EZBUS_ERR_PARAM;
	}
	return err;
}

ezbus_ms_tick_t ezbus_packet_queue_age_at( ezbus_packet_queue_t* queue, int index )
{
	ezbus_ms_tick_t age = 0;
	if ( queue != NULL )
	{
		if ( index >= 0 && index < queue->count )
		{
			ezbus_packet_queue_item_t* item = queue->items[index];
			age = ezbus_platform_get_ms_ticks() - item->timestamp;
		}
	}
	return age;
}

int	ezbus_packet_queue_count(ezbus_packet_queue_t* queue)
{
	return queue->count;
}

int	ezbus_packet_queue_limit(ezbus_packet_queue_t* queue)
{
	return queue->limit;
}

/**
 * @return 1 if queue is full, else 0
 */
int ezbus_packet_queue_full(ezbus_packet_queue_t* queue)
{
	return (queue->count < queue->limit);
}

int	ezbus_packet_queue_empty(ezbus_packet_queue_t* queue)
{
	return (queue->count==0);
}

/**
 * @brief Determine if a packet can be transmitted at this time. Taking into account
 * the age of the packet and number of re-transmissions.
 * @return EZBUS_ERR_OKAY if packet may be transmitted now, and packet is populated, retries is incremented
 */
EZBUS_ERR ezbus_packet_queue_can_tx( ezbus_packet_queue_t* queue, ezbus_packet_t* packet, int index)
{
	EZBUS_ERR err = EZBUS_ERR_OKAY;
	if ( index >= 0 && index <= queue->count )
	{
		ezbus_packet_queue_item_t* item = queue->items[index];
		if ( item->retries == 0 || ezbus_platform_get_ms_ticks() - item->timestamp > EZBUS_RETRANSMIT_TO )
		{
			if ( ++item->retries <= EZBUS_RETRANSMIT_TRIES )
			{
				ezbus_platform_memcpy(packet,&item->packet,sizeof(ezbus_packet_t));
				err = EZBUS_ERR_OKAY;
			}
			else
			{
				err = EZBUS_ERR_LIMIT;	/* retry limit exceeded. */
			}
		}
		else
		{
			err = EZBUS_ERR_NOTREADY;	/* Not time yet */
		}

	}
	else
	{
		err = EZBUS_ERR_RANGE;
	}
	return err;
}

int ezbus_packet_queue_index_of_seq( ezbus_packet_queue_t* queue, uint8_t seq )
{
	for(int index=0; index < queue->count; index++)
	{
		ezbus_packet_queue_item_t* item = queue->items[index];
		if ( item->packet.header.data.field.seq == seq )
		{
			return index;
		}
	}
	return -1;
}


extern void ezbus_packet_queue_dump( ezbus_packet_queue_t* queue, const char* prefix)
{
	char print_buffer[EZBUS_TMP_BUF_SZ];

	printf( "%s.count=%d\n", prefix, queue->count );
	printf( "%s.limit=%d\n", prefix, queue->limit );

	for(int index=0; index < queue->count; index++)
	{
		ezbus_packet_queue_item_t* item = queue->items[index];

		printf( "%s.items[%d].retries=%d\n",        prefix, index, item->retries );
		printf( "%s.items[%d].timestamp=%04X%04X\n",prefix, index, (uint32_t)(item->timestamp>>32), (uint32_t)(item->timestamp&0xFFFFFFFF) );

		sprintf( print_buffer, "%s.items[%d].packet", prefix, index );
		ezbus_packet_dump( &item->packet, print_buffer );
	}
}

