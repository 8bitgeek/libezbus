/******************************************************************************
 * Copyright © 2018 by @author Mike Sharkey <mike@8bitgeek.net>
 * All Rights Reserved
 *****************************************************************************/
#include "ezbus_platform.h"

#if defined(_CARIBOU_RTOS_)
	#include <ezbus_platform_caribou.c>
#elif defined(__linux__)
	#include <ezbus_platform_linux.c>
#else
	#error No platform.
#endif
