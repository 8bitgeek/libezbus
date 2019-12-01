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
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <ezbus.h>
#include <ezbus_flip.h>
#include <ezbus_port.h>

ezbus_port_t    port;
ezbus_t         ezbus;
ezbus_address_t address;

void run(void* arg)
{
    char* serial_port_name = (char*)arg;

    ezbus_port_init_struct( &port );

    ezbus_platform_port_set_name(&port,serial_port_name);
    ezbus_platform_port_set_handle(&port,-1);

    if ( ezbus_port_open( &port, ezbus_port_speeds[EZBUS_SPEED_INDEX_DEF] ) )
    {
        ezbus_init( &ezbus, &port );
        ezbus_platform_address( &address );

        fprintf( stderr, "ID:%08X%08X%08X\n",
                ezbus_flip32(address.word[0]),
                ezbus_flip32(address.word[1]),
                ezbus_flip32(address.word[2]) );

        for(;;) /* forever... */
        {
            ezbus_run(&ezbus);
        }
    }
}


int main(int argc,char* argv[])
{
    if ( argc == 2 )
    {
        for(;;)
        {
            run(argv[1]);
        }
    }
    else
    {
        fprintf(stderr,"usage: %s /dev/ttyxxx\n", argv[0]);
    }
    return 0;
}
