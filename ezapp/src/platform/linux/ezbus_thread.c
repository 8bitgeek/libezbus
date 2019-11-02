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
#include "ezbus_thread.h"
#include "ezbus_signal.h"

static void ezbus_rx_callback(ezbus_packet_io_t* io);

void ezbus_thread_run(void* arg)
{
	char* serial_port_name = (char*)arg;

	ezbus_instance_t ezbus_instance;
	ezbus_instance_init_struct(&ezbus_instance);

	ezbus_platform_address(&ezbus_instance.io.address);
	ezbus_instance_set_tx_cb(&ezbus_instance,ezbus_rx_callback);

	fprintf( stderr, "ID:%08X%08X%08X\n",
			ezbus_packet_flip32(ezbus_instance.io.address.word[0]),
			ezbus_packet_flip32(ezbus_instance.io.address.word[1]),
			ezbus_packet_flip32(ezbus_instance.io.address.word[2]) );

	
	ezbus_instance.io.port.platform_port.serial_port_name = serial_port_name;

	if ( ezbus_instance_init(&ezbus_instance,ezbus_port_speeds[EZBUS_SPEED_INDEX_DEF],EZBUS_TX_QUEUE_SZ) >= 0 )
	{
		ezbus_signal_init(&ezbus_instance);
		for(;;) /* forever... */
		{
			ezbus_signal_run();
			ezbus_instance_run(&ezbus_instance);
		}
	}
}

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
				fprintf(stderr,"packet_type_disco\n");
				break;
			case packet_type_give_token:	/* 0x02: Give Token */
				fprintf(stderr,"packet_type_give_token\n");
				break;
			case packet_type_take_token:	/* 0x03: Take Token */
				fprintf(stderr,"packet_type_take_token\n");
				break;
			case packet_type_ack:			/* 0x04: (N)Ack / Return */
				fprintf(stderr,"packet_type_ack\n");
				break;
			case packet_type_parcel:		/* 0x05: Data Parcel */
				fprintf(stderr,"packet_type_parcel\n");
				break;
			case packet_type_reset:			/* 0x06: Bus Reset */
				fprintf(stderr,"packet_type_reset\n");
				break;
			case packet_type_speed:			/* 0x07: Set Bus Speed */
				fprintf(stderr,"packet_type_speed\n");
				break;
		}
	}
}

