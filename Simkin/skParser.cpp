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

  $Id: skParser.cpp,v 1.29 2003/05/27 17:34:22 simkin_cvs Exp $
*/
#include "skParser.h"
#include "skParseNode.h"
#include <ctype.h>
#include "skTracer.h"
#include "skLang_tab.cpp"

extern int yyparse (void *);


skNAMED_LITERAL(QuoteStart,skSTR(" near \""));
skNAMED_LITERAL(QuoteEnd,skSTR("\""));

// define DEBUGLEXER to get a verbose output of the workings of the lexer
//#define _DEBUGLEXER
#ifdef _DEBUGLEXER
#define DB(t) skTracer::trace(skString::from(m_Pos)+s_colon+skSTR(t)+s_cr);
#define DBN(t,n) skTracer::trace(skString::from(m_Pos)+s_colon+skSTR(t)+s_cr+n+s_cr); 
#define DBI(t,n) skTracer::trace(skString::from(m_Pos)+s_colon+skSTR(t)+s_cr+skString::from(n)+s_cr); 
#define DBC(t,c) skTracer::trace(skString::from(m_Pos)+s_colon+skSTR(t)+s_cr+skString(c,1)+s_cr); 
#else
#define DB(t) 
#define DBN(t,n) 
#define DBI(t,n) 
#define DBC(t,c) 
#endif
// this file contains a gperf-generated lookup function which checks a string against
// a keyword list
#include "skKeywords.inl"

//------------------------------------------
inline int skParser::nextChar()
//------------------------------------------
{
  int c=0;
  if (m_PutBack){
    c=m_PutBack;
    m_PutBack=0;
  }else{
    c=m_InputBuffer.at(m_Pos++);
  }
  return c;
}
//------------------------------------------
inline void skParser::putbackchar(int i)
//------------------------------------------
{
  DBC("**putback",i);
  m_PutBack=i;
}
//------------------------------------------
inline bool skParser::eof()
//------------------------------------------
{
  bool e=false;
  if (!m_PutBack)
    e=(m_Pos==m_InputBuffer.length()+1);
#ifdef _DEBUG        
  if (e){
    DB("Eof");
  }
#endif          
  return e;  
}


//---------------------------------------------------
skParser::skParser(const skString& code,const skString& location)
//---------------------------------------------------
  : m_TopNode(0),m_LexBuffer(0),m_InputBuffer(code),m_PutBack(0),m_LineNum(0),m_Pos(0),m_Location(location)
{
}
//---------------------------------------------------
skParser::~skParser()
//---------------------------------------------------
{
  delete [] m_LexBuffer;
  cleanupTempNodes();
}
#ifdef EXECUTE_PARSENODES
//------------------------------------------
skString * skParser::lookupIdentifier(const Char * id)
//------------------------------------------
{
  skString * s=0;
  for (USize i=0;i<m_Identifiers.entries();i++)
    if (*m_Identifiers[i]==id){
      s=m_Identifiers[i];
      break;
    }
  // not found - add to the list
  if (s==0){
    s=skNEW(skString);
    SAVE_POINTER(s);
    *s=id;
    m_Identifiers.append(s);
    RELEASE_POINTER(s);
  }
  return s;
}
#else
//------------------------------------------
int skParser::lookupIdentifier(const Char * id)
//------------------------------------------
{
  int index=-1;
  for (USize i=0;i<m_Identifiers.entries();i++)
    if (m_Identifiers[i]==id){
      index=i;
      break;
    }
  // not found - add to the list
  if (index==-1){
    index=m_Identifiers.entries();
    skString s_id;
    SAVE_VARIABLE(s_id);
    s_id=id;
    m_Identifiers.append(s_id);
    RELEASE_VARIABLE(s_id);
  }
  return index;
}
#endif
//------------------------------------------
int skParser::lex(void * lvalp,void * llocp)
//------------------------------------------
  // Lexical Analyser
{
  YYSTYPE * yylval=(YYSTYPE *)lvalp;
  YYLTYPE * yylloc=(YYLTYPE *)llocp;
  int c=0;
  yylloc->first_line=m_LineNum;
  int yypos=0;
  m_LexBuffer[0]=0;
  while (!eof()){
    c=nextChar();
    if (c=='\n')
      m_LineNum++;
    // Whitespace----------------------------------------
    if (ISSPACE(c)){
      yylloc->first_line=m_LineNum;
      continue;
    }
    // NEQ -------------------------------------------
    if (c=='!'){
      int c1=nextChar();
      if (c1=='='){
        c=L_NEQ;
        break;
      }else
        putbackchar(c1);
    }
    // Identifier/Keyword--------------------------------
    if (ISALPHA(c) || c=='_' || c=='@' || c=='$'){
      do{
        if (yypos<MAXYYTEXT-2)
          m_LexBuffer[yypos++]=c;
        else
          break;
        c=nextChar();
      }while (!eof() && ( ISALNUM(c) || c=='_' || c=='@' || c=='$'));
      if (!eof())
        putbackchar(c);
      m_LexBuffer[yypos]=0;
      // use the gperf generated function to lookup the keyword
      const keyword * key=in_word_set(m_LexBuffer,yypos);
      if (key){
        c=key->value;
        yylval->keyword=key->name;
        DBN("keyword",*yylval->string);
      }else{
        // this is an identifier - look through the list of those we have already found to see if there is a match
        c=L_ID;
        yylval->idReference=lookupIdentifier(m_LexBuffer);
#ifdef EXECUTE_PARSENODES
        DBN("id",*yylval->idReference);
#else
        DBN("id",yylval->idReference);
#endif
      }
      break;
    }
    // character--------------------------------------------
    if (c=='\''){
      c=nextChar();
      if (c=='\''){
        // this copes with '' - a blank string
        m_LexBuffer[yypos]=0;
        c=L_STRING;
        skString * s=skNEW(skString);
        m_TempStringLiterals.append(s);
        *s=m_LexBuffer;
        yylval->string=s;
        DBN("string",*yylval->string);
        break;
      }
      if (c=='\\'){
        c=nextChar();
        if (c=='n')
          c='\n';
        else
          if (c=='t')
            c='\t';
      }
      int next_c=nextChar();
      if (next_c=='\''){
        // this copes with 'c' where c is a single character
        yylval->character=c;
        c=L_CHARACTER;
        DBC("character",yylval->character);
      }else{
        // for backwards compatibility cope with 'xxx' as a string
        m_LexBuffer[yypos++]=c;
        putbackchar(next_c);
        while (!eof()){
          c=nextChar();
          if (c=='\\'){
            c=nextChar();
            if (c=='n')
              c='\n';
            else
              if (c=='t')
                c='\t';
          }else
            if (c=='\'')
              break;
          if (yypos<MAXYYTEXT-2)
            m_LexBuffer[yypos++]=c;
          else
            break;
        }
        m_LexBuffer[yypos]=0;
        c=L_STRING;
        skString * s=skNEW(skString);
        m_TempStringLiterals.append(s);
        *s=m_LexBuffer;
        yylval->string=s;
        DBN("string",*yylval->string);
      }
      break;
    }
    // string--------------------------------------------
    if (c=='"'){
      while(!eof()){
        c=nextChar();
        if (c=='\\'){
          c=nextChar();
          if (c=='n')
            c='\n';
          else
            if (c=='t')
              c='\t';
        }else
          if (c=='"')
            break;
        if (yypos<MAXYYTEXT-2)
          m_LexBuffer[yypos++]=c;
        else
          break;
      }
      m_LexBuffer[yypos]=0;
      c=L_STRING;
      skString * s=skNEW(skString);
      m_TempStringLiterals.append(s);
      *s=m_LexBuffer;
      yylval->string=s;
      DBN("string",*yylval->string);
      break;
    }
    // Integer or float----------------------------------
    if (ISDIGIT(c)){
      enum {
        NUM_INTEGER,
#ifdef USE_FLOATING_POINT
        NUM_FRACTION,
        NUM_EXPONENT_SIGN,
        NUM_EXPONENT,
#endif
        NUM_END
      } state;
#ifdef USE_FLOATING_POINT
      bool floating=false;
#endif
      state = NUM_INTEGER;
      while (state != NUM_END){
        switch (state){
        case NUM_INTEGER:
          if (ISDIGIT(c))
            m_LexBuffer[yypos++]=c;
#ifndef USE_FLOATING_POINT
          else
            state=NUM_END;
          break;
#else
          else if (c == '.'){
            m_LexBuffer[yypos++]=c;
            state = NUM_FRACTION;
            floating=true;
          } else if (c == 'e' || c == 'E'){
            floating=true;
            m_LexBuffer[yypos++]=c;
            state = NUM_EXPONENT_SIGN;
          } else
            state = NUM_END;
          break;
        case NUM_FRACTION:
          if (ISDIGIT(c))
            m_LexBuffer[yypos++]=c;
          else if (c == 'e' || c == 'E'){
            m_LexBuffer[yypos++]=c;
            state = NUM_EXPONENT_SIGN;
          } else
            state = NUM_END;
          break;
        case NUM_EXPONENT_SIGN:
          if (c == '+' || c == '-' || ISDIGIT(c)){
            m_LexBuffer[yypos++]=c;
            state = NUM_EXPONENT;
          }else
            state = NUM_END;
          break;
        case NUM_EXPONENT:
          if (ISDIGIT(c)) 
            m_LexBuffer[yypos++]=c;
          else
            state = NUM_END;
          break;
#endif
        }
        if (state != NUM_END)
          c = nextChar();
      }
      putbackchar(c);
      m_LexBuffer[yypos++]=0;
#ifdef USE_FLOATING_POINT
      if (floating==true){
        yylval->floating=(float)(ATOF(m_LexBuffer));
        c=L_FLOAT;
      }else{
#endif
        yylval->integer=ATOI(m_LexBuffer);
        c=L_INTEGER;
#ifdef USE_FLOATING_POINT
      }
#endif
      DBI("integer",(yylval->integer));
      break;    
    }
    // Comment-------------------------------------------
    if (c=='/'){
      int c1=nextChar();
      if (c1=='*'){
        do{
          c1=nextChar();
          if (c1=='\n'){
            m_LineNum++;
          }else if (c1=='*'){
            c1=nextChar();
            if (c1=='/')
              break;
          }
        }while (!eof());
        yylloc->first_line=m_LineNum;
        continue;
      }else
        if (c1=='/'){
          do{
            c1=nextChar();
            if (c1=='\n'){
              m_LineNum++;
              break;
            }
          }while (!eof());
          yylloc->first_line=m_LineNum;
          continue;
      }else
        putbackchar(c1);
      break;
    }
    // Default case
    DBC("char",c);
    break;
  }
  return c;
}
//---------------------------------------------------
void skParser::cleanupTempNodes()
//---------------------------------------------------
{
  // delete the list of nodes accumulated during the parse - in the event of an error
  skParseNodeListIterator iter(m_TempNodes);
  skParseNode * node=0;
  while ((node=iter())!=0)
    node->clear();
  m_TempNodes.clearAndDestroy();
  m_TempStringLiterals.clearAndDestroy();
#ifdef EXECUTE_PARSENODES
  m_Identifiers.clearAndDestroy();
#else
  m_Identifiers.clear();
#endif
}
//---------------------------------------------------
void skParser::setTopNode(skParseNode* pNode) 
//---------------------------------------------------
{
  m_TopNode = pNode; 
}
//---------------------------------------------------
void  skParser::addParseNode(skParseNode* pNode) 
//---------------------------------------------------
{
  m_TempNodes.append(pNode); 
}
//---------------------------------------------------
void  skParser::appendError(const skString& msg)
//---------------------------------------------------
{
  skString s;
  s=m_LexBuffer;
  skCompileError pErr(m_Location,m_LineNum,msg,s);
  m_ErrList.append(pErr);
}
//---------------------------------------------------
void  skParser::appendError(const Char * msg)
//---------------------------------------------------
{
  skString s;
  s=m_LexBuffer;
  skString s_msg;
  s_msg=msg;
  skCompileError pErr(m_Location,m_LineNum,s_msg,s);
  m_ErrList.append(pErr);
}
//---------------------------------------------------
skCompileErrorList&  skParser::getErrList()
//---------------------------------------------------
{
  return m_ErrList;
}
//---------------------------------------------------
skMethodDefNode * skParser::parseMethod()
//---------------------------------------------------
{
  if(m_LexBuffer==0)
    m_LexBuffer=skARRAY_NEW(Char,MAXYYTEXT);
  skMethodDefNode * node=0;
  yyparse(this);
  if (m_TopNode){
    if (m_TopNode->getType()!=s_MethodDef){
      appendError(skSTR("Not a valid method definition"));
    }else{
      node=(skMethodDefNode *)m_TopNode;
      m_TopNode=0;
#ifndef EXECUTE_PARSENODES
      node->getCompiledCode().moveIdentifiers(m_Identifiers);
      node->compile();
#endif
    }
  }
  if (node)
    clearTempNodes();
  else
    cleanupTempNodes();
  return node;
}
//---------------------------------------------------
#ifdef EXECUTE_PARSENODES
skExprNode * skParser::parseExpression()
#else
skCompiledExprNode * skParser::parseExpression()
#endif
//---------------------------------------------------
{
  if(m_LexBuffer==0)
    m_LexBuffer=skARRAY_NEW(Char,MAXYYTEXT);
#ifdef EXECUTE_PARSENODES
  skExprNode * node=0;
#else
  skCompiledExprNode * node=0;
#endif
  yyparse(this);
  if (m_TopNode){
    if (m_TopNode->getType()==s_MethodDef){
      appendError(skSTR("Not a valid expression"));
    }else{
#ifdef EXECUTE_PARSENODES
      node=(skExprNode *)m_TopNode;
#else
      node=(skCompiledExprNode *)m_TopNode;
      node->getCompiledCode().moveIdentifiers(m_Identifiers);
      node->compile();
#endif
      m_TopNode=0;
    }
  }
  if (node)
    clearTempNodes();
  else
    cleanupTempNodes();
  return node;
}

//---------------------------------------------------
void skParser::removeTempString(skString* str)
//---------------------------------------------------
{
  m_TempStringLiterals.remove(str);
}
//---------------------------------------------------
void  skParser::clearTempNodes()
//---------------------------------------------------
{
  m_TempNodes.clear();
  m_TempStringLiterals.clear();
#ifdef EXECUTE_PARSENODES
  m_Identifiers.clearAndDestroy();
#else
  m_Identifiers.clear();
#endif
}
#ifdef __SYMBIAN32__
//-------------------------------------------------------------------------
skParser::operator TCleanupItem()
//-------------------------------------------------------------------------
// used for leave support of local variable lists
{
  return TCleanupItem(Cleanup,this);
}
//-------------------------------------------------------------------------
void skParser::Cleanup(TAny * s)
//-------------------------------------------------------------------------
// called in case there is a leave
{
  // destroy the temporary nodes if there has been a leave
  ((skParser *)s)->cleanupTempNodes();
}
#endif
/**
 * Lex function called by the yacc script
 * Symbian Note: a leaving function
 */
//------------------------------------------
int yylex(YYSTYPE * lvalp, void * yylloc,void* context)
//------------------------------------------
{
  // this global function is called by the generated yyparse() function to produce the next token
  return ((skParser *)context)->lex((void *)lvalp,(void *)yylloc);
}
/**
 * Error function called by the yacc script
 * Symbian Note: a leaving function
 */
//------------------------------------------
void real_yyerror(Char *  msg, void* context)
//------------------------------------------
{
  // this global function is called by the generated yyparse() function if there is an error
  ((skParser *)context)->appendError(skSTR("Parse error"));
}
