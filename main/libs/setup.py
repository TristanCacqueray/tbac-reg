#!/usr/bin/python -OO
# -*- coding: utf8 -*-
############################################################################
# (c) 2005-2010 freenode#rsbac
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
############################################################################
"""	Filename:	main/libs/setup.py
	Project:	tbac-reg
	Last update:	2009/06/11
	Purpose:	Library distutils setup
"""
import os
from distutils.core import setup, Extension

os.environ["CFLAGS"] = "-Wall"
setup(
	name = "tbac",
	version = "0.0.1",
	py_modules = ['tbac'],
	package_dir = {'': 'python-binding'},
	ext_modules=[Extension(
		"_tbac",
		include_dirs = ["../headers"],
		library_dirs = [".libs"],
		libraries = [ 'rsbac', 'tbac' ],
		sources = ["python-binding/tbac_module.c"]
	)],
)
