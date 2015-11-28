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

* $Id: skTextNode.h,v 1.11 2003/04/19 13:22:24 simkin_cvs Exp $
*/
#ifndef skTEXTNODE_H
#define skTEXTNODE_H

#include "skNode.h"

/**
 * This class represents a text node. This holds the text within or between two elements in an XML document
 */
class CLASSEXPORT skTextNode : public skNode
{
 public:
  /** Contructs a new text node with the given text 
   * @param text - the text to use as the node's value
   */
  IMPORT_C skTextNode(const skString& text);
  /**
  * Virtual destructor 
  */
  IMPORT_C virtual ~skTextNode();
  /**
   * returns the text within the text node
   *@return the text for this node 
   */
  IMPORT_C virtual skString getNodeValue() const;
  /**
   * sets the text in this node
   *@param s - the new text for this node
   */
  IMPORT_C virtual void setNodeValue(const skString& s);
  /**
   * this returns the type of this node
   * @return returns TEXT_NODE
   */
  IMPORT_C virtual NodeType getNodeType() const;
  /**
   * this method returns a new skTextNode that has the same text value as this one
   * @return a new copy of this node
   * @exception Symbian - a leaving function
   */
  IMPORT_C virtual skNode * clone();
  /**
   * writes the text in this node to the given destination
   * @param out - the destination to write to
   * @exception Symbian - a leaving function
   */
  IMPORT_C virtual void write(skOutputDestination& out) const;
  /**
   * writes the text in this node to a string
   * @return the string containing the text
   * @exception Symbian - a leaving function
   */
  IMPORT_C virtual skString toString() const;
  /**
   * Does a deep comparison against another node
   */
  IMPORT_C virtual bool equals(const skNode& other) const;
 protected:
  /** this member variable holds the text for this node */
  skString m_Text;
};
#endif
