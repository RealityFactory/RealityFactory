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

* $Id: skCDataNode.h,v 1.9 2003/04/19 13:22:23 simkin_cvs Exp $
*/
#ifndef skCDATANODE_H
#define skCDATANODE_H

#include "skTextNode.h"

/**
 * This node represents a CData node in an XML document
 */
class CLASSEXPORT skCDataNode : public skTextNode
{
 public:
  /**
   * Constructs a new CData node with the given text
   * @param text - the text for the CData node 
   */
  IMPORT_C skCDataNode(const skString& text);
  /**
  * Virtual destructor
  */
  IMPORT_C virtual ~skCDataNode();
  /**
   * Returns the type of this node
   * @return returns CDATA_NODE
   */
  IMPORT_C virtual NodeType getNodeType() const;
  /**
   * Returns a new CData node with the same text
   * @return a new CData node that's a copy of this one
   * @exception Symbian - a leaving function
   */
  IMPORT_C virtual skNode * clone();
  /**
   * writes the CData text in this node to the given destination
   * @param out - the destination to write to
   * @exception Symbian - a leaving function
   */
  IMPORT_C virtual void write(skOutputDestination& out) const;
};
#endif
