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

  $Id: skBoundsException.h,v 1.16 2003/04/14 15:24:57 simkin_cvs Exp $
*/
#ifndef SKBOUNDSEXCEPTION_H
#define SKBOUNDSEXCEPTION_H

#include "skString.h"
#include "skConstants.h"

const int skBoundsException_Code=1;

/**
 * this class represents an exception thrown by the collection classes
 */
class CLASSEXPORT skBoundsException {
 public:
  /**
   * Constructor
   */
  skBoundsException(const skString& msg,const Char * file,int linenum)
    : m_Msg(msg),m_File(file),m_LineNum(linenum){
  }
  /**
   * produces a string describing the exception
   * @exception Symbian - a leaving function
   */
  skString toString() const{
     skString file;
     file=m_File;
	  return skString::addStrings(file.ptr(),s_colon,skString::from(m_LineNum).ptr(),m_Msg.ptr());
  }
 private:
  skString      m_Msg;
  const Char *  m_File;
  int           m_LineNum;

};
#endif
