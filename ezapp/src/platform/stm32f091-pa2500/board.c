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
#include <caribou/lib/stdio.h>

caribou_gpio_t	gpio_enc28j60_nss	= CARIBOU_GPIO_INIT(GPIOA,CARIBOU_GPIO_PIN_4);
caribou_gpio_t	gpio_enc28j60_reset	= CARIBOU_GPIO_INIT(GPIOA,CARIBOU_GPIO_PIN_15);
caribou_gpio_t	gpio_enc28j60_int	= CARIBOU_GPIO_INIT(GPIOB,CARIBOU_GPIO_PIN_4);
caribou_gpio_t	gpio_rs485_dir		= CARIBOU_GPIO_INIT(GPIOA,CARIBOU_GPIO_PIN_12);
caribou_gpio_t	gpio_eeprom_wp		= CARIBOU_GPIO_INIT(GPIOB,CARIBOU_GPIO_PIN_3);
caribou_gpio_t	gpio_i2c_scl		= CARIBOU_GPIO_INIT(GPIOB,CARIBOU_GPIO_PIN_6);
caribou_gpio_t	gpio_i2c_sda		= CARIBOU_GPIO_INIT(GPIOB,CARIBOU_GPIO_PIN_7);
caribou_gpio_t	gpio_status			= CARIBOU_GPIO_INIT(GPIOB,CARIBOU_GPIO_PIN_0);
caribou_gpio_t	gpio_init			= CARIBOU_GPIO_INIT(GPIOB,CARIBOU_GPIO_PIN_1);

caribou_gpio_t led1 		= CARIBOU_GPIO_INIT(GPIOA,CARIBOU_GPIO_PIN_5);
caribou_gpio_t pb1			= CARIBOU_GPIO_INIT(GPIOC,CARIBOU_GPIO_PIN_13);
caribou_gpio_t ezbus_dir	= CARIBOU_GPIO_INIT(GPIOA,CARIBOU_GPIO_PIN_10);

extern void notify_heap_invalid_dealloc(void* pointer)
{
	chip_reset();
}

extern void notify_heap_invalid_realloc(void* pointer, size_t size)
{
	chip_reset();
}

extern void notify_heap_alloc_failed(size_t size)
{
	chip_reset();
}

static void CLOCK_Configuration()
{
	#if 0 /* HSE */
		RCC->CR |= RCC_CR_HSEON;								/* Turn HSE On */
		while(!RCC->CR & RCC_CR_HSERDY);						/* Wait for HSE ready */
		RCC->CFGR |= RCC_CFGR_PLLSRC;							/* Select HSE clock as PLL source*/
		RCC->CFGR |= (
						RCC_CFGR_HPRE_DIV1	|					/* HCLK Prescaler 1:1 */
						RCC_CFGR_PPRE1_DIV2	|
						RCC_CFGR_PPRE2_DIV1 |
						RCC_CFGR_ADCPRE_DIV4
					 );
		RCC->CFGR |= RCC_CFGR_PLLMULL9;							/* 6.25MHz x 11 = 68.75MHz */
	#else
		RCC->CR |= RCC_CR_HSION;								/* Turn HSI On */
		RCC->CR &= ~RCC_CR_HSEON;								/* Turn HSE Off */
		RCC->CFGR |= (
						RCC_CFGR_HPRE_DIV1						/* HCLK Prescaler 1:1 */
					 );
		RCC->CFGR |= RCC_CFGR_PLLMULL12;						/* 4MHz x 12 = 48MHz */
	#endif
	RCC->CR |= RCC_CR_PLLON;									/* Start PLL */
	while( !(RCC->CR & RCC_CR_PLLRDY) );						/* Wait for PLL Ready */
	FLASH->ACR |= FLASH_ACR_LATENCY;							/* 1 FLASH Waits States */
	RCC->CFGR |= RCC_CFGR_SW_PLL;								/* Select PLL as SYSCLK */
	while((RCC->CFGR & RCC_CFGR_SWS)!=RCC_CFGR_SWS_PLL);		/* Wait for switch to PLL */
}

extern void early_init()
{
	CLOCK_Configuration();

	RCC->AHBENR |=	(	RCC_AHBENR_DMA1EN		|
						RCC_AHBENR_CRCEN		|
						RCC_AHBENR_GPIOAEN		|
						RCC_AHBENR_GPIOBEN		|
						RCC_AHBENR_GPIOCEN		|
						RCC_AHBENR_GPIODEN		);

	RCC->APB1ENR |= (	RCC_APB1ENR_SPI2EN		|
						RCC_APB1ENR_USART2EN	);

	RCC->APB2ENR |= (	RCC_APB2ENR_SPI1EN		|
                        RCC_APB2ENR_USART1EN	);


	/* PORT A */
	GPIOA->MODER = CARIBOU_PORTA_MODE;
	GPIOA->OTYPER = CARIBOU_PORTA_OTYPER;
	GPIOA->OSPEEDR = CARIBOU_PORTA_OSPEEDR;
	GPIOA->PUPDR = CARIBOU_PORTA_PUPDR;
	GPIOA->AFR[0] = CARIBOU_PORTA_AFRL;
	GPIOA->AFR[1] = CARIBOU_PORTA_AFRH;
	/* PORT B */
	GPIOB->MODER = CARIBOU_PORTB_MODE;
	GPIOB->OTYPER = CARIBOU_PORTB_OTYPER;
	GPIOB->OSPEEDR = CARIBOU_PORTB_OSPEEDR;
	GPIOB->PUPDR = CARIBOU_PORTB_PUPDR;
	GPIOB->AFR[0] = CARIBOU_PORTB_AFRL;
	GPIOB->AFR[1] = CARIBOU_PORTB_AFRH;
	/* PORT C */
	GPIOC->MODER = CARIBOU_PORTC_MODE;
	GPIOC->OTYPER = CARIBOU_PORTC_OTYPER;
	GPIOC->OSPEEDR = CARIBOU_PORTC_OSPEEDR;
	GPIOC->PUPDR = CARIBOU_PORTC_PUPDR;
	GPIOC->AFR[0] = CARIBOU_PORTC_AFRL;
	GPIOC->AFR[1] = CARIBOU_PORTC_AFRH;
	/* PORT D */
	GPIOD->MODER = CARIBOU_PORTC_MODE;
	GPIOD->OTYPER = CARIBOU_PORTC_OTYPER;
	GPIOD->OSPEEDR = CARIBOU_PORTC_OSPEEDR;
	GPIOD->PUPDR = CARIBOU_PORTC_PUPDR;
	GPIOD->AFR[0] = CARIBOU_PORTC_AFRL;
	GPIOD->AFR[1] = CARIBOU_PORTC_AFRH;

	caribou_gpio_set(&gpio_eeprom_wp);
	caribou_gpio_set(&gpio_i2c_sda);
	caribou_gpio_set(&gpio_i2c_scl);
	caribou_gpio_reset(&gpio_enc28j60_reset);	/* Assert enc20j60 reset */
	caribou_gpio_set(&gpio_enc28j60_int);		/* Enable pullup on Interrupt input */
	caribou_gpio_set(&gpio_enc28j60_nss);		/* Release ENC28J60 SPI chip select. */
	rs485_rx();									/* RS485 RX direction. */
	caribou_gpio_set(&gpio_status);

   	//caribou_thread_watchdog_init(CARIBOU_THREAD_O_SW_WATCHDOG,100);	/* Initialize the software watchdog */
}


extern void late_init()
{

	/**
	 * Open the standard I/O (Modbus RS485).
	 */
    rs485_rx();
	caribou_uart_config_t config;

	/* Console: RS485 port. */
	_stdout = _stdin = fopen(CONSOLE_USART,"rw");
	caribou_uart_init_config(&config);
	config.baud_rate	= CARIBOU_UART_BAUD_RATE_115200;
	config.word_size	= CARIBOU_UART_WORDSIZE_8;
	config.stop_bits	= CARIBOU_UART_STOPBITS_1;
	config.parity_bits	= CARIBOU_UART_PARITY_NONE;
	config.flow_control	= CARIBOU_UART_FLOW_NONE;
	config.dma_mode		= CARIBOU_UART_DMA_RX;
	config.dma_prio		= CARIBOU_UART_DMA_PRIO_MEDIUM;
	caribou_uart_set_config(CONSOLE_USART,&config);

	/* Stderr: debugging port. */
    _stderr = fopen(DEBUG_USART,"rw");
	caribou_uart_init_config(&config);
	config.baud_rate	= CARIBOU_UART_BAUD_RATE_115200;
	config.word_size	= CARIBOU_UART_WORDSIZE_8;
	config.stop_bits	= CARIBOU_UART_STOPBITS_1;
	config.parity_bits	= CARIBOU_UART_PARITY_NONE;
	config.flow_control	= CARIBOU_UART_FLOW_NONE;
	caribou_uart_set_config(DEBUG_USART,&config);

}

void sleep(uint32_t ms)
{
	caribou_tick_t start=caribou_timer_ticks();
	while(!caribou_timer_ticks_timeout(start,from_ms(ms)))
		caribou_thread_yield();
}

void usleep(uint32_t us)
{
	volatile uint32_t n=us;
	while(n--)
	{
		__ASM("	nop\n");
		__ASM("	nop\n");
		__ASM("	nop\n");
		__ASM("	nop\n");
		__ASM("	nop\n");
		__ASM("	nop\n");
		__ASM("	nop\n");
		__ASM("	nop\n");
		__ASM("	nop\n");
		__ASM("	nop\n");
		__ASM("	nop\n");
		__ASM("	nop\n");
		__ASM("	nop\n");
		__ASM("	nop\n");
	}
}

extern void board_reset()
{
	chip_reset();
}

extern uint16_t __attribute__((naked)) flip16(uint16_t x)
{
	__asm(	" rev16  r0,r0	\n"
			" bx     lr		\n"		);
}

