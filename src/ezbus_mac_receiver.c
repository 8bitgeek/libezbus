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
#include <ezbus_mac_receiver.h>
#include <ezbus_hex.h>

static void ezbus_transceiver_handle_receiver_state_empty     		( ezbus_mac_receiver_t* mac_receiver );
static void ezbus_transceiver_handle_receiver_state_full      		( ezbus_mac_receiver_t* mac_receiver );
static void ezbus_transceiver_handle_receiver_state_receive_fault   ( ezbus_mac_receiver_t* mac_receiver );
static void ezbus_transceiver_handle_receiver_state_transit_to_ack  ( ezbus_mac_receiver_t* mac_receiver );
static void ezbus_transceiver_handle_receiver_state_wait_ack_sent   ( ezbus_mac_receiver_t* mac_receiver );



void ezbus_mac_receiver_run ( ezbus_mac_receiver_t* mac_receiver )
{
	switch ( ezbus_mac_receiver_get_state( mac_receiver ) )
	{

		case receiver_state_empty:
			ezbus_transceiver_handle_receiver_state_empty( mac_receiver );
			break;

		case receiver_state_full:
			ezbus_transceiver_handle_receiver_state_full( mac_receiver );
			break;

		case receiver_state_receive_fault:
			ezbus_transceiver_handle_receiver_state_receive_fault( mac_receiver );
			break;
	
		case receiver_state_transit_to_ack:
			ezbus_transceiver_handle_receiver_state_transit_to_ack( mac_receiver );
			break;
	
		case receiver_state_wait_ack_sent:
			ezbus_transceiver_handle_receiver_state_wait_ack_sent( mac_receiver );
			break;

	}
}

void ezbus_mac_receiver_init( ezbus_mac_receiver_t* mac_receiver, ezbus_port_t* port, ezbus_receiver_callback_t callback, void* callback_arg )
{
	ezbus_platform_memset( mac_receiver, 0, sizeof(ezbus_mac_receiver_t) );
	mac_receiver->port     = port;
	mac_receiver->arg      = callback_arg;
}

void ezbus_mac_receiver_get( ezbus_mac_receiver_t* mac_receiver, ezbus_packet_t* packet )
{
	ezbus_packet_copy( packet, ezbus_mac_receiver_get_packet( mac_receiver ) );
}


static void ezbus_transceiver_handle_receiver_state_empty( ezbus_mac_receiver_t* mac_receiver )
{
	ezbus_mac_receiver_empty_callback( mac_receiver, mac_receiver->arg );
	ezbus_mac_receiver_set_err( mac_receiver, 
									ezbus_port_recv( ezbus_mac_receiver_get_port( mac_receiver ), 
										ezbus_mac_receiver_get_packet( mac_receiver ) ) );
	if ( ezbus_mac_receiver_get_err( mac_receiver ) == EZBUS_ERR_OKAY )
	{
		ezbus_mac_receiver_set_state( mac_receiver, receiver_state_full );
	}
	else
	{
		ezbus_mac_receiver_set_state( mac_receiver, receiver_state_receive_fault );
	}
}

static void ezbus_transceiver_handle_receiver_state_receive_fault( ezbus_mac_receiver_t* mac_receiver )
{
	ezbus_mac_receiver_fault_callback( mac_receiver, mac_receiver->arg );

	ezbus_mac_receiver_set_err( mac_receiver, EZBUS_ERR_OKAY );
	ezbus_mac_receiver_set_state( mac_receiver, receiver_state_empty );
}

static void ezbus_transceiver_handle_receiver_state_full( ezbus_mac_receiver_t* mac_receiver )
{
	bool needs_ack;
	
	ezbus_mac_receiver_full_callback( mac_receiver, mac_receiver->arg );
	
	needs_ack = ( ezbus_packet_type( &mac_receiver->packet ) == packet_type_parcel );
	ezbus_mac_receiver_set_state( mac_receiver, needs_ack ? receiver_state_transit_to_ack : receiver_state_empty );
}

static void ezbus_transceiver_handle_receiver_state_transit_to_ack( ezbus_mac_receiver_t* mac_receiver )
{
	ezbus_mac_receiver_sent_callback( mac_receiver, mac_receiver->arg );
	ezbus_mac_receiver_set_state( mac_receiver, receiver_state_wait_ack_sent );
}

static void ezbus_transceiver_handle_receiver_state_wait_ack_sent( ezbus_mac_receiver_t* mac_receiver )
{
	ezbus_mac_receiver_wait_callback( mac_receiver, mac_receiver->arg );
}



extern void ezbus_mac_receiver_empty_callback ( ezbus_mac_transmitter_t*, void* )  __attribute__((weak))
{
    ezbus_log( EZBUS_LOG_RECEIVER, "ezbus_mac_receiver_empty_callback\n" );
}

extern void ezbus_mac_receiver_full_callback  ( ezbus_mac_transmitter_t*, void* )  __attribute__((weak))
{
    ezbus_log( EZBUS_LOG_RECEIVER, "ezbus_mac_receiver_full_callback\n" );
}

extern void ezbus_mac_receiver_wait_callback  ( ezbus_mac_transmitter_t*, void* )  __attribute__((weak))
{
    ezbus_log( EZBUS_LOG_RECEIVER, "ezbus_mac_receiver_wait_callback\n" );
}

extern void ezbus_mac_receiver_fault_callback ( ezbus_mac_transmitter_t*, void* )  __attribute__((weak))
{
    ezbus_log( EZBUS_LOG_RECEIVER, "ezbus_mac_receiver_fault_callback\n" );
}


