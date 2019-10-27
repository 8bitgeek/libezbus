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
#include "ezbus_packet.h"
#include "ezbus_crc.h"

void ezbus_packet_init(ezbus_packet_t* packet)
{
	ezbus_platform_memset(packet,0,sizeof(ezbus_packet_t));
}

void ezbus_packet_deinit(ezbus_packet_t* packet)
{
	if ( packet != NULL )
	{
		ezbus_packet_clear_parcel(packet);
		ezbus_platform_memset(packet,0,sizeof(ezbus_packet_t));
	}
}

/**
 * @brief copy data and size into header and attach a CRC.
 */
int ezbus_packet_set_parcel(ezbus_packet_t* packet,uint8_t* data,uint8_t size)
{
	int err=EZBUS_ERR_OKAY;
	if ( packet != NULL )
	{
		ezbus_platform_memcpy(packet->attachment.parcel.bytes,data,size);
		packet->header.data.field.size_code = size;
		packet->header.crc.word = ezbus_packet_calc_crc(packet);
		packet->attachment.parcel.crc.word = ezbus_packet_calc_parcel_crc(packet);
	}
	else
	{
		err = EZBUS_ERR_PARAM;
	}
	return err;
}

int ezbus_packet_clear_parcel(ezbus_packet_t* packet)
{
	int err=EZBUS_ERR_OKAY;
	if ( packet != NULL )
	{
		packet->header.data.field.size_code = 0;
		ezbus_platform_memset(&packet->attachment.parcel,0,sizeof(ezbus_parcel_t));
	}
	else
	{
		err = EZBUS_ERR_PARAM;
	}
	return err;
}

/**
 * @brief Calculate the packet header CRC
 * @return packet header CRC in natural byte order.
 */
uint16_t ezbus_packet_calc_crc(ezbus_packet_t* packet)
{
	uint16_t crc = 0;
	for(int n=0; n < sizeof(struct _header_field_); n++)
	{
		crc = ezbus_crc_update(crc,packet->header.data.bytes[n]);
	}
	return crc;
}

/**
 * @brief Calculate the packet data CRC
 * @return packet data CRC in natural bytes order.
 */
uint16_t ezbus_packet_calc_parcel_crc(ezbus_packet_t* packet)
{
	uint16_t crc=0;
	for(int n=0; n < EZBUS_DATA_LN; n++)
	{
		crc = ezbus_crc_update(crc,packet->attachment.parcel.bytes[n]);
	}
	return crc;
}

/**
 * @brief Calculate the packet speed CRC
 * @return packet speed CRC in natural bytes order.
 */
uint16_t ezbus_packet_calc_speed_crc(ezbus_packet_t* packet)
{
	uint16_t crc=0;
	for(int n=0; n < EZBUS_DATA_LN; n++)
	{
		crc = ezbus_crc_update(crc,packet->attachment.speed.data.bytes[n]);
	}
	return crc;
}

/**
 * Bus byte order is big endian. This function flips 16-bit integers
 * to (send) and from (recv) bus byte order.
 */
uint16_t ezbus_packet_flip16(uint16_t d)
{
	#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		uint16_t t = d&0xFF;
		d >>= 8;
		d |= t<<8;
	#endif
	return d;
}

/**
 * Bus byte order is big endian. This function flips 32-bit integers
 * to (send) and from (recv) bus byte order.
 */
uint32_t ezbus_packet_flip32(uint32_t d)
{
	#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		uint32_t t = ezbus_packet_flip16(d&0xFFFF);
		d = ezbus_packet_flip16(d >> 16);
		d |= t<<16;
	#endif
	return d;
}

uint16_t ezbuf_packet_bytes_to_send(ezbus_packet_t* packet)
{
	uint16_t rc=0;
	switch( (ezbus_packet_type_t)packet->header.data.field.type )
	{
		case packet_type_parcel:
			rc = sizeof(ezbus_header_t) + sizeof(ezbus_parcel_t);
			break;
		case packet_type_speed:
			rc = sizeof(ezbus_header_t) + sizeof(ezbus_speed_t);
			break;
		default:
			rc = sizeof(ezbus_header_t);
			break;
	}
	return rc;
}


extern void ezbus_packet_dump( ezbus_packet_t* packet, const char* prefix )
{
	char print_buffer[EZBUS_TMP_BUF_SZ];

	printf( "%s.header.data.field.mark=%02X\n",     prefix, packet->header.data.field.mark );
	printf( "%s.header.data.field.seq=%d\n",        prefix, packet->header.data.field.seq );
	printf( "%s.header.data.field.size_code=%d\n", 	prefix, packet->header.data.field.size_code );
	printf( "%s.header.data.field.type=%02X\n", 	prefix, packet->header.data.field.type );

	sprintf( print_buffer, "%s.header.data.field.src", prefix );
	ezbus_address_dump( &packet->header.data.field.src, print_buffer );

	sprintf( print_buffer, "%s.header.data.field.dst", prefix );
	ezbus_address_dump( &packet->header.data.field.dst, print_buffer );

	sprintf( print_buffer, "%s.header.crc", prefix );
	ezbus_crc_dump( packet->header.crc.word, print_buffer );
}
