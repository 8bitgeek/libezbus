/*****************************************************************************
* Copyright © 2019-2020 Mike Sharkey <mike.sharkey@mineairquality.com>       *
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

typedef unsigned char                   ezbus_socket_t;
#define EZBUS_SOCKET_ANY                0xFF
#define EZBUS_SOCKET_INVALID            EZBUS_SOCKET_ANY
#define EZBUS_MAX_SOCKETS               10
#define EZBUS_LOG_STREAM                stderr

#if defined(__linux__)
    #define EZBUS_LOG_TOKEN             0
    #define EZBUS_LOG_RECEIVER          0
    #define EZBUS_LOG_ARBITER           0
    #define EZBUS_LOG_TRANSMITTER       0
    #define EZBUS_LOG_SOCKET            0
    #define EZBUS_LOG_TIMERS            0
    #define EZBUS_LOG_BOOTSTATE         0
    #define EZBUS_LOG_TRANSMITTERSTATE  0
    #define EZBUS_LOG_WARMBOOT          0
    #define EZBUS_LOG_COLDBOOT          0
    #define EZBUS_LOG_DOMINANT          0
    #define EZBUS_LOG_HEX               0
    #define EZBUS_LOG_HEXTOKEN          0
    #define EZBUS_LOG_ADDRESS           0
    #define EZBUS_LOG_PEERS             0
    #define EZBUS_LOG_ONLINE            0
#else
    #define EZBUS_LOG_TOKEN             0
    #define EZBUS_LOG_RECEIVER          0
    #define EZBUS_LOG_ARBITER           0
    #define EZBUS_LOG_TRANSMITTER       0
    #define EZBUS_LOG_SOCKET            0
    #define EZBUS_LOG_TIMERS            0
    #define EZBUS_LOG_BOOTSTATE         0
    #define EZBUS_LOG_TRANSMITTERSTATE  0
    #define EZBUS_LOG_WARMBOOT          0
    #define EZBUS_LOG_COLDBOOT          0
    #define EZBUS_LOG_DOMINANT          0
    #define EZBUS_LOG_HEX               0
    #define EZBUS_LOG_HEXTOKEN          0
    #define EZBUS_LOG_ADDRESS           0
    #define EZBUS_LOG_PEERS             0
    #define EZBUS_LOG_ONLINE            0
#endif

#define EZBUS_MARK                  0x55                /* Packet lead-in marks */
#define EZBUS_PARCEL_DATA_LN        64                  /* Maximum data length */
#define EZBUS_TOKEN_HOLD_CYCLES     2                   /* Polling cycles to hold token for */
#define EZBUS_SPEED_COUNT           10                  /* Number of baud rates in baud table */
#define EZBUS_SPEED_INDEX_DEF       4                   /* Default speed index */
//#define EZBUS_SPEED_INDEX_DEF       5                   /* Default speed index */
#define EZBUS_RETRANSMIT_TRIES      8                   /* Number of re-transmit attempts */

#define EZBUS_MAX_PEERS             32                  /* Maximum number of peers (from T.I. RS485 ref.) */
#define EZBUS_ASSUMED_PEERS         EZBUS_MAX_PEERS     /* Peers to assume when 0 peers known */

#define EZBUS_ADDR_LN               12                  /* 96 bit unique address */
#define EZBUS_ADDR_LN_STR           ((EZBUS_ADDR_LN*2)+1)
#define EZBUS_ADDR_WORDS            3

#define EZBUS_TMP_BUF_SZ            128

#define EZBUS_COLDBOOT_SILENT_TIME  500                 /* coldboot silent timeout ms */
#define EZBUS_COLDBOOT_TIMER_MIN    10                  /* randomized response timer min. */
#define EZBUS_COLDBOOT_TIMER_MAX    50                  /* randomized response timer max. */
#define EZBUS_COLDBOOT_CYCLES       10                  /* # 'hello' cycles to determine token owner */

#define EZBUS_WARMBOOT_TIMER_MIN    10                  /* randomized response timer min. */
#define EZBUS_WARMBOOT_TIMER_MAX    50                  /* randomized response timer max. */
#define EZBUS_WARMBOOT_TIMER_PERIOD (EZBUS_WARMBOOT_TIMER_MAX+10) /* period at which to issue warmboot requests */
#define EZBUS_WARMBOOT_AGE          (1000*32)           /* age of token  */
#define EZBUS_WARMBOOT_CYCLES       5                   /* number of cycles to repeat warmboot with no replies */

#endif /* EZBUS_CONST_H_ */
