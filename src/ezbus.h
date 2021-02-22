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
#ifndef EZBUS_H_
#define EZBUS_H_

/*! 
* @mainpage EzBus RS-485 Token Bus Protocol
* - ezBus, Where is it?\n
*   - http://github.com/8bitgeek/libezbus\n
* \n
* - eZBus, What is it?\n
*   - EzBus is a token-bus protocol designed specifically for the RS-485 bus physical layer.\n
*   - EzBus is functional, but still under development.\n
*   - EzBus can provide stream socket semantics to resource constrained architectures.\n
*   - EzBus like other token bus and token ring architectures, has deterministic properties.\n
*   - EzBus is a peer-to-peer topology, where all peers are created equal, there is no "master".\n
*   - EzBus nodes may join and leave the network at any time. The token-ring is "self-healing".\n
*   - EzBus is intended to be lightweight and simple enough to run on small memory footprint micro-controllers.\n
*   - EzBuz + CARIBOU RTOS + Minimal socket-echo app is currently using about 38K FLASH, and 8K of RAM on STM32F746.\n
*   - EzBus currently works with CARIBOU RTOS, Linux, can be easily ported to other targets.\n
*   - EzBus API is easy to use by implementing a small number of easy to use callback functions.\n
*   - EzBus is written in ANSI C and currently targets the GCC compiler.\n
* \n
*/

/**
 * @page ezBus Application API
 * Ezbus application API performs top level functions. The @ref ezbus_init() function must be
 * called to initialuze the @ref ezbus_t structure before @ref ezbus_run() may be used.
 * 
 * Example Usage:\n
 * \n
 *    @ref ezbus_port_t        port;\n
 *    @ref ezbus_t             ezbus;\n
 *    @ref ezbus_mac_t*        mac;\n
 *    \n
 *    int main(int argc, char* argv[])\n
 *    {\n
 *        @ref ezbus_port_init_struct( port );\n
 *        \n
 *        @ref ezbus_platform_port_set_name(port,EZBUS_USART_NO);\n
 *        @ref ezbus_platform_port_set_handle(port,NULL);\n
 *        @ref ezbus_platform_port_set_dir_gpio(port,&gpio_rs485_dir,NULL);\n
 *        \n
 *        if ( @ref ezbus_port_open( &port, @ref EZBUS_SPEED_DEF ) == @ref EZBUS_ERR_OKAY )\n
 *        {\n
 *            @ref ezbus_init( &ezbus, &port );\n
 *            mac = @ref ezbus_mac( &ezbus );\n
 *        }\n
 *        \n
 *        for(;;)\n
 *        {\n
 *            @ref ezbus_run( mac );\n
 *        }\n
 *    }\n
 *  
 */

#include <ezbus_mac_struct.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ezbus_t
{
    struct _ezbus_mac_t         mac;
    ezbus_port_t*               port;
} ezbus_t;

/**
 * @brief Initialize the @ref ezbus_t structure, using a populated @ref ezbus_port_t structure.
 * Must be called before @ref ezbus_run() may be used.
 */
extern void ezbus_init   ( ezbus_t* ezbus, ezbus_port_t* port );

/**
 * @brief Run the ezbus protocol. Must be called continuously, in a non-blocking loop. 
 */
extern void ezbus_run( ezbus_t* ezbus );

/**
 * @brief Retrieve the ezbus mac from an @ref ezbus_t struct.
 * @param ezbus A pointer to an initialized @ref ezbus_t structure. see: @ref ezbus_init().
 * @return A pointer to the @ezbus_mac_t structure.
 */ 
extern struct _ezbus_mac_t* ezbus_mac( ezbus_t* ezbus );

#ifdef __cplusplus
}
#endif


#endif /* EZBUS_H_ */
