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
#include <ezbus_hex.h>
#if defined(_CARIBOU_RTOS_)
  #include <caribou/lib/stdio.h>
#else
  #include <stdio.h>
#endif

extern void ezbus_string_init( ezbus_string_t* string )
{
  ezbus_platform_memset(string,0,sizeof(ezbus_string_t));
}

extern void ezbus_string_deinit( ezbus_string_t* string )
{
  if ( string->chars )
    ezbus_plarform_free(string->chars);
}


extern char* ezbus_string_str( ezbus_string_t* string )
{
  return string->chars;
}

extern int ezbus_string_length( ezbus_string_t* string )
{
  if ( string->chars )
    return ezbus_platform_strlen( string->chars );

  return 0;
}

extern void ezbus_string_set_length( ezbus_string_t* string, uint16_t len )
{
  string->chars = (char*)ezbus_platform_realloc(string->chars,len+1);
  ezbus_platform_memset(string->chars,' ',len);
  string->chars[len]='\0';
}

extern char* ezbus_string_copy( ezbus_string_t* dst, ezbus_string_t* src )
{
  ezbus_string_set_length( dst, ezbus_string_length(src) );
  ezbus_platform_strcpy(dst->chars,src->chars);
  return ezbus_string_str( dst );
}

extern char* ezbus_string_cat( ezbus_string_t* dst, ezbus_string_t* src )
{
  int len = ezbus_string_length( dst ) + ezbus_string_length( src );
  dst->chars = (char*)ezbus_platform_realloc(dst->chars,len+1);
  ezbus_platform_strcat(dst->chars,src->chars);
  return ezbus_string_str( dst );
}


extern char* ezbus_string_left( ezbus_string_t* dst, ezbus_string_t* src, uint16_t cnt )
{
  if ( ezbus_string_length( src ) <= cnt )
    return ezbus_string_copy( dst, src );
  ezbus_string_set_length( dst, cnt );
  ezbus_platform_strncpy( dst->chars, src->chars, cnt );
  return ezbus_string_str( dst );
}

extern char* ezbus_string_right( ezbus_string_t* dst, ezbus_string_t* src, uint16_t cnt )
{
  if ( ezbus_string_length( src ) <= cnt )
    return ezbus_string_copy( dst, src );
  ezbus_string_set_length( dst, cnt );
  ezbus_platform_strncpy( dst->chars, &src->chars[ezbus_string_length(src)-cnt], cnt );
  return ezbus_string_str( dst );
}


extern void ezbus_string_from_c( ezbus_string_t* string, const char* cstr )
{
  int cstr_strlen = ezbus_platform_strlen( cstr );
  ezbus_string_set_length( string, cstr_strlen );
  ezbus_platform_strncpy( string->chars, cstr, cstr_strlen );
}

extern void ezbus_string_to_c( ezbus_string_t* string, const char* cstr, uint16_t cstr_max_strlen )
{
  ezbus_platform_strncpy( cstr, string->chars, cstr_max_strlen );
}


