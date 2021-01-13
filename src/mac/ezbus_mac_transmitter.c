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
#include <ezbus_mac_transmitter.h>
#include <ezbus_hex.h>
#include <ezbus_log.h>

static ezbus_mac_transmitter_t ezbus_mac_transmitter_stack[EZBUS_MAC_STACK_SIZE];

static void ezbus_mac_transmitter_set_err                 ( ezbus_mac_t* mac, EZBUS_ERR err );
static void do_mac_transmitter_state_send                 ( ezbus_mac_t* mac );
static void do_mac_transmitter_state_sent                 ( ezbus_mac_t* mac );

void ezbus_mac_transmitter_init( ezbus_mac_t* mac )
{
    ezbus_mac_transmitter_t* transmitter = ezbus_mac_get_transmitter( mac );

    ezbus_platform_memset(transmitter,0,sizeof(ezbus_mac_transmitter_t));
}


void ezbus_mac_transmitter_run ( ezbus_mac_t* mac )
{
    static ezbus_mac_transmitter_state_t transmitter_state=(ezbus_mac_transmitter_state_t)0xff;

    if ( ezbus_mac_transmitter_get_state( mac ) != transmitter_state )
    {
        EZBUS_LOG( EZBUS_LOG_TRANSMITTERSTATE, "%s", ezbus_mac_transmitter_get_state_str(mac) );
        transmitter_state = ezbus_mac_transmitter_get_state( mac );
    }

    switch( ezbus_mac_transmitter_get_state( mac ) )
    {
        
        case transmitter_state_empty:   
            ezbus_mac_transmitter_signal_empty( mac );
            break;
        
        case transmitter_state_full:
            ezbus_mac_transmitter_signal_full( mac );
            ezbus_mac_transmitter_set_state( mac, transmitter_state_send );
            break;
        
        case transmitter_state_send:
            do_mac_transmitter_state_send( mac );
           break;
        
        case transmitter_state_sent:
            do_mac_transmitter_state_sent( mac );
            break;
    }
}

extern void  ezbus_mac_transmitter_push( ezbus_mac_t* mac, uint8_t level )
{
    ezbus_mac_transmitter_t* transmitter = ezbus_mac_get_transmitter( mac );
    ezbus_platform_memcpy(&ezbus_mac_transmitter_stack[level],transmitter,sizeof(ezbus_mac_transmitter_t));
}

extern void  ezbus_mac_transmitter_pop ( ezbus_mac_t* mac, uint8_t level )
{
    ezbus_mac_transmitter_t* transmitter = ezbus_mac_get_transmitter( mac );
    ezbus_platform_memcpy(transmitter,&ezbus_mac_transmitter_stack[level],sizeof(ezbus_mac_transmitter_t));
}

extern void ezbus_mac_transmitter_put( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    ezbus_packet_copy( ezbus_mac_get_transmitter_packet( mac ), packet );
    ezbus_mac_transmitter_set_state( mac, transmitter_state_full );
}

extern void  ezbus_mac_transmitter_reload( ezbus_mac_t* mac )
{
    ezbus_mac_transmitter_set_state( mac, transmitter_state_full );
}

extern void ezbus_mac_transmitter_reset( ezbus_mac_t* mac )
{
    ezbus_mac_transmitter_set_state( mac, transmitter_state_empty );
}


static void do_mac_transmitter_state_send( ezbus_mac_t* mac ) 
{
    ezbus_mac_transmitter_set_err( mac, ezbus_port_send( ezbus_mac_get_port( mac ), ezbus_mac_get_transmitter_packet( mac ) ) );
    if ( ezbus_mac_transmitter_get_err( mac ) == EZBUS_ERR_OKAY )
    {
        ezbus_mac_transmitter_set_state( mac, transmitter_state_sent );
    }
    else
    {
        ezbus_mac_transmitter_signal_fault( mac );
    }
}

static void do_mac_transmitter_state_sent( ezbus_mac_t* mac )
{
    ezbus_packet_t* tx_packet = ezbus_mac_get_transmitter_packet( mac );

    ezbus_mac_transmitter_signal_sent( mac );
    if ( ezbus_packet_type( tx_packet  ) == packet_type_parcel )
    {
        if ( ezbus_packet_ack_req( tx_packet ) )
        {
            ezbus_mac_transmitter_signal_wait( mac );
        }
    }
    ezbus_mac_transmitter_reset( mac );
}


extern ezbus_packet_type_t ezbus_mac_transmitter_get_packet_type( ezbus_mac_t* mac )
{
    ezbus_packet_t* tx_packet = ezbus_mac_get_transmitter_packet( mac );

    return ezbus_packet_type( tx_packet  );
}

static void ezbus_mac_transmitter_set_err( ezbus_mac_t* mac, EZBUS_ERR err )
{
    ezbus_mac_transmitter_t* transmitter = ezbus_mac_get_transmitter( mac );
    transmitter->err = err;
}

extern EZBUS_ERR ezbus_mac_transmitter_get_err( ezbus_mac_t* mac )
{
    ezbus_mac_transmitter_t* transmitter = ezbus_mac_get_transmitter( mac );
    return transmitter->err;
}


extern void  ezbus_mac_transmitter_set_state( ezbus_mac_t* mac, ezbus_mac_transmitter_state_t state )
{
    ezbus_mac_transmitter_t* transmitter = ezbus_mac_get_transmitter( mac );
    transmitter->state = state;
}

extern ezbus_mac_transmitter_state_t ezbus_mac_transmitter_get_state( ezbus_mac_t* mac )
{
    ezbus_mac_transmitter_t* transmitter = ezbus_mac_get_transmitter( mac );
    return transmitter->state;
}

const char* ezbus_mac_transmitter_get_state_str( ezbus_mac_t* mac )
{
    const char* rc="";
    switch ( ezbus_mac_transmitter_get_state( mac ) )
    {
        case transmitter_state_empty:            rc="transmitter_state_empty";              break;
        case transmitter_state_full:             rc="transmitter_state_full";               break;
        case transmitter_state_send:             rc="transmitter_state_send";               break;
        case transmitter_state_sent:             rc="transmitter_state_sent";               break;   
    }
    return rc;
}

