#FILES_TOOLS := $(wildcard src/*.c)
#PROGRAMS := src/tbac_get_attr_fd\
	src/tbac_set_attr_fd\
	src/tbac_get_conf\
	src/tbac_set_conf

SCRIPTS := src/scripts/rsbac_tbac_menu.py src/scripts/tbac_get_attr.py #src/scripts/tbac_set_attr.py

PROGS_USR_BIN := $(PROGRAMS) $(SCRIPTS)
