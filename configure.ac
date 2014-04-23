
dnl -*- autoconf -*-
dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.62)
AC_INIT(xml_data_handler, 1.0.4, opendap-tech@opendap.org)
AC_CONFIG_AUX_DIR([conf])
AC_CONFIG_MACRO_DIR([conf])

AM_INIT_AUTOMAKE(1.10)

# The handler is a shared object library, so we don't normally need
# the static lib. Override this default by specifying `--enable-static',
# which can make running unit tests with valgrind far more pleasant. 
AM_DISABLE_STATIC

AC_CONFIG_SRCDIR([BESXDModule.cc])
AM_CONFIG_HEADER([config.h])

AC_CONFIG_TESTDIR(tests, [.])

dnl flags for the compilers and linkers - set these before locating the
dnl actual tools since some of the AC_PROG macros set these 'flag variables'
dnl to default values otherwise.

AC_CANONICAL_HOST
AC_SUBST(host)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdlib.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([putenv strtol])

dnl Checks for specific libraries
AC_CHECK_LIBDAP([3.13.0],
 [
  LIBS="$LIBS $DAP_SERVER_LIBS"
dnl $DAP_CLIENT_LIBS 
  CPPFLAGS="$CPPFLAGS $DAP_CFLAGS"
 ],
 [ AC_MSG_ERROR([Cannot find libdap])
])

AC_CHECK_BES([3.13.0],
[
],
[ AC_MSG_ERROR([Cannot find bes])
])

AM_PATH_CPPUNIT(1.12.0,
	[AM_CONDITIONAL([CPPUNIT], [true])],
	[AM_CONDITIONAL([CPPUNIT], [false])])

dnl I wrote these checks because we need the *-config scripts to build, so 
dnl the AC_CHECK_LIB macro is not needed.

AC_PATH_PROG([CURL], [curl], [*** Where is curl? ***], [/bin:/usr/bin])

OPENDAP_DEBUG_OPTION

AC_ARG_ENABLE(unit-tests, 
    [  --enable-unit-tests     On by default (yes|no)],
    [case "$enableval" in
        no)
          AC_MSG_NOTICE(Unit tests disabled)
           UNIT_TESTS_VAL=0
           ;;
         yes)
          AC_MSG_NOTICE(Unit tests enabled)
          UNIT_TESTS_VAL=1
          ;;
         *)
          AC_MSG_ERROR(Bad unit tests value, use 'yes' or 'no')
          ;;
     esac],
     [UNIT_TESTS_VAL=1
     AC_MSG_NOTICE(Unit tests enabled)])

AC_DEFINE_UNQUOTED([ENABLE_UNIT_TESTS], $UNIT_TESTS_VAL, [Should the code be compiled to support unit tests?])
    
dnl autoheader macros; tack some text at the top and bottom of config.h.in
AH_TOP([#ifndef _config_h
#define _config_h])

AH_BOTTOM([/* Shorthand for gccs unused attribute feature */
#if defined(__GNUG__) || defined(__GNUC__)
#define not_used __attribute__ ((unused))
#else
#define not_used 
#endif /* __GNUG__ || __GNUC__ */

#endif /* _config_h */])

# Test for a readlink bug, see
# <http://lists.gnu.org/archive/html/bug-coreutils/2008-02/msg00126.html>.
# We use perl to replace 'readlink -f' if it doesn't exist...
AC_CHECK_PROG(PERL, perl, perl)
AC_MSG_CHECKING([for a suitable ``readlink -f'' program])
[mkdir readlink-test &&
mkdir readlink-test/a-long &&
: > readlink-test/a-long/f &&
ln -s a-long readlink-test/a &&
for READLINK_F in \
  'readlink -f' \
  realpath \
  false
do
  if
    $READLINK_F > /dev/null 2>&1 \
      readlink-test/a/../a/f 
  then
    break
  fi
done &&
rm -rf readlink-test &&
if [ "$READLINK_F" = false ]
then]
  AC_MSG_RESULT([none found, using own replacement])
  [READLINK_F='perl -e '\''use Cwd qw(abs_path); print abs_path $$ARGV[0];'\'
else]
  AC_MSG_RESULT([$READLINK_F])
[fi]
AC_SUBST([READLINK_F])

AC_CONFIG_FILES([Makefile
                 unit-tests/Makefile
		         unit-tests/test_config.h
                 tests/Makefile
                 tests/atlocal])

AC_CONFIG_FILES([tests/generate_metadata_baseline.sh],
	[chmod +x tests/generate_metadata_baseline.sh])
AC_CONFIG_FILES([tests/generate_data_baseline.sh],
	[chmod +x tests/generate_data_baseline.sh])

AC_OUTPUT
