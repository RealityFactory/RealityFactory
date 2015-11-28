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

  $Id: skElementObject.cpp,v 1.31 2003/11/20 16:24:22 sdw Exp $
*/

#include "skStringTokenizer.h"
#include "skElementObject.h"
#include "skElementObjectEnumerator.h"
#include "skRValueArray.h"
#include "skElement.h"
#include "skMethodTable.h"
#include "skInterpreter.h"
#include "skTextNode.h"
#include "skStringBuffer.h"
#include "skConstants.h"
#include "skOutputDestination.h"
#ifdef STL_STREAMS
#include <iostream>
using namespace std;
#endif

skLITERAL(ArrayElement);
skLITERAL(containsElement);
skLITERAL(addElement);
skLITERAL(removeElement);
skLITERAL(tagName);
skLITERAL(params);
skLITERAL(nodename);
skNAMED_LITERAL(comma_space,skSTR(", "));
skLITERAL(dump);

//------------------------------------------
EXPORT_C skElementObject::skElementObject(const skString& scriptLocation,skElement * elem,bool created)
  //------------------------------------------
  : m_Element(0)
{
  m_ScriptLocation=scriptLocation;
  m_Created=created;
  m_Element=elem;
  m_MethodCache=0;
  m_AddIfNotPresent=false;
  m_Created=created;
}
//------------------------------------------
EXPORT_C skElementObject::skElementObject()
  //------------------------------------------
{
  m_Element=0;
  m_MethodCache=0;
  m_AddIfNotPresent=false;
  m_Created=false;
}
//------------------------------------------
EXPORT_C skElementObject::~skElementObject()
  //------------------------------------------
{
  delete m_MethodCache;
  if (m_Created)
    delete m_Element;
}
#ifdef __SYMBIAN32__
//------------------------------------------
EXPORT_C void skElementObject::setElement(const TDesC& location,skElement * elem,bool created)
  //------------------------------------------
{
  skString s_location;
  s_location=location;
  setElement(s_location,elem,created);
}
#endif
//------------------------------------------
EXPORT_C void skElementObject::setElement(const skString& location,skElement * elem,bool created)
  //------------------------------------------
{
  if (m_Created)
    delete m_Element;
  delete m_MethodCache;
  m_MethodCache=0;
  m_Element=elem;
  m_Created=created;
  m_ScriptLocation=location;
}
//------------------------------------------
EXPORT_C int skElementObject::executableType() const
  //------------------------------------------
{
  return ELEMENT_TYPE;
}
//------------------------------------------
EXPORT_C int skElementObject::intValue() const
  //------------------------------------------
{
  int i=0;
  if (m_Element)
    i=getData(m_Element).to();
  return i;
}
#ifdef USE_FLOATING_POINT
//------------------------------------------
EXPORT_C float skElementObject::floatValue() const
  //------------------------------------------
{
  float f=0;
  if (m_Element)
    f=getData(m_Element).toFloat();
  return f;
}
#endif
//------------------------------------------
EXPORT_C bool skElementObject::boolValue() const
  //------------------------------------------
{
  bool ret=false;
  if (m_Element){
    skString value=getData(m_Element);
    if (value==s_true)
      ret=true;
  }
  return ret;
}
//------------------------------------------
EXPORT_C Char skElementObject::charValue() const
  //------------------------------------------
{
  Char c=0;
  if (m_Element){
    skString value=getData(m_Element);
    c=value.at(0);
  }
  return c;
}
//------------------------------------------
EXPORT_C skString skElementObject::strValue() const
  //------------------------------------------
{
  skString data;
  if (m_Element)
    data=getData(m_Element);
  return data;
}
//------------------------------------------
EXPORT_C bool skElementObject::setValueAt(const skRValue& array_index,const skString& attrib,const skRValue& v)
  //------------------------------------------
{
  bool bRet=true;
  if (m_Element){
    skiExecutable * other=0;
    bool otherIs=false;
    if (v.type()==skRValue::T_Object){
      other=v.obj();
      if (other!=0 && other->executableType()==ELEMENT_TYPE)
        otherIs=true;
    }
    int index=array_index.intValue();
    skElement * child=m_Element;
    child=findChild(m_Element,index);
    if (child==0){
      if (m_AddIfNotPresent){
        unsigned int num_children=countChildren(m_Element);
        if (index>=num_children){
          int num_to_add=index-num_children+1;
          skString tagname;
          SAVE_VARIABLE(tagname);
          tagname=s_ArrayElement;
          for (int i=0;i<num_to_add;i++){
            child=skNEW(skElement(tagname));
            m_Element->appendChild(child);
          }
          RELEASE_VARIABLE(tagname);
        }
      }else
        bRet=false;
    }
    if (child){
      if (otherIs)
        ((skElementObject *)other)->copyItemsInto(child);
      else
        if (attrib.length()==0)
          setData(child,v.str());
        else
          child->setAttribute(attrib,v.str());
    }
  }else
    bRet=false;
  if (bRet==false)
    bRet=skExecutable::setValueAt(array_index,attrib,v);
  return bRet;
}
//------------------------------------------
EXPORT_C bool skElementObject::setValue(const skString& name,const skString& attrib,const skRValue& v)
  //------------------------------------------
{
  bool bRet=true;
  if (m_Element){
    skiExecutable * other=0;
    bool otherIs=false;
    if (v.type()==skRValue::T_Object){
      other=v.obj();
      if (other!=0 && other->executableType()==ELEMENT_TYPE)
        otherIs=true;
    }
    skElement * child=m_Element;
    if (name.length()>0)
      child=findChild(m_Element,name);
    if (child==0){
      if (m_AddIfNotPresent){
        child=skNEW(skElement(name));
        m_Element->appendChild(child);
      }else
        bRet=false;
    }
    if (child){
      if (otherIs)
        ((skElementObject *)other)->copyItemsInto(child);
      else
        if (attrib.length()==0)
          setData(child,v.str());
        else
          child->setAttribute(attrib,v.str());
    }
  }else
    bRet=false;
  if (bRet==false)
    bRet=skExecutable::setValue(name,attrib,v);
  return bRet;
}
//------------------------------------------
EXPORT_C void skElementObject::copyItemsInto(skElement * other)
  //------------------------------------------
{
  // first clear the other node
  skNodeList& nodes=other->getChildNodes();
  while (nodes.entries() > 0)
    other->removeAndDestroyChild(nodes[0]);  
  if (m_Element==0)
    m_Element=skNEW(skElement(other->getTagName()));
  // now copy our nodes in
  skNodeList& our_nodes=m_Element->getChildNodes();
  if (our_nodes.entries()>0){
    int numNodes=our_nodes.entries();
    for (int i=0;i<numNodes;i++)
      other->appendChild(our_nodes[i]->clone());
  }
}
//------------------------------------------
EXPORT_C skElement * skElementObject::getElement()
  //------------------------------------------
{
  return m_Element;
}
//------------------------------------------
EXPORT_C const skElement * skElementObject::getElement() const
  //------------------------------------------
{
  return m_Element;
}
//------------------------------------------
EXPORT_C bool skElementObject::getValueAt(const skRValue& array_index,const skString& attribute,skRValue& value)
  //------------------------------------------
{
  bool bRet=true;
  if (m_Element){
    int index=array_index.intValue();
    skElement * child=findChild(m_Element,index);
    if (child==0){
      if (m_AddIfNotPresent){
        unsigned int num_children=countChildren(m_Element);
        if (index>=num_children){
          int num_to_add=index-num_children+1;
          skString tagname;
          SAVE_VARIABLE(tagname);
          tagname=s_ArrayElement;
          for (int i=0;i<num_to_add;i++){
            child=skNEW(skElement(tagname));
            m_Element->appendChild(child);
          }
          RELEASE_VARIABLE(tagname);
        }
      }else
        bRet=false;
    }
    if (child){
      if (attribute.length()==0)
        value.assignObject(createElementObject(skString::addStrings(m_ScriptLocation.ptr(),s_leftbracket,skString::from(index).ptr(),s_rightbracket),child,false),true);
      else{
        value=skRValue(child->getAttribute(attribute));
      }
    }
  }else
    bRet=false;
  if (bRet==false)
    bRet=skExecutable::getValueAt(array_index,attribute,value);
  return bRet;
}
//------------------------------------------
EXPORT_C skElementObject * skElementObject::createElementObject(const skString& location,skElement * element,bool created)
  //------------------------------------------
{
  skElementObject * obj= skNEW(skElementObject(location,element,created));
  obj->setAddIfNotPresent(getAddIfNotPresent());
  return obj;
}
//------------------------------------------
EXPORT_C bool skElementObject::getValue(const skString& name,const skString& attrib,skRValue& v) 
  //------------------------------------------
{
  bool bRet=true;
  if (m_Element){
    skElement * child=m_Element;
    if (name == s_nodename){
      v=m_Element->getTagName();
    }else if (name == s_numChildren){
      v=countChildren(m_Element);
    }else{
      if (name.length()>0){
        child=findChild(m_Element,name);
        if (child==0){
          if (m_AddIfNotPresent){
            child=skNEW(skElement(name));
            m_Element->appendChild(child);
          }else
            bRet=false;
        }
      }
      if (child){
        if (attrib.length()==0)
          v.assignObject(createElementObject(skString::addStrings(m_ScriptLocation.ptr(),s_colon,name.ptr()),child,false),true);
        else{
          v=skRValue(child->getAttribute(attrib));
        }
      }
    }
  }else
    bRet=false;
  if (bRet==false)
    bRet=skExecutable::getValue(name,attrib,v);
  return bRet;
}
//------------------------------------------
EXPORT_C skString skElementObject::getData(skElement * element)
  //------------------------------------------
{
  skStringBuffer str(50);
  skNodeList& nodes=element->getChildNodes();
  for (unsigned int i=0;i<nodes.entries();i++){
    skNode * node=nodes[i];
    skNode::NodeType type=node->getNodeType();
    if (type==skNode::CDATA_SECTION_NODE || type==skNode::TEXT_NODE)
      str.append(node->getNodeValue());
  }
  return str.toString();
}
//------------------------------------------
EXPORT_C void skElementObject::setData(skElement * element,const skString& data)
  //------------------------------------------
{
  // This needs to be adjusted - the element may have multiple
  // sequential text nodes... these need to be replaced altogether.
  // So the first one will have its data set, and the rest will be deleted.
  // We don't want to delete while we are iterating, though. So add them to a list.
  skNodeList& nodes=element->getChildNodes();
  skNodeList nodesToRemove;
  bool found=false;
  unsigned int i=0;
  for (i=0;i<nodes.entries();i++){
    skNode * node=nodes[i];
    skNode::NodeType type=node->getNodeType();
    if (type==skNode::CDATA_SECTION_NODE || type==skNode::TEXT_NODE){
      if (!found) {                     
        node->setNodeValue(data);
        found=true;
      } else {
        nodesToRemove.append(node);
      }
    }
  }
  if (found==false){
    element->appendChild(skNEW(skTextNode(data)));
  } else {
    for (unsigned int j = 0; j < nodesToRemove.entries(); j++)
      element->removeChild(nodesToRemove[i]);
  }
}
//------------------------------------------
EXPORT_C skElement * skElementObject::findChild(skElement * parent,int index)
  //------------------------------------------
{
  skElement * ret=0;
  if (parent){
    skNodeList& nodes=parent->getChildNodes();
    if (index<nodes.entries()){
      int element_count=0;
      for (unsigned int i=0;i<nodes.entries();i++){
        skNode * node=nodes[i];
        if (node->getNodeType()==skNode::ELEMENT_NODE){
          if (element_count==index){
            ret=reinterpret_cast<skElement *>(node);
            break;
          }else
            element_count++;
        }
      }
    }
  }
  return ret;
}
//------------------------------------------
EXPORT_C int skElementObject::countChildren(skElement * parent)
  //------------------------------------------
{
  int count=0;
  if (parent){
    skNodeList& nodes=parent->getChildNodes();
    for (unsigned int i=0;i<nodes.entries();i++){
      skNode * node=nodes[i];
      if (node->getNodeType()==skNode::ELEMENT_NODE){
        count++;
      }
    }
  }
  return count;
}
#ifdef __SYMBIAN32__
//------------------------------------------
EXPORT_C skElement * skElementObject::findChild(skElement * parent,const TDesC& tagname)
  //------------------------------------------
{
  skString s_tagname;
  s_tagname=tagname;
  return findChild(parent,s_tagname);
}
#endif
//------------------------------------------
EXPORT_C skElement * skElementObject::findChild(skElement * parent,const skString& tagname)
  //------------------------------------------
{
  skElement * ret=0;
  if (parent){
    skNodeList& nodes=parent->getChildNodes();
    for (unsigned int i=0;i<nodes.entries();i++){
      skNode * node=nodes[i];
      if (node->getNodeType()==skNode::ELEMENT_NODE && reinterpret_cast<skElement *>(node)->getTagName()==tagname){
        ret=reinterpret_cast<skElement *>(node);
        break;
      }
    }
  }
  return ret;
}
#ifdef __SYMBIAN32__
//------------------------------------------
EXPORT_C skElement * skElementObject::findChild(skElement * parent,const TDesC& tagname,const TDesC& attribute,const skString& value)
  //------------------------------------------
{
  skString tag_name;
  SAVE_VARIABLE(tag_name);
  tag_name=tagname;
  skString attr_name;
  SAVE_VARIABLE(attr_name);
  attr_name=attribute;
  skElement * ret=findChild(parent,tag_name,attr_name,value);
  RELEASE_VARIABLE(attr_name);
  RELEASE_VARIABLE(tag_name);
  return ret;
}
#endif
//------------------------------------------
EXPORT_C skElement * skElementObject::findChild(skElement * parent,const skString& tagname,const skString& attribute,const skString& value)
  //------------------------------------------
{
  skElement * ret=0;
  if (parent){
    skNodeList& nodes=parent->getChildNodes();
    for (unsigned int i=0;i<nodes.entries();i++){
      skNode * node=nodes[i];
      skNode::NodeType type=node->getNodeType();
      if (type==skNode::ELEMENT_NODE){
        skElement * thisElement=reinterpret_cast<skElement *>(node);
        if (thisElement->getTagName()==tagname){
          if (thisElement->getAttribute(attribute)==value){
            ret=thisElement;
            break;
          }
        }
      }
    }
  }
  return ret;
}
//------------------------------------------
EXPORT_C void skElementObject::setAttribute(skString name,const skString& value)
  //------------------------------------------
{
  if (m_Element)
    m_Element->setAttribute(name,value);
}
#ifdef __SYMBIAN32__
//------------------------------------------
EXPORT_C skString skElementObject::getAttribute(const TDesC& name) const
  //------------------------------------------
{
  return m_Element->getAttribute(name);
}
#endif
//------------------------------------------
EXPORT_C skString skElementObject::getAttribute(const skString& name) const
  //------------------------------------------
{
  skString a;
  if (m_Element)
    a=m_Element->getAttribute(name);
  return a;
}
//------------------------------------------
EXPORT_C bool skElementObject::method(const skString& s,skRValueArray& args,skRValue& ret,skExecutableContext& ctxt)  
  //------------------------------------------
{
  bool bRet=false;
  if (m_Element){
    if (s==s_containsElement){
      bRet=true;
      skElement * child=findChild(m_Element,args[0].str());
      if (child)
        ret=true;
      else
        ret=false;
    }else if (s==s_addElement){
      bRet=true;
      m_Element->appendChild(skNEW(skElement(args[0].str())));
    }else if (s==s_removeElement){
      bRet=true;
      skElement * child=findChild(m_Element,args[0].str());
      if (child)
        m_Element->removeAndDestroyChild(child);
    }else if (s==s_tagName){
      bRet=true;
      ret=m_Element->getTagName();
#ifdef STREAMS_ENABLED
    }else if (s==s_dump){
      bRet=true;
      skStringBuffer buffer(1024);
      skOutputString out(buffer);
      m_Element->write(out);
      cout << buffer.toString();
#endif
    }else if (s==s_enumerate && (args.entries()==0 || args.entries()==1)){
      // return an enumeration object for this element
      bRet=true;
      if (args.entries()==0)
        ret.assignObject(skNEW(skElementObjectEnumerator(this,getLocation())),true);
      else
        ret.assignObject(skNEW(skElementObjectEnumerator(this,getLocation(),args[0].str())),true);
    }else{
      skString location=skString::addStrings(m_ScriptLocation.ptr(),s_colon,s.ptr());
      if (m_Element){
        skMethodDefNode * methNode=0;
        if (m_MethodCache!=0)
          methNode=m_MethodCache->value(s);
        if (methNode==0){
          // no parse tree found in the cache - try and construct a new one
          skElement * node=skElementObject::findChild(m_Element,s_function,s_name,s);
          if (node){
            // we found some  with the script in - now parse it
            skString code=skElementObject::getData(node);
            bRet=true;
            // pull out the parameters from the "params" attribute
            skString params=node->getAttribute(s_params);
            skStringList paramList;
            if (params.length()>0){
              skStringTokenizer tokenizer;
              skString delims;
              SAVE_VARIABLE(delims);
              delims=s_comma_space;
              tokenizer.init(params,delims,false);
              while (tokenizer.hasMoreTokens())
                paramList.append(tokenizer.nextToken());
              RELEASE_VARIABLE(delims);
            }
            code=code.removeInitialBlankLines();
            ctxt.getInterpreter()->executeStringExternalParams(location,this,paramList,code,args,ret,&methNode,ctxt);
            if (methNode){
              if (m_MethodCache==0)
                m_MethodCache=skNEW(skMethodTable);
              m_MethodCache->insertKeyAndValue(s,methNode);
            }
          }else
            bRet=skExecutable::method(s,args,ret,ctxt);
        }else{  
          ctxt.getInterpreter()->executeParseTree(location,this,methNode,args,ret,ctxt);
          bRet=true;
        }
      }
    }
  }else
    bRet=false;
  if (bRet==false)
    bRet=skExecutable::method(s,args,ret,ctxt);
  return bRet;
}
//------------------------------------------
skElementObject::skElementObject(const skElementObject& /*other*/)
  //------------------------------------------
{
}
//------------------------------------------
skElementObject& skElementObject::operator=(const skElementObject& /*other*/)
  //------------------------------------------
{
  return *this;
}
//------------------------------------------
EXPORT_C skString skElementObject::getLocation() const
  //------------------------------------------
{
  return m_ScriptLocation;
}
//------------------------------------------
EXPORT_C void skElementObject::setLocation(const skString& loc) 
  //------------------------------------------
{
  m_ScriptLocation=loc;
}
//------------------------------------------
EXPORT_C void skElementObject::setAddIfNotPresent(bool enable)
  //------------------------------------------
{
  m_AddIfNotPresent=enable;
}
//------------------------------------------
EXPORT_C bool skElementObject::getAddIfNotPresent()
  //------------------------------------------
{
  return m_AddIfNotPresent;
}
//------------------------------------------
EXPORT_C skExecutableIterator * skElementObject::createIterator(const skString& qualifier)
  //------------------------------------------
{
  skExecutableIterator * iter=0;
  if (m_Element)
    iter=skNEW(skElementObjectEnumerator(this,getLocation(),qualifier));
  return iter;
}
//------------------------------------------
EXPORT_C skExecutableIterator * skElementObject::createIterator()
  //------------------------------------------
{
  skExecutableIterator * iter=0;
  if (m_Element)
    iter=skNEW(skElementObjectEnumerator(this,getLocation()));
  return iter;
}
//------------------------------------------
EXPORT_C skString skElementObject::getSource(const skString& location)
  //------------------------------------------
{
  skString src;
  if (m_Element){
    // extract the object name
    int index=location.indexOfLast(c_colon);
    skString name=location;
    if (index!=-1)
      name=location.substr(index+1);
    skElement * node=skElementObject::findChild(m_Element,s_function,s_name,name);
    if (node)
      src=skElementObject::getData(node);
    src=src.removeInitialBlankLines();
  }
  return src;
}
//------------------------------------------
EXPORT_C void skElementObject::getInstanceVariables(skRValueTable& table)
  //------------------------------------------
{
  if (m_Element){
    skNodeList& nodes=m_Element->getChildNodes();
    for (unsigned int i=0;i<nodes.entries();i++){
      skNode * node=nodes[i];
      skNode::NodeType type=node->getNodeType();
      if (type==skNode::ELEMENT_NODE){
        skElement * element=reinterpret_cast<skElement *>(node);
        skString name=element->getTagName();
        skRValue * value=skNEW(skRValue);
        SAVE_POINTER(value);
        value->assignObject(skNEW(skElementObject(name,element,false)),true);
        table.insertKeyAndValue(name,value);
        RELEASE_POINTER(value);
      }
    }
  }
}
//------------------------------------------
EXPORT_C void skElementObject::getAttributes(skRValueTable& table)
  //------------------------------------------
{
  if (m_Element){
    skAttributeList& attrs=m_Element->getAttributes();
    for (unsigned int i=0;i<attrs.entries();i++){
      skAttribute * attr=attrs[i];
      skRValue * value=skNEW(skRValue(attr->getValue()));
      SAVE_POINTER(value);
      table.insertKeyAndValue(attr->getName(),value);
      RELEASE_POINTER(value);
    }
  }
}
//------------------------------------------
EXPORT_C bool skElementObject::equals(const skiExecutable * o) const
  //------------------------------------------
{
  bool equals=false;
  // is the other an ElementObject?
  if (o->executableType()==executableType() && m_Element){
    const skElement * other=reinterpret_cast<const skElementObject *>(o)->getElement();
    if (other)
      // do a deep comparison on the elements
      equals=(*m_Element==*other);
  }else
    // otherwise just check the string value
    equals=(strValue()==o->strValue());
  return equals;
}
