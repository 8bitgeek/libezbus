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
#ifndef EZBUS_PEER_H_
#define EZBUS_PEER_H_

#include <ezbus_platform.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    ezbus_address_t     address;
    uint8_t             seq;
} ezbus_peer_t;

extern void             ezbus_peer_init         ( ezbus_peer_t* peer, ezbus_address_t* address, uint8_t seq );
extern ezbus_address_t* ezbus_peer_get_address  ( const ezbus_peer_t* peer );
extern uint8_t          ezbus_peer_get_seq      ( const ezbus_peer_t* peer );
extern uint8_t          ezbus_peer_set_seq      ( const ezbus_peer_t* peer, uint8_t seq );

extern int              ezbus_peer_compare      ( const ezbus_peer_t* a, const ezbus_peer_t* b );
extern uint8_t*         ezbus_peer_copy         ( ezbus_peer_t* dst, const ezbus_peer_t* src );
extern void             ezbus_peer_swap         ( ezbus_peer_t* dst, ezbus_peer_t* src );
extern char*            ezbus_peer_string       ( ezbus_peer_t* peer );

extern void             ezbus_peer_dump         ( const ezbus_peer_t* peer, const char* prefix );


#ifdef __cplusplus
}
#endif

#endif /* EZBUS_PEER_H_ */
