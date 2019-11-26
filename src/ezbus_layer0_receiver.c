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
#include <ezbus_layer0_receiver.h>
#include <ezbus_hex.h>

static void ezbus_transceiver_handle_receiver_state_empty     		( ezbus_layer0_receiver_t* layer0_receiver );
static void ezbus_transceiver_handle_receiver_state_full      		( ezbus_layer0_receiver_t* layer0_receiver );
static void ezbus_transceiver_handle_receiver_state_receive_fault   ( ezbus_layer0_receiver_t* layer0_receiver );
static void ezbus_transceiver_handle_receiver_state_transit_to_ack  ( ezbus_layer0_receiver_t* layer0_receiver );
static void ezbus_transceiver_handle_receiver_state_wait_ack_sent   ( ezbus_layer0_receiver_t* layer0_receiver );



void ezbus_layer0_receiver_run ( ezbus_layer0_receiver_t* layer0_receiver )
{
	switch ( ezbus_layer0_receiver_get_state( layer0_receiver ) )
	{

		case receiver_state_empty:
			ezbus_transceiver_handle_receiver_state_empty( layer0_receiver );
			break;

		case receiver_state_full:
			ezbus_transceiver_handle_receiver_state_full( layer0_receiver );
			break;

		case receiver_state_receive_fault:
			ezbus_transceiver_handle_receiver_state_receive_fault( layer0_receiver );
			break;
	
		case receiver_state_transit_to_ack:
			ezbus_transceiver_handle_receiver_state_transit_to_ack( layer0_receiver );
			break;
	
		case receiver_state_wait_ack_sent:
			ezbus_transceiver_handle_receiver_state_wait_ack_sent( layer0_receiver );
			break;
	
	}
}

void ezbus_layer0_receiver_init( ezbus_layer0_receiver_t* layer0_receiver, ezbus_port_t* port, ezbus_receiver_callback_t callback, void* arg )
{
	ezbus_platform_memset( layer0_receiver, 0, sizeof(ezbus_layer0_receiver_t) );
	layer0_receiver->port     = port;
	layer0_receiver->callback = callback;
	layer0_receiver->arg      = arg;
}

void ezbus_layer0_receiver_get( ezbus_layer0_receiver_t* layer0_receiver, ezbus_packet_t* packet )
{
	ezbus_packet_copy( packet, ezbus_layer0_receiver_get_packet( layer0_receiver ) );
}




static void ezbus_transceiver_handle_receiver_state_empty( ezbus_layer0_receiver_t* layer0_receiver )
{
	ezbus_layer0_receiver_set_err( layer0_receiver, 
									ezbus_port_recv( ezbus_layer0_receiver_get_port( layer0_receiver ), 
										ezbus_layer0_receiver_get_packet( layer0_receiver ) ) );
	if ( ezbus_layer0_receiver_get_err( layer0_receiver ) == EZBUS_ERR_OKAY )
	{
		ezbus_hex_dump( "RX:", (uint8_t*)ezbus_layer0_receiver_get_packet( layer0_receiver ), sizeof(ezbus_header_t) );
		ezbus_layer0_receiver_set_state( layer0_receiver, receiver_state_full );
	}
	else
	{
		ezbus_layer0_receiver_set_state( layer0_receiver, receiver_state_receive_fault );
	}
}

static void ezbus_transceiver_handle_receiver_state_receive_fault( ezbus_layer0_receiver_t* layer0_receiver )
{
	/* 
	* callback should acknowledge the fault to return receiver back to receiver_empty state 
	*/
	if ( layer0_receiver->callback( layer0_receiverm layer0_receiver->arg ) )
	{
		ezbus_layer0_receiver_set_err( layer0_receiver, EZBUS_ERR_OKAY );
		ezbus_layer0_receiver_set_state( layer0_receiver, receiver_state_empty );
	}
}

static void ezbus_transceiver_handle_receiver_state_full( ezbus_layer0_receiver_t* layer0_receiver )
{
	/* 
	* callback should return true when packet has been received. 
	*/
	if ( layer0_receiver->callback( layer0_receiver, layer0_receiver->arg ) )
	{
		bool needs_ack = ( ezbus_packet_type( &layer0_receiver->packet ) == packet_type_parcel );
		ezbus_layer0_receiver_set_state( layer0_receiver, needs_ack ? receiver_state_transit_to_ack : receiver_state_empty );
	}
}

static void ezbus_transceiver_handle_receiver_state_transit_to_ack( ezbus_layer0_receiver_t* layer0_receiver )
{
	if ( layer0_receiver->callback( layer0_receiver, layer0_receiver->arg ) )
	{
		ezbus_layer0_receiver_set_state( layer0_receiver, receiver_state_ack );
	}
}

static void ezbus_transceiver_handle_receiver_state_wait_ack_sent( ezbus_layer0_receiver_t* layer0_receiver )
{
	/*
	* callback should return true when ack has been sent.
	*/ 
	if ( layer0_receiver->callback( layer0_receiver, layer0_receiver->arg ) )
	{
		ezbus_layer0_receiver_set_state( layer0_receiver, receiver_state_empty );
	}
}
