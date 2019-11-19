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
#ifndef EZBUS_DRIVER_ACTIVITY_H_
#define EZBUS_DRIVER_ACTIVITY_H_

#include <ezbus_platform.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef void (*ezbus_activity_callback_t) ( void* );

typedef struct 
{

    ezbus_ms_tick_t             time;
    ezbus_ms_tick_t             inactivity_timeout;

    ezbus_activity_callback_t   activity_callback;
    void*                       activity_arg;

    ezbus_activity_callback_t   inactivity_callback;
    void*                       inactivity_arg;

} ezbus_activity_t;


extern void             ezbus_activity_init         ( ezbus_activity_t* activity );
extern void             ezbus_active_run            ( ezbus_activity_t* activity );

extern void             ezbus_activity_set_callacks ( ezbus_activity_t* activity,
                                                      ezbus_activity_callback_t activity_callback,
                                                      void* activity_arg,
                                                      ezbus_activity_callback_t inactivity_callback,
                                                      void* inactivity_arg );

extern void             ezbus_activity_set          ( ezbus_activity_t* activity );
extern ezbus_ms_tick_t  ezbus_activity_get          ( ezbus_activity_t* activity );
extern ezbus_ms_tick_t  ezbus_inactivity            ( ezbus_activity_t* activity );
extern void             ezbus_inactivity_set_timeout( ezbus_activity_t* activity, ezbus_ms_tick_t inactivity_timeout );
extern ezbus_ms_tick_t  ezbus_inactivity_timeout    ( ezbus_activity_t* activity );

#ifdef __cplusplus
}
#endif

#endif /* EZBUS_DRIVER_ACTIVITY_H_ */
