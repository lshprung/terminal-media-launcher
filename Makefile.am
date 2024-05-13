SUBDIRS = src docs data nsis spec
dist_man_MANS = man
man1_MANS = man/$(PACKAGE_NAME).1
man5_MANS = man/$(PACKAGE_NAME)-config.5
EXTRA_DIST = debian

# remove everything, such that autoreconf -ivf must be run again
full-clean: maintainer-clean
	-rm -rf aclocal.m4
	-rm -rf compile
	-rm -rf config.*
	-rm -rf configure
	-rm -rf depcomp
	-rm -rf install-sh
	-rm -rf Makefile.in
	-rm -rf */Makefile.in
	-rm -rf missing
