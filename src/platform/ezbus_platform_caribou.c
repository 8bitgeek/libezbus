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
#include <ezbus_platform_caribou.h>
#include <ezbus_address.h>
#include <caribou/lib/stdio.h>
#include <caribou/dev/uart.h>
#include <caribou/lib/heap.h>
#include <caribou/lib/rand.h>
#include <caribou/lib/uuid.h>

#define EZBUS_PACKET_DEBUG  1

int ezbus_platform_open(ezbus_platform_port_t* port,uint32_t speed)
{
    if (( port->fd = fopen(port->serial_port_no,"rw"))!=NULL)
    {
        ezbus_platform_set_speed(port,speed);
        return 0;
    }
    return -1;
}

int ezbus_platform_send(ezbus_platform_port_t* port,void* bytes,size_t size)
{
    uint8_t* p = (uint8_t*)bytes;
    size_t sent=0;
    #if EZBUS_PACKET_DEBUG
        ezbus_hex_dump( "TX:", p, size );
    #endif
    do {
        sent += fwrite(p,1,size-sent,port->fd);
        p = (uint8_t*)bytes;
        p = &p[sent];
    } while (sent<size);
    return sent;
}

int ezbus_platform_recv(ezbus_platform_port_t* port,void* bytes,size_t size)
{
    int rc = fread(bytes,1,size,port->fd);
    return rc;
}

int ezbus_platform_getc(ezbus_platform_port_t* port)
{
    int ch = fgetc(port->fd);
    return ch;
}

void ezbus_platform_close(ezbus_platform_port_t* port)
{
    fclose(port->fd);
}

void ezbus_platform_drain(ezbus_platform_port_t* port)
{
    while(ezbus_platform_getc(port)>=0);
}

int ezbus_platform_set_speed(ezbus_platform_port_t* port,uint32_t speed)
{
    caribou_uart_config_t config;
    caribou_uart_init_config(&config);
    config.baud_rate = speed;
    config.flow_control = CARIBOU_UART_FLOW_RS485_GPIO;
    config.gpio = port->dir_pin;
    caribou_uart_set_config(port->serial_port_no,&config);
    return 0;
}

void ezbus_platform_flush(ezbus_platform_port_t* port)
{
    fflush(port->fd);
}

void* ezbus_platform_memset(void* dest, int c, size_t n)
{
    return memset(dest,c,n);
}

void* ezbus_platform_memcpy(void* dest, const void *src, size_t n)
{
    return memcpy(dest,src,n);
}

void* ezbus_platform_memmove(void* dest, const void *src, size_t n)
{
    return memmove(dest,src,n);
}

int ezbus_platform_memcmp(const void* dest, const void *src, size_t n)
{
    return memcmp(dest,src,n);
}

char* ezbus_platform_strcpy( char* dest, const char *src )
{
    return strcpy( dest, src );
}

char* ezbus_platform_strncpy( char* dest, const char *src, size_t n )
{
    return strncpy( dest, src, n );
}

size_t ezbus_platform_strlen ( const char* s)
{
    return strlen( s );
}

void* ezbus_platform_malloc( size_t n)
{
    return malloc(n);
}

void* ezbus_platform_realloc(void* src,size_t n)
{
    return realloc(src,n);
}

void ezbus_platform_free(void *src)
{
    free(src);
}

ezbus_ms_tick_t ezbus_platform_get_ms_ticks()
{
    return caribou_timer_ticks();
}

int ezbus_platform_rand(void)
{
    return rand();
}

void ezbus_platform_srand(unsigned int seed)
{
    srand(seed);
}

int ezbus_platform_random(int lower, int upper)
{
    int num = (rand() % (upper - lower + 1)) + lower;
    return num;
}

void  ezbus_platform_rand_init (void)
{
    ezbus_platform_srand( ezbus_platform_get_ms_ticks()&0xFFFFFFFF );
}

void ezbus_platform_delay(unsigned int ms)
{
    ezbus_ms_tick_t start = ezbus_platform_get_ms_ticks();
    while ( (ezbus_platform_get_ms_ticks() - start) < ms )
    {
        caribou_thread_yield();
    }
}

void ezbus_platform_address(ezbus_address_t* address)
{
    caribou_get_uuid(address->word);
}

void ezbus_platform_port_dump( ezbus_platform_port_t* platform_port, const char* prefix )
{
    fprintf(stderr, "%s.serial_port_no=%d\n", prefix, platform_port->serial_port_no );
    fprintf(stderr, "%s.dir_pin=%08X\n",      prefix, platform_port->dir_pin );
    fprintf(stderr, "%s.fd=%08X\n",           prefix, platform_port->fd );
    fflush(stderr);
}

