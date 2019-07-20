/******************************************************************************
 * Copyright © 2018 by @author Mike Sharkey <mike@8bitgeek.net>
 * All Rights Reserved
 *****************************************************************************/
#include <ezbus_hex.h>
#include <caribou/lib/stdio.h>

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

extern void ezbus_hex_dump(char* tag, uint8_t* data, uint32_t size)
{
	printf("%s",tag);
	for(int n=0; n < size; n++ )
	{
		char hex[3];
		ezbus_hex8(data[n],hex);
		printf("%s",hex);
	}
	printf("\n");
}
