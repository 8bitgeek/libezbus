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
#include <ezbus_layer1_transmitter.h>
#include <ezbus_hex.h>

static void ezbus_layer1_handle_transmitter_state_empty      ( ezbus_layer1_transmitter_t* layer1_transmitter );
static void ezbus_layer1_handle_transmitter_state_full       ( ezbus_layer1_transmitter_t* layer1_transmitter );
static void ezbus_layer1_handle_transmitter_state_send       ( ezbus_layer1_transmitter_t* layer1_transmitter );


void ezbus_layer1_transmitter_run ( ezbus_layer1_transmitter_t* layer1_transmitter )
{
    switch( ezbus_layer1_transmitter_get_state( layer1_transmitter ) )
    {
        
        case transmitter_state_empty:   
            ezbus_layer1_handle_transmitter_state_empty( layer1_transmitter );
            break;
        
        case transmitter_state_full:
            ezbus_layer1_handle_transmitter_state_full( layer1_transmitter );
            break;
        
        case transmitter_state_send:
            ezbus_layer1_handle_transmitter_state_send( layer1_transmitter );
           break;
        
    }
}




void ezbus_layer1_transmitter_init( ezbus_layer1_transmitter_t* layer1_transmitter, ezbus_port_t* port, ezbus_transmitter_callback_t callback, void* arg )
{
    ezbus_platform_memset(layer1_transmitter,0,sizeof(ezbus_layer1_transmitter_t));
    layer1_transmitter->port     = port;
    layer1_transmitter->callback = callback;
    layer1_transmitter->arg      = arg;
}

void ezbus_layer1_transmitter_put( ezbus_layer1_transmitter_t* layer1_transmitter, ezbus_string_t* string )
{
    /* FIXME */
    ezbus_layer1_transmitter_set_state( layer1_transmitter, transmitter_state_full );
}


static void ezbus_layer1_handle_transmitter_state_empty( ezbus_layer1_transmitter_t* layer1_transmitter )
{
    if ( layer1_transmitter->callback( layer1_transmitter, layer1_transmitter->arg ) )
    {
        ezbus_layer1_transmitter_set_state( layer1_transmitter, transmitter_state_full );
    }
}

static void ezbus_layer1_handle_transmitter_state_full( ezbus_layer1_transmitter_t* layer1_transmitter )
{
    if ( ezbus_layer1_transmitter_get_token( layer1_transmitter ) )
    {
        ezbus_layer1_transmitter_set_state( layer1_transmitter, transmitter_state_send );
    }
    else
    {
        if ( layer1_transmitter->callback( layer1_transmitter, layer1_transmitter->arg ) )
        {
            ezbus_layer1_transmitter_set_state( layer1_transmitter, transmitter_state_send );
        }
    }
}

static void ezbus_layer1_handle_transmitter_state_send( ezbus_layer1_transmitter_t* layer1_transmitter )
{
    if ( layer1_transmitter->callback( layer1_transmitter, layer1_transmitter->arg ) )
    {
        ezbus_layer1_transmitter_set_err( layer1_transmitter, EZBUS_ERR_OKAY );
    }
}
