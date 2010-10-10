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
"""	Filename:	main/libs/python-binding/tbac.py
	Project:	tbac-reg
	Last update:	2009/06/11
	Purpose:	Tbac python library entrypoint
"""

from _tbac import *

class TypesDict(dict):
	def get_name(self, valeur):
		for k,v in self.items():
			if v == valeur:
				return k
		return "Unknown"

class Flags(TypesDict):
	def __init__(self):
		TypesDict.__init__(self, {
			'UNSET': UNSET,
			'WORKING_HOURS': WORKING_HOURS,
			'SPARE_HOURS': SPARE_HOURS,
			'RANGE': RANGE,
			'SINCE': SINCE
		})

flags = Flags()
