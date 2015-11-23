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

  $Id: skTreeNodp.h,v 1.14 2001/11/22 11:13:21 sdw Exp $
*/
#include "skAlist.h"

const int       MAXBUFFER=20000;


EXTERN_TEMPLATE template class CLASSEXPORT skTAList<skTreeNode>;

/**
 * This class represents a list of  {@link skTreeNode skTreeNodes}
 */
class  CLASSEXPORT skTreeNodeList :  public skTAList<skTreeNode>
{           
 public:
  /** Constructs a blank list */
  skTreeNodeList();
  /** Creates a list which is a deep copy of another list
   * @param list - list to copy
   */
  skTreeNodeList(const skTreeNodeList& list);
  /** Destroys list and its children */
  virtual ~skTreeNodeList();
  /** Searches for a child item with the given label
   * @param label - the label of the child to look for
   * @return the first matching child - or 0 if not found
   */
  skTreeNode *      findItem(const skString& label) const;
  /** Searches for a child item with the given label and data
   * @param label - the label of the child to look for
   * @param data - the data of the child to look for
   * @return the first matching child - or 0 if not found
   */
  skTreeNode *      findItem(const skString& label,const skString& data) const;
  /** Returns the nth child from this list
   * @param i - the zero-based index of the item to retrieve
   * @return the nth child
   */
  skTreeNode *      nthElt(USize  i) const;
  /** This method performs a deep copy from another list
   * @param list - the other list to copy
   */
  skTreeNodeList&   operator=(const skTreeNodeList& list);
};
// switches on the optimization where a shared class buffer is used
#define USECLASSBUFFER

class P_TreeNodeReader 
{
 public:
#ifdef STREAMS_ENABLED
  /** Construct a reader from a given stream */
  P_TreeNodeReader(istream& in);
#endif
  enum Lexeme		{ L_IDENT, L_TEXT, L_LBRACE, L_RBRACE, L_EOF, L_ERROR };
  /** this method sets up the LexText buffer */
  void grabBuffer();
  /** this method sets the error flag */
  void error(const skString& msg);
  /** this method returns true if the end of the input stream was found */
  bool eof();
  /** this method returns the next character from the stream */
  int get();
  /** this method returns the next character from the stream, without consuming it */
  int peek();
  /** this method returns the next lexical token from the current stream
   */
  Lexeme lex();
  /** this method puts the current lexical token back */
  void 	unLex();
  /** this method parses an entire tree node 
   * @param pparent - the parent of the node
   * @return a new tree node added to the parent, or 0 if there was a parsing error
   */
  skTreeNode * 	parseTreeNode(skTreeNode * pparent);
  /** This method parses a list of treenodes
   * @param list - the list to add children to as they are parsed
   */
  void parseTreeNodeList(skTreeNode * list);
  /** this method adds the given character to the lextext buffer */
  void addToLexText(Char c);
  /** the file name of the input stream */
  skString m_FileName;
  /** a flag indicating if an unlex has been performed */
  bool m_UnLex;
  /** this indicates the last lexical token retrieved by the lex() function */
  Lexeme m_LastLexeme;
  /** the buffer used to collect the text for the current lexical token */
  Char	* m_LexText;
  /** the position within the current input stream */
  unsigned short m_Pos;
  /** the line number of the current input stream position */
  unsigned short m_LineNum;
#ifdef STREAMS_ENABLED
  /** the input stream */
  istream& m_In;
#endif
  /** a flag indicating whether there was a parse error in the input stream */
  bool 	m_Error;
#ifdef USECLASSBUFFER
  /** this flag indicates whether the parser is using the class-level buffer */
  bool	m_UsingClassLexText;
  /** a class shared buffer which can be used as an optimization */
  static Char g_ClassLexText[MAXBUFFER];
  /** this flag indicates whether the class buffer is in use by a parser */
  static bool g_LexTextUsed;
#endif
};








