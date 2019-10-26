/******************************************************************************
 * Copyright © 2018 by @author Mike Sharkey <mike@8bitgeek.net>
 * All Rights Reserved
 *****************************************************************************/
#ifndef EZBUS_CONST_H_
#define EZBUS_CONST_H_

#if !defined(EZBUS_TX_QUEUE_SZ)
	#define EZBUS_TX_QUEUE_SZ	4						/* Number of transmit packets allowed to queue */
#endif

#define EZBUS_MAX_PORTS			10						/* Max number of open ports */
#define EZBUS_MARK				0x55					/* Packet lead-in marks */
#define EZBUS_DATA_LN			512						/* Maximum data length */
#define EZBUS_SPEED_COUNT		10						/* Number of baud rates in baud table */
#define EZBUS_SPEED_INDEX_DEF	3						/* Default speed index */
#define EZBUS_BYTES_TIMEOUT		sizeof(ezbus_packet_t);	/* Number of byte times to constitute a timeout */
#define EZBUS_DISCO_PERIOD		128						/* Number of milliseconds in a discovery period */
#define EZBUS_DISCO_COUNT		4						/* Minimum number of discovery attempts */
#define EZBUS_RETRANSMIT_TO		256						/* Time to re-try transmitting a packet */
#define EZBUS_RETRANSMIT_TRIES	8						/* Number of re-transmit attempts */

typedef int EZBUS_ERR;
#define EZBUS_ERR_OKAY			0
#define EZBUS_ERR_PARAM			-1						/* Bad Parameter */
#define	EZBUS_ERR_MALLOC		-2						/* Memory Allocation Error */
#define	EZBUS_ERR_LIMIT			-3						/* Resource Limitation */
#define	EZBUS_ERR_RANGE			-4						/* Out of Range */
#define	EZBUS_ERR_TIMEOUT		-5						/* Timeout Fault */
#define	EZBUS_ERR_CRC			-6						/* CRC Fault */
#define EZBUS_ERR_IO			-7						/* I/O Fault */
#define EZBUS_ERR_NOTREADY		-8						/* Not Ready */
#define	EZBUS_ERR_MISMATCH		-9						/* Not a match (address) */
#define EZBUS_ERR_DUP			-10						/* Duplicate */


#define	EZBUS_ADDR_LN			12				/* 96 bit unique address */
#define EZBUS_ADDR_WORDS		3

#endif /* EZBUS_CONST_H_ */
