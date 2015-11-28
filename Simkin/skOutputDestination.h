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

  $Id: skOutputDestination.h,v 1.11 2003/11/20 16:24:22 sdw Exp $
*/
#ifndef skOutputDestination_h
#define skOutputDestination_h

#include "skStringBuffer.h"
#include <stdio.h>

#ifdef STREAMS_ENABLED
#ifdef STL_STREAMS
#include <fstream>
using std::ofstream;
#else
#include <fstream.h>
#endif
#endif

/**
* this interface encapsulates a destination to which an object can be written
* It is implemented by concrete classes
*/
class CLASSEXPORT skOutputDestination
#ifdef __SYMBIAN32__
: public CBase
#endif
{
public:
  /**
   * Virtual destructor
   */
  virtual ~skOutputDestination();
  /**
   * Writes a string out
   */
  virtual void  write(const skString& s)=0;
#ifdef __SYMBIAN32__
  /**
   * Writes a string out
   * \remarks only available in Symbian version
   */
  virtual void  write(const TDesC& s)=0;
#endif
};
class CLASSEXPORT skOutputFile : public skOutputDestination
{
public:
  /**
   * Opens a file for writing to
   * @param file the name of the file to open
   */
  IMPORT_C skOutputFile(const skString& file);
  /**
   * Destructor - closes the file
   */
  virtual IMPORT_C ~skOutputFile();
  /**
   * Writes a string to a file
   */
  virtual IMPORT_C void write(const skString& s);
#ifdef __SYMBIAN32__
  /**
   * Writes a string to a file
   * \remarks only available in Symbian version
   */
  virtual IMPORT_C void write(const TDesC& s);
#endif
private:
#ifdef STREAMS_ENABLED
  ofstream      m_Out;
#else
  FILE *        m_Out;
#endif
};
class CLASSEXPORT skOutputString : public skOutputDestination
{
public:
  /**
   * Creates an output wrapped around a string buffer
   * @param out the string buffer to be wrapped
   */
  IMPORT_C skOutputString(skStringBuffer& out);
  virtual IMPORT_C ~skOutputString();
  /**
   * Adds a string to the buffer
   * @exception Symbian - a leaving function
   */
  virtual IMPORT_C void write(const skString& s);
#ifdef __SYMBIAN32__
  /**
   * Adds a string to the buffer
   * \remarks only available in Symbian version
   * @exception Symbian - a leaving function
   */
  virtual IMPORT_C void write(const TDesC& s);
#endif
private:
  skStringBuffer& m_Out;
};

#endif
