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
#ifndef EZBUS_DRIVER_PACKET_H_
#define EZBUS_DRIVER_PACKET_H_

#include <ezbus_driver.h>
#include <ezbus_parcel.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void      ezbus_driver_packet_tx_reset       ( ezbus_driver_t* driver );
extern void      ezbus_driver_packet_tx_speed       ( ezbus_driver_t* driver );
extern void      ezbus_driver_packet_tx_give_token  ( ezbus_driver_t* driver, const ezbus_address_t* dst );
extern void      ezbus_driver_packet_tx_take_token  ( ezbus_driver_t* driver, const ezbus_address_t* dst );
extern void      ezbus_driver_packet_tx_ack         ( ezbus_driver_t* driver, const ezbus_address_t* dst );
extern void      ezbus_driver_packet_tx_nack        ( ezbus_driver_t* driver, const ezbus_address_t* dst );
extern void      ezbus_driver_packet_tx_parcel      ( ezbus_driver_t* driver, const ezbus_address_t* dst, ezbus_parcel_t* parcel );

extern void      ezbus_driver_packet_rx             ( ezbus_driver_t* driver );
extern void      ezbus_driver_packet_rx_reset       ( ezbus_driver_t* driver );
extern void      ezbus_driver_packet_rx_speed       ( ezbus_driver_t* driver );
extern void      ezbus_driver_packet_rx_give_token  ( ezbus_driver_t* driver );
extern void      ezbus_driver_packet_rx_take_token  ( ezbus_driver_t* driver );
extern void      ezbus_driver_packet_rx_ack         ( ezbus_driver_t* driver );
extern void      ezbus_driver_packet_rx_nack        ( ezbus_driver_t* driver );
extern void      ezbus_driver_packet_rx_parcel      ( ezbus_driver_t* driver );

extern bool      ezbus_driver_packet_tx_empty       ( ezbus_driver_t* driver );
extern bool      ezbus_driver_packet_tx_full        ( ezbus_driver_t* driver );

extern void      ezbus_driver_packet_dump           ( ezbus_driver_t* driver );

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_DRIVER_PACKET_H_ */
