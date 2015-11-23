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

  $Id: skParseException.h,v 1.9 2001/11/22 11:13:21 sdw Exp $
*/
#ifndef SKPARSEEXCEPTION_H
#define SKPARSEEXCEPTION_H

#include "skString.h"
#include "skValist.h"

/**
  This class encapsulates an error message from the parser, representing a syntax error in the script
*/
class CLASSEXPORT skCompileError
{
 public:
  /** 
   * Default constructor
   */
  skCompileError() 
    : m_LineNum(0){
  }
  /**
   * Constructor which is passed information about the error
   */
  skCompileError(skString location,int line_num,const skString& msg,const skString& lex_buffer)
    : m_Location(location),m_LineNum(line_num),m_Msg(msg),m_LexBuffer(lex_buffer){
  }
  /**
   * returns the location of the script, as passed into the parse() function
   */
  skString location() const{
    return m_Location;
  }
  /**
   * shows the line number where the syntax error occurred
   */
  int lineNum() const{
    return m_LineNum;
  }
  /**
   * gives a message about the error
   */
  skString msg() const{
    return m_Msg;
  }
  /**
   * returns the lex buffer being handled at the time 
   */
  skString lexBuffer() const{
    return m_LexBuffer;
  }
  /**
   * this method returns a string representation of the whole error
   */
  skString toString() const {
    return m_Location+skSTR(":")+skString::from(m_LineNum)+skSTR(":")+m_Msg+skSTR(" near \"")+m_LexBuffer+skSTR("\"");
  }
  /** this message compares two compile errors
   * @return true if this message text is the same as the other message text
   */
  bool operator==(const skCompileError& err) const {
    return m_Msg==err.m_Msg;
  }
 private:
  int m_LineNum;
  skString m_Location;
  skString m_LexBuffer;
  skString m_Msg;
};
EXTERN_TEMPLATE template class CLASSEXPORT skTVAList<skCompileError>;

/**
 * This is a list of compile errors
 */
class CLASSEXPORT skCompileErrorList : public skTVAList<skCompileError> 
{
};
const int skParseException_Code=2;

/**
 * This exception is thrown when there are parse errors in some Simkin script
 */
class CLASSEXPORT skParseException {
 public:
  /**
   * Constructor - the exception is passed a list of errors
   */
  skParseException(const skCompileErrorList& errors) : m_Errors(errors) {
  }
  /**
   * this method returns a list of errors encountered in parsing the script
   */
  const skCompileErrorList& getErrors() const{
    return m_Errors;
  }
  /**
   * this method returns a string representation of the exception - concatenating all the error messages onto different lines
   */
  skString toString() const {
    skString ret;
    for (unsigned int i=0;i<m_Errors.entries();i++)
      ret+=m_Errors[i].toString()+skSTR("\n");
    return ret;
  }
 private:
  skCompileErrorList m_Errors;
};

#endif
