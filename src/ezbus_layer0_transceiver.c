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
#include <ezbus_layer0_transceiver.h>
#include <ezbus_token.h>
#include <ezbus_hex.h>

static bool ezbus_layer0_transceiver_give_token  ( ezbus_layer0_transceiver_t* layer0_transceiver );
static bool ezbus_layer0_transceiver_tx_callback ( ezbus_layer0_transmitter_t* layer0_transmitter, void* arg );
static bool ezbus_layer0_transceiver_rx_callback ( ezbus_layer0_receiver_t*    layer0_receiver,    void* arg );


void ezbus_layer0_transceiver_init (    
                                        ezbus_layer0_transceiver_t*             layer0_transceiver, 
                                        ezbus_port_t*                           port,
                                        
                                        ezbus_next_in_token_ring_callback_t     token_ring_callback, 
                                        ezbus_peer_list_callback_t              peer_list_callback,

                                        ezbus_layer1_callback_t                 layer1_tx_callback,
                                        ezbus_layer1_callback_t                 layer1_rx_callback
                                    )
{
    layer0_transceiver->port = port;

    layer0_transceiver->token_ring_callback = token_ring_callback;
    layer0_transceiver->peer_list_callback  = peer_list_callback;

    layer0_transceiver->layer1_tx_callback = layer1_tx_callback;
    layer0_transceiver->layer1_rx_callback = layer1_rx_callback;

    ezbus_layer0_receiver_init    ( &layer0_transceiver->layer0_receiver,    port, ezbus_layer0_transceiver_rx_callback, layer0_transceiver );
    ezbus_layer0_transmitter_init ( &layer0_transceiver->layer0_transmitter, port, ezbus_layer0_transceiver_tx_callback, layer0_transceiver );
}

void ezbus_layer0_transceiver_run  ( ezbus_layer0_transceiver_t* layer0_transceiver )
{
    ezbus_layer0_receiver_run( &layer0_transceiver->layer0_receiver );
    ezbus_layer0_transmitter_run( &layer0_transceiver->layer0_transmitter );    
}


static bool ezbus_layer0_transceiver_tx_callback( ezbus_layer0_transmitter_t* layer0_transmitter, void* arg )
{
    bool rc = false;
    ezbus_layer0_transceiver_t* layer0_transceiver = (ezbus_layer0_transceiver_t*)arg;

    switch( ezbus_layer0_transmitter_get_state( layer0_transmitter ) )
    {
        case transmitter_state_empty:
            /*
            * In the event the callback would like to transmit, it should store a packet, and return 'true'.
            */
            if ( (rc = layer0_transceiver->layer1_tx_callback( layer0_transceiver )) )
            {
                layer0_transceiver->transmitter_full_time = ezbus_platform_get_ms_ticks();
            }
            break;
        case transmitter_state_full:
            /*
            * callback (tx full + no token) should return 'true' to send regardless of token state, 
            * else 'false' and/or remedial action on timeout.
            */
            if ( ezbus_platform_get_ms_ticks() - layer0_transceiver->transmitter_full_time > ezbus_layer0_transceiver_token_timeout(layer0_transceiver) )
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
            rc = ezbus_layer0_transceiver_give_token( layer0_transceiver );
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

static bool ezbus_layer0_transceiver_rx_callback( ezbus_layer0_receiver_t* layer0_receiver, void* arg )
{
    bool rc=false;
    ezbus_layer0_transceiver_t* layer0_transceiver = (ezbus_layer0_transceiver_t*)arg;

    switch ( ezbus_layer0_receiver_get_state( layer0_receiver ) )
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
            rc = layer0_transceiver->layer1_rx_callback( layer0_transceiver );
            break;
        case receiver_state_ack:
            /*
             * callback should return true when acknowledge is acknowledged.
             */
            rc = ( ezbus_layer0_transmitter_get_state( &layer0_transceiver->layer0_transmitter ) == transmitter_state_wait_ack );
            break;
    }
    return rc;
}


ezbus_ms_tick_t ezbus_layer0_transceiver_token_timeout( ezbus_layer0_transceiver_t* layer0_transceiver )
{
    ezbus_peer_list_t peer_list;

    layer0_transceiver->peer_list_callback( &peer_list );

    return ezbus_token_calc_timeout_period ( sizeof(ezbus_packet_t), ezbus_peer_list_count(&peer_list), ezbus_port_get_speed(layer0_transceiver->port) );
}


static bool ezbus_layer0_transceiver_give_token( ezbus_layer0_transceiver_t* layer0_transceiver )
{
    ezbus_packet_t  tx_packet;

    ezbus_packet_init( &tx_packet );
    ezbus_packet_set_type( &tx_packet, packet_type_give_token );

    ezbus_platform_address( ezbus_packet_src( &tx_packet ) );
    layer0_transceiver->token_ring_callback( ezbus_packet_dst( &tx_packet ) );
    
    ezbus_port_send( ezbus_layer0_transmitter_get_port( &layer0_transceiver->layer0_transmitter ), &tx_packet );

    return true; /* FIXME ?? */
}


