
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

// Interface definition for XDArray. See XDByte.h for more information
//
// 3/12/98 jhrg

#ifndef _asciiarray_h
#define _asciiarray_h 1

#include <Array.h>

#include "XMLWriter.h"
#include "XDOutput.h"

using namespace libdap ;

class XDArray: public Array, public XDOutput {
private:
    int m_get_index(vector<int> indices) throw(InternalErr);

    void m_print_xml_vector(XMLWriter *writer, string element);
    void m_print_xml_array(XMLWriter *writer, string element);
    void m_print_xml_complex_array(XMLWriter *writer, string element);

    int m_print_xml_row(XMLWriter *writer, int index, int number);

    friend class XDArrayTest;

public:
    XDArray(const string &n, BaseType *v);
    XDArray( Array *bt ) ;
    virtual ~XDArray();

    virtual BaseType *ptr_duplicate();

    int get_nth_dim_size(size_t n) throw(InternalErr);

    vector<int> get_shape_vector(size_t n) throw(InternalErr);

    virtual void start_xml_declaration(XMLWriter *writer, string element = "")  throw(InternalErr);

    void print_xml_map_data(XMLWriter *writer, bool show_type) throw(InternalErr);

    void print_xml_data(XMLWriter *writer, bool show_type) throw(InternalErr);

};

#endif
