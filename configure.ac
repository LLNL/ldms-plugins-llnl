AC_INIT([ldms-plugins-llnl], [1.5])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall foreign])
AC_PROG_CC
AM_PROG_AR
LT_INIT

# ldms_set_publish/unplish are not exported until ovis v4
AC_CHECK_DECLS([ldms_set_publish],[],[],[#include <ldms/ldms.h>])

AC_ARG_WITH([dcgm],
  [AS_HELP_STRING([--with-dcgm],
    [Include plugin for Nvidia DCGM @<:@default=check@:>@])],
  [],
  [with_dcgm=check])
LIBDCGM=
AS_IF([test "x$with_dcgm" != xno],
      [AC_CHECK_LIB([dcgm], [dcgmInit],
         [have_dcgm=true],
         [have_dcgm=false
          if test "x$with_dcgm" != xcheck; then
            AC_MSG_FAILURE([--with-dcgm was given, but test for dcgm failed])
          fi
         ])],
      [have_dcgm=false])
AM_CONDITIONAL([HAVE_DCGM], [test x$have_dcgm = xtrue])

AC_CONFIG_FILES([ldms-plugins-llnl.spec
                 Makefile
                 src/Makefile
                 man/Makefile])

AC_OUTPUT
