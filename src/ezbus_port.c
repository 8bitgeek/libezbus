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
#include <ezbus_port.h>
#include <ezbus_packet.h>

static int ezbus_private_recv(ezbus_port_t* port, uint8_t* p, uint32_t index, size_t size);

uint32_t ezbus_port_speeds[EZBUS_SPEED_COUNT] = {	2400,
													9600,
													57600,
													115200,
													460800,
													1152000,
													2000000,
													3000000,
													5000000,
													10000000 };

ezbus_port_t ports[EZBUS_MAX_PORTS];


extern EZBUS_ERR ezbus_port_open(ezbus_port_t* port,uint32_t speed)
{
	if ( ezbus_platform_open(&port->platform_port,speed) == 0 )
	{
		port->speed = speed;
		port->packet_timeout = ezbus_port_packet_timeout_time_ms(port);
		port->rx_err_crc_count = 0;
		port->rx_err_timeout_count = 0;
		port->rx_err_overrun_count = 0;
		port->tx_err_overrun_count = 0;
		return EZBUS_ERR_OKAY;
	}
	return EZBUS_ERR_IO;
}


extern EZBUS_ERR ezbus_port_send(ezbus_port_t* port,ezbus_packet_t* packet)
{
	EZBUS_ERR err        = EZBUS_ERR_OKAY;
	size_t bytes_to_send = ezbuf_packet_bytes_to_send ( packet );
	size_t bytes_sent;

	ezbus_packet_calc_crc ( packet );
	ezbus_packet_flip     ( packet );

	bytes_sent = ezbus_platform_send( &port->platform_port, packet, bytes_to_send );

	ezbus_platform_flush( &port->platform_port );

	err = ( bytes_to_send == bytes_sent ) ? EZBUS_ERR_OKAY : EZBUS_ERR_IO;

	ezbus_packet_flip ( packet );

	return err;
}


static int ezbus_private_recv(ezbus_port_t* port, uint8_t* p, uint32_t index, size_t size)
{
	int ch;
	ezbus_ms_tick_t start = ezbus_platform_get_ms_ticks();
	/* receive the entire header or timeout... */
	while ( index < size && (ezbus_platform_get_ms_ticks() - start) <= port->packet_timeout )
	{
		if ( (ch = ezbus_port_getch(port)) >= 0 )
		{
			p[index++] = ch;
			start = ezbus_platform_get_ms_ticks();
		}
	}
	return index;
}


extern EZBUS_ERR ezbus_port_recv( ezbus_port_t* port, ezbus_packet_t* packet )
{
	EZBUS_ERR err=EZBUS_ERR_NOTREADY; /* assume no packet */
	int index = 0;
	uint8_t* p = (uint8_t*)&packet->header;
	int ch;
	if ( ( ch = ezbus_port_getch( port ) ) == EZBUS_MARK )
	{
		p[ index++ ] = ch;
		index = ezbus_private_recv( port, p, index, sizeof( ezbus_header_t ) );
		if ( index == sizeof( ezbus_header_t ) )
		{

			switch( ezbus_packet_type(packet) )
			{
				case packet_type_reset:
				case packet_type_disco_rq:
				case packet_type_disco_rp:
				case packet_type_disco_rk:
				case packet_type_take_token:
				case packet_type_give_token:
				case packet_type_parcel:
				case packet_type_speed:
				case packet_type_ack:
				case packet_type_nack:
			}

			ezbus_packet_flip( packet );




			/* Flip the header CRC into natural byte order... */
			packet->header.crc.word = ezbus_packet_flip16(packet->header.crc.word);
			if ( packet->header.crc.word == ezbus_packet_calc_crc(packet) )
			{
				switch( (ezbus_packet_type_t)packet->header.data.field.type )
				{
					case packet_type_parcel:
						if ( (index = ezbus_private_recv(port,p,index,sizeof(ezbus_header_t)+sizeof(ezbus_parcel_t))) == sizeof(ezbus_header_t)+sizeof(ezbus_parcel_t) )
						{
							packet->attachment.parcel.crc.word = ezbus_packet_flip16(packet->attachment.parcel.crc.word);
							if ( packet->attachment.parcel.crc.word == ezbus_packet_calc_parcel_crc(packet) )
								err = EZBUS_ERR_OKAY;
						}
						else
							err = EZBUS_ERR_CRC; /* signal the caller that the CRC test failed */
						break;
					case packet_type_speed:
						if ( (index = ezbus_private_recv(port,p,index,sizeof(ezbus_header_t)+sizeof(ezbus_speed_t))) == sizeof(ezbus_header_t)+sizeof(ezbus_speed_t) )
						{
							packet->attachment.speed.crc.word = ezbus_packet_flip16(packet->attachment.speed.crc.word);
							if ( packet->attachment.speed.crc.word == ezbus_packet_calc_speed_crc(packet) )
								err = EZBUS_ERR_OKAY;
						}
						else
							err = EZBUS_ERR_CRC; /* signal the caller that the CRC test failed */
						break;
					default:
						err = EZBUS_ERR_OKAY;
						break;
				}
			}
			else
			{
				err=EZBUS_ERR_CRC; /* signal the caller that the CRC test failed */
			}
		}
		else
		{
			if ( index > 0 )
				err = EZBUS_ERR_TIMEOUT;
		}
	}
	else
	{
		/*
		 * If we got an improper lead-in, then we want to just trash
		 * the while receive buffer rather that trying to parse it out
		 * to find the beginning of the next packet.
		 */
		ezbus_platform_flush(&port->platform_port);
	}
	return err;
}

int	ezbus_port_getch(ezbus_port_t* port)
{
	return ezbus_platform_getc(&port->platform_port);
}

void ezbus_port_close(ezbus_port_t* port)
{
	ezbus_platform_close(&port->platform_port);
}

void ezbus_port_drain(ezbus_port_t* port)
{
	ezbus_platform_drain(&port->platform_port);
}

void ezbus_port_set_speed(ezbus_port_t* port,uint32_t speed)
{
	port->speed = speed;
	port->packet_timeout = ezbus_port_packet_timeout_time_ms(port);
	ezbus_platform_set_speed(&port->platform_port,port->speed);
}

uint32_t ezbus_port_get_speed(ezbus_port_t* port)
{
	return port->speed;
}

uint32_t ezbus_port_byte_time_ns(ezbus_port_t* port)
{
	uint32_t bits_sec = port->speed;
	uint32_t nsec_bit = 1000000000 / bits_sec;	/* ex. 1000000000 / 10000000 = 100 */
	uint32_t nsec_byte = nsec_bit * 10;			/* ex. 100 * 10 = 1000 */
	return nsec_byte;
}

uint32_t ezbus_port_packet_timeout_time_ms(ezbus_port_t* port)
{
	/* 1000000 ns in a ms. */
	uint32_t nsec_byte = ezbus_port_byte_time_ns(port);
	uint32_t nsec_packet = sizeof(ezbus_packet_t) * nsec_byte;
	uint32_t msec_packet = nsec_packet/1000000;
	return msec_packet?msec_packet:1;
}

extern void ezbus_port_dump(ezbus_port_t* port,const char* prefix)
{
	char print_buffer[EZBUS_TMP_BUF_SZ];

	sprintf( print_buffer, "%s.platform_port", prefix );
	ezbus_platform_port_dump( &port->platform_port, print_buffer );

	fprintf(stderr, "%s.speed=%d\n", 					prefix, port->speed );
	fprintf(stderr, "%s.packet_timeout=%d\n", 			prefix, port->packet_timeout );
	fprintf(stderr, "%s.rx_err_crc_count=%d\n", 		prefix, port->rx_err_crc_count );
	fprintf(stderr, "%s.rx_err_timeout_count=%d\n", 	prefix, port->rx_err_timeout_count );
	fprintf(stderr, "%s.rx_err_overrun_count=%d\n", 	prefix, port->rx_err_overrun_count );
	fprintf(stderr, "%s.tx_err_overrun_count=%d\n", 	prefix, port->tx_err_overrun_count );
	fprintf(stderr, "%s.tx_err_retry_fail_count=%d\n", 	prefix, port->tx_err_retry_fail_count );
	fflush(stderr);
}
