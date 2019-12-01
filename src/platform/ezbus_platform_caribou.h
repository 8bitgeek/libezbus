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
#ifndef EZBUS_PLATFORM_CARIBOU_H_
#define EZBUS_PLATFORM_CARIBOU_H_

#include "ezbus_const.h"

#define _STM32_HAL_ 1   /* FIXME */


#ifdef __cplusplus
extern "C" {
#endif

#include <caribou/kernel/timer.h>
#include <caribou/lib/stdio.h>
#include <caribou/lib/string.h>
#include <caribou/dev/gpio.h>

typedef struct
{
    int             serial_port_no;
    caribou_gpio_t* dir_pin;
    FILE*           fd;
} ezbus_platform_port_t;
typedef caribou_tick_t ezbus_ms_tick_t;

#define ezbus_platform_port_set_name(p,n)       ((p)->platform_port.serial_port_no=(n))
#define ezbus_platform_port_get_name(p)         ((p)->platform_port.serial_port_no)

#define ezbus_platform_port_set_handle(p,h)     ((p)->platform_port.fd=(h))
#define ezbus_platform_port_get_handle(p)       ((p)->platform_port.fd)

#define ezbus_platform_port_set_dir_gpio(p,d)   ((p)->platform_port.dir_pin=(d))
#define ezbus_platform_port_get_dir_gpio(p)     ((p)->platform_port.dir_pin)

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_PLATFORM_CARIBOU_H_ */
