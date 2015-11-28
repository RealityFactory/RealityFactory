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

  $Id: skMSXMLElement.h,v 1.1 2003/01/23 14:22:09 simkin_cvs Exp $
*/
#ifndef skMSXMLELEMENT_H
#define skMSXMLELEMENT_H

#include "skString.h"
#include "msxmldefs.h"

/**
 * This class is used to create additional XML elements that are added to an XML document, it overrides Element from the Microsoft XML parser library
 */
class CLASSEXPORT skMSXMLElement : public XMLElement {
 public:
  /**
   * Constructor - passes an owner and a tagname
   */
  inline skXMLElement(XMLElement& owner,const skString& tagname){
  		(XMLElement)this = owner->ownerDocument->createElement(_bstr_t((const char *)tagname));
  }
};

#endif
