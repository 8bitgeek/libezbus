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
#ifndef _CARIBOU_CONFIG_H_
#define _CARIBOU_CONFIG_H_


#ifndef CONSOLE_USART
	#define CONSOLE_USART		0
#endif

#ifndef DEBUG_USART
	#define DEBUG_USART			1
#endif

/**
 * @brief Uncomment if used LwIP TCP/IP stack.
 */
//#define	CARIBOU_LWIP

/**
 * @brief Uncomment if using IRQ-safe CARIBOU lib functions. The trade off is with respect
 * to interrupt latency performance, as interrupts must be disabled during critical regions when
 * CARIBOU_LIBIRQ_SAFE is enabled. When CARIBOU_LIB_IRQ_SAFE is disabled, then CARIBOU library 
 * functions may not be accessed from interrupt context.
 */
#define	CARIBOU_LIB_IRQ_SAFE 0

/**
 * @brief Uncomment of there is a hardware FPU present.
 */
/* #define CARIBOU_FPU_PRESENT */

/**
 * @brief Uncomment in order to use the *EXPERIMENTAL* hand-assembled library functions.
 * May provide some performance gains particularly when doing debug build.
 */
/* #define	CARIBOU_ARM_ASM */

/**
 * @brief Define the type accepted by the compiler as the argument to the new() operator.
 */
#define NEW_TAKES_SIZE_T
/* #define NEW_TAKES_ULONG */

/** ----- No Modification below this line -------------- **/

#if defined(CARIBOU_FPU_PRESENT)
	#if defined(__FPU_PRESENT)
		#undef __FPU_PRESENT
	#endif
	#if defined(__FPU_USED)
		#undef __FPU_USED
	#endif
	#define __FPU_PRESENT	1	/* Some platform headers depend on this symbol being defined */
	#define _FPU_USED		1
#endif

#endif /* _CARIBOU_CONFIG_H_ */
