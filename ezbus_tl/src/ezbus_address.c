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
	for(int n=0; n < EZBUS_ADDR_LN; n++)
	{
		ezbus_hex8(address->byte[n],&string[n*2]);
	}
	string[EZBUS_ADDR_LN*2]='\0';
	return string;
}

#if 0
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
	EZBUS_ERR err = EZBUS_ERR_OKAY;
	if ( address_list != NULL )
	{
		if ( ezbus_address_list_lookup( address_list, address ) < 0 )
		{
			address_list->list = (ezbus_address_t**)ezbus_platform_realloc( address_list->list, sizeof(ezbus_address_t**)*(address_list->count+1) );
			if ( address_list->list != NULL )
			{
				address_list->list[ address_list->count ] = ezbus_platform_malloc( sizeof(ezbus_address_t) );
				if ( address_list->list[ address_list->count ] != NULL )
				{
					/* Take a copy of the packet and increment the count */
					ezbus_platform_memcpy( address_list->list[ address_list->count ], address, sizeof(ezbus_address_t) );
					++address_list->count;
					err=EZBUS_ERR_OKAY;
				}
				else
				{
					/* Remove the list allocation */
					address_list->list = (ezbus_address_t**)ezbus_platform_realloc( address_list->list, sizeof(ezbus_address_t**)*(address_list->count) );
					err = EZBUS_ERR_MALLOC;
				}
			}
			else
			{
				address_list->count = 0;
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
			ezbus_address_copy(address,taddress);
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
	if ( index >= 0 && index < ezbus_address_list_count(address_list) )
	{
		ezbus_address_copy(address,address_list->list[index]);
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

extern void ezbus_address_list_dump( ezbus_address_list_t* address_list, const char* prefix )
{
	char print_buffer[EZBUS_TMP_BUF_SZ];
	fprintf(stderr, "%s.count=%d\n", prefix, address_list->count );
	for(int index=0; index < address_list->count; index++)
	{
		ezbus_address_t* address = address_list->list[index];
		sprintf(print_buffer,"%s[%d]", prefix, index );
		ezbus_address_dump( address, print_buffer );
	}
	fflush(stderr);
}
#endif

extern void ezbus_address_dump( const ezbus_address_t* address, const char* prefix )
{
	fprintf(stderr, "%s=", prefix );
	for(int n=0; n < EZBUS_ADDR_LN; n++)
	{
		fprintf(stderr, "%02X", address->byte[n] );
	}
	fprintf(stderr, "\n" );
}

