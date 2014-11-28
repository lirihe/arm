Log and Debug
=============

The gomspace library has several ways of logging or outputting debug information on the Serial port of a device. This is a short list of the different methods used within gomsapce products:

printf
------

This is the most basic method of outputting data to the serial port. It calls a c-library function in order to format the output string and then calls `putchar` for each of the chars of the string. In order for this to work, the `usart_init` function must be called prior to calling printf. On some platforms the output may be buffered, which means that a call to printf before the task context has been started (i.e. in the `main.c` file) whould be illegal. Furthermore, using printf inside of ISR functions are very time consuming, and may block the processor long enough to prevent things from working. Additionally, printf uses quite a lot of stack for formatting the string, so any printf containing arguments like "my number %u\\r\\n" should be used catioulsy in ISR's and low-stack tasks.

log\_debug
----------

The general log system is declared in the `util/log.h` file, and is the preferred method to display debug output to the user. It consists of four macros, `log_debug`, `log_info`, `log_error` and `log_critical` that each can do various things depending on the following compile time directives:

- `LOG_DISABLE` This will completely disable all log macros and ensure they are removed from the codebase and optimized away.
- `LOG_PRINTF` This will make all the functions a wrapper around printf
- `LOG_COLOR` This is the same as printf, with the addition of a blue color for debug, green for info, yellow for error and red for critical
- `LOG_CDH` This option will redirect all the calls to the CDH logd client, which is a distributed log service which can be configured to post all log messages as events to the logd server on the satellite bus. This is further described in the CDH documentation.
		
csp\_debug
----------

CSP debug is briefly mentioned here, and is essentially the same as log debug with the log_color option and seven different log-levels that can be enabled and disabled run-time. The functions are private to libCSP and should not be used by any other library or module. In order to activate the output from the CSP library the GOSH command line function `debug` is available. The syntax is

		debug <enable/disable> <all|error|warn|info|buffer|packet|protocol|lock>

Note that this may affect the performance and timings of the microprocessor so use with caution.

driver\_debug
-------------

The final debugging option is used by the driver library. It is a run-time switchable debug output, similar to csp_debug, but designed for drivers and low-level modules where debug should normally be disabled. At start-up all drivers debug is disabled, but can be enabled using the following command

		tdebug <level>

Where the levels are defined in `driver_debug.h` and subject to change. The level is a number only. For example to enable I2C driver debugging use: `tdebug 0`. This will then do a printout each time the microprocessor is interrupted by the I2C hardware so the state machine can be monitored. It should be needless to say that enabling a printout within the ISR is going to affect the response time of the I2C ISR immensiely, and should be used with caution as well.

