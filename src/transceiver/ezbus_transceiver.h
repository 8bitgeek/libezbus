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
#ifndef EZBUS_TRANSCEIVER_H_
#define EZBUS_TRANSCEIVER_H_

#include <ezbus_platform.h>
#include <ezbus_packet.h>
#include <ezbus_mac.h>
#include <ezbus_fault.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void             ezbus_transceiver_init      ( void );
extern void             ezbus_transceiver_run       ( void );

extern int32_t          ezbus_transceiver_open      ( ezbus_mac_t* mac, ezbus_address_t* peer );
extern void             ezbus_transceiver_close     ( int32_t handle );

extern ezbus_mac_t*     ezbus_transceiver_mac       ( int32_t handle );
extern ezbus_packet_t*  ezbus_transceiver_tx_packet ( int32_t handle );
extern ezbus_packet_t*  ezbus_transceiver_rx_packet ( int32_t handle );

extern EZBUS_ERR        ezbus_transceiver_err       ( int32_t handle );
extern void             ezbus_transceiver_reset_err ( int32_t handle );

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_TRANSCEIVER_H_ */
