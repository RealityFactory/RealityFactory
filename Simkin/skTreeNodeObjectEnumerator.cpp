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

  $Id: skTreeNodeObjectEnumerator.cpp,v 1.8 2001/11/22 11:13:21 sdw Exp $
*/

#include "skTreeNodeObjectEnumerator.h"
#include "skTreeNodeObject.h"
#include "skRValue.h"
#include "skInterpreter.h"

skLITERAL(next);
skLITERAL(reset);

//-----------------------------------------------------------------
skTreeNodeObjectEnumerator::skTreeNodeObjectEnumerator(skTreeNode * element,const skString& location)
  //-----------------------------------------------------------------
  : m_Iter(*(element)),m_Location(location)
{
  m_CurrentNode=m_Iter();
}
//-----------------------------------------------------------------
skTreeNodeObjectEnumerator::skTreeNodeObjectEnumerator(skTreeNode * element,const skString& location,const skString& tag)
  //-----------------------------------------------------------------
  : m_Iter(*(element)),m_Location(location)
{
  // first wind forward to the first occurrence of the tag
  findNextNode();
}
//------------------------------------------
bool skTreeNodeObjectEnumerator::method(const skString& s,skRValueArray& args,skRValue& r)
  //------------------------------------------
{
  bool bRet=false;
  if (s==s_next){
    if (next(r)==false)
      r=skRValue(&skInterpreter::g_Null);
    bRet=true;
  }else if (s==s_reset){
    m_Iter.reset();
    if (m_Tag.length())
      findNextNode();
    else
      m_CurrentNode=m_Iter();
    bRet=true;
  }else
    bRet=skExecutable::method(s,args,r);
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
    skTreeNodeObject * obj=new skTreeNodeObject(m_Location,m_CurrentNode,false);
    r=skRValue(obj,true);
    if (m_Tag.length())
      findNextNode();
    else
      m_CurrentNode=m_Iter();
    ret=true;
  }
  return ret;
}
