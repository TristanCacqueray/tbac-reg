/**************************************/
/* Timed Based Access Control         */
/* Author and (c) 2009-2010: GCM SARL */
/* reg/data_structures.c              */
/* Last modified: 11/Jun/2009         */
/**************************************/

#include <rsbac/getname.h>
#include <rsbac/error.h>
#include <rsbac/reg.h>
#include <rsbac/aci_data_structures.h>

#include <tbac/types.h>
#include <tbac/helpers.h>
#include <tbac/data_structures.h>

/************************************************************************** */
/*                          Global Variables                                */
/************************************************************************** */
#define RSBAC_TBAC_LIST_KEY			0x76787102

/* Enforced device */
static struct rsbac_tbac_device_list_item_t *	head = NULL;
/* TBAC configuration handle */
static rsbac_list_handle_t			conf = NULL;


/************************************************* */
/*               Internal Help functions           */
/************************************************* */
static u_int	nr_fd_hashes = RSBAC_TBAC_NR_FLAG_FD_LISTS;

static int	tbac_register_fd_lists(struct rsbac_tbac_device_list_item_t *	device_p)
{
	int				err = 0;
	int				tmperr;
	struct rsbac_list_info_t	list_info;

	if (!device_p)
		return -RSBAC_EINVALIDPOINTER;

	list_info.version = RSBAC_TBAC_LIST_VERSION;
	list_info.key = RSBAC_TBAC_LIST_KEY;
	list_info.desc_size = sizeof(rsbac_inode_nr_t);
	list_info.data_size = sizeof(rsbac_tbac_flags_t);
	list_info.max_age = 0;

	tmperr = rsbac_list_register_hashed(RSBAC_LIST_VERSION,
			&device_p->flags_handle,	/* list handle */
			&list_info,			/* list info */
			RSBAC_LIST_PERSIST |		/* persistent */
			RSBAC_LIST_DEF_DATA |		/* use default data */
#if 0
			RSBAC_LIST_IGNORE_OLD |
			RSBAC_LIST_NO_WRITE |		/* dev purpose */
#endif
			RSBAC_LIST_AUTO_HASH_RESIZE,	/* auto rehash */
			NULL,			/* lookup and list optim, can be NULL */
			NULL,			/* list version conversion provider, can be NULL */
			NULL,			/* def_data */
			RSBAC_TBAC_FD_FLAGS_FILENAME,	/* list name */
			device_p->id,			/* device (0 for root dev) (ignored if !LIST_PERSIST*/
			nr_fd_hashes,		/* u_int nr_hashes */
			rsbac_list_hash_fd,
			NULL);
	if (tmperr) {
		char tmp[RSBAC_MAXNAMELEN];
		tbac_printk(ERR, "registering list %s for device %02u:%02u "
			"failed with error %s!",
			RSBAC_TBAC_FD_FLAGS_FILENAME,
			RSBAC_MAJOR(device_p->id),
			RSBAC_MINOR(device_p->id),
			get_error_name(tmp, tmperr));
		err = tmperr;
	}

	list_info.version = RSBAC_TBAC_LIST_VERSION;
	list_info.key = RSBAC_TBAC_LIST_KEY;
	list_info.desc_size = sizeof(rsbac_inode_nr_t);
	list_info.data_size = sizeof(rsbac_tbac_range_t);
	list_info.max_age = 0;

	tmperr = rsbac_list_register_hashed(RSBAC_LIST_VERSION,
			&device_p->range_handle,	/* list handle */
			&list_info,			/* list info */
			RSBAC_LIST_PERSIST |		/* savable and  restorable */
			RSBAC_LIST_DEF_DATA |		/* use default data */
			RSBAC_LIST_AUTO_HASH_RESIZE,	/* auto rehash */
			NULL,			/* lookup and list optim, can be NULL */
			NULL,			/* list version conversion provider, can be NULL */
			NULL,			/* def_data */
			RSBAC_TBAC_FD_RANGE_FILENAME,	/* list name */
			device_p->id,			/* device (0 for root dev) (ignored if !LIST_PERSIST*/
			nr_fd_hashes,		/* u_int nr_hashes */
			rsbac_list_hash_fd,
			NULL);
	if (tmperr) {
		char tmp[RSBAC_MAXNAMELEN];
		tbac_printk(ERR, "registering list %s for device %02u:%02u "
			"failed with error %s!",
			RSBAC_TBAC_FD_RANGE_FILENAME,
			RSBAC_MAJOR(device_p->id),
			RSBAC_MINOR(device_p->id),
			get_error_name(tmp, tmperr));
		err = tmperr;
	}
	return err;
}

static int	tbac_detach_fd_lists(struct rsbac_tbac_device_list_item_t *device_p)
{
	int err = 0;
	int tmperr;

	if (!device_p)
		return -RSBAC_EINVALIDPOINTER;

	if (device_p->flags_handle) {
		tmperr = rsbac_list_detach(&device_p->flags_handle, RSBAC_TBAC_LIST_KEY);

		if (tmperr) {
			char tmp[RSBAC_MAXNAMELEN];

			tbac_printk(ERR, "detaching from list %s for device %02u:%02u failed with error %s!",
					RSBAC_TBAC_FD_FLAGS_FILENAME,
					RSBAC_MAJOR(device_p->id),
					RSBAC_MINOR(device_p->id),
					get_error_name(tmp, tmperr));
			err = tmperr;
		}
	}
	if (device_p->range_handle) {
		tmperr = rsbac_list_detach(&device_p->range_handle, RSBAC_TBAC_LIST_KEY);

		if (tmperr) {
			char tmp[RSBAC_MAXNAMELEN];

			tbac_printk(ERR, "detaching from list %s for device %02u:%02u failed with error %s!",
					RSBAC_TBAC_FD_RANGE_FILENAME,
					RSBAC_MAJOR(device_p->id),
					RSBAC_MINOR(device_p->id),
					get_error_name(tmp, tmperr));
			err = tmperr;
		}
	}
	return err;
}



/*               Device List functions             */
static struct rsbac_tbac_device_list_item_t *	create_device_item(kdev_t kdev)
{
	struct rsbac_tbac_device_list_item_t *new_item_p;

	/* allocate memory for new device, return NULL, if failed */
	if (!(new_item_p = (struct rsbac_tbac_device_list_item_t *)
	      rsbac_kmalloc(sizeof(*new_item_p))))
		return NULL;

	new_item_p->id = kdev;
	new_item_p->mount_count = 1;

	/* init file/dir sublists */
	new_item_p->flags_handle = NULL;
	new_item_p->range_handle = NULL;

	new_item_p->next = NULL;
	return new_item_p;
}
static void	clear_device_item(struct rsbac_tbac_device_list_item_t *item_p)
{
	if (!item_p)
		return;

	/* First deregister lists... */
	tbac_detach_fd_lists(item_p);
	/* OK, lets remove the device item itself */
	rsbac_kfree(item_p);
}


/*               Configuration functions             */
static void	tbac_set_default_configuration(void)
{
	int	err = 0;
	char	desc;
	time_t	data;

	desc = MORNING_HOUR_IDX;
	err = rsbac_list_get_data(conf, &desc, &data);
	tbac_printk(INFO, "Recover %ld for morning hour", data);
	if (err == -RSBAC_ENOTFOUND) {
		data = DEFAULT_MORNING_HOUR;
		rsbac_list_add(conf, &desc, &data);
	} else if (err) {
		tbac_printk(ERR, "rsbac_list_add_data() return %d for morning_hour", err);
	}

	desc = EVENING_HOUR_IDX;
	err = rsbac_list_get_data(conf, &desc, &data);
	tbac_printk(INFO, "Recover %ld for evening hour", data);
	if (err == -RSBAC_ENOTFOUND) {
		data = DEFAULT_EVENING_HOUR;
		rsbac_list_add(conf, &desc, &data);
	} else if (err) {
		tbac_printk(ERR, "rsbac_list_get_data() return %d for end_hour", err);
	}

	desc = ONESHOT_HOLIDAY_IDX;
	err = rsbac_list_get_data(conf, &desc, &data);
	tbac_printk(INFO, "Recover %ld for oneshot holiday", data);
	if (err == -RSBAC_ENOTFOUND) {
		data = 0;
		rsbac_list_add(conf, &desc, &data);
	} else if (err) {
		tbac_printk(ERR, "rsbac_list_get_data() return %d for oneshot", err);
	}
}

/************************************************* */
/*               Init functions                    */
/************************************************* */
int	tbac_init_configuration(void)
{
	int err = 0;
	int tmperr;
	struct rsbac_list_info_t	list_info;

	if (conf != NULL){
		tbac_printk(ERR, "Tbac configuration list already initialized !");
		return -RSBAC_EREINIT;
	}

	/* register list */
	list_info.version = RSBAC_TBAC_LIST_VERSION;
	list_info.key = RSBAC_TBAC_LIST_KEY;
	list_info.desc_size = CONF_DESC_SIZE;
	list_info.data_size = sizeof(time_t);
	list_info.max_age = 0;

	tmperr = rsbac_list_register_hashed(RSBAC_LIST_VERSION,
			&conf,	/* list handle */
			&list_info,			/* list info */
			RSBAC_LIST_PERSIST |		/* savable and  restorable */
			RSBAC_LIST_DEF_DATA |		/* use default data */
			RSBAC_LIST_AUTO_HASH_RESIZE,	/* auto rehash */
			NULL,			/* lookup and list optim, can be NULL */
			NULL,			/* list version conversion provider, can be NULL */
			NULL,			/* def_data */
			RSBAC_TBAC_CONF_FILENAME,	/* list name */
			rsbac_root_dev,			/* device (0 for root dev) (ignored if !LIST_PERSIST*/
			nr_fd_hashes,		/* u_int nr_hashes */
			rsbac_list_hash_fd,
			NULL);
	if (tmperr) {
		char tmp[RSBAC_MAXNAMELEN];
		tbac_printk(ERR, "registering list %s for device %02u:%02u failed with error %s!",
					RSBAC_TBAC_CONF_FILENAME,
					RSBAC_MAJOR(rsbac_root_dev),
					RSBAC_MINOR(rsbac_root_dev),
					get_error_name(tmp, tmperr));
		err = tmperr;
	}
	if (!err)
		tbac_set_default_configuration();
	return err;
}

int	tbac_cleanup_configuration(void)
{
	int err = 0;
	int tmperr;

	if (conf == NULL){
		tbac_printk(ERR, "Tbac configuration list not initialized !");
		return -RSBAC_ENOTINITIALIZED;
	}

	tmperr = rsbac_list_detach(&conf, RSBAC_TBAC_LIST_KEY);
	if (tmperr) {
		char tmp[RSBAC_MAXNAMELEN];

		tbac_printk(ERR, "detaching from list %s for device %02u:%02u failed with error %s!",
				RSBAC_TBAC_FD_RANGE_FILENAME,
				RSBAC_MAJOR(rsbac_root_dev),
				RSBAC_MINOR(rsbac_root_dev),
				get_error_name(tmp, tmperr));
		err = tmperr;
	}
	return err;
}


/************************************************* */
/*               Access functions                  */
/************************************************* */
/* tbac_get_*_list_handle() */
/* For administration (set/get) syscalls */
rsbac_list_handle_t	*tbac_get_conf_list_handle(void)
{
	return &conf;
}
rsbac_list_handle_t	*tbac_get_flags_list_handle(kdev_t kdev)
{
	if (head == NULL){
		tbac_printk(ERR, "No device was init, can not get handle");
		return NULL;
	}
	if (head->id != kdev){
		return NULL;
	}
	return &(head->flags_handle);
}
rsbac_list_handle_t	*tbac_get_range_list_handle(kdev_t kdev)
{
	if (head == NULL){
		tbac_printk(ERR, "No device was init, can not get handle");
		return NULL;
	}
	if (head->id != kdev){
		return NULL;
	}
	return &(head->range_handle);
}

int	tbac_init_device(kdev_t kdev)
{
	int	err;

	if (head != NULL){
		tbac_printk(ERR, "Device %02u:%02u already registered",
				RSBAC_MAJOR(kdev),
				RSBAC_MINOR(kdev));
		return -RSBAC_EREINIT;
	}
	head = create_device_item(kdev);
	if (head == NULL) {
		tbac_printk(ERR, "Device item allocation failed. Unloading.");
		return -RSBAC_ENOMEM;
	}
	err = tbac_register_fd_lists(head);
	if (err < 0) {
		tbac_printk(ERR, "tbac_register_fd_lists failled. Unloading.");
		rsbac_kfree(head);
		return err;
	}
	return 0;
}
int	tbac_cleanup_device(kdev_t kdev)
{
	if (head == NULL){
		tbac_printk(ERR, "No device was init, can not cleanup");
		return -RSBAC_ENOTINITIALIZED;
	}
	if (head->id != kdev){
		tbac_printk(ERR, "Device %02u:%02u (!= %02u:%02u) was previously init",
				RSBAC_MAJOR(head->id),
				RSBAC_MINOR(head->id),
				RSBAC_MAJOR(kdev),
				RSBAC_MINOR(kdev)
				);
		return -RSBAC_ENOTINITIALIZED;
	}
	clear_device_item(head);
	return 0;
}

/* ADF access function facilities */
void	tbac_get_flags_from_lists(struct rsbac_fs_file_t fs_file, rsbac_tbac_flags_t *flags)
{
	rsbac_list_handle_t *	list_handle;
	int			err;

	if (!(list_handle = tbac_get_flags_list_handle(fs_file.device)))
	{
		*flags = TBAC_unset;
		return ;
	}
	err = rsbac_list_get_data(*list_handle, &(fs_file.inode), flags);
	if (err < 0) 
		*flags = TBAC_unset;
}
void	tbac_get_range_from_lists(struct rsbac_fs_file_t fs_file, rsbac_tbac_range_t *range)
{
	rsbac_list_handle_t *	list_handle;
	int			err;

	if (!(list_handle = tbac_get_range_list_handle(fs_file.device)))
	{
		range->min = 0;
		range->max = 0;
		return ;
	}
	err = rsbac_list_get_data(*list_handle, &(fs_file.inode), range);
	if (err < 0) 
	{
		range->min = 0;
		range->max = 0;
	}
}

int	get_working_hours(rsbac_tbac_range_t *range)
{
	int	err = 0;
	int	tmperr;
	char	desc;

	desc = MORNING_HOUR_IDX;
	tmperr = rsbac_list_get_data(conf, &desc, &range->min);
	if (tmperr){
		tbac_printk(ERR, "could not get working hours, using default");
		range->min = DEFAULT_MORNING_HOUR;
		err = tmperr;
	}
	desc = EVENING_HOUR_IDX;
	tmperr = rsbac_list_get_data(conf, &desc, &range->max);
	if (tmperr){
		tbac_printk(ERR, "could not get working hours, using default");
		range->max = DEFAULT_EVENING_HOUR;
		err = tmperr;
	}
	return err;
}

int	get_oneshot_holiday(time_t *oneshot_holiday)
{
	int	err = 0;
	int	tmperr;
	char	desc;

	desc = ONESHOT_HOLIDAY_IDX;
	tmperr = rsbac_list_get_data(conf, &desc, oneshot_holiday);
	if (tmperr){
		tbac_printk(ERR, "could not get oneshot holiday, using default (no holiday");
		oneshot_holiday = 0;
		err = tmperr;
	}
	return err;
}
