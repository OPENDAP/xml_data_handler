
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of asciival, software which can return an ASCII
// representation of the data read from a DAP server.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: James Gallagher <jgallagher@opendap.org>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
// 
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

// Tests for the DataDDS class.

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <DDS.h>
#include <Grid.h>
#include <debug.h>

#include "XDOutput.h"
#include "XDOutputFactory.h"
#include "XDGrid.h"
#include "XDArray.h"

#include "test_config.h"

// These globals are defined in ascii_val.cc and are needed by the XD*
// classes. This code has to be linked with those so that the XD*
// specializations of Byte, ..., Grid will be instantiated by DDS when it
// parses a .dds file. Each of those subclasses is a child of XDOutput in
// addition to its regular lineage. This test code depends on being able to
// cast each variable to an XDOutput object. 01/24/03 jhrg
bool translate = false;

using namespace CppUnit;
using namespace std;
using namespace libdap;

class XDOutputTest : public TestFixture {
private:
    DDS *dds;
    XDOutputFactory *aof;
    
public: 
    XDOutputTest() {}
    ~XDOutputTest() {}

    void setUp() {
        aof = new XDOutputFactory;
	dds = new DDS(aof, "xml_data_output_test");
	string parsefile = (string)TEST_SRC_DIR
			    + "/testsuite/XDOutputTest1.dds";
	dds->parse(parsefile);

	// Load various arrays with data
	// g.xy[12][12], g.x[12], g.y[12]
	XDGrid &g = dynamic_cast<XDGrid&>(*(dds->var("g")));
	XDArray &xy = dynamic_cast<XDArray&>(*(g.array_var()));
	Grid::Map_iter m = g.map_begin();
	XDArray &x = dynamic_cast<XDArray&>(**m++);
	XDArray &y = dynamic_cast<XDArray&>(**m);

	vector<dods_float64> f64x;
	for (int i = 0; i < 12; i++)
	    f64x.push_back(i * (-51.2));
	x.set_value(f64x, f64x.size());
	y.set_value(f64x, f64x.size());

	vector<dods_byte> bxy;
	for (int i = 0; i < 12; i++)
	    for (int j = 0; j < 12; j++)
		bxy.push_back(i * j * (2));
	xy.set_value(bxy, bxy.size());
    }

    void tearDown() {
        delete aof; aof = 0;
	delete dds; dds = 0;
    }

    CPPUNIT_TEST_SUITE(XDOutputTest);
#if 0
    CPPUNIT_TEST(test_get_full_name);
#endif
    CPPUNIT_TEST(test_print_xml_data);
    CPPUNIT_TEST(test_print_xml_data_structure);
    CPPUNIT_TEST(test_print_xml_data_grid);

    CPPUNIT_TEST_SUITE_END();
#if 0
    void test_get_full_name() {
	CPPUNIT_ASSERT(dynamic_cast<XDOutput*>(dds->var("a"))->get_full_name() == "a");
	DBG(cerr << "full name: " 
	     << dynamic_cast<XDOutput*>(dds->var("e.c"))->get_full_name()
	     << endl);

	CPPUNIT_ASSERT(dynamic_cast<XDOutput*>(dds->var("e.c"))->get_full_name() == "e.c");
	CPPUNIT_ASSERT(dynamic_cast<XDOutput*>(dds->var("f.c"))->get_full_name() == "f.c");
	CPPUNIT_ASSERT(dynamic_cast<XDOutput*>(dds->var("g.y"))->get_full_name() == "g.y");
	CPPUNIT_ASSERT(dynamic_cast<XDOutput*>(dds->var("k.h.i"))->get_full_name() == "k.h.i");
    }
#endif
    void test_print_xml_data() {
	dds->var("a")->set_send_p(true);
	XMLWriter writer;
	dynamic_cast<XDOutput*>(dds->var("a"))->print_xml_data(&writer, true);
	cout << writer.get_doc() << endl;
    }

    void test_print_xml_data_structure() {
	dds->var("e")->set_send_p(true);
	XMLWriter writer;
	dynamic_cast<XDOutput*>(dds->var("e"))->print_xml_data(&writer, true);
	cout << writer.get_doc() << endl;
    }

    void test_print_xml_data_grid() {
	dds->var("g")->set_send_p(true);
	XMLWriter writer;
	dynamic_cast<XDOutput*>(dds->var("g"))->print_xml_data(&writer, true);
	cout << writer.get_doc() << endl;
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(XDOutputTest);

int 
main( int argc, char* argv[] )
{
    CppUnit::TextTestRunner runner;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() );

    bool wasSuccessful = runner.run( "", false ) ;

    return wasSuccessful ? 0 : 1;
}

