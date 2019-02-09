/******************************************************************************
 * Copyright © 2018 by @author Mike Sharkey <mike@8bitgeek.net>
 * All Rights Reserved
 *****************************************************************************/
#ifndef EZBUS_INSTANCE_H_
#define EZBUS_INSTANCE_H_

#include "ezbus_platform.h"
#include "ezbus_port.h"
#include "ezbus_packet.h"
#include "ezbus_address.h"
#include "ezbus_packet_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	ezbus_packet_t	packet;
	EZBUS_ERR		err;
	uint8_t			seq;
} ezbus_packet_state_t;

typedef struct
{
	uint8_t					address[EZBUS_ADDR_LN];	/* self address */
	ezbus_port_t			port;					/* ezbus active port */
	ezbus_packet_state_t	rx_state;				/* rx packet state >0 == rx ready */
	ezbus_packet_state_t	tx_state;				/* tx packet state >0 == rx ready */
	ezbus_packet_queue_t*	tx_queue;				/* transmitter queue */
	uint8_t					disco_seq;				/* discovery sequence # */
	ezbus_address_list_t	peers;					/* peer list */
} ezbus_packet_io_t;

typedef void (*ezbus_packet_callback_t)(ezbus_packet_io_t*);

typedef struct
{
	ezbus_packet_io_t			io;					/* I/O state */
	ezbus_packet_callback_t		rx_callback;		/* Application RX callback */
} ezbus_instance_t;

extern void			ezbus_instance_run			( ezbus_instance_t* instance );
extern void 		ezbus_instance_init_struct	( ezbus_instance_t* instance );
extern EZBUS_ERR	ezbus_instance_init			( ezbus_instance_t* instance, uint32_t speed, uint32_t tx_queue_limit );
extern void			ezbus_instance_deinit		( ezbus_instance_t* instance );

/* ******************** PACKET HANDLERS *************************************/

extern void ezbus_instance_tx_disco			( ezbus_instance_t* instance, ezbus_address_t dst, uint8_t seq, ezbus_packet_code_t code );
extern void ezbus_instance_tx_give_token	( ezbus_instance_t* instance, ezbus_address_t dst );
extern void ezbus_instance_tx_take_token	( ezbus_instance_t* instance, ezbus_address_t dst );
extern void ezbus_instance_tx_ack			( ezbus_instance_t* instance, ezbus_address_t dst );
extern void ezbus_instance_tx_parcel		( ezbus_instance_t* instance, ezbus_address_t dst );
extern void ezbus_instance_tx_reset			( ezbus_instance_t* instance, ezbus_address_t dst );
extern void ezbus_instance_tx_speed			( ezbus_instance_t* instance, ezbus_address_t dst );

extern void ezbus_instance_rx_disco			( ezbus_instance_t* instance );
extern void ezbus_instance_rx_give_token	( ezbus_instance_t* instance );
extern void ezbus_instance_rx_take_token	( ezbus_instance_t* instance );
extern void ezbus_instance_rx_ack			( ezbus_instance_t* instance );
extern void ezbus_instance_rx_parcel		( ezbus_instance_t* instance );
extern void ezbus_instance_rx_reset			( ezbus_instance_t* instance );
extern void ezbus_instance_rx_speed			( ezbus_instance_t* instance );

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_INSTANCE_H_ */
