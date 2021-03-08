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
#include <ezbus_mac_receiver.h>
#include <ezbus_hex.h>
#include <ezbus_log.h>
#include <ezbus_fault.h>
#include <ezbus_platform.h>

#define ezbus_mac_receiver_empty(mac_receiver)     (ezbus_mac_receiver_get_state((mac_receiver))==receiver_state_empty)
#define ezbus_mac_receiver_full(mac_receiver)      (ezbus_mac_receiver_get_state((mac_receiver))!=receiver_state_empty)

static void do_receiver_state_empty     	 	   ( ezbus_mac_t* mac );
static void do_receiver_state_full      	 	   ( ezbus_mac_t* mac );
static void do_receiver_state_receive_fault   	   ( ezbus_mac_t* mac );

extern void ezbus_mac_receiver_run ( ezbus_mac_t* mac )
{
	switch ( ezbus_mac_receiver_get_state( mac ) )
	{

		case receiver_state_empty:
			do_receiver_state_empty( mac );
			break;

		case receiver_state_full:
			do_receiver_state_full( mac );
			break;

		case receiver_state_receive_fault:
			do_receiver_state_receive_fault( mac );
			break;
	}
}

extern void ezbus_mac_receiver_init( ezbus_mac_t* mac )
{
	ezbus_mac_receiver_t* receiver = ezbus_mac_get_receiver( mac );
	ezbus_platform.callback_memset( receiver, 0, sizeof(ezbus_mac_receiver_t) );
}

extern void ezbus_mac_receiver_set_state( ezbus_mac_t* mac, ezbus_receiver_state_t state )
{
	ezbus_mac_receiver_t* receiver = ezbus_mac_get_receiver( mac );
	receiver->state = state;
}

extern ezbus_receiver_state_t ezbus_mac_receiver_get_state( ezbus_mac_t* mac )
{
	ezbus_mac_receiver_t* receiver = ezbus_mac_get_receiver( mac );
	return receiver->state;
}

extern void ezbus_mac_receiver_set_err( ezbus_mac_t* mac, EZBUS_ERR err )
{
	ezbus_mac_receiver_t* receiver = ezbus_mac_get_receiver( mac );
	receiver->err = err;

	if ( err != EZBUS_ERR_OKAY && err != EZBUS_ERR_NOTREADY )
	{
		EZBUS_LOG( EZBUS_LOG_RECEIVER_ERR, "%s", ezbus_fault_str(err) );
	}
}

extern EZBUS_ERR ezbus_mac_receiver_get_err( ezbus_mac_t* mac )
{
	ezbus_mac_receiver_t* receiver = ezbus_mac_get_receiver( mac );
	return receiver->err;
}



extern void ezbus_mac_receiver_get( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
	ezbus_packet_copy( packet, ezbus_mac_get_receiver_packet( mac ) );
}


static void do_receiver_state_empty( ezbus_mac_t* mac )
{
	ezbus_mac_receiver_signal_empty( mac );
	ezbus_mac_receiver_set_err( mac, ezbus_port_recv( ezbus_mac_get_port( mac ), ezbus_mac_get_receiver_packet( mac ) ) );
	
	if ( ezbus_mac_receiver_get_err( mac ) == EZBUS_ERR_OKAY )
	{
		ezbus_mac_receiver_set_state( mac, receiver_state_full );
	}
	else
	{
		ezbus_mac_receiver_set_state( mac, receiver_state_receive_fault );
	}
}

static void do_receiver_state_receive_fault( ezbus_mac_t* mac )
{
	ezbus_mac_receiver_signal_fault( mac );

	ezbus_mac_receiver_set_err( mac, EZBUS_ERR_OKAY );
	ezbus_mac_receiver_set_state( mac, receiver_state_empty );
    ezbus_packet_init( ezbus_mac_get_receiver_packet( mac ) );
}

static void do_receiver_state_full( ezbus_mac_t* mac )
{
	ezbus_mac_receiver_set_state( mac, receiver_state_empty );
	ezbus_mac_receiver_signal_full( mac );
}

