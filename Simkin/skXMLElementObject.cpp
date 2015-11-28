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

  $Id: skXMLElementObject.cpp,v 1.50 2003/04/16 13:23:06 simkin_cvs Exp $
*/

#include "skStringTokenizer.h"
#include "skXMLElementObject.h"
#include "skXMLElementObjectEnumerator.h"
#include "skRValueArray.h"
#include "skXMLElement.h"
#include "skMethodTable.h"
#include "skInterpreter.h"

#include <xercesc/dom/deprecated/DOMString.hpp>
#include <xercesc/dom/deprecated/DOM_NodeList.hpp>
#include <xercesc/dom/deprecated/DOM_CharacterData.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/dom/deprecated/DOM_NamedNodeMap.hpp>
#include <xercesc/dom/deprecated/DOMParser.hpp>


#include <xercesc/dom/deprecated/DOM_NamedNodeMap.hpp>
ostream& operator<<(ostream& target, const DOMString& toWrite);

//------------------------------------------
skXMLElementObject::skXMLElementObject(const skString& scriptLocation,DOM_Element elem)
  //------------------------------------------
{
  m_ScriptLocation=scriptLocation;
  m_Element=elem;
  m_MethodCache=0;
  m_AddIfNotPresent=false;
}
//------------------------------------------
skXMLElementObject::skXMLElementObject()
  //------------------------------------------
{
  m_MethodCache=0;
  m_AddIfNotPresent=false;
}
//------------------------------------------
skXMLElementObject::~skXMLElementObject()
  //------------------------------------------
{
  delete m_MethodCache;
}
//------------------------------------------
void skXMLElementObject::setElement(DOM_Element elem)
  //------------------------------------------
{
  m_Element=elem;
  delete m_MethodCache;
  m_MethodCache=0;
}
//------------------------------------------
int skXMLElementObject::executableType() const
  //------------------------------------------
{
  return XMLELEMENT_TYPE;
}
//------------------------------------------
int skXMLElementObject::intValue() const
  //------------------------------------------
{
  return getData(m_Element).to();
}
#ifdef USE_FLOATING_POINT
//------------------------------------------
float skXMLElementObject::floatValue() const
  //------------------------------------------
{
  return getData(m_Element).toFloat();
}
#endif
//------------------------------------------
bool skXMLElementObject::boolValue() const
  //------------------------------------------
{
  bool ret=false;
  skString value=getData(m_Element);
  if (value==s_true)
    ret=true;
  return ret;
}
//------------------------------------------
Char skXMLElementObject::charValue() const
  //------------------------------------------
{
  skString value=getData(m_Element);
  return value.at(0);
}
//------------------------------------------
skString skXMLElementObject::strValue() const
  //------------------------------------------
{
  return getData(m_Element);
}
//------------------------------------------
bool skXMLElementObject::setValueAt(const skRValue& array_index,const skString& attrib,const skRValue& v)
  //------------------------------------------
{
  bool bRet=true;
  skiExecutable * other=0;
  bool otherIsXML=false;
  if (v.type()==skRValue::T_Object){
    other=v.obj();
    if (other!=0 && other->executableType()==XMLELEMENT_TYPE)
      otherIsXML=true;
  }
  int index=array_index.intValue();
  DOM_Element child=m_Element;
  child=findChild(m_Element,index);
  if (child.isNull()){
    if (m_AddIfNotPresent){
      unsigned int num_children=countChildren(m_Element);
      if (index>=num_children){
        int num_to_add=index-num_children+1;
        for (int i=0;i<num_to_add;i++){
          child=m_Element.getOwnerDocument().createElement(fromString("array_element"));
          m_Element.appendChild(child);
        }
      }
    }else
      bRet=false;
  }
  if (child.isNull()==false){
    if (otherIsXML)
      ((skXMLElementObject *)other)->copyItemsInto(child);
    else
      if (attrib.length()==0)
        setData(child,v.str());
      else
        child.setAttribute(DOMString((const char *)attrib),DOMString((const char *)v.str()));
  }
  if (bRet==false)
    bRet=skExecutable::setValueAt(array_index,attrib,v);
  return bRet;
}
//------------------------------------------
bool skXMLElementObject::setValue(const skString& name,const skString& attrib,const skRValue& v)
  //------------------------------------------
{
  bool bRet=true;
  skiExecutable * other=0;
  bool otherIsXML=false;
  if (v.type()==skRValue::T_Object){
    other=v.obj();
    if (other!=0 && other->executableType()==XMLELEMENT_TYPE)
      otherIsXML=true;
  }
  DOM_Element child=m_Element;
  if (name.length()>0)
    child=findChild(m_Element,name);
  if (child.isNull()){
    if (m_AddIfNotPresent){
      child=m_Element.getOwnerDocument().createElement(fromString(name));
      m_Element.appendChild(child);
    }else
      bRet=false;
  }
  if (child.isNull()==false){
    if (otherIsXML)
      ((skXMLElementObject *)other)->copyItemsInto(child);
    else
      if (attrib.length()==0)
        setData(child,v.str());
      else
        child.setAttribute(DOMString((const char *)attrib),DOMString((const char *)v.str()));
  }
  if (bRet==false)
    bRet=skExecutable::setValue(name,attrib,v);
  return bRet;
}
//------------------------------------------
void skXMLElementObject::copyItemsInto(DOM_Element other)
  //------------------------------------------
{
  // first clear the other node
  DOM_NodeList nodes=other.getChildNodes();
  while (nodes.getLength() > 0)
    other.removeChild(nodes.item(0));  
  bool bSameOwner = m_Element.getOwnerDocument() == other.getOwnerDocument();
  if (m_Element.isNull()==false){
    // now copy our nodes in
    nodes=m_Element.getChildNodes();
    if (nodes.getLength()>0){
      int numNodes=nodes.getLength();
      for (int i=0;i<numNodes;i++)
	if (bSameOwner)
	  other.appendChild(nodes.item(i).cloneNode(true));
	else
	  other.appendChild(other.getOwnerDocument().importNode(nodes.item(i),true));	
    }
  }
}
//------------------------------------------
DOM_Element skXMLElementObject::getElement()
  //------------------------------------------
{
  return m_Element;
}
//------------------------------------------
bool skXMLElementObject::getValueAt(const skRValue& array_index,const skString& attribute,skRValue& value)
  //------------------------------------------
{
  bool bRet=true;
  int index=array_index.intValue();
  DOM_Element child=findChild(m_Element,index);
  if (child.isNull()){
    if (m_AddIfNotPresent){
      unsigned int num_children=countChildren(m_Element);
      if (index>=num_children){
        int num_to_add=index-num_children+1;
        for (int i=0;i<num_to_add;i++){
          child=m_Element.getOwnerDocument().createElement(fromString("array_element"));
          m_Element.appendChild(child);
        }
      }
    }else
      bRet=false;
  }
  if (child.isNull()==false){
    if (attribute.length()==0)
      value=skRValue(createXMLElementObject(skString::addStrings(m_ScriptLocation,"[",skString::from(index),"]"),child),true);
    else{
      DOMString attrName=fromString(attribute);
      DOMString attrValue=child.getAttribute(attrName);
      value=skRValue(toString(attrValue));
    }
  }else
    bRet=skExecutable::getValueAt(array_index,attribute,value);
  return bRet;
}
//------------------------------------------
skXMLElementObject * skXMLElementObject::createXMLElementObject(const skString& location,DOM_Element element)
//------------------------------------------
{
  skXMLElementObject * obj= new skXMLElementObject(location,element);
  obj->setAddIfNotPresent(getAddIfNotPresent());
  return obj;
}
//------------------------------------------
bool skXMLElementObject::getValue(const skString& name,const skString& attrib,skRValue& v) 
  //------------------------------------------
{
  bool bRet=true;
  DOM_Element child=m_Element;
  if (name == "nodename"){
    v=toString(m_Element.getNodeName());
  }else if (name == "numChildren"){
    v=countChildren(m_Element);
  }else{
    if (name.length()>0){
      child=findChild(m_Element,name);
      if (child.isNull()){
        if (m_AddIfNotPresent){
	        child=m_Element.getOwnerDocument().createElement(fromString(name));
	        m_Element.appendChild(child);
        }else
	        bRet=false;
      }
    }
    if (child.isNull()==false){
      if (attrib.length()==0)
        v=skRValue(createXMLElementObject(skString::addStrings(m_ScriptLocation,s_colon,name),child),true);
      else{
        DOMString attrName=fromString(attrib);
        DOMString attrValue=child.getAttribute(attrName);
        v=skRValue(toString(attrValue));
      }
    }else
      bRet=skExecutable::getValue(name,attrib,v);
  }
  return bRet;
}
//------------------------------------------
skString skXMLElementObject::getData(DOM_Element element)
  //------------------------------------------
{
  DOMString str="";
  DOM_NodeList nodes=element.getChildNodes();
  for (unsigned int i=0;i<nodes.getLength();i++){
    DOM_Node node=nodes.item(i);
    int type=node.getNodeType();
    if (type==DOM_Node::CDATA_SECTION_NODE || type==DOM_Node::TEXT_NODE)
      str+=node.getNodeValue();
  }
  return toString(str);
}
//------------------------------------------
void skXMLElementObject::setData(DOM_Element element,const skString& data)
  //------------------------------------------
{
  DOM_NodeList nodes=element.getChildNodes();
  bool found=false;
  for (unsigned int i=0;i<nodes.getLength();i++){
    DOM_Node node=nodes.item(i);
    int type=node.getNodeType();
    if (type==DOM_Node::CDATA_SECTION_NODE || type==DOM_Node::TEXT_NODE){
      node.setNodeValue(fromString(data));
      found=true;
      break;
    }
  }
  if (found==false){
    element.appendChild(element.getOwnerDocument().createTextNode(fromString(data)));
  }
}
//------------------------------------------
DOM_Element skXMLElementObject::findChild(DOM_Element parent,int index)
  //------------------------------------------
{
  DOM_Element ret;
  if (parent.isNull()==false){
    DOM_NodeList nodes=parent.getChildNodes();
    if (index<nodes.getLength()){
      int element_count=0;
      for (unsigned int i=0;i<nodes.getLength();i++){
        DOM_Node node=nodes.item(i);
        if (node.getNodeType()==DOM_Node::ELEMENT_NODE){
	        if (element_count==index){
	          ret=*(DOM_Element *)&node;
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
int skXMLElementObject::countChildren(DOM_Element parent)
  //------------------------------------------
{
  int count=0;
  if (parent.isNull()==false){
    DOM_NodeList nodes=parent.getChildNodes();
    for (unsigned int i=0;i<nodes.getLength();i++){
      DOM_Node node=nodes.item(i);
      if (node.getNodeType()==DOM_Node::ELEMENT_NODE){
        count++;
      }
    }
  }
  return count;
}
//------------------------------------------
DOM_Element skXMLElementObject::findChild(DOM_Element parent,const skString& tagname)
  //------------------------------------------
{
  DOM_Element ret;
  if (parent.isNull()==false){
    DOM_NodeList nodes=parent.getChildNodes();
    DOMString sTagName=fromString(tagname);
    for (unsigned int i=0;i<nodes.getLength();i++){
      DOM_Node node=nodes.item(i);
      if (node.getNodeType()==DOM_Node::ELEMENT_NODE && node.getNodeName().equals(sTagName)){
        ret=*(DOM_Element *)&node;
        break;
      }
    }
  }
  return ret;
}
//------------------------------------------
skString skXMLElementObject::getSource(const skString& location)
//------------------------------------------
{
  skString src;
  if (m_Element.isNull()==false){
    // extract the object name
    int index=location.indexOfLast(c_colon);
    skString name=location;
    if (index!=-1)
      name=location.substr(index+1);
    DOM_Element node=findChild(m_Element,s_function,s_name,name);
    if (node.isNull()==false)
      src=getData(node);
    src=src.removeInitialBlankLines();
  }
  return src;
}
//------------------------------------------
void skXMLElementObject::getInstanceVariables(skRValueTable& table)
//------------------------------------------
{
  if (m_Element.isNull()==false){
    DOM_NodeList nodes=m_Element.getChildNodes();
    for (unsigned int i=0;i<nodes.getLength();i++){
      DOM_Node node=nodes.item(i);
      int type=node.getNodeType();
      if (type==DOM_Node::ELEMENT_NODE){
	      DOM_Element element=*(DOM_Element *)&node;
        skString name=toString(element.getNodeName());
        table.insertKeyAndValue(name,
                    new skRValue(new skXMLElementObject(name,element),true));
      }
    }
  }
}
//------------------------------------------
void skXMLElementObject::getAttributes(skRValueTable& table)
//------------------------------------------
{
  if (m_Element.isNull()==false){
    const DOM_NamedNodeMap&  attrs=m_Element.getAttributes();
    for (unsigned int i=0;i<attrs.getLength();i++){
      DOM_Node attr=attrs.item(i);
      skString name=toString(attr.getNodeName());
      skString value=toString(attr.getNodeValue());
      table.insertKeyAndValue(name,new skRValue(value));
    }
  }
}
//------------------------------------------
DOM_Element skXMLElementObject::findChild(DOM_Element parent,const skString& tagname,const skString& attribute,const skString& value)
  //------------------------------------------
{
  DOM_Element ret;
  if (parent.isNull()==false){
    DOM_NodeList nodes=parent.getChildNodes();
    DOMString sTagName=fromString(tagname);
    DOMString sValue=fromString(value);
    DOMString sAttribute=fromString(attribute);
    for (unsigned int i=0;i<nodes.getLength();i++){
      DOM_Node node=nodes.item(i);
      int type=node.getNodeType();
      DOMString nodeName=node.getNodeName();
      DOMString nodeValue=node.getNodeValue();
      if (type==DOM_Node::ELEMENT_NODE)
        if (nodeName.equals(sTagName)){
	        DOM_Element thisElement=*(DOM_Element *)&node;
	        if (thisElement.getAttribute(sAttribute).equals(sValue)){
	          ret=thisElement;
	          break;
	        }
        }
    }
  }
  return ret;
}
//------------------------------------------
void skXMLElementObject::setAttribute(skString name,const skString& value)
  //------------------------------------------
{
  m_Element.setAttribute(fromString(name),fromString(value));
}
//------------------------------------------
skString skXMLElementObject::getAttribute(const skString& name)
  //------------------------------------------
{
  return toString(m_Element.getAttribute(fromString(name)));
}
//------------------------------------------
bool skXMLElementObject::method(const skString& s,skRValueArray& args,skRValue& ret,skExecutableContext& ctxt)  
  //------------------------------------------
{
  bool bRet=false;
  if (s=="containsElement"){
    bRet=true;
    DOM_Element child=findChild(m_Element,args[0].str());
    if (child.isNull()==false)
      ret=true;
    else
      ret=false;
  }else if (s=="addElement"){
    bRet=true;
    m_Element.appendChild(m_Element.getOwnerDocument().createElement(fromString(args[0].str())));
  }else if (s=="tagName"){
    bRet=true;
    ret=toString(m_Element.getNodeName());
  }else if (s==("dump")){
    bRet=true;
    save(cout);
  }else if (s=="enumerate" && (args.entries()==0 || args.entries()==1)){
    // return an enumeration object for this element
    bRet=true;
    if (args.entries()==0)
      ret=skRValue(new skXMLElementObjectEnumerator(m_Element,m_AddIfNotPresent,getLocation()),true);
    else
      ret=skRValue(new skXMLElementObjectEnumerator(m_Element,m_AddIfNotPresent,getLocation(),args[0].str()),true);
  }else{
    skString location=skString::addStrings(m_ScriptLocation,":",s);
    if (m_Element.isNull()==false){
      skMethodDefNode * methNode=0;
      if (m_MethodCache!=0)
        methNode=m_MethodCache->value(s);
      if (methNode==0){
        // no parse tree found in the cache - try and construct a new one
        DOM_Element node=skXMLElementObject::findChild(m_Element,s_function,s_name,s);
        if (node.isNull()==false){
	        // we found some XML with the script in - now parse it
	        skString code=skXMLElementObject::getData(node);
	        bRet=true;
	        skString params=toString(node.getAttribute("params"));
          // pull out the parameters from the "params" attribute
          skStringList paramList;
          if (params.length()>0){
	          skStringTokenizer tokenizer(params,", ");
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
skString skXMLElementObject::toString(DOMString str)
  // ---------------------------------------------------------------------------
{
  // nb this now owns the buffer, but does not delete it
  return skString::fromBuffer(str.transcode());
}
// ---------------------------------------------------------------------------
DOMString skXMLElementObject::fromString(const skString& str)
  // ---------------------------------------------------------------------------
{
  return DOMString((const char *)str);
}
//------------------------------------------
skXMLElementObject::skXMLElementObject(const skXMLElementObject& other)
  //------------------------------------------
{
}
//------------------------------------------
skXMLElementObject& skXMLElementObject::operator=(const skXMLElementObject& other)
  //------------------------------------------
{
  return *this;
}
//------------------------------------------
skString skXMLElementObject::getLocation() const
  //------------------------------------------
{
  return m_ScriptLocation;
}
//------------------------------------------
void skXMLElementObject::setAddIfNotPresent(bool enable)
//------------------------------------------
{
  m_AddIfNotPresent=enable;
}
//------------------------------------------
bool skXMLElementObject::getAddIfNotPresent()
//------------------------------------------
{
  return m_AddIfNotPresent;
}
//------------------------------------------
skExecutableIterator * skXMLElementObject::createIterator(const skString& qualifier)
//------------------------------------------
{
  return new skXMLElementObjectEnumerator(m_Element,m_AddIfNotPresent,getLocation(),qualifier);
}
//------------------------------------------
skExecutableIterator * skXMLElementObject::createIterator()
//------------------------------------------
{
  return new skXMLElementObjectEnumerator(m_Element,m_AddIfNotPresent,getLocation());
}
// ---------------------------------------------------------------------------
//  Local classes
// ---------------------------------------------------------------------------

class DOMPrintFormatTarget : public XMLFormatTarget
{
public:
    DOMPrintFormatTarget(ostream& out) : m_Out(out)  {}
    ~DOMPrintFormatTarget() {} 

    // -----------------------------------------------------------------------
    //  Implementations of the format target interface
    // -----------------------------------------------------------------------
    virtual void writeChars
    (
        const   XMLByte* const      toWrite
        , const unsigned int        count
        ,       XMLFormatter* const formatter
    ) 
    {
        // Surprisingly, Solaris was the only platform on which
        // required the char* cast to print out the string correctly.
        // Without the cast, it was printing the pointer value in hex.
        // Quite annoying, considering every other platform printed
        // the string with the explicit cast to char* below.
        m_Out << (char *) toWrite;
    }

private:
    // -----------------------------------------------------------------------
    //  Unimplemented methods.
    // -----------------------------------------------------------------------
    DOMPrintFormatTarget(const DOMPrintFormatTarget& other);
    void operator=(const DOMPrintFormatTarget& rhs);
	ostream& m_Out;
};

//------------------------------------------
void skXMLElementObject::save(ostream& out) 
//------------------------------------------
{
  DOM_Element elem=getElement();
  DOMPrintFormatTarget formatTarget(out);
  DOMString encNameStr("UTF-8");
  unsigned int lent = encNameStr.length();
  XMLCh * encodingName = new XMLCh[lent + 1];
  XMLString::copyNString(encodingName, encNameStr.rawBuffer(), lent);
  encodingName[lent] = 0;
  XMLFormatter formatter(encodingName,&formatTarget,XMLFormatter::NoEscapes,XMLFormatter::UnRep_CharRef);
  save(out,elem,formatter,encodingName);
  delete [] encodingName;
}
// ---------------------------------------------------------------------------
// This streaming code is taken mainly from the DOMPrint sample in Xerces
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//  ostream << DOMString
//
//  Stream out a DOM string. Doing this requires that we first transcode
//  to char * form in the default code page for the system
// ---------------------------------------------------------------------------
ostream& operator<< (ostream& target, const DOMString& s)
{
  char *p = s.transcode();
  target << p;
  delete [] p;
  return target;
}
// ---------------------------------------------------------------------------
XMLFormatter& operator<< (XMLFormatter& strm, const DOMString& s)
// ---------------------------------------------------------------------------
{
    unsigned int lent = s.length();

	if (lent <= 0)
		return strm;

    XMLCh*  buf = new XMLCh[lent + 1];
    XMLString::copyNString(buf, s.rawBuffer(), lent);
    buf[lent] = 0;
    strm << buf;
    delete [] buf;
    return strm;
}
// ---------------------------------------------------------------------------
//  ostream << DOM_Node   
//
//  Stream out a DOM node, and, recursively, all of its children. This
//  function is the heart of writing a DOM tree out as XML source. Give it
//  a document node and it will do the whole thing.
// ---------------------------------------------------------------------------
static const XMLCh  gEndElement[] = { chOpenAngle, chForwardSlash, chNull };
static const XMLCh  gStartPI[] = { chOpenAngle, chQuestion, chNull };
static const XMLCh  gEndPI[] = { chQuestion, chCloseAngle, chNull};
static const XMLCh  gStartCDATA[] =
{ 
        chOpenAngle, chBang, chOpenSquare, chLatin_C, chLatin_D,
        chLatin_A, chLatin_T, chLatin_A, chOpenSquare, chNull
};

static const XMLCh  gEndCDATA[] =
{
    chCloseSquare, chCloseSquare, chCloseAngle, chNull
};
static const XMLCh  gStartComment[] =
{ 
    chOpenAngle, chBang, chDash, chDash, chNull
};

static const XMLCh  gEndComment[] =
{
    chDash, chDash, chCloseAngle, chNull
};

static const XMLCh  gStartDoctype[] =
{ 
    chOpenAngle, chBang, chLatin_D, chLatin_O, chLatin_C, chLatin_T,
    chLatin_Y, chLatin_P, chLatin_E, chSpace, chNull
};
static const XMLCh  gPublic[] =
{ 
    chLatin_P, chLatin_U, chLatin_B, chLatin_L, chLatin_I,
    chLatin_C, chSpace, chDoubleQuote, chNull
};
static const XMLCh  gSystem[] =
{ 
    chLatin_S, chLatin_Y, chLatin_S, chLatin_T, chLatin_E,
    chLatin_M, chSpace, chDoubleQuote, chNull
};
static const XMLCh  gStartEntity[] =
{ 
    chOpenAngle, chBang, chLatin_E, chLatin_N, chLatin_T, chLatin_I,
    chLatin_T, chLatin_Y, chSpace, chNull
};
static const XMLCh  gNotation[] =
{ 
    chLatin_N, chLatin_D, chLatin_A, chLatin_T, chLatin_A,
    chSpace, chDoubleQuote, chNull
};
static const XMLCh  gXMLDecl1[] =
{
        chOpenAngle, chQuestion, chLatin_x, chLatin_m, chLatin_l
    ,   chSpace, chLatin_v, chLatin_e, chLatin_r, chLatin_s, chLatin_i
    ,   chLatin_o, chLatin_n, chEqual, chDoubleQuote, chNull
};
static const XMLCh  gXMLDecl2[] =
{
        chDoubleQuote, chSpace, chLatin_e, chLatin_n, chLatin_c
    ,   chLatin_o, chLatin_d, chLatin_i, chLatin_n, chLatin_g, chEqual
    ,   chDoubleQuote, chNull
};
static const XMLCh  gXMLDecl3[] =
{
        chDoubleQuote, chSpace, chLatin_s, chLatin_t, chLatin_a
    ,   chLatin_n, chLatin_d, chLatin_a, chLatin_l, chLatin_o
    ,   chLatin_n, chLatin_e, chEqual, chDoubleQuote, chNull
};
static const XMLCh  gXMLDecl4[] =
{
        chDoubleQuote, chQuestion, chCloseAngle
    ,   chCR, chLF, chNull
};
// ---------------------------------------------------------------------------
void skXMLElementObject::save(ostream& target, DOM_Node& toWrite,XMLFormatter& formatter,XMLCh * encoding_name)
// ---------------------------------------------------------------------------
{
    // Get the name and value out for convenience
    DOMString   nodeName = toWrite.getNodeName();
    DOMString   nodeValue = toWrite.getNodeValue();
    unsigned long lent = nodeValue.length();

    switch (toWrite.getNodeType())
    {
        case DOM_Node::TEXT_NODE:
        {
            formatter.formatBuf(nodeValue.rawBuffer(), 
                                  lent, XMLFormatter::CharEscapes);
            break;
        }


        case DOM_Node::PROCESSING_INSTRUCTION_NODE :
        {
            formatter << XMLFormatter::NoEscapes << gStartPI  << nodeName;
            if (lent > 0)
            {
                formatter << chSpace << nodeValue;
            }
            formatter << XMLFormatter::NoEscapes << gEndPI;
            break;
        }


        case DOM_Node::DOCUMENT_NODE :
        {

            DOM_Node child = toWrite.getFirstChild();
            while( child != 0)
            {
				save(target,child,formatter,encoding_name);
                target << endl;
                child = child.getNextSibling();
            }
            break;
        }


        case DOM_Node::ELEMENT_NODE :
        {
            // The name has to be representable without any escapes
            formatter  << XMLFormatter::NoEscapes
                         << chOpenAngle << nodeName;

            // Output the element start tag.

            // Output any attributes on this element
            DOM_NamedNodeMap attributes = toWrite.getAttributes();
            int attrCount = attributes.getLength();
            for (int i = 0; i < attrCount; i++)
            {
                DOM_Node  attribute = attributes.item(i);

                //
                //  Again the name has to be completely representable. But the
                //  attribute can have refs and requires the attribute style
                //  escaping.
                //
                formatter  << XMLFormatter::NoEscapes
                             << chSpace << attribute.getNodeName()
                             << chEqual << chDoubleQuote
                             << XMLFormatter::AttrEscapes
                             << attribute.getNodeValue()
                             << XMLFormatter::NoEscapes
                             << chDoubleQuote;
            }

            //
            //  Test for the presence of children, which includes both
            //  text content and nested elements.
            //
            DOM_Node child = toWrite.getFirstChild();
            if (child != 0)
            {
                // There are children. Close start-tag, and output children.
                // No escapes are legal here
                formatter << XMLFormatter::NoEscapes << chCloseAngle;

                while( child != 0)
                {
                    save(target,child,formatter,encoding_name);
                    child = child.getNextSibling();
                }

                //
                // Done with children.  Output the end tag.
                //
                formatter << XMLFormatter::NoEscapes << gEndElement
                            << nodeName << chCloseAngle;
            }
            else
            {
                //
                //  There were no children. Output the short form close of
                //  the element start tag, making it an empty-element tag.
                //
                formatter << XMLFormatter::NoEscapes << chForwardSlash << chCloseAngle;
            }
            break;
        }
        
        
        case DOM_Node::ENTITY_REFERENCE_NODE:
            {
                DOM_Node child;
#if 0
                for (child = toWrite.getFirstChild();
                child != 0;
                child = child.getNextSibling())
                {
                    target << child;
                }
#else
                //
                // Instead of printing the refernece tree 
                // we'd output the actual text as it appeared in the xml file.
                // This would be the case when -e option was chosen
                //
                    formatter << XMLFormatter::NoEscapes << chAmpersand
                        << nodeName << chSemiColon;
#endif
                break;
            }
            
            
        case DOM_Node::CDATA_SECTION_NODE:
            {
            formatter << XMLFormatter::NoEscapes << gStartCDATA
                        << nodeValue << gEndCDATA;
            break;
        }

        
        case DOM_Node::COMMENT_NODE:
        {
            formatter << XMLFormatter::NoEscapes << gStartComment
                        << nodeValue << gEndComment;
            break;
        }

        
        case DOM_Node::DOCUMENT_TYPE_NODE:
        {
            DOM_DocumentType doctype = (DOM_DocumentType &)toWrite;;

            formatter << XMLFormatter::NoEscapes  << gStartDoctype
                        << nodeName;
 
            DOMString id = doctype.getPublicId();
            if (id != 0)
            {
                formatter << XMLFormatter::NoEscapes << chSpace << gPublic
                    << id << chDoubleQuote;
                id = doctype.getSystemId();
                if (id != 0)
                {
                    formatter << XMLFormatter::NoEscapes << chSpace 
                       << chDoubleQuote << id << chDoubleQuote;
                }
            }
            else
            {
                id = doctype.getSystemId();
                if (id != 0)
                {
                    formatter << XMLFormatter::NoEscapes << chSpace << gSystem
                        << id << chDoubleQuote;
                }
            }
            
            id = doctype.getInternalSubset(); 
            if (id !=0)
                formatter << XMLFormatter::NoEscapes << chOpenSquare
                            << id << chCloseSquare;

            formatter << XMLFormatter::NoEscapes << chCloseAngle;
            break;
        }
        
        
        case DOM_Node::ENTITY_NODE:
        {
            formatter << XMLFormatter::NoEscapes << gStartEntity
                        << nodeName;

            DOMString id = ((DOM_Entity &)toWrite).getPublicId();
            if (id != 0)
                formatter << XMLFormatter::NoEscapes << gPublic
                            << id << chDoubleQuote;

            id = ((DOM_Entity &)toWrite).getSystemId();
            if (id != 0)
                formatter << XMLFormatter::NoEscapes << gSystem
                            << id << chDoubleQuote;
            
            id = ((DOM_Entity &)toWrite).getNotationName();
            if (id != 0)
                formatter << XMLFormatter::NoEscapes << gNotation
                            << id << chDoubleQuote;

            formatter << XMLFormatter::NoEscapes << chCloseAngle << chCR << chLF;

            break;
        }
        
        
        case DOM_Node::XML_DECL_NODE:
        {
            DOMString  str;

            formatter << gXMLDecl1 << ((DOM_XMLDecl &)toWrite).getVersion();

            formatter << gXMLDecl2 << encoding_name;
            
            str = ((DOM_XMLDecl &)toWrite).getStandalone();
            if (str != 0)
                formatter << gXMLDecl3 << str;
            
            formatter << gXMLDecl4;

            break;
        }
        
        
        default:
            cerr << "Unrecognized node type = "
                 << (long)toWrite.getNodeType() << endl;
    }
}
