#!/usr/bin/make -f
# Licensed under the terms of the GPLv2
# Originally written by Guillaume Destuynder <kang@rsbac.org>

#
# Configuration
#

VERSION         := 0.1.1
PACKAGE         := tbac-tools

INSTALL         := install
MSGFMT          := msgfmt
STRIP           := strip
CC              := gcc
GZIP            := gzip
CP              := cp
ECHO            := $(shell which echo)
ifeq ($(ECHO),)
ECHO            := echo
endif

DESTDIR         :=
PREFIX          := /usr/local
LOCALEDIR       := $(PREFIX)/share/locale
DIR_PO          := fr
DIR_DOC         := $(PREFIX)/share/doc/$(PACKAGE)-$(VERSION)
DIR_BIN         := $(PREFIX)/bin
DIR_RBIN        := /bin

NLS             := 0

CFLAGS          := -fPIC -O2 -fomit-frame-pointer -Wall -Wextra
CFLAGS          += -Isrc -I../headers -I/usr/include -I/usr/local/include \
                   -I$(PREFIX)/include
LDFLAGS         :=
DEFINES         := -DPACKAGE=\"$(PACKAGE)\" \
                   -DVERSION=\"$(VERSION)\" \
                   -DLOCALEDIR=\"$(LOCALEDIR)\" \
                   -DENABLE_NLS=$(NLS)

LIBS            := -L../libs/.libs -L$(PREFIX)/lib -ltbac -lrsbac

QUIET           :=  > /dev/null 2>&1

include module.mk

#FILES_MAN       := $(wildcard man/*1)
FILES_PO        := $(wildcard po/*.po)
#DIR_EXAMPLE     := examples
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

all: $(FILES_TOOLS:.c=) $(FILES_PO:.po=.mo)

$(FILES_TOOLS:.c=): % : %.c
	$(E) "CC\t\t$@"
	$(CC) $(CFLAGS) $(LDFLAGS) $(DEFINES) $(LIBS) $@.c -o $@

$(FILES_PO:.po=.mo): $(FILES_PO)
ifeq ($(NLS), 1)
	$(foreach f, $(FILES_PO), $(ECHO) -e "  PO\t\t$(f)"; \
	$(MSGFMT) -o $(f:.po=.mo) $(f);)
else
	@touch $@
endif

clean:
	$(foreach f, $(FILES_TOOLS:.c=), $(ECHO) -e "  CLEAN\t\t$(f)"; \
	rm -f $(f);)
	$(E) "CLEAN\t\t$(FILES_PO:.po=.mo)"
	rm -f $(FILES_PO:.po=.mo)

distclean: clean
	$(E) "CLEAN\t\t$(FILES_PO:.po=.po~)"
	rm -f $(FILES_PO:.po=.po~)
#	$(E) "CLEAN\t\t$(FILES_MAN:.1=.1.gz)"
#	rm -f $(FILES_MAN:.1=.1.gz)

install: all
	$(E) "INTO\t\t$(DESTDIR) ($(PREFIX))"
ifeq ($(NLS), 1)
	$(E) "DIR\t\t$(LOCALEDIR)"
	$(foreach f, $(DIR_PO), $(INSTALL) -d $(DESTDIR)/$(LOCALEDIR)/$(f)/LC_MESSAGES; \
	$(INSTALL) -d $(DESTDIR)/$(LOCALEDIR)/$(f)/LC_TIME;)
	$(E) "INSTALL\t$(DIR_PO)"
	$(foreach f, $(DIR_PO), \
	$(INSTALL) -m644 po/$(f).mo \
	$(DESTDIR)/$(LOCALEDIR)/$(f)/LC_MESSAGES/$(PACKAGE).mo; \
	$(INSTALL) -m644 po/$(f).mo \
	$(DESTDIR)/$(LOCALEDIR)/$(f)/LC_MESSAGES/$(PACKAGE)-$(VERSION).mo;)
endif
	$(foreach f, $(PROGS_USR_BIN), $(ECHO) -e "  INSTALL\t$(f)"; \
	$(INSTALL) -m755 $(f) $(DESTDIR)/$(DIR_BIN);)
	#$(E) "INSTALL\t\tAUTHORS INSTALL README COPYING Changes"
	#$(INSTALL) -m644 AUTHORS INSTALL README COPYING Changes $(DESTDIR)/$(DIR_DOC)

install-strip: install
	$(foreach f,  $(subst scripts/,,$(subst src/,,$(PROGS_USR_BIN))), $(ECHO) -e "  STRIP\t\t$(f)"; \
	$(STRIP) -s $(DESTDIR)/$(DIR_BIN)/$(f);) \

uninstall:
ifeq ($(NLS), 1)
	$(foreach f, $(DIR_PO), $(ECHO) -e "  UNINSTALL\t$(f)"; rm -f po/$(f).mo \
	$(DESTDIR)/$(LOCALEDIR)/$(f)/LC_MESSAGES/$(PACKAGE).mo;)
endif
	$(foreach f, $(subst scripts/,,$(subst src/,,$(PROGS_USR_BIN))), $(ECHO) -e "  UNINSTALL\t$(f)"; \
	rm -f $(DESTDIR)/$(DIR_BIN)/$(f);)
	#$(E) "UNINSTALL\t$(FILES_MAN)"
	#rm -f $(DESTDIR)/$(DIR_MAN)/$(FILES_MAN)

#
# gettext translation stuff
#

# Re-create the messages.po file
nls-messages:
	$(E) "XGETTEXT\t\tGenerating po/messages.tpo"
	xgettext -p po -o messages.tpo src/*.c

# Merges translations
nls-merge-keep: $(FILES_PO)
	$(foreach f, $(FILES_PO), $(ECHO) -e "  MSGMERGE\t\tMerging $(f)"; \
	msgmerge -o $(f).new $(f) po/messages.tpo $(QUIET);)
	$(E) "MSGMERGE\t\tGenerated: $(FILES_PO:.po=.po.new)"

nls-merge-update: $(FILES_PO)
	$(foreach f, $(FILES_PO), $(ECHO) -e "  MSGMERGE\t\tMerging $(f)"; \
	msgmerge -U $(f) po/messages.tpo $(QUIET);)
	$(E) "MSGMERGE\t\tUpdated: $(FILES_PO)"


.PHONY: all clean distclean uninstall install nls-messages

