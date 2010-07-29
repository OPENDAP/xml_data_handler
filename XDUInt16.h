
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

// (c) COPYRIGHT URI/MIT 1999,2000
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      jhrg,jimg       James Gallagher <jgallagher@gso.uri.edu>

// XDUInt16 interface. See XDByte.h for more info.
//
// 3/12/98 jhrg

#ifndef _asciiuint16_h
#define _asciiuint16_h 1

#include "UInt16.h"
#include "XDOutput.h"

class XDUInt16: public UInt16, public XDOutput {
public:
    XDUInt16(const string &n) : UInt16( n ) {}
    XDUInt16( UInt16 *bt ) : UInt16( bt->name() ), XDOutput( bt ) {
        set_send_p(bt->send_p());
    }
    virtual ~XDUInt16() {}

    virtual BaseType *ptr_duplicate();
};

#endif

