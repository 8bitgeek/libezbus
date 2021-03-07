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
#include <ezbus_mac_arbiter.h>
#include <ezbus_mac_arbiter_transmit.h>
#include <ezbus_mac_transmitter.h>
#include <ezbus_mac_receiver.h>
#include <ezbus_mac_token.h>
#include <ezbus_mac_peers.h>
#include <ezbus_socket_callback.h>
#include <ezbus_crc.h>
#include <ezbus_hex.h>
#include <ezbus_log.h>
#include <ezbus_mac_pause.h>
#include <ezbus_mac_arbiter_pause.h>
#include <ezbus_platform.h>

#define ezbus_mac_arbiter_transmitter_ready(mac)    ( ezbus_mac_transmitter_empty((mac)) && !ezbus_mac_arbiter_transmit_busy((mac)) )

#define ezbus_mac_arbiter_ready_to_give_token(mac)  ( ezbus_mac_arbiter_transmitter_ready((mac)) && arbiter->token_hold++ > EZBUS_TOKEN_HOLD_CYCLES )

#define ezbus_mac_arbiter_give_token(mac)           {                                                   \
                                                        ezbus_mac_arbiter_transmit_token((mac));        \
                                                        ezbus_mac_token_relinquish((mac));              \
                                                    }  

#define ezbus_mac_arbiter_ready_to_ack(mac)         ( ezbus_mac_arbiter_transmitter_ready((mac)) && arbiter->rx_ack_pend )

#define ezbus_mac_arbiter_ready_to_nack(mac)        ( ezbus_mac_arbiter_transmitter_ready((mac)) && arbiter->rx_nack_pend )

#define ezbus_mac_arbiter_send_ack_parcel(mac)      {                                               \
                                                        ezbus_mac_arbiter_ack_parcel( (mac), arbiter->rx_ack_seq, &arbiter->rx_ack_address );       \
                                                        arbiter->rx_ack_pend=false;                 \
                                                        arbiter->rx_ack_seq=0;                      \
                                                    }

#define ezbus_mac_arbiter_send_nack_parcel(mac)     {                                               \
                                                        ezbus_mac_arbiter_nack_parcel( (mac), arbiter->rx_nack_seq, &arbiter->rx_nack_address );    \
                                                        arbiter->rx_nack_pend=false;                \
                                                        arbiter->rx_nack_seq=0;                     \
                                                    }

/****************************************************************************/

#define ezbus_mac_boot1_set_emit_count(boot,c)   ((boot)->emit_count=(c))
#define ezbus_mac_boot1_get_emit_count(boot)     ((boot)->emit_count)
#define ezbus_mac_boot1_inc_emit_count(boot)     ezbus_mac_boot1_set_emit_count(boot,ezbus_mac_boot1_get_emit_count(boot)+1)

#define ezbus_mac_boot1_set_emit_seq(boot,c)     ((boot)->emit_count=(c))
#define ezbus_mac_boot1_get_emit_seq(boot)       ((boot)->emit_count)
#define ezbus_mac_boot1_inc_emit_seq(boot)       ezbus_mac_boot1_set_emit_count(boot,ezbus_mac_boot1_get_emit_count(boot)+1)

/****************************************************************************/

/* boot 0 */
static void ezbus_mac_boot0_init                ( ezbus_mac_t* mac );
static void ezbus_mac_boot0_deinit              ( ezbus_mac_t* mac );
static void ezbus_mac_boot0_stop                ( ezbus_mac_t* mac );
static bool ezbus_mac_arbiter_in_boot0_wait     ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_boot0_restart  ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_boot0_start    ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_boot0_active   ( ezbus_mac_t* mac );

/* boot 1 */
static void ezbus_mac_boot1_init                ( ezbus_mac_t* mac );
static void ezbus_mac_boot1_deinit              ( ezbus_mac_t* mac );
static void ezbus_mac_boot1_stop                ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_boot1_stop     ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_boot1_stopped  ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_boot1_start    ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_boot1_active   ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_boot1_dominant ( ezbus_mac_t* mac );

/* boot 2 */
static void     ezbus_mac_boot2_init                    ( ezbus_mac_t* mac );
static void     ezbus_mac_boot2_deinit                  ( ezbus_mac_t* mac );
static void     ezbus_mac_boot2_stop                    ( ezbus_mac_t* mac );
static bool     ezbus_mac_boot2_is_active               ( ezbus_mac_t* mac );
static uint8_t  ezbus_mac_arbiter_set_boot2_seq         ( ezbus_mac_t* mac, uint8_t seq );
static uint8_t  ezbus_mac_arbiter_get_boot2_seq         ( ezbus_mac_t* mac );
static uint8_t  ezbus_mac_arbiter_inc_boot2_seq         ( ezbus_mac_t* mac );
static void     do_mac_arbiter_state_boot2_idle         ( ezbus_mac_t* mac );
static void     do_mac_arbiter_state_boot2_restart      ( ezbus_mac_t* mac );
static void     do_mac_arbiter_state_boot2_start        ( ezbus_mac_t* mac );
static void     do_mac_arbiter_state_boot2_active       ( ezbus_mac_t* mac );
static void     do_mac_arbiter_state_boot2_stop         ( ezbus_mac_t* mac );
static void     do_mac_arbiter_state_boot2_finished     ( ezbus_mac_t* mac );

/* online */
static void do_mac_arbiter_state_offline        ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_service_start  ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_online         ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_pause          ( ezbus_mac_t* mac );

/* timer callbacks */
static void ezbus_mac_boot0_timer_callback      ( ezbus_timer_t* timer, void* arg );
static void ezbus_mac_boot1_timer_callback      ( ezbus_timer_t* timer, void* arg );
static void ezbus_mac_boot2_timer_callback      ( ezbus_timer_t* timer, void* arg );

/* parcel / token synchronization */
static bool ezbus_mac_arbiter_receive_token     ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static void ezbus_mac_arbiter_ack_parcel        ( ezbus_mac_t* mac, uint8_t seq, ezbus_address_t* address );
static void ezbus_mac_arbiter_nack_parcel       ( ezbus_mac_t* mac, uint8_t seq, ezbus_address_t* address );

/* senders */
static void ezbus_mac_arbiter_boot2_send_reply  ( ezbus_timer_t* timer, void* arg );
static void ezbus_mac_arbiter_boot2_send_ack    ( ezbus_mac_t* mac, ezbus_packet_t* packet );

/* receiver */
static void ezbus_mac_arbiter_receive_init      ( ezbus_mac_t* mac );
static void ezbus_mac_arbiter_receive_set_filter(ezbus_mac_t* mac, mac_arbiter_recieve_callback_t receiver_filter);

/* reciever filters */
static bool ezbus_mac_arbiter_boot_filter       ( ezbus_mac_t* mac, ezbus_packet_t* packet );

/* receiver handers */
static void do_receiver_packet_type_reset       ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static void do_receiver_packet_type_take_token  ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static void do_receiver_packet_type_give_token  ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static void do_receiver_packet_type_parcel      ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static void do_receiver_packet_type_pause       ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static void do_receiver_packet_type_speed       ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static void do_receiver_packet_type_ack         ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static void do_receiver_packet_type_nack        ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static void do_receiver_packet_type_boot1    ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static void do_receiver_packet_type_boot2_rq    ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static void do_receiver_packet_type_boot2_rp    ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static void do_receiver_packet_type_boot2_ak    ( ezbus_mac_t* mac, ezbus_packet_t* packet );


extern void  ezbus_mac_arbiter_init ( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );

    ezbus_platform.callback_memset( arbiter, 0 , sizeof( ezbus_mac_arbiter_t) );
    ezbus_mac_arbiter_rst_boot2_cycles( mac );

    ezbus_mac_boot0_init( mac );
    ezbus_mac_boot1_init( mac );
    ezbus_mac_boot2_init( mac );

    ezbus_mac_arbiter_receive_init( mac );
    ezbus_mac_arbiter_receive_set_filter( mac, ezbus_mac_arbiter_boot_filter );

}

extern void ezbus_mac_arbiter_run( ezbus_mac_t* mac )
{
    switch( ezbus_mac_arbiter_get_state( mac ) )
    {
        /* boot 0 */
        case mac_arbiter_state_boot0_restart:   do_mac_arbiter_state_boot0_restart( mac );  break;
        case mac_arbiter_state_boot0_start:     do_mac_arbiter_state_boot0_start( mac );    break;
        case mac_arbiter_state_boot0_active:    do_mac_arbiter_state_boot0_active( mac );   break;

        /* boot 1 */
        case mac_arbiter_state_boot1_stop:      do_mac_arbiter_state_boot1_stop( mac );     break;
        case mac_arbiter_state_boot1_stopped:   do_mac_arbiter_state_boot1_stopped( mac );  break;
        case mac_arbiter_state_boot1_start:     do_mac_arbiter_state_boot1_start( mac );    break;
        case mac_arbiter_state_boot1_active:    do_mac_arbiter_state_boot1_active( mac );   break;
        case mac_arbiter_state_boot1_dominant:  do_mac_arbiter_state_boot1_dominant( mac ); break;

        /* boot 2 */
        case mac_arbiter_state_boot2_idle:      do_mac_arbiter_state_boot2_idle( mac );     break;
        case mac_arbiter_state_boot2_restart:   do_mac_arbiter_state_boot2_restart( mac );  break;
        case mac_arbiter_state_boot2_start:     do_mac_arbiter_state_boot2_start( mac );    break;
        case mac_arbiter_state_boot2_active:    do_mac_arbiter_state_boot2_active( mac );   break;
        case mac_arbiter_state_boot2_stop:      do_mac_arbiter_state_boot2_stop( mac );     break;
        case mac_arbiter_state_boot2_finished:  do_mac_arbiter_state_boot2_finished( mac ); break;

        /* online */
        case mac_arbiter_state_offline:         do_mac_arbiter_state_offline( mac );        break;      
        case mac_arbiter_state_service_start:   do_mac_arbiter_state_service_start( mac );  break;
        case mac_arbiter_state_online:          do_mac_arbiter_state_online( mac );         break;               
        case mac_arbiter_state_pause:           do_mac_arbiter_state_pause( mac );          break;               
    }
}

const char* ezbus_mac_arbiter_get_state_str( ezbus_mac_t* mac )
{
    const char* rc="";

    switch( ezbus_mac_arbiter_get_state( mac ) )
    {
        /* boot 0 */
        case mac_arbiter_state_boot0_restart:   rc="mac_arbiter_state_boot0_restart";  break;
        case mac_arbiter_state_boot0_start:     rc="mac_arbiter_state_boot0_start";    break;
        case mac_arbiter_state_boot0_active:    rc="mac_arbiter_state_boot0_active";   break;

        /* boot 1 */
        case mac_arbiter_state_boot1_stop:      rc="mac_arbiter_state_boot1_stop";     break;
        case mac_arbiter_state_boot1_stopped:   rc="mac_arbiter_state_boot1_stopped";  break;
        case mac_arbiter_state_boot1_start:     rc="mac_arbiter_state_boot1_start";    break;
        case mac_arbiter_state_boot1_active:    rc="mac_arbiter_state_boot1_active";   break;
        case mac_arbiter_state_boot1_dominant:  rc="mac_arbiter_state_boot1_dominant"; break;

        /* boot 2 */
        case mac_arbiter_state_boot2_idle:      rc="mac_arbiter_state_boot2_idle";     break;
        case mac_arbiter_state_boot2_restart:   rc="mac_arbiter_state_boot2_restart";  break;
        case mac_arbiter_state_boot2_start:     rc="mac_arbiter_state_boot2_start";    break;
        case mac_arbiter_state_boot2_active:    rc="mac_arbiter_state_boot2_active";   break;
        case mac_arbiter_state_boot2_stop:      rc="mac_arbiter_state_boot2_stop";     break;
        case mac_arbiter_state_boot2_finished:  rc="mac_arbiter_state_boot2_finished"; break;

        /* online */
        case mac_arbiter_state_offline:         rc="mac_arbiter_state_offline";        break;      
        case mac_arbiter_state_service_start:   rc="mac_arbiter_state_service_start";  break;
        case mac_arbiter_state_online:          rc="mac_arbiter_state_online";         break;               
        case mac_arbiter_state_pause:           rc="mac_arbiter_state_pause";          break;               
    }

    return rc;
}


extern void ezbus_mac_arbiter_set_state ( ezbus_mac_t* mac, ezbus_mac_arbiter_state_t state )
{ 
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    arbiter->state = state;
    
    EZBUS_LOG( EZBUS_LOG_ARBITER, "%s", ezbus_mac_arbiter_get_state_str( mac ));
}

extern ezbus_mac_arbiter_state_t ezbus_mac_arbiter_get_state ( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    return arbiter->state;
}


extern void ezbus_mac_arbiter_bootstrap ( ezbus_mac_t* mac)
{
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_boot0_restart );
}

extern void ezbus_mac_arbiter_warm_bootstrap( ezbus_mac_t* mac)
{
    EZBUS_LOG( EZBUS_LOG_ARBITER, "" );
    ezbus_mac_boot0_deinit( mac );
    ezbus_mac_boot1_deinit( mac );
    ezbus_mac_boot2_deinit( mac );
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_boot1_start );
}

static bool ezbus_mac_arbiter_boot_filter( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    ezbus_peer_t peer;

    uint8_t seq = ezbus_packet_seq( packet );
    ezbus_address_t* src = ezbus_packet_src( packet );
    ezbus_address_t* dst = ezbus_packet_dst( packet );

    ezbus_peer_init( &peer, src, seq );
    ezbus_mac_peers_insort( mac, &peer );

    ezbus_peer_init( &peer, dst, seq );
    ezbus_mac_peers_insort( mac, &peer );

    return true;
}

/*****************************************************************************
******************************************************************************
******************************************************************************
* BOOT 0 STATE (DORMANT)                                                     *
******************************************************************************
******************************************************************************
*****************************************************************************/

static void ezbus_mac_boot0_init( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot0_state_t* boot0 = &arbiter->boot0_state;

    ezbus_mac_timer_setup( mac, &boot0->timer, true );
    ezbus_timer_set_key( &boot0->timer, "boot0_timer" );
    ezbus_timer_set_callback( &boot0->timer, 
                                ezbus_mac_boot0_timer_callback, 
                                mac );
}

static void ezbus_mac_boot0_deinit( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot0_state_t* boot0 = &arbiter->boot0_state;
    ezbus_timer_stop( &boot0->timer );
}

static void ezbus_mac_boot0_stop( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot0_state_t* boot0 = &arbiter->boot0_state;
    ezbus_timer_stop( &boot0->timer );
}

static bool ezbus_mac_arbiter_in_boot0_wait( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_state_t arbiter_state = ezbus_mac_arbiter_get_state( mac );
    return ( (int)arbiter_state >= (int)mac_arbiter_state_boot0_restart &&
             (int)arbiter_state <= (int)mac_arbiter_state_boot0_active );
}

static void ezbus_mac_arbiter_boot0_reset( ezbus_mac_t* mac )
{
    if ( ezbus_mac_arbiter_in_boot0_wait( mac ) )
    {
        /*********************************************************************
        * @brief we want to reset boot0 timer upon most any packet rx        *
        *********************************************************************/
        ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
        ezbus_mac_boot0_state_t* boot0 = &arbiter->boot0_state;
        ezbus_timer_stop( &boot0->timer );
    }
}

static void do_mac_arbiter_state_boot0_restart( ezbus_mac_t* mac )
{
    ezbus_mac_boot2_deinit( mac );
    ezbus_mac_boot1_deinit( mac );
    ezbus_mac_boot0_deinit( mac );
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_boot0_start );
}

static void do_mac_arbiter_state_boot0_start( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot0_state_t* boot0 = &arbiter->boot0_state;

    ezbus_timer_stop( &boot0->timer );

    ezbus_timer_set_period( &boot0->timer, 
                                ezbus_mac_token_ring_time(mac) + 
                                EZBUS_BOOT0_TIME );

    ezbus_timer_start( &boot0->timer );
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_boot0_active );
}

static void do_mac_arbiter_state_boot0_active( ezbus_mac_t* mac )
{
    /*************************************************************************
    * @brief remain in boot0 state while receiving non-boot traffic,     *
    *        rx boot1                                                        *
    *************************************************************************/
}

extern void ezbus_mac_boot0_timer_callback( ezbus_timer_t* timer, void* arg )
{
    /*************************************************************************
    * @brief remain in boot0 state while receiving non-boot traffic,     *
    *        rx boot1                                                        *
    *************************************************************************/
    ezbus_mac_t* mac=(ezbus_mac_t*)arg;
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot0_state_t* boot0 = &arbiter->boot0_state;
    
    EZBUS_LOG( EZBUS_LOG_ARBITER, "" );

    ezbus_timer_stop( &boot0->timer );
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_boot1_start );
}

/*****************************************************************************
******************************************************************************
******************************************************************************
* BOOT 1 STATE                                                               *
******************************************************************************
******************************************************************************
*****************************************************************************/

extern void ezbus_mac_boot1_init( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot1_state_t* boot1 = &arbiter->boot1_state;

    ezbus_mac_timer_setup( mac, &boot1->timer, true );
    ezbus_timer_set_key( &boot1->timer, "boot1_timer" );    
    ezbus_timer_set_callback( &boot1->timer, ezbus_mac_boot1_timer_callback, mac );
}

static void ezbus_mac_boot1_deinit( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot1_state_t* boot1 = &arbiter->boot1_state;
    ezbus_timer_stop( &boot1->timer );
}

static void ezbus_mac_boot1_stop( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot1_state_t* boot1 = &arbiter->boot1_state;
    ezbus_timer_stop( &boot1->timer );
}

static void do_mac_arbiter_state_boot1_start( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot1_state_t* boot1 = &arbiter->boot1_state;

    ezbus_timer_stop( &boot1->timer );
    ezbus_mac_peers_clear( mac );
    ezbus_timer_set_period  (
                                &boot1->timer,
                                ezbus_platform.callback_random( EZBUS_BOOT1_TIMER_MIN, EZBUS_BOOT1_TIMER_MAX )
                            );
    
    ezbus_timer_start( &boot1->timer );
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_boot1_active );
}

static void do_mac_arbiter_state_boot1_active( ezbus_mac_t* mac )
{
    /*************************************************************************
    * @brief If I'm the "last man standing" then seize control of the bus    *
    *************************************************************************/
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot1_state_t* boot1 = &arbiter->boot1_state;
    ++boot1->seq;
    if ( ezbus_mac_boot1_get_emit_count( boot1 ) > EZBUS_BOOT1_CYCLES )
    {
        ezbus_timer_stop( &boot1->timer );
        ezbus_mac_boot1_set_emit_count( boot1, 0 );
        ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_boot1_dominant );
    }
}

static void do_mac_arbiter_state_boot1_stop( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot1_state_t* boot1 = &arbiter->boot1_state;

    ezbus_timer_stop( &boot1->timer );
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_boot1_stopped );
}

static void do_mac_arbiter_state_boot1_stopped( ezbus_mac_t* mac )
{
    /*************************************************************************
    * @brief just wait for external event to change state                    *
    *************************************************************************/
}

static void do_mac_arbiter_state_boot1_dominant( ezbus_mac_t* mac )
{
    /*************************************************************************
    * @brief Get here once boot1 has determined that we have the          *
    *        dominant address.                                               *
    *************************************************************************/
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot1_state_t* boot1 = &arbiter->boot1_state;
    ++boot1->seq;
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_boot2_restart );
}

extern uint8_t ezbus_mac_boot1_get_seq( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot1_state_t* boot1 = &arbiter->boot1_state;
    return boot1->seq;
}

/*****************************************************************************
* BOOT 1 TIMERS                                                              *
*****************************************************************************/

static void ezbus_mac_boot1_timer_callback( ezbus_timer_t* timer, void* arg )
{
    ezbus_mac_t* mac = (ezbus_mac_t*)arg;
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot1_state_t* boot1 = &arbiter->boot1_state;
    
    EZBUS_LOG( EZBUS_LOG_ARBITER, "" );

    if ( ezbus_mac_transmitter_empty( mac ) )
    {
        ezbus_packet_t packet;

        ezbus_packet_init           ( &packet );
        ezbus_packet_set_type       ( &packet, packet_type_boot1 );
        ezbus_packet_set_seq        ( &packet, ezbus_mac_boot1_get_seq( mac ) );
        ezbus_packet_set_dst_socket ( &packet, EZBUS_SOCKET_ANY );
        ezbus_packet_set_src_socket ( &packet, EZBUS_SOCKET_ANY );
        ezbus_packet_set_src        ( &packet, ezbus_port_get_address(ezbus_mac_get_port(mac)) );

        EZBUS_LOG( EZBUS_LOG_BOOTSTATE, "%cboot1> %s %3d | ", ezbus_mac_token_acquired(mac)?'*':' ', ezbus_address_string( ezbus_packet_src( &packet ) ), ezbus_packet_seq( &packet ) );

        ezbus_mac_transmitter_put( mac, &packet );
    }
    ezbus_mac_boot1_inc_emit_count( boot1 );


    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_boot1_start );
}

/*****************************************************************************
* BOOT 1 RECEIVER                                                            *
*****************************************************************************/

static void do_receiver_packet_type_boot1( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    EZBUS_LOG( EZBUS_LOG_ARBITER, "" );
    ezbus_peer_t peer;
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );

    ezbus_mac_boot0_stop( mac );
    ezbus_mac_boot1_stop( mac );
    ezbus_mac_boot2_stop( mac );

    EZBUS_LOG( EZBUS_LOG_ARBITER, "%ccoldboot <%s %3d | ", ezbus_mac_token_acquired(mac)?'*':' ', ezbus_address_string( ezbus_packet_src( packet ) ), ezbus_packet_seq( packet ) );
    
    arbiter->boot2_state.seq=0;
    ezbus_peer_init( &peer, ezbus_packet_src( packet ), ezbus_packet_seq( packet ) );

    if ( ezbus_address_compare( ezbus_port_get_address(ezbus_mac_get_port(mac)), ezbus_packet_src( packet ) ) > 0 )
    {
        ezbus_mac_arbiter_bootstrap( mac );
    }
}


/*****************************************************************************
******************************************************************************
******************************************************************************
* BOOT 2 STATE                                                               *
******************************************************************************
******************************************************************************
*****************************************************************************/


extern void ezbus_mac_boot2_init( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot2_state_t* boot2 = &arbiter->boot2_state;

    ezbus_mac_arbiter_set_boot2_seq( mac, 1 );

    ezbus_mac_timer_setup   ( mac, &boot2->timeout_timer, true );
    ezbus_timer_set_key     ( &boot2->timeout_timer, "timeout_timer" );
    ezbus_timer_set_callback( &boot2->timeout_timer, ezbus_mac_boot2_timer_callback, mac );
    ezbus_timer_set_period  ( &boot2->timeout_timer, EZBUS_BOOT2_TIMER_PERIOD );

    ezbus_mac_timer_setup   ( mac, &boot2->reply_timer, true );
    ezbus_timer_set_key     ( &boot2->reply_timer, "reply_timer" );
    ezbus_timer_set_callback( &boot2->reply_timer, ezbus_mac_arbiter_boot2_send_reply, mac );

}

static void ezbus_mac_boot2_deinit( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot2_state_t* boot2 = &arbiter->boot2_state;

    ezbus_timer_stop( &boot2->timeout_timer );
    ezbus_mac_peers_clear( mac );
}

static bool ezbus_mac_boot2_is_active( ezbus_mac_t* mac )
{
    return ( (int)ezbus_mac_arbiter_get_state(mac) >= (int)mac_arbiter_state_boot2_idle &&
             (int)ezbus_mac_arbiter_get_state(mac) <  (int)mac_arbiter_state_boot2_finished );
}

static void ezbus_mac_boot2_stop( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot2_state_t* boot2 = &arbiter->boot2_state;
    ezbus_timer_stop( &boot2->timeout_timer );
    ezbus_timer_stop( &boot2->reply_timer );
}

/*****************************************************************************
* BOOT 2 COUNTERS                                                            *
*****************************************************************************/

static uint8_t ezbus_mac_arbiter_set_boot2_seq ( ezbus_mac_t* mac, uint8_t seq )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot2_state_t* boot2 = &arbiter->boot2_state;

    return (boot2->seq = seq);
}

static uint8_t ezbus_mac_arbiter_get_boot2_seq ( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot2_state_t* boot2 = &arbiter->boot2_state;
    return boot2->seq;
}

static uint8_t ezbus_mac_arbiter_inc_boot2_seq ( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot2_state_t* boot2 = &arbiter->boot2_state;
    if ( ++boot2->seq == 0 )
        boot2->seq=1;    /* always != 0 */
    return boot2->seq;
}

extern uint8_t ezbus_mac_arbiter_get_boot2_cycles( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot2_state_t* boot2 = &arbiter->boot2_state;
    return boot2->cycles;
}

extern void ezbus_mac_arbiter_set_boot2_cycles( ezbus_mac_t* mac, uint8_t cycles )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot2_state_t* boot2 = &arbiter->boot2_state;
    boot2->cycles = cycles;
}

extern void ezbus_mac_arbiter_dec_boot2_cycles( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );\
    ezbus_mac_boot2_state_t* boot2 = &arbiter->boot2_state;

    if ( boot2->cycles )
    {
        --boot2->cycles;
    }
}

/*****************************************************************************
* BOOT 2 HANDLERS                                                            *
*****************************************************************************/

static void do_mac_arbiter_state_boot2_idle( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot2_state_t* boot2 = &arbiter->boot2_state;
    ezbus_timer_stop( &boot2->timeout_timer );
}

static void do_mac_arbiter_state_boot2_restart( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot1_state_t* boot1 = &arbiter->boot1_state;

    ezbus_timer_stop( &boot1->timer );

    ezbus_mac_arbiter_rst_boot2_cycles( mac );

    ezbus_mac_boot2_deinit( mac );
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_boot2_start );
}

static void do_mac_arbiter_state_boot2_start( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot1_state_t* boot1 = &arbiter->boot1_state;
    ezbus_mac_boot2_state_t* boot2 = &arbiter->boot2_state;
    
    ezbus_timer_stop( &boot1->timer );
    ezbus_timer_stop( &boot2->reply_timer );

    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_boot2_active );
    
    ezbus_timer_restart( &boot2->timeout_timer );
}

static void do_mac_arbiter_state_boot2_active( ezbus_mac_t* mac )
{
    /* wait timer(s) */
}

static void do_mac_arbiter_state_boot2_stop( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot2_state_t* boot2 = &arbiter->boot2_state;

    ezbus_timer_stop( &boot2->timeout_timer );
    if ( ezbus_mac_transmitter_empty( mac ) )
    {
        ezbus_packet_t packet;

        EZBUS_LOG( EZBUS_LOG_ARBITER, "seq %d", ezbus_mac_arbiter_get_boot2_seq( mac ) );

        ezbus_packet_init           ( &packet );
        ezbus_packet_set_type       ( &packet, packet_type_boot2_rq );
        ezbus_packet_set_dst_socket ( &packet, EZBUS_SOCKET_ANY );
        ezbus_packet_set_src_socket ( &packet, EZBUS_SOCKET_ANY );
        ezbus_packet_set_seq        ( &packet, ezbus_mac_arbiter_get_boot2_seq( mac ) );
        ezbus_packet_set_src        ( &packet, ezbus_port_get_address(ezbus_mac_get_port(mac)) );

        ezbus_mac_transmitter_put( mac, &packet );
    }
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_boot2_start );
}

static void do_mac_arbiter_state_boot2_finished( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot2_state_t* boot2 = &arbiter->boot2_state;

    ezbus_mac_arbiter_inc_boot2_seq( mac );
    ezbus_timer_stop( &boot2->timeout_timer );
    ezbus_mac_arbiter_rst_boot2_cycles( mac );
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_service_start );
    ezbus_mac_token_acquire( mac );
    ezbus_mac_token_reset( mac );
}

extern void ezbus_mac_arbiter_rst_boot2_cycles( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot2_state_t* boot2 = &arbiter->boot2_state;
    boot2->cycles = EZBUS_BOOT2_CYCLES;
}

/*****************************************************************************
* BOOT 2 TIMERS                                                              *
*****************************************************************************/

static void ezbus_mac_boot2_timer_callback( ezbus_timer_t* timer, void* arg )
{
    ezbus_mac_t* mac = (ezbus_mac_t*)arg;

    ezbus_timer_stop( timer );

    ezbus_mac_arbiter_dec_boot2_cycles( mac );

    EZBUS_LOG( EZBUS_LOG_ARBITER, "cycles = %d", ezbus_mac_arbiter_get_boot2_cycles( mac ) );

    if ( ezbus_mac_arbiter_get_boot2_cycles( mac ) == 0 )
    {
        ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_boot2_finished );
    }
    else
    {
        ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_boot2_stop );
    }
}

static void ezbus_mac_arbiter_boot2_send_reply( ezbus_timer_t* timer, void* arg )
{
    ezbus_packet_t tx_packet;
    ezbus_mac_t* mac = (ezbus_mac_t*)arg;
    ezbus_packet_t* rx_packet = ezbus_mac_get_receiver_packet( mac );

    ezbus_timer_stop( timer );

    EZBUS_LOG( EZBUS_LOG_ARBITER, "seq %d", ezbus_packet_seq( rx_packet ) );

    ezbus_packet_init           ( &tx_packet );
    ezbus_packet_set_type       ( &tx_packet, packet_type_boot2_rp );
    ezbus_packet_set_dst_socket ( &tx_packet, EZBUS_SOCKET_ANY  );
    ezbus_packet_set_src_socket ( &tx_packet, EZBUS_SOCKET_ANY  );
    ezbus_packet_set_seq        ( &tx_packet, ezbus_packet_seq( rx_packet ) );
    ezbus_packet_set_src        ( &tx_packet, ezbus_port_get_address(ezbus_mac_get_port(mac)) );
    ezbus_packet_set_dst        ( &tx_packet, ezbus_packet_src( rx_packet ) );

    ezbus_mac_transmitter_put( mac, &tx_packet );
}

/*****************************************************************************
* BOOT 2 RECEIVERS                                                           *
*****************************************************************************/

static void do_receiver_packet_type_boot2_rq( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    /*************************************************************************
    * @brief Receive a wb request from src, and this node's wb seq# does not *
    * match the rx seq#, then we must reply. If the seq# matches, then we've *
    * already been acknowledged during this session identified by seq#.      *
    *************************************************************************/
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot2_state_t* boot2 = &arbiter->boot2_state;

    if ( ezbus_address_is_broadcast( ezbus_packet_dst(packet) ) )
    {
        EZBUS_LOG( EZBUS_LOG_ARBITER, "%d != %d ?", ezbus_mac_arbiter_get_boot2_seq( mac ), ezbus_packet_seq( packet ) );

        if ( ezbus_mac_arbiter_get_boot2_seq( mac ) != ezbus_packet_seq( packet ) )
        {
            ezbus_timer_stop( &boot2->reply_timer );
            ezbus_timer_set_period  ( 
                                        &boot2->reply_timer, 
                                        ezbus_platform.callback_random( EZBUS_BOOT2_TIMER_MIN, EZBUS_BOOT2_TIMER_MAX ) 
                                    );
            ezbus_timer_restart( &boot2->reply_timer );
        }
        else
        {
            ezbus_timer_stop( &boot2->reply_timer );
        }
    }
}


static void do_receiver_packet_type_boot2_rp( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    /*************************************************************************
    * @brief I am the src of the boot2, and a node has replied.              *
    *************************************************************************/
    if ( ezbus_port_get_address_is_self( ezbus_mac_get_port(mac), ezbus_packet_dst( packet ) ) )
    {   
        /*********************************************************************
        * @brief Are we talking about the same boot2 sequence?               *
        *********************************************************************/
        if ( ezbus_mac_arbiter_get_boot2_seq( mac ) == ezbus_packet_seq( packet ) )
        {
            ezbus_mac_arbiter_boot2_send_ack( mac, packet );
        }
    }
}

static void do_receiver_packet_type_boot2_ak( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    /*************************************************************************
    * @brief Receive an wb acknolegment from src, and disable replying to    *
    *        this wb sequence#                                               *
    *************************************************************************/
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot2_state_t* boot2 = &arbiter->boot2_state;

    if ( ezbus_port_get_address_is_self( ezbus_mac_get_port(mac), ezbus_packet_dst( packet ) ) )
    {      
        /*********************************************************************
        * @brief Are we talking about the same boot2 sequence?               *
        *********************************************************************/
        if ( ezbus_mac_arbiter_get_boot2_seq( mac ) == ezbus_packet_seq( packet ) )
        {
            /*****************************************************************
            * @brief acknowledged, stop replying to this seq#                *
            *****************************************************************/
            ezbus_mac_arbiter_set_boot2_seq(mac, ezbus_packet_seq( packet ) );
            ezbus_timer_stop( &boot2->reply_timer );
        }
    }
}


static void ezbus_mac_arbiter_boot2_send_ack( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    ezbus_packet_t tx_packet;
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot2_state_t* boot2 = &arbiter->boot2_state;

    ezbus_timer_stop( &boot2->reply_timer );

    ezbus_packet_init           ( &tx_packet );
    ezbus_packet_set_type       ( &tx_packet, packet_type_boot2_ak );
    ezbus_packet_set_dst_socket ( &tx_packet, EZBUS_SOCKET_ANY  );
    ezbus_packet_set_src_socket ( &tx_packet, EZBUS_SOCKET_ANY  );
    ezbus_packet_set_seq        ( &tx_packet, ezbus_packet_seq( packet ) );
    ezbus_packet_set_src        ( &tx_packet, ezbus_port_get_address(ezbus_mac_get_port(mac)) );
    ezbus_packet_set_dst        ( &tx_packet, ezbus_packet_src( packet ) );

    ezbus_mac_transmitter_put( mac, &tx_packet );
}

/*****************************************************************************
******************************************************************************
******************************************************************************
* ONLINE STATE                                                               *
******************************************************************************
******************************************************************************
*****************************************************************************/


extern bool ezbus_mac_arbiter_callback( ezbus_mac_t* mac )
{
    switch( ezbus_mac_pause_get_state( mac ) )
    {
        case ezbus_pause_state_start:
            {
                ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
                arbiter->pre_pause_state = ezbus_mac_arbiter_get_state( mac );
                arbiter->state = mac_arbiter_state_pause;
                // fprintf( stderr, "A" );
            }
            break;
        case ezbus_pause_state_finish:
            {
                ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
                arbiter->state = arbiter->pre_pause_state;
                // fprintf( stderr, "B" );
            }
            break;
        default:
            break;
    }
    return true;
}

extern bool ezbus_mac_arbiter_online( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_state_t mac_arbiter_state = ezbus_mac_arbiter_get_state( mac );
    return (mac_arbiter_state == mac_arbiter_state_online);
}

extern uint16_t ezbus_mac_arbiter_get_token_age( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    return arbiter->token_age;
}

extern void ezbus_mac_arbiter_set_token_age( ezbus_mac_t* mac, uint16_t age )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    arbiter->token_age = age;
}

static void do_mac_arbiter_state_offline( ezbus_mac_t* mac )
{
    ezbus_mac_token_relinquish( mac );
}

static void do_mac_arbiter_state_service_start( ezbus_mac_t* mac )
{
    EZBUS_LOG( EZBUS_LOG_ARBITER, "%d %d", ezbus_mac_token_acquired( mac ), ezbus_mac_pause_active( mac ) );
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_online );
}

static void do_mac_arbiter_state_pause( ezbus_mac_t* mac )
{
   /* @note do nothing */
}

static void do_mac_arbiter_state_online( ezbus_mac_t* mac )
{
    ezbus_socket_callback_run( mac );
    if ( ezbus_mac_token_acquired( mac ) && !ezbus_mac_pause_active( mac ) )
    {
        ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );

        EZBUS_LOG( EZBUS_LOG_ONLINE, "rx_ack_pend %d rx_nack_pend %d", arbiter->rx_ack_pend, arbiter->rx_nack_pend );

        if ( ezbus_mac_arbiter_ready_to_ack(mac) )              ezbus_mac_arbiter_send_ack_parcel(mac)
        else if ( ezbus_mac_arbiter_ready_to_nack(mac) )        ezbus_mac_arbiter_send_nack_parcel(mac)
        else if ( ezbus_mac_arbiter_ready_to_give_token(mac) )  ezbus_mac_arbiter_give_token(mac)
        else if ( ezbus_mac_arbiter_transmitter_ready(mac) && !ezbus_socket_callback_transmitter_empty(mac) )
        {
            // fprintf( stderr, " X4 %d", ezbus_mac_arbiter_get_token_age(mac) );
            if ( ezbus_mac_arbiter_get_token_age(mac) > EZBUS_BOOT2_AGE && ezbus_mac_arbiter_transmitter_ready(mac) )
            {
                ezbus_mac_arbiter_set_token_age( mac, 0 );
                //fprintf(stderr," X5"); // invite
                EZBUS_LOG( EZBUS_LOG_ARBITER, "initiate mac_arbiter_state_reboot_boot2 - token age" );
                ezbus_mac_arbiter_warm_bootstrap( mac );
            }
        }
    }
}

static bool ezbus_mac_arbiter_receive_token( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_crc_t crc;

    arbiter->token_hold=0;
    ezbus_mac_peers_crc( mac, &crc );
    if ( ezbus_crc_equal( &crc, ezbus_packet_get_token_crc( packet ) ) )
    {
        ezbus_mac_token_acquire( mac );
        if ( ezbus_mac_boot2_is_active( mac ) )
        {
            ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_boot2_finished );
        }
        return true;
    }
    else
    {
        EZBUS_LOG( EZBUS_LOG_ARBITER, "bad token crc -> boot2" );
        ezbus_mac_arbiter_warm_bootstrap( mac );
    }
    return false;
}


/*****************************************************************************
******************************************************************************
******************************************************************************
* PARCEL                                                                     *
******************************************************************************
******************************************************************************
*****************************************************************************/

static void ezbus_mac_arbiter_ack_parcel( ezbus_mac_t* mac, uint8_t seq, ezbus_address_t* address )
{
    ezbus_packet_t tx_packet;
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_packet_t* rx_packet  = ezbus_mac_get_receiver_packet( mac );

    ezbus_packet_init           ( &tx_packet );
    ezbus_packet_set_type       ( &tx_packet, packet_type_ack );
    ezbus_packet_set_dst_socket ( &tx_packet, arbiter->rx_ack_src_socket );
    ezbus_packet_set_src_socket ( &tx_packet, arbiter->rx_ack_dst_socket );
    ezbus_packet_set_seq        ( &tx_packet, ezbus_packet_seq( rx_packet ) );
    ezbus_packet_set_src        ( &tx_packet, ezbus_port_get_address(ezbus_mac_get_port(mac)) );
    ezbus_packet_set_dst        ( &tx_packet, ezbus_packet_src( rx_packet ) );

    ezbus_mac_transmitter_put( mac, &tx_packet );
}

static void ezbus_mac_arbiter_nack_parcel( ezbus_mac_t* mac, uint8_t seq, ezbus_address_t* address )
{
    ezbus_packet_t tx_packet;
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_packet_t* rx_packet  = ezbus_mac_get_receiver_packet( mac );

    ezbus_packet_init           ( &tx_packet );
    ezbus_packet_set_type       ( &tx_packet, packet_type_nack );
    ezbus_packet_set_dst_socket ( &tx_packet, arbiter->rx_nack_src_socket );
    ezbus_packet_set_src_socket ( &tx_packet, arbiter->rx_nack_dst_socket );
    ezbus_packet_set_seq        ( &tx_packet, ezbus_packet_seq( rx_packet ) );
    ezbus_packet_set_src        ( &tx_packet, ezbus_port_get_address(ezbus_mac_get_port(mac)) );
    ezbus_packet_set_dst        ( &tx_packet, ezbus_packet_src( rx_packet ) );

    ezbus_mac_transmitter_put( mac, &tx_packet );
}


/*****************************************************************************
******************************************************************************
******************************************************************************
* RECEIVER                                                                   *
******************************************************************************
******************************************************************************
*****************************************************************************/

static void ezbus_mac_arbiter_receive_init  ( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    arbiter->receiver_filter = NULL;
}

static void ezbus_mac_arbiter_receive_set_filter(ezbus_mac_t* mac, mac_arbiter_recieve_callback_t receiver_filter)
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    arbiter->receiver_filter = receiver_filter;
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

    ezbus_mac_token_reset( mac );
    
    if ( ezbus_port_get_address_is_self( ezbus_mac_get_port(mac), ezbus_packet_dst( packet ) ) )
    {
        if ( ezbus_mac_arbiter_receive_token( mac, packet ) )
        {
            if ( !ezbus_mac_arbiter_online( mac ) )
            {
                ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_service_start );
            }
        }
    }
}

static void do_receiver_packet_type_parcel( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    if ( ezbus_port_get_address_is_self( ezbus_mac_get_port(mac), ezbus_packet_dst( packet ) ) )
    {
        if ( ezbus_port_get_address_is_self( ezbus_mac_get_port(mac), ezbus_packet_dst( packet ) ) )
        {
            ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
            ezbus_packet_t* rx_packet = ezbus_mac_get_receiver_packet( mac );

            EZBUS_LOG( EZBUS_LOG_RECEIVER, "" );
        
            if ( !arbiter->rx_ack_pend && !arbiter->rx_nack_pend )
            {
                if ( ezbus_socket_callback_receiver_ready( mac, packet ) )
                {
                    arbiter->rx_ack_pend = (ezbus_packet_src_socket( rx_packet ) == EZBUS_SOCKET_INVALID) ? false : true;
                    arbiter->rx_ack_seq = ezbus_packet_seq( rx_packet );
                    arbiter->rx_ack_dst_socket = ezbus_packet_dst_socket( rx_packet );
                    arbiter->rx_ack_src_socket = ezbus_packet_src_socket( rx_packet );
                    ezbus_address_copy( &arbiter->rx_ack_address, ezbus_packet_src( rx_packet ) );
                }
                else
                {
                    arbiter->rx_nack_pend = (ezbus_packet_src_socket( rx_packet ) == EZBUS_SOCKET_INVALID) ? false : true;
                    arbiter->rx_nack_seq = ezbus_packet_seq( rx_packet );         
                    arbiter->rx_nack_dst_socket = ezbus_packet_dst_socket( rx_packet );
                    arbiter->rx_nack_src_socket = ezbus_packet_src_socket( rx_packet );
                    ezbus_address_copy( &arbiter->rx_nack_address, ezbus_packet_src( rx_packet ) );
                }
            }
        }
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


extern void ezbus_mac_receiver_signal_full( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_packet_t* packet  = ezbus_mac_get_receiver_packet( mac );
    
    EZBUS_LOG( EZBUS_LOG_ARBITER, "" );

    ezbus_mac_arbiter_boot0_reset( mac );

    if ( arbiter->receiver_filter == NULL || 
         (arbiter->receiver_filter != NULL && 
            arbiter->receiver_filter(mac,packet) ) )
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
            case packet_type_boot1:       do_receiver_packet_type_boot1       ( mac, packet ); break;
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

extern void ezbus_mac_receiver_signal_fault( ezbus_mac_t* mac )
{
    if ( ezbus_mac_receiver_get_err( mac ) != EZBUS_ERR_NOTREADY ) // not_ready means rx empty.
    {
        EZBUS_LOG( EZBUS_LOG_RECEIVER, "%s",ezbus_fault_str( ezbus_mac_receiver_get_err( mac ) ) );
    }
}

