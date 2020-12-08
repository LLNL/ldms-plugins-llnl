ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST = $(top_srcdir)/.version ldms-plugins-llnl.spec COPYRIGHT LICENSE-BSD LICENSE-GPL NOTICE README.md
EXTRA_DIST += build-aux/config.rpath
EXTRA_DIST += m4/gnulib-cache.m4
SUBDIRS = lib src man

BUILT_SOURCES = $(top_srcdir)/.version
$(top_srcdir)/.version:
	echo $(VERSION) > $@-t && mv $@-t $@
dist-hook:
	echo $(VERSION) > $(distdir)/.tarball-version
