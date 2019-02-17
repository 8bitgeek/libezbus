/******************************************************************************
 * Copyright © 2018 by @author Mike Sharkey <mike@8bitgeek.net>
 * All Rights Reserved
 *****************************************************************************/
#ifndef __EZBUS_HEX_H__
#define __EZBUS_HEX_H__

#include "ezbus_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void ezbus_hex4  	( uint8_t  nybble, char* hex );
extern void ezbus_hex8  	( uint8_t  byte,   char* hex );
extern void ezbus_hex16 	( uint16_t word,   char* hex );
extern void ezbus_hex32 	( uint32_t word,   char* hex );
extern void ezbus_hex_dump 	(char* tag, uint8_t* data, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif



