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
#include <ezbus_platform.h>
#include <ezbus_fault.h>

extern const char* ezbus_fault_str(EZBUS_ERR err)
{
    switch(err)
    {
        case EZBUS_ERR_OKAY:          return "EZBUS_ERR_OKAY (0) ";                             break; 
        case EZBUS_ERR_PARAM:         return "EZBUS_ERR_PARAM (-1) Bad Parameter";              break;
        case EZBUS_ERR_MALLOC:        return "EZBUS_ERR_MALLOC (-2) Memory Allocation Error";   break;
        case EZBUS_ERR_LIMIT:         return "EZBUS_ERR_LIMIT (-3) Resource Limitation";        break;
        case EZBUS_ERR_RANGE:         return "EZBUS_ERR_RANGE (-4) Out of Range";               break;
        case EZBUS_ERR_TIMEOUT:       return "EZBUS_ERR_TIMEOUT (-5) Timeout Fault";            break;
        case EZBUS_ERR_CRC:           return "EZBUS_ERR_CRC (-6) CRC Fault";                    break;
        case EZBUS_ERR_IO:            return "EZBUS_ERR_IO (-7) I/O Fault";                     break;
        case EZBUS_ERR_NOTREADY:      return "EZBUS_ERR_NOTREADY (-8) Not Ready";               break;
        case EZBUS_ERR_MISMATCH:      return "EZBUS_ERR_MISMATCH (-9) Not a match (address)";   break;
        case EZBUS_ERR_DUP:           return "EZBUS_ERR_DUP (-10) Duplicate";                   break;
        case EZBUS_ERR_OVERFLOW:      return "EZBUS_ERR_OVERFLOW (-11) Overflow occured";       break;
    }
    return "(unknown)";
}
