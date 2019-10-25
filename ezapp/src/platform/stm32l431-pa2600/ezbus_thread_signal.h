/******************************************************************************
 * Copyright © 2019 by @author Mike Sharkey <mike@8bitgeek.net>
 * All Rights Reserved
 *****************************************************************************/
#ifndef __EZBUS_THREAD_SIGNAL_H_
#define __EZBUS_THREAD_SIGNAL_H_

#include <ezbus_thread.h>
#include <board.h>

extern void ezbus_thread_signal_init ( void );
extern void ezbus_thread_signal_run  ( ezbus_instance_t* ezbus_instance );
extern void ezbus_thread_signal_disco( void );

#endif
