/**
 * @file ds1302.h
 * Bit banging serial driver for the Maxim DS1302 RTC
 *
 * @note Data is transmitted LSB first. Write data is
 * sampled on the rising edge of SCLK. Read data is
 * output on the falling edge of SCLK.
 *
 * @author Jeppe Ledet-Pedersen
 * Copyright 2011 GomSpace ApS. All rights reserved.
 */

#include <stdint.h>

#define DS1302_RAM_BYTES		31
#define DS1302_PIN_SCLK			(1 << 7)	/* PIOA7, pin 80 */
#define DS1302_PIN_IO 			(1 << 8)	/* PIOA8, pin 81 */
#define DS1302_PIN_CE 			(1 << 10)	/* PIOA10, pin 84 */

#define DS1302_CMD_WRITE		(0 << 0)
#define DS1302_CMD_READ			(1 << 0)

#define DS1302_CH_MASK			(0x80)
#define DS1302_12HR_MASK		(0x80)
#define DS1302_PM_MASK			(0x20)
#define DS1302_WP_MASK			(0x80)

#define DS1302_CMD_CLK_SECONDS	(0x80)
#define DS1302_CMD_CLK_MINUTES	(0x82)
#define DS1302_CMD_CLK_HOUR		(0x84)
#define DS1302_CMD_CLK_DATE		(0x86)
#define DS1302_CMD_CLK_MONTH	(0x88)
#define DS1302_CMD_CLK_DAY		(0x8A)
#define DS1302_CMD_CLK_YEAR		(0x8C)
#define DS1302_CMD_CLK_WP		(0x8E)
#define DS1302_CMD_CLK_TRICKLE	(0x90)
#define DS1302_CMD_CLK_BURST	(0xBE)

#define DS1302_CMD_RAM_OFFSET	(0xC0)
#define DS1302_CMD_RAM_BURST	(0xFE)

#define DS1302_TRICKLE_TCS		(0xA0)
#define DS1302_TRICKLE_DS_1		(0x04)
#define DS1302_TRICKLE_DS_2		(0x08)
#define DS1302_TRICKLE_RS_2K	(0x01)
#define DS1302_TRICKLE_RS_4K	(0x02)
#define DS1302_TRICKLE_RS_8K	(0x03)

struct ds1302_clock {
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hour;
	uint8_t date;
	uint8_t month;
	uint8_t day;
	uint8_t year;
	uint8_t wp;
} __attribute__ ((packed));

int ds1302_clock_halt(void);
int ds1302_clock_resume(void);

uint8_t ds1302_get_seconds(void);
int ds1302_set_seconds(uint8_t seconds);

uint8_t ds1302_get_minutes(void);
int ds1302_set_minutes(uint8_t minutes);

uint8_t ds1302_get_hour(void);
int ds1302_set_hour(uint8_t hour);

int ds1302_set_12hr(void);
int ds1302_set_24hr(void);

uint8_t ds1302_get_date(void);
int ds1302_set_date(uint8_t date);

uint8_t ds1302_get_month(void);
int ds1302_set_month(uint8_t month);

uint8_t ds1302_get_day(void);
int ds1302_set_day(uint8_t day);

uint8_t ds1302_get_year(void);
int ds1302_set_year(uint8_t year);

int ds1302_clock_write_burst(struct ds1302_clock *clock);
int ds1302_clock_read_burst(struct ds1302_clock *clock);

int ds1302_write_ram(uint8_t address, uint8_t *data, int datalen);
int ds1302_read_ram(uint8_t address, uint8_t *data, int datalen);

int ds1302_read_ram_burst(uint8_t data[DS1302_RAM_BYTES]);
int ds1302_write_ram_burst(uint8_t data[DS1302_RAM_BYTES]);

int ds1302_init(void);

int ds1302_time_to_clock(time_t *time, struct ds1302_clock *clock);
int ds1302_clock_to_time(time_t *time, struct ds1302_clock *clock);
