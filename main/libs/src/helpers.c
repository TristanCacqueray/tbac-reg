/**************************************/
/* Timed Based Access Control         */
/* Author and (c) 2009-2010: GCM SARL */
/* libs/src/helpers.c                 */
/* Last modified: 11/Jun/2009         */
/**************************************/

#include <tbac/types.h>
#include <tbac/helpers.h>

// could not use _XOPEN_SOURCE to trig this declaration from time.h
extern char *strptime(const char *s, const char *format, struct tm *tm);

typedef struct	tbac_flags_name_s {
	rsbac_tbac_flags_t	flags;
	char *			name;
}		tbac_flags_name_t;

static tbac_flags_name_t flags_names[] = {
	{ TBAC_unset, "UNSET" },
	{ TBAC_workinghours, "WORKING_HOURS" },
	{ TBAC_sparehours, "SPARE_HOURS" },
	{ TBAC_since, "SINCE" },
	{ TBAC_range, "RANGE" },
	{ -1, NULL },
};

char *get_tbac_flags_name(char *flags_name, rsbac_tbac_flags_t flags)
{
	int idx;

	if (flags_name == NULL)
		return NULL;

	for (idx = 0; flags_names[idx].name; idx++) {
		if (flags_names[idx].flags == flags)
			return strcpy(flags_name, flags_names[idx].name);
	}
	return strcpy(flags_name, "UNKNOWN!");
}

rsbac_tbac_flags_t get_tbac_flags_nr(char *flags_name)
{
	int idx;

	if (flags_name == NULL)
		return -1;

	for (idx = 0; flags_names[idx].name; idx++) {
		if (!strcmp(flags_names[idx].name, flags_name))
			return flags_names[idx].flags;
	}
	return -1;
}

#ifndef __KERNEL__
int localtime_offset()
{
	time_t result;
	time_t now;
	struct tm tms;

	// get timezone
	tzset();
	result = -timezone;

	// day saving time adjustment
	now = time(NULL);
	localtime_r(&now, &tms);
	if (tms.tm_isdst > 0)
		result += 3600;
	return result;
}

char *format_hour(char *str, time_t time)
{
	time_t localtime;
	struct tm tms;

	if (str == NULL)
		return NULL;

	if (time == 0)
		return strcpy(str, "undef");
	localtime = time + localtime_offset();
	gmtime_r(&localtime, &tms);
	strftime(str, RSBAC_MAXNAMELEN, "%H:%M", &tms);
	return str;
}

char *format_date(char *str, time_t time)
{
	time_t localtime;
	struct tm tms;

	if (str == NULL)
		return NULL;

	if (time == 0)
		return strcpy(str, "undef");
	localtime = time + localtime_offset();
	gmtime_r(&localtime, &tms);
	strftime(str, RSBAC_MAXNAMELEN, "%Y-%m-%d %H:%M:%S", &tms);
	return str;
}

time_t parse_hour(char *str)
{
	time_t result;
	struct tm tm;

	if (str == NULL)
		return 0;

	if (strptime(str, "%H:%M", &tm) == NULL)
		return 0;
	result = tm.tm_hour * 3600 + tm.tm_min * 60 - localtime_offset();
	return result;
}

time_t parse_date(char *str)
{
	time_t result;
	struct tm tm;

	if (str == NULL)
		return 0;

	if (strptime(str, "%Y-%m-%d %H:%M:%S", &tm) == NULL &&
		strptime(str, "%Y-%m-%d", &tm) == NULL)
		return 0;
	result = mktime(&tm);
	if (result == -1)
		return 0;
	return result;
}
#endif
