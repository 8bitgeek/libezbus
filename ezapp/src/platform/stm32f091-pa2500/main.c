/******************************************************************************
* Copyright © 2017 by Pike Aerospace Research Corporation
* All Rights Reserved
******************************************************************************/
#include <board.h>
#include <chip/chip.h>


void thread_checkin_callback( void );
void thread_timeout_callback( caribou_thread_t* node );

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

	caribou_thread_yield();
}

static void initialize_rs485(void)
{

	/* Initialize the RS-485 USART port */
	caribou_uart_config_t config;
	caribou_uart_init_config(&config);
	config.baud_rate	= 115200;
	config.word_size	= 8;
	config.stop_bits	= 1;
	config.dma_mode		= CARIBOU_UART_DMA_RX;
	config.dma_prio		= CARIBOU_UART_DMA_PRIO_MEDIUM;
	config.flow_control	= CARIBOU_UART_FLOW_NONE;
	caribou_uart_set_config(CONSOLE_USART,&config);
}

#if PRODUCT_WATCHDOG_ENABLED
	void thread_checkin_callback( void )
	{
		chip_watchdog_feed();
	}

	void thread_timeout_callback( caribou_thread_t* node )
	{
		fprintf(stderr,">>> '%s' triggered W/D timeout.\r\n",caribou_thread_name(node));
		fflush(stderr);
		for(;;);
	}
#endif

static void print_banner()
{
	fprintf( stderr, "pclk=%d\n", SystemCoreClock );
	fprintf( stderr, "Heap: block size: %d bytes free: %d\n",heap_block_size(), heap_bytes_free());
	if ( RCC->CSR & RCC_CSR_LPWRRSTF )	fprintf( stderr, "[Low-power] " );
	if ( RCC->CSR & RCC_CSR_WWDGRSTF )	fprintf( stderr, "[Window watchdog] " );
	if ( RCC->CSR & RCC_CSR_IWDGRSTF )	fprintf( stderr, "[Independent watchdog] " );
	if ( RCC->CSR & RCC_CSR_SFTRSTF )	fprintf( stderr, "[Software] " );
	if ( RCC->CSR & RCC_CSR_PINRSTF )	fprintf( stderr, "[PIN] " );
	if ( RCC->CSR & RCC_CSR_OBLRSTF )	fprintf( stderr, "[Option byte loader] " );
	fprintf( stderr, "\n");
	fflush( stderr );
}

int main(void)
{
	caribou_init(0);

	print_banner();

	#if PRODUCT_WATCHDOG_ENABLED
		caribou_thread_watchdog_init( thread_checkin_callback, thread_timeout_callback );
		chip_watchdog_init( PRODUCT_WD_PERIOD_MS ); 
	#endif

	initialize_rs485();

   	caribou_exec();

	return 0;
}
