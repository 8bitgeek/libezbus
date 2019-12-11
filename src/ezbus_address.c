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

extern char* ezbus_address_string( ezbus_address_t* address )
{
    static char string[ EZBUS_ADDR_LN_STR ];
    for(int n=0; n < EZBUS_ADDR_LN; n++)
    {
        ezbus_hex8(address->byte[n],&string[n*2]);
    }
    string[EZBUS_ADDR_LN_STR-1]='\0';
    return string;
}

extern void ezbus_address_dump( const ezbus_address_t* address, const char* prefix )
{
    fprintf(stderr, "%s=", prefix );
    for(int n=0; n < EZBUS_ADDR_LN; n++)
    {
        fprintf(stderr, "%02X", address->byte[n] );
    }
    fprintf(stderr, "\n" );
}

