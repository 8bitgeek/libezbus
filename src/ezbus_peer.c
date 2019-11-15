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

extern char* ezbus_peer_string( ezbus_peer_t* peer, char* string )
{
    ezbus_address_string( &peer->address, string );
    return string;
}

extern void ezbus_peer_list_init(ezbus_peer_list_t* peer_list)
{
    if ( peer_list )
    {
        ezbus_platform_memset(peer_list,0,sizeof(ezbus_peer_list_t));
    }
}

extern void ezbus_peer_list_deinit( ezbus_peer_list_t* peer_list)
{
    if ( peer_list )
    {
        ezbus_peer_t peer;
        while(ezbus_peer_list_count(peer_list)>0)
            ezbus_peer_list_take(peer_list,&peer);
        ezbus_platform_memset(peer_list,0,sizeof(ezbus_peer_list_t));
    }
}

extern ezbus_peer_t* ezbus_peer_list_append( ezbus_peer_list_t* peer_list, const ezbus_peer_t* peer )
{
    ezbus_peer_t* peer_p = NULL;
    if ( peer_list != NULL )
    {
        if ( ezbus_peer_list_lookup( peer_list, ezbus_peer_get_address(peer) ) == NULL )
        {
            peer_list->list = (ezbus_peer_t**)ezbus_platform_realloc( peer_list->list, sizeof(ezbus_peer_t**)*(peer_list->count+1) );
            if ( peer_list->list != NULL )
            {
                peer_list->list[ peer_list->count ] = ezbus_platform_malloc( sizeof(ezbus_peer_t) );
                if ( peer_list->list[ peer_list->count ] != NULL )
                {
                    /* Take a copy of the packet and increment the count */
                    ezbus_platform_memcpy( peer_list->list[ peer_list->count ], peer, sizeof(ezbus_peer_t) );
                    peer_p = peer_list->list[ peer_list->count++ ];
                }
                else
                {
                    /* Remove the list allocation */
                    peer_list->list = (ezbus_peer_t**)ezbus_platform_realloc( peer_list->list, sizeof(ezbus_peer_t**)*(peer_list->count) );
                }
            }
            else
            {
                peer_list->count = 0;
            }
        }
    }
    return peer_p;
}

extern EZBUS_ERR ezbus_peer_list_take( ezbus_peer_list_t* peer_list, ezbus_peer_t* peer )
{
    EZBUS_ERR err=EZBUS_ERR_OKAY;
    if ( peer_list != NULL )
    {
        if ( !ezbus_peer_list_empty(peer_list) )
        {
            ezbus_peer_t** tpeer_list = peer_list->list;
            ezbus_peer_t* tpeer = tpeer_list[--peer_list->count];
            ezbus_peer_copy(peer,tpeer);
            ezbus_platform_free(tpeer);
            peer_list->list = (ezbus_peer_t**)ezbus_platform_realloc(peer_list->list,sizeof(ezbus_peer_t**)*(peer_list->count));
            if ( peer_list->list != NULL )
            {
                err=EZBUS_ERR_OKAY;
            }
            else
            {
                /* realloc() failed, let's don't leak */
                peer_list->list = tpeer_list;
                err = EZBUS_ERR_MALLOC;
            }
        }
        else
        {
            err = EZBUS_ERR_LIMIT;
        }
    }
    else
    {
        err = EZBUS_ERR_PARAM;
    }
    return err;
}

extern ezbus_peer_t* ezbus_peer_list_at( ezbus_peer_list_t* peer_list, int index )
{
    ezbus_peer_t* peer = NULL;
    if ( index >= 0 && index < ezbus_peer_list_count(peer_list) )
    {
        peer = peer_list->list[index];
    }
    return peer;
}

extern int ezbus_peer_list_count( ezbus_peer_list_t* peer_list )
{
    return (peer_list->count);
}

extern int ezbus_peer_list_empty( ezbus_peer_list_t* peer_list )
{
    return (peer_list->count==0);
}

extern ezbus_peer_t* ezbus_peer_list_lookup( ezbus_peer_list_t* peer_list, const ezbus_address_t* address )
{
    for(int index=0; index < peer_list->count; index++)
    {
        if ( ezbus_address_compare( ezbus_peer_get_address(peer_list->list[index]), address ) == 0 )
        {
            return peer_list->list[index];
        }
    }
    return NULL;
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

extern void ezbus_peer_list_dump( ezbus_peer_list_t* peer_list, const char* prefix )
{
    char print_buffer[EZBUS_TMP_BUF_SZ];
    fprintf(stderr, "%s.count=%d\n", prefix, peer_list->count );
    for(int index=0; index < peer_list->count; index++)
    {
        ezbus_peer_t* peer = peer_list->list[index];
        sprintf(print_buffer,"%s[%d]", prefix, index );
        ezbus_peer_dump( peer, print_buffer );
    }
    fflush(stderr);
}