/*****************************************************************************
* Copyright © 2019-2020 Mike Sharkey <mike.sharkey@mineairquality.com>       *
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
#include <board.h>
#include <chip/chip.h>
#include <ezbus.h>
#include <ezbus_flip.h>
#include <ezbus_port.h>
#include <ezbus_log.h>

static void setup_threads( void );
static void print_banner ( void );
static void ezapp_app_run(ezbus_t* ezbus);
static void ezbus_socket_open_peer ( ezbus_mac_t* mac, ezbus_address_t* peer_address );

#define EZBUS_CARIBOU_USART_NO  CONSOLE_USART 
#define EZBUS_STACK_SZ          (1024*4)

static caribou_thread_t*        ezbus_thread=NULL;
static uint32_t                 ezbus_stack[ EZBUS_STACK_SZ/4 ];
static ezbus_platform_port_t    platform_port;
static ezbus_port_t             port;
static ezbus_t                  ezbus;
static ezbus_address_t          address;
static ezbus_socket_t           socket = EZBUS_SOCKET_INVALID;
static int                      recv_number=0;
static bool                     recvd=false;

void thread_checkin_callback( void );
void thread_timeout_callback( caribou_thread_t* node );

extern bool ezbus_socket_callback_send ( ezbus_socket_t socket )
{
    if ( recvd )
    {
        char number[33];
        recvd=false;
        sprintf( number, "%d", recv_number );
        return ezbus_socket_send( socket, number, ezbus_platform_strlen(number) ) > 0;
    }
    return false;
}

extern bool ezbus_socket_callback_recv ( ezbus_socket_t socket )
{
    char number[33];
    ezbus_platform_memset(number,0,33);
    if ( ezbus_socket_recv( socket, number, 32 ) > 0 )
    {
        recvd=true;
        recv_number = atoi(number);
        fprintf( stderr, "%d %d %d\n", socket, ezbus_socket_get_peer_socket(socket), recv_number );
    }
    return true;
}


static ezbus_address_t* get_a_peer( ezbus_mac_t* mac )
{
    ezbus_address_t* rc = NULL;;
    for( int n=0; rc == NULL && n < ezbus_mac_peers_count( mac ); n++ )
    {
        ezbus_peer_t* peer = ezbus_mac_peers_at( mac, n );
        ezbus_address_t* peer_address = ezbus_peer_get_address( peer );
        if ( !ezbus_address_is_self(peer_address) )
        {
            rc = peer_address;
        }
    }
    return rc;
}

static void ezbus_socket_open_peer ( ezbus_mac_t* mac, ezbus_address_t* peer_address )
{
    if ( socket == EZBUS_SOCKET_INVALID )
    {
        ezbus_address_t* peer_address = get_a_peer(mac);
        socket = ezbus_socket_open( mac, peer_address, EZBUS_SOCKET_ANY );
        ezbus_log( EZBUS_LOG_SOCKET, "peer %s %d\n", ezbus_address_string(peer_address), socket );
    }
}

static void ezapp_app_run(ezbus_t* ezbus)
{
    if ( socket == EZBUS_SOCKET_INVALID )
    {
        ezbus_mac_t* mac = ezbus_mac( ezbus );
        ezbus_address_t* peer_address = get_a_peer( ezbus_mac( ezbus ) );
        if ( peer_address )
        {
            ezbus_socket_open_peer( mac, peer_address );
        }
    }
}

/**
 * @brief Call-back from caribou main thread.
 */
void board_idle()
{
    caribou_gpio_toggle( &gpio_status);
    caribou_thread_yield();
}

void run(void* arg)
{

    char* serial_port_name = (char*)arg;

    ezbus_port_init_struct( &port );

    ezbus_platform_port_set_name(&port,EZBUS_CARIBOU_USART_NO);
    ezbus_platform_port_set_handle(&port,NULL);
    ezbus_platform_port_set_dir_gpio(&port,&gpio_rs485_dir);

    if ( ezbus_port_open( &port, ezbus_port_speeds[EZBUS_SPEED_INDEX_DEF] ) == EZBUS_ERR_OKAY )
    {
        ezbus_init( &ezbus, &port );

        for(;;) /* forever... */
        {
            ezbus_run(&ezbus);
            ezapp_app_run(&ezbus);
        }
    }
}

static void setup_threads( void )
{
    ezbus_thread = caribou_thread_create( "run", run, NULL, NULL, ezbus_stack, EZBUS_STACK_SZ, 1, 0 );
}

#if PRODUCT_WATCHDOG_ENABLED
    void thread_checkin_callback( void )
    {
        chip_watchdog_feed();
    }

    void thread_timeout_callback( caribou_thread_t* node )
    {
        fprintf(stderr,">>> '%s' triggered W/D timeout.\r\n",caribou_thread_name(node));
        fflush(stderr);
        for(;;);
    }
#endif

static void print_banner()
{
    fprintf( stderr, "pclk=%d\n", SystemCoreClock );
    fprintf( stderr, "RCC_CFGR=%08X\n",RCC->CFGR );
    fprintf( stderr, "RCC_PLLCFGR=%08X\n",RCC->PLLCFGR );
    fprintf( stderr, "Heap: block size: %d bytes free: %d\n",heap_block_size(), heap_bytes_free());
    if ( RCC->CSR & RCC_CSR_LPWRRSTF )  fprintf( stderr, "[Low-power] " );
    if ( RCC->CSR & RCC_CSR_WWDGRSTF )  fprintf( stderr, "[Window watchdog] " );
    if ( RCC->CSR & RCC_CSR_IWDGRSTF )  fprintf( stderr, "[Independent watchdog] " );
    if ( RCC->CSR & RCC_CSR_SFTRSTF )   fprintf( stderr, "[Software] " );
    if ( RCC->CSR & RCC_CSR_BORRSTF )   fprintf( stderr, "[Brownout] " );
    if ( RCC->CSR & RCC_CSR_PINRSTF )   fprintf( stderr, "[PIN] " );
    if ( RCC->CSR & RCC_CSR_OBLRSTF )   fprintf( stderr, "[Option byte loader] " );
    if ( RCC->CSR & RCC_CSR_FWRSTF )    fprintf( stderr, "[Firewall] " );
    fprintf( stderr, "\n");
    fflush( stderr );
}

int main(void)
{
    caribou_init(0);

    print_banner();

    #if PRODUCT_WATCHDOG_ENABLED
        caribou_thread_watchdog_init( thread_checkin_callback, thread_timeout_callback );
        chip_watchdog_init( PRODUCT_WD_PERIOD_MS ); 
    #endif

    setup_threads();

    caribou_exec();

    return 0;
}
