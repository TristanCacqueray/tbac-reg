/**************************************/
/* Timed Based Access Control         */
/* Author and (c) 2009-2010: GCM SARL */
/* headers/tbac/types.h               */
/* Last modified: 11/Jun/2009         */
/**************************************/

#ifndef __TBAC_TYPES_H
#define __TBAC_TYPES_H

#include <rsbac/types.h>

#ifndef __KERNEL__
#define MINORBITS	20
#define MKDEV(ma,mi)	(((ma) << MINORBITS) | (mi))
typedef __u32 kdev_t;
#endif

/* fix me (bouchon) */
#define BOUCHON_RDEV_MAJOR	8
#define BOUCHON_RDEV_MINOR	8
extern kdev_t tbac_root_device;

#define UTC1	-1*3600
#define DEFAULT_MORNING_HOUR	(8.5*3600) - UTC1
#define DEFAULT_EVENING_HOUR	(19*3600) - UTC1

/* Attributes */
typedef __u32	rsbac_tbac_flags_t;
#define TBAC_unset		0x0
#define TBAC_workinghours	0x7
#define TBAC_sparehours		0x6
#define TBAC_range		0x5
#define TBAC_since		0x4

typedef struct	rsbac_tbac_range_s {
	time_t			min;
	time_t			max;
}		rsbac_tbac_range_t;

/* Syscalls parameters */
struct		rsys_tbac_set_flags
{
	kdev_t			device;
	rsbac_inode_nr_t	inode;
	rsbac_tbac_flags_t	flags;
};
struct		rsys_tbac_get_flags
{
	kdev_t			device;
	rsbac_inode_nr_t	inode;
	rsbac_tbac_flags_t	flags;
};

struct		rsys_tbac_set_range
{
	kdev_t			device;
	rsbac_inode_nr_t	inode;
	rsbac_tbac_range_t	range;
};
struct		rsys_tbac_get_range
{
	kdev_t			device;
	rsbac_inode_nr_t	inode;
	rsbac_tbac_range_t	range;
};

struct		rsys_tbac_set_working_hours
{
	time_t			morning;
	time_t			evening;
};
struct		rsys_tbac_get_working_hours
{
	time_t			morning;
	time_t			evening;
};

struct		rsys_tbac_set_oneshot_holiday
{
	time_t			time;
};
struct		rsys_tbac_get_oneshot_holiday
{
	time_t			time;
};

#endif
