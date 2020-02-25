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
#ifndef EZBUS_PORT_H_
#define EZBUS_PORT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <ezbus_platform.h>
#include <ezbus_packet.h>


extern uint32_t ezbus_port_speeds[EZBUS_SPEED_COUNT];

typedef struct
{
    ezbus_platform_port_t   platform_port;
    uint32_t                speed;
    uint32_t                packet_timeout;
    uint32_t                rx_err_crc_count;
    uint32_t                rx_err_timeout_count;
    uint32_t                rx_err_overrun_count;
    uint32_t                tx_err_overrun_count;
    uint32_t                tx_err_retry_fail_count;
} ezbus_port_t;

#define ezbus_port_getch(port) ezbus_platform_getc(&(port)->platform_port)

extern void         ezbus_port_init_struct            ( ezbus_port_t* port );
extern EZBUS_ERR    ezbus_port_open                   ( ezbus_port_t* port, uint32_t speed );
extern EZBUS_ERR    ezbus_port_send                   ( ezbus_port_t* port, ezbus_packet_t* packet );
extern EZBUS_ERR    ezbus_port_recv                   ( ezbus_port_t* port, ezbus_packet_t* packet );
extern void         ezbus_port_close                  ( ezbus_port_t* port );
extern void         ezbus_port_drain                  ( ezbus_port_t* port );
extern void         ezbus_port_set_speed              ( ezbus_port_t* port, uint32_t speed );
extern uint32_t     ezbus_port_get_speed              ( ezbus_port_t* port );
extern uint32_t     ezbus_port_byte_time_ns           ( ezbus_port_t* port );
extern uint32_t     ezbus_port_packet_timeout_time_ms ( ezbus_port_t* port );

extern void         ezbus_port_dump                   ( ezbus_port_t* port, const char* prefix );


#ifdef __cplusplus
}
#endif

#endif /* EZBUS_PORT_H_ */
