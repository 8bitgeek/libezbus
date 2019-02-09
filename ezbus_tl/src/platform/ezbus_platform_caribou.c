/******************************************************************************
 * Copyright © 2018 by @author Mike Sharkey <mike@8bitgeek.net>
 * All Rights Reserved
 *****************************************************************************/
#include "ezbus_platform_caribou.h"

#include <caribou/lib/stdio.h>
#include <caribou/dev/uart.h>

int ezbus_platform_open(ezbus_platform_port_t* port,uint32_t speed)
{
	if (( port->fd = caribou_fopen(port->serial_port_no,"rw"))!=NULL)
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
	do {
		sent += caribou_fwrite(p,1,size-sent,port->fd);
		p = (uint8_t*)bytes;
		p = &p[sent];
	} while (sent<size);
	return sent;
}

int ezbus_platform_recv(ezbus_platform_port_t* port,void* bytes,size_t size)
{
	int rc = caribou_fread(bytes,1,size,port->fd);
	return rc;
}

int ezbus_platform_getc(ezbus_platform_port_t* port)
{
	int ch = caribou_fgetc(port->fd);
	return ch;
}

void ezbus_platform_close(ezbus_platform_port_t* port)
{
	caribou_fclose(port->fd);
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
	config.gpio = &port->dir_pin;
	caribou_uart_set_config(port->serial_port_no,&config);
	return 0;
}

void ezbus_platform_flush(ezbus_platform_port_t* port)
{
	caribou_fflush(port->fd);
}

void* ezbus_platform_memset(void* dest, int c, size_t n)
{
	return caribou_memset(dest,c,n);
}

void* ezbus_platform_memcpy(void* dest, const void *src, size_t n)
{
	return caribou_memcpy(dest,src,n);
}

void* ezbus_platform_memmove(void* dest, const void *src, size_t n)
{
	return caribou_memmove(dest,src,n);
}

int ezbus_platform_memcmp(const void* dest, const void *src, size_t n)
{
	return caribou_memcmp(dest,src,n);
}

void* ezbus_platform_malloc( size_t n)
{
	return caribou_malloc(n);
}

void* ezbus_platform_realloc(void* src,size_t n)
{
	return caribou_realloc(src,n);
}

void ezbus_platform_free(void *src)
{
	caribou_free(src);
}

ezbus_ms_tick_t ezbus_platform_get_ms_ticks()
{
	return caribou_timer_ticks();
}

void ezbus_platform_address(uint8_t* address)
{
	/* STM32F091 */
	volatile uint32_t* a = (uint32_t*)0x1FFFF7AC;
	uint32_t b[3];
	b[0] = *a++;
	b[1] = *a++;
	b[2] = *a++;
	ezbus_platform_memcpy(address,b,sizeof(uint32_t)*3);
}

