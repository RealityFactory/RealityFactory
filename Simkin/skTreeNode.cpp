/*
  copyright 1996-2003
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

  $Id: skTreeNode.cpp,v 1.43 2003/11/20 16:24:22 sdw Exp $
*/

#include <string.h>
#include "skTreeNode.h"
#include <stdlib.h>
#include <stdio.h>
#include "skOutputDestination.h"
#ifdef STREAMS_ENABLED
#ifdef STL_STREAMS
#include <fstream>
using std::ofstream;
#else
#include <fstream.h>
#endif
#endif
#include <ctype.h>
#include "skExecutableContext.h"
#include "skInputSource.h"
#include "skConstants.h"


#ifndef __SYMBIAN32__
//-----------------------------------------------------------------
skTreeNode::skTreeNode(const skTreeNode& node)
  //-----------------------------------------------------------------
  : m_Label(node.m_Label),m_Data(node.m_Data),m_Items(0)
{
  if (node.m_Items)
    m_Items=skNEW(skTreeNodeList(*node.m_Items));
}
#endif
//-----------------------------------------------------------------
EXPORT_C skTreeNode::skTreeNode(const skString& label,bool data)
  //-----------------------------------------------------------------
  : m_Label(label),m_Items(0)
{
  if (data==true)
    m_Data=s_true;
  else
    m_Data=s_false;
}
//-----------------------------------------------------------------
EXPORT_C skTreeNode::skTreeNode(const skString& label,int data)
  //-----------------------------------------------------------------
  : m_Label(label),m_Items(0)
{
  m_Data=skString::from(data);
}
#ifdef USE_FLOATING_POINT
//-----------------------------------------------------------------
EXPORT_C skTreeNode::skTreeNode(const skString& label,float data)
  //-----------------------------------------------------------------
  : m_Label(label),m_Items(0)
{
  m_Data=skString::from(data);
}
#endif
//-----------------------------------------------------------------
skTreeNode::~skTreeNode()
  //-----------------------------------------------------------------
{
  delete m_Items;
}
//-----------------------------------------------------------------
EXPORT_C skTreeNode& skTreeNode::operator=(const skTreeNode& node)
  //-----------------------------------------------------------------
{
  if(&node!=this){
    m_Label=node.m_Label;
    m_Data=node.m_Data;
    delete m_Items;
    m_Items=0;
    if (node.m_Items){
      m_Items=skNEW(skTreeNodeList);
      *m_Items=*node.m_Items;
    }
  }
  return * this;
}
//-----------------------------------------------------------------
EXPORT_C void skTreeNode::boolData(bool b) 
  //-----------------------------------------------------------------
{
  if (b==true)
    m_Data=s_true;
  else
    m_Data=s_false;
}
//-----------------------------------------------------------------
EXPORT_C void skTreeNode::intData(int i) 
  //-----------------------------------------------------------------
{
  m_Data=skString::from(i);
}
#ifdef USE_FLOATING_POINT
//-----------------------------------------------------------------
EXPORT_C void skTreeNode::floatData(float f)
  //-----------------------------------------------------------------
{
  m_Data=skString::from(f);
}
#endif
//-----------------------------------------------------------------
EXPORT_C bool skTreeNode::boolData() const
  //-----------------------------------------------------------------
{
  return (bool)(m_Data==s_true);
}
//-----------------------------------------------------------------
EXPORT_C void skTreeNode::clear()
  //-----------------------------------------------------------------
{
  if (m_Items)
    m_Items->clear();
}
//-----------------------------------------------------------------
EXPORT_C void skTreeNode::copyItems(skTreeNode& node)
  //-----------------------------------------------------------------
{
  delete m_Items;
  m_Items=0;
  if (node.m_Items){
    m_Items=skNEW(skTreeNodeList);
    *m_Items=*node.m_Items;
  }
}
//-----------------------------------------------------------------
EXPORT_C void skTreeNode::moveItemsFrom(skTreeNode& node)
  //-----------------------------------------------------------------
{
  if (m_Items)
    m_Items->clearAndDestroy();
  else
    m_Items=skNEW(skTreeNodeList);
  skTreeNodeListIterator iter(node);
  skTreeNode * item=0;
  while ((item=iter()))
    m_Items->append(item);
  // make sure these items are not stored twice
  node.m_Items->clear();
}
//-----------------------------------------------------------------
EXPORT_C USize skTreeNode::numChildren() const
  //-----------------------------------------------------------------
{
  USize entries=0;
  if (m_Items)
    entries=m_Items->entries();
  return entries;
}
//-----------------------------------------------------------------
EXPORT_C skTreeNode * skTreeNode::nthChild(USize  i) const
  //-----------------------------------------------------------------
{
  skTreeNode * node=0;
  if (m_Items)
    node=(*m_Items)[i];
  return node;
}
//-----------------------------------------------------------------
EXPORT_C void skTreeNode::setChild(skTreeNode* child)
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
EXPORT_C void skTreeNode::prependChild(skTreeNode* child)
  //-----------------------------------------------------------------
{              
  assert(child!=this);
  if (child!=this){
    if (m_Items==0)
      m_Items=skNEW(skTreeNodeList);
    m_Items->prepend(child);
  }
}
//-----------------------------------------------------------------
EXPORT_C void skTreeNode::addChild(skTreeNode* child)
  //-----------------------------------------------------------------
{              
  assert(child!=this);
  if (child!=this){
    if (m_Items==0)
      m_Items=skNEW(skTreeNodeList);
    m_Items->append(child);
  }
}
//-----------------------------------------------------------------
EXPORT_C void skTreeNode::removeChild(skTreeNode* child)
  //-----------------------------------------------------------------
{
  if (m_Items){
    m_Items->remove(child);
  }else{
    assert("removeChild: error, node has no children!"==0);
  }
}
//-----------------------------------------------------------------
EXPORT_C void skTreeNode::deleteChild(skTreeNode* child)
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
EXPORT_C int  skTreeNode::intData() const
  //-----------------------------------------------------------------
{
  return m_Data.to();
}
#ifdef USE_FLOATING_POINT
//-----------------------------------------------------------------
EXPORT_C float skTreeNode::floatData() const
  //-----------------------------------------------------------------
{
  return m_Data.toFloat();
}
#endif
//-----------------------------------------------------------------
EXPORT_C skString skTreeNode::nthChildData(USize index) const
  //-----------------------------------------------------------------
{
  skTreeNode * pchild=nthChild(index);
  if (pchild)
    return pchild->data();
  else
    return skString(); 
}
//-----------------------------------------------------------------
EXPORT_C int skTreeNode::nthChildIntData(USize index) const
  //-----------------------------------------------------------------
{
  skTreeNode * pchild=nthChild(index);
  if (pchild)
    return pchild->intData();
  else
    return 0;   
}
//-----------------------------------------------------------------
EXPORT_C skString skTreeNode::findChildData(const skString& label, const skString& defaultVal) const
  //-----------------------------------------------------------------
{             
  skString s = defaultVal;
  skTreeNode * pchild=findChild(label);
  if (pchild)
    s=pchild->data();
  return s;
}
//-----------------------------------------------------------------
EXPORT_C skString skTreeNode::findChildData(const skString& label) const
  //-----------------------------------------------------------------
{             
  skString s;
  skTreeNode * pchild=findChild(label);
  if (pchild)
    s=pchild->data();
  return s;
}
#ifdef __SYMBIAN32__
//-----------------------------------------------------------------
EXPORT_C skString skTreeNode::findChildData(const TDesC& label) const
  //-----------------------------------------------------------------
{             
  skString s_label;
  s_label=label;
  return findChildData(s_label);
}
#endif
//-----------------------------------------------------------------
EXPORT_C bool skTreeNode::findChildboolData(const skString& label,bool defaultVal) const
  //-----------------------------------------------------------------
{               
  bool ret=defaultVal;
  skTreeNode * pchild=findChild(label);
  if (pchild)
    ret=pchild->boolData();
  return ret;   
}
//-----------------------------------------------------------------
EXPORT_C int skTreeNode::findChildIntData(const skString& label,int defaultVal) const
  //-----------------------------------------------------------------
{               
  int ret=defaultVal;
  skTreeNode * pchild=findChild(label);
  if (pchild)
    ret=pchild->intData();
  return ret;   
}
#ifdef __SYMBIAN32__
//-----------------------------------------------------------------
EXPORT_C int skTreeNode::findChildIntData(const TDesC& label,int defaultVal) const
  //-----------------------------------------------------------------
{               
  skString s_label;
  s_label=label;
  return findChildIntData(s_label,defaultVal);
}
#endif
#ifdef USE_FLOATING_POINT
//-----------------------------------------------------------------
EXPORT_C float skTreeNode::findChildFloatData(const skString& label,float defaultVal) const
  //-----------------------------------------------------------------
{               
  float ret=defaultVal;
  skTreeNode * pchild=findChild(label);
  if (pchild)
    ret=pchild->floatData();
  return ret;   
}
#endif
//-----------------------------------------------------------------
EXPORT_C skTreeNode* skTreeNode::findChild(const skString& label) const
  //-----------------------------------------------------------------
{                   
  skTreeNode * node=0;
  if (m_Items)
    node=m_Items->findItem(label);
  return node;
}
#ifdef __SYMBIAN32__
//-----------------------------------------------------------------
EXPORT_C skTreeNode* skTreeNode::findChild(const TDesC& label) const
//-----------------------------------------------------------------
{                   
  skString s_label;
  s_label=label;
  return findChild(s_label);
}
#endif
//-----------------------------------------------------------------
EXPORT_C skTreeNode* skTreeNode::findChild(const skString& label,const skString& data) const
  //-----------------------------------------------------------------
{
  skTreeNode * node=0;
  if (m_Items)
    node=m_Items->findItem(label,data);
  return node;
}
//-----------------------------------------------------------------
void skTreeNode::writeTabs(skOutputDestination& out,int tabstops)
  //-----------------------------------------------------------------
{
  for (int i=0;i<tabstops;i++)
    out.write(s_tab);
}
//-----------------------------------------------------------------
EXPORT_C bool skTreeNode::write(const skString& filename) const
  //-----------------------------------------------------------------
{   
  bool bRet=true;
  skOutputFile o(filename);
  write(o,0);
  return bRet;  
}
//-----------------------------------------------------------------
EXPORT_C void skTreeNode::write(skOutputDestination& out,USize tabstops,bool include_tabs) const
  //-----------------------------------------------------------------
{
  if (include_tabs)
    writeTabs(out,tabstops);
  out.write(m_Label);
  skString data=m_Data;
  if (data.length()){
    SAVE_VARIABLE(data);
    // make sure the special characters \ [ and ] are escaped
    if (data.indexOf('\\')!=-1)
      data=m_Data.replace(skSTR("\\"),skSTR("\\\\"));
    if (data.indexOf(']')!=-1)
      data=data.replace(skSTR("]"),skSTR("\\]"));
    if (data.indexOf('[')!=-1)
      data=data.replace(skSTR("["),skSTR("\\["));
    out.write(s_left_bracket);
    out.write(data);
    out.write(s_right_bracket);
    RELEASE_VARIABLE(data);
  }
  if (numChildren()){
    out.write(s_cr);
    if (include_tabs)
      writeTabs(out,tabstops);
    out.write(s_left_brace);
    skTreeNodeListIterator iter(*this);
    skTreeNode * pnode=0;
    while ((pnode=iter())!=0)
      pnode->write(out,tabstops+1,include_tabs);
    if (include_tabs)
      writeTabs(out,tabstops);
    out.write(s_right_brace);
  }
  out.write(s_cr);
}
//-----------------------------------------------------------------
EXPORT_C skTreeNode * skTreeNode::read(const skString& filename,skExecutableContext& ctxt)
  //-----------------------------------------------------------------
{
  skInputFile in;
  in.open(filename);
  skTreeNodeReader r(in,filename);
  SAVE_VARIABLE(r);
  skTreeNode * node=r.read(ctxt);
  RELEASE_VARIABLE(r);
  return node;
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
//-------------------------------------------------------------------------
EXPORT_C bool skTreeNode::operator==(const skTreeNode& other) const
//-------------------------------------------------------------------------
{
  // don't check the label
  return deepCompare(other,false);
}
//-------------------------------------------------------------------------
EXPORT_C bool skTreeNode::deepCompare(const skTreeNode& other,bool check_tagname) const
//-------------------------------------------------------------------------
{
  bool equals=false;
  if (check_tagname==false || m_Label==other.m_Label){
    if (m_Items && other.m_Items && m_Items->entries()==other.m_Items->entries()){
      equals=true;
      for (USize i=0;i<m_Items->entries();i++){
        if ((*m_Items)[i]->deepCompare(*((*other.m_Items)[i]),true)==false){
          equals=false;
          break;
        }
      }
    }else{
      if (m_Items==0 && other.m_Items==0)
        equals=true;
    }
  }
  return equals;
}
//-----------------------------------------------------------------
EXPORT_C skTreeNodeListIterator::skTreeNodeListIterator(const skTreeNode& l)
  //-----------------------------------------------------------------
  : m_Node(l),m_Index(0)
{
}
//-----------------------------------------------------------------
EXPORT_C skTreeNodeListIterator::~skTreeNodeListIterator()
  //-----------------------------------------------------------------
{
}
//-----------------------------------------------------------------
EXPORT_C void skTreeNodeListIterator::reset()
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
#ifndef __SYMBIAN32__
//-----------------------------------------------------------------
skTreeNodeList::skTreeNodeList(const skTreeNodeList& l)
  //-----------------------------------------------------------------
{
  //    do a deep copy
  if (l.m_Entries){
    growTo(l.m_Entries);
    m_Entries=l.m_Entries;
    for (USize i=0;i<l.m_Entries;i++)
      m_Array[i]=skNEW(skTreeNode(*(skTreeNode*)l.m_Array[i]));
  }
}
#endif
//-----------------------------------------------------------------
EXPORT_C skTreeNodeList::~skTreeNodeList()
  //-----------------------------------------------------------------
{
  clearAndDestroy();
}   
//-----------------------------------------------------------------
EXPORT_C skTreeNode * skTreeNodeList::findItem(const skString& label) const
  //-----------------------------------------------------------------
{                                                  
  skTreeNode * pnode=0;
  USize i;
  for (i=0;i<m_Entries;i++){
    pnode=pnode=(skTreeNode *)m_Array[i];
    if (pnode->label()==label)
      break;
  }
  if (i==m_Entries)
    pnode=0;
  return pnode;
}
//-----------------------------------------------------------------
EXPORT_C skTreeNode * skTreeNodeList::findItem(const skString& label,const skString& data) const
  //-----------------------------------------------------------------
{
  skTreeNode * pnode=0;
  USize i;
  for (i=0;i<m_Entries;i++){
    pnode=pnode=(skTreeNode *)m_Array[i];
    if (pnode->label()==label && pnode->data()==data)
      break;
  }
  if (i==m_Entries)
    pnode=0;
  return pnode;
}
//-----------------------------------------------------------------
EXPORT_C skTreeNodeList& skTreeNodeList::operator=(const skTreeNodeList& l)
  //-----------------------------------------------------------------
{
  if (&l!=this){
    clearAndDestroy();
    //  do a deep copy
    if (l.m_Entries){
      growTo(l.m_Entries);
      m_Entries=l.m_Entries;
      for (USize i=0;i<l.m_Entries;i++){
#ifdef __SYMBIAN32__
        skTreeNode * old_node=(skTreeNode*)l.m_Array[i];
        skTreeNode * new_node=skNEW(skTreeNode(old_node->label(),old_node->data()));
        new_node->copyItems(*old_node);
        m_Array[i]=new_node;
#else
        m_Array[i]=skNEW(skTreeNode(*(skTreeNode*)l.m_Array[i]));
#endif
      }
    }
  }
  return *this; 
}
//-----------------------------------------------------------------
EXPORT_C skTreeNode * skTreeNodeList::nthElt(USize  i) const
  //-----------------------------------------------------------------
{
  return operator[](i);
}
#ifdef USECLASSBUFFER
Char skTreeNodeReader::g_ClassLexText[MAXBUFFER];
bool skTreeNodeReader::g_LexTextUsed=false;
//      an optimization reusing the same buffer, but costs extra memory
#endif
//-----------------------------------------------------------------
EXPORT_C skTreeNodeReader::skTreeNodeReader()
  //-----------------------------------------------------------------
  : m_UnLex(false),m_LastLexeme(L_EOF),m_LexText(0),m_LineNum(0),m_Pos(0),m_In(0),m_Error(false)
#ifdef USECLASSBUFFER
  , m_UsingClassLexText(false)
#endif
{
}
//-----------------------------------------------------------------
EXPORT_C skTreeNodeReader::skTreeNodeReader(skInputSource& in, const skString& source_name)
  //-----------------------------------------------------------------
  : m_FileName(source_name),m_UnLex(false),m_LastLexeme(L_EOF),m_LexText(0),m_LineNum(0),m_Pos(0),m_In(&in),m_Error(false)
#ifdef USECLASSBUFFER
  , m_UsingClassLexText(false)
#endif
{
}
//-----------------------------------------------------------------
EXPORT_C skTreeNodeReader::~skTreeNodeReader()
  //-----------------------------------------------------------------
{
  clear();
}
//-----------------------------------------------------------------
EXPORT_C void skTreeNodeReader::clear()
  //-----------------------------------------------------------------
{
#ifdef USECLASSBUFFER
  if (m_UsingClassLexText==false)
    delete [] m_LexText;
  else  
    skTreeNodeReader::g_LexTextUsed=false;
#else
  if (m_LexText)
    delete [] m_LexText;
  m_LexText=0;
#endif
}
//-----------------------------------------------------------------
void skTreeNodeReader::grabBuffer()
  //-----------------------------------------------------------------
{
#ifdef USECLASSBUFFER
  if (skTreeNodeReader::g_LexTextUsed==false){
    skTreeNodeReader::g_LexTextUsed=true;
    m_UsingClassLexText=true;
    m_LexText=g_ClassLexText;
  }else
#endif
    m_LexText=skARRAY_NEW(Char,MAXBUFFER);
}
//-----------------------------------------------------------------
void skTreeNodeReader::addToLexText(Char c)
  //-----------------------------------------------------------------
{
  if (m_Pos<MAXBUFFER-1)
    m_LexText[m_Pos++]=c;
}
//-----------------------------------------------------------------
void skTreeNodeReader::unLex()
  //-----------------------------------------------------------------
{
  m_UnLex=true;
}
//-----------------------------------------------------------------
void skTreeNodeReader::error(const skString& msg)
  //-----------------------------------------------------------------
{
  m_Error=true;
  // cut the skString down a bit
  m_LexText[50]=0;
}
//-----------------------------------------------------------------
void skTreeNodeReader::error(const Char * msg)
  //-----------------------------------------------------------------
{
  m_Error=true;
  // cut the skString down a bit
  m_LexText[50]=0;
}
#ifdef __SYMBIAN32__
//-----------------------------------------------------------------
EXPORT_C skTreeNode* skTreeNodeReader::read(skInputSource * in, const TDesC& source_name,skExecutableContext& ctxt)
//-----------------------------------------------------------------
{
  m_In=in;
  m_FileName=source_name;
  return read(ctxt);
}
#endif
//-----------------------------------------------------------------
EXPORT_C skTreeNode * skTreeNodeReader::read(skExecutableContext& ctxt)
  //-----------------------------------------------------------------
{       
  grabBuffer();
  skTreeNode * pret=0;
  m_LineNum=0;
  pret=parseTreeNode(0);

  if (m_Error){
    delete pret;
    pret=0;
#ifdef EXCEPTIONS_DEFINED
    throw skTreeNodeReaderException(m_FileName,m_LexText);
#else
   skString lex_text;
   lex_text=m_LexText;
   ctxt.getError().setError(skScriptError::TREENODEPARSE_ERROR,skNEW(skTreeNodeReaderException(m_FileName,lex_text)));
#endif
  }
  return pret;  
}
//-----------------------------------------------------------------
skTreeNode * skTreeNodeReader::parseTreeNode(skTreeNode * pparent)
  //-----------------------------------------------------------------
{                          
  skTreeNode * pnode=skNEW(skTreeNode);
  SAVE_POINTER(pnode);
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
  RELEASE_POINTER(pnode);
  return pnode;
}
//-----------------------------------------------------------------
void skTreeNodeReader::parseTreeNodeList(skTreeNode * pnode)
  //-----------------------------------------------------------------
{
  bool loop=true;
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
      loop=false;
    }
  }while (loop && m_Error==false);
}
//-----------------------------------------------------------------
skTreeNodeReader::Lexeme skTreeNodeReader::lex()
  //-----------------------------------------------------------------
{
  if (m_UnLex)
    m_UnLex=false;
  else{
    m_Pos=0;
    int c;
    bool loop=true;
    m_LastLexeme=L_EOF;
    while (loop && !m_In->eof() && m_Error==false){
      c=m_In->get();
      if (c=='\n')
        m_LineNum++;
      switch (c){
      case '{':
        addToLexText(c);
        m_LastLexeme=L_LBRACE;
        loop=false;
        break;
      case '}':
        addToLexText(c);
        m_LastLexeme=L_RBRACE;
        loop=false;
        break;
      case EOF:
        m_LastLexeme=L_EOF;
        loop=false;
        break;
      case '[':{
        int textloop=true;
        int num_braces=1;
        m_LastLexeme=L_TEXT;
        while(textloop && !m_In->eof()){
          c=m_In->get();
          switch(c){
          case EOF:
            m_LastLexeme=L_ERROR;
            error(skSTR("Expected EOF in text string"));
            textloop=false;
            break;
          case '\\':
            // let any character through
            c=m_In->get();
            addToLexText(c);
            break;
          case '[':
            num_braces++;
            addToLexText(c);
            break;
          case ']':
            num_braces--;
            if (num_braces==0)
              textloop=false;
            else
              addToLexText(c);
            break;
          default:
            addToLexText(c);
          }
        }
        loop=false;
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
        if (c==':' || c=='-' || c=='.' || c=='/' || c=='\\' || c=='_' || c=='~' || ISALNUM(c)){
          m_LastLexeme=L_IDENT;
          addToLexText(c);
          int identloop=true;
          while (identloop && !m_In->eof()){
            int peeked=m_In->peek();
            if (ISALNUM(peeked) || peeked=='\\' 
                || peeked=='_' || peeked=='~' || peeked=='-' 
                || peeked=='/' || peeked=='.' || peeked==':'){
              c=m_In->get();
              addToLexText(c);
            }else
              identloop=false;
          }
          loop=false;   
        }else
          if (!ISSPACE(c)){
            m_LastLexeme=L_ERROR;
            error(skSTR("Expected ~ _ or alpha numeric character"));
            loop=false; 
          }
      }
    }
    m_LexText[m_Pos]=0;
  }
  return m_LastLexeme;
}
