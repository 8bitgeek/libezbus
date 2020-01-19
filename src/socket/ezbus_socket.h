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
#ifndef _EZBUS_SOCKET_H_
#define _EZBUS_SOCKET_H_

/**
 * @brief This API implements an interface for using transciever channels to exchange data packets
 *          with other ezbus nodes. Such a channel is termed a `socket`.
 *          One `socket` replresents a signular tranceiver channel between this node, and a
 *          single peer. A `socket` must be 'opened' before it can be used. See @ref ezbus_socket_open()
 *          for more details. 
 *          In the case that an unsolicited parcel packet arrives from a peer (socket==EXBUS_SICKET_ANY), 
 *          a local `socket` will automatically be opened prior to @ref ezbus_socket_callback_recv() 
 *          being invoked. The socket remain opened until closed by the consumer of this API.
 *          Be sure to review @ref ezbus_socket_init(), @ref ezbus_socket_callback_send() and 
 *          @ref ezbus_socket_callback_recv()
 */

#include <ezbus_platform.h>
#include <ezbus_packet.h>
#include <ezbus_mac.h>
#include <ezbus_fault.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The consumer of this API is required to implement @ref ezbus_socket_callback_send(), 
 *          see @ref ezbus_socket_send() for details.
 */
extern bool ezbus_socket_callback_send ( ezbus_socket_t socket );


/**
 * @brief The consumer of this API is required to implement @ref ezbus_socket_callback_recv(), 
 *          see @ref ezbus_socket_recv() for details.
 */
extern bool ezbus_socket_callback_recv ( ezbus_socket_t socket );


/**
 * @brief Initializes the socket layer, and must be invoked once prior to using any socket functions
 *          Invocation must take place from with the thre ad context of the consumer of the API.
 */
extern void ezbus_socket_init ( void );


/** 
 * @brief @ref ezbus_socket_send() must be sync'ed with @ref ezbus_tranceiver_callback_send() 
 *          When @ref ezbus_tranceiver_callback_send() is invoked, and consumer has data to send,
 *          then consumer should invoke @ref ezbus_socket_send(). Bye return `true` from  
 *          @ref ezbus_tranceiver_callback_send(), the consumer signals the desire to have the packet transmitted,
 *          otherwise the prepared parcel packet will remain dormant until such time that 'true' is
 *          returned by @ref ezbus_tranceiver_callback_send().
 * @param socket Integer value that references the given socket connection, @ref ezbus_socket_open()
 * @param data Pointer to the data bytes to transmit. May be of arbitrary length, however, all of the 
 *          data bytes may not be transmitted, see return value for usage suggestion.
 * @param size Represents the total nbumber of bytes to send.
 * @return The number of bytes sent. If return is < `size` and > 0 tis indicates that the transmission
 *          was successful, however not all bytes where transmitted. In this case, the consumer
 *          may choose to use the return value to continue to transmit the remaining bytes.
 *          If 0 is returned, then the socket was unable to transmit the bytes at this time (be sure
 *          to synchronize @ref ezbus_tranceiver_send() with ezbus_tranceiver_callback_send() to ensure
 *          that it is an appropriate time to populate a transmitter packet. If -1 is returned, then
 *          a fault has occured, and @ezbus_socket_err() will return the nature of the failure.
 */
extern int ezbus_socket_send ( ezbus_socket_t socket, void* data, size_t size );

/**
 * @brief @ref ezbus_socket_recv() must be sync'ed with @ref ezbus_tranceiver_callback_recv() 
 *          When @ref ezbus_tranceiver_callback_recv() is invoked, and the consumer wishes to receive
 *          the data, then the consumer should invoke @ref ezbus_socket_recv() to extract the packet
 *          parcel data. Returning `true` will initiate an `ack` response to the received parcel packet.
 *          otherwise a `nack` response.
 * @param socket Integer value that references the given socket connection, @ref ezbus_socket_open()
 *          The local socket to reply on will be resocketed by invoking @ref ezbus_packet_dst_socket().
 * @param data Pointer to the destination storage for received bytes. Storage must be large enough to store `size` bytes.
 * @param size The maximum number of bytes to extract from the parcel packet.
 * @return The number of bytes actually copied to `data`. 
 */
extern int ezbus_socket_recv ( ezbus_socket_t socket, void* data, size_t size );

/**
 * @brief Open a tranceiver channel (socket) with a peer node.
 * @param mac The MAC interface instance to use for this socket connection.
 * @param peer The peer node address to connect this socket to.
 * @return A new socket handle, or EZBUS_SOCKET_INVALID if the request could not be successfuly 
 *          completed. See @ref ezbus_socket_err(). Most likely cause of faulure is that
 *          @ref ezbus_socket_count() has reached @ref ezbus_socket_mac(). A finite number
 *          of sockets may be open at any one time, as defined by @ref EZBUS_NUM_SOCKETS.
 *          When an unsolicited packet arrives when no more sockets are available, the
 *          incoming parcel packet is rejected with a 'nack'.
 */
extern ezbus_socket_t ezbus_socket_open ( ezbus_mac_t* mac, ezbus_address_t* peer );

/**
 * @brief Close a previously opened socket. Once invoked, the socket can no longer be
 *          referenced. See also @ref ezbus_socket_open()
 */
extern void ezbus_socket_close ( ezbus_socket_t socket );

/**
 * @brief Used to determine if a given socket is currently open and available for use.
 */
extern bool ezbus_socket_is_open ( ezbus_socket_t socket );


#ifdef __cplusplus
}
#endif

#endif /* _EZBUS_SOCKET_H_ */
