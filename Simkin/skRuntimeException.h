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

  $Id: skRuntimeException.h,v 1.8 2001/11/22 11:13:21 sdw Exp $
*/
#ifndef SKRUNTIMEEXCEPTION_H
#define SKRUNTIMEEXCEPTION_H

#include "skString.h"

const int skRuntimeException_Code=3;

/**
 * This exception is thrown when there are execution errors in a Simkin script
 */
class CLASSEXPORT skRuntimeException {
 public:
  /**
   * Constructor - receives information about the exception
   */
  skRuntimeException(const skString& location,int line_num,const skString& msg) 
      : m_Location(location),m_Msg(msg),m_LineNum(line_num){
  }
  /**
   * this method returns a description of the exception
   */
  skString toString() const {
    return m_Location+skSTR(":")+skString::from(m_LineNum)+skSTR("-")+m_Msg;
  }
   /**
   * returns the location of the script
   */
  skString location() const{
    return m_Location;
  }
  /**
   * shows the line number where the runtime error occurred
   */
  int lineNum() const{
    return m_LineNum;
  }
private:
  int m_LineNum;
  skString m_Msg;
  skString m_Location;
};
#endif
