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

* $Id: skExpatParser.cpp,v 1.18 2003/05/15 19:20:06 simkin_cvs Exp $
*/
#include "skExpatParser.h"
#include "skXMLParseException.h"
#include "skCDataNode.h"
#include "skInputSource.h"
#include <stdio.h>
//-----------------------------------------------------------------
EXPORT_C skExpatParser::skExpatParser()
//-----------------------------------------------------------------
  : m_InCData(false),m_RootElement(0)
{
}
//-----------------------------------------------------------------
EXPORT_C skExpatParser::~skExpatParser()
//-----------------------------------------------------------------
{
  delete m_RootElement;
}
//-----------------------------------------------------------------
skExpatParser::skExpatParser(const skExpatParser&)
//-----------------------------------------------------------------
{
}
//-----------------------------------------------------------------
skExpatParser& skExpatParser::operator=(const skExpatParser&)
//-----------------------------------------------------------------
{
  return *this;
}
//-----------------------------------------------------------------
void skExpatParser::startElementHandler(void *userData, const Char *name, const Char **atts)
//-----------------------------------------------------------------
{
  ((skExpatParser *)userData)->startElement(name,atts);
}
//-----------------------------------------------------------------
void skExpatParser::startElement(const Char *name, const Char **attr)
//-----------------------------------------------------------------
{
  skString s_name;
  SAVE_VARIABLE(s_name);
  s_name=name;
  skElement * elem=skNEW(skElement(s_name));
  if (m_RootElement==0)
    m_RootElement=elem;
  for (int i = 0; attr[i]; i += 2) {
    skString attr_name;
    SAVE_VARIABLE(attr_name);
    attr_name=attr[i];
    skString attr_value;
    SAVE_VARIABLE(attr_value);
    attr_value=attr[i+1];
    elem->setAttribute(attr_name,attr_value);
    RELEASE_VARIABLE(attr_value);
    RELEASE_VARIABLE(attr_name);
  }
  if (m_ElementStack.entries())
    ((skElement *)m_ElementStack[m_ElementStack.entries()-1])->appendChild(elem);
  m_ElementStack.append(elem);
  RELEASE_VARIABLE(s_name);
}
//-----------------------------------------------------------------
void skExpatParser::endElementHandler(void *userData, const Char *name)
//-----------------------------------------------------------------
{
  ((skExpatParser *)userData)->endElement(name);
}
//-----------------------------------------------------------------
void skExpatParser::endElement(const Char *name)
//-----------------------------------------------------------------
{
  // pop latest off the stack
  if (m_ElementStack.entries())
    m_ElementStack.remove(m_ElementStack[m_ElementStack.entries()-1]);
}
//-----------------------------------------------------------------
void skExpatParser::characterDataHandler(void *userData, const XML_Char *s,int len)
//-----------------------------------------------------------------
{
  ((skExpatParser *)userData)->characterData(s,len);
}
//-----------------------------------------------------------------
void skExpatParser::characterData(const XML_Char *s,int len)
//-----------------------------------------------------------------
{
  if (m_ElementStack.entries()){
    skElement * element=(skElement * )m_ElementStack[m_ElementStack.entries()-1];
    skString text;
    SAVE_VARIABLE(text);
    text=skString::copyFromBuffer(s,(USize)len);
    if (m_InCData)
      element->appendChild(skNEW(skCDataNode(text)));
    else
      element->appendChild(skNEW(skTextNode(text)));
    RELEASE_VARIABLE(text);
  }
}
//-----------------------------------------------------------------
void skExpatParser::startCDataHandler(void *userData)
//-----------------------------------------------------------------
{
  ((skExpatParser *)userData)->startCData();
}
//-----------------------------------------------------------------
void skExpatParser::startCData()
//-----------------------------------------------------------------
{
  m_InCData=true;
}
//-----------------------------------------------------------------
void skExpatParser::endCDataHandler(void *userData)
//-----------------------------------------------------------------
{
  ((skExpatParser *)userData)->endCData();
}
//-----------------------------------------------------------------
void skExpatParser::endCData ()
//-----------------------------------------------------------------
{
  m_InCData=false;
}
//-----------------------------------------------------------------
EXPORT_C skElement * skExpatParser::parse(skInputSource& in,skExecutableContext& context)
//-----------------------------------------------------------------
{
  return parse(in,context,skString());
}
//-----------------------------------------------------------------
EXPORT_C skElement * skExpatParser::parse(skInputSource& in,skExecutableContext& context,skString encoding)
//-----------------------------------------------------------------
{
  m_InCData=false;
  m_RootElement=0;
  skElement * elem=0;
  XML_Parser parser = 0;
  if (encoding.length()==0)
    parser=XML_ParserCreate(NULL);
  else
    parser=XML_ParserCreate(encoding);
  XML_SetUserData(parser,this);
  XML_SetElementHandler(parser, startElementHandler, endElementHandler);
  XML_SetCharacterDataHandler(parser, characterDataHandler);
  XML_SetCdataSectionHandler(parser, startCDataHandler, endCDataHandler);
  skString in_string=in.readAllToString();
  unsigned int buf_len=in_string.length()*sizeof(Char);
  if (!XML_Parse(parser, (const char *)in_string.c_str(), buf_len, true)) {
#ifdef EXCEPTIONS_DEFINED
    delete m_RootElement;
    m_ElementStack.clear();
    m_RootElement=0;
    skXMLParseException e(XML_ErrorString(XML_GetErrorCode(parser)),XML_GetCurrentLineNumber(parser));
    XML_ParserFree(parser);
    throw e;
#else
    delete m_RootElement;
    m_ElementStack.clear();
    m_RootElement=0;
    skString error;
    SAVE_VARIABLE(error);
    error=XML_ErrorString(XML_GetErrorCode(parser));
    skXMLParseException * e=skNEW(skXMLParseException(error,XML_GetCurrentLineNumber(parser)));
    context.getError().setError(skScriptError::XMLPARSE_ERROR,e);
    RELEASE_VARIABLE(error);
#endif
  }
  XML_ParserFree(parser);
  elem=m_RootElement;
  m_RootElement=0;
  m_ElementStack.clear();
  return elem;
}
