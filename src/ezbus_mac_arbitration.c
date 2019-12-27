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
#include <ezbus_mac_arbitration.h>
#include <ezbus_mac_transmitter.h>
#include <ezbus_mac_receiver.h>
#include <ezbus_hex.h>
#include <ezbus_log.h>

static void ezbus_arbitration_ack_tx_timer_callback ( ezbus_timer_t* timer, void* arg );
static void ezbus_arbitration_ack_rx_timer_callback ( ezbus_timer_t* timer, void* arg );

static void ezbuz_mac_arbitration_run_timers        ( ezbus_mac_arbitration_t* mac_arbitration );
static void ezbus_mac_arbitration_receive           ( ezbus_mac_arbitration_t* mac_arbitration )


extern void  ezbus_mac_arbitration_init ( 
                                            ezbus_mac_arbitration_t* mac_arbitration, 
                                            ezbus_mac_transmitter_t* mac_transmitter, 
                                            ezbus_mac_receiver_t*    mac_receiver 
                                        )
{
    memset( mac_arbitration, 0 , sizeof( ezbus_mac_arbitration_t) );
    ezbus_mac_set_state( mac_arbitration, mac_state_offline );

    mac_arbitration->mac_transmitter = mac_transmitter;
    mac_arbitration->mac_receiver = mac_receiver;

    ezbus_timer_init( &mac_arbitration->ack_tx_timer );
    ezbus_timer_set_callback( &mac_arbitration->ack_tx_timer, ezbus_arbitration_ack_tx_timer_callback, mac_arbitration );

    ezbus_timer_init( &mac_arbitration->ack_rx_timer );
    ezbus_timer_set_callback( &mac_arbitration->ack_rx_timer, ezbus_arbitration_ack_rx_timer_callback, mac_arbitration );
}

static void ezbuz_mac_arbitration_run_timers( ezbus_mac_arbitration_t* mac_arbitration )
{
    ezbus_timer_run( &mac->ack_tx_timer );
    ezbus_timer_run( &mac->ack_rx_timer );
    ezbus_timer_run( &mac->warmboot_timer );    
}

extern void ezbus_mac_arbitration_run( ezbus_mac_arbitration_t* mac_arbitration )
{
    ezbus_timer_run( &mac->ack_tx_timer );
    ezbus_timer_run( &mac->ack_rx_timer );
    switch( ezbus_mac_get_state( mac ) )
    {
        case mac_state_offline:
            do_mac_state_offline( mac );
            break;
        case mac_state_service:
            do_mac_state_service( mac );
            break;
        case mac_state_online:
            do_mac_state_online( mac );
            break;
    }
}

extern void ezbus_mac_arbitration_set_state ( ezbus_mac_arbitration_t* mac_arbitration, ezbus_mac_state_t state )
{
    mac_arbitration->state = state;
}

extern ezbus_mac_state_t ezbus_mac_get_state ( ezbus_mac_arbitration_t* mac_arbitration )
{
    return mac_arbitration->state;
}




static void ezbus_arbitration_ack_tx_timer_callback( ezbus_timer_t* timer, void* arg )
{
    ezbus_log( EZBUS_LOG_ARBITRATION, "ezbus_arbitration_ack_tx_timer_callback\n" );
}

static void ezbus_arbitration_ack_rx_timer_callback( ezbus_timer_t* timer, void* arg )
{
    ezbus_log( EZBUS_LOG_ARBITRATION, "ezbus_arbitration_ack_rx_timer_callback\n" );
}




extern void ezbus_mac_transmitter_signal_empty( ezbus_mac_transmitter_t* transmitter, void* arg )
{
    ezbus_mac_arbitration_t* mac_arbitration = (ezbus_mac_arbitration_t*)arg;

    ezbus_log( EZBUS_LOG_ARBITRATION, "ezbus_mac_transmitter_signal_empty\n" );

    if ( ezbus_mac_get_token( mac ) )
    {
        // fprintf( stderr, "A\n");
        if ( mac->warmboot_pending )
        {   
            // fprintf( stderr, "B\n");
            if ( ezbus_peer_list_am_dominant( &mac->peer_list ) )
            {
                // fprintf( stderr, "C\n");
                mac->warmboot_pending = false;
                break;
            }
        }

        if ( !( rc = mac->layer1_tx_callback( mac ) ) )
        {
            ezbus_mac_transmitter_set_state( mac_transmitter, transmitter_state_give_token );
        }
        else
        {
            ezbus_mac_set_ack_tx_retry( mac, EZBUS_RETRANSMIT_TRIES );
        }
    }

}

extern void ezbus_mac_transmitter_signal_full( ezbus_mac_transmitter_t* transmitter, void* arg  )
{
    ezbus_mac_arbitration_t* mac_arbitration = (ezbus_mac_arbitration_t*)arg;
    
    ezbus_log( EZBUS_LOG_ARBITRATION, "ezbus_mac_transmitter_signal_full\n" );
}

extern void ezbus_mac_transmitter_signal_sent( ezbus_mac_transmitter_t* transmitter, void* arg  )
{
    ezbus_mac_arbitration_t* mac_arbitration = (ezbus_mac_arbitration_t*)arg;
    
    ezbus_log( EZBUS_LOG_ARBITRATION, "ezbus_mac_transmitter_signal_sent\n" );
}

extern void ezbus_mac_transmitter_signal_wait( ezbus_mac_transmitter_t* transmitter, void* arg  )
{
    ezbus_mac_arbitration_t* mac_arbitration = (ezbus_mac_arbitration_t*)arg;
    
    ezbus_log( EZBUS_LOG_ARBITRATION, "ezbus_mac_transmitter_signal_wait\n" );

    ezbus_mac_arbitration_set_ack_tx_begin( mac_arbitration, ezbus_platform_get_ms_ticks() );
}

extern void ezbus_mac_transmitter_signal_fault( ezbus_mac_transmitter_t* transmitter, void* arg  )
{
    ezbus_mac_arbitration_t* mac_arbitration = (ezbus_mac_arbitration_t*)arg;
    
    ezbus_log( EZBUS_LOG_ARBITRATION, "ezbus_mac_transmitter_signal_fault\n" );
}



extern void ezbus_mac_receiver_signal_empty ( ezbus_mac_receiver_t* receiver, void* arg )
{
    ezbus_mac_arbitration_t* mac_arbitration = (ezbus_mac_arbitration_t*)arg;
    
    ezbus_log( EZBUS_LOG_ARBITRATION, "ezbus_mac_receiver_signal_empty\n" );

    if ( ezbus_mac_recv_packet( mac ) )
}

extern void ezbus_mac_receiver_signal_full  ( ezbus_mac_receiver_t* receiver, void* arg )
{
    ezbus_mac_arbitration_t* mac_arbitration = (ezbus_mac_arbitration_t*)arg;
    ezbus_packet_t* rx_packet  = ezbus_mac_receiver_get_packet( ezbus_mac_get_receiver( mac ) );
    ezbus_packet_t* tx_packet  = ezbus_mac_transmitter_get_packet( ezbus_mac_get_transmitter( mac ) );
    
    ezbus_log( EZBUS_LOG_ARBITRATION, "ezbus_mac_receiver_signal_full\n" );

    switch( ezbus_packet_type( rx_packet ) )
    {
        case packet_type_reset:
            break;
        case packet_type_take_token:
            ezbus_mac_set_token( mac, false );
            rc = true;
            break;
        case packet_type_give_token:
            ezbus_mac_accept_token( mac, rx_packet );
            rc = true;
            break;
        case packet_type_parcel:
            rc = mac->layer1_rx_callback( mac );
            break;
        case packet_type_speed:
            rc = true;
            break;
        case packet_type_ack:
            if ( ezbus_mac_transmitter_get_state( ezbus_mac_get_transmitter( mac ) ) == transmitter_state_wait_ack )
            {
                if ( ezbus_address_compare( ezbus_packet_src(rx_packet), ezbus_packet_dst(tx_packet) ) == 0 )
                {
                    if ( ezbus_packet_seq(rx_packet) == ezbus_packet_seq(tx_packet) )
                    {
                        ezbus_mac_transmitter_set_state( ezbus_mac_get_transmitter( mac ), transmitter_state_empty );
                        rc = true;
                    }
                    else
                    {
                        /* FIXME - throw a fault here?? */
                        ezbus_log( EZBUS_LOG_RECEIVER, "recv: ack packet sequence mismatch\n");
                    }
                }
                else
                {
                    /* FIXME - throw a fault here? */
                    ezbus_log( EZBUS_LOG_RECEIVER, "recv: ack src and dst mismatch\n");
                }
            }
            else
            {
                /* FIXME - throw a fauld herre? */
                ezbus_log( EZBUS_LOG_RECEIVER, "recv: received unexpected ack\n" );
            }
            break;
        case packet_type_nack:
            rc = true;
            break;

        case packet_type_coldboot:
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
            rc = true;
            break;

        case packet_type_warmboot:
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
            rc = true;
            break;
    }

    return rc;
}

extern void ezbus_mac_receiver_signal_wait  ( ezbus_mac_receiver_t* receiver, void* arg )
{
    ezbus_mac_arbitration_t* mac_arbitration = (ezbus_mac_arbitration_t*)arg;
    
    ezbus_log( EZBUS_LOG_ARBITRATION, "ezbus_mac_receiver_signal_wait\n" );
}

extern void ezbus_mac_receiver_signal_fault ( ezbus_mac_receiver_t* receiver, void* arg )
{
    ezbus_mac_arbitration_t* mac_arbitration = (ezbus_mac_arbitration_t*)arg;
    
    ezbus_log( EZBUS_LOG_ARBITRATION, "ezbus_mac_receiver_signal_fault\n" );
}


extern void  ezbus_mac_bootstrap_signal_silent_start( ezbus_mac_bootstrap_t* bootstrap )
{
    ezbus_log( EZBUS_LOG_BOOTSTRAP, "ezbus_mac_bootstrap_signal_silent_start\n" );
}

extern void  ezbus_mac_bootstrap_signal_silent_continue( ezbus_mac_bootstrap_t* bootstrap )
{
    ezbus_log( EZBUS_LOG_BOOTSTRAP, "ezbus_mac_bootstrap_signal_silent_continue\n" );
}

extern void  ezbus_mac_bootstrap_signal_silent_stop( ezbus_mac_bootstrap_t* bootstrap )
{
    ezbus_log( EZBUS_LOG_BOOTSTRAP, "ezbus_mac_bootstrap_signal_silent_stop\n" );
}


extern void  ezbus_mac_bootstrap_signal_coldboot_start( ezbus_mac_bootstrap_t* bootstrap )
{
    ezbus_log( EZBUS_LOG_BOOTSTRAP, "ezbus_mac_bootstrap_signal_coldboot_start\n" );
}

extern void  ezbus_mac_bootstrap_signal_coldboot_continue( ezbus_mac_bootstrap_t* bootstrap )
{
    ezbus_log( EZBUS_LOG_BOOTSTRAP, "ezbus_mac_bootstrap_signal_coldboot_continue\n" );
}

extern void  ezbus_mac_bootstrap_signal_coldboot_stop( ezbus_mac_bootstrap_t* bootstrap )
{
    ezbus_log( EZBUS_LOG_BOOTSTRAP, "ezbus_mac_bootstrap_signal_coldboot_stop\n" );
}


extern void  ezbus_mac_bootstrap_signal_warmboot_tx_first( ezbus_mac_bootstrap_t* bootstrap )
{
    ezbus_log( EZBUS_LOG_BOOTSTRAP, "ezbus_mac_bootstrap_signal_warmboot_tx_first\n" );
}

extern void  ezbus_mac_bootstrap_signal_warmboot_tx_start( ezbus_mac_bootstrap_t* bootstrap )
{
    ezbus_log( EZBUS_LOG_BOOTSTRAP, "ezbus_mac_bootstrap_signal_warmboot_tx_start\n" );
}

extern void  ezbus_mac_bootstrap_signal_warmboot_tx_restart( ezbus_mac_bootstrap_t* bootstrap )
{
    ezbus_log( EZBUS_LOG_BOOTSTRAP, "ezbus_mac_bootstrap_signal_warmboot_tx_restart\n" );
}

extern void  ezbus_mac_bootstrap_signal_warmboot_tx_continue( ezbus_mac_bootstrap_t* bootstrap )
{
    ezbus_log( EZBUS_LOG_BOOTSTRAP, "ezbus_mac_bootstrap_signal_warmboot_tx_continue\n" );
}

extern void  ezbus_mac_bootstrap_signal_warmboot_tx_stop( ezbus_mac_bootstrap_t* bootstrap )
{
    ezbus_log( EZBUS_LOG_BOOTSTRAP, "ezbus_mac_bootstrap_signal_warmboot_tx_stop\n" );
}


extern void  ezbus_mac_bootstrap_signal_warmboot_rx_start( ezbus_mac_bootstrap_t* bootstrap )
{
    ezbus_log( EZBUS_LOG_BOOTSTRAP, "ezbus_mac_bootstrap_signal_warmboot_rx_start\n" );
}

extern void  ezbus_mac_bootstrap_signal_warmboot_rx_continue( ezbus_mac_bootstrap_t* bootstrap )
{
    ezbus_log( EZBUS_LOG_BOOTSTRAP, "ezbus_mac_bootstrap_signal_warmboot_rx_continue\n" );
}

extern void  ezbus_mac_bootstrap_signal_warmboot_rx_stop( ezbus_mac_bootstrap_t* bootstrap )
{
    ezbus_log( EZBUS_LOG_BOOTSTRAP, "ezbus_mac_bootstrap_signal_warmboot_rx_stop\n" );
}























#define EZBUS_HELLO_TIMER_MIN   1
#define EZBUS_HELLO_TIMER_MAX   50

static bool ezbus_mac_arbitration_give_token              ( ezbus_mac_arbitration_t* mac_arbitration );
static bool ezbus_mac_arbitration_accept_token            ( ezbus_mac_arbitration_t* mac_arbitration, ezbus_packet_t* rx_packet );
static bool ezbus_mac_arbitration_acknowledge_token       ( ezbus_mac_arbitration_t* mac_arbitration, ezbus_packet_t* packet );
static bool ezbus_mac_arbitration_prepare_ack             ( ezbus_mac_arbitration_t* mac_arbitration );
static bool ezbus_mac_arbitration_send_ack                ( ezbus_mac_arbitration_t* mac_arbitration );
static bool ezbus_mac_arbitration_recv_packet             ( ezbus_mac_arbitration_t* mac_arbitration );
static bool ezbus_mac_arbitration_coldboot_emit           ( ezbus_mac_arbitration_t* mac_arbitration, ezbus_address_t* src_address, ezbus_address_t* dst_address, uint8_t boot_seq );
static bool ezbus_mac_arbitration_warmboot_emit           ( ezbus_mac_arbitration_t* mac_arbitration, ezbus_address_t* src_address, ezbus_address_t* dst_address, uint8_t boot_seq );
static bool ezbus_mac_arbitration_arbitration_tx_callback ( ezbus_mac_transmitter_t* mac_transmitter, void* arg );
static bool ezbus_mac_arbitration_rx_callback             ( ezbus_mac_receiver_t*    mac_receiver,    void* arg );

static void ezbus_mac_arbitration_set_state               ( ezbus_mac_arbitration_t* mac_arbitration, ezbus_mac_state_t state );
static ezbus_mac_state_t ezbus_mac_get_state  ( ezbus_mac_arbitration_t* mac_arbitration );

static void ezbus_mac_arbitration_run_state_machine       ( ezbus_mac_arbitration_t* mac_arbitration );
static void ezbuz_mac_arbitration_run_timers              ( ezbus_mac_arbitration_t* mac_arbitration );
static void ezbuz_mac_run_boot                ( ezbus_mac_arbitration_t* mac_arbitration );
static void do_mac_state_offline              ( ezbus_mac_arbitration_t* mac_arbitration );
static void do_mac_state_service              ( ezbus_mac_arbitration_t* mac_arbitration );
static void do_mac_state_online               ( ezbus_mac_arbitration_t* mac_arbitration );


void ezbus_mac_arbitration_init ( ezbus_mac_arbitration_t* mac_arbitration )
{
    memset( mac, 0 , sizeof( ezbus_mac_arbitration_t) );


    ezbus_mac_set_state( mac, mac_state_offline );

    ezbus_timer_init( &mac->ack_tx_timer );
    ezbus_timer_init( &mac->ack_rx_timer );
}



void ezbus_mac_arbitration_run( ezbus_mac_arbitration_t* mac_arbitration )
{
    ezbus_mac_run_state_machine( mac );
}










static void ezbus_mac_arbitration_warmboot_timer_callback( ezbus_timer_t* timer, void* arg )
{
    ezbus_mac_arbitration_t* mac_arbitration = (ezbus_mac_arbitration_t*)arg;
    mac->warmboot_pending = true;
    ezbus_timer_stop( &mac->warmboot_timer );
}


static bool ezbus_mac_arbitration_arbitration_tx_callback( ezbus_mac_transmitter_t* mac_transmitter, void* arg )
{
    bool rc = false;
    ezbus_mac_arbitration_t* mac_arbitration = (ezbus_mac_arbitration_t*)arg;

    switch( ezbus_mac_transmitter_get_state( mac_transmitter ) )
    {

        case transmitter_state_empty:

            if ( ezbus_mac_get_token( mac ) )
            {
                // fprintf( stderr, "A\n");
                if ( mac->warmboot_pending )
                {   
                    // fprintf( stderr, "B\n");
                    if ( ezbus_peer_list_am_dominant( &mac->peer_list ) )
                    {
                        // fprintf( stderr, "C\n");
                        mac->warmboot_pending = false;
                        break;
                    }
                }

                if ( !( rc = mac->layer1_tx_callback( mac ) ) )
                {
                    ezbus_mac_transmitter_set_state( mac_transmitter, transmitter_state_give_token );
                }
                else
                {
                    ezbus_mac_set_ack_tx_retry( mac, EZBUS_RETRANSMIT_TRIES );
                }
            }
            break;

        case transmitter_state_transit_full:
            rc = ezbus_mac_get_token( mac );
            break;

        case transmitter_state_full:
            rc = ezbus_mac_get_token( mac );
            break;

        case transmitter_state_send:
            rc = ezbus_mac_get_token( mac );
            break;

        case transmitter_state_give_token:
            ezbus_mac_give_token( mac );
            rc = true;
            break;

        case transmitter_state_transit_wait_ack:
            ezbus_mac_set_ack_tx_begin( mac, ezbus_platform_get_ms_ticks() );
            rc = true;
            break;

        case transmitter_state_wait_ack:
            // if ( ezbus_platform_get_ms_ticks() - ezbus_mac_get_ack_tx_begin( mac ) > ezbus_mac_arbitration_tx_ack_timeout( mac ) )
            // {
            //     if ( ezbus_mac_get_ack_tx_retry( mac ) > 0 )
            //     {
            //         ezbus_mac_set_ack_tx_retry( mac, ezbus_mac_get_ack_tx_retry( mac )-1 );

            //     }
            //     else
            //     {
            //         // throw a fault?
            //         rc = true;  // terrminate & empty the transmitter
            //     }
            // }
            rc = true;

            break;
    }
    return rc;
}

static bool ezbus_mac_arbitration_rx_callback( ezbus_mac_receiver_t* mac_receiver, void* arg )
{
    bool rc=false;
    ezbus_mac_arbitration_t* mac_arbitration = (ezbus_mac_arbitration_t*)arg;
    ezbus_mac_transmitter_t* mac_transmitter = ezbus_mac_get_transmitter( mac );

    switch ( ezbus_mac_receiver_get_state( mac_receiver ) )
    {

        case receiver_state_empty:

            /* 
             * callback should examine fault, return true to reset fault. 
             */
            rc = true;
            break;

        case receiver_state_full:
            
            rc = ezbus_mac_recv_packet( mac );
            break;

        case receiver_state_receive_fault:

            /* 
             * callback should acknowledge the fault to return receiver back to receiver_empty state 
             */
            switch( ezbus_mac_receiver_get_err( mac_receiver ) )
            {
                case EZBUS_ERR_NOTREADY:
                    break;
                default:
                    ezbus_log( EZBUS_LOG_RECEIVER, "RX_FAULT: %d\n", (int)ezbus_mac_receiver_get_err( mac_receiver ) );
                    break;
            }
            rc = true;
            break;

        case receiver_state_transit_to_ack:
            
            ezbus_mac_prepare_ack( mac );
            ezbus_mac_set_ack_rx_begin( mac, ezbus_platform_get_ms_ticks() );
            ezbus_mac_set_ack_rx_pending( mac, true );
            rc = true;
            break;

        case receiver_state_wait_ack_sent:

            if ( ( ezbus_mac_transmitter_get_state( mac_transmitter ) == transmitter_state_empty ) && ezbus_mac_get_token( mac ) )
            {
                ezbus_mac_send_ack( mac );
                ezbus_mac_set_ack_rx_begin( mac, 0 );
                ezbus_mac_set_ack_rx_pending( mac, false );
                rc = true;
            }
            break;
    }
    return rc;
}

static bool ezbus_mac_arbitration_give_token( ezbus_mac_arbitration_t* mac_arbitration )
{
    bool rc=true;
    ezbus_packet_t  tx_packet;
    ezbus_address_t peer_address;

    ezbus_address_copy( &peer_address, ezbus_peer_list_next( &mac->peer_list, &ezbus_self_address ) );

    ezbus_log( EZBUS_LOG_TOKEN, "%ctok> %s ", ezbus_mac_get_token(mac)?'*':' ', ezbus_address_string(&ezbus_self_address) );
    ezbus_log( EZBUS_LOG_TOKEN, "> %s\n",   ezbus_address_string(&peer_address) );

    ezbus_packet_init( &tx_packet );
    ezbus_packet_set_type( &tx_packet, packet_type_give_token );
    ezbus_packet_set_seq( &tx_packet, mac->token_packet_seq++ );

    ezbus_address_copy( ezbus_packet_src( &tx_packet ), &ezbus_self_address );
    ezbus_address_copy( ezbus_packet_dst( &tx_packet ), &peer_address );
    
    tx_packet.data.attachment.token.token_counter = mac->token_counter + 1;
    ezbus_peer_list_crc( &mac->peer_list, &tx_packet.data.attachment.token.peer_list_crc );

    ezbus_port_send( ezbus_mac_transmitter_get_port( ezbus_mac_get_transmitter( mac ) ), &tx_packet );
    
    if ( ezbus_address_compare( &ezbus_self_address, &peer_address ) == 0 )
    {
        ezbus_mac_acknowledge_token( mac, &tx_packet );
    }
    else
    {
        ezbus_mac_set_token( mac, false );
    }

    return rc;
}


static bool ezbus_mac_arbitration_accept_token( ezbus_mac_arbitration_t* mac_arbitration, ezbus_packet_t* rx_packet )
{
    ezbus_log( EZBUS_LOG_TOKEN, "%ctok <%s ", ezbus_mac_get_token(mac)?'*':' ', ezbus_address_string(ezbus_packet_dst( rx_packet )) );
    ezbus_log( EZBUS_LOG_TOKEN, "< %s\n",   ezbus_address_string(ezbus_packet_src( rx_packet )) );

    if ( ezbus_address_compare( &ezbus_self_address, ezbus_packet_dst( rx_packet ) ) == 0 )
    {
        ezbus_mac_acknowledge_token( mac, rx_packet );
    }
    else
    {
        ezbus_boot_signal_token_seen( ezbus_mac_get_boot( mac ), rx_packet );
    }
    return true;
}


static bool ezbus_mac_arbitration_acknowledge_token( ezbus_mac_arbitration_t* mac_arbitration, ezbus_packet_t* packet )
{
    ezbus_crc_t peer_list_crc;

    mac->token_counter = packet->data.attachment.token.token_counter;
    ezbus_peer_list_crc( &mac->peer_list, &peer_list_crc );
    ezbus_mac_set_token( mac, true );
    ezbus_boot_signal_token_seen( ezbus_mac_get_boot( mac ), packet );
    ezbus_log( EZBUS_LOG_TOKEN, "%ctok <%s\n", ezbus_mac_get_token(mac)?'*':' ', ezbus_address_string( ezbus_packet_src( packet ) ) );
    if ( !ezbus_crc_equal( &peer_list_crc, &packet->data.attachment.token.peer_list_crc ) )
    {
        ezbus_log( EZBUS_LOG_TOKEN, "tok crc bad< expect 0x%04X got 0x%04X ", peer_list_crc, packet->data.attachment.token.peer_list_crc );
        /* FIXME - jump to warm-boot state here ?? */
    }
    return true;
}

static bool ezbus_mac_arbitration_prepare_ack( ezbus_mac_arbitration_t* mac_arbitration )
{
    ezbus_packet_t* tx_packet  = ezbus_mac_transmitter_get_packet( ezbus_mac_get_transmitter( mac ) );
    ezbus_packet_t* ack_rx_packet = ezbus_mac_get_ack_rx_packet( mac );

    ezbus_packet_init( ack_rx_packet );
    ezbus_packet_set_type( ack_rx_packet, packet_type_ack );
    ezbus_address_copy( ezbus_packet_src( ack_rx_packet ), ezbus_packet_dst( tx_packet ) );
    ezbus_address_copy( ezbus_packet_dst( ack_rx_packet ), ezbus_packet_src( tx_packet ) );
    ezbus_packet_set_seq( ack_rx_packet, ezbus_packet_seq( tx_packet ) );

    return true; /* FIXME ?? */
}

static bool ezbus_mac_arbitration_send_ack( ezbus_mac_arbitration_t* mac_arbitration )
{
    ezbus_port_send( 
            ezbus_mac_transmitter_get_port( 
                ezbus_mac_get_transmitter( mac ) ), 
                    ezbus_mac_get_ack_rx_packet( mac ) );

    return true; /* FIXME ?? */
}

static bool ezbus_mac_arbitration_recv_packet( ezbus_mac_arbitration_t* mac_arbitration )
{
    bool rc=false;
    ezbus_packet_t* rx_packet  = ezbus_mac_receiver_get_packet( ezbus_mac_get_receiver( mac ) );
    ezbus_packet_t* tx_packet  = ezbus_mac_transmitter_get_packet( ezbus_mac_get_transmitter( mac ) );

    switch( ezbus_packet_type( rx_packet ) )
    {
        case packet_type_reset:
            break;
        case packet_type_take_token:
            ezbus_mac_set_token( mac, false );
            rc = true;
            break;
        case packet_type_give_token:
            ezbus_mac_accept_token( mac, rx_packet );
            rc = true;
            break;
        case packet_type_parcel:
            rc = mac->layer1_rx_callback( mac );
            break;
        case packet_type_speed:
            rc = true;
            break;
        case packet_type_ack:
            if ( ezbus_mac_transmitter_get_state( ezbus_mac_get_transmitter( mac ) ) == transmitter_state_wait_ack )
            {
                if ( ezbus_address_compare( ezbus_packet_src(rx_packet), ezbus_packet_dst(tx_packet) ) == 0 )
                {
                    if ( ezbus_packet_seq(rx_packet) == ezbus_packet_seq(tx_packet) )
                    {
                        ezbus_mac_transmitter_set_state( ezbus_mac_get_transmitter( mac ), transmitter_state_empty );
                        rc = true;
                    }
                    else
                    {
                        /* FIXME - throw a fault here?? */
                        ezbus_log( EZBUS_LOG_RECEIVER, "recv: ack packet sequence mismatch\n");
                    }
                }
                else
                {
                    /* FIXME - throw a fault here? */
                    ezbus_log( EZBUS_LOG_RECEIVER, "recv: ack src and dst mismatch\n");
                }
            }
            else
            {
                /* FIXME - throw a fauld herre? */
                ezbus_log( EZBUS_LOG_RECEIVER, "recv: received unexpected ack\n" );
            }
            break;
        case packet_type_nack:
            rc = true;
            break;

        case packet_type_coldboot:
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
            rc = true;
            break;

        case packet_type_warmboot:
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
            rc = true;
            break;
    }

    return rc;
}

static bool ezbus_mac_arbitration_coldboot_emit( ezbus_mac_arbitration_t* mac_arbitration, ezbus_address_t* src_address, ezbus_address_t* dst_address, uint8_t boot_seq )
{
    if ( ezbus_platform_get_ms_ticks() - mac->boot_time )
    {
        ezbus_packet_t* boot_packet = ezbus_mac_transmitter_get_packet( ezbus_mac_get_transmitter( mac ) );
        ezbus_packet_init( boot_packet );
        ezbus_packet_set_type( boot_packet, packet_type_coldboot );
        ezbus_packet_set_seq( boot_packet, boot_seq );
        ezbus_address_copy( ezbus_packet_src( boot_packet ), src_address );
        ezbus_address_copy( ezbus_packet_dst( boot_packet ), dst_address );
        ezbus_port_send( ezbus_mac_transmitter_get_port( ezbus_mac_get_transmitter( mac ) ), boot_packet );
    }
    return true;
}

static bool ezbus_mac_arbitration_warmboot_emit( ezbus_mac_arbitration_t* mac_arbitration, ezbus_address_t* src_address, ezbus_address_t* dst_address, uint8_t boot_seq )
{
    if ( ezbus_platform_get_ms_ticks() - mac->boot_time )
    {
        ezbus_packet_t* boot_packet = ezbus_mac_transmitter_get_packet( ezbus_mac_get_transmitter( mac ) );
        ezbus_packet_init( boot_packet );
        ezbus_packet_set_type( boot_packet, packet_type_warmboot );
        ezbus_packet_set_seq( boot_packet, boot_seq );
        ezbus_address_copy( ezbus_packet_src( boot_packet ), src_address );
        ezbus_address_copy( ezbus_packet_dst( boot_packet ), dst_address );
        ezbus_port_send( ezbus_mac_transmitter_get_port( ezbus_mac_get_transmitter( mac ) ), boot_packet );
    }
    return true;
}

static void ezbus_mac_arbitration_boot_callback( ezbus_boot_t* boot, void* arg )
{
    ezbus_mac_arbitration_t* mac_arbitration = (ezbus_mac_arbitration_t*)arg;

    switch ( ezbus_boot_get_state( boot ) )
    {
        case boot_state_silent_start:
            //fprintf( stderr, "%cboot_state_silent_start>\n", ezbus_mac_get_token(mac)?'*':' ' );
            ezbus_mac_receiver_run    ( ezbus_mac_get_receiver   ( mac ) );
            ezbus_mac_transmitter_run ( ezbus_mac_get_transmitter( mac ) );
            break;

        case boot_state_silent_continue:
            //fprintf( stderr, "%cboot_state_silent_continue>\n", ezbus_mac_get_token(mac)?'*':' ' );
            ezbus_mac_receiver_run    ( ezbus_mac_get_receiver   ( mac ) );
            ezbus_mac_transmitter_run ( ezbus_mac_get_transmitter( mac ) );
            break;

        case boot_state_silent_stop:
            mac->coldboot_seq=0;
            break;

        case boot_state_coldboot_start:
            ezbus_mac_set_token( mac, false );
            break;

        case boot_state_coldboot_continue:
            ezbus_log( EZBUS_LOG_COLDBOOT, "%ccoldboot> %s %3d | ", ezbus_mac_get_token(mac)?'*':' ', ezbus_address_string( &ezbus_self_address ), mac->coldboot_seq );
            ezbus_mac_coldboot_emit( mac, &ezbus_self_address, (ezbus_address_t*)&ezbus_broadcast_address, mac->coldboot_seq++ );
            #if EZBUS_LOG_COLDBOOT
                ezbus_peer_list_log( &mac->peer_list );
            #endif
            break;

        case boot_state_coldboot_stop:
            mac->coldboot_seq = 0;
            break;
        
        case boot_state_warmboot_tx_first:
            ezbus_mac_set_token( mac, true );
            break;

        case boot_state_warmboot_tx_start:
            if ( ++mac->warmboot_seq == 0 )
                mac->warmboot_seq = 1;
            ezbus_timer_stop( &mac->warmboot_timer );
            mac->warmboot_pending = false;
            break;

        case boot_state_warmboot_tx_restart:
            ezbus_log( EZBUS_LOG_WARMBOOT, "%cwarmboot} %s %3d | ", ezbus_mac_get_token(mac)?'*':' ', ezbus_address_string( (ezbus_address_t*)&ezbus_warmboot_address ), mac->warmboot_seq );
            ezbus_mac_warmboot_emit( mac, (ezbus_address_t*)&ezbus_warmboot_address, (ezbus_address_t*)&ezbus_broadcast_address, mac->warmboot_seq );
            ezbus_boot_signal_peer_seen( ezbus_mac_get_boot( mac ), ezbus_mac_transmitter_get_packet( ezbus_mac_get_transmitter( mac ) ) );
            #if EZBUS_LOG_WARMBOOT
                ezbus_peer_list_log( &mac->peer_list );
            #endif
            break;

        case boot_state_warmboot_tx_continue:
            ezbus_log( EZBUS_LOG_WARMBOOT, "%cwarmboot> %s %3d | ", ezbus_mac_get_token(mac)?'*':' ', ezbus_address_string( &ezbus_self_address ), mac->warmboot_seq );
            ezbus_mac_warmboot_emit( mac, &ezbus_self_address, (ezbus_address_t*)&ezbus_warmboot_address, mac->warmboot_seq );
            ezbus_boot_signal_peer_seen( ezbus_mac_get_boot( mac ), ezbus_mac_transmitter_get_packet( ezbus_mac_get_transmitter( mac ) ) );
            #if EZBUS_LOG_WARMBOOT
                ezbus_peer_list_log( &mac->peer_list );
            #endif
            break;

        case boot_state_warmboot_tx_stop:
            ezbus_timer_start( &mac->warmboot_timer );
            break;

        case boot_state_warmboot_rx_start:
            break;

        case boot_state_warmboot_rx_continue:
            ezbus_mac_warmboot_emit( mac, &ezbus_self_address, (ezbus_address_t*)&ezbus_warmboot_address, ezbus_packet_seq( ezbus_mac_receiver_get_packet( ezbus_mac_get_receiver( mac ) ) ) );
            break;

        case boot_state_warmboot_rx_stop:
            break;

    }
}

