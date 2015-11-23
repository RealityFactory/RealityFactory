/*
Copyright 1996-2001
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

$Id: skString.inl,v 1.9 2002/01/08 23:04:40 sdw Exp $
*/

#ifndef _STR_INL
#define _STR_INL

#include "skString.h"
#include <string.h>
#include <ctype.h>

class   P_String 
{
 public:
    Char *			m_PString;
    USize			m_Length;
    USize			m_Hash;
    bool			m_Const;
    USize			m_RefCount;
    
    P_String();
    ~P_String();
    void			init();

};    
extern const Char * blank;                    

extern P_String * blank_string;

#ifdef UNICODE_STRINGS
#define STRCMP wcscmp
#define STRCAT wcscat
#define STRLEN wcslen
#define STRCPY wcscpy
#define STRNCPY wcsncpy
#define STRCHR wcschr
#define ISSPACE iswspace
#else
#define STRCPY strcpy
#define STRCAT strcat
#define STRCMP strcmp
#define STRLEN strlen
#define STRCHR strchr
#define STRNCPY strncpy
#define ISSPACE isspace
#endif

//---------------------------------------------------
inline P_String::~P_String() 
//---------------------------------------------------
{ 
    if (m_Const==0) 
	delete[] m_PString; 
}
//---------------------------------------------------
inline skString::operator const Char * () const 
//---------------------------------------------------
{
    return (const Char *)pimp->m_PString;
}
//---------------------------------------------------
inline USize skString::length() const 
//---------------------------------------------------
{
    return pimp->m_Length;
}
//---------------------------------------------------
inline  bool skString::operator<(const skString& s) const
//---------------------------------------------------
{
    bool ret=false;
    if (pimp==s.pimp)
	ret=false;
    else{
	int i=strcmp((const char *)pimp->m_PString,(const char *)s.pimp->m_PString);
	if (i<0)
	    ret=true;
    }
    return ret;
}
//---------------------------------------------------
inline  bool skString::operator==(const skString& s) const
//---------------------------------------------------
{
    if (pimp==s.pimp)
      return true;
    else
      if (pimp->m_Hash==s.pimp->m_Hash && pimp->m_Length==s.pimp->m_Length)
	return (strcmp((const char *)pimp->m_PString,(const char *)s.pimp->m_PString)==0);
    else
      return false;
}
//---------------------------------------------------
inline  bool skString::operator==(const Char * s) const
//---------------------------------------------------
{
  if (s)
    return (STRCMP(pimp->m_PString,s)==0);
  else
    return false;
}
//---------------------------------------------------
inline void skString::deRef()
     //---------------------------------------------------
{
    pimp->m_RefCount--;
    if (pimp->m_RefCount==0)
	delete pimp;
}

//---------------------------------------------------
inline  skString::skString()
//---------------------------------------------------
{
    assign(0);
}
//---------------------------------------------------
inline skString& skString::operator=(const skString& s) 
//---------------------------------------------------
{
    if (&s!=this){
	s.pimp->m_RefCount++;
	deRef();
	pimp=s.pimp;
    }	
    return *this;
}
//---------------------------------------------------
inline skString::skString(const skString& s) 
//---------------------------------------------------
    : pimp(s.pimp)
{
    pimp->m_RefCount++;
}
//---------------------------------------------------
inline skString::~skString() 
//---------------------------------------------------
{
    deRef();
}
//---------------------------------------------------
inline USize skString::hash() const 
//---------------------------------------------------
{
    return pimp->m_Hash;
}
//---------------------------------------------------
inline Char skString::at(USize index) const
//---------------------------------------------------
{
  Char c=0;
  if (index<pimp->m_Length)
    c=pimp->m_PString[index];
  return c;	
}

#endif





