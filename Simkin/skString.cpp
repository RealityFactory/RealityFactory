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

  $Id: skString.cpp,v 1.15 2002/01/09 23:04:14 sdw Exp $
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "skString.h"

#ifdef UNICODE_STRINGS
const Char * blank=L"";                    
#else
const Char * blank="";                    
#endif

class skBlankString : public P_String
{
public:
  skBlankString()
  {
    m_RefCount++;
  }
};
skBlankString g_blank_string;
P_String * blank_string=&g_blank_string;

//---------------------------------------------------
void P_String::init()
  //---------------------------------------------------
{
  const Char * buffer=m_PString;
  m_Hash=0;
  while (*buffer){
    m_Hash^=*buffer;
    buffer++;
  }
  m_Length=(USize)STRLEN(m_PString);
}
//---------------------------------------------------
skString::skString(const Char * s,int /* i */)
  //---------------------------------------------------
{                          
  //	create literal string
  pimp=new P_String;
  pimp->m_PString=(Char *)s;
  pimp->init();
}
//---------------------------------------------------
skString::skString(const Char repeatChar, USize len)
  //---------------------------------------------------
{
  // create skString of length len with repeated unsigned char
  pimp=new P_String;
  pimp->m_Const=false;
  Char * str = new Char[(len+1)];
  USize i;
  for (i = 0; i < len; i++)
    str[i] = repeatChar;
  str[i] = 0;	// NULL string terminator
  pimp->m_PString=str;
  pimp->init();
}
//---------------------------------------------------
skString::skString(P_String * p)
  //---------------------------------------------------
{                          
  pimp=p;
  pimp->init();
}
//---------------------------------------------------
int skString::indexOf(Char c) const
  //---------------------------------------------------
{
  int pos=-1;
  Char * ppos=STRCHR(pimp->m_PString,c);
  if (ppos!=0)
    pos=ppos-pimp->m_PString;
  return pos;
}
//---------------------------------------------------
skString skString::substr(USize start) const
  //---------------------------------------------------
{
  if (start <= pimp->m_Length)
    return substr(start,pimp->m_Length-start);
  else
    return skSTR("");
}
//---------------------------------------------------
skString skString::substr(USize start,USize length) const
  //---------------------------------------------------
{
  if (start <= pimp->m_Length){
    Char * buffer=new Char[length+1];
    STRNCPY(buffer,pimp->m_PString+start,length);
    buffer[length]=0;
    P_String * pnew=new P_String;
    pnew->m_Const=false;
    pnew->m_PString=buffer;
    // call internal non-copying constructor
    return skString(pnew);
  }else
    return skString(blank);
}
//---------------------------------------------------
skString& skString::operator+=(const skString& s)
  //---------------------------------------------------
{
  operator=(operator+(s));
  return *this;
}
//---------------------------------------------------
skString skString::operator+(const skString& s)  const
  //---------------------------------------------------
{
  USize len=pimp->m_Length+s.pimp->m_Length+1;
  Char * buffer=new Char[len];
  STRCPY(buffer,pimp->m_PString);
  STRCAT(buffer,s.pimp->m_PString);
  P_String * pnew=new P_String;
  pnew->m_Const=false;
  pnew->m_PString=buffer;
  // call internal non-copying constructor
  return skString(pnew);
}
//---------------------------------------------------
skString& skString::operator=(const Char * s) 
  //---------------------------------------------------
{
  if (s!=pimp->m_PString){
    deRef();
    assign(s);
  }	
  return *this;
}
//---------------------------------------------------
bool skString::operator!=(const skString& s) const
  //---------------------------------------------------
{
  if (pimp==s.pimp)
    return false;
  else
    if (pimp->m_Hash==s.pimp->m_Hash)
      return (STRCMP(pimp->m_PString,s.pimp->m_PString)!=0);
    else
      return true;
}
//---------------------------------------------------
bool skString::operator!=(const Char * s) const
  //---------------------------------------------------
{
  if (s)
    return (STRCMP(pimp->m_PString,s)!=0);
  else
    return true;
}
//---------------------------------------------------
P_String::P_String():m_Length(0),m_Hash(0),m_RefCount(1), m_PString((Char *)blank),m_Const(true)
  //---------------------------------------------------
{
}
//---------------------------------------------------
skString skString::fromBuffer(Char * buffer)
  //---------------------------------------------------
{
  //	create literal string
  P_String * pimp=new P_String;
  pimp->m_PString=buffer;
  pimp->m_Const=false;
  pimp->init();
  return skString(pimp);
}
//---------------------------------------------------
skString skString::from(int i)
  //---------------------------------------------------
{
  Char buffer[100];
#ifdef UNICODE_STRINGS
  swprintf(buffer,L"%d",i);
#else
  sprintf(buffer,"%d",i);
#endif
  return skString(buffer);
}
//---------------------------------------------------
skString skString::from(float i)
  //---------------------------------------------------
{
  Char buffer[100];
#ifdef UNICODE_STRINGS
  swprintf(buffer,L"%f",i);
#else
  sprintf(buffer,"%f",i);
#endif
  return skString(buffer);
}
//---------------------------------------------------
skString skString::from(USize i)
  //---------------------------------------------------
{
  Char buffer[100];
#ifdef UNICODE_STRINGS
  swprintf(buffer,L"%u",i);
#else
  sprintf(buffer,"%u",i);
#endif
  return skString(buffer);
}
//---------------------------------------------------
int skString::to(void) const
  //---------------------------------------------------
{
  return atoi((const char *)pimp->m_PString);
}
//---------------------------------------------------
float skString::toFloat(void) const
  //---------------------------------------------------
{
  return (float)atof((const char *)pimp->m_PString);
}
//---------------------------------------------------
skString::skString(const Char * s, USize len)
  //---------------------------------------------------
{   
  assign(s,len);
}
//---------------------------------------------------
skString::skString(const Char * s) 
  //---------------------------------------------------
{   
  assign(s);
}
//---------------------------------------------------
skString skString::from(Char ch)
  //---------------------------------------------------
{
  Char	s[2];

  s[0] = ch;
  s[1] = '\0';

  return skString(s);
}
//---------------------------------------------------
skString skString::literal(const Char * s)
  //---------------------------------------------------
{
  return skString(s,1);
}
//---------------------------------------------------
skString skString::ltrim() const
  //---------------------------------------------------
{
  char * p=pimp->m_PString;
  while (*p && ISSPACE(*p))
    p++;
  return skString(p);
}
//---------------------------------------------------
void skString::assign(const Char * s,int len)
  //---------------------------------------------------
{
  if (s){
    if (len){
      pimp=new P_String;
      pimp->m_PString=new Char[len+1];
      pimp->m_Const=false;
      memcpy(pimp->m_PString,s,len);
      pimp->m_PString[len]=0;
      pimp->init();
    }else{
      int s_len=STRLEN(s);
      if (s_len){
	pimp=new P_String;
	pimp->m_PString=new Char[s_len+1];
	pimp->m_Const=false;
	STRCPY((Char *)pimp->m_PString,s);
	pimp->init();
      }else{
	pimp=blank_string;	
	pimp->m_RefCount++;
      }
    }
  }else{
    pimp=blank_string;	
    pimp->m_RefCount++;
  }
}
#ifdef STREAMS_ENABLED
//-----------------------------------------------------------------
ostream& operator<<(ostream& out,const skString& s)
  //-----------------------------------------------------------------
{
  out << (const Char *)s;
  return out;
}
#endif

