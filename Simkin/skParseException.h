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

  $Id: skParseException.h,v 1.20 2003/04/15 07:02:00 simkin_cvs Exp $
*/
#ifndef SKPARSEEXCEPTION_H
#define SKPARSEEXCEPTION_H

#include "skException.h"
#include "skString.h"
#include "skValist.h"
#include "skStringBuffer.h"
xskNAMED_LITERAL(QuoteStart,skSTR(" near \""));
xskNAMED_LITERAL(QuoteEnd,skSTR("\""));
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
   * Copy constructor
   */
  skCompileError(const skCompileError& e) 
    : m_LineNum(e.m_LineNum),m_LexBuffer(e.m_LexBuffer),m_Msg(e.m_Msg){
  }
  /**
   * Constructor which is passed information about the error
   */
  skCompileError(skString location,int line_num,const skString& msg,const skString& lex_buffer)
    : m_LineNum(line_num),m_Location(location),m_LexBuffer(lex_buffer),m_Msg(msg){
  }
  /** 
   * Assignment operator
   */
  skCompileError& operator=(const skCompileError& e){
    m_LineNum=e.m_LineNum;
    m_Msg=e.m_Msg;
    m_LexBuffer=e.m_LexBuffer;
    return *this;
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
    return skString::addStrings(m_Location.ptr(),s_colon,skString::from(m_LineNum).ptr(),s_colon,m_Msg.ptr(),s_QuoteStart,m_LexBuffer.ptr(),s_QuoteEnd);
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
#ifdef INSTANTIATE_TEMPLATES
EXTERN_TEMPLATE template class CLASSEXPORT skTVAList<skCompileError>;
#endif
/**
 * This is a list of compile errors
 */
class CLASSEXPORT skCompileErrorList : public skTVAList<skCompileError> 
{
};
/**
 * This exception is thrown when there are parse errors in some Simkin script
 */
class CLASSEXPORT skParseException : public skException
{
 public:
  /**
   * Constructor 
   */
  skParseException(){
  }
  /**
   * Sets the errors in this exception
   */
  void setErrors(const skCompileErrorList& errors){
    m_Errors=errors;
  }
  /**
   * this method returns a list of errors encountered in parsing the script
   */
  const skCompileErrorList& getErrors() const{
    return m_Errors;
  }
  /**
   * this method returns a terse string representation of the exception - returning the just the error message of the first error
   */
  skString getMessage() const {
    skString ret;
    if (m_Errors.entries())
      ret=m_Errors[0].msg();
    return ret;
  }
  /**
   * this method returns a verbose string representation of the exception - concatenating all the verbose error messages onto different lines
   * it includes location and line number information
   */
  skString toString() const {
    skStringBuffer ret(50);
    for (unsigned int i=0;i<m_Errors.entries();i++){
      ret.append(m_Errors[i].toString());
      ret.append(skSTR("\n"));
    }
    return ret.toString();
  }
 private:
  skCompileErrorList m_Errors;
};

#endif
