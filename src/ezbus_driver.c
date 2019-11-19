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
#include <ezbus_driver.h>
#include <ezbus_driver_disco.h>
#include <ezbus_hex.h>

static void             ezbus_driver_tx_reset        ( ezbus_driver_t* driver );
static void             ezbus_driver_tx_parcel       ( ezbus_driver_t* driver, const ezbus_address_t* dst );
static void             ezbus_driver_tx_enqueue      ( ezbus_driver_t* driver );
static void             ezbus_driver_tx_speed        ( ezbus_driver_t* driver );
static void             ezbus_driver_tx_give_token   ( ezbus_driver_t* driver, const ezbus_address_t* dst );
static void             ezbus_driver_tx_take_token   ( ezbus_driver_t* driver, const ezbus_address_t* dst );
static void             ezbus_driver_tx_ack          ( ezbus_driver_t* driver, const ezbus_address_t* dst );

static void             ezbus_driver_rx              ( ezbus_driver_t* driver );
static bool             ezbus_driver_rx_receivable   ( ezbus_driver_t* driver );
static void             ezbus_driver_rx_give_token   ( ezbus_driver_t* driver );
static void             ezbus_driver_rx_take_token   ( ezbus_driver_t* driver );
static void             ezbus_driver_rx_ack          ( ezbus_driver_t* driver );
static void             ezbus_driver_rx_nack         ( ezbus_driver_t* driver );
static void             ezbus_driver_rx_parcel       ( ezbus_driver_t* driver );
static void             ezbus_driver_rx_reset        ( ezbus_driver_t* driver );
static void             ezbus_driver_rx_speed        ( ezbus_driver_t* driver );
static void             ezbus_driver_init_struct     ( ezbus_driver_t* driver );

static void             ezbus_driver_rx_flush        ( ezbus_driver_t* driver );
static void             ezbus_driver_tx_flush        ( ezbus_driver_t* driver );
static void             ezbus_driver_tx_callback     ( ezbus_driver_t* driver, ezbus_tx_state_t tx_state );

static void             ezbus_driver_inactivity      ( void* arg );

extern EZBUS_ERR ezbus_driver_init( ezbus_driver_t* driver, ezbus_platform_port_t* platform_port, uint32_t speed, uint8_t tx_retry )
{
    EZBUS_ERR err;
    
    ezbus_platform_rand_init();
    
    ezbus_driver_init_struct( driver );
    
    ezbus_platform_address( &driver->io.address );
    
    driver->io.tx_retry = tx_retry;
    
    err = ezbus_port_open( &driver->io.port, platform_port, speed );

    ezbus_token_init                ( &driver->io.token );
    ezbus_token_calc_timeout_period ( &driver->io.token, sizeof(ezbus_packet_t), 1, speed );

    ezbus_activity_init          ( &driver->activity );
    ezbus_activity_set_callacks  ( &driver->activity, NULL, NULL, ezbus_driver_inactivity, driver );
    ezbus_inactivity_set_timeout ( &driver->activity, ezbus_token_timeout_period( &driver->io.token )*2 );

    return err;
}

extern void ezbus_driver_deinit( ezbus_driver_t* driver )
{
    ezbus_port_close( &driver->io.port );
    ezbus_platform_memset( driver, 0, sizeof( ezbus_driver_t ) );
    driver->io.rx_state.err = EZBUS_ERR_OKAY;
    driver->io.tx_state.err = EZBUS_ERR_OKAY;
}

/**
 * @brief The main driver of ezbus flow. Call often.
 * @param driver An initialized ezbus driver struct.
 * @return void
 */
extern void ezbus_driver_run( ezbus_driver_t* driver )
{
    ezbus_driver_rx_flush( driver );
    ezbus_driver_tx_flush( driver );

    if ( ezbus_token_present( &driver->io.token ) )
    {
        ezbus_driver_tx_give_token( driver, ezbus_peer_list_next( &driver->disco.peers, &driver->io.address ) );
    }

    ezbus_active_run( &driver->activity );
}

static void ezbus_driver_rx_flush( ezbus_driver_t* driver )
{
    driver->io.rx_state.err = ezbus_port_recv( &driver->io.port, &driver->io.rx_state.packet );
    switch( driver->io.rx_state.err )
    {
        case EZBUS_ERR_NOTREADY:
            break;
        case EZBUS_ERR_TIMEOUT:
            #if EZBUS_DRIVER_DEBUG
                fprintf( stderr, "EZBUS_ERR_TIMEOUT\n" );
            #endif
            ++driver->io.port.rx_err_timeout_count;
            break;
        case EZBUS_ERR_CRC:
            #if EZBUS_DRIVER_DEBUG
                fprintf( stderr, "EZBUS_ERR_CRC\n" );
            #endif
            ++driver->io.port.rx_err_crc_count;
            break;
        case EZBUS_ERR_OKAY:
            #if EZBUS_DRIVER_DEBUG
                fprintf( stderr, "EZBUS_ERR_OKAY\n" );
                ezbus_hex_dump( "RX:", (uint8_t*)&driver->io.rx_state.packet.header, sizeof(ezbus_header_t) );
            #endif
            {
                if ( driver->rx_callback != NULL )
                {
                    driver->rx_callback( &driver->io );
                }
                ezbus_driver_rx( driver );
            }
            break;
    }
}



static void ezbus_driver_tx_flush( ezbus_driver_t* driver )
{
    if ( ezbus_token_present( &driver->io.token ) && ezbus_driver_tx_full( driver ) )
    {
        ezbus_packet_state_t* tx_state = &driver->io.tx_state;

        ezbus_driver_low_level_send( driver );
        
        tx_state->flags |= EXBUS_PACKET_WAIT_ACK;
        tx_state->retry = driver->io.tx_retry;

        ezbus_driver_tx_callback( driver, ezbus_tx_state_busy );
    }
}

extern void ezbus_driver_low_level_send( ezbus_driver_t* driver )
{
    ezbus_packet_state_t* tx_state = &driver->io.tx_state;
    tx_state->err = ezbus_port_send( &driver->io.port, &tx_state->packet );
    ezbus_activity_set( &driver->activity );
}

static void ezbus_driver_tx_callback( ezbus_driver_t* driver, ezbus_tx_state_t tx_state )
{
    if ( driver->tx_callback )
    {
        driver->tx_callback( &driver->io );
    }
}



static void ezbus_driver_init_struct( ezbus_driver_t* driver )
{
    ezbus_platform_memset( driver, 0, sizeof( ezbus_driver_t ) );
    driver->io.rx_state.err = EZBUS_ERR_OKAY;
    driver->io.tx_state.err = EZBUS_ERR_OKAY;
    driver->disco.seq=0xFF;
}

extern void ezbus_driver_set_rx_cb( ezbus_driver_t* driver, ezbus_rx_callback_t rx_callback )
{
    driver->rx_callback = rx_callback;
}

extern void ezbus_driver_set_tx_cb( ezbus_driver_t* driver, ezbus_tx_callback_t tx_callback )
{
    driver->tx_callback = tx_callback;
}



static void ezbus_driver_inactivity( void* arg )
{
    ezbus_driver_t* driver = (ezbus_driver_t*)arg;
    if ( ezbus_peer_list_count( &driver->disco.peers ) == 0 )
    {
        ezbus_driver_disco( driver, EZBUS_DISCO_COUNT, NULL );
    }
    else
    {
        /** FIXME ?? - this will only work for first node ?? */
        if ( ezbus_peer_list_index_of( &driver->disco.peers, &driver->io.address ) == 0 )
        {
            ezbus_driver_tx_take_token( driver, &ezbus_broadcast_address );
        }
    }
}


/****************************************************************************
 ****************************** Consumer Transmit ***************************
 ****************************************************************************/

static void ezbus_driver_tx_enqueue( ezbus_driver_t* driver )
{
    ezbus_packet_state_t* tx_state = &driver->io.tx_state;

    tx_state->flags = 0;
    tx_state->flags |= EZBUS_PACKET_FULL;
    tx_state->retry = driver->io.tx_retry;

    ezbus_driver_tx_callback( driver, ezbus_tx_state_full );
}

extern bool ezbus_driver_tx_empty( ezbus_driver_t* driver )
{
    return !ezbus_driver_tx_full( driver );
}

extern bool ezbus_driver_tx_full( ezbus_driver_t* driver )
{
    ezbus_packet_state_t* tx_state = &driver->io.tx_state;

    return ( tx_state->flags & EZBUS_PACKET_FULL );
}

extern bool ezbus_driver_tx_wait_tok( ezbus_driver_t* driver )
{
    return ( ezbus_driver_tx_full( driver ) && !ezbus_driver_tx_wait_ack( driver ) );
}

extern bool ezbus_driver_tx_wait_ack( ezbus_driver_t* driver )
{
    ezbus_packet_state_t* tx_state = &driver->io.tx_state;

    return ( tx_state->flags & EXBUS_PACKET_WAIT_ACK );
}

extern bool ezbus_driver_tx_put( ezbus_driver_t* driver, void* buf, uint8_t size, ezbus_address_t* dst )
{
    if ( ezbus_driver_tx_empty( driver ) )
    {
        ezbus_packet_t* tx_packet = &driver->io.tx_state.packet;

        ezbus_packet_set_type( tx_packet, packet_type_parcel );
        ezbus_address_copy( ezbus_packet_src( tx_packet ), &driver->io.address );
        ezbus_address_copy( ezbus_packet_dst( tx_packet ), dst );

        ezbus_platform_memcpy( tx_packet->data.attachment.parcel.bytes, buf, size );
        tx_packet->data.attachment.parcel.size = size;
 
        ezbus_driver_tx_enqueue( driver );
        
        return true;
    }
    return false;
}




static void ezbus_driver_tx_give_token( ezbus_driver_t* driver, const ezbus_address_t* dst )
{
    ezbus_packet_t* tx_packet = &driver->io.tx_state.packet;

    ezbus_packet_set_type( tx_packet, packet_type_give_token );
    ezbus_address_copy( ezbus_packet_src( tx_packet ), &driver->io.address );
    ezbus_address_copy( ezbus_packet_dst( tx_packet ), dst );

    ezbus_driver_low_level_send( driver );
}

static void ezbus_driver_tx_take_token( ezbus_driver_t* driver, const ezbus_address_t* dst )
{
    ezbus_packet_t* tx_packet = &driver->io.tx_state.packet;

    ezbus_packet_set_type( tx_packet, packet_type_give_token );
    ezbus_address_copy( ezbus_packet_src( tx_packet ), &driver->io.address );
    ezbus_address_copy( ezbus_packet_dst( tx_packet ), dst );

    ezbus_driver_low_level_send( driver );
}

static void ezbus_driver_tx_ack( ezbus_driver_t* driver, const ezbus_address_t* dst )
{
}

static void ezbus_driver_tx_parcel( ezbus_driver_t* driver, const ezbus_address_t* dst )
{
}

static void ezbus_driver_tx_reset( ezbus_driver_t* driver )
{
}

static void ezbus_driver_tx_speed( ezbus_driver_t* driver )
{
}

/*****************************************************************************
 *****************************  RECEIVERS ************************************
 ****************************************************************************/


static void ezbus_driver_rx_give_token( ezbus_driver_t* driver )
{
    ezbus_token_set_present( &driver->io.token, true );
}

static void ezbus_driver_rx_take_token( ezbus_driver_t* driver )
{
     ezbus_token_set_present( &driver->io.token, false );
}

static void ezbus_driver_rx_ack( ezbus_driver_t* driver )
{
    /* *NOTE do somemthing here. */
}

static void ezbus_driver_rx_nack( ezbus_driver_t* driver )
{
}

static void ezbus_driver_rx_parcel( ezbus_driver_t* driver )
{
}

static void ezbus_driver_rx_reset( ezbus_driver_t* driver )
{
}

static void ezbus_driver_rx_speed( ezbus_driver_t* driver )
{
}

static bool ezbus_driver_rx_receivable ( ezbus_driver_t* driver )
{
    /* rx packet is either addressed to this node, or is a broadcast */
    return ( ezbus_address_compare( ezbus_packet_dst( &driver->io.rx_state.packet ), &driver->io.address ) == 0 ||
             ezbus_address_compare( ezbus_packet_dst( &driver->io.rx_state.packet ), &ezbus_broadcast_address ) == 0 );
}


static void ezbus_driver_rx( ezbus_driver_t* driver )
{
    ezbus_activity_set( &driver->activity );
    /* A valid rx_packet is present? */
    if ( driver->io.rx_state.err == EZBUS_ERR_OKAY )
    {
        if ( ezbus_driver_rx_receivable(driver ) )
        {
            switch( ezbus_packet_type( &driver->io.rx_state.packet ) )
            {
                case packet_type_reset      :   ezbus_driver_rx_reset      ( driver );      break;
                case packet_type_disco_rq   :   ezbus_driver_rx_disco_rq   ( driver );      break;
                case packet_type_disco_rp   :   ezbus_driver_rx_disco_rp   ( driver );      break;
                case packet_type_disco_rk   :   ezbus_driver_rx_disco_rk   ( driver );      break;
                case packet_type_take_token :   ezbus_driver_rx_take_token ( driver );      break;
                case packet_type_give_token :   ezbus_driver_rx_give_token ( driver );      break;
                case packet_type_parcel     :   ezbus_driver_rx_parcel     ( driver );      break;
                case packet_type_speed      :   ezbus_driver_rx_speed      ( driver );      break;
                case packet_type_ack        :   ezbus_driver_rx_ack        ( driver );      break;
                case packet_type_nack       :   ezbus_driver_rx_nack       ( driver );      break;
            }
        }
        else
        {
            /* It's not addressed to this peer. */
            driver->io.rx_state.err = EZBUS_ERR_MISMATCH;
        }
    }
}

static void ezbus_packet_state_dump( ezbus_packet_state_t* packet_state, const char* prefix )
{
    char print_buffer[EZBUS_TMP_BUF_SZ];
    
    sprintf( print_buffer, "%s.packet", prefix );
    ezbus_packet_dump ( &packet_state->packet, print_buffer );

    fprintf(stderr, "%s.err=%d\n", prefix, packet_state->err );
    fprintf(stderr, "%s.seq=%d\n", prefix, packet_state->seq );
}

extern void ezbus_driver_dump( ezbus_driver_t* driver )
{
    ezbus_address_dump     ( &driver->io.address,   "driver->io.address" );
    ezbus_port_dump        ( &driver->io.port,      "driver->io.port" );
    ezbus_packet_state_dump( &driver->io.rx_state,  "driver->io.rx_state" );
    ezbus_packet_state_dump( &driver->io.tx_state,  "driver->io.tx_state" );
    fprintf(stderr, "driver->disco.seq=%d\n", driver->disco.seq );
    ezbus_peer_list_dump( &driver->disco.peers,     "driver->disco.peers" );
    fflush(stderr);
}
