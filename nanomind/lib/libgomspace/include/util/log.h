/**
 * Generic log interface with support for:
 * 	LOG_DISABLED
 * 	LOG_PRINTF
 * 	LOG_COLOR
 * 	LOG_CDH
 *
 * Copyright 2011 GomSpace ApS. All rights reserved.
 */

#include <conf_gomspace.h>

#ifdef __AVR__
#undef printf
#undef sscanf
#undef scanf
#undef sprintf
#undef snprintf
#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#define printf(s, ...) printf_P(PSTR(s), ## __VA_ARGS__)
#define sscanf(buf, s, ...) sscanf_P(buf, PSTR(s), ## __VA_ARGS__)
#define scanf(s, ...) scanf_P(PSTR(s), ## __VA_ARGS__)
#define sprintf(buf, s, ...) sprintf_P(buf, PSTR(s), ## __VA_ARGS__)
#define snprintf(buf, size, s, ...) snprintf_P(buf, size, PSTR(s), ## __VA_ARGS__)
#else
#include <stdio.h>
#endif

#if defined(LOG_DISABLE)

#define log_debug(event, format, ...) { do { } while(0); }
#define log_info(event, format, ...) { do { } while(0); }
#define log_warning(event, format, ...) { do { } while(0); }
#define log_error(event, format, ...) { do { } while(0); }

#elif defined(LOG_PRINTF)

#define log_debug(event, format, ...) { do { printf("%s: ", event); printf(format, ##__VA_ARGS__); printf("\r\n"); } while(0); }
#define log_info(event, format, ...) { do { printf("%s: ", event); printf(format, ##__VA_ARGS__); printf("\r\n"); } while(0); }
#define log_warning(event, format, ...) { do { printf("%s: ", event); printf(format, ##__VA_ARGS__); printf("\r\n"); } while(0); }
#define log_error(event, format, ...) { do { printf("%s: ", event); printf(format, ##__VA_ARGS__); printf("\r\n"); } while(0); }

#elif defined(LOG_COLOR)

#include <util/color_printf.h>
#define log_debug(event, format, ...) { do { color_printf(COLOR_BLUE, "%s: ", event); color_printf(COLOR_BLUE, format, ##__VA_ARGS__); printf("\r\n"); } while(0); }
#define log_info(event, format, ...) { do { color_printf(COLOR_GREEN, "%s: ", event); color_printf(COLOR_GREEN, format, ##__VA_ARGS__); printf("\r\n"); } while(0); }
#define log_warning(event, format, ...) { do { color_printf(COLOR_YELLOW, "%s: ", event); color_printf(COLOR_YELLOW, format, ##__VA_ARGS__); printf("\r\n"); } while(0); }
#define log_error(event, format, ...) { do { color_printf(COLOR_RED, "%s: ", event); color_printf(COLOR_RED, format, ##__VA_ARGS__); printf("\r\n"); } while(0); }

#elif defined(LOG_CDH)
#include <log/log.h>
#endif
