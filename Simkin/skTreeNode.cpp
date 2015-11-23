/*
  copyright 1996-2001
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

  $Id: skTreeNode.cpp,v 1.15 2001/11/22 11:13:21 sdw Exp $
*/

#include <string.h>
#include "skTreeNode.h"
#include "skTreeNodp.h"
#include <stdlib.h>
#include <stdio.h>
#ifdef STREAMS_ENABLED
#include <fstream.h>
#endif
#include <ctype.h>

static skString false_string=skSTR("false");
static skString true_string=skSTR("true");

//-----------------------------------------------------------------
skTreeNodeList::skTreeNodeList()
//-----------------------------------------------------------------
{
}
//-----------------------------------------------------------------
skTreeNodeList::skTreeNodeList(const skTreeNodeList& l)
//-----------------------------------------------------------------
{
  //	do a deep copy
  if (l.m_Entries){
    growTo(l.m_Entries);
    m_Entries=l.m_Entries;
    for (USize i=0;i<l.m_Entries;i++)
      m_Array[i]=new skTreeNode(*(skTreeNode*)l.m_Array[i]);
  }
}
//-----------------------------------------------------------------
skTreeNode * skTreeNodeList::findItem(const skString& label) const
//-----------------------------------------------------------------
{                                                  
  skTreeNode * pnode=0;
  USize i;
  for (i=0;i<m_Entries;i++){
    pnode=pnode=(skTreeNode *)m_Array[i];
    if (pnode->m_Label==label)
      break;
  }
  if (i==m_Entries)
    pnode=0;
  return pnode;
}
//-----------------------------------------------------------------
skTreeNode * skTreeNodeList::findItem(const skString& label,const skString& data) const
//-----------------------------------------------------------------
{
  skTreeNode * pnode=0;
  USize i;
  for (i=0;i<m_Entries;i++){
    pnode=pnode=(skTreeNode *)m_Array[i];
    if (pnode->m_Label==label && pnode->m_Data==data)
      break;
  }
  if (i==m_Entries)
    pnode=0;
  return pnode;
}
//-----------------------------------------------------------------
skTreeNodeList& skTreeNodeList::operator=(const skTreeNodeList& l)
//-----------------------------------------------------------------
{
  if (&l!=this){
    clearAndDestroy();
    //	do a deep copy
    if (l.m_Entries){
      growTo(l.m_Entries);
      m_Entries=l.m_Entries;
      for (USize i=0;i<l.m_Entries;i++)
      m_Array[i]=new skTreeNode(*(skTreeNode*)l.m_Array[i]);
    }
  }
  return *this;	
}
//-----------------------------------------------------------------
skTreeNode * skTreeNodeList::nthElt(USize  i) const
//-----------------------------------------------------------------
{
  return operator[](i);
}
//-----------------------------------------------------------------
skTreeNode::skTreeNode()
//-----------------------------------------------------------------
: m_Items(0)
{
}
//-----------------------------------------------------------------
skTreeNode::~skTreeNode()
//-----------------------------------------------------------------
{
  delete m_Items;
}
//-----------------------------------------------------------------
skTreeNode::skTreeNode(const skTreeNode& node)
//-----------------------------------------------------------------
: m_Label(node.m_Label),m_Data(node.m_Data),m_Items(0)
{
  if (node.m_Items)
    m_Items=new skTreeNodeList(*node.m_Items);
}
//-----------------------------------------------------------------
skTreeNode::skTreeNode(const skString& label)
//-----------------------------------------------------------------
: m_Label(label),m_Items(0)
{
}
//-----------------------------------------------------------------
skTreeNode::skTreeNode(const skString& label,const skString&  data)
//-----------------------------------------------------------------
: m_Label(label),m_Data(data),m_Items(0)
{
}
//-----------------------------------------------------------------
skTreeNode::skTreeNode(const skString& label,bool data)
//-----------------------------------------------------------------
: m_Label(label),m_Items(0)
{
  if (data==true)
    m_Data=true_string;
  else
    m_Data=false_string;
}
//-----------------------------------------------------------------
skTreeNode::skTreeNode(const skString& label,int data)
//-----------------------------------------------------------------
: m_Label(label),m_Items(0)
{
	m_Data=skString::from(data);
}
//-----------------------------------------------------------------
skTreeNode::skTreeNode(const skString& label,float data)
//-----------------------------------------------------------------
: m_Label(label),m_Items(0)
{
	m_Data=skString::from(data);
}
//-----------------------------------------------------------------
skTreeNode& skTreeNode::operator=(const skTreeNode& node)
//-----------------------------------------------------------------
{
  if(&node!=this){
    m_Label=node.m_Label;
    m_Data=node.m_Data;
    delete m_Items;
    m_Items=0;
    if (node.m_Items)
    m_Items=new skTreeNodeList(*node.m_Items);
  }
  return * this;
}
//-----------------------------------------------------------------
void skTreeNode::boolData(bool b) 
//-----------------------------------------------------------------
{
  if (b==true)
    m_Data=true_string;
  else
    m_Data=false_string;
}
//-----------------------------------------------------------------
void skTreeNode::intData(int i) 
//-----------------------------------------------------------------
{
    m_Data=skString::from(i);
}
//-----------------------------------------------------------------
void skTreeNode::floatData(float f)
//-----------------------------------------------------------------
{
    m_Data=skString::from(f);
}
//-----------------------------------------------------------------
bool skTreeNode::boolData() const
//-----------------------------------------------------------------
{
    return (bool)(m_Data==true_string);
}
//-----------------------------------------------------------------
void skTreeNode::clear()
//-----------------------------------------------------------------
{
  if (m_Items)
    m_Items->clear();
}
//-----------------------------------------------------------------
void skTreeNode::copyItems(skTreeNode& node)
//-----------------------------------------------------------------
{
  delete m_Items;
  m_Items=0;
  if (node.m_Items)
    m_Items=new skTreeNodeList(*node.m_Items);
}
//-----------------------------------------------------------------
void skTreeNode::moveItemsFrom(skTreeNode& node)
//-----------------------------------------------------------------
{
  if (m_Items)
    m_Items->clearAndDestroy();
  else
    m_Items=new skTreeNodeList;
  skTreeNodeListIterator iter(node);
  skTreeNode * item=0;
  while ((item=iter()))
    m_Items->append(item);
  // make sure these items are not stored twice
  node.m_Items->clear();
}
//-----------------------------------------------------------------
USize skTreeNode::numChildren() const
//-----------------------------------------------------------------
{
  USize entries=0;
  if (m_Items)
    entries=m_Items->entries();
  return entries;
}
//-----------------------------------------------------------------
skTreeNode * skTreeNode::nthChild(USize  i) const
//-----------------------------------------------------------------
{
  skTreeNode * node=0;
  if (m_Items)
    node=(*m_Items)[i];
  return node;
}
//-----------------------------------------------------------------
skTreeNodeListIterator::skTreeNodeListIterator(const skTreeNode& l)
//-----------------------------------------------------------------
: m_Node(l),m_Index(0)
{
}
//-----------------------------------------------------------------
skTreeNodeListIterator::~skTreeNodeListIterator()
//-----------------------------------------------------------------
{
}
//-----------------------------------------------------------------
void skTreeNodeListIterator::reset()
//-----------------------------------------------------------------
{
  m_Index=0;
}
//-----------------------------------------------------------------
skTreeNode * skTreeNodeListIterator::operator ()()
//-----------------------------------------------------------------
{
  skTreeNode * node=0;
  if (m_Index<m_Node.numChildren())
    node=m_Node.nthChild(m_Index++);
  return node;
}
//-----------------------------------------------------------------
skTreeNodeList::~skTreeNodeList()
//-----------------------------------------------------------------
{
  clearAndDestroy();
}   
//-----------------------------------------------------------------
skString skTreeNode::label() const
//-----------------------------------------------------------------
{
    return m_Label;
}
//-----------------------------------------------------------------
void skTreeNode::label(const skString& s) 
//-----------------------------------------------------------------
{
    m_Label=s;
}
//-----------------------------------------------------------------
skString skTreeNode::data() const
//-----------------------------------------------------------------
{
    return m_Data;
}
//-----------------------------------------------------------------
void skTreeNode::data(const skString& t) 
//-----------------------------------------------------------------
{
    m_Data=t;
}
//-----------------------------------------------------------------
void skTreeNode::setChild(skTreeNode* child)
//-----------------------------------------------------------------
{              
  skTreeNode * existing=findChild(child->label());
  if (existing){
    existing->data(child->data());
    existing->copyItems(*child);
    delete child;
  }else
    addChild(child);
}
//-----------------------------------------------------------------
void skTreeNode::prependChild(skTreeNode* child)
//-----------------------------------------------------------------
{              
  assert(child!=this);
  if (child!=this){
    if (m_Items==0)
      m_Items=new skTreeNodeList;
    m_Items->prepend(child);
  }
}
//-----------------------------------------------------------------
void skTreeNode::addChild(skTreeNode* child)
//-----------------------------------------------------------------
{              
  assert(child!=this);
  if (child!=this){
    if (m_Items==0)
      m_Items=new skTreeNodeList;
    m_Items->append(child);
  }
}
//-----------------------------------------------------------------
void skTreeNode::deleteChild(skTreeNode* child)
//-----------------------------------------------------------------
{
  if (m_Items){
    m_Items->remove(child);
    delete child;
  }else{
    assert("deleteChild: error, node has no children!"==0);
  }
}
//-----------------------------------------------------------------
int  skTreeNode::intData() const
//-----------------------------------------------------------------
{
	return m_Data.to();
}
//-----------------------------------------------------------------
float skTreeNode::floatData() const
//-----------------------------------------------------------------
{
	return m_Data.toFloat();
}
//-----------------------------------------------------------------
skString 	skTreeNode::nthChildData(USize index) const
//-----------------------------------------------------------------
{
  skTreeNode * pchild=nthChild(index);
  if (pchild)
    return pchild->data();
  else
    return skString(); 
}
//-----------------------------------------------------------------
int skTreeNode::nthChildIntData(USize index) const
//-----------------------------------------------------------------
{
  skTreeNode * pchild=nthChild(index);
  if (pchild)
    return pchild->intData();
  else
    return 0;	
}
//-----------------------------------------------------------------
skString skTreeNode::findChildData(const skString& label, const skString& defaultVal) const
//-----------------------------------------------------------------
{             
  skString s = defaultVal;
  skTreeNode * pchild=findChild(label);
  if (pchild)
    s=pchild->data();
  return s;
}
//-----------------------------------------------------------------
bool skTreeNode::findChildboolData(const skString& label,bool defaultVal) const
//-----------------------------------------------------------------
{               
  bool ret=defaultVal;
  skTreeNode * pchild=findChild(label);
  if (pchild)
    ret=pchild->boolData();
  return ret;	
}
//-----------------------------------------------------------------
int skTreeNode::findChildIntData(const skString& label,int defaultVal) const
//-----------------------------------------------------------------
{               
  int ret=defaultVal;
  skTreeNode * pchild=findChild(label);
  if (pchild)
    ret=pchild->intData();
  return ret;	
}
//-----------------------------------------------------------------
float skTreeNode::findChildFloatData(const skString& label,float defaultVal) const
//-----------------------------------------------------------------
{               
  float ret=defaultVal;
  skTreeNode * pchild=findChild(label);
  if (pchild)
    ret=pchild->floatData();
  return ret;	
}
//-----------------------------------------------------------------
skTreeNode* skTreeNode::findChild(const skString& label) const
//-----------------------------------------------------------------
{                   
  skTreeNode * node=0;
  if (m_Items)
    node=m_Items->findItem(label);
  return node;
}
//-----------------------------------------------------------------
skTreeNode* skTreeNode::findChild(const skString& label,const skString& data) const
//-----------------------------------------------------------------
{
  skTreeNode * node=0;
  if (m_Items)
    node=m_Items->findItem(label,data);
  return node;
}
#ifdef STREAMS_ENABLED
//-----------------------------------------------------------------
void writeTabs(ostream& out,int tabstops)
//-----------------------------------------------------------------
{
    for (int i=0;i<tabstops;i++)
        out << '\t';
}
#endif
//-----------------------------------------------------------------
bool skTreeNode::write(const skString& s) const
//-----------------------------------------------------------------
{   
  bool bRet=false;
#ifdef STREAMS_ENABLED
  ofstream o(s);
  if (o.good()){
    write(o,0);
    if (o.good())
      bRet=true;
  }
#endif
  return bRet;	
}
#ifdef STREAMS_ENABLED
//-----------------------------------------------------------------
void skTreeNode::write(ostream& out,USize tabstops) const
//-----------------------------------------------------------------
{

  writeTabs(out,tabstops);
  out << m_Label;
  const char * databuffer=m_Data;
  int datalen=strlen(databuffer);
  if (datalen)
	out << " [" << databuffer << "]";
  if (numChildren()){
    out << "\n";
    writeTabs(out,tabstops);
    out << "{\n";
    skTreeNodeListIterator iter(*this);
    skTreeNode * pnode=0;
    while ((pnode=iter())!=0)
      pnode->write(out,tabstops+1);
    writeTabs(out,tabstops);
    out << "}";
  }
  out << "\n";
}
#endif
//-----------------------------------------------------------------
skTreeNode * skTreeNode::read(const skString& s)
//-----------------------------------------------------------------
{
  skTreeNode * pnode=0;
#ifdef STREAMS_ENABLED
#ifdef __MWERKS__
  ifstream i(s,ios::in|ios::binary);
#else
  ifstream i(s,ios::in|ios::nocreate|ios::binary);
#endif
  if (i.good()){
    skTreeNodeReader r(i,s);
    pnode=r.read();
  }
#endif
  return pnode;	
}
#ifdef USECLASSBUFFER
Char P_TreeNodeReader::g_ClassLexText[MAXBUFFER];
bool	P_TreeNodeReader::g_LexTextUsed=false;
//	an optimization reusing the same buffer, but costs extra memory
#endif
#ifdef STREAMS_ENABLED
//-----------------------------------------------------------------
skTreeNodeReader::skTreeNodeReader(istream& in)
//-----------------------------------------------------------------
{
    pimp=new P_TreeNodeReader(in);
    pimp->m_Error=false;
    pimp->grabBuffer();
}
#endif
//-----------------------------------------------------------------
void P_TreeNodeReader::grabBuffer()
//-----------------------------------------------------------------
{
#ifdef USECLASSBUFFER
  if (P_TreeNodeReader::g_LexTextUsed==false){
    P_TreeNodeReader::g_LexTextUsed=true;
    m_UsingClassLexText=true;
    m_LexText=g_ClassLexText;
  }else
#endif
    m_LexText=new Char[MAXBUFFER];
}
#ifdef STREAMS_ENABLED
//-----------------------------------------------------------------
skTreeNodeReader::skTreeNodeReader(istream& in,const char * fileName)
//-----------------------------------------------------------------
{
  pimp=new P_TreeNodeReader(in);
  pimp->m_Error=false;
  pimp->m_FileName=fileName;
  pimp->grabBuffer();
}
#endif
//-----------------------------------------------------------------
skTreeNodeReader::~skTreeNodeReader()
//-----------------------------------------------------------------
{
#ifdef USECLASSBUFFER
  if (pimp->m_UsingClassLexText==false)
    delete [] pimp->m_LexText;
  else	
    P_TreeNodeReader::g_LexTextUsed=false;
#else
    delete [] pimp->m_LexText;
#endif
  delete pimp;
}
#ifdef STREAMS_ENABLED
//-----------------------------------------------------------------
P_TreeNodeReader::P_TreeNodeReader(istream& in)
//-----------------------------------------------------------------
: m_In(in),m_UnLex(false),m_LastLexeme(L_EOF),m_Pos(0)
#ifdef USECLASSBUFFER
, m_UsingClassLexText(false)
#endif
{
}
#endif
//-----------------------------------------------------------------
void P_TreeNodeReader::addToLexText(Char c)
//-----------------------------------------------------------------
{
    if (m_Pos<MAXBUFFER-1)
        m_LexText[m_Pos++]=c;
}
//-----------------------------------------------------------------
void P_TreeNodeReader::unLex()
//-----------------------------------------------------------------
{
    m_UnLex=true;
}
//-----------------------------------------------------------------
void P_TreeNodeReader::error(const skString& msg)
//-----------------------------------------------------------------
{
  m_Error=true;
  // cut the skString down a bit
  m_LexText[50]=0;
}
//-----------------------------------------------------------------
skTreeNode * skTreeNodeReader::read()
//-----------------------------------------------------------------
{       
  skTreeNode * pret=0;
  pimp->m_LineNum=0;
  pret=pimp->parseTreeNode(0);

  if (pimp->m_Error){
    delete pret;
    pret=0;
    THROW(skTreeNodeReaderException(pimp->m_FileName,pimp->m_LexText),skBoundsException_Code);
  }
  return pret;	
}
//-----------------------------------------------------------------
skTreeNode * P_TreeNodeReader::parseTreeNode(skTreeNode * pparent)
//-----------------------------------------------------------------
{                          
  skTreeNode * pnode=new skTreeNode;
  if (pparent)
    pparent->addChild(pnode);
  Lexeme lexeme=lex();
  switch(lexeme){
  case L_ERROR:
    break;
  case L_IDENT:
    pnode->label(m_LexText);
    lexeme=lex();
    switch (lexeme){
    case L_ERROR:
      break;
    case L_IDENT:
      unLex();
      break;
    case L_TEXT:
      pnode->data(m_LexText);
      lexeme=lex();
      switch (lexeme){
      case L_TEXT:
      case L_IDENT:
	unLex();
	break;
      case L_LBRACE:
	parseTreeNodeList(pnode);
	break;
      case L_RBRACE:
	if (pparent)
	  unLex();
	else
	  error(skSTR("Unexpected right brace parsing text after label - no parent node"));
	break;
      }
      break;
    case L_LBRACE:
      parseTreeNodeList(pnode);
      break;
    case L_RBRACE:
      if (pparent)
	unLex();
      else
	error(skSTR("Unexpected right brace parsing after label - no parent node"));
      break;
    }
    break;
  case L_TEXT:
    pnode->data(m_LexText);
    lexeme=lex();
    switch (lexeme){
    case L_ERROR:
      break;
    case L_IDENT:
    case L_TEXT:
      unLex();
      break;
    case L_LBRACE:
      parseTreeNodeList(pnode);
      break;
    case L_RBRACE:
      if (pparent)
	unLex();
      else
	error(skSTR("Unexpected right brace parsing text with no label - no parent node"));
      break;
    }
    break;
  case L_LBRACE:
    parseTreeNodeList(pnode);
    break;
  case L_RBRACE:
    if (pparent)
      unLex();
    else
      error(skSTR("Unexpected right brace parsing sublist with no label or text - no parent node"));
    break;
  }
  return pnode;
}
//-----------------------------------------------------------------
void P_TreeNodeReader::parseTreeNodeList(skTreeNode * pnode)
//-----------------------------------------------------------------
{
  int loop=1;
  do{
    Lexeme lexeme=lex();
    switch(lexeme){
    case L_IDENT:
    case L_TEXT:
    case L_LBRACE:
      unLex();
      parseTreeNode(pnode);
      break;
    case L_EOF:
      error(skSTR("Expected right brace parsing sub-list"));
    case L_ERROR:
    case L_RBRACE:
      loop=0;
    }
  }while (loop && m_Error==false);
}
//-----------------------------------------------------------------
bool P_TreeNodeReader::eof()
//-----------------------------------------------------------------
{
  bool eof=false;
#ifdef STREAMS_ENABLED
  if (m_In.eof())
	  eof=true;
#endif
  return eof;
}
//-----------------------------------------------------------------
int P_TreeNodeReader::get()
//-----------------------------------------------------------------
{
  int c=0;
#ifdef STREAMS_ENABLED
  c=m_In.get();
#endif
  return c;
}
//-----------------------------------------------------------------
int P_TreeNodeReader::peek()
//-----------------------------------------------------------------
{
  int c=0;
#ifdef STREAMS_ENABLED
  c=m_In.peek();
#endif
  return c;
}
//-----------------------------------------------------------------
P_TreeNodeReader::Lexeme P_TreeNodeReader::lex()
//-----------------------------------------------------------------
{
  if (m_UnLex)
    m_UnLex=false;
  else{
    m_Pos=0;
    int c;
    int loop=1;
    while (loop && !eof() && m_Error==false){
      c=get();
      if (c=='\n')
	m_LineNum++;
      switch (c){
      case '{':
	addToLexText((unsigned char)c);
	m_LastLexeme=L_LBRACE;
	loop=0;
	break;
      case '}':
	addToLexText((unsigned char)c);
	m_LastLexeme=L_RBRACE;
	loop=0;
	break;
      case EOF:
	m_LastLexeme=L_EOF;
	loop=0;
	break;
      case '[':{
	int textloop=1;
	int num_braces=1;
	m_LastLexeme=L_TEXT;
	while(textloop && !eof()){
	  c=get();
	  switch(c){
	  case EOF:
	    m_LastLexeme=L_ERROR;
	    error(skSTR("Expected EOF in text string"));
	    textloop=0;
	    break;
	  case '\\':
	    // let any character through
	    c=get();
	    addToLexText((unsigned char)c);
	    break;
	  case '[':
	    num_braces++;
	    addToLexText((unsigned char)c);
	    break;
	  case ']':
	    num_braces--;
	    if (num_braces==0)
	      textloop=0;
	    else
	      addToLexText((unsigned char)c);
	    break;
	  default:
	      addToLexText((unsigned char)c);
	  }
	}
	loop=0;
	break;	
      }
      case '/':
      case '.':
      case '\\':
      case '~':
      case '_':
      case '-':
      case ':':
      default:
	if (c==':' || c=='-' || c=='.' || c=='/' || c=='\\' || c=='_' || c=='~' || isalnum(c)){
	  m_LastLexeme=L_IDENT;
	  addToLexText((unsigned char)c);
	  int identloop=1;
	  while (identloop && !eof())
	    if (isalnum(peek()) || peek()=='\\' 
		|| peek()=='_' || peek()=='~' || peek()=='-' 
		|| peek()=='/' || peek()=='.' || peek()==':'){
	      c=get();
	      addToLexText((unsigned char)c);
	    }else
	      identloop=0;
	  loop=0;	
	}else
	  if (!isspace(c)){
	    m_LastLexeme=L_ERROR;
	    error(skSTR("Expected ~ _ or alpha numeric character"));
	    loop=0;	
	  }
      }
    }
    m_LexText[m_Pos]=0;
  }
  return m_LastLexeme;
}
//-----------------------------------------------------------------
bool skTreeNode::containsChild(skTreeNode* child)
//-----------------------------------------------------------------
{
  bool bRet=false;
  if (m_Items)
    bRet=m_Items->contains(child);
  return bRet;
}
