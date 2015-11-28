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

* $Id: skRValue.inl,v 1.13 2003/04/14 15:24:57 simkin_cvs Exp $
*/
#include "skiExecutable.h"
#include "skConstants.h"

/**
* This class is used to hold a pointer to an object, along with a reference cound
*/
class skObjectRef 
{
public:
  /** The object referred to */
  skiExecutable * m_Object;
  /** a flag indicating if the object should be deleted when the reference count reaches zero */
  bool m_Created;
  /** the number of pointers to this reference */
  unsigned short m_RefCount;
}; 					
#undef inline
//------------------------------------------
inline skRValue::skRValue(const skString& s)
  //------------------------------------------
  : m_Type(T_String),m_String(s)
{
  m_Value.m_ObjectRef=0;
}
//------------------------------------------
inline skRValue::skRValue(Char c)
  //------------------------------------------
  : m_Type(T_Char)
{ 
  m_Value.m_Char=c;
}
//------------------------------------------
inline skRValue::skRValue(int n)
  //------------------------------------------
  : m_Type(T_Int)
{ 
  m_Value.m_Int=n;
}
//------------------------------------------
inline skRValue::skRValue(unsigned int n)
  //------------------------------------------
  : m_Type(T_Int)
{ 
  m_Value.m_Int=n;
}
#ifdef USE_FLOATING_POINT
//------------------------------------------
inline skRValue::skRValue(float f)
  //------------------------------------------
  : m_Type(T_Float)
{ 
  m_Value.m_Float=f;
}
#endif
//------------------------------------------
inline skRValue::skRValue()
  //------------------------------------------
  : m_Type(T_String)
{
  m_Value.m_ObjectRef=0;
}
#ifndef __SYMBIAN32__
//------------------------------------------
inline skRValue::skRValue(bool b)
  //------------------------------------------
  : m_Type(T_Bool)
{
  m_Value.m_Bool=b;
}
#endif
//------------------------------------------
inline skRValue::RType skRValue::type() const
  //------------------------------------------
{
  return (skRValue::RType)m_Type;
}
//------------------------------------------
inline skiExecutable *  skRValue::obj() const
  //------------------------------------------
{
  skiExecutable * obj=0;
  if (m_Type==T_Object)
    obj=m_Value.m_ObjectRef->m_Object;
  return obj;
}
#ifndef __SYMBIAN32__
// this function is not available in Symbian, as a leaving memory allocation
// is made within the constructor
//------------------------------------------
inline skRValue::skRValue(skiExecutable * o,bool created)
  //------------------------------------------
  : m_Type(T_Object)
{
  m_Value.m_ObjectRef=skNEW(skObjectRef);
  m_Value.m_ObjectRef->m_Object=o;
  m_Value.m_ObjectRef->m_Created=created;
  m_Value.m_ObjectRef->m_RefCount=1;
}
#endif
//------------------------------------------
inline void skRValue::assignObject(skiExecutable * o,bool created)
  //------------------------------------------
{
  deRef();
  m_Type=T_Object;
  m_Value.m_ObjectRef=skNEW(skObjectRef);
  m_Value.m_ObjectRef->m_Object=o;
  m_Value.m_ObjectRef->m_Created=created;
  m_Value.m_ObjectRef->m_RefCount=1;
}
//------------------------------------------
inline skRValue::~skRValue()
  //------------------------------------------
{
  deRef();
}
//------------------------------------------
inline void skRValue::deRef()
  //------------------------------------------
{
  if (m_Type==T_Object){
    m_Value.m_ObjectRef->m_RefCount--;
    if (m_Value.m_ObjectRef->m_RefCount==0){
      if (m_Value.m_ObjectRef->m_Created)
	delete m_Value.m_ObjectRef->m_Object;
      delete m_Value.m_ObjectRef;	
      m_Value.m_ObjectRef=0;
    }	
  }	
}
#ifdef __SYMBIAN32__
//-------------------------------------------------------------------------
inline skRValue::operator TCleanupItem()
  //-------------------------------------------------------------------------
  // used for leave support of local variable RValues
{
  return TCleanupItem(Cleanup,this);
}
//-------------------------------------------------------------------------
inline void skRValue::Cleanup(TAny * s)
  //-------------------------------------------------------------------------
  // called in case there is a leave, it makes sure that any associated
  // object is deleted, if the reference count reaches zero
{
  ((skRValue *)s)->deRef();
}
#endif












