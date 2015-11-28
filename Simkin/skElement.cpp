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

* $Id: skElement.cpp,v 1.18 2003/05/19 17:58:33 simkin_cvs Exp $
*/
#include "skElement.h"
#include "skStringBuffer.h"
#include "skOutputDestination.h"
skNAMED_LITERAL(tag_start,skSTR("<"));
skNAMED_LITERAL(close_tag_start,skSTR("</"));
skNAMED_LITERAL(tag_end,skSTR(">"));
skNAMED_LITERAL(close_tag_end,skSTR("/>"));
skNAMED_LITERAL(space,skSTR(" "));
//------------------------------------------
EXPORT_C skElement::skElement(const skString& tagname)
  //------------------------------------------
  :m_TagName(tagname)
{
}
//------------------------------------------
EXPORT_C skElement::~skElement()
  //------------------------------------------
{
}
//------------------------------------------
EXPORT_C void skElement::appendChild(skNode * child)
  //------------------------------------------
{
  m_ChildNodes.append(child);
  child->setParent(this);
}
//------------------------------------------
EXPORT_C void skElement::removeAndDestroyChild(skNode * child)
  //------------------------------------------
{
  child->setParent(0);
  m_ChildNodes.removeAndDestroy(child);
}
//------------------------------------------
EXPORT_C void skElement::removeChild(skNode * child)
  //------------------------------------------
{
  child->setParent(0);
  m_ChildNodes.remove(child);
}
//------------------------------------------
EXPORT_C void skElement::setAttribute(const skString& name,const skString& value)
  //------------------------------------------
{
  m_Attributes.setAttribute(name,value);
}
//------------------------------------------
EXPORT_C bool skElement::removeAttribute(const skString& name)
  //------------------------------------------
{
  return m_Attributes.removeAttribute(name);
}
#ifdef __SYMBIAN32__
//------------------------------------------
EXPORT_C skString skElement::getAttribute(const TDesC& name) const
  //------------------------------------------
{
  skString attr_name;
  attr_name=name;
  return m_Attributes.getAttribute(attr_name);
}
#endif
//------------------------------------------
EXPORT_C skString skElement::getAttribute(const skString& name) const
  //------------------------------------------
{
  return m_Attributes.getAttribute(name);
}
//------------------------------------------
EXPORT_C skString skElement::getAttribute(const skString& name,const skString& default_value) const
  //------------------------------------------
{
  return m_Attributes.getAttribute(name,default_value);
}
//------------------------------------------
EXPORT_C skAttributeList& skElement::getAttributes()
  //------------------------------------------
{
  return m_Attributes;
}
//------------------------------------------
EXPORT_C skNodeList& skElement::getChildNodes()
  //------------------------------------------
{
  return m_ChildNodes;
}
//------------------------------------------
EXPORT_C skString skElement::getTagName() const
  //------------------------------------------
{
  return m_TagName;
}
//------------------------------------------
EXPORT_C  skNode::NodeType skElement::getNodeType() const
  //------------------------------------------
{
  return ELEMENT_NODE;
}
//------------------------------------------
EXPORT_C  skNode * skElement::clone()
  //------------------------------------------
{
  skElement * element=new skElement(m_TagName);
  for (USize i=0;i<m_Attributes.entries();i++){
    skAttribute * attr=m_Attributes[i];
    element->setAttribute(attr->getName(),attr->getValue());
  }
  for (USize j=0;j<m_ChildNodes.entries();j++){
    skNode * node=m_ChildNodes[j];
    element->appendChild(node->clone());
  }
  return element;
}
//------------------------------------------
EXPORT_C void skElement::write(skOutputDestination& out) const
  //------------------------------------------
{
  out.write(s_tag_start);
  out.write(m_TagName);
  for (USize i=0;i<m_Attributes.entries();i++){
    out.write(s_space); 
    m_Attributes[i]->write(out);
  }
  if (m_ChildNodes.entries()){
    out.write(s_tag_end);
    for (USize j=0;j<m_ChildNodes.entries();j++)
      m_ChildNodes[j]->write(out);
    out.write(s_close_tag_start);
    out.write(m_TagName);
    out.write(s_tag_end);
  }else{
    out.write(s_close_tag_end);
  }
}
//------------------------------------------
EXPORT_C skString skElement::toString() const
  //------------------------------------------
{
  skStringBuffer buffer(100);
  buffer.append(s_tag_start);
  buffer.append(m_TagName);
  for (USize i=0;i<m_Attributes.entries();i++){
    buffer.append(s_space);
    buffer.append(m_Attributes[i]->toString());
  }
  buffer.append(s_tag_end);
  for (USize j=0;j<m_ChildNodes.entries();j++)
    buffer.append(m_ChildNodes[j]->toString());
  buffer.append(s_close_tag_start);
  buffer.append(m_TagName);
  buffer.append(s_tag_end);
  return buffer.toString();
}
//------------------------------------------
EXPORT_C bool skElement::equals(const skNode& other) const
  //------------------------------------------
{
  bool equals=false;
  if (getNodeType()==other.getNodeType())
    equals=deepCompare(*(skElement*)&other,true);
  return equals;
}
//------------------------------------------
EXPORT_C bool skElement::operator==(const skElement& other) const
  //------------------------------------------
{
  // at this level, *do not* check tag name
  return deepCompare(other,false);
}
//------------------------------------------
EXPORT_C bool skElement::deepCompare(const skElement& other,bool check_tagname) const
  //------------------------------------------
{
  bool equals=false;
  // check tag name
  if (check_tagname==false || m_TagName==other.m_TagName){
    // check attributes
    if (m_Attributes.entries()==other.m_Attributes.entries()){
      equals=true;
      for (USize i=0;i<m_Attributes.entries();i++){
	skAttribute * attr=m_Attributes[i];
	if (other.getAttribute(attr->getName())!=attr->getValue()){
	  equals=false;
	  break;
	}
      }
      if (equals){
	// check children match
	if (m_ChildNodes.entries()==other.m_ChildNodes.entries()){
	  for (USize i=0;i<m_ChildNodes.entries();i++){
	    skNode * this_node=m_ChildNodes[i];
	    skNode * other_node=other.m_ChildNodes[i];
	    if (this_node->getNodeType()==other_node->getNodeType()){
	      if (this_node->equals(*other_node)==false){
		equals=false;
		break;
	      }
	    }else{
	      equals=false;
	      break;
	    }
	  }
	}
      }
    }
  }
  return equals;
}
