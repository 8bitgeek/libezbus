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
#include <ezbus_driver_packet.h>
#include <ezbus_hex.h>

extern void ezbus_driver_packet_tx_reset( ezbus_driver_t* driver )
{

}

extern void ezbus_driver_packet_tx_speed( ezbus_driver_t* driver )
{

}

extern void ezbus_driver_packet_tx_give_token( ezbus_driver_t* driver, const ezbus_address_t* dst )
{

}

extern void ezbus_driver_packet_tx_take_token( ezbus_driver_t* driver, const ezbus_address_t* dst )
{

}

extern void ezbus_driver_packet_tx_ack( ezbus_driver_t* driver, const ezbus_address_t* dst )
{

}

extern void ezbus_driver_packet_tx_nack( ezbus_driver_t* driver, const ezbus_address_t* dst )
{

}

extern void ezbus_driver_packet_tx_parcel( ezbus_driver_t* driver, const ezbus_address_t* dst, ezbus_parcel_t* parcel )
{
    if ( ezbus_driver_tx_empty( driver ) )
    {
        ezbus_packet_t* tx_packet = &ezbus_driver_tx_state(driver)->packet;

        ezbus_packet_set_type( ezbus_driver_tx_state, packet_type_parcel );
        ezbus_address_copy( ezbus_packet_src( tx_packet ), &driver->io.address );
        ezbus_address_copy( ezbus_packet_dst( tx_packet ), dst );

        ezbus_parcel_copy( &tx_packet->data.attachment.parcel, parcel );
 
        ezbus_driver_tx_enqueue( driver );
    }
    else
    {
        ezbus_driver_tx_set_err(driver,EZBUS_ERR_OVERFLOW)
    }
}

