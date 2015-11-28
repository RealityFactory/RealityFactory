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

  $Id: skScriptError.h,v 1.4 2003/04/11 18:05:39 simkin_cvs Exp $
*/
#ifndef SKSCRIPTERROR_H
#define SKSCRIPTERROR_H

#include "skGeneral.h"

#include "skException.h"


/**
 * This class captures an error that occurs during script execution
 * It can be used in place of an exception handling on platforms such as Windows CE
 */
class CLASSEXPORT skScriptError
#ifdef __SYMBIAN32__
: public CBase
#endif
{
 public:
  enum ErrorType { NONE=0,PARSE_ERROR, RUNTIME_ERROR, XMLPARSE_ERROR, TREENODEPARSE_ERROR };
  
  skScriptError()
    : m_ErrorCode(NONE),m_Exception(0)
    {}
  virtual ~skScriptError(){
    delete m_Exception;
  }
  /**
   * Returns the error code
   */
  ErrorType     getErrorCode() const {
    return m_ErrorCode;
  }
  /**
   * Returns the exception associated with the error 
   * This can be downcast to a specific exception type
   */
  skException *  getException(){
    return m_Exception;
  }
  void          setError(ErrorType type,skException * exception){
    m_ErrorCode=type;
    if (m_Exception){
      delete m_Exception;
      m_Exception=0;
    }
    m_Exception=exception;
  }
 private:
  /** this code indicates the type of exception */
  ErrorType     m_ErrorCode;
  skException * m_Exception;  
};
#endif
