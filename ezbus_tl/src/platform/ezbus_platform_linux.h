/******************************************************************************
 * Copyright © 2018 by @author Mike Sharkey <mike@8bitgeek.net>
 * All Rights Reserved
 *****************************************************************************/
#ifndef EZBUS_PLATFORM_LINUX_H_
#define EZBUS_PLATFORM_LINUX_H_

#include "ezbus_const.h"
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
	char*			serial_port_name;
	int				fd;
} ezbus_platform_port_t;

typedef uint64_t ezbus_ms_tick_t;

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_PLATFORM_LINUX_H_ */
