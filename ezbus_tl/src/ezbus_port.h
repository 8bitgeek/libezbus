/******************************************************************************
 * Copyright © 2018 by @author Mike Sharkey <mike@8bitgeek.net>
 * All Rights Reserved
 *****************************************************************************/
#ifndef EZBUS_PORT_H_
#define EZBUS_PORT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ezbus_platform.h"
#include "ezbus_packet.h"

extern uint32_t ezbus_port_speeds[EZBUS_SPEED_COUNT];

typedef struct
{
	ezbus_platform_port_t	platform_port;
	uint32_t				speed;
	uint32_t				packet_timeout;
	uint32_t				rx_err_crc_count;
	uint32_t				rx_err_timeout_count;
	uint32_t				rx_err_overrun_count;
	uint32_t				tx_err_overrun_count;
	uint32_t				tx_err_retry_fail_count;
} ezbus_port_t;

extern EZBUS_ERR	ezbus_port_open                  (ezbus_port_t* port,uint32_t speed);
extern EZBUS_ERR	ezbus_port_send                  (ezbus_port_t* port,ezbus_packet_t* packet);
extern EZBUS_ERR	ezbus_port_recv                  (ezbus_port_t* port,ezbus_packet_t* packet);
extern int			ezbus_port_getch                 (ezbus_port_t* port);
extern void			ezbus_port_close                 (ezbus_port_t* port);
extern void			ezbus_port_drain                 (ezbus_port_t* port);
extern void			ezbus_port_set_speed             (ezbus_port_t* port,uint32_t speed);
extern uint32_t		ezbus_port_get_speed             (ezbus_port_t* port);
extern uint32_t		ezbus_port_byte_time_ns          (ezbus_port_t* port);
extern uint32_t 	ezbus_port_packet_timeout_time_ms(ezbus_port_t* port);
extern void 		ezbus_port_dump                  (ezbus_port_t* port, const char* prefix);

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_PORT_H_ */
