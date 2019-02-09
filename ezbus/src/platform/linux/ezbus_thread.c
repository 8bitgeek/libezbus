/******************************************************************************
 * Copyright © 2018 by @author Mike Sharkey <mike@8bitgeek.net>
 * All Rights Reserved
 *****************************************************************************/
#include "ezbus_thread.h"

#define EZBUS_UART "/dev/ttyUSB1"

/**
 * @brief Activated upon receiving a packet.
 * @return <0 if packet was not handled.
 */
static void ezbus_rx_callback(ezbus_packet_io_t* io)
{
	if ( io->rx_state.err == EZBUS_ERR_OKAY )
	{
		/**
		 * If the received packet is handled by the application
		 * and no further processing is required, then
		 * set the io->rx_state.ready = EZBUS_ERR_NOTREADY
		 * otherwise if io->rx_state.ready == EZBUS_ERR_OKAY
		 * then ezbus will continue to process the packet.
		 */
		switch( (ezbus_packet_type_t)io->rx_state.packet.header.data.field.type )
		{
			case packet_type_disco:			/* 0x00: Discover */
				break;
			case packet_type_give_token:	/* 0x02: Give Token */
				break;
			case packet_type_take_token:	/* 0x03: Take Token */
				break;
			case packet_type_ack:			/* 0x04: (N)Ack / Return */
				break;
			case packet_type_parcel:		/* 0x05: Data Parcel */
				break;
			case packet_type_reset:			/* 0x06: Bus Reset */
				break;
			case packet_type_speed:			/* 0x07: Set Bus Speed */
				break;
		}
	}
}

/**
 * This is the thread which is running the ezbus protocol.
 */
void ezbus_thread(void* arg)
{
	ezbus_instance_t ezbus_instance;
	ezbus_instance_init_struct(&ezbus_instance);

	/*
	 * Set up the platform specific I/O parameters...
	 */

	/* This host's address */
	ezbus_platform_address(ezbus_instance.io.address);

	/* RX Handler callback */
	ezbus_instance.rx_callback = ezbus_rx_callback;

	/* UART Number */
	ezbus_instance.io.port.platform_port.serial_port_name = EZBUS_UART;

	/*
	 * Open the port and initialize the instance...
	 */
	if ( ezbus_instance_init(&ezbus_instance,ezbus_port_speeds[EZBUS_SPEED_INDEX_DEF],EZBUS_TX_QUEUE_SZ) >= 0 )
	{
		for(;;) /* forever... */
		{
			ezbus_instance_run(&ezbus_instance);
		}
	}
}
