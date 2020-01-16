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
#ifndef EZBUS_PLATFORM_H_
#define EZBUS_PLATFORM_H_

#define _STM32_HAL_ 1   /* FIXME */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_CARIBOU_RTOS_)
    #include <ezbus_platform_caribou.h>
#elif defined(__linux__)
    #include <ezbus_platform_linux.h>
#else
    #error No platform supported.
#endif

#include <ezbus_const.h>


extern int      ezbus_platform_open        ( ezbus_platform_port_t* port, uint32_t speed );
extern int      ezbus_platform_send        ( ezbus_platform_port_t* port, void* bytes, size_t size );
extern int      ezbus_platform_recv        ( ezbus_platform_port_t* port, void* bytes, size_t size );
extern int      ezbus_platform_getc        ( ezbus_platform_port_t* port );
extern void     ezbus_platform_close       ( ezbus_platform_port_t* port );
extern void     ezbus_platform_flush       ( ezbus_platform_port_t* port );
extern void     ezbus_platform_drain       ( ezbus_platform_port_t* port );
extern int      ezbus_platform_set_speed   ( ezbus_platform_port_t* port, uint32_t speed );

extern void*    ezbus_platform_memset      ( void* dest, int c, size_t n );
extern void*    ezbus_platform_memcpy      ( void* dest, const void *src, size_t n );
extern void*    ezbus_platform_memmove     ( void* dest, const void *src, size_t n );
extern int      ezbus_platform_memcmp      ( const void* dest, const void *src, size_t n );
extern char*    ezbus_platform_strcpy      ( char* dest, const char *src );
extern char*    ezbus_platform_strcat      ( char* dest, const char *src );
extern char*    ezbus_platform_strncpy     ( char* dest, const char *src, size_t n );
extern size_t   ezbus_platform_strlen      ( const char* s);
extern void*    ezbus_platform_malloc      ( size_t n );
extern void*    ezbus_platform_realloc     ( void* src, size_t n );
extern void     ezbus_platform_free        ( void *src );

extern int      ezbus_platform_rand        ( void );
extern void     ezbus_platform_srand       ( unsigned int seed );
extern int      ezbus_platform_random      ( int lower, int upper );
extern void     ezbus_platform_rand_init   ( void );
extern void     ezbus_platform_delay       ( unsigned int ms );
extern void     ezbus_platform_set_address ( void* address, size_t size );
extern void     ezbus_platform_address     ( void* address );

extern ezbus_ms_tick_t  ezbus_platform_get_ms_ticks();

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_PLATFORM_H_ */
