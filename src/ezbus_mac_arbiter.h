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
#ifndef EZBUS_MAC_ARBITER_H_
#define EZBUS_MAC_ARBITER_H_

#include <ezbus_platform.h>
#include <ezbus_mac.h>
#include <ezbus_timer.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    mac_arbiter_state_offline=0,
    mac_arbiter_state_reboot_cold,
    mac_arbiter_state_reboot_warm,    
    mac_arbiter_state_coldboot,
    mac_arbiter_state_warmboot,                   
    mac_arbiter_state_service_start,
    mac_arbiter_state_online                        
} ezbus_mac_arbiter_state_t;

typedef struct _ezbus_mac_arbiter_t
{
    ezbus_mac_arbiter_state_t   state;
    uint8_t                     warmboot_cycles;
    uint16_t                    token_age;   
} ezbus_mac_arbiter_t;


extern void     ezbus_mac_arbiter_init              ( ezbus_mac_t* mac );
extern void     ezbus_mac_arbiter_run               ( ezbus_mac_t* mac );

extern uint16_t ezbus_mac_arbiter_get_token_age     ( ezbus_mac_t* mac );
extern void     ezbus_mac_arbiter_set_token_age     ( ezbus_mac_t* mac, uint16_t age );

extern void                      ezbus_mac_arbiter_set_state ( ezbus_mac_t* mac, ezbus_mac_arbiter_state_t state );
extern ezbus_mac_arbiter_state_t ezbus_mac_arbiter_get_state ( ezbus_mac_t* mac );

extern uint8_t  ezbus_mac_arbiter_get_warmboot_cycles( ezbus_mac_t* mac );
extern void     ezbus_mac_arbiter_set_warmboot_cycles( ezbus_mac_t* mac, uint8_t cycles );
extern void     ezbus_mac_arbiter_dec_warmboot_cycles( ezbus_mac_t* mac );
extern void     ezbus_mac_arbiter_rst_warmboot_cycles( ezbus_mac_t* mac );


#ifdef __cplusplus
}
#endif

#endif /* EZBUS_MAC_ARBITER_H_ */
