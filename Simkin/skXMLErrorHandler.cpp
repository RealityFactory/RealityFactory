/*
  Copyright 1996-2003
  Simon Whiteside

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

* $Id: skXMLErrorHandler.cpp,v 1.7 2003/01/20 18:48:18 simkin_cvs Exp $
*/
#include "skXMLErrorHandler.h"
#include <xercesc/dom/deprecated/DOMString.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include "skXMLElementObject.h"

//------------------------------------------
void  skXMLErrorHandler::warning (const SAXParseException& exception)
//------------------------------------------
{
}
//------------------------------------------
void  skXMLErrorHandler::error (const SAXParseException& toCatch)
//------------------------------------------
{
  cerr << "Error at file \"" << DOMString(toCatch.getSystemId())
       << "\", line " << toCatch.getLineNumber()
       << ", column " << toCatch.getColumnNumber()
       << "\n   Message: " << DOMString(toCatch.getMessage()) << endl;
  throw SAXParseException(toCatch);  // Copy the 'toCatch' object before throwing - 
  //   otherwise we would be throwing a reference to
  //   a local object that gets destroyed before
  //   the catch.
}
//------------------------------------------
void  skXMLErrorHandler::fatalError (const SAXParseException& toCatch)
//------------------------------------------
{
  cerr << "Fatal Error at file \"" << DOMString(toCatch.getSystemId())
       << "\", line " << toCatch.getLineNumber()
       << ", column " << toCatch.getColumnNumber()
       << "\n   Message: " << DOMString(toCatch.getMessage()) << endl;
  throw SAXParseException(toCatch);
}
//------------------------------------------
void  skXMLErrorHandler::resetErrors ()
//------------------------------------------
{
}
