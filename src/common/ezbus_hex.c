/*****************************************************************************
* Copyright © 2019-2020 Mike Sharkey <mike.sharkey@mineairquality.com>       *
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
#include <ezbus_log.h>

extern void ezbus_hex4(uint8_t nybble, char* hex)
{
  nybble &= 0x0F;
  if ( nybble < 10 )
    *hex++ = '0'+nybble;
  else
    *hex++ = 'A'+(nybble-10);
  *hex='\0';
}

extern void ezbus_hex8(uint8_t byte, char* hex)
{
  ezbus_hex4(byte>>4,hex++);
  ezbus_hex4(byte,hex++);
  *hex='\0';
}

extern void ezbus_hex16(uint16_t word, char* hex )
{
  ezbus_hex8(word>>8,hex);  hex+=2;
  ezbus_hex8(word,hex);     hex+=2;
  *hex='\0';
}

extern void ezbus_hex32(uint32_t word, char* hex )
{
  ezbus_hex8(word>>24,hex); hex+=2;
  ezbus_hex8(word>>16,hex); hex+=2;
  ezbus_hex8(word>>8,hex);  hex+=2;
  ezbus_hex8(word,hex);     hex+=2;
  *hex='\0';
}

extern void ezbus_hex_dump(char* tag, void* data, uint32_t size)
{
    uint8_t* p = (uint8_t*)data;
    fprintf( stderr, "%s", tag);
    for(int n=0; n < size; n++ )
    {
        fprintf( stderr, "%02X", p[n] );
    }
    fprintf( stderr, "\r\n" );
}
