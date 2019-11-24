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
#include <ezbus_packet_transceiver.h>
#include <ezbus_token.h>
#include <ezbus_hex.h>

static bool ezbus_packet_transciever_give_token  ( ezbus_packet_transceiver_t* packet_transceiver );
static bool ezbus_packet_transceiver_tx_callback ( ezbus_packet_transmitter_t* packet_transmitter, void* arg );
static bool ezbus_packet_transceiver_rx_callback ( ezbus_packet_receiver_t*    packet_receiver,    void* arg );


void ezbus_packet_transceiver_init (    
                                        ezbus_packet_transceiver_t*             packet_transceiver, 
                                        ezbus_port_t*                           port,
                                        
                                        ezbus_next_in_token_ring_callback_t     token_ring_callback, 
                                        ezbus_peer_list_callback_t              peer_list_callback,

                                        ezbus_layer1_callback_t                 layer1_tx_callback,
                                        ezbus_layer1_callback_t                 layer1_rx_callback
                                    )
{
    packet_transceiver->port = port;

    packet_transceiver->token_ring_callback = token_ring_callback;
    packet_transceiver->peer_list_callback  = peer_list_callback;

    packet_transceiver->layer1_tx_callback = layer1_tx_callback;
    packet_transceiver->layer1_rx_callback = layer1_rx_callback;

    ezbus_packet_receiver_init    ( &packet_transceiver->packet_receiver,    port, ezbus_packet_transceiver_rx_callback, packet_transceiver );
    ezbus_packet_transmitter_init ( &packet_transceiver->packet_transmitter, port, ezbus_packet_transceiver_tx_callback, packet_transceiver );
}

void ezbus_packet_transceiver_run  ( ezbus_packet_transceiver_t* packet_transceiver )
{
    ezbus_packet_receiver_run( &packet_transceiver->packet_receiver );
    ezbus_packet_transmitter_run( &packet_transceiver->packet_transmitter );    
}


static bool ezbus_packet_transceiver_tx_callback( ezbus_packet_transmitter_t* packet_transmitter, void* arg )
{
    bool rc = false;
    ezbus_packet_transceiver_t* packet_transceiver = (ezbus_packet_transceiver_t*)arg;

    switch( ezbus_packet_transmitter_get_state( packet_transmitter ) )
    {
        case transmitter_state_empty:
            /*
            * In the event the callback would like to transmit, it should store a packet, and return 'true'.
            */
            if ( (rc = packet_transceiver->layer1_tx_callback( packet_transceiver )) )
            {
                packet_transceiver->transmitter_full_time = ezbus_platform_get_ms_ticks();
            }
            break;
        case transmitter_state_full:
            /*
            * callback (tx full + no token) should return 'true' to send regardless of token state, 
            * else 'false' and/or remedial action on timeout.
            */
            if ( ezbus_platform_get_ms_ticks() - packet_transceiver->transmitter_full_time > ezbus_packet_transceiver_token_timeout(packet_transceiver) )
            {
                /* FIXME - do we make this more sophisticated? */
                rc = true;
            }
            break;
        case transmitter_state_send:
            /* 
            * callback should examine fault, return true to reset fault, and/or take remedial action. 
            */
            /** NOTE does this need to be more sophisticated? */
            rc = true;
            break;
        case transmitter_state_give_token:
            /* 
            * callback should return 'true' when prepared to give up token.
            */
            rc = ezbus_packet_transciever_give_token( packet_transceiver );
            break;
        case transmitter_state_wait_ack:
            /* 
            * callback should determine if the packet requires an acknowledge, and return 'true' when it arrives. 
            * else upon timeout or ack not required, then callback should reset transmitter state accordingly.
            */
            rc = true;
            break;
    }
    return rc;
}

static bool ezbus_packet_transceiver_rx_callback( ezbus_packet_receiver_t* packet_receiver, void* arg )
{
    bool rc=false;
    ezbus_packet_transceiver_t* packet_transceiver = (ezbus_packet_transceiver_t*)arg;

    switch ( ezbus_packet_receiver_get_state( packet_receiver ) )
    {
        case receiver_state_empty:
            /* 
             * callback should examine fault, return true to reset fault. 
             */
            rc = true;
            break;
        case receiver_state_full:
            /* 
             * callback should return true when packet has been received. 
             */
            rc = packet_transceiver->layer1_rx_callback( packet_transceiver );
            break;
        case receiver_state_ack:
            /*
             * callback should return true when acknowledge is acknowledged.
             */
            rc = ( ezbus_packet_transmitter_get_state( &packet_transceiver->packet_transmitter ) == transmitter_state_wait_ack );
            break;
    }
    return rc;
}


ezbus_ms_tick_t ezbus_packet_transceiver_token_timeout( ezbus_packet_transceiver_t* packet_transceiver )
{
    ezbus_peer_list_t* peer_list;


    packet_transceiver->peer_list_callback( peer_list );

    return ezbus_token_calc_timeout_period ( sizeof(ezbus_packet_t), ezbus_peer_list_count(peer_list), ezbus_port_get_speed(packet_transceiver->port) );
}


static bool ezbus_packet_transciever_give_token( ezbus_packet_transceiver_t* packet_transceiver )
{
    ezbus_packet_t  tx_packet;

    ezbus_packet_init( &tx_packet );
    ezbus_packet_set_type( &tx_packet, packet_type_give_token );

    ezbus_platform_address( ezbus_packet_src( &tx_packet ) );
    packet_transceiver->token_ring_callback( ezbus_packet_dst( &tx_packet ) );
    
    ezbus_port_send( ezbus_packet_transmitter_get_port( &packet_transceiver->packet_transmitter ), &tx_packet );

    return true; /* FIXME ?? */
}


