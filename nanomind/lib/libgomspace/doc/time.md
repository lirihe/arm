Common time functions
=====================

Absolute time is commonly described using the standard posix seconds since epoch timestamp format in UTC. The standard c-library (newlib) includes functions to get or set the system time on most platforms. This is done using the system supplied `timespec_t` type. However this is not portable and not the same on Linux glibc, newlibc or avr-libc. Therefore gomspace uses a common portable timestamp format called a `timestamp_t`:

		typedef struct __attribute__((packed)) {
			uint32_t tv_sec;
			uint32_t tv_nsec;
		} timestamp_t;

This consists of seconds and nanoseconds since epoch represented as two portable integer types. This type is included in the `timestamp.h` file which also includes a couple of helper functions to add two timestamps, compare or copy.

System clock
------------

Whenever a system boots its clock will be reset to zero and start ticking an increment at the rate of the FreeRTOS kernel tick. This means that if you try to get a timestamp from the system it would read out a very low amount of seconds since epoch and result in a timestamp being around 1. jan 1970. In order to adjust the time to the correct absolute time, the functions in 'clock.h' is provided:

		void clock_get_time(timestamp_t *);
		void clock_set_time(timestamp_t *);
		void clock_get_monotonic(timestamp_t * time);

The monotonic clock equates to the system tick count since boot. This clock cannot be set and is useful for relative time comparison. The absolute timestamp (UTC time) is returned byt the `clock_get_time` function and can be set with the `clock_set_time` function.

Real-Time-Clock
---------------

The NanoMind OBC version C and above features an on-board optional real-time-clock. This is a little external ultra-low-power chip, that can accurately keep track of the time using it's own 32.768 kHz crystal. It's features a small charging system with a diode that charges a big tantalium capacitor with enough capacity to run the RTC clock running up to 1 hour after powering off the NanoMind. This means that even for longer phases of power-off (eclipse) the NanoMind will still remember it's absolute timestamp. During bootup if the OBC the timestamp is read from the RTC and the `clock_set_time` function is called to restore the timestamp offset. From here and on, the timestamp is updated only by the tick counter of the operating system. If the NanoMind time drifts away from the RTC it can only be restored by rebooting. This is to ensure that all events on the microprocessor happens in linear increasing time. It is better to drift a bit than do a backwards jump in time which can cause the order of data to be corrupted.

NanoMind Time-Sync
------------------

The client API for the NanoMind OBC includes a `obc_timesync` function that can be used to remotely update the time of the NanoMind computer.


