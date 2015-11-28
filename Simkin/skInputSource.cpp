/*
  copyright 1996-2003
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

  $Id: skInputSource.cpp,v 1.20 2003/11/20 16:24:22 sdw Exp $
*/
#include "skInputSource.h"
#include "skStringBuffer.h"
#ifdef UNICODE_STRINGS
#include "skAsciiStringBuffer.h"
#include "skEncodingUtils.h"
#endif

//-----------------------------------------------------------------
EXPORT_C skInputSource::~skInputSource()
//-----------------------------------------------------------------
{
}
//-----------------------------------------------------------------
EXPORT_C skInputFile::skInputFile()
//-----------------------------------------------------------------
: skInputSource()
#ifndef STREAMS_ENABLED
,m_In(0),m_Peeked(false),m_PeekedChar(0)
#endif
#ifdef UNICODE_STRINGS
,m_FileIsUnicode(true)
#endif
{
}
//-----------------------------------------------------------------
EXPORT_C skInputFile::skInputFile(const skString& filename)
//-----------------------------------------------------------------
:
#ifdef STREAMS_ENABLED
#ifdef STL_STREAMS
m_In(filename,ios::in)
#else
m_In(filename,ios::in|ios::nocreate)
#endif
#else
m_In(0),m_Peeked(false),m_PeekedChar(0)
#endif
#ifdef UNICODE_STRINGS
,m_FileIsUnicode(true)
#endif
{
}
//-----------------------------------------------------------------
EXPORT_C skInputFile::~skInputFile()
//-----------------------------------------------------------------
{
#ifndef STREAMS_ENABLED
  if (m_In)
    fclose(m_In);
  m_In=0;
#endif
}
//-----------------------------------------------------------------
EXPORT_C void skInputFile::open(const skString& filename)
//-----------------------------------------------------------------
{
#ifndef STREAMS_ENABLED
  if (m_In)
    fclose(m_In);
  m_In=0;
#endif
#ifdef UNICODE_STRINGS
  m_FileIsUnicode=true;
#endif
#ifdef STREAMS_ENABLED
  m_In.open(filename);
#else
  m_In=0;
  m_Peeked=false;
  m_PeekedChar=0;
#endif
#ifndef STREAMS_ENABLED
#ifdef UNICODE_STRINGS
#ifdef __SYMBIAN32__
  // SYMBIAN_QUESTION: does this function leave???
  m_In=wfopen(filename.c_str(),skSTR("rb"));
#else
  m_In=_wfopen(filename,skSTR("rb"));
#endif
#else
  m_In=fopen(filename,skSTR("r"));
#endif
#endif
#ifdef UNICODE_STRINGS
  if(m_In){
    // check what the encoding of the file is
    int first_word=peek();
    int first_byte=first_word & 0xff;
    int second_byte=(first_word & 0xff00) >> 8;
    if (first_byte==0xFF && second_byte==0xFE){
      m_FileIsUnicode=true;
      // absorb the leading marker character
      m_Peeked=false;    
    }else{
      m_FileIsUnicode=false;
      // Make last peek invalid and re-seek. 
      fseek(m_In, 0, SEEK_SET);
      m_Peeked=false;    
    }
  }
#endif
}
#ifdef __SYMBIAN32__
//-----------------------------------------------------------------
EXPORT_C void skInputFile::open(const TDesC& filename)
//-----------------------------------------------------------------
{
  skString s_filename;
  s_filename=filename;
  open(s_filename);
}
#endif
//-----------------------------------------------------------------
EXPORT_C bool skInputFile::eof() const
//-----------------------------------------------------------------
{
  bool eof=true;
#ifdef STREAMS_ENABLED
  if (m_In.good())
    eof=(m_In.eof()==1);
#else
  // SYMBIAN_QUESTION: does this function leave???
  if (m_In)
    eof=feof(m_In)!=0;
#endif
  return eof;
}
//-----------------------------------------------------------------
EXPORT_C int skInputFile::get()
//-----------------------------------------------------------------
{
#ifdef STREAMS_ENABLED
  return m_In.get();
#else
  int c=0;
  if (m_Peeked){
    c=m_PeekedChar;
    m_Peeked=false;
  }else{
#ifdef UNICODE_STRINGS
    if (m_FileIsUnicode){
#ifdef __SYMBIAN32__
      wchar_t c1;
      // SYMBIAN_QUESTION: does this function leave???
      fread(&c1,sizeof(c1),1,m_In);
      c=c1;
#else
      c=fgetwc(m_In);
#endif
    }else{
      c=fgetc(m_In);
    }
#else
    c=fgetc(m_In);
#endif
  }
  return c;
#endif
}
//-----------------------------------------------------------------
EXPORT_C skString skInputFile::readAllToString()
//-----------------------------------------------------------------
{
  skString str;
  const int BUF_SIZE=1024;
#ifdef STREAMS_ENABLED
  skStringBuffer s_buffer(BUF_SIZE);
  Char buffer[BUF_SIZE];
  while (eof()==false){
    memset(buffer,0,BUF_SIZE*sizeof(Char));
    m_In.read(buffer,(BUF_SIZE-1)*sizeof(Char));
    s_buffer.append(buffer);
  }
  str=s_buffer.toString();
#else
#ifndef UNICODE_STRINGS
  if (m_In){
     // reset file pointer
     fseek(m_In,0,SEEK_SET);
     skStringBuffer s_buffer(BUF_SIZE);
     Char buffer[BUF_SIZE];
     while (eof()==false){
       memset(buffer,0,BUF_SIZE*sizeof(Char));
       fread(buffer,sizeof(Char),BUF_SIZE-1,m_In);
       s_buffer.append(buffer);
     }
     str=s_buffer.toString();
  }
#else
  if (m_In){
     if (m_FileIsUnicode){
       // reset file pointer
       fseek(m_In,sizeof(wchar_t),SEEK_SET);
       skStringBuffer s_buffer(BUF_SIZE);
       Char buffer[BUF_SIZE];
       while (eof()==false){
         memset(buffer,0,BUF_SIZE*sizeof(Char));
         fread(buffer,sizeof(Char),BUF_SIZE-1,m_In);
         s_buffer.append(buffer);
       }
       str=s_buffer.toString();
     }else{
       // reading an ASCII file
       // reset file pointer
       fseek(m_In,0,SEEK_SET);
       skAsciiStringBuffer s_buffer(BUF_SIZE);
       char buffer[BUF_SIZE];
       while (eof()==false){
         memset(buffer,0,BUF_SIZE);
         fread(buffer,1,BUF_SIZE-1,m_In);
         s_buffer.append(buffer);
       }
       str=skEncodingUtils::fromAscii(s_buffer.toString());
     }
#endif
  }
#endif
  return str;
}
//-----------------------------------------------------------------
EXPORT_C int skInputFile::peek()
//-----------------------------------------------------------------
{
#ifdef STREAMS_ENABLED
  return m_In.peek();
#else
  if (m_Peeked==false){
    m_Peeked=true;
#ifdef UNICODE_STRINGS
    if (m_FileIsUnicode){
#ifdef __SYMBIAN32__
      wchar_t c1;
      // SYMBIAN_QUESTION: does this function leave???
      fread(&c1,sizeof(c1),1,m_In);
      m_PeekedChar=c1;
#else
      m_PeekedChar=fgetwc(m_In);
#endif
    }else{
      m_PeekedChar=fgetc(m_In);
    }
#else
    m_PeekedChar=fgetc(m_In);
#endif
  }
  return m_PeekedChar;
#endif
}
//-----------------------------------------------------------------
EXPORT_C skInputString::skInputString(const skString& in)
//-----------------------------------------------------------------
: m_In(in),m_Pos(0),m_Peeked(false),m_PeekedChar(0)
{
}
//-----------------------------------------------------------------
EXPORT_C skInputString::~skInputString()
//-----------------------------------------------------------------
{
}
//-----------------------------------------------------------------
EXPORT_C bool skInputString::eof() const
//-----------------------------------------------------------------
{
  return m_Pos>=m_In.length();
}
//-----------------------------------------------------------------
EXPORT_C int skInputString::get()
//-----------------------------------------------------------------
{
  int c=0;
  if (m_Peeked){
    c=m_PeekedChar;
    m_Peeked=false;
  }else{
    if (m_Pos<m_In.length())
      c=m_In.at(m_Pos++);
    else
      c=-1;
  }
  return c;
}
//-----------------------------------------------------------------
EXPORT_C int skInputString::peek()
//-----------------------------------------------------------------
{
  if (m_Peeked==false){
    m_Peeked=true;
    if (m_Pos<m_In.length())
      m_PeekedChar=m_In.at(m_Pos++);
    else
      m_PeekedChar=-1;
  }
  return m_PeekedChar;
}
//-----------------------------------------------------------------
EXPORT_C skString skInputString::readAllToString()
//-----------------------------------------------------------------
{
  return m_In;
}
