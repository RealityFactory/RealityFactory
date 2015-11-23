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

  $Id: skTreeNodeObject.cpp,v 1.22 2001/11/22 11:13:21 sdw Exp $
*/

#include "skTreeNodeObject.h"
#include "skTreeNode.h"
#include "skTreeNodeObjectEnumerator.h"
#include "skRValueArray.h"
#include "skInterpreter.h"
#include "skMethodTable.h"

skLITERAL(numChildren);
skLITERAL(enumerate);
skLITERAL(label);
skString s_colon=skSTR(":");
skString s_leftbracket=skSTR("[");
skString s_rightbracket=skSTR("]");

//------------------------------------------
skTreeNodeObject::skTreeNodeObject()
//------------------------------------------
  : m_Node(0),m_Created(false),m_MethodCache(0)
{
}
//------------------------------------------
skTreeNodeObject::skTreeNodeObject(const skString& location,skTreeNode * node,bool created)
//------------------------------------------
  : m_Node(node),m_Created(created),m_Location(location),m_MethodCache(0)
{
}
//------------------------------------------
skTreeNodeObject::~skTreeNodeObject()
//------------------------------------------
{
  if (m_Created)
    delete m_Node;
  delete m_MethodCache;
}
//------------------------------------------
int	skTreeNodeObject::executableType() const
//------------------------------------------
{
  return TREENODE_TYPE;
}
//------------------------------------------
int	skTreeNodeObject::intValue() const
//------------------------------------------
{
  return m_Node->intData();
}
//------------------------------------------
float	skTreeNodeObject::floatValue() const
//------------------------------------------
{
  return m_Node->floatData();
}
//------------------------------------------
bool skTreeNodeObject::boolValue() const
//------------------------------------------
{
  return m_Node->boolData();
}
//------------------------------------------
Char skTreeNodeObject::charValue() const
//------------------------------------------
{
  return m_Node->data().at(0);
}
//------------------------------------------
skString skTreeNodeObject::strValue() const
//------------------------------------------
{
  return m_Node->data();
}
//------------------------------------------
bool skTreeNodeObject::setValueAt(const skRValue& array_index,const skString& attribute,const skRValue& v)
//------------------------------------------
{
  bool bRet=false;
  int index=array_index.intValue();
  skTreeNode * child=m_Node->nthChild(index);
  if (child){
    bRet=true;
    skiExecutable * other=v.obj();
    if (other && other->executableType()==TREENODE_TYPE)
      child->copyItems(*((skTreeNodeObject *)other)->m_Node);
    else
      child->data(v.str());
  }else
    bRet=skExecutable::setValueAt(array_index,attribute,v);
  return bRet;
}
//------------------------------------------
bool skTreeNodeObject::setValue(const skString& s,const skString& attrib,const skRValue& v)
//------------------------------------------
{
  bool bRet=false;
  skTreeNode * child=m_Node->findChild(s);
  if (child){
    bRet=true;
    skiExecutable * other=v.obj();
    if (other && other->executableType()==TREENODE_TYPE)
      child->copyItems(*((skTreeNodeObject *)other)->m_Node);
    else
      child->data(v.str());
  }else
    bRet=skExecutable::setValue(s,attrib,v);
  return bRet;
}
//------------------------------------------
skTreeNode *	skTreeNodeObject::getNode()
//------------------------------------------
{
  return m_Node;
}
//------------------------------------------
void skTreeNodeObject::setNode(skTreeNode * n)
//------------------------------------------
{
  m_Node=n;
  delete m_MethodCache;
}
//------------------------------------------
bool skTreeNodeObject::getValueAt(const skRValue& array_index,const skString& attribute,skRValue& value)
//------------------------------------------
{
  bool bRet=false;
  int index=array_index.intValue();
  skTreeNode * child=m_Node->nthChild(index);
  if (child){
    bRet=true;
    value=skRValue(new skTreeNodeObject(m_Location+s_leftbracket+skString::from(index)+s_rightbracket,child,false),true);
  }else
    bRet=skExecutable::getValueAt(array_index,attribute,value);
  return bRet;
}
//------------------------------------------
bool skTreeNodeObject::getValue(const skString& name,const skString& attrib,skRValue& v)
//------------------------------------------
{
  bool bRet=false;
  if (name == s_numChildren){
    bRet=true;
    v=m_Node->numChildren();
  }else
  if (name == s_label){
    bRet=true;
    v=m_Node->label();
  }else{
    skTreeNode * child=m_Node->findChild(name);
    if (child){
      bRet=true;
      v=skRValue(new skTreeNodeObject(m_Location+s_colon+name,child,false),true);
    }else
      bRet=skExecutable::getValue(name,attrib,v);
  }
  return bRet;
}
//------------------------------------------
bool skTreeNodeObject::method(const skString& s,skRValueArray& args,skRValue& ret)
//------------------------------------------
{
  bool bRet=false;
  if (s==s_enumerate && (args.entries()==0 || args.entries()==1)){
    // return an enumeration object for this element
    bRet=true;
    if (args.entries()==0)
      ret=skRValue(new skTreeNodeObjectEnumerator(m_Node,getLocation()),true);
    else
      ret=skRValue(new skTreeNodeObjectEnumerator(m_Node,getLocation(),args[0].str()),true);
  }else{
    skString location=m_Location+s_colon+s;
    if (m_Node){
      skMethodDefNode * methNode=0;
      if (m_MethodCache!=0)
	methNode=m_MethodCache->value(&s);
      if (methNode==0){
	// if no parse tree is found - try and build one
	skTreeNode * func_node=0;
	func_node=m_Node->findChild(s);
	if (func_node){
	  bRet=true;
	  assert(skInterpreter::getInterpreter());
	  skInterpreter::getInterpreter()->executeString(location,this,func_node->data(),args,ret,&methNode);
	  if (methNode){
	    if (m_MethodCache==0)
	      m_MethodCache=new skMethodTable();
	    m_MethodCache->insertKeyAndValue(new skString(s),methNode);
	  }
	}else
	  bRet=skExecutable::method(s,args,ret);
      }else{
	// otherwise execute the parse tree immediately
	skInterpreter::getInterpreter()->executeParseTree(location,this,methNode,args,ret);
	bRet=true;
      }
    }else
      bRet=skExecutable::method(s,args,ret);
  }
  return bRet;
}
//------------------------------------------
skTreeNodeObject::skTreeNodeObject(const skTreeNodeObject& other)
//------------------------------------------
{
}
//------------------------------------------
skTreeNodeObject& skTreeNodeObject::operator=(const skTreeNodeObject& other)
//------------------------------------------
{
  return *this;
} 
//------------------------------------------
skString skTreeNodeObject::getLocation() const
//------------------------------------------
{
  return m_Location;
}
//------------------------------------------
skExecutableIterator * skTreeNodeObject::createIterator(const skString& qualifier)
//------------------------------------------
{
  return new skTreeNodeObjectEnumerator(m_Node,getLocation(),qualifier);
}
//------------------------------------------
skExecutableIterator * skTreeNodeObject::createIterator()
//------------------------------------------
{
  return new skTreeNodeObjectEnumerator(m_Node,getLocation());
}

