#
# Nice make. adapted from rsbac-admin package
#
ifneq ($(VERBOSE), 1)
E = @echo
else
E = @:
endif

all:
	@echo "Welcome to TBAC!"
	@echo "To compile all packages, type:"
	@echo "\`\`make build'"
	@echo 
	@echo "To install all packages, as administrator user, type:"
	@echo "\`\`make install'"
	@echo 
	@echo "To uninstall all packages, as administrator user, type:"
	@echo "\`\`make uninstall'"
	@echo 
	@echo "To compile or install per package, type:"
	@echo "\`\`make <package>' or \`\`make <package>-install'"
	@echo 
	@echo "with package one of:"
	@echo 
	@echo " --------------------------- "
	@echo "|   headers reg libs tools  |"
	@echo " --------------------------- "
	@echo 
	@echo "Or change to the right directory and use the traditional"
	@echo "make; su; make install"
	@echo
	@echo "Type make <target> VERBOSE=1 to display compilation lines."
	@echo "Type make <target> TBAC_DEBUG=1 to include *verbose* debug."
	@echo
	@echo "Be sure to read the \`\`README' file for more information."
	@echo 

build: reg libs tools

install: headers-install reg-install libs-install tools-install

uninstall: tools-uninstall libs-uninstall reg-uninstall headers-uninstall

clean: tools-clean reg-clean libs-clean

distclean : clean

# Display function
E = @echo;echo "$(1)";echo " -------------------------------------"


#
# Building
#

reg:
	$(call E, "Building TBAC REG module...")
	@$(MAKE) --no-print-directory -C main/reg

libs:
	$(call E, "Building TBAC libraries...")
	@$(MAKE) --no-print-directory -C main/libs

tools:
	$(call E, "Builing TBAC tools...")
	@$(MAKE) --no-print-directory -C main/tools


#
# Installations
#

headers-install:
	$(call E, "Installing TBAC headers...")
	@$(MAKE) install --no-print-directory -C main/headers

reg-install:
	$(call E, "Installing TBAC REG module...")
	@$(MAKE) install --no-print-directory -C main/reg

libs-install: headers-install
	$(call E, "Installing TBAC libraries...")
	@$(MAKE) install --no-print-directory -C main/libs

tools-install: libs-install
	$(call E, "Installing TBAC tools...")
	@$(MAKE) install --no-print-directory -C main/tools


#
# Uninstallations
#

headers-uninstall:
	$(call E, "Uninstalling TBAC headers...")
	@$(MAKE) uninstall --no-print-directory -C main/headers

reg-uninstall:
	$(call E, "Uninstalling TBAC REG module...")
	@$(MAKE) uninstall --no-print-directory uninstall -C main/reg

libs-uninstall:
	$(call E, "Uninstalling TBAC libraries...")
	@$(MAKE) uninstall --no-print-directory uninstall -C main/libs

tools-uninstall: libs-uninstall
	$(call E, "Uninstalling TBAC tools...")
	@$(MAKE) uninstall --no-print-directory uninstall -C main/tools


#
# Cleaning
#

reg-clean:
	$(call E, "Cleaning TBAC REG module...")
	@$(MAKE) distclean --no-print-directory -C main/reg

libs-clean:
	$(call E, "Cleaning TBAC libraries...")
	@$(MAKE) distclean --no-print-directory -C main/libs

tools-clean:
	$(call E, "Cleaning TBAC tools...")
	@$(MAKE) distclean --no-print-directory -C main/tools

.PHONY: all build clean distclean
