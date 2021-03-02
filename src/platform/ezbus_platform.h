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

#include <ezbus_types.h>
#include <ezbus_const.h>
#include <ezbus_address.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ezbus_platform
{
    /* cmdline */
    void*           cmdline;
    /* public platform */
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
    ezbus_ms_tick_t (*callback_get_ms_ticks)    (void);

} ezbus_platform_t;

extern ezbus_platform_t ezbus_platform;

extern int ezbus_platform_setup(void* cmdline_obj);

#define ezbus_platform_get_cmdline(ezbus_platform) ((ezbus_cmdline_t*)ezbus_platform.cmdline)

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_PLATFORM_H_ */
