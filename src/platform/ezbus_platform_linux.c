/*****************************************************************************
* Copyright © 2019-2020 Mike Sharkey <mike@8bitgeek.net>                     *
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
#include <ezbus_platform.h>
#include <ezbus_address.h>
#include <ezbus_crc32.h>
#include <linux/serial.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h> 
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <errno.h>
#include <stdbool.h>
#include <time.h>
#include <sys/select.h>
#include <sys/time.h>
#include <asm/termbits.h>

#include <ezbus_hex.h>

#define EZBUS_PACKET_DEBUG  1

#define ezbus_platform_port_get_udp(p)      ((p)->udp_cmdline->serial_device==NULL)

static uint32_t _platform_address = 0;

static void serial_set_blocking (int fd, int should_block);

int ezbus_platform_open(ezbus_platform_port_t* port,uint32_t speed)
{
    if ( ezbus_platform_port_get_udp(port) )
    {
        _platform_address = port->udp_cmdline->id;

        if ( ezbus_udp_broadcast_setup( &port->udp_broadcast, 
                ezbus_udp_cmdline_address(port->udp_cmdline), 
                ezbus_udp_cmdline_port(port->udp_cmdline) ) >= 0 )
        {
            if ( ezbus_udp_listen_setup( &port->udp_listen,
                ezbus_udp_cmdline_address(port->udp_cmdline), 
                ezbus_udp_cmdline_port(port->udp_cmdline) ) >= 0 )
            {
                return 0;
            }
        }
    }
    else
    {
        if ( (port->fd = open(port->serial_port_name,O_RDWR)) >=0 )
        {
            struct serial_rs485 rs485conf = {0};

            rs485conf.flags |= SER_RS485_ENABLED;
            rs485conf.flags |= SER_RS485_RTS_ON_SEND;
            rs485conf.flags &= ~(SER_RS485_RTS_AFTER_SEND);

            ioctl (port->fd, TIOCSRS485, &rs485conf);
            ezbus_platform_set_speed(port,speed);
            serial_set_blocking(port->fd,false);
            return 0;
        }
    }
    perror("ezbus_platform_open");
    return -1;
}

int ezbus_platform_send(ezbus_platform_port_t* port,void* bytes,size_t size)
{
    if ( ezbus_platform_port_get_udp(port) )
    {
        uint8_t* p = (uint8_t*)bytes;
        size_t sent=0;
        do {
            sent += ezbus_udp_broadcast_send(&port->udp_broadcast,p,size-sent);
            if ( sent > 0)
                p += sent;
        } while (sent<size&&sent>=0);
        ezbus_platform_flush( port );
        return sent;
    }
    else
    {
        uint8_t* p = (uint8_t*)bytes;
        size_t sent=0;
        do {
            sent += write(port->fd,p,size-sent);
            if ( sent > 0)
                p += sent;
        } while (sent<size&&sent>=0);
        ezbus_platform_flush( port );
        return sent;
    }
}

int ezbus_platform_recv(ezbus_platform_port_t* port,void* bytes,size_t size)
{
    if ( ezbus_platform_port_get_udp(port) )
    {
        int rc = ezbus_udp_listen_recv(&port->udp_listen,bytes,size);
        return rc;
    }
    else
    {
        int rc = read(port->fd,bytes,size);
        return rc;
    }
}

int ezbus_platform_getc(ezbus_platform_port_t* port)
{
    if ( ezbus_platform_port_get_udp(port) )
    {
        uint8_t ch;
        int rc = ezbus_udp_listen_recv(&port->udp_listen,&ch,1);
        if ( rc == 1 )
        {
            if ( ch == 0x55 )
                fputc('\n',stderr);

            fprintf( stderr, "%02X", ch );
            
            return (int)ch;
        }
        return -1;
    }
    else
    {
        uint8_t ch;
        int rc = read(port->fd,&ch,1);
        if ( rc == 1 )
        {
            return (int)ch;
        }
        return -1;
    }
}

void ezbus_platform_close(ezbus_platform_port_t* port)
{
    if ( ezbus_platform_port_get_udp(port) )
    {
        ezbus_udp_listen_close(&port->udp_listen);
        ezbus_udp_broadcast_close(&port->udp_broadcast);
    }
    else
    {
        close(port->fd);
    }
}

void ezbus_platform_drain(ezbus_platform_port_t* port)
{
    while(ezbus_platform_getc(port)>=0);
}

int ezbus_platform_set_speed(ezbus_platform_port_t* port,uint32_t baud)
{
    if ( !ezbus_platform_port_get_udp(port) )
    {
        struct termios2 options;

        if ( port->fd >= 0 )
        {
            fcntl(port->fd, F_SETFL, 0);

            ioctl(port->fd, TCGETS2, &options);

            options.c_cflag &= ~CBAUD;
            options.c_cflag |= BOTHER;
            options.c_ispeed = baud;
            options.c_ospeed = baud;
            
            options.c_cflag |= (CLOCAL | CREAD);
            options.c_cflag &= ~CRTSCTS;
            options.c_cflag &= ~HUPCL;

            options.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
            options.c_oflag &= ~OPOST;
            options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
            options.c_cflag &= ~(CSIZE | PARENB);
            options.c_cflag |= CS8;

            ioctl(port->fd, TCSETS2, &options);

            return 0;
        }
    }
    return -1;
}

void ezbus_platform_flush(ezbus_platform_port_t* port)
{
    if ( !ezbus_platform_port_get_udp(port) )
    {
        fsync(port->fd);
    }
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

char* ezbus_platform_strcat( char* dest, const char *src )
{
    return strcat( dest, src );
}

char* ezbus_platform_strncpy( char* dest, const char *src, size_t n )
{
    return strncpy( dest, src, n );
}

size_t ezbus_platform_strlen( const char* s)
{
    return strlen( s );
}

extern int ezbus_platform_strcmp( const char* s1, const char *s2 )
{
    return strcmp(s1,s2);
}

extern int ezbus_platform_strcasecmp( const char* s1, const char *s2 )
{
    return strcasecmp(s1,s2);
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
    ezbus_ms_tick_t ticks;
    struct timeval tm;
    gettimeofday(&tm,NULL);
    ticks = ((tm.tv_sec*1000)+(tm.tv_usec/1000));
    return ticks;
}

void ezbus_platform_set_address ( const ezbus_address_t* address )
{
    _platform_address = address->word;
}

void ezbus_platform_address(ezbus_address_t* address)
{
    if ( _platform_address == 0 )
    {
        uint32_t words[3];
        words[0] = ezbus_platform_rand();
        words[1] = ezbus_platform_rand();
        words[2] = ezbus_platform_rand();
        _platform_address = ezbus_crc32(words,3*sizeof(uint32_t));
    }
    address->word = _platform_address;
}

static void serial_set_blocking (int fd, int should_block)
{
    struct termios2 options;

    if ( fd >= 0 )
    {
        fcntl(fd, F_SETFL, 0);

        ioctl(fd, TCGETS2, &options);

        if ( should_block )
        {
            options.c_cc[VMIN]  = 1;
            options.c_cc[VTIME] = 1;
        }
        else
        {
            options.c_cc[VMIN]  = 0;
            options.c_cc[VTIME] = 0;
        }

        ioctl(fd, TCSETS2, &options);
    }
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

void ezbus_platform_rand_init (void)
{
    ezbus_platform_srand( ezbus_platform_get_ms_ticks()&0xFFFFFFFF );
}

void ezbus_platform_delay(unsigned int ms)
{
    ezbus_ms_tick_t start = ezbus_platform_get_ms_ticks();
    while ( (ezbus_platform_get_ms_ticks() - start) < ms );
}

void ezbus_platform_port_dump( ezbus_platform_port_t* platform_port, const char* prefix )
{
    fprintf(stderr, "%s.serial_port_name=%s\n", prefix, platform_port->serial_port_name );
    fprintf(stderr, "%s.fd=%d\n",               prefix, platform_port->fd );
    fflush(stderr);
}
