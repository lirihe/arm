/*
 *  console.h
 *
 *
 *  Created by Karl Kaas Laursen on 8/13/09.
 *  Copyright 2009 GomSpace ApS. All rights reserved.
 *
 */

#ifndef CONSOLE_H
#define CONSOLE_H

#include <conf_gomspace.h>
#include <command/command.h>

/* Number of history elements */
#ifndef CONSOLE_HISTORY_ELEMENTS
#define CONSOLE_HISTORY_ELEMENTS	10
#endif

/* Size of console line buffer */
#ifndef CONSOLE_BUFSIZ
#define CONSOLE_BUFSIZ 				100
#endif

/**
 * Initialize the debugger console thread
 */
int console_init(void);

/**
 * Exit and restore terminal settings
 */
int console_exit(void);

/**
 * Set console hostname
 */
void console_set_hostname(char *host);

/**
 * Clear the console screen
 */
void console_clear(void);

#ifndef __linux__
void debug_console(void *pvParameters);
#else
void *debug_console(void *parameters);
#endif

#endif
