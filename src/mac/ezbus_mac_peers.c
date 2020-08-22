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
#include <ezbus_mac_peers.h>
#include <ezbus_hex.h>
#include <ezbus_crc.h>
#include <ezbus_log.h>
#include <ezbus_socket_callback.h>

static void         ezbus_mac_peers_insort_self( ezbus_mac_t* mac );
static EZBUS_ERR    ezbus_mac_peers_append  ( ezbus_mac_t* mac, const ezbus_peer_t* peer );
static EZBUS_ERR    ezbus_mac_peers_insert  ( ezbus_mac_t* mac, const ezbus_peer_t* peer, int index );


extern void ezbus_mac_peers_init(ezbus_mac_t* mac)
{
    ezbus_mac_peers_t* peers = ezbus_mac_get_peers(mac);
    ezbus_platform_memset(peers,0,sizeof(ezbus_mac_peers_t));
    ezbus_mac_peers_insort_self( mac );
}

extern void ezbus_mac_peers_deinit(ezbus_mac_t* mac)
{
    while ( ezbus_mac_peers_count( mac ) > 0 )
        ezbus_mac_peers_take( mac, 0 );
}

extern void ezbus_mac_peers_run( ezbus_mac_t* mac )
{
    /* FIXME insert code here */
}

extern void  ezbus_mac_peers_clear( ezbus_mac_t* mac )
{
    int peer_count = ezbus_mac_peers_count( mac );
    EZBUS_LOG( EZBUS_LOG_PEERS, "count %d", peer_count );
    for( int n=0; n < peer_count; n++ )
    {
        ezbus_peer_t* peer = ezbus_mac_peers_at( mac, 0 );
        ezbus_address_t* peer_address = ezbus_peer_get_address( peer );
        if ( (ezbus_address_compare( peer_address, &ezbus_self_address ) != 0) && !ezbus_socket_callback_peer_active( mac, peer_address ) )
        {
            ezbus_mac_peers_take( mac, 0 );
        }
    }
    ezbus_mac_peers_insort_self( mac );
}

static void ezbus_mac_peers_insort_self( ezbus_mac_t* mac )
{
    ezbus_peer_t self_peer;
    ezbus_peer_init( &self_peer, &ezbus_self_address, 0 );
    ezbus_mac_peers_insort( mac, &self_peer );
}

extern EZBUS_ERR ezbus_mac_peers_insort( ezbus_mac_t* mac, const ezbus_peer_t* peer )
{
    EZBUS_ERR err = EZBUS_ERR_OKAY;

    if ( !ezbus_mac_peers_full( mac) )
    {
        if ( !ezbus_address_is_broadcast( ezbus_peer_get_address( peer ) ) )
        {
            if ( ezbus_mac_peers_index_of( mac, ezbus_peer_get_address( peer ) ) < 0 )
            {
                for( int index=0; index < ezbus_mac_peers_count( mac ); index++ )
                {
                    ezbus_address_t* other_address = ezbus_peer_get_address( ezbus_mac_peers_at( mac, index ) );
                    ezbus_address_t* peer_address = ezbus_peer_get_address( peer );

                    if ( ezbus_address_compare( peer_address, other_address ) < 0 )
                    {
                        err = ezbus_mac_peers_insert( mac, peer, index );
                        return err;
                    }
                }
                err = ezbus_mac_peers_append( mac, peer );
            }
            else
            {
                err = EZBUS_ERR_DUP;
            }
        }
        else
        {
            err = EZBUS_ERR_PARAM;
        }
    }
    else
    {
        err = EZBUS_ERR_LIMIT;
    }

    return err;
}


extern ezbus_peer_t* ezbus_mac_peers_at( ezbus_mac_t* mac, int index )
{
    ezbus_mac_peers_t* peers = ezbus_mac_get_peers( mac );
    ezbus_peer_t* peer = NULL;
    if ( index >= 0 && index < ezbus_mac_peers_count(mac) )
    {
        peer = &peers->list[index];
    }
    return peer;
}

extern int ezbus_mac_peers_count( ezbus_mac_t* mac )
{
    ezbus_mac_peers_t* peers = ezbus_mac_get_peers( mac );
    return (peers->count);
}

extern int ezbus_mac_peers_empty( ezbus_mac_t* mac )
{
    ezbus_mac_peers_t* peers = ezbus_mac_get_peers( mac );
    return (peers->count==0);
}

extern int  ezbus_mac_peers_full( ezbus_mac_t* mac )
{
    ezbus_mac_peers_t* peers = ezbus_mac_get_peers( mac );
    return (peers->count==EZBUS_MAX_PEERS);
}

extern int ezbus_mac_peers_index_of( ezbus_mac_t* mac, const ezbus_address_t* address )
{
    for(int index=0; index < ezbus_mac_peers_count( mac ); index++)
    {
        if ( ezbus_address_compare( ezbus_peer_get_address( ezbus_mac_peers_at(mac,index) ), address ) == 0 )
        {
            return index;
        }
    }
    return -1;

}

extern ezbus_peer_t* ezbus_mac_peers_lookup( ezbus_mac_t* mac, const ezbus_address_t* address )
{
    int index = ezbus_mac_peers_index_of( mac, address );
    if ( index >= 0 )
    {
        return ezbus_mac_peers_at(mac,index);
    }
    return NULL;
}

extern ezbus_address_t* ezbus_mac_peers_next( ezbus_mac_t* mac, const ezbus_address_t* address )
{
    if ( ezbus_mac_peers_count( mac ) > 0 )
    {
        for( int index=ezbus_mac_peers_index_of( mac, address ); index >= 0 && index < ezbus_mac_peers_count( mac ); index++ )
        {
            ezbus_peer_t* other = ezbus_mac_peers_at(mac,index);
            if ( ezbus_address_compare( address, &other->address ) < 0 )
            {
                return ezbus_peer_get_address( other );
            }
        }
        return ezbus_peer_get_address( ezbus_mac_peers_at(mac,0) );
    }
    return (ezbus_address_t*)address;
}

extern void ezbus_mac_peers_dump( ezbus_mac_t* mac, const char* prefix )
{
    ezbus_mac_peers_t* peers = ezbus_mac_get_peers( mac );
    char print_buffer[EZBUS_TMP_BUF_SZ];
    fprintf(stderr, "%s.count=%d", prefix, peers->count );
    for(int index=0; index < ezbus_mac_peers_count(mac); index++)
    {
        ezbus_peer_t* peer = ezbus_mac_peers_at(mac,index);
        sprintf(print_buffer,"%s[%d]", prefix, index );
        ezbus_peer_dump( peer, print_buffer );
    }
    fflush(stderr);
}



static EZBUS_ERR ezbus_mac_peers_append( ezbus_mac_t* mac, const ezbus_peer_t* peer )
{
    EZBUS_ERR err = EZBUS_ERR_LIMIT;
    if ( !ezbus_mac_peers_full( mac) )
    {
        if ( ezbus_mac_peers_index_of( mac, ezbus_peer_get_address( peer ) ) < 0 )
        {
            ezbus_mac_peers_t* peers = ezbus_mac_get_peers( mac );
            ezbus_peer_copy( ezbus_mac_peers_at(mac,peers->count++), peer );

            if ( ezbus_address_compare( &ezbus_self_address, ezbus_peer_get_address( peer ) ) != 0 )
            {
                ezbus_socket_callback_peer( mac, ezbus_peer_get_address( peer ), true );
            }
        }
        err = EZBUS_ERR_OKAY;
    }
    return err;
}

static EZBUS_ERR ezbus_mac_peers_insert( ezbus_mac_t* mac, const ezbus_peer_t* peer, int index )
{
    EZBUS_ERR err = EZBUS_ERR_LIMIT;
    if ( !ezbus_mac_peers_full( mac) )
    {
        if ( index >= ezbus_mac_peers_count( mac ) )
        {
            err = ezbus_mac_peers_append( mac, peer );
        }
        else
        {
            ezbus_mac_peers_t* peers = ezbus_mac_get_peers( mac );
            int peers_to_move = ( ezbus_mac_peers_count( mac ) - index );
            int bytes_to_move = ( peers_to_move * sizeof(ezbus_peer_t) );
            ezbus_peer_t* move_dst = &peers->list[index+1];
            ezbus_peer_t* dst = &peers->list[index];

            ezbus_platform_memmove( move_dst, dst, bytes_to_move );
            ezbus_peer_copy( dst, peer );

            ++peers->count;

            if ( ezbus_address_compare( &ezbus_self_address, ezbus_peer_get_address( peer ) ) != 0 )
            {
                ezbus_socket_callback_peer( mac, ezbus_peer_get_address( peer ), true );
            }
        }
        err = EZBUS_ERR_OKAY;
    }    
    return err;
}

extern EZBUS_ERR ezbus_mac_peers_take( ezbus_mac_t* mac, int index )
{
    ezbus_mac_peers_t* peers = ezbus_mac_get_peers( mac );
    EZBUS_ERR err=EZBUS_ERR_LIMIT;

    if ( ezbus_mac_peers_count(mac) > 0 && index < ezbus_mac_peers_count(mac) )
    {
        ezbus_peer_t peer;
        int bytes_to_move;

        ezbus_peer_copy( &peer, ezbus_mac_peers_at(mac,index) );
        
        bytes_to_move = (sizeof(ezbus_peer_t)*(ezbus_mac_peers_count(mac)-index));
        ezbus_platform_memmove( &peers->list[ index ], &peers->list[ index+1 ], bytes_to_move );
        --peers->count;
        
        if ( ezbus_address_compare( &ezbus_self_address, ezbus_peer_get_address( &peer ) ) != 0 )
        {
            ezbus_socket_callback_peer( mac, ezbus_peer_get_address( &peer ), false );
        }
    }

    return err;
}

extern void ezbus_mac_peers_clean( ezbus_mac_t* mac, uint8_t seq )
{
    for(int index=0; index < ezbus_mac_peers_count(mac); index++)
    {
        ezbus_peer_t* peer = ezbus_mac_peers_at(mac,index);
        if ( ezbus_peer_get_seq( peer ) != seq )
        {
            if ( ezbus_address_is_self(ezbus_peer_get_address( peer )) )
            {
                ezbus_peer_set_seq( peer, seq );
            }
            else
            {
                ezbus_mac_peers_take( mac, index-- );
            }
        }
    }
}

extern bool ezbus_mac_peers_am_dominant( ezbus_mac_t* mac )
{
    if ( !ezbus_mac_peers_empty( mac ) )
    {
        ezbus_peer_t* peer = ezbus_mac_peers_at(mac,0);
        return ezbus_address_is_self(ezbus_peer_get_address( peer ));
    }
    return false;
}

extern void ezbus_mac_peers_crc( ezbus_mac_t* mac, ezbus_crc_t* crc )
{
    ezbus_crc_init( crc );
    for(int index=0; index < ezbus_mac_peers_count(mac); index++)
    {
        ezbus_peer_t* peer = ezbus_mac_peers_at(mac,index);
        ezbus_crc( crc, ezbus_peer_get_address( peer ), sizeof(ezbus_address_t) );
    }
}

extern void ezbus_mac_peers_log( ezbus_mac_t* mac )
{
    for(int index=0; index < ezbus_mac_peers_count(mac); index++)
    {
        ezbus_peer_t* peer = ezbus_mac_peers_at(mac,index);
        EZBUS_LOG( EZBUS_LOG_PEERS, "%s:%3d, ", ezbus_address_string( ezbus_peer_get_address( peer ) ), ezbus_peer_get_seq( peer ) );
    }
    EZBUS_LOG( EZBUS_LOG_PEERS, "" );
}
