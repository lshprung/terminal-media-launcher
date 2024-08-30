EXTRA_DIST = installer.nsi.in

CLEANFILES = *Setup.exe deps/*

SEARCHPATH=$$PATH
DLLS = $(shell ntldd -R ../src/$(PACKAGE_NAME).exe | sed 's/.*=> //g' | grep -v "not found" | sed 's/ (.*)$$//g' | sed 's/\\/\\\\/g' | xargs cygpath -u | grep "$$MINGW_PREFIX")

.PHONY: installer
installer: installer.nsi
	mkdir -p deps
	for file in $(DLLS); do install "$$file" "deps/$$(basename "$$file")"; done
	makensis installer.nsi
