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

* $Id: skXMLParseException.h,v 1.7 2003/03/06 13:05:14 simkin_cvs Exp $
*/

#ifndef sk_XMLPARSEEXCEPTION_H
#define sk_XMLPARSEEXCEPTION_H

#include "skException.h"

const int skXMLParseException_Code=5;

/**
 * This class represents an exception during the parsing of an XML document - using the Simkin DOM classes.
 */
class CLASSEXPORT skXMLParseException : public skException
{
 public:
  /** Constructs a new exception */
  skXMLParseException (const skString& error,int line_num): m_Error(error),m_LineNum(line_num) {}
  /** returns the line number within the document where the error occurred */
  int getLineNum() const { return m_LineNum; }
  /** returns a string describing the error */
  skString getErrorMessage() const { return m_Error; }
  skString getMessage() const { return m_Error; }
  skString toString() const { return m_Error; }
 private:
  int m_LineNum;
  skString m_Error;
};
#endif
