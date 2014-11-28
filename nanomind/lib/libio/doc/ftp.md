File Transfer Protocol
======================

The file transfer functionality is built on top of CSP and RDP as a transport layer protocol. This ensures flow control and correct ordering of messages. FTP uses a simple chunk based protocol with a bitmap file to mark which cunks has been transferred. The protocol is used by the NanoMind OBC to uplaod new software images to the /boot/ partition or directly to RAM for rapid testing of code.

The implementation in `libio` includes both a client, a server and three backends. One for RAM, and one for each filesystem; FAT and UFFS.

Server
------

Before the server can be used, the right backend needs to be registered using the following function:

```c
#include <ftp/ftp_server.h>
init() {
	ftp_register_backend(BACKEND_FAT, &backend_fat);
	ftp_register_backend(BACKEND_UFFS, &backend_uffs);
	ftp_register_backend(BACKEND_UFFS, &backend_uffs);
}
```

On the NanoMind the server task takes care of spawning a new `task_ftp` whenever a new connection is opened on the `OBC_PORT_FTP`. This avoids the need for a separate task to be running all the time. The task spawning is done like this:

```c
/* Spawn new task for SW upload */
if (csp_conn_dport(conn) == OBC_PORT_FTP) {
        extern void task_ftp(void * conn_param);
        if (xTaskCreate(task_ftp, (signed char *) "FTP", 1024*4, conn, 2, NULL) != pdTRUE) {
                printf("Failed to allocate memory for RAM upload task\r\n");
                csp_close(conn);
        }
        continue;
}
```

The thing to notice here is that the connection pointer is passed to the task in the task arguments. The ftp server task will automatically close the connection when it is not needed any longer.

Client
------

The client API is found in `#include <ftp/ftp_client.h>` and can be used to implement the file transfer in a C-application. The client is only designed to be used on a linux PC, and therefore can not run on-board a satellite subsystem. So far FTP has been built into CSP-Term as client commands, but is also available in the stand-alone FTP-client application.

### CSP-Term ###

To see the list of commands from CSP-term, type `ftp <tab>`:

```
csp-term # ftp
'ftp' contains sub-commands:
  done                done transfer
  ls                  list files
  rm                  rm files
  mv                  move files
  crc                 crc transfer
  download_run        Continue download (send status reply)
  server              set host and port
  download_file       Download file
  download_mem        Download memory
```

In order to download a file first call `ftp server <node> <port>` to adjust which server to connect to. Then run the `ftp ls /sd/` command to get a list of files. Finally do a `ftp download_file /sd/<filename>.<ext>` to start the file transfer.

The CSP-Term client does not support upload.

### FTP-Client ###

The FTP-Client is a standalone application used mainly to upload software to subsystems. The list of command-line arguments to the program can be seen below:

```
johan@ubuntu:~/git/csp-term$ ftp-client -h
  -a ADDRESS,	Set address (default: 8)
  -b BAUD,	Set baud rate (default: 500000)
  -B BACKEND,	FTP server backend [1 = FAT]
  -c,		Set /boot/boot.conf to boot uploaded image
  -d DEVICE,	Set device (default: /dev/ttyUSB0)
  -e,		Exit immediately after up or download
  -f IMAGE,	Upload image. If no other option is used, the image is uploaded to RAM
  -g PATH,	Download file from remote path PATH
  -h,		Print help and exit
  -l PATH,	FTP list path
  -n NODE,	FTP server address
  -p PORT,	FTP server port
  -r,		Copy uploaded image from RAM to ROM. Requires -f
  -R,		Reboots the device before uploading image
  -s SIZE,	Set FTP chunk size to SIZE bytes
  -u PATH,	Upload file to remote path PATH. Requires -f
```

More info on the usage of this tool for software upload is in the INSTALL file of the NanoMind repository.

