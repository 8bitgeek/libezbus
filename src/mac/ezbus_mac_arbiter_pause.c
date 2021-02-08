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
#include <ezbus_mac_arbiter_pause.h>
#include <ezbus_mac_arbiter.h>
#include <ezbus_mac_pause.h>
#include <ezbus_mac_transmitter.h>

static void ezbus_mac_arbiter_pause_set_packet              ( ezbus_mac_t* mac, ezbus_packet_t* packet, const ezbus_address_t* address );

static void do_mac_arbiter_state_pause_stopping             ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_pause_stopped              ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_pause_start                ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_pause_wait_send            ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_pause_continue             ( ezbus_mac_t* mac );
static void do_mac_arbiter_state_pause_finish               ( ezbus_mac_t* mac );

static bool ezbus_mac_arbiter_pause_callback_exec           ( ezbus_mac_t* mac );
static bool ezbus_mac_arbiter_pause_callback                ( ezbus_mac_t* mac );
static bool ezbus_mac_arbiter_pause_get_sender              ( ezbus_mac_t* mac );
static void ezbus_mac_arbiter_pause_broadcast_start         ( ezbus_mac_t* mac );

#define ezbus_mac_arbiter_pause_get_sender(mac)             (!ezbus_mac_pause_one_shot( mac ))

extern void ezbus_mac_arbiter_pause_init( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_pause_t* arbiter_pause = ezbus_mac_get_arbiter_pause( mac );
    memset( arbiter_pause, 0, sizeof(ezbus_mac_arbiter_pause_t) ); 
}

extern void ezbus_mac_arbiter_pause_setup( 
                                            ezbus_mac_t*     mac, 
                                            ezbus_ms_tick_t  duration, 
                                            ezbus_ms_tick_t  period, 
                                            ezbus_mac_arbiter_pause_callback_t callback 
                                        )
{
    ezbus_mac_arbiter_pause_set_callback( mac, callback );
    ezbus_mac_pause_setup( 
                            mac, 
                            duration, 
                            period, 
                            ezbus_mac_arbiter_pause_callback 
                        );    
}

static bool ezbus_mac_arbiter_pause_callback_exec( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_pause_t* arbiter = ezbus_mac_get_arbiter_pause( mac );

    if ( ezbus_mac_arbiter_callback( mac ) )
    {
        if ( arbiter->callback )
        {
            return arbiter->callback( mac );
        }
    }
    return false;
}

static bool ezbus_mac_arbiter_pause_callback( ezbus_mac_t* mac )
{
    if ( ezbus_mac_arbiter_get_state( mac ) == mac_arbiter_state_online || ezbus_mac_arbiter_get_state( mac ) == mac_arbiter_state_pause )
    {
        switch( ezbus_mac_pause_get_state( mac ) )
        {
            case ezbus_pause_state_start:
                if ( ezbus_mac_arbiter_pause_get_sender( mac ) )
                {
                        ezbus_mac_arbiter_pause_set_state( mac, mac_arbiter_state_pause_start );
                }
                break;
            case ezbus_pause_state_finish:
                ezbus_mac_arbiter_pause_set_state( mac, mac_arbiter_state_pause_finish );
                break;
            default:
                break;
        }
        return ezbus_mac_arbiter_pause_callback_exec( mac );
    }
    return false;
}

extern bool ezbus_mac_arbiter_pause_ready( ezbus_mac_t* mac )
{
    if ( ezbus_mac_token_acquired( mac ) )
    {
        if ( ezbus_mac_transmitter_empty( mac ) )
        {
            if ( !ezbus_mac_arbiter_transmit_busy( mac ) )
            {
                return true;
            }
        }
    }
    return false;
}

extern void ezbus_mac_arbiter_pause_set_state( ezbus_mac_t* mac, ezbus_mac_arbiter_pause_state_t state )
{
    fprintf( stderr, " %d%s ", state, ezbus_mac_token_acquired( mac )?"*":"" );
    ezbus_mac_arbiter_pause_t* arbiter_pause = ezbus_mac_get_arbiter_pause( mac );
    arbiter_pause->state = state;
}

extern ezbus_mac_arbiter_pause_state_t ezbus_mac_arbiter_pause_get_state( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_pause_t* arbiter_pause = ezbus_mac_get_arbiter_pause( mac );
    return arbiter_pause->state;
}

extern void ezbus_mac_arbiter_pause_start( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_pause_set_state( mac, mac_arbiter_state_pause_start );
}

extern void ezbus_mac_arbiter_pause_stop ( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_pause_set_state( mac, mac_arbiter_state_pause_stopping );
}

static void ezbus_mac_arbiter_pause_set_packet( ezbus_mac_t* mac, ezbus_packet_t* packet, const ezbus_address_t* address )
{
        ezbus_mac_arbiter_t* arbiter = ezbus_mac_get_arbiter( mac );

        ezbus_packet_init           ( packet );
        ezbus_packet_set_type       ( packet, packet_type_pause );
        ezbus_packet_set_dst_socket ( packet, arbiter->rx_ack_src_socket );
        ezbus_packet_set_src_socket ( packet, arbiter->rx_ack_dst_socket );
        ezbus_packet_set_src        ( packet, &ezbus_self_address );
        ezbus_packet_set_dst        ( packet, address );
}


static void ezbus_mac_arbiter_pause_broadcast_start( ezbus_mac_t* mac )
{
    ezbus_packet_t packet;

    ezbus_mac_arbiter_pause_set_packet( mac, &packet, &ezbus_broadcast_address );
    ezbus_pause_set_active            ( ezbus_packet_get_pause( &packet ), true );
    ezbus_pause_set_duration          ( ezbus_packet_get_pause( &packet ), ezbus_mac_arbiter_pause_get_duration( mac ) );
    ezbus_mac_transmitter_put         ( mac, &packet );
}

extern void ezbus_mac_arbiter_pause_set_duration( ezbus_mac_t* mac, ezbus_ms_tick_t duration )
{
    ezbus_mac_pause_set_duration( mac, duration );
}

extern ezbus_ms_tick_t ezbus_mac_arbiter_pause_get_duration( ezbus_mac_t* mac )
{
    return ezbus_mac_pause_get_duration( mac );
}

extern void ezbus_mac_arbiter_pause_set_period( ezbus_mac_t* mac, ezbus_ms_tick_t period )
{
    ezbus_mac_pause_set_period( mac, period );
}

extern ezbus_ms_tick_t  ezbus_mac_arbiter_pause_get_period( ezbus_mac_t* mac )
{
    return ezbus_mac_pause_get_period( mac );
}

extern void ezbus_mac_arbiter_pause_set_callback( ezbus_mac_t* mac, ezbus_mac_arbiter_pause_callback_t callback )
{
    ezbus_mac_arbiter_pause_t* arbiter_pause = ezbus_mac_get_arbiter_pause( mac );
    arbiter_pause->callback = callback;

}


/*****************************************************************************
* State Machine                                                              *
*****************************************************************************/

extern void ezbus_mac_arbiter_pause_run( ezbus_mac_t* mac )
{
    switch( ezbus_mac_arbiter_pause_get_state(mac) )
    {
        case mac_arbiter_state_pause_stopping:  do_mac_arbiter_state_pause_stopping( mac ); break;
        case mac_arbiter_state_pause_stopped:   do_mac_arbiter_state_pause_stopped( mac );  break;
        case mac_arbiter_state_pause_wait_send: do_mac_arbiter_state_pause_wait_send( mac );break;
        case mac_arbiter_state_pause_start:     do_mac_arbiter_state_pause_start( mac );    break;
        case mac_arbiter_state_pause_continue:  do_mac_arbiter_state_pause_continue( mac ); break;
        case mac_arbiter_state_pause_finish:    do_mac_arbiter_state_pause_finish( mac );   break;
    }
}


static void do_mac_arbiter_state_pause_stopping( ezbus_mac_t* mac )
{
    ezbus_mac_arbiter_pause_set_state( mac, mac_arbiter_state_pause_stopped );
}

static void do_mac_arbiter_state_pause_stopped ( ezbus_mac_t* mac )
{
    /* @note do nothing */
}

static void do_mac_arbiter_state_pause_start( ezbus_mac_t* mac )
{
    if( ezbus_mac_arbiter_pause_get_sender( mac ) )
    {
        fputc('>',stderr);
        ezbus_mac_arbiter_pause_broadcast_start( mac );
        ezbus_mac_arbiter_pause_set_state( mac, mac_arbiter_state_pause_wait_send );
    }
    else
    {
        fputc('<',stderr);
        ezbus_mac_arbiter_pause_set_state( mac, mac_arbiter_state_pause_continue );
        ezbus_mac_pause_start( mac );
    }
}

static void do_mac_arbiter_state_pause_wait_send( ezbus_mac_t* mac )
{
    if ( ezbus_mac_transmitter_empty( mac ) )
    {
        ezbus_mac_arbiter_pause_set_state( mac, mac_arbiter_state_pause_continue );
    }    
}

static void do_mac_arbiter_state_pause_continue( ezbus_mac_t* mac )
{
    /* @brief wait... */
}

static void do_mac_arbiter_state_pause_finish( ezbus_mac_t* mac )
{    
    ezbus_mac_arbiter_pause_stop( mac );
}
