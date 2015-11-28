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

  $Id: skMSXMLElementObjectEnumerator.cpp,v 1.4 2003/04/11 18:05:39 simkin_cvs Exp $
*/

#include "skMSXMLElementObjectEnumerator.h"
#include "skMSXMLElementObject.h"
#include "skRValue.h"
#include "skInterpreter.h"

//-----------------------------------------------------------------
skMSXMLElementObjectEnumerator::skMSXMLElementObjectEnumerator(XMLElement& element,bool add_if_not_present,const skString& location)
//-----------------------------------------------------------------
  : m_Element(element),m_NodeNum(0),m_AddIfNotPresent(add_if_not_present),m_Location(location)
{
  // first wind forward to the first occurrence of the tag
  findNextNode();
}
//-----------------------------------------------------------------
skMSXMLElementObjectEnumerator::skMSXMLElementObjectEnumerator(XMLElement& element,bool add_if_not_present,const skString& location,const skString& tag)
//-----------------------------------------------------------------
    : m_Element(element),m_NodeNum(0),m_Tag(tag),m_AddIfNotPresent(add_if_not_present),m_Location(location)
{
  // first wind forward to the first occurrence of the tag
  findNextNode();
}
//------------------------------------------
bool skMSXMLElementObjectEnumerator::method(const skString& s,skRValueArray& args,skRValue& r,skExecutableContext& ctxt)
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
void skMSXMLElementObjectEnumerator::findNextNode()
//------------------------------------------
{
  XMLNodeList nodes=m_Element->childNodes;
  _bstr_t sTagName;
  if (m_Tag.length()>0)
    sTagName=skMSXMLElementObject::fromString(m_Tag);
  for (;m_NodeNum<nodes->length;m_NodeNum++){
    XMLNode node=nodes->Getitem(m_NodeNum);
    if (node->nodeType==MSXML2::NODE_ELEMENT && (m_Tag.length()==0 || node->nodeName==sTagName)){
      break;
    }
  }
}
//------------------------------------------
bool skMSXMLElementObjectEnumerator::next(skRValue& r)
  //------------------------------------------
{
  bool ret=false;
  XMLNodeList nodes=m_Element->childNodes;
  if (m_NodeNum<nodes->length){
    XMLNode node=nodes->Getitem(m_NodeNum);
    assert(node->nodeType==MSXML2::NODE_ELEMENT);
    skMSXMLElementObject * obj=new skMSXMLElementObject(m_Location,(XMLElement)node);
    r=skRValue(obj,true);
    obj->setAddIfNotPresent(m_AddIfNotPresent);
    m_NodeNum++;
    findNextNode();
    ret=true;
  }
  return ret;
}

