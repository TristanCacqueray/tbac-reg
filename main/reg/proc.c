/**************************************/
/* Timed Based Access Control         */
/* Author and (c) 2009-2010: GCM SARL */
/* reg/proc.c                         */
/* Last modified: 11/Jun/2009         */
/**************************************/

#include <linux/module.h>
#include <linux/seq_file.h>
#include <rsbac/types.h>
/* rsbac_adf_request */
#include <rsbac/error.h>
#include <rsbac/adf.h>
/* rsbac_is_initialized() */
#include <rsbac/aci.h>

#include <tbac/types.h>
#include <tbac/data_structures.h>
#include <tbac/adf.h>
#include <tbac/proc.h>

#if defined(CONFIG_RSBAC_PROC)

static int tbac_proc_show(struct seq_file *m, void *v)
{
	union rsbac_target_id_t		rsbac_target_id;
	union rsbac_attribute_value_t	rsbac_attribute_value;
	rsbac_tbac_range_t		working_hours;
	time_t				oneshot_holiday;

	if (!rsbac_is_initialized())
		return -ENOSYS;

	rsbac_target_id.scd = ST_rsbac;
	rsbac_attribute_value.dummy = 0;
	if (!rsbac_adf_request(R_GET_STATUS_DATA,
		task_pid(current),
		T_SCD,
		rsbac_target_id,
		A_none,
		rsbac_attribute_value)) {
		return -EPERM;
	}
	seq_puts(m, "RSBAC REG TBAC\n"
		    "--------------\n");

	seq_puts(m, "Current configuration (UTC):\n");
	get_working_hours(&working_hours);
	get_oneshot_holiday(&oneshot_holiday);
	seq_printf(m, "Working hours: %02ld:%02ld -> %02ld:%02ld\n",
		working_hours.min / 3600, working_hours.min % 3600 / 60,
		working_hours.max / 3600, working_hours.max % 3600 / 60
	);
	if (oneshot_holiday)
		seq_printf(m, "Oneshot holiday set to %ld\n", oneshot_holiday);
	else
		seq_puts(m, "No holiday planned\n\n");

	seq_puts(m, "List statistics:\n");
	{
		rsbac_list_handle_t *list_handle;
		seq_printf(m, "Controlled device: %02u:%02u\n",
			RSBAC_MAJOR(tbac_root_device),
			RSBAC_MINOR(tbac_root_device)
		);
		list_handle = tbac_get_flags_list_handle(tbac_root_device);
		if (list_handle)
			seq_printf(m, "Inode flags set: %ld\n", rsbac_list_count(*list_handle));
		list_handle = tbac_get_range_list_handle(tbac_root_device);
		if (list_handle)
			seq_printf(m, "Inode range set: %ld\n\n", rsbac_list_count(*list_handle));
	}
	{
		struct timespec	ts = current_kernel_time();
		time_t		hour = ts.tv_sec % 86400;
		int		day = (ts.tv_sec / 86400 + 3) % 7;
		char * 		inf = "WORKING_HOURS";

		seq_puts(m, "Current time information:\n");
		if (IN_SPARE_TIME(day, hour, ts.tv_sec))
			inf = "SPARE_TIME";
		seq_printf(m, "Day = %d, hour = %ld, time = %ld -> %s\n",
			day, hour, ts.tv_sec, inf);
	}
	return 0;
}

static int tbac_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, tbac_proc_show, NULL);
}

const struct file_operations tbac_proc_fops = {
	.owner          = THIS_MODULE,
	.open           = tbac_proc_open,
	.read           = seq_read,
	.llseek         = seq_lseek,
	.release        = single_release,
};

#endif
