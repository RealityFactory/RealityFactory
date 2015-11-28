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

* $Id: skNode.h,v 1.17 2003/04/19 13:22:24 simkin_cvs Exp $
*/
#ifndef skNODE_H
#define skNODE_H

#include "skAlist.h"

class CLASSEXPORT skElement;
class CLASSEXPORT skOutputDestination;

/**
 * This class describes a node within an XML document hierarchy. It is part of the classes used in the Simkin XML DOM (Document Object Model)
 */
class CLASSEXPORT skNode 
#ifdef __SYMBIAN32__
: public CBase
#endif
{
 public:
   /**
   * Virtual destructor
   */
   IMPORT_C virtual ~skNode();
  /** 
   * This enumeration is used to distinguish between different types of node in a document
   */
  enum NodeType {ELEMENT_NODE,CDATA_SECTION_NODE, TEXT_NODE};
  /**
   * This method returns the "value" of a node. Typically this would be the text for a text or cdata node.
   * @return returns a string. By default this is an empty string.
   */
  virtual IMPORT_C skString getNodeValue() const;
  /**
   * This method sets the "value" of the node. Typically this would be the text for a text or cdata node. For other nodes, this does nothing.
   * @param s - the new value
   */
  virtual IMPORT_C void setNodeValue(const skString& s);
  /**
   * This abstract virtual function returns the type of this node. It will be one of the NodeType values
   * @return a NodeType value indicating the class to which this node belongs
   */
  virtual IMPORT_C NodeType getNodeType() const=0;
  /**
   * This abstract virtual function requests that the node create a copy of itself. How this is done depends on the actual class of the node.
   * @return a new node which is a clone of this one. A "deep" copy is performed.
   * @exception Symbian - a leaving function
   */
  virtual IMPORT_C skNode * clone()=0;
  /**
   * This abstract virtual function writes a textual representation of this node and its children to a string
   * @return the string containing the node
   * @exception Symbian - a leaving function
   */
  virtual IMPORT_C skString toString() const=0;
  /**
   * This abstract virtual function writes a textual representation of this node and its children to the given destination
   * @param out - the destination to write to
   * @exception Symbian - a leaving function
   */
  virtual IMPORT_C void write(skOutputDestination& out) const=0;
  /**
   * This static method searches the given text for characters which must be "escaped" in an XML document
   * @param text the text to search for special characters within
   * @param include_quote if true, the function replaces quotes with the quote escape character, otherwise it ignores quotes
   * @return a version of text with the special characters replaced with escaped versions
   * @exception Symbian - a leaving function
   */
  static IMPORT_C skString escapeXMLDelimiters(const skString& text,bool include_quote);
  /** Sets the owning element */
  IMPORT_C void setParent(skElement * parent);
  /** Returns the owning element  */
  IMPORT_C skElement * getParent();
  /**
   * Does a deep comparison against another node
   */
  IMPORT_C virtual bool equals(const skNode& other) const=0;
 protected:
  /** Blank constructor */
  skNode();
  /** Pointer to the parent Element */
  skElement * m_Parent;
 private:
  /** Prevent nodes from being copied by the default C++ behaviour */
  skNode(const skNode&);
  skNode& operator=(const skNode& other);
};
#ifdef INSTANTIATE_TEMPLATES
EXTERN_TEMPLATE template class CLASSEXPORT skTAList<skNode>;
#endif

/**
 * This class represents a list of nodes
 */
class CLASSEXPORT skNodeList : public skTAList<skNode>{
};


#endif
