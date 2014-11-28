Welcome to the Source
=====================

This source-tree includes a wide feature set ranging from drivers and operationg system to network stack, filesystems and even a complete ADCS system. Some modules are included as default, others are available as an option.

Common to all the code is that it's configured and build using a single python tool called 'waf', this enables a wide range of configurable options that will help customise the software for your needs.

Getting started
---------------

If you have not done so yet, please open and read the `INSTALL.md` file. This will tell you all the details on how to install the toolchain, fetch the sources, configure and build the software, and finally upload to the FLASH chip.

What is included
----------------

- **nanomind**: Main boot files, linker script, library and board initialization
- **libgomspace**: Drivers, Shell, Utilities and FreeRTOS operating system
- **libcsp**: The CubeSat Space Protocol stak
- **libio**: Client library for all gomspace products _nanomind_, _nanopower_, _nanocom_, _nanocam_, _nanohub_, and FTP server/client
- **libstorage**: UFFS and FAT filesystem [option]
- **libcdh**: Flightplanner, Round-robin file database, Remote Shell Server and Distributed logging deamon [option]
- **libadcs**: B-dot detumbling, Kalman-filter and Nadir-pointing algorithm [option]
                
The optional libraries can be part of a binary release or a separate software contract between the customer and gomspace.

Where can i read more?
----------------------

The documentation is included in the `doc` directory of the root-folder or in the `lib/libname/doc` folder. The source itself is documented using doxygen in the header files. Each exported function should have its own documentation attached directly to it. This can also be parsed automatically by the doxygen generator or an IDE such as Eclipse.

For documentation about the hardware, please refer to the datasheet accompanying your NanoMind OBC.

