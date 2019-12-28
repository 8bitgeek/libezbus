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
#ifndef EZBUS_MAC_ARBITRATION_H_
#define EZBUS_MAC_ARBITRATION_H_

#include <ezbus_platform.h>
#include <ezbus_mac.h>
#include <ezbus_timer.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    mac_arbitration_state_offline=0,
    mac_arbitration_state_coldboot,
    mac_arbitration_state_warmboot,                   
    mac_arbitration_state_service_start,
    mac_arbitration_state_service,                
    mac_arbitration_state_online                        
} ezbus_mac_arbitration_state_t;

typedef struct _ezbus_mac_arbitration_t
{
    ezbus_mac_arbitration_state_t state;
    ezbus_timer_t                 ack_tx_timer;
    ezbus_timer_t                 ack_rx_timer;
} ezbus_mac_arbitration_t;


extern void  ezbus_mac_arbitration_init ( ezbus_mac_t* mac );
extern void  ezbus_mac_arbitration_run  ( ezbus_mac_t* mac );

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_MAC_ARBITRATION_H_ */
