EXTRA_DIST = find_dlls.sh installer.nsi.in

CLEANFILES = $(PACKAGE_NAME)-Setup.exe deps/*

SEARCHPATH=$$PATH
DEPENDENCIES = $(shell ./find_dlls.sh -q -s $(SEARCHPATH) '../src/$(PACKAGE_NAME).exe')

.PHONY: installer
installer: $(PACKAGE_NAME)-Setup.exe
$(PACKAGE_NAME)-Setup.exe: installer.nsi
	mkdir -p deps
	for file in $(DEPENDENCIES); do install "$$file" "deps/$$(basename "$$file")"; done
	makensis installer.nsi
