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
#include <ezbus_mac.h>
#include <ezbus_mac_struct.h>

void ezbus_mac_init ( ezbus_mac_t* mac, ezbus_port_t* port )
{
    memset( mac, 0 , sizeof( ezbus_mac_t) );

    mac->port = port;

    ezbus_mac_arbitration_init         ( mac );
    ezbus_mac_arbitration_receive_init ( mac );
    ezbus_mac_bootstrap_init           ( mac );
    ezbus_mac_receiver_init            ( mac );
    ezbus_mac_token_init               ( mac );
    ezbus_mac_transmitter_init         ( mac );
}

void ezbus_mac_run( ezbus_mac_t* mac )
{
    ezbus_mac_arbitration_run         ( mac );
    ezbus_mac_arbitration_receive_run ( mac );
    ezbus_mac_bootstrap_run           ( mac );
    ezbus_mac_receiver_run            ( mac );
    ezbus_mac_token_run               ( mac );
    ezbus_mac_transmitter_run         ( mac );
}

extern ezbus_port_t* ezbus_mac_get_port(ezbus_mac_t* mac)
{
    return mac->port;
}

extern ezbus_peer_list_t* ezbus_mac_get_peer_list(ezbus_mac_t* mac)
{
    return &mac->peer_list;
}

extern ezbus_mac_bootstrap_t* ezbus_mac_get_bootstrap(ezbus_mac_t* mac)
{
    return &mac->bootstrap;
}

extern ezbus_mac_transmitter_t* ezbus_mac_get_transmitter(ezbus_mac_t* mac)
{
    return &mac->transmitter;
}

extern ezbus_mac_receiver_t* ezbus_mac_get_receiver(ezbus_mac_t* mac)
{
    return &mac->receiver;
}

extern ezbus_mac_arbitration_t* ezbus_mac_get_arbitration(ezbus_mac_t* mac)
{
    return &mac->arbitration;
}

extern ezbus_mac_arbitration_receive_t* ezbus_mac_get_arbitration_receive(ezbus_mac_t* mac)
{
    return &mac->arbitration_receive;
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


