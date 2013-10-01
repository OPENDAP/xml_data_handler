// BESXDModule.cc

// This file is part of bes, A C++ back-end server implementation framework
// for the OPeNDAP Data Access Protocol.

// Copyright (c) 2004,2005 University Corporation for Atmospheric Research
// Author: Patrick West <pwest@ucar.edu>
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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact University Corporation for Atmospheric Research at
// 3080 Center Green Drive, Boulder, CO 80301

// Authors:
//      pwest       Patrick West <pwest@ucar.edu>

#include <iostream>

using std::endl;

#include <BESXDModule.h>
#include <BESDebug.h>

#include <BESDapNames.h>
#include <BESResponseNames.h>
#include <BESResponseHandlerList.h>
#include <BESRequestHandlerList.h>

#include <BESDapService.h>

#include <BESTransmitter.h>
#include <BESReturnManager.h>
#include <BESTransmitterNames.h>

#include "BESXDNames.h"
#include "BESXDResponseHandler.h"
#include "BESXDRequestHandler.h"
#include "BESXDTransmit.h"

void BESXDModule::initialize(const string &modname)
{
    BESDEBUG("xd", "Initializing OPeNDAP XD module " << modname << endl);

    BESDEBUG("xd", "    adding " << modname << " request handler" << endl);

    BESRequestHandler *handler = new BESXDRequestHandler(modname);
    BESRequestHandlerList::TheList()->add_handler(modname, handler);

    BESDEBUG("xd", "    adding " << XD_RESPONSE << " response handler" << endl);

    BESResponseHandlerList::TheList()->add_handler(XD_RESPONSE, BESXDResponseHandler::XDResponseBuilder);

    BESDEBUG("xd", "Adding to dap services" << endl);

    BESDapService::add_to_dap_service(XD_SERVICE, "OPeNDAP xml data representation");

    BESTransmitter *t = BESReturnManager::TheManager()->find_transmitter(DAP2_FORMAT);
    if (t) {
        BESDEBUG("xd", "    adding basic " << XD_TRANSMITTER << " transmit function" << endl);
        t->add_method(XD_TRANSMITTER, BESXDTransmit::send_basic_ascii);
    }

    BESDEBUG("xd", "    adding xd debug context" << endl);

    BESDebug::Register("xd");

    BESDEBUG("xd", "Done Initializing OPeNDAP XD module " << modname << endl);
}

void BESXDModule::terminate(const string &modname)
{
    BESDEBUG("xd", "Cleaning OPeNDAP XD module " << modname << endl);

    BESDEBUG("xd", "    removing " << modname << " request handler " << endl);
    BESRequestHandler *rh = BESRequestHandlerList::TheList()->remove_handler(modname);
    if (rh)
        delete rh;

    BESDEBUG("xd", "    removing " << XD_RESPONSE << " response handler" << endl);
    BESResponseHandlerList::TheList()->remove_handler(XD_RESPONSE);

    BESTransmitter *t = BESReturnManager::TheManager()->find_transmitter(DAP2_FORMAT);
    if (t) {
        BESDEBUG("xd", "    removing basic " << XD_TRANSMITTER << " transmit function" << endl);
        t->remove_method(XD_TRANSMITTER);
    }

    t = BESReturnManager::TheManager()->find_transmitter(DAP2_FORMAT);
    if (t) {
        BESDEBUG("xd", "    removing http " << XD_TRANSMITTER << " transmit function" << endl);
        t->remove_method(XD_TRANSMITTER);
    }

    BESDEBUG("xd", "Done Cleaning OPeNDAP XD module " << modname << endl);
}

/** @brief dumps information about this object
 *
 * Displays the pointer value of this instance
 *
 * @param strm C++ i/o stream to dump the information to
 */
void BESXDModule::dump(ostream &strm) const
{
    strm << BESIndent::LMarg << "BESXDModule::dump - (" << (void *) this << ")" << endl;
}

extern "C" {
BESAbstractModule *maker()
{
    return new BESXDModule;
}
}

