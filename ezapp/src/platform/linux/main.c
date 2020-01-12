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


static uint8_t  hex_to_mybble( char ch );
static uint8_t  hex_to_byte( char* s );
static void     set_address( char* s );
static void     run(void* arg);

static void run(void* arg)
{
    char* serial_port_name = (char*)arg;
    
    ezbus_port_init_struct( &port );

    ezbus_platform_port_set_name(&port,serial_port_name);
    ezbus_platform_port_set_handle(&port,-1);

    if ( ezbus_port_open( &port, ezbus_port_speeds[EZBUS_SPEED_INDEX_DEF] ) == EZBUS_ERR_OKAY )
    {
        ezbus_init( &ezbus, &port );

        for(;;) /* forever... */
        {
            ezbus_run(&ezbus);
        }
    }
}

static uint8_t hex_to_mybble( char ch )
{
    if ( ch >= 'a' && ch <= 'z' )
        ch -= 0x20;
    if ( ch >= 'A' && ch <= 'Z' )
        ch = ch -'A' + 10;
    return ch & 0x0F; 
}

static uint8_t hex_to_byte( char* s )
{
    return (hex_to_mybble(s[0])<<4) | hex_to_mybble(s[1]);
}

static void set_address( char* s )
{
    uint8_t address[sizeof(uint32_t)*4];
    uint32_t i=0;
    for(int n=0; n < ezbus_platform_strlen(s); n+=2 )
    {
        address[i++] = hex_to_byte(&s[n]);
    }
    ezbus_platform_set_address(address,sizeof(ezbus_address_t));
}

int main(int argc,char* argv[])
{
    if ( argc == 2 )
    {
        run(argv[1]);
    }
    if ( argc == 3 )
    {
        set_address(argv[1]);
        run(argv[2]);
    }
    else
    {
        fprintf(stderr,"usage: %s [XXXXXXXXXXXXXXXXXXXXXXXX] /dev/ttyxxx\n", argv[0]);
    }
    return 0;
}
