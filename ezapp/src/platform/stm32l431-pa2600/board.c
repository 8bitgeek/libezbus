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
#include <caribou/lib/bitmap_heap.h>

caribou_gpio_t  gpio_enc28j60_nss   = CARIBOU_GPIO_INIT(GPIOA,CARIBOU_GPIO_PIN_4);
caribou_gpio_t  gpio_enc28j60_reset = CARIBOU_GPIO_INIT(GPIOA,CARIBOU_GPIO_PIN_15);
caribou_gpio_t  gpio_enc28j60_int   = CARIBOU_GPIO_INIT(GPIOB,CARIBOU_GPIO_PIN_4);
caribou_gpio_t  gpio_rs485_tx       = CARIBOU_GPIO_INIT(GPIOA,CARIBOU_GPIO_PIN_9);
caribou_gpio_t  gpio_rs485_rx       = CARIBOU_GPIO_INIT(GPIOA,CARIBOU_GPIO_PIN_10);
caribou_gpio_t  gpio_rs485_dir      = CARIBOU_GPIO_INIT(GPIOA,CARIBOU_GPIO_PIN_12);
caribou_gpio_t  gpio_eeprom_wp      = CARIBOU_GPIO_INIT(GPIOB,CARIBOU_GPIO_PIN_3);
caribou_gpio_t  gpio_i2c_scl        = CARIBOU_GPIO_INIT(GPIOB,CARIBOU_GPIO_PIN_6);
caribou_gpio_t  gpio_i2c_sda        = CARIBOU_GPIO_INIT(GPIOB,CARIBOU_GPIO_PIN_7);
caribou_gpio_t  gpio_status         = CARIBOU_GPIO_INIT(GPIOB,CARIBOU_GPIO_PIN_0);
caribou_gpio_t  gpio_init           = CARIBOU_GPIO_INIT(GPIOB,CARIBOU_GPIO_PIN_1);


static void CLOCK_Configuration()
{
    /* Enable Internal 16 MHz Oscillator */
    RCC->CR |= RCC_CR_HSION | RCC_CR_HSIKERON;
    while ( !(RCC->CR & RCC_CR_HSIRDY) );

    /* SYSCLK is 16MHz HSI */
    RCC->CFGR = RCC_CFGR_SW_0 | RCC_CFGR_STOPWUCK;

    /* Configure the PLL */
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSI;                     /* Select 16MHz HSI Clk Src */
    RCC->PLLCFGR |= (20 << RCC_PLLCFGR_PLLN_Pos);               /* 16MHz * 20 = 320MHz */
    RCC->PLLCFGR |= ((2-1) << RCC_PLLCFGR_PLLM_Pos);            /* 320MHz / 2 = 160MHz */
    RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLR_1|RCC_PLLCFGR_PLLR_0);   /* 160MHz / 2 = 80MHz */
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN;                         /* Enable PLLCLK (/R) Output */

    /* Start the PLL */
    RCC->CR |= RCC_CR_PLLON;                                    /* Start PLL */
    while( !(RCC->CR & RCC_CR_PLLRDY) );                        /* Wait for PLL Ready */

    /* Configure FLASH Wait States */
    FLASH->ACR |= FLASH_ACR_LATENCY_4WS;

    /* Select PLL as clock source */
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while((RCC->CFGR & RCC_CFGR_SWS)!=RCC_CFGR_SWS_PLL);

    /* Put UART1 on HSI16 Clock */
    RCC->CCIPR &= ~RCC_CCIPR_USART1SEL_Msk;
    RCC->CCIPR |= RCC_CCIPR_USART1SEL_1;
}

void early_init()
{
    CLOCK_Configuration();
    SystemCoreClockUpdate();

    RCC->AHB1ENR |= (
                        RCC_AHB1ENR_DMA1EN      |
                        RCC_AHB1ENR_DMA2EN      |
                        RCC_AHB1ENR_CRCEN
                    );

    RCC->AHB2ENR |= ( 
                        RCC_AHB2ENR_GPIOAEN     |
                        RCC_AHB2ENR_GPIOBEN     |
                        RCC_AHB2ENR_GPIOCEN     |
                        RCC_AHB2ENR_GPIOHEN     |
                        RCC_AHB2ENR_ADCEN       
                    );

    RCC->APB1ENR1 |=(
                        RCC_APB1ENR1_WWDGEN     |
                        RCC_APB1ENR1_USART2EN   |
                        RCC_APB1ENR1_SPI2EN     
                    );

    RCC->APB2ENR |= (
                        RCC_APB2ENR_SYSCFGEN    |
                        RCC_APB2ENR_USART1EN    |
                        RCC_APB2ENR_SPI1EN
                    );

    /* PORT A */
    GPIOA->MODER = CARIBOU_PORTA_MODE;
    GPIOA->OTYPER = CARIBOU_PORTA_OTYPER;
    GPIOA->OSPEEDR = CARIBOU_PORTA_OSPEEDR;
    GPIOA->PUPDR = CARIBOU_PORTA_PUPDR;
    GPIOA->AFR[0] = CARIBOU_PORTA_AFRL;
    GPIOA->AFR[1] = CARIBOU_PORTA_AFRH;
    /* POST B */
    GPIOB->MODER = CARIBOU_PORTB_MODE;
    GPIOB->OTYPER = CARIBOU_PORTB_OTYPER;
    GPIOB->OSPEEDR = CARIBOU_PORTB_OSPEEDR;
    GPIOB->PUPDR = CARIBOU_PORTB_PUPDR;
    GPIOB->AFR[0] = CARIBOU_PORTB_AFRL;
    GPIOB->AFR[1] = CARIBOU_PORTB_AFRH;
    /* POST C */
    GPIOC->MODER = CARIBOU_PORTC_MODE;
    GPIOC->OTYPER = CARIBOU_PORTC_OTYPER;
    GPIOC->OSPEEDR = CARIBOU_PORTC_OSPEEDR;
    GPIOC->PUPDR = CARIBOU_PORTC_PUPDR;
    GPIOC->AFR[0] = CARIBOU_PORTC_AFRL;
    GPIOC->AFR[1] = CARIBOU_PORTC_AFRH;
    /* POST H */
    GPIOH->MODER = CARIBOU_PORTH_MODE;
    GPIOH->OTYPER = CARIBOU_PORTH_OTYPER;
    GPIOH->OSPEEDR = CARIBOU_PORTH_OSPEEDR;
    GPIOH->PUPDR = CARIBOU_PORTH_PUPDR;
    GPIOH->AFR[0] = CARIBOU_PORTH_AFRL;
    GPIOH->AFR[1] = CARIBOU_PORTH_AFRH;

    caribou_gpio_set(&gpio_eeprom_wp);
    caribou_gpio_set(&gpio_i2c_sda);
    caribou_gpio_set(&gpio_i2c_scl);
    caribou_gpio_reset(&gpio_enc28j60_reset);   /* Assert enc20j60 reset */
    caribou_gpio_set(&gpio_enc28j60_int);       /* Enable pullup on Interrupt input */
    caribou_gpio_set(&gpio_enc28j60_nss);       /* Release ENC28J60 SPI chip select. */
    caribou_gpio_reset(&gpio_status);
    rs485_rx();                                 /* RS485 RX direction. */
}


void late_init()
{
    /**
     * Open the standard I/O (Modbus RS485).
     */
    caribou_uart_config_t config;

    /* Console: RS485 port. */
    _stdout = _stdin = fopen(CONSOLE_USART,"rw");
    caribou_uart_init_config(&config);
    config.baud_rate    = CARIBOU_UART_BAUD_RATE_115200;
    config.word_size    = CARIBOU_UART_WORDSIZE_8;
    config.stop_bits    = CARIBOU_UART_STOPBITS_1;
    config.parity_bits  = CARIBOU_UART_PARITY_NONE;
    config.flow_control = CARIBOU_UART_FLOW_RS485_GPIO;
    config.gpio         = &gpio_rs485_dir;
    caribou_uart_set_config(CONSOLE_USART,&config);

    /* Stderr: debugging port. */
    _stderr = fopen(DEBUG_USART,"rw");
    caribou_uart_init_config(&config);
    config.baud_rate    = CARIBOU_UART_BAUD_RATE_115200;
    config.word_size    = CARIBOU_UART_WORDSIZE_8;
    config.stop_bits    = CARIBOU_UART_STOPBITS_1;
    config.parity_bits  = CARIBOU_UART_PARITY_NONE;
    config.flow_control = CARIBOU_UART_FLOW_NONE;
    caribou_uart_set_config(DEBUG_USART,&config);

    //caribou_gpio_reset(&gpio_status);

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
        __ASM(" nop\n");
        __ASM(" nop\n");
        __ASM(" nop\n");
        __ASM(" nop\n");
        __ASM(" nop\n");
        __ASM(" nop\n");
        __ASM(" nop\n");
        __ASM(" nop\n");
        __ASM(" nop\n");
        __ASM(" nop\n");
        __ASM(" nop\n");
        __ASM(" nop\n");
        __ASM(" nop\n");
        __ASM(" nop\n");
    }
}


void notify_heap_invalid_dealloc(void* pointer)
{
    chip_reset();
}

void notify_heap_invalid_realloc(void* pointer, size_t size)
{
    chip_reset();
}

void notify_heap_alloc_failed(size_t size)
{
    chip_reset();
}


void board_reset()
{
    chip_reset();
}

uint16_t __attribute__((naked)) flip16(uint16_t x)
{
    __asm(  " rev16  r0,r0  \n"
            " bx     lr     \n"     );
}
