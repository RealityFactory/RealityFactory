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

  $Id: skMSXMLElementObject.cpp,v 1.11 2003/04/16 13:23:06 simkin_cvs Exp $
*/

#include "skStringTokenizer.h"
#include "skMSXMLElementObject.h"
#include "skMSXMLElementObjectEnumerator.h"
#include "skRValueArray.h"
#include "skMSXMLElement.h"
#include "skMethodTable.h"
#include "skInterpreter.h"

ostream& operator<<(ostream& target, const _bstr_t& toWrite);

//------------------------------------------
skMSXMLElementObject::skMSXMLElementObject(const skString& scriptLocation,XMLElement& elem)
  //------------------------------------------
{
  m_ScriptLocation=scriptLocation;
  m_Element=elem;
  m_MethodCache=0;
  m_AddIfNotPresent=false;
}
//------------------------------------------
skMSXMLElementObject::skMSXMLElementObject()
  //------------------------------------------
{
  m_MethodCache=0;
  m_AddIfNotPresent=false;
}
//------------------------------------------
skMSXMLElementObject::~skMSXMLElementObject()
  //------------------------------------------
{
  delete m_MethodCache;
}
//------------------------------------------
void skMSXMLElementObject::setElement(XMLElement& elem)
  //------------------------------------------
{
  m_Element=elem;
  delete m_MethodCache;
  m_MethodCache=0;
}
//------------------------------------------
int skMSXMLElementObject::executableType() const
  //------------------------------------------
{
  return MSXMLELEMENT_TYPE;
}
//------------------------------------------
int skMSXMLElementObject::intValue() const
  //------------------------------------------
{
  return getData(m_Element).to();
}
#ifdef USE_FLOATING_POINT
//------------------------------------------
float skMSXMLElementObject::floatValue() const
  //------------------------------------------
{
  return getData(m_Element).toFloat();
}
#endif
//------------------------------------------
bool skMSXMLElementObject::boolValue() const
  //------------------------------------------
{
  bool ret=false;
  skString value=getData(m_Element);
  if (value==s_true)
    ret=true;
  return ret;
}
//------------------------------------------
Char skMSXMLElementObject::charValue() const
  //------------------------------------------
{
  skString value=getData(m_Element);
  return value.at(0);
}
//------------------------------------------
skString skMSXMLElementObject::strValue() const
  //------------------------------------------
{
  return getData(m_Element);
}
//------------------------------------------
bool skMSXMLElementObject::setValueAt(const skRValue& array_index,const skString& attrib,const skRValue& v)
  //------------------------------------------
{
  bool bRet=true;
  skiExecutable * other=0;
  bool otherIsXML=false;
  if (v.type()==skRValue::T_Object){
    other=v.obj();
    if (other!=0 && other->executableType()==MSXMLELEMENT_TYPE)
      otherIsXML=true;
  }
  int index=array_index.intValue();
  XMLElement child=m_Element;
  child=findChild(m_Element,index);
  if (child==0){
    if (m_AddIfNotPresent){
      unsigned int num_children=countChildren(m_Element);
      if (index>=num_children){
        int num_to_add=index-num_children+1;
        for (int i=0;i<num_to_add;i++){
          child=m_Element->ownerDocument->createElement(fromString("array_element"));
          m_Element->appendChild(child);
        }
      }
    }else
      bRet=false;
  }
  if (child!=0){
    if (otherIsXML)
      ((skMSXMLElementObject *)other)->copyItemsInto(child);
    else
      if (attrib.length()==0)
        setData(child,v.str());
      else
        child->setAttribute(_bstr_t((const char *)attrib),_bstr_t((const char *)v.str()));
  }
  if (bRet==false)
    bRet=skExecutable::setValueAt(array_index,attrib,v);
  return bRet;
}
//------------------------------------------
bool skMSXMLElementObject::setValue(const skString& name,const skString& attrib,const skRValue& v)
  //------------------------------------------
{
  bool bRet=true;
  skiExecutable * other=0;
  bool otherIsXML=false;
  if (v.type()==skRValue::T_Object){
    other=v.obj();
    if (other!=0 && other->executableType()==MSXMLELEMENT_TYPE)
      otherIsXML=true;
  }
  XMLElement child=m_Element;
  if (name.length()>0)
    child=findChild(m_Element,name);
  if (child==0){
    if (m_AddIfNotPresent){
      child=m_Element->ownerDocument->createElement(fromString(name));
      m_Element->appendChild(child);
    }else
      bRet=false;
  }
  if (child!=0){
    if (otherIsXML)
      ((skMSXMLElementObject *)other)->copyItemsInto(child);
    else
      if (attrib.length()==0)
        setData(child,v.str());
      else
        child->setAttribute(_bstr_t((const char *)attrib),_bstr_t((const char *)v.str()));
  }
  if (bRet==false)
    bRet=skExecutable::setValue(name,attrib,v);
  return bRet;
}
//------------------------------------------
void skMSXMLElementObject::copyItemsInto(XMLElement& other)
  //------------------------------------------
{
  // first clear the other node
  XMLNodeList nodes=other->GetchildNodes();
  while (nodes->length > 0){
    XMLNode child_node=nodes->Getitem(0);
    other->removeChild(child_node);  
  }
  bool bSameOwner = m_Element->ownerDocument == other->ownerDocument;
  if (m_Element!=0){
    // now copy our nodes in
    nodes=m_Element->GetchildNodes();
    if (nodes->length>0){
      int numNodes=nodes->length;
      for (int i=0;i<numNodes;i++)
        if (bSameOwner){
          XMLNode childNode = nodes->Getitem(i);
          childNode = childNode->cloneNode(true);
          other->appendChild(childNode);
        }
        else
          other->appendChild(other);    
    }
  }
}
//------------------------------------------
XMLElement skMSXMLElementObject::getElement()
  //------------------------------------------
{
  return m_Element;
}
//------------------------------------------
bool skMSXMLElementObject::getValueAt(const skRValue& array_index,const skString& attribute,skRValue& value)
  //------------------------------------------
{
  bool bRet=true;
  int index=array_index.intValue();
  XMLElement child=findChild(m_Element,index);
  if (child==0){
    if (m_AddIfNotPresent){
      unsigned int num_children=countChildren(m_Element);
      if (index>=num_children){
        int num_to_add=index-num_children+1;
        for (int i=0;i<num_to_add;i++){
          child=m_Element->ownerDocument->createElement(fromString("array_element"));
          m_Element->appendChild(child);
        }
      }
    }else
      bRet=false;
  }
  if (child!=0){
    if (attribute.length()==0)
      value=skRValue(createXMLElementObject(skString::addStrings(m_ScriptLocation,skSTR("["),
                                                                 skString::from(index),skSTR("]")),child),true);
    else{
      skString attrValue=getAttribute(child,attribute);
      value=skRValue(attrValue);
    }
  }else
    bRet=skExecutable::getValueAt(array_index,attribute,value);
  return bRet;
}
//------------------------------------------
skMSXMLElementObject * skMSXMLElementObject::createXMLElementObject(const skString& location,XMLElement& element)
  //------------------------------------------
{
  skMSXMLElementObject * obj= new skMSXMLElementObject(location,element);
  obj->setAddIfNotPresent(getAddIfNotPresent());
  return obj;
}
//------------------------------------------
bool skMSXMLElementObject::getValue(const skString& name,const skString& attrib,skRValue& v) 
  //------------------------------------------
{
  bool bRet=true;
  XMLElement child=m_Element;
  if (name == "nodename"){
    v=toString(m_Element->nodeName);
  }else if (name == "numChildren"){
    v=countChildren(m_Element);
  }else{
    if (name.length()>0){
      child=findChild(m_Element,name);
      if (child==0){
        if (m_AddIfNotPresent){
          child=m_Element->ownerDocument->createElement(fromString(name));
          m_Element->appendChild(child);
        }else
          bRet=false;
      }
    }
    if (child!=0){
      if (attrib.length()==0)
        v=skRValue(createXMLElementObject(skString::addStrings(m_ScriptLocation,s_colon,name),child),true);
      else{
        skString attrValue=getAttribute(child,attrib);
        v=skRValue(attrValue);
      }
    }else
      bRet=skExecutable::getValue(name,attrib,v);
  }
  return bRet;
}
//------------------------------------------
skString skMSXMLElementObject::getData(const XMLElement& element)
  //------------------------------------------
{
  _bstr_t str="";
  XMLNodeList nodes=element->GetchildNodes();
  for (int i=0;i<nodes->length;i++){
    XMLNode node=nodes->Getitem(i);
    int type=node->nodeType;
    if (type==MSXML2::NODE_CDATA_SECTION || type == MSXML2::NODE_TEXT){
      _variant_t v = node->nodeValue;
      if (v.vt == VT_BSTR)
        str += _bstr_t(v);
    }
  }
  return toString(str);
}
//------------------------------------------
void skMSXMLElementObject::setData(XMLElement& element,const skString& data)
  //------------------------------------------
{
  XMLNodeList nodes=element->GetchildNodes();
  bool found=false;
  for (int i=0;i<nodes->length;i++){
    XMLNode node=nodes->Getitem(i);
    int type=node->nodeType;
    if (type==MSXML2::NODE_CDATA_SECTION || type == MSXML2::NODE_TEXT){
      node->nodeValue = _variant_t( fromString(data) );
      found=true;
      break;
    }
  }
  if (found==false){
    element->appendChild(element->ownerDocument->createTextNode(fromString(data)));
  }
}
//------------------------------------------
XMLElement skMSXMLElementObject::findChild(XMLElement& parent,int index)
  //------------------------------------------
{
  XMLElement ret;
  if (parent!=0){
    XMLNodeList nodes=parent->GetchildNodes();
    if (index<nodes->length){
      int element_count=0;
      for (int i=0;i<nodes->length;i++){
        XMLNode node=nodes->Getitem(i);
        if (node->nodeType==MSXML2::NODE_ELEMENT){
          if (element_count==index){
            ret=node;
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
int skMSXMLElementObject::countChildren(XMLElement& parent)
  //------------------------------------------
{
  int count=0;
  if (parent!=0){
    XMLNodeList nodes=parent->GetchildNodes();
    for (int i=0;i<nodes->length;i++){
      XMLNode node=nodes->Getitem(i);
      if (node->nodeType==MSXML2::NODE_ELEMENT){
        count++;
      }
    }
  }
  return count;
}
//------------------------------------------
XMLElement skMSXMLElementObject::findChild(XMLElement& parent,const skString& tagname)
  //------------------------------------------
{
  XMLElement ret;
  if (parent!=0){
    XMLNodeList nodes=parent->GetchildNodes();
    _bstr_t sTagName=fromString(tagname);
    for (int i=0;i<nodes->length;i++){
      XMLNode node=nodes->Getitem(i);
      if (node->nodeType==MSXML2::NODE_ELEMENT && node->nodeName==sTagName){
        ret=node;
        break;
      }
    }
  }
  return ret;
}
//------------------------------------------
XMLElement skMSXMLElementObject::findChild(XMLElement& parent,const skString& tagname,const skString& attribute,const skString& value)
  //------------------------------------------
{
  XMLElement ret = 0;
  if (parent!=0){
    _bstr_t sTagName    (fromString(tagname));
    _bstr_t sValue              (fromString(value));
    _bstr_t sAttribute  (fromString(attribute));

    // queryString = "sTagName[@sAttribute="sValue"]
    _bstr_t queryString = sTagName + "[@" + sAttribute + "=" + "\"" + sValue + "\"]";
    ret=(parent->selectSingleNode(queryString));
  }
  return ret;
}
//------------------------------------------
void skMSXMLElementObject::setAttribute(skString name,const skString& value)
  //------------------------------------------
{
  m_Element->setAttribute(fromString(name),fromString(value));
}
//------------------------------------------
skString skMSXMLElementObject::getAttribute(const skString& name)
  //------------------------------------------
{
  return toString((bstr_t)m_Element->getAttribute(fromString(name)));
}
//------------------------------------------
bool skMSXMLElementObject::method(const skString& s,skRValueArray& args,skRValue& ret,skExecutableContext& ctxt)  
  //------------------------------------------
{
  bool bRet=false;
  if (s=="containsElement"){
    bRet=true;
    XMLElement child=findChild(m_Element,args[0].str());
    if (child==0==false)
      ret=true;
    else
      ret=false;
  }else if (s=="addElement"){
    bRet=true;
    m_Element->appendChild(m_Element->ownerDocument->createElement(fromString(args[0].str())));
  }else if (s=="tagName"){
    bRet=true;
    ret=toString(m_Element->nodeName);
  }else if (s==("dump")){
    bRet=true;
    save(cout);
  }else if (s=="enumerate" && (args.entries()==0 || args.entries()==1)){
    // return an enumeration object for this element
    bRet=true;
    if (args.entries()==0)
      ret=skRValue(new skMSXMLElementObjectEnumerator(m_Element,m_AddIfNotPresent,getLocation()),true);
    else
      ret=skRValue(new skMSXMLElementObjectEnumerator(m_Element,m_AddIfNotPresent,getLocation(),args[0].str()),true);
  }else{
    skString location=skString::addStrings(m_ScriptLocation,skSTR(":"),s);
    if (m_Element!=0){
      skMethodDefNode * methNode=0;
      if (m_MethodCache!=0)
        methNode=m_MethodCache->value(s);
      if (methNode==0){
        // no parse tree found in the cache - try and construct a new one
        XMLElement node=skMSXMLElementObject::findChild(m_Element,s_function,s_name,s);
        if (node!=0){
          // we found some XML with the script in - now parse it
          skString code=skMSXMLElementObject::getData(node);
          bRet=true;
          // pull out the parameters from the "params" attribute
          skString params=getAttribute(node,skSTR("params"));
          skStringList paramList;
          if (params.length()>0){
            skStringTokenizer tokenizer(params,skSTR(", "));
            while (tokenizer.hasMoreTokens())
              paramList.append(tokenizer.nextToken());
          }
          code=code.removeInitialBlankLines();
          ctxt.getInterpreter()->executeStringExternalParams(location,this,paramList,code,args,ret,&methNode,ctxt);
          if (methNode){
            if (m_MethodCache==0)
              m_MethodCache=new skMethodTable();
            m_MethodCache->insertKeyAndValue(s,methNode);
          }
        }else
          bRet=skExecutable::method(s,args,ret,ctxt);
      }else{    
        ctxt.getInterpreter()->executeParseTree(location,this,methNode,args,ret,ctxt);
        bRet=true;
      }
    }else
      bRet=skExecutable::method(s,args,ret,ctxt);
  }
  return bRet;
}
// ---------------------------------------------------------------------------
skString skMSXMLElementObject::getAttribute(XMLElement& elem,const skString& name)
  // ---------------------------------------------------------------------------
{
  _bstr_t attrib("");
  _variant_t v = elem->getAttribute(fromString(name));
  if (v.vt == VT_BSTR)
    attrib = v.bstrVal;
  return toString(attrib);
}
// ---------------------------------------------------------------------------
skString skMSXMLElementObject::toString(_bstr_t& str)
  // ---------------------------------------------------------------------------
{
  // nb this now owns the buffer, but does not delete it
  return skString((char *)str);
}
// ---------------------------------------------------------------------------
_bstr_t skMSXMLElementObject::fromString(const skString& str)
  // ---------------------------------------------------------------------------
{
  return _bstr_t((const char *)str);
}
//------------------------------------------
skMSXMLElementObject::skMSXMLElementObject(const skMSXMLElementObject& other)
  //------------------------------------------
{
}
//------------------------------------------
skMSXMLElementObject& skMSXMLElementObject::operator=(const skMSXMLElementObject& other)
  //------------------------------------------
{
  return *this;
}
//------------------------------------------
skString skMSXMLElementObject::getLocation() const
  //------------------------------------------
{
  return m_ScriptLocation;
}
//------------------------------------------
void skMSXMLElementObject::setAddIfNotPresent(bool enable)
  //------------------------------------------
{
  m_AddIfNotPresent=enable;
}
//------------------------------------------
bool skMSXMLElementObject::getAddIfNotPresent()
  //------------------------------------------
{
  return m_AddIfNotPresent;
}
//------------------------------------------
skExecutableIterator * skMSXMLElementObject::createIterator(const skString& qualifier)
  //------------------------------------------
{
  return new skMSXMLElementObjectEnumerator(m_Element,m_AddIfNotPresent,getLocation(),qualifier);
}
//------------------------------------------
skExecutableIterator * skMSXMLElementObject::createIterator()
  //------------------------------------------
{
  return new skMSXMLElementObjectEnumerator(m_Element,m_AddIfNotPresent,getLocation());
}
//------------------------------------------
void skMSXMLElementObject::save(ostream& out)
  //------------------------------------------
{
  XMLElement elem = getElement();
  out << (char *)elem->xml;

}
// ---------------------------------------------------------------------------
ostream& operator<< (ostream& target, const _bstr_t& s)
  // ---------------------------------------------------------------------------
{
  target << (char *)s;
  return target;
}
//------------------------------------------
skString skMSXMLElementObject::getSource(const skString& location)
  //------------------------------------------
{
  skString src;
  if (m_Element){
    // extract the object name
    int index=location.indexOfLast(c_colon);
    skString name=location;
    if (index!=-1)
      name=location.substr(index+1);
    XMLElement node=findChild(m_Element,s_function,s_name,name);
    if (node)
      src=getData(node);
    src=src.removeInitialBlankLines();
  }
  return src;
}
//------------------------------------------
void skMSXMLElementObject::getInstanceVariables(skRValueTable& table)
  //------------------------------------------
{
  if (m_Element){
    XMLNodeList nodes=m_Element->childNodes;
    for (int i=0;i<nodes->length;i++){
      XMLNode node=nodes->Getitem(i);
      int type=node->nodeType;
      if (type==MSXML2::NODE_ELEMENT){
        XMLElement element=node;
        skString name=toString(element->tagName);
        table.insertKeyAndValue(name,new skRValue(new skMSXMLElementObject(name,element),true));
      }
    }
  }
}
//------------------------------------------
void skMSXMLElementObject::getAttributes(skRValueTable& table)
  //------------------------------------------
{
  if (m_Element){
    XMLNamedNodeMap attrs=m_Element->Getattributes();
    long num_attrs=attrs->Getlength();
    for (long i=0;i<num_attrs;i++){
      XMLNode attr=attrs->Getitem(i);
      skString name=toString(attr->GetnodeName());
      skString value="";
      _variant_t v = attr->GetnodeValue();
      if (v.vt == VT_BSTR){
        _bstr_t b_value=v.bstrVal;
        value = toString(b_value);
      }
      table.insertKeyAndValue(name,new skRValue(value));
    }
  }
}
