#!/usr/bin/make -f
# Licensed under the terms of the GPLv2
# Originally written by Guillaume Destuynder <kang@rsbac.org>

#
# Configuration
#

VERSION		:= 0.1.1
PACKAGE         := tbac-tools
LIBVERSION 	:= 1

INSTALL		:= install
LIBTOOL 	:= libtool
CC		:= gcc
ECHO		:= $(shell which echo)
ifeq ($(ECHO),)
ECHO		:= echo
endif
HOST		:= $(shell uname -m)

DESTDIR		:= 
PREFIX		:= /usr/local
ifeq ($(DIR_LIBS),)
ifeq ($(HOST), x86_64)
LIBDIR		:= $(PREFIX)/lib64
else
LIBDIR		:= $(PREFIX)/lib
endif
else
LIBDIR		:= $(DIR_LIBS)
endif
LOCALEDIR	:= $(PREFIX)/share/locale

TBACLIB 	:= libtbac.la
PYBINDING	:= _tbac.so

NLS		:= 1

CFLAGS		:= -fPIC -O2 -fomit-frame-pointer -Wall
CFLAGS		+= -I../headers -I/usr/include -I/usr/local/include  \
		   -I$(PREFIX)/include
LDFLAGS		:=
DEFINES		:= -DPACKAGE=\"$(PACKAGE)\" \
                   -DVERSION=\"$(VERSION)\" \
		   -DLOCALEDIR=\"$(LOCALEDIR)\"
		   -DENABLE_NLS=$(NLS)

QUIET 		:= > /dev/null

LIBS		:= -L$(PREFIX)/lib #-lrsbac

FILES_LIBS	:= $(wildcard src/*.c)
PYFILES		:= $(wildcard python-binding/*.py)

#
# Nice make. Use make VERBOSE=1 to verbose compilation.
#
ifneq ($(VERBOSE), 1)
.SILENT:
E = @$(ECHO) -e " "
else
QUIET =
E = @:
endif

#
# Targets
#

all: $(TBACLIB) $(PYBINDING)

$(TBACLIB): $(FILES_LIBS)
	$(foreach f, $(FILES_LIBS), $(ECHO) -e "  CC\t\t$(f)"; \
	$(LIBTOOL) --tag=CC --mode=compile $(CC) $(CFLAGS) $(LDFLAGS) $(DEFINES) -c $(f) $(QUIET) -o $(f:.c=.lo);)
	$(E) "LD\t\t$(TBACLIB)"
	$(LIBTOOL) --tag=CC --mode=link $(CC) $(CFLAGS) $(LDFLAGS) $(DEFINES) $(LIBS) \
	-o $@ $(FILES_LIBS:.c=.lo) -rpath $(LIBDIR) -version-info $(LIBVERSION) \
	$(QUIET)

$(PYBINDING): $(TBACLIB) $(PYFILES)
	$(E) "BINDING\t$(PYBINDING)"
	@python setup.py build
	@cp build/lib*/$(PYBINDING) .

clean:
	$(E) "CLEAN\t\t$(TBACLIB) librsbac.a"
	@rm -f $(TBACLIB) libtbac.a
	$(E) "CLEAN\t\tLibtool objects"
	@rm -rf $(FILES_LIBS:.c=.lo) $(FILES_LIBS:.c=.o) src/.libs
	@rm -rf .libs
	$(E) "CLEAN\t\t$(PYBINDING) BINDING..."
	@rm -rf build 
	@rm -f $(PYBINDING)

distclean: clean

install: all
	$(E) "INTO\t\t$(DESTDIR) ($(PREFIX))"
	$(E) "DIR\t\t$(LIBDIR)"
	$(INSTALL) -d $(DESTDIR)/$(LIBDIR)
	$(E) "INSTALL\t$(TBACLIB)"
	$(LIBTOOL) --mode=install install -c $(TBACLIB) \
	$(DESTDIR)/$(LIBDIR)/$(TBACLIB) $(QUIET)
	$(E) "LIBTOOL\t$(TBACLIB)"
	$(LIBTOOL) -n --mode=finish $(DESTDIR)/$(LIBDIR) $(QUIET)
	$(E) "BINDING\t$(PYBINDING) tbac.py..."
	@python setup.py install --prefix=$(DESTDIR)/$(PREFIX) --record=python-installed-files.log

uninstall:
	$(E) "UNINSTALL\t $(LIBDIR)/$(TBACLIB) \
	$(LIBDIR)/libtbac.so $(LIBDIR)/libtbac.a"
	$(LIBTOOL) --mode=uninstall rm \
	$(DESTDIR)/$(LIBDIR)/$(TBACLIB) $(QUIET)
	$(foreach f, $(shell cat python-installed-files.log) python-installed-files.log, \
	$(ECHO) -e "  UNINSTALL\t $(f)"; rm -f $(f);)

.PHONY: all clean distclean
