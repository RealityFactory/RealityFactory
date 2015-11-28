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

 * $Id: skTreeNode.h,v 1.41 2003/04/23 14:34:51 simkin_cvs Exp $
*/


#ifndef TREENODE_H
#define TREENODE_H

#include "skString.h"
#include "skException.h"
#include "skConstants.h"

class  CLASSEXPORT skTreeNode;
class  CLASSEXPORT skTreeNodeList;
class  CLASSEXPORT skExecutableContext;
class  CLASSEXPORT skInputSource;
class  CLASSEXPORT skOutputDestination;


/**
 * This class encapsulates a node in a tree.
 * The node has a label, a piece of data and a list of subitems
 * <p>The label and data are both stored as strings. TreeNodes can be used to conveniently store hierchically ordered trees of textual data.
*/
class  CLASSEXPORT skTreeNode 
#ifdef __SYMBIAN32__
: public CBase
#endif
{
 public:
  /**
   * Default Constructor - blank label, data and an empty children list
   */
  inline skTreeNode();
#ifndef __SYMBIAN32__
  /**
   * Copy Constructor - does a deep copy
   * \remarks not available in Symbian version 
   */
  skTreeNode(const skTreeNode& );
#endif
  /**
   * Constructor - creates a treenode with the given label
   */
  inline skTreeNode(const skString& label); 
  /**
   * Constructor - creates a treenode with the given label and data
   */
  inline skTreeNode(const skString& label,const skString& data);
  /**
   * Constructor - creates a treenode with the given label and data (converted to a string form)
   */
  IMPORT_C skTreeNode(const skString& label,bool data);
  /**
   * Constructor - creates a treenode with the given label and data (converted to a string form)
   */
  IMPORT_C skTreeNode(const skString& label,int  data);
#ifdef USE_FLOATING_POINT
  /**
   * Constructor - creates a treenode with the given label and data (converted to a string form)
   */
  IMPORT_C skTreeNode(const skString& label,float data);
#endif
  /**
   * Destructor
   */
  virtual ~skTreeNode();
  /**
   * Assignment operator - does a deep copy
   * @exception Symbian - a leaving function
   */
  IMPORT_C skTreeNode& operator=(const skTreeNode& );
  /**
   * Does a deep comparison against another node, without checking the label of this node
   */
  IMPORT_C bool operator==(const skTreeNode& other) const;
  /**
   * Does a deep compare another node, with an optional check on the label
   */
  IMPORT_C bool deepCompare(const skTreeNode& other,bool check_tagname) const;
  /**
   * Returns this node's label
   */
  inline skString label() const;
  /**
   * Changes this node's label
   */
  inline void  label(const skString& s);
  /**
   * Changes this node's label
   */
  inline void  label(const Char * s);
  /**
   * Returns this node's data
   */
  inline skString data() const;
  /**
   * Changes this node's data
   */
  inline void  data(const skString& s);
  /**
   * Changes this node's data
   */
  inline void  data(const Char * s);
  /**
   * Returns this node's data as a boolean value
   */
  IMPORT_C bool	boolData() const;
  /**
   * Changes this node's data - converting the value to a string
   * @exception Symbian - a leaving function
   */
  IMPORT_C void	boolData(bool);
  /**
   * Returns this node's data as an integer value
   */
  IMPORT_C int 	intData() const;
  /**
   * Changes this node's data - converting the value to a string
   * @exception Symbian - a leaving function
   */
  IMPORT_C void	intData(int);
#ifdef USE_FLOATING_POINT
  /**
   * Returns this node's data as a float value
   */
  IMPORT_C float floatData() const;
  /**
   * Changes this node's data - converting the value to a string
   * @exception Symbian - a leaving function
   */
  IMPORT_C void	floatData(float);
#endif
  /**
   * adds the given node to the start of the child list for this node
   * @exception Symbian - a leaving function
   */
  IMPORT_C void  prependChild(skTreeNode*);
  /**
   * adds the given node to the end of the child list for this node
   * @exception Symbian - a leaving function
   */
  IMPORT_C void  addChild(skTreeNode*);
  /**
   * Assigns the given node to the first node with the matching label in the child list of this node. If no match is found, the node is added to the end of the list
   * @exception Symbian - a leaving function
   */
  IMPORT_C void  setChild(skTreeNode*);	
  /**
   * removes the given node from the list of children at this node
   */
  IMPORT_C void  removeChild(skTreeNode*);
  /**
   * deletes the given node from the list of children at this node
   */
  IMPORT_C void  deleteChild(skTreeNode*);
  /**
   * Returns true if the child list of this node contains the given node
   */
  IMPORT_C bool	containsChild(skTreeNode*);
  /**
   * This method searches for a node whose label matches the one given
   * @return the first match, or 0 if none found
   */
  IMPORT_C skTreeNode* findChild(const skString& label) const;
#ifdef __SYMBIAN32__
  /**
   * This method searches for a node whose label matches the one given
   * \remarks only available in Symbian version
   * @return the first match, or 0 if none found
   */
  IMPORT_C skTreeNode* findChild(const TDesC& label) const;
#endif
  /**
   * This method searches for a node whose label and data matches the ones given
   * @return the first match, or 0 if none found
   */
  IMPORT_C skTreeNode* findChild(const skString& label,const skString& data) const;
  /**
   * Finds the data associated with the first child whose label matches that given
   * @param label - the label to look for
   * @return the value of a matched child's data, or the default value
   */
  IMPORT_C skString findChildData(const skString& label) const;
  /**
   * Finds the data associated with the first child whose label matches that given
   * @param label - the label to look for
   * @param defaultVal - the value to return if a match is not found
   * @return the value of a matched child's data, or the default value
   */
  IMPORT_C skString findChildData(const skString& label,const skString& defaultVal) const;
#ifdef __SYMBIAN32__
  /**
   * Finds the data associated with the first child whose label matches that given
   * \remarks only available in Symbian version
   * @param label - the label to look for
   * @return the value of a matched child's data, or the default value
   */
  IMPORT_C skString findChildData(const TDesC& label) const;
#endif
  /**
   * Finds the data associated with the first child whose label matches that given as a boolean
   * @param label - the label to look for
   * @param defaultVal - the value to return if a match is not found
   * @return the value of a matched child's data, or the default value
   */
  IMPORT_C bool	findChildboolData(const skString& label, bool defaultVal=false) const;
  /**
   * Finds the data associated with the first child whose label matches that given as an integer
   * @param label - the label to look for
   * @param defaultVal - the value to return if a match is not found
   * @return the value of a matched child's data, or the default value
   */
  IMPORT_C int	findChildIntData(const skString& label, int defaultVal=0) const;
#ifdef __SYMBIAN32__
  /**
   * Finds the data associated with the first child whose label matches that given as an integer
   * \remarks only available in Symbian version
   * @param label - the label to look for
   * @param defaultVal - the value to return if a match is not found
   * @return the value of a matched child's data, or the default value
   */
  IMPORT_C int	findChildIntData(const TDesC& label, int defaultVal=0) const;
#endif
#ifdef USE_FLOATING_POINT
  /**
   * Finds the data associated with the first child whose label matches that given as a float
   * @param label - the label to look for
   * @param defaultVal - the value to return if a match is not found
   * @return the value of a matched child's data, or the default value
   */
  IMPORT_C float findChildFloatData(const skString& label, float defaultVal=0.0f) const;
#endif
  /**
   * Returns the data for nth child in the list of children at this node
   */
  IMPORT_C skString nthChildData(USize index) const;
  /**
   * Returns the data for nth child in the list of children at this node as an integer
   */
  IMPORT_C int	nthChildIntData(USize index) const;
  /**
   * Writes this node to an output destination with the given indentation
   * @param out the destination to write to
   * @param tabstops the current tab indentation level
   * @param include_tabs set to false to exclude indentation
   * @exception Symbian - a leaving function
   */
  IMPORT_C void  write(skOutputDestination& out,USize tabstops,bool include_tabs=true) const;
  /**
   * Writes this node out to a file
   * @return true if the file could be written, or false if there was a problem
   */
  IMPORT_C bool	write(const skString& file) const;
  /**
   * Returns a child from the list at this node
   * @param i - the index of the item
   * @return the child
   * @exception skBoundsException if the index is outside the range of the list
   */
  IMPORT_C skTreeNode *  nthChild(USize  i) const;
  /**
   * Returns the number of children at this node
   */
  IMPORT_C USize numChildren() const;
  /**
   * makes a deep copy of the items from the other node, but does not change the label or data of this node
   */
  IMPORT_C void	copyItems(skTreeNode& node);
  /**
   * Moves the items from the child list of the given node into this node
   */
  IMPORT_C void	moveItemsFrom(skTreeNode& node);
  /**
   * deletes all children from the list
   */
  IMPORT_C void	clear();
  /**
   * Reads a treenode from the given file
   * @param file filename of file containing TreeNode information
   * @param ctxt context object to receive errors
   * @exception skTreeNodeReaderException if there was an error in the file
   * @exception Symbian - a leaving function
   */
  static IMPORT_C skTreeNode * read(const skString& file,skExecutableContext& ctxt);
  
 private:
  /**
   * Writes tabs out
   */
  static void writeTabs(skOutputDestination& out,int tabstops);
#ifdef __SYMBIAN32__
  /**
   * Copy Constructor - private, prevents copying
   * \remarks only available in Symbian version
   */
  skTreeNode(const skTreeNode& );
#endif
  /**
   * the label for this node
   */
  skString m_Label;
  /**
   * the data for this node
   */
  skString m_Data;
  /**
   * this children of this node
   */
  skTreeNodeList * m_Items;
};
/**
 * This class provides an iterator over the children of a {@link skTreeNode skTreeNode}
 */
class  CLASSEXPORT skTreeNodeListIterator 
#ifdef __SYMBIAN32__
: public CBase
#endif
{
 public:
  /**
   * Constructor - creates an iterator for the given treenode
   */
  IMPORT_C skTreeNodeListIterator(const skTreeNode&);
  /**
   * Default constructor
   */
  IMPORT_C virtual ~skTreeNodeListIterator();
  /**
   * returns the next child of the treenode
   */
  IMPORT_C skTreeNode * operator ()();
  /**
   * resets the iteration to the start of the list of children in the treenode
   */
  IMPORT_C void reset();
 private:
  const skTreeNode& m_Node;
  USize	m_Index;
};
/**
 * this class is used to read a {@link skTreeNode skTreeNode} from a text stream.
 */
// switches on the optimization where a shared class buffer is used
#ifndef __SYMBIAN32__
#define USECLASSBUFFER
#endif
const int MAXBUFFER=20000;
class  CLASSEXPORT skTreeNodeReader 
#ifdef __SYMBIAN32__
: public CBase
#endif
{
 public:
   IMPORT_C skTreeNodeReader();
   /**
    * Constructs a TreeNodeReader to read a Treenode
    */
   IMPORT_C skTreeNodeReader(skInputSource& in, const skString& source_name);
  /**
   * Destructor
   */
  IMPORT_C virtual ~skTreeNodeReader();
  /**
   * Starts the parse of the input stream
   * @param context context object to receive errors
   * @exception skTreeNodeReaderException if there was an error in the file
   * @return a TreeNode, if the read was successful, or null, otherwise
   * @exception Symbian - a leaving function
   */
  IMPORT_C skTreeNode* read(skExecutableContext& ctxt);
#ifdef __SYMBIAN32__
  /**
   * Starts the parse of the input stream
   * \remarks only available in Symbian version
   * @param context context object to receive errors
   * @exception skTreeNodeReaderException if there was an error in the file
   * @return a TreeNode, if the read was successful, or null, otherwise
   * @exception Symbian - a leaving function
   */
  IMPORT_C skTreeNode* read(skInputSource * in, const TDesC& source_name,skExecutableContext& ctxt);
#endif
#ifdef __SYMBIAN32__
  /**
   * Conversion operator to Symbian TCleanupItem. This is provided to allow this object to be pushed by value onto the 
   * Symbian Cleanup Stack
   * \remarks only available in Symbian version
   */
  inline operator TCleanupItem();
  /**
   * Called via Symbian CleanupStack in the event of a leave. This calls the clear method in the object
   * \remarks only available in Symbian version
   */
  inline static void Cleanup(TAny * s);
#endif
  /**
   * Clears the contained buffers
   */
  IMPORT_C void clear();
 private:
  enum Lexeme		{ L_IDENT, L_TEXT, L_LBRACE, L_RBRACE, L_EOF, L_ERROR };
  /** this method sets up the LexText buffer 
   * @exception Symbian - a leaving function
   */
  void grabBuffer();
  /** this method sets the error flag */
  void error(const skString& msg);
  /** this method sets the error flag */
  void error(const Char * msg);
  /** this method returns the next lexical token from the current stream
   */
  Lexeme lex();
  /** this method puts the current lexical token back */
  void 	unLex();
  /** this method parses an entire tree node 
   * @param pparent - the parent of the node
   * @return a new tree node added to the parent, or 0 if there was a parsing error
   * @exception Symbian - a leaving function
   */
  skTreeNode * 	parseTreeNode(skTreeNode * pparent);
  /** This method parses a list of treenodes
   * @param list - the list to add children to as they are parsed
   * @exception Symbian - a leaving function
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
  /** the line number of the current input stream position */
  unsigned short m_LineNum;
  /** the position within the current lex text */
  unsigned short m_Pos;
  /** the input source */
  skInputSource * m_In;
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

/**
 * this class encapsulates an error encountered while parsing a {@link skTreeNode skTreeNode} text stream
 */
class CLASSEXPORT  skTreeNodeReaderException : public skException
{
 public:
  /**
   * Constructor - takes information about the exception
   */
  skTreeNodeReaderException(const skString& fileName,const skString& msg)
    : m_FileName(fileName),m_Msg(msg){
  }
  /**
   * Returns a string describing the exception which includes location information
   */
  skString getMessage() const{
    return toString();
  }
  /**
   * Returns a string describing the exception which includes location information
   */
  skString toString() const{
    return skString::addStrings(m_FileName.ptr(),s_colon,m_Msg.ptr());
  }
 private:
  skString m_FileName;
  skString m_Msg;
};
#include "skAlist.h"
#ifdef INSTANTIATE_TEMPLATES
EXTERN_TEMPLATE template class CLASSEXPORT skTAList<skTreeNode>;
#endif

/**
 * This class represents a list of  {@link skTreeNode skTreeNodes}
 */
class  CLASSEXPORT skTreeNodeList :  public skTAList<skTreeNode>
{           
 public:
  /** Constructs a blank list */
  IMPORT_C skTreeNodeList();
#ifndef __SYMBIAN32__
  /** Creates a list which is a deep copy of another list
   * @param list - list to copy
   */
  skTreeNodeList(const skTreeNodeList& list);
#endif
  /** Destroys list and its children */
  IMPORT_C virtual ~skTreeNodeList();
  /** Searches for a child item with the given label
   * @param label - the label of the child to look for
   * @return the first matching child - or 0 if not found
   */
  IMPORT_C skTreeNode * findItem(const skString& label) const;
  /** Searches for a child item with the given label and data
   * @param label - the label of the child to look for
   * @param data - the data of the child to look for
   * @return the first matching child - or 0 if not found
   */
  IMPORT_C skTreeNode * findItem(const skString& label,const skString& data) const;
  /** Returns the nth child from this list
   * @param i - the zero-based index of the item to retrieve
   * @return the nth child
   */
  IMPORT_C skTreeNode * nthElt(USize  i) const;
  /** This method performs a deep copy from another list
   * @param list - the other list to copy
   * @exception Symbian - a leaving function
   */
  IMPORT_C skTreeNodeList&   operator=(const skTreeNodeList& list);
};

//-----------------------------------------------------------------
inline skTreeNode::skTreeNode()
//-----------------------------------------------------------------
  : m_Items(0)
{
}
//-----------------------------------------------------------------
inline skTreeNode::skTreeNode(const skString& label)
  //-----------------------------------------------------------------
  : m_Label(label),m_Items(0)
{
}
//-----------------------------------------------------------------
inline skTreeNode::skTreeNode(const skString& label,const skString&  data)
  //-----------------------------------------------------------------
  : m_Label(label),m_Data(data),m_Items(0)
{
}
//-----------------------------------------------------------------
inline skString skTreeNode::label() const
  //-----------------------------------------------------------------
{
  return m_Label;
}
//-----------------------------------------------------------------
inline void skTreeNode::label(const skString& s) 
  //-----------------------------------------------------------------
{
  m_Label=s;
}
//-----------------------------------------------------------------
inline void skTreeNode::label(const Char * s) 
  //-----------------------------------------------------------------
{
  m_Label=s;
}
//-----------------------------------------------------------------
inline skString skTreeNode::data() const
//-----------------------------------------------------------------
{
  return m_Data;
}
//-----------------------------------------------------------------
inline void skTreeNode::data(const skString& t) 
//-----------------------------------------------------------------
{
  m_Data=t;
}
//-----------------------------------------------------------------
inline void skTreeNode::data(const Char * t) 
//-----------------------------------------------------------------
{
  m_Data=t;
}
#ifdef __SYMBIAN32__
//-------------------------------------------------------------------------
inline skTreeNodeReader::operator TCleanupItem()
  //-------------------------------------------------------------------------
  // used for leave support of local variable lists
{
  return TCleanupItem(Cleanup,this);
}
//-------------------------------------------------------------------------
inline void skTreeNodeReader::Cleanup(TAny * s)
  //-------------------------------------------------------------------------
  // called in case there is a leave
{
  ((skTreeNodeReader *)s)->clear();
}
#endif
//-----------------------------------------------------------------
inline skTreeNodeList::skTreeNodeList()
//-----------------------------------------------------------------
{
}

#endif
			








