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

  $Id: skInputSource.h,v 1.12 2003/11/20 16:24:22 sdw Exp $
*/
#ifndef skInputSource_h
#define skInputSource_h


#include "skString.h"
#include <stdio.h>

#ifdef STREAMS_ENABLED
#ifdef STL_STREAMS
#include <fstream>
using std::istream;
using std::ifstream;
#else
#include <fstream.h>
#endif
#endif

/**
* this interface encapsulates a source from which an object can be read
* It is implemented by concrete classes
*/
class CLASSEXPORT skInputSource
#ifdef __SYMBIAN32__
: public CBase
#endif
{
public:
  IMPORT_C virtual ~skInputSource();
  /**
   * Returns whether the source has reached the end
   */
  virtual bool eof() const=0;
  /**
   * Returns and consumes the next character in the input
   */
  virtual int get()=0;
  /**
   * Returns, but does not consume the next character in the input
   */
  virtual int peek()=0;
  /**
   * Reads the whole source into a string
   */
  virtual skString readAllToString()=0;
};

class CLASSEXPORT skInputFile : public skInputSource
{
public:
  /**
   * Constructor. Opens the given file
   * @param filename the name of the file to open
   */
  IMPORT_C skInputFile(const skString& filename);
  /**
   * Blank Constructor. 
   */
  IMPORT_C skInputFile();
  /**
   * Opens the file
   * @param filename the name of the file to open
   */
  IMPORT_C void open(const skString& filename);
#ifdef __SYMBIAN32__
  /**
   * Opens the file
   * \remarks only available in Symbian version
   * @param filename the name of the file to open
   */
  IMPORT_C void open(const TDesC& file);
#endif
  IMPORT_C virtual ~skInputFile();
  /**
   * Returns whether the file has reached the end
   */
  IMPORT_C virtual bool eof() const;
  /**
   * Returns and consumes the next character in the file
   */
  IMPORT_C virtual int get();
  /**
   * Returns, but does not consume the next character in the file
   */
  IMPORT_C virtual int peek();
  /**
   * Reads the whole source into a string
   * @exception Symbian - a leaving function
   */
  IMPORT_C virtual skString readAllToString();
private:
#ifdef STREAMS_ENABLED
  ifstream      m_In;
#else
  FILE *        m_In;
  bool          m_Peeked;
  int           m_PeekedChar;
#endif
#ifdef UNICODE_STRINGS
  bool          m_FileIsUnicode;
#endif
};
class CLASSEXPORT skInputString : public skInputSource
{
public:
  /**
   * Constructor. Wraps an input source around a string
   * @param in the string to read in
   */
  IMPORT_C skInputString(const skString& in);
  /**
   * Destructor
   */
  IMPORT_C virtual ~skInputString();
  /**
   * Returns whether the input has reached the end of the string
   */
  IMPORT_C virtual bool eof() const;
  /**
   * Returns and consumes the next character in the string
   */
  IMPORT_C virtual int get();
  /**
   * Returns, but does not consume the next character in the string
   */
  IMPORT_C virtual int peek();
  /**
   * Returns the input string in its entirety
   */
  IMPORT_C virtual skString readAllToString();
private:
  skString      m_In;
  unsigned int  m_Pos;
  bool          m_Peeked;
  int           m_PeekedChar;
};

#endif
