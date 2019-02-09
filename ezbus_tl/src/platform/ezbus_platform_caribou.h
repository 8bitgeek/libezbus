/******************************************************************************
 * Copyright © 2018 by @author Mike Sharkey <mike@8bitgeek.net>
 * All Rights Reserved
 *****************************************************************************/
#ifndef EZBUS_PLATFORM_CARIBOU_H_
#define EZBUS_PLATFORM_CARIBOU_H_

#include "ezbus_const.h"

#define _STM32_HAL_	1	/* FIXME */


#ifdef __cplusplus
extern "C" {
#endif

#include <caribou/kernel/timer.h>
#include <caribou/lib/stdio.h>
#include <caribou/lib/string.h>
#include <caribou/dev/gpio.h>

typedef struct
{
	int				serial_port_no;
	caribou_gpio_t*	dir_pin;
	FILE*			fd;
} ezbus_platform_port_t;
typedef caribou_tick_t ezbus_ms_tick_t;

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_PLATFORM_CARIBOU_H_ */
