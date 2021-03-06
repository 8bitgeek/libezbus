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
#include <ezbus_parcel.h>
#include <ezbus_platform.h>

extern void ezbus_parcel_init( ezbus_parcel_t* parcel )
{
    ezbus_platform.callback_memset( parcel, 0, sizeof(ezbus_parcel_t) );
}

extern uint16_t ezbus_parcel_get_tx_size ( ezbus_parcel_t* parcel )
{
    return sizeof(parcel->size)+parcel->size;
}

extern uint16_t ezbus_parcel_get_max( ezbus_parcel_t* parcel )
{
    return EZBUS_PARCEL_DATA_LN;
}

extern uint16_t ezbus_parcel_get_size( ezbus_parcel_t* parcel )
{
    return parcel->size;
}

extern void* ezbus_parcel_get_ptr( ezbus_parcel_t* parcel )
{
    return parcel->bytes;
}

extern uint16_t ezbus_parcel_get_data( ezbus_parcel_t* parcel, void* data )
{
    ezbus_platform.callback_memcpy( data, parcel->bytes, parcel->size );
    return parcel->size;
}

extern uint16_t ezbus_parcel_get_string( ezbus_parcel_t* parcel, char* string )
{
    ezbus_platform.callback_strncpy( string, ezbus_parcel_get_ptr( parcel ), ezbus_parcel_get_max( parcel ) );
    return ezbus_platform.callback_strlen( string );
}

extern uint16_t ezbus_parcel_set_size( ezbus_parcel_t* parcel, uint16_t size )
{
    return (parcel->size = size);
}

extern uint16_t ezbus_parcel_set_data( ezbus_parcel_t* parcel, const void* data, uint16_t size )
{
    parcel->size = size <= ezbus_parcel_get_max( parcel ) ? size : ezbus_parcel_get_max( parcel );
    ezbus_platform.callback_memcpy( parcel->bytes, data, parcel->size );
    return parcel->size;
}

extern uint16_t ezbus_parcel_set_string( ezbus_parcel_t* parcel, const char* string )
{
    parcel->size = ezbus_platform.callback_strlen(string) < ezbus_parcel_get_max( parcel ) ? ezbus_platform.callback_strlen(string) : ezbus_parcel_get_max( parcel );
    ezbus_platform.callback_memset( parcel->bytes, 0, parcel->size );
    ezbus_platform.callback_strncpy( (char*)parcel->bytes, string, parcel->size );
    return parcel->size;
}

extern void ezbus_parcel_copy( ezbus_parcel_t* dst, ezbus_parcel_t* src )
{
    ezbus_platform.callback_memcpy( dst, src, ezbus_parcel_get_tx_size(src) );
}

