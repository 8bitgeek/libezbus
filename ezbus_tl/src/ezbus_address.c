/******************************************************************************
 * Copyright © 2018 by @author Mike Sharkey <mike@8bitgeek.net>
 * All Rights Reserved
 *****************************************************************************/
#include "ezbus_address.h"

const uint8_t ezbus_broadcast_address[EZBUS_ADDR_LN] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
const uint8_t ezbus_controller_address[EZBUS_ADDR_LN] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

/**
 * @brief Compare address a vs b
 * @return <, =, or > 0
 */
int ezbus_address_compare( const ezbus_address_t a, const ezbus_address_t b )
{
	return ezbus_platform_memcmp(a,b,EZBUS_ADDR_LN);
}

uint8_t* ezbus_address_copy( ezbus_address_t dst, const ezbus_address_t src )
{
	return ezbus_platform_memcpy(dst,src,EZBUS_ADDR_LN);
}

void ezbus_address_swap( ezbus_address_t dst, ezbus_address_t src )
{
	uint8_t tmp[EZBUS_ADDR_LN];
	ezbus_address_copy(tmp,dst);
	ezbus_address_copy(dst,src);
	ezbus_address_copy(src,tmp);
}

void ezbus_address_list_init(ezbus_address_list_t* address_list)
{
	if ( address_list )
	{
		ezbus_platform_memset(address_list,0,sizeof(ezbus_address_list_t));
	}
}

void ezbus_address_list_deinit( ezbus_address_list_t* address_list)
{
	if ( address_list )
	{
		uint8_t address[EZBUS_ADDR_LN];
		while(ezbus_address_list_count(address_list)>0)
			ezbus_address_list_take(address_list,address);
		ezbus_platform_memset(address_list,0,sizeof(ezbus_address_list_t));
	}
}

EZBUS_ERR ezbus_address_list_append( ezbus_address_list_t* address_list, const ezbus_address_t address )
{
	EZBUS_ERR err=EZBUS_ERR_OKAY;
	if ( address_list != NULL )
	{
		if ( ezbus_address_list_lookup( address_list, address ) < 0 )
		{
			uint8_t** taddress_list = address_list->list;
			address_list->list = (uint8_t**)ezbus_platform_realloc(address_list->list,sizeof(uint8_t**)*(address_list->count+1));
			if ( address_list->list != NULL )
			{
				uint8_t* taddress = ezbus_platform_malloc(EZBUS_ADDR_LN);
				if ( taddress != NULL )
				{
					/* Take a copy of the packet and increment the count */
					ezbus_platform_memcpy(taddress,address,EZBUS_ADDR_LN);
					address_list->list[address_list->count++] = taddress;
					err=EZBUS_ERR_OKAY;
				}
				else
				{
					/* Remove the list allocation */
					address_list->list = (uint8_t**)ezbus_platform_realloc(address_list->list,sizeof(uint8_t**)*(address_list->count));
					err = EZBUS_ERR_MALLOC;
				}
			}
			else
			{
				/* realloc() failed, let's don't leak */
				address_list->list = taddress_list;
				err = EZBUS_ERR_MALLOC;
			}
		}
		else
		{
			err = EZBUS_ERR_DUP;
		}
	}
	else
	{
		err = EZBUS_ERR_PARAM;
	}
	return err;
}

EZBUS_ERR ezbus_address_list_take( ezbus_address_list_t* address_list, ezbus_address_t address )
{
	EZBUS_ERR err=EZBUS_ERR_OKAY;
	if ( address_list != NULL )
	{
		if ( !ezbus_address_list_empty(address_list) )
		{
			uint8_t** taddress_list = address_list->list;
			uint8_t* taddress = taddress_list[--address_list->count];
			ezbus_platform_memcpy(address,taddress,EZBUS_ADDR_LN);
			ezbus_platform_free(taddress);
			address_list->list = (uint8_t**)ezbus_platform_realloc(address_list->list,sizeof(uint8_t**)*(address_list->count));
			if ( address_list->list != NULL )
			{
				err=EZBUS_ERR_OKAY;
			}
			else
			{
				/* realloc() failed, let's don't leak */
				address_list->list = taddress_list;
				err = EZBUS_ERR_MALLOC;
			}
		}
		else
		{
			err = EZBUS_ERR_LIMIT;
		}
	}
	else
	{
		err = EZBUS_ERR_PARAM;
	}
	return err;
}

EZBUS_ERR ezbus_address_list_at( ezbus_address_list_t* address_list, ezbus_address_t address, int index )
{
	EZBUS_ERR err = EZBUS_ERR_OKAY;
	if ( index > 0 && index < address_list->count )
	{
		ezbus_platform_memcpy(address,address_list->list[index],EZBUS_ADDR_LN);
	}
	else
	{
		err = EZBUS_ERR_RANGE;
	}
	return err;
}

int ezbus_address_list_count( ezbus_address_list_t* address_list )
{
	return (address_list->count);
}

int ezbus_address_list_empty( ezbus_address_list_t* address_list )
{
	return (address_list->count==0);
}

int ezbus_address_list_lookup( ezbus_address_list_t* address_list, ezbus_address_t  address )
{
	for(int index=0; index < address_list->count; index++)
	{
		if ( ezbus_address_compare(address_list->list[index],address) == 0 )
		{
			return index;
		}
	}
	return -1;
}




