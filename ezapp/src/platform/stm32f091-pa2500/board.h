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
#ifndef _EZAPP_STM32F091_BOARD_H_
#define _EZAPP_STM32F091_BOARD_H_

#include <caribou.h>
#include <caribou/lib/stdio.h>
#include <caribou/dev/gpio.h>
#include <chip/chip.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief These GPIO pins have been defined in board.h
 */
extern caribou_gpio_t	gpio_enc28j60_nss;
extern caribou_gpio_t	gpio_enc28j60_reset;
extern caribou_gpio_t	gpio_enc28j60_int;
extern caribou_gpio_t	gpio_rs485_dir;
extern caribou_gpio_t	gpio_eeprom_wp;
extern caribou_gpio_t	gpio_i2c_scl;
extern caribou_gpio_t	gpio_i2c_sda;
extern caribou_gpio_t	gpio_status;
extern caribou_gpio_t	gpio_init;

#define rs485_rx()				caribou_gpio_reset(&gpio_rs485_dir);	/* RS485 Receive Direction */
#define rs485_tx()				caribou_gpio_set(&gpio_rs485_dir);		/* RS485 Transmit Direction */

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
#define GPIO_AF_USART1			GPIO_AF_1
#define GPIO_AF_USART2			GPIO_AF_1
#define GPIO_AF_SPI1			GPIO_AF_0
#define GPIO_AF_SPI2			GPIO_AF_0

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

#define	CARIBOU_PORTA_OSPEEDR	PIN_OSPEEDR(0,GPIO_Speed_2MHz) |	/* ~ETH_WOL */		\
								PIN_OSPEEDR(1,GPIO_Speed_10MHz) |	/* SRAM_WP */		\
								PIN_OSPEEDR(2,GPIO_Speed_2MHz) |	/* DEBUG_TX */		\
								PIN_OSPEEDR(3,GPIO_Speed_2MHz) |	/* DEBUG_RX */		\
								PIN_OSPEEDR(4,GPIO_Speed_50MHz) |	/* ETH_NSS */		\
								PIN_OSPEEDR(5,GPIO_Speed_50MHz) |	/* ETH_SCK */		\
								PIN_OSPEEDR(6,GPIO_Speed_50MHz) |	/* ETH_MISO */		\
								PIN_OSPEEDR(7,GPIO_Speed_50MHz) |	/* ETH_MOSI */		\
								PIN_OSPEEDR(8,GPIO_Speed_10MHz) |	/* ~SRAM_HOLD */	\
								PIN_OSPEEDR(9,GPIO_Speed_2MHz) |	/* UART_TX */		\
								PIN_OSPEEDR(10,GPIO_Speed_2MHz) |	/* UART_RX */		\
								PIN_OSPEEDR(11,GPIO_Speed_2MHz) |	/* UART_CTS */		\
								PIN_OSPEEDR(12,GPIO_Speed_2MHz) |	/* UART_RTS */		\
								PIN_OSPEEDR(13,GPIO_Speed_50MHz) |	/* SWDIO */			\
								PIN_OSPEEDR(14,GPIO_Speed_50MHz) |	/* SWCLK */			\
								PIN_OSPEEDR(15,GPIO_Speed_10MHz)		/* ~ETH_RESET */

#define	CARIBOU_PORTA_PUPDR		PIN_PUPDR(0,GPIO_PuPd_UP) |			/* ~ETH_WOL */		\
								PIN_PUPDR(1,GPIO_PuPd_NOPULL) |		/* SRAM_WP */		\
								PIN_PUPDR(2,GPIO_PuPd_NOPULL) |		/* DEBUG_TX */		\
								PIN_PUPDR(3,GPIO_PuPd_NOPULL) |		/* DEBUG_RX */		\
								PIN_PUPDR(4,GPIO_PuPd_NOPULL) |		/* ETH_NSS */		\
								PIN_PUPDR(5,GPIO_PuPd_NOPULL) |		/* ETH_SCK */		\
								PIN_PUPDR(6,GPIO_PuPd_NOPULL) |		/* ETH_MISO */		\
								PIN_PUPDR(7,GPIO_PuPd_NOPULL) |		/* ETH_MOSI */		\
								PIN_PUPDR(8,GPIO_PuPd_NOPULL) |		/* ~SRAM_HOLD */	\
								PIN_PUPDR(9,GPIO_PuPd_NOPULL) |		/* UART_TX */		\
								PIN_PUPDR(10,GPIO_PuPd_UP) |		/* UART_RX */		\
								PIN_PUPDR(11,GPIO_PuPd_NOPULL) |	/* UART_CTS */		\
								PIN_PUPDR(12,GPIO_PuPd_NOPULL) |	/* UART_RTS */		\
								PIN_PUPDR(13,GPIO_PuPd_UP) |		/* SWDIO */			\
								PIN_PUPDR(14,GPIO_PuPd_DOWN) |		/* SWCLK */			\
								PIN_PUPDR(15,GPIO_PuPd_UP)			/* ~ETH_RESET */

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
#define	CARIBOU_PORTB_CONFIG_L	PIN_MODE(0,GPIO_CNF_OUT_PP | GPIO_MODE_OUT_2MHZ) |	/* STATUS */		\
								PIN_MODE(1,GPIO_CNF_IN | GPIO_MODE_IN) |			/* ~INIT */			\
								PIN_MODE(2,GPIO_CNF_IN | GPIO_MODE_IN) |			/* BOOT1 */			\
								PIN_MODE(3,GPIO_CNF_OUT_PP | GPIO_MODE_OUT_2MHZ) |	/* EEPROM_WP */		\
								PIN_MODE(4,GPIO_CNF_IN_PUD | GPIO_MODE_IN) |		/* ~ETH_INT */		\
								PIN_MODE(5,GPIO_CNF_IN | GPIO_MODE_IN) |			/* N/C */			\
								PIN_MODE(6,GPIO_CNF_OUT_OD | GPIO_MODE_OUT_2MHZ) |	/* EEPROM_SCL */	\
								PIN_MODE(7,GPIO_CNF_OUT_OD | GPIO_MODE_OUT_2MHZ)	/* EEPROM_SDA */
#define	CARIBOU_PORTB_CONFIG_H	PIN_MODE(0,GPIO_CNF_IN | GPIO_MODE_IN) |			/* N/C */			\
								PIN_MODE(1,GPIO_CNF_IN | GPIO_MODE_IN) |			/* N/C */			\
								PIN_MODE(2,GPIO_CNF_IN | GPIO_MODE_IN) |			/* N/C */			\
								PIN_MODE(3,GPIO_CNF_IN | GPIO_MODE_IN) |			/* N/C */			\
								PIN_MODE(4,GPIO_CNF_OUT_PP | GPIO_MODE_OUT_10MHZ) |	/* ~SRAM_CS */		\
								PIN_MODE(5,GPIO_CNF_AF_PP | GPIO_MODE_OUT_10MHZ) |	/* SRAM_SCK */		\
								PIN_MODE(6,GPIO_CNF_IN | GPIO_MODE_IN) |			/* SRAM_MISO */		\
								PIN_MODE(7,GPIO_CNF_AF_PP | GPIO_MODE_OUT_10MHZ)	/* SRAM_MOSI */

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
								PIN_MODE(8,GPIO_Mode_IN) |			/* N/C */			\
								PIN_MODE(9,GPIO_Mode_IN) |			/* N/C */			\
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
								PIN_OTYPER(8,GPIO_OType_PP) |		/* N/C */			\
								PIN_OTYPER(9,GPIO_OType_PP) |		/* N/C */			\
								PIN_OTYPER(10,GPIO_OType_PP) |		/* N/C */			\
								PIN_OTYPER(11,GPIO_OType_PP) |		/* N/C */			\
								PIN_OTYPER(12,GPIO_OType_PP) |		/* ~SRAM_CS */		\
								PIN_OTYPER(13,GPIO_OType_PP) |		/* SRAM_SCK */		\
								PIN_OTYPER(14,GPIO_OType_PP) |		/* SRAM_MISO */		\
								PIN_OTYPER(15,GPIO_OType_PP)		/* SRAM_MOSI */

#define	CARIBOU_PORTB_OSPEEDR	PIN_OSPEEDR(0,GPIO_Speed_2MHz) |	/* STATUS */		\
								PIN_OSPEEDR(1,GPIO_Speed_2MHz) |	/* ~INIT */			\
								PIN_OSPEEDR(2,GPIO_Speed_2MHz) |	/* BOOT1 (N/C) */	\
								PIN_OSPEEDR(3,GPIO_Speed_2MHz) |	/* EEPROM_WP */		\
								PIN_OSPEEDR(4,GPIO_Speed_2MHz) |	/* ~ETH_INT */		\
								PIN_OSPEEDR(5,GPIO_Speed_2MHz) |	/* N/C */			\
								PIN_OSPEEDR(6,GPIO_Speed_2MHz) |	/* EEPROM_SCL */	\
								PIN_OSPEEDR(7,GPIO_Speed_2MHz) |	/* EEPROM_SDA */	\
								PIN_OSPEEDR(8,GPIO_Speed_2MHz) |	/* N/C */			\
								PIN_OSPEEDR(9,GPIO_Speed_2MHz) |	/* N/C */			\
								PIN_OSPEEDR(10,GPIO_Speed_2MHz) |	/* N/C */			\
								PIN_OSPEEDR(11,GPIO_Speed_2MHz) |	/* N/C */			\
								PIN_OSPEEDR(12,GPIO_Speed_10MHz) |	/* ~SRAM_CS */		\
								PIN_OSPEEDR(13,GPIO_Speed_50MHz) |	/* SRAM_SCK */		\
								PIN_OSPEEDR(14,GPIO_Speed_50MHz) |	/* SRAM_MISO */		\
								PIN_OSPEEDR(15,GPIO_Speed_50MHz)	/* SRAM_MOSI */

#define	CARIBOU_PORTB_PUPDR		PIN_PUPDR(0,GPIO_PuPd_NOPULL) |		/* STATUS */		\
								PIN_PUPDR(1,GPIO_PuPd_UP) |			/* ~INIT */			\
								PIN_PUPDR(2,GPIO_PuPd_NOPULL) |		/* BOOT1 (N/C) */	\
								PIN_PUPDR(3,GPIO_PuPd_NOPULL) |		/* EEPROM_WP */		\
								PIN_PUPDR(4,GPIO_PuPd_UP) |			/* ~ETH_INT */		\
								PIN_PUPDR(5,GPIO_PuPd_NOPULL) |		/* N/C */			\
								PIN_PUPDR(6,GPIO_PuPd_NOPULL) |		/* EEPROM_SCL */	\
								PIN_PUPDR(7,GPIO_PuPd_NOPULL) |		/* EEPROM_SDA */	\
								PIN_PUPDR(8,GPIO_PuPd_NOPULL) |		/* N/C */			\
								PIN_PUPDR(9,GPIO_PuPd_NOPULL) |		/* N/C */			\
								PIN_PUPDR(10,GPIO_PuPd_NOPULL) |	/* N/C */			\
								PIN_PUPDR(11,GPIO_PuPd_NOPULL) |	/* N/C */			\
								PIN_PUPDR(12,GPIO_PuPd_NOPULL) |	/* ~SRAM_CS */		\
								PIN_PUPDR(13,GPIO_PuPd_NOPULL) |	/* SRAM_SCK */		\
								PIN_PUPDR(14,GPIO_PuPd_NOPULL) |	/* SRAM_MISO */		\
								PIN_PUPDR(15,GPIO_PuPd_NOPULL)		/* SRAM_MOSI */

#define	CARIBOU_PORTB_AFRL		PIN_AFR(0,GPIO_AF_GPIO) |			/* STATUS */		\
								PIN_AFR(1,GPIO_AF_GPIO) |			/* ~INIT */			\
								PIN_AFR(2,GPIO_AF_GPIO) |			/* BOOT1 (N/C) */	\
								PIN_AFR(3,GPIO_AF_GPIO) |			/* EEPROM_WP */		\
								PIN_AFR(4,GPIO_AF_GPIO) |			/* ~ETH_INT */		\
								PIN_AFR(5,GPIO_AF_GPIO) |			/* N/C */			\
								PIN_AFR(6,GPIO_AF_GPIO) |			/* EEPROM_SCL */	\
								PIN_AFR(7,GPIO_AF_GPIO)				/* EEPROM_SDA */
#define	CARIBOU_PORTB_AFRH		PIN_AFR(0,GPIO_AF_GPIO) |			/* N/C */			\
								PIN_AFR(1,GPIO_AF_GPIO) |			/* N/C */			\
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

#define	CARIBOU_PORTC_OSPEEDR	PIN_OSPEEDR(0,GPIO_Speed_2MHz) |	\
								PIN_OSPEEDR(1,GPIO_Speed_2MHz) |	\
								PIN_OSPEEDR(2,GPIO_Speed_2MHz) |	\
								PIN_OSPEEDR(3,GPIO_Speed_2MHz) |	\
								PIN_OSPEEDR(4,GPIO_Speed_2MHz) |	\
								PIN_OSPEEDR(5,GPIO_Speed_2MHz) |	\
								PIN_OSPEEDR(6,GPIO_Speed_2MHz) |	\
								PIN_OSPEEDR(7,GPIO_Speed_2MHz) |	\
								PIN_OSPEEDR(8,GPIO_Speed_2MHz) |	\
								PIN_OSPEEDR(9,GPIO_Speed_2MHz) |	\
								PIN_OSPEEDR(10,GPIO_Speed_2MHz) |	\
								PIN_OSPEEDR(11,GPIO_Speed_2MHz) |	\
								PIN_OSPEEDR(12,GPIO_Speed_2MHz) |	\
								PIN_OSPEEDR(13,GPIO_Speed_2MHz) |	\
								PIN_OSPEEDR(14,GPIO_Speed_2MHz) |	\
								PIN_OSPEEDR(15,GPIO_Speed_2MHz)

#define	CARIBOU_PORTC_PUPDR		PIN_PUPDR(0,GPIO_PuPd_NOPULL) |	\
								PIN_PUPDR(1,GPIO_PuPd_NOPULL) |	\
								PIN_PUPDR(2,GPIO_PuPd_NOPULL) |	\
								PIN_PUPDR(3,GPIO_PuPd_NOPULL) |	\
								PIN_PUPDR(4,GPIO_PuPd_NOPULL) |	\
								PIN_PUPDR(5,GPIO_PuPd_NOPULL) |	\
								PIN_PUPDR(6,GPIO_PuPd_NOPULL) |	\
								PIN_PUPDR(7,GPIO_PuPd_NOPULL) |	\
								PIN_PUPDR(8,GPIO_PuPd_NOPULL) |	\
								PIN_PUPDR(9,GPIO_PuPd_NOPULL) |	\
								PIN_PUPDR(10,GPIO_PuPd_NOPULL) |	\
								PIN_PUPDR(11,GPIO_PuPd_NOPULL) |	\
								PIN_PUPDR(12,GPIO_PuPd_NOPULL) |	\
								PIN_PUPDR(13,GPIO_PuPd_UP) |	\
								PIN_PUPDR(14,GPIO_PuPd_UP) |	\
								PIN_PUPDR(15,GPIO_PuPd_UP)

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
 ** PORT D
 */
#define	CARIBOU_PORTD_MODE		PIN_MODE(0,GPIO_Mode_IN) |	\
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

#define	CARIBOU_PORTD_OTYPER	PIN_OTYPER(0,GPIO_OType_PP) |	\
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

#define	CARIBOU_PORTD_OSPEEDR	PIN_OSPEEDR(0,GPIO_Speed_2MHz) |	\
								PIN_OSPEEDR(1,GPIO_Speed_2MHz) |	\
								PIN_OSPEEDR(2,GPIO_Speed_2MHz) |	\
								PIN_OSPEEDR(3,GPIO_Speed_2MHz) |	\
								PIN_OSPEEDR(4,GPIO_Speed_2MHz) |	\
								PIN_OSPEEDR(5,GPIO_Speed_2MHz) |	\
								PIN_OSPEEDR(6,GPIO_Speed_2MHz) |	\
								PIN_OSPEEDR(7,GPIO_Speed_2MHz) |	\
								PIN_OSPEEDR(8,GPIO_Speed_2MHz) |	\
								PIN_OSPEEDR(9,GPIO_Speed_2MHz) |	\
								PIN_OSPEEDR(10,GPIO_Speed_2MHz) |	\
								PIN_OSPEEDR(11,GPIO_Speed_2MHz) |	\
								PIN_OSPEEDR(12,GPIO_Speed_2MHz) |	\
								PIN_OSPEEDR(13,GPIO_Speed_2MHz) |	\
								PIN_OSPEEDR(14,GPIO_Speed_2MHz) |	\
								PIN_OSPEEDR(15,GPIO_Speed_2MHz)

#define	CARIBOU_PORTD_PUPDR		PIN_PUPDR(0,GPIO_PuPd_NOPULL) |	\
								PIN_PUPDR(1,GPIO_PuPd_NOPULL) |	\
								PIN_PUPDR(2,GPIO_PuPd_NOPULL) |	\
								PIN_PUPDR(3,GPIO_PuPd_NOPULL) |	\
								PIN_PUPDR(4,GPIO_PuPd_NOPULL) |	\
								PIN_PUPDR(5,GPIO_PuPd_NOPULL) |	\
								PIN_PUPDR(6,GPIO_PuPd_NOPULL) |	\
								PIN_PUPDR(7,GPIO_PuPd_NOPULL) |	\
								PIN_PUPDR(8,GPIO_PuPd_NOPULL) |	\
								PIN_PUPDR(9,GPIO_PuPd_NOPULL) |	\
								PIN_PUPDR(10,GPIO_PuPd_NOPULL) |	\
								PIN_PUPDR(11,GPIO_PuPd_NOPULL) |	\
								PIN_PUPDR(12,GPIO_PuPd_NOPULL) |	\
								PIN_PUPDR(13,GPIO_PuPd_UP) |	\
								PIN_PUPDR(14,GPIO_PuPd_UP) |	\
								PIN_PUPDR(15,GPIO_PuPd_UP)

#define	CARIBOU_PORTD_AFRL		PIN_AFR(0,GPIO_AF_GPIO) |	\
								PIN_AFR(1,GPIO_AF_GPIO) |	\
								PIN_AFR(2,GPIO_AF_GPIO) |	\
								PIN_AFR(3,GPIO_AF_GPIO) |	\
								PIN_AFR(4,GPIO_AF_GPIO) |	\
								PIN_AFR(5,GPIO_AF_GPIO) |	\
								PIN_AFR(6,GPIO_AF_GPIO) |	\
								PIN_AFR(7,GPIO_AF_GPIO)
#define	CARIBOU_PORTD_AFRH		PIN_AFR(0,GPIO_AF_GPIO) |	\
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
extern void early_init(void);

/**
 ** @brief A hook to perform late initialization. Static stacks, BSS, heap, and static contructors are all initialized at this stage.
 **/
extern void late_init(void);

/**
 ** @brief A hok for board specific idle time
 */
extern void board_idle(void);

#endif

#ifdef __cplusplus
}
#endif
