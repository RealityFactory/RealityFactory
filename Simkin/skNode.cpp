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

* $Id: skNode.cpp,v 1.14 2003/04/23 14:34:51 simkin_cvs Exp $
*/
#include "skNode.h"
#include "skStringBuffer.h"

//------------------------------------------
skNode::skNode()
//------------------------------------------
:m_Parent(0)
{
}
//------------------------------------------
skNode::skNode(const skNode& other)
//------------------------------------------
:m_Parent(0)
{
}
//------------------------------------------
EXPORT_C skNode::~skNode()
//------------------------------------------
{
}
//------------------------------------------
skNode& skNode::operator=(const skNode& other)
//------------------------------------------
{
  return *this;
}
//------------------------------------------
EXPORT_C skString skNode::getNodeValue() const
//------------------------------------------
{
  return skString();
}
//------------------------------------------
EXPORT_C void skNode::setNodeValue(const skString& )
//------------------------------------------
{
}
static const Char g_SpecialChars[]={'&','<','>','"'};
static const Char * const g_SpecialCharEscapes[]={skSTR("&amp;"),skSTR("&lt;"),skSTR("&gt;"),skSTR("&quot;")};
static const USize g_NumSpecialChars=sizeof(g_SpecialChars)/sizeof(Char);
//------------------------------------------
EXPORT_C skString skNode::escapeXMLDelimiters(const skString& text,bool include_quote)
//------------------------------------------
{
  skString out=text;
  bool need_to_scan=false;
  USize num_to_scan=g_NumSpecialChars;
  // don't replace the quote sign - so ignore the final special character
  if (include_quote==false)
    num_to_scan--;
  // first check if the text has any special characters in it
  for (USize i=0;i<num_to_scan;i++)
    if (text.indexOf(g_SpecialChars[i])!=-1){
      need_to_scan=true;
      break;
    }
  // if it does, need to scan and replace them
  if (need_to_scan){
    skStringBuffer buffer(text.length());
    for (USize i=0;i<text.length();i++){
      Char c=out.at(i);
      bool char_replaced=false;
      for (USize i=0;i<num_to_scan;i++)
        if (c==g_SpecialChars[i]){
          char_replaced=true;
          buffer.append(g_SpecialCharEscapes[i]);
          break;
        }
      if (char_replaced==false)
        buffer.append(c);
    }
    out=buffer.toString();
  }
  return out;
}
//------------------------------------------
EXPORT_C void skNode::setParent(skElement * parent)
//------------------------------------------
{
  m_Parent=parent;
}
//------------------------------------------
EXPORT_C skElement * skNode::getParent()
//------------------------------------------
{
  return m_Parent;
}

