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

  $Id: skAsciiString.inl,v 1.2 2003/04/16 13:23:06 simkin_cvs Exp $
*/

#ifndef skASCIISTRING_INL
#define skASCIISTRING_INL

#include "skAsciiString.h"
#include <string.h>
#include <ctype.h>

class   P_AsciiString 
{
public:
  char * m_PString;
  USize m_Length:31;
  struct {
    unsigned m_Hash:16;
    /** nb limits number of reference counts to 32k */
    unsigned m_RefCount:15;
    unsigned m_Const:1;
  } s;
    
  P_AsciiString();
  ~P_AsciiString();
  void			init();

};    

//---------------------------------------------------
inline P_AsciiString::~P_AsciiString() 
  //---------------------------------------------------
{ 
  if (s.m_Const==0) 
    delete[] m_PString; 
}
#ifndef __SYMBIAN32__
//---------------------------------------------------
inline skAsciiString::operator const char * () const 
  //---------------------------------------------------
{
  return c_str();
}
#endif
//---------------------------------------------------
inline USize skAsciiString::length() const 
  //---------------------------------------------------
{
  USize length=0;
  if (pimp)
    length=pimp->m_Length;
  return length;
}
//---------------------------------------------------
inline  bool skAsciiString::operator<(const skAsciiString& s) const
  //---------------------------------------------------
{
  bool ret=false;
  if (pimp==s.pimp)
    ret=false;
  else{
    int i=strcmp(c_str(),s.c_str());
    if (i<0)
      ret=true;
  }
  return ret;
}
//---------------------------------------------------
inline  bool skAsciiString::operator==(const skAsciiString& s) const
  //---------------------------------------------------
{
  bool equals=false;
  if (pimp==s.pimp)
    equals=true;
  else if (pimp && s.pimp){
    if (pimp->s.m_Hash==s.pimp->s.m_Hash && pimp->m_Length==s.pimp->m_Length)
      equals=(strcmp(pimp->m_PString,s.pimp->m_PString)==0);
  }else{
    equals=(strcmp(c_str(),s.c_str())==0);
  }
  return equals;
}
//---------------------------------------------------
inline  bool skAsciiString::operator==(const char * s) const
  //---------------------------------------------------
{
  if (s)
    return (strcmp(c_str(),s)==0);
  else
    return false;
}
//---------------------------------------------------
inline void skAsciiString::deRef()
  //---------------------------------------------------
{
  if (pimp){
    pimp->s.m_RefCount--;
    if (pimp->s.m_RefCount==0)
      delete pimp;
  }
}

//---------------------------------------------------
inline  skAsciiString::skAsciiString()
  //---------------------------------------------------
  : pimp(0)
{
}
//---------------------------------------------------
inline skAsciiString& skAsciiString::operator=(const skAsciiString& s) 
  //---------------------------------------------------
{
  if (&s!=this){
    if (s.pimp)
      s.pimp->s.m_RefCount++;
    deRef();
    pimp=s.pimp;
  }	
  return *this;
}
//---------------------------------------------------
inline skAsciiString::skAsciiString(const skAsciiString& s) 
  //---------------------------------------------------
  : pimp(s.pimp)
{
  if (pimp)
    pimp->s.m_RefCount++;
}
//---------------------------------------------------
inline skAsciiString::~skAsciiString() 
  //---------------------------------------------------
{
  deRef();
}
//---------------------------------------------------
inline USize skAsciiString::hash() const 
  //---------------------------------------------------
{
  USize hash=0;
  if (pimp)
    hash=pimp->s.m_Hash;
  return hash;
}
//---------------------------------------------------
inline char skAsciiString::at(USize index) const
  //---------------------------------------------------
{
  char c=0;
  if (index<length() && pimp)
    c=pimp->m_PString[index];
  return c;	
}
//---------------------------------------------------
inline const char * skAsciiString::c_str() const 
  //---------------------------------------------------
{
  const char * c="";
  if (pimp)
    c=(const char *)(pimp->m_PString);
  return c;
}

#endif





