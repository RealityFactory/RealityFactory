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

  $Id: skAsciiString.cpp,v 1.7 2003/05/15 19:20:06 simkin_cvs Exp $
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "skAsciiString.h"

//---------------------------------------------------
void P_AsciiString::init()
  //---------------------------------------------------
{
  const char * buffer=m_PString;
  s.m_Hash=0;
  while (*buffer){
    s.m_Hash^=*buffer;
    buffer++;
  }
  m_Length=(USize)strlen(m_PString);
}
#ifndef __SYMBIAN32__
//---------------------------------------------------
skAsciiString::skAsciiString(const char * s,int /* i */)
  //---------------------------------------------------
{                          
  //	create literal string
  pimp=new P_AsciiString;
  pimp->m_PString=(char *)s;
  pimp->init();
}
// This can leave, so not available in Symbian
//---------------------------------------------------
skAsciiString::skAsciiString(const char repeatchar, USize len)
  //---------------------------------------------------
{
  // create skAsciiString of length len with repeated unsigned char
  pimp=new P_AsciiString;
  pimp->s.m_Const=false;
  char * str = new char[(len+1)];
  USize i;
  for (i = 0; i < len; i++)
    str[i] = repeatchar;
  str[i] = 0;	// NULL string terminator
  pimp->m_PString=str;
  pimp->init();
}
#endif
//---------------------------------------------------
skAsciiString::skAsciiString(P_AsciiString * p)
  //---------------------------------------------------
{                          
  pimp=p;
  pimp->init();
}
//---------------------------------------------------
EXPORT_C int skAsciiString::indexOf(char c) const
  //---------------------------------------------------
{
  int pos=-1;
  if (pimp){
    char * ppos=strchr(pimp->m_PString,c);
    if (ppos!=0)
      pos=ppos-pimp->m_PString;
  }
  return pos;
}
//---------------------------------------------------
EXPORT_C int skAsciiString::indexOfLast(char c) const
  //---------------------------------------------------
{
  int pos=-1;
  if (pimp){
    char * ppos=strrchr(pimp->m_PString,c);
    if (ppos!=0)
      pos=ppos-pimp->m_PString;
  }
  return pos;
}
//---------------------------------------------------
EXPORT_C int skAsciiString::indexOf(const skAsciiString& s) const
  //---------------------------------------------------
{
  int pos=-1;
  if (pimp){
    char * ppos=strstr(pimp->m_PString,s.pimp->m_PString);
    if (ppos!=0)
      pos=ppos-pimp->m_PString;
  }
  return pos;
}
//---------------------------------------------------
EXPORT_C skAsciiString skAsciiString::substr(USize start) const
  //---------------------------------------------------
{
  skAsciiString str;
  if (pimp){
    if (start <= pimp->m_Length)
      str=substr(start,pimp->m_Length-start);
  }
  return str;
}
//---------------------------------------------------
EXPORT_C skAsciiString skAsciiString::substr(USize start,USize length) const
  //---------------------------------------------------
{
  skAsciiString str;
  if (pimp){
    if (start <= pimp->m_Length){
      char * buffer=new char[length+1];
      strncpy(buffer,pimp->m_PString+start,length);
      buffer[length]=0;
      P_AsciiString * pnew=new P_AsciiString;
      pnew->s.m_Const=false;
      pnew->m_PString=buffer;
      // call internal non-copying constructor
      str=skAsciiString(pnew);
    }
  }
  return str;
}
//---------------------------------------------------
EXPORT_C skAsciiString& skAsciiString::operator+=(const skAsciiString& s)
  //---------------------------------------------------
{
  operator=(operator+(s));
  return *this;
}
//---------------------------------------------------
EXPORT_C skAsciiString operator+(const char * s1,const skAsciiString& s2)
//---------------------------------------------------
{
  USize len=strlen(s1)+s2.length()+1;
  char * buffer=new char[len];
  strcpy(buffer,s1);
  strcat(buffer,s2.c_str());
  return skAsciiString::fromBuffer(buffer);
}
//---------------------------------------------------
EXPORT_C skAsciiString skAsciiString::operator+(const skAsciiString& s)  const
  //---------------------------------------------------
{
  USize len=length()+s.length()+1;
  char * buffer=new char[len];
  buffer[0]=0;
 if (pimp)
    strcpy(buffer,pimp->m_PString);
  if (s.pimp)
    strcat(buffer,s.pimp->m_PString);
  P_AsciiString * pnew=new P_AsciiString;
  pnew->s.m_Const=false;
  pnew->m_PString=buffer;
  // call internal non-copying constructor
  return skAsciiString(pnew);
}
//---------------------------------------------------
EXPORT_C skAsciiString& skAsciiString::operator=(const char * s) 
  //---------------------------------------------------
{
  if (pimp==0 || s!=pimp->m_PString){
    deRef();
    assign(s);
  }	
  return *this;
}
//---------------------------------------------------
EXPORT_C bool skAsciiString::operator!=(const skAsciiString& s) const
  //---------------------------------------------------
{
  return (*this==(s))==false;
}
//---------------------------------------------------
EXPORT_C bool skAsciiString::operator!=(const char * s) const
  //---------------------------------------------------
{
  return (*this==(s))==false;
}
//---------------------------------------------------
P_AsciiString::P_AsciiString()
  //---------------------------------------------------
: m_Length(0),m_PString("")
{
  s.m_Hash=0;
  s.m_RefCount=1;
  s.m_Const=true;
}
//---------------------------------------------------
EXPORT_C skAsciiString skAsciiString::fromBuffer(char * buffer)
  //---------------------------------------------------
{
  //	create literal string
  P_AsciiString * pimp=new P_AsciiString;
  pimp->m_PString=buffer;
  pimp->s.m_Const=false;
  pimp->init();
  return skAsciiString(pimp);
}
//---------------------------------------------------
EXPORT_C skAsciiString skAsciiString::from(int i)
  //---------------------------------------------------
{
  skAsciiString s;
  char buffer[100];
  sprintf(buffer,"%d",i);
  s=buffer;
  return s;
}
#ifdef USE_FLOATING_POINT
//---------------------------------------------------
EXPORT_C skAsciiString skAsciiString::from(float i)
  //---------------------------------------------------
{
  skAsciiString s;
  char buffer[100];
  sprintf(buffer,"%f",i);
  s=buffer;
  return s;
}
#endif
//---------------------------------------------------
EXPORT_C skAsciiString skAsciiString::from(USize i)
  //---------------------------------------------------
{
  skAsciiString s;
  char buffer[100];
  sprintf(buffer,"%u",i);
  s=buffer;
  return s;
}
//---------------------------------------------------
EXPORT_C int skAsciiString::to(void) const
  //---------------------------------------------------
{
  int i=0;
  if (pimp)
    i=atoi(pimp->m_PString);
  return i;
}
#ifdef USE_FLOATING_POINT
//---------------------------------------------------
EXPORT_C float skAsciiString::toFloat(void) const
  //---------------------------------------------------
{
  float f=0;
  if (pimp)
    f=atof(pimp->m_PString);
  return f;
}
#endif
#ifndef __SYMBIAN32__
//---------------------------------------------------
skAsciiString::skAsciiString(const char * s, USize len)
  //---------------------------------------------------
{   
  assign(s,len);
}
//---------------------------------------------------
skAsciiString::skAsciiString(const char * s) 
  //---------------------------------------------------
{   
  assign(s);
}
#endif
//---------------------------------------------------
EXPORT_C skAsciiString skAsciiString::from(char ch)
  //---------------------------------------------------
{
  skAsciiString str;
  char	s[2];

  s[0] = ch;
  s[1] = '\0';
  str=s;

  return str;
}
//---------------------------------------------------
EXPORT_C skAsciiString skAsciiString::literal(const char * s)
  //---------------------------------------------------
{
  //    create literal string
  P_AsciiString * pimp=skNEW(P_AsciiString);
  pimp->m_PString=(char *)s;
  pimp->init();
  return skAsciiString(pimp);
}
//---------------------------------------------------
EXPORT_C skAsciiString skAsciiString::ltrim() const
  //---------------------------------------------------
{
  skAsciiString s;
  if (pimp){
    char * p=pimp->m_PString;
    while (*p && isspace(*p))
      p++;
    s=p;
  }
  return s;
}
//---------------------------------------------------
void skAsciiString::assign(const char * s,int len)
//---------------------------------------------------
{
  if (s){
    if (len){
      pimp=new P_AsciiString;
      pimp->m_PString=new char[len+1];
      pimp->s.m_Const=false;
      memcpy(pimp->m_PString,s,sizeof(char)*len);
      pimp->m_PString[len]=0;
      pimp->init();
    }else{
      int s_len=strlen(s);
      if (s_len){
        pimp=new P_AsciiString;
        pimp->m_PString=new char[s_len+1];
        pimp->s.m_Const=false;
        strcpy((char *)pimp->m_PString,s);
        pimp->init();
      }else{
        pimp=0;
      }
    }
  }else{
    pimp=0;
  }
}
#ifdef STREAMS_ENABLED
//-----------------------------------------------------------------
ostream& operator<<(ostream& out,const skAsciiString& s)
  //-----------------------------------------------------------------
{
  out << (const char *)s;
  return out;
}
#endif
