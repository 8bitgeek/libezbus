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
#include <ezbus_driver_activity.h>
#include <ezbus_hex.h>

extern void ezbus_activity_init( ezbus_activity_t* activity )
{
    memset( activity, 0, sizeof( ezbus_activity_t ) );
    ezbus_activity_set( activity );
}

extern void ezbus_active_run( ezbus_activity_t* activity )
{
    if ( ezbus_inactivity_timeout( activity ) != 0 && ezbus_inactivity_timeout( activity ) > ezbus_inactivity( activity )  )
    {
        if ( activity->inactivity_callback )
        {
            activity->inactivity_callback( activity->inactivity_arg );   
        }
    }
}


extern void ezbus_activity_set_callacks ( ezbus_activity_t* activity,
                                          ezbus_activity_callback_t activity_callback,
                                          void* activity_arg,
                                          ezbus_activity_callback_t inactivity_callback,
                                          void* inactivity_arg )
{
    activity->activity_callback   = activity_callback;
    activity->activity_arg        = activity_arg;
    activity->inactivity_callback = inactivity_callback;
    activity->inactivity_arg      = inactivity_arg;
}

extern void ezbus_activity_set( ezbus_activity_t* activity )
{
    activity->time = ezbus_platform_get_ms_ticks();
    if ( activity->activity_callback )
    {
        activity->activity_callback( activity->activity_arg );
    }
}

extern ezbus_ms_tick_t ezbus_activity_get( ezbus_activity_t* activity )
{
    return activity->time;
}

extern ezbus_ms_tick_t ezbus_inactivity( ezbus_activity_t* activity )
{
    return ezbus_platform_get_ms_ticks() - activity->time;
}

extern void ezbus_inactivity_set_timeout( ezbus_activity_t* activity, ezbus_ms_tick_t inactivity_timeout )
{
    activity->inactivity_timeout = inactivity_timeout;
}  

extern ezbus_ms_tick_t ezbus_inactivity_timeout( ezbus_activity_t* activity )
{
    return activity->inactivity_timeout;
}

