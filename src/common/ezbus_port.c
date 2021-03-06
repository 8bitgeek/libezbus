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
#include <ezbus_port.h>
#include <ezbus_packet.h>
#include <ezbus_hex.h>
#include <ezbus_log.h>
#include <ezbus_platform.h>

static int ezbus_private_recv(ezbus_port_t* port, void* buf, uint32_t index, size_t size);
static int ezbus_seek_leadin(ezbus_port_t* port);

extern void ezbus_port_init_struct( ezbus_port_t* port )
{
    ezbus_platform.callback_memset( port, 0, sizeof(ezbus_port_t) );
}

extern EZBUS_ERR ezbus_port_open( ezbus_port_t* port )
{
    if ( port->callback_open( port ) == 0 )
    {
        port->packet_timeout = ezbus_port_packet_timeout_time_ms(port);
        port->rx_err_crc_count = 0;
        port->rx_err_timeout_count = 0;
        port->rx_err_overrun_count = 0;
        port->tx_err_overrun_count = 0;
        return EZBUS_ERR_OKAY;
    }
    return EZBUS_ERR_IO;
}

extern EZBUS_ERR ezbus_port_send( ezbus_port_t* port, ezbus_packet_t* packet )
{
    EZBUS_ERR err        = EZBUS_ERR_OKAY;
    size_t bytes_to_send = ezbus_packet_tx_size ( packet );
    size_t bytes_sent;

    packet->header.data.field.mark = EZBUS_MARK;

    ezbus_packet_calc_crc ( packet );
    ezbus_packet_flip     ( packet );

    bytes_sent = port->callback_send( port, packet, bytes_to_send );
    
    ezbus_packet_dump( "TX:", packet, bytes_to_send );

    err = ( bytes_to_send == bytes_sent ) ? EZBUS_ERR_OKAY : EZBUS_ERR_IO;

    ezbus_packet_flip ( packet );

    return err;
}


static int ezbus_private_recv( ezbus_port_t* port, void* buf, uint32_t index, size_t size )
{
    register int ch;
    register uint8_t* p = (uint8_t*)buf;
    ezbus_ms_tick_t start = ezbus_platform.callback_get_ms_ticks();
    /* receive the entire header or timeout... */
    while ( index < size && (ezbus_platform.callback_get_ms_ticks() - start) <= port->packet_timeout )
    {
        if ( (ch = ezbus_port_getch(port)) >= 0 )
        {
            p[index++] = ch;
            start = ezbus_platform.callback_get_ms_ticks();
        }
    }
    return index;
}


static int ezbus_seek_leadin( ezbus_port_t* port )
{
    int ch;

    do { ch = ezbus_port_getch( port ); } while ( ch >= 0 && ch != EZBUS_MARK );
    
    return ch;
}

extern EZBUS_ERR ezbus_port_recv( ezbus_port_t* port, ezbus_packet_t* packet )
{
    EZBUS_ERR err   = EZBUS_ERR_NOTREADY;
    int       index = 0;
    uint8_t*  p     = (uint8_t*)&packet->header;
    int ch;

    ch = ezbus_seek_leadin( port );
    if ( ch == EZBUS_MARK )
    {
        p[ index++ ] = ch;
        index = ezbus_private_recv( port, p, index, sizeof( ezbus_header_t ) );
        if ( index == sizeof( ezbus_header_t ) )
        {
            ezbus_packet_header_flip( packet );
            if ( ezbus_packet_header_valid_crc( packet ) )
            {
                if ( ezbus_packet_has_data( packet ) )
                {
                    index = ezbus_private_recv( port, &packet->data.crc, 0, sizeof( ezbus_crc_t ) );
                    if ( index == sizeof( ezbus_crc_t ) )
                    {
                        switch ( ezbus_packet_type( packet ) )
                        {
                            case packet_type_parcel: 
                            {
                               /* variable parcel data transport size... */
                                ezbus_parcel_t* parcel = ezbus_packet_get_parcel( packet );
                                index = ezbus_private_recv( port, &parcel->size, 0, sizeof(parcel->size) );
                                if ( index == sizeof(uint16_t) )
                                {
                                    if ( parcel->size <= EZBUS_PARCEL_DATA_LN  )
                                    {
                                        index = ezbus_private_recv( port, parcel->bytes, 0, parcel->size );
                                        if ( index == parcel->size )
                                        {
                                            ezbus_packet_data_flip( packet );
                                            err = ezbus_packet_data_valid_crc( packet ) ? EZBUS_ERR_OKAY : EZBUS_ERR_DATA_CRC;
                                        }
                                        else
                                        {
                                            err = EZBUS_ERR_TIMEOUT;
                                            EZBUS_LOG( EZBUS_LOG_PORT, "parcel %s", ezbus_fault_str(err) );
                                        }
                                    }
                                    else
                                    {
                                        err = EZBUS_ERR_RANGE;
                                        EZBUS_LOG( EZBUS_LOG_PORT, "parcel %s", ezbus_fault_str(err) );
                                    }
                                    
                                }
                                else
                                {
                                    err = EZBUS_ERR_RANGE;
                                    EZBUS_LOG( EZBUS_LOG_PORT, "parcel %s", ezbus_fault_str(err) );
                                }
                                
                            }
                            break;
                            case packet_type_take_token:
                            case packet_type_give_token:
                            case packet_type_speed:
                            case packet_type_pause:
                            {
                                index = ezbus_private_recv( port, ezbus_packet_data( packet ), 0, ezbus_packet_data_tx_size( packet ) );
                                if ( index == ezbus_packet_data_tx_size( packet ) )
                                {
                                    ezbus_packet_data_flip( packet );
                                    err = ezbus_packet_data_valid_crc( packet ) ? EZBUS_ERR_OKAY : EZBUS_ERR_DATA_CRC;
                                }
                                else
                                {
                                    err = EZBUS_ERR_TIMEOUT;
                                    EZBUS_LOG( EZBUS_LOG_PORT, "parcel %s", ezbus_fault_str(err) );
                                }
                            }
                            break;
                            default:
                            break;
                        }
                    }
                    else
                    {
                        err = EZBUS_ERR_DATA_CRC;
                        EZBUS_LOG( EZBUS_LOG_PORT, "%s", ezbus_fault_str(err) );
                    }
                }
                else
                {
                    err = EZBUS_ERR_OKAY;
                }
            }
            else
            {
                err = EZBUS_ERR_HEADER_CRC;
                EZBUS_LOG( EZBUS_LOG_PORT, "%s", ezbus_fault_str(err) );
            }
        }
        else
        {
            err = EZBUS_ERR_TIMEOUT;
            EZBUS_LOG( EZBUS_LOG_PORT, "header %s", ezbus_fault_str(err) );
        }
    }

    if ( err == EZBUS_ERR_OKAY )
    {
        /** @note In case of hardware loopback, discard our own packets */ 
        if ( ezbus_address_compare(ezbus_packet_src(packet),ezbus_port_get_address(port)) == 0 )
        {
            ezbus_packet_dump( "DROP RX:", packet, ezbus_packet_tx_size( packet ) );
            err = EZBUS_ERR_NOTREADY;
        }
        else 
        {
            ezbus_packet_dump( "RX:", packet, ezbus_packet_tx_size( packet ) );
        }
    }

    return err;
}

void ezbus_port_close( ezbus_port_t* port )
{
    port->callback_close(port);
}

void ezbus_port_drain( ezbus_port_t* port )
{
    port->callback_drain(port);
}

extern int ezbus_port_getch( ezbus_port_t* port )
{
    return port->callback_getch( port );
}

void ezbus_port_set_speed( ezbus_port_t* port, uint32_t speed )
{
    port->packet_timeout = ezbus_port_packet_timeout_time_ms(port);
    port->callback_set_speed(port,speed);
}

uint32_t ezbus_port_get_speed( ezbus_port_t* port )
{
    return port->callback_get_speed(port);
}

extern void ezbus_port_set_address( ezbus_port_t* port, const ezbus_address_t* address )
{
    port->callback_set_address(port,address);
}

extern const ezbus_address_t* ezbus_port_get_address( ezbus_port_t* port )
{
    return port->callback_get_address(port);
}

extern bool ezbus_port_get_address_is_self( ezbus_port_t* port, const ezbus_address_t* address )
{
    return ezbus_address_compare(&port->self_address,address) == 0;
}

uint32_t ezbus_port_byte_time_ns( ezbus_port_t* port )
{
    uint32_t bits_sec = ezbus_port_get_speed(port);
    uint32_t nsec_bit = 1000000000 / bits_sec;  /* ex. 1000000000 / 10000000 = 100 */
    uint32_t nsec_byte = nsec_bit * 10;         /* ex. 100 * 10 = 1000 */
    return nsec_byte;
}

extern uint32_t ezbus_port_packet_timeout_time_ms( ezbus_port_t* port )
{
    /* 1000000 ns in a ms. */
    uint32_t nsec_byte = ezbus_port_byte_time_ns(port);
    uint32_t nsec_packet = sizeof(ezbus_packet_t) * nsec_byte;
    uint32_t msec_packet = nsec_packet/1000000;
    return msec_packet?msec_packet:1;
}

extern void ezbus_port_dump( ezbus_port_t* port,const char* prefix )
{
    // char print_buffer[EZBUS_TMP_BUF_SZ];

    // sprintf( print_buffer, "%s.platform_port", prefix );
    // ezbus_platform.callback_port_dump( &port->platform_port, print_buffer );

    // fprintf(stderr, "%s.speed=%d\n",                    prefix, port->speed );
    // fprintf(stderr, "%s.packet_timeout=%d\n",           prefix, port->packet_timeout );
    // fprintf(stderr, "%s.rx_err_crc_count=%d\n",         prefix, port->rx_err_crc_count );
    // fprintf(stderr, "%s.rx_err_timeout_count=%d\n",     prefix, port->rx_err_timeout_count );
    // fprintf(stderr, "%s.rx_err_overrun_count=%d\n",     prefix, port->rx_err_overrun_count );
    // fprintf(stderr, "%s.tx_err_overrun_count=%d\n",     prefix, port->tx_err_overrun_count );
    // fprintf(stderr, "%s.tx_err_retry_fail_count=%d\n",  prefix, port->tx_err_retry_fail_count );
    // fflush(stderr);
}
