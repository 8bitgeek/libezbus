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
#include <ezbus_mac_warmboot.h>
#include <ezbus_mac_peers.h>
#include <ezbus_hex.h>
#include <ezbus_log.h>
#include <ezbus_timer.h>

#define ezbus_mac_warmboot_set_emit_count(boot,c)      ((boot)->emit_count=(c))
#define ezbus_mac_warmboot_get_emit_count(boot)        ((boot)->emit_count)
#define ezbus_mac_warmboot_inc_emit_count(boot)        ezbus_mac_warmboot_set_emit_count(boot,ezbus_mac_warmboot_get_emit_count(boot)+1)

#define ezbus_mac_warmboot_set_emit_seq(boot,c)        ((boot)->emit_count=(c))
#define ezbus_mac_warmboot_get_emit_seq(boot)          ((boot)->emit_count)
#define ezbus_mac_warmboot_inc_emit_seq(boot)          ezbus_mac_warmboot_set_emit_count(boot,ezbus_mac_warmboot_get_emit_count(boot)+1)

#define ezbus_mac_warmboot_get_warmboot_crc(boot)      (&(boot)->warmboot_crc)

static void ezbus_mac_warmboot_timer_callback_silent           ( ezbus_timer_t* timer, void* arg );
static void ezbus_mac_warmboot_timer_callback_coldboot         ( ezbus_timer_t* timer, void* arg );
static void ezbus_mac_warmboot_timer_callback_warmboot_reply   ( ezbus_timer_t* timer, void* arg );
static void ezbus_mac_warmboot_timer_callback_warmboot_send    ( ezbus_timer_t* timer, void* arg );

static void ezbus_mac_warmboot_init_peer_list                  ( ezbus_mac_warmboot_t* boot );

static void ezbus_mac_warmboot_signal_peer_seen_warmboot_src   ( ezbus_mac_warmboot_t* boot, ezbus_packet_t* packet );
static void ezbus_mac_warmboot_signal_peer_seen_warmboot_dst   ( ezbus_mac_warmboot_t* boot, ezbus_packet_t* packet );
static void ezbus_mac_warmboot_signal_peer_seen_warmboot       ( ezbus_mac_warmboot_t* boot, ezbus_packet_t* packet );
static void ezbus_mac_warmboot_signal_peer_seen_coldboot       ( ezbus_mac_warmboot_t* boot, ezbus_packet_t* packet );

static void do_boot_state_silent_start                 ( ezbus_mac_t* mac );
static void do_boot_state_silent_continue              ( ezbus_mac_t* mac );
static void do_boot_state_silent_stop                  ( ezbus_mac_t* mac );

static void do_boot_state_coldboot_start               ( ezbus_mac_t* mac );
static void do_boot_state_coldboot_continue            ( ezbus_mac_t* mac );
static void do_boot_state_coldboot_stop                ( ezbus_mac_t* mac );

static void do_boot_state_warmboot_tx_first            ( ezbus_mac_t* mac );
static void do_boot_state_warmboot_tx_start            ( ezbus_mac_t* mac );
static void do_boot_state_warmboot_tx_restart          ( ezbus_mac_t* mac );
static void do_boot_state_warmboot_tx_continue         ( ezbus_mac_t* mac );
static void do_boot_state_warmboot_tx_stop             ( ezbus_mac_t* mac );

static void do_boot_state_warmboot_rx_start            ( ezbus_mac_t* mac );
static void do_boot_state_warmboot_rx_continue         ( ezbus_mac_t* mac );
static void do_boot_state_warmboot_rx_stop             ( ezbus_mac_t* mac );


extern void ezbus_mac_warmboot_init( ezbus_mac_t* mac )
{
    ezbus_mac_warmboot_t* boot = ezbus_mac_get_bootstrap( mac );

    ezbus_platform_memset( boot, 0 , sizeof( ezbus_mac_warmboot_t) );

    ezbus_timer_init( &boot->coldboot_timer );
    ezbus_timer_init( &boot->warmboot_reply_timer );
    ezbus_timer_init( &boot->warmboot_send_timer );
    ezbus_timer_init( &boot->silent_timer );

    ezbus_timer_set_callback( &boot->coldboot_timer,       ezbus_mac_warmboot_timer_callback_coldboot, mac );
    ezbus_timer_set_callback( &boot->warmboot_reply_timer, ezbus_mac_warmboot_timer_callback_warmboot_reply, mac );
    ezbus_timer_set_callback( &boot->warmboot_send_timer,  ezbus_mac_warmboot_timer_callback_warmboot_send, mac );
    ezbus_timer_set_callback( &boot->silent_timer,         ezbus_mac_warmboot_timer_callback_silent, mac );
}


extern void ezbus_mac_warmboot_run( ezbus_mac_t* mac )
{
    // ezbus_mac_warmboot_t* boot = ezbus_mac_get_bootstrap( mac );

    static ezbus_mac_warmboot_state_t boot_state=(ezbus_mac_warmboot_state_t)0xff;

    if ( ezbus_mac_warmboot_get_state( mac ) != boot_state )
    {
        ezbus_log( EZBUS_LOG_BOOTSTATE, "%s\n", ezbus_mac_warmboot_get_state_str(mac) );
        boot_state = ezbus_mac_warmboot_get_state( mac );
    }

    switch ( ezbus_mac_warmboot_get_state( mac ) )
    {
        case boot_state_silent_start:         do_boot_state_silent_start         ( mac );  break;
        case boot_state_silent_continue:      do_boot_state_silent_continue      ( mac );  break;
        case boot_state_silent_stop:          do_boot_state_silent_stop          ( mac );  break;

        case boot_state_coldboot_start:       do_boot_state_coldboot_start       ( mac );  break;
        case boot_state_coldboot_continue:    do_boot_state_coldboot_continue    ( mac );  break;
        case boot_state_coldboot_stop:        do_boot_state_coldboot_stop        ( mac );  break;

        case boot_state_warmboot_tx_first:    do_boot_state_warmboot_tx_first    ( mac );  break;
        case boot_state_warmboot_tx_start:    do_boot_state_warmboot_tx_start    ( mac );  break;
        case boot_state_warmboot_tx_restart:  do_boot_state_warmboot_tx_restart  ( mac );  break;
        case boot_state_warmboot_tx_continue: do_boot_state_warmboot_tx_continue ( mac );  break;
        case boot_state_warmboot_tx_stop:     do_boot_state_warmboot_tx_stop     ( mac );  break;

        case boot_state_warmboot_rx_start:    do_boot_state_warmboot_rx_start    ( mac );  break;
        case boot_state_warmboot_rx_continue: do_boot_state_warmboot_rx_continue ( mac );  break;
        case boot_state_warmboot_rx_stop:     do_boot_state_warmboot_rx_stop     ( mac );  break;
    }
}

extern const char* ezbus_mac_warmboot_get_state_str( ezbus_mac_t* mac )
{
    ezbus_mac_warmboot_t* boot = ezbus_mac_get_bootstrap( mac );

    switch(boot->state)
    {
        case boot_state_silent_start:               return "boot_state_silent_start";           break;
        case boot_state_silent_continue:            return "boot_state_silent_continue";        break;
        case boot_state_silent_stop:                return "boot_state_silent_stop";            break;
        
        case boot_state_coldboot_start:             return "boot_state_coldboot_start";         break;
        case boot_state_coldboot_continue:          return "boot_state_coldboot_continue";      break;
        case boot_state_coldboot_stop:              return "boot_state_coldboot_stop";          break;

        case boot_state_warmboot_tx_first:          return "boot_state_warmboot_tx_first";      break;
        case boot_state_warmboot_tx_start:          return "boot_state_warmboot_tx_start";      break;
        case boot_state_warmboot_tx_restart:        return "boot_state_warmboot_tx_restart";    break;
        case boot_state_warmboot_tx_continue:       return "boot_state_warmboot_tx_continue";   break;
        case boot_state_warmboot_tx_stop:           return "boot_state_warmboot_tx_stop";       break;

        case boot_state_warmboot_rx_start:          return "boot_state_warmboot_rx_start";      break;
        case boot_state_warmboot_rx_continue:       return "boot_state_warmboot_rx_continue";   break;
        case boot_state_warmboot_rx_stop:           return "boot_state_warmboot_rx_stop";       break;

    }
    return "";
}


void ezbus_mac_warmboot_set_state( ezbus_mac_t* mac, ezbus_mac_warmboot_state_t state )
{
    ezbus_mac_warmboot_t* boot = ezbus_mac_get_bootstrap( mac );
    boot->state = state;
}


ezbus_mac_warmboot_state_t ezbus_mac_warmboot_get_state( ezbus_mac_t* mac )
{
    ezbus_mac_warmboot_t* boot = ezbus_mac_get_bootstrap( mac );
    return boot->state;
}


/**** SILENT BEGIN ****/

static void do_boot_state_silent_start( ezbus_mac_t* mac )
{
    ezbus_mac_warmboot_t* boot = ezbus_mac_get_bootstrap( mac );
    ezbus_mac_warmboot_set_emit_count( boot, 0 );
    ezbus_timer_stop( &boot->coldboot_timer );
    ezbus_timer_stop( &boot->silent_timer );
    ezbus_timer_stop( &boot->warmboot_send_timer );
    ezbus_timer_stop( &boot->warmboot_reply_timer );
    ezbus_timer_set_period  ( 
                                &boot->silent_timer, 
                                ezbus_timing_ring_time( 
                                    ezbus_port_get_speed( 
                                        ezbus_mac_warmboot_get_port( boot ) ), 
                                        ezbus_peer_list_count( ezbus_mac_warmboot_get_peer_list( boot ) ) ) + 
                                            ezbus_platform_random( 500, 1000 )
                            );
    ezbus_timer_start( &boot->silent_timer );
    ezbus_mac_warmboot_signal_silent_start(boot);
    ezbus_mac_warmboot_set_state( boot, boot_state_silent_continue );

}

static void do_boot_state_silent_continue( ezbus_mac_t* mac )
{
    ezbus_mac_warmboot_t* boot = ezbus_mac_get_bootstrap( mac );
    ezbus_mac_warmboot_signal_silent_continue(boot);
}

static void do_boot_state_silent_stop( ezbus_mac_t* mac )
{
    ezbus_mac_warmboot_t* boot = ezbus_mac_get_bootstrap( mac );
    ezbus_timer_stop( &boot->coldboot_timer );
    ezbus_timer_stop( &boot->silent_timer );
    ezbus_timer_stop( &boot->warmboot_send_timer );
    ezbus_timer_stop( &boot->warmboot_reply_timer );
    ezbus_mac_warmboot_signal_silent_stop(boot);
    ezbus_mac_warmboot_set_state( boot, boot_state_coldboot_start );
}

/**** SILENT ENDD ****/



/**** COLDBOOT BEGIN ****/

static void do_boot_state_coldboot_start( ezbus_mac_t* mac )
{
    ezbus_mac_warmboot_t* boot = ezbus_mac_get_bootstrap( mac );
    ezbus_timer_stop( &boot->silent_timer );
    ezbus_timer_stop( &boot->coldboot_timer );
    ezbus_mac_warmboot_signal_coldboot_start(boot);
    ezbus_timer_set_period  ( 
                                &boot->coldboot_timer, 
                                /* ezbus_timing_ring_time( boot->baud_rate, ezbus_peer_list_count( boot->peer_list ) ) + */
                                    ezbus_platform_random( EZBUS_EMIT_TIMER_MIN, EZBUS_EMIT_TIMER_MAX ) 
                            );
    ezbus_timer_start( &boot->coldboot_timer );
    ezbus_mac_warmboot_set_state( boot, boot_state_coldboot_continue );
}

static void do_boot_state_coldboot_continue( ezbus_mac_t* mac )
{
    ezbus_mac_warmboot_t* boot = ezbus_mac_get_bootstrap( mac );
    ezbus_mac_warmboot_signal_coldboot_continue(boot);
    /* If I'm the "last man standing" then seize control of the bus */
    if ( ezbus_mac_warmboot_get_emit_count( boot ) > EZBUS_EMIT_CYCLES )
    {
        ezbus_timer_stop( &boot->coldboot_timer );
        ezbus_mac_warmboot_set_state( boot, boot_state_warmboot_tx_first );
    }
}

static void do_boot_state_coldboot_stop( ezbus_mac_t* mac )
{
    ezbus_mac_warmboot_t* boot = ezbus_mac_get_bootstrap( mac );
    ezbus_mac_warmboot_signal_coldboot_stop(boot);
    ezbus_timer_stop( &boot->coldboot_timer );
    ezbus_mac_warmboot_set_state( boot, boot_state_silent_start );
}

static void ezbus_mac_warmboot_timer_callback_coldboot( ezbus_timer_t* timer, void* arg )
{
    ezbus_mac_t* mac = (ezbus_mac_t*)arg;
    ezbus_mac_warmboot_t* boot = ezbus_mac_get_bootstrap( mac );
    if ( ezbus_timer_expired( timer ) )
    {
        ezbus_mac_warmboot_inc_emit_count( boot );
        ezbus_mac_warmboot_set_state( boot, boot_state_coldboot_start );
    }
}

/**** COLDBOOT END ****/




/**** WARMBOOT_TX BEGIN ****/


static void do_boot_state_warmboot_tx_first( ezbus_mac_t* mac )
{
    ezbus_mac_warmboot_t* boot = ezbus_mac_get_bootstrap( mac );
    ezbus_timer_stop( &boot->coldboot_timer );
    ezbus_timer_stop( &boot->silent_timer );
    ezbus_mac_warmboot_signal_warmboot_tx_first(boot);
    ezbus_mac_warmboot_set_state( boot, boot_state_warmboot_tx_start );
}

static void do_boot_state_warmboot_tx_start( ezbus_mac_t* mac )
{
    ezbus_mac_warmboot_t* boot = ezbus_mac_get_bootstrap( mac );
    ezbus_timer_stop( &boot->coldboot_timer );
    ezbus_timer_stop( &boot->silent_timer );
    boot->warmboot_count = 0;
    ezbus_mac_warmboot_init_peer_list( boot );
    ezbus_mac_warmboot_signal_warmboot_tx_start(boot);
    ezbus_mac_warmboot_set_state( boot, boot_state_warmboot_tx_restart );
}

static void do_boot_state_warmboot_tx_restart( ezbus_mac_t* mac )
{
    ezbus_mac_warmboot_t* boot = ezbus_mac_get_bootstrap( mac );
    ezbus_peer_list_crc( ezbus_mac_warmboot_get_peer_list(boot), ezbus_mac_warmboot_get_warmboot_crc(boot) );
    ezbus_mac_warmboot_set_emit_count( boot, 0 );
 
    ezbus_timer_stop( &boot->warmboot_send_timer );
    ezbus_timer_set_period  ( 
                                &boot->warmboot_send_timer, 
                                EZBUS_WARMBOOT_TIMER_MAX
                            );
 
    ezbus_timer_stop( &boot->warmboot_reply_timer );
    ezbus_timer_set_period  ( 
                                &boot->warmboot_reply_timer,  
                                ezbus_platform_random( EZBUS_WARMBOOT_TIMER_MIN, EZBUS_WARMBOOT_TIMER_MAX )
                            );
 
     ezbus_mac_warmboot_signal_warmboot_tx_restart(boot);

    ezbus_timer_start( &boot->warmboot_send_timer );
    ezbus_timer_start( &boot->warmboot_reply_timer );

    ezbus_mac_warmboot_set_state( boot, boot_state_warmboot_tx_continue );
}

static void do_boot_state_warmboot_tx_continue( ezbus_mac_t* mac )
{
    ezbus_mac_warmboot_t* boot = ezbus_mac_get_bootstrap( mac );
     ezbus_mac_warmboot_signal_warmboot_tx_continue(boot);
}

static void do_boot_state_warmboot_tx_stop( ezbus_mac_t* mac )
{
    ezbus_mac_warmboot_t* boot = ezbus_mac_get_bootstrap( mac );
    ezbus_crc_t peers_crc;
    ezbus_timer_stop( &boot->warmboot_send_timer );
    ezbus_timer_stop( &boot->warmboot_reply_timer );

    /* has the peer list changed? */
    ezbus_peer_list_crc( ezbus_mac_warmboot_get_peer_list(boot), &peers_crc );
    if ( ezbus_crc_equal( &peers_crc, ezbus_mac_warmboot_get_warmboot_crc(boot) ) )
    {
        if ( ++boot->warmboot_count >= EZBUS_EMIT_CYCLES  )
        {
            ezbus_mac_warmboot_signal_warmboot_tx_stop(boot);
            ezbus_mac_warmboot_set_state( boot, boot_state_silent_start );            
        }
        else
        {
            ezbus_mac_warmboot_set_state( boot, boot_state_warmboot_tx_restart ); 
        }
    }
    else
    {
        boot->warmboot_count = 0;
        ezbus_mac_warmboot_set_state( boot, boot_state_warmboot_tx_restart ); 
    }
}

static void ezbus_mac_warmboot_timer_callback_warmboot_send( ezbus_timer_t* timer, void* arg )
{
    ezbus_mac_t* mac = (ezbus_mac_t*)arg;
    ezbus_mac_warmboot_t* boot = ezbus_mac_get_bootstrap( mac );
    ezbus_mac_warmboot_set_state( boot, boot_state_warmboot_tx_stop );
}

/**** WARMBOOT_TX END ****/



/**** WARMBOOT_RX  BEGIN ****/

static void do_boot_state_warmboot_rx_start( ezbus_mac_t* mac )
{
    ezbus_mac_warmboot_set_emit_count( boot, 0 );
    ezbus_timer_stop( &boot->silent_timer );
    ezbus_timer_stop( &boot->warmboot_reply_timer );
    ezbus_timer_stop( &boot->coldboot_timer );

    ezbus_timer_set_period  ( 
                                &boot->warmboot_reply_timer,  
                                ezbus_platform_random( EZBUS_WARMBOOT_TIMER_MIN, EZBUS_WARMBOOT_TIMER_MAX )
                            );
    ezbus_mac_warmboot_signal_warmboot_rx_start(boot);
    ezbus_timer_start( &boot->warmboot_reply_timer );
    ezbus_mac_warmboot_set_state( boot, boot_state_warmboot_rx_continue );
}

static void do_boot_state_warmboot_rx_continue( ezbus_mac_t* mac )
{
    ezbus_mac_warmboot_signal_warmboot_rx_continue(boot);
}

static void do_boot_state_warmboot_rx_stop( ezbus_mac_t* mac )
{
    ezbus_timer_stop( &boot->warmboot_reply_timer );
    ezbus_mac_warmboot_signal_warmboot_rx_stop(boot);
    ezbus_mac_warmboot_set_state( boot, boot_state_silent_start );
}

static void ezbus_mac_warmboot_timer_callback_warmboot_reply( ezbus_timer_t* timer, void* arg )
{
    ezbus_mac_t* mac = (ezbus_mac_t*)arg;
    ezbus_mac_warmboot_t* boot = ezbus_mac_get_bootstrap( mac );
    switch( ezbus_mac_warmboot_get_state( boot ) )
    {
        case boot_state_warmboot_tx_stop:  

            ezbus_mac_warmboot_set_state( boot, boot_state_warmboot_tx_stop );
            break;

        case boot_state_warmboot_rx_continue:    

            ezbus_mac_warmboot_set_state( boot, boot_state_warmboot_rx_stop );
        
            break;

        default:
            break;
    }
 }

/* WARMBOOT_RX END */



/**
 * @brief The source address is the warmboot designate. Meaning we should reply to warmboot adderss.
 */
static void ezbus_mac_warmboot_signal_peer_seen_warmboot_src( ezbus_mac_warmboot_t* boot, ezbus_packet_t* packet )
{
    ezbus_peer_t peer;
    ezbus_peer_init( &peer, ezbus_packet_dst( packet ), ezbus_packet_seq( packet ) );

    ezbus_peer_list_clean( ezbus_mac_warmboot_get_peer_list(boot), ezbus_packet_seq( packet ) );
    ezbus_peer_list_insort( ezbus_mac_warmboot_get_peer_list(boot), &peer );

    if ( ezbus_address_compare( ezbus_packet_dst( packet ), &ezbus_broadcast_address ) == 0 )
    {
        ezbus_mac_warmboot_set_state( boot, boot_state_warmboot_rx_start );
    }
    else
    if ( ezbus_address_compare( ezbus_packet_dst( packet ), &ezbus_self_address ) == 0 )
    {
        if ( ezbus_packet_seq( packet ) != boot->seq )
        {
            ezbus_mac_warmboot_set_state( boot, boot_state_silent_start );
            boot->seq = ezbus_packet_seq( packet );
        }
    }
}

/**
 * @brief The destination address is the warmboot designate. Meaning we should sniff the source address as a peer..
 */
static void ezbus_mac_warmboot_signal_peer_seen_warmboot_dst( ezbus_mac_warmboot_t* boot, ezbus_packet_t* packet )
{
    ezbus_peer_t peer;
    ezbus_peer_init( &peer, ezbus_packet_src( packet ), ezbus_packet_seq( packet ) );

    ezbus_peer_list_clean( ezbus_mac_warmboot_get_peer_list(boot), ezbus_packet_seq( packet ) );
    ezbus_peer_list_insort( ezbus_mac_warmboot_get_peer_list(boot), &peer );
}

static void ezbus_mac_warmboot_signal_peer_seen_warmboot( ezbus_mac_warmboot_t* boot, ezbus_packet_t* packet )
{
    if ( ezbus_address_compare( &ezbus_warmboot_address, ezbus_packet_src( packet ) ) == 0 )
    {
        ezbus_mac_warmboot_signal_peer_seen_warmboot_src( boot, packet );
    }
    else
    if ( ezbus_address_compare( &ezbus_warmboot_address, ezbus_packet_dst( packet ) ) == 0 )
    {
        ezbus_mac_warmboot_signal_peer_seen_warmboot_dst( boot, packet );
    }
}

/**
 * @brief During cold boot, we've seen a peer source address, so rercord it, and go dormant if 
 *        the address is more dominant than self.
 */
static void ezbus_mac_warmboot_signal_peer_seen_coldboot( ezbus_mac_warmboot_t* boot, ezbus_packet_t* packet )
{
    ezbus_peer_t peer;

    ezbus_peer_init( &peer, ezbus_packet_src( packet ), ezbus_packet_seq( packet ) );
    ezbus_peer_list_clean( ezbus_mac_warmboot_get_peer_list(boot), ezbus_packet_seq( packet ) );
    ezbus_peer_list_insort( ezbus_mac_warmboot_get_peer_list(boot), &peer );

    if ( ezbus_address_compare( &ezbus_self_address, ezbus_packet_src( packet ) ) > 0 )
    {
        if ( ezbus_mac_warmboot_get_state( boot ) == boot_state_coldboot_continue )
        {
            ezbus_timer_stop( &boot->coldboot_timer );
            ezbus_mac_warmboot_set_state( boot, boot_state_silent_start );
        } 
    }
}

extern void ezbus_mac_warmboot_signal_peer_seen( ezbus_mac_warmboot_t* boot, ezbus_packet_t* packet )
{
    if ( ezbus_packet_is_warmboot( packet ) )
    {
        ezbus_mac_warmboot_signal_peer_seen_warmboot( boot, packet );
    }
    else
    {
        ezbus_mac_warmboot_signal_peer_seen_coldboot( boot, packet );
    }
}

extern void ezbus_mac_warmboot_signal_token_seen( ezbus_mac_warmboot_t* boot, ezbus_packet_t* packet )
{
    ezbus_peer_t peer;

    ezbus_peer_init( &peer, ezbus_packet_src( packet ), ezbus_packet_seq( packet ) );
    ezbus_peer_list_clean( ezbus_mac_warmboot_get_peer_list(boot), ezbus_packet_seq( packet ) );
    ezbus_peer_list_insort( ezbus_mac_warmboot_get_peer_list(boot), &peer );

    ezbus_mac_warmboot_set_state( boot, boot_state_silent_start );
}

static void ezbus_mac_warmboot_timer_callback_silent( ezbus_timer_t* timer, void* arg )
{
    ezbus_mac_warmboot_t* boot=(ezbus_mac_warmboot_t*)arg;
    if ( ezbus_timer_expired( timer ) )
    {
        ezbus_mac_warmboot_set_state( boot, boot_state_silent_stop );
    }
}

static void ezbus_mac_warmboot_init_peer_list( ezbus_mac_warmboot_t* boot )
{
    ezbus_peer_t self_peer;

    ezbus_peer_list_clear( ezbus_mac_warmboot_get_peer_list(boot) );
    ezbus_peer_init( &self_peer, &ezbus_self_address, boot->seq );
    ezbus_peer_list_insort( ezbus_mac_warmboot_get_peer_list(boot), &self_peer );    
}




extern void ezbus_mac_arbitration_receive_signal_warmboot( ezbus_mac_arbitration_receive_t* arbitration_receive, ezbus_packet_t* rx_packet )
{
    ezbus_log( EZBUS_LOG_WARMBOOT, "%cwarmboot <%s %3d | ", ezbus_mac_warmboot_get_token(mac)?'*':' ', ezbus_address_string( ezbus_packet_src( rx_packet ) ), ezbus_packet_seq( rx_packet ) );
    #if EZBUS_LOG_WARMBOOT
        ezbus_peer_list_log( &mac->peer_list );
    #endif
}
