      
        .text
        .arm
        .global  irq_enable, irq_disable
        
        .section .text.irq_enable
		.type	irq_enable, %function
irq_enable:
        mrs     r12, cpsr
        bic     r12, r12, #0x80
        msr     cpsr_c, r12
        mov     pc, lr
        .size	irq_enable, .-irq_enable
        
        .section .text.irq_disable
		.type	irq_disable, %function
irq_disable:
        mrs     r12, cpsr
        orr     r12, r12, #0x80
        msr     cpsr_c, r12
        mov     pc, lr
       .size	irq_disable, .-irq_disable
 
		.end
		