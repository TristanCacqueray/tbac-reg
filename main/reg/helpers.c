/**************************************/
/* Timed Based Access Control         */
/* Author and (c) 2009-2010: GCM SARL */
/* reg/helpers.c                      */
/* Last modified: 11/Jun/2009         */
/**************************************/

#include <tbac/types.h>
#include <tbac/helpers.h>

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
