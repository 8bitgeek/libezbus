/******************************************************************************
 * Copyright © 2018 by @author Mike Sharkey <mike@8bitgeek.net>
 * All Rights Reserved
 *****************************************************************************/
#include <ezbus_platform.h>
#include <ezbus_address.h>
#include <ezbus_hex.h>

const ezbus_address_t ezbus_broadcast_address = 
{
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
};

const ezbus_address_t ezbus_controller_address = 
{
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}
};

/**
 * @brief Compare address a vs b
 * @return <, =, or > 0
 */
int ezbus_address_compare( const ezbus_address_t* a, const ezbus_address_t* b )
{
	return ezbus_platform_memcmp(a,b,sizeof(ezbus_address_t));
}

uint8_t* ezbus_address_copy( ezbus_address_t* dst, const ezbus_address_t* src )
{
	return ezbus_platform_memcpy(dst,src,sizeof(ezbus_address_t));
}

void ezbus_address_swap( ezbus_address_t* dst, ezbus_address_t* src )
{
	ezbus_address_t tmp;
	ezbus_address_copy(&tmp,dst);
	ezbus_address_copy(dst,src);
	ezbus_address_copy(src,&tmp);
}

extern char* ezbus_address_string( ezbus_address_t* address, char* string )
{
	for(int n=0; n < sizeof(ezbus_address_t); n++)
	{
		ezbus_hex8(address->byte[n],&string[n*2]);
	}
	string[sizeof(ezbus_address_t)]='\0';
	return string;
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
		ezbus_address_t address;
		while(ezbus_address_list_count(address_list)>0)
			ezbus_address_list_take(address_list,&address);
		ezbus_platform_memset(address_list,0,sizeof(ezbus_address_list_t));
	}
}

EZBUS_ERR ezbus_address_list_append( ezbus_address_list_t* address_list, const ezbus_address_t* address )
{
	EZBUS_ERR err=EZBUS_ERR_OKAY;
	if ( address_list != NULL )
	{
		if ( ezbus_address_list_lookup( address_list, address ) < 0 )
		{
			ezbus_address_t** taddress_list = address_list->list;
			address_list->list = (ezbus_address_t**)ezbus_platform_realloc(address_list->list,sizeof(ezbus_address_t**)*(address_list->count+1));
			if ( address_list->list != NULL )
			{
				ezbus_address_t* taddress = ezbus_platform_malloc(sizeof(ezbus_address_t));
				if ( taddress != NULL )
				{
					/* Take a copy of the packet and increment the count */
					ezbus_platform_memcpy(taddress,&address,sizeof(ezbus_address_t));
					address_list->list[address_list->count++] = taddress;
					err=EZBUS_ERR_OKAY;
				}
				else
				{
					/* Remove the list allocation */
					address_list->list = (ezbus_address_t**)ezbus_platform_realloc(address_list->list,sizeof(ezbus_address_t**)*(address_list->count));
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

EZBUS_ERR ezbus_address_list_take( ezbus_address_list_t* address_list, ezbus_address_t* address )
{
	EZBUS_ERR err=EZBUS_ERR_OKAY;
	if ( address_list != NULL )
	{
		if ( !ezbus_address_list_empty(address_list) )
		{
			ezbus_address_t** taddress_list = address_list->list;
			ezbus_address_t* taddress = taddress_list[--address_list->count];
			ezbus_platform_memcpy(address,taddress,sizeof(ezbus_address_t));
			ezbus_platform_free(taddress);
			address_list->list = (ezbus_address_t**)ezbus_platform_realloc(address_list->list,sizeof(ezbus_address_t**)*(address_list->count));
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

EZBUS_ERR ezbus_address_list_at( ezbus_address_list_t* address_list, ezbus_address_t* address, int index )
{
	EZBUS_ERR err = EZBUS_ERR_OKAY;
	if ( index > 0 && index < address_list->count )
	{
		ezbus_platform_memcpy(address,address_list->list[index],sizeof(ezbus_address_t));
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

int ezbus_address_list_lookup( ezbus_address_list_t* address_list, const ezbus_address_t*  address )
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

extern void ezbus_address_dump( ezbus_address_t* address, const char* prefix )
{
	printf( "%s=", prefix );
	for(int n=0; n < EZBUS_ADDR_LN; n++)
	{
		printf( "%02X", address->byte[n] );
	}
	printf( "\n" );
}

extern void ezbus_address_list_dump( ezbus_address_list_t* address_list, const char* prefix )
{
	char print_buffer[EZBUS_TMP_BUF_SZ];
	for(int index=0; index < address_list->count; index++)
	{
		ezbus_address_t* address = address_list->list[index];
		sprintf(print_buffer,"%s[%d]", prefix, index );
		ezbus_address_dump( address, print_buffer );
	}
}
