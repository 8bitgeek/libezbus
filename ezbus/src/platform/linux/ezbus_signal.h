/******************************************************************************
 * Copyright © 2018 by @author Mike Sharkey <mike@8bitgeek.net>
 * All Rights Reserved
 *****************************************************************************/
#ifndef __EZBUS_UNIX_SIGNAL_H_
#define __EZBUS_UNIX_SIGNAL_H_

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <ezbus_instance.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void ezbus_signal_init( ezbus_instance_t* ezbus_instance );
extern void ezbus_signal_run ( void );

#ifdef __cplusplus
}
#endif

#endif

