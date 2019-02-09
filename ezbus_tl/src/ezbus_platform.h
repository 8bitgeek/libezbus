/******************************************************************************
 * Copyright © 2018 by @author Mike Sharkey <mike@8bitgeek.net>
 * All Rights Reserved
 *****************************************************************************/
#ifndef EZBUS_PLATFORM_H_
#define EZBUS_PLATFORM_H_

#include "ezbus_const.h"

#define _STM32_HAL_	1	/* FIXME */


#ifdef __cplusplus
extern "C" {
#endif

#if defined(_CARIBOU_RTOS_)

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

#else
	#error No platform supported.
#endif

extern int	ezbus_platform_open(ezbus_platform_port_t* port,uint32_t speed);
extern int	ezbus_platform_send(ezbus_platform_port_t* port,void* bytes,size_t size);
extern int	ezbus_platform_recv(ezbus_platform_port_t* port,void* bytes,size_t size);
extern int	ezbus_platform_getc(ezbus_platform_port_t* port);
extern void	ezbus_platform_close(ezbus_platform_port_t* port);
extern void	ezbus_platform_flush(ezbus_platform_port_t* port);
extern void	ezbus_platform_drain(ezbus_platform_port_t* port);
extern void	ezbus_platform_set_speed(ezbus_platform_port_t* port,uint32_t speed);

extern ezbus_ms_tick_t 	ezbus_platform_get_ms_ticks();

extern void* ezbus_platform_memset(void* dest, int c, size_t n);
extern void* ezbus_platform_memcpy(void* dest, const void *src, size_t n);
extern void* ezbus_platform_memmove(void* dest, const void *src, size_t n);
extern int 	 ezbus_platform_memcmp(const void* dest, const void *src, size_t n);
extern void* ezbus_platform_malloc(size_t n);
extern void* ezbus_platform_realloc(void* src,size_t n);
extern void* ezbus_platform_free(const void *src);

extern void	ezbus_platform_address(uint8_t* address);

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_PLATFORM_H_ */
