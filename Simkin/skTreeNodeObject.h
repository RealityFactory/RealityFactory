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

* $Id: skTreeNodeObject.h,v 1.20 2001/11/22 11:13:21 sdw Exp $
*/


#ifndef skTREENODEOBJECT_H
#define skTREENODEOBJECT_H

#include "skExecutable.h"
class CLASSEXPORT skTreeNode;
class CLASSEXPORT skMethodTable;

#define TREENODE_TYPE	1

/**
 * This class gives an skExecutable wrapper to an skTreeNode object
 * The class implements methods from the Executable interface.
 * The methods getValue, setValue and method all search for matching child labels within the TreeNode document. Only the first matching label is used.  
 * <p>The class supports the following fields:<ul>
 * <li>"numChildren" - returns number of children of this node</li>
 * <li>"label" - returns label for this node</li>
 * <li>enumerate([label]) - returns an skTreeNodeObjectEnumerator which enumerates over the child nodes of this node. If no label is passed the enumerator lists all the children. A label can be passed to show only children with the matching label.</li>
 * </ul>
*/
class CLASSEXPORT skTreeNodeObject : public skExecutable
{
 public:
  /**
   * Default Constructor
   */
  skTreeNodeObject();
  /**
   * Constructor providing a treenode
   * @param location - the location of this treenode (e.g. file or database), this is used in error messages
   * @param node - the node itself
   * @param created - set this to true to allow the TreeNodeObject to delete the node when it is deleted
   */
  skTreeNodeObject(const skString& location,skTreeNode * node,bool created);
  /**
   * Destructor - will delete the associated node if the created flag is set
   */
  ~skTreeNodeObject();
  /**
   * returns the value TREENODE_TYPE to identify this as a TreeNodeObject
   */
  int executableType() const;			
  /**
   * Returns the data field of the node as an integer
   */
  int intValue() const;
  /**
   * Returns the data field of the node as a float
   */
  float floatValue() const;
  /**
   * Returns the data field of the node as a boolean
   */
  bool boolValue() const;
  /**
   * Returns the first character of the data field of the node
   */
  Char charValue() const;
  /**
   * Returns the data field of the node as a string
   */
  skString strValue() const;
  /**
   * Sets a value within the node. The field name is matched to a child of the treenode with the same label.
   * If a match is found, the child's data is changed. 
   * @param name - the name of the field
   * @param attribute - the attribute name is ignored
   * @param value - the value to be assigned to the child. If this is a TREENODE_TYPE object, the full treenode is copied
   */
  bool setValue(const skString& name,const skString& attribute,const skRValue& value);
  /**
   * Sets a value within the nth node of the tree node. 
   * @param array_index - the identifier of the item - this might be a string, integer or any other legal value
   * @param attribute - the attribute name to set (may be blank)
   * @param value - the value to be set
   * @return true if the field was changed, false if the field could not be set or found
   */
  bool setValueAt(const skRValue& array_index,const skString& attribute,const skRValue& value); 
  /**
   * Retrieves a value from within the node. The field name is matched to a child of the treenode with the same label.
   * If a match is found, a new TreeNodeObject encapsulating the child is returned. 
   */
  bool getValue(const skString& name,const skString& attribute,skRValue& v);
  /**
   * Retrieves the nth value from within the node. If the array index falls within the range of the number of children of this node, 
   * a new TreeNodeObject encapsulating the child is returned. 
   */
  bool getValueAt(const skRValue& array_index,const skString& attribute,skRValue& value);
  /**
   * This function attempts to call a method defined within the TreeNode. It searches for a child whose label matches the method name, and tries to execute its data as a Simkin script
   * @param name - the name of the method
   * @param args - the arguments to pass to the method
   * @param ret - the RValue to receive the results of the method call
   * @return true if the method was found, otherwise false
   */
  bool method(const skString& name,skRValueArray& args,skRValue& ret);
  /**
   * This function returns the treenode wrapped by this object
   */
  skTreeNode * getNode();
  /**
   * This function changes the node associated with this object
   */
  void setNode(skTreeNode * node);
  /**
   * This function tests if this object is equal to the other object. It does this by checking the string values are equal
   */
  //  bool equals(skExecutable * o) const;
  /**
   * This function returns the location associated with this object (typically a file name)
   */
  skString getLocation() const;
  /**
   * This function returns an skExecutableIterator object which is used in the for each statement. It will iterate over nodes with the given node label.
   * @param qualifier node label - only nodes with this label will appear in the iteration
   */
  skExecutableIterator * createIterator(const skString& qualifier);
  /**
   * This function returns an skExecutableIterator object which is used in the for each statement. It will iterate over *all* children of this node
   */
  skExecutableIterator * createIterator();
 protected:
  /**
   * the location the node came from
   */
  skString m_Location;
 private:
  /**
   * the underlying node
   */
  skTreeNode * m_Node;
  /**
   * if this flag is true, the TreeNodeObject will delete the underlying node in its destructor
   */
  bool m_Created;
  /**
   * this cache is used to hold parse trees for methods already executed
   */
  skMethodTable * m_MethodCache;
  /**
   * Executables can't be copied
   */
  skTreeNodeObject(const skTreeNodeObject&);
  /**
   * Executables can't be copied
   */
  skTreeNodeObject& operator=(const skTreeNodeObject&);
};
#endif
