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
#include <ezbus_mac_arbiter_receive.h>
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

#define ezbus_mac_coldboot_set_emit_count(boot,c)   ((boot)->emit_count=(c))
#define ezbus_mac_coldboot_get_emit_count(boot)     ((boot)->emit_count)
#define ezbus_mac_coldboot_inc_emit_count(boot)     ezbus_mac_coldboot_set_emit_count(boot,ezbus_mac_coldboot_get_emit_count(boot)+1)

#define ezbus_mac_coldboot_set_emit_seq(boot,c)     ((boot)->emit_count=(c))
#define ezbus_mac_coldboot_get_emit_seq(boot)       ((boot)->emit_count)
#define ezbus_mac_coldboot_inc_emit_seq(boot)       ezbus_mac_coldboot_set_emit_count(boot,ezbus_mac_coldboot_get_emit_count(boot)+1)

/****************************************************************************/

/* boot 0 */
static void ezbus_mac_boot0_init                ( ezbus_mac_t* mac );
static void ezbus_mac_boot0_deinit              ( ezbus_mac_t* mac );
static bool ezbus_mac_boot0_is_active           ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_boot0_restart  ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_boot0_start    ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_boot0_active   ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_boot0_stop     ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_boot0_stopped  ( ezbus_mac_t* mac );

/* boot 1 */
static void ezbus_mac_boot1_init                ( ezbus_mac_t* mac );
static void ezbus_mac_boot1_deinit              ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_boot1_stop     ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_boot1_stopped  ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_boot1_start    ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_boot1_active   ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_boot1_dominant ( ezbus_mac_t* mac );

/* boot 2 */
static void ezbus_mac_boot2_init                ( ezbus_mac_t* mac );
static void ezbus_mac_boot2_deinit              ( ezbus_mac_t* mac );
static bool ezbus_mac_boot2_is_active           ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_boot2_idle     ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_boot2_restart  ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_boot2_start    ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_boot2_active   ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_boot2_stop     ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_boot2_finished ( ezbus_mac_t* mac );

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

/* signals */
static void ezbus_mac_boot1_signal_active       ( ezbus_mac_t* mac );
static void ezbus_mac_boot2_signal_stop         ( ezbus_mac_t* mac );
static void ezbus_mac_boot2_signal_finished     ( ezbus_mac_t* mac );


extern void  ezbus_mac_arbiter_init ( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );

    ezbus_platform.callback_memset( arbiter, 0 , sizeof( ezbus_mac_arbiter_t) );
    ezbus_mac_arbiter_rst_boot2_cycles( mac );

    ezbus_mac_boot0_init( mac );
    ezbus_mac_boot1_init( mac );
    ezbus_mac_boot2_init( mac );
}

extern void ezbus_mac_arbiter_run( ezbus_mac_t* mac )
{
    switch( ezbus_mac_arbiter_get_state( mac ) )
    {
        /* boot 0 */
        case mac_arbiter_state_boot0_restart:   do_mac_arbiter_state_boot0_restart( mac );  break;
        case mac_arbiter_state_boot0_start:     do_mac_arbiter_state_boot0_start( mac );    break;
        case mac_arbiter_state_boot0_active:    do_mac_arbiter_state_boot0_active( mac );   break;
        case mac_arbiter_state_boot0_stop:      do_mac_arbiter_state_boot0_stop( mac );     break;
        case mac_arbiter_state_boot0_stopped:   do_mac_arbiter_state_boot0_stopped( mac );  break;

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

extern void ezbus_mac_arbiter_bootstrap ( ezbus_mac_t* mac)
{
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_boot0_restart );

}

extern void ezbus_mac_arbiter_warm_bootstrap( ezbus_mac_t* mac)
{
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_boot2_restart );
}

/*****************************************************************************
******************************************************************************
******************************************************************************
* BOOT 0 STATE                                                               *
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

static bool ezbus_mac_boot0_is_active( ezbus_mac_t* mac )
{
    return (ezbus_mac_arbiter_get_state( mac ) != mac_arbiter_state_boot0_stop &&
            ezbus_mac_arbiter_get_state( mac ) != mac_arbiter_state_boot0_stopped);
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
    * @brief just wait for timer expire or forced to change boot level by    *
    *        rx boot1                                                        *
    *************************************************************************/
}

static void do_mac_arbiter_state_boot0_stop( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot0_state_t* boot0 = &arbiter->boot0_state;
    ezbus_timer_stop( &boot0->timer );
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_boot0_stopped );
}

static void do_mac_arbiter_state_boot0_stopped( ezbus_mac_t* mac )
{
    /*************************************************************************
    * @brief just wait for external event to change state                    *
    *************************************************************************/
}

extern void ezbus_mac_boot0_timer_callback( ezbus_timer_t* timer, void* arg )
{
    ezbus_mac_t* mac=(ezbus_mac_t*)arg;

    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_boot0_stop );
}

/*****************************************************************************
* BOOT 0 SIGNALS                                                             *
*****************************************************************************/

extern void ezbus_mac_arbiter_receive_signal_boot0( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    /*************************************************************************
    * @brief Determin of the incoming packet source address is superior      *
    *        to self port address, and of it is, then go dorman.             *
    **************************************************************************/
    const ezbus_address_t* self_address = ezbus_port_get_address(ezbus_mac_get_port(mac));
    ezbus_address_t* src_address = ezbus_packet_src( packet );
    if ( ezbus_address_compare( self_address, src_address ) > 0 )
    {
        ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_boot0_restart );
    }
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
    if ( ezbus_mac_coldboot_get_emit_count( boot1 ) > EZBUS_BOOT1_CYCLES )
    {
        ezbus_timer_stop( &boot1->timer );
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
    * @brief Get here once coldboot has determined that we have the          *
    *        dominant address.                                               *
    *************************************************************************/
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot1_state_t* boot1 = &arbiter->boot1_state;
    ++boot1->seq;
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_boot2_restart );
}

extern uint8_t ezbus_mac_coldboot_get_seq( ezbus_mac_t* mac )
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
    
    ezbus_mac_boot1_signal_active( mac );
    ezbus_mac_coldboot_inc_emit_count( boot1 );
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_boot1_start );
}

/*****************************************************************************
* BOOT 1 SIGNALS                                                             *
*****************************************************************************/

static void  ezbus_mac_boot1_signal_active( ezbus_mac_t* mac )
{
    if ( ezbus_mac_transmitter_empty( mac ) )
    {
        ezbus_packet_t packet;

        EZBUS_LOG( EZBUS_LOG_BOOT1, "" );

        ezbus_packet_init           ( &packet );
        ezbus_packet_set_type       ( &packet, packet_type_coldboot );
        ezbus_packet_set_seq        ( &packet, ezbus_mac_coldboot_get_seq( mac ) );
        ezbus_packet_set_dst_socket ( &packet, EZBUS_SOCKET_ANY );
        ezbus_packet_set_src_socket ( &packet, EZBUS_SOCKET_ANY );
        ezbus_packet_set_src        ( &packet, ezbus_port_get_address(ezbus_mac_get_port(mac)) );

        EZBUS_LOG( EZBUS_LOG_BOOTSTATE, "%coldboot> %s %3d | ", ezbus_mac_token_acquired(mac)?'*':' ', ezbus_address_string( ezbus_packet_src( &packet ) ), ezbus_packet_seq( &packet ) );

        ezbus_mac_transmitter_put( mac, &packet );
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

    boot2->seq=1;

    ezbus_mac_timer_setup   ( mac, &boot2->timer, true );
    ezbus_timer_set_key     ( &boot2->timer, "boot2_timer" );
    ezbus_timer_set_callback( &boot2->timer, ezbus_mac_boot2_timer_callback, mac );
    ezbus_timer_set_period  ( &boot2->timer, EZBUS_BOOT2_TIMER_PERIOD );
}

static void ezbus_mac_boot2_deinit( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot2_state_t* boot2 = &arbiter->boot2_state;
    ezbus_timer_stop( &boot2->timer );
    ezbus_mac_arbiter_receive_set_boot2_seq( mac, 0 );
    ezbus_mac_peers_clear( mac );
}

static bool ezbus_mac_boot2_is_active( ezbus_mac_t* mac )
{
    return ( (int)ezbus_mac_arbiter_get_state(mac) >= (int)mac_arbiter_state_boot2_idle &&
             (int)ezbus_mac_arbiter_get_state(mac) <  (int)mac_arbiter_state_boot2_finished );
}

extern uint8_t ezbus_mac_boot2_get_seq( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot2_state_t* boot2 = &arbiter->boot2_state;
    return boot2->seq;
}

static void do_mac_arbiter_state_boot2_idle( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot2_state_t* boot2 = &arbiter->boot2_state;
    ezbus_timer_stop( &boot2->timer );
}

static void do_mac_arbiter_state_boot2_restart( ezbus_mac_t* mac )
{
    ezbus_mac_boot2_deinit( mac );
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_boot2_start );
}

static void do_mac_arbiter_state_boot2_start( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot2_state_t* boot2 = &arbiter->boot2_state;
    boot2->cycles = 0;
    ezbus_timer_stop( &boot2->timer );
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_boot2_active );
    ezbus_timer_start( &boot2->timer );
}

static void do_mac_arbiter_state_boot2_active( ezbus_mac_t* mac )
{
    /* wait timer(s) */
}

static void do_mac_arbiter_state_boot2_stop( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot2_state_t* boot2 = &arbiter->boot2_state;

    ezbus_timer_stop( &boot2->timer );
    ezbus_mac_boot2_signal_stop( mac );
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_boot2_start );
}

static void do_mac_arbiter_state_boot2_finished( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot2_state_t* boot2 = &arbiter->boot2_state;

    if ( ++boot2->seq == 0 )
        boot2->seq=1;    /* always != 0 */
    ezbus_timer_stop( &boot2->timer );
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_boot2_idle );
    ezbus_mac_boot2_signal_finished( mac );
}

extern uint8_t ezbus_mac_arbiter_get_boot2_cycles( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    return arbiter->boot2_cycles;
}

extern void ezbus_mac_arbiter_set_boot2_cycles( ezbus_mac_t* mac, uint8_t cycles )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    arbiter->boot2_cycles = cycles;
}

extern void ezbus_mac_arbiter_dec_boot2_cycles( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    if ( arbiter->boot2_cycles )
    {
        --arbiter->boot2_cycles;
    }
}

extern void ezbus_mac_arbiter_rst_boot2_cycles( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    arbiter->boot2_cycles = EZBUS_BOOT2_CYCLES;
}

/*****************************************************************************
* BOOT 2 TIMERS                                                              *
*****************************************************************************/

static void ezbus_mac_boot2_timer_callback( ezbus_timer_t* timer, void* arg )
{
    ezbus_mac_t* mac = (ezbus_mac_t*)arg;

    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_mac_boot2_state_t* boot2 = &arbiter->boot2_state;
    ezbus_timer_stop( &boot2->timer );

    ezbus_mac_arbiter_dec_boot2_cycles( mac );
    if ( ezbus_mac_arbiter_get_boot2_cycles( mac ) == 0 )
    {
        ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_boot2_finished );
    }
    else
    {
        ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_boot2_stop );
    }
}

/*****************************************************************************
* BOOT 2 SIGNALS                                                             *
*****************************************************************************/

static void ezbus_mac_boot2_signal_stop( ezbus_mac_t* mac )
{
    if ( ezbus_mac_transmitter_empty( mac ) )
    {
        ezbus_packet_t packet;

        EZBUS_LOG( EZBUS_LOG_BOOT2, "" );

        ezbus_packet_init           ( &packet );
        ezbus_packet_set_type       ( &packet, packet_type_boot2_rq );
        ezbus_packet_set_dst_socket ( &packet, EZBUS_SOCKET_ANY );
        ezbus_packet_set_src_socket ( &packet, EZBUS_SOCKET_ANY );
        ezbus_packet_set_seq        ( &packet, ezbus_mac_boot2_get_seq( mac ) );
        ezbus_packet_set_src        ( &packet, ezbus_port_get_address(ezbus_mac_get_port(mac)) );

        ezbus_mac_transmitter_put( mac, &packet );
    }
}

static void ezbus_mac_boot2_signal_finished( ezbus_mac_t* mac )
{
    /*************************************************************************
    * @brief boot2 has completed, this node has the token.                   *
    *************************************************************************/
    EZBUS_LOG( EZBUS_LOG_BOOT2, "" );
    ezbus_mac_arbiter_rst_boot2_cycles( mac );
    ezbus_mac_arbiter_set_state( mac, mac_arbiter_state_service_start );
    ezbus_mac_token_acquire( mac );
    ezbus_mac_token_reset( mac );
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


extern void ezbus_mac_arbiter_set_state ( ezbus_mac_t* mac, ezbus_mac_arbiter_state_t state )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    arbiter->state = state;
}

extern ezbus_mac_arbiter_state_t ezbus_mac_arbiter_get_state ( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    return arbiter->state;
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

/*****************************************************************************
* @brief A give-token packet has been received                               *
*****************************************************************************/
extern void ezbus_mac_arbiter_receive_signal_token ( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
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

static bool ezbus_mac_arbiter_receive_token( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );
    ezbus_crc_t crc;

    arbiter->token_hold=0;
    ezbus_mac_peers_crc( mac, &crc );
    // fprintf( stderr, " X3 %d", ezbus_mac_arbiter_get_token_age(mac) );
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
* parcel receive                                                             *
*****************************************************************************/

extern void ezbus_mac_arbiter_receive_signal_parcel( ezbus_mac_t* mac, ezbus_packet_t* packet )
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


extern void  ezbus_mac_token_signal_expired ( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_warm_bootstrap( mac );
    EZBUS_LOG( EZBUS_LOG_TOKEN, "" );
}

