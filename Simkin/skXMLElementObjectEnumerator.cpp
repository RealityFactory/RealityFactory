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

  $Id: skXMLElementObjectEnumerator.cpp,v 1.11 2003/04/11 18:05:39 simkin_cvs Exp $
*/

#include "skXMLElementObjectEnumerator.h"
#include "skXMLElementObject.h"
#include "skRValue.h"
#include "skInterpreter.h"
#include <xercesc/dom/deprecated/DOM_NodeList.hpp>

//-----------------------------------------------------------------
skXMLElementObjectEnumerator::skXMLElementObjectEnumerator(DOM_Element element,bool add_if_not_present,const skString& location)
//-----------------------------------------------------------------
  : m_Element(element),m_NodeNum(0),m_AddIfNotPresent(add_if_not_present),m_Location(location)
{
  // first wind forward to the first occurrence of the tag
  findNextNode();
}
//-----------------------------------------------------------------
skXMLElementObjectEnumerator::skXMLElementObjectEnumerator(DOM_Element element,bool add_if_not_present,const skString& location,const skString& tag)
//-----------------------------------------------------------------
    : m_Element(element),m_NodeNum(0),m_Tag(tag),m_AddIfNotPresent(add_if_not_present),m_Location(location)
{
  // first wind forward to the first occurrence of the tag
  findNextNode();
}
//------------------------------------------
bool skXMLElementObjectEnumerator::method(const skString& s,skRValueArray& args,skRValue& r,skExecutableContext& ctxt)
  //------------------------------------------
{
  bool bRet=false;
  if (s=="next"){
    if (next(r)==false)
      r=skRValue(&ctxt.getInterpreter()->getNull());
    bRet=true;
  }else if (s=="reset"){
    m_NodeNum=0;
    findNextNode();
    bRet=true;
  }else
    bRet=skExecutable::method(s,args,r,ctxt);
  return bRet;
}
/**
 * this method winds m_NodeNum onto the next matching element - or beyond the end of the list for the end of enumeration
 */ 
//------------------------------------------
void skXMLElementObjectEnumerator::findNextNode()
//------------------------------------------
{
  DOM_NodeList nodes=m_Element.getChildNodes();
  DOMString sTagName;
  if (m_Tag.length()>0)
    sTagName=skXMLElementObject::fromString(m_Tag);
  for (;m_NodeNum<nodes.getLength();m_NodeNum++){
    DOM_Node node=nodes.item(m_NodeNum);
    if (node.getNodeType()==DOM_Node::ELEMENT_NODE && (m_Tag.length()==0 || node.getNodeName().equals(sTagName))){
      break;
    }
  }
}
//------------------------------------------
bool skXMLElementObjectEnumerator::next(skRValue& r)
  //------------------------------------------
{
  bool ret=false;
  DOM_NodeList nodes=m_Element.getChildNodes();
  if (m_NodeNum<nodes.getLength()){
    skXMLElementObject * obj=new skXMLElementObject(m_Location,*(DOM_Element *)&(nodes.item(m_NodeNum)));
    r=skRValue(obj,true);
    obj->setAddIfNotPresent(m_AddIfNotPresent);
    m_NodeNum++;
    findNextNode();
    ret=true;
  }
  return ret;
}

