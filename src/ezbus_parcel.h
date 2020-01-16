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
#ifndef EZBUS_PARCEL_H_
#define EZBUS_PARCEL_H_

#include <ezbus_platform.h>

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push)
#pragma pack(1)

typedef struct
{
	uint8_t 			size;
	uint8_t				bytes[EZBUS_PARCEL_DATA_LN];
} ezbus_parcel_t;

#pragma pack(pop)

extern void 	ezbus_parcel_init  	    ( ezbus_parcel_t* parcel );
extern uint16_t ezbus_parcel_get_max 	( ezbus_parcel_t* parcel );

extern uint16_t ezbus_parcel_get_size 	( ezbus_parcel_t* parcel );
extern void*    ezbus_parcel_get_ptr    ( ezbus_parcel_t* parcel );
extern uint16_t	ezbus_parcel_get_data  	( ezbus_parcel_t* parcel, void* data );
extern uint16_t ezbus_parcel_get_string	( ezbus_parcel_t* parcel, char* string );

extern uint16_t ezbus_parcel_set_size 	( ezbus_parcel_t* parcel, uint16_t size );
extern uint16_t	ezbus_parcel_set_data  	( ezbus_parcel_t* parcel, const void* data, uint16_t size );
extern uint16_t	ezbus_parcel_set_string	( ezbus_parcel_t* parcel, const char* string );

extern void     ezbus_parcel_copy       ( ezbus_parcel_t* dst, ezbus_parcel_t* src );

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_PARCEL_H_ */
