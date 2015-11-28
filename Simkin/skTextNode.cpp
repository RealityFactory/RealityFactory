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

* $Id: skTextNode.cpp,v 1.11 2003/04/19 13:22:24 simkin_cvs Exp $
*/
#include "skTextNode.h"
#include "skOutputDestination.h"
//------------------------------------------
EXPORT_C skTextNode::skTextNode(const skString& text)
//------------------------------------------
  :m_Text(text)
{
}
//------------------------------------------
EXPORT_C skTextNode::~skTextNode()
//------------------------------------------
{
}
//------------------------------------------
EXPORT_C skString skTextNode::getNodeValue() const
//------------------------------------------
{
  return m_Text;
}
//------------------------------------------
EXPORT_C void skTextNode::setNodeValue(const skString& text)
//------------------------------------------
{
  m_Text=text;
}
//------------------------------------------
EXPORT_C skNode::NodeType skTextNode::getNodeType() const
//------------------------------------------
{
  return TEXT_NODE;
}
//------------------------------------------
EXPORT_C skNode * skTextNode::clone()
//------------------------------------------
{
  return new skTextNode(m_Text);
}
//------------------------------------------
EXPORT_C void skTextNode::write(skOutputDestination& out) const
//------------------------------------------
{
  out.write(escapeXMLDelimiters(m_Text,false));
}
//------------------------------------------
EXPORT_C skString skTextNode::toString() const
//------------------------------------------
{
  return escapeXMLDelimiters(m_Text,false);
}
//------------------------------------------
EXPORT_C bool skTextNode::equals(const skNode& other) const
//------------------------------------------
{
  bool equals=false;
  if (other.getNodeType()==getNodeType()){
    equals=(m_Text==((skTextNode *)&other)->m_Text);
  }
  return equals;
}
