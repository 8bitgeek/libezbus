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
#include <ezbus_thread.h>
#include <ezbus_thread_signal.h>
#include <board.h>

#define EZBUS_CARIBOU_USART_NO	CONSOLE_USART 

static void ezbus_rx_callback(ezbus_packet_io_t* io);

static ezbus_platform_port_t platform_port;

/**
 * This is the thread which is running the ezbus protocol.
 */
void ezbus_thread_run(void* arg)
{
	ezbus_driver_t ezbus_driver;

	ezbus_thread_signal_init();

	platform_port.serial_port_no	= EZBUS_CARIBOU_USART_NO;
	platform_port.dir_pin			= &gpio_rs485_dir;
	platform_port.fd 				= NULL;

	if ( ezbus_driver_init( &ezbus_driver, &platform_port, ezbus_port_speeds[EZBUS_SPEED_INDEX_DEF], EZBUS_TX_QUEUE_SZ ) >= 0 )
	{
		ezbus_driver_set_rx_cb( &ezbus_driver, ezbus_rx_callback );
		for(;;)
		{
			ezbus_driver_run		(&ezbus_driver);
			ezbus_thread_signal_run	(&ezbus_driver);
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

