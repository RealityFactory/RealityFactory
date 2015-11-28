/*
  copyright 1996-2003
  Simon Whiteside

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed out the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  $Id: skOutputDestination.cpp,v 1.11 2003/04/14 15:24:57 simkin_cvs Exp $
*/
#include "skOutputDestination.h"

//-----------------------------------------------------------------
skOutputDestination::~skOutputDestination()
//-----------------------------------------------------------------
{
}
//-----------------------------------------------------------------
EXPORT_C skOutputFile::skOutputFile(const skString& filename)
//-----------------------------------------------------------------
: 
#ifdef STREAMS_ENABLED
m_Out(filename)
#else
m_Out(0)
#endif
{
#ifndef STREAMS_ENABLED
#ifdef UNICODE_STRINGS
#ifdef __SYMBIAN32__
  // SYMBIAN_QUESTION: does this function leave???
  m_Out=wfopen(filename.c_str(),skSTR("wb"));
#else
  m_Out=_wfopen(filename,skSTR("wb"));
#endif
  // write out a unicode tag
  if (m_Out){
    fputc(0xFF,m_Out);
    fputc(0xFE,m_Out);
  }
#else
  m_Out=fopen(filename,skSTR("w"));
#endif
#endif
}
//-----------------------------------------------------------------
EXPORT_C skOutputFile::~skOutputFile()
//-----------------------------------------------------------------
{
#ifndef STREAMS_ENABLED
  if (m_Out)
    fclose(m_Out);
#endif
}
//-----------------------------------------------------------------
EXPORT_C void skOutputFile::write(const skString& s)
//-----------------------------------------------------------------
{
#ifdef STREAMS_ENABLED
  m_Out << s;
#else
  // SYMBIAN_QUESTION: does this function leave???
  fwrite(s.c_str(),s.length()*sizeof(Char),1,m_Out);
#endif
}
#ifdef __SYMBIAN32__
// Symbian-friendly version
//-----------------------------------------------------------------
EXPORT_C void skOutputFile::write(const TDesC& s)
//-----------------------------------------------------------------
{
#ifdef STREAMS_ENABLED
  m_Out << s;
#else
  // SYMBIAN_QUESTION: does this function leave???
  fwrite((const Char *)s.Ptr(),s.Length()*sizeof(Char),1,m_Out);
#endif
}
#endif
//-----------------------------------------------------------------
EXPORT_C skOutputString::skOutputString(skStringBuffer& out)
//-----------------------------------------------------------------
: m_Out(out)
{
}
//-----------------------------------------------------------------
EXPORT_C skOutputString::~skOutputString()
//-----------------------------------------------------------------
{
}
//-----------------------------------------------------------------
EXPORT_C void skOutputString::write(const skString& s)
//-----------------------------------------------------------------
{
  m_Out.append(s);
}
#ifdef __SYMBIAN32__
// Symbian-friendly version
//-----------------------------------------------------------------
EXPORT_C void skOutputString::write(const TDesC& s)
//-----------------------------------------------------------------
{
  m_Out.append(s);
}
#endif
