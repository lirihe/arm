Understanding ARM7TDMI Interrupt Vectors
========================================

The ARM7TDMI has 8 interrupt vectors which are setup in `boot.c`, these are located at address 0x0 in internal SRAM. When the hardware gets a harware exception it will bank some registers switch mode and lookup the vector at the corresponding address in RAM. The vectors are laid out like this:

		/** Setup interrupt vectors */
		set_isr_vector((unsigned int *) 0x00, AT91F_Soft_Reset_Handler);
		set_isr_vector((unsigned int *) 0x04, AT91F_Undefined_Instruction_Handler);
		set_isr_vector((unsigned int *) 0x08, AT91F_Swi_Handler);
		set_isr_vector((unsigned int *) 0x0C, AT91F_Prefetch_Abort_Handler);
		set_isr_vector((unsigned int *) 0x10, AT91F_Data_Abort_Handler);
		set_isr_vector((unsigned int *) 0x14, AT91F_Prefetch_Abort_Handler);
		set_isr_vector((unsigned int *) 0x18, AT91C_AIC_BRANCH_OPCODE);
		set_isr_vector((unsigned int *) 0x1C, AT91C_AIC_BRANCH_OPCODE);

The function set_isr_vector will take the address of the handler, for example `AT91F_Swi_Handler`, and place it next to the vector table with an offset of 0x20. Then it will write a long jump instruction into the vector, reading from this address causing the processor to execute the code directly. This is used mainly for the software interrupt handler (swi), to generate a forced yield of a FreeRTOS task within the `taskYield()` call. The swi handler will change context perform rescheduling and enter the selected context, for example the idle task.

Other handlers such as: Soft Reset, Undefined Instruction, Prefetch Abort and Data Abort are handled as a crash of the software. A variable will be written to memory and the system rebooted. Upon boot-up, the variable will be read from memory and the log-system will detect an event called `E_OBC_CRASH`. This will happen if you try to write or read beyond the physical memory boundaries (Data Abort), dereference a null-pointer and thereby executing invalid code (Undefined Instruction) or execution of the program continues beyond memory boundaries (Prefetch Abort). The Soft Reset is a special instruction in the ARM assembler, but it is not really used to perform reboots, since a watchdog hard-reboot is preferred over a software reset. An example of a crash handler is shown below:

		void AT91F_Data_Abort_Handler(void) __attribute__((naked));
		void AT91F_Data_Abort_Handler(void) {
			portDISABLE_INTERRUPTS();
			int lr = get_banked_lr();
	
			printf("DATA ABORT: LR=%#0x\r\n", lr);
	
			if (cpu_set_reset_cause)
				cpu_set_reset_cause(CPU_RESET_DATA_ABORT);
			cpu_reset();
		}

The example shows how a printout is made of the link register address causing the violating data abort and how the cpu reset cause is written to RAM just before the CPU is reset. The OBC uses a special property of SRAM memory that keeps a short lifetime of the bits of around 10 seconds after the power is turned off. Therefore the value stored in the reset cause register actually is preserved during a reboot and is read by the task_init and inserted into the log system at bootup.

IRQ: Interrupts
---------------

The IRQ is a special case of the exception vector (0x18), which calls the `AT91C_AIC_BRANCH_OPCODE`. The branch opcode is in fact a branch to an addrress which corresponds to the AIC (Advanced Interrupt Controller) perihperal's highest priority interrupt. In other words, the AIC is askes which interrupt to handle first. The AIC is also initialised by th boot.c file, and it looks like this:

		/* Set up the default interrupts handler vectors in the GIC_SVR table */
		AT91C_BASE_AIC->AIC_SPU = (int) AT91F_Spurious_handler;
		AT91C_BASE_AIC->AIC_SVR[0] = (int) AT91F_Default_FIQ_handler;
		for (i = 1; i < 31; i++)
			AT91C_BASE_AIC->AIC_SVR[i] = (int) AT91F_Default_IRQ_handler;

The AIC has 31 IRQ's and one FIQ. The FIQ is a non maskable intterupt which has several hardware banked registers and is placed at the bottom of the interrupt vector table in order to start executing code directly without the jump. It is also referred to as Fast Interrupt Request, but is unused on the NanoMind.

The other 31 IRQ's can be used for peripherals, external interrupts and so on. They ara initialised to point at the default IRQ handler, which will print out an error and reboot the CPU if an invalid interrupt occurs.

The special Spurious interrupt is a function that is called if the cause of the interrupt has disappeared before the ARM7TDMI core was halted and has read the SVR. The default action is to write out "SPUR" on the USART and continue execution. If you see lots of these it can indicate a hardware malfunction.

How to register your own ISR
----------------------------

The simplest possible interrupt service routine will look like this:

		static void __attribute__((naked)) my_isr(void) {
			portSAVE_CONTEXT();
			...
			AT91C_BASE_AIC->AIC_EOICR = 1;
			portRESTORE_CONTEXT();
		}

It's very important to follow this template. It will mark the function as naked to avoid the compiler from implementing push/pop actions at the beginning and end of the code. This is handled instead by the FreeRTOS macros `save_context` and `restore_context`. And finally, it's important that the AIC is told to unnest its interrupt stack by writing a dummy value to the AIC_EOICR register.

If you are making API calls to FreeRTOS that could cause a task to wake up, you must add a few extras to the isr, so it looks like this:

		static void __attribute__((naked)) pca9665_isr(void) {
			static portBASE_TYPE task_woken;
			portSAVE_CONTEXT();
			task_woken = pdFALSE;
			my_function_with_api_call(&task_woken);
			if (task_woken == pdTRUE)
				portYIELD_FROM_ISR();
			AT91C_BASE_AIC->AIC_EOICR = 1;
			portRESTORE_CONTEXT();
		}

The check for `task_woken == true` ensures that a call to the scheduler is made before we are restoring the context. The scheudler may or may not decide that the new task woken has higher priority and switch the stack poitner to the new task, so the context of the higher priority task would be restored.

