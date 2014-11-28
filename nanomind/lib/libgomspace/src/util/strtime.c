/*
 * strtime.c
 *
 *  Created on: 22-08-2009
 *      Author: Administrator
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define isLeapYear(x) (((x % 4) == 0) && (((x % 100) != 0) || ((x % 400) == 0)))

int strtime(char *str, int64_t utime, int year_base) {
	char sign = ' ';
	if(utime < 0) {
		sign = '-';
		utime = -utime;
	}
	int days[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	int daysl[12] = {31,29,31,30,31,30,31,31,30,31,30,31};
	int *daysp;
	int year = year_base;
	int day = 1;
	int mon = 1;
	int hour = 0;
	int min = 0;
	int sec = 365*24*60*60;
	int tmp;
	int ms = utime%1000;
	int s = (int)(utime/1000);

	// Count years
	tmp = isLeapYear(year) ? 366 : 365;
	sec = tmp*24*60*60;
	// Keep counting years for as long as there are more seconds left
	// in the pool than there seconds in the next coming year.
	while(s > sec) {
		s -= sec;
		year ++;
		tmp = isLeapYear(year) ? 366 : 365;
		sec = tmp*24*60*60;
	}
	sec = 31*24*60*60;
	daysp = isLeapYear(year) ? daysl : days;
	// Count months
	while(s > sec) {
		mon ++;
		s -= sec;
		sec = daysp[mon-1]*24*60*60;
	}
	day = s/(24*60*60);
	s -= day*24*60*60;
	day ++;
	hour = s/(60*60);
	s -= hour*60*60;
	min = s/60;
	s -= min*60;
	// If year base is the Unix Epoch Year then we have an absolute time/date
	if(year_base == 1970) {
		sprintf(str,"%02d-%02d-%4d %02d:%02d:%02d.%03d",day,mon,year,hour,min,s,ms);
	}
	// Otherwise, we just count the years, months, days, hours, minutes, seconds....
	else {
		mon--;
		day--;
		if(year > 0)
			sprintf(str,"%c%d years %d months %d days %d hours %d minutes %d seconds %d milliseconds",sign,year,mon,day,hour,min,s,ms);
		else if(mon > 0)
			sprintf(str,"%c%d months %d days %d hours %d minutes %d seconds %d milliseconds",sign,mon,day,hour,min,s,ms);
		else if(day > 0)
			sprintf(str,"%c%d days %d hours %d minutes %d seconds %d milliseconds",sign,day,hour,min,s,ms);
		else if(hour > 0)
			sprintf(str,"%c%d hours %d minutes %d seconds %d milliseconds",sign,hour,min,s,ms);
		else if(min > 0)
			sprintf(str,"%c%d minutes %d seconds %d milliseconds",sign,min,s,ms);
		else
			sprintf(str,"%c%d seconds %d milliseconds",sign,s,ms);
	}
	return strlen(str);
}
