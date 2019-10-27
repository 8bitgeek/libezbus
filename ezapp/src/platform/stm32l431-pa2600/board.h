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
#ifndef _STM32L431_BOARD_H_
#define _STM32L431_BOARD_H_

#include <caribou.h>
#include <caribou/kernel/timer.h>
#include <caribou/dev/gpio.h>

#include <chip/chip.h>
#include <stm32l4xx.h>
#include <stm32l431xx.h>
#include <stm32l4xx_hal_dma.h>

#define AP_ADDR1_TEST	(1)

#ifdef __cplusplus
extern "C" {
#endif

extern caribou_gpio_t			gpio_enc28j60_nss;
extern caribou_gpio_t			gpio_enc28j60_reset;
extern caribou_gpio_t			gpio_enc28j60_int;
extern caribou_gpio_t			gpio_rs485_dir;
extern caribou_gpio_t			gpio_eeprom_wp;
extern caribou_gpio_t			gpio_i2c_scl;
extern caribou_gpio_t			gpio_i2c_sda;
extern caribou_gpio_t			gpio_status;
extern caribou_gpio_t			gpio_init;

#define rs485_rx()				caribou_gpio_reset(&gpio_rs485_dir);	/* RS485 Receive Direction */
#define rs485_tx()				caribou_gpio_set(&gpio_rs485_dir);		/* RS485 Transmit Direction */

#ifndef RCC_CFGR_PLLMULL17
	#define RCC_CFGR_PLLMULL17 ((uint32_t)0x003C0000)
#endif

#ifndef USART_RX_QUEUE_SZ
	#define USART_RX_QUEUE_SZ	32
#endif

#ifndef USART_TX_QUEUE_SZ
	#define	USART_TX_QUEUE_SZ	16
#endif

/*********************************************/

typedef enum
{
  GPIO_Mode_IN   = 0x00, /*!< GPIO Input Mode              */
  GPIO_Mode_OUT  = 0x01, /*!< GPIO Output Mode             */
  GPIO_Mode_AF   = 0x02, /*!< GPIO Alternate function Mode */
  GPIO_Mode_AN   = 0x03  /*!< GPIO Analog In/Out Mode      */
} GPIOMode_TypeDef;

// #define IS_GPIO_MODE(MODE) (((MODE) == GPIO_Mode_IN)|| ((MODE) == GPIO_Mode_OUT) || ((MODE) == GPIO_Mode_AF)|| ((MODE) == GPIO_Mode_AN))
typedef enum
{
  GPIO_OType_PP = 0x00,
  GPIO_OType_OD = 0x01
} GPIOOType_TypeDef;

#define IS_GPIO_OTYPE(OTYPE) (((OTYPE) == GPIO_OType_PP) || ((OTYPE) == GPIO_OType_OD))

#define PIN_MODE_MASK(n)		(0x3<<(n*2))			/* 16 bits */
#define PIN_MODE(n,mode)		((mode&0x3)<<(n*2))		/* 16 bits, mode 2 bits */

#define	PIN_OTYPER_MASK(n)		(1<<n)					/* 16 bits */
#define	PIN_OTYPER(n,otyper)	((otyper&1)<<n)			/* 16 bits, otyper 1 bit */

#define PIN_OSPEEDR_MASK(n)		(0x3<<(n*2))			/* 16 bits */
#define PIN_OSPEEDR(n,ospeedr)	((ospeedr&0x3)<<(n*2))	/* 16 bits, mode 2 bits */

#define PIN_PUPDR_MASK(n)		(0x3<<(n*2))			/* 16 bits */
#define PIN_PUPDR(n,pupdr)		((pupdr&0x3)<<(n*2))	/* 16 bits, mode 2 bits */

#define PIN_AFR_MASK(n)			(0xf<<(n*4))			/* 8 bits */
#define PIN_AFR(n,afr)			((afr&0xf)<<(n*4))		/* 8 bits, mode 4 bits */

#define GPIO_AF_GPIO			0						/* Alternate Function GPIO or Default */
#define GPIO_AF_USART1			GPIO_AF7_USART1
#define GPIO_AF_USART2			GPIO_AF7_USART2
#define GPIO_AF_SPI1			GPIO_AF5_SPI1
#define GPIO_AF_SPI2			GPIO_AF5_SPI2
#define GPIO_AF_CAN1			GPIO_AF9_CAN1

/**
 ** PORT A
 */
#define	CARIBOU_PORTA_MODE		PIN_MODE(0,GPIO_Mode_IN) |			/* ~ETH_WOL */		\
								PIN_MODE(1,GPIO_Mode_OUT) |			/* SRAM_WP */		\
								PIN_MODE(2,GPIO_Mode_AF) |			/* DEBUG_TX */		\
								PIN_MODE(3,GPIO_Mode_AF) |			/* DEBUG_RX */		\
								PIN_MODE(4,GPIO_Mode_OUT) |			/* ETH_NSS */		\
								PIN_MODE(5,GPIO_Mode_AF) |			/* ETH_SCK */		\
								PIN_MODE(6,GPIO_Mode_AF) |			/* ETH_MISO */		\
								PIN_MODE(7,GPIO_Mode_AF) |			/* ETH_MOSI */		\
								PIN_MODE(8,GPIO_Mode_OUT) |			/* ~SRAM_HOLD */	\
								PIN_MODE(9,GPIO_Mode_AF) |			/* UART_TX */		\
								PIN_MODE(10,GPIO_Mode_AF) |			/* UART_RX */		\
								PIN_MODE(11,GPIO_Mode_IN) |			/* UART_CTS */		\
								PIN_MODE(12,GPIO_Mode_OUT) |		/* UART_RTS */		\
								PIN_MODE(13,GPIO_Mode_AF) |			/* SWDIO */			\
								PIN_MODE(14,GPIO_Mode_AF) |			/* SWCLK */			\
								PIN_MODE(15,GPIO_Mode_OUT)			/* ~ETH_RESET */

#define	CARIBOU_PORTA_OTYPER	PIN_OTYPER(0,GPIO_OType_PP) |		/* ~ETH_WOL */		\
								PIN_OTYPER(1,GPIO_OType_PP) |		/* SRAM_WP */		\
								PIN_OTYPER(2,GPIO_OType_PP) |		/* DEBUG_TX */		\
								PIN_OTYPER(3,GPIO_OType_PP) |		/* DEBUG_RX */		\
								PIN_OTYPER(4,GPIO_OType_PP) |		/* ETH_NSS */		\
								PIN_OTYPER(5,GPIO_OType_PP) |		/* ETH_SCK */		\
								PIN_OTYPER(6,GPIO_OType_PP) |		/* ETH_MISO */		\
								PIN_OTYPER(7,GPIO_OType_PP) |		/* ETH_MOSI */		\
								PIN_OTYPER(8,GPIO_OType_PP) |		/* ~SRAM_HOLD */	\
								PIN_OTYPER(9,GPIO_OType_PP) |		/* UART_TX */		\
								PIN_OTYPER(10,GPIO_OType_PP) |		/* UART_RX */		\
								PIN_OTYPER(11,GPIO_OType_PP) |		/* UART_CTS */		\
								PIN_OTYPER(12,GPIO_OType_PP) |		/* UART_RTS */		\
								PIN_OTYPER(13,GPIO_OType_PP) |		/* SWDIO */			\
								PIN_OTYPER(14,GPIO_OType_PP) |		/* SWCLK */			\
								PIN_OTYPER(15,GPIO_OType_OD)		/* ~ETH_RESET */

#define	CARIBOU_PORTA_OSPEEDR	PIN_OSPEEDR(0,GPIO_SPEED_FREQ_LOW) |	/* ~ETH_WOL */		\
								PIN_OSPEEDR(1,GPIO_SPEED_FREQ_MEDIUM) |	/* SRAM_WP */		\
								PIN_OSPEEDR(2,GPIO_SPEED_FREQ_LOW) |	/* DEBUG_TX */		\
								PIN_OSPEEDR(3,GPIO_SPEED_FREQ_LOW) |	/* DEBUG_RX */		\
								PIN_OSPEEDR(4,GPIO_SPEED_FREQ_HIGH) |	/* ETH_NSS */		\
								PIN_OSPEEDR(5,GPIO_SPEED_FREQ_HIGH) |	/* ETH_SCK */		\
								PIN_OSPEEDR(6,GPIO_SPEED_FREQ_HIGH) |	/* ETH_MISO */		\
								PIN_OSPEEDR(7,GPIO_SPEED_FREQ_HIGH) |	/* ETH_MOSI */		\
								PIN_OSPEEDR(8,GPIO_SPEED_FREQ_MEDIUM) |	/* ~SRAM_HOLD */	\
								PIN_OSPEEDR(9,GPIO_SPEED_FREQ_LOW) |	/* UART_TX */		\
								PIN_OSPEEDR(10,GPIO_SPEED_FREQ_LOW) |	/* UART_RX */		\
								PIN_OSPEEDR(11,GPIO_SPEED_FREQ_LOW) |	/* UART_CTS */		\
								PIN_OSPEEDR(12,GPIO_SPEED_FREQ_LOW) |	/* UART_RTS */		\
								PIN_OSPEEDR(13,GPIO_SPEED_FREQ_HIGH) |	/* SWDIO */			\
								PIN_OSPEEDR(14,GPIO_SPEED_FREQ_HIGH) |	/* SWCLK */			\
								PIN_OSPEEDR(15,GPIO_SPEED_FREQ_MEDIUM)	/* ~ETH_RESET */

#define	CARIBOU_PORTA_PUPDR		PIN_PUPDR(0,GPIO_PULLUP) |			/* ~ETH_WOL */		\
								PIN_PUPDR(1,GPIO_NOPULL) |			/* SRAM_WP */		\
								PIN_PUPDR(2,GPIO_NOPULL) |			/* DEBUG_TX */		\
								PIN_PUPDR(3,GPIO_NOPULL) |			/* DEBUG_RX */		\
								PIN_PUPDR(4,GPIO_NOPULL) |			/* ETH_NSS */		\
								PIN_PUPDR(5,GPIO_NOPULL) |			/* ETH_SCK */		\
								PIN_PUPDR(6,GPIO_NOPULL) |			/* ETH_MISO */		\
								PIN_PUPDR(7,GPIO_NOPULL) |			/* ETH_MOSI */		\
								PIN_PUPDR(8,GPIO_NOPULL) |			/* ~SRAM_HOLD */	\
								PIN_PUPDR(9,GPIO_NOPULL) |			/* UART_TX */		\
								PIN_PUPDR(10,GPIO_PULLUP) |			/* UART_RX */		\
								PIN_PUPDR(11,GPIO_NOPULL) |			/* UART_CTS */		\
								PIN_PUPDR(12,GPIO_NOPULL) |			/* UART_RTS */		\
								PIN_PUPDR(13,GPIO_PULLUP) |			/* SWDIO */			\
								PIN_PUPDR(14,GPIO_PULLDOWN) |		/* SWCLK */			\
								PIN_PUPDR(15,GPIO_PULLUP)			/* ~ETH_RESET */

#define	CARIBOU_PORTA_AFRL		PIN_AFR(0,GPIO_AF_GPIO) |			/* ~ETH_WOL */		\
								PIN_AFR(1,GPIO_AF_GPIO) |			/* SRAM_WP */		\
								PIN_AFR(2,GPIO_AF_USART2) |			/* DEBUG_TX */		\
								PIN_AFR(3,GPIO_AF_USART2) |			/* DEBUG_RX */		\
								PIN_AFR(4,GPIO_AF_GPIO) |			/* ETH_NSS */		\
								PIN_AFR(5,GPIO_AF_SPI1) |			/* ETH_SCK */		\
								PIN_AFR(6,GPIO_AF_SPI1) |			/* ETH_MISO */		\
								PIN_AFR(7,GPIO_AF_SPI1)				/* ETH_MOSI */			
#define	CARIBOU_PORTA_AFRH		PIN_AFR(0,GPIO_AF_GPIO) |			/* ~SRAM_HOLD */	\
								PIN_AFR(1,GPIO_AF_USART1) |			/* UART_TX */		\
								PIN_AFR(2,GPIO_AF_USART1) |			/* UART_RX */		\
								PIN_AFR(3,GPIO_AF_GPIO) |			/* UART_CTS */		\
								PIN_AFR(4,GPIO_AF_GPIO) |			/* UART_RTS */		\
								PIN_AFR(5,GPIO_AF_GPIO) |			/* SWDIO */			\
								PIN_AFR(6,GPIO_AF_GPIO) |			/* SWCLK */			\
								PIN_AFR(7,GPIO_AF_GPIO)				/* ~ETH_RESET */

/**
 ** PORT B
 */
#define	CARIBOU_PORTB_MODE		PIN_MODE(0,GPIO_Mode_OUT) |			/* STATUS */		\
								PIN_MODE(1,GPIO_Mode_IN) |			/* ~INIT */			\
								PIN_MODE(2,GPIO_Mode_IN) |			/* BOOT1 (N/C) */	\
								PIN_MODE(3,GPIO_Mode_OUT) |			/* EEPROM_WP */		\
								PIN_MODE(4,GPIO_Mode_IN) |			/* ~ETH_INT */		\
								PIN_MODE(5,GPIO_Mode_IN) |			/* N/C */			\
								PIN_MODE(6,GPIO_Mode_OUT) |			/* EEPROM_SCL */	\
								PIN_MODE(7,GPIO_Mode_OUT) |			/* EEPROM_SDA */	\
								PIN_MODE(8,GPIO_Mode_AF) |			/* CAN1_RX */		\
								PIN_MODE(9,GPIO_Mode_AF) |			/* CAN1_TX */		\
								PIN_MODE(10,GPIO_Mode_IN) |			/* N/C */			\
								PIN_MODE(11,GPIO_Mode_IN) |			/* N/C */			\
								PIN_MODE(12,GPIO_Mode_OUT) |		/* ~SRAM_CS */		\
								PIN_MODE(13,GPIO_Mode_AF) |			/* SRAM_SCK */		\
								PIN_MODE(14,GPIO_Mode_AF) |			/* SRAM_MISO */		\
								PIN_MODE(15,GPIO_Mode_AF)			/* SRAM_MOSI */

#define	CARIBOU_PORTB_OTYPER	PIN_OTYPER(0,GPIO_OType_PP) |		/* STATUS */		\
								PIN_OTYPER(1,GPIO_OType_PP) |		/* ~INIT */			\
								PIN_OTYPER(2,GPIO_OType_PP) |		/* BOOT1 (N/C) */	\
								PIN_OTYPER(3,GPIO_OType_PP) |		/* EEPROM_WP */		\
								PIN_OTYPER(4,GPIO_OType_PP) |		/* ~ETH_INT */		\
								PIN_OTYPER(5,GPIO_OType_PP) |		/* N/C */			\
								PIN_OTYPER(6,GPIO_OType_OD) |		/* EEPROM_SCL */	\
								PIN_OTYPER(7,GPIO_OType_OD) |		/* EEPROM_SDA */	\
								PIN_OTYPER(8,GPIO_OType_PP) |		/* CAN1_RX */		\
								PIN_OTYPER(9,GPIO_OType_PP) |		/* CAN1_TX */		\
								PIN_OTYPER(10,GPIO_OType_PP) |		/* N/C */			\
								PIN_OTYPER(11,GPIO_OType_PP) |		/* N/C */			\
								PIN_OTYPER(12,GPIO_OType_PP) |		/* ~SRAM_CS */		\
								PIN_OTYPER(13,GPIO_OType_PP) |		/* SRAM_SCK */		\
								PIN_OTYPER(14,GPIO_OType_PP) |		/* SRAM_MISO */		\
								PIN_OTYPER(15,GPIO_OType_PP)		/* SRAM_MOSI */

#define	CARIBOU_PORTB_OSPEEDR	PIN_OSPEEDR(0,GPIO_SPEED_FREQ_LOW) |	/* STATUS */		\
								PIN_OSPEEDR(1,GPIO_SPEED_FREQ_LOW) |	/* ~INIT */			\
								PIN_OSPEEDR(2,GPIO_SPEED_FREQ_LOW) |	/* BOOT1 (N/C) */	\
								PIN_OSPEEDR(3,GPIO_SPEED_FREQ_LOW) |	/* EEPROM_WP */		\
								PIN_OSPEEDR(4,GPIO_SPEED_FREQ_LOW) |	/* ~ETH_INT */		\
								PIN_OSPEEDR(5,GPIO_SPEED_FREQ_LOW) |	/* N/C */			\
								PIN_OSPEEDR(6,GPIO_SPEED_FREQ_LOW) |	/* EEPROM_SCL */	\
								PIN_OSPEEDR(7,GPIO_SPEED_FREQ_LOW) |	/* EEPROM_SDA */	\
								PIN_OSPEEDR(8,GPIO_SPEED_FREQ_LOW) |	/* CAN1_RX */		\
								PIN_OSPEEDR(9,GPIO_SPEED_FREQ_LOW) |	/* CAN1_TX */		\
								PIN_OSPEEDR(10,GPIO_SPEED_FREQ_LOW) |	/* N/C */			\
								PIN_OSPEEDR(11,GPIO_SPEED_FREQ_LOW) |	/* N/C */			\
								PIN_OSPEEDR(12,GPIO_SPEED_FREQ_MEDIUM) |/* ~SRAM_CS */		\
								PIN_OSPEEDR(13,GPIO_SPEED_FREQ_HIGH) |	/* SRAM_SCK */		\
								PIN_OSPEEDR(14,GPIO_SPEED_FREQ_HIGH) |	/* SRAM_MISO */		\
								PIN_OSPEEDR(15,GPIO_SPEED_FREQ_HIGH)	/* SRAM_MOSI */

#define	CARIBOU_PORTB_PUPDR		PIN_PUPDR(0,GPIO_NOPULL) |			/* STATUS */		\
								PIN_PUPDR(1,GPIO_PULLUP) |			/* ~INIT */			\
								PIN_PUPDR(2,GPIO_NOPULL) |			/* BOOT1 (N/C) */	\
								PIN_PUPDR(3,GPIO_NOPULL) |			/* EEPROM_WP */		\
								PIN_PUPDR(4,GPIO_PULLUP) |			/* ~ETH_INT */		\
								PIN_PUPDR(5,GPIO_NOPULL) |			/* N/C */			\
								PIN_PUPDR(6,GPIO_PULLUP) |			/* EEPROM_SCL */	\
								PIN_PUPDR(7,GPIO_PULLUP) |			/* EEPROM_SDA */	\
								PIN_PUPDR(8,GPIO_NOPULL) |			/* CAN1_RX */		\
								PIN_PUPDR(9,GPIO_NOPULL) |			/* CAN1_TX */		\
								PIN_PUPDR(10,GPIO_NOPULL) |			/* N/C */			\
								PIN_PUPDR(11,GPIO_NOPULL) |			/* N/C */			\
								PIN_PUPDR(12,GPIO_NOPULL) |			/* ~SRAM_CS */		\
								PIN_PUPDR(13,GPIO_NOPULL) |			/* SRAM_SCK */		\
								PIN_PUPDR(14,GPIO_NOPULL) |			/* SRAM_MISO */		\
								PIN_PUPDR(15,GPIO_NOPULL)			/* SRAM_MOSI */

#define	CARIBOU_PORTB_AFRL		PIN_AFR(0,GPIO_AF_GPIO) |			/* STATUS */		\
								PIN_AFR(1,GPIO_AF_GPIO) |			/* ~INIT */			\
								PIN_AFR(2,GPIO_AF_GPIO) |			/* BOOT1 (N/C) */	\
								PIN_AFR(3,GPIO_AF_GPIO) |			/* EEPROM_WP */		\
								PIN_AFR(4,GPIO_AF_GPIO) |			/* ~ETH_INT */		\
								PIN_AFR(5,GPIO_AF_GPIO) |			/* N/C */			\
								PIN_AFR(6,GPIO_AF_GPIO) |			/* EEPROM_SCL */	\
								PIN_AFR(7,GPIO_AF_GPIO)				/* EEPROM_SDA */
#define	CARIBOU_PORTB_AFRH		PIN_AFR(0,GPIO_AF_CAN1) |			/* CAN1_RX */		\
								PIN_AFR(1,GPIO_AF_CAN1) |			/* CAN1_TX */		\
								PIN_AFR(2,GPIO_AF_GPIO) |			/* N/C */			\
								PIN_AFR(3,GPIO_AF_GPIO) |			/* N/C */			\
								PIN_AFR(4,GPIO_AF_GPIO) |			/* ~SRAM_CS */		\
								PIN_AFR(5,GPIO_AF_SPI2) |			/* SRAM_SCK */		\
								PIN_AFR(6,GPIO_AF_SPI2) |			/* SRAM_MISO */		\
								PIN_AFR(7,GPIO_AF_SPI2)				/* SRAM_MOSI */

/**
 ** PORT C
 */
#define	CARIBOU_PORTC_MODE		PIN_MODE(0,GPIO_Mode_IN) |	\
								PIN_MODE(1,GPIO_Mode_IN) |	\
								PIN_MODE(2,GPIO_Mode_IN) |	\
								PIN_MODE(3,GPIO_Mode_IN) |	\
								PIN_MODE(4,GPIO_Mode_IN) |	\
								PIN_MODE(5,GPIO_Mode_IN) |	\
								PIN_MODE(6,GPIO_Mode_IN) |	\
								PIN_MODE(7,GPIO_Mode_IN) |	\
								PIN_MODE(8,GPIO_Mode_IN) |	\
								PIN_MODE(9,GPIO_Mode_IN) |	\
								PIN_MODE(10,GPIO_Mode_IN) | \
								PIN_MODE(11,GPIO_Mode_IN) | \
								PIN_MODE(12,GPIO_Mode_IN) | \
								PIN_MODE(13,GPIO_Mode_IN) | \
								PIN_MODE(14,GPIO_Mode_IN) | \
								PIN_MODE(15,GPIO_Mode_IN)

#define	CARIBOU_PORTC_OTYPER	PIN_OTYPER(0,GPIO_OType_PP) |	\
								PIN_OTYPER(1,GPIO_OType_PP) |	\
								PIN_OTYPER(2,GPIO_OType_PP) |	\
								PIN_OTYPER(3,GPIO_OType_PP) |	\
								PIN_OTYPER(4,GPIO_OType_PP) |	\
								PIN_OTYPER(5,GPIO_OType_PP) |	\
								PIN_OTYPER(6,GPIO_OType_PP) |	\
								PIN_OTYPER(7,GPIO_OType_PP) |	\
								PIN_OTYPER(8,GPIO_OType_PP) |	\
								PIN_OTYPER(9,GPIO_OType_PP) |	\
								PIN_OTYPER(10,GPIO_OType_PP) |	\
								PIN_OTYPER(11,GPIO_OType_PP) |	\
								PIN_OTYPER(12,GPIO_OType_PP) |	\
								PIN_OTYPER(13,GPIO_OType_PP) |	\
								PIN_OTYPER(14,GPIO_OType_PP) |	\
								PIN_OTYPER(15,GPIO_OType_PP)

#define	CARIBOU_PORTC_OSPEEDR	PIN_OSPEEDR(0,GPIO_SPEED_FREQ_LOW) |	\
								PIN_OSPEEDR(1,GPIO_SPEED_FREQ_LOW) |	\
								PIN_OSPEEDR(2,GPIO_SPEED_FREQ_LOW) |	\
								PIN_OSPEEDR(3,GPIO_SPEED_FREQ_LOW) |	\
								PIN_OSPEEDR(4,GPIO_SPEED_FREQ_LOW) |	\
								PIN_OSPEEDR(5,GPIO_SPEED_FREQ_LOW) |	\
								PIN_OSPEEDR(6,GPIO_SPEED_FREQ_LOW) |	\
								PIN_OSPEEDR(7,GPIO_SPEED_FREQ_LOW) |	\
								PIN_OSPEEDR(8,GPIO_SPEED_FREQ_LOW) |	\
								PIN_OSPEEDR(9,GPIO_SPEED_FREQ_LOW) |	\
								PIN_OSPEEDR(10,GPIO_SPEED_FREQ_LOW) |	\
								PIN_OSPEEDR(11,GPIO_SPEED_FREQ_LOW) |	\
								PIN_OSPEEDR(12,GPIO_SPEED_FREQ_LOW) |	\
								PIN_OSPEEDR(13,GPIO_SPEED_FREQ_LOW) |	\
								PIN_OSPEEDR(14,GPIO_SPEED_FREQ_LOW) |	\
								PIN_OSPEEDR(15,GPIO_SPEED_FREQ_LOW)

#define	CARIBOU_PORTC_PUPDR		PIN_PUPDR(0,GPIO_NOPULL) |	\
								PIN_PUPDR(1,GPIO_NOPULL) |	\
								PIN_PUPDR(2,GPIO_NOPULL) |	\
								PIN_PUPDR(3,GPIO_NOPULL) |	\
								PIN_PUPDR(4,GPIO_NOPULL) |	\
								PIN_PUPDR(5,GPIO_NOPULL) |	\
								PIN_PUPDR(6,GPIO_NOPULL) |	\
								PIN_PUPDR(7,GPIO_NOPULL) |	\
								PIN_PUPDR(8,GPIO_NOPULL) |	\
								PIN_PUPDR(9,GPIO_NOPULL) |	\
								PIN_PUPDR(10,GPIO_NOPULL) |	\
								PIN_PUPDR(11,GPIO_NOPULL) |	\
								PIN_PUPDR(12,GPIO_NOPULL) |	\
								PIN_PUPDR(13,GPIO_PULLDOWN) |	\
								PIN_PUPDR(14,GPIO_PULLDOWN) |	\
								PIN_PUPDR(15,GPIO_PULLDOWN)

#define	CARIBOU_PORTC_AFRL		PIN_AFR(0,GPIO_AF_GPIO) |	\
								PIN_AFR(1,GPIO_AF_GPIO) |	\
								PIN_AFR(2,GPIO_AF_GPIO) |	\
								PIN_AFR(3,GPIO_AF_GPIO) |	\
								PIN_AFR(4,GPIO_AF_GPIO) |	\
								PIN_AFR(5,GPIO_AF_GPIO) |	\
								PIN_AFR(6,GPIO_AF_GPIO) |	\
								PIN_AFR(7,GPIO_AF_GPIO)
#define	CARIBOU_PORTC_AFRH		PIN_AFR(0,GPIO_AF_GPIO) |	\
								PIN_AFR(1,GPIO_AF_GPIO) |	\
								PIN_AFR(2,GPIO_AF_GPIO) |	\
								PIN_AFR(3,GPIO_AF_GPIO) |	\
								PIN_AFR(4,GPIO_AF_GPIO) |	\
								PIN_AFR(5,GPIO_AF_GPIO) |	\
								PIN_AFR(6,GPIO_AF_GPIO) |	\
								PIN_AFR(7,GPIO_AF_GPIO)

/**
 ** PORT H
 */
#define	CARIBOU_PORTH_MODE		PIN_MODE(0,GPIO_Mode_IN) |	\
								PIN_MODE(1,GPIO_Mode_IN) |	\
								PIN_MODE(2,GPIO_Mode_IN) |	\
								PIN_MODE(3,GPIO_Mode_IN) |	\
								PIN_MODE(4,GPIO_Mode_IN) |	\
								PIN_MODE(5,GPIO_Mode_IN) |	\
								PIN_MODE(6,GPIO_Mode_IN) |	\
								PIN_MODE(7,GPIO_Mode_IN) |	\
								PIN_MODE(8,GPIO_Mode_IN) |	\
								PIN_MODE(9,GPIO_Mode_IN) |	\
								PIN_MODE(10,GPIO_Mode_IN) | \
								PIN_MODE(11,GPIO_Mode_IN) | \
								PIN_MODE(12,GPIO_Mode_IN) | \
								PIN_MODE(13,GPIO_Mode_IN) | \
								PIN_MODE(14,GPIO_Mode_IN) | \
								PIN_MODE(15,GPIO_Mode_IN)

#define	CARIBOU_PORTH_OTYPER	PIN_OTYPER(0,GPIO_OType_PP) |	\
								PIN_OTYPER(1,GPIO_OType_PP) |	\
								PIN_OTYPER(2,GPIO_OType_PP) |	\
								PIN_OTYPER(3,GPIO_OType_PP) |	\
								PIN_OTYPER(4,GPIO_OType_PP) |	\
								PIN_OTYPER(5,GPIO_OType_PP) |	\
								PIN_OTYPER(6,GPIO_OType_PP) |	\
								PIN_OTYPER(7,GPIO_OType_PP) |	\
								PIN_OTYPER(8,GPIO_OType_PP) |	\
								PIN_OTYPER(9,GPIO_OType_PP) |	\
								PIN_OTYPER(10,GPIO_OType_PP) |	\
								PIN_OTYPER(11,GPIO_OType_PP) |	\
								PIN_OTYPER(12,GPIO_OType_PP) |	\
								PIN_OTYPER(13,GPIO_OType_PP) |	\
								PIN_OTYPER(14,GPIO_OType_PP) |	\
								PIN_OTYPER(15,GPIO_OType_PP)

#define	CARIBOU_PORTH_OSPEEDR	PIN_OSPEEDR(0,GPIO_SPEED_FREQ_LOW) |	\
								PIN_OSPEEDR(1,GPIO_SPEED_FREQ_LOW) |	\
								PIN_OSPEEDR(2,GPIO_SPEED_FREQ_LOW) |	\
								PIN_OSPEEDR(3,GPIO_SPEED_FREQ_LOW) |	\
								PIN_OSPEEDR(4,GPIO_SPEED_FREQ_LOW) |	\
								PIN_OSPEEDR(5,GPIO_SPEED_FREQ_LOW) |	\
								PIN_OSPEEDR(6,GPIO_SPEED_FREQ_LOW) |	\
								PIN_OSPEEDR(7,GPIO_SPEED_FREQ_LOW) |	\
								PIN_OSPEEDR(8,GPIO_SPEED_FREQ_LOW) |	\
								PIN_OSPEEDR(9,GPIO_SPEED_FREQ_LOW) |	\
								PIN_OSPEEDR(10,GPIO_SPEED_FREQ_LOW) |	\
								PIN_OSPEEDR(11,GPIO_SPEED_FREQ_LOW) |	\
								PIN_OSPEEDR(12,GPIO_SPEED_FREQ_LOW) |	\
								PIN_OSPEEDR(13,GPIO_SPEED_FREQ_LOW) |	\
								PIN_OSPEEDR(14,GPIO_SPEED_FREQ_LOW) |	\
								PIN_OSPEEDR(15,GPIO_SPEED_FREQ_LOW)

#define	CARIBOU_PORTH_PUPDR		PIN_PUPDR(0,GPIO_PULLDOWN) |	\
								PIN_PUPDR(1,GPIO_PULLDOWN) |	\
								PIN_PUPDR(2,GPIO_PULLDOWN) |	\
								PIN_PUPDR(3,GPIO_PULLDOWN) |	\
								PIN_PUPDR(4,GPIO_PULLDOWN) |	\
								PIN_PUPDR(5,GPIO_PULLDOWN) |	\
								PIN_PUPDR(6,GPIO_PULLDOWN) |	\
								PIN_PUPDR(7,GPIO_PULLDOWN) |	\
								PIN_PUPDR(8,GPIO_PULLDOWN) |	\
								PIN_PUPDR(9,GPIO_PULLDOWN) |	\
								PIN_PUPDR(10,GPIO_PULLDOWN) |	\
								PIN_PUPDR(11,GPIO_PULLDOWN) |	\
								PIN_PUPDR(12,GPIO_PULLDOWN) |	\
								PIN_PUPDR(13,GPIO_PULLDOWN) |	\
								PIN_PUPDR(14,GPIO_PULLDOWN) |	\
								PIN_PUPDR(15,GPIO_PULLDOWN)

#define	CARIBOU_PORTH_AFRL		PIN_AFR(0,GPIO_AF_GPIO) |	\
								PIN_AFR(1,GPIO_AF_GPIO) |	\
								PIN_AFR(2,GPIO_AF_GPIO) |	\
								PIN_AFR(3,GPIO_AF_GPIO) |	\
								PIN_AFR(4,GPIO_AF_GPIO) |	\
								PIN_AFR(5,GPIO_AF_GPIO) |	\
								PIN_AFR(6,GPIO_AF_GPIO) |	\
								PIN_AFR(7,GPIO_AF_GPIO)
#define	CARIBOU_PORTH_AFRH		PIN_AFR(0,GPIO_AF_GPIO) |	\
								PIN_AFR(1,GPIO_AF_GPIO) |	\
								PIN_AFR(2,GPIO_AF_GPIO) |	\
								PIN_AFR(3,GPIO_AF_GPIO) |	\
								PIN_AFR(4,GPIO_AF_GPIO) |	\
								PIN_AFR(5,GPIO_AF_GPIO) |	\
								PIN_AFR(6,GPIO_AF_GPIO) |	\
								PIN_AFR(7,GPIO_AF_GPIO)


/**
 ** @brief A hook to perform early board initialization. Static stacks are initialized, BSS is *NOT* initialized at this stage.
 **/
extern void early_init();

/**
 ** @brief A hook to perform late initialization. Static stacks, BSS, heap, and static contructors are all initialized at this stage.
 **/
extern void late_init();

/**
 ** @brief A hok for board specific idle time
 */
extern void sleep(uint32_t ms);
extern void usleep(uint32_t us);

extern void board_reset();

extern void NMI_Handler(void);
extern void HardFault_Handler(void);
extern void SVC_Handler(void);
extern void PendSV_Handler(void);
extern void SysTick_Handler(void);
extern void EXTI0_1_IRQHandler(void);

extern uint16_t __attribute__((naked)) flip16(uint16_t x);


#ifdef __cplusplus
}
#endif

#endif 