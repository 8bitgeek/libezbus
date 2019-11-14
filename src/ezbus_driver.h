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
#ifndef EZBUS_DRIVER_H_
#define EZBUS_DRIVER_H_

#include <ezbus_platform.h>
#include <ezbus_port.h>
#include <ezbus_packet.h>
#include <ezbus_address.h>
#include <ezbus_packet_queue.h>
#include <ezbus_peer.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef enum 
{
    ezbus_tx_state_empty=0,
    ezbus_tx_state_busy,
    ezbus_tx_state_full
} ezbus_tx_state_t;

typedef void (*ezbus_tx_callback_t)(ezbus_tx_state_t);


typedef struct 
{
    ezbus_peer_list_t       peers;                  
    ezbus_ms_tick_t         start;
    uint8_t                 seq;                
} ezbus_disco_state_t;

typedef struct
{
    ezbus_packet_t  packet;
    EZBUS_ERR       err;
    uint8_t         seq;
} ezbus_packet_state_t;

typedef struct
{
    ezbus_address_t         address;                
    ezbus_port_t            port;                   
    ezbus_packet_state_t    rx_state;               
    ezbus_packet_state_t    tx_state;               
    ezbus_packet_queue_t*   tx_queue;               
    uint8_t                 tx_seq;     
} ezbus_packet_io_t;

typedef void (*ezbus_rx_callback_t)(ezbus_packet_io_t*);


typedef struct
{
    ezbus_disco_state_t         disco;
    ezbus_packet_io_t           io;                 
    ezbus_rx_callback_t         rx_callback;
    ezbus_tx_callback_t         tx_callback;
} ezbus_driver_t;

typedef bool (*ezbus_disco_callback_t)(ezbus_driver_t*);


/**
 * @brief Service the driver by providing some run time.
 * @param driver Pointer to an initialized driver object.
 */
extern void      ezbus_driver_run         ( ezbus_driver_t* driver );

/**
 * @brief Initialize the driver for use.
 * @param driver Initialized structure and populated.
 * @param speed Must be one of ezbus_port_speeds[].
 * @param tx_queue_limit Queue size limit number of pending transmit packets.
 */
extern EZBUS_ERR ezbus_driver_init        ( ezbus_driver_t* driver, ezbus_platform_port_t* platform_port, uint32_t speed, uint32_t tx_queue_limit );

extern void      ezbus_driver_deinit      ( ezbus_driver_t* driver );

extern void      ezbus_driver_set_rx_cb   ( ezbus_driver_t* driver, ezbus_rx_callback_t rx_callback );
extern void      ezbus_driver_set_tx_cb   ( ezbus_driver_t* driver, ezbus_tx_callback_t tx_callback );

extern void      ezbus_driver_disco       ( ezbus_driver_t* driver, uint32_t cycles, ezbus_disco_callback_t progress_callback );

extern void      ezbus_driver_dump        ( ezbus_driver_t* driver );

#ifdef __cplusplus
}
#endif


#endif /* EZBUS_DRIVER_H_ */
