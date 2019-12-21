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
#include <ezbus_mac_transmitter.h>
#include <ezbus_hex.h>
#include <ezbus_log.h>

static void do_mac_transmitter_state_empty             ( ezbus_mac_transmitter_t* mac_transmitter );
static void do_mac_transmitter_state_transit_full      ( ezbus_mac_transmitter_t* mac_transmitter );
static void do_mac_transmitter_state_full              ( ezbus_mac_transmitter_t* mac_transmitter );
static void do_mac_transmitter_state_send              ( ezbus_mac_transmitter_t* mac_transmitter );
static void do_mac_transmitter_state_give_token        ( ezbus_mac_transmitter_t* mac_transmitter );
static void do_mac_transmitter_state_transit_wait_ack  ( ezbus_mac_transmitter_t* mac_transmitter );
static void do_mac_transmitter_state_wait_ack          ( ezbus_mac_transmitter_t* mac_transmitter );




void ezbus_mac_transmitter_init( ezbus_mac_transmitter_t* mac_transmitter, ezbus_port_t* port, ezbus_transmitter_callback_t callback, void* arg )
{
    ezbus_platform_memset(mac_transmitter,0,sizeof(ezbus_mac_transmitter_t));
    mac_transmitter->port     = port;
    mac_transmitter->callback = callback;
    mac_transmitter->arg      = arg;
}



void ezbus_mac_transmitter_run ( ezbus_mac_transmitter_t* mac_transmitter )
{
    static ezbus_mac_transmitter_state_t transmitter_state=(ezbus_mac_transmitter_state_t)0xff;

    if ( ezbus_mac_transmitter_get_state( mac_transmitter ) != transmitter_state )
    {
        ezbus_log( EZBUS_LOG_TRANSMITTERSTATE, "%s\n", ezbus_mac_transmitter_get_state_str(mac_transmitter) );
        transmitter_state = ezbus_mac_transmitter_get_state( mac_transmitter );
    }

    switch( ezbus_mac_transmitter_get_state( mac_transmitter ) )
    {
        
        case transmitter_state_empty:   
            do_mac_transmitter_state_empty( mac_transmitter );
            break;
        
        case transmitter_state_transit_full:
            do_mac_transmitter_state_transit_full( mac_transmitter );
            break;
        
        case transmitter_state_full:
            do_mac_transmitter_state_full( mac_transmitter );
            break;
        
        case transmitter_state_send:
            do_mac_transmitter_state_send( mac_transmitter );
           break;
        
        case transmitter_state_give_token:
            do_mac_transmitter_state_give_token( mac_transmitter );
            break;
        
        case transmitter_state_transit_wait_ack:
            do_mac_transmitter_state_transit_wait_ack( mac_transmitter );
            break;
    
        case transmitter_state_wait_ack:
            do_mac_transmitter_state_wait_ack( mac_transmitter );
            break;
    
    }
}




void ezbus_mac_transmitter_put( ezbus_mac_transmitter_t* mac_transmitter, ezbus_packet_t* packet )
{
    ezbus_packet_copy( &mac_transmitter->packet, packet );
    ezbus_mac_transmitter_set_state( mac_transmitter, transmitter_state_full );
}


static void do_mac_transmitter_state_empty( ezbus_mac_transmitter_t* mac_transmitter )
{
    /*
     * In the event the callback would like to transmit, it should store a packet, and return 'true'.
     */
    if ( mac_transmitter->callback( mac_transmitter, mac_transmitter->arg ) )
    {
        ezbus_mac_transmitter_set_state( mac_transmitter, transmitter_state_transit_full );
    }
}

static void do_mac_transmitter_state_transit_full( ezbus_mac_transmitter_t* mac_transmitter )
{
    if ( mac_transmitter->callback( mac_transmitter, mac_transmitter->arg ) )
    {
        ezbus_mac_transmitter_set_state( mac_transmitter, transmitter_state_full );
    }
}

static void do_mac_transmitter_state_full( ezbus_mac_transmitter_t* mac_transmitter )
{
    if ( mac_transmitter->callback( mac_transmitter, mac_transmitter->arg ) )
    {
        ezbus_mac_transmitter_set_state( mac_transmitter, transmitter_state_send );
    }
}

static void do_mac_transmitter_state_send( ezbus_mac_transmitter_t* mac_transmitter )
{
    ezbus_mac_transmitter_set_err( mac_transmitter, 
                                ezbus_port_send( ezbus_mac_transmitter_get_port( mac_transmitter ), 
                                    ezbus_mac_transmitter_get_packet( mac_transmitter ) ) );
    if ( ezbus_mac_transmitter_get_err( mac_transmitter ) == EZBUS_ERR_OKAY )
    {
        ezbus_hex_dump( "transmitter_state_give_token:", (uint8_t*)ezbus_mac_transmitter_get_packet( mac_transmitter ), sizeof(ezbus_header_t) );
        ezbus_mac_transmitter_set_state( mac_transmitter, transmitter_state_give_token );
    }
    else
    {
        /* 
         * callback should examine fault, return true to reset fault, and/or take remedial action. 
         */
        if ( mac_transmitter->callback( mac_transmitter, mac_transmitter->arg ) )
        {
            ezbus_mac_transmitter_set_err( mac_transmitter, EZBUS_ERR_OKAY );
        }
    }
}

static void do_mac_transmitter_state_give_token( ezbus_mac_transmitter_t* mac_transmitter )
{
    /* 
     * callback should give up the token without disturning the contents of the transmitter.
     * i.e. it should use port directly to transmit.. 
     * callback should return 'true' upon giving up token.
     */
    if ( mac_transmitter->callback( mac_transmitter, mac_transmitter->arg ) )
    {
        if ( ezbus_packet_type( &mac_transmitter->packet ) == packet_type_parcel )
        {
            //fprintf( stderr, ">>packet_type_parcel\n");
            ezbus_mac_transmitter_set_state( mac_transmitter, transmitter_state_wait_ack );
        }
        else
        {
            //fprintf( stderr, ">>header\n");
            ezbus_mac_transmitter_set_state( mac_transmitter, transmitter_state_empty );
        }
    }

}

static void do_mac_transmitter_state_transit_wait_ack( ezbus_mac_transmitter_t* mac_transmitter )
{
    if ( mac_transmitter->callback( mac_transmitter, mac_transmitter->arg ) )
    {
        ezbus_mac_transmitter_set_state( mac_transmitter, transmitter_state_wait_ack );
    }
}

static void do_mac_transmitter_state_wait_ack( ezbus_mac_transmitter_t* mac_transmitter )
{
    /* 
     * callback should determine if the packet requires an acknowledge, and return 'true' when it arrives. 
     * else upon timeout or ack not required, then callback should reset transmitter state accordingly.
     */
    if ( mac_transmitter->callback( mac_transmitter, mac_transmitter->arg ) )
    {
        ezbus_mac_transmitter_set_state( mac_transmitter, transmitter_state_empty );
    }
}

const char* ezbus_mac_transmitter_get_state_str( ezbus_mac_transmitter_t* mac_transmitter )
{
    const char* rc="";
    switch ( ezbus_mac_transmitter_get_state( mac_transmitter ) )
    {
        case transmitter_state_empty:            rc="transmitter_state_empty";               break;
        case transmitter_state_transit_full:     rc="transmitter_state_transit_full";        break;
        case transmitter_state_full:             rc="transmitter_state_full";                break;
        case transmitter_state_send:             rc="transmitter_state_send";                break;
        case transmitter_state_give_token:       rc="transmitter_state_give_token";          break;   
        case transmitter_state_transit_wait_ack: rc="transmitter_state_transit_wait_ack";    break;
        case transmitter_state_wait_ack:         rc="transmitter_state_wait_ack";            break;
    }
    return rc;
}
