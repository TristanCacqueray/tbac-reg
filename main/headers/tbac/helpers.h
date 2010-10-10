/**************************************/
/* Timed Based Access Control         */
/* Author and (c) 2009-2010: GCM SARL */
/* headers/tbac/helpers.h             */
/* Last modified: 11/Jun/2009         */
/**************************************/

#ifndef __TBAC_HELPERS_H
#define __TBAC_HELPERS_H

#include <rsbac/types.h>
#ifdef __KERNEL__
#include <rsbac/debug.h>
#else
#include <time.h>
#include <string.h>
#include <stdio.h>
#endif

/* debug */
#ifdef __KERNEL__
#include <rsbac/debug.h>
#define tbac_printk(code, fmt, arg...)					\
do {									\
	rsbac_printk(KERN_##code "RSBAC REG TBAC: %s():%d:\t" fmt "\n",	\
		__FUNCTION__, __LINE__, ##arg);				\
} while (0)
#ifdef TBAC_DEBUG
#define tbac_debug(fmt, arg...)	tbac_printk(DEBUG, fmt, ##arg)
#else
#define tbac_debug(fmt, arg...)	do {} while (0)
#endif
#else
#ifdef TBAC_DEBUG
#define tbac_debug(fmt, arg...)						\
do {									\
	printf("RSBAC TBAC: %s():%d:\t" fmt "\n",			\
		__FUNCTION__, __LINE__, ##arg);				\
} while (0)
#else
#define tbac_debug(fmt, arg...)	do {} while (0)
#endif
#endif

char *get_tbac_flags_name(char *flags_name, rsbac_tbac_flags_t flags);
rsbac_tbac_flags_t	get_tbac_flags_nr(char *flags_name);

#ifndef __KERNEL__
/* return localtime difference from utc in seconds */
int localtime_offset();

/*
 * format_* take UTC time as parameter
 * return ISO-8601 compatible localized representation
 */
char *format_hour(char *str, time_t hour);
char *format_date(char *str, time_t date);

/*
 * parse_* return UTC time in second since THE_EPOCH
 * return 0 if it can not parse input
 */
time_t parse_hour(char *str);
time_t parse_date(char *str);
#endif

#endif
