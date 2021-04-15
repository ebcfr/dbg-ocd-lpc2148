/*---------------------------------------------------------------------------
 *      Linker script for running in FLASH on the LPC2148
 *-------------------------------------------------------------------------*/

OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

/* The sizes of the stacks used by the application. NOTE: you need to adjust */
USR_STACK_SIZE = 256;
IRQ_STACK_SIZE = 256;
FIQ_STACK_SIZE = 0;
SVC_STACK_SIZE = 0;
ABT_STACK_SIZE = 0;
UND_STACK_SIZE = 0;

HEAP_END = 0x40008000;

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x00000000, LENGTH = 512K
    SRAM1 (rwx) : ORIGIN = 0x40000000, LENGTH = 32K
    SRAM2 (rwx) : ORIGIN = 0x7FD00000, LENGTH = 8K
}

SECTIONS
{  
	/* ----------------------------------------------------------------------
     * .vectors section : exception vectors (remapped in SRAM)
     * --------------------------------------------------------------------*/
	.vectors : {
		*(.vectors);
        . = 0x40;
	} >FLASH
	
	/* ----------------------------------------------------------------------
     * .fastcode section : functions to be executed from RAM (faster)
     * --------------------------------------------------------------------*/
    .fastcode : {
        . = ALIGN (4);
        __fastcode_load = LOADADDR (.fastcode);
        __fastcode_start = .;

        *(.text.fastcode)

        __fastcode_end = .;
    } >SRAM1 AT>FLASH

	/* ----------------------------------------------------------------------
     * .text section : code 
     * --------------------------------------------------------------------*/
    .text : {
		*(.init);
        *(.text .text.* .gnu.linkonce.t.*)
        
        /* The sections are named 'glue' because they glue together ARM and Thumb
		code.  I assume the 7 is there because the ARM7 was the first one to
		support Thumb code.  .glue_7 is used for ARM code calling Thumb code,
		and .glue_7t is used for Thumb code calling ARM code. */
        *(.glue_7t) *(.glue_7)
		
		*(.vfp11_veneer)
		*(.v4_bx)
		
       _fini = .;
    } >FLASH
    
	/* ----------------------------------------------------------------------
     * .rodata section (read-only (const) initialized variables)
     * --------------------------------------------------------------------*/
    .rodata :
    {
        *(.rodata .rodata.* .gnu.linkonce.r.*)
    } > FLASH
    
	/* ----------------------------------------------------------------------
     * .data section (read/write initialized variables)
     * 
     * initial values need to be copyed to their RAM location
     * --------------------------------------------------------------------*/
    .data : {
        . = ALIGN (4);
        __data_load = LOADADDR (.data);
        __data_start = .;
        *(.data .data.* .gnu.linkonce.d.*)
        . = ALIGN (4);
        __data_end = .;
    } >SRAM1 AT>FLASH

    /* ----------------------------------------------------------------------
     * .bss section (uninitialized variables)
     * 
     * this section need to be zeroed
     * --------------------------------------------------------------------*/
    .bss (NOLOAD) : {
        . = ALIGN (4);
        __bss_start = . ;
        *(.bss .bss.* .gnu.linkonce.b.*)
        *(COMMON)
        . = ALIGN (4);
        __bss_end = .;
    } >SRAM1

     /* ----------------------------------------------------------------------
     * .noinit section (uninitialized area)
     * 
     * no initialization is required
     * --------------------------------------------------------------------*/
    .noinit (NOLOAD) : {
        *(.noinit)
     } >SRAM1

    /* ----------------------------------------------------------------------
     * .stack section
     * --------------------------------------------------------------------*/
    .stack (NOLOAD) : {
        . = ALIGN (8);
        __stack_start = . ;
		
        . += IRQ_STACK_SIZE;
        . = ALIGN (4);
        __irq_stack_top = . ;

        . += FIQ_STACK_SIZE;
        . = ALIGN (4);
        __fiq_stack_top = . ;

        . += SVC_STACK_SIZE;
        . = ALIGN (4);
        __svc_stack_top = . ;

        . += ABT_STACK_SIZE;
        . = ALIGN (4);
        __abt_stack_top = . ;

        . += UND_STACK_SIZE;
        . = ALIGN (4);
        __und_stack_top = . ;

        . += USR_STACK_SIZE;
        . = ALIGN (4);
        __usr_stack_top = . ;

        __stack_end = .;
    } >SRAM1
    
    /* ----------------------------------------------------------------------
     * .stack section
     * --------------------------------------------------------------------*/
   .heap (NOLOAD) : {
        . = ALIGN (8);
		__heap_start = .;
		__heap_end = HEAP_END ;
   } >SRAM1
}
