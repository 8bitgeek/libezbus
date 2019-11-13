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
#include <ezbus_packet.h>
#include <ezbus_crc.h>

void ezbus_packet_init(ezbus_packet_t* packet)
{
	ezbus_platform_memset(packet,0,sizeof(ezbus_packet_t));
}

void ezbus_packet_deinit(ezbus_packet_t* packet)
{
	if ( packet != NULL )
	{
		ezbus_packet_clear_data(packet);
		ezbus_platform_memset(packet,0,sizeof(ezbus_packet_t));
	}
}

extern int ezbus_packet_set_parcel( ezbus_packet_t* packet, uint8_t* data, uint16_t size )
{
	int err=EZBUS_ERR_OKAY;
	if ( packet != NULL && size <= EZBUS_PARCEL_DATA_LN )
	{
		packet->data.attachment.parcel.size = size;
		ezbus_platform_memcpy( packet->data.attachment.parcel.bytes, data, size );
	}
	else
	{
		err = EZBUS_ERR_PARAM;
	}
	return err;
}

int ezbus_packet_clear_data(ezbus_packet_t* packet)
{
	int err=EZBUS_ERR_OKAY;
	if ( packet != NULL )
	{
		ezbus_platform_memset(&packet->data,0,sizeof(ezbus_data_t));
	}
	else
	{
		err = EZBUS_ERR_PARAM;
	}
	return err;
}


extern ezbus_address_t* ezbus_packet_dst( ezbus_packet_t* packet )
{
	return &packet->header.data.field.dst;
}

extern ezbus_address_t* ezbus_packet_src( ezbus_packet_t* packet )
{
	return &packet->header.data.field.src;
}

extern ezbus_packet_type_t ezbus_packet_type( ezbus_packet_t* packet )
{
	return (ezbus_packet_type_t)packet->header.data.field.type;
}

extern void ezbus_packet_calc_crc( ezbus_packet_t* packet )
{
	ezbus_crc( &packet->header.crc, packet->header.data.bytes, sizeof(struct _header_field_) );
	switch ( ezbus_packet_type(packet) )
	{
		case packet_type_reset:
			break;
		case packet_type_disco_rq:
		case packet_type_disco_rp:
		case packet_type_disco_rk:
			ezbus_crc( &packet->data.crc, &packet->data.attachment.disco, sizeof(ezbus_disco_t) );
			break;
		case packet_type_take_token:
		case packet_type_give_token:
			break;
		
		/* Synchronous Data Packets */
		
		case packet_type_parcel:
			ezbus_crc( &packet->data.crc, &packet->data.attachment.parcel, sizeof(ezbus_parcel_t) );
			break;
		case packet_type_speed:
			ezbus_crc( &packet->data.crc, &packet->data.attachment.speed, sizeof(ezbus_speed_t) );	
			break;
		case packet_type_ack:
		case packet_type_nack:
			break;
	}
}

extern bool ezbus_packet_valid_crc( ezbus_packet_t* packet )
{
	ezbus_crc_t crc;
	bool rc=false;
	if ( ezbus_crc_equal( &packet->header.crc, ezbus_crc( &crc, packet->header.data.bytes, sizeof(struct _header_field_) ) ) )
	{
		switch ( ezbus_packet_type(packet) )
		{
			case packet_type_reset:
				break;
			case packet_type_disco_rq:
			case packet_type_disco_rp:
			case packet_type_disco_rk:
				rc = ezbus_crc_equal( &packet->data.crc, ezbus_crc( &crc, &packet->data.attachment.disco, sizeof(ezbus_disco_t) ) );
				break;
			case packet_type_take_token:
			case packet_type_give_token:
				break;
			
			/* Synchronous Data Packets */
			
			case packet_type_parcel:
				rc = ezbus_crc_equal( &packet->data.crc, ezbus_crc( &crc, &packet->data.attachment.parcel, sizeof(ezbus_parcel_t) ) );
				break;
			case packet_type_speed:
				rc = ezbus_crc_equal( &packet->data.crc, ezbus_crc( &crc, &packet->data.attachment.speed, sizeof(ezbus_speed_t) ) );
				break;
			case packet_type_ack:
			case packet_type_nack:
				break;
		}
	}
	return rc;
}


uint16_t ezbuf_packet_bytes_to_send( ezbus_packet_t* packet )
{
	uint16_t rc = sizeof(ezbus_header_t);
	switch ( ezbus_packet_type(packet) )
	{
		case packet_type_reset:
			break;
		case packet_type_disco_rq:
		case packet_type_disco_rp:
		case packet_type_disco_rk:
			rc += sizeof(ezbus_crc_t) + sizeof(ezbus_disco_t);
			break;
		case packet_type_take_token:
		case packet_type_give_token:
			break;
		
		/* Synchronous Data Packets */
		
		case packet_type_parcel:
			rc += sizeof(ezbus_crc_t) + sizeof(ezbus_parcel_t);
			break;
		case packet_type_speed:
			rc += sizeof(ezbus_crc_t) + sizeof(ezbus_speed_t);
			break;
		case packet_type_ack:
		case packet_type_nack:
			break;
	}
	return rc;
}

extern void ezbus_packet_flip( ezbus_packet_t* packet )
{
	ezbus_crc_flip( &packet->header.crc );
	ezbus_crc_flip( &packet->data.crc );
}



extern void ezbus_packet_dump( ezbus_packet_t* packet, const char* prefix )
{
	char print_buffer[EZBUS_TMP_BUF_SZ];

	fprintf(stderr, "%s.header.data.field.mark=%02X\n",     prefix, packet->header.data.field.mark );
	fprintf(stderr, "%s.header.data.field.seq=%d\n",        prefix, packet->header.data.field.seq );
	fprintf(stderr, "%s.header.data.field.type=%02X\n", 	prefix, packet->header.data.field.type );

	sprintf( print_buffer, "%s.header.data.field.src", prefix );
	ezbus_address_dump( &packet->header.data.field.src, print_buffer );

	sprintf( print_buffer, "%s.header.data.field.dst", prefix );
	ezbus_address_dump( &packet->header.data.field.dst, print_buffer );

	sprintf( print_buffer, "%s.header.crc", prefix );
	ezbus_crc_dump( packet->header.crc.word, print_buffer );

	fflush(stderr);
}
