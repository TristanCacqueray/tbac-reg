/**************************************/
/* Timed Based Access Control         */
/* Author and (c) 2009-2010: GCM SARL */
/* libs/python-binding/tbac_module.c  */
/* Last modified: 11/Jun/2009         */
/**************************************/

#include <Python.h>
#include <rsbac/getname.h>
#include <tbac/types.h>
#include <tbac/syscalls.h>
#include <tbac/helpers.h>

static PyObject *handle_error(int err)
{
	char			tmp[RSBAC_MAXNAMELEN];

	PyErr_SetString(PyExc_OSError, get_error_name(tmp, err));
	return NULL;
}

/*
 * All wrappers raise exception on error, and return results for getters
 */
static PyObject *get_flags(PyObject *self, PyObject *args)
{
	char *			path;
	rsbac_tbac_flags_t	flags;
	int			err;
	char 			tmp[RSBAC_MAXNAMELEN];

	if (!PyArg_ParseTuple(args, "s", &path))
		return NULL;

	err = tbac_get_flags(path, &flags);
	if (err < 0)
		return handle_error(err);
	return Py_BuildValue("s", get_tbac_flags_name(tmp, flags));
}

static PyObject *set_flags(PyObject *self, PyObject *args)
{
	char *			path;
	rsbac_tbac_flags_t	flags;
	int			err;

	if (!PyArg_ParseTuple(args, "si", &path, &flags))
		return NULL;

	err = tbac_set_flags(path, flags);
	if (err < 0)
		return handle_error(err);
	return Py_None;
}

static PyObject *get_range(PyObject *self, PyObject *args)
{
	char *			path;
	rsbac_tbac_range_t	range;
	int			err;
	PyObject *		ret;
	char 			tmp[RSBAC_MAXNAMELEN];

	if (!PyArg_ParseTuple(args, "s", &path))
		return NULL;

	err = tbac_get_range(path, &range);
	if (err < 0)
		return handle_error(err);
	ret = PyTuple_New(2);
	PyTuple_SetItem(ret, 0, PyString_FromString(format_date(tmp, range.min)));
	PyTuple_SetItem(ret, 1, PyString_FromString(format_date(tmp, range.max)));
	return ret;
}

static PyObject *set_range(PyObject *self, PyObject *args)
{
	char *			path;
	rsbac_tbac_range_t	range;
	int			err;

	if (!PyArg_ParseTuple(args, "sll", &path, &range.min, &range.max))
		return NULL;

	err = tbac_set_range(path, range);
	if (err < 0)
		return handle_error(err);
	return Py_None;
}

static PyObject *get_working_hours(PyObject *self, PyObject *args)
{
	time_t			morning;
	time_t			evening;
	int			err;
	PyObject *		ret;
	char 			tmp[RSBAC_MAXNAMELEN];

	err = tbac_get_working_hours(&morning, &evening);
	if (err < 0)
		return handle_error(err);
	ret = PyTuple_New(2);
	PyTuple_SetItem(ret, 0, PyString_FromString(format_hour(tmp, morning)));
	PyTuple_SetItem(ret, 1, PyString_FromString(format_hour(tmp, evening)));
	return ret;
}

static PyObject *set_working_hours(PyObject *self, PyObject *args)
{
	time_t			morning;
	time_t			evening;
	int			err;

	if (!PyArg_ParseTuple(args, "ll", &morning, &evening))
		return NULL;

	err = tbac_set_working_hours(morning, evening);
	if (err < 0)
		return handle_error(err);
	return Py_None;
}

static PyObject *get_oneshot_holiday(PyObject *self, PyObject *args)
{
	time_t			holiday;
	int			err;
	char 			tmp[RSBAC_MAXNAMELEN];

	err = tbac_get_oneshot_holiday(&holiday);
	if (err < 0)
		return handle_error(err);
	return Py_BuildValue("s", format_date(tmp, holiday));
}

static PyObject *set_oneshot_holiday(PyObject *self, PyObject *args)
{
	time_t			holiday;
	int			err;

	if (!PyArg_ParseTuple(args, "l", &holiday))
		return NULL;

	err = tbac_set_oneshot_holiday(holiday);
	if (err < 0)
		return handle_error(err);
	return Py_None;
}

static PyMethodDef TbacMethods[] = {
	{"set_flags",  set_flags, METH_VARARGS,
		"set_flags(path, flag)\nSet tbac flags on a path."},
	{"get_flags",  get_flags, METH_VARARGS,
		"get_flags(path) -> tbac flag name\nGet tbac flags for a path."},
	{"set_range",  set_range, METH_VARARGS,
		"set_range(path, range.min, range.max)\nSet tbac range on a path."},
	{"get_range",  get_range, METH_VARARGS,
		"get_range(path) -> range.min, range.max\nGet tbac range on a path."},
	{"set_oneshot_holiday",	set_oneshot_holiday,	METH_VARARGS,
		"set_oneshot_holiday(time)\nSet TBAC oneshot holiday."},
	{"get_oneshot_holiday",	get_oneshot_holiday,	METH_VARARGS,
		"set_oneshot_holiday() -> time\nGet TBAC oneshot holiday."},
	{"set_working_hours",  	set_working_hours,	METH_VARARGS,
		"set_working_hours(morning, evening)\nSet TBAC working hours."},
	{"get_working_hours",  get_working_hours,	METH_VARARGS,
		"get_working_hours() -> morning, evening\nGet TBAC working hours."},
	{NULL, NULL, 0, NULL}        /* Sentinel */
};

static PyObject *TBAC_UNSET;
static PyObject *TBAC_WORKING_HOURS;
static PyObject *TBAC_SPARE_HOURS;
static PyObject *TBAC_RANGE;
static PyObject *TBAC_SINCE;

/*
 * m : the module
 * c : the python constant variable
 * name : the python constant name
 * flag : the effective flag
 */
void init_constant(PyObject *m, PyObject *c, char * name, rsbac_tbac_flags_t flag)
{
	c = Py_BuildValue("i", flag);
	Py_INCREF(c);
	PyModule_AddObject(m, name, c);
}

PyMODINIT_FUNC init_tbac(void)
{
	PyObject *m;
        m = Py_InitModule("_tbac", TbacMethods);

	init_constant(m, TBAC_UNSET, "UNSET", TBAC_unset);
	init_constant(m, TBAC_WORKING_HOURS, "WORKING_HOURS", TBAC_workinghours);
	init_constant(m, TBAC_SPARE_HOURS, "SPARE_HOURS", TBAC_sparehours);
	init_constant(m, TBAC_RANGE, "RANGE", TBAC_range);
	init_constant(m, TBAC_SINCE, "SINCE", TBAC_since);
}
