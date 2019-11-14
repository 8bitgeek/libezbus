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
#ifndef EZBUS_CRC_H_
#define EZBUS_CRC_H_

#include <ezbus_platform.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef union
{
    uint16_t        word;                       /* Header CRC Word */
    uint8_t         bytes[sizeof(uint16_t)];    /* Header CRC Bytes */
} ezbus_crc_t;


extern ezbus_crc_t* ezbus_crc         ( ezbus_crc_t* crc, void* p, size_t size );
extern ezbus_crc_t* ezbus_crc_update  ( ezbus_crc_t* crc, uint8_t c );
extern uint16_t     ezbus_crc_word    ( ezbus_crc_t* crc );
extern uint8_t*     ezbus_crc_bytes   ( ezbus_crc_t* crc );
extern bool         ezbus_crc_equal   ( ezbus_crc_t* crc1, ezbus_crc_t* crc2 );
extern ezbus_crc_t* ezbus_crc_flip    ( ezbus_crc_t* crc );
extern void         ezbus_crc_dump    ( ezbus_crc_t* crc, const char* prefix );

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_CRC_H_ */
