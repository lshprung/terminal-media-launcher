EXTRA_DIST = installer.nsi.in

CLEANFILES = *Setup.exe deps/* deps_luarocks/*

SEARCHPATH=$$PATH
DLLS = $(shell ntldd -R ../src/$(PACKAGE_NAME).exe | sed 's/.*=> //g' | grep -v "not found" | sed 's/ (.*)$$//g' | sed 's/\\/\\\\/g' | xargs cygpath -u | grep "$$MINGW_PREFIX")

.PHONY: installer
installer: installer.nsi
	mkdir -p deps deps_luarocks
	for file in $(DLLS); do install "$$file" "deps/$$(basename "$$file")"; done
	cp /"$$(pacman -Fl $$MINGW_PACKAGE_PREFIX-lua-luafilesystem | grep -io $${MSYSTEM}.* | grep lfs.dll)" deps_luarocks/
	makensis installer.nsi
