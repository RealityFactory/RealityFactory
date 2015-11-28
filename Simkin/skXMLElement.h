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

  $Id: skXMLElement.h,v 1.7 2003/01/20 18:48:18 simkin_cvs Exp $
*/
#ifndef skXMLELEMENT_H
#define skXMLELEMENT_H

#include "skString.h"
#include <xercesc/dom/deprecated/DOM_Element.hpp>
#include <xercesc/dom/deprecated/DOM_Document.hpp>

/**
 * This class is used to create additional XML elements that are added to an XML document, it overrides DOM_Element from the Xerces library
 */
class CLASSEXPORT skXMLElement : public DOM_Element {
 public:
  /**
   * Constructor - passes an owner and a tagname
   */
    inline skXMLElement(DOM_Element owner,const skString& tagname)
  : DOM_Element(owner.getOwnerDocument().createElement(DOMString((const char *)tagname))){
    }
};

#endif
