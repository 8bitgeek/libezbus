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
#ifndef EZBUS_DRIVER_DISCO_H_
#define EZBUS_DRIVER_DISCO_H_

#include <ezbus_platform.h>
#include <ezbus_driver.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void      ezbus_driver_disco         ( ezbus_driver_t* driver, uint32_t cycles, ezbus_progress_callback_t progress_callback );
extern void      ezbus_driver_tx_disco_wait ( ezbus_driver_t* driver );

extern void      ezbus_driver_tx_disco_rq   ( ezbus_driver_t* driver, const ezbus_address_t* dst );
extern void      ezbus_driver_tx_disco_rp   ( ezbus_driver_t* driver, const ezbus_address_t* dst );
extern void      ezbus_driver_tx_disco_rk   ( ezbus_driver_t* driver, const ezbus_address_t* dst );

extern void      ezbus_driver_rx_disco_rq   ( ezbus_driver_t* driver );
extern void      ezbus_driver_rx_disco_rp   ( ezbus_driver_t* driver );
extern void      ezbus_driver_rx_disco_rk   ( ezbus_driver_t* driver );


#ifdef __cplusplus
}
#endif

#endif /* EZBUS_DRIVER_DISCO_H_ */
