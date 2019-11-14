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
#include <ezbus_signal.h>
#include <ezbus_peer.h>
#include <ezbus_platform.h>
#include <string.h>

ezbus_driver_t* driver=NULL;

static  void ezbus_signal_handler ( int signo );
static  bool disco_callback       ( ezbus_driver_t* driver);

extern void ezbus_signal_init( ezbus_driver_t* ezbus_driver )
{
    driver = ezbus_driver;

    signal( SIGUSR1, ezbus_signal_handler );
    signal( SIGUSR2, ezbus_signal_handler );
    signal( SIGINT , ezbus_signal_handler );
}

static void ezbus_signal_handler(int signo)
{
	switch(signo)
	{
		case SIGUSR1:
			exit(0);
	        break;
		case SIGUSR2:
		case SIGINT:
			ezbus_driver_disco( driver, EZBUS_DISCO_COUNT, disco_callback );
			break;
		default:
			break;
	}
}

static  bool disco_callback( ezbus_driver_t* driver)
{
	fprintf( stderr, "disco_callback()\n" );
	return true;
}
