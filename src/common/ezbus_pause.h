/*****************************************************************************
* Copyright © 2019-2020 Mike Sharkey <mike@8bitgeek.net>                     *
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
#ifndef EZBUS_PAUSE_H_
#define EZBUS_PAUSE_H_

#include <ezbus_types.h>

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push)
#pragma pack(1)

typedef union
{
	ezbus_ms_tick_t		duration;
    bool                active;
    uint8_t             function;
} __attribute__((packed)) ezbus_pause_t ;

#pragma pack(pop)

extern void 	        ezbus_pause_init  	        ( ezbus_pause_t* pause );

extern ezbus_ms_tick_t  ezbus_pause_get_duration    ( ezbus_pause_t* pause );
extern void 	        ezbus_pause_set_duration    ( ezbus_pause_t* pause, ezbus_ms_tick_t duration );

extern bool             ezbus_pause_get_active      ( ezbus_pause_t* pause );
extern void 	        ezbus_pause_set_active      ( ezbus_pause_t* pause, bool active );

extern void             ezbus_pause_copy            ( ezbus_pause_t* dst, ezbus_pause_t* src );

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_PAUSE_H_ */
