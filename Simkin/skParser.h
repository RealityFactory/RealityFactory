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

  $Id: skParser.h,v 1.5 2001/11/22 11:13:21 sdw Exp $
*/

#ifndef skPARSER_H
#define skPARSER_H

#include "skParseException.h"
#include "skParseNode.h"

const int MAXYYTEXT=256;

/**
 * This class is used to parse a string of Simkin code
 */
class  skParser {
 public:
  /**
   * constructor
   * @param code a string containing the Simkin code
   * @param location the location of the script (used for compile/parse errors)
   */
  skParser(const skString& code,const skString& location);
  /**
   * Destructor - *clears* the temporary node list
   */
  ~skParser();
  /**
   * this method instructs the parser to begin parsing the string passed into the constructor
   * <p>If the parse is valid, then getTopNode() will return the top of the parse tree
   */
  void parse();
  /**
   * sets the current top-level parse node
   * @param pNode the method definition node
   */
  void setTopNode(skMethodDefNode* pNode);
  /** 
   * returns the current top-level parse node
   */
  skMethodDefNode * getTopNode();
  /**
   * Saves a parse node to the list of temporary nodes. If an error occurs these will be cleared up
   */
  void addParseNode(skParseNode* pNode);
  /**
   * this adds a compile error message to the list of messages 
   */
  void appendError(const skString& msg);
  /** 
      This returns the current compile error list
   */
  const skCompileErrorList& getErrList();
  /**
   * This message returns the next token in the stream
   * @param lvalp pointer to the YYSTYPE (token structure)
   * @param lloc pointer to the YYLTYPE (token position structure)
   */
  int lex(void * lvalp,void * lloc); // returns a token
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
   * deletes the parse tree if an error occurs during parsing
   */
  void cleanupTempNodes(); 
  /**
   * clears the list of nodes, but does *not* delete them (this is done if no error occurs 
   */
  void clearTempNodes();
  
 private:
  /**
   * private members to support the parser.
   */
   skMethodDefNode*		m_TopNode;
   skParseNodeList		m_TempNodes;
   skCompileErrorList	m_ErrList;
   
   Char m_LexBuffer[MAXYYTEXT]; //  used by lexical analyser to hold the current token
   skString m_InputBuffer; // the buffer read by the lexer
   int m_PutBack; // character to be put back
   int m_LineNum; // linenumber being lexed
   unsigned int m_Pos; //  position in the line
   skString m_Location;
};

#endif
