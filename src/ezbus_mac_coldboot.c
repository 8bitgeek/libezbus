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
#include <ezbus_mac_coldboot.h>
#include <ezbus_mac_peers.h>
#include <ezbus_hex.h>
#include <ezbus_log.h>
#include <ezbus_timer.h>

#define ezbus_mac_coldboot_set_emit_count(boot,c)      ((boot)->emit_count=(c))
#define ezbus_mac_coldboot_get_emit_count(boot)        ((boot)->emit_count)
#define ezbus_mac_coldboot_inc_emit_count(boot)        ezbus_mac_coldboot_set_emit_count(boot,ezbus_mac_coldboot_get_emit_count(boot)+1)

#define ezbus_mac_coldboot_set_emit_seq(boot,c)        ((boot)->emit_count=(c))
#define ezbus_mac_coldboot_get_emit_seq(boot)          ((boot)->emit_count)
#define ezbus_mac_coldboot_inc_emit_seq(boot)          ezbus_mac_coldboot_set_emit_count(boot,ezbus_mac_coldboot_get_emit_count(boot)+1)

static void ezbus_mac_coldboot_timer_callback_silent   ( ezbus_timer_t* timer, void* arg );
static void ezbus_mac_coldboot_timer_callback          ( ezbus_timer_t* timer, void* arg );

static void ezbus_mac_coldboot_init_peers              ( ezbus_mac_coldboot_t* boot );

static void do_boot_state_silent_start                 ( ezbus_mac_t* mac );
static void do_boot_state_silent_continue              ( ezbus_mac_t* mac );
static void do_boot_state_silent_stop                  ( ezbus_mac_t* mac );

static void do_boot_state_coldboot_start               ( ezbus_mac_t* mac );
static void do_boot_state_coldboot_continue            ( ezbus_mac_t* mac );
static void do_boot_state_coldboot_stop                ( ezbus_mac_t* mac );


extern void ezbus_mac_coldboot_init( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_t* boot = ezbus_mac_get_bootstrap( mac );

    ezbus_platform_memset( boot, 0 , sizeof( ezbus_mac_coldboot_t) );

    ezbus_timer_init( &boot->coldboot_timer );
    ezbus_timer_init( &boot->warmboot_reply_timer );
    ezbus_timer_init( &boot->warmboot_send_timer );
    ezbus_timer_init( &boot->silent_timer );

    ezbus_timer_set_callback( &boot->coldboot_timer,       ezbus_mac_coldboot_timer_callback, mac );
    ezbus_timer_set_callback( &boot->warmboot_reply_timer, ezbus_mac_coldboot_timer_callback_warmboot_reply, mac );
    ezbus_timer_set_callback( &boot->warmboot_send_timer,  ezbus_mac_coldboot_timer_callback_warmboot_send, mac );
    ezbus_timer_set_callback( &boot->silent_timer,         ezbus_mac_coldboot_timer_callback_silent, mac );
}


extern void ezbus_mac_coldboot_run( ezbus_mac_t* mac )
{
    // ezbus_mac_coldboot_t* boot = ezbus_mac_get_bootstrap( mac );

    static ezbus_mac_coldboot_state_t boot_state=(ezbus_mac_coldboot_state_t)0xff;

    if ( ezbus_mac_coldboot_get_state( mac ) != boot_state )
    {
        ezbus_log( EZBUS_LOG_BOOTSTATE, "%s\n", ezbus_mac_coldboot_get_state_str(mac) );
        boot_state = ezbus_mac_coldboot_get_state( mac );
    }

    switch ( ezbus_mac_coldboot_get_state( mac ) )
    {
        case boot_state_silent_start:         do_boot_state_silent_start         ( mac );  break;
        case boot_state_silent_continue:      do_boot_state_silent_continue      ( mac );  break;
        case boot_state_silent_stop:          do_boot_state_silent_stop          ( mac );  break;

        case boot_state_coldboot_start:       do_boot_state_coldboot_start       ( mac );  break;
        case boot_state_coldboot_continue:    do_boot_state_coldboot_continue    ( mac );  break;
        case boot_state_coldboot_stop:        do_boot_state_coldboot_stop        ( mac );  break;
    }
}

extern const char* ezbus_mac_coldboot_get_state_str( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_t* boot = ezbus_mac_get_bootstrap( mac );

    switch(boot->state)
    {
        case boot_state_silent_start:               return "boot_state_silent_start";           break;
        case boot_state_silent_continue:            return "boot_state_silent_continue";        break;
        case boot_state_silent_stop:                return "boot_state_silent_stop";            break;
        
        case boot_state_coldboot_start:             return "boot_state_coldboot_start";         break;
        case boot_state_coldboot_continue:          return "boot_state_coldboot_continue";      break;
        case boot_state_coldboot_stop:              return "boot_state_coldboot_stop";          break;
    }
    return "";
}


void ezbus_mac_coldboot_set_state( ezbus_mac_t* mac, ezbus_mac_coldboot_state_t state )
{
    ezbus_mac_coldboot_t* boot = ezbus_mac_get_bootstrap( mac );
    boot->state = state;
}


ezbus_mac_coldboot_state_t ezbus_mac_coldboot_get_state( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_t* boot = ezbus_mac_get_bootstrap( mac );
    return boot->state;
}


/**** SILENT BEGIN ****/

static void do_boot_state_silent_start( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_t* boot = ezbus_mac_get_bootstrap( mac );
    ezbus_mac_coldboot_set_emit_count( boot, 0 );
    ezbus_timer_stop( &boot->coldboot_timer );
    ezbus_timer_stop( &boot->silent_timer );
    ezbus_timer_stop( &boot->warmboot_send_timer );
    ezbus_timer_stop( &boot->warmboot_reply_timer );
    ezbus_timer_set_period  ( 
                                &boot->silent_timer, 
                                ezbus_timing_ring_time( 
                                    ezbus_port_get_speed( 
                                        ezbus_mac_coldboot_get_port( boot ) ), 
                                        ezbus_mac_peers_count( ezbus_mac_coldboot_get_peers( boot ) ) ) + 
                                            ezbus_platform_random( 500, 1000 )
                            );
    ezbus_timer_start( &boot->silent_timer );
    ezbus_mac_coldboot_signal_silent_start(boot);
    ezbus_mac_coldboot_set_state( boot, boot_state_silent_continue );

}

static void do_boot_state_silent_continue( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_t* boot = ezbus_mac_get_bootstrap( mac );
    ezbus_mac_coldboot_signal_silent_continue(boot);
}

static void do_boot_state_silent_stop( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_t* boot = ezbus_mac_get_bootstrap( mac );
    ezbus_timer_stop( &boot->coldboot_timer );
    ezbus_timer_stop( &boot->silent_timer );
    ezbus_timer_stop( &boot->warmboot_send_timer );
    ezbus_timer_stop( &boot->warmboot_reply_timer );
    ezbus_mac_coldboot_signal_silent_stop(boot);
    ezbus_mac_coldboot_set_state( boot, boot_state_coldboot_start );
}

/**** SILENT ENDD ****/



/**** COLDBOOT BEGIN ****/

static void do_boot_state_coldboot_start( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_t* boot = ezbus_mac_get_bootstrap( mac );
    ezbus_timer_stop( &boot->silent_timer );
    ezbus_timer_stop( &boot->coldboot_timer );
    ezbus_mac_coldboot_signal_coldboot_start(boot);
    ezbus_timer_set_period  ( 
                                &boot->coldboot_timer, 
                                /* ezbus_timing_ring_time( boot->baud_rate, ezbus_mac_peers_count( boot->peers ) ) + */
                                    ezbus_platform_random( EZBUS_EMIT_TIMER_MIN, EZBUS_EMIT_TIMER_MAX ) 
                            );
    ezbus_timer_start( &boot->coldboot_timer );
    ezbus_mac_coldboot_set_state( boot, boot_state_coldboot_continue );
}

static void do_boot_state_coldboot_continue( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_t* boot = ezbus_mac_get_bootstrap( mac );
    ezbus_mac_coldboot_signal_coldboot_continue(boot);
    /* If I'm the "last man standing" then seize control of the bus */
    if ( ezbus_mac_coldboot_get_emit_count( boot ) > EZBUS_EMIT_CYCLES )
    {
        ezbus_timer_stop( &boot->coldboot_timer );
        ezbus_mac_coldboot_set_state( boot, boot_state_warmboot_tx_first );
    }
}

static void do_boot_state_coldboot_stop( ezbus_mac_t* mac )
{
    ezbus_mac_coldboot_t* boot = ezbus_mac_get_bootstrap( mac );
    ezbus_mac_coldboot_signal_coldboot_stop(boot);
    ezbus_timer_stop( &boot->coldboot_timer );
    ezbus_mac_coldboot_set_state( boot, boot_state_silent_start );
}

static void ezbus_mac_coldboot_timer_callback( ezbus_timer_t* timer, void* arg )
{
    ezbus_mac_t* mac = (ezbus_mac_t*)arg;
    ezbus_mac_coldboot_t* boot = ezbus_mac_get_bootstrap( mac );
    if ( ezbus_timer_expired( timer ) )
    {
        ezbus_mac_coldboot_inc_emit_count( boot );
        ezbus_mac_coldboot_set_state( boot, boot_state_coldboot_start );
    }
}

/**** COLDBOOT END ****/

extern void ezbus_mac_coldboot_signal_peer_seen( ezbus_mac_coldboot_t* boot, ezbus_packet_t* packet )
{
    ezbus_peer_t peer;

    ezbus_peer_init( &peer, ezbus_packet_src( packet ), ezbus_packet_seq( packet ) );
    ezbus_mac_peers_clean( ezbus_mac_coldboot_get_peers(boot), ezbus_packet_seq( packet ) );
    ezbus_mac_peers_insort( ezbus_mac_coldboot_get_peers(boot), &peer );

    if ( ezbus_address_compare( &ezbus_self_address, ezbus_packet_src( packet ) ) > 0 )
    {
        if ( ezbus_mac_coldboot_get_state( boot ) == boot_state_coldboot_continue )
        {
            ezbus_timer_stop( &boot->coldboot_timer );
            ezbus_mac_coldboot_set_state( boot, boot_state_silent_start );
        } 
    }
}

extern void ezbus_mac_coldboot_signal_token_seen( ezbus_mac_coldboot_t* boot, ezbus_packet_t* packet )
{
    ezbus_peer_t peer;

    ezbus_peer_init( &peer, ezbus_packet_src( packet ), ezbus_packet_seq( packet ) );
    ezbus_mac_peers_clean( ezbus_mac_coldboot_get_peers(boot), ezbus_packet_seq( packet ) );
    ezbus_mac_peers_insort( ezbus_mac_coldboot_get_peers(boot), &peer );

    ezbus_mac_coldboot_set_state( boot, boot_state_silent_start );
}

static void ezbus_mac_coldboot_timer_callback_silent( ezbus_timer_t* timer, void* arg )
{
    ezbus_mac_coldboot_t* boot=(ezbus_mac_coldboot_t*)arg;
    if ( ezbus_timer_expired( timer ) )
    {
        ezbus_mac_coldboot_set_state( boot, boot_state_silent_stop );
    }
}

static void ezbus_mac_coldboot_init_peers( ezbus_mac_coldboot_t* boot )
{
    ezbus_peer_t self_peer;

    ezbus_mac_peers_clear( ezbus_mac_coldboot_get_peers(boot) );
    ezbus_peer_init( &self_peer, &ezbus_self_address, boot->seq );
    ezbus_mac_peers_insort( ezbus_mac_coldboot_get_peers(boot), &self_peer );    
}



extern void ezbus_mac_arbitration_receive_signal_coldboot( ezbus_mac_arbitration_receive_t* arbitration_receive, ezbus_packet_t* rx_packet )
{
    ezbus_log( EZBUS_LOG_COLDBOOT, "%ccoldboot <%s %3d | ", ezbus_mac_coldboot_get_token(mac)?'*':' ', ezbus_address_string( ezbus_packet_src( rx_packet ) ), ezbus_packet_seq( rx_packet ) );
    #if EZBUS_LOG_COLDBOOT
        ezbus_mac_peers_log( &mac->peers );
    #endif
    
    ezbus_mac_coldboot_t* boot = ezbus_mac_arbitration_receive_get_bootstrap( arbitration_receive );
    ezbus_peer_t peer;

    ezbus_peer_init( &peer, ezbus_packet_src( rx_packet ), ezbus_packet_seq( rx_packet ) );
    ezbus_mac_peers_clean( ezbus_mac_coldboot_get_peers( boot ), ezbus_packet_seq( rx_packet ) );
    ezbus_mac_peers_insort( ezbus_mac_coldboot_get_peers( boot ), &peer );

    if ( ezbus_address_compare( &ezbus_self_address, ezbus_packet_src( rx_packet ) ) > 0 )
    {
        if ( ezbus_mac_coldboot_get_state( boot ) == boot_state_coldboot_continue )
        {
            ezbus_timer_stop( &boot->coldboot_timer );
            ezbus_mac_coldboot_set_state( boot, boot_state_silent_start );
        } 
    }

}
