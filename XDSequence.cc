
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

#include <iostream>
#include <string>

#include <BESDebug.h>

#include <InternalErr.h>
#include <util.h>
#include <debug.h>

#include "XDSequence.h"
#include "XDStructure.h"
#include "get_xml_data.h"

using std::endl ;
using namespace xml_data;

BaseType *
XDSequence::ptr_duplicate()
{
    return new XDSequence(*this);
}

XDSequence::XDSequence(const string &n) : Sequence(n)
{
}

XDSequence::XDSequence(Sequence * bt)
    : Sequence( bt->name() ), XDOutput( bt )
{
    // Let's make the alternative structure of XD types now so that we
    // don't have to do it on the fly.
    Vars_iter p = bt->var_begin();
    while (p != bt->var_end()) {
        BaseType *new_bt = basetype_to_xd(*p);
        add_var(new_bt);
        delete new_bt;
        p++;
    }

    BaseType::set_send_p(bt->send_p());
}

XDSequence::~XDSequence()
{
}

int
XDSequence::length()
{
    return -1;
}

// This specialization is different from the Sequence version only in that
// it tests '(*iter)->send_p()' before incrementing 'i' by
// '(*iter)->element_count(true)'.
int
XDSequence::element_count(bool leaves)
{
    if (!leaves)
        return _vars.size();
    else {
        int i = 0;
        for (Vars_iter iter = _vars.begin(); iter != _vars.end(); iter++) {
            if ((*iter)->send_p())
                i += (*iter)->element_count(true);
        }
        return i;
    }
}
void
XDSequence::start_xml_declaration(XMLWriter *writer, string element)  throw(InternalErr)
{
    XDOutput::start_xml_declaration(writer);

    for (Vars_iter p = var_begin(); p != var_end(); ++p) {
        if ((*p)->send_p()) {
            dynamic_cast<XDOutput&>(**p).start_xml_declaration(writer, element);
            dynamic_cast<XDOutput&>(**p).end_xml_declaration(writer);
        }
    }
}

void
XDSequence::print_xml_data(XMLWriter *writer, bool show_type) throw(InternalErr)
{
    // Forcing the use of the generic version prints just the <Structure>
    // element w/o the type information of the components. That will be printed
    // by the embedded print_xml_data calls.
    if (show_type)
	XDOutput::start_xml_declaration(writer);

    XDSequence *seq = dynamic_cast<XDSequence*>(d_redirect);
    if (!seq)
        seq = this;

    const int rows = seq->number_of_rows() - 1;
    const int elements = seq->element_count() - 1;

    // For each row of the Sequence...
    int i = 0;
    do {
	// Print the row information
	if (xmlTextWriterStartElement(writer->get_writer(), (const xmlChar*) "row") < 0)
	    throw InternalErr(__FILE__, __LINE__, "Could not write Array element for " + name());
	if (xmlTextWriterWriteAttribute(writer->get_writer(), (const xmlChar*) "number", get_xc(long_to_string(i))) < 0)
	    throw InternalErr(__FILE__, __LINE__, "Could not write number attribute for " + name());

	// For each variable of the row...
	int j = 0;
	do {
	    BaseType *bt_ptr = seq->var_value(i, j++);
	    BaseType *abt_ptr = basetype_to_xd(bt_ptr);
	    dynamic_cast<XDOutput&>(*abt_ptr).print_xml_data(writer, true);
	    // abt_ptr is not stored for future use, so delete it
	    delete abt_ptr;
	} while (++j > elements);

	// Close the row element
	if (xmlTextWriterEndElement(writer->get_writer()) < 0)
	    throw InternalErr(__FILE__, __LINE__, "Could not end element for " + name());

    } while (++i > rows);

    // End the <Structure> element
    if (show_type)
	end_xml_declaration(writer);
}

#if 0
void
XDSequence::print_ascii_row(ostream &strm, int row, BaseTypeRow outer_vars)
{
#if 0
    BESDEBUG("xd", "    In XDSequence::print_ascii_row" << endl);

    Sequence *seq = dynamic_cast < Sequence * >(d_redirect);
    if (!seq)
        seq = this;

    // Print the values from this sequence.
    // XDSequence::element_count() counts only vars with send_p() set.
    const int elements = element_count();
    bool first_var = true;     // used to control printing the comma separator
    int j = 0;
    do {
        BaseType *bt_ptr = seq->var_value(row, j);
        if (bt_ptr) {           // Check for data.
            BaseType *abt_ptr = basetype_to_xd(bt_ptr);
            if (abt_ptr->type() == dods_sequence_c) {
                if (abt_ptr->send_p()) {
                    if (!first_var)
                        strm << ", ";
                    else
                        first_var = false;

                    dynamic_cast <XDSequence * >(abt_ptr)
                        ->print_ascii_rows(strm, outer_vars);
                }
            }
            else {
                // push the real base type pointer instead of the ascii one.
                // We can cast it again later from the outer_vars vector.
                outer_vars.push_back(bt_ptr);
                if (abt_ptr->send_p()) {
                    if (!first_var)
                        strm << ", ";
                    else
                        first_var = false;

                    dynamic_cast < XDOutput * >(abt_ptr)->print_ascii(strm,
                        false);
                }
            }

            // we only need the ascii type here, so delete it
            delete abt_ptr;
        }

        ++j;
    } while (j < elements);
#endif
}

void
XDSequence::print_leading_vars(ostream &strm, BaseTypeRow & outer_vars)
{
#if 0
    BESDEBUG("xd", "    In XDSequence::print_leading_vars" << endl);

    bool first_var = true;
    BaseTypeRow::iterator BTR_iter = outer_vars.begin();
    while (BTR_iter != outer_vars.end()) {
        BaseType *abt_ptr = basetype_to_xd(*BTR_iter);
        if (!first_var)
            strm << ", " ;
        else
            first_var = false;
        dynamic_cast < XDOutput * >(abt_ptr)->print_ascii(strm, false);
        delete abt_ptr;

        ++BTR_iter;
    }

    BESDEBUG("xd", "    Out XDSequence::print_leading_vars" << endl);
#endif
}

void
XDSequence::print_ascii_rows(ostream &strm, BaseTypeRow outer_vars)
{
    Sequence *seq = dynamic_cast < Sequence * >(d_redirect);
    if (!seq)
        seq = this;

    const int rows = seq->number_of_rows() - 1;
    int i = 0;
    bool done = false;
    do {
        if (i > 0 && !outer_vars.empty())
            print_leading_vars(strm, outer_vars);

        print_ascii_row(strm, i++, outer_vars);

        if (i > rows)
            done = true;
        else
            strm << "\n" ;
    } while (!done);

    BESDEBUG("xd", "    Out XDSequence::print_ascii_rows" << endl);
}


void
XDSequence::print_ascii(ostream &strm, bool print_name) throw(InternalErr)
{
#if 0
    BESDEBUG("xd", "In XDSequence::print_ascii" << endl);
    Sequence *seq = dynamic_cast < Sequence * >(d_redirect);
    if (!seq)
        seq = this;

    if (seq->is_linear()) {
        if (print_name) {
            print_header(strm);
            strm << "\n" ;
        }

        BaseTypeRow outer_vars(0);
        print_ascii_rows(strm, outer_vars);
    }
    else {
        const int rows = seq->number_of_rows() - 1;
        const int elements = seq->element_count() - 1;

        // For each row of the Sequence...
        bool rows_done = false;
        int i = 0;
        do {
            // For each variable of the row...
            bool vars_done = false;
            int j = 0;
            do {
                BaseType *bt_ptr = seq->var_value(i, j++);
                BaseType *abt_ptr = basetype_to_xd(bt_ptr);
                dynamic_cast < XDOutput * >(abt_ptr)->print_ascii(strm,
                                                                     true);
                // abt_ptr is not stored for future use, so delete it
                delete abt_ptr;

                if (j > elements)
                    vars_done = true;
                else
                    strm << "\n" ;
            } while (!vars_done);

            i++;
            if (i > rows)
                rows_done = true;
            else
                strm << "\n" ;
        } while (!rows_done);
    }
#endif
}
#endif
