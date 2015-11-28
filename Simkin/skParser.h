/*
  Copyright 1996-2002
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

  $Id: skParser.h,v 1.16 2003/04/14 15:24:57 simkin_cvs Exp $
*/

#ifndef skPARSER_H
#define skPARSER_H

#include "skParseException.h"
#include "skParseNode.h"

#ifdef __SYMBIAN32__
const int MAXYYTEXT=256;
#else
const int MAXYYTEXT=4096;
#endif

/**
 * This class is used to parse a string of Simkin code
 */
class  skParser 
#ifdef __SYMBIAN32__
: public CBase
#endif
{
 public:
  /**
   * constructor
   * @param code a string containing the Simkin code
   * @param location the location of the script (used for compile/parse errors)
   */
  skParser(const skString& code,const skString& location);
  /**
   * Destructor - destroys the temporary nodes and strings
   */
  ~skParser();
  /**
   * this method instructs the parser to begin parsing the string passed into the constructor
   * The string is assumed to contain a method definition
   * @return If the parse is valid, then this will return the top of the parse tree, which the caller should delete
   * @exception Symbian - a leaving function
   */
  skMethodDefNode * parseMethod();
  /**
   * this method instructs the parser to begin parsing the string passed into the constructor
   * The string is assumed to contain an expression, beginning with "="
   * @return If the parse is valid, then this will return the top of the parse tree, which the caller should delete
   * @exception Symbian - a leaving function
   */
#ifdef EXECUTE_PARSENODES
  skExprNode * parseExpression();
#else
  skCompiledExprNode * parseExpression();
#endif
#ifdef __SYMBIAN32__
  /**
   * Conversion operator to Symbian TCleanupItem. This is provided to allow this object to be pushed by value onto the 
   * Symbian Cleanup Stack
   * \remarks only available in Symbian version
   */
  operator TCleanupItem();
  /**
   * Called via Symbian CleanupStack in the event of a leave. This calls the cleanupTempNodes method in the parser
   * \remarks only available in Symbian version
   */
  static void Cleanup(TAny * s);
#endif

 /**
   * sets the current top-level parse node
   * @param pNode the method definition or expression node
   */
  void setTopNode(skParseNode* pNode);
  /**
   * removes  a string from the temporary string list
   * @param str the string to remove
   */
  void removeTempString(skString* str);
  /**
   * Saves a parse node to the list of temporary nodes. If an error occurs these will be cleared up
   * @exception Symbian - a leaving function
   */
  void addParseNode(skParseNode* pNode);
  /**
   * this adds a compile error message to the list of messages 
   * @exception Symbian - a leaving function
   */
  void appendError(const skString& msg);
  /**
   * this adds a compile error message to the list of messages 
   * @exception Symbian - a leaving function
   */
  void appendError(const Char * msg);
  /** 
      This returns the current compile error list
   */
  skCompileErrorList& getErrList();
  /**
   * This message returns the next token in the stream
   * @param lvalp pointer to the YYSTYPE (token structure)
   * @param lloc pointer to the YYLTYPE (token position structure)
   * @exception Symbian - a leaving function
   */
  int lex(void * lvalp,void * lloc); // returns a token
 private:
  /**
   * returns the next character to be lexed (might be a put-back character)
   */
  int nextChar(); 
  /**
   * puts the given character back
   */
  void putbackchar(int i); 
  /**
   * returns true if the eof the stream has been reached
   */
  bool eof(); 
  /**
   * private members to support the parser.
   */
#ifdef EXECUTE_PARSENODES
  /**
   * returns a string object for the given id. The function first searches for a match in the existing identifier list
   * @exception Symbian - a leaving function
   */
  skString * lookupIdentifier(const Char * id);
#else
  /**
   * returns an index for the given id. The function first searches for a match in the existing identifier list
   * @exception Symbian - a leaving function
   */
  int lookupIdentifier(const Char * id);
#endif
  /**
   * deletes the parse tree if an error occurs during parsing
   */
  void cleanupTempNodes(); 
  /**
   * clears the list of nodes, but does *not* delete them (this is done if no error occurs 
   */
  void clearTempNodes();
  
   skParseNode*		m_TopNode;
   /** this list holds the nodes while the script is being parsed - in case an exception is thrown and they need to be deleted */
   skParseNodeList	m_TempNodes;
   /** this list holds literal strings while the script is being parsed - in case an exception is thrown and they need to be deleted */
   skTAList<skString>	m_TempStringLiterals;
   /** this list holds identifiers while the script is being parsed - in case an exception is thrown and they need to be deleted */
#ifdef EXECUTE_PARSENODES
   skTAList<skString>	m_Identifiers;
#else
   skStringList m_Identifiers;
#endif
   skCompileErrorList	m_ErrList;
   
   Char * m_LexBuffer; //  used by lexical analyser to hold the current token
   skString m_InputBuffer; // the buffer read by the lexer
   int m_PutBack; // character to be put back
   int m_LineNum; // linenumber being lexed
   unsigned int m_Pos; //  position in the line
   skString m_Location;
};

#endif
