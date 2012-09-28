// BESXDTransmit.cc

// This file is part of bes, A C++ back-end server implementation framework
// for the OPeNDAP Data Access Protocol.

// Copyright (c) 2004,2005 University Corporation for Atmospheric Research
// Author: Patrick West <pwest@ucar.edu> and Jose Garcia <jgarcia@ucar.edu>
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
// You can contact University Corporation for Atmospheric Research at
// 3080 Center Green Drive, Boulder, CO 80301

// (c) COPYRIGHT University Corporation for Atmospheric Research 2004-2005
// Please read the full copyright statement in the file COPYRIGHT_UCAR.
//
// Authors:
//      pwest       Patrick West <pwest@ucar.edu>
//      jgarcia     Jose Garcia <jgarcia@ucar.edu>

#include <BaseType.h>
#include <Sequence.h>
#include <ConstraintEvaluator.h>
#include <XMLWriter.h>
#include <DODSFilter.h>
#include <escaping.h>
#include <InternalErr.h>
#include <util.h>
#include <mime_util.h>

#include <BESDapTransmit.h>
#include <BESContainer.h>
#include <BESDataNames.h>
#include <BESDataDDSResponse.h>
#include <BESDapError.h>
#include <BESInternalFatalError.h>
#include <BESDebug.h>

//#include "XMLWriter.h"
#include "BESXDTransmit.h"
#include "get_xml_data.h"

using namespace xml_data;

void BESXDTransmit::send_basic_ascii(BESResponseObject * obj, BESDataHandlerInterface & dhi)
{
    BESDEBUG("xd", "BESXDTransmit::send_base_ascii" << endl);
    BESDataDDSResponse *bdds = dynamic_cast<BESDataDDSResponse *>(obj);
    DataDDS *dds = bdds->get_dds();
    ConstraintEvaluator & ce = bdds->get_ce();

    dhi.first_container();

    string constraint = www2id(dhi.data[POST_CONSTRAINT], "%", "%20%26");

    try {
        BESDEBUG("xd", "BESXDTransmit::send_base_ascii - " "parsing constraint: " << constraint << endl);
        ce.parse_constraint(constraint, *dds);
    }
    catch (InternalErr &e) {
        string err = "Failed to parse the constraint expression: " + e.get_error_message();
        throw BESDapError(err, true, e.get_error_code(), __FILE__, __LINE__);
    }
    catch (Error &e) {
        string err = "Failed to parse the constraint expression: " + e.get_error_message();
        throw BESDapError(err, false, e.get_error_code(), __FILE__, __LINE__);
    }
    catch (...) {
        string err = (string) "Failed to parse the constraint expression: " + "Unknown exception caught";
        throw BESInternalFatalError(err, __FILE__, __LINE__);
    }

    BESDEBUG("xd", "BESXDTransmit::send_base_ascii - " "tagging sequences" << endl);
    dds->tag_nested_sequences(); // Tag Sequences as Parent or Leaf node.

    BESDEBUG("xd", "BESXDTransmit::send_base_ascii - " "accessing container" << endl);
    string dataset_name = dhi.container->access();

    BESDEBUG("xd", "BESXDTransmit::send_base_ascii - dataset_name = " << dataset_name << endl);

    bool functional_constraint = false;
    try {
        // Handle *functional* constraint expressions specially
        if (ce.functional_expression()) {
            BESDEBUG("xd", "processing a functional constraint." << endl);
            // This returns a new BaseType, not a pointer to one in the DataDDS
            // So once the data has been read using this var create a new
            // DataDDS and add this new var to the it.
            BaseType *var = ce.eval_function(*dds, dataset_name);
            if (!var)
                throw Error(unknown_error, "Error calling the CE function.");

            var->read();

            dds = new DataDDS(NULL, "virtual");
            functional_constraint = true;
            dds->add_var(var);
        }
        else {
            // Iterate through the variables in the DataDDS and read in the data
            // if the variable has the send flag set.
            for (DDS::Vars_iter i = dds->var_begin(); i != dds->var_end(); i++) {
                BESDEBUG("xd", "processing var: " << (*i)->name() << endl);
                if ((*i)->send_p()) {
                    BESDEBUG("xd", "reading some data for: " << (*i)->name() << endl);
                    (**i).intern_data(ce, *dds);
                }
            }
        }
    }
    catch (InternalErr &e) {
        if (functional_constraint)
            delete dds;
        string err = "Failed to read data: " + e.get_error_message();
        throw BESDapError(err, true, e.get_error_code(), __FILE__, __LINE__);
    }
    catch (Error & e) {
        if (functional_constraint)
            delete dds;
        string err = "Failed to read data: " + e.get_error_message();
        throw BESDapError(err, false, e.get_error_code(), __FILE__, __LINE__);
    }
    catch (...) {
        if (functional_constraint)
            delete dds;
        string err = "Failed to read data: Unknown exception caught";
        throw BESInternalFatalError(err, __FILE__, __LINE__);
    }

    try {
        // Now that we have constrained the DataDDS and read in the data,
        // send it as ascii
        BESDEBUG("xd", "converting to xd datadds" << endl);
        DataDDS *xd_dds = datadds_to_xd_datadds(dds);

        BESDEBUG("xd", "getting xd values" << endl);
        XMLWriter writer;
        get_data_values_as_xml(xd_dds, &writer);
        dhi.get_output_stream() << writer.get_doc();

        BESDEBUG("xd", "got the ascii values" << endl);
        dhi.get_output_stream() << flush;
        delete xd_dds;

        BESDEBUG("xd", "done transmitting ascii" << endl);
    }
    catch (InternalErr &e) {
        if (functional_constraint)
            delete dds;
        string err = "Failed to get values as ascii: " + e.get_error_message();
        throw BESDapError(err, true, e.get_error_code(), __FILE__, __LINE__);
    }
    catch (Error &e) {
        if (functional_constraint)
            delete dds;
        string err = "Failed to get values as ascii: " + e.get_error_message();
        throw BESDapError(err, false, e.get_error_code(), __FILE__, __LINE__);
    }
    catch (...) {
        if (functional_constraint)
            delete dds;
        string err = "Failed to get values as ascii: Unknown exception caught";
        throw BESInternalFatalError(err, __FILE__, __LINE__);
    }

    if (functional_constraint)
        delete dds;
}

void BESXDTransmit::send_http_ascii(BESResponseObject * obj, BESDataHandlerInterface & dhi)
{
    set_mime_text(dhi.get_output_stream(), dods_data);
    BESXDTransmit::send_basic_ascii(obj, dhi);
}

