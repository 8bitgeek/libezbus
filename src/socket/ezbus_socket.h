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
#ifndef EZBUS_SOCKET_T
#define EZBUS_SOCKET_T

#include <ezbus_platform.h>
#include <ezbus_packet.h>
#include <ezbus_mac.h>
#include <ezbus_fault.h>

#ifdef __cplusplus
extern "C" {
#endif


/** 
 * @brief @ref ezbus_socket_send() must be sync'ed with @ref ezbus_tranceiver_callback_send() 
 *          When @ref ezbus_tranceiver_callback_send() is invoked, and consumer has data to send,
 *          then consumer should invoke @ref ezbus_socket_send(). Bye return `true` from  
 *          @ref ezbus_tranceiver_callback_send(), the consumer signals the desire to have the packet transmitted,
 *          otherwise the prepared parcel packet will remain dormant until such time that 'true' is
 *          returned by @ref ezbus_tranceiver_callback_send().
 * @param port Integer value that references the given port connection, @ref ezbus_socket_open()
 * @param data Pointer to the data bytes to transmit. May be of arbitrary length, however, all of the 
 *          data bytes may not be transmitted, see return value for usage suggestion.
 * @param size Represents the total nbumber of bytes to send.
 * @return The number of bytes sent. If return is < `size` and > 0 tis indicates that the transmission
 *          was successful, however not all bytes where transmitted. In this case, the consumer
 *          may choose to use the return value to continue to transmit the remaining bytes.
 *          If 0 is returned, then the port was unable to transmit the bytes at this time (be sure
 *          to synchronize @ref ezbus_tranceiver_send() with ezbus_tranceiver_callback_send() to ensure
 *          that it is an appropriate time to populate a transmitter packet. If -1 is returned, then
 *          a fault has occured, and @ezbus_socket_err() will return the nature of the failure.
 */
extern int              ezbus_socket_send         ( ezbus_socket_t port, void* data, size_t size );

/**
 * @brief @ref ezbus_socket_recv() must be sync'ed with @ref ezbus_tranceiver_callback_recv() 
 *          When @ref ezbus_tranceiver_callback_recv() is invoked, and the consumer wishes to receive
 *          the data, then the consumer should invoke @ref ezbus_socket_recv() to extract the packet
 *          parcel data. Returning `true` will initiate an `ack` response to the received parcel packet.
 *          otherwise a `nack` response.
 * @param port Integer value that references the given port connection, @ref ezbus_socket_open()
 *          The local port to reply on will be reported by invoking @ref ezbus_packet_dst_port().
 * @param data Pointer to the destination storage for received bytes. Storage must be large enough to store `size` bytes.
 * @param size The maximum number of bytes to extract from the parcel packet.
 * @return The number of bytes actually copied to `data`. 
 */
extern int              ezbus_socket_recv         ( ezbus_socket_t port, void* data, size_t size );


/**
 * @brief Initializes the trnasceiver, must be invoked once prior to using any tranceiver functions
 */
extern void             ezbus_socket_init         ( void );

/**
 * @brief Provides run-time to the port. Should be invoked in the same thread context
 *        as the consumer of the port API.
 */
extern void             ezbus_socket_run          ( void );


extern ezbus_socket_t   ezbus_socket_open         ( ezbus_mac_t* mac, ezbus_address_t* peer );
extern void             ezbus_socket_close        ( ezbus_socket_t port );
extern bool             ezbus_socket_is_open      ( ezbus_socket_t port );
extern EZBUS_ERR        ezbus_socket_err          ( ezbus_socket_t port );


extern ezbus_mac_t*     ezbus_socket_mac          ( ezbus_socket_t port );

extern ezbus_packet_t*  ezbus_socket_tx_packet    ( ezbus_socket_t port );
extern uint8_t          ezbus_socket_tx_seq       ( ezbus_socket_t port );

extern ezbus_packet_t*  ezbus_socket_rx_packet    ( ezbus_socket_t port );
extern uint8_t          ezbus_socket_rx_seq       ( ezbus_socket_t port );

extern void             ezbus_socket_set_tx_seq   ( ezbus_socket_t port, uint8_t seq);
extern void             ezbus_socket_set_rx_seq   ( ezbus_socket_t port, uint8_t seq);

extern void             ezbus_socket_reset_err    ( ezbus_socket_t port );

#define                 ezbus_socket_count()      EZBUS_NUM_PORTS

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_SOCKET_T */
