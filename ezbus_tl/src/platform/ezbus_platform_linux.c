/******************************************************************************
 * Copyright © 2018 by @author Mike Sharkey <mike@8bitgeek.net>
 * All Rights Reserved
 *****************************************************************************/
#include <ezbus_platform.h>
#include <linux/serial.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <errno.h>
#include <stdbool.h>
#include <ezbus_hex.h>

#define EZBUS_PACKET_DEBUG	1

static void serial_set_blocking (int fd, int should_block);

int ezbus_platform_open(ezbus_platform_port_t* port,uint32_t speed)
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
	perror("ezbus_platform_open");
	return -1;
}

int ezbus_platform_send(ezbus_platform_port_t* port,void* bytes,size_t size)
{

	uint8_t* p = (uint8_t*)bytes;
	#if EZBUS_PACKET_DEBUG
		ezbus_hex_dump( "TX:", p, size );
	#endif
	size_t sent=0;
	do {
		sent += write(port->fd,p,size-sent);
		p = (uint8_t*)bytes;
		p = &p[sent];
	} while (sent<size);
	return sent;
}

int ezbus_platform_recv(ezbus_platform_port_t* port,void* bytes,size_t size)
{
	int rc = read(port->fd,bytes,size);
	#if EZBUS_PACKET_DEBUG
		ezbus_hex_dump( "RX:", bytes, rc );
	#endif
	return rc;
}

int ezbus_platform_getc(ezbus_platform_port_t* port)
{
	uint8_t ch;
	int rc = read(port->fd,&ch,1);
	if ( rc == 1 )
		return ch;
	return -1;
}

void ezbus_platform_close(ezbus_platform_port_t* port)
{
	close(port->fd);
}

void ezbus_platform_drain(ezbus_platform_port_t* port)
{
	while(ezbus_platform_getc(port)>=0);
}

int ezbus_platform_set_speed(ezbus_platform_port_t* port,uint32_t speed)
{
    struct termios tty;
    memset (&tty, 0, sizeof tty);
    if (tcgetattr (port->fd, &tty) == 0)
    {
	    cfsetospeed(&tty, (speed_t)speed);
	    cfsetispeed(&tty, (speed_t)speed);

	    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
	    tty.c_cflag &= ~CSIZE;
	    tty.c_cflag |= CS8;         /* 8-bit characters */
	    tty.c_cflag &= ~PARENB;     /* no parity bit */
	    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
	    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

	    /* setup for non-canonical mode */
	    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
	    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	    tty.c_oflag &= ~OPOST;

	    /* fetch bytes as they become available */
	    tty.c_cc[VMIN] = 1;
	    tty.c_cc[VTIME] = 1;

	    if (tcsetattr (port->fd, TCSANOW, &tty) == 0)
	    	return 0;

	}
	perror("ezbus_platform_set_speed");
	return -1;
}

void ezbus_platform_flush(ezbus_platform_port_t* port)
{
	fsync(port->fd);
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

void ezbus_platform_address(uint8_t* address)
{
	static uint32_t b[3] = {0,0,0};
	if ( b[0]==0 && b[1]==0 && b[2]==0 )
	{
		b[0] = rand();
		b[1] = rand();
		b[2] = rand();
	}
	ezbus_platform_memcpy(address,b,sizeof(uint32_t)*3);
}


static void serial_set_blocking (int fd, int should_block)
{
    struct termios tty;
    memset (&tty, 0, sizeof tty);
    if (tcgetattr (fd, &tty) != 0)
    {
            fprintf (stderr, "error %d from tggetattr", errno);
            return;
    }

    tty.c_cc[VMIN]  = should_block ? 1 : 0;
    tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

    if (tcsetattr (fd, TCSANOW, &tty) != 0)
            fprintf (stderr, "error %d setting term attributes", errno);
}

