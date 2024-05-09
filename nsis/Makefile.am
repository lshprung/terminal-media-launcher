EXTRA_DIST = installer.nsi.in

CLEANFILES = $(PACKAGE_NAME)-Setup.exe

.PHONY: installer
$(PACKAGE_NAME)-Setup.exe: installer.nsi
	makensis installer.nsi
installer: $(PACKAGE_NAME)-Setup.exe
