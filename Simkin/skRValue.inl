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

* $Id: skRValue.inl,v 1.5 2001/11/22 11:13:21 sdw Exp $
*/
#include "skiExecutable.h"
#include <stdio.h>

xskLITERAL(true);
xskLITERAL(false);

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
inline skRValue::skRValue(skString s)
  //------------------------------------------
  : m_String(s),m_Type(T_String)
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
//------------------------------------------
inline skRValue::skRValue(float f)
  //------------------------------------------
  : m_Type(T_Float)
{ 
  m_Value.m_Float=f;
}
//------------------------------------------
inline skRValue::skRValue()
  //------------------------------------------
  : m_Type(T_String)
{
  m_Value.m_ObjectRef=0;
}
//------------------------------------------
inline skRValue::skRValue(bool b)
  //------------------------------------------
  : m_Type(T_Bool)
{
  m_Value.m_Bool=b;
}
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












