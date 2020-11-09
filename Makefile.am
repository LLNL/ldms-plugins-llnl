EXTRA_DIST = $(top_srcdir)/.version config.rpath ldms-plugins-llnl.spec COPYRIGHT LICENSE-BSD LICENSE-GPL NOTICE README.md
SUBDIRS = src man

BUILT_SOURCES = $(top_srcdir)/.version
$(top_srcdir)/.version:
	echo $(VERSION) > $@-t && mv $@-t $@
dist-hook:
	echo $(VERSION) > $(distdir)/.tarball-version
