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
#include <caribou/lib/fault.h>
#include <chip/chip.h>

typedef struct
{
    volatile unsigned long stacked_r0 ;
    volatile unsigned long stacked_r1 ;
    volatile unsigned long stacked_r2 ;
    volatile unsigned long stacked_r3 ;
    volatile unsigned long stacked_r12 ;
    volatile unsigned long stacked_lr ;
    volatile unsigned long stacked_pc ;
    volatile unsigned long stacked_psr ;
    volatile unsigned long _CFSR ;
    volatile unsigned long _HFSR ;
    volatile unsigned long _DFSR ;
    volatile unsigned long _AFSR ;
    volatile unsigned long _BFAR ;
    volatile unsigned long _MMAR ;

} fault_state_t;

volatile fault_state_t fault_state;

/**
 * fault:
 * This is called from the _fault() entry point with a pointer to the Fault stack
 * as the parameter. We can then read the values from the stack and place them
 * into local variables for ease of reading.
 * We then read the various Fault Status and Address Registers to help decode
 * cause of the fault.
 * The function ends with a BKPT instruction to force control back into the debugger
 */
extern void fault(unsigned long *hardfault_args)
{
        fault_state.stacked_r0 = ((unsigned long)hardfault_args[0]) ;
        fault_state.stacked_r1 = ((unsigned long)hardfault_args[1]) ;
        fault_state.stacked_r2 = ((unsigned long)hardfault_args[2]) ;
        fault_state.stacked_r3 = ((unsigned long)hardfault_args[3]) ;
        fault_state.stacked_r12 = ((unsigned long)hardfault_args[4]) ;
        fault_state.stacked_lr = ((unsigned long)hardfault_args[5]) ;
        fault_state.stacked_pc = ((unsigned long)hardfault_args[6]) ;
        fault_state.stacked_psr = ((unsigned long)hardfault_args[7]) ;

        // Configurable Fault Status Register
        // Consists of MMSR, BFSR and UFSR
        fault_state._CFSR = (*((volatile unsigned long *)(0xE000ED28))) ;

        // Hard Fault Status Register
        fault_state._HFSR = (*((volatile unsigned long *)(0xE000ED2C))) ;

        // Debug Fault Status Register
        fault_state._DFSR = (*((volatile unsigned long *)(0xE000ED30))) ;

        // Auxiliary Fault Status Register
        fault_state._AFSR = (*((volatile unsigned long *)(0xE000ED3C))) ;

        // Read the Fault Address Registers. These may not contain valid values.
        // Check BFARVALID/MMARVALID to see if they are valid values
        // MemManage Fault Address Register
        fault_state._MMAR = (*((volatile unsigned long *)(0xE000ED34))) ;
        // Bus Fault Address Register
        fault_state._BFAR = (*((volatile unsigned long *)(0xE000ED38))) ;

        #if 1
            chip_reset();
        #else
            __asm(" bkpt #0\n") ; // Break into the debugger
        #endif
}

/**
 * _fault:
 * Hard Fault handler to help debug the reason for a fault.
 * To use, edit the vector table to reference this function in the HardFault vector
 * This code is suitable for Cortex-M3 and Cortex-M0 cores
 */

extern __attribute__((naked)) void _fault(void)
{
    /*
     * Get the appropriate stack pointer, depending on our mode,
     * and use it as the parameter to the C handler. This function
     * will never return
     */

#if 1
    __asm(  ".syntax unified\n"
                    " movs   r0, #4  \n"
                    " mov   r1, lr  \n"
                    " tst   r0, r1  \n"
                    " beq   _msp    \n"
                    " mrs   r0, psp \n"
                    " b   fault   \n"
            "_msp:  \n"
                    " mrs   r0, msp \n"
                    " b   fault   \n"
                    " b   _fault     \n"
            ".syntax divided\n") ;
#else
            chip_reset();
#endif
}

