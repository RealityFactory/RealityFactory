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

  $Id: skString.cpp,v 1.40 2003/05/15 19:20:06 simkin_cvs Exp $
*/

#include <stdio.h>
#include "skStringBuffer.h"

//---------------------------------------------------
void P_String::init()
  //---------------------------------------------------
{
  const Char * buffer=m_PString;
  s.m_Hash=0;
  while (*buffer){
    s.m_Hash^=*buffer;
    buffer++;
  }
  m_Length=(USize)STRLEN(m_PString);
}
#ifndef __SYMBIAN32__
// This can leave, so not available in Symbian
//---------------------------------------------------
skString::skString(const Char repeatChar, USize len)
  //---------------------------------------------------
{
  // create skString of length len with repeated unsigned char
  pimp=skNEW(P_String);
  pimp->s.m_Const=false;
  Char * str = skARRAY_NEW(Char,len+1);
  USize i;
  for (i = 0; i < len; i++)
    str[i] = repeatChar;
  str[i] = 0;   // NULL string terminator
  pimp->m_PString=str;
  pimp->init();
}
#endif
//---------------------------------------------------
skString::skString(P_String * p)
  //---------------------------------------------------
{                          
  pimp=p;
  pimp->init();
}
//---------------------------------------------------
EXPORT_C int skString::indexOf(Char c) const
  //---------------------------------------------------
{
  int pos=-1;
  if (pimp){
    Char * ppos=STRCHR(pimp->m_PString,c);
    if (ppos!=0)
      pos=ppos-pimp->m_PString;
  }
  return pos;
}
//---------------------------------------------------
EXPORT_C int skString::indexOfLast(Char c) const
  //---------------------------------------------------
{
  int pos=-1;
  if (pimp){
    Char * ppos=STRRCHR(pimp->m_PString,c);
    if (ppos!=0)
      pos=ppos-pimp->m_PString;
  }
  return pos;
}
//---------------------------------------------------
EXPORT_C int skString::indexOf(const skString& s) const
  //---------------------------------------------------
{
  int pos=-1;
  if (pimp){
    Char * ppos=STRSTR(pimp->m_PString,s.pimp->m_PString);
    if (ppos!=0)
      pos=ppos-pimp->m_PString;
  }
  return pos;
}
//---------------------------------------------------
EXPORT_C skString skString::substr(USize start) const
  //---------------------------------------------------
{
  skString str;
  if (pimp){
    if (start <= pimp->m_Length)
      str=substr(start,pimp->m_Length-start);
  }
  return str;
}
//---------------------------------------------------
EXPORT_C skString skString::substr(USize start,USize s_length) const
  //---------------------------------------------------
{
  skString str;
  if (pimp){
    // watch for special case of asking for the whole string
    if (start==0 && s_length==pimp->m_Length){
      str=*this;
    }else{
      if (start <= length()){
        Char * buffer=skARRAY_NEW(Char,s_length+1);
        SAVE_POINTER(buffer);
        STRNCPY(buffer,pimp->m_PString+start,s_length);
        buffer[s_length]=0;
        P_String * pnew=skNEW(P_String);
        pnew->s.m_Const=false;
        pnew->m_PString=buffer;
        // call internal non-copying constructor
        str=skString(pnew);
        RELEASE_POINTER(buffer);
      }
    }
  }
  return str;
}
//---------------------------------------------------
EXPORT_C skString& skString::operator+=(const skString& s)
  //---------------------------------------------------
{
  operator=(operator+(s));
  return *this;
}
#ifdef __SYMBIAN32__
// Symbian-friendly function
//---------------------------------------------------
EXPORT_C skString operator+(const TDesC& s1,const skString& s2)
  //---------------------------------------------------
{
  USize len=s1.Length()+s2.length()+1;
  Char * buffer=skARRAY_NEW(Char,len);
  SAVE_POINTER(buffer);
  STRNCPY(buffer,(const Char *)s1.Ptr(),s1.Length());
  STRCAT(buffer,s2.c_str());
  P_String * pnew=skNEW(P_String);
  pnew->s.m_Const=false;
  pnew->m_PString=buffer;
  // call internal non-copying constructor
  skString str=skString(pnew);
  RELEASE_POINTER(buffer);
  return str;
}
#endif
//---------------------------------------------------
EXPORT_C skString operator+(const Char * s1,const skString& s2)
  //---------------------------------------------------
{
  USize len=STRLEN(s1)+s2.length()+1;
  Char * buffer=skARRAY_NEW(Char,len);
  SAVE_POINTER(buffer);
  STRCPY(buffer,s1);
  STRCAT(buffer,s2.c_str());
  P_String * pnew=skNEW(P_String);
  pnew->s.m_Const=false;
  pnew->m_PString=buffer;
  // call internal non-copying constructor
  skString str=skString(pnew);
  RELEASE_POINTER(buffer);
  return str;
}
//---------------------------------------------------
EXPORT_C skString skString::operator+(const skString& s)  const
  //---------------------------------------------------
{
  if (pimp || s.pimp){
    USize len=length()+s.length()+1;
    Char * buffer=skARRAY_NEW(Char,len);
    buffer[0]=0;
    SAVE_POINTER(buffer);
    if (pimp)
      STRCPY(buffer,pimp->m_PString);
    if (s.pimp)
      STRCAT(buffer,s.pimp->m_PString);
    P_String * pnew=skNEW(P_String);
    pnew->s.m_Const=false;
    pnew->m_PString=buffer;
    // call internal non-copying constructor
    skString str=skString(pnew);
    RELEASE_POINTER(buffer);
    return str;
  }else{
    return skString();
  }
}
//---------------------------------------------------
EXPORT_C skString& skString::operator=(const Char * s) 
  //---------------------------------------------------
{
  if (pimp==0 || s!=pimp->m_PString){
    deRef();
    assign(s);
  }     
  return *this;
}
//---------------------------------------------------
EXPORT_C bool skString::operator!=(const skString& s) const
  //---------------------------------------------------
{
  return (*this==(s))==false;
}
//---------------------------------------------------
EXPORT_C bool skString::operator!=(const Char * s) const
  //---------------------------------------------------
{
  return (*this==(s))==false;
}
//---------------------------------------------------
EXPORT_C skString skString::fromBuffer(Char * buffer)
  //---------------------------------------------------
{
  P_String * pimp=skNEW(P_String);
  pimp->m_PString=buffer;
  pimp->s.m_Const=false;
  pimp->init();
  skString str=skString(pimp);
  return str;
}
//---------------------------------------------------
EXPORT_C skString skString::copyFromBuffer(const Char * buffer,USize len)
  //---------------------------------------------------
{
  skString s;
  s.assign(buffer,len);
  return s;
}
//---------------------------------------------------
EXPORT_C skString skString::from(int i)
  //---------------------------------------------------
{
  skString str;
#ifdef __SYMBIAN32__
  char buffer[100];
  sprintf(buffer,"%d",i);
#ifdef UNICODE_STRINGS
  Char ubuffer[200];
  int size=mbstowcs (ubuffer,buffer,sizeof(ubuffer));
  if (size>=0){
    ubuffer[size]=0;
    str=ubuffer;
  }
#else
  str=buffer;
#endif
#else
  Char buffer[100];
#ifdef UNICODE_STRINGS
  swprintf(buffer,L"%d",i);
#else
  sprintf(buffer,"%d",i);
#endif
  str=skString(buffer);
#endif
  return str;
}
#ifdef USE_FLOATING_POINT
//---------------------------------------------------
EXPORT_C skString skString::from(float i)
  //---------------------------------------------------
{
  skString str;
#ifdef __SYMBIAN32__
  char buffer[100];
  sprintf(buffer,"%f",i);
#ifdef UNICODE_STRINGS
  Char ubuffer[200];
  int size=mbstowcs (ubuffer,buffer,sizeof(ubuffer));
  if (size>=0){
    ubuffer[size]=0;
    str=ubuffer;
  }
#else
  str=buffer;
#endif
#else
  Char buffer[100];
#ifdef UNICODE_STRINGS
  swprintf(buffer,L"%f",i);
#else
  sprintf(buffer,"%f",i);
#endif
  str=skString(buffer);
#endif
  return str;
}
#endif
//---------------------------------------------------
EXPORT_C skString skString::from(USize i)
  //---------------------------------------------------
{
  skString str;
#ifdef __SYMBIAN32__
  char buffer[100];
  sprintf(buffer,"%u",i);
#ifdef UNICODE_STRINGS
  Char ubuffer[200];
  int size=mbstowcs (ubuffer,buffer,sizeof(ubuffer));
  if (size>=0){
    ubuffer[size]=0;
    str=ubuffer;
  }
#else
  str=buffer;
#endif
#else
  Char buffer[100];
#ifdef UNICODE_STRINGS
  swprintf(buffer,L"%u",i);
#else
  sprintf(buffer,"%u",i);
#endif
  str=skString(buffer);
#endif
  return str;
}
//---------------------------------------------------
EXPORT_C int skString::to(void) const
  //---------------------------------------------------
{
  int i=0;
  if (pimp)
    i=ATOI(pimp->m_PString);
  return i;
}
#ifdef USE_FLOATING_POINT
//---------------------------------------------------
EXPORT_C float skString::toFloat(void) const
  //---------------------------------------------------
{
  float f=0;
  if (pimp)
    f=ATOF(pimp->m_PString);
  return f;
}
#endif
//---------------------------------------------------
EXPORT_C skString skString::from(Char ch)
  //---------------------------------------------------
{
  skString s;
  s=ch;
  return s;
}
//---------------------------------------------------
EXPORT_C skString skString::literal(const Char * s)
  //---------------------------------------------------
{
  //    create literal string
  P_String * pimp=skNEW(P_String);
  pimp->m_PString=(Char *)s;
  pimp->init();
  return skString(pimp);
}
//---------------------------------------------------
EXPORT_C skString skString::ltrim() const
  //---------------------------------------------------
{
  skString s;
  if (pimp){
    Char * p=pimp->m_PString;
    while (*p && ISSPACE(*p))
      p++;
    s=p;
  }
  return s;
}
//---------------------------------------------------
EXPORT_C void skString::assign(const Char * s,int len)
  //---------------------------------------------------
{
  if (s){
    if (len){
      pimp=skNEW(P_String);
      pimp->m_PString=skARRAY_NEW(Char,len+1);
      pimp->s.m_Const=false;
      memcpy(pimp->m_PString,s,sizeof(Char)*len);
      pimp->m_PString[len]=0;
      pimp->init();
    }else{
      int s_len=STRLEN(s);
      if (s_len){
        pimp=skNEW(P_String);
        pimp->m_PString=skARRAY_NEW(Char,s_len+1);
        pimp->s.m_Const=false;
        STRCPY((Char *)pimp->m_PString,s);
        pimp->init();
      }else{
        pimp=0;
      }
    }
  }else{
    pimp=0;
  }
}
//-----------------------------------------------------------------
EXPORT_C bool skString::equalsIgnoreCase(const skString& s) const
  //-----------------------------------------------------------------
{
#ifdef __SYMBIAN32__
  // do a folding comparison on Symbian
  return ptr().CompareF(s.ptr());
#else
  return (STRCMPI((const Char *)*this,(const Char *)s)==0);
#endif
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
#ifndef __SYMBIAN32__
// Not available in Symbian version
//-----------------------------------------------------------------
skString skString::readFromFile(const skString& fileName)
  //-----------------------------------------------------------------
{
  skString s;
#ifdef UNICODE_STRINGS
  FILE * in=_wfopen(fileName,skSTR("rb"));
#else
  FILE * in=fopen(fileName,skSTR("r"));
#endif
  if (in){
    skStringBuffer buffer(2048);
    Char achbuffer[1024];
    while(!feof(in)){
      memset(achbuffer,0,sizeof(achbuffer));
#ifdef UNICODE_STRINGS
      fgetws(achbuffer,sizeof(achbuffer),in);
#else
      fgets(achbuffer,sizeof(achbuffer),in);
#endif
      buffer.append(achbuffer);
    }
    fclose(in);
    s=buffer.toString();
  }
  return s;
}
//-----------------------------------------------------------------
void skString::writeToFile(const skString& fileName)
  //-----------------------------------------------------------------
{
#ifdef UNICODE_STRINGS
  FILE * out=_wfopen(fileName,skSTR("wb+"));
#else
  FILE * out=fopen(fileName,skSTR("w+"));
#endif
  if (out){
    fwrite(pimp->m_PString,sizeof(Char)*pimp->m_Length,0,out);
    fclose(out);
  }
}
#endif
#ifdef __SYMBIAN32__
// Symbian-friendly function
//-----------------------------------------------------------------
EXPORT_C skString skString::replace(const TDesC& old_substr,const TDesC& new_substr) const
  //-----------------------------------------------------------------
{
  skString str;
  skString old_string;
  SAVE_VARIABLE(old_string);
  old_string=old_substr;
  skString new_string;
  SAVE_VARIABLE(new_string);
  new_string=new_substr;
  str=replace(old_string,new_string);
  RELEASE_VARIABLE(new_string);
  RELEASE_VARIABLE(old_string);
  return str;
}
#endif
//-----------------------------------------------------------------
EXPORT_C skString skString::replace(const Char * old_substr,const Char * new_substr) const
  //-----------------------------------------------------------------
{
  skString str;
  skString old_string;
  SAVE_VARIABLE(old_string);
  old_string=old_substr;
  skString new_string;
  SAVE_VARIABLE(new_string);
  new_string=new_substr;
  str=replace(old_string,new_string);
  RELEASE_VARIABLE(new_string);
  RELEASE_VARIABLE(old_string);
  return str;
}
//-----------------------------------------------------------------
EXPORT_C skString skString::replace(const skString& old_substr,const skString& new_substr) const
  //-----------------------------------------------------------------
{
  skString new_substring = *this;
  if (pimp && old_substr.pimp){
    //    count number of instances
    int numInsts = 0;
    int index = 0;
    Char * pos = 0;
    do{
      pos = STRSTR(pimp->m_PString + index, old_substr.pimp->m_PString);
      if (pos){
        numInsts++;
        index = pos - pimp->m_PString + old_substr.pimp->m_Length;
      }
    }while (pos);
    if (numInsts){
      int newLength = int(pimp->m_Length) + numInsts * (int(new_substr.length()) - int(old_substr.pimp->m_Length));
      if (newLength > 0) {
        Char * buffer=skARRAY_NEW(Char,newLength+1);
        index = 0;
        pos = 0;
        unsigned int targetIndex = 0;
        unsigned int lenBefore = 0;
        do{
          pos = STRSTR(pimp->m_PString + index, old_substr.pimp->m_PString);
          if (pos) {
            lenBefore = pos - (pimp->m_PString + index);
            ::memcpy(buffer + targetIndex, pimp->m_PString + index, lenBefore * sizeof(Char));
            ::memcpy(buffer + targetIndex + lenBefore, new_substr.c_str(), new_substr.length() * sizeof(Char));
            index = pos - pimp->m_PString + old_substr.pimp->m_Length;
            targetIndex += new_substr.length() + lenBefore;
          }else{
            lenBefore = pimp->m_Length - index;
            ::memcpy(buffer + targetIndex, pimp->m_PString + index, lenBefore * sizeof(Char));
            targetIndex += lenBefore;
          }
        }while (pos);
        buffer[targetIndex] = 0;
        new_substring = skString::fromBuffer(buffer);
      }else
        new_substring = skString();
    }
  }
  return new_substring;
}
//-----------------------------------------------------------------
EXPORT_C skString skString::removeInitialBlankLines() const
  //-----------------------------------------------------------------
{
  skString s=*this;
  if (pimp){
    bool loop=true;
    while (loop){
      int cr_pos=s.indexOf('\n');
      if (cr_pos!=-1){
        skString line=s.substr(0,cr_pos);
        if (line.ltrim().length()==0){
          s=s.substr(cr_pos+1);
        }else
          loop=false;
      }else
        loop=false;
    }
  }
  return s;
}
#ifdef __SYMBIAN32__
// Symbian-friendly versions of the addStrings functions
//-------------------------------------------------------------------------
EXPORT_C skString skString::addStrings(const TDesC& s1,const TDesC& s2,const TDesC& s3)
  //-------------------------------------------------------------------------
{
  skString str;
  skStringBuffer s(s1.Length()+s2.Length()+s3.Length());
  s.append(s1);
  s.append(s2);
  s.append(s3);
  str=s.toString();
  return str;
}
//-------------------------------------------------------------------------
EXPORT_C skString skString::addStrings(const TDesC& s1,const TDesC& s2)
  //-------------------------------------------------------------------------
{
  skStringBuffer s(s1.Length()+s2.Length());
  s.append(s1);
  s.append(s2);
  return s.toString();
}
//-------------------------------------------------------------------------
EXPORT_C skString skString::addStrings(const TDesC& s1,const TDesC& s2,const TDesC& s3,const TDesC& s4)
  //-------------------------------------------------------------------------
{
  skStringBuffer s(s1.Length()+s2.Length()+s3.Length()+s4.Length());
  s.append(s1);
  s.append(s2);
  s.append(s3);
  s.append(s4);
  return s.toString();
}
//-------------------------------------------------------------------------
EXPORT_C skString skString::addStrings(const TDesC& s1,const TDesC& s2,const TDesC& s3,const TDesC& s4,const TDesC& s5)
  //-------------------------------------------------------------------------
{
  skStringBuffer s(s1.Length()+s2.Length()+s3.Length()+s4.Length()+s5.Length());
  s.append(s1);
  s.append(s2);
  s.append(s3);
  s.append(s4);
  s.append(s5);
  return s.toString();
}
//-------------------------------------------------------------------------
EXPORT_C skString skString::addStrings(const TDesC& s1,const TDesC& s2,const TDesC& s3,const TDesC& s4,const TDesC& s5,const TDesC& s6)
  //-------------------------------------------------------------------------
{
  skStringBuffer s(s1.Length()+s2.Length()+s3.Length()+s4.Length()+s5.Length()+s6.Length());
  s.append(s1);
  s.append(s2);
  s.append(s3);
  s.append(s4);
  s.append(s5);
  s.append(s6);
  return s.toString();
}
//-------------------------------------------------------------------------
EXPORT_C skString skString::addStrings(const TDesC& s1,const TDesC& s2,const TDesC& s3,const TDesC& s4,const TDesC& s5,const TDesC& s6,const TDesC& s7)
  //-------------------------------------------------------------------------
{
  skStringBuffer s(s1.Length()+s2.Length()+s3.Length()+s4.Length()+s5.Length()+s6.Length()+s7.Length());
  s.append(s1);
  s.append(s2);
  s.append(s3);
  s.append(s4);
  s.append(s5);
  s.append(s6);
  s.append(s7);
  return s.toString();
}
//-------------------------------------------------------------------------
EXPORT_C skString skString::addStrings(const TDesC& s1,const TDesC& s2,const TDesC& s3,const TDesC& s4,const TDesC& s5,const TDesC& s6,const TDesC& s7,const TDesC&  s8)
  //-------------------------------------------------------------------------
{
  skStringBuffer s(s1.Length()+s2.Length()+s3.Length()+s4.Length()+s5.Length()+s6.Length()+s7.Length()+s8.Length());
  s.append(s1);
  s.append(s2);
  s.append(s3);
  s.append(s4);
  s.append(s5);
  s.append(s6);
  s.append(s7);
  s.append(s8);
  return s.toString();
}
#endif
//-------------------------------------------------------------------------
EXPORT_C skString skString::addStrings(const skString& s1,const skString& s2,const skString& s3)
  //-------------------------------------------------------------------------
{
  skString str;
  skStringBuffer s(s1.length()+s2.length()+s3.length());
  s.append(s1);
  s.append(s2);
  s.append(s3);
  str=s.toString();
  return str;
}
//-------------------------------------------------------------------------
EXPORT_C skString skString::addStrings(const skString& s1,const skString& s2)
  //-------------------------------------------------------------------------
{
  skStringBuffer s(s1.length()+s2.length());
  s.append(s1);
  s.append(s2);
  return s.toString();
}
//-------------------------------------------------------------------------
EXPORT_C skString skString::addStrings(const skString& s1,const skString& s2,const skString& s3,const skString& s4)
  //-------------------------------------------------------------------------
{
  skStringBuffer s(s1.length()+s2.length()+s3.length()+s4.length());
  s.append(s1);
  s.append(s2);
  s.append(s3);
  s.append(s4);
  return s.toString();
}
//-------------------------------------------------------------------------
EXPORT_C skString skString::addStrings(const skString& s1,const skString& s2,const skString& s3,const skString& s4,const skString& s5)
  //-------------------------------------------------------------------------
{
  skStringBuffer s(s1.length()+s2.length()+s3.length()+s4.length()+s5.length());
  s.append(s1);
  s.append(s2);
  s.append(s3);
  s.append(s4);
  s.append(s5);
  return s.toString();
}
//-------------------------------------------------------------------------
EXPORT_C skString skString::addStrings(const skString& s1,const skString& s2,const skString& s3,const skString& s4,const skString& s5,const skString& s6)
  //-------------------------------------------------------------------------
{
  skStringBuffer s(s1.length()+s2.length()+s3.length()+s4.length()+s5.length()+s6.length());
  s.append(s1);
  s.append(s2);
  s.append(s3);
  s.append(s4);
  s.append(s5);
  s.append(s6);
  return s.toString();
}
//-------------------------------------------------------------------------
EXPORT_C skString skString::addStrings(const skString& s1,const skString& s2,const skString& s3,const skString& s4,const skString& s5,const skString& s6,const skString& s7)
  //-------------------------------------------------------------------------
{
  skStringBuffer s(s1.length()+s2.length()+s3.length()+s4.length()+s5.length()+s6.length()+s7.length());
  s.append(s1);
  s.append(s2);
  s.append(s3);
  s.append(s4);
  s.append(s5);
  s.append(s6);
  s.append(s7);
  return s.toString();
}
//-------------------------------------------------------------------------
EXPORT_C skString skString::addStrings(const skString& s1,const skString& s2,const skString& s3,const skString& s4,const skString& s5,const skString& s6,const skString& s7,const skString&  s8)
  //-------------------------------------------------------------------------
{
  skStringBuffer s(s1.length()+s2.length()+s3.length()+s4.length()+s5.length()+s6.length()+s7.length()+s8.length());
  s.append(s1);
  s.append(s2);
  s.append(s3);
  s.append(s4);
  s.append(s5);
  s.append(s6);
  s.append(s7);
  s.append(s8);
  return s.toString();
}
//---------------------------------------------------
EXPORT_C skString& skString::operator=(Char c)
  //---------------------------------------------------
{
  deRef();
  assign(&c,1);
  return *this;
}
//---------------------------------------------------
EXPORT_C void skString::replaceInPlace(Char old_char,Char new_char)
  //---------------------------------------------------
{
  if (pimp){
    for (USize i=0;i<pimp->m_Length;i++){
      if (pimp->m_PString[i]==old_char){
        pimp->m_PString[i]=new_char;
      }
    }
  }
}
