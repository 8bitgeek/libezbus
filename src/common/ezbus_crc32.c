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
#include <ezbus_types.h>
#include <ezbus_crc32.h>

#define POLYNOMIAL			0x04C11DB7
#define INITIAL_REMAINDER	0xFFFFFFFF
#define FINAL_XOR_VALUE		0xFFFFFFFF
#define REFLECT_DATA		TRUE
#define REFLECT_REMAINDER	TRUE

#define WIDTH    (8 * sizeof(uint32_t))
#define TOPBIT   (1 << (WIDTH - 1))

#if (REFLECT_DATA == TRUE)
#undef  REFLECT_DATA
#define REFLECT_DATA(X)			((unsigned char) reflect((X), 8))
#else
#undef  REFLECT_DATA
#define REFLECT_DATA(X)			(X)
#endif

#if (REFLECT_REMAINDER == TRUE)
#undef  REFLECT_REMAINDER
#define REFLECT_REMAINDER(X)	((uint32_t) reflect((X), WIDTH))
#else
#undef  REFLECT_REMAINDER
#define REFLECT_REMAINDER(X)	(X)
#endif

static unsigned long reflect(unsigned long data, unsigned char nBits)
{
	unsigned long  reflection = 0x00000000;
	unsigned char  bit;

	for (bit = 0; bit < nBits; ++bit)
	{
		if (data & 0x01)
		{
			reflection |= (1 << ((nBits - 1) - bit));
		}

		data = (data >> 1);
	}
	return reflection;
}

extern uint32_t ezbus_crc32( void* p, size_t size )
{
    const char*    message = (const char*)p;
    uint32_t       remainder = INITIAL_REMAINDER;
	int            byte;
	unsigned char  bit;

    for (byte = 0; byte < size; ++byte)
    {
        remainder ^= (REFLECT_DATA(message[byte]) << (WIDTH - 8));
        for (bit = 8; bit > 0; --bit)
        {
            if (remainder & TOPBIT)
            {
                remainder = (remainder << 1) ^ POLYNOMIAL;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }
    }
    return REFLECT_REMAINDER(remainder) ^ FINAL_XOR_VALUE;

}  
