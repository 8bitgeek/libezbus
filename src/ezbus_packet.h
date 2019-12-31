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
#ifndef EZBUS_PACKET_H_
#define EZBUS_PACKET_H_

#include <ezbus_platform.h>
#include <ezbus_parcel.h>
#include <ezbus_address.h>
#include <ezbus_crc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PACKET_BITS_VERSION_POS 	0
#define PACKET_BITS_VERSION_MASK 	(0x0F<<PACKET_BITS_VERSION_POS)
#define PACKET_BITS_VERSION_0 		(0x01<<PACKET_BITS_VERSION_POS)
#define PACKET_BITS_VERSION_1 		(0x02<<PACKET_BITS_VERSION_POS)
#define PACKET_BITS_VERSION_2 		(0x04<<PACKET_BITS_VERSION_POS)
#define PACKET_BITS_VERSION_3 		(0x08<<PACKET_BITS_VERSION_POS)
#define PACKET_BITS_VERSION 		(PACKET_BITS_VERSION_0)

#define PACKET_BITS_CHAIN_POS		4
#define PACKET_BITS_CHAIN_MASK 		(0x03<<PACKET_BITS_CHAIN_POS)
#define PACKET_BITS_CHAIN_0 		(0x01<<PACKET_BITS_CHAIN_POS)
#define PACKET_BITS_CHAIN_1 		(0x02<<PACKET_BITS_CHAIN_POS)
#define PACKET_BITS_CHAIN_BEGIN		(PACKET_BITS_CHAIN_0)
#define PACKET_BITS_CHAIN_MIDDLE	(0x00)
#define PACKET_BITS_CHAIN_LAST		(PACKET_BITS_CHAIN_1)
#define PACKET_BITS_CHAIN_SINGLE	(PACKET_BITS_CHAIN_1|PACKET_BITS_CHAIN_0)

typedef enum
{
	packet_type_reset = 0x00,
	packet_type_coldboot,
	packet_type_warmboot,
	packet_type_take_token,
	packet_type_give_token,
	packet_type_parcel,
	packet_type_speed,
	packet_type_ack,
	packet_type_nack,
} ezbus_packet_type_t;



#pragma pack(push)
#pragma pack(1)



typedef struct
{
	union {
		struct _header_field_ 
		{
			uint8_t			mark;
			uint8_t			bits;
			uint8_t			seq;
			uint8_t			type;
			ezbus_address_t	src;
			ezbus_address_t dst;
		} field;
		uint8_t				bytes[sizeof(struct _header_field_)];
	} data;
	ezbus_crc_t 			crc;
} ezbus_header_t;


typedef union
{
	uint32_t			word;
	uint8_t				bytes[sizeof(uint32_t)];
} ezbus_speed_t ;

typedef struct
{
	ezbus_crc_t 		peer_list_crc;
	uint16_t			token_counter;
} ezbus_token_t;

typedef struct
{
	ezbus_crc_t 		crc;
	union
	{
		ezbus_parcel_t	parcel;
		ezbus_speed_t	speed;
		ezbus_token_t	token;
	} attachment;
} ezbus_data_t;



typedef struct
{
	ezbus_header_t		header;
	ezbus_data_t 		data;
} ezbus_packet_t;


#pragma pack(pop)


extern void					ezbus_packet_init 				( ezbus_packet_t* packet );
extern void					ezbus_packet_deinit 			( ezbus_packet_t* packet );


extern void 				ezbus_packet_set_bits 			( ezbus_packet_t* packet, uint8_t bits );
extern void 				ezbus_packet_set_version		( ezbus_packet_t* packet, uint8_t version );
extern void 				ezbus_packet_set_chain 			( ezbus_packet_t* packet, uint8_t chain );
extern void 				ezbus_packet_set_seq 			( ezbus_packet_t* packet, uint8_t seq );
extern void 				ezbus_packet_set_type 			( ezbus_packet_t* packet, ezbus_packet_type_t type );
extern void 				ezbus_packet_set_src			( ezbus_packet_t* packet, ezbus_address_t* address );
extern void 				ezbus_packet_src_dst 			( ezbus_packet_t* packet, ezbus_address_t* address );

extern uint8_t 				ezbus_packet_bits           	( ezbus_packet_t* packet );	
extern uint8_t 				ezbus_packet_version           	( ezbus_packet_t* packet );	
extern uint8_t 				ezbus_packet_chain           	( ezbus_packet_t* packet );	
extern uint8_t 				ezbus_packet_version           	( ezbus_packet_t* packet );	
extern uint8_t 				ezbus_packet_chain           	( ezbus_packet_t* packet );	
extern uint8_t 				ezbus_packet_seq           		( ezbus_packet_t* packet );	
extern ezbus_packet_type_t 	ezbus_packet_type           	( ezbus_packet_t* packet );	
extern ezbus_address_t*		ezbus_packet_dst 				( ezbus_packet_t* packet );
extern ezbus_address_t* 	ezbus_packet_src 				( ezbus_packet_t* packet );


extern uint16_t				ezbuf_packet_bytes_to_send 		( ezbus_packet_t* packet );
extern void 				ezbus_packet_flip 				( ezbus_packet_t* packet );
extern void					ezbus_packet_calc_crc       	( ezbus_packet_t* packet );
extern bool 				ezbus_packet_valid_crc 			( ezbus_packet_t* packet );
extern void 				ezbus_packet_copy 				( ezbus_packet_t* dst, ezbus_packet_t* src );

extern bool 				ezbus_packet_header_valid_crc 	( ezbus_packet_t* packet );
extern void 				ezbus_packet_header_flip		( ezbus_packet_t* packet );

extern bool					ezbus_packet_data_valid_crc		( ezbus_packet_t* packet );
extern void					ezbus_packet_data_flip			( ezbus_packet_t* packet );
extern uint8_t* 			ezbus_packet_data				( ezbus_packet_t* packet );
extern uint16_t 			ezbus_packet_data_size			( ezbus_packet_t* packet );

extern void     			ezbus_packet_dump           	( ezbus_packet_t* packet, const char* prefix );

#define ezbus_packet_is_warmboot(packet)	(ezbus_packet_type( ((packet)) ) == packet_type_warmboot)
#define ezbus_packet_is_coldboot(packet)	(ezbus_packet_type( ((packet)) ) == packet_type_coldboot)

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_PACKET_H_ */
