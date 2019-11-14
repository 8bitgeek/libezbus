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
#ifndef EZBUS_ADDRESS_H_
#define EZBUS_ADDRESS_H_

#include <ezbus_platform.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    ezbus_address_t**   list;
    uint8_t             count;
} ezbus_address_list_t;


extern const ezbus_address_t ezbus_broadcast_address;
extern const ezbus_address_t ezbus_controller_address;

extern int      ezbus_address_compare   ( const ezbus_address_t* a, const ezbus_address_t* b );
extern uint8_t* ezbus_address_copy      ( ezbus_address_t* dst, const ezbus_address_t* src );
extern void     ezbus_address_swap      ( ezbus_address_t* dst, ezbus_address_t* src );
extern char*    ezbus_address_string    ( ezbus_address_t* address, char* string );
extern void     ezbus_address_dump      ( const ezbus_address_t* address, const char* prefix );

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_ADDRESS_H_ */
