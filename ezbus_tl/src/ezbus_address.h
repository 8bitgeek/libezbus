/******************************************************************************
 * Copyright © 2018 by @author Mike Sharkey <mike@8bitgeek.net>
 * All Rights Reserved
 *****************************************************************************/
#ifndef EZBUS_ADDRESS_H_
#define EZBUS_ADDRESS_H_

#include "ezbus_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	ezbus_address_t**	list;
	uint8_t				count;
} ezbus_address_list_t;


extern const ezbus_address_t ezbus_broadcast_address;
extern const ezbus_address_t ezbus_controller_address;

extern int 		ezbus_address_compare   ( const ezbus_address_t* a, const ezbus_address_t* b );
extern uint8_t* ezbus_address_copy      ( ezbus_address_t* dst, const ezbus_address_t* src );
extern void		ezbus_address_swap      ( ezbus_address_t* dst, ezbus_address_t* src );
extern char*	ezbus_address_string    ( ezbus_address_t* address, char* string );
extern void 	ezbus_address_dump 		( ezbus_address_t* address, const char* prefix );

extern void			ezbus_address_list_init		( ezbus_address_list_t* address_list );
extern void			ezbus_address_list_deinit	( ezbus_address_list_t* address_list );
extern EZBUS_ERR 	ezbus_address_list_append	( ezbus_address_list_t* address_list, const ezbus_address_t* address );
extern EZBUS_ERR	ezbus_address_list_take		( ezbus_address_list_t* address_list, ezbus_address_t* address );
extern EZBUS_ERR 	ezbus_address_list_at		( ezbus_address_list_t* address_list, ezbus_address_t* address, int index );
extern int			ezbus_address_list_count	( ezbus_address_list_t* address_list );
extern int			ezbus_address_list_empty	( ezbus_address_list_t* address_list );
extern int			ezbus_address_list_lookup	( ezbus_address_list_t* address_list, const ezbus_address_t* address );
extern void 		ezbus_address_list_dump     ( ezbus_address_list_t* address_list, const char* prefix );

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_ADDRESS_H_ */
