/**************************************/
/* Timed Based Access Control         */
/* Author and (c) 2009-2010: GCM SARL */
/* reg/module.c                       */
/* Last modified: 11/Jun/2009         */
/**************************************/

/* general stuff */
#include <linux/module.h>

/* rsbac reg stuff */
#include <rsbac/reg.h>

/* rsbac: root_dev */
#include <rsbac/aci_data_structures.h>

#include <tbac/types.h>
#include <tbac/helpers.h>
#include <tbac/syscalls.h>
#include <tbac/data_structures.h>
#include <tbac/adf.h>

#if defined(CONFIG_RSBAC_PROC)
#include <rsbac/proc_fs.h>
#include <tbac/proc.h>
#endif


/************************************************* */
/*            Linux Modules                        */
/************************************************* */
MODULE_AUTHOR("Tristan de CACQUERAY");
MODULE_DESCRIPTION("RSBAC REG TBAC");
MODULE_LICENSE("GPL");

static long tbac_root_device_minor = BOUCHON_RDEV_MINOR;
static long tbac_root_device_major = BOUCHON_RDEV_MAJOR;
kdev_t tbac_root_device;

module_param(tbac_root_device_minor, long, S_IRUSR);
//MODULE_PARM_DESC(tbac_root_device_minor, "minor");
module_param(tbac_root_device_major, long, S_IRUSR);
//MODULE_PARM_DESC(tbac_root_device_major, "major");


/************************************************* */
/*            Reg Handles                          */
/************************************************* */
/* module key */
#define RSBAC_TBAC_KEY				0x76787101

#define TBAC_SYSCALL_KEY_set_flags		0x3b3c3880
#define TBAC_SYSCALL_KEY_get_flags		0x3b3c3881
#define TBAC_SYSCALL_KEY_set_range		0x3b3c3882
#define TBAC_SYSCALL_KEY_get_range		0x3b3c3883

#define TBAC_SYSCALL_KEY_set_oneshot_holiday	0x3b3c3884
#define TBAC_SYSCALL_KEY_get_oneshot_holiday	0x3b3c3885
#define TBAC_SYSCALL_KEY_set_working_hours	0x3b3c3886
#define TBAC_SYSCALL_KEY_get_working_hours	0x3b3c3887


/* REG module main handle */
static long tbac_handle = RSBAC_TBAC_KEY;

/* for crappy unloading purpose */
long syscalls_registered[10];

#define TBAC_DEFINE_SYSCALL(N)						\
    static long syscall_reg_hdl_tbac_##N = TBAC_SYSCALL_KEY_##N;	\
    static long syscall_dispatcher_hdl_tbac_##N = TBAC_HDL_##N;		\

TBAC_DEFINE_SYSCALL(set_flags)
TBAC_DEFINE_SYSCALL(get_flags)
TBAC_DEFINE_SYSCALL(set_range)
TBAC_DEFINE_SYSCALL(get_range)

TBAC_DEFINE_SYSCALL(set_oneshot_holiday)
TBAC_DEFINE_SYSCALL(get_oneshot_holiday)
TBAC_DEFINE_SYSCALL(set_working_hours)
TBAC_DEFINE_SYSCALL(get_working_hours)

/*
 * Register syscall during module_init and store it's handle
 * if something goes wrong, goto init_module_failure
 */
#define TBAC_REG_SYSCALL(N)						\
{									\
    struct rsbac_reg_syscall_entry_t		s_entry;		\
									\
    syscalls_registered[syscall_dispatcher_hdl_tbac_##N] = 0;		\
    memset(&s_entry, 0, sizeof(s_entry));				\
									\
    strncpy(s_entry.name, "RSBAC TBAC SYSCALL " #N,			\
		RSBAC_REG_NAME_LEN);					\
    s_entry.registration_handle = syscall_reg_hdl_tbac_##N;		\
    s_entry.dispatcher_handle = syscall_dispatcher_hdl_tbac_##N;	\
    s_entry.syscall_func = syscall_func_tbac_##N;			\
    tbac_debug("Name: %s, Dispatcher Handle: %li",			\
		s_entry.name, s_entry.dispatcher_handle);		\
    tbac_debug("Registering syscall." #N " "				\
		"to rsbac_reg_register_syscall()");			\
    syscall_reg_hdl_tbac_##N = rsbac_reg_register_syscall(		\
		RSBAC_REG_VERSION, s_entry);				\
    if (syscall_reg_hdl_tbac_##N < 0) {					\
	tbac_printk(ERR, "Registering " #N " "				\
		"failed (%li). Unloading.", syscall_reg_hdl_tbac_##N);	\
	goto init_module_failure;					\
    }									\
    syscalls_registered[syscall_dispatcher_hdl_tbac_##N] =		\
		syscall_reg_hdl_tbac_##N;				\
}

#define TBAC_UNREG_SYSCALL_HANDLE(handle)				\
{									\
    if (rsbac_reg_unregister_syscall(handle)) {				\
	tbac_debug("Unregistering of " #handle " (= %li). "		\
		"failed ! - beware of possible system failure!",	\
			(handle));					\
    }									\
}

static void tbac_unload(const char *msg)
{
	int idx;

	tbac_printk(INFO, "%s", msg);
	remove_proc_entry(PROC_NAME, proc_rsbac_root_p);
	for (idx = 0; idx < 10; idx++)
		if (syscalls_registered[idx] > 0)
			TBAC_UNREG_SYSCALL_HANDLE(syscalls_registered[idx]);

	if (rsbac_reg_unregister(tbac_handle)) {
		tbac_debug("Unregistering failed - beware of possible system failure!");
	}
	tbac_cleanup_device(tbac_root_device);
	tbac_cleanup_configuration();
	tbac_printk(INFO, "Unloaded.");
}

int init_module(void)
{
    int err;

    tbac_root_device = MKDEV(tbac_root_device_major, tbac_root_device_minor);
    tbac_printk(INFO, "Initializing (REG Version: %u).", RSBAC_REG_VERSION);

    /* TBAC CONFIGURATION */
    {
	tbac_debug("Name: LIST CONF, Handle: %s, Device %02u:%02u.",
			RSBAC_TBAC_CONF_FILENAME,
		    	RSBAC_MAJOR(rsbac_root_dev),
			RSBAC_MINOR(rsbac_root_dev)
			);
	tbac_debug("Registering to Generic list.");
	err = tbac_init_configuration();
	if (err)
		return -ENOEXEC;
    }

    /* LISTS */
    {
	tbac_debug("Name: LIST FD flags, Handle: %s, Device %02u:%02u.",
			RSBAC_TBAC_FD_FLAGS_FILENAME,
		    	RSBAC_MAJOR(tbac_root_device),
			RSBAC_MINOR(tbac_root_device)
			);
	tbac_debug("Name: LIST FD range, Handle: %s, Device %02u:%02u.",
			RSBAC_TBAC_FD_RANGE_FILENAME,
		    	RSBAC_MAJOR(tbac_root_device),
			RSBAC_MINOR(tbac_root_device)
			);
	tbac_debug("Registering to Generic list.");
	err = tbac_init_device(tbac_root_device);
	if (err) {
		tbac_cleanup_device(tbac_root_device);
		tbac_cleanup_configuration();
		return -ENOEXEC;
	}
    }

    /* REG MODULE */
    {
	struct rsbac_reg_entry_t entry;
	memset(&entry, 0, sizeof(entry));

	strcpy(entry.name, "RSBAC TBAC ADF module");
	tbac_debug("Name: %s, Handle: %li.",
			entry.name, tbac_handle);

	entry.handle = tbac_handle;
	entry.request_func = tbac_request_func;
	//entry.set_attr_func = tbac_set_attr_func;
	entry.switch_on = TRUE;

	tbac_printk(INFO, "Registering to ADF.");

	if (rsbac_reg_register(RSBAC_REG_VERSION, entry) < 0) {
		tbac_printk(ERR, "Registering failed. Unloading.");
		tbac_cleanup_device(tbac_root_device);
		tbac_cleanup_configuration();
		return -ENOEXEC;
	}
    }

    /* SYSCALL */
    {
	int		idx;

	for (idx = 0; idx < 10; idx++)
		syscalls_registered[idx] = -1;
	TBAC_REG_SYSCALL(set_flags);
	TBAC_REG_SYSCALL(get_flags);
	TBAC_REG_SYSCALL(set_range);
	TBAC_REG_SYSCALL(get_range);
	TBAC_REG_SYSCALL(set_oneshot_holiday);
	TBAC_REG_SYSCALL(get_oneshot_holiday);
	TBAC_REG_SYSCALL(set_working_hours);
	TBAC_REG_SYSCALL(get_working_hours);
    }

    /* PROC */
    #if defined(CONFIG_RSBAC_PROC)
    {
	struct proc_dir_entry *	tbac_proc_p;

	tbac_printk(INFO, "Creating proc entry.");
	tbac_proc_p = proc_create(PROC_NAME, S_IFREG | S_IRUGO, proc_rsbac_root_p, &tbac_proc_fops);
	if (!tbac_proc_p)
	{
		tbac_printk(ERR, "Failled to create proc entry.");
		goto init_module_failure;
	}
    }
    #endif

    tbac_printk(INFO, "Loaded.");
    return 0;

init_module_failure:
    tbac_unload("Somethings went wrong...");
    return -ENOEXEC;
}

void cleanup_module(void)
{
	tbac_unload("Unregistering.");
}
