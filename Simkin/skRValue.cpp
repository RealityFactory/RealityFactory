/*
  Copyright 1996-2002
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

  $Id: skRValue.cpp,v 1.12 2001/11/22 11:13:21 sdw Exp $
*/
#include <ostream.h>
#include "skRValue.h"
#include "skExecutable.h"
#include <stdio.h>

//------------------------------------------
skRValue::skRValue(skiExecutable * o,bool created)
  //------------------------------------------
  : m_Type(T_Object)
{
  m_Value.m_ObjectRef=new skObjectRef;
  m_Value.m_ObjectRef->m_Object=o;
  m_Value.m_ObjectRef->m_Created=created;
  m_Value.m_ObjectRef->m_RefCount=1;
}
//------------------------------------------
skRValue::~skRValue()
//------------------------------------------
{
  if (m_Type==T_Object){
    m_Value.m_ObjectRef->m_RefCount--;
    if (m_Value.m_ObjectRef->m_RefCount==0){
      if (m_Value.m_ObjectRef->m_Created)
	delete m_Value.m_ObjectRef->m_Object;
      delete m_Value.m_ObjectRef;	
    }	
  }	
}
//------------------------------------------
skRValue::skRValue(const skRValue& v)
  //------------------------------------------
  : m_Type(v.m_Type),m_String(v.m_String)
{
  switch (m_Type){
  case T_Object:
    m_Value.m_ObjectRef=v.m_Value.m_ObjectRef;
    m_Value.m_ObjectRef->m_RefCount++;
    break;
  case T_Int:
    m_Value.m_Int=v.m_Value.m_Int;
    break;
  case T_Char:
    m_Value.m_Char=v.m_Value.m_Char;
    break;
  case T_Bool:
    m_Value.m_Bool=v.m_Value.m_Bool;
    break;
  case T_Float:
    m_Value.m_Float=v.m_Value.m_Float;
    break;
  }
}
//------------------------------------------
skRValue& skRValue::operator=(const skRValue& v)
  //------------------------------------------
{
  if (&v!=this){
    if (m_Type==T_Object){
      m_Value.m_ObjectRef->m_RefCount--;
      if (m_Value.m_ObjectRef->m_RefCount==0){
	if (m_Value.m_ObjectRef->m_Created)
	  delete m_Value.m_ObjectRef->m_Object;
	delete m_Value.m_ObjectRef;	
	m_Value.m_ObjectRef=0;
      }	
    }
    m_Type=v.m_Type;
    switch (m_Type){
    case T_String:
      m_String=v.m_String;
      break;
    case T_Bool:
      m_Value.m_Bool=v.m_Value.m_Bool;
      break;
    case T_Char:
      m_Value.m_Char=v.m_Value.m_Char;
      break;
    case T_Int:
      m_Value.m_Int=v.m_Value.m_Int;
      break;
    case T_Float:
      m_Value.m_Float=v.m_Value.m_Float;
      break;
    case T_Object:
      m_Value.m_ObjectRef=v.m_Value.m_ObjectRef;
      m_Value.m_ObjectRef->m_RefCount++;
      break;
    }
  }
  return *this;
}
//------------------------------------------
 Char skRValue::charValue() const
  //------------------------------------------
{
  Char r=0;
  switch(m_Type){
  case T_String:
    r=m_String.at(0);
    break;
  case T_Int:
    r=(Char)(m_Value.m_Int);
    break;
  case T_Bool:
    if (m_Value.m_Bool)
      r='t';
    else
      r='f';
    break;
  case T_Char:
    r=m_Value.m_Char;
    break;
  case T_Float:
    r=(Char)m_Value.m_Float;
    break;
  case T_Object:
    r=m_Value.m_ObjectRef->m_Object->charValue();
    break;
  }
  return r;	
}
//------------------------------------------
 int skRValue::intValue() const
  //------------------------------------------
{
  int r=0;
  switch(m_Type){
  case T_String:
    r=m_String.to();
    break;
  case T_Int:
    r=m_Value.m_Int;
    break;
  case T_Bool:
    r=(int)(m_Value.m_Bool);
    break;
  case T_Char:
    r=(int)(m_Value.m_Int);
    break;
  case T_Float:
    r=(int)m_Value.m_Float;
    break;
  case T_Object:
    r=m_Value.m_ObjectRef->m_Object->intValue();
    break;
  }
  return r;	
}
//------------------------------------------
 float skRValue::floatValue() const
  //------------------------------------------
{
  float r=0;
  switch(m_Type){
  case T_Bool:
    r=(float)(m_Value.m_Bool);
    break;
  case T_Char:
    r=(float)(m_Value.m_Int);
    break;
  case T_String:
    r=m_String.toFloat();
    break;
  case T_Int:
    r=(float)m_Value.m_Int;
    break;
  case T_Float:
    r=m_Value.m_Float;
    break;
  case T_Object:
    r=m_Value.m_ObjectRef->m_Object->floatValue();
    break;
  }
  return r;	
}
//------------------------------------------
 bool skRValue::boolValue() const
  //------------------------------------------
{
  bool bRet=false;
  switch(m_Type){
  case T_Bool:
    bRet=m_Value.m_Bool;
    break;
  case T_Char:
    if (m_Value.m_Char=='t')
      bRet=true;
    break;
  case T_String:
    if (m_String==skSTR("1") || m_String==s_true)
      bRet=true;
    break;
  case T_Int:
    if (m_Value.m_Int)
      bRet=true;
    break;
  case T_Float:
    if (m_Value.m_Float)
      bRet=true;
    break;
  case T_Object:
    bRet=m_Value.m_ObjectRef->m_Object->boolValue();
    break;
  }
  return bRet;	
}
//------------------------------------------
 skString skRValue::str() const
  //------------------------------------------
{
  skString s;
  switch(m_Type){
  case T_String:
    s=m_String;
    break;
  case T_Int:
    s=skString::from(m_Value.m_Int);
    break;
  case T_Char:
    s=skString((Char)(m_Value.m_Char),1);
    break;
  case T_Bool:
    if (m_Value.m_Bool)
      s=s_true;
    else
      s=s_false;
    break;
  case T_Float:
    s=skString::from(m_Value.m_Float);
    break;
  case T_Object:
    s=m_Value.m_ObjectRef->m_Object->strValue();
    break;
  }
  return s;	
}
//------------------------------------------
bool skRValue::operator==(const skRValue& v)
  //------------------------------------------   
{
  bool r=false;
  switch(m_Type){
  case T_String:
    r=(m_String==v.str());
    break;
  case T_Int:
    r=(m_Value.m_Int==v.intValue());
    break;
  case T_Char:
    r=(m_Value.m_Char==v.charValue());
    break;
  case T_Bool:
    r=(m_Value.m_Bool==v.boolValue());
    break;
  case T_Float:
    r=(m_Value.m_Float==v.floatValue());
    break;
  case T_Object:
    switch (v.m_Type){
    case T_Char:
      r=(m_Value.m_ObjectRef->m_Object->charValue()==v.m_Value.m_Char);
      break;
    case T_Bool:
      r=(m_Value.m_ObjectRef->m_Object->boolValue()==v.m_Value.m_Bool);
      break;
    case T_Object:
      r=(m_Value.m_ObjectRef->m_Object->equals(v.m_Value.m_ObjectRef->m_Object));
      break;
    case T_Int:
      r=(m_Value.m_ObjectRef->m_Object->intValue()==v.m_Value.m_Int);
      break;
    case T_Float:
      r=(m_Value.m_ObjectRef->m_Object->floatValue()==v.m_Value.m_Float);
      break;
    case T_String:
      r=(m_Value.m_ObjectRef->m_Object->strValue()==v.m_String);
      break;
    }
    break;
  }
  return r;
}

