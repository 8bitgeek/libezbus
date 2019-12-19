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
#include <ezbus_layer0_transceiver.h>
#include <ezbus_token.h>
#include <ezbus_hex.h>
#include <ezbus_log.h>

#define EZBUS_HELLO_TIMER_MIN   1
#define EZBUS_HELLO_TIMER_MAX   50

static bool ezbus_layer0_transceiver_give_token         ( ezbus_layer0_transceiver_t* layer0_transceiver );
static bool ezbus_layer0_transceiver_accept_token       ( ezbus_layer0_transceiver_t* layer0_transceiver, ezbus_packet_t* rx_packet );
static bool ezbus_layer0_transceiver_acknowledge_token  ( ezbus_layer0_transceiver_t* layer0_transceiver, ezbus_packet_t* packet );
static bool ezbus_layer0_transceiver_prepare_ack        ( ezbus_layer0_transceiver_t* layer0_transceiver );
static bool ezbus_layer0_transceiver_send_ack           ( ezbus_layer0_transceiver_t* layer0_transceiver );
static bool ezbus_layer0_transceiver_recv_packet        ( ezbus_layer0_transceiver_t* layer0_transceiver );
static bool ezbus_layer0_transceiver_boot_emit          ( ezbus_layer0_transceiver_t* layer0_transceiver, ezbus_address_t* src_address, uint8_t boot_seq );
static bool ezbus_layer0_transceiver_tx_callback        ( ezbus_layer0_transmitter_t* layer0_transmitter, void* arg );
static bool ezbus_layer0_transceiver_rx_callback        ( ezbus_layer0_receiver_t*    layer0_receiver,    void* arg );
static void ezbus_layer0_transceiver_boot_callback      ( ezbus_boot_t* boot, void* arg );

void ezbus_layer0_transceiver_init (    
                                        ezbus_layer0_transceiver_t*             layer0_transceiver, 
                                        ezbus_port_t*                           port,
                                        ezbus_layer1_callback_t                 layer1_tx_callback,
                                        ezbus_layer1_callback_t                 layer1_rx_callback
                                    )
{
    ezbus_address_init();

    layer0_transceiver->port = port;

    layer0_transceiver->layer1_tx_callback = layer1_tx_callback;
    layer0_transceiver->layer1_rx_callback = layer1_rx_callback;

    ezbus_peer_list_init( &layer0_transceiver->peer_list );

    ezbus_layer0_receiver_init    ( ezbus_layer0_transceiver_get_receiver    ( layer0_transceiver ), port, ezbus_layer0_transceiver_rx_callback, layer0_transceiver );
    ezbus_layer0_transmitter_init ( ezbus_layer0_transceiver_get_transmitter ( layer0_transceiver ), port, ezbus_layer0_transceiver_tx_callback, layer0_transceiver );

    ezbus_timer_init( &layer0_transceiver->ack_tx_timer );
    ezbus_timer_init( &layer0_transceiver->ack_rx_timer );

    ezbus_boot_init(   
                        ezbus_layer0_transceiver_get_boot( layer0_transceiver ), 
                        ezbus_port_get_speed(port), 
                        &layer0_transceiver->peer_list,
                        ezbus_layer0_transceiver_boot_callback,
                        layer0_transceiver
                    );
}

void ezbus_layer0_transceiver_run  ( ezbus_layer0_transceiver_t* layer0_transceiver )
{
    ezbus_timer_run( &layer0_transceiver->ack_tx_timer );
    ezbus_timer_run( &layer0_transceiver->ack_rx_timer );

    ezbus_layer0_receiver_run    ( ezbus_layer0_transceiver_get_receiver   ( layer0_transceiver ) );
    ezbus_layer0_transmitter_run ( ezbus_layer0_transceiver_get_transmitter( layer0_transceiver ) );

    ezbus_boot_run( ezbus_layer0_transceiver_get_boot( layer0_transceiver ) );
}


static bool ezbus_layer0_transceiver_tx_callback( ezbus_layer0_transmitter_t* layer0_transmitter, void* arg )
{
    bool rc = false;
    ezbus_layer0_transceiver_t* layer0_transceiver = (ezbus_layer0_transceiver_t*)arg;

    switch( ezbus_layer0_transmitter_get_state( layer0_transmitter ) )
    {
        case transmitter_state_empty:
            if ( ezbus_layer0_transceiver_get_token( layer0_transceiver ) )   // ??
            {
                if ( !( rc = layer0_transceiver->layer1_tx_callback( layer0_transceiver ) ) )
                {
                    ezbus_layer0_transmitter_set_state( layer0_transmitter, transmitter_state_give_token );
                }
            }
            ezbus_layer0_transceiver_set_ack_tx_retry( layer0_transceiver, EZBUS_RETRANSMIT_TRIES );
            break;
        case transmitter_state_transit_full:
            rc = ezbus_layer0_transceiver_get_token( layer0_transceiver );
            break;
        case transmitter_state_full:
            rc = ezbus_layer0_transceiver_get_token( layer0_transceiver );
            break;
        case transmitter_state_send:
            rc = ezbus_layer0_transceiver_get_token( layer0_transceiver );
            break;
        case transmitter_state_give_token:
            rc = ezbus_layer0_transceiver_give_token( layer0_transceiver );
            ezbus_layer0_transceiver_set_token( layer0_transceiver, !rc );
            break;
        case transmitter_state_transit_wait_ack:

            ezbus_layer0_transceiver_set_ack_tx_begin( layer0_transceiver, ezbus_platform_get_ms_ticks() );

            rc = true;
            break;
        case transmitter_state_wait_ack:

            // if ( ezbus_platform_get_ms_ticks() - ezbus_layer0_transceiver_get_ack_tx_begin( layer0_transceiver ) > ezbus_layer0_transceiver_tx_ack_timeout( layer0_transceiver ) )
            // {
            //     if ( ezbus_layer0_transceiver_get_ack_tx_retry( layer0_transceiver ) > 0 )
            //     {
            //         ezbus_layer0_transceiver_set_ack_tx_retry( layer0_transceiver, ezbus_layer0_transceiver_get_ack_tx_retry( layer0_transceiver )-1 );

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

static bool ezbus_layer0_transceiver_rx_callback( ezbus_layer0_receiver_t* layer0_receiver, void* arg )
{
    bool rc=false;
    ezbus_layer0_transceiver_t* layer0_transceiver = (ezbus_layer0_transceiver_t*)arg;
    ezbus_layer0_transmitter_t* layer0_transmitter = ezbus_layer0_transceiver_get_transmitter( layer0_transceiver );

    switch ( ezbus_layer0_receiver_get_state( layer0_receiver ) )
    {

        case receiver_state_empty:

            /* 
             * callback should examine fault, return true to reset fault. 
             */
            rc = true;
            break;

        case receiver_state_full:
            
            rc = ezbus_layer0_transceiver_recv_packet( layer0_transceiver );
            break;

        case receiver_state_receive_fault:

            /* 
             * callback should acknowledge the fault to return receiver back to receiver_empty state 
             */
            switch( ezbus_layer0_receiver_get_err( layer0_receiver ) )
            {
                case EZBUS_ERR_NOTREADY:
                    break;
                default:
                    ezbus_log( EZBUS_LOG_RECEIVER, "RX_FAULT: %d\n", (int)ezbus_layer0_receiver_get_err( layer0_receiver ) );
                    break;
            }
            rc = true;
            break;

        case receiver_state_transit_to_ack:
            
            ezbus_layer0_transceiver_prepare_ack( layer0_transceiver );
            ezbus_layer0_transceiver_set_ack_rx_begin( layer0_transceiver, ezbus_platform_get_ms_ticks() );
            ezbus_layer0_transceiver_set_ack_rx_pending( layer0_transceiver, true );
            rc = true;
            break;

        case receiver_state_wait_ack_sent:

            if ( ( ezbus_layer0_transmitter_get_state( layer0_transmitter ) == transmitter_state_empty ) && ezbus_layer0_transceiver_get_token( layer0_transceiver ) )
            {
                ezbus_layer0_transceiver_send_ack( layer0_transceiver );
                ezbus_layer0_transceiver_set_ack_rx_begin( layer0_transceiver, 0 );
                ezbus_layer0_transceiver_set_ack_rx_pending( layer0_transceiver, false );
                rc = true;
            }
            break;
    }
    return rc;
}

static bool ezbus_layer0_transceiver_give_token( ezbus_layer0_transceiver_t* layer0_transceiver )
{
    bool rc=true;
    ezbus_packet_t  tx_packet;
    ezbus_address_t peer_address;

    ezbus_address_copy( &peer_address, ezbus_peer_list_next( &layer0_transceiver->peer_list, &ezbus_self_address ) );

    ezbus_log( EZBUS_LOG_TOKEN, "tok> %s ", ezbus_address_string(&ezbus_self_address) );
    ezbus_log( EZBUS_LOG_TOKEN, "> %s\n",   ezbus_address_string(&peer_address) );

    ezbus_packet_init( &tx_packet );
    ezbus_packet_set_type( &tx_packet, packet_type_give_token );
    ezbus_packet_set_seq( &tx_packet, layer0_transceiver->token_seq++ );

    ezbus_address_copy( ezbus_packet_src( &tx_packet ), &ezbus_self_address );
    ezbus_address_copy( ezbus_packet_dst( &tx_packet ), &peer_address );
    
    ezbus_port_send( ezbus_layer0_transmitter_get_port( ezbus_layer0_transceiver_get_transmitter( layer0_transceiver ) ), &tx_packet );
    
    if ( ezbus_address_compare( &ezbus_self_address, &peer_address ) == 0 )
    {
        ezbus_layer0_transceiver_acknowledge_token( layer0_transceiver, &tx_packet );
        rc=false;
    }

    return rc;
}


static bool ezbus_layer0_transceiver_accept_token( ezbus_layer0_transceiver_t* layer0_transceiver, ezbus_packet_t* rx_packet )
{
    ezbus_log( EZBUS_LOG_TOKEN, "tok< %s ", ezbus_address_string(ezbus_packet_dst( rx_packet )) );
    ezbus_log( EZBUS_LOG_TOKEN, "< %s\n",   ezbus_address_string(ezbus_packet_src( rx_packet )) );

    if ( ezbus_address_compare( &ezbus_self_address, ezbus_packet_dst( rx_packet ) ) == 0 )
    {
        ezbus_layer0_transceiver_acknowledge_token( layer0_transceiver, rx_packet );
    }
    else
    {
        ezbus_boot_signal_token_seen( ezbus_layer0_transceiver_get_boot( layer0_transceiver ), rx_packet );
    }
    return true;
}


static bool ezbus_layer0_transceiver_acknowledge_token( ezbus_layer0_transceiver_t* layer0_transceiver, ezbus_packet_t* packet )
{
    ezbus_layer0_transceiver_set_token( layer0_transceiver, true );
    ezbus_boot_signal_token_seen( ezbus_layer0_transceiver_get_boot( layer0_transceiver ), packet );
    return true;
}

static bool ezbus_layer0_transceiver_prepare_ack( ezbus_layer0_transceiver_t* layer0_transceiver )
{
    ezbus_packet_t* tx_packet  = ezbus_layer0_transmitter_get_packet( ezbus_layer0_transceiver_get_transmitter( layer0_transceiver ) );
    ezbus_packet_t* ack_rx_packet = ezbus_layer0_transceiver_get_ack_rx_packet( layer0_transceiver );

    ezbus_packet_init( ack_rx_packet );
    ezbus_packet_set_type( ack_rx_packet, packet_type_ack );
    ezbus_address_copy( ezbus_packet_src( ack_rx_packet ), ezbus_packet_dst( tx_packet ) );
    ezbus_address_copy( ezbus_packet_dst( ack_rx_packet ), ezbus_packet_src( tx_packet ) );
    ezbus_packet_set_seq( ack_rx_packet, ezbus_packet_seq( tx_packet ) );

    return true; /* FIXME ?? */
}

static bool ezbus_layer0_transceiver_send_ack( ezbus_layer0_transceiver_t* layer0_transceiver )
{
    ezbus_port_send( 
            ezbus_layer0_transmitter_get_port( 
                ezbus_layer0_transceiver_get_transmitter( layer0_transceiver ) ), 
                    ezbus_layer0_transceiver_get_ack_rx_packet( layer0_transceiver ) );

    return true; /* FIXME ?? */
}

static bool ezbus_layer0_transceiver_recv_packet( ezbus_layer0_transceiver_t* layer0_transceiver )
{
    bool rc=false;
    ezbus_packet_t* rx_packet  = ezbus_layer0_receiver_get_packet( ezbus_layer0_transceiver_get_receiver( layer0_transceiver ) );
    ezbus_packet_t* tx_packet  = ezbus_layer0_transmitter_get_packet( ezbus_layer0_transceiver_get_transmitter( layer0_transceiver ) );

    switch( ezbus_packet_type( rx_packet ) )
    {
        case packet_type_reset:
            break;
        case packet_type_take_token:
            ezbus_layer0_transceiver_set_token( layer0_transceiver, false );
            rc = true;
            break;
        case packet_type_give_token:
            ezbus_layer0_transceiver_accept_token( layer0_transceiver, rx_packet );
            rc = true;
            break;
        case packet_type_parcel:
            rc = layer0_transceiver->layer1_rx_callback( layer0_transceiver );
            break;
        case packet_type_speed:
            rc = true;
            break;
        case packet_type_ack:
            if ( ezbus_layer0_transmitter_get_state( ezbus_layer0_transceiver_get_transmitter( layer0_transceiver ) ) == transmitter_state_wait_ack )
            {
                if ( ezbus_address_compare( ezbus_packet_src(rx_packet), ezbus_packet_dst(tx_packet) ) == 0 )
                {
                    if ( ezbus_packet_seq(rx_packet) == ezbus_packet_seq(tx_packet) )
                    {
                        ezbus_layer0_transmitter_set_state( ezbus_layer0_transceiver_get_transmitter( layer0_transceiver ), transmitter_state_empty );
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

        case packet_type_boot:
            ezbus_boot_signal_peer_seen( ezbus_layer0_transceiver_get_boot( layer0_transceiver ), rx_packet );
            rc = true;
            break;
    }

    return rc;
}

static bool ezbus_layer0_transceiver_boot_emit( ezbus_layer0_transceiver_t* layer0_transceiver, ezbus_address_t* src_address, uint8_t boot_seq )
{
    if ( ezbus_platform_get_ms_ticks() - layer0_transceiver->boot_time )
    {
        ezbus_packet_t  boot_packet;
        ezbus_packet_init( &boot_packet );
        ezbus_packet_set_type( &boot_packet, packet_type_boot );
        ezbus_packet_set_seq( &boot_packet, layer0_transceiver->boot_seq++ );
        ezbus_address_copy( ezbus_packet_src( &boot_packet ), src_address );
        ezbus_port_send( ezbus_layer0_transmitter_get_port( ezbus_layer0_transceiver_get_transmitter( layer0_transceiver ) ), &boot_packet );
    }
    return true;
}

static void ezbus_layer0_transceiver_boot_callback( ezbus_boot_t* boot, void* arg )
{
    ezbus_layer0_transceiver_t* transceiver = (ezbus_layer0_transceiver_t*)arg;

    switch ( ezbus_boot_get_state( boot ) )
    {
        case boot_state_silent_start:
            ezbus_layer0_transceiver_set_token( transceiver, false );
            break;
        case boot_state_silent_continue:
            break;
        case boot_state_silent_stop:
            ezbus_layer0_transceiver_set_token( transceiver, false );
            break;
        case boot_state_coldboot_start:
            ezbus_layer0_transceiver_set_token( transceiver, false );
            break;
        case boot_state_coldboot_stop:
            break;
        case boot_state_coldboot_continue:
            ezbus_layer0_transceiver_boot_emit( transceiver, &ezbus_self_address, transceiver->boot_seq++ );
            break;
        case boot_state_warmboot_tx_start:
            if ( ++transceiver->boot_seq == 0 )
                transceiver->boot_seq = 1;
            ezbus_layer0_transceiver_set_token( transceiver, true );
            break;
        case boot_state_warmboot_tx_continue:
            ezbus_layer0_transceiver_boot_emit( transceiver, (ezbus_address_t*)&ezbus_warmboot_address, transceiver->boot_seq );
            break;
        case boot_state_warmboot_tx_stop:
            ezbus_layer0_transceiver_set_token( transceiver, false );
            break;
        case boot_state_warmboot_rx_start:
            break;
        case boot_state_warmboot_rx_continue:
            ezbus_layer0_transceiver_boot_emit( transceiver, &ezbus_self_address, boot->seq );
            break;
        case boot_state_warmboot_rx_stop:
            break;
    }
}

