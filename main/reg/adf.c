/**************************************/
/* Timed Based Access Control         */
/* Author and (c) 2009-2010: GCM SARL */
/* reg/adf.c                          */
/* Last modified: 11/Jun/2009         */
/**************************************/

#include <tbac/types.h>
#include <tbac/helpers.h>

/* tbac genlist access */
#include <tbac/data_structures.h>

#include <tbac/adf.h>

/* this shall be exported by rsbac */
int rsbac_debug_reg = 1;

inline int
  check_tbac(rsbac_tbac_flags_t flags, rsbac_tbac_range_t range)
{
	// current time in second
	struct timespec ts;

	// current second in day
	time_t hour;

	// current day of week, 0 meaning monday
	int day;

	rsbac_tbac_range_t	working_hours;
	time_t oneshot_holiday;

	get_working_hours(&working_hours);
	get_oneshot_holiday(&oneshot_holiday);

	ts = current_kernel_time();
	hour = ts.tv_sec % 86400;
	day = (ts.tv_sec / 86400 + 3) % 7;

	tbac_debug("day = %d, hour = %ld, time = %ld, working_hour = [%ld:%ld], holyday = %ld",
		day, hour, ts.tv_sec, working_hours, oneshot_holiday);

	switch(flags)
	{
		case TBAC_workinghours:
#ifdef TBAC_OPTIM
			if (IN_SPARE_TIME(day, hour, ts.tv_sec))
			    return NOT_GRANTED;
#else
			if (CHECK_WEEKEND(day)) {
				rsbac_pr_debug(reg, "Access to workinghours ressources during weekend, day %d -> NOT_GRANTED!\n", day);
				return NOT_GRANTED;
			}
			if (CHECK_MORNING(hour)) {
				rsbac_pr_debug(reg, "Access to workinghours ressources in the morning (%ld), hour %ld sec -> NOT_GRANTED!\n",
					working_hours.min, hour);
				return NOT_GRANTED;
			}
			if (CHECK_EVENING(hour)) {
				rsbac_pr_debug(reg, "Access to workinghours ressources in the evening (%ld), hour %ld sec -> NOT_GRANTED!\n",
					working_hours.max, hour);
				return NOT_GRANTED;
			}
			if (CHECK_HOLIDAY(ts.tv_sec)) {
				rsbac_pr_debug(reg, "Access to workinghours ressources during holiday (%ld), time %ld sec -> NOT_GRANTED!\n",
					oneshot_holiday, ts.tv_sec);
				return NOT_GRANTED;
			}
#endif
			return GRANTED;
		case TBAC_sparehours:
			if (IN_SPARE_TIME(day, hour, ts.tv_sec))
				return GRANTED;
			rsbac_pr_debug(reg, "Access to sparehours ressource during workinghours, day %d (not weekend), hours %ld (day time), time %ld (not holiday) -> NOT_GRANTED!\n",
				day, hour, ts.tv_sec);
			return NOT_GRANTED;
		case TBAC_range:
			if (ts.tv_sec >= range.min && ts.tv_sec <= range.max)
				return GRANTED;
			rsbac_pr_debug(reg, "Access to ressource outside its range [%ld:%ld], time %ld -> NOT_GRANTED!\n",
				range.min, range.max, ts.tv_sec);
			return NOT_GRANTED;
		case TBAC_since:
			if (ts.tv_sec >= range.min)
				return GRANTED;
			rsbac_pr_debug(reg, "Access to ressource before it's starting date %ld, time %ld -> NOT_GRANTED!\n",
				range.min, ts.tv_sec);
			return NOT_GRANTED;
		default:
			tbac_printk(ERR, "Unknown flag %d !", flags);
			return DO_NOT_CARE;
	}
	return NOT_GRANTED;
}

inline int
 tbac_request_func(	enum  rsbac_adf_request_t	request,
			rsbac_pid_t			caller_pid,
			enum  rsbac_target_t		target,
			union rsbac_target_id_t		tid,
			enum  rsbac_attribute_t		attr,
			union rsbac_attribute_value_t	attr_val,
			rsbac_uid_t			owner)
{
	rsbac_tbac_flags_t flags;
	rsbac_tbac_range_t range;

	switch (request) {
		case R_READ:
		case R_READ_OPEN:
		case R_MAP_EXEC:
		case R_EXECUTE:
		case R_APPEND_OPEN:
		case R_READ_WRITE_OPEN:
		case R_CHDIR:
		case R_CREATE:
		case R_DELETE:
		case R_RENAME:
		case R_CHANGE_GROUP:
		case R_MODIFY_PERMISSIONS_DATA:
		case R_CHANGE_OWNER:
	//	case R_SEARCH:
		case R_LINK_HARD:
		case R_MOUNT:
		case R_UMOUNT:
		case R_TRUNCATE:
		case R_WRITE_OPEN:
		case R_WRITE:
		case R_MODIFY_ACCESS_DATA:
			switch (target) {
				case T_DIR:
				case T_FILE:
					tbac_get_flags_from_lists(tid.file, &flags);
					if (flags == TBAC_unset)
						return DO_NOT_CARE;
					else if (flags == TBAC_range || flags == TBAC_since)
						tbac_get_range_from_lists(tid.file, &range);
					return check_tbac(flags, range);
				default:
					return DO_NOT_CARE;
			}
		default:
			return DO_NOT_CARE;
	}
	return DO_NOT_CARE;
}
