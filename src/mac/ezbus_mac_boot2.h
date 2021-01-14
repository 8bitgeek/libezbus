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
#ifndef EZBUS_MAC_BOOT2_H_
#define EZBUS_MAC_BOOT2_H_

#include <ezbus_platform.h>
#include <ezbus_mac.h>
#include <ezbus_mac_timer.h>
#include <ezbus_packet.h>
#include <ezbus_port.h>
#include <ezbus_crc.h>


typedef enum
{
    state_boot2_idle=0,
    state_boot2_start,
    state_boot2_active,
    state_boot2_stop,
    state_boot2_finished,

} ezbus_mac_boot2_state_t;


typedef struct _ezbus_mac_boot2_t
{
    ezbus_mac_boot2_state_t  state;
    uint8_t                  seq;                   /* always != 0 */
    ezbus_timer_t            timer;
    uint8_t                  cycles;
} ezbus_mac_boot2_t;


#ifdef __cplusplus
extern "C" {
#endif


extern void                     ezbus_mac_boot2_init            ( ezbus_mac_t* mac );
extern void                     ezbus_mac_boot2_run             ( ezbus_mac_t* mac );
extern uint8_t                  ezbus_mac_boot2_get_seq         ( ezbus_mac_t* mac );

extern void                     ezbus_mac_boot2_set_state       ( ezbus_mac_t* mac, ezbus_mac_boot2_state_t state );
extern ezbus_mac_boot2_state_t  ezbus_mac_boot2_get_state       ( ezbus_mac_t* mac );
    
extern const char*              ezbus_mac_boot2_get_state_str   ( ezbus_mac_t* mac );

extern void                     ezbus_mac_boot2_signal_idle     ( ezbus_mac_t* mac );
extern void                     ezbus_mac_boot2_signal_start    ( ezbus_mac_t* mac );
extern void                     ezbus_mac_boot2_signal_active   ( ezbus_mac_t* mac );
extern void                     ezbus_mac_boot2_signal_stop     ( ezbus_mac_t* mac );
extern void                     ezbus_mac_boot2_signal_finished ( ezbus_mac_t* mac );

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_MAC_BOOT2_H_ */
