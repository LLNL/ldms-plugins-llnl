ACLOCAL_AMFLAGS = -I m4

COMMON_LIBADD = $(LTLIBLDMS) -lcoll
COMMON_LDFLAGS = -no-undefined \
        -export-symbols-regex 'get_plugin' \
        -version-info 1:0:0

noinst_LTLIBRARIES = libjobid_helper.la
libjobid_helper_la_SOURCES = jobid_helper.c jobid_helper.h
libjobid_helper_la_LIBADD = $(COMMON_LIBADD)

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
libllnl_lustre_client_la_LIBADD = libjobid_helper.la $(COMMON_LIBADD)
libllnl_lustre_client_la_LDFLAGS = $(COMMON_LDFLAGS)
ldmsplugin_LTLIBRARIES += libllnl_lustre_client.la

# lustre oss io plugin
libllnl_lustre_oss_io_la_SOURCES = \
	lustre_oss_io.c \
	lustre_oss_io.h \
libllnl_lustre_oss_io_la_LIBADD = libjobid_helper.la $(COMMON_LIBADD)
libllnl_lustre_oss_io_la_LDFLAGS = $(COMMON_LDFLAGS)
ldmsplugin_LTLIBRARIES += libllnl_lustre_oss_io.la

if HAVE_DCGM
# nvidia dcgm plugin
libllnl_dcgm_la_SOURCES = dcgm.c
libllnl_dcgm_la_LIBADD = libjobid_helper.la $(COMMON_LIBADD) -ldcgm
libllnl_dcgm_la_LDFLAGS = $(COMMON_LDFLAGS)
ldmsplugin_LTLIBRARIES += libllnl_dcgm.la
endif

if HAVE_INFINIBAND
libllnl_ibmad_la_SOURCES = ibmad.c
libllnl_ibmad_la_CPPFLAGS = -I /usr/include/infiniband
libllnl_ibmad_la_LIBADD = libjobid_helper.la $(COMMON_LIBADD) -libmad -libumad
libllnl_ibmad_la_LDFLAGS = $(COMMON_LDFLAGS)
ldmsplugin_LTLIBRARIES += libllnl_ibmad.la
endif
