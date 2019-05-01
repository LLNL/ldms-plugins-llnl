ACLOCAL_AMFLAGS = -I m4

COMMON_LIBADD = -lldms -lcoll
COMMON_LDFLAGS = -no-undefined \
        -export-symbols-regex 'get_plugin' \
        -version-info 1:0:0

ldmsplugindir = ${libdir}/ovis-ldms
ldmsplugin_LTLIBRARIES =

# lustre ost plugin
libllnl_lustre_ost_la_SOURCES = \
	lustre_ost.c \
	lustre_ost_general.c \
	lustre_ost_job_stats.c \
	lustre_ost.h \
	lustre_ost_general.h \
	lustre_ost_job_stats.h
libllnl_lustre_ost_la_LIBADD = $(COMMON_LIBADD)
libllnl_lustre_ost_la_LDFLAGS = $(COMMON_LDFLAGS)
ldmsplugin_LTLIBRARIES += libllnl_lustre_ost.la

# lustre mdt plugin
libllnl_lustre_mdt_la_SOURCES = \
	lustre_mdt.c \
	lustre_mdt_general.c \
	lustre_mdt_job_stats.c \
	lustre_mdt.h \
	lustre_mdt_general.h \
	lustre_mdt_job_stats.h
libllnl_lustre_mdt_la_LIBADD = $(COMMON_LIBADD)
libllnl_lustre_mdt_la_LDFLAGS = $(COMMON_LDFLAGS)
ldmsplugin_LTLIBRARIES += libllnl_lustre_mdt.la

# lustre client plugin
libllnl_lustre_client_la_SOURCES = \
	lustre_client.c \
	lustre_client_general.c \
	lustre_client.h \
	lustre_client_general.h
libllnl_lustre_client_la_LIBADD = $(COMMON_LIBADD)
libllnl_lustre_client_la_LDFLAGS = $(COMMON_LDFLAGS)
ldmsplugin_LTLIBRARIES += libllnl_lustre_client.la
