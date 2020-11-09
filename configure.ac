AC_INIT([ldms-plugins-llnl],
        m4_esyscmd([config/git-version-gen --prefix '' .tarball-version]))
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall foreign])
AC_PROG_CC
AM_PROG_AR
LT_INIT

AC_ARG_ENABLE([dist],
  [AS_HELP_STRING([--with-dcgm],
    [Include plugin for Nvidia DCGM @<:@default=check@:>@])],
  [],
  [enable_dcgm=no])

AS_IF([test "x$enable_dist" != "xyes"],[


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

AC_LIB_HAVE_LINKFLAGS([ldms], [], [#include <ldms/ldms.h>])
AS_IF([test "x$HAVE_LIBLDMS" != xyes],
	[AC_MSG_ERROR([libldms of ldms/ldms.h not found])])

# ldms_set_publish/unplish are not exported until ovis v4
AC_CHECK_DECLS([ldms_set_publish],[],[],[#include <ldms/ldms.h>])

]) dnl not enable_dist

AM_CONDITIONAL([HAVE_DCGM], [test x$have_dcgm = xtrue])

AC_CONFIG_FILES([ldms-plugins-llnl.spec
                 Makefile
                 src/Makefile
                 man/Makefile])

AC_OUTPUT
