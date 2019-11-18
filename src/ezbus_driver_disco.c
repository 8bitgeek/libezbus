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
#include <ezbus_driver_disco.h>

static void             ezbus_driver_tx_packet      ( ezbus_driver_t* driver );
static ezbus_peer_t*    ezbus_driver_rx_src_peer    ( ezbus_driver_t* driver, uint8_t seq);

extern void ezbus_driver_disco( ezbus_driver_t* driver, uint32_t cycles, ezbus_progress_callback_t progress_callback )
{
    #if DISCO_PEER_LIST_DEINIT
        ezbus_peer_list_deinit( &driver->disco.peers );
    #endif
    
    int cycle_count         = cycles;
    int peer_count          = ezbus_peer_list_count( &driver->disco.peers );
    driver->io.tx_state.err = EZBUS_ERR_OKAY;
    driver->disco.start     = ezbus_platform_get_ms_ticks();

    do
    {
        ezbus_driver_tx_disco_rq( driver, &ezbus_broadcast_address );
        if ( ezbus_peer_list_count( &driver->disco.peers ) == peer_count )
        {
            --cycle_count;
        }
        else
        {
            cycle_count = cycles;
            peer_count = ezbus_peer_list_count( &driver->disco.peers );
        }

        if ( progress_callback != NULL )
        {
            if ( !progress_callback( &driver->disco ) )
            {
                break;
            }
        }

    } while ( cycle_count > 0 );

    if ( ezbus_peer_list_count( &driver->disco.peers ) > 0 )
    {
        ezbus_address_copy( &driver->io.next_address, ezbus_peer_list_next( &driver->disco.peers, &driver->io.address ) );
        ezbus_address_dump( &driver->io.next_address, "next" );
    }
}

static void ezbus_driver_tx_packet( ezbus_driver_t* driver )
{
    driver->io.tx_state.err = ezbus_port_send( &driver->io.port, &driver->io.tx_state.packet );
}

static ezbus_peer_t* ezbus_driver_rx_src_peer( ezbus_driver_t* driver, uint8_t seq)
{
    ezbus_peer_t peer;
    ezbus_peer_t* peer_p=NULL;

    if ( ( peer_p = ezbus_peer_list_lookup( &driver->disco.peers, &driver->io.rx_state.packet.header.data.field.src ) ) == NULL )
    {
        ezbus_peer_init( &peer, &driver->io.rx_state.packet.header.data.field.src, seq );
        peer_p = ezbus_peer_list_insort( &driver->disco.peers, &peer );
    }

    return peer_p;
}

/*****************************************************************************
 ***************************** TRANSMITTERS **********************************
 ****************************************************************************/

extern void ezbus_driver_tx_disco_rq( ezbus_driver_t* driver, const ezbus_address_t* dst )
{
    ezbus_packet_t* tx_packet = &driver->io.tx_state.packet;
    
    ezbus_packet_set_type( tx_packet, packet_type_disco_rq);
    ezbus_address_copy( ezbus_packet_src( tx_packet ), &driver->io.address );
    ezbus_address_copy( ezbus_packet_dst( tx_packet ), dst );

    tx_packet->data.attachment.disco.features    = DISCO_FEATURES;
    tx_packet->data.attachment.disco.request_seq = ++driver->disco.seq;
    tx_packet->data.attachment.disco.reply_seq   = 0;

    ezbus_driver_tx_packet( driver );
    ezbus_driver_tx_disco_wait( driver );
}

extern void ezbus_driver_tx_disco_rp( ezbus_driver_t* driver, const ezbus_address_t* dst )
{
    ezbus_packet_t* tx_packet = &driver->io.tx_state.packet;
    ezbus_packet_t* rx_packet = &driver->io.rx_state.packet;

    ezbus_packet_set_type( tx_packet, packet_type_disco_rp );
    ezbus_address_copy( ezbus_packet_src( tx_packet ), &driver->io.address );
    ezbus_address_copy( ezbus_packet_dst( tx_packet ), dst );

    tx_packet->data.attachment.disco.features    = DISCO_FEATURES;
    tx_packet->data.attachment.disco.request_seq = rx_packet->data.attachment.disco.request_seq;
    tx_packet->data.attachment.disco.reply_seq   = driver->disco.seq;

    ezbus_driver_tx_packet( driver );
}

extern void ezbus_driver_tx_disco_rk( ezbus_driver_t* driver, const ezbus_address_t* dst )
{
    ezbus_packet_t* tx_packet = &driver->io.tx_state.packet;
    ezbus_packet_t* rx_packet = &driver->io.rx_state.packet;
    ezbus_packet_t  tx_packet_save;

    ezbus_packet_copy( &tx_packet_save, tx_packet );
    ezbus_packet_set_type( tx_packet, packet_type_disco_rk );
    ezbus_address_copy( ezbus_packet_src( tx_packet ), &driver->io.address );
    ezbus_address_copy( ezbus_packet_dst( tx_packet ), dst );

    tx_packet->data.attachment.disco.features    = DISCO_FEATURES;
    tx_packet->data.attachment.disco.request_seq = rx_packet->data.attachment.disco.request_seq;
    tx_packet->data.attachment.disco.reply_seq   = 0;

    ezbus_driver_tx_packet( driver );
    ezbus_packet_copy( tx_packet, &tx_packet_save );
}

extern void ezbus_driver_tx_disco_wait( ezbus_driver_t* driver )
{
    ezbus_ms_tick_t start = ezbus_platform_get_ms_ticks();
    while ( ezbus_platform_get_ms_ticks() - start < EZBUS_DISCO_PERIOD )
        ezbus_driver_run( driver );
}

/*****************************************************************************
 *****************************  RECEIVERS ************************************
 ****************************************************************************/

extern void ezbus_driver_rx_disco_rq( ezbus_driver_t* driver )
{
    ezbus_peer_t* peer_p=NULL;

    if ( (peer_p=ezbus_driver_rx_src_peer(driver,0)) && ezbus_peer_get_seq( peer_p ) != driver->io.rx_state.packet.header.data.field.seq )
    {
        ezbus_platform_delay( ezbus_platform_random( EZBUS_RAND_LOWER, EZBUS_RAND_UPPER ) );
        ezbus_driver_tx_disco_rp( driver, ezbus_packet_src( &driver->io.rx_state.packet ) );
        
        #if EZBUS_DRIVER_DEBUG
            ezbus_driver_dump( driver );
        #endif
    }
}

extern void ezbus_driver_rx_disco_rp( ezbus_driver_t* driver )
{
    ezbus_peer_t* peer_p=NULL;

    #if EZBUS_DRIVER_DEBUG
        fprintf(stderr,"packet_code_rp\n");
    #endif

    if ( (peer_p = ezbus_driver_rx_src_peer( driver, ezbus_packet_seq( &driver->io.rx_state.packet ) )) != NULL )
    {
        ezbus_driver_tx_disco_rk( driver, ezbus_peer_get_address(peer_p) );
    }

    #if EZBUS_DRIVER_DEBUG
        ezbus_driver_dump( driver );
    #endif
}


extern void ezbus_driver_rx_disco_rk( ezbus_driver_t* driver )
{
    ezbus_peer_t* peer_p=NULL;

    #if EZBUS_DRIVER_DEBUG
        fprintf(stderr,"packet_code_rk\n");
    #endif

    if ( (peer_p = ezbus_driver_rx_src_peer( driver, ezbus_packet_seq( &driver->io.rx_state.packet ) )) != NULL )
    {
        ezbus_peer_set_seq( peer_p, ezbus_packet_seq( &driver->io.rx_state.packet ) );
    }
}

