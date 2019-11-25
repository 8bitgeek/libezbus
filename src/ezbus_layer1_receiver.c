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
#include <ezbus_layer1_receiver.h>
#include <ezbus_hex.h>

static void ezbus_transceiver_handle_receiver_state_empty      ( ezbus_layer1_receiver_t* layer1_receiver );
static void ezbus_transceiver_handle_receiver_state_full       ( ezbus_layer1_receiver_t* layer1_receiver );


void ezbus_layer1_receiver_run ( ezbus_layer1_receiver_t* layer1_receiver )
{
	switch ( ezbus_layer1_receiver_get_state( layer1_receiver ) )
	{

		case receiver_state_empty:
			ezbus_transceiver_handle_receiver_state_empty( layer1_receiver );
			break;

		case receiver_state_full:
			ezbus_transceiver_handle_receiver_state_full( layer1_receiver );
			break;

	}
}

void ezbus_layer1_receiver_init( ezbus_layer1_receiver_t* layer1_receiver, ezbus_port_t* port, ezbus_receiver_callback_t callback, void* arg )
{
	ezbus_platform_memset( layer1_receiver, 0, sizeof(ezbus_layer1_receiver_t) );
	layer1_receiver->port     = port;
	layer1_receiver->callback = callback;
	layer1_receiver->arg      = arg;
}

void ezbus_layer1_receiver_get( ezbus_layer1_receiver_t* layer1_receiver, ezbus_packet_t* packet )
{
	ezbus_packet_copy( packet, ezbus_layer1_receiver_get_packet( layer1_receiver ) );
}




static void ezbus_transceiver_handle_receiver_state_empty( ezbus_layer1_receiver_t* layer1_receiver )
{
	if ( layer1_receiver->callback( layer1_receiver, layer1_receiver->arg ) )
	{
		ezbus_layer1_receiver_set_err( layer1_receiver, EZBUS_ERR_OKAY );
	}
}

static void ezbus_transceiver_handle_receiver_state_full( ezbus_layer1_receiver_t* layer1_receiver )
{
	if ( layer1_receiver->callback( layer1_receiver, layer1_receiver->arg ) )
	{
	}
}
