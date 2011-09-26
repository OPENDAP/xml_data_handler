
# Tests

AUTOMAKE_OPTIONS = foreign

# Arrange to build with the backward compatibility mode enabled.
AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/asciival $(BES_CPPFLAGS)

AM_LDADD = 
AM_CXXFLAGS = 

if CPPUNIT
AM_CPPFLAGS += $(CPPUNIT_CFLAGS)
AM_LDADD += $(CPPUNIT_LIBS)
endif

# These are not used by automake but are often useful for certain types of
# debugging. Set CXXFLAGS to this in the nightly build using export ...
CXXFLAGS_DEBUG = -g3 -O0 -fno-defer-pop -Wall -W -Wcast-align -Werror
TEST_COV_FLAGS = -ftest-coverage -fprofile-arcs

# This header file is used for parse files
noinst_HEADERS = test_config.h

check_PROGRAMS = $(UNIT_TESTS)

TESTS = $(UNIT_TESTS)

EXTRA_DIST = testsuite test_config.h.in

CLEANFILES = 

DISTCLEANFILES = *.Po test_config.h

############################################################################
# Unit Tests
#

test_config.h: test_config.h.in Makefile
	sed -e "s%[@]srcdir[@]%${srcdir}%" $< > test_config.h

if CPPUNIT
UNIT_TESTS = XDArrayTest XDOutputTest
else
UNIT_TESTS =

check-local:
	@echo ""
	@echo "**********************************************************"
	@echo "You must have cppunit 1.12.x or greater installed to run *"
	@echo "check target in ppt unit-tests directory                 *"
	@echo "**********************************************************"
	@echo ""
endif

XDArrayTest_SOURCES = XDArrayTest.cc
XDOutputTest_SOURCES = XDOutputTest.cc

# if not linking with the module, use $(XDOBJS) below
XDArrayTest_LDADD = -L.. -lxml_data_module $(BES_DAP_LIBS) $(AM_LDADD)
XDOutputTest_LDADD = -L.. -lxml_data_module $(BES_DAP_LIBS) $(AM_LDADD)

