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
#include <ezbus_layer0_transmitter.h>
#include <ezbus_hex.h>

static void ezbus_layer0_handle_transmitter_state_empty      ( ezbus_layer0_transmitter_t* layer0_transmitter );
static void ezbus_layer0_handle_transmitter_state_full       ( ezbus_layer0_transmitter_t* layer0_transmitter );
static void ezbus_layer0_handle_transmitter_state_send       ( ezbus_layer0_transmitter_t* layer0_transmitter );
static void ezbus_layer0_handle_transmitter_state_give_token ( ezbus_layer0_transmitter_t* layer0_transmitter );
static void ezbus_layer0_handle_transmitter_state_wait_ack   ( ezbus_layer0_transmitter_t* layer0_transmitter );



void ezbus_layer0_transmitter_run ( ezbus_layer0_transmitter_t* layer0_transmitter )
{
    switch( ezbus_layer0_transmitter_get_state( layer0_transmitter ) )
    {
        
        case transmitter_state_empty:   
            ezbus_layer0_handle_transmitter_state_empty( layer0_transmitter );
            break;
        
        case transmitter_state_full:
            ezbus_layer0_handle_transmitter_state_full( layer0_transmitter );
            break;
        
        case transmitter_state_send:
            ezbus_layer0_handle_transmitter_state_send( layer0_transmitter );
           break;
        
        case transmitter_state_give_token:
            ezbus_layer0_handle_transmitter_state_give_token( layer0_transmitter );
            break;
        
        case transmitter_state_wait_ack:
            ezbus_layer0_handle_transmitter_state_wait_ack( layer0_transmitter );
            break;
    
    }
}




void ezbus_layer0_transmitter_init( ezbus_layer0_transmitter_t* layer0_transmitter, ezbus_port_t* port, ezbus_transmitter_callback_t callback, void* arg )
{
    ezbus_platform_memset(layer0_transmitter,0,sizeof(ezbus_layer0_transmitter_t));
    layer0_transmitter->port     = port;
    layer0_transmitter->callback = callback;
    layer0_transmitter->arg      = arg;
}

void ezbus_layer0_transmitter_put( ezbus_layer0_transmitter_t* layer0_transmitter, ezbus_packet_t* packet )
{
    ezbus_packet_copy( &layer0_transmitter->packet, packet );
    ezbus_layer0_transmitter_set_state( layer0_transmitter, transmitter_state_full );
}


static void ezbus_layer0_handle_transmitter_state_empty( ezbus_layer0_transmitter_t* layer0_transmitter )
{
    /*
     * In the event the callback would like to transmit, it should store a packet, and return 'true'.
     */
    if ( layer0_transmitter->callback( layer0_transmitter, layer0_transmitter->arg ) )
    {
        ezbus_layer0_transmitter_set_state( layer0_transmitter, transmitter_state_full );
    }
}

static void ezbus_layer0_handle_transmitter_state_full( ezbus_layer0_transmitter_t* layer0_transmitter )
{
    if ( ezbus_layer0_transmitter_get_token( layer0_transmitter ) )
    {
        ezbus_layer0_transmitter_set_state( layer0_transmitter, transmitter_state_send );
    }
    else
    {
        /**
         * callback should return 'true' to send regardless of token state, 
         * else 'false' and/or remedial action on timeout.
         */
        if ( layer0_transmitter->callback( layer0_transmitter, layer0_transmitter->arg ) )
        {
            ezbus_layer0_transmitter_set_state( layer0_transmitter, transmitter_state_send );
        }
    }
}

static void ezbus_layer0_handle_transmitter_state_send( ezbus_layer0_transmitter_t* layer0_transmitter )
{
    ezbus_layer0_transmitter_set_err( layer0_transmitter, 
                                ezbus_port_send( ezbus_layer0_transmitter_get_port( layer0_transmitter ), 
                                    &ezbus_layer0_transmitter_get_packet( layer0_transmitter ) ) );
    if ( ezbus_layer0_transmitter_get_err( layer0_transmitter ) == EZBUS_ERR_OKAY )
    {
        ezbus_hex_dump( "TX:", (uint8_t*)&ezbus_layer0_transmitter_get_packet( layer0_transmitter ), sizeof(ezbus_header_t) );
        ezbus_layer0_transmitter_set_state( layer0_transmitter, transmitter_state_give_token );
    }
    else
    {
        /* 
         * callback should examine fault, return true to reset fault, and/or take remedial action. 
         */
        if ( layer0_transmitter->callback( layer0_transmitter, layer0_transmitter->arg ) )
        {
            ezbus_layer0_transmitter_set_err( layer0_transmitter, EZBUS_ERR_OKAY );
        }
    }
}

static void ezbus_layer0_handle_transmitter_state_give_token( ezbus_layer0_transmitter_t* layer0_transmitter )
{
    /* 
     * callback should give up the token without disturning the contents of the transmitter.
     * i.e. it should use port directly to transmit.. 
     * callback should return 'true' upon giving up token.
     */
    if ( layer0_transmitter->callback( layer0_transmitter, layer0_transmitter->arg ) )
    {
        if ( ezbus_packet_type( &layer0_transmitter->packet ) == packet_type_parcel )
        {
            ezbus_layer0_transmitter_set_state( layer0_transmitter, transmitter_state_wait_ack );
        }
        else
        {
            ezbus_layer0_transmitter_set_state( layer0_transmitter, transmitter_state_empty );
        }
    }

}

static void ezbus_layer0_handle_transmitter_state_wait_ack( ezbus_layer0_transmitter_t* layer0_transmitter )
{
    /* 
     * callback should determine if the packet requires an acknowledge, and return 'true' when it arrives. 
     * else upon timeout or ack not required, then callback should reset transmitter state accordingly.
     */
    if ( layer0_transmitter->callback( layer0_transmitter, layer0_transmitter->arg ) )
    {
        ezbus_layer0_transmitter_set_state( layer0_transmitter, transmitter_state_empty );
    }
}

