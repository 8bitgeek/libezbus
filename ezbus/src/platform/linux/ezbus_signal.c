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

    if (signal(SIGUSR1, ezbus_signal_handler) == SIG_ERR)
        printf("\ncan't catch SIGUSR1\n");
    if (signal(SIGUSR2, ezbus_signal_handler) == SIG_ERR)
        printf("\ncan't catch SIGUSR2\n");
    if (signal(SIGINT, ezbus_signal_handler) == SIG_ERR)
        printf("\ncan't catch SIGINT\n");
}

static  void ezbus_signal_handler(int signo)
{
	switch(signo)
	{
		case SIGUSR1:
	        printf("received SIGUSR1\n");
	        break;
		case SIGUSR2:
	        printf("received SIGUSR2\n");
	        break;
		case SIGINT:
			printf("received SIGINT\n");
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
	printf("run_op_disco_start\n");

	async_state.ezbus_instance->io.tx_state.err = EZBUS_ERR_OKAY;	/* FIXME ?? */

	async_state.disco_start_time = ezbus_platform_get_ms_ticks();
	ezbus_instance_tx_disco(async_state.ezbus_instance,(ezbus_address_t)ezbus_broadcast_address,async_state.disco_seq++,packet_code_rq);
	ezbus_signal_set_op( op_disco_idle );
}

static	void run_op_disco_idle( void )
{
	if ( ezbus_platform_get_ms_ticks() - async_state.disco_start_time > 500 )
	{
		printf("run_op_disco_idle\n");
		ezbus_signal_set_op( op_idle );
	}
}

