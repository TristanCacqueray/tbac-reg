/**************************************/
/* Timed Based Access Control         */
/* Author and (c) 2009-2010: GCM SARL */
/* headers/tbac/data_structures.h     */
/* Last modified: 11/Jun/2009         */
/**************************************/

#ifndef __TBAC_DATA_STRUCTURES_H
#define __TBAC_DATA_STRUCTURES_H

#include <tbac/types.h>
#include <rsbac/lists.h>

/* Lists */
#define RSBAC_TBAC_LIST_VERSION		1
#define RSBAC_TBAC_NR_FLAG_FD_LISTS	4

#define RSBAC_TBAC_CONF_FILENAME	"tbac_t"
#define RSBAC_TBAC_FD_FLAGS_FILENAME	"fd_tbac"
#define RSBAC_TBAC_FD_RANGE_FILENAME	"fd_tbacr"


/* Configuration indexes (genlist desc)*/
#define MORNING_HOUR_IDX	0
#define EVENING_HOUR_IDX	1
#define ONESHOT_HOLIDAY_IDX	2
#define CONF_DESC_SIZE		sizeof(char)


struct rsbac_tbac_device_list_item_t {
	kdev_t					id;
	u_int					mount_count;
	rsbac_list_handle_t			flags_handle;
	rsbac_list_handle_t			range_handle;
	/* ... */
	struct rsbac_tbac_device_list_item_t	*next;
};


/* exported for module init/unload */
int	tbac_init_configuration(void);
int	tbac_cleanup_configuration(void);


int	tbac_init_lists(kdev_t kdev);
int	tbac_init_device(kdev_t kdev);
int	tbac_cleanup_device(kdev_t kdev);


/* exported for syscalls access */
rsbac_list_handle_t	*tbac_get_flags_list_handle(kdev_t kdev);
rsbac_list_handle_t	*tbac_get_range_list_handle(kdev_t kdev);
rsbac_list_handle_t	*tbac_get_conf_list_handle(void);


/* exported for adf */
void	tbac_get_flags_from_lists(struct rsbac_fs_file_t fs_file, rsbac_tbac_flags_t *flags);
void	tbac_get_range_from_lists(struct rsbac_fs_file_t fs_file, rsbac_tbac_range_t *range);
int	get_working_hours(rsbac_tbac_range_t *range);
int 	get_oneshot_holiday(time_t *oneshot_holiday);

#endif
