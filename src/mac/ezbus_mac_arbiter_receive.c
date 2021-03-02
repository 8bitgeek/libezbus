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
#include <ezbus_mac_boot0.h>
#include <ezbus_mac_boot1.h>
#include <ezbus_mac_boot2.h>
#include <ezbus_mac_token.h>
#include <ezbus_mac_peers.h>
#include <ezbus_socket_callback.h>
#include <ezbus_address.h>
#include <ezbus_packet.h>
#include <ezbus_peer.h>
#include <ezbus_hex.h>
#include <ezbus_log.h>
#include <ezbus_platform.h>

static ezbus_mac_arbiter_receive_t ezbus_mac_arbiter_receive_stack[EZBUS_MAC_STACK_SIZE];

static void do_receiver_packet_type_reset           ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static void do_receiver_packet_type_take_token      ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static void do_receiver_packet_type_give_token      ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static void do_receiver_packet_type_parcel          ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static void do_receiver_packet_type_pause           ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static void do_receiver_packet_type_speed           ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static void do_receiver_packet_type_ack             ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static void do_receiver_packet_type_nack            ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static void do_receiver_packet_type_coldboot        ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static void do_receiver_packet_type_boot2_rq        ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static void do_receiver_packet_type_boot2_rp        ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static void do_receiver_packet_type_boot2_ak        ( ezbus_mac_t* mac, ezbus_packet_t* packet );

static void ezbus_mac_arbiter_boot2_send_reply      ( ezbus_timer_t* timer, void* arg );
static void ezbus_mac_arbiter_boot2_send_ack        ( ezbus_mac_t* mac, ezbus_packet_t* rx_packet );
static void ezbus_mac_arbiter_receive_sniff         ( ezbus_mac_t* mac, ezbus_packet_t* rx_packet );


extern void ezbus_mac_arbiter_receive_init  ( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_receive_t* arbiter_receive = ezbus_mac_get_arbiter_receive( mac );

    ezbus_mac_timer_setup( mac, &arbiter_receive->boot2_timer, true );
    ezbus_timer_set_key( &arbiter_receive->boot2_timer, "boot2_timer" );
    ezbus_timer_set_callback( &arbiter_receive->boot2_timer, ezbus_mac_arbiter_boot2_send_reply, mac );

    ezbus_mac_timer_setup( mac, &arbiter_receive->ack_rx_timer, true );
    ezbus_timer_set_key( &arbiter_receive->ack_rx_timer, "ack_rx_timer" );
    ezbus_timer_set_callback( &arbiter_receive->boot2_timer, ezbus_mac_arbiter_boot2_send_reply, mac );
    ezbus_timer_set_period( &arbiter_receive->ack_rx_timer, ezbus_mac_token_ring_time(mac)*4 ); // FIXME *4 ??
}


extern void ezbus_mac_arbiter_receive_run( ezbus_mac_t* mac )
{
    /* ?? */
}

extern void ezbus_mac_arbiter_receive_push ( ezbus_mac_t* mac, uint8_t level )
{
    ezbus_mac_arbiter_receive_t* arbiter_receive = ezbus_mac_get_arbiter_receive( mac );
    ezbus_platform.callback_memcpy(&ezbus_mac_arbiter_receive_stack[level],arbiter_receive,sizeof(ezbus_mac_arbiter_receive_t));
}

extern void ezbus_mac_arbiter_receive_pop  ( ezbus_mac_t* mac, uint8_t level )
{
    ezbus_mac_arbiter_receive_t* arbiter_receive = ezbus_mac_get_arbiter_receive( mac );
    ezbus_platform.callback_memcpy(arbiter_receive,&ezbus_mac_arbiter_receive_stack[level],sizeof(ezbus_mac_arbiter_receive_t));
}


static void do_receiver_packet_type_reset( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    /* FIXME - write code here */
}

static void do_receiver_packet_type_take_token( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    ezbus_mac_arbiter_receive_t* arbiter_receive = ezbus_mac_get_arbiter_receive( mac );
    arbiter_receive->boot2_seq=0;
    if ( !ezbus_port_get_address_is_self( ezbus_mac_get_port(mac), ezbus_packet_dst( packet ) ) )
    {
        ezbus_mac_token_relinquish( mac );
    }
}

static void do_receiver_packet_type_give_token( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    ezbus_mac_arbiter_receive_t* arbiter_receive = ezbus_mac_get_arbiter_receive( mac );
    arbiter_receive->boot2_seq=0;
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
    ezbus_peer_t peer;
    ezbus_mac_arbiter_receive_t* arbiter_receive = ezbus_mac_get_arbiter_receive( mac );

    ezbus_mac_boot2_set_state( mac, state_boot2_idle );

    if ( ezbus_mac_boot0_is_active( mac ) )
    {
        ezbus_mac_boot0_set_state( mac, state_boot0_stop );
        ezbus_mac_boot1_set_state( mac, state_boot1_stop );
    }

    EZBUS_LOG( EZBUS_LOG_BOOTSTATE, "%coldboot <%s %3d | ", ezbus_mac_token_acquired(mac)?'*':' ', ezbus_address_string( ezbus_packet_src( packet ) ), ezbus_packet_seq( packet ) );
    // ezbus_mac_peers_log( mac );
    
    arbiter_receive->boot2_seq=0;
    ezbus_peer_init( &peer, ezbus_packet_src( packet ), ezbus_packet_seq( packet ) );

    if ( ezbus_address_compare( ezbus_port_get_address(ezbus_mac_get_port(mac)), ezbus_packet_src( packet ) ) > 0 )
    {
        ezbus_mac_coldboot_reset( mac );
    }
}

/**
 * @brief Receive a wb request from src, and this node's wb seq# does not match the rx seq#,
 * then we must reply. If the seq# matches, then we've already been acknowledged during this
 * session identified by seq#.
 * 
 */
static void do_receiver_packet_type_boot2_rq( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    ezbus_mac_arbiter_receive_t* arbiter_receive = ezbus_mac_get_arbiter_receive( mac );

    if ( ezbus_address_is_broadcast( ezbus_packet_dst(packet) ) )
    {
        if ( arbiter_receive->boot2_seq != ezbus_packet_seq( packet ) )
        {
            ezbus_timer_stop( &arbiter_receive->boot2_timer );
            ezbus_timer_set_period  ( 
                                        &arbiter_receive->boot2_timer, 
                                        ezbus_platform.callback_random( EZBUS_BOOT2_TIMER_MIN, EZBUS_BOOT2_TIMER_MAX ) 
                                    );
            ezbus_timer_restart( &arbiter_receive->boot2_timer );
        }
        else
        {
            ezbus_timer_stop( &arbiter_receive->boot2_timer );
        }
    }
}


/**
 * @brief I am the src of the boot2, and a node has replied.
 */
static void do_receiver_packet_type_boot2_rp( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    if ( ezbus_port_get_address_is_self( ezbus_mac_get_port(mac), ezbus_packet_dst( packet ) ) )
    {   
        ezbus_mac_arbiter_boot2_send_ack( mac, packet );
    }
}

/**
 * @brief Receive an wb acknolegment from src, and disable replying to this wb sequence#
 */
static void do_receiver_packet_type_boot2_ak( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    ezbus_mac_arbiter_receive_t* arbiter_receive = ezbus_mac_get_arbiter_receive( mac );

    if ( ezbus_port_get_address_is_self( ezbus_mac_get_port(mac), ezbus_packet_dst( packet ) ) )
    {       
        /* acknowledged, stop replying to this seq# */
        arbiter_receive->boot2_seq=ezbus_packet_seq( packet );
        ezbus_timer_stop( &arbiter_receive->boot2_timer );
    }
}

static void ezbus_mac_arbiter_boot2_send_reply( ezbus_timer_t* timer, void* arg )
{
    ezbus_packet_t tx_packet;
    ezbus_mac_t* mac = (ezbus_mac_t*)arg;
    ezbus_mac_arbiter_receive_t* arbiter_receive = ezbus_mac_get_arbiter_receive( mac );
    ezbus_packet_t* rx_packet = ezbus_mac_get_receiver_packet( mac );

    ezbus_timer_stop( &arbiter_receive->boot2_timer );

    ezbus_packet_init           ( &tx_packet );
    ezbus_packet_set_type       ( &tx_packet, packet_type_boot2_rp );
    ezbus_packet_set_dst_socket ( &tx_packet, EZBUS_SOCKET_ANY  );
    ezbus_packet_set_src_socket ( &tx_packet, EZBUS_SOCKET_ANY  );
    ezbus_packet_set_seq        ( &tx_packet, ezbus_packet_seq( rx_packet ) );
    ezbus_packet_set_src        ( &tx_packet, ezbus_port_get_address(ezbus_mac_get_port(mac)) );
    ezbus_packet_set_dst        ( &tx_packet, ezbus_packet_src( rx_packet ) );

    ezbus_mac_transmitter_put( mac, &tx_packet );
}


static void ezbus_mac_arbiter_boot2_send_ack( ezbus_mac_t* mac, ezbus_packet_t* rx_packet )
{
    ezbus_packet_t tx_packet;
    ezbus_mac_arbiter_receive_t* arbiter_receive = ezbus_mac_get_arbiter_receive( mac );

    ezbus_timer_stop( &arbiter_receive->boot2_timer );

    ezbus_packet_init           ( &tx_packet );
    ezbus_packet_set_type       ( &tx_packet, packet_type_boot2_ak );
    ezbus_packet_set_dst_socket ( &tx_packet, EZBUS_SOCKET_ANY  );
    ezbus_packet_set_src_socket ( &tx_packet, EZBUS_SOCKET_ANY  );
    ezbus_packet_set_seq        ( &tx_packet, ezbus_packet_seq( rx_packet ) );
    ezbus_packet_set_src        ( &tx_packet, ezbus_port_get_address(ezbus_mac_get_port(mac)) );
    ezbus_packet_set_dst        ( &tx_packet, ezbus_packet_src( rx_packet ) );

    ezbus_mac_transmitter_put( mac, &tx_packet );
}

static void ezbus_mac_arbiter_receive_sniff( ezbus_mac_t* mac, ezbus_packet_t* rx_packet )
{
    ezbus_peer_t peer;
    
    ezbus_peer_init( &peer, ezbus_packet_src( rx_packet ), ezbus_packet_seq( rx_packet ) );
    ezbus_mac_peers_insort( mac, &peer );

    ezbus_peer_init( &peer, ezbus_packet_dst( rx_packet ), ezbus_packet_seq( rx_packet ) );
    ezbus_mac_peers_insort( mac, &peer );
}

extern void ezbus_mac_receiver_signal_full  ( ezbus_mac_t* mac )
{
    ezbus_packet_t* packet  = ezbus_mac_get_receiver_packet( mac );
    
    EZBUS_LOG( EZBUS_LOG_RECEIVER, "" );

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
        case packet_type_boot2_rq: do_receiver_packet_type_boot2_rq ( mac, packet ); break;
        case packet_type_boot2_rp: do_receiver_packet_type_boot2_rp ( mac, packet ); break;
        case packet_type_boot2_ak: do_receiver_packet_type_boot2_ak ( mac, packet ); break;
    }

    ezbus_mac_arbiter_receive_sniff( mac, packet );
    ezbus_mac_coldboot_reset( mac ); 
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

