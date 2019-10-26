/******************************************************************************
 * Copyright © 2018 by @author Mike Sharkey <mike@8bitgeek.net>
 * All Rights Reserved
 *****************************************************************************/
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <ezbus_thread.h>

#include "ezbus_thread.h"


int main(int argc,char* argv[])
{
	if ( argc == 2 )
	{
		for(;;)
		{
			ezbus_thread_run(argv[1]);
		}
	}
	else
	{
		fprintf(stderr,"usage: %s /dev/ttyxxx\n", argv[0]);
	}
    return 0;
}
