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

#ifndef _DS1302_H_
#define _DS1302_H_

int ds1302_clock_halt(void);
int ds1302_clock_resume(void);
uint8_t ds1302_get_seconds(void);
int ds1302_set_seconds(uint8_t seconds);
uint8_t ds1302_get_minutes(void);
int ds1302_set_minutes(uint8_t minutes);
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
int ds1302_write_ram(uint8_t address, uint8_t *data, int datalen);
int ds1302_read_ram(uint8_t address, uint8_t *data, int datalen);
int ds1302_init(void);

#endif // _DS1302_H_
