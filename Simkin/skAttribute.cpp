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

* $Id: skAttribute.cpp,v 1.3 2003/05/19 17:58:33 simkin_cvs Exp $
*/
#include "skAttribute.h"
#include "skStringBuffer.h"
#include "skNode.h"
#include "skOutputDestination.h"

skNAMED_LITERAL(equals_start,skSTR("=\""));
skNAMED_LITERAL(equals_end,skSTR("\""));
//------------------------------------------
EXPORT_C skString skAttribute::toString() const
  //------------------------------------------
{
  skStringBuffer buffer(100);
  buffer.append(getName());
  buffer.append(s_equals_start);
  buffer.append(skNode::escapeXMLDelimiters(getValue(),true));
  buffer.append(s_equals_end);
  return buffer.toString();
}
//------------------------------------------
EXPORT_C void skAttribute::write(skOutputDestination& out)
  //------------------------------------------
{
  out.write(m_Name);
  out.write(s_equals_start);
  out.write(skNode::escapeXMLDelimiters(m_Value,true));
  out.write(s_equals_end);
}
//------------------------------------------
EXPORT_C bool skAttributeList::removeAttribute(const skString& name)
//------------------------------------------
{
  bool removed=false;
  skAttribute * attr=findAttribute(name);
  if (attr){
    removeAndDestroy(attr);
    removed=true;
  }
  return removed;
}
//------------------------------------------
EXPORT_C void skAttributeList::setAttribute(const skString& name,const skString& value)
//------------------------------------------
{
  skAttribute * attr=findAttribute(name);
  if (attr)
    attr->setValue(value);
  else
    append(new skAttribute(name,value));
}
//------------------------------------------
EXPORT_C skAttribute * skAttributeList::findAttribute(const skString& name) const
//------------------------------------------
{
  skAttribute * ret=0;
  for (USize i=0;i<entries();i++){
    skAttribute * attr=(*this)[i];
    if (attr->getName()==name){
      ret=attr;
      break;
    }
  }
  return ret;
}
//------------------------------------------
EXPORT_C skString skAttributeList::getAttribute(const skString& name) const
//------------------------------------------
{
  skString value;
  skAttribute * attr=findAttribute(name);
  if (attr)
    value=attr->getValue();
  return value;
}
//------------------------------------------
EXPORT_C skString skAttributeList::getAttribute(const skString& name,const skString& default_value) const
//------------------------------------------
{
  skString value=default_value;
  skAttribute * attr=findAttribute(name);
  if (attr)
    value=attr->getValue();
  return value;
}
