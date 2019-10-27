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
		.extern	reset_handler			/* Reset Vector */
		.extern _pendsv
		.extern _systick
		.extern _fault
		.extern	_swi
		.extern _nmi

		.global _vectors
		.section .vectors, "ax"
		.code 16
		.align 4
		.thumb_func

_vectors:
		.word __main_stack_end__	/* Reserved */
		.word reset_handler			/* Reset Vector */
		.word _nmi					/* Nmi_ISR */
		.word _fault				/* Hard Fault_ISR */
		.word _fault				/* Mem Manage Fault_ISR */
		.word _fault				/* Bus Fault_ISR */
		.word _fault				/* Usage Fault_ISR */
		.word _fault
		.word _fault
		.word _fault
		.word _fault
		.word _swi					/* Software Interrupt */
		.word _fault
		.word _fault
		.word _pendsv				/* 14 PendSV  */
		.word _systick				/* 15 SysTick */
		/* ---- */
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		/* 7 */
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		/* 15 */
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		/* 23 */
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		/* 31 */
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		/* 39 */
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		/* 47 */
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		/* 35 */
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		/* 59 */



		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
		.word nvic_isr
_vectors_end:


		.end
