#!/usr/bin/make -f
# Licensed under the terms of the GPLv2

#
# Configuration
#

VERSION		:= 0.1.1
PACKAGE		:= tbac-reg

CC		:= gcc
ifeq ($(ECHO),)
ECHO		:= echo
endif

PWD		:= $(shell pwd)
INCFLAGS	+= -I$(PWD)/../headers

ifneq ($(VERBOSE), 1)
E = @$(ECHO) -e " "
else
E = @:
endif


ifeq ($(PATCHLEVEL),4)
# 2.4.x

modules:
	@( echo "todo :)" )

else
# 2.6.x
tbac-objs	:= module.o syscalls.o adf.o data_structures.o helpers.o proc.o
obj-m		+= tbac.o

KERNELDIR	?= /lib/modules/$(shell uname -r)/build

modules:
	@$(MAKE) -C $(KERNELDIR) M=$(PWD) EXTRA_CFLAGS="$(INCFLAGS)" modules

modules_install:
	@$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install

endif

clean:
	$(E) "CLEAN\t\tModule objects"
	@rm -f *.o *~ core .dependt .*.cmd *.ko *.mod.c ../lib/*.o
	$(E) "CLEAN\t\tKernel tmp_versions and junk"
	@rm -Rf Module.symvers modules.order .tmp_versions

distclean: clean

install: modules_install

uninstall:
	rm -f /lib/modules/$(shell uname -r)/extra/tbac.ko

.PHONY: clean distclean install uninstall
