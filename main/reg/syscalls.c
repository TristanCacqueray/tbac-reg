/**************************************/
/* Timed Based Access Control         */
/* Author and (c) 2009-2010: GCM SARL */
/* reg/syscalls.c                     */
/* Last modified: 11/Jun/2009         */
/**************************************/

/* current_uid() */
#include <linux/sched.h>

/* rsbac_printk() */
#include <rsbac/reg.h>

/* rsbac_(get|put)_user */
#include <rsbac/helpers.h>

/* get_error_name */
#include <rsbac/getname.h>

/* RSBAC_E* const */
#include <rsbac/error.h>

/* persistent generic lists */
#include <rsbac/lists.h>

#include <tbac/types.h>
#include <tbac/helpers.h>
#include <tbac/data_structures.h>
#include <tbac/syscalls.h>

/************************************************* */
/*            Attrs admin functions                */
/************************************************* */
int	tbac_set_flags(struct rsys_tbac_set_flags *arg, void *uptr)
{
	int			err = 0;
	int			tmperr;
	char			tmp[RSBAC_MAXNAMELEN];
	rsbac_list_handle_t *	list_handle;

	tbac_debug("called with %02u:%02u, inode=%d, flags=%d (%s)",
		RSBAC_MAJOR(arg->device), RSBAC_MINOR(arg->device),
		arg->inode, arg->flags,
		get_tbac_flags_name(tmp, arg->flags));

	switch(arg->flags) {
		case TBAC_unset:
		case TBAC_workinghours:
		case TBAC_sparehours:
		case TBAC_range:
		case TBAC_since:
			break;
		default:
			return -RSBAC_EINVALIDVALUE;
	}

	list_handle = tbac_get_flags_list_handle(arg->device);
	if (!list_handle) {
		tbac_printk(WARNING, "failed to retrieve FD attributes list, "
			"maybe wrong device ?");
		return -RSBAC_EACCESS;
	}

	if (arg->flags == TBAC_unset) {
		tmperr = rsbac_list_remove(*list_handle, &arg->inode);
		if (tmperr < 0 && tmperr != RSBAC_ENOTFOUND) {
			tbac_printk(ERR, "list_remove() returned %s",
				get_error_name(tmp, tmperr));
			err = tmperr;
		}
	} else {
		tmperr = rsbac_list_add(*list_handle, &arg->inode, &arg->flags);
		if (tmperr) {
			tbac_printk(ERR, "list_add() returned %s",
				get_error_name(tmp, tmperr));
			err = tmperr;
		}
	}
	return err;
}

int	tbac_get_flags(struct rsys_tbac_get_flags *arg, void *uptr)
{
	int			err = 0;
	int			tmperr;
	char			tmp[RSBAC_MAXNAMELEN];
	rsbac_list_handle_t *	list_handle;

	tbac_debug("called with %02u:%02u %d...",
		RSBAC_MAJOR(arg->device), RSBAC_MINOR(arg->device),
		arg->inode);

	list_handle = tbac_get_flags_list_handle(arg->device);
	if (!list_handle) {
		tbac_printk(WARNING, "failed to retrieve FD attributes list, "
			"maybe wrong device ?");
		return -RSBAC_EACCESS;
	}

	tmperr = rsbac_list_get_data(*list_handle, &arg->inode, &arg->flags);
	if (tmperr == -RSBAC_ENOTFOUND) {
		arg->flags = TBAC_unset;
	} else if (tmperr) {
		tbac_printk(ERR, "list_get_data() returned %s",
			get_error_name(tmp, tmperr));
		err = tmperr;
	}

	tbac_debug("... returning %d (%s)", arg->flags,
		get_tbac_flags_name(tmp, arg->flags));
	tmperr = rsbac_put_user((u_char *)arg, (u_char *)uptr, sizeof(*arg));
	if (tmperr) {
		tbac_printk(ERR, "rsbac_put_user() failled %s !!",
			get_error_name(tmp, tmperr));
		err = tmperr;
	}
	return err;
}

int	tbac_set_range(struct rsys_tbac_set_range *arg, void *uptr)
{
	int			err = 0;
	int			tmperr;
	char			tmp[RSBAC_MAXNAMELEN];
	rsbac_list_handle_t *	list_handle;

	tbac_debug("called with %02u:%02u, inode=%d, range=[%ld:%ld]",
		RSBAC_MAJOR(arg->device), RSBAC_MINOR(arg->device),
		arg->inode, arg->range.min, arg->range.max);

	if (arg->range.min > arg->range.max)
		return -RSBAC_EINVALIDVALUE;

	list_handle = tbac_get_range_list_handle(arg->device);
	if (!list_handle) {
		tbac_printk(WARNING, "failed to retrieve FD attributes list, "
			"maybe wrong device ?");
		return -RSBAC_EACCESS;
	}

	if (arg->range.min == 0 && arg->range.max == 0) {
		tmperr = rsbac_list_remove(*list_handle, &arg->inode);
		if (tmperr < 0 && tmperr != RSBAC_ENOTFOUND) {
			tbac_printk(ERR, "list_remove() returned %s",
				get_error_name(tmp, tmperr));
			err = tmperr;
		}
	} else {
		tmperr = rsbac_list_add(*list_handle, &(arg->inode), &arg->range);
		if (tmperr) {
			tbac_printk(ERR, "list_add() returned %s",
				get_error_name(tmp, tmperr));
			err = tmperr;
		}
	}
	return err;
}

int	tbac_get_range(struct rsys_tbac_get_range *arg, void *uptr)
{
	int			err = 0;
	int			tmperr;
	char			tmp[RSBAC_MAXNAMELEN];
	rsbac_list_handle_t *	list_handle;

	tbac_debug("called with %02u:%02u, inode=%ld...",
		RSBAC_MAJOR(arg->device), RSBAC_MINOR(arg->device),
		arg->inode);

	list_handle = tbac_get_range_list_handle(arg->device);
	if (!list_handle) {
		tbac_printk(WARNING, "failed to retrieve FD attributes list, "
			"maybe wrong device ?");
		return -RSBAC_EACCESS;
	}

	tmperr = rsbac_list_get_data(*list_handle, &arg->inode, &arg->range);
	if (tmperr == -RSBAC_ENOTFOUND) {
		arg->range.min = 0;
		arg->range.max = 0;
	} else if (tmperr) {
		tbac_printk(ERR, "list_get_data() returned %s",
			get_error_name(tmp, err));
		err = tmperr;
	}

	tbac_debug("... returning [%ld,%ld]", arg->range.min, arg->range.max);
	tmperr = rsbac_put_user((u_char *)arg, (u_char *)uptr, sizeof(*arg));
	if (tmperr) {
		tbac_printk(ERR, "rsbac_put_user() failled %s !!",
			get_error_name(tmp, tmperr));
		err = tmperr;
	}
	return err;
}


/************************************************* */
/*            Conf admin functions                 */
/************************************************* */
int	tbac_set_working_hours(struct rsys_tbac_set_working_hours *arg, void *uptr)
{
	int			err = 0;
	int			tmperr;
	char			tmp[RSBAC_MAXNAMELEN];
	rsbac_list_handle_t *	conf_handle;
	char			desc;

	tbac_debug("called with morning=%ld and evening=%ld",
		arg->morning, arg->evening);

	if (arg->morning > arg->evening || arg->evening >= 86400)
		return -RSBAC_EINVALIDVALUE;

	conf_handle = tbac_get_conf_list_handle();
	if (!conf_handle) {
		tbac_printk(ERR, "Failed to retrieve conf list!");
		return -RSBAC_EACCESS;
	}

	desc = MORNING_HOUR_IDX;
	tmperr = rsbac_list_add(*conf_handle, &desc, &arg->morning);
	if (tmperr) {
		tbac_printk(ERR, "list_add() returned %s",
			get_error_name(tmp, tmperr));
		err = tmperr;
	}
	desc = EVENING_HOUR_IDX;
	tmperr = rsbac_list_add(*conf_handle, &desc, &arg->evening);
	if (tmperr) {
		tbac_printk(ERR, "list_add() returned %s",
			get_error_name(tmp, tmperr));
		err = tmperr;
	} else {
		tbac_printk(INFO, "change working hour to %ld -> %ld", arg->morning, arg->evening);
	}
	return err;
}

int	tbac_get_working_hours(struct rsys_tbac_get_working_hours *arg, void *uptr)
{
	int			err = 0;
	int			tmperr;
	char			tmp[RSBAC_MAXNAMELEN];
	rsbac_list_handle_t *	conf_handle;
	char			desc;

	tbac_debug("called !...");

	conf_handle = tbac_get_conf_list_handle();
	if (!conf_handle) {
		tbac_printk(ERR, "Failed to retrieve conf list!");
		return -RSBAC_EACCESS;
	}

	desc = MORNING_HOUR_IDX;
	tmperr = rsbac_list_get_data(*conf_handle, &desc, &arg->morning);
	if (tmperr) {
		tbac_printk(ERR, "list_get_data() returned %s",
			get_error_name(tmp, tmperr));
		err = tmperr;
	}
	desc = EVENING_HOUR_IDX;
	tmperr = rsbac_list_get_data(*conf_handle, &desc, &arg->evening);
	if (tmperr) {
		tbac_printk(ERR, "list_get_data() returned %s",
			get_error_name(tmp, tmperr));
		err = tmperr;
	}

	tbac_debug("... returning morning=%ld and evening=%ld",
		arg->morning, arg->evening);
	tmperr = rsbac_put_user((u_char *)arg, (u_char *)uptr, sizeof(*arg));
	if (tmperr) {
		tbac_printk(ERR, "rsbac_put_user() failled %s !!",
			get_error_name(tmp, tmperr));
		err = tmperr;
	}
	return err;
}

int	tbac_set_oneshot_holiday(struct rsys_tbac_set_oneshot_holiday *arg, void *uptr)
{
	int			err = 0;
	int			tmperr;
	char			tmp[RSBAC_MAXNAMELEN];
	rsbac_list_handle_t *	conf_handle;
	char			desc;
	time_t			holiday_day;

	tbac_debug("called with holiday=%ld", arg->time);

	holiday_day = arg->time / 86400 * 86400;
	conf_handle = tbac_get_conf_list_handle();
	if (!conf_handle) {
		tbac_printk(ERR, "Failed to retrieve conf list!");
		return -RSBAC_EACCESS;
	}

	desc = ONESHOT_HOLIDAY_IDX;
	tmperr = rsbac_list_add(*conf_handle, &desc, &holiday_day);
	if (tmperr) {
		tbac_printk(ERR, "list_add() returned %s",
			get_error_name(tmp, tmperr));
		err = tmperr;
	} else {
		tbac_printk(INFO, "change oneshot holiday to %ld", holiday_day);
	}
	return err;
}

int	tbac_get_oneshot_holiday(struct rsys_tbac_get_oneshot_holiday *arg, void *uptr)
{
	int			err = 0;
	int			tmperr;
	char			tmp[RSBAC_MAXNAMELEN];
	rsbac_list_handle_t *	conf_handle;
	char			desc;

	tbac_debug("called...");

	conf_handle = tbac_get_conf_list_handle();
	if (!conf_handle) {
		tbac_printk(ERR, "Failed to retrieve conf list!");
		return -RSBAC_EACCESS;
	}

	desc = ONESHOT_HOLIDAY_IDX;
	tmperr = rsbac_list_get_data(*conf_handle, &desc, &arg->time);
	if (tmperr) {
		tbac_printk(ERR, "list_get_data() returned %s"
			"and oneshot holiday should be set",
			get_error_name(tmp, tmperr));
		arg->time = 0;
		err = tmperr;
	}

	tbac_debug("... returning oneshot holiday=%ld (UTC)",
		arg->time);
	tmperr = rsbac_put_user((u_char *)arg, (u_char *)uptr, sizeof(*arg));
	if (tmperr) {
		tbac_printk(ERR, "rsbac_put_user() failled %s !!",
			get_error_name(tmp, tmperr));
		err = tmperr;
	}
	return err;
}

#define TBAC_GEN_SYSCALL(N)						\
int syscall_func_tbac_##N(void *arg_p){					\
    struct rsys_tbac_##N	arg;					\
    int			err;						\
									\
    if (current_uid() != RSBAC_SECOFF_UID)				\
	return -RSBAC_EPERM;						\
    if (arg_p){								\
	err = rsbac_get_user((u_char *) &arg, (u_char *) arg_p,		\
			sizeof(arg));					\
	if (err)							\
		return err;						\
    } else {								\
	memset(&arg, 0, sizeof(arg));					\
    }									\
    return (tbac_##N(&arg, arg_p));					\
}

TBAC_GEN_SYSCALL(set_flags);
TBAC_GEN_SYSCALL(get_flags);
TBAC_GEN_SYSCALL(set_range);
TBAC_GEN_SYSCALL(get_range);

TBAC_GEN_SYSCALL(set_oneshot_holiday);
TBAC_GEN_SYSCALL(get_oneshot_holiday);
TBAC_GEN_SYSCALL(set_working_hours);
TBAC_GEN_SYSCALL(get_working_hours);
