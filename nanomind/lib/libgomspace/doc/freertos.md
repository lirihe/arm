Preemptive Real Time Operating System
=====================================

All gomspace products run on the FreeRTOS operating system [FreeRTOS.org](http://freertos.org). FreeRTOS is a preemptive real-time operating system which real time scheduling functionality, inter-task communication, timing and synchronisation primitives. This means it is more accurately described as a real time kernel, or real time executive. Additional functionality, such as a command console interface, or networking stacks, can be then be included with add-on components. 

Why use a RTOS?
---------------

You do not need to use an RTOS to write good embedded software. At some point though, as your application grows in size or complexity, the services of an RTOS might become beneficial for one or more of the reasons listed below. These are not absolutes, but opinion. As with everything else, selecting the right tools for the job in hand is an important first step in any project.

* Abstract out timing information: The real time scheduler is effectively a piece of code that allows you to specify the timing characteristics of your application - permitting greatly simplified, smaller (and therefore easier to understand) application code.
* Maintainability/Extensibility: Not having the timing information within your code allows for greater maintainability and extensibility as there will be fewer interdependencies between your software modules. Changing one module should not effect the temporal behaviour of another module (depending on the prioritisation of your tasks). The software will also be less susceptible to changes in the hardware. For example, code can be written such that it is temporally unaffected by a change in the processor frequency (within reasonable limits).
* Modularity: Organising your application as a set of autonomous tasks permits more effective modularity. Tasks should be loosely coupled and functionally cohesive units that within themselves execute in a sequential manner. For example, there will be no need to break functions up into mini state machines to prevent them taking too long to execute to completion.
* Cleaner interfaces: Well defined inter task communication interfaces facilitates design and team development.
* Easier testing (in some cases): Task interfaces can be exercised without the need to add instrumentation that may have changed the behaviour of the module under test.
* Code reuse: Greater modularity and less module interdependencies facilitates code reuse across projects. The tasks themselves facilitate code reuse within a project. For an example of the latter, consider an application that receives connections from a TCP/IP stack - the same task code can be spawned to handle each connection - one task per connection.
* Improved efficiency (no polling): Using FreeRTOS permits a task to block on events - be they temporal or external to the system. This means that no time is wasted polling or checking timers when there are actually no events that require processing. This can result in huge savings in processor utilisation. Code only executes when it needs to. Counter to that however is the need to run the RTOS tick and the time taken to switch between tasks. Whether the saving outweighs the overhead or visa versa is dependent of the application. Most applications will run some form of tick anyway, so making use of this with a tick hook function removes any additional overhead.
* Idle time: It is easy to measure the processor loading when using FreeRTOS.org. Whenever the idle task is running you know that the processor has nothing else to do. The idle task also provides a very simple and automatic method of placing the processor into a low power mode.
* Flexible interrupt handling: Deferring the processing triggered by an interrupt to the task level permits the interrupt handler itself to be very short - and for interrupts to remain enabled while the task level processing completes. Also, processing at the task level permits flexible prioritisation - more so than might be achievable by using the priority assigned to each peripheral by the hardware itself (depending on the architecture being used).
* Mixed processing requirements: Simple design patterns can be used to achieve a mix of periodic, continuous and event driven processing within your application. In addition, hard and soft real time requirements can be met though the use of interrupt and task prioritisation.
* Easier control over peripherals: Gatekeeper tasks facilitate serialisation of access to peripherals - and provide a good mutual exclusion mechanism. 

Using the FreeRTOS API
----------------------

The api is located in the `include/freertos/` folder of the libgomspace library. In order to use the task functionality you must include the `task.h` file in the follwing matter:

		#include <freertos/FreeRTOS.h>
		#include <freertos/task.h>

It's important that the `FreeRTOS.h` header is included before the `task.h` header

A note about interrupt safe API calls
-------------------------------------

Please note that it is imperative for the stability of the kernel that all calls to FreeRTOS within interrupt context have the special `_from_isr` ending. For many functions there are a thread-safe version, that can be called in task context, and an ISR safe version to be used in interrupt context. For example:

		xTaskGetTickCount();
		xTaskGetTickCountFromISR();

These functions are very different in the implementation. The first function will enter a critical region while fetching the 32-bit system tick counter. It does so by disabling interrupts in this period. When executing within the ISR context there are no nested interrupts which can endanger the 32-bit value read-out, so the critical region has been skipped.

It's a requirement to the programmer to know exactly in which context the application is executing at all times. Furthermore it's recommended to defer data processing from ISR to task context. This can be done by receiving data in ISR and putting it into a queue, using the `xQueueSendFromISR()` function, and then having a processing task block on the queue with the `xQueueRead()` function.

For more information on FreeRTOS, please read the online documentation.

