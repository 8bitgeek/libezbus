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

#include <ezbus_fault.h>


#define EZBUS_LOG_STREAM            stderr
#define EZBUS_LOG_TOKEN             0
#define EZBUS_LOG_RECEIVER          1
#define EZBUS_LOG_ARBITRATION       0
#define EZBUS_LOG_TRANSMITTER       1
#define EZBUS_LOG_TIMERS            0
#define EZBUS_LOG_BOOTSTATE         0
#define EZBUS_LOG_TRANSMITTERSTATE  0
#define EZBUS_LOG_WARMBOOT          0
#define EZBUS_LOG_COLDBOOT          1
#define EZBUS_LOG_HEX               1
#define EZBUS_LOG_TX_STATE          0
#define EZBUS_LOG_ADDRESS           0
#define EZBUS_LOG_PEERS             1


#define EZBUS_MARK                  0x55                /* Packet lead-in marks */
#define EZBUS_PARCEL_DATA_LN        256                 /* Maximum data length */
#define EZBUS_SPEED_COUNT           10                  /* Number of baud rates in baud table */
#define EZBUS_SPEED_INDEX_DEF       5                   /* Default speed index */
#define EZBUS_RETRANSMIT_TO         256                 /* Time to re-try transmitting a packet */
#define EZBUS_RETRANSMIT_TRIES      8                   /* Number of re-transmit attempts */

#if !defined(EZBUS_MAX_PEERS)
    #define EZBUS_MAX_PEERS         32                  /* Maximum number of peers */
#endif
#define EZBUS_ASSUMED_PEERS         EZBUS_MAX_PEERS     /* Peers to assume when 0 peers known */

#define EZBUS_ADDR_LN               12                  /* 96 bit unique address */
#define EZBUS_ADDR_LN_STR           ((EZBUS_ADDR_LN*2)+1)
#define EZBUS_ADDR_WORDS            3

#define EZBUS_TMP_BUF_SZ            128

#define EZBUS_COLDBOOT_TIMER_MIN    10                  /* FIXME - calculate this at run time */
#define EZBUS_COLDBOOT_TIMER_MAX    100                 /* FIXME - calculate this at run time */
#define EZBUS_COLDBOOT_CYCLES       20                  /* # 'hello' cycles to determine token owner */

#define EZBUS_WARMBOOT_TIMER_MIN    10                  /* FIXME - calculate this at run time */
#define EZBUS_WARMBOOT_TIMER_MAX    100                 /* FIXME - calculate this at run time */
#define EZBUS_WARMBOOT_PERIOD       (1000*15)

#endif /* EZBUS_CONST_H_ */
