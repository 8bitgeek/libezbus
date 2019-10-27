# ezbus (In Early Development)
* EzBus is a simple token-bus protocol for the RS-485 bus written in C. 
* EzBus is intended to be lightweight and simple for small memory footprint embedded systems.
* The reference implimentation is using CARIBOU-RTOS on a pair of STM32 Nucleo Board with a MAX3485 3.3V RS-485 chip wired to USART2 using pins PA2(TX), PA3(RX), PA10(DIRECTION).
* See ezbus_platform.c/.h and board.c/.h for how to port to other O/S or H/W platforms.

