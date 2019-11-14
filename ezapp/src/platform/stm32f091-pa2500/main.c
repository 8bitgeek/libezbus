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
#include <board.h>
#include <chip/chip.h>
#include <ezbus_thread.h>

#define EZBUS_STACK_SZ      (1024*4)
static caribou_thread_t*    ezbus_thread=NULL;
static uint32_t             ezbus_stack[ EZBUS_STACK_SZ/4 ];

static void setup_rs485  ( void );
static void setup_threads( void );
static void print_banner ( void );

int main(void)
{
    caribou_init(0);

    print_banner();

    setup_rs485();
    setup_threads();

    caribou_exec();

    return 0;
}

/**
 * @brief Call-back from caribou main thread.
 */
void board_idle()
{
    static caribou_tick_t start=0;

    if ( caribou_timer_ticks_timeout(start,from_ms(250) ) )
    {
        caribou_gpio_toggle( &gpio_status);
        start = caribou_timer_ticks();
    }

    caribou_wfi();
}

static void setup_threads( void )
{
    ezbus_thread = caribou_thread_create( "ezbus", ezbus_thread_run, NULL, NULL, ezbus_stack, EZBUS_STACK_SZ, 1, 0 );
}

static void setup_rs485(void)
{

    /* Initialize the RS-485 USART port */
    caribou_uart_config_t config;
    caribou_uart_init_config(&config);
    config.baud_rate    = 115200;
    config.word_size    = 8;
    config.stop_bits    = 1;
    config.dma_mode     = CARIBOU_UART_DMA_RX;
    config.dma_prio     = CARIBOU_UART_DMA_PRIO_MEDIUM;
    config.flow_control = CARIBOU_UART_FLOW_NONE;
    caribou_uart_set_config(CONSOLE_USART,&config);
}

static void print_banner()
{
    fprintf( stderr, "pclk=%d\n", SystemCoreClock );
    fprintf( stderr, "Heap: block size: %d bytes free: %d\n",heap_block_size(), heap_bytes_free());
    if ( RCC->CSR & RCC_CSR_LPWRRSTF )  fprintf( stderr, "[Low-power] " );
    if ( RCC->CSR & RCC_CSR_WWDGRSTF )  fprintf( stderr, "[Window watchdog] " );
    if ( RCC->CSR & RCC_CSR_IWDGRSTF )  fprintf( stderr, "[Independent watchdog] " );
    if ( RCC->CSR & RCC_CSR_SFTRSTF )   fprintf( stderr, "[Software] " );
    if ( RCC->CSR & RCC_CSR_PINRSTF )   fprintf( stderr, "[PIN] " );
    if ( RCC->CSR & RCC_CSR_OBLRSTF )   fprintf( stderr, "[Option byte loader] " );
    fprintf( stderr, "\n");
    fflush( stderr );
}

