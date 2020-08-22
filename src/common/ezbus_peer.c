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
#include <ezbus_platform.h>
#include <ezbus_address.h>
#include <ezbus_peer.h>
#include <ezbus_hex.h>

extern void ezbus_peer_init( ezbus_peer_t* peer, ezbus_address_t* address, uint8_t seq )
{
    ezbus_address_copy( &peer->address, address );
    peer->seq = seq;
}

extern ezbus_address_t* ezbus_peer_get_address( const ezbus_peer_t* peer )
{
    if ( peer != NULL )
    {
        ezbus_address_t* address_p = (ezbus_address_t*)&peer->address;
        return address_p;
    }
    return NULL;
}

extern uint8_t ezbus_peer_get_seq( const ezbus_peer_t* peer )
{
    if ( peer != NULL )
    {
        return peer->seq;
    }
    return 0;
}

extern uint8_t ezbus_peer_set_seq( const ezbus_peer_t* peer, uint8_t seq )
{
    if ( peer != NULL )
    {
        ezbus_peer_t* peer_p = (ezbus_peer_t*)peer;
        return ( peer_p->seq = seq );
    }
    return 0;
}


/**
 * @brief Compare address a vs b
 * @return <, =, or > 0
 */
extern int ezbus_peer_compare( const ezbus_peer_t* a, const ezbus_peer_t* b )
{
    return ezbus_platform_memcmp(a,b,sizeof(ezbus_peer_t));
}

extern uint8_t* ezbus_peer_copy( ezbus_peer_t* dst, const ezbus_peer_t* src )
{
    return ezbus_platform_memcpy(dst,src,sizeof(ezbus_peer_t));
}

extern void ezbus_peer_swap( ezbus_peer_t* dst, ezbus_peer_t* src )
{
    ezbus_peer_t tmp;
    ezbus_peer_copy(&tmp,dst);
    ezbus_peer_copy(dst,src);
    ezbus_peer_copy(src,&tmp);
}

extern char* ezbus_peer_string( ezbus_peer_t* peer )
{
    return ezbus_address_string( &peer->address );
}

extern void ezbus_peer_dump( const ezbus_peer_t* peer, const char* prefix )
{
    fprintf(stderr, "%s=", prefix );
    for(int n=0; n < EZBUS_ADDR_LN; n++)
    {
        fprintf(stderr, "%02X", peer->address.byte[n] );
    }
    fprintf(stderr, ":%02X\n", peer->seq );
}
