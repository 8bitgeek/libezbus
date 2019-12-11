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

#include <ezbus_err.h>


#define EZBUS_LOG_STREAM        stderr
#define EZBUS_LOG_TOKEN         1
#define EZBUS_LOG_RECEIVER      1
#define EZBUS_LOG_TRANSMITTER   1
#define EZBUS_LOG_TIMERS        1
#define EZBUS_LOG_HELLO         1
#define EZBUS_LOG_HEX           1


#define EZBUS_MARK              0x55                    /* Packet lead-in marks */
#define EZBUS_PARCEL_DATA_LN    256                     /* Maximum data length */
#define EZBUS_SPEED_COUNT       10                      /* Number of baud rates in baud table */
#define EZBUS_SPEED_INDEX_DEF   3                       /* Default speed index */
#define EZBUS_BYTES_TIMEOUT     sizeof(ezbus_packet_t)  /* Number of byte times to constitute a timeout */
#define EZBUS_DISCO_PERIOD      128                     /* Number of milliseconds in a discovery period */
#define EZBUS_DISCO_COUNT       4                       /* Minimum number of discovery attempts */
#define EZBUS_RETRANSMIT_TO     256                     /* Time to re-try transmitting a packet */
#define EZBUS_RETRANSMIT_TRIES  8                       /* Number of re-transmit attempts */
#define EZBUS_TOKEN_TIMEOUT_DEF	1000 					/* Default token timout period */

#if !defined(EZBUS_MAX_PEERS)
    #define EZBUS_MAX_PEERS         32                  /* Maximum number of peers */
#endif
#define EZBUS_ASSUMED_PEERS     EZBUS_MAX_PEERS         /* Peers to assume when 0 peers known */

typedef int EZBUS_ERR;

#define EZBUS_ERR_OKAY          0
#define EZBUS_ERR_PARAM         -1                      /* Bad Parameter */
#define EZBUS_ERR_MALLOC        -2                      /* Memory Allocation Error */
#define EZBUS_ERR_LIMIT         -3                      /* Resource Limitation */
#define EZBUS_ERR_RANGE         -4                      /* Out of Range */
#define EZBUS_ERR_TIMEOUT       -5                      /* Timeout Fault */
#define EZBUS_ERR_CRC           -6                      /* CRC Fault */
#define EZBUS_ERR_IO            -7                      /* I/O Fault */
#define EZBUS_ERR_NOTREADY      -8                      /* Not Ready */
#define EZBUS_ERR_MISMATCH      -9                      /* Not a match (address) */
#define EZBUS_ERR_DUP           -10                     /* Duplicate */
#define EZBUS_ERR_OVERFLOW      -11                     /* Overflow occured */

#define EZBUS_ADDR_LN           12                      /* 96 bit unique address */
#define EZBUS_ADDR_LN_STR       ((EZBUS_ADDR_LN*2)+1)
#define EZBUS_ADDR_WORDS        3

#define EZBUS_TMP_BUF_SZ        128

#define EZBUS_RAND_LOWER        (EZBUS_DISCO_PERIOD/16)
#define EZBUS_RAND_UPPER        EZBUS_DISCO_PERIOD

#define EZBUS_FEATURES_TRAIN    0x01                    /* Node can accept train size packets */
#define EZBUS_FEATURES_SPEED    0x02                    /* Node can change speed */

#define EZBUS_DRIVER_DEBUG      1
#define DISCO_FEATURES          ( EZBUS_FEATURES_TRAIN | EZBUS_FEATURES_SPEED )

#define EZBUS_TOKEN_TIMER_MIN   100                     /* FIXME - calculate this at run time */
#define EZBUS_TOKEN_TIMER_MAX   200                     /* FIXME - calculate this at run time */

#define EZBUS_EMIT_TIMER_MIN    10                      /* FIXME - calculate this at run time */
#define EZBUS_EMIT_TIMER_MAX    100                     /* FIXME - calculate this at run time */

#endif /* EZBUS_CONST_H_ */
