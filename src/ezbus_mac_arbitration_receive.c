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
#include <ezbus_mac_arbitration_receive.h>
#include <ezbus_mac_bootstrap.h>
#include <ezbus_mac_receiver.h>
#include <ezbus_mac_token.h>
#include <ezbus_hex.h>
#include <ezbus_log.h>

#define ezbus_mac_arbitration_get_receiver(mac_arbitration_receive) (((mac_arbitration_receive))->mac_receiver)

static void ezbus_mac_receiver_packet_type_reset      ( ezbus_mac_arbitration_receive_t* arbitration_receive, ezbus_packet_t* rx_packet );
static void ezbus_mac_receiver_packet_type_take_token ( ezbus_mac_arbitration_receive_t* arbitration_receive, ezbus_packet_t* rx_packet );
static void ezbus_mac_receiver_packet_type_give_token ( ezbus_mac_arbitration_receive_t* arbitration_receive, ezbus_packet_t* rx_packet );
static void ezbus_mac_receiver_packet_type_parcel     ( ezbus_mac_arbitration_receive_t* arbitration_receive, ezbus_packet_t* rx_packet );
static void ezbus_mac_receiver_packet_type_speed      ( ezbus_mac_arbitration_receive_t* arbitration_receive, ezbus_packet_t* rx_packet );
static void ezbus_mac_receiver_packet_type_ack        ( ezbus_mac_arbitration_receive_t* arbitration_receive, ezbus_packet_t* rx_packet );
static void ezbus_mac_receiver_packet_type_coldboot   ( ezbus_mac_arbitration_receive_t* arbitration_receive, ezbus_packet_t* rx_packet );
static void ezbus_mac_receiver_packet_type_warmboot   ( ezbus_mac_arbitration_receive_t* arbitration_receive, ezbus_packet_t* rx_packet );

static void ezbus_mac_arbitration_receive_packet      ( ezbus_mac_t* mac );


extern void ezbus_mac_arbitration_receive_init  ( ezbus_mac_t* mac );
{
    ezbus_mac_arbitration_receive_t* arbitration_receive = ezbus_mac_arbitration_receive( mac );

    ezbus_timer_init( &arbitration_receive->ack_rx_timer );
    ezbus_timer_set_period( &arbitration_receive->ack_rx_timer,  )
}


static void ezbus_mac_arbitration_receive_packet ( ezbus_mac_t* mac )
{
    ezbus_packet_t* rx_packet  = ezbus_mac_receiver_get_packet( ezbus_mac_arbitration_get_receiver(mac_arbitration_receive) );
    
    ezbus_log( EZBUS_LOG_ARBITRATION, "ezbus_mac_receiver_signal_full\n" );

    switch( ezbus_packet_type( rx_packet ) )
    {
        case packet_type_reset:      ezbus_mac_receiver_packet_type_reset      ( arbitration_receive, rx_packet ); break;
        case packet_type_take_token: ezbus_mac_receiver_packet_type_take_token ( arbitration_receive, rx_packet ); break;
        case packet_type_give_token: ezbus_mac_receiver_packet_type_give_token ( arbitration_receive, rx_packet ); break;
        case packet_type_parcel:     ezbus_mac_receiver_packet_type_parcel     ( arbitration_receive, rx_packet ); break;
        case packet_type_speed:      ezbus_mac_receiver_packet_type_speed      ( arbitration_receive, rx_packet ); break;
        case packet_type_ack:        ezbus_mac_receiver_packet_type_ack        ( arbitration_receive, rx_packet ); break;
        case packet_type_nack:       ezbus_mac_receiver_packet_type_nack       ( arbitration_receive, rx_packet ); break;
        case packet_type_coldboot:   ezbus_mac_receiver_packet_type_coldboot   ( arbitration_receive, rx_packet ); break;
        case packet_type_warmboot:   ezbus_mac_receiver_packet_type_warmboot   ( arbitration_receive, rx_packet ); break;
    }
}


static void ezbus_mac_receiver_packet_type_reset( ezbus_mac_arbitration_receive_t* arbitration_receive, ezbus_packet_t* rx_packet )
{
    /* FIXME - write code here */
}

static void ezbus_mac_receiver_packet_type_take_token( ezbus_mac_arbitration_receive_t* arbitration_receive, ezbus_packet_t* rx_packet )
{
    if ( ezbus_address_compare( ezbus_packet_dst(rx_packet), &ezbus_self_address ) != 0 )
    {
        ezbus_token_relinquish( ezbus_mac_arbitration_get_token( ezbus_mac_arbitration_receive_get_arbitration( arbitration_receive ) ) );
    }
}

static void ezbus_mac_receiver_packet_type_give_token( ezbus_mac_arbitration_receive_t* arbitration_receive, ezbus_packet_t* rx_packet )
{
    if ( ezbus_address_compare( ezbus_packet_dst(rx_packet), &ezbus_self_address ) == 0 )
    {
        ezbus_token_acquire( ezbus_mac_arbitration_get_token( ezbus_mac_arbitration_receive_get_arbitration( arbitration_receive ) ) );
    }
}

static void ezbus_mac_receiver_packet_type_parcel( ezbus_mac_arbitration_receive_t* arbitration_receive, ezbus_packet_t* rx_packet )
{
    if ( ezbus_address_compare( ezbus_packet_dst(rx_packet), &ezbus_self_address ) == 0 )
    {
        ezbus_layer1_receive_parcel( rx_packet );
    }
}

static void ezbus_mac_receiver_packet_type_speed( ezbus_mac_arbitration_receive_t* arbitration_receive, ezbus_packet_t* rx_packet )
{
    /* FIXME - write code here */
}

static void ezbus_mac_receiver_packet_type_ack( ezbus_mac_arbitration_receive_t* arbitration_receive, ezbus_packet_t* rx_packet )
{
    if ( ezbus_address_compare( ezbus_packet_dst(rx_packet), &ezbus_self_address ) == 0 )
    {
        if ( ezbus_address_compare( ezbus_packet_src(rx_packet), ezbus_packet_dst(tx_packet) ) == 0 )
        {
            if ( ezbus_packet_seq(rx_packet) == ezbus_packet_seq(tx_packet) )
            {
                ezbus_mac_transmitter_set_state( 
                    ezbus_mac_arbitration_get_transmitter( 
                        ezbus_mac_arbitration_receive_get_arbitration( arbitration_receive ) ), transmitter_state_empty );
            }
            else
            {
                /* FIXME - throw a fault here? */
                ezbus_log( EZBUS_LOG_ARBITRATION, "recv: ack seq mismatch\n");
            }
        }
        else
        {
            /* FIXME - throw a fauld herre? */
            ezbus_log( EZBUS_LOG_ARBITRATION, "recv: ack address mismatch\n" );
        }
    }
}

static void ezbus_mac_receiver_packet_type_nack( ezbus_mac_arbitration_receive_t* arbitration_receive, ezbus_packet_t* rx_packet )
{
    if ( ezbus_address_compare( ezbus_packet_dst(rx_packet), &ezbus_self_address ) == 0 )
    {
        if ( ezbus_address_compare( ezbus_packet_src(rx_packet), ezbus_packet_dst(tx_packet) ) == 0 )
        {
            if ( ezbus_packet_seq(rx_packet) == ezbus_packet_seq(tx_packet) )
            {
                /* FIXME - cap # re-tries */
                ezbus_mac_transmitter_set_state( 
                    ezbus_mac_arbitration_get_transmitter( 
                        ezbus_mac_arbitration_receive_get_arbitration( arbitration_receive ) ), transmitter_state_send );
            }
            else
            {
                /* FIXME - throw a fault here? */
                ezbus_log( EZBUS_LOG_ARBITRATION, "recv: ack seq mismatch\n");
            }
        }
        else
        {
            /* FIXME - throw a fauld herre? */
            ezbus_log( EZBUS_LOG_ARBITRATION, "recv: ack address mismatch\n" );
        }
    }
}

static void ezbus_mac_receiver_packet_type_coldboot( ezbus_mac_arbitration_receive_t* arbitration_receive, ezbus_packet_t* rx_packet )
{


    ezbus_mac_arbitration_receive_signal_coldboot( arbitration_receive, rx_packet );


    ezbus_token_acquire( ezbus_mac_arbitration_get_token( ezbus_mac_arbitration_receive_get_arbitration( arbitration_receive ) ) );

    ezbus_boot_signal_peer_seen( ezbus_mac_get_boot( mac ), rx_packet );
    if ( ezbus_packet_is_warmboot( rx_packet ) )
    {
        ezbus_log( EZBUS_LOG_WARMBOOT, "%cwarmboot <%s %3d | ", ezbus_mac_get_token(mac)?'*':' ', ezbus_address_string( ezbus_packet_src( rx_packet ) ), ezbus_packet_seq( rx_packet ) );
        #if EZBUS_LOG_WARMBOOT
            ezbus_peer_list_log( &mac->peer_list );
        #endif
    }
    else
    if ( ezbus_packet_is_coldboot( rx_packet ) )
    {
        ezbus_log( EZBUS_LOG_COLDBOOT, "%ccoldboot <%s %3d | ", ezbus_mac_get_token(mac)?'*':' ', ezbus_address_string( ezbus_packet_src( rx_packet ) ), ezbus_packet_seq( rx_packet ) );
        #if EZBUS_LOG_COLDBOOT
            ezbus_peer_list_log( &mac->peer_list );
        #endif
    }
}

static void ezbus_mac_receiver_packet_type_warmboot( ezbus_mac_arbitration_receive_t* arbitration_receive, ezbus_packet_t* rx_packet )
{
    ezbus_boot_signal_peer_seen( ezbus_mac_get_boot( mac ), rx_packet );
    if ( ezbus_packet_is_warmboot( rx_packet ) )
    {
        ezbus_log( EZBUS_LOG_WARMBOOT, "%cwarmboot <%s %3d | ", ezbus_mac_get_token(mac)?'*':' ', ezbus_address_string( ezbus_packet_src( rx_packet ) ), ezbus_packet_seq( rx_packet ) );
        #if EZBUS_LOG_WARMBOOT
            ezbus_peer_list_log( &mac->peer_list );
        #endif
    }
    else
    if ( ezbus_packet_is_coldboot( rx_packet ) )
    {
        ezbus_log( EZBUS_LOG_COLDBOOT, "%ccoldboot <%s %3d | ", ezbus_mac_get_token(mac)?'*':' ', ezbus_address_string( ezbus_packet_src( rx_packet ) ), ezbus_packet_seq( rx_packet ) );
        #if EZBUS_LOG_COLDBOOT
            ezbus_peer_list_log( &mac->peer_list );
        #endif
    }
}

