/*
  Copyright 1996-2001
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

  $Id: skTreeNodeObjectEnumerator.h,v 1.7 2001/11/22 11:13:21 sdw Exp $
*/
#ifndef TREENODEOBJECTENUMERATOR_H
#define TREENODEOBJECTENUMERATOR_H

#include "skExecutable.h"
#include "skExecutableIterator.h"
#include "skTreeNode.h"

class CLASSEXPORT skTreeNodeObject;

/**
 * This class enumerates the elements in an TreeNodeObject 
 * 
*/
class CLASSEXPORT skTreeNodeObjectEnumerator : public skExecutable, public skExecutableIterator{
 public:
  /**
   * This constructor is for an enumerator that gives *all* the child elements of this element
   */
  skTreeNodeObjectEnumerator(skTreeNode * element,const skString& location);
  /**
   * This constructor is for an enumerator that gives the child elements of this element whose tag name matches that given
   */
  skTreeNodeObjectEnumerator(skTreeNode * element,const skString& location,const skString& tag);
  /**
   * This method exposes the following methods to Simkin scripts:
   * returns the next object in the enumeration - or null if there are no more
     */
  bool method(const skString& s,skRValueArray& args,skRValue& r);
  /**
   * This method implements the method in skExecutableIterator
   */
  bool next(skRValue&);
 private:
  void findNextNode();

  skString m_Tag;
  skTreeNodeListIterator m_Iter;
  skTreeNode * m_CurrentNode;
  skString m_Location;
};
#endif
