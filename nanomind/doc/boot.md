Understading how NanoMind boots
===============================

### `crt.s`
The hardware places the program counter at address 0x0 after a power-on-reset. At the time of boot, this address is mapped to the physical address of 0x40000000 which coressponds to the FLASH chip situated on chip-select zero. At this address of the flash, there is a tiny bit of assembler code (found in src/crt.s) which configures the addresses, sizes and wait-states for the external bus interface of the microprocessor. When this is completed the assembler code does a remap of the memory, which changes the memory map of the microprocessor to move the CS0 flash to address 0x40000000 and at the same time does a long jump to the first instruction at the _jump:_ section of the code. From here it crt.s assembly continues to setup a stack pointer for each processor mode, switches to supervisor mode and calls the low_level_init function which is located in boot.c

### `boot.s`
The `low_level_init` function starts by locking the CPU PLL which multiplies the 8 MHz crystal oscillator by five to a speed of 40 MHz. Then the master clock is switched to use the PLL in order to speed up the rest of the boot process. The tasks performed by the `low_level_init` function is then to relocate the data section from FLASH into SRAM, Clear the BSS and initialize the interrupts. You can read more about the interrupt handling in the `interrupts.md` file. The final thing `low_level_init` does is to enable the watchdog timer. When `low_level_init` returns, crt.s makes the call to main() which should never return.

### `main.c`
The main file sets up the first peripherals such as the USART and the optional external Real-time-clock. It also initialises the network stack, the netowrk interfaces and the basic console commands. The purpose of the main.c file is to only initialise what is necessary before the kernel clock is started. The reason is that even though interrupts vectors have been loaded, the global interrupt has not been enabled yet. Main.c is also the final code executed before the system goes from single-threaded to multi-threaded, and all blocking calls to the FreeRTOS API is therefore prohibited. The final thing main.c does is to register the tasks that should start immediately. This includes the supervisor task which rests the watchdog, the csp router and server which handles network requests, and the actual system initialisation task_init.

_Please note: Do not add any code to main.c unless its absolutely necessary to execute before starting the FreeRTOS kernel. Adding your own init code is much more safe to add in `task_init`_

### `task_init.c`
The init task has the responsibility of staring up some of the more heavy systems such as the filesystem, log, ftp, and rsh servers. Big systems such as the ADCS task and other tasks which rely on the filesystem, is also required to wait till later in the `task_init` procedure.

This is the place to put your own initialization procedure for your own system tasks. At this point all systems are running and its safe to execute both network and filesystem code.

Booting alternative software image
==================================

After mounting the filesystem, the init task will look for an alternative software image. The location used is the `/boot` partition located on the secondary FLASH chip at address 0x48000000. In this chip the upper half is reserved for a filesystem, which can hold multiple compressed or uncompressed software images.

If a file called `/boot/boot.conf` exists, it will read two lines from this file where the first line must be the path to the desired image, and the second line the checksum of the image. Both lines in ASCII code. So an example of a boot file would be:

		/boot/myimage.hex
		3A828C2A

The OBC has a command in its server API called `OBC_PORT_BOOT_CONF` which accepts the path and the checksum of an image. This would typically be called after a successful FTP upload of the image itself.

If the bootloader finds the boot configuration file, it will delete it after reading the contents and before booting the actual image. After deletion it will start moving the image from the filesystem into RAM memory, and finally check the checksum and jump to address 0x50000000.

If the bootloader does not find a boot configuration file, it will abort loading to RAM and continue with the startup of the ROM image as previously described.

There is also a command called `OBC_PORT_LOAD_IMG` which can be used to do this on an already booted system. After using the load function and controlling the returned checksum, call the `OBC_PORT_JUMP` function to jump to any desired address in the image you loaded.

When you have already booted a RAM image (not configured with --rom), do not try to load another RAM image, since this would cause you to overwrite already excecuting code which will generate an exception. If you use the ftp-client to upload an image directly to RAM, ensure that the system has been reset and are executing the FTP-server from ROM and not from an already loaded RAM image.

The linker script for the ROM image will ensure that no memory in the range from 0x50000000 to 0x50100000 will be used. This is reserved for uploading a new software image to RAM.

