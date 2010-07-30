
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of asciival, software which can return an XML data
// representation of the data read from a DAP server.

// Copyright (c) 2010 OPeNDAP, Inc.
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

// (c) COPYRIGHT URI/MIT 1998,2000
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// Implementation for XDArray. See XDByte.cc
//
// 3/12/98 jhrg

#include "config.h"

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

// #define DODS_DEBUG

#include <InternalErr.h>
#include <escaping.h>
#include <util.h>
#include <debug.h>

#include "XDArray.h"
#include "get_xml_data.h"

using namespace xml_data;

BaseType *
XDArray::ptr_duplicate()
{
    return new XDArray(*this);
}

XDArray::XDArray(const string &n, BaseType *v) : Array(n, v)
{
}

XDArray::XDArray( Array *bt )
    : Array(bt->name(), 0), XDOutput( bt )
{
    // By calling var() without any parameters we get back the template
    // itself, then we can add it to this Array as the template. By doing
    // this we set the parent as well, which is what we need.
    BaseType *abt = basetype_to_asciitype( bt->var() ) ;
    add_var( abt ) ;
    // add_var makes a copy of the base type passed, so delete the original
    delete abt ;

    // Copy the dimensions
    Dim_iter p = bt->dim_begin();
    while ( p != bt->dim_end() ) {
        append_dim(bt->dimension_size(p, true), bt->dimension_name(p));
        ++p;
    }

    // I'm not particularly happy with this constructor; we should be able to
    // use the libdap ctors like BaseType::BaseType(const BaseType &copy_from)
    // using that via the Array copy ctor won't let us use the
    // basetype_to_asciitype() factory class. jhrg 5/19/09
    set_send_p(bt->send_p());
}

XDArray::~XDArray()
{
}

void XDArray::print_xml_map_data(XMLWriter *writer, bool show_type) throw(InternalErr)
{
    if (var()->is_simple_type()) {
        if (dimensions(true) > 1) {
            // We might have n-dimensional maps someday...
            m_print_xml_array(writer, "Map");
        }
        else {
            m_print_xml_vector(writer, "Map");
        }
    }
    else {
	throw InternalErr(__FILE__, __LINE__, "A Map must be a simple type.");
    }
}


void XDArray::print_xml_data(XMLWriter *writer, bool print_name) throw(InternalErr)
{
#if 0
    Array *btp = dynamic_cast < Array * >(d_redirect);
    if (!btp) {
        btp = this;
    }
#endif

    if (/*btp->*/var()->is_simple_type()) {
        if (dimensions(true) > 1) {
            m_print_xml_array(writer, "Array");
        } else {
            m_print_xml_vector(writer, "Array");
        }
    } else {
	cerr << "I don't know how to print complex arrays!" << endl;
	// TODO: Fix me
        //print_xml_complex_array(writer, print_name);
    }
}

class PrintArrayDimXML : public unary_function<Array::dimension&, void>
{
    XMLWriter *d_writer;
    bool d_constrained;

public:
    PrintArrayDimXML(XMLWriter *writer, bool c)
            : d_writer(writer), d_constrained(c)
    {}

    void operator()(Array::dimension &d)
    {
	int size = d_constrained ? d.c_size : d.size;
	if (d.name.empty()) {
	    if (xmlTextWriterStartElement(d_writer->get_writer(),  (const xmlChar*) "dimension") < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write dimension element");
	    if (xmlTextWriterWriteAttribute(d_writer->get_writer(), (const xmlChar*) "size", get_xc(long_to_string(size))))
		throw InternalErr(__FILE__, __LINE__, "Could not write attribute for size");
	    if (xmlTextWriterEndElement(d_writer->get_writer()) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not end dimension element");
	}
	else {
	    if (xmlTextWriterStartElement(d_writer->get_writer(), (const xmlChar*) "dimension") < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write dimension element");
	    if (xmlTextWriterWriteAttribute(d_writer->get_writer(), (const xmlChar*) "name", get_xc(id2xml(d.name))))
		throw InternalErr(__FILE__, __LINE__, "Could not write attribute for name");
	    if (xmlTextWriterWriteAttribute(d_writer->get_writer(), (const xmlChar*) "size", get_xc(long_to_string(size))))
		throw InternalErr(__FILE__, __LINE__, "Could not write attribute for size");
	    if (xmlTextWriterEndElement(d_writer->get_writer()) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not end dimension element");
	}
    }
};

void XDArray::m_start_array_element(XMLWriter *writer, string element)
{
    // Start the Array element (includes the name)
    if (xmlTextWriterStartElement(writer->get_writer(), get_xc(element)) < 0)
	throw InternalErr(__FILE__, __LINE__, "Could not write Array element for " + /*btp->*/name());
    if (xmlTextWriterWriteAttribute(writer->get_writer(), (const xmlChar*) "name", get_xc(/*btp->*/name())) < 0)
	throw InternalErr(__FILE__, __LINE__, "Could not write attribute for " + /*btp->*/name());

    // Start and End the Type element
    if (xmlTextWriterStartElement(writer->get_writer(), get_xc(/*btp->*/var()->type_name())) < 0)
	throw InternalErr(__FILE__, __LINE__, "Could not write type element for " + /*btp->*/name());
    if (xmlTextWriterEndElement(writer->get_writer()) < 0)
	throw InternalErr(__FILE__, __LINE__, "Could not end element for " + /*btp->*/name());

    for_each(dim_begin(), dim_end(), PrintArrayDimXML(writer, true));
}

void XDArray::m_end_array_element(XMLWriter *writer)
{
    // End the element for the Array/name
    if (xmlTextWriterEndElement(writer->get_writer()) < 0)
	throw InternalErr(__FILE__, __LINE__, "Could not end element for " + /*btp->*/name());
}

// Print out a values for a vector (one dimensional array) of simple types.
void XDArray::m_print_xml_vector(XMLWriter *writer, string element)
{
#if 0
    Array *btp = dynamic_cast < Array * >(d_redirect);
    if (!btp) {
        btp = this;
    }
#endif


    m_start_array_element(writer, element);

    // only one dimension
    int end = dimension_size(dim_begin(), true) - 1;

    for (int i = 0; i < end; ++i) {
        BaseType *curr_var = basetype_to_asciitype(/*btp->*/var(i));
        dynamic_cast < XDOutput & >(*curr_var).print_xml_data(writer, false);
        // we're not saving curr_var for future use, so delete it here
        delete curr_var;
    }
    BaseType *curr_var = basetype_to_asciitype(/*btp->*/var(end));
    dynamic_cast < XDOutput & >(*curr_var).print_xml_data(writer, false);
    // we're not saving curr_var for future use, so delete it here
    delete curr_var;

    m_end_array_element(writer);
}

void XDArray::m_print_xml_array(XMLWriter *writer, string element)
{
    DBG(cerr << "Entering XDArray::print_xml_array" << endl);
#if 0
    Array *btp = dynamic_cast < Array * >(d_redirect);
    if (!btp) {
        btp = this;
    }
#endif

    int dims = dimensions(true);
    if (dims <= 1)
        throw InternalErr(__FILE__, __LINE__, "Dimension count is <= 1 while printing multidimensional array.");

    m_start_array_element(writer, element);

    // shape holds the maximum index value of all but the last dimension of
    // the array (not the size; each value is one less that the size).
    vector < int >shape = get_shape_vector(dims - 1);
    int rightmost_dim_size = get_nth_dim_size(dims - 1);

    // state holds the indexes of the current row being printed. For an N-dim
    // array, there are N-1 dims that are iterated over when printing (the
    // Nth dim is not printed explicitly. Instead it's the number of values
    // on the row).
    vector < int >state(dims - 1, 0);

    bool more_indices;
    int index = 0;
    do {
        for (int i = 0; i < dims - 1; ++i) {
	    if (xmlTextWriterStartElement(writer->get_writer(), (const xmlChar*) "dim") < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write Array element for " + /*btp->*/name());
	    if (xmlTextWriterWriteAttribute(writer->get_writer(), (const xmlChar*) "number", get_xc(long_to_string(i))) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write number attribute for " + /*btp->*/name() + ": " + long_to_string(i));
	    if (xmlTextWriterWriteAttribute(writer->get_writer(), (const xmlChar*) "index", get_xc(long_to_string(state[i]))) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not write index attribute for " + /*btp->*/name());
	    if (xmlTextWriterEndElement(writer->get_writer()) < 0)
		throw InternalErr(__FILE__, __LINE__, "Could not end element for " + /*btp->*/name());
	}

	index = m_print_xml_row(writer, index, rightmost_dim_size - 1);

        more_indices = increment_state(&state, shape);
    } while (more_indices);

    m_end_array_element(writer);

    DBG(cerr << "ExitingXDArray::print_array" << endl);
}

/** Print a single row of values for a N-dimensional array. Since we store
    N-dim arrays in vectors, #index# gives the starting point in that vector
    for this row and #number# is the number of values to print. The counter
    #index# is returned.

    @note This is called only for simple types.

    @param writer Write to this xml sink.
    @param index Print values starting from this point.
    @param number Print this many values.
    @return One past the last value printed (i.e., the index of the next
    row's first value).
    @see print\_array */
int XDArray::m_print_xml_row(XMLWriter *writer, int index, int number)
{
#if 0
    Array *bt = dynamic_cast < Array * >(d_redirect);
    if (!bt) {
        bt = this;
    }
#endif
    for (int i = 0; i < number; ++i) {
        BaseType *curr_var = basetype_to_asciitype(/*bt->*/var(index++));
        dynamic_cast < XDOutput & >(*curr_var).print_xml_data(writer, false);
        // we're not saving curr_var for future use, so delete it here
        delete curr_var;
    }
    BaseType *curr_var = basetype_to_asciitype(/*bt->*/var(index++));
    dynamic_cast < XDOutput & >(*curr_var).print_xml_data(writer, false);
    // we're not saving curr_var for future use, so delete it here
    delete curr_var;

    return index;
}

// Given a vector of indices, return the corresponding index.

int XDArray::m_get_index(vector < int >indices) throw(InternalErr)
{
    if (indices.size() != /*bt->*/dimensions(true)) {
        throw InternalErr(__FILE__, __LINE__,
                          "Index vector is the wrong size!");
    }
    // suppose shape is [3][4][5][6] for x,y,z,t. The index is
    // t + z(6) + y(5 * 6) + x(4 * 5 *6).
    // Assume that indices[0] holds x, indices[1] holds y, ...

    // It's hard to work with Pixes
    vector < int >shape = get_shape_vector(indices.size());

    // We want to work from the rightmost index to the left
    reverse(indices.begin(), indices.end());
    reverse(shape.begin(), shape.end());

    vector < int >::iterator indices_iter = indices.begin();
    vector < int >::iterator shape_iter = shape.begin();

    int index = *indices_iter++;        // in the ex. above, this adds `t'
    int multiplier = 1;
    while (indices_iter != indices.end()) {
        multiplier *= *shape_iter++;
        index += multiplier * *indices_iter++;
    }

    return index;
}

// get_shape_vector and get_nth_dim_size are public because that are called
// from Grid. 9/14/2001 jhrg

vector < int > XDArray::get_shape_vector(size_t n) throw(InternalErr)
{
    if (n < 1 || n > dimensions(true)) {
        string msg = "Attempt to get ";
        msg += long_to_string(n) + " dimensions from " + name()
            + " which has only " + long_to_string(dimensions(true))
            + " dimensions.";

        throw InternalErr(__FILE__, __LINE__, msg);
    }

    vector < int >shape;
    Array::Dim_iter p = dim_begin();
    for (unsigned i = 0; i < n && p != dim_end(); ++i, ++p) {
        shape.push_back(dimension_size(p, true));
    }

    return shape;
}

/** Get the size of the Nth dimension. The first dimension is N == 0.
    @param n The index. Uses sero-based indexing.
    @return the size of the dimension. */
int XDArray::get_nth_dim_size(size_t n) throw(InternalErr)
{
    if (n > /*bt->*/dimensions(true) - 1) {
        string msg = "Attempt to get dimension ";
        msg +=
            long_to_string(n + 1) + " from `" + /*bt->*/name() +
            "' which has " + long_to_string(/*bt->*/dimensions(true)) +
            " dimension(s).";
        throw InternalErr(__FILE__, __LINE__, msg);
    }

    return /*bt->*/dimension_size(/*bt->*/dim_begin() + n, true);
}

#if 0
void XDArray::print_complex_array(ostream &strm, bool /*print_name */ )
{
#if 0
    DBG(cerr << "Entering XDArray::print_complex_array" << endl);

    Array *bt = dynamic_cast < Array * >(d_redirect);
    if (!bt)
        bt = this;

    int dims = /*bt->*/dimensions(true);
    if (dims < 1)
        throw InternalErr(__FILE__, __LINE__,
            "Dimension count is <= 1 while printing multidimensional array.");

    // shape holds the maximum index value of all but the last dimension of
    // the array (not the size; each value is one less that the size).
    vector < int >shape = get_shape_vector(dims);

    vector < int >state(dims, 0);

    bool more_indices;
    do {
        // Print indices for all dimensions except the last one.
        strm << dynamic_cast <XDOutput *>(this)->get_full_name() ;

        for (int i = 0; i < dims; ++i) {
            strm << "[" << state[i] << "]" ;
        }
        strm << "\n" ;

        BaseType *curr_var =
            basetype_to_asciitype(bt->var(m_get_index(state)));
        dynamic_cast < XDOutput & >(*curr_var).print_ascii(strm, true);
        // we are not saving curr_var for future reference, so delete it
        delete curr_var;

        more_indices = increment_state(&state, shape);
        if (more_indices)
            strm << "\n" ;

    } while (more_indices);

    DBG(cerr << "ExitingXDArray::print_complex_array" << endl);
#endif
}
#endif
