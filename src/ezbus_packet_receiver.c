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
#include <ezbus_packet_receiver.h>
#include <ezbus_hex.h>

static void ezbus_transceiver_handle_receiver_state_empty      ( ezbus_packet_receiver_t* packet_receiver );
static void ezbus_transceiver_handle_receiver_state_full       ( ezbus_packet_receiver_t* packet_receiver );
static void ezbus_transceiver_handle_receiver_state_ack        ( ezbus_packet_receiver_t* packet_receiver );



void ezbus_packet_receiver_run ( ezbus_packet_receiver_t* packet_receiver )
{
	switch ( ezbus_packet_receiver_get_state( packet_receiver ) )
	{

		case receiver_state_empty:
			ezbus_transceiver_handle_receiver_state_empty( packet_receiver );
			break;

		case receiver_state_full:
			ezbus_transceiver_handle_receiver_state_full( packet_receiver );
			break;

		case receiver_state_ack:
			ezbus_transceiver_handle_receiver_state_ack( packet_receiver );
			break;
	
	}
}

void ezbus_packet_receiver_init( ezbus_packet_receiver_t* packet_receiver, ezbus_port_t* port, ezbus_receiver_callback_t callback, void* arg )
{
	ezbus_platform_memset( packet_receiver, 0, sizeof(ezbus_packet_receiver_t) );
	packet_receiver->port     = port;
	packet_receiver->callback = callback;
	packet_receiver->arg      = arg;
}

void ezbus_packet_receiver_get( ezbus_packet_receiver_t* packet_receiver, ezbus_packet_t* packet )
{
	ezbus_packet_copy( packet, ezbus_packet_receiver_get_packet( packet_receiver ) );
}




static void ezbus_transceiver_handle_receiver_state_empty( ezbus_packet_receiver_t* packet_receiver )
{
	ezbus_packet_receiver_set_err( packet_receiver, 
									ezbus_port_recv( ezbus_packet_receiver_get_port( packet_receiver ), 
										ezbus_packet_receiver_get_packet( packet_receiver ) ) );
	if ( ezbus_packet_receiver_get_err( packet_receiver ) == EZBUS_ERR_OKAY )
	{
		ezbus_hex_dump( "RX:", (uint8_t*)ezbus_packet_receiver_get_packet( packet_receiver ), sizeof(ezbus_header_t) );
		ezbus_packet_receiver_set_state( packet_receiver, receiver_state_full );
	}
	else
	{
		/* callback should examine fault, return true to reset fault. */
		if ( packet_receiver->callback( packet_receiver, packet_receiver->arg ) )
		{
			ezbus_packet_receiver_set_err( packet_receiver, EZBUS_ERR_OKAY );
		}
	}
}

static void ezbus_transceiver_handle_receiver_state_full( ezbus_packet_receiver_t* packet_receiver )
{
	/* 
	* callback should return true when packet has been received. 
	*/
	if ( packet_receiver->callback( packet_receiver, packet_receiver->arg ) )
	{
		bool needs_ack = ( ezbus_packet_type( &packet_receiver->packet ) == packet_type_parcel );
		ezbus_packet_receiver_set_state( packet_receiver, needs_ack ? receiver_state_ack : receiver_state_empty );
	}
}

static void ezbus_transceiver_handle_receiver_state_ack( ezbus_packet_receiver_t* packet_receiver )
{
	/*
	* callback should return true when acknowledge is acknowledged.
	*/ 
	if ( packet_receiver->callback( packet_receiver, packet_receiver->arg ) )
	{
		ezbus_packet_receiver_set_state( packet_receiver, receiver_state_empty );
	}
}
