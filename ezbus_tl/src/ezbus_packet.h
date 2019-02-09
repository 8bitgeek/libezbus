/******************************************************************************
 * Copyright © 2018 by @author Mike Sharkey <mike@8bitgeek.net>
 * All Rights Reserved
 ******************************************************************************
 *
 * @brief Takes care of low-level header operations,
 * transmitting, receiving, validating, etc.
 *
 * Bus byte-order is big-endian
 *
 *  ---------PACKET---------
 *  0x00: 0x55
 *  0x01: <sequence_number>
 *  0x02: <code>
 *  0x03: <packet_type>
 *  0x04: <src_serialno_msb>
 *  0x05: <src_serialno_...>
 *  0x06: <src_serialno_...>
 *  0x07: <src_serialno_...>
 *  0x08: <src_serialno_...>
 *  0x09: <src_serialno_...>
 *  0x0A: <src_serialno_...>
 *  0x0B: <src_serialno_...>
 *  0x0C: <src_serialno_...>
 *  0x0D: <src_serialno_...>
 *  0x0E: <src_serialno_...>
 *  0x0F: <src_serialno_lsb>
 *  0x10: <dst_serialno_msb>
 *  0x11: <dst_serialno_...>
 *  0x12: <dst_serialno_...>
 *  0x13: <dst_serialno_...>
 *  0x14: <dst_serialno_...>
 *  0x15: <dst_serialno_...>
 *  0x16: <dst_serialno_...>
 *  0x17: <dst_serialno_...>
 *  0x18: <dst_serialno_...>
 *  0x19: <dst_serialno_...>
 *  0x1A: <dst_serialno_...>
 *  0x1B: <dst_serialno_lsb>
 *  0x1C: <header-crc-msb>
 *  0x1D: <header-crc-lsb>
 *  ** (packet_type == 5) **
 *  0x1E: <data-size-msb>
 *  0x1F: <data-size-lsb>
 *  ****
 *  0x??: <optional data-crc-msb>
 *  0x??: <optional data-crc-lsb>
 */

#ifndef EZBUS_HEADER_H_
#define EZBUS_HEADER_H_

#include "ezbus_platform.h"
#include "ezbus_address.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
	packet_type_disco		=0x00,			/* 0x00: Discover */
	packet_type_give_token	=0x01,			/* 0x01: Give Token */
	packet_type_take_token	=0x02,			/* 0x02: Take Token */
	packet_type_ack			=0x03,			/* 0x03: (N)Ack / Return */
	packet_type_parcel		=0x04,			/* 0x04: Data Parcel */
	packet_type_reset		=0x05,			/* 0x05: Bus Reset */
	packet_type_speed		=0x06,			/* 0x06: Set Bus Speed */
} ezbus_packet_type_t;

typedef enum
{
	packet_code_ok			=0x00,			/* 0x00: No Problem */
	packet_code_rq			=0x01,			/* 0x01: packet_type_disco [request] */
	packet_code_rp			=0x02,			/* 0x02: packet_type_disco [reply] */
	packet_code_io			=0x03,			/* 0x03: I/O Error */
	packet_code_auth		=0x04,			/* 0x04: Authentication Error */
	packet_code_perm		=0x05,			/* 0x05: Permission Error */
	packet_code_crc			=0x06,			/* 0x06: CRC Error */
	packet_code_timeout		=0x07,			/* 0x07: Timeout Error */
	packet_code_overrun		=0x08,			/* 0x08: Overrun Error */
	packet_code_memory		=0x09,			/* 0x09: Memory Error */
	packet_code_open		=0x0A,			/* 0x0A: Open Error */
} ezbus_packet_code_t;

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */

typedef struct
{
	union {
		struct _header_field_ {
			uint8_t		mark;				/* Leading byte always AZBUS_MARK */
			uint8_t		seq;				/* Sequence number */
			uint8_t		size_code;			/* data size / return code */
			uint8_t		type;				/* ezbus_packet_type_t */
			uint8_t		src[EZBUS_ADDR_LN];	/* Source address */
			uint8_t		dst[EZBUS_ADDR_LN];	/* Destination address */
		} field;
		uint8_t			bytes[sizeof(struct _header_field_)];	/* bytes of the header field */
	} data;
	union {
		uint16_t		word;						/* Header CRC Word */
		uint8_t			bytes[sizeof(uint16_t)];	/* Header CRC Bytes */
	} crc;
} ezbus_header_t;

typedef struct
{
	uint8_t		bytes[EZBUS_DATA_LN];		/* The data bytes */
	union {
		uint16_t		word;						/* Data CRC Word */
		uint8_t			bytes[sizeof(uint16_t)];	/* Data CRC Bytes */
	} crc;
} ezbus_parcel_t;

typedef struct
{
	union {
		uint32_t		word;						/* Speed Word */
		uint8_t			bytes[sizeof(uint32_t)];	/* Speed Bytes */
	} data;
	union {
		uint16_t		word;						/* Speed CRC Word */
		uint8_t			bytes[sizeof(uint16_t)];	/* Speed CRC Bytes */
	} crc;
} ezbus_speed_t ;

typedef struct
{
	ezbus_header_t		header;				/* packet header */
	union {
		ezbus_parcel_t	parcel;				/* parcel data max EZBUS_DATA_LN bytes */
		ezbus_speed_t	speed;				/* speed specification data */
	} attachment;
} ezbus_packet_t;

#pragma pack(pop)   /* restore original alignment from stack */

extern void		ezbus_packet_init(ezbus_packet_t* packet);
extern void		ezbus_packet_deinit(ezbus_packet_t* packet);
extern int		ezbus_packet_set_parcel(ezbus_packet_t* packet,uint8_t* data,uint8_t size);
extern int		ezbus_packet_clear_parcel(ezbus_packet_t* packet);
extern uint16_t	ezbus_packet_calc_crc(ezbus_packet_t* packet);
extern uint16_t	ezbus_packet_calc_parcel_crc(ezbus_packet_t* packet);
extern uint16_t	ezbus_packet_calc_speed_crc(ezbus_packet_t* packet);
extern uint16_t	ezbus_packet_flip16(uint16_t d);
extern uint32_t	ezbus_packet_flip32(uint32_t d);
extern uint16_t	ezbuf_packet_bytes_to_send(ezbus_packet_t* packet);

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_HEADER_H_ */
