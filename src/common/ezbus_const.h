/*****************************************************************************
* Copyright © 2019-2020 Mike Sharkey <mike@8bitgeek.net>                     *
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

#include <ezbus_types.h>
#include <ezbus_fault.h> 

typedef unsigned char                   ezbus_socket_t;
#define EZBUS_SOCKET_ANY                0xFF
#define EZBUS_SOCKET_INVALID            EZBUS_SOCKET_ANY

#ifndef EZBUS_MAX_SOCKETS
    #define EZBUS_MAX_SOCKETS           10
#endif

#ifndef EZBUS_LOG_STREAM
    #define EZBUS_LOG_STREAM            stderr
#endif

#ifndef EZBUS_SPEED_INDEX_DEF
    #define EZBUS_SPEED_INDEX_DEF       4 /* Default speed index */
#endif



#ifndef EZBUS_LOG_TOKEN
    #define EZBUS_LOG_TOKEN             0
#endif
#ifndef EZBUS_LOG_PORT
    #define EZBUS_LOG_PORT              0
#endif
#ifndef EZBUS_LOG_RECEIVER
    #define EZBUS_LOG_RECEIVER          0
#endif
#ifndef EZBUS_LOG_RECEIVER_ERR
    #define EZBUS_LOG_RECEIVER_ERR      0
#endif
#ifndef EZBUS_LOG_ARBITER
    #define EZBUS_LOG_ARBITER           0
#endif
#ifndef EZBUS_LOG_TRANSMITTER
    #define EZBUS_LOG_TRANSMITTER       0
#endif
#ifndef EZBUS_LOG_SOCKET
    #define EZBUS_LOG_SOCKET            0
#endif
#ifndef EZBUS_LOG_TIMERS
    #define EZBUS_LOG_TIMERS            0
#endif
#ifndef EZBUS_LOG_BOOTSTATE
    #define EZBUS_LOG_BOOTSTATE         0
#endif
#ifndef EZBUS_LOG_TRANSMITTERSTATE
    #define EZBUS_LOG_TRANSMITTERSTATE  0
#endif
#ifndef EZBUS_LOG_BOOT2
    #define EZBUS_LOG_BOOT2             0
#endif
#ifndef EZBUS_LOG_BOOT1
    #define EZBUS_LOG_BOOT1             0
#endif
#ifndef EZBUS_LOG_BOOT0
    #define EZBUS_LOG_BOOT0             0
#endif
#ifndef EZBUS_LOG_DOMINANT
    #define EZBUS_LOG_DOMINANT          0
#endif
#ifndef EZBUS_LOG_HEX
    #define EZBUS_LOG_HEX               0
#endif
#ifndef EZBUS_LOG_HEXTOKEN
    #define EZBUS_LOG_HEXTOKEN          0
#endif
#ifndef EZBUS_LOG_ADDRESS
    #define EZBUS_LOG_ADDRESS           0
#endif
#ifndef EZBUS_LOG_PEERS
    #define EZBUS_LOG_PEERS             0
#endif
#ifndef EZBUS_LOG_ONLINE
    #define EZBUS_LOG_ONLINE            0
#endif
#ifndef EZBUS_LOG_APP
    #define EZBUS_LOG_APP               0
#endif
#ifndef EZBUS_LOG_TIMEOUT
    #define EZBUS_LOG_TIMEOUT           0
#endif



#define EZBUS_MARK                  0x55                /* Packet lead-in marks */
#if !defined(EZBUS_PARCEL_DATA_LN)
    #define EZBUS_PARCEL_DATA_LN    2048                /* Maximum data length */
#endif
#define EZBUS_TOKEN_HOLD_CYCLES     2                   /* Polling cycles to hold token for */
#define EZBUS_RETRANSMIT_TRIES      8                   /* Number of re-transmit attempts */
#ifndef EZBUS_SPEED_DEF
    #define EZBUS_SPEED_DEF         1000000
#endif
#define EZBUS_MAX_PEERS             32                  /* Maximum number of peers (from T.I. RS485 ref.) */
#define EZBUS_ASSUMED_PEERS         EZBUS_MAX_PEERS     /* Peers to assume when 0 peers known */

#define EZBUS_ADDR_LN               sizeof(uint32_t)    /* 32-bit hashed unique address */
#define EZBUS_ADDR_LN_STR           ((EZBUS_ADDR_LN*2)+1)

#define EZBUS_TMP_BUF_SZ            128

#define EZBUS_BOOT0_TIME            500                 /* coldboot silent timeout ms */
#define EZBUS_BOOT1_TIMER_MIN       10                  /* randomized response timer min. */
#define EZBUS_BOOT1_TIMER_MAX       50                  /* randomized response timer max. */
#define EZBUS_BOOT1_CYCLES          10                  /* # 'hello' cycles to determine token owner */

#define EZBUS_BOOT2_TIMER_MIN       10                  /* randomized response timer min. */
#define EZBUS_BOOT2_TIMER_MAX       50                  /* randomized response timer max. */
#define EZBUS_BOOT2_TIMER_PERIOD    (EZBUS_BOOT2_TIMER_MAX+10) /* period at which to issue boot2 requests */
#ifndef EZBUS_BOOT2_AGE
    #define EZBUS_BOOT2_AGE         (1000*10)           /* age of token  */
#endif
#define EZBUS_BOOT2_CYCLES          5                   /* number of cycles to repeat boot2 with no replies */

#define EZBUS_KEEPALIVE_CYCLES      (1000)              /* Number of cycles before keepalive times out and closes socket */

#endif /* EZBUS_CONST_H_ */
