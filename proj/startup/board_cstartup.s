
//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "board.h"

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

#define	ARM_MODE_USR	 0x10      /* Normal User Mode */
#define ARM_MODE_FIQ     0x11      /* FIQ Fast Interrupts Mode */
#define ARM_MODE_IRQ     0x12      /* IRQ Standard Interrupts Mode */
#define ARM_MODE_SVC     0x13      /* Supervisor Interrupts Mode */
#define ARM_MODE_ABT     0x17      /* Abort Processing memory Faults Mode */
#define	ARM_MODE_UND	 0x1B      /* Undefined Instructions Mode */
#define	ARM_MODE_SYS	 0x1F      /* System Running in Priviledged Operating Mode */
#define	ARM_MODE_MASK	 0x1F

#define I_BIT            0x80      /* disable IRQ when I bit is set */
#define F_BIT            0x40      /* disable FIQ when F bit is set */

//------------------------------------------------------------------------------
//         Startup routine
//------------------------------------------------------------------------------

            .align      4
            .arm

/* Exception vectors
 *******************/
        .section    .vectors, "ax" , %progbits
        .global     _start, _exit
_start:
        ldr     pc, resetVector			/* Reset */
        ldr     pc, undefVector			/* Undefined instruction */
        ldr     pc, swiVector			/* Software interrupt */
        ldr     pc, prefetchAbortVector	/* Prefetch abort */
        ldr     pc, dataAbortVector		/* Data abort */
#ifdef __IRQ_HANDLER__
        .word   0xb8a06f58				/* Magic number */
        ldr     pc, irqVector			/* Interrupt */
#else
        .word   0xb9205f80				/* Magic number */
		ldr		pc, [pc, #-0xFF0]		/* jump directly to address supplied by VIC */
#endif
        ldr     pc, fiqVector			/* Fast interrupt */
resetVector:
		.word	resetHandler
undefVector:
		.word	undefHandler
swiVector:
		.word	swiHandler
prefetchAbortVector:
		.word	prefetchAbortHandler
dataAbortVector:
		.word	dataAbortHandler
reservedVector:
		.word	0
irqVector:
		.word	irqHandler	
fiqVector:
		.word	fiqHandler
		
//------------------------------------------------------------------------------
/// Error Handlers
//------------------------------------------------------------------------------
        .section     .text.undefHandler
        .weak        undefHandler
        .type        undefHandler, %function
undefHandler:
        b       undefHandler
		.size        undefHandler, .-undefHandler
		
        .section     .text.swiHandler
        .weak        swiHandler
        .type        swiHandler, %function
swiHandler:
        b       swiHandler
 		.size        swiHandler, .-swiHandler
		
        .section     .text.prefetchAbortHandler
        .weak        prefetchAbortHandler
        .type        prefetchAbortHandler, %function
prefetchAbortHandler:
        b       prefetchAbortHandler
		.size        prefetchAbortHandler, .-prefetchAbortHandler

        .section     .text.dataAbortHandler
        .weak        dataAbortHandler
        .type        dataAbortHandler, %function
dataAbortHandler:
        b       dataAbortHandler
		.size        dataAbortHandler, .-dataAbortHandler

//------------------------------------------------------------------------------
/// Handles a fast interrupt request by branching to the address defined in the
/// AIC.
//------------------------------------------------------------------------------
        .section     .text.fiqHandler
        .weak        fiqHandler
        .type        fiqHandler, %function
fiqHandler:
        b       fiqHandler
		.size        fiqHandler, .-fiqHandler
	
//------------------------------------------------------------------------------
/// Handles incoming interrupt requests by branching to the corresponding
/// handler, as defined in the VIC. Supports interrupt nesting.
//------------------------------------------------------------------------------
        .section	.text.irqHandler
        .weak		irqHandler
        .type		irqHandler, %function

#ifdef __IRQ_HANDLER__
irqHandler:
/* Save interrupt context on the stack to allow nesting */
        sub     lr, lr, #4
        stmfd   sp!, {lr}
        mrs     lr, SPSR
        stmfd   sp!, {r0, lr}

/* Get the interrupt vector and acknowledge interrupt */
        ldr     lr,=_VICVectAddr
        ldr     r0, [lr]

/* Branch to interrupt handler in System mode */
        msr     CPSR_c, #ARM_MODE_SYS | F_BIT
        stmfd   sp!, {r1-r3, r12, lr}
        mov     lr, pc
        bx      r0
        ldmia   sp!, {r1-r3, r12, lr}
        msr     CPSR_c, #ARM_MODE_IRQ | I_BIT | F_BIT

/* Get the interrupt vector and acknowledge interrupt */
        ldr     lr,=_VICVectAddr
        str		lr, [lr]

/* Restore interrupt context and branch back to calling code */
        ldmia   sp!, {r0, lr}
        msr     SPSR_cxsf, lr
        ldmia   sp!, {pc}^
#else
irqHandler:
		b		irqHandler
#endif
		.size 		irqHandler, .-irqHandler

//------------------------------------------------------------------------------
/// Initializes the chip and branches to the main() function.
//------------------------------------------------------------------------------

        .section     .text.resetHandler
        .weak        resetHandler
        .type        resetHandler, %function

resetHandler:
/* Perform low-level initialization of the chip using LowLevelInit() */
        ldr     sp, =__usr_stack_top
	    ldr     r0, =LowLevelInit
        mov     lr, pc
        bx      r0

/* Relocate the .data section (copy from ROM to RAM) */
        ldr     r0, =__data_load
        ldr     r1, =__data_start
        ldr     r2, =__data_end
1:
        cmp     r1, r2
        ldrcc   r3, [r0], #4
        strcc   r3, [r1], #4
        bcc     1b

/* Clear the .bss section (zero init) */
	    ldr     r0, =__bss_start
        ldr     r1, =__bss_end
        mov     r2, #0
1:
        cmp     r0, r1
        strcc   r2, [r0], #4
        bcc     1b

/* Initialize stack pointers for all ARM modes */
        msr     CPSR_c,#(ARM_MODE_IRQ | I_BIT | F_BIT)
        ldr     sp,=__irq_stack_top           /* set the IRQ stack pointer */

        msr     CPSR_c,#(ARM_MODE_FIQ | I_BIT | F_BIT)
        ldr     sp,=__fiq_stack_top           /* set the FIQ stack pointer */

        msr     CPSR_c,#(ARM_MODE_SVC | I_BIT | F_BIT)
        ldr     sp,=__svc_stack_top           /* set the SVC stack pointer */

        msr     CPSR_c,#(ARM_MODE_ABT | I_BIT | F_BIT)
        ldr     sp,=__abt_stack_top           /* set the ABT stack pointer */

        msr     CPSR_c,#(ARM_MODE_UND | I_BIT | F_BIT)
        ldr     sp,=__und_stack_top           /* set the UND stack pointer */

        msr     CPSR_c,#(ARM_MODE_SYS | F_BIT) /* System Mode (interrupt enabled) */
        ldr     sp,=__usr_stack_top           /* set the USR stack pointer */

/* Branch to main() */
        ldr     r0, =main
        mov     lr, pc
        bx      r0

/* Loop indefinitely when program is finished */
_exit:
        b       _exit

		.size  resetHandler, .-resetHandler
