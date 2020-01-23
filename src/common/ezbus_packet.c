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
#include <ezbus_packet.h>
#include <ezbus_crc.h>
#include <ezbus_hex.h>

void ezbus_packet_init(ezbus_packet_t* packet)
{
    ezbus_platform_memset(packet,0,sizeof(ezbus_packet_t));
    ezbus_packet_set_version(packet,PACKET_BITS_VERSION);
    ezbus_packet_set_ack_req ( packet, PACKET_BITS_ACK_REQ );
}

void ezbus_packet_deinit(ezbus_packet_t* packet)
{
    if ( packet != NULL )
    {
        ezbus_platform_memset(packet,0,sizeof(ezbus_packet_t));
    }
}

extern void ezbus_packet_set_bits( ezbus_packet_t* packet, uint16_t bits )
{
    packet->header.data.field.bits = bits;
}

extern void ezbus_packet_set_version( ezbus_packet_t* packet, uint16_t version )
{
    packet->header.data.field.bits &= PACKET_BITS_VERSION_MASK;
    packet->header.data.field.bits |= (version & PACKET_BITS_VERSION_MASK);
}

extern void ezbus_packet_set_chain( ezbus_packet_t* packet, uint16_t chain )
{
    packet->header.data.field.bits &= PACKET_BITS_CHAIN_MASK;
    packet->header.data.field.bits |= (chain & PACKET_BITS_CHAIN_MASK);
}

extern void ezbus_packet_set_ack_req( ezbus_packet_t* packet, uint16_t ack_req )
{
    packet->header.data.field.bits &= PACKET_BITS_ACK_REQ_MASK;
    packet->header.data.field.bits |= (ack_req & PACKET_BITS_ACK_REQ_MASK);
}

extern void ezbus_packet_set_seq( ezbus_packet_t* packet, uint8_t seq )
{
    packet->header.data.field.seq = seq;
}

extern void ezbus_packet_set_type( ezbus_packet_t* packet, ezbus_packet_type_t type )
{
    packet->header.data.field.type = (uint8_t)type;
}

extern void ezbus_packet_set_src( ezbus_packet_t* packet, ezbus_address_t* address )
{
    ezbus_address_copy( ezbus_packet_src( packet ), address );
}
extern void ezbus_packet_set_dst( ezbus_packet_t* packet, ezbus_address_t* address )
{
    ezbus_address_copy( ezbus_packet_dst( packet ), address );
}

extern void ezbus_packet_set_src_socket( ezbus_packet_t* packet, ezbus_socket_t socket )
{
    packet->header.data.field.src_socket = socket;
}

extern void ezbus_packet_set_dst_socket( ezbus_packet_t* packet, ezbus_socket_t socket )
{
    packet->header.data.field.dst_socket = socket;
}



extern void ezbus_packet_set_token_crc( ezbus_packet_t* packet, ezbus_crc_t* crc )
{
    memcpy( &packet->data.attachment.token.crc, crc, sizeof(ezbus_crc_t) );
}

extern void ezbus_packet_set_token_age( ezbus_packet_t* packet, uint16_t age )
{
    packet->data.attachment.token.age = age;
}

extern ezbus_crc_t* ezbus_packet_get_token_crc( ezbus_packet_t* packet )
{
    return &packet->data.attachment.token.crc;
}

extern uint16_t ezbus_packet_get_token_age( ezbus_packet_t* packet )
{
    return packet->data.attachment.token.age;
}



extern uint16_t ezbus_packet_bits( ezbus_packet_t* packet )
{
    return packet->header.data.field.bits;
}

extern uint16_t ezbus_packet_version( ezbus_packet_t* packet )
{
    return packet->header.data.field.bits & PACKET_BITS_VERSION_MASK;
}

extern uint16_t ezbus_packet_chain( ezbus_packet_t* packet )
{
    return packet->header.data.field.bits & PACKET_BITS_CHAIN_MASK;
}

extern uint16_t ezbus_packet_ack_req( ezbus_packet_t* packet )
{
    return packet->header.data.field.bits & PACKET_BITS_ACK_REQ_MASK;
}

extern uint8_t ezbus_packet_seq( ezbus_packet_t* packet )
{
    return packet->header.data.field.seq;
}

extern ezbus_packet_type_t ezbus_packet_type( ezbus_packet_t* packet )
{
    return (ezbus_packet_type_t)packet->header.data.field.type;
}



extern ezbus_address_t* ezbus_packet_dst( ezbus_packet_t* packet )
{
    return &packet->header.data.field.dst;
}

extern ezbus_address_t* ezbus_packet_src( ezbus_packet_t* packet )
{
    return &packet->header.data.field.src;
}

extern ezbus_socket_t ezbus_packet_dst_socket( ezbus_packet_t* packet )
{
    return packet->header.data.field.dst_socket;
}

extern ezbus_socket_t ezbus_packet_src_socket( ezbus_packet_t* packet )
{
    return packet->header.data.field.src_socket;
}


extern void ezbus_packet_copy( ezbus_packet_t* dst, ezbus_packet_t* src )
{
    ezbus_platform_memcpy( dst, src, sizeof( ezbus_packet_t ) );
}

extern void ezbus_packet_calc_crc( ezbus_packet_t* packet )
{
    ezbus_crc_init( &packet->header.crc );
    ezbus_crc( &packet->header.crc, packet->header.data.bytes, sizeof(struct _header_field_) );

    ezbus_crc_init( &packet->data.crc );
    ezbus_crc( &packet->data.crc, ezbus_packet_data( packet ), ezbus_packet_data_size( packet ) );
}



extern bool ezbus_packet_valid_crc( ezbus_packet_t* packet )
{
    if ( ezbus_packet_header_valid_crc( packet ) )
    {
        return ezbus_packet_data_valid_crc( packet );
    }
    return false;
}

extern bool ezbus_packet_header_valid_crc( ezbus_packet_t* packet )
{
    ezbus_crc_t crc;
    ezbus_crc_init( &crc );
    return ezbus_crc_equal( &packet->header.crc, ezbus_crc( &crc, packet->header.data.bytes, sizeof(struct _header_field_) ) );
}

extern bool ezbus_packet_data_valid_crc( ezbus_packet_t* packet )
{
    ezbus_crc_t crc;
    ezbus_crc_init( &crc );
    return ezbus_crc_equal( &packet->data.crc, ezbus_crc( &crc, ezbus_packet_data( packet ), ezbus_packet_data_size( packet ) ) );
}



uint16_t ezbuf_packet_bytes_to_send( ezbus_packet_t* packet )
{
    uint16_t data_size = ezbus_packet_data_size( packet );
    uint16_t bytes_to_send = sizeof( ezbus_header_t ) + ( data_size ? data_size + sizeof( ezbus_crc_t ) : 0 );
    return bytes_to_send;
}

extern uint8_t* ezbus_packet_data( ezbus_packet_t* packet )
{
    return (uint8_t*)&packet->data.attachment;
}

extern uint16_t ezbus_packet_data_size( ezbus_packet_t* packet )
{
    uint16_t size=0;
    switch ( ezbus_packet_type( packet ) )
    {
        case packet_type_reset:
        case packet_type_take_token:
                size = sizeof( ezbus_token_t );
                break;
        case packet_type_give_token:
                size = sizeof( ezbus_token_t );
                break;
        case packet_type_ack:
        case packet_type_nack:
        case packet_type_coldboot:
        case packet_type_warmboot_rq:
        case packet_type_warmboot_rp:
        case packet_type_warmboot_ak:
                break;
        case packet_type_parcel:
                size = sizeof( ezbus_parcel_t );
                break;
        case packet_type_speed:
                size = sizeof( ezbus_speed_t );
                break;
    }
    return size;
}

extern ezbus_parcel_t* ezbus_packet_get_parcel( ezbus_packet_t* packet )
{
    return &packet->data.attachment.parcel;
}


extern void ezbus_packet_flip( ezbus_packet_t* packet )
{
    ezbus_packet_header_flip ( packet );
    ezbus_packet_data_flip   ( packet );
}

extern void ezbus_packet_header_flip( ezbus_packet_t* packet )
{
    ezbus_crc_flip( &packet->header.crc );
}

extern void ezbus_packet_data_flip( ezbus_packet_t* packet )
{
    ezbus_crc_flip( &packet->data.crc );
}

extern void ezbus_packet_dump( const char* prefix, ezbus_packet_t* packet, size_t bytes_to_send )
{
    char h8[3];
    uint8_t* p = (uint8_t*)packet;
    fprintf( stderr, "%s%02X", prefix, p[0] );
    switch( ezbus_packet_type( packet ) )
    {
        case packet_type_reset:         fprintf( stderr, "<RST>" );    break;
        case packet_type_coldboot:      fprintf( stderr, "<CBT>" );    break;
        case packet_type_warmboot_rq:   fprintf( stderr, "<WBQ>" );    break;
        case packet_type_warmboot_rp:   fprintf( stderr, "<WBR>" );    break;
        case packet_type_warmboot_ak:   fprintf( stderr, "<WBK>" );    break;
        case packet_type_take_token:    fprintf( stderr, "<TTK>" );    break;
        case packet_type_give_token:    fprintf( stderr, "<GTK>" );    break;
        case packet_type_parcel:        fprintf( stderr, "<PAC>" );    break;
        case packet_type_speed:         fprintf( stderr, "<SPD>" );    break;
        case packet_type_ack:           fprintf( stderr, "<ACK>" );    break;
        case packet_type_nack:          fprintf( stderr, "<NAK>" );    break;
        default:
            break;
    }
    for( int n=2; n < bytes_to_send; n++ )
    {
        ezbus_hex8( p[n], h8 );
        fputc( h8[0], stderr );
        fputc( h8[1], stderr );
    }
    fputc( '\r', stderr );
    fputc( '\n', stderr );
    fflush(stderr);
}
