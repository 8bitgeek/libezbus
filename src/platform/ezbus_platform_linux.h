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
#ifndef EZBUS_PLATFORM_LINUX_H_
#define EZBUS_PLATFORM_LINUX_H_

#include <ezbus_const.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
    char*           serial_port_name;
    int             fd;
} ezbus_platform_port_t;

typedef uint64_t ezbus_ms_tick_t;

#define ezbus_platform_port_set_name(p,n)   ((p)->platform_port.serial_port_name=(n))
#define ezbus_platform_port_get_name(p)     ((p)->platform_port.serial_port_name)

#define ezbus_platform_port_set_handle(p,h) ((p)->platform_port.fd=(h))
#define ezbus_platform_port_get_handle(p)   ((p)->platform_port.platform_port.fd)

void ezbus_platform_port_dump( ezbus_platform_port_t* platform_port, const char* prefix );

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_PLATFORM_LINUX_H_ */
