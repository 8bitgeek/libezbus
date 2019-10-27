/******************************************************************************
 * Copyright © 2018 by @author Mike Sharkey <mike@8bitgeek.net>
 * All Rights Reserved
 *****************************************************************************/
#include "ezbus_signal.h"
#include <ezbus_address.h>
#include <ezbus_platform.h>
#include <string.h>

typedef enum
{
	op_idle,
	op_disco_start,
	op_disco_idle,
} op_state_t;

typedef struct
{
	ezbus_ms_tick_t		disco_start_time;
	ezbus_instance_t*	ezbus_instance;
	op_state_t			op_state;
	uint8_t 			disco_seq;
} async_state_t;

async_state_t async_state;

static	void ezbus_signal_set_op( op_state_t op_state );
static  void ezbus_signal_handler(int signo);
static	void run_op_disco_start( void );
static	void run_op_disco_idle( void );

extern void ezbus_signal_init( ezbus_instance_t* ezbus_instance )
{
	memset(&async_state,0,sizeof(async_state_t));

	async_state.ezbus_instance = ezbus_instance;

    signal( SIGUSR1, ezbus_signal_handler );
    signal( SIGUSR2, ezbus_signal_handler );
    signal( SIGINT , ezbus_signal_handler );
}

static void ezbus_signal_handler(int signo)
{
	switch(signo)
	{
		case SIGUSR1:
			exit(0);
	        break;
		case SIGUSR2:
	        break;
		case SIGINT:
	        ezbus_signal_set_op( op_disco_start );
			break;
		default:
			break;
	}
}

static void ezbus_signal_set_op( op_state_t op_state )
{
	async_state.op_state = op_state;
}


extern void ezbus_signal_run(void)
{
	switch( async_state.op_state )
	{
		case op_idle:
			break;
		case op_disco_start:
			run_op_disco_start();
			break;
		case op_disco_idle:
			run_op_disco_idle();
	}
}

static	void run_op_disco_start( void )
{
	async_state.ezbus_instance->io.tx_state.err = EZBUS_ERR_OKAY;	/* FIXME ?? */

	async_state.disco_start_time = ezbus_platform_get_ms_ticks();
	ezbus_instance_tx_disco_rq( async_state.ezbus_instance, &ezbus_broadcast_address, async_state.disco_seq++, packet_code_rq );
	ezbus_signal_set_op( op_disco_idle );
}

static	void run_op_disco_idle( void )
{
	if ( ezbus_platform_get_ms_ticks() - async_state.disco_start_time > 500 )
	{
		for(int n=0; n < ezbus_address_list_count( &async_state.ezbus_instance->io.peers ); n++  )
		{
			char address_string[(EZBUS_ADDR_LN*2)+1];
			ezbus_address_t address;
			
			ezbus_address_list_at(&async_state.ezbus_instance->io.peers,&address,n);

			printf( ">>%s\n", ezbus_address_string(&address,address_string));;
		}
		ezbus_signal_set_op( op_idle );
	}
}

