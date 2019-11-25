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


extern const char* ezbus_string_str( ezbus_string_t* string )
{
  return string->chars;
}

extern const char* ezbus_string_length( ezbus_string_t* string )
{
  if ( string->chars )
    return ezbus_platform_strlen( string->chars );

  return 0;
}

extern const char* ezbus_string_set_length( ezbus_string_t* string, uint16_t len )
{
  string->chars = (char*)ezbus_platform_realloc(string->chars,len+1);
  ezbus_platform_memset(string->chars,' ',len);
  string->chars[len]='\0';
}

extern const char* ezbus_string_copy( ezbus_string_t* dst, ezbus_string_t* src )
{
  
  dst->chars = (char*)ezbus_platform_realloc(dst->chars,ezbus_string_length(src)+1);

}

extern const char* ezbus_string_cat( ezbus_string_t* dst, ezbus_string_t* src )
{

}


extern const char* ezbus_string_left( ezbus_string_t* dst, ezbus_string_t* src, uint16_t cnt )
{

}

extern const char* ezbus_string_right( ezbus_string_t* dst, ezbus_string_t* src, uint16_t cnt )
{

}


extern void ezbus_string_import_str( ezbus_string_t* string, const char* str )
{

}

extern void ezbus_string_export_str( ezbus_string_t* string, const char* str, uint16_t max )
{

}


