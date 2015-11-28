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

  $Id: skElementObjectEnumerator.cpp,v 1.10 2003/04/19 13:22:23 simkin_cvs Exp $
*/

#include "skElementObjectEnumerator.h"
#include "skElementObject.h"
#include "skRValue.h"
#include "skInterpreter.h"

//-----------------------------------------------------------------
EXPORT_C skElementObjectEnumerator::skElementObjectEnumerator(skElementObject * element,const skString& location)
//-----------------------------------------------------------------
  : m_Object(element),m_NodeNum(0),m_Location(location)
{
  // first wind forward to the first occurrence of the tag
  findNextNode();
}
//-----------------------------------------------------------------
EXPORT_C skElementObjectEnumerator::skElementObjectEnumerator(skElementObject * element,const skString& location,const skString& tag)
//-----------------------------------------------------------------
    : m_Object(element),m_NodeNum(0),m_Tag(tag),m_Location(location)
{
  // first wind forward to the first occurrence of the tag
  findNextNode();
}
//-----------------------------------------------------------------
EXPORT_C skElementObjectEnumerator::~skElementObjectEnumerator()
//-----------------------------------------------------------------
{
}
//------------------------------------------
EXPORT_C bool skElementObjectEnumerator::method(const skString& s,skRValueArray& args,skRValue& r,skExecutableContext& context)
  //------------------------------------------
{
  bool bRet=false;
  if (s==s_next){
    if (next(r)==false)
      r.assignObject(&context.getInterpreter()->getNull(),false);
    bRet=true;
  }else if (s==s_reset){
    m_NodeNum=0;
    findNextNode();
    bRet=true;
  }else
    bRet=skExecutable::method(s,args,r,context);
  return bRet;
}
/**
 * this method winds m_NodeNum onto the next matching element - or beyond the end of the list for the end of enumeration
 */ 
//------------------------------------------
void skElementObjectEnumerator::findNextNode()
//------------------------------------------
{
  skNodeList& nodes=m_Object->getElement()->getChildNodes();
  for (;m_NodeNum<nodes.entries();m_NodeNum++){
    skNode * node=nodes[m_NodeNum];
    if (node->getNodeType()==skNode::ELEMENT_NODE && (m_Tag.length()==0 || ((skElement *)node)->getTagName()==m_Tag)){
      break;
    }
  }
}
//------------------------------------------
EXPORT_C bool skElementObjectEnumerator::next(skRValue& r)
  //------------------------------------------
{
  bool ret=false;
  skNodeList& nodes=m_Object->getElement()->getChildNodes();
  if (m_NodeNum<nodes.entries()){
    skElement * element=reinterpret_cast<skElement *>(nodes[m_NodeNum]);
    skElementObject * obj=m_Object->createElementObject(m_Location,element,false);
    r.assignObject(obj,true);
    m_NodeNum++;
    findNextNode();
    ret=true;
  }
  return ret;
}

