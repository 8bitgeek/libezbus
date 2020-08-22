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
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <ezbus.h>
#include <ezbus_flip.h>
#include <ezbus_port.h>
#include <ezbus_mac_peers.h>
#include <ezbus_log.h>
#include <ezbus_socket.h>
#include <ezbus_mac.h>
#include <ezbus_peer.h>
#include <ezbus_address.h>

static ezbus_port_t    port;
static ezbus_t         ezbus;
static ezbus_socket_t  socket = EZBUS_SOCKET_INVALID;

static int             recv_number=0;
static bool            recvd=true;

static uint8_t  hex_to_mybble( char ch );
static uint8_t  hex_to_byte( char* s );
static void     set_address( char* s );
static void     run(void* arg);
static void     ezbus_app_run(ezbus_t* ezbus);
static void     ezbus_socket_open_peer ( ezbus_mac_t* mac, ezbus_address_t* peer_address );


static ezbus_address_t* get_a_peer( ezbus_mac_t* mac )
{
    ezbus_address_t* rc = NULL;;
    for( int n=0; rc == NULL && n < ezbus_mac_peers_count( mac ); n++ )
    {
        ezbus_peer_t* peer = ezbus_mac_peers_at( mac, n );
        ezbus_address_t* peer_address = ezbus_peer_get_address( peer );
        if ( !ezbus_address_is_self(peer_address) )
        {
            rc = peer_address;
        }
    }
    return rc;
}

static void ezbus_socket_open_peer ( ezbus_mac_t* mac, ezbus_address_t* peer_address )
{
    if ( socket == EZBUS_SOCKET_INVALID )
    {
        ezbus_address_t* peer_address = get_a_peer(mac);
        socket = ezbus_socket_open( mac, peer_address, EZBUS_SOCKET_ANY );
        EZBUS_LOG( EZBUS_LOG_SOCKET, "peer %s %d", ezbus_address_string(peer_address), socket );
    }
}

static void ezbus_app_run(ezbus_t* ezbus)
{
    if ( socket == EZBUS_SOCKET_INVALID )
    {
        ezbus_mac_t* mac = ezbus_mac( ezbus );
        ezbus_address_t* peer_address = get_a_peer( ezbus_mac( ezbus ) );
        if ( peer_address )
        {
            ezbus_socket_open_peer( mac, peer_address );
        }
    }
}



extern bool ezbus_socket_callback_send ( ezbus_socket_t socket )
{
    if ( recvd )
    {
        char number[33];
        recvd=false;
        sprintf( number, "%d", ++recv_number );
        fprintf( stderr, "%d ", recv_number );
        return ezbus_socket_send( socket, number, ezbus_platform_strlen(number) ) > 0;
    }
    return false;
}

extern bool ezbus_socket_callback_recv ( ezbus_socket_t socket )
{
    char number[33];
    ezbus_platform_memset(number,0,33);
    if ( ezbus_socket_recv( socket, number, 32 ) > 0 )
    {
        recvd=true;
        recv_number = atoi(number);
        fprintf( stderr, "%d\t%d\t%d\n", socket, ezbus_socket_get_peer_socket(socket), recv_number );
    }
    return true;
}




static void run(void* arg)
{
    char* serial_port_name = (char*)arg;
    
    ezbus_port_init_struct( &port );

    ezbus_platform_port_set_name(&port,serial_port_name);
    ezbus_platform_port_set_handle(&port,-1);

    if ( ezbus_port_open( &port, EZBUS_SPEED_DEF ) == EZBUS_ERR_OKAY )
    {
        ezbus_init( &ezbus, &port );

        for(;;) /* forever... */
        {
            ezbus_run(&ezbus);
            ezbus_app_run(&ezbus);
        }
    }
}

static uint8_t hex_to_mybble( char ch )
{
    if ( ch >= 'a' && ch <= 'z' )
        ch -= 0x20;
    if ( ch >= 'A' && ch <= 'Z' )
        ch = ch -'A' + 10;
    return ch & 0x0F; 
}

static uint8_t hex_to_byte( char* s )
{
    return (hex_to_mybble(s[0])<<4) | hex_to_mybble(s[1]);
}

static void set_address( char* s )
{
    ezbus_address_t address;
    uint32_t i=0;
    for(int n=0; n < ezbus_platform_strlen(s) && n < (EZBUS_ADDR_LN*2); n+=2 )
    {
        address.byte[i++] = hex_to_byte(&s[n]);
    }
    ezbus_platform_set_address(&address);
}

int main(int argc,char* argv[])
{
    if ( argc == 2 )
    {
        run(argv[1]);
    }
    if ( argc == 3 )
    {
        set_address(argv[1]);
        run(argv[2]);
    }
    else
    {
        fprintf(stderr,"usage: %s [XXXXXXXXXXXXXXXXXXXXXXXX] /dev/ttyxxx\n", argv[0]);
    }
    return 0;
}
