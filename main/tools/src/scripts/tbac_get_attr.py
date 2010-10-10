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
"""	Filename:	main/tools/src/scripts/tbac_get_attr.py
	Project:	tbac-reg
	Last update:	2009/06/11
	Purpose:	Get attr command line tools
"""
import tbac

def main():
	import sys

	if len(sys.argv) < 3:
		print "usage: %s flags|range file/dirname(s)"
		return -1
	for path in sys.argv[2:]:
		if sys.argv[1].lower() == "flags":
			val = tbac.get_flags(path)
		else:
			val = tbac.get_range(path)
		print "%s: returned '%s'" % (path, val)


if __name__ == "__main__":
	main()
