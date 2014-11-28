GOSH - Gomspace Shell
=====================

One of the first things encountered when booting the NanoMind computer is the gomspace shell (GOSH) found on the serial port. This provides a simple but extensive debug interface to the NanoMind computer. At the time of writing GOSH is running on NanoMind, NanoCom, NanoCam, NanoHub, TNC1 and CSP-Term. The gomspace shell includes a comprehensive console task and an extendable command parser with autocompletion and usage information.

Console
-------

The console provides a text-interface to a given input/output stream such as a serial port. This provides a mean of typing a command in human readable text format, and passing this command to the command parser. The console uses a traditional keyboard shortcut layout for navigating history, line editing and includes command name tab completion and command usage information. Any user who have tried a text-interface before should feel right at home.

The console task is written for both Linux and FreeRTOS and uses the `getchar` and `usart_getc` functions respectively, to get input from the user's console. The console assumes a standard VT102 terminal emulator, but features a few fixes for the quirks that `minicom` has.

The console will greet the user with a hostname and promt. The hostname is set in the `console_set_hostname` function and the promt is hardcoded in `console.h` as `"\033[1;30m # \033[0m"`. This special syntax gives a colored promt for the user. Here is an example promt:

		Welcome to Gomspace Shell
		csp-term # 

The console will react to user input and commit to the `command_run` whenver <enter> is pushed and commit to `command_complete` when <tab> is pushed. This gives the same behaviour as for example a unix shell where entire commands are written as strings and comitted when enter is pushed. A list of key inputs are understood such as <left>, <right> to change the cursor position, and <up> and <down> to go through the command history. The complete list of command shortcuts are:

		<ctrl><a> - go to beginning of line
		<ctrl><b> or <left> - go back a char
		<ctrl><d> - delete char to the right of cursor
		<ctrl><e> - go to end of line
		<ctrl><f> or <right>- go forward a char
		<ctrl><k> - kill rest of line line
		<ctrl><l> - clear
		<ctrl><n> or <up> - next in history
		<ctrl><p> or <dn> - prev in history
		<ctrl><t> - transpose chars
		<ctrl><u> - kill line from beginning
		<ctrl><h> - backspace
		<backspace> - backspace
		<enter> - run
		<tab> - complete

Command Parser
--------------

The command parser splits the typed command into tokens based on the space delimter. It searches for the first word in the list of root-commands. The list of root-commands can be displayed by the `help` command or pressing <tab> on an empty propmt. Here is a typical list of root-commands for `CSP-Term` running on Linux:

		Welcome to Gomspace Shell
		csp-term # help
		  help                Show help
		  sleep               Sleep X ms
		  watch               Run cmd at intervals, abort with key
		  exit                Exit program
		  ping                Send CSP ping
		  rps                 Remote ps
		  memfree             Memory free
		  buffree             Buffers
		  reboot              a subsystem
		  uptime              of subsystem
		  cmp                 CSP management protocol
		  route               Show routing table
		  ifc                 Show interfaces
		  conn                Show connection table
		  debug               Toggle CSP debug levels ON/OFF
		  rdpopt              Set RDP options
		  obc                 OBC subsystem
		  eps                 EPS subsystem
		  com                 COM subsystem
		  cam                 CAM subsystem
		  hub                 HUB subsystem
		  ftp                 FTP commands
		  gatoss              GATOSS subsystem
		  cdh                 CDH/OBC subsystem

The list includes CSP network utilities, general utilities and subsystem commands. Some of the root-commands have sub-commands. A sub command is the next following word on the command line, so for example in:

		eps hk

The word `hk` is the sub command to the `eps` root command. It's possible to make an arbitrary number of sub-commands. In order to get the list of sub commands for a given root command, type the sub command and <enter> or <tab>. For the EPS the list of sub commands are for example:

		csp-term # eps
		  node                Set EPS address in OBC host table
		  hk                  Get HK
		  hk2                 Get HK2
		  ver                 Get version
		  o                   Set on/off, argument hex value of output char
		  so                  Set channel on/off, argument (channel) to (1 or 0)
		  v                   Set pvolt, arguments hex pv1, pv2, pv3 (mV)
		  ppt                 Set PPT mode. off = 0, auto = 1, fixed = 2
		  heater              Set heater auto mode. off = 0, auto = 1
		  persistentreset     Set persistent variables = 0
		  gndwdtreset         Resets the ground com WDT
		  bootdelay           Set/Get boot delay for channels: x x x x x x [s]
		csp-term # eps

Each command that does not have a sub-command, wether its a root command or not, has an associated handler. This is a pointer to the function that will be called with the arguments. It also has a help text and a usage text. The help text is shown in the help output as shown above, and the usage text is shown when <tab> is pressed or when an invliad number of arguments are given to a command:

		csp-term # eps so
		usage: so <channel> <mode> <delay>

Defining new commands
---------------------

In order to define a new root command, define a command structure in one of your c-files like this:

		command_t __root_command eps_commands[] = {
			{
				.name = "eps",
				.help = "EPS subsystem",
				.chain = INIT_CHAIN(eps_subcommands),
			}
		};

Remember to include `<command/command.h>` to get the `command_t` declaration. In this example the root-command `"eps"` points to a sub-command using the `INIT_CHAIN()` macro, which does a compile-time count of the number of sub-commands and inserts in the root-command, so the command-parser knows the size of the array.

		command_t __sub_command eps_subcommands[] = {
			{
				.name = "hk",
				.help = "Get HK",
				.handler = eps_hk,
			},{
				.name = "so",
				.help = "Set channel on/off, argument (channel) to (1 or 0)",
				.usage = "<channel> <mode> <delay>",
				.handler = eps_single_output,
			},
		};

Here the example shows a two-subcommands from the examples above. It shows that calling `eps hk` will run the `eps_hk()` handler function. This command does not have any arguments, so there is no usage information associated. The `so`, command does have three arguments, so the usage info is specified. The array can be further extended with any number of sub commands.

Linker-optimizations
--------------------

Instead of building the list of commands run-time using a linked list or similar datastructure, the root-commands are built by the linker using a special GCC-attribute to pack the command structs into the same memory area. In other words, the entire command list is always initialised as default and therefore requires zero time to initialize during startup. It's therefore very important that all root-commands are tagged with the `__root_command` macro, and all sub-commands with the `__sub_command`. On X86 where these link time optimizations are impossible, the list of commands a built using a linked list and malloc.

Implementing a handler function
-------------------------------

The handler function needs to follow the following prototype:

		typedef int (*command_handler_t)(struct command_context * context);

An example of this is the `eps_single_output` handler:

		int eps_single_output(struct command_context *ctx) {

			char * args = command_args(ctx);
			unsigned int channel;
			unsigned int mode;
			int delay;
			printf("Input channel, mode (0=off, 1=on), and delay\r\n");
			if (sscanf(args, "%u %u %d", &channel,&mode,&delay) != 3)
				return CMD_ERROR_SYNTAX;
			printf("Channel %d is set to %d with delay %d\r\n", channel,mode,delay);

			eps_set_single_output((uint8_t) channel, (uint8_t) mode, (int16_t) delay);

			return CMD_ERROR_NONE;

		}

The functions can use the `command_args()` function to get the argument string and parse it using for example scanf. The alternative and preferred method is to use argc/argv syntax like this:


		int my_cmd(struct command_context * ctx) {

			if (ctx->argc != 2)
				return CMD_ERROR_SYNTAX;

			uint8_t my_arg1 = atoi(ctx->argv[1]);

			return CMD_ERROR_NONE;

		}

Remember to return with `CMD_ERROR_NONE` if the handler executed correctly, `CMD_ERROR_SYNTAX` if the arguments could not be parsed, or any of the other error codes found in `<command/command.h>`.

