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

  $Id: skExecutable.cpp,v 1.42 2003/11/20 17:20:24 sdw Exp $
*/
#include "skExecutable.h"
#include "skRValue.h"
#include "skRValueArray.h"
#include "skTracer.h"
#include "skInterpreter.h"


//------------------------------------------
EXPORT_C skExecutable::skExecutable()
//------------------------------------------
{
}
//------------------------------------------
skExecutable::skExecutable(const skExecutable& other)
//------------------------------------------
{
}
//------------------------------------------
skExecutable& skExecutable::operator=(const skExecutable& other)
//------------------------------------------
{
  return *this;
}
//------------------------------------------
EXPORT_C skExecutable::~skExecutable()
//------------------------------------------
{
}
//------------------------------------------
EXPORT_C int skExecutable::executableType() const
//------------------------------------------
{
  return UNDEFINED_TYPE;
}
//------------------------------------------
EXPORT_C int skExecutable::intValue() const
//------------------------------------------
{
  return 0;
}
#ifdef USE_FLOATING_POINT
//------------------------------------------
EXPORT_C float skExecutable::floatValue() const
//------------------------------------------
{
  return 0.0f;
}
#endif
//------------------------------------------
EXPORT_C bool skExecutable::boolValue() const
//------------------------------------------
{
  return false;
}
//------------------------------------------
EXPORT_C Char skExecutable::charValue() const
//------------------------------------------
{
  return ' ';
}
//------------------------------------------
EXPORT_C skString skExecutable::strValue() const
//------------------------------------------
{
  return skString();
}
//------------------------------------------
EXPORT_C bool skExecutable::setValue(const skString& s,const skString& attribute,const skRValue& r)
//------------------------------------------
{
  return false;
}
//------------------------------------------
bool skExecutable::setValueAt(const skRValue& array_index,const skString& attribute,const skRValue& r)
//------------------------------------------
{
  return false;
}
//------------------------------------------
EXPORT_C bool skExecutable::getValue(const skString& s,const skString& attribute,skRValue& r)
//------------------------------------------
{
  return false;
}
//------------------------------------------
EXPORT_C bool skExecutable::getValueAt(const skRValue& array_index,const skString& attribute,skRValue& r)
//------------------------------------------
{
  return false;
}
//------------------------------------------
EXPORT_C bool skExecutable::method(const skString& s,skRValueArray& args,skRValue& r,skExecutableContext& ctxt)
//------------------------------------------
{
  bool bRet=false;
  // coercion methods
  if (s==s_toInt){
    if (args.entries()==1){
      bRet=true;
      r=args[0].intValue();
    }
#ifdef USE_FLOATING_POINT
  }else if (s==s_toFloat){
    if (args.entries()==1){
      bRet=true;
      r=args[0].floatValue();
    }
#endif
  }else if (s==s_toString){
    if (args.entries()==1){
      bRet=true;
      r=args[0].str();
    }
  }else if (s==s_toChar){
    if (args.entries()==1){
      bRet=true;
      r=args[0].charValue();
    }
  }else if (s==s_toBool){
    if (args.entries()==1){
      bRet=true;
      r=args[0].boolValue();
    }
  }else if (s==s_trace){
    if (args.entries()>0)
      ctxt.getInterpreter()->trace(skString::addStrings(args[0].str().ptr(),s_cr));
    bRet=true;
  }else if (s==s_isObject){
    if (args.entries()==1){
      bRet=true;
      r=bool(args[0].type()==skRValue::T_Object);
    }
  }else if (s==s_length){
    if (args.entries()==1){
      bRet=true;
      r=skRValue(args[0].str().length());
    }
  }else if (s==s_charAt){
    if (args.entries()==2){
      bRet=true;
      r=skRValue((Char)(args[0].str().at((Char)args[1].intValue())));
    }
  }
  return bRet;
}
//------------------------------------------
EXPORT_C bool skExecutable::equals(const skiExecutable * o) const
//------------------------------------------
{
  return (o==this);
}
//------------------------------------------
EXPORT_C skExecutableIterator * skExecutable::createIterator(const skString& qualifier)
//------------------------------------------
{
  return 0;
}
//------------------------------------------
EXPORT_C skExecutableIterator * skExecutable::createIterator()
//------------------------------------------
{
  return 0;
}
//------------------------------------------
EXPORT_C skString skExecutable::getSource(const skString& location)
//------------------------------------------
{
  return skString();
}
//------------------------------------------
EXPORT_C void skExecutable::getInstanceVariables(skRValueTable& table)
//------------------------------------------
{
}
//------------------------------------------
EXPORT_C void skExecutable::getAttributes(skRValueTable& table)
//------------------------------------------
{
}
