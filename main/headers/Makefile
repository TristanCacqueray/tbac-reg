#!/usr/bin/make -f
# Licensed under the terms of the GPLv2
# Originally written by Guillaume Destuynder <kang@rsbac.org>

#
# Configuration
#

INSTALL         := install
ECHO            := $(shell which echo)
ifeq ($(ECHO),)
ECHO            := echo
endif

DESTDIR         := 
PREFIX          := /usr/local
DIR_INC         := $(PREFIX)/include/tbac


FILES_HDR       := $(wildcard tbac/*.h)

#
# Nice make. Use make VERBOSE=1 to verbose compilation.
#
ifneq ($(VERBOSE), 1)
.SILENT:
E = @$(ECHO) -e " "
else
E = @:
endif

#
# Targets
#

all: install

clean:

distclean: clean

install: $(FILES_HDR)
	$(E) "INTO\t\t$(DESTDIR) ($(PREFIX))"
	$(E) "DIR\t\t $(DESTDIR)/$(DIR_INC)"
	$(INSTALL) -d $(DESTDIR)/$(DIR_INC)
	$(foreach f, $(FILES_HDR), $(ECHO) -e "  INSTALL\t$(shell basename $(f))"; \
	$(INSTALL) -m644 $(f) $(DESTDIR)/$(DIR_INC);)

uninstall:
	$(foreach f, $(FILES_HDR), $(ECHO) -e "  UNINSTALL\t$(f)"; \
	rm $(DESTDIR)/$(DIR_INC)/$(shell basename $(f));)


.PHONY: all install uninstall clean distclean

