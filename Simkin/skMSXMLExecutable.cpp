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

* $Id: skMSXMLExecutable.cpp,v 1.1 2003/01/23 14:22:09 simkin_cvs Exp $
*/

#include <fstream.h>
#include "skMSXMLExecutable.h"
#include "skXMLParseException.h"

//------------------------------------------
skMSXMLExecutable::skMSXMLExecutable() 
//------------------------------------------
{
}
//------------------------------------------
skMSXMLExecutable::skMSXMLExecutable(const skString& scriptLocation,const skString& in,skExecutableContext& context)
//------------------------------------------
{
  load(scriptLocation,in,context);
}
//------------------------------------------
skMSXMLExecutable::skMSXMLExecutable(const skString& fileName,skExecutableContext& context)
//------------------------------------------
{
  load(fileName,context);
}
//------------------------------------------
void skMSXMLExecutable::load(const skString& scriptLocation,const skString& in,skExecutableContext& context) 
//------------------------------------------
{
	XMLElement elm = getElement();
	XMLDoc DOMdoc;
	if (elm != NULL)
		DOMdoc = elm->ownerDocument;
  else{
		HRESULT hr = DOMdoc.CreateInstance(skSTR("MSXML.DOMDocument"));
		if (hr != ERROR_SUCCESS)
			throw skXMLParseException(skSTR("Create MSXML Instance Failed"),0);
	}
	BOOL bloaded = DOMdoc->loadXML( _bstr_t(fromString(in)) );
  if (bloaded){
		setElement(DOMdoc->documentElement);
		m_ScriptLocation = scriptLocation;
	}
	else{
#ifdef EXCEPTIONS_DEFINED
		throw skXMLParseException(skSTR("Loading xml string failed"), 0);
#else
    context.getError().setError(skScriptError::XMLPARSE_ERROR,skXMLParseException(skSTR("Loading xml string failed"), 0);
#endif
  }
}
//------------------------------------------
void skMSXMLExecutable::load(const skString& fileName,skExecutableContext& context) 
//------------------------------------------
{
	XMLElement elm = getElement();
	XMLDoc DOMdoc;
	if (elm != NULL)
		DOMdoc = elm->ownerDocument;
  else{
		HRESULT hr = DOMdoc.CreateInstance(skSTR("MSXML.DOMDocument"));
		if (hr != ERROR_SUCCESS)
			throw skXMLParseException(skSTR("Create MSXML Instance Failed"),0);
	}
	BOOL bloaded = DOMdoc->load( _bstr_t(fromString(fileName)) );
  if (bloaded){
		setElement(DOMdoc->documentElement);
		m_ScriptLocation = fileName;
	}
  else{
#ifdef EXCEPTIONS_DEFINED
		throw skXMLParseException(skSTR("Loading xml file failed"), 0);
#else
    context.getError().setError(skScriptError::XMLPARSE_ERROR,skXMLParseException(skSTR("Loading xml string failed"), 0);
#endif
  }
}
//------------------------------------------
void skMSXMLExecutable::save(const skString& fileName) 
//------------------------------------------
{
	XMLElement elm = getElement();
	elm->ownerDocument->save( _bstr_t(fromString(fileName)) );
}
//------------------------------------------
skMSXMLExecutable::skMSXMLExecutable(const skMSXMLExecutable& other)
//------------------------------------------
{
}
//------------------------------------------
skMSXMLExecutable& skMSXMLExecutable::operator=(const skMSXMLExecutable& other)
//------------------------------------------
{
  return *this;
}
