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

* $Id: skXMLErrorHandler.h,v 1.9 2003/01/20 18:48:18 simkin_cvs Exp $
*/

#ifndef skXMLERRORHANDLER_H
#define skXMLERRORHANDLER_H

#include "skGeneral.h"
#include <xercesc/sax/ErrorHandler.hpp>


/**
 * This class picks up errors encountered while parsing an XML document and throws SAXParseExceptions
 */
class CLASSEXPORT skXMLErrorHandler : public ErrorHandler {
 public:
  /**
   * This function is called when there is a warning - it does nothing
   */
  void  warning (const SAXParseException& exception);
  /**
   * This function is called when there is an error - it throws the exception
   * @exception SAXParseException - the exception received is thrown
   */
  void  error (const SAXParseException& exception);
  /**
   * This function is called when there is a fatal error - it throws the exception
   * @exception SAXParseException - the exception received is thrown
   */
  void  fatalError (const SAXParseException& exception);
  /**
   * this function does nothing
   */
  void  resetErrors ();
};

#endif
