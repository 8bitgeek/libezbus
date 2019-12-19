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
#include <ezbus_boot.h>
#include <ezbus_peer_list.h>
#include <ezbus_hex.h>
#include <ezbus_log.h>
#include <ezbus_timing.h>


static void ezbus_boot_timer_callback_token            ( ezbus_timer_t* timer, void* arg );
static void ezbus_boot_timer_callback_emit             ( ezbus_timer_t* timer, void* arg );
static void ezbus_boot_timer_callback_warmboot_reply   ( ezbus_timer_t* timer, void* arg );
static void ezbus_boot_timer_callback_warmboot_send    ( ezbus_timer_t* timer, void* arg );

static void ezbus_boot_state_machine_run               ( ezbus_boot_t* boot );
static void ezbus_boot_peer_list_log                   ( ezbus_boot_t* boot );
static void ezbus_boot_init_peer_list                  ( ezbus_boot_t* boot );

static void ezbus_boot_signal_peer_seen_warmboot_src   ( ezbus_boot_t* boot, ezbus_packet_t* packet );
static void ezbus_boot_signal_peer_seen_warmboot_dst   ( ezbus_boot_t* boot, ezbus_packet_t* packet );
static void ezbus_boot_signal_peer_seen_coldboot       ( ezbus_boot_t* boot, ezbus_packet_t* packet );

static void do_boot_state_silent_start                 ( ezbus_boot_t* boot );
static void do_boot_state_silent_continue              ( ezbus_boot_t* boot );
static void do_boot_state_silent_stop                  ( ezbus_boot_t* boot );

static void do_boot_state_coldboot_start               ( ezbus_boot_t* boot );
static void do_boot_state_coldboot_continue            ( ezbus_boot_t* boot );
static void do_boot_state_coldboot_stop                ( ezbus_boot_t* boot );

static void do_boot_state_warmboot_tx_start            ( ezbus_boot_t* boot );
static void do_boot_state_warmboot_tx_continue         ( ezbus_boot_t* boot );
static void do_boot_state_warmboot_tx_stop             ( ezbus_boot_t* boot );

static void do_boot_state_warmboot_rx_start            ( ezbus_boot_t* boot );
static void do_boot_state_warmboot_rx_continue         ( ezbus_boot_t* boot );
static void do_boot_state_warmboot_rx_stop             ( ezbus_boot_t* boot );


extern void ezbus_boot_init(   
                                ezbus_boot_t* boot, 
                                uint32_t baud_rate, 
                                ezbus_peer_list_t* peer_list, 
                                ezbus_boot_callback_t callback, 
                                void* callback_arg 
                            )
{
    boot->baud_rate    = baud_rate;
    boot->peer_list    = peer_list;
    boot->callback     = callback;
    boot->callback_arg = callback_arg;

    ezbus_timer_init( &boot->token_timer );
    ezbus_timer_init( &boot->emit_timer );

    ezbus_timer_set_callback( &boot->warmboot_reply_timer, ezbus_boot_timer_callback_warmboot_reply, boot );
    ezbus_timer_set_callback( &boot->warmboot_send_timer, ezbus_boot_timer_callback_warmboot_send, boot );
    ezbus_timer_set_callback( &boot->token_timer, ezbus_boot_timer_callback_token, boot );
    ezbus_timer_set_callback( &boot->emit_timer,  ezbus_boot_timer_callback_emit, boot );

    ezbus_boot_init_peer_list( boot );
}


extern void ezbus_boot_run( ezbus_boot_t* boot )
{
    ezbus_boot_state_machine_run( boot );
    ezbus_timer_run( &boot->token_timer );
    ezbus_timer_run( &boot->emit_timer );
}


static void ezbus_boot_state_machine_run( ezbus_boot_t* boot )
{
    switch ( ezbus_boot_get_state( boot ) )
    {
        case boot_state_silent_start:         do_boot_state_silent_start         ( boot );  break;
        case boot_state_silent_continue:      do_boot_state_silent_continue      ( boot );  break;
        case boot_state_silent_stop:          do_boot_state_silent_stop          ( boot );  break;

        case boot_state_coldboot_start:       do_boot_state_coldboot_start       ( boot );  break;
        case boot_state_coldboot_continue:    do_boot_state_coldboot_continue    ( boot );  break;
        case boot_state_coldboot_stop:        do_boot_state_coldboot_stop        ( boot );  break;

        case boot_state_warmboot_tx_start:    do_boot_state_warmboot_tx_start    ( boot );  break;
        case boot_state_warmboot_tx_continue: do_boot_state_warmboot_tx_continue ( boot );  break;
        case boot_state_warmboot_tx_stop:     do_boot_state_warmboot_tx_stop     ( boot );  break;

        case boot_state_warmboot_rx_start:    do_boot_state_warmboot_rx_start    ( boot );  break;
        case boot_state_warmboot_rx_continue: do_boot_state_warmboot_rx_continue ( boot );  break;
        case boot_state_warmboot_rx_stop:     do_boot_state_warmboot_rx_stop     ( boot );  break;
    }
}



/**** SILENT BEGIN ****/

static void do_boot_state_silent_start( ezbus_boot_t* boot )
{
    ezbus_boot_set_emit_count( boot, 0 );
    ezbus_timer_stop( &boot->emit_timer );
    ezbus_timer_set_period  ( 
                                &boot->token_timer, 
                                ezbus_timing_ring_time( boot->baud_rate, ezbus_peer_list_count( boot->peer_list ) ) + 
                                    /* ezbus_platform_random( EZBUS_TOKEN_TIMER_MIN, EZBUS_TOKEN_TIMER_MAX ) */
                                    EZBUS_TOKEN_TIMER_MAX
                            );
    ezbus_timer_start( &boot->token_timer );
    boot->callback(boot,boot->callback_arg);
    ezbus_boot_set_state( boot, boot_state_silent_continue );

}

static void do_boot_state_silent_continue( ezbus_boot_t* boot )
{
    /* continue to stay silent until token timeout */
}

static void do_boot_state_silent_stop( ezbus_boot_t* boot )
{
    ezbus_timer_stop( &boot->token_timer );
    ezbus_boot_set_state( boot, boot_state_coldboot_start );
}

/**** SILENT ENDD ****/



/**** COLDBOOT BEGIN ****/

static void do_boot_state_coldboot_start( ezbus_boot_t* boot )
{
    //ezbus_boot_init_peer_list( boot );
    ezbus_timer_stop( &boot->emit_timer );
    ezbus_timer_set_period  ( 
                                &boot->emit_timer, 
                                /* ezbus_timing_ring_time( boot->baud_rate, ezbus_peer_list_count( boot->peer_list ) ) + */
                                    ezbus_platform_random( EZBUS_EMIT_TIMER_MIN, EZBUS_EMIT_TIMER_MAX ) 
                            );
    ezbus_timer_start( &boot->emit_timer );
    ezbus_boot_set_state( boot, boot_state_coldboot_continue );
}

static void do_boot_state_coldboot_continue( ezbus_boot_t* boot )
{
    /* If I'm the "last man standing" then seize control of the bus */
    if ( ezbus_boot_get_emit_count( boot ) > EZBUS_EMIT_CYCLES )
    {
        ezbus_timer_stop( &boot->emit_timer );
        ezbus_boot_set_state( boot, boot_state_warmboot_tx_start );
    }
}

static void do_boot_state_coldboot_stop( ezbus_boot_t* boot )
{
    ezbus_timer_stop( &boot->emit_timer );
    ezbus_timer_stop( &boot->token_timer );
    ezbus_boot_set_state( boot, boot_state_silent_start );
}

/**** COLDBOOT END ****/




/**** WARMBOOT_TX BEGIN ****/

static void do_boot_state_warmboot_tx_start( ezbus_boot_t* boot )
{
    ezbus_boot_set_emit_count( boot, 0 );
    ezbus_timer_stop( &boot->warmboot_send_timer );
    ezbus_timer_set_period  ( 
                                &boot->token_timer, 
                                ezbus_timing_ring_time( boot->baud_rate, ezbus_peer_list_count( boot->peer_list ) ) + 
                                    /* ezbus_platform_random( EZBUS_TOKEN_TIMER_MIN, EZBUS_TOKEN_TIMER_MAX ) */
                                    EZBUS_TOKEN_TIMER_MAX
                            );
    ezbus_timer_start( &boot->token_timer );
    boot->callback(boot,boot->callback_arg);
    ezbus_boot_set_state( boot, boot_state_warmboot_continue );
}

static void do_boot_state_warmboot_tx_continue( ezbus_boot_t* boot )
{
    /* wait for token to timeout */
}

static void do_boot_state_warmboot_tx_stop( ezbus_boot_t* boot )
{
    ezbus_timer_stop( &boot->token_timer );
    boot->callback(boot,boot->callback_arg);
    ezbus_boot_set_state( boot, boot_state_silent_start );
}

/**** WARMBOOT_TX END ****/



/**** WARMBOOT_RX  BEGIN ****/

static void do_boot_state_warmboot_rx_start( ezbus_boot_t* boot )
{
    ezbus_timer_stop( &boot->warmboot_reply_timer );
    ezbus_timer_set_period  ( 
                                &boot->warmboot_reply_timer,  
                                ezbus_platform_random( EZBUS_WARMBOOT_TIMER_MIN, EZBUS_WARMBOOT_TIMER_MIN )
                            );
    ezbus_timer_start( &boot->warmboot );
    ezbus_boot_set_state( boot, boot_state_warmboot_rx_continue );
}

static void do_boot_state_warmboot_rx_continue( ezbus_boot_t* boot )
{
    /* do nothing? */
}

static void do_boot_state_warmboot_rx_stop( ezbus_boot_t* boot )
{
    ezbus_timer_stop( &boot->warmboot_reply_timer );
    ezbus_boot_set_state( boot, boot_state_coldboot_start );
}

/* WARMBOOT_RX END */



/**
 * @brief The source address is the warmboot designate. Meaning we should reply to warmboot adderss.
 */
static void ezbus_boot_signal_peer_seen_warmboot_src( ezbus_boot_t* boot, ezbus_packet_t* packet )
{
}

/**
 * @brief The destination address is the warmboot designate. Meaning we should sniff the source address as a peer..
 */
static void ezbus_boot_signal_peer_seen_warmboot_dst( ezbus_boot_t* boot, ezbus_packet_t* packet )
{
    ezbus_peer_t peer;
    ezbus_peer_init( &peer, ezbus_packet_src( packet ), 0 );
    ezbus_peer_list_insort( boot->peer_list, &peer );
    ezbus_log( EZBUS_LOG_WARMBOOT, "warmboot peer< %s < ", ezbus_address_string( ezbus_packet_src( packet ) ) );
}

/**
 * @brief During cold boot, we've seen a peer source address, so rercord it, and go dormant if 
 *        the address is more dominant than self.
 */
static void ezbus_boot_signal_peer_seen_coldboot( ezbus_boot_t* boot, ezbus_packet_t* packet )
{
    ezbus_peer_t peer;
    ezbus_address_t address;

    ezbus_address_copy( &address, ezbus_packet_src( packet ) );
    ezbus_peer_init( &peer, &address, 0 );
    ezbus_peer_list_insort( boot->peer_list, &peer );
    ezbus_boot_peer_list_log( boot );

    if ( ezbus_address_compare( &ezbus_self_address, &address ) > 0 )
    {
        ezbus_log( EZBUS_LOG_COLDBOOT, "coldboot peer< %s < ", ezbus_address_string( &address ) );
        ezbus_log( EZBUS_LOG_COLDBOOT, "%s \n", ezbus_address_string( &ezbus_self_address ) );

        if ( ezbus_boot_get_state( boot ) == boot_state_coldboot_continue )
        {
            ezbus_timer_stop( &boot->emit_timer );
            ezbus_boot_set_state( boot, boot_state_silent_start );
        } 
    }
}

extern void ezbus_boot_signal_peer_seen( ezbus_boot_t* boot, ezbus_packet_t* packet )
{
    if ( ezbus_address_compare( &ezbus_warmboot_address, ezbus_packet_src( packet ) ) == 0 )
    {
        ezbus_boot_signal_peer_seen_warmboot_src( boot, packet );
    }
    else
    if ( ezbus_address_compare( &ezbus_warmboot_address, ezbus_packet_dst( packet ) ) == 0 )
    {
        ezbus_boot_signal_peer_seen_warmboot_dst( boot, packet );
    }
    else
    {
        ezbus_boot_signal_peer_seen_coldboot( boot, packet );
    }
}

extern void ezbus_boot_signal_token_seen( ezbus_boot_t* boot, ezbus_packet_t* packet )
{
    ezbus_peer_t    peer;

    ezbus_peer_init( &peer, ezbus_packet_src( packet ), 0 );
    ezbus_peer_list_insort( boot->peer_list, &peer );

    ezbus_log( EZBUS_LOG_TOKEN, "tok< %s\n", ezbus_address_string( ezbus_packet_src( packet ) ) );
    ezbus_boot_peer_list_log( boot )
;
    ezbus_boot_set_state( boot, boot_state_silent_start );
}

static void ezbus_boot_timer_callback_token( ezbus_timer_t* timer, void* arg )
{
    ezbus_boot_t* boot=(ezbus_boot_t*)arg;
    if ( ezbus_timer_expired( timer ) )
    {
        switch( ezbus_boot_get_state( boot ) )
        {
            case boot_state_silent_start:
                break;
            case boot_state_silent_continue:
                ezbus_boot_set_state( boot, boot_state_silent_stop );
                break;
            case boot_state_silent_stop:
                break;
            case boot_state_coldboot_start:
                break;
            case boot_state_coldboot_continue:
                break;
            case boot_state_coldboot_stop:
                break;
            case boot_state_warmboot_start:
                break;
            case boot_state_warmboot_continue:
                ezbus_boot_set_state( boot, boot_state_warmboot_stop );
                break;
            case boot_state_warmboot_stop:
                break;

        }
    }
}

static void ezbus_boot_peer_list_log( ezbus_boot_t* boot )
{
    for(int index=0; index < ezbus_peer_list_count(boot->peer_list); index++)
    {
        ezbus_peer_t* peer = ezbus_peer_list_at(boot->peer_list,index);
        ezbus_log( EZBUS_LOG_PEERS, "%s, ", ezbus_address_string( ezbus_peer_get_address( peer ) ) );
    }
    ezbus_log( EZBUS_LOG_PEERS, "\n" );
}

static void ezbus_boot_init_peer_list( ezbus_boot_t* boot )
{
    ezbus_peer_t    self_peer;

    ezbus_peer_list_clear( boot->peer_list );
    ezbus_peer_init( &self_peer, &ezbus_self_address, 0 );
    ezbus_peer_list_insort( boot->peer_list, &self_peer );    
}

static void ezbus_boot_timer_callback_emit( ezbus_timer_t* timer, void* arg )
{
    ezbus_boot_t* boot=(ezbus_boot_t*)arg;
    if ( ezbus_timer_expired( timer ) )
    {
        ezbus_boot_inc_emit_count( boot );
        ezbus_log( EZBUS_LOG_COLDDBOOT, "coldboot> %s %d\n", ezbus_address_string( &ezbus_self_address ), ezbus_boot_get_emit_count( boot ) );
        boot->callback(boot,boot->callback_arg);
        ezbus_boot_set_state( boot, boot_state_coldboot_start );
    }
}

static void ezbus_boot_timer_callback_warmboot_reply( ezbus_timer_t* timer, void* arg )
{
    boot->callback(boot,boot->callback_arg);
    ezbus_boot_set_state( boot, boot_state_warmboot_rx_stop );
}

static void ezbus_boot_timer_callback_warmboot_send( ezbus_timer_t* timer, void* arg )
{
    boot->callback(boot,boot->callback_arg);
    if ( warm_boot_send_count > send_limit )
        ezbus_boot_set_state( boot, boot_state_warmboot_tx_stop );
}
