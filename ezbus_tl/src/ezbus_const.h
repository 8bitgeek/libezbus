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
#ifndef EZBUS_CONST_H_
#define EZBUS_CONST_H_

#if !defined(EZBUS_TX_QUEUE_SZ)
	#define EZBUS_TX_QUEUE_SZ	4						/* Number of transmit packets allowed to queue */
#endif

#define EZBUS_MAX_PORTS			10						/* Max number of open ports */
#define EZBUS_MARK				0x55					/* Packet lead-in marks */
#define EZBUS_DATA_LN			256						/* Maximum data length */
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


#define	EZBUS_ADDR_LN			12						/* 96 bit unique address */
#define EZBUS_ADDR_WORDS		3

#define EZBUS_TMP_BUF_SZ 		128

#define EZBUS_RAND_LOWER 		(EZBUS_DISCO_PERIOD/16)
#define EZBUS_RAND_UPPER 		EZBUS_DISCO_PERIOD

#endif /* EZBUS_CONST_H_ */
