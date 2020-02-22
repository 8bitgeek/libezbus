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
#ifndef EZBUS_FAULT_H_
#define EZBUS_FAULT_H_

typedef int EZBUS_ERR;

#define EZBUS_ERR_OKAY          0
#define EZBUS_ERR_PARAM         -1                      /* Bad Parameter */
#define EZBUS_ERR_MALLOC        -2                      /* Memory Allocation Error */
#define EZBUS_ERR_LIMIT         -3                      /* Resource Limitation */
#define EZBUS_ERR_RANGE         -4                      /* Out of Range */
#define EZBUS_ERR_TIMEOUT       -5                      /* Timeout Fault */
#define EZBUS_ERR_HEADER_CRC    -6                      /* CRC Fault */
#define EZBUS_ERR_DATA_CRC      -7                      /* CRC Fault */
#define EZBUS_ERR_IO            -8                      /* I/O Fault */
#define EZBUS_ERR_NOTREADY      -9                      /* Not Ready */
#define EZBUS_ERR_MISMATCH      -10                     /* Not a match (address) */
#define EZBUS_ERR_DUP           -11                     /* Duplicate */
#define EZBUS_ERR_OVERFLOW      -12                     /* Overflow occured */

extern const char* ezbus_fault_str(EZBUS_ERR err);


#endif /* EZBUS_FAULT_H_ */
