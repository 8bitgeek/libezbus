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
#include <ezbus_peer_list.h>
#include <ezbus_hex.h>
#include <ezbus_crc.h>
#include <ezbus_log.h>

static EZBUS_ERR    ezbus_peer_list_append  ( ezbus_peer_list_t* peer_list, const ezbus_peer_t* peer );
static EZBUS_ERR    ezbus_peer_list_insert  ( ezbus_peer_list_t* peer_list, const ezbus_peer_t* peer, int index );


extern void ezbus_peer_list_init(ezbus_peer_list_t* peer_list)
{
    if ( peer_list )
    {
        ezbus_platform_memset(peer_list,0,sizeof(ezbus_peer_list_t));
    }
}

extern void ezbus_peer_list_deinit( ezbus_peer_list_t* peer_list)
{
    //printf( "deinit\n" );
    peer_list->count=0;
}


extern EZBUS_ERR ezbus_peer_list_insort( ezbus_peer_list_t* peer_list, const ezbus_peer_t* peer )
{
    EZBUS_ERR err = EZBUS_ERR_OKAY;
    
    if ( !ezbus_peer_list_full( peer_list) )
    {
        if ( ezbus_address_compare( ezbus_peer_get_address( peer ), (ezbus_address_t*)&ezbus_warmboot_address ) != 0 &&
             ezbus_address_compare( ezbus_peer_get_address( peer ), (ezbus_address_t*)&ezbus_broadcast_address ) != 0)
        {
            if ( ezbus_peer_list_index_of( peer_list, ezbus_peer_get_address( peer ) ) < 0 )
            {
                for( int index=0; index < ezbus_peer_list_count( peer_list ); index++ )
                {
                    ezbus_address_t* other_address = ezbus_peer_get_address( ezbus_peer_list_at( peer_list, index ) );
                    ezbus_address_t* peer_address = ezbus_peer_get_address( peer );

                    if ( ezbus_address_compare( peer_address, other_address ) < 0 )
                    {
                        err = ezbus_peer_list_insert( peer_list, peer, index );
                        return err;
                    }
                }
                err = ezbus_peer_list_append( peer_list, peer );
            }
        }
    }
    else
    {
        err = EZBUS_ERR_LIMIT;
    }

    return err;
}


extern EZBUS_ERR ezbus_peer_list_take( ezbus_peer_list_t* peer_list, int index )
{
    EZBUS_ERR err=EZBUS_ERR_LIMIT;

    if ( ezbus_peer_list_count(peer_list) > 0 && index < ezbus_peer_list_count(peer_list) )
    {
        int bytes_to_move = (sizeof(ezbus_peer_t)*(ezbus_peer_list_count( peer_list )-index));
        ezbus_platform_memmove( &peer_list->list[ index ], &peer_list->list[ index+1 ], bytes_to_move );
        --peer_list->count;
    }

    return err;
}

extern ezbus_peer_t* ezbus_peer_list_at( ezbus_peer_list_t* peer_list, int index )
{
    ezbus_peer_t* peer = NULL;
    if ( index >= 0 && index < ezbus_peer_list_count(peer_list) )
    {
        peer = &peer_list->list[index];
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

extern int  ezbus_peer_list_full( ezbus_peer_list_t* peer_list )
{
    return (peer_list->count==EZBUS_MAX_PEERS);
}

extern int ezbus_peer_list_index_of( ezbus_peer_list_t* peer_list, const ezbus_address_t* address )
{
    for(int index=0; index < ezbus_peer_list_count( peer_list ); index++)
    {
        if ( ezbus_address_compare( ezbus_peer_get_address( ezbus_peer_list_at(peer_list,index) ), address ) == 0 )
        {
            return index;
        }
    }
    return -1;

}

extern ezbus_peer_t* ezbus_peer_list_lookup( ezbus_peer_list_t* peer_list, const ezbus_address_t* address )
{
    int index = ezbus_peer_list_index_of( peer_list, address );
    if ( index >= 0 )
    {
        return ezbus_peer_list_at(peer_list,index);
    }
    return NULL;
}

extern ezbus_address_t* ezbus_peer_list_next( ezbus_peer_list_t* peer_list, const ezbus_address_t* address )
{
    if ( ezbus_peer_list_count( peer_list ) > 0 )
    {
        for( int index=ezbus_peer_list_index_of( peer_list, address ); index >= 0 && index < ezbus_peer_list_count( peer_list ); index++ )
        {
            ezbus_peer_t* other = ezbus_peer_list_at(peer_list,index);
            if ( ezbus_address_compare( address, &other->address ) < 0 )
            {
                return ezbus_peer_get_address( other );
            }
        }
        return ezbus_peer_get_address( ezbus_peer_list_at(peer_list,0) );
    }
    return (ezbus_address_t*)address;
}

extern void ezbus_peer_list_dump( ezbus_peer_list_t* peer_list, const char* prefix )
{
    char print_buffer[EZBUS_TMP_BUF_SZ];
    fprintf(stderr, "%s.count=%d\n", prefix, peer_list->count );
    for(int index=0; index < ezbus_peer_list_count(peer_list); index++)
    {
        ezbus_peer_t* peer = ezbus_peer_list_at(peer_list,index);
        sprintf(print_buffer,"%s[%d]", prefix, index );
        ezbus_peer_dump( peer, print_buffer );
    }
    fflush(stderr);
}



static EZBUS_ERR ezbus_peer_list_append( ezbus_peer_list_t* peer_list, const ezbus_peer_t* peer )
{
    EZBUS_ERR err = EZBUS_ERR_LIMIT;
    if ( !ezbus_peer_list_full( peer_list) )
    {
        if ( ezbus_peer_list_index_of( peer_list, ezbus_peer_get_address( peer ) ) < 0 )
        {
            ezbus_peer_copy( ezbus_peer_list_at(peer_list,peer_list->count++), peer );
        }
        err = EZBUS_ERR_OKAY;
    }
    return err;
}

static EZBUS_ERR ezbus_peer_list_insert( ezbus_peer_list_t* peer_list, const ezbus_peer_t* peer, int index )
{
    EZBUS_ERR err = EZBUS_ERR_LIMIT;
    if ( !ezbus_peer_list_full( peer_list) )
    {
        if ( index >= ezbus_peer_list_count( peer_list ) )
        {
            err = ezbus_peer_list_append( peer_list, peer );
        }
        else
        {
            int bytes_to_move = (sizeof(ezbus_peer_t)*(ezbus_peer_list_count( peer_list )-index))+1;
            ezbus_platform_memmove( &peer_list->list[ index+1 ], &peer_list->list[ index ], bytes_to_move );
            ezbus_peer_copy( ezbus_peer_list_at(peer_list,index), peer );
            ++peer_list->count;
        }
        err = EZBUS_ERR_OKAY;
    }    
    return err;
}

extern void ezbus_peer_list_clean( ezbus_peer_list_t* peer_list, uint8_t seq )
{
    for(int index=0; index < ezbus_peer_list_count(peer_list); index++)
    {
        ezbus_peer_t* peer = ezbus_peer_list_at(peer_list,index);
        if ( ezbus_peer_get_seq( peer ) != seq )
        {
            if ( ezbus_address_compare( ezbus_peer_get_address( peer ), &ezbus_self_address ) == 0 )
            {
                ezbus_peer_set_seq( peer, seq );
            }
            else
            {
                ezbus_peer_list_take( peer_list, index-- );
            }
        }
    }
}

extern bool ezbus_peer_list_am_dominant( ezbus_peer_list_t* peer_list )
{
    if ( !ezbus_peer_list_empty( peer_list ) )
    {
        ezbus_peer_t* peer = ezbus_peer_list_at(peer_list,0);
        return ( ezbus_address_compare( ezbus_peer_get_address( peer ), &ezbus_self_address ) == 0 );
    }
    return false;
}

extern void ezbus_peer_list_crc( ezbus_peer_list_t* peer_list, ezbus_crc_t* crc )
{
    ezbus_crc_init( crc );
    for(int index=0; index < ezbus_peer_list_count(peer_list); index++)
    {
        ezbus_peer_t* peer = ezbus_peer_list_at(peer_list,index);
        ezbus_crc( crc, ezbus_peer_get_address( peer ), sizeof(ezbus_address_t) );
    }
}

extern void ezbus_peer_list_log( ezbus_peer_list_t* peer_list )
{
    for(int index=0; index < ezbus_peer_list_count(peer_list); index++)
    {
        ezbus_peer_t* peer = ezbus_peer_list_at(peer_list,index);
        ezbus_log( EZBUS_LOG_PEERS, "%s:%3d, ", ezbus_address_string( ezbus_peer_get_address( peer ) ), ezbus_peer_get_seq( peer ) );
    }
    ezbus_log( 1, "\n" );
}
