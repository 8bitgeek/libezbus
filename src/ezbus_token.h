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
#ifndef EZBUS_TOKEN_H_
#define EZBUS_TOKEN_H_

#include <ezbus_platform.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
    ezbus_ms_tick_t         touched;
    ezbus_ms_tick_t         timeout_period;
    bool                    present;
} ezbus_token_t;


extern void             ezbus_token_init                ( ezbus_token_t* token );

extern void             ezbus_token_set_present         ( ezbus_token_t* token, bool present );
extern bool             ezbus_token_present             ( ezbus_token_t* token );

extern void             ezbus_token_touch               ( ezbus_token_t* token );
extern ezbus_ms_tick_t  ezbus_token_touched             ( ezbus_token_t* token );

extern ezbus_ms_tick_t  ezbus_token_timeout_period      ( ezbus_token_t* token );

extern bool             ezbus_token_timeout             ( ezbus_token_t* token );

extern uint32_t         ezbus_token_calc_timeout_period ( uint32_t packet_sz, uint32_t num_peers, uint32_t baud_rate );

#ifdef __cplusplus
}
#endif


#endif /* EZBUS_TOKEN_H_ */
