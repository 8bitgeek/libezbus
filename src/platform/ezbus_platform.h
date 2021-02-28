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
#ifndef EZBUS_PLATFORM_H_
#define EZBUS_PLATFORM_H_

#include <ezbus_const.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ezbus_platform_port
{

} ezbus_platform_port_t;

typedef struct _ezbus_platform
{
    int             (*callback_open)            ( struct _ezbus_platform* platform, uint32_t speed );
    int             (*callback_send)            ( struct _ezbus_platform* platform, void* bytes, size_t size );
    int             (*callback_recv)            ( struct _ezbus_platform* platform, void* bytes, size_t size );
    void            (*callback_close)           ( struct _ezbus_platform* platform );
    void            (*callback_flush)           ( struct _ezbus_platform* platform );
    void            (*callback_drain)           ( struct _ezbus_platform* platform );
    int             (*callback_set_speed)       ( struct _ezbus_platform* platform, uint32_t speed );
    bool            (*callback_set_tx)          ( struct _ezbus_platform* platform, bool enable );

    void*           (*callback_memset)          ( void* dest, int c, size_t n );
    void*           (*callback_memcpy)          ( void* dest, const void *src, size_t n );
    void*           (*callback_memmove)         ( void* dest, const void *src, size_t n );
    int             (*callback_memcmp)          ( const void* dest, const void *src, size_t n );
    char*           (*callback_strcpy)          ( char* dest, const char *src );
    char*           (*callback_strcat)          ( char* dest, const char *src );
    char*           (*callback_strncpy)         ( char* dest, const char *src, size_t n );
    int             (*callback_strcmp)          ( const char* s1, const char *s2 );
    int             (*callback_strcasecmp)      ( const char* s1, const char *s2 );
    size_t          (*callback_strlen)          ( const char* s);
    void*           (*callback_malloc)          ( size_t n );
    void*           (*callback_realloc)         ( void* src, size_t n );
    void            (*callback_free)            ( void *src );

    int             (*callback_rand)            ( void );
    void            (*callback_srand)           ( unsigned int seed );
    int             (*callback_random)          ( int lower, int upper );
    void            (*callback_rand_init)       ( void );
    void            (*callback_delay)           ( unsigned int ms );
    void            (*callback_set_address)     ( const ezbus_address_t* address );
    void            (*callback_address)         ( ezbus_address_t* address );

    ezbus_ms_tick_t (*callback_get_ms_ticks)    (void);

} ezbus_platform_t;

extern int ezbus_platform_setup(ezbus_platform_t* ezbus_platform, int argc,char* argv[]);

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_PLATFORM_H_ */
