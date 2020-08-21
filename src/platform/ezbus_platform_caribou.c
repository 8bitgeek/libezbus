/*****************************************************************************
* Copyright © 2019-2020 Mike Sharkey <mike.sharkey@mineairquality.com>       *
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
#if defined(EZBUS_USE_FLOW_CALLBACK) && !defined(EZBUS_USE_DEFAULT_FLOW_CALLBACK)
    #include <ezbus_callback.h>
#endif

static uint32_t _platform_address = 0;

int ezbus_platform_open(ezbus_platform_port_t* port,uint32_t speed)
{
    if (( port->fd = fopen(port->serial_port_no,"rw"))!=NULL)
    {
        ezbus_platform_set_speed(port,speed);
        return 0;
    }
    return -1;
}

#if defined(EZBUS_USE_FLOW_CALLBACK) && defined(EZBUS_USE_DEFAULT_FLOW_CALLBACK)
    // define this function for your platform
    extern bool ezbus_platform_set_tx( ezbus_platform_port_t* port, bool enable )
    {
        if ( enable )
        {
            caribou_gpio_set(ezbus_platform_port_get_dir_gpio_rx(port));
            caribou_gpio_set(ezbus_platform_port_get_dir_gpio_tx(port));
        }
        else
        {
            caribou_gpio_reset(ezbus_platform_port_get_dir_gpio_rx(port));
            caribou_gpio_reset(ezbus_platform_port_get_dir_gpio_tx(port));
        }
        return enable;
    }
#endif

#if !defined(PRIVATE_EZBUS_PLATFORM_SEND)
int ezbus_platform_send(ezbus_platform_port_t* port,void* bytes,size_t size)
{
    uint8_t* p = (uint8_t*)bytes;
    size_t sent=0;
    #if defined(EZBUS_USE_FLOW_CALLBACK)
        ezbus_platform_set_tx( port, true );
    #endif
    do {
        sent += fwrite(p,1,size-sent,port->fd);
        p = (uint8_t*)bytes;
        p = &p[sent];
    } while (sent<size);
    ezbus_platform_flush( port );
    #if defined(EZBUS_USE_FLOW_CALLBACK)
        ezbus_platform_set_tx( port, false );
    #endif
    return sent;
}
#endif

int ezbus_platform_recv(ezbus_platform_port_t* port,void* bytes,size_t size)
{
    int rc = fread(bytes,1,size,port->fd);
    return rc;
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
    
    #if defined(EZBUS_USE_DMA)
        #define EZBUS_USE_RX_DMA 1
        #define EZBUS_USE_TX_DMA 1
    #endif

    #if defined(EZBUS_USE_RX_DMA)
        config.dma_mode |= CARIBOU_UART_DMA_RX;
    #endif
    
    #if defined(EZBUS_USE_TX_DMA)
        config.dma_mode |= CARIBOU_UART_DMA_TX;
    #endif
    
    #if defined( EZBUS_USE_RX_DMA ) || defined( EZBUS_USE_TX_DMA )
        config.dma_prio = CARIBOU_UART_DMA_PRIO_MEDIUM;
    #endif 

    #if !defined(EZBUS_USE_FLOW_CALLBACK)
        config.flow_control = CARIBOU_UART_FLOW_RS485_GPIO;
        config.gpio = port->dir_tx_pin;
    #endif
    
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

char* ezbus_platform_strcat( char* dest, const char *src )
{
    return strcat( dest, src );
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

void ezbus_platform_set_address ( const ezbus_address_t* address )
{
    _platform_address = address->word;
}

void ezbus_platform_address(ezbus_address_t* address)
{
    if ( _platform_address == 0 )
    {
        uint32_t words[3];
        caribou_get_uuid(words);
        _platform_address = ezbus_crc32(words,3*sizeof(uint32_t));
    }
    address->word = _platform_address;
}

void ezbus_platform_port_dump( ezbus_platform_port_t* platform_port, const char* prefix )
{
    fprintf(stderr, "%s.serial_port_no=%d\n", prefix, platform_port->serial_port_no );
    fprintf(stderr, "%s.dir_tx_pin=%08X\n",   prefix, platform_port->dir_tx_pin );
    fprintf(stderr, "%s.ndir_rx_pin=%08X\n",  prefix, platform_port->ndir_rx_pin );
    fprintf(stderr, "%s.fd=%08X\n",           prefix, platform_port->fd );
    fflush(stderr);
}

