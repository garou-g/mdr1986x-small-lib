/* File: startup_ARMCM0.S
 * Purpose: startup file for Cortex-M0 devices. Should use with
 *   GCC for ARM Embedded Processors
 * Version: V2.0
 * Date: 16 August 2013
 *
 * Copyright (c) 2011 - 2013 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/
	.syntax	unified
	.arch	armv6-m

	.section .stack
	.align	3
#ifdef __STACK_SIZE
	.equ	Stack_Size, __STACK_SIZE
#else
	.equ	Stack_Size, 0x2000
#endif
	.globl	__StackTop
	.globl	__StackLimit
__StackLimit:
	.space	Stack_Size
	.size	__StackLimit, . - __StackLimit
__StackTop:
	.size	__StackTop, . - __StackTop

	.section .heap
	.align	3
#ifdef __HEAP_SIZE
	.equ	Heap_Size, __HEAP_SIZE
#else
	.equ	Heap_Size, 0x2000
#endif
	.globl	__HeapBase
	.globl	__HeapLimit
__HeapBase:
	.if	Heap_Size
	.space	Heap_Size
	.endif
	.size	__HeapBase, . - __HeapBase
__HeapLimit:
	.size	__HeapLimit, . - __HeapLimit

	.section .isr_vector
	.align 2
	.globl	__isr_vector
__isr_vector:
	.long	__StackTop                  /* Top of Stack */
	.long	Reset_Handler               /* Reset Handler */
	.long	NMI_Handler                 /* NMI Handler */
	.long	HardFault_Handler           /* Hard Fault Handler */
	.long	0                           /* Reserved */
	.long	0                           /* Reserved */
	.long	0                           /* Reserved */
	.long	0                           /* Reserved */
	.long	0                           /* Reserved */
	.long	0                           /* Reserved */
	.long	0                           /* Reserved */
	.long	SVC_Handler                 /* SVCall Handler */
	.long	0                           /* Reserved */
	.long	0                           /* Reserved */
	.long	PendSV_Handler              /* PendSV Handler */
	.long	SysTick_Handler             /* SysTick Handler */

	/* External interrupts */
	.long	MIL_STD_1553B2_IRQHandler   /* IRQ0 */
	.long	MIL_STD_1553B1_IRQHandler   /* IRQ1 */
	.long	USB_IRQHandler              /* IRQ2 */
	.long	CAN1_IRQHandler             /* IRQ3 */
	.long	CAN2_IRQHandler             /* IRQ4 */
	.long	DMA_IRQHandler              /* IRQ5 */
	.long	UART1_IRQHandler            /* IRQ6 */
	.long	UART2_IRQHandler            /* IRQ7 */
	.long	SSP1_IRQHandler             /* IRQ8 */
	.long	BUSY_IRQHandler             /* IRQ9 */
	.long	ARINC429R_IRQHandler        /* IRQ10 */
	.long	POWER_IRQHandler            /* IRQ11 */
	.long	WWDG_IRQHandler             /* IRQ12 */
	.long	TIMER4_IRQHandler           /* IRQ13 */
	.long	TIMER1_IRQHandler           /* IRQ14 */
	.long	TIMER2_IRQHandler           /* IRQ15 */
	.long	TIMER3_IRQHandler           /* IRQ16 */
	.long	ADC_IRQHandler              /* IRQ17 */
	.long	ETHERNET_IRQHandler         /* IRQ18 */
	.long	SSP3_IRQHandler             /* IRQ19 */
	.long	SSP2_IRQHandler             /* IRQ20 */
	.long	ARINC429T1_IRQHandler       /* IRQ21 */
	.long	ARINC429T2_IRQHandler       /* IRQ22 */
	.long	ARINC429T3_IRQHandler       /* IRQ23 */
	.long	ARINC429T4_IRQHandler       /* IRQ24 */
	.long	0                           /* IRQ25 reserved */
	.long	0                           /* IRQ26 reserved */
	.long	BKP_IRQHandler              /* IRQ27 */
	.long	EXT_INT1_IRQHandler         /* IRQ28 */
	.long	EXT_INT2_IRQHandler         /* IRQ29 */
	.long	EXT_INT3_IRQHandler         /* IRQ30 */
	.long	EXT_INT4_IRQHandler         /* IRQ31 */

	.size	__isr_vector, . - __isr_vector

	.text
	.thumb
	.thumb_func
	.align	1
	.globl	Reset_Handler
	.type	Reset_Handler, %function
Reset_Handler:
/*  Firstly it copies data from read only memory to RAM. There are two schemes
 *  to copy. One can copy more than one sections. Another can only copy
 *  one section.  The former scheme needs more instructions and read-only
 *  data to implement than the latter.
 *  Macro __STARTUP_COPY_MULTIPLE is used to choose between two schemes.  */

#ifdef __STARTUP_COPY_MULTIPLE
/*  Multiple sections scheme.
 *
 *  Between symbol address __copy_table_start__ and __copy_table_end__,
 *  there are array of triplets, each of which specify:
 *    offset 0: LMA of start of a section to copy from
 *    offset 4: VMA of start of a section to copy to
 *    offset 8: size of the section to copy. Must be multiply of 4
 *
 *  All addresses must be aligned to 4 bytes boundary.
 */
	ldr	r4, =__copy_table_start__
	ldr	r5, =__copy_table_end__

.L_loop0:
	cmp	r4, r5
	bge	.L_loop0_done
	ldr	r1, [r4]
	ldr	r2, [r4, #4]
	ldr	r3, [r4, #8]

.L_loop0_0:
	subs	r3, #4
	blt	.L_loop0_0_done
	ldr	r0, [r1, r3]
	str	r0, [r2, r3]
	b	.L_loop0_0

.L_loop0_0_done:
	adds	r4, #12
	b	.L_loop0

.L_loop0_done:
#else
/*  Single section scheme.
 *
 *  The ranges of copy from/to are specified by following symbols
 *    __etext: LMA of start of the section to copy from. Usually end of text
 *    __data_start__: VMA of start of the section to copy to
 *    __data_end__: VMA of end of the section to copy to
 *
 *  All addresses must be aligned to 4 bytes boundary.
 */
	ldr	r1, =__etext
	ldr	r2, =__data_start__
	ldr	r3, =__data_end__
	bl	.L_copy_mm

	ldr	r1, =__edata
	ldr	r2, =__ramfunc_start__
	ldr	r3, =__ramfunc_end__
	bl	.L_copy_mm

#endif /*__STARTUP_COPY_MULTIPLE */

/*  This part of work usually is done in C library startup code. Otherwise,
 *  define this macro to enable it in this startup.
 *
 *  There are two schemes too. One can clear multiple BSS sections. Another
 *  can only clear one section. The former is more size expensive than the
 *  latter.
 *
 *  Define macro __STARTUP_CLEAR_BSS_MULTIPLE to choose the former.
 *  Otherwise define macro __STARTUP_CLEAR_BSS to choose the later.
 */
#ifdef __STARTUP_CLEAR_BSS_MULTIPLE
/*  Multiple sections scheme.
 *
 *  Between symbol address __copy_table_start__ and __copy_table_end__,
 *  there are array of tuples specifying:
 *    offset 0: Start of a BSS section
 *    offset 4: Size of this BSS section. Must be multiply of 4
 */
	ldr	r3, =__zero_table_start__
	ldr	r4, =__zero_table_end__

.L_loop2:
	cmp	r3, r4
	bge	.L_loop2_done
	ldr	r1, [r3]
	ldr	r2, [r3, #4]
	movs	r0, #0

.L_loop2_0:
	subs	r2, #4
	blt	.L_loop2_0_done
	str	r0, [r1, r2]
	b	.L_loop2_0
.L_loop2_0_done:

	adds	r3, #8
	b	.L_loop2
.L_loop2_done:
#elif defined (__STARTUP_CLEAR_BSS)
/*  Single BSS section scheme.
 *
 *  The BSS section is specified by following symbols
 *    __bss_start__: start of the BSS section.
 *    __bss_end__: end of the BSS section.
 *
 *  Both addresses must be aligned to 4 bytes boundary.
 */
	ldr	r1, =__bss_start__
	ldr	r2, =__bss_end__

	movs	r0, #0

	subs	r2, r1
	ble	.L_loop3_done

.L_loop3:
	subs	r2, #4
	str	r0, [r1, r2]
	bgt	.L_loop3
.L_loop3_done:
#endif /* __STARTUP_CLEAR_BSS_MULTIPLE || __STARTUP_CLEAR_BSS */

#ifndef __NO_SYSTEM_INIT
	bl	SystemInit
#endif

#ifndef __START
#define __START _start
#endif
	bl	__START

/* Copy memory to memory
	r1: src;
	r2: dest start;
	r3: dest end
*/
.L_copy_mm:
	subs	r3, r2
	ble	.L_loop1_done

.L_loop1:
	subs	r3, #4
	ldr	r0, [r1,r3]
	str	r0, [r2,r3]
	bgt	.L_loop1

.L_loop1_done:
	bx	lr
/* End .L_copy_mm */

	.pool
	.size	Reset_Handler, . - Reset_Handler

	.align	1
	.thumb_func
	.weak	Default_Handler
	.type	Default_Handler, %function
Default_Handler:
	b	.
	.size	Default_Handler, . - Default_Handler

/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
	.macro	def_irq_handler	handler_name
	.weak	\handler_name
	.set	\handler_name, Default_Handler
	.endm

#ifdef RAM_BOOT
NMI_Handler:
    ldr	r3, =0x1008
    ldr r2, [r3]
    bx r2

HardFault_Handler:
    ldr	r3, =0x100C
    ldr r2, [r3]
    bx r2

SVC_Handler:
    ldr	r3, =0x102C
    ldr r2, [r3]
    bx r2

PendSV_Handler:
    ldr	r3, =0x1038
    ldr r2, [r3]
    bx r2

SysTick_Handler:
    ldr	r3, =0x103C
    ldr r2, [r3]
    bx r2

/* External Interrupts */

MIL_STD_1553B2_IRQHandler:
    ldr	r3, =0x1040
    ldr r2, [r3]
    bx r2

MIL_STD_1553B1_IRQHandler:
    ldr	r3, =0x1044
    ldr r2, [r3]
    bx r2

USB_IRQHandler:
    ldr	r3, =0x1048
    ldr r2, [r3]
    bx r2

CAN1_IRQHandler:
    ldr	r3, =0x104C
    ldr r2, [r3]
    bx r2

CAN2_IRQHandler:
    ldr	r3, =0x1050
    ldr r2, [r3]
    bx r2

DMA_IRQHandler:
    ldr	r3, =0x1054
    ldr r2, [r3]
    bx r2

UART1_IRQHandler:
    ldr	r3, =0x1058
    ldr r2, [r3]
    bx r2

UART2_IRQHandler:
    ldr	r3, =0x105C
    ldr r2, [r3]
    bx r2

SSP1_IRQHandler:
    ldr	r3, =0x1060
    ldr r2, [r3]
    bx r2

BUSY_IRQHandler:
    ldr	r3, =0x1064
    ldr r2, [r3]
    bx r2

ARINC429R_IRQHandler:
    ldr	r3, =0x1068
    ldr r2, [r3]
    bx r2

POWER_IRQHandler:
    ldr	r3, =0x106C
    ldr r2, [r3]
    bx r2

WWDG_IRQHandler:
    ldr	r3, =0x1070
    ldr r2, [r3]
    bx r2

TIMER4_IRQHandler:
    ldr	r3, =0x1074
    ldr r2, [r3]
    bx r2

TIMER1_IRQHandler:
    ldr	r3, =0x1078
    ldr r2, [r3]
    bx r2

TIMER2_IRQHandler:
    ldr	r3, =0x107C
    ldr r2, [r3]
    bx r2

TIMER3_IRQHandler:
    ldr	r3, =0x1080
    ldr r2, [r3]
    bx r2

ADC_IRQHandler:
    ldr	r3, =0x1084
    ldr r2, [r3]
    bx r2

ETHERNET_IRQHandler:
    ldr	r3, =0x1088
    ldr r2, [r3]
    bx r2

SSP3_IRQHandler:
    ldr	r3, =0x108C
    ldr r2, [r3]
    bx r2

SSP2_IRQHandler:
    ldr	r3, =0x1090
    ldr r2, [r3]
    bx r2

ARINC429T1_IRQHandler:
    ldr	r3, =0x1094
    ldr r2, [r3]
    bx r2

ARINC429T2_IRQHandler:
    ldr	r3, =0x1098
    ldr r2, [r3]
    bx r2

ARINC429T3_IRQHandler:
    ldr	r3, =0x109C
    ldr r2, [r3]
    bx r2

ARINC429T4_IRQHandler:
    ldr	r3, =0x10A0
    ldr r2, [r3]
    bx r2

BKP_IRQHandler:
    ldr	r3, =0x10A4
    ldr r2, [r3]
    bx r2

EXT_INT1_IRQHandler:
    ldr	r3, =0x10A8
    ldr r2, [r3]
    bx r2

EXT_INT2_IRQHandler:
    ldr	r3, =0x10AC
    ldr r2, [r3]
    bx r2

EXT_INT3_IRQHandler:
    ldr	r3, =0x10B0
    ldr r2, [r3]
    bx r2

EXT_INT4_IRQHandler:
    ldr	r3, =0x10B4
    ldr r2, [r3]
    bx r2

#else
	def_irq_handler NMI_Handler
	def_irq_handler HardFault_Handler
	def_irq_handler SVC_Handler
	def_irq_handler PendSV_Handler
	def_irq_handler SysTick_Handler

/* External Interrupts */

	def_irq_handler MIL_STD_1553B2_IRQHandler
	def_irq_handler MIL_STD_1553B1_IRQHandler
	def_irq_handler USB_IRQHandler
	def_irq_handler CAN1_IRQHandler
	def_irq_handler CAN2_IRQHandler
	def_irq_handler DMA_IRQHandler
	def_irq_handler UART1_IRQHandler
	def_irq_handler UART2_IRQHandler
	def_irq_handler SSP1_IRQHandler
	def_irq_handler BUSY_IRQHandler
	def_irq_handler ARINC429R_IRQHandler
	def_irq_handler POWER_IRQHandler
	def_irq_handler WWDG_IRQHandler
	def_irq_handler TIMER4_IRQHandler
	def_irq_handler TIMER1_IRQHandler
	def_irq_handler TIMER2_IRQHandler
	def_irq_handler TIMER3_IRQHandler
	def_irq_handler ADC_IRQHandler
	def_irq_handler ETHERNET_IRQHandler
	def_irq_handler SSP3_IRQHandler
	def_irq_handler SSP2_IRQHandler
	def_irq_handler ARINC429T1_IRQHandler
	def_irq_handler ARINC429T2_IRQHandler
	def_irq_handler ARINC429T3_IRQHandler
	def_irq_handler ARINC429T4_IRQHandler
	def_irq_handler BKP_IRQHandler
	def_irq_handler EXT_INT1_IRQHandler
	def_irq_handler EXT_INT2_IRQHandler
	def_irq_handler EXT_INT3_IRQHandler
	def_irq_handler EXT_INT4_IRQHandler
#endif

	.end
