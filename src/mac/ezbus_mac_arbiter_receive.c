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
#include <ezbus_mac_arbiter_receive.h>
#include <ezbus_mac_arbiter_transmit.h>
#include <ezbus_mac_arbiter_pause.h>
#include <ezbus_mac_transmitter.h>
#include <ezbus_mac_receiver.h>
#include <ezbus_mac_token.h>
#include <ezbus_mac_peers.h>
#include <ezbus_socket_callback.h>
#include <ezbus_address.h>
#include <ezbus_packet.h>
#include <ezbus_peer.h>
#include <ezbus_hex.h>
#include <ezbus_log.h>
#include <ezbus_platform.h>

static void do_receiver_packet_type_reset           ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static void do_receiver_packet_type_take_token      ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static void do_receiver_packet_type_give_token      ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static void do_receiver_packet_type_parcel          ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static void do_receiver_packet_type_pause           ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static void do_receiver_packet_type_speed           ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static void do_receiver_packet_type_ack             ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static void do_receiver_packet_type_nack            ( ezbus_mac_t* mac, ezbus_packet_t* packet );

static void do_receiver_packet_type_coldboot        ( ezbus_mac_t* mac, ezbus_packet_t* packet );

extern void do_receiver_packet_type_boot2_rq        ( ezbus_mac_t* mac, ezbus_packet_t* packet );
extern void do_receiver_packet_type_boot2_rp        ( ezbus_mac_t* mac, ezbus_packet_t* packet );
extern void do_receiver_packet_type_boot2_ak        ( ezbus_mac_t* mac, ezbus_packet_t* packet );



extern void ezbus_mac_arbiter_receive_init  ( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_receive_t* arbiter_receive = ezbus_mac_get_arbiter_receive( mac );
    ezbus_platform.callback_memset(arbiter_receive,0,sizeof(ezbus_mac_arbiter_receive_t));
}


extern void ezbus_mac_arbiter_receive_run( ezbus_mac_t* mac )
{
    /* ?? */
}

extern void ezbus_mac_arbiter_receive_set_filter(ezbus_mac_t* mac, mac_arbiter_recieve_callback_t callback_filter)
{
    ezbus_mac_arbiter_receive_t* arbiter_receive = ezbus_mac_get_arbiter_receive( mac );
    arbiter_receive->callback_filter = callback_filter;
}

static void do_receiver_packet_type_reset( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    /* FIXME - write code here */
}

static void do_receiver_packet_type_take_token( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    if ( !ezbus_port_get_address_is_self( ezbus_mac_get_port(mac), ezbus_packet_dst( packet ) ) )
    {
        ezbus_mac_token_relinquish( mac );
    }
}

static void do_receiver_packet_type_give_token( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    ezbus_mac_arbiter_set_token_age( mac, ezbus_packet_get_token_age( packet ) );
    ezbus_mac_arbiter_receive_signal_token( mac, packet );
}

static void do_receiver_packet_type_parcel( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    if ( ezbus_port_get_address_is_self( ezbus_mac_get_port(mac), ezbus_packet_dst( packet ) ) )
    {
        ezbus_mac_arbiter_receive_signal_parcel( mac, packet );
    }
}

static void do_receiver_packet_type_pause( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    if ( ezbus_address_is_broadcast( ezbus_packet_dst( packet ) ) || ezbus_port_get_address_is_self( ezbus_mac_get_port(mac), ezbus_packet_dst( packet ) ) )
    {
        ezbus_pause_t* pause = ezbus_packet_get_pause( packet );
        uint16_t pause_duration = ezbus_pause_get_duration( pause );
        bool active = ezbus_pause_get_active( pause );

        ezbus_mac_arbiter_pause_set_period( mac, 0 );
        ezbus_mac_arbiter_pause_set_duration( mac, pause_duration );
        ezbus_mac_arbiter_pause_start( mac );                             
        
        EZBUS_LOG( EZBUS_LOG_ARBITER, "recv: do_receiver_packet_type_pause %d %d", pause_duration, active );
    }
    else
    {
        EZBUS_LOG( EZBUS_LOG_ARBITER, "recv: do_receiver_packet_type_pause wrong address %s", ezbus_address_string(ezbus_packet_dst(packet)) );
    }
}

static void do_receiver_packet_type_speed( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    /* FIXME - write code here */
}

static void do_receiver_packet_type_ack( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    if ( ezbus_port_get_address_is_self( ezbus_mac_get_port(mac), ezbus_packet_dst( packet ) ) )
    {
        ezbus_packet_t* tx_packet = ezbus_mac_get_transmitter_packet( mac );
        if ( ezbus_address_compare( ezbus_packet_src(packet), ezbus_packet_dst(tx_packet) ) == 0 )
        {
            if ( ezbus_packet_seq(packet) == ezbus_packet_seq(tx_packet) )
            {
                ezbus_mac_arbiter_transmit_reset( mac );
                ezbus_socket_callback_transmitter_ack( mac );
            }
            else
            {
                ezbus_socket_callback_transmitter_fault( mac );
                EZBUS_LOG( EZBUS_LOG_ARBITER, "recv: ack seq mismatch");
            }
        }
        else
        {
            ezbus_socket_callback_transmitter_fault( mac );
            EZBUS_LOG( EZBUS_LOG_ARBITER, "recv: ack address mismatch" );
        }
    }
}

static void do_receiver_packet_type_nack( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    if ( ezbus_port_get_address_is_self( ezbus_mac_get_port(mac), ezbus_packet_dst( packet ) ) )
    {
        ezbus_packet_t* tx_packet = ezbus_mac_get_transmitter_packet( mac );
        if ( ezbus_address_compare( ezbus_packet_src(packet), ezbus_packet_dst(tx_packet) ) == 0 )
        {
            if ( ezbus_packet_seq(packet) == ezbus_packet_seq(tx_packet) )
            {
                ezbus_mac_arbiter_transmit_reset( mac );
                ezbus_socket_callback_transmitter_nack( mac );
            }
            else
            {
                ezbus_socket_callback_transmitter_fault( mac );
                EZBUS_LOG( EZBUS_LOG_ARBITER, "recv: nack seq mismatch");
            }
        }
        else
        {
            ezbus_socket_callback_transmitter_fault( mac );
            EZBUS_LOG( EZBUS_LOG_ARBITER, "recv: nack address mismatch" );
        }
    }
}


static void do_receiver_packet_type_coldboot( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    ezbus_mac_arbiter_receive_signal_boot0( mac, packet );
}

extern void ezbus_mac_receiver_signal_full( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_receive_t* arbiter_receive = ezbus_mac_get_arbiter_receive( mac );
    ezbus_packet_t* packet  = ezbus_mac_get_receiver_packet( mac );
    
    EZBUS_LOG( EZBUS_LOG_RECEIVER, "" );

    if ( arbiter_receive->callback_filter == NULL || 
         (arbiter_receive->callback_filter != NULL && 
            arbiter_receive->callback_filter(mac,packet) ) )
    {
        switch( ezbus_packet_type( packet ) )
        {
            case packet_type_reset:       do_receiver_packet_type_reset       ( mac, packet ); break;
            case packet_type_take_token:  do_receiver_packet_type_take_token  ( mac, packet ); break;
            case packet_type_give_token:  do_receiver_packet_type_give_token  ( mac, packet ); break;
            case packet_type_parcel:      do_receiver_packet_type_parcel      ( mac, packet ); break;
            case packet_type_pause:       do_receiver_packet_type_pause       ( mac, packet ); break;
            case packet_type_speed:       do_receiver_packet_type_speed       ( mac, packet ); break;
            case packet_type_ack:         do_receiver_packet_type_ack         ( mac, packet ); break;
            case packet_type_nack:        do_receiver_packet_type_nack        ( mac, packet ); break;
            case packet_type_coldboot:    do_receiver_packet_type_coldboot    ( mac, packet ); break;
            case packet_type_boot2_rq:    do_receiver_packet_type_boot2_rq    ( mac, packet ); break;
            case packet_type_boot2_rp:    do_receiver_packet_type_boot2_rp    ( mac, packet ); break;
            case packet_type_boot2_ak:    do_receiver_packet_type_boot2_ak    ( mac, packet ); break;
        }
    }
}


extern void ezbus_mac_receiver_signal_empty( ezbus_mac_t* mac )
{
    //EZBUS_LOG( EZBUS_LOG_RECEIVER, "ezbus_mac_receiver_signal_empty" );
}

extern void ezbus_mac_receiver_signal_ack( ezbus_mac_t* mac )
{
}

extern void ezbus_mac_receiver_signal_wait( ezbus_mac_t* mac )
{
    EZBUS_LOG( EZBUS_LOG_RECEIVER, "" );
    if ( ezbus_mac_transmitter_empty( mac ) )
    {
        ezbus_mac_receiver_set_state( mac, receiver_state_empty );
    }
}

extern void ezbus_mac_receiver_signal_fault( ezbus_mac_t* mac )
{
    if ( ezbus_mac_receiver_get_err( mac ) != EZBUS_ERR_NOTREADY ) // not_ready means rx empty.
    {
        EZBUS_LOG( EZBUS_LOG_RECEIVER, "%s",ezbus_fault_str( ezbus_mac_receiver_get_err( mac ) ) );
    }
}

