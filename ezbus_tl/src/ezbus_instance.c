/******************************************************************************
 * Copyright © 2018 by @author Mike Sharkey <mike@8bitgeek.net>
 * All Rights Reserved
 *****************************************************************************/
#include "ezbus_instance.h"
#include "ezbus_packet.h"

static void ezbus_instance_rx(ezbus_instance_t* instance);
static void ezbus_instance_tx_packet(ezbus_instance_t* instance);
static void ezbus_instance_tx_queue(ezbus_instance_t* instance);

/**
 * @brief The main driver of ezbus flow. Call often.
 * @param instance An initialized ezbus instance struct.
 * @return void
 */
void ezbus_instance_run(ezbus_instance_t* instance)
{
	instance->io.rx_state.err = ezbus_port_recv(&instance->io.port,&instance->io.rx_state.packet);
	switch( instance->io.rx_state.err )
	{
		case EZBUS_ERR_NOTREADY:
			/* no data ready */
			break;
		case EZBUS_ERR_TIMEOUT:
			/* timeout */
			++instance->io.port.rx_err_timeout_count;
			break;
		case EZBUS_ERR_CRC:
			/* received packet with CRC error */
			++instance->io.port.rx_err_crc_count;
			break;
		case EZBUS_ERR_OKAY:
			{
				/* Application packet processing... */
				if ( instance->rx_callback != NULL )
				{
					instance->rx_callback(&instance->io);
				}
				/* EzBus packet processing... */
				ezbus_instance_rx(&instance->io);
			}
			break;
	}
}

/**
 * Initialize an empty instance structure.
 */
void ezbus_instance_init_struct(ezbus_instance_t* instance)
{
	ezbus_platform_memset(instance,0,sizeof(ezbus_instance_t));
	instance->io.rx_state.err = EZBUS_ERR_NOTREADY;
	instance->io.tx_state.err = EZBUS_ERR_NOTREADY;
}

/**
 * @brief Initialize the instance for use.
 * @brief Platform parameters must be populated.
 * @param instance Initialized structure and populated.
 * @param speed Must be one of ezbus_port_speeds[].
 * @param tx_queue_limit Queue size limit number of pending transmit packets.
 */
EZBUS_ERR ezbus_instance_init(ezbus_instance_t* instance,uint32_t speed,uint32_t tx_queue_limit)
{
	EZBUS_ERR err = EZBUS_ERR_OKAY;
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

void ezbus_instance_deinit(ezbus_instance_t* instance)
{
	ezbus_port_close(&instance->io.port);
	ezbus_platform_memset(instance,0,sizeof(ezbus_instance_t));
	instance->io.rx_state.err = EZBUS_ERR_NOTREADY;
	instance->io.tx_state.err = EZBUS_ERR_NOTREADY;
}

/*****************************************************************************
 ****************************** TRANSMITTERS *********************************
 ****************************************************************************/

static void ezbus_instance_tx_disco_once(ezbus_instance_t* instance)
{
	ezbus_instance_tx_packet(instance);
	ezbus_ms_tick_t start = ezbus_platform_get_ms_ticks();
	while ( ezbus_platform_get_ms_ticks() - start < EZBUS_DISCO_PERIOD )
		ezbus_instance_run(instance);
}


void ezbus_instance_tx_disco(ezbus_instance_t* instance, ezbus_address_t dst, uint8_t seq, ezbus_packet_code_t code)
{
	int peer_count = 0;
	int disco_count = EZBUS_DISCO_COUNT;
	ezbus_packet_t* tx_packet = &instance->io.tx_state.packet;
	ezbus_address_list_deinit(&instance->io.peers);
	tx_packet->header.data.field.mark = EZBUS_MARK;
	tx_packet->header.data.field.type = packet_type_disco;
	tx_packet->header.data.field.size_code = code;
	tx_packet->header.data.field.seq = seq;
	ezbus_platform_address(tx_packet->header.data.field.src);
	ezbus_platform_memcpy(tx_packet->header.data.field.dst,dst,EZBUS_ADDR_LN);
	tx_packet->header.crc.word = ezbus_packet_calc_crc(tx_packet);
	do
	{
		ezbus_instance_tx_disco_once(instance);
		if ( ezbus_address_list_count(&instance->io.peers) == peer_count )
		{
			--disco_count;
		}
		else
		{
			disco_count = EZBUS_DISCO_COUNT;
			peer_count = ezbus_address_list_count(&instance->io.peers);
		}
	} while (disco_count>0);
}

void ezbus_instance_tx_give_token(ezbus_instance_t* instance, ezbus_address_t dst)
{
	ezbus_packet_t* tx_packet = &instance->io.tx_state.packet;
	ezbus_address_list_deinit(&instance->io.peers);
	tx_packet->header.data.field.mark = EZBUS_MARK;
	tx_packet->header.data.field.type = packet_type_give_token;
	tx_packet->header.data.field.size_code = 0;
	tx_packet->header.data.field.seq = ++instance->io.tx_state.seq;
	ezbus_platform_address(tx_packet->header.data.field.src);
	ezbus_platform_memcpy(tx_packet->header.data.field.dst,dst,EZBUS_ADDR_LN);
	tx_packet->header.crc.word = ezbus_packet_calc_crc(tx_packet);
	ezbus_packet_queue_append(instance->io.tx_queue,tx_packet);
}

void ezbus_instance_tx_take_token(ezbus_instance_t* instance, ezbus_address_t dst)
{
	ezbus_packet_t* tx_packet = &instance->io.tx_state.packet;
	ezbus_address_list_deinit(&instance->io.peers);
	tx_packet->header.data.field.mark = EZBUS_MARK;
	tx_packet->header.data.field.type = packet_type_give_token;
	tx_packet->header.data.field.size_code = 0;
	tx_packet->header.data.field.seq = ++instance->io.tx_state.seq;
	ezbus_platform_address(tx_packet->header.data.field.src);
	ezbus_platform_memcpy(tx_packet->header.data.field.dst,dst,EZBUS_ADDR_LN);
	tx_packet->header.crc.word = ezbus_packet_calc_crc(tx_packet);
	ezbus_instance_tx_packet(instance);
	ezbus_instance_tx_queue(instance);
}

void ezbus_instance_tx_ack(ezbus_instance_t* instance, ezbus_address_t dst)
{
}

void ezbus_instance_tx_parcel(ezbus_instance_t* instance, ezbus_address_t dst)
{
}

void ezbus_instance_tx_reset(ezbus_instance_t* instance, ezbus_address_t dst)
{
}

void ezbus_instance_tx_speed(ezbus_instance_t* instance, ezbus_address_t dst)
{
}

/**
 * @brief Send io->tx_state.packet down the wire if
 * io->tx_state == EZBUS_ERR_OKAY.
 * @return io->tx_state == EZBUS_ERR_OKAY or fault code.
 */
void ezbus_instance_tx_packet(ezbus_instance_t* instance)
{
	if ( instance->io.tx_state.err == EZBUS_ERR_OKAY )
	{
		instance->io.tx_state.err = ezbus_port_send(&instance->io.port,&instance->io.tx_state.packet);
	}
}

/**
 * @brief Send packets from the transmit queue.
 * @param count Send up to this many packets.
 * @return io->tx_state == EZBUS_ERR_OKAY or fault code.
 */
void ezbus_instance_tx_queue(ezbus_instance_t* instance)
{
	/* FIXME - Handle re-transmit timer, re-queing, etc... */
	for(int index=0; index < ezbus_packet_queue_count(instance->io.tx_queue); index++)
	{
		EZBUS_ERR err = ezbus_packet_queue_can_tx(instance->io.tx_queue,&instance->io.tx_state.packet,index);
		switch( err )
		{
			case EZBUS_ERR_OKAY:
				ezbus_instance_tx_packet(instance);
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

/*****************************************************************************
 *****************************  RECEIVERS ************************************
 ****************************************************************************/

/**
 * receive the discovery negotiation packet.
 */
void ezbus_instance_rx_disco(ezbus_instance_t* instance)
{
	switch( (ezbus_packet_code_t)instance->io.rx_state.packet.header.data.field.size_code )
	{
		default:
		case packet_code_ok:			/* 0x00: No Problem */
			/* FIXME */
			instance->io.disco_seq = instance->io.rx_state.packet.header.data.field.seq;
			break;
		case packet_code_rq:			/* 0x01: packet_type_disco [request] */
			if ( instance->io.disco_seq != instance->io.rx_state.packet.header.data.field.seq )
			{
				ezbus_instance_tx_disco(
										instance,
										instance->io.address,
										instance->io.rx_state.packet.header.data.field.seq,
										packet_code_rp
										);
				ezbus_instance_tx_packet(instance);	/* send immediately */

			}
			break;
		case packet_code_rp:			/* 0x02: packet_type_disco [reply] */
			instance->io.rx_state.err = ezbus_address_list_append(&instance->io.peers,instance->io.rx_state.packet.header.data.field.src);
			break;
	}
}

void ezbus_instance_rx_give_token(ezbus_instance_t* instance)
{
	ezbus_instance_tx_queue(instance);
}

void ezbus_instance_rx_take_token(ezbus_instance_t* instance)
{
	/* NOTE do something, or no? */
}

void ezbus_instance_rx_ack(ezbus_instance_t* instance)
{
	int index = ezbus_packet_queue_index_of_seq(&instance->io.tx_queue,instance->io.rx_state.packet.header.data.field.seq);
	if ( index >= 0 )
	{
		ezbus_packet_t packet;
		ezbus_packet_queue_take_at(&packet,index);
	}
}

void ezbus_instance_rx_parcel(ezbus_instance_t* instance)
{
}

void ezbus_instance_rx_reset(ezbus_instance_t* instance)
{
}

void ezbus_instance_rx_speed(ezbus_instance_t* instance)
{
}

/**
 * @brief Activated upon receiving a packet.
 * @return io->tx_state == EZBUS_ERR_OKAY or fault code.
 */
static void ezbus_instance_rx(ezbus_instance_t* instance)
{
	/* A valid rx_packet is present? */
	if ( instance->io.rx_state.err == EZBUS_ERR_OKAY )
	{
		/* Is the rx_packet addressed to this peer or is a broadcast? */
		if ( ezbus_address_compare(instance->io.rx_state.packet.header.data.field.dst,instance->io.address) == 0 ||
			 ezbus_address_compare(instance->io.rx_state.packet.header.data.field.dst,ezbus_broadcast_address) == 0 )
		{
			switch( (ezbus_packet_type_t)instance->io.rx_state.packet.header.data.field.type )
			{
				case packet_type_disco:				/* 0x00: Discover */
					ezbus_instance_rx_disco(instance);
					break;
				case packet_type_give_token:		/* 0x01: Give Token */
					ezbus_instance_rx_give_token(instance);
					break;
				case packet_type_take_token:		/* 0x02: Take Token */
					ezbus_instance_rx_take_token(instance);
					break;
				case packet_type_ack:				/* 0x03: (N)Ack / Return */
					ezbus_instance_rx_ack(instance);
					break;
				case packet_type_parcel:			/* 0x04: Data Parcel */
					ezbus_instance_rx_parcel(instance);
					break;
				case packet_type_reset:				/* 0x05: Bus Reset */
					ezbus_instance_rx_reset(instance);
					break;
				case packet_type_speed:				/* 0x06: Set Bus Speed */
					ezbus_instance_rx_speed(instance);
					break;
			}
		}
		else
		{
			/* It's not addressed to this peer. */
			instance->io.rx_state.err = EZBUS_ERR_MISMATCH;
		}
	}
}


