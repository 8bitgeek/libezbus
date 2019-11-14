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
#include <ezbus_driver.h>
#include <ezbus_packet.h>
#include <ezbus_hex.h>


static void 			ezbus_driver_tx_reset     	 ( ezbus_driver_t* driver );
static void 			ezbus_driver_tx_packet     	 ( ezbus_driver_t* driver );
static void 			ezbus_driver_tx_parcel     	 ( ezbus_driver_t* driver, const ezbus_address_t* dst  );
static void 			ezbus_driver_tx_enqueue 	 ( ezbus_driver_t* driver, ezbus_packet_t* tx_packet );
static void 			ezbus_driver_tx_queued       ( ezbus_driver_t* driver );
static void 			ezbus_driver_tx_disco_wait 	 ( ezbus_driver_t* driver );
static void 	 		ezbus_driver_tx_disco_rq	 ( ezbus_driver_t* driver, const ezbus_address_t* dst );
static void 			ezbus_driver_tx_disco_rp	 ( ezbus_driver_t* driver, const ezbus_address_t* dst );
static void 			ezbus_driver_tx_disco_rk	 ( ezbus_driver_t* driver, const ezbus_address_t* dst );
static void 			ezbus_driver_tx_speed	  	 ( ezbus_driver_t* driver );
static void 	 		ezbus_driver_tx_give_token 	 ( ezbus_driver_t* driver, const ezbus_address_t* dst );
static void 	 		ezbus_driver_tx_take_token   ( ezbus_driver_t* driver, const ezbus_address_t* dst );
static void 	 		ezbus_driver_tx_ack		     ( ezbus_driver_t* driver, const ezbus_address_t* dst );

static void 			ezbus_driver_rx            	 ( ezbus_driver_t* driver );
static bool 			ezbus_driver_rx_receivable 	 ( ezbus_driver_t* driver );
static void 			ezbus_driver_rx_disco_rq 	 ( ezbus_driver_t* driver );
static void 			ezbus_driver_rx_disco_rp 	 ( ezbus_driver_t* driver );
static void 			ezbus_driver_rx_disco_rk 	 ( ezbus_driver_t* driver );
static ezbus_peer_t* 	ezbus_driver_rx_src_peer 	 ( ezbus_driver_t* driver, uint8_t seq);
static void 	 		ezbus_driver_rx_give_token 	 ( ezbus_driver_t* driver );
static void 	 		ezbus_driver_rx_take_token 	 ( ezbus_driver_t* driver );
static void 	 		ezbus_driver_rx_ack		  	 ( ezbus_driver_t* driver );
static void 	 		ezbus_driver_rx_nack		 ( ezbus_driver_t* driver );
static void 	 		ezbus_driver_rx_parcel	  	 ( ezbus_driver_t* driver );
static void 	 		ezbus_driver_rx_reset	  	 ( ezbus_driver_t* driver );
static void 	 		ezbus_driver_rx_speed	     ( ezbus_driver_t* driver );


/**
 * @brief The main driver of ezbus flow. Call often.
 * @param driver An initialized ezbus driver struct.
 * @return void
 */
extern void ezbus_driver_run(ezbus_driver_t* driver)
{
	driver->io.rx_state.err = ezbus_port_recv( &driver->io.port, &driver->io.rx_state.packet );
	switch( driver->io.rx_state.err )
	{
		case EZBUS_ERR_NOTREADY:
			/* no data ready */
			break;
		case EZBUS_ERR_TIMEOUT:
			#if EZBUS_driver_DEBUG
				fprintf( stderr, "EZBUS_ERR_TIMEOUT\n" );
			#endif
			/* timeout */
			++driver->io.port.rx_err_timeout_count;
			break;
		case EZBUS_ERR_CRC:
			#if EZBUS_driver_DEBUG
				fprintf( stderr, "EZBUS_ERR_CRC\n" );
			#endif
			/* received packet with CRC error */
			++driver->io.port.rx_err_crc_count;
			break;
		case EZBUS_ERR_OKAY:
			#if EZBUS_driver_DEBUG
				fprintf( stderr, "EZBUS_ERR_OKAY\n" );
				ezbus_hex_dump( "RX:", (uint8_t*)&driver->io.rx_state.packet.header, sizeof(ezbus_header_t) );
			#endif
			{
				/* Application packet processing... */
				if ( driver->rx_callback != NULL )
				{
					driver->rx_callback( &driver->io );
				}
				/* EzBus packet processing... */
				ezbus_driver_rx( driver );
			}
			break;
	}
}

/**
 * Initialize an empty driver structure.
 */
extern void ezbus_driver_init_struct(ezbus_driver_t* driver)
{
	ezbus_platform_memset( driver, 0, sizeof( ezbus_driver_t ) );
	driver->io.rx_state.err = EZBUS_ERR_OKAY;
	driver->io.tx_state.err = EZBUS_ERR_OKAY;
	driver->disco.seq=0xFF;
}

/**
 * @brief Initialize the driver for use.
 * @brief Platform parameters must be populated.
 * @param driver Initialized structure and populated.
 * @param speed Must be one of ezbus_port_speeds[].
 * @param tx_queue_limit Queue size limit number of pending transmit packets.
 */
extern EZBUS_ERR ezbus_driver_init(ezbus_driver_t* driver,uint32_t speed,uint32_t tx_queue_limit)
{
	EZBUS_ERR err = EZBUS_ERR_OKAY;
	
	ezbus_platform_rand_init();

	driver->io.tx_queue = ezbus_packet_queue_init( tx_queue_limit );
	if ( driver->io.tx_queue != NULL )
	{
		err = ezbus_port_open( &driver->io.port, speed );
	}
	else
	{
		err = EZBUS_ERR_MALLOC;
	}
	return err;
}

extern void ezbus_driver_deinit(ezbus_driver_t* driver)
{
	ezbus_port_close( &driver->io.port );
	ezbus_platform_memset( driver, 0, sizeof( ezbus_driver_t ) );
	driver->io.rx_state.err = EZBUS_ERR_OKAY;
	driver->io.tx_state.err = EZBUS_ERR_OKAY;
}

extern void ezbus_driver_set_tx_cb( ezbus_driver_t* driver, ezbus_packet_callback_t rx_callback )
{
	driver->rx_callback = rx_callback;
}


/****************************************************************************
 ****************************** DISCOVER ************************************
 ****************************************************************************/


extern void ezbus_driver_disco( ezbus_driver_t* driver, uint32_t cycles, ezbus_disco_callback_t progress_callback )
{
	#if DISCO_PEER_LIST_DEINIT
		ezbus_peer_list_deinit( &driver->disco.peers );
	#endif
	
	int cycle_count			  = cycles;
	int peer_count            = ezbus_peer_list_count( &driver->disco.peers );
	driver->io.tx_state.err = EZBUS_ERR_OKAY;
	driver->disco.start     = ezbus_platform_get_ms_ticks();

	do
	{
		ezbus_driver_tx_disco_rq( driver, &ezbus_broadcast_address );
		if ( ezbus_peer_list_count( &driver->disco.peers ) == peer_count )
		{
			--cycle_count;
		}
		else
		{
			cycle_count = cycles;
			peer_count = ezbus_peer_list_count( &driver->disco.peers );
		}

		if ( progress_callback )
		{
			if ( !progress_callback( driver ) )
			{
				break;
			}
		}

	} while ( cycle_count > 0 );

}


static void ezbus_driver_tx_disco_rq( ezbus_driver_t* driver, const ezbus_address_t* dst )
{
	ezbus_packet_t* tx_packet = &driver->io.tx_state.packet;
	
	ezbus_packet_set_type( tx_packet, packet_type_disco_rq);
	ezbus_address_copy( ezbus_packet_src( tx_packet ), &driver->io.address );
	ezbus_address_copy( ezbus_packet_dst( tx_packet ), dst );

	tx_packet->data.attachment.disco.features    = DISCO_FEATURES;
	tx_packet->data.attachment.disco.request_seq = ++driver->disco.seq;
	tx_packet->data.attachment.disco.reply_seq   = 0;

	ezbus_driver_tx_packet( driver );
	ezbus_driver_tx_disco_wait( driver );
}

static void ezbus_driver_rx_disco_rq( ezbus_driver_t* driver )
{
	ezbus_peer_t* peer_p=NULL;

	if ( (peer_p=ezbus_driver_rx_src_peer(driver,0)) && ezbus_peer_get_seq( peer_p ) != driver->io.rx_state.packet.header.data.field.seq )
	{
		ezbus_platform_delay( ezbus_platform_random( EZBUS_RAND_LOWER, EZBUS_RAND_UPPER ) );
		ezbus_driver_tx_disco_rp( driver, ezbus_packet_src( &driver->io.rx_state.packet ) );
		
		#if EZBUS_driver_DEBUG
			ezbus_driver_dump( driver );
		#endif
	}
}

static void ezbus_driver_rx_disco_rp( ezbus_driver_t* driver )
{
	ezbus_peer_t* peer_p=NULL;

	#if EZBUS_driver_DEBUG
		fprintf(stderr,"packet_code_rp\n");
	#endif

	if ( (peer_p = ezbus_driver_rx_src_peer( driver, ezbus_packet_seq( &driver->io.rx_state.packet ) )) != NULL )
	{
		ezbus_driver_tx_disco_rk( driver, ezbus_peer_get_address(peer_p) );
	}

	#if EZBUS_driver_DEBUG
		ezbus_driver_dump( driver );
	#endif
}


static void ezbus_driver_rx_disco_rk( ezbus_driver_t* driver )
{
	ezbus_peer_t* peer_p=NULL;

	#if EZBUS_driver_DEBUG
		fprintf(stderr,"packet_code_rk\n");
	#endif

	if ( (peer_p = ezbus_driver_rx_src_peer( driver, ezbus_packet_seq( &driver->io.rx_state.packet ) )) != NULL )
	{
		ezbus_peer_set_seq( peer_p, ezbus_packet_seq( &driver->io.rx_state.packet ) );
	}
}


static void ezbus_driver_tx_disco_rp( ezbus_driver_t* driver, const ezbus_address_t* dst )
{
	ezbus_packet_t* tx_packet = &driver->io.tx_state.packet;
	ezbus_packet_t* rx_packet = &driver->io.rx_state.packet;

	ezbus_packet_set_type( tx_packet, packet_type_disco_rp );
	ezbus_address_copy( ezbus_packet_src( tx_packet ), &driver->io.address );
	ezbus_address_copy( ezbus_packet_dst( tx_packet ), dst );

	tx_packet->data.attachment.disco.features    = DISCO_FEATURES;
	tx_packet->data.attachment.disco.request_seq = rx_packet->data.attachment.disco.request_seq;
	tx_packet->data.attachment.disco.reply_seq   = driver->disco.seq;

	ezbus_driver_tx_packet( driver );
}

static void ezbus_driver_tx_disco_rk( ezbus_driver_t* driver, const ezbus_address_t* dst )
{
	ezbus_packet_t* tx_packet = &driver->io.tx_state.packet;
	ezbus_packet_t* rx_packet = &driver->io.rx_state.packet;
	ezbus_packet_t  tx_packet_save;

	ezbus_packet_copy( &tx_packet_save, tx_packet );
	ezbus_packet_set_type( tx_packet, packet_type_disco_rk );
	ezbus_address_copy( ezbus_packet_src( tx_packet ), &driver->io.address );
	ezbus_address_copy( ezbus_packet_dst( tx_packet ), dst );

	tx_packet->data.attachment.disco.features    = DISCO_FEATURES;
	tx_packet->data.attachment.disco.request_seq = rx_packet->data.attachment.disco.request_seq;
	tx_packet->data.attachment.disco.reply_seq   = 0;

	ezbus_driver_tx_packet( driver );
	ezbus_packet_copy( tx_packet, &tx_packet_save );
}

static void ezbus_driver_tx_disco_wait( ezbus_driver_t* driver )
{
	ezbus_ms_tick_t start = ezbus_platform_get_ms_ticks();
	while ( ezbus_platform_get_ms_ticks() - start < EZBUS_DISCO_PERIOD )
		ezbus_driver_run( driver );
}


static void ezbus_driver_tx_give_token( ezbus_driver_t* driver, const ezbus_address_t* dst )
{
	ezbus_packet_t* tx_packet = &driver->io.tx_state.packet;

	ezbus_packet_set_type( tx_packet, packet_type_give_token );
	ezbus_address_copy( ezbus_packet_src( tx_packet ), &driver->io.address );
	ezbus_address_copy( ezbus_packet_dst( tx_packet ), dst );

	ezbus_driver_tx_enqueue( driver, tx_packet );
}

static void ezbus_driver_tx_take_token( ezbus_driver_t* driver, const ezbus_address_t* dst )
{
	ezbus_packet_t* tx_packet = &driver->io.tx_state.packet;

	ezbus_packet_set_type( tx_packet, packet_type_give_token );
	ezbus_address_copy( ezbus_packet_src( tx_packet ), &driver->io.address );
	ezbus_address_copy( ezbus_packet_dst( tx_packet ), dst );

	ezbus_driver_tx_packet( driver );
	ezbus_driver_tx_queued( driver );
}

static void ezbus_driver_tx_ack( ezbus_driver_t* driver, const ezbus_address_t* dst )
{
}

static void ezbus_driver_tx_parcel( ezbus_driver_t* driver, const ezbus_address_t* dst )
{
}

static void ezbus_driver_tx_reset( ezbus_driver_t* driver )
{
}

static void ezbus_driver_tx_speed( ezbus_driver_t* driver )
{
}

static void ezbus_driver_tx_packet( ezbus_driver_t* driver )
{
	if ( driver->io.tx_state.err == EZBUS_ERR_OKAY )
	{
		driver->io.tx_state.err = ezbus_port_send( &driver->io.port, &driver->io.tx_state.packet );
	}
}

static void ezbus_driver_tx_queued(ezbus_driver_t* driver)
{
	/* FIXME - Handle re-transmit timer, re-queing, etc... */
	for(int index=0; index < ezbus_packet_queue_count(driver->io.tx_queue); index++)
	{
		EZBUS_ERR err = ezbus_packet_queue_can_tx(driver->io.tx_queue,&driver->io.tx_state.packet,index);
		switch( err )
		{
			case EZBUS_ERR_OKAY:
				driver->io.tx_state.err = ezbus_port_send(&driver->io.port,&driver->io.tx_state.packet);
				break;
			case EZBUS_ERR_LIMIT:		/* retry limit exceeded. */
				++driver->io.port.tx_err_retry_fail_count;
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

static void ezbus_driver_tx_enqueue( ezbus_driver_t* driver, ezbus_packet_t* tx_packet )
{
	ezbus_packet_queue_append(driver->io.tx_queue,tx_packet);
}

/*****************************************************************************
 *****************************  RECEIVERS ************************************
 ****************************************************************************/

static ezbus_peer_t* ezbus_driver_rx_src_peer(ezbus_driver_t* driver, uint8_t seq)
{
	ezbus_peer_t peer;
	ezbus_peer_t* peer_p=NULL;

	if ( ( peer_p = ezbus_peer_list_lookup( &driver->disco.peers, &driver->io.rx_state.packet.header.data.field.src ) ) == NULL )
	{
		ezbus_peer_init( &peer, &driver->io.rx_state.packet.header.data.field.src, seq );
		peer_p = ezbus_peer_list_append( &driver->disco.peers, &peer );
	}

	return peer_p;
}


static void ezbus_driver_rx_give_token(ezbus_driver_t* driver)
{
	ezbus_driver_tx_queued( driver );
}

static void ezbus_driver_rx_take_token(ezbus_driver_t* driver)
{
	/* NOTE do something, or no? */
}

static void ezbus_driver_rx_ack(ezbus_driver_t* driver)
{
	int index = ezbus_packet_queue_index_of_seq(driver->io.tx_queue,driver->io.rx_state.packet.header.data.field.seq);
	if ( index >= 0 )
	{
		ezbus_packet_t packet;
		ezbus_packet_queue_take_at( driver->io.tx_queue,&packet,index );
	}
}

static void ezbus_driver_rx_nack(ezbus_driver_t* driver)
{
}

static void ezbus_driver_rx_parcel(ezbus_driver_t* driver)
{
}

static void ezbus_driver_rx_reset(ezbus_driver_t* driver)
{
}

static void ezbus_driver_rx_speed(ezbus_driver_t* driver)
{
}

static bool ezbus_driver_rx_receivable (ezbus_driver_t* driver)
{
	/* rx packet is either addressed to this node, or is a broadcast */
	return ( ezbus_address_compare( ezbus_packet_dst( &driver->io.rx_state.packet ), &driver->io.address ) == 0 ||
		     ezbus_address_compare( ezbus_packet_dst( &driver->io.rx_state.packet ), &ezbus_broadcast_address ) == 0 );
}


static void ezbus_driver_rx(ezbus_driver_t* driver)
{
	/* A valid rx_packet is present? */
	if ( driver->io.rx_state.err == EZBUS_ERR_OKAY )
	{
		if ( ezbus_driver_rx_receivable(driver) )
		{
			switch( ezbus_packet_type( &driver->io.rx_state.packet ) )
			{
				case packet_type_reset		:	ezbus_driver_rx_reset      ( driver );		break;
				case packet_type_disco_rq	:	ezbus_driver_rx_disco_rq   ( driver );		break;
				case packet_type_disco_rp	:	ezbus_driver_rx_disco_rp   ( driver );		break;
				case packet_type_disco_rk	:	ezbus_driver_rx_disco_rk 	 ( driver );		break;
				case packet_type_take_token	:	ezbus_driver_rx_take_token ( driver );		break;
				case packet_type_give_token	:	ezbus_driver_rx_give_token ( driver );		break;
				
				/* Synchronous Data Packets */
				
				case packet_type_parcel		:	ezbus_driver_rx_parcel 	 ( driver );		break;
				case packet_type_speed		:	ezbus_driver_rx_speed 	 ( driver );		break;
				case packet_type_ack		:	ezbus_driver_rx_ack 	 	 ( driver );		break;
				case packet_type_nack		:	ezbus_driver_rx_nack 		 ( driver );		break;
			}
		}
		else
		{
			/* It's not addressed to this peer. */
			driver->io.rx_state.err = EZBUS_ERR_MISMATCH;
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

extern void ezbus_driver_dump( ezbus_driver_t* driver )
{
	ezbus_address_dump     ( &driver->io.address,		"driver->io.address" );
	ezbus_port_dump        ( &driver->io.port,    	"driver->io.port" );
	ezbus_packet_state_dump( &driver->io.rx_state, 	"driver->io.rx_state" );
	ezbus_packet_state_dump( &driver->io.tx_state, 	"driver->io.tx_state" );
	ezbus_packet_queue_dump( driver->io.tx_queue, 	"driver->io.tx_queue" );
	fprintf(stderr, "driver->disco.seq=%d\n", driver->disco.seq );
	ezbus_peer_list_dump( &driver->disco.peers,       "driver->disco.peers" );
	fflush(stderr);
}
