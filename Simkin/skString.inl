/*
  Copyright 1996-2002
  Simon Whiteside

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITaY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  $Id: skString.inl,v 1.27 2003/04/14 15:24:57 simkin_cvs Exp $
*/

#ifndef _STR_INL
#define _STR_INL

#include "skString.h"
#ifndef __SYMBIAN32__
#include <string.h>
#include <ctype.h>
#endif

class   P_String 
{
public:
  Char * m_PString;
  USize m_Length:31;
  struct {
  unsigned m_Hash:16;
    /** nb limits number of reference counts to 32k */
  unsigned m_RefCount:15;
  unsigned m_Const:1;
  } s;
  
  inline P_String();
  inline ~P_String();
  void init();

};    

#ifdef UNICODE_STRINGS
#define STRCMPI wcsicmp
#define STRCMP wcscmp
#define STRNCMP wcsncmp
#define STRCAT wcscat
#define STRLEN wcslen
#define STRCPY wcscpy
#define STRNCPY wcsncpy
#define STRCHR wcschr
#define STRRCHR wcsrchr
#define STRSTR wcsstr
#define ISSPACE iswspace
#define ISALNUM iswalnum
#define ISALPHA iswalpha
#define ISDIGIT iswdigit
#else
#if defined __GNUC__
#define STRCMPI strcasecmp
#else
#define STRCMPI strcmpi
#endif
#define STRCPY strcpy
#define STRCAT strcat
#define STRNCMP strncmp
#define STRCMP strcmp
#define STRLEN strlen
#define STRCHR strchr
#define STRRCHR strrchr
#define STRSTR strstr
#define STRNCPY strncpy
#define ISSPACE isspace
#define ISALNUM isalnum
#define ISALPHA isalpha
#define ISDIGIT isdigit
#endif


//---------------------------------------------------
inline  skString::skString()
  //---------------------------------------------------
  :pimp(0)
{
}
//---------------------------------------------------
inline P_String::P_String()
  //---------------------------------------------------
  : m_PString(skSTR(""))
{
  s.m_Hash=0;
  s.m_RefCount=1;
  s.m_Const=true;
  m_Length=0;
}
//---------------------------------------------------
inline skString::skString(const skString& s) 
  //---------------------------------------------------
  : pimp(s.pimp)
{
  if (pimp)
    pimp->s.m_RefCount++;
}
//---------------------------------------------------
inline skString::~skString() 
  //---------------------------------------------------
{
  deRef();
}
//---------------------------------------------------
inline P_String::~P_String() 
  //---------------------------------------------------
{ 
  if (s.m_Const==0) 
    delete [] m_PString; 
}
#ifndef __SYMBIAN32__
// This can leave, so not available in Symbian
//---------------------------------------------------
inline skString::skString(const Char * s, USize len)
  //---------------------------------------------------
{   
  assign(s,len);
}
// This can leave, so not available in Symbian
//---------------------------------------------------
inline skString::skString(const Char * s) 
  //---------------------------------------------------
{   
  assign(s);
}
//---------------------------------------------------
inline skString::operator const Char * () const 
  //---------------------------------------------------
{ 
   return c_str();
}
#endif
//---------------------------------------------------
inline USize skString::length() const 
  //---------------------------------------------------
{
  USize length=0;
  if (pimp)
    length=pimp->m_Length;
  return length;
}
//---------------------------------------------------
inline  bool skString::operator<(const skString& s) const
  //---------------------------------------------------
{
  bool ret=false;
  if (pimp==s.pimp)
    ret=false;
  else{
    int i=STRCMP(c_str(),s.c_str());
    if (i<0)
      ret=true;
  }
  return ret;
}
//---------------------------------------------------
inline  bool skString::operator==(const skString& s) const
  //---------------------------------------------------
{
  bool equals=false;
  if (pimp==s.pimp)
    equals=true;
  else if (pimp && s.pimp){
    if (pimp->s.m_Hash==s.pimp->s.m_Hash && pimp->m_Length==s.pimp->m_Length)
      equals=(STRCMP(pimp->m_PString,s.pimp->m_PString)==0);
  }else{
    equals=(STRCMP(c_str(),s.c_str())==0);
  }
  return equals;
}
#ifdef __SYMBIAN32__
//---------------------------------------------------
inline bool skString::operator==(const TDesC& s) const
//---------------------------------------------------
{
  bool equals=false;
  if (pimp){
    TPtrC t((TUint16 *)pimp->m_PString,pimp->m_Length);
    equals=(t.Compare(s)==0);
  }else
    equals=(s.Length()==0);
  return equals;
}
#endif
//---------------------------------------------------
inline  bool skString::operator==(const Char * s) const
//---------------------------------------------------
{
  if (s)
    return (STRCMP(c_str(),s)==0);
  else
    return false;
}
//---------------------------------------------------
inline void skString::deRef()
  //---------------------------------------------------
{
  if (pimp){
    pimp->s.m_RefCount--;
    if (pimp->s.m_RefCount==0)
      delete pimp;
    pimp=0;
  }
}

//---------------------------------------------------
inline skString& skString::operator=(const skString& s) 
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
#ifdef __SYMBIAN32__
// Symbian-friendly function
//---------------------------------------------------
inline skString& skString::operator=(const TDesC& s) 
  //---------------------------------------------------
{
  deRef();
  assign((Char *)s.Ptr(),s.Length());
  return *this;
}
#endif
//---------------------------------------------------
inline USize skString::hash() const 
  //---------------------------------------------------
{
  USize hash=0;
  if (pimp)
    hash=pimp->s.m_Hash;
  return hash;
}
//---------------------------------------------------
inline Char skString::at(USize index) const
//---------------------------------------------------
{
  Char c=0;
  if (index<length() && pimp)
    c=pimp->m_PString[index];
  return c;	
}
#ifdef __SYMBIAN32__
//---------------------------------------------------
inline TPtrC skString::ptr() const 
  //---------------------------------------------------
{
  if (pimp){
    return TPtrC((const TUint16 *)(pimp->m_PString),pimp->m_Length);
  }else{
    return TPtrC();
  }
}
#else
//---------------------------------------------------
inline const Char * skString::ptr() const 
  //---------------------------------------------------
{
  return c_str();
}
#endif
//---------------------------------------------------
inline const Char * skString::c_str() const 
  //---------------------------------------------------
{
  const Char * c=skSTR("");
  if (pimp)
    c=(const Char *)(pimp->m_PString);
  return c;
}
#ifdef __SYMBIAN32__
//-------------------------------------------------------------------------
inline skString::operator TCleanupItem()
  //-------------------------------------------------------------------------
  // used for leave support of local variable Strings
{
  return TCleanupItem(Cleanup,this);
}
//-------------------------------------------------------------------------
inline void skString::Cleanup(TAny * s)
  //-------------------------------------------------------------------------
  // called in case there is a leave, make sure the underlying string is deleted
{
  reinterpret_cast<skString *>(s)->deRef();
}
#endif
#ifdef USE_FLOATING_POINT
//---------------------------------------------------
inline float ATOF(const Char * c)
//---------------------------------------------------
{
#ifdef UNICODE_STRINGS
#ifdef __SYMBIAN32__
  float f=0.0f;
  char temp[100];
  int size=wcstombs (temp, c, sizeof(temp));
  if (size>=0){
    temp[size]=0;
    f=atof(temp);
  }
  return f;
#else
  return (float) wcstod(c,0);
#endif
#else
  return (float) atof(c);
#endif
}
#endif
//---------------------------------------------------
inline int ATOI(const Char * c)
//---------------------------------------------------
{
#ifdef UNICODE_STRINGS
#ifdef __SYMBIAN32__
  int i=0;
  char temp[100];
  int size=wcstombs (temp, c, sizeof(temp));
  if (size>=0){
    temp[size]=0;
    i=atoi(temp);
  }
  return i;
#else
  return _wtoi(c);
#endif
#else
  return atoi(c);
#endif
}

#endif





