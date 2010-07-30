
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

// Implementation for the class XDStructure. See XDByte.cc
//
// 3/12/98 jhrg

#include "config.h"

#include <string>

#include <BESDebug.h>

#include "InternalErr.h"
#include "XDStructure.h"
#include "XDSequence.h"
//#include "name_map.h"
#include "get_xml_data.h"

using namespace xml_data;

BaseType *
XDStructure::ptr_duplicate()
{
    return new XDStructure(*this);
}

XDStructure::XDStructure(const string &n) : Structure(n)
{
}

XDStructure::XDStructure( Structure *bt )
    : Structure( bt->name() ), XDOutput( bt )
{
    // Let's make the alternative structure of XD types now so that we
    // don't have to do it on the fly. This will also set the parents of
    // each of the underlying vars of the structure.
    Vars_iter p = bt->var_begin();
    while (p != bt->var_end()) {
        BaseType *new_bt = basetype_to_asciitype(*p);
        add_var(new_bt);
        // add_var makes a copy of the base type passed to it, so delete
        // it here
        delete new_bt;
        p++;
    }

    BaseType::set_send_p(bt->send_p());
}

XDStructure::~XDStructure()
{
}

void
XDStructure::start_xml_declaration(XMLWriter *writer, string element)  throw(InternalErr)
{
    XDOutput::start_xml_declaration(writer);

    for (Vars_iter p = var_begin(); p != var_end(); ++p) {
        if ((*p)->send_p()) {
            dynamic_cast<XDOutput&>(**p).start_xml_declaration(writer, element);
            dynamic_cast<XDOutput&>(**p).end_xml_declaration(writer);
        }
    }
}

#if 0
void XDStructure::m_start_structure_element(XMLWriter *writer)
{
    // Start the Array element (includes the name)
    if (xmlTextWriterStartElement(writer->get_writer(), get_xc(/*btp->*/type_name())) < 0)
	throw InternalErr(__FILE__, __LINE__, "Could not write Structure element for " + /*btp->*/name());
    if (xmlTextWriterWriteAttribute(writer->get_writer(), (const xmlChar*) "name", get_xc(/*btp->*/name())) < 0)
	throw InternalErr(__FILE__, __LINE__, "Could not write name attribute for " + /*btp->*/name());

#if 0
    // For a structure, write each variable and its value, then move on to the
    // next variable/value. Do not emulate the ASCII response, whcih is
    // designed to work with excel.

    // Start and End the Type element
    if (xmlTextWriterStartElement(writer->get_writer(), get_xc(/*btp->*/var()->type_name())) < 0)
	throw InternalErr(__FILE__, __LINE__, "Could not write type element for " + /*btp->*/name());
    if (xmlTextWriterEndElement(writer->get_writer()) < 0)
	throw InternalErr(__FILE__, __LINE__, "Could not end element for " + /*btp->*/name());

    Array &a = dynamic_cast<Array&>(*btp);
    for_each(a.dim_begin(), a.dim_end(), PrintArrayDimXML(writer, true));
#endif
}

void XDStructure::m_end_structure_element(XMLWriter *writer)
{
    // End the element for the Array/name
    if (xmlTextWriterEndElement(writer->get_writer()) < 0)
	throw InternalErr(__FILE__, __LINE__, "Could not end element for " + /*btp->*/name());
}
#endif

void
XDStructure::print_xml_data(XMLWriter *writer, bool /*show_type*/) throw(InternalErr)
{
#if 0
    BaseType *btp = d_redirect;
    if (!btp)
	btp = dynamic_cast<XDStructure*>(this);
    if (!btp)
	throw InternalErr(__FILE__, __LINE__, "Could not get a valid BaseType");
#endif

    // Start the <Structure> element
    //m_start_structure_element(writer);

    // Forcing the use of the generic version prints just the <Structure>
    // element w/o the type information of the components. That will be printed
    // by the embedded print_xml_data calls.
    XDOutput::start_xml_declaration(writer);

    for (Vars_iter p = var_begin(); p != var_end(); ++p) {
        if ((*p)->send_p()) {
            dynamic_cast<XDOutput*> ((*p))->print_xml_data(writer, true);
        }
    }

    // End the <Structure> element
    //m_end_structure_element(writer);
    end_xml_declaration(writer);
}
