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

  $Id: skTreeNodeObjectEnumerator.cpp,v 1.16 2003/04/11 18:05:39 simkin_cvs Exp $
*/

#include "skTreeNodeObjectEnumerator.h"
#include "skTreeNodeObject.h"
#include "skRValue.h"
#include "skInterpreter.h"


//-----------------------------------------------------------------
EXPORT_C skTreeNodeObjectEnumerator::skTreeNodeObjectEnumerator(skTreeNodeObject * obj,const skString& location)
//-----------------------------------------------------------------
  : m_Iter(*obj->getNode()),m_Location(location),m_Object(obj)
{
  m_CurrentNode=m_Iter();
}
//-----------------------------------------------------------------
EXPORT_C skTreeNodeObjectEnumerator::skTreeNodeObjectEnumerator(skTreeNodeObject * obj,const skString& location,const skString& tag)
//-----------------------------------------------------------------
  : m_Iter(*obj->getNode()),m_Location(location),m_Object(obj)
{
  // first wind forward to the first occurrence of the tag
  findNextNode();
}
//-----------------------------------------------------------------
skTreeNodeObjectEnumerator::~skTreeNodeObjectEnumerator()
//-----------------------------------------------------------------
{
}
//------------------------------------------
bool skTreeNodeObjectEnumerator::method(const skString& s,skRValueArray& args,skRValue& r,skExecutableContext& ctxt)
//------------------------------------------
{
  bool bRet=false;
  if (s==s_next){
    if (next(r)==false)
      r.assignObject(&ctxt.getInterpreter()->getNull());
    bRet=true;
  }else if (s==s_reset){
    m_Iter.reset();
    if (m_Tag.length())
      findNextNode();
    else
      m_CurrentNode=m_Iter();
    bRet=true;
  }else
    bRet=skExecutable::method(s,args,r,ctxt);
  return bRet;
}
/**
 * this method winds m_NodeNum onto the next matching element - or beyond the end of the list for the end of enumeration
 */ 
//------------------------------------------
void skTreeNodeObjectEnumerator::findNextNode()
//------------------------------------------
{
  m_CurrentNode=0;
  while ((m_CurrentNode=m_Iter())!=0){
    if (m_CurrentNode->label()==m_Tag)
      break;
  }
}
//------------------------------------------
bool skTreeNodeObjectEnumerator::next(skRValue& r)
//------------------------------------------
{
  bool ret=false;
  if (m_CurrentNode){
    skTreeNodeObject * obj=m_Object->createTreeNodeObject(m_Location,m_CurrentNode,false);
    r.assignObject(obj,true);
    if (m_Tag.length())
      findNextNode();
    else
      m_CurrentNode=m_Iter();
    ret=true;
  }
  return ret;
}
