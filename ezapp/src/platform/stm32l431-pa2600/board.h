/******************************************************************************
* Copyright © 2016 by Pike Aerospace Research Corporation
* All Rights Reserved
******************************************************************************/
#ifndef _PA2600_BOARD_H_
#define _PA2600_BOARD_H_

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

#define PRODUCT_WATCHDOG_ENABLED		(1)

#define PRODUCT_HTTP_ROOT				"/"

#define PRODUCT_HTTP_ENABLED			(1)
#define PRODUCT_CANBUS_ENABLED			(1)
#define PRODUCT_PAP_ENABLED				(1)

#define PRODUCT_DEBUG_CANBUS_SERVER		(0)
#define PRODUCT_DEBUG_CANBUS_SESSION	(0)
#define PRODUCT_DEBUG_HTTP_SERVER		(0)
#define PRODUCT_DEBUG_HTTP_SESSION		(0)
#define PRODUCT_DEBUG_PAP_SESSION		(0)
#define PRODUCT_DEBUG_SETTINGS			(0)
#define PRODUCT_DEBUG_HEAP_FREE			(0)

#define PRODUCT_DESCRIPTION				"PIKEAERO.COM : PA2600 CAN BUS Gateway"
#define PRODUCT_HOSTNAME				"PA2600"
#define	PRODUCT_VERSION					"1.0.0"
#define PRODUCT_VERSION_HW				"C"
#define PRODUCT_COPYRIGHT				"Copyright(c)2019 by Pike Aerospace Research Corp."
#define PRODUCT_YEAR 					"2019"
#define PRODUCT_URL						"http://www.pikeaero.com/pa2600"
#define PRODUCT_COMPANY					"Pike Aerospace Research Corp."

#define PRODUCT_HTTP_SERVER_NAME		"httpd"
#define PRODUCT_HTTP_SESSION_NAME		"http"
#define PRODUCT_CANBUS_SERVER_NAME		"cbd"
#define PRODUCT_CANBUS_SESSION_NAME		"cb"
#define PRODUCT_PAP_SERVER_NAME			"pap"

#define PRODUCT_WD_PERIOD_MS			(1024)
#if PRODUCT_WATCHDOG_ENABLED
	#define PRODUCT_HTTPD_WD_COUNT			(0)
	#define PRODUCT_HTTP_WD_COUNT			(200)
	#define PRODUCT_MBD_WD_COUNT			(0)
	#define PRODUCT_MB_WD_COUNT				(1000)
	#define PRODUCT_ETHIN_WD_COUNT			(200)
	#define PRODUCT_PAP_WD_COUNT			(0)
#else

	#define PRODUCT_HTTPD_WD_COUNT			(0)
	#define PRODUCT_HTTP_WD_COUNT			(0)
	#define PRODUCT_MBD_WD_COUNT			(0)
	#define PRODUCT_MB_WD_COUNT				(0)
	#define PRODUCT_ETHIN_WD_COUNT			(0)
	#define PRODUCT_PAP_WD_COUNT			(0)
#endif

#define PRODUCT_HTTP_SERVER_ROOT		"/"
#define CANBUS_SESSION_SEMAPHORE_COUNT	(1)

#define STATIC_ADDRESS(a1,a2,a3,a4) (uint32_t)(a1<<24)|(a2<<16)|(a3<<8)|(a4)
#define PIKEAERO_MULTICAST_GROUP			STATIC_ADDRESS(239,255,255,128)
#define PIKEAERO_MULTICAST_GROUP_STRING		"239.255.255.128"
#define PIKEAERO_MULTICAST_GROUP_OCTETS		{239,255,255,128}
#define PIKEAERO_MULTICAST_PORT				(5454)
#define PIKEAERO_MULTICAST_BACKLOG			(8)

#define PRODUCT_HTTP_PORT					(80)
#define PRODUCT_HTTP_BACKLOG				(4)
#define PRODUCT_CANBUS_PORT					(502)
#define PRODUCT_CANBUS_BACKLOG				(2)

#define	PIKEAERO_PAP_SIGNATURE							"PIKEAER0"	/* 'PIKEAER0' */
#define	PIKEAERO_PAP_SIGNATURE_SZ						8			/* 'PIKEAER0' */
#define	PIKEAERO_PAP_HOSTNAME_SZ						32			/* 'HOSTNAME' */
#define PIKEAERO_PAP_DHCP_MASK							0x01		/* DHCP bit mask */

#define PIKEAERO_PAP_TYPE_REQUEST_MASK                  0x0F
#define PIKEAERO_PAP_TYPE_RESPONSE_MASK                 0xF0

#define PIKEAERO_PAP_TYPE_DISCOVER_REQUEST				0x01		/* A discovery request was received */
#define PIKEAERO_PAP_TYPE_DISCOVER_RESPONSE				0x10		/* Discovery response sent out */

#define	PIKEAERO_PAP_TYPE_IP_SETTINGS_REQUEST			0x02		/* A request to change IP settings */
#define	PIKEAERO_PAP_TYPE_IP_SETTINGS_RESPONSE			0x20		/* A request to change IP settings */

#define	PIKEAERO_PAP_TYPE_RESET_REQUEST					0x03		/* A request to reset the device */
#define	PIKEAERO_PAP_TYPE_RESET_RESPONSE				0x30		/* A confirmation response to reset request */

#define PRODUCT_I2CSPEED				(100)
#define PRODUCT_EEPROM_BPP				(16)	/* EEPROM Bytes Per Page */

#define PRODUCT_FILE_XFR_BUFSZ			(256)

#define PRODUCT_HTTP_SEND_BUFFER_SZ		(720)	/* HTTP send buffer overflow size */
//#define PRODUCT_HTTP_SEND_BUFFER_SZ		(256)	/* HTTP send buffer overflow size */

#define PRODUCT_THREAD_NORMAL_PRIO		(1)
#define PRODUCT_DHCP_THREAD_PRIORITY	PRODUCT_THREAD_NORMAL_PRIO
#define PRODUCT_HTTP_THREAD_PRIO		PRODUCT_THREAD_NORMAL_PRIO
#define PRODUCT_HTTP_SESSION_PRIO		PRODUCT_THREAD_NORMAL_PRIO
#define PRODUCT_CANBUS_THREAD_PRIO		PRODUCT_THREAD_NORMAL_PRIO
#define PRODUCT_CANBUS_SESSION_PRIO		PRODUCT_THREAD_NORMAL_PRIO
#define PRODUCT_ETHIF_THREAD_PRIO		PRODUCT_THREAD_NORMAL_PRIO
#define PRODUCT_PAP_THREAD_PRIO			PRODUCT_THREAD_NORMAL_PRIO

#define PRODUCT_DHCP_THREAD_STACK_SZ	(1200)
#define PRODUCT_HTTP_SERVER_STK_SZ		(512*3)
#define PRODUCT_HTTP_THREAD_STK_SZ		(512*6)
#define PRODUCT_CANBUS_SERVER_STK_SZ	(512*2)
#define PRODUCT_CANBUS_THREAD_STK_SZ	(512*3)
#define PRODUCT_ETHIF_THREAD_STK_SZ		(1200)
#define PRODUCT_PAP_THREAD_STK_SZ		(1200)

#if PA_DEBUG
	extern int debug_printf(const char *format, ...);
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

extern char* board_get_hostname();
extern void  board_set_net_hostname(char* hostname);

/* MAC ADDRESS*/
#define MAC_ADDR0   0x00
#define MAC_ADDR1   0x00
#define MAC_ADDR2   0xCF
#define MAC_ADDR3   0xFF
#define MAC_ADDR4   0xFF
#define MAC_ADDR5   0xFE
 
#if defined(AP_ADDR1_TEST)

	/*Static IP ADDRESS*/
	#define IP_ADDR0   192
	#define IP_ADDR1   168
	#define IP_ADDR2   0
	#define IP_ADDR3   10
   
	/*Gateway Address*/
	#define GW_ADDR0   192
	#define GW_ADDR1   168
	#define GW_ADDR2   0
	#define GW_ADDR3   1  

#else

	/*Static IP ADDRESS*/
	#define IP_ADDR0   10
	#define IP_ADDR1   84
	#define IP_ADDR2   4
	#define IP_ADDR3   252
   
	/*Gateway Address*/
	#define GW_ADDR0   10
	#define GW_ADDR1   84
	#define GW_ADDR2   4
	#define GW_ADDR3   1  

#endif

/*NETMASK*/
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0


/* Ethernet Interface Name */
#define IFNAME0 'e'
#define IFNAME1 'i'
#define PRODUCT_IF_NAME	"ei0"

#define	ENC28J60_SPI			SPI1
#if !defined(ENC28J60_USE_DMA)
	#define ENC28J60_USE_DMA	(0)
#endif
#define ENC28J60_MAX_PACKET		(1500)
#define ENC28J60_DMA_TX_CHAN	DMA1_Channel3
#define ENC28J60_DMA_RX_CNAN	DMA1_Channel2

#define ENC28J60_DMA_TX_CSELR 	DMA1_CSELR
#define ENC28J60_DMA_RX_CSELR 	DMA1_CSELR

#define ENC28J60_DMA_TX_CSEL	(0x01 << DMA_CSELR_C3S_Pos)
#define ENC28J60_DMA_RX_CSEL	(0x01 << DMA_CSELR_C2S_Pos)

#define SRAM_SPI				SPI2
#define SRAM_USE_DMA			(0)

#ifndef RCC_CFGR_PLLMULL17
	#define RCC_CFGR_PLLMULL17 ((uint32_t)0x003C0000)
#endif

#ifndef USART_RX_QUEUE_SZ
	#define USART_RX_QUEUE_SZ	32
#endif

#ifndef USART_TX_QUEUE_SZ
	#define	USART_TX_QUEUE_SZ	16
#endif

#define rs485_rx()				caribou_gpio_reset(&gpio_rs485_dir);	/* RS485 Receive Direction */
#define rs485_tx()				caribou_gpio_set(&gpio_rs485_dir);		/* RS485 Transmit Direction */

/**
 * Ethernet Interrupt Line (PB4) 
 */
#define ETH_IRQn				EXTI4_IRQn								/* ENC28J60 Interrupt pin IRQ vector */
#define	ETH_LINE				EXTI_Line4								/* ENC28J60 Interript pin */

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