
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
CXXFLAGS_DEBUG = -g3 -O0  -Wall -W -Wcast-align -Werror
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

#test_config.h: test_config.h.in Makefile
#	sed -e "s%[@]abs_srcdir[@]%${abs_srcdir}%" $< > test_config.h

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

XD_SOURCES = ../XDArray.cc ../XDInt16.cc ../XDStructure.cc          \
        ../XDInt32.cc ../XDUInt16.cc ../XDByte.cc ../XDOutput.cc   \
        ../XDUInt32.cc ../XDFloat32.cc ../XDUrl.cc ../XDFloat64.cc \
        ../XDSequence.cc  ../XDGrid.cc ../XDStr.cc ../XDArray.h    \
        ../XDInt32.h ../XDUInt32.h ../XDByte.h ../XDOutput.h       \
        ../XDUrl.h ../XDFloat32.h ../XDSequence.h ../XDFloat64.h   \
        ../XDStr.h ../XDGrid.h ../XDStructure.h ../XDInt16.h       \
        ../XDUInt16.h ../XDOutputFactory.cc ../XDOutputFactory.h   \
        ../get_xml_data.cc ../get_xml_data.h 

XDArrayTest_SOURCES = XDArrayTest.cc $(XD_SOURCES)
XDOutputTest_SOURCES = XDOutputTest.cc $(XD_SOURCES)

# if not linking with the module, use $(XDOBJS) below
XDArrayTest_LDADD = $(BES_DAP_LIBS) $(AM_LDADD)
XDOutputTest_LDADD = $(BES_DAP_LIBS) $(AM_LDADD)

