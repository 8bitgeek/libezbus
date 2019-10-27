/******************************************************************************
 * Copyright © 2018 by @author Mike Sharkey <mike@8bitgeek.net>
 * All Rights Reserved
 *****************************************************************************/
#ifndef EZBUS_CRC_H_
#define EZBUS_CRC_H_

#include "ezbus_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

extern uint16_t ezbus_crc       ( void* p, size_t size );
extern uint16_t ezbus_crc_update( uint16_t crc, uint8_t c );
extern void     ezbus_crc_dump  ( uint16_t crc, const char* prefix );

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_CRC_H_ */
