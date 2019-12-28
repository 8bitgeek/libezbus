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
#include <ezbus_mac_peers.h>
#include <ezbus_hex.h>
#include <ezbus_crc.h>
#include <ezbus_log.h>

static EZBUS_ERR    ezbus_mac_peers_append  ( ezbus_mac_peers_t* peers, const ezbus_peer_t* peer );
static EZBUS_ERR    ezbus_mac_peers_insert  ( ezbus_mac_peers_t* peers, const ezbus_peer_t* peer, int index );


extern void ezbus_mac_peers_init(ezbus_mac_t* mac)
{
    ezbus_mac_peers_t* peers = ezbus_mac_get_peers(mac);
    ezbus_platform_memset(peers,0,sizeof(ezbus_mac_peers_t));
}

extern void ezbus_mac_peers_deinit(ezbus_mac_t* mac)
{
    ezbus_mac_peers_t* peers = ezbus_mac_get_peers(mac);
    peers->count=0;
}

extern void ezbus_mac_peers_run( ezbus_mac_t* mac )
{
    /* FIXME insert code here */
}

extern EZBUS_ERR ezbus_mac_peers_insort( ezbus_mac_t* mac, const ezbus_peer_t* peer )
{
    ezbus_mac_peers_t* peers = ezbus_mac_get_peers( mac );
    EZBUS_ERR err = EZBUS_ERR_OKAY;

    if ( !ezbus_mac_peers_full( mac) )
    {
        if ( ezbus_address_compare( ezbus_peer_get_address( peer ), (ezbus_address_t*)&ezbus_warmboot_address ) != 0 &&
             ezbus_address_compare( ezbus_peer_get_address( peer ), (ezbus_address_t*)&ezbus_broadcast_address ) != 0)
        {
            if ( ezbus_mac_peers_index_of( mac, ezbus_peer_get_address( peer ) ) < 0 )
            {
                for( int index=0; index < ezbus_mac_peers_count( mac ); index++ )
                {
                    ezbus_address_t* other_address = ezbus_peer_get_address( ezbus_mac_peers_at( mac, index ) );
                    ezbus_address_t* peer_address = ezbus_peer_get_address( peer );

                    if ( ezbus_address_compare( peer_address, other_address ) < 0 )
                    {
                        err = ezbus_mac_peers_insert( peers, peer, index );
                        return err;
                    }
                }
                err = ezbus_mac_peers_append( peers, peer );
            }
        }
    }
    else
    {
        err = EZBUS_ERR_LIMIT;
    }

    return err;
}


extern EZBUS_ERR ezbus_mac_peers_take( ezbus_mac_t* mac, int index )
{
    ezbus_mac_peers_t* peers = ezbus_mac_get_peers( mac );
    EZBUS_ERR err=EZBUS_ERR_LIMIT;

    if ( ezbus_mac_peers_count(mac) > 0 && index < ezbus_mac_peers_count(mac) )
    {
        int bytes_to_move = (sizeof(ezbus_peer_t)*(ezbus_mac_peers_count(mac)-index));
        ezbus_platform_memmove( &peers->list[ index ], &peers->list[ index+1 ], bytes_to_move );
        --peers->count;
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
    ezbus_mac_peers_t* peers = ezbus_mac_get_peers( mac );
    for(int index=0; index < ezbus_mac_peers_count( peers ); index++)
    {
        if ( ezbus_address_compare( ezbus_peer_get_address( ezbus_mac_peers_at(peers,index) ), address ) == 0 )
        {
            return index;
        }
    }
    return -1;

}

extern ezbus_peer_t* ezbus_mac_peers_lookup( ezbus_mac_t* mac, const ezbus_address_t* address )
{
    ezbus_mac_peers_t* peers = ezbus_mac_get_peers( mac );
    int index = ezbus_mac_peers_index_of( peers, address );
    if ( index >= 0 )
    {
        return ezbus_mac_peers_at(peers,index);
    }
    return NULL;
}

extern ezbus_address_t* ezbus_mac_peers_next( ezbus_mac_t* mac, const ezbus_address_t* address )
{
    ezbus_mac_peers_t* peers = ezbus_mac_get_peers( mac );
    if ( ezbus_mac_peers_count( peers ) > 0 )
    {
        for( int index=ezbus_mac_peers_index_of( peers, address ); index >= 0 && index < ezbus_mac_peers_count( peers ); index++ )
        {
            ezbus_peer_t* other = ezbus_mac_peers_at(peers,index);
            if ( ezbus_address_compare( address, &other->address ) < 0 )
            {
                return ezbus_peer_get_address( other );
            }
        }
        return ezbus_peer_get_address( ezbus_mac_peers_at(peers,0) );
    }
    return (ezbus_address_t*)address;
}

extern void ezbus_mac_peers_dump( ezbus_mac_t* mac, const char* prefix )
{
    ezbus_mac_peers_t* peers = ezbus_mac_get_peers( mac );
    char print_buffer[EZBUS_TMP_BUF_SZ];
    fprintf(stderr, "%s.count=%d\n", prefix, peers->count );
    for(int index=0; index < ezbus_mac_peers_count(peers); index++)
    {
        ezbus_peer_t* peer = ezbus_mac_peers_at(peers,index);
        sprintf(print_buffer,"%s[%d]", prefix, index );
        ezbus_peer_dump( peer, print_buffer );
    }
    fflush(stderr);
}



static EZBUS_ERR ezbus_mac_peers_append( ezbus_mac_peers_t* peers, const ezbus_peer_t* peer )
{
    EZBUS_ERR err = EZBUS_ERR_LIMIT;
    if ( !ezbus_mac_peers_full( peers) )
    {
        if ( ezbus_mac_peers_index_of( peers, ezbus_peer_get_address( peer ) ) < 0 )
        {
            ezbus_peer_copy( ezbus_mac_peers_at(peers,peers->count++), peer );
        }
        err = EZBUS_ERR_OKAY;
    }
    return err;
}

static EZBUS_ERR ezbus_mac_peers_insert( ezbus_mac_peers_t* peers, const ezbus_peer_t* peer, int index )
{
    EZBUS_ERR err = EZBUS_ERR_LIMIT;
    if ( !ezbus_mac_peers_full( peers) )
    {
        if ( index >= ezbus_mac_peers_count( peers ) )
        {
            err = ezbus_mac_peers_append( peers, peer );
        }
        else
        {
            int bytes_to_move = (sizeof(ezbus_peer_t)*(ezbus_mac_peers_count( peers )-index))+1;
            ezbus_platform_memmove( &peers->list[ index+1 ], &peers->list[ index ], bytes_to_move );
            ezbus_peer_copy( ezbus_mac_peers_at(peers,index), peer );
            ++peers->count;
        }
        err = EZBUS_ERR_OKAY;
    }    
    return err;
}

extern void ezbus_mac_peers_clean( ezbus_mac_peers_t* peers, uint8_t seq )
{
    for(int index=0; index < ezbus_mac_peers_count(peers); index++)
    {
        ezbus_peer_t* peer = ezbus_mac_peers_at(peers,index);
        if ( ezbus_peer_get_seq( peer ) != seq )
        {
            if ( ezbus_address_compare( ezbus_peer_get_address( peer ), &ezbus_self_address ) == 0 )
            {
                ezbus_peer_set_seq( peer, seq );
            }
            else
            {
                ezbus_mac_peers_take( peers, index-- );
            }
        }
    }
}

extern bool ezbus_mac_peers_am_dominant( ezbus_mac_peers_t* peers )
{
    if ( !ezbus_mac_peers_empty( peers ) )
    {
        ezbus_peer_t* peer = ezbus_mac_peers_at(peers,0);
        return ( ezbus_address_compare( ezbus_peer_get_address( peer ), &ezbus_self_address ) == 0 );
    }
    return false;
}

extern void ezbus_mac_peers_crc( ezbus_mac_peers_t* peers, ezbus_crc_t* crc )
{
    ezbus_crc_init( crc );
    for(int index=0; index < ezbus_mac_peers_count(peers); index++)
    {
        ezbus_peer_t* peer = ezbus_mac_peers_at(peers,index);
        ezbus_crc( crc, ezbus_peer_get_address( peer ), sizeof(ezbus_address_t) );
    }
}

extern void ezbus_mac_peers_log( ezbus_mac_t* mac )
{
    ezbus_mac_peers_t* peers = ezbus_mac_get_peers(mac);
    for(int index=0; index < ezbus_mac_peers_count(mac); index++)
    {
        ezbus_peer_t* peer = ezbus_mac_peers_at(peers,index);
        ezbus_log( EZBUS_LOG_PEERS, "%s:%3d, ", ezbus_address_string( ezbus_peer_get_address( peer ) ), ezbus_peer_get_seq( peer ) );
    }
    ezbus_log( 1, "\n" );
}
