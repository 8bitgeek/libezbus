/******************************************************************************
 * Copyright © 2018 by @author Mike Sharkey <mike@8bitgeek.net>
 * All Rights Reserved
 *****************************************************************************/
#include <stdio.h>
#include "ezbus_thread.h"


int main(int argc,char* argv[])
{
	if ( argc == 2 )
	{
		for(;;)
		{
			ezbus_thread(argv[1]);
		}
	}
	else
	{
		fprintf(stderr,"usage: %s /dev/ttyxxx\n", argv[0]);
	}
    return 0;
}
