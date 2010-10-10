/**************************************/
/* Timed Based Access Control         */
/* Author and (c) 2009-2010: GCM SARL */
/* libs/src/wrappers.c                */
/* Last modified: 11/Jun/2009         */
/**************************************/

#include <sys/stat.h>
#include <rsbac/types.h>
#include <rsbac/syscalls.h>
#include <tbac/types.h>
#include <tbac/syscalls.h>

int tbac_get_flags(const char * path, rsbac_tbac_flags_t * flags)
{
	struct rsys_tbac_set_flags	arg;
	struct stat			st;
	int				err;

	if (flags == NULL)
		return -RSBAC_EINVALIDPOINTER;

	err = stat(path, &st);
	if (err < 0)
		return err;

	arg.device = MKDEV(major(st.st_dev), minor(st.st_dev));
	arg.inode = (rsbac_inode_nr_t) st.st_ino;
	err = rsbac_reg(TBAC_HDL_get_flags, &arg);
	if (err)
		return err;
	*flags = arg.flags;
	return 0;
}

int tbac_get_range(const char * path, rsbac_tbac_range_t * range)
{
	struct rsys_tbac_get_range	arg;
	struct stat			st;
	int				err;

	if (range == NULL)
		return -RSBAC_EINVALIDPOINTER;

	err = stat(path, &st);
	if (err)
		return err;

	arg.device = MKDEV(major(st.st_dev), minor(st.st_dev));
	arg.inode = st.st_ino;
	err = rsbac_reg(TBAC_HDL_get_range, &arg);
	if (err)
		return err;
	*range = arg.range;
	return 0;
}

int tbac_set_flags(const char * path, const rsbac_tbac_flags_t flags)
{
	struct rsys_tbac_set_flags	arg;
	struct stat			st;
	int				err;

	err = stat(path, &st);
	if (err < 0)
		return err;

	arg.device = MKDEV(major(st.st_dev), minor(st.st_dev));
	arg.inode = st.st_ino;
	arg.flags = flags;
	err = rsbac_reg(TBAC_HDL_set_flags, &arg);
	return err;
}

int tbac_set_range(const char * path, const rsbac_tbac_range_t range)
{
	struct rsys_tbac_set_range	arg;
	struct stat			st;
	int				err;

	err = stat(path, &st);
	if (err < 0)
		return err;

	arg.device = MKDEV(major(st.st_dev), minor(st.st_dev));
	arg.inode = st.st_ino;
	arg.range = range;
	err = rsbac_reg(TBAC_HDL_set_range, &arg);
	return err;
}

int tbac_set_working_hours(const time_t morning, const time_t evening)
{
	struct rsys_tbac_set_working_hours	arg;
	int					err;

	arg.morning = morning;
	arg.evening = evening;
	err = rsbac_reg(TBAC_HDL_set_working_hours, &arg);
	return err;
}

int tbac_get_working_hours(time_t * morning, time_t * evening)
{
	struct rsys_tbac_get_working_hours	arg;
	int					err;

	if (morning == NULL || evening == NULL)
		return -RSBAC_EINVALIDPOINTER;

	err = rsbac_reg(TBAC_HDL_get_working_hours, &arg);
	if (err)
		return err;

	*morning = arg.morning;
	*evening = arg.evening;
	return err;
}

int tbac_set_oneshot_holiday(const time_t holiday)
{
	struct rsys_tbac_set_oneshot_holiday	arg;
	int					err;

	arg.time = holiday;
	err = rsbac_reg(TBAC_HDL_set_oneshot_holiday, &arg);
	return err;
}

int tbac_get_oneshot_holiday(time_t * holiday)
{
	struct rsys_tbac_get_oneshot_holiday	arg;
	int					err;

	if (holiday == NULL)
		return -RSBAC_EINVALIDPOINTER;

	err = rsbac_reg(TBAC_HDL_get_oneshot_holiday, &arg);
	if (err)
		return err;
	*holiday = arg.time;
	return err;
}
