/* Memory Controller Registers */
.equ CS0_REG, 0xFFE00000
.equ CS1_REG, 0xFFE00004
.equ CS2_REG, 0xFFE00008
.equ CS3_REG, 0xFFE0000C

/* Memory Controller, Chip addresses and wait state */
.equ CS0_VAL, 0x400030A5
.equ CS1_VAL, 0x480030A5
.equ CS2_VAL, 0x500030A5
.equ CS3_VAL, 0x5800303E

/* Stack Sizes */
.set  UND_STACK_SIZE, 0x00000000		/* stack for "undefined instruction" interrupts				 	 */
.set  ABT_STACK_SIZE, 0x00000000		/* stack for "abort" interrupts				                 	 */
.set  FIQ_STACK_SIZE, 0x00000000		/* stack for "FIQ" interrupts					 				 */
.set  IRQ_STACK_SIZE, 0X00004000		/* stack for "IRQ" normal interrupts			 				 */
.set  SVC_STACK_SIZE, 0x00004000		/* stack for "SVC" supervisor mode				  				 */

/* Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs (program status registers) 	 */
.set  ARM_MODE_USR, 0x10            	/* Normal User Mode 											 */
.set  ARM_MODE_FIQ, 0x11            	/* FIQ Processing Fast Interrupts Mode 							 */
.set  ARM_MODE_IRQ, 0x12            	/* IRQ Processing Standard Interrupts Mode 						 */
.set  ARM_MODE_SVC, 0x13            	/* Supervisor Processing Software Interrupts Mode 				 */
.set  ARM_MODE_ABT, 0x17            	/* Abort Processing memory Faults Mode 							 */
.set  ARM_MODE_UND, 0x1B            	/* Undefined Processing Undefined Instructions Mode 			 */
.set  ARM_MODE_SYS, 0x1F            	/* System Running Priviledged Operating System Tasks  Mode		 */
.set  I_BIT, 0x80               		/* when I bit is set, IRQ is disabled (program status registers) */
.set  F_BIT, 0x40               		/* when F bit is set, FIQ is disabled (program status registers) */

/* Definitions of external symbols */
.extern low_level_init

/* identify all GLOBAL symbols  */
.global _start

/* GNU assembler controls  */
.arm									/* compile for 32-bit ARM instruction set						 */
.align									/* align section on 32-bit boundary								 */

.section .init							/* all assembler code that follows will go into .init section	 */
.align									/* align section on 32-bit boundary								 */
_start:

				/* Chip select setup */
				ldr   r3, =CS0_REG
				ldr	  r4, =CS0_VAL
				str	  r4, [r3]
				ldr	  r3, =CS1_REG
				ldr	  r4, =CS1_VAL
				str	  r4, [r3]
				ldr	  r3, =CS2_REG
				ldr	  r4, =CS2_VAL
				str	  r4, [r3]
				ldr	  r3, =CS3_REG
				ldr	  r4, =CS3_VAL
				str	  r4, [r3]

				/* Load the ip register with the jump address */
				ldr	  ip, _ptr_jump

				/* Load remap register location and value (0x00000001) */
				ldr	  r3, =0xFFE00020
				mov	  r4, #1

				/* Remap! */
				str	  r4, [r3]

				/* Break pipeline and Scram! */
				mov	  pc, ip

_ptr_jump:
				/* Provide a pointer to the ROM/RAM jum address */
				.long jump
.section .jump
jump:

				/* Setup a stack for each mode with interrupts initially disabled. */
    			ldr			r0, = _sstack						/* r0 = top-of-stack  */
    			
    			msr			CPSR_c, #ARM_MODE_UND|I_BIT|F_BIT 	/* switch to Undefined Instruction Mode  */
    			mov			sp, r0								/* set stack pointer for UND mode  */
    			sub			r0, r0, #UND_STACK_SIZE				/* adjust r0 past UND stack  */
    			
    			msr			CPSR_c, #ARM_MODE_ABT|I_BIT|F_BIT 	/* switch to Abort Mode */
    			mov			sp, r0								/* set stack pointer for ABT mode  */
    			sub			r0, r0, #ABT_STACK_SIZE				/* adjust r0 past ABT stack  */
    			
    			msr			CPSR_c, #ARM_MODE_FIQ|I_BIT|F_BIT 	/* switch to FIQ Mode */
    			mov			sp, r0								/* set stack pointer for FIQ mode  */
   				sub			r0, r0, #FIQ_STACK_SIZE				/* adjust r0 past FIQ stack  */
   				
    			msr			CPSR_c, #ARM_MODE_IRQ|I_BIT|F_BIT 	/* switch to IRQ Mode */
    			mov			sp, r0								/* set stack pointer for IRQ mode  */
    			sub			r0, r0, #IRQ_STACK_SIZE				/* adjust r0 past IRQ stack  */
    			
    			msr			CPSR_c, #ARM_MODE_SVC|I_BIT|F_BIT 	/* switch to Supervisor Mode */
    			mov			sp, r0								/* set stack pointer for SVC mode  */
    			sub			r0, r0, #SVC_STACK_SIZE				/* adjust r0 past SVC stack  */
    			
    			msr			CPSR_c, #ARM_MODE_SYS|I_BIT|F_BIT 	/* switch to System Mode */
    			mov			sp, r0								/* set stack pointer for SYS mode  */

				/* We want to start in supervisor mode.  Operation will switch to system
				   mode when the first task starts. */
				msr			CPSR_c, #ARM_MODE_SVC|I_BIT|F_BIT
				bl			low_level_init

				/* Branch to main() */
				ldr     	r0, =main
				mov     	lr, pc
				bx      	r0

				/* Loop indefinitely when program is finished */
1:
				b       	1b
