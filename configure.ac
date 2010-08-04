
dnl -*- autoconf -*-
dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(xml_data_handler, 1.0.0, opendap-tech@opendap.org)
AC_CONFIG_AUX_DIR([conf])
AC_CONFIG_MACRO_DIR([conf])

AM_INIT_AUTOMAKE(1.10)
AC_CONFIG_SRCDIR([BESXDModule.cc])
AM_CONFIG_HEADER([config.h])

AC_CONFIG_TESTDIR(tests, [.])

dnl flags for the compilers and linkers - set these before locating the
dnl actual tools since some of the AC_PROG macros set these `flag variables'
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
AC_CHECK_LIBDAP([3.10.2],
 [
  LIBS="$LIBS $DAP_CLIENT_LIBS $DAP_SERVER_LIBS"
  CPPFLAGS="$CPPFLAGS $DAP_CFLAGS"
 ],
 [ AC_MSG_ERROR([Cannot find libdap])
])

AC_CHECK_BES([3.8.3],
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

AC_DEFINE_UNQUOTED([ENABLE_UNIT_TESTS], $UNIT_TESTS_VAL, [Should the code be compiled to suport unit tests?])
    
dnl autoheader macros; tack some text at the top and bottom of config.h.in
AH_TOP([#ifndef _config_h
#define _config_h])

AH_BOTTOM([/* Shorthand for gcc's unused attribute feature */
#if defined(__GNUG__) || defined(__GNUC__)
#define not_used __attribute__ ((unused))
#else
#define not_used 
#endif /* __GNUG__ || __GNUC__ */

#endif /* _config_h */])

AC_CONFIG_FILES([Makefile
                 unit-tests/Makefile
                 tests/Makefile
                 tests/atlocal])

AC_OUTPUT
