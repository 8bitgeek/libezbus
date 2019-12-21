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
#include <ezbus_mac.h>
#include <ezbus_hex.h>
#include <ezbus_log.h>
#if defined(__linux__)
    #include <unistd.h>         /* FIXME */
#endif

#define EZBUS_HELLO_TIMER_MIN   1
#define EZBUS_HELLO_TIMER_MAX   50

static bool ezbus_mac_give_token         ( ezbus_mac_t* mac );
static bool ezbus_mac_accept_token       ( ezbus_mac_t* mac, ezbus_packet_t* rx_packet );
static bool ezbus_mac_acknowledge_token  ( ezbus_mac_t* mac, ezbus_packet_t* packet );
static bool ezbus_mac_prepare_ack        ( ezbus_mac_t* mac );
static bool ezbus_mac_send_ack           ( ezbus_mac_t* mac );
static bool ezbus_mac_recv_packet        ( ezbus_mac_t* mac );
static bool ezbus_mac_boot_emit          ( ezbus_mac_t* mac, ezbus_address_t* src_address, ezbus_address_t* dst_address, uint8_t boot_seq );
static bool ezbus_mac_tx_callback        ( ezbus_mac_transmitter_t* mac_transmitter, void* arg );
static bool ezbus_mac_rx_callback        ( ezbus_mac_receiver_t*    mac_receiver,    void* arg );
static void ezbus_mac_boot_callback      ( ezbus_boot_t* boot, void* arg );

void ezbus_mac_init (    
                                        ezbus_mac_t*             mac, 
                                        ezbus_port_t*                           port,
                                        ezbus_layer1_callback_t                 layer1_tx_callback,
                                        ezbus_layer1_callback_t                 layer1_rx_callback
                                    )
{
    ezbus_address_init();

    mac->port = port;

    mac->layer1_tx_callback = layer1_tx_callback;
    mac->layer1_rx_callback = layer1_rx_callback;

    ezbus_peer_list_init( &mac->peer_list );

    ezbus_mac_receiver_init    ( ezbus_mac_get_receiver    ( mac ), port, ezbus_mac_rx_callback, mac );
    ezbus_mac_transmitter_init ( ezbus_mac_get_transmitter ( mac ), port, ezbus_mac_tx_callback, mac );

    ezbus_timer_init( &mac->ack_tx_timer );
    ezbus_timer_init( &mac->ack_rx_timer );

    ezbus_boot_init(   
                        ezbus_mac_get_boot( mac ), 
                        ezbus_port_get_speed(port), 
                        &mac->peer_list,
                        ezbus_mac_boot_callback,
                        mac
                    );
}

void ezbus_mac_run  ( ezbus_mac_t* mac )
{
    ezbus_timer_run( &mac->ack_tx_timer );
    ezbus_timer_run( &mac->ack_rx_timer );

    ezbus_mac_receiver_run    ( ezbus_mac_get_receiver   ( mac ) );
    ezbus_mac_transmitter_run ( ezbus_mac_get_transmitter( mac ) );

    ezbus_boot_run( ezbus_mac_get_boot( mac ) );
}


static bool ezbus_mac_tx_callback( ezbus_mac_transmitter_t* mac_transmitter, void* arg )
{
    bool rc = false;
    ezbus_mac_t* mac = (ezbus_mac_t*)arg;

    switch( ezbus_mac_transmitter_get_state( mac_transmitter ) )
    {
        case transmitter_state_empty:
            // if ( ezbus_mac_get_token( mac ) )   // ??
            // {
            //     if ( !( rc = mac->layer1_tx_callback( mac ) ) )
            //     {
            //         ezbus_mac_transmitter_set_state( mac_transmitter, transmitter_state_give_token );
            //     }
            // }
            // ezbus_mac_set_ack_tx_retry( mac, EZBUS_RETRANSMIT_TRIES );
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
            rc = ezbus_mac_give_token( mac );
            ezbus_mac_set_token( mac, !rc );
            break;
        case transmitter_state_transit_wait_ack:

            ezbus_mac_set_ack_tx_begin( mac, ezbus_platform_get_ms_ticks() );

            rc = true;
            break;
        case transmitter_state_wait_ack:

            // if ( ezbus_platform_get_ms_ticks() - ezbus_mac_get_ack_tx_begin( mac ) > ezbus_mac_tx_ack_timeout( mac ) )
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

static bool ezbus_mac_rx_callback( ezbus_mac_receiver_t* mac_receiver, void* arg )
{
    bool rc=false;
    ezbus_mac_t* mac = (ezbus_mac_t*)arg;
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

static bool ezbus_mac_give_token( ezbus_mac_t* mac )
{
    bool rc=true;
    ezbus_packet_t  tx_packet;
    ezbus_address_t peer_address;

    ezbus_address_copy( &peer_address, ezbus_peer_list_next( &mac->peer_list, &ezbus_self_address ) );

    ezbus_log( EZBUS_LOG_TOKEN, "tok> %s ", ezbus_address_string(&ezbus_self_address) );
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
        #if defined(__linux__)
            usleep(1000*20);
        #endif
 
        rc=false;
    }

    return rc;
}


static bool ezbus_mac_accept_token( ezbus_mac_t* mac, ezbus_packet_t* rx_packet )
{
    ezbus_log( EZBUS_LOG_TOKEN, "tok< %s ", ezbus_address_string(ezbus_packet_dst( rx_packet )) );
    ezbus_log( EZBUS_LOG_TOKEN, "< %s\n",   ezbus_address_string(ezbus_packet_src( rx_packet )) );

    if ( ezbus_address_compare( &ezbus_self_address, ezbus_packet_dst( rx_packet ) ) == 0 )
    {
        ezbus_mac_acknowledge_token( mac, rx_packet );
    }
    else
    {
        ezbus_boot_signal_token_seen( ezbus_mac_get_boot( mac ), rx_packet );
        ezbus_log( EZBUS_LOG_TOKEN, "tok< %s\n", ezbus_address_string( ezbus_packet_src( rx_packet ) ) );

    }
    return true;
}


static bool ezbus_mac_acknowledge_token( ezbus_mac_t* mac, ezbus_packet_t* packet )
{
    ezbus_crc_t peer_list_crc;

    mac->token_counter = packet->data.attachment.token.token_counter;
    ezbus_peer_list_crc( &mac->peer_list, &peer_list_crc );
    ezbus_mac_set_token( mac, true );
    ezbus_boot_signal_token_seen( ezbus_mac_get_boot( mac ), packet );
    ezbus_log( EZBUS_LOG_TOKEN, "tok< %s\n", ezbus_address_string( ezbus_packet_src( packet ) ) );
    if ( !ezbus_crc_equal( &peer_list_crc, &packet->data.attachment.token.peer_list_crc ) )
    {
        ezbus_log( EZBUS_LOG_TOKEN, "tok crc bad< expect 0x%04X got 0x%04X ", peer_list_crc, packet->data.attachment.token.peer_list_crc );
        /* FIXME - jump to warm-boot state here ?? */
    }
    return true;
}

static bool ezbus_mac_prepare_ack( ezbus_mac_t* mac )
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

static bool ezbus_mac_send_ack( ezbus_mac_t* mac )
{
    ezbus_port_send( 
            ezbus_mac_transmitter_get_port( 
                ezbus_mac_get_transmitter( mac ) ), 
                    ezbus_mac_get_ack_rx_packet( mac ) );

    return true; /* FIXME ?? */
}

static bool ezbus_mac_recv_packet( ezbus_mac_t* mac )
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

        case packet_type_boot:
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

static bool ezbus_mac_boot_emit( ezbus_mac_t* mac, ezbus_address_t* src_address, ezbus_address_t* dst_address, uint8_t boot_seq )
{
    if ( ezbus_platform_get_ms_ticks() - mac->boot_time )
    {
        ezbus_packet_t* boot_packet = ezbus_mac_transmitter_get_packet( ezbus_mac_get_transmitter( mac ) );
        ezbus_packet_init( boot_packet );
        ezbus_packet_set_type( boot_packet, packet_type_boot );
        ezbus_packet_set_seq( boot_packet, boot_seq );
        ezbus_address_copy( ezbus_packet_src( boot_packet ), src_address );
        ezbus_address_copy( ezbus_packet_dst( boot_packet ), dst_address );
        ezbus_port_send( ezbus_mac_transmitter_get_port( ezbus_mac_get_transmitter( mac ) ), boot_packet );
    }
    return true;
}

static void ezbus_mac_boot_callback( ezbus_boot_t* boot, void* arg )
{
    ezbus_mac_t* mac = (ezbus_mac_t*)arg;

    switch ( ezbus_boot_get_state( boot ) )
    {
        case boot_state_silent_start:
            break;
        case boot_state_silent_continue:
            break;
        case boot_state_silent_stop:
            mac->coldboot_seq=0;
            break;
        case boot_state_coldboot_start:
            ezbus_mac_set_token( mac, false );
            break;
        case boot_state_coldboot_continue:
            ezbus_log( EZBUS_LOG_COLDBOOT, "%ccoldboot> %s %3d | ", ezbus_mac_get_token(mac)?'*':' ', ezbus_address_string( &ezbus_self_address ), mac->coldboot_seq );
            ezbus_mac_boot_emit( mac, &ezbus_self_address, (ezbus_address_t*)&ezbus_broadcast_address, mac->coldboot_seq++ );
            #if EZBUS_LOG_COLDBOOT
                ezbus_peer_list_log( &mac->peer_list );
            #endif
            break;
        case boot_state_coldboot_stop:
            mac->coldboot_seq = 0;
            break;
        case boot_state_warmboot_tx_start:
            if ( ++mac->warmboot_seq == 0 )
                mac->warmboot_seq = 1;
            break;
        case boot_state_warmboot_tx_restart:
            ezbus_mac_set_token( mac, true );
            ezbus_log( EZBUS_LOG_WARMBOOT, "%cwarmboot} %s %3d | ", ezbus_mac_get_token(mac)?'*':' ', ezbus_address_string( (ezbus_address_t*)&ezbus_warmboot_address ), mac->warmboot_seq );
            ezbus_mac_boot_emit( mac, (ezbus_address_t*)&ezbus_warmboot_address, (ezbus_address_t*)&ezbus_broadcast_address, mac->warmboot_seq );
            ezbus_boot_signal_peer_seen( ezbus_mac_get_boot( mac ), ezbus_mac_transmitter_get_packet( ezbus_mac_get_transmitter( mac ) ) );
            #if EZBUS_LOG_WARMBOOT
                ezbus_peer_list_log( &mac->peer_list );
            #endif
            break;
        case boot_state_warmboot_tx_continue:
            ezbus_log( EZBUS_LOG_WARMBOOT, "%cwarmboot> %s %3d | ", ezbus_mac_get_token(mac)?'*':' ', ezbus_address_string( &ezbus_self_address ), mac->warmboot_seq );
            ezbus_mac_boot_emit( mac, &ezbus_self_address, (ezbus_address_t*)&ezbus_warmboot_address, mac->warmboot_seq );
            ezbus_boot_signal_peer_seen( ezbus_mac_get_boot( mac ), ezbus_mac_transmitter_get_packet( ezbus_mac_get_transmitter( mac ) ) );
            #if EZBUS_LOG_WARMBOOT
                ezbus_peer_list_log( &mac->peer_list );
            #endif            
            break;
        case boot_state_warmboot_tx_stop:
            break;
        case boot_state_warmboot_rx_start:
            break;
        case boot_state_warmboot_rx_continue:
            ezbus_mac_boot_emit( mac, &ezbus_self_address, (ezbus_address_t*)&ezbus_warmboot_address, ezbus_packet_seq( ezbus_mac_receiver_get_packet( ezbus_mac_get_receiver( mac ) ) ) );
            break;
        case boot_state_warmboot_rx_stop:
            break;
    }
}

