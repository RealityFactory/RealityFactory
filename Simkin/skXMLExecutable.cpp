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

* $Id: skXMLExecutable.cpp,v 1.12 2003/01/20 18:48:18 simkin_cvs Exp $
*/

#include "skXMLErrorHandler.h"
#include <fstream.h>
#include <xercesc/dom/deprecated/DOMParser.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include "skXMLExecutable.h"

//------------------------------------------
skXMLExecutable::skXMLExecutable() 
//------------------------------------------
{
}
//------------------------------------------
skXMLExecutable::skXMLExecutable(const skString& scriptLocation,InputSource& in)
//------------------------------------------
{
  load(scriptLocation,in);
}
//------------------------------------------
skXMLExecutable::skXMLExecutable(const skString& scriptLocation,const skString& in)
//------------------------------------------
{
  load(scriptLocation,in);
}
//------------------------------------------
skXMLExecutable::skXMLExecutable(const skString& fileName)
//------------------------------------------
{
  load(fileName);
}
//------------------------------------------
void skXMLExecutable::load(const skString& scriptLocation,const skString& in) 
//------------------------------------------
{
  DOMString domStr=fromString(in);
  DOMString domLocation=fromString(scriptLocation);
  MemBufInputSource inputSource((const XMLByte *)domStr.transcode(),domStr.length(),domLocation.rawBuffer(),true);
  load(scriptLocation,inputSource);
}
//------------------------------------------
void skXMLExecutable::load(const skString& scriptLocation,InputSource& in) 
//------------------------------------------
{
  m_ScriptLocation=scriptLocation;
  DOMParser * parser = new DOMParser;
  skXMLErrorHandler handler;
  parser->setErrorHandler(&handler);
  parser->parse(in);
  DOM_Element elem=parser->getDocument().getDocumentElement();
  delete parser;
  setElement(elem);
}
//------------------------------------------
void skXMLExecutable::load(const skString& fileName) 
//------------------------------------------
{
  DOMString sFileName((const char *)fileName);
  const XMLCh * buffer=sFileName.rawBuffer();
  LocalFileInputSource in(buffer);
  load(fileName,in);
}

//------------------------------------------
void skXMLExecutable::save(const skString& fileName) 
//------------------------------------------
{
  ofstream out(fileName);
  skXMLElementObject::save(out);
}
//------------------------------------------
skXMLExecutable::skXMLExecutable(const skXMLExecutable& other)
//------------------------------------------
{
}
//------------------------------------------
skXMLExecutable& skXMLExecutable::operator=(const skXMLExecutable& other)
//------------------------------------------
{
  return *this;
}
