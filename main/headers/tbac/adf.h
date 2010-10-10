/**************************************/
/* Timed Based Access Control         */
/* Author and (c) 2009-2010: GCM SARL */
/* headers/tbac/adf.h                 */
/* Last modified: 11/Jun/2009         */
/**************************************/

#ifndef __TBAC_ADF_H
#define __TBAC_ADF_H

/* working_hours && oneshot_holiday variable must exists */
#define CHECK_WEEKEND(day)	(day == 5 || day == 6)
#define CHECK_MORNING(hour)	(hour < working_hours.min)
#define CHECK_EVENING(hour)	(hour > working_hours.max)
#define CHECK_HOLIDAY(epoch_sec)	(epoch_sec / 86400 == oneshot_holiday / 86400)

#define IN_SPARE_TIME(day, hour, epoch_sec)				\
	CHECK_WEEKEND(day) || 						\
	CHECK_MORNING(hour) || CHECK_EVENING(hour) || 			\
	CHECK_HOLIDAY(epoch_sec)

extern int
 tbac_request_func(	enum  rsbac_adf_request_t	request,
			rsbac_pid_t			caller_pid,
			enum  rsbac_target_t		target,
			union rsbac_target_id_t		tid,
			enum  rsbac_attribute_t		attr,
			union rsbac_attribute_value_t	attr_val,
			rsbac_uid_t			owner);

#endif
