Howto install NanoMind workspace
================================

This is a simple guide to get started with development for the NanoMind.

Requirements
------------

- Recent GNU/Linux distribution, for example Ubuntu or Debian
- NanoMind On-board computer
- USB to Serial converter and 'minicom' for serial communications
- Optional, but recommened, an Amontech JtagKEY-Tiny interface
- Registration on gomspace github organisation

Toolchain
------------

1. Download the latest ARM toolchain from gomspace.com. This is specially built for the NanoMind OBC based on the linaro GNU GCC distribution, and is available for both 32-bit and 64-bit host machines.
2. Extract tar-ball, tip: 'tar -xfzv <filename>'
3. Move toolchain folder to '/usr/local/arm' or your preferred location
4. Insert the following line to the end of your ~/.profile

		PATH="/usr/local/arm/bin:$PATH"

5. Log-out and log-in again to the GNOME environment to update your PATH enviroment variable globally

Download source
------------------

Gomspace uses `git` for software version control and allows all its customers full access to the source code, and the commit history. We belive that open source is the best way to help our customers build better products, and we encourage customers to use the online issue tracker for any software related enquiries.

The latest source is available at github.com. In order to gain access, you must register your product with gomspace by sending an email to <johan@gomspace.com> containing your *name*, *email*, *organisation* and *github username*. When you have gained access, and can see the nanomind repository when you open [http://github.com/gomspace/](http://github.com/gomspace/) you are ready to clone the source.

You can choose to clone the software using either HTTP or SSH. The easiest method is to use the HTTP method and enter your github username and password when asked for it. SSH requires that you setup a private- and public-key on your PC and share your public-key with github.com, before you can download the source.

1. Clone software using HTTP

		cd ~
		mkdir git
		cd git
		git clone https://github.com/GomSpace/nanomind.git

2. Clone submodules (libraries)

		cd nanomind-pub
		git submodule init
		git submodule update

That's it.

Configure source
-------------------

The NanoMind software uses a modern python based build system called 'waf'. This system takes replaces the well known 'autotools' family and replaces ./configure, make, make install with ./waf configure build program instead. In order to execute the build script, you must have pyton installed. This is standard on most GNU/Linux distributions. The executable scipt is called 'waf' and is placed in the root-folder of the nanomind software repository. It uses a recursive scheme to read the local 'wscript' file, and submerge into the 'lib/lib...' folders and find their respective 'wscript' files. This enables an ellaborate configuration of both the NanoMind software but also the libraries.

To generate a help output for the configuration run this:

		./waf configure --help

You will now see that each module included in the build, nanomind, libcsp, libgomspace and libio has each its separate options.

Since there are several ways to configure the NanoMind compile-time, there are also included a set of finished configurations for different boards. These reside in the root folder and is called 'configure*.sh' and can be opened using a text editor. In order to execute these configurations, just execute the files as a shell script:

		./configure-a712.sh

The configure script will check if the toolchain has been properly installed and report any issues.

Build
--------

When the source has been configured, execute the build step like this:

		./waf build

If the build fails at this step, please ensure that you have the latest 'git' master release like this:

		cd ~
		cd git/nanomind-pub
		git checkout master
		git pull
		git submodule update
		./waf clean
		./configure-a712.sh
		./waf build

If the build continues to fail, please file an issue on github with a full dump of your console.

Programming the NanoMind
---------------------------
Now that the build has completed, there are two methods of loading the software onto the nanomind obc. You can upload the image as a --rom image, or as a ram image. The first option '--rom' is written like this, be cause this is the option that you must apply to the './waf configure --rom' command, in order to include the right linker script to link the software to reside on the flash chip at address 0x40000000. If you wish to write the program to flash, you will need generally need to use the JTAG interface.

The other option is to load the software into RAM. This is a quick method to test some software before deploying it to the FLASH chip. The software will be linked to address 0x50000000 and uploaded into a reserved address space by the 'ftp-program' tool over the serial port. When the software image has been completely copied to RAM the OBC has a command to jump to the start address and execute the image. This method uses the same technique as is used when doing in-orbit software upload, apart from the fact that an in-orbit upload would use a compressed image and place it onto the filesystem before loading into memory. It's important when you use this option that the software is not configured with the '--rom' option. There is no '--ram' option.

A: Write to FLASH
------------------

The ARM processor supprots several types of JTAG device. Gomspace uses a cheap device from Amontech called JTAGkey-Tiny, which is available directly from their web page, but can also be ordered as an option when you buy a nanomind. By ordering through gomspace you will also receive a small interface connector to go from the 20-pin amontech JTAG header to the 8-pin Picoblade connector on the Nanomind OBC.

In order to execute the JTAG programmer, please see how to install in the <root>/jtag folder of your nanomind repository. When openocd has been installed, please run this command:

		./waf program

This should invoke the openocd programmer and write the program to FLASH. If it's not working please ensure you have the proper rights to acceess the JTAG device. Optionally you may temporarily use 'sudo' to gain full access.

B: Upload to RAM
-----------------  

The FTP-Client program is used to upload software to the NanoMind Computer. This terminal application uses the CSP network protocol, and can be used over any CSP compatible link, such as KISS/RS232. FTP-Client can be found on github as well as the nanomind source code.

### Installing FTP-Client

		tar -xfzv ftp-client.tar.gz
		cd ftp-client
		git submodule init
		git submodule update  
		./waf configure clean build
		sudo ./waf install

### Using FTP-Client

FTP-Client has the following options:

		-a ADDRESS,	Set address (default: 8)
		-b BAUD,	Set baud rate (default: 500000)
		-B BACKEND,	FTP server backend [0 = RAM, 1 = FAT, 2 = VFS]
		-c,			Set /boot/boot.conf to boot uploaded image
		-d DEVICE,	Set device (default: /dev/ttyUSB0)
		-e,			Exit immediately after up or download
		-f IMAGE,	Upload image. If no other option is used, the image is uploaded to RAM
		-g PATH,	Download file from remote path PATH
		-h,			Print help and exit
		-l PATH,	FTP list path
		-n NODE,	FTP server address
		-p PORT,	FTP server port
		-r,			Copy uploaded image from RAM to ROM. Requires -f and -B0
		-R,			Reboots the device before uploading image
		-s SIZE,	Set FTP chunk size to SIZE bytes
		-u PATH,	Upload file to remote path PATH. Requires -f

FTP-Client supports both uncompressed images and images compressed with the LZO compression algorithm. A tool for compressing images is available on www.lzop.org. To compress a software image, simply execute:
		
		lzop --best nanomind.bin

This creates the file nanomind.bin.lzo, which you can upload in a similar way as shown below.

### Upload To RAM:

1. Connect the NanoMind computer to your Linux PC using the included USB to serial converter.
2. Open a 'minicom' terminal, if not install use 'apt-get install minicom'
3. Test the connection using minicom or any other terminal application using 500000 baud 8N1. Exit minicom again.
4. Ensure that the nanomind terminal is situated on '/dev/ttyUSB0', otherwise edit the wscript and find the ftp-client   command and insert the '-d /dev/ttyUSBX' argument were you replace accordingly.
5. Go to the root directory of nanomind and execute:

		./waf upload

6. Observe the output of the programming of the NanoMind OBC. NOTE: The ftp-client application both echoes the output of the PC and the NanoCom computer in the same window.
7. After the NanoMind computer is booted, ftp-client waits a short period before exiting. This is useful to see the boot message from the OBC. FTP-client will stay open for a bit so you can inspect the boot message. If you want to exit quicker, type <ctrl><c>
8. If you wish to access the serial console of the OBC, open minicom again 500000 baud 8N1

Using the Eclipse IDE
---------------------

### Generate eclipse project files
1. Install/copy the eclipse.py tool in your source/tools/ folder, the latest file can be found at: https://github.com/GomSpace/support/blob/master/eclipse.py
2. Change directory to your source dir source/nanomind, source/csp-term or source/ftp-client
3. Auto-generate eclipse project files by running './waf configure eclipse' where you remember to add the correct options. For nanomind you may add the word eclipse as the last line on the ./configure-a712.sh script for example.

### Install Eclipse
1. Extract 'eclipse<version>.tar.bz2' somewhere in your home folder, or use your own eclipse installation, with the C/C++ plugins.
2. Open the folder and execute the 'eclipse' binary

### Import projects:
1. Ensure you have an empty eclipse workspace open
2. Open 'file' -> 'import'
3. Select 'general' -> 'Existing projects into workspace'
4. Choose 'select archive file'
5. Navigate to and find 'NanoMind-<version>.tar.bz2'
6. Select all projects and click finish.

Depending on your eclipse settings, eclipse will automatically build the different projects for you.
Please note that eclipse is only a frontend to the 'waf' build-tool, and <CTRL+B> still only calls 'waf build', so you'll need to configure the source first.

