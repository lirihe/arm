Driver library
==============

The gomspace driver library covers the most basic functionality of UART, I2C, SPI, CPU and WDT on all gomspace platforms based on AVR8, AVR32, ARM and to some extend GNU Linux. The drivers use the same API for all the platforms, so in that respect the driver library is also a hardware abstraction layer that goes across several pieces of hardware. This enables device drivers for sensors and storage devices to be cross platform compatible. In order to use the drivers, use the top level include file of the `dev` directory like this:

		#include <dev/usart.h>

Be sure top open up the include file and read the function documentation and take special care to call the driver's init function, if present, before using any of the included functions.

The main difference on the standard atmel driver library and the gomspace library is the use of the RX/TX queues, interrupts and DMA. All GomSpace drivers use non-blocking transmissions with buffered outputs and interrupt based transmission. So tasks will never have to sit and wait for a transmission to complete, unless the output buffer of the driver is full there will be a timeout on the transmission or the message is lost. Since all data reception is interrupt based, the application does not have to poll hardware for updated data. The use of Queus and Semaphores will automaticly wake up the task when its doing a blocking read on any driver.

This design philosophy makes multiprogramming easier and separates the hardware access from the task itself with a good amount of buffer in each direction allowing for more flexible processor use and power savings in the idle task.

Gomspace uses the Newlib C-library, which is an almost complete set of functions similar to GNU Lib-C, but specialized for embedded computers. All mandatory system calls from the C-library has been implemented in the driver library. This means that functions such as printf, fopen and others will work as expected.

Specific documentation for each of the drivers can be found in the header files and in the example code using the given peripheral.

Note regarding I2C driver
-------------------------

The I2C driver is typically used by the CSP library on most gomspace products. This means that special care should be taken when using the same I2C bus in direct I2C mode. However, the NanoMind OBC has two I2C busses, of which CSP is used on the satellite bus and Master/slave mode is used on the secondary bus. The seconday bus is routed to the magnetometer, but is also availabe in a pin-header for use with external I2C sensors.

Note regarding SPI driver
-------------------------

Since the SPI protocol is so simple, the driver requires external locking or another mutual exclusion technique to ensure that two SPI transfers does not interfere with eachother. This should especially be taken into consideration when using SD-card or memory over SPI from one task, while at the same time, polling sensors from another task. This is because some devices will reset its state if the chip select is de-asserted during a communication sequence.

