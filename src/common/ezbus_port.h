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
#ifndef EZBUS_PORT_H_
#define EZBUS_PORT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <ezbus_types.h>
#include <ezbus_packet.h>

typedef struct _ezbus_port
{
    void*           private;

    int                     (*callback_open)        (struct _ezbus_port* port );
    int                     (*callback_send)        (struct _ezbus_port* port, void* bytes, size_t size );
    int                     (*callback_recv)        (struct _ezbus_port* port, void* bytes, size_t size );
    void                    (*callback_close)       (struct _ezbus_port* port );
    void                    (*callback_flush)       (struct _ezbus_port* port );
    void                    (*callback_drain)       (struct _ezbus_port* port );
    int                     (*callback_getch)       (struct _ezbus_port* port );
    int                     (*callback_set_speed)   (struct _ezbus_port* port, uint32_t speed );
    uint32_t                (*callback_get_speed)   (struct _ezbus_port* port );
    bool                    (*callback_set_tx)      (struct _ezbus_port* port, bool enable );
    void                    (*callback_set_address) (struct _ezbus_port* port, const ezbus_address_t* address );
    const ezbus_address_t*  (*callback_get_address) (struct _ezbus_port* port );

    uint32_t        packet_timeout;
    uint32_t        rx_err_crc_count;
    uint32_t        rx_err_timeout_count;
    uint32_t        rx_err_overrun_count;
    uint32_t        tx_err_overrun_count;
    uint32_t        tx_err_retry_fail_count;
    
    ezbus_address_t self_address;

} ezbus_port_t;

extern int                      ezbus_port_setup                    ( ezbus_port_t* port );
extern void                     ezbus_port_dispose                  ( ezbus_port_t* port );
extern EZBUS_ERR                ezbus_port_open                     ( ezbus_port_t* port );
extern EZBUS_ERR                ezbus_port_send                     ( ezbus_port_t* port, ezbus_packet_t* packet );
extern EZBUS_ERR                ezbus_port_recv                     ( ezbus_port_t* port, ezbus_packet_t* packet );
extern void                     ezbus_port_close                    ( ezbus_port_t* port );
extern void                     ezbus_port_drain                    ( ezbus_port_t* port );
extern int                      ezbus_port_getch                    ( ezbus_port_t* port );
extern void                     ezbus_port_set_speed                ( ezbus_port_t* port, uint32_t speed );
extern uint32_t                 ezbus_port_get_speed                ( ezbus_port_t* port );
extern void                     ezbus_port_set_address              ( ezbus_port_t* port, const ezbus_address_t* address );
extern const ezbus_address_t*   ezbus_port_get_address              ( ezbus_port_t* port );
extern bool                     ezbus_port_get_address_is_self      ( ezbus_port_t* port, const ezbus_address_t* address );
extern uint32_t                 ezbus_port_byte_time_ns             ( ezbus_port_t* port );
extern uint32_t                 ezbus_port_packet_timeout_time_ms   ( ezbus_port_t* port );
extern void                     ezbus_port_dump                     ( ezbus_port_t* port, const char* prefix );

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_PORT_H_ */
