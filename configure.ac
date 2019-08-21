AC_INIT([ldms-plugins-llnl], [1.1])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall foreign])
AC_PROG_CC
AM_PROG_AR
LT_INIT

# ldms_set_publish/unplish are not exported until ovis v4
AC_CHECK_DECLS([ldms_set_publish],[],[],[#include <ldms/ldms.h>])

AC_CONFIG_FILES([ldms-plugins-llnl.spec
                 Makefile
                 src/Makefile
                 man/Makefile])

AC_OUTPUT
