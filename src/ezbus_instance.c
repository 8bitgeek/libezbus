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
#include <ezbus_instance.h>
#include <ezbus_packet.h>
#include <ezbus_hex.h>


static void 			ezbus_instance_tx_reset     	 ( ezbus_instance_t* instance );
static void 			ezbus_instance_tx_packet     	 ( ezbus_instance_t* instance );
static void 			ezbus_instance_tx_parcel     	 ( ezbus_instance_t* instance, const ezbus_address_t* dst  );
static void 			ezbus_instance_tx_enqueue 		 ( ezbus_instance_t* instance, ezbus_packet_t* tx_packet );
static void 			ezbus_instance_tx_queued      	 ( ezbus_instance_t* instance );
static void 			ezbus_instance_tx_disco_wait 	 ( ezbus_instance_t* instance );
static void 	 		ezbus_instance_tx_disco_rq	  	 ( ezbus_instance_t* instance, const ezbus_address_t* dst );
static void 			ezbus_instance_tx_disco_rp		 ( ezbus_instance_t* instance, const ezbus_address_t* dst );
static void 			ezbus_instance_tx_disco_rk		 ( ezbus_instance_t* instance, const ezbus_address_t* dst );
static void 			ezbus_instance_tx_speed	  		 ( ezbus_instance_t* instance );
static void 	 		ezbus_instance_tx_give_token 	 ( ezbus_instance_t* instance, const ezbus_address_t* dst );
static void 	 		ezbus_instance_tx_take_token     ( ezbus_instance_t* instance, const ezbus_address_t* dst );
static void 	 		ezbus_instance_tx_ack		     ( ezbus_instance_t* instance, const ezbus_address_t* dst );

static void 			ezbus_instance_rx            	 ( ezbus_instance_t* instance );
static bool 			ezbus_instance_rx_receivable 	 ( ezbus_instance_t* instance );
static void 			ezbus_instance_rx_disco_rq 	 	 ( ezbus_instance_t* instance );
static void 			ezbus_instance_rx_disco_rp 	     ( ezbus_instance_t* instance );
static void 			ezbus_instance_rx_disco_rk 	 	 ( ezbus_instance_t* instance );
static ezbus_peer_t* 	ezbus_instance_rx_src_peer 		 ( ezbus_instance_t* instance, uint8_t seq);
static void 	 		ezbus_instance_rx_give_token 	 ( ezbus_instance_t* instance );
static void 	 		ezbus_instance_rx_take_token 	 ( ezbus_instance_t* instance );
static void 	 		ezbus_instance_rx_ack		  	 ( ezbus_instance_t* instance );
static void 	 		ezbus_instance_rx_nack		  	 ( ezbus_instance_t* instance );
static void 	 		ezbus_instance_rx_parcel	  	 ( ezbus_instance_t* instance );
static void 	 		ezbus_instance_rx_reset	  		 ( ezbus_instance_t* instance );
static void 	 		ezbus_instance_rx_speed	  		 ( ezbus_instance_t* instance );


/**
 * @brief The main driver of ezbus flow. Call often.
 * @param instance An initialized ezbus instance struct.
 * @return void
 */
extern void ezbus_instance_run(ezbus_instance_t* instance)
{
	instance->io.rx_state.err = ezbus_port_recv(&instance->io.port,&instance->io.rx_state.packet);
	switch( instance->io.rx_state.err )
	{
		case EZBUS_ERR_NOTREADY:
			/* no data ready */
			break;
		case EZBUS_ERR_TIMEOUT:
			#if EZBUS_INSTANCE_DEBUG
				fprintf( stderr, "EZBUS_ERR_TIMEOUT\n" );
			#endif
			/* timeout */
			++instance->io.port.rx_err_timeout_count;
			break;
		case EZBUS_ERR_CRC:
			#if EZBUS_INSTANCE_DEBUG
				fprintf( stderr, "EZBUS_ERR_CRC\n" );
			#endif
			/* received packet with CRC error */
			++instance->io.port.rx_err_crc_count;
			break;
		case EZBUS_ERR_OKAY:
			#if EZBUS_INSTANCE_DEBUG
				fprintf( stderr, "EZBUS_ERR_OKAY\n" );
				ezbus_hex_dump( "RX:", (uint8_t*)&instance->io.rx_state.packet.header, sizeof(ezbus_header_t) );
			#endif
			{
				/* Application packet processing... */
				if ( instance->rx_callback != NULL )
				{
					instance->rx_callback(&instance->io);
				}
				/* EzBus packet processing... */
				ezbus_instance_rx(instance);
			}
			break;
	}
}

/**
 * Initialize an empty instance structure.
 */
extern void ezbus_instance_init_struct(ezbus_instance_t* instance)
{
	ezbus_platform_memset(instance,0,sizeof(ezbus_instance_t));
	instance->io.rx_state.err = EZBUS_ERR_OKAY;
	instance->io.tx_state.err = EZBUS_ERR_OKAY;
	instance->disco.seq=0xFF;
}

/**
 * @brief Initialize the instance for use.
 * @brief Platform parameters must be populated.
 * @param instance Initialized structure and populated.
 * @param speed Must be one of ezbus_port_speeds[].
 * @param tx_queue_limit Queue size limit number of pending transmit packets.
 */
extern EZBUS_ERR ezbus_instance_init(ezbus_instance_t* instance,uint32_t speed,uint32_t tx_queue_limit)
{
	EZBUS_ERR err = EZBUS_ERR_OKAY;
	
	ezbus_platform_rand_init();

	instance->io.tx_queue = ezbus_packet_queue_init(tx_queue_limit);
	if ( instance->io.tx_queue != NULL )
	{
		err = ezbus_port_open(&instance->io.port,speed);
	}
	else
	{
		err = EZBUS_ERR_MALLOC;
	}
	return err;
}

extern void ezbus_instance_deinit(ezbus_instance_t* instance)
{
	ezbus_port_close(&instance->io.port);
	ezbus_platform_memset(instance,0,sizeof(ezbus_instance_t));
	instance->io.rx_state.err = EZBUS_ERR_OKAY;
	instance->io.tx_state.err = EZBUS_ERR_OKAY;
}

extern void ezbus_instance_set_tx_cb( ezbus_instance_t* instance, ezbus_packet_callback_t rx_callback )
{
	instance->rx_callback = rx_callback;
}


/****************************************************************************
 ****************************** DISCOVER ************************************
 ****************************************************************************/


extern void ezbus_instance_disco( ezbus_instance_t* instance, uint32_t cycles, ezbus_disco_callback_t progress_callback )
{
	#if DISCO_PEER_LIST_DEINIT
		ezbus_peer_list_deinit( &instance->disco.peers );
	#endif
	
	int cycle_count			  = cycles;
	int peer_count            = ezbus_peer_list_count( &instance->disco.peers );
	instance->io.tx_state.err = EZBUS_ERR_OKAY;
	instance->disco.start     = ezbus_platform_get_ms_ticks();

	do
	{
		ezbus_instance_tx_disco_rq( instance, &ezbus_broadcast_address );
		if ( ezbus_peer_list_count( &instance->disco.peers ) == peer_count )
		{
			--cycle_count;
		}
		else
		{
			cycle_count = cycles;
			peer_count = ezbus_peer_list_count( &instance->disco.peers );
		}

		if ( progress_callback )
		{
			if ( !progress_callback( instance ) )
			{
				break;
			}
		}

	} while ( cycle_count > 0 );

}


static void ezbus_instance_tx_disco_rq( ezbus_instance_t* instance, const ezbus_address_t* dst )
{
	ezbus_packet_t* tx_packet = &instance->io.tx_state.packet;
	
	tx_packet->header.data.field.mark = EZBUS_MARK;
	tx_packet->header.data.field.type = packet_type_disco_rq;
	ezbus_address_copy(&tx_packet->header.data.field.src,&instance->io.address);
	ezbus_address_copy(&tx_packet->header.data.field.dst,dst);

	tx_packet->data.attachment.disco.features    = DISCO_FEATURES;
	tx_packet->data.attachment.disco.request_seq = ++instance->disco.seq;
	tx_packet->data.attachment.disco.reply_seq   = 0;

	ezbus_instance_tx_packet(instance);
	ezbus_instance_tx_disco_wait(instance);
}

static void ezbus_instance_rx_disco_rq( ezbus_instance_t* instance )
{
	ezbus_peer_t* peer_p=NULL;

	if ( (peer_p=ezbus_instance_rx_src_peer(instance,0)) && ezbus_peer_get_seq( peer_p ) != instance->io.rx_state.packet.header.data.field.seq )
	{
		ezbus_platform_delay( ezbus_platform_random( EZBUS_RAND_LOWER, EZBUS_RAND_UPPER ) );
		ezbus_instance_tx_disco_rp( instance, &instance->io.rx_state.packet.header.data.field.src );
		
		#if EZBUS_INSTANCE_DEBUG
			ezbus_instance_dump( instance );
		#endif
	}
}

static void ezbus_instance_rx_disco_rp(ezbus_instance_t* instance)
{
	ezbus_peer_t* peer_p=NULL;

	#if EZBUS_INSTANCE_DEBUG
		fprintf(stderr,"packet_code_rp\n");
	#endif

	if ( (peer_p = ezbus_instance_rx_src_peer( instance, instance->io.rx_state.packet.header.data.field.seq )) != NULL )
	{
		ezbus_instance_tx_disco_rk( instance, ezbus_peer_get_address(peer_p) );
	}

	#if EZBUS_INSTANCE_DEBUG
		ezbus_instance_dump( instance );
	#endif
}


static void ezbus_instance_rx_disco_rk(ezbus_instance_t* instance)
{
	ezbus_peer_t* peer_p=NULL;

	#if EZBUS_INSTANCE_DEBUG
		fprintf(stderr,"packet_code_rk\n");
	#endif

	if ( (peer_p = ezbus_instance_rx_src_peer( instance, instance->io.rx_state.packet.header.data.field.seq )) != NULL )
	{
		ezbus_peer_set_seq( peer_p, instance->io.rx_state.packet.header.data.field.seq );
	}
}


static void ezbus_instance_tx_disco_rp( ezbus_instance_t* instance, const ezbus_address_t* dst )
{
	ezbus_packet_t* tx_packet = &instance->io.tx_state.packet;
	ezbus_packet_t* rx_packet = &instance->io.rx_state.packet;

	tx_packet->header.data.field.mark = EZBUS_MARK;
	tx_packet->header.data.field.type = packet_type_disco_rp;
	ezbus_address_copy(&tx_packet->header.data.field.src,&instance->io.address);
	ezbus_address_copy(&tx_packet->header.data.field.dst,dst);

	tx_packet->data.attachment.disco.features    = DISCO_FEATURES;
	tx_packet->data.attachment.disco.request_seq = rx_packet->data.attachment.disco.request_seq;
	tx_packet->data.attachment.disco.reply_seq   = instance->disco.seq;

	ezbus_instance_tx_packet(instance);
}

static void ezbus_instance_tx_disco_rk( ezbus_instance_t* instance, const ezbus_address_t* dst )
{
	ezbus_packet_t* tx_packet = &instance->io.tx_state.packet;
	ezbus_packet_t* rx_packet = &instance->io.rx_state.packet;
	ezbus_packet_t  tx_packet_save;

	memcpy(&tx_packet_save,tx_packet,sizeof(ezbus_packet_t));

	tx_packet->header.data.field.mark = EZBUS_MARK;
	tx_packet->header.data.field.type = packet_type_disco_rk;
	ezbus_address_copy(&tx_packet->header.data.field.src,&instance->io.address);
	ezbus_address_copy(&tx_packet->header.data.field.dst,dst);

	tx_packet->data.attachment.disco.features    = DISCO_FEATURES;
	tx_packet->data.attachment.disco.request_seq = rx_packet->data.attachment.disco.request_seq;
	tx_packet->data.attachment.disco.reply_seq   = 0;

	ezbus_instance_tx_packet(instance);
	memcpy(tx_packet,&tx_packet_save,sizeof(ezbus_packet_t));
}

static void ezbus_instance_tx_disco_wait(ezbus_instance_t* instance)
{
	ezbus_ms_tick_t start = ezbus_platform_get_ms_ticks();
	while ( ezbus_platform_get_ms_ticks() - start < EZBUS_DISCO_PERIOD )
		ezbus_instance_run(instance);
}




static void ezbus_instance_tx_give_token(ezbus_instance_t* instance, const ezbus_address_t* dst)
{
	ezbus_packet_t* tx_packet = &instance->io.tx_state.packet;

	tx_packet->header.data.field.mark = EZBUS_MARK;
	tx_packet->header.data.field.type = packet_type_give_token;
	ezbus_address_copy(&tx_packet->header.data.field.src,&instance->io.address);
	ezbus_address_copy(&tx_packet->header.data.field.dst,dst);
	
	ezbus_instance_tx_enqueue(instance,tx_packet);
}

static void ezbus_instance_tx_take_token(ezbus_instance_t* instance, const ezbus_address_t* dst)
{
	ezbus_packet_t* tx_packet = &instance->io.tx_state.packet;

	tx_packet->header.data.field.mark = EZBUS_MARK;
	tx_packet->header.data.field.type = packet_type_give_token;
	ezbus_address_copy(&tx_packet->header.data.field.src,&instance->io.address);
	ezbus_address_copy(&tx_packet->header.data.field.dst,dst);

	ezbus_instance_tx_packet(instance);
	ezbus_instance_tx_queued(instance);
}

static void ezbus_instance_tx_ack(ezbus_instance_t* instance, const ezbus_address_t* dst)
{
}

static void ezbus_instance_tx_parcel(ezbus_instance_t* instance, const ezbus_address_t* dst)
{
}

static void ezbus_instance_tx_reset( ezbus_instance_t* instance )
{
}

static void ezbus_instance_tx_speed( ezbus_instance_t* instance )
{
}

static void ezbus_instance_tx_packet(ezbus_instance_t* instance)
{
	if ( instance->io.tx_state.err == EZBUS_ERR_OKAY )
	{
		instance->io.tx_state.err = ezbus_port_send( &instance->io.port, &instance->io.tx_state.packet );
	}
}

static void ezbus_instance_tx_queued(ezbus_instance_t* instance)
{
	/* FIXME - Handle re-transmit timer, re-queing, etc... */
	for(int index=0; index < ezbus_packet_queue_count(instance->io.tx_queue); index++)
	{
		EZBUS_ERR err = ezbus_packet_queue_can_tx(instance->io.tx_queue,&instance->io.tx_state.packet,index);
		switch( err )
		{
			case EZBUS_ERR_OKAY:
				instance->io.tx_state.err = ezbus_port_send(&instance->io.port,&instance->io.tx_state.packet);
				break;
			case EZBUS_ERR_LIMIT:		/* retry limit exceeded. */
				++instance->io.port.tx_err_retry_fail_count;
				break;
			case EZBUS_ERR_NOTREADY:	/* Not time yet */
				break;
			case EZBUS_ERR_RANGE:		/* out of range index */
				break;
			default:
				break;
		}
	}
}

static void ezbus_instance_tx_enqueue( ezbus_instance_t* instance, ezbus_packet_t* tx_packet )
{
	ezbus_packet_queue_append(instance->io.tx_queue,tx_packet);
}

/*****************************************************************************
 *****************************  RECEIVERS ************************************
 ****************************************************************************/

static ezbus_peer_t* ezbus_instance_rx_src_peer(ezbus_instance_t* instance, uint8_t seq)
{
	ezbus_peer_t peer;
	ezbus_peer_t* peer_p=NULL;

	if ( ( peer_p = ezbus_peer_list_lookup( &instance->disco.peers, &instance->io.rx_state.packet.header.data.field.src ) ) == NULL )
	{
		ezbus_peer_init( &peer, &instance->io.rx_state.packet.header.data.field.src, seq );
		peer_p = ezbus_peer_list_append( &instance->disco.peers, &peer );
	}

	return peer_p;
}


static void ezbus_instance_rx_give_token(ezbus_instance_t* instance)
{
	ezbus_instance_tx_queued(instance);
}

static void ezbus_instance_rx_take_token(ezbus_instance_t* instance)
{
	/* NOTE do something, or no? */
}

static void ezbus_instance_rx_ack(ezbus_instance_t* instance)
{
	int index = ezbus_packet_queue_index_of_seq(instance->io.tx_queue,instance->io.rx_state.packet.header.data.field.seq);
	if ( index >= 0 )
	{
		ezbus_packet_t packet;
		ezbus_packet_queue_take_at(instance->io.tx_queue,&packet,index);
	}
}

static void ezbus_instance_rx_nack(ezbus_instance_t* instance)
{
}

static void ezbus_instance_rx_parcel(ezbus_instance_t* instance)
{
}

static void ezbus_instance_rx_reset(ezbus_instance_t* instance)
{
}

static void ezbus_instance_rx_speed(ezbus_instance_t* instance)
{
}

static bool ezbus_instance_rx_receivable (ezbus_instance_t* instance)
{
	/* rx packet is either addressed to this node, or is a broadcast */
	return ( ezbus_address_compare( ezbus_packet_dst( &instance->io.rx_state.packet ), &instance->io.address ) == 0 ||
		     ezbus_address_compare( ezbus_packet_dst( &instance->io.rx_state.packet ), &ezbus_broadcast_address ) == 0 );
}


static void ezbus_instance_rx(ezbus_instance_t* instance)
{
	/* A valid rx_packet is present? */
	if ( instance->io.rx_state.err == EZBUS_ERR_OKAY )
	{
		if ( ezbus_instance_rx_receivable(instance) )
		{
			switch( ezbus_packet_type( &instance->io.rx_state.packet ) )
			{
				case packet_type_reset		:	ezbus_instance_rx_reset      ( instance );		break;
				case packet_type_disco_rq	:	ezbus_instance_rx_disco_rq   ( instance );		break;
				case packet_type_disco_rp	:	ezbus_instance_rx_disco_rp   ( instance );		break;
				case packet_type_disco_rk	:	ezbus_instance_rx_disco_rk 	 ( instance );		break;
				case packet_type_take_token	:	ezbus_instance_rx_take_token ( instance );		break;
				case packet_type_give_token	:	ezbus_instance_rx_give_token ( instance );		break;
				
				/* Synchronous Data Packets */
				
				case packet_type_parcel		:	ezbus_instance_rx_parcel 	 ( instance );		break;
				case packet_type_speed		:	ezbus_instance_rx_speed 	 ( instance );		break;
				case packet_type_ack		:	ezbus_instance_rx_ack 	 	 ( instance );		break;
				case packet_type_nack		:	ezbus_instance_rx_nack 		 ( instance );		break;
			}
		}
		else
		{
			/* It's not addressed to this peer. */
			instance->io.rx_state.err = EZBUS_ERR_MISMATCH;
		}
	}
}

static void ezbus_packet_state_dump( ezbus_packet_state_t* packet_state, const char* prefix )
{
	char print_buffer[EZBUS_TMP_BUF_SZ];
	
	sprintf( print_buffer, "%s.packet", prefix );
	ezbus_packet_dump ( &packet_state->packet, print_buffer );

	fprintf(stderr, "%s.err=%d\n", prefix, packet_state->err );
	fprintf(stderr, "%s.seq=%d\n", prefix, packet_state->seq );
}

extern void ezbus_instance_dump( ezbus_instance_t* instance )
{
	ezbus_address_dump     ( &instance->io.address,		"instance->io.address" );
	ezbus_port_dump        ( &instance->io.port,    	"instance->io.port" );
	ezbus_packet_state_dump( &instance->io.rx_state, 	"instance->io.rx_state" );
	ezbus_packet_state_dump( &instance->io.tx_state, 	"instance->io.tx_state" );
	ezbus_packet_queue_dump( instance->io.tx_queue, 	"instance->io.tx_queue" );
	fprintf(stderr, "instance->disco.seq=%d\n", instance->disco.seq );
	ezbus_peer_list_dump( &instance->disco.peers,       "instance->disco.peers" );
	fflush(stderr);
}
