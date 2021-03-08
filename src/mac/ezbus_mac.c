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
#include <ezbus_mac.h>
#include <ezbus_mac_struct.h>
#include <ezbus_platform.h>

void ezbus_mac_init ( ezbus_mac_t* mac, ezbus_port_t* port )
{
    ezbus_platform.callback_memset( mac, 0 , sizeof( ezbus_mac_t) );

    mac->port = port;

    ezbus_mac_timer_init            ( mac );
    ezbus_mac_pause_init            ( mac );
    ezbus_mac_peers_init            ( mac );
    ezbus_mac_token_init            ( mac );
    ezbus_mac_receiver_init         ( mac );
    ezbus_mac_transmitter_init      ( mac );
    ezbus_mac_arbiter_transmit_init ( mac );
    ezbus_mac_arbiter_init          ( mac );
    ezbus_mac_arbiter_pause_init    ( mac );
}

void ezbus_mac_run( ezbus_mac_t* mac )
{
    ezbus_mac_timer_run             ( mac );
    ezbus_mac_pause_run             ( mac );
    ezbus_mac_peers_run             ( mac );  
    ezbus_mac_token_run             ( mac );
    ezbus_mac_receiver_run          ( mac );  
    ezbus_mac_arbiter_transmit_run  ( mac );
    ezbus_mac_arbiter_run           ( mac );
    ezbus_mac_arbiter_pause_run     ( mac );   
    ezbus_mac_transmitter_run       ( mac );
}

extern inline ezbus_port_t* ezbus_mac_get_port(ezbus_mac_t* mac) 
{
    return mac->port;
}

extern ezbus_mac_peers_t* ezbus_mac_get_peers(ezbus_mac_t* mac)
{
    return &mac->peers;
}

extern ezbus_mac_transmitter_t* ezbus_mac_get_transmitter(ezbus_mac_t* mac)
{
    return &mac->transmitter;
}

extern ezbus_mac_receiver_t* ezbus_mac_get_receiver(ezbus_mac_t* mac)
{
    return &mac->receiver;
}

extern ezbus_mac_arbiter_t* ezbus_mac_get_arbiter(ezbus_mac_t* mac)
{
    return &mac->arbiter;
}

extern ezbus_mac_arbiter_pause_t* ezbus_mac_get_arbiter_pause(ezbus_mac_t* mac)
{
    return &mac->arbiter_pause;
}

extern ezbus_mac_arbiter_transmit_t* ezbus_mac_get_arbiter_transmit(ezbus_mac_t* mac)
{
    return &mac->arbiter_transmit;
}

extern ezbus_mac_token_t* ezbus_mac_get_token(ezbus_mac_t* mac)
{
    return &mac->token;
}

extern ezbus_packet_t* ezbus_mac_get_transmitter_packet(ezbus_mac_t* mac)
{
    return &mac->transmitter.packet;
}

extern ezbus_packet_t* ezbus_mac_get_receiver_packet(ezbus_mac_t* mac)
{
    return &mac->receiver.packet;
}

extern ezbus_mac_pause_t* ezbus_mac_get_pause(ezbus_mac_t* mac)
{
    return &mac->pause;
}

extern ezbus_mac_timer_t* ezbus_mac_get_timer(ezbus_mac_t* mac)
{
    return &mac->timer;
}


