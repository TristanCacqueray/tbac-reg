/**************************************/
/* Timed Based Access Control         */
/* Author and (c) 2009-2010: GCM SARL */
/* headers/tbac/syscalls.h            */
/* Last modified: 11/Jun/2009         */
/**************************************/

#ifndef __TBAC_SYSCALLS_H
#define __TBAC_SYSCALLS_H

#include <tbac/types.h>

/* Syscalls */
#define TBAC_HDL_set_flags		1
#define TBAC_HDL_get_flags		2
#define TBAC_HDL_set_range		3
#define TBAC_HDL_get_range		4
#define TBAC_HDL_set_oneshot_holiday	5
#define TBAC_HDL_get_oneshot_holiday	6
#define TBAC_HDL_set_working_hours	7
#define TBAC_HDL_get_working_hours	8


#ifdef __KERNEL__
/* methods used by reg module registration */
int syscall_func_tbac_set_flags(void *arg);
int syscall_func_tbac_get_flags(void *arg);
int syscall_func_tbac_set_range(void *arg);
int syscall_func_tbac_get_range(void *arg);

int syscall_func_tbac_set_oneshot_holiday(void *arg);
int syscall_func_tbac_get_oneshot_holiday(void *arg);
int syscall_func_tbac_set_working_hours(void *arg);
int syscall_func_tbac_get_working_hours(void *arg);


/* real syscalls */
int tbac_set_flags(struct rsys_tbac_set_flags *arg, void *uptr);
int tbac_get_flags(struct rsys_tbac_get_flags *arg, void *uptr);
int tbac_set_range(struct rsys_tbac_set_range *arg, void *uptr);
int tbac_get_range(struct rsys_tbac_get_range *arg, void *uptr);

int tbac_set_oneshot_holiday(struct rsys_tbac_set_oneshot_holiday *arg, void *uptr);
int tbac_get_oneshot_holiday(struct rsys_tbac_get_oneshot_holiday *arg, void *uptr);
int tbac_set_working_hours(struct rsys_tbac_set_working_hours *arg, void *uptr);
int tbac_get_working_hours(struct rsys_tbac_get_working_hours *arg, void *uptr);
#else
/* library exported wrappers */
int tbac_get_flags(const char *path, rsbac_tbac_flags_t * flags);
int tbac_get_range(const char * path, rsbac_tbac_range_t * range);
int tbac_set_flags(const char * path, const rsbac_tbac_flags_t flags);
int tbac_set_range(const char * path, const rsbac_tbac_range_t range);

int tbac_get_working_hours(time_t * morning, time_t * evening);
int tbac_get_oneshot_holiday(time_t * holiday);
int tbac_set_working_hours(const time_t morning, const time_t evening);
int tbac_set_oneshot_holiday(const time_t holiday);
#endif

#endif
