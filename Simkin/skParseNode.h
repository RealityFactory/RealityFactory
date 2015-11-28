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

  * $Id: skParseNode.h,v 1.42 2004/12/17 21:31:17 sdw Exp $
  */

#ifndef PARSENODE_H
#define PARSENODE_H
#include "skStringList.h"
#include "skAlist.h"


static const int s_If=4;
static const int s_While=5;
static const int s_Return=6;
static const int s_Assign=7;
static const int s_Method=8;
static const int s_IdList=9;
static const int s_String=10;
static const int s_Integer=11;
static const int s_Not=12;
static const int s_And=13;
static const int s_Or=14;
static const int s_Less=15;
static const int s_More=16;
static const int s_Equals=17;
static const int s_Plus=18;
static const int s_Minus=19;
static const int s_Subtract=20;
static const int s_Concat=21;
static const int s_Divide=22;
static const int s_Mult=23;
static const int s_Mod=24;
static const int s_Float=25;
static const int s_Character=26;
static const int s_CaseList=27;
static const int s_Case=28;
static const int s_Switch=29;
static const int s_Id=30;
static const int s_NotEquals=31;
static const int s_ForEach=32;
static const int s_LessEqual=33;
static const int s_MoreEqual=34;
static const int s_For=35;
static const int s_MethodDef=36;
static const int s_Break=37;

// Switch on to check size of node tree
//#define _DEBUG_SIZE

#ifndef EXECUTE_PARSENODES
const int NOT_PRESENT_INDEX=0xfff;

/**
 *   Instructions are 32-bit. Top 8 bits gives instruction type,
 *   lower 24 bits give instruction argument.
 *
 *
 * Restrictions: 
 * up to 4k unique id's per method
 * up to 4k literal strings per method
 * up to 4k literal floats per method
 * up to 4k literal ints per method
 * up to 4k literal chars per method
 * up to 4k lines per method
 * up to 4k ids in a series of field names
 *
 *
 */
// integers whose absolute value is less than this will be stored directly in the byte code
const int INLINE_INT=((1<<24)/2);
const int PARAM_MASK=0xffffff;
const int PARAM1_MASK=0xfff000;
const int PARAM2_MASK=0x000fff;
/**
 * This class provides a compact representation of a parse tree.
 * This representation is used to execute a script
 */
class skCompiledCode 
#ifdef __SYMBIAN32__
: public CBase
#endif
{
 public:
/**
 * Byte code definitions
 */
enum skInstruction{
  b_NullStat,        // param1 = 0                 param2 = line
  b_StatList,        // param1 = num stats         param2 = number of bytes in stat list
  b_Switch,          // param1 = has default       param2 = line
  b_If,              // param1 = has else          param2 = line
  b_Return,          // param1 = has return expr   param2 = line
  b_While,           // param1 = 0                 param2 = line
  b_ForEach,         // param1 = id index          param2 = line
  b_QualifierIndex,  // param1 = qualifier index   param2 = number of bytes in stat list
  b_For,             // param1 = has step expr     param2 = line
  b_Assign,          // param1 = 0                 param2 = line
  b_Method,          // param1 = 0                 param2 = line
  b_IdList,          // param1 = number of ids     param2 = attribute index
  b_IdWithMethod,    // param1 = id index          param2 = has array index
  b_Id,              // param1 = id index          param2 = has array index
  b_ExprList,        // param1 = 0                 param2 = number of expressions
  b_String,          // param1 = string index
  b_Int,             // param1 = int index
  b_IntInline,       // param1 = int value
  b_Char,            // param1 = char value
#ifdef USE_FLOATING_POINT
  b_Float,           // param1 = float index
#endif
  b_Op,              // param1 = op type           param2 = has 2nd expression
  b_CaseList,        // param1 = number of cases   param2 = number of byte codes in case list
  b_StatsSize,       // param1 = 0                 param2 = number of bytes in stat list
  b_Break,           // param1 = 0                 
  b_NUMCODES
};

  /** add an instruction each parameter is stored in 12 bits (4k)*/
  void addInstruction(skInstruction instruction,int parameter1,int parameter2);
  /** add an indentifier
   * @return the index of the indentifier, or 0xfff if the identifier is a blank string
   */
  int addId(const skString& id);
  /** add a literal string
   * @return the index of the string, or 0xfff if the string is a blank string
   */
  int addString(const skString& id);
  /** add a literal integer
   * @return the index of the integer
   */
  int addInt(int i);
#ifdef USE_FLOATING_POINT
  /** add a literal float
   * @return the index of the float
   */
  int addFloat(float f);
#endif
  /**
   * changes the parameters of the instruction
   */
  void setInstruction(USize pc,skInstruction instruction,int parameter1,int parameter2);
  /**
   * returns an instruction and its parameters
   */
  void getInstruction(USize pc,skInstruction& instruction,int& parameter1,int& parameter2);
  /**
   * returns an instruction and its parameters
   */
  void getInstruction(USize pc,skInstruction& instruction,int& parameter1,bool& parameter2);
  /**
   * Returns the identifier with the given id
   */
  skString getId(int id);
  /**
   * Returns the literal integer with the given id
   */
  int getInt(int id);
  /**
   * Returns the literal string with the given id
   */
  skString getString(int id);
#ifdef USE_FLOATING_POINT
  /**
   * Returns the literal float with the given id
   */
  float getFloat(int id);
#endif
  /**
   * Returns the index of the next instruction to be added to the list of instructions
   */
  USize getPC() const;
#ifdef _DEBUG_SIZE
  /**
   * Returns the overall memory used by this structure 
   */
  USize getSize() const;
#endif
  /**
   * Moves the identifiers from the given list into the identifiers in this compiled unit
   */
  void moveIdentifiers(skStringList& identifiers);
 private:
  /**
   * The list of the instructions in this unit
   */
  skTVAList<USize> m_Instructions;
  /**
   * The list of literal strings used by the instructions
   */
  skStringList m_LiteralStrings;
  /**
   * The list of identifiers used by the instructions
   */
  skStringList m_Identifiers;
  /**
   * The list of literal integers used by the instructions
   */
  skTVAList<int> m_LiteralInts;
#ifdef USE_FLOATING_POINT
  /**
   * The list of literal floats used by the instructions
   */
  skTVAList<float> m_LiteralFloats;
#endif
};
#endif

/**
 * This is the base class for Simkin parse nodes
 */
class  skParseNode 
#ifdef __SYMBIAN32__
: public CBase
#endif
{
 public:
  /** constructor for a parse node
   * @param linenum - the source code line number where the construct was declared */
  skParseNode(int linenum){
    m_LineNum=linenum;
  }
  /** destructor for the parse node */
  virtual ~skParseNode(){
  }
  /** returns the type of the node */
  virtual int getType(){
    return 0;
  }
  /** called if an error occurs during the parse */
  virtual void clear(){
  }
  int getLineNum() const{
    return m_LineNum;
  }
#ifdef _DEBUG_SIZE
  virtual USize getSize() const=0;
#endif
 protected:
  /** the line number where the construct was defined */
  int m_LineNum;
 private:
  skParseNode(const skParseNode& ){
  }
  skParseNode& operator=(const skParseNode&){
    return *this;
  }
};
class  skParseNodeList : public skTAList<skParseNode>{
};
typedef  skTAListIterator<skParseNode> skParseNodeListIterator;
class  skStatNode : public skParseNode {
 public:
  skStatNode(int linenum) 
    : skParseNode(linenum){
  }
  virtual ~skStatNode(){
  }
#ifdef _DEBUG_SIZE
  virtual USize getSize() const {
    return sizeof(this)+sizeof(void *);
  }
#endif
#ifndef EXECUTE_PARSENODES
 public:
  /**
   * @exception Symbian - a leaving function
   */
  virtual void compile(skCompiledCode& compiled_code);
#endif
};
class  skExprNode : public skParseNode {
 public:
  skExprNode(int linenum) 
    : skParseNode(linenum){
  }
  virtual ~skExprNode(){
  }
#ifndef EXECUTE_PARSENODES
  /**
   * @exception Symbian - a leaving function
   */
  virtual void compile(skCompiledCode& compiled_code)=0;
#endif
};
#ifndef EXECUTE_PARSENODES
class skCompiledExprNode : public skParseNode{
 public:
  skCompiledExprNode(skExprNode * node)
    : skParseNode(0),m_Node(node)
    {
  }
  skCompiledCode& getCompiledCode(){
    return m_Compiled;
  }
  void compile();
#ifdef _DEBUG_SIZE
  USize getSize() const{
    USize size=m_Compiled.getSize();
    if (m_Node)
      size+=m_Node->getSize();
    return size;
  }
#endif
 private:
  skExprNode * m_Node;
  skCompiledCode m_Compiled;
};
#endif
typedef skTAList<skStatNode> skStatList;
typedef skTAListIterator<skStatNode> skStatListIterator;

class  skStatListNode : public skParseNode {
 public:
  skStatListNode(int linenum)
    : skParseNode(linenum){
  }
  inline ~skStatListNode(){
    m_Stats.clearAndDestroy();
  }
  /**
   * @exception Symbian - a leaving function
   */
  inline void addStat(skStatNode * stat){
    m_Stats.append(stat);
  }
  inline void clear(){
    m_Stats.clear();
  }
  inline USize numStats(){
    return m_Stats.entries();
  }
  inline skStatNode * getStat(USize i){
    return m_Stats[i];
  }
#ifdef _DEBUG_SIZE
  virtual USize getSize() const {
    USize size=sizeof(this)+sizeof(void *);
    for (USize i=0;i<m_Stats.entries();i++)
      size+=+m_Stats[i]->getSize();
    size+=m_Stats.getArraySize()*sizeof(skStatNode *);
    return size;
  }
#endif
#ifndef EXECUTE_PARSENODES
 public:
  /**
   * @exception Symbian - a leaving function
   */
  void compile(skCompiledCode& compiled_code);
#endif
 private:
  skStatList m_Stats;
};
typedef skTAList<skExprNode> skExprList;
typedef skTAListIterator<skExprNode> skExprListIterator;
class  skExprListNode : public skParseNode {
 public:
  skExprListNode(int linenum)
    : skParseNode(linenum){
  }
  inline ~skExprListNode(){
    m_Exprs.clearAndDestroy();
  }
  /**
   * @exception Symbian - a leaving function
   */
  inline void addExpr(skExprNode * expr){
    m_Exprs.append(expr);
  }
  inline int numExprs(){
    return m_Exprs.entries();
  }
  inline void clear(){
    m_Exprs.clear();
  }
  skExprList& getExprs(){
    return m_Exprs;
  }
#ifdef _DEBUG_SIZE
  virtual USize getSize() const {
    USize size=sizeof(this)+sizeof(void *);
    for (USize i=0;i<m_Exprs.entries();i++)
      size+=+m_Exprs[i]->getSize();
    size+=m_Exprs.getArraySize()*sizeof(skExprNode *);
    return size;
  }
#endif
#ifndef EXECUTE_PARSENODES
  /**
   * @exception Symbian - a leaving function
   */
  void compile(skCompiledCode& compiled_code);
#endif
 private:
  skExprList m_Exprs;
};
class  skIdNode : public skExprNode {
 public:
#ifdef EXECUTE_PARSENODES
  inline skIdNode(int linenum,skString * id,skExprNode * arrayIndex,skExprListNode * exprs) 
    : skExprNode(linenum),m_Id(*id),m_Exprs(exprs),m_ArrayIndex(arrayIndex)
    {
  }
#else
  inline skIdNode(int linenum,int id,skExprNode * arrayIndex,skExprListNode * exprs) 
    : skExprNode(linenum),m_Id(id),m_Exprs(exprs),m_ArrayIndex(arrayIndex)
    {
  }
#endif
  inline ~skIdNode(){
    delete m_Exprs;
    delete m_ArrayIndex;
  }
  inline void clear(){
    m_Exprs=0;
    m_ArrayIndex=0;
  }
  inline int getType(){
    return s_Id;
  }
#ifdef EXECUTE_PARSENODES
  inline skString getId() const{
    return m_Id;
  }
#else
  inline int getId() const{
    return m_Id;
  }
#endif
  inline skExprListNode * getExprs(){
    return m_Exprs;
  }
  inline skExprNode * getArrayIndex(){
    return m_ArrayIndex;
  }
#ifdef _DEBUG_SIZE
  virtual USize getSize() const {
    USize size=sizeof(this)+sizeof(void *);
#ifdef EXECUTE_PARSENODES
    size+=m_Id.length();
#endif
    if (m_Exprs)
      size+=m_Exprs->getSize();
    if (m_ArrayIndex)
      size+=m_ArrayIndex->getSize();
    return size;
  }
#endif
#ifndef EXECUTE_PARSENODES
  /**
   * @exception Symbian - a leaving function
   */
  virtual void compile(skCompiledCode& compiled_code);
#endif
 private:
#ifdef EXECUTE_PARSENODES
  skString m_Id;
#else
  int m_Id;
#endif
  skExprListNode * m_Exprs;
  skExprNode * m_ArrayIndex;
};
class  skIdNodeList : public skTAList<skIdNode>{
};
typedef  skTAListIterator<skIdNode> skIdNodeListIterator;
class  skIdListNode : public skExprNode {
 public:
  inline skIdListNode(int linenum) 
    : skExprNode(linenum)
#ifndef EXECUTE_PARSENODES
    ,m_Attribute(NOT_PRESENT_INDEX)
#endif
    {
  }
  inline ~skIdListNode(){
    m_Ids.clearAndDestroy();
  }
  /**
   * @exception Symbian - a leaving function
   */
  inline void addId(skIdNode * node){
    m_Ids.append(node);
  }
  /**
   * @exception Symbian - a leaving function
   */
  inline void prependId(skIdNode * node){
    m_Ids.prepend(node);
  }
  inline USize numIds(){
    return m_Ids.entries();
  }
  inline void clear(){
    m_Ids.clear();
  }
  inline skIdNode * getLastId(){
    return m_Ids[m_Ids.entries()-1];
  }
  inline skIdNode * getId(USize i){
    return m_Ids[i];
  }
  inline int getType(){
    return s_IdList;
  }
#ifdef EXECUTE_PARSENODES
  inline void setAttribute(skString * attr){
    m_Attribute=*attr;
  }
  inline skString getAttribute() const {
    return m_Attribute;
  }
#else
  inline void setAttribute(int attr){
    m_Attribute=attr;
  }
#endif
#ifdef _DEBUG_SIZE
  virtual USize getSize() const {
    USize size=sizeof(this)+sizeof(void *);
#ifdef EXECUTE_PARSENODES
    size+=m_Attribute.length();
#endif
    for (USize i=0;i<m_Ids.entries();i++)
      size+=m_Ids[i]->getSize();
    size+=m_Ids.getArraySize()*sizeof(skIdNode *);
    return size;
  }
#endif
#ifndef EXECUTE_PARSENODES
  /**
   * @exception Symbian - a leaving function
   */
  void compile(skCompiledCode& compiled_code);
#endif
 private:
#ifdef EXECUTE_PARSENODES
  skString m_Attribute;
#else
  int m_Attribute;
#endif
  skIdNodeList m_Ids;
};
class  skCaseNode : public skParseNode {
 public:
  inline skCaseNode(int linenum,skExprNode * expr,skStatListNode * stat) 
    : skParseNode(linenum), m_Expr(expr),m_Stats(stat){
  }
  inline ~skCaseNode(){
    delete m_Expr;
    delete m_Stats;
  }
  inline void clear(){
    m_Expr=0;
    m_Stats=0;
  }
  inline int getType(){
    return s_Case;
  }
  inline skExprNode * getExpr(){
    return m_Expr;
  }
  inline skStatListNode * getStats(){
    return m_Stats;
  }
#ifdef _DEBUG_SIZE
  virtual USize getSize() const {
    return sizeof(this)+sizeof(void *)+m_Expr->getSize()+m_Stats->getSize();
  }
#endif
#ifndef EXECUTE_PARSENODES
  /**
   * @exception Symbian - a leaving function
   */
  void compile(skCompiledCode& compiled_code);
#endif
 private:
  skExprNode * m_Expr;
  skStatListNode * m_Stats;
};
typedef skTAList<skCaseNode> skCaseList;
typedef skTAListIterator<skCaseNode> skCaseListIterator;
class  skCaseListNode : public skParseNode {
 public:
  inline skCaseListNode(int linenum) 
    : skParseNode(linenum){
  }
  inline ~skCaseListNode(){
    m_Cases.clearAndDestroy();
  }
  /**
   * @exception Symbian - a leaving function
   */
  inline void addCase(skCaseNode * expr){
    m_Cases.append(expr);
  }
  inline int numCases(){
    return m_Cases.entries();;
  }
  inline void clear(){
    m_Cases.clear();
  }
  inline skCaseList& getCases(){
    return m_Cases;
  }
#ifdef _DEBUG_SIZE
  virtual USize getSize() const {
    USize size=sizeof(this)+sizeof(void *);
    for (USize i=0;i<m_Cases.entries();i++)
      size+=m_Cases[i]->getSize();
    size+=m_Cases.getArraySize()*sizeof(skCaseNode*);
    return size;
  }
#endif
#ifndef EXECUTE_PARSENODES
 public:
  /**
   * @exception Symbian - a leaving function
   */
  void compile(skCompiledCode& compiled_code);
#endif
 private:
  skCaseList m_Cases;
};
class  skSwitchNode : public skStatNode {
 public:
  inline skSwitchNode(int linenum,skExprNode * expr,skCaseListNode * cases,skStatListNode * defaultStat) 
    : skStatNode(linenum),m_Expr(expr),m_Cases(cases),m_Default(defaultStat){
  }
  inline ~skSwitchNode(){
    delete m_Expr;
    delete m_Cases;
    delete m_Default;
  }
  inline void clear(){
    m_Expr=0;
    m_Cases=0;
    m_Default=0;
  }
  inline int getType(){
    return s_Switch;
  }
  inline skExprNode * getExpr(){
    return m_Expr;
  }
  inline skCaseListNode * getCases(){
    return m_Cases;
  }
  inline skStatListNode * getDefault(){
    return m_Default;
  }
#ifdef _DEBUG_SIZE
  virtual USize getSize() const {
    USize size=sizeof(this)+sizeof(void *)+m_Expr->getSize()+m_Cases->getSize();
    if (m_Default)
      size+=m_Default->getSize();
    return size;
  }
#endif
#ifndef EXECUTE_PARSENODES
 public:
  /**
   * @exception Symbian - a leaving function
   */
  void compile(skCompiledCode& compiled_code);
#endif
 private:
  skExprNode * m_Expr;
  skCaseListNode * m_Cases;
  skStatListNode * m_Default;
};
class  skIfNode : public skStatNode {
 public:
  inline skIfNode(int linenum,skExprNode * expr,skStatListNode * stat,skStatListNode * elseStat) 
    : skStatNode(linenum),m_Expr(expr),m_Stats(stat),m_Else(elseStat){
  }
  inline ~skIfNode(){
    delete m_Expr;
    delete m_Stats;
    delete m_Else;
  }
  inline void clear(){
    m_Expr=0;
    m_Stats=0;
    m_Else=0;
  }
  inline int getType(){
    return s_If;
  }
  inline skExprNode * getExpr(){
    return m_Expr;
  }
  inline skStatListNode * getStats(){
    return m_Stats;
  }
  inline skStatListNode * getElse(){
    return m_Else;
  }
#ifdef _DEBUG_SIZE
  virtual USize getSize() const {
    USize size=sizeof(this)+sizeof(void *)+m_Expr->getSize()+m_Stats->getSize();
    if (m_Else)
      size+=m_Else->getSize();
    return size;
  }
#endif
#ifndef EXECUTE_PARSENODES
 public:
  /**
   * @exception Symbian - a leaving function
   */
  void compile(skCompiledCode& compiled_code);
#endif
 private:
  skExprNode * m_Expr;
  skStatListNode * m_Stats;
  skStatListNode * m_Else;
};
class  skReturnNode : public skStatNode {
 public:
  inline skReturnNode(int linenum,skExprNode * expr) 
    : skStatNode(linenum),m_Expr(expr){
  }
  inline skReturnNode(int linenum) 
    : skStatNode(linenum),m_Expr(0){
  }
  inline ~skReturnNode(){
    delete m_Expr;
  }
  inline void clear(){
    m_Expr=0;
  }
  inline int getType(){
    return s_Return;
  }
  inline skExprNode * getExpr(){
    return m_Expr;
  }
#ifdef _DEBUG_SIZE
  virtual USize getSize() const {
    USize size=sizeof(this);
    if (m_Expr)
      size+=m_Expr->getSize();
    return size;
  }
#endif
#ifndef EXECUTE_PARSENODES
 public:
  /**
   * @exception Symbian - a leaving function
   */
  void compile(skCompiledCode& compiled_code);
#endif
 private:
  skExprNode * m_Expr;
};
class  skBreakNode : public skStatNode {
 public:
  inline skBreakNode(int linenum) 
    : skStatNode(linenum){
  }
  inline ~skBreakNode(){
  }
  inline void clear(){
  }
  inline int getType(){
    return s_Break;
  }
#ifdef _DEBUG_SIZE
  virtual USize getSize() const {
    return sizeof(this);
  }
#endif
#ifndef EXECUTE_PARSENODES
 public:
  /**
   * @exception Symbian - a leaving function
   */
  void compile(skCompiledCode& compiled_code);
#endif
 private:
};
class  skWhileNode : public skStatNode {
 public:
  inline skWhileNode(int linenum,skExprNode * expr,skStatListNode * stat) 
    : skStatNode(linenum),m_Expr(expr),m_Stats(stat){
  }
  inline ~skWhileNode(){
    delete m_Expr;
    delete m_Stats;
  }
  inline void clear(){
    m_Expr=0;
    m_Stats=0;
  }
  inline int getType(){
    return s_While;
  }
  inline skExprNode * getExpr(){
    return m_Expr;
  }
  inline skStatListNode * getStats(){
    return m_Stats;
  }
#ifdef _DEBUG_SIZE
  virtual USize getSize() const {
    return sizeof(this)+sizeof(void *)+m_Expr->getSize()+m_Stats->getSize();
  }
#endif
#ifndef EXECUTE_PARSENODES
 public:
  /**
   * @exception Symbian - a leaving function
   */
  void compile(skCompiledCode& compiled_code);
#endif
 private:
  skExprNode * m_Expr;
  skStatListNode * m_Stats;
};
class  skForEachNode : public skStatNode {
 public:
#ifdef EXECUTE_PARSENODES
  inline skForEachNode(int linenum,skString * id,skExprNode * expr,skStatListNode * stat) 
    : skStatNode(linenum),m_Id(*id),m_Expr(expr),m_Stats(stat){
  }
  inline skForEachNode(int linenum,skString * qualifier,skString * id,skExprNode * expr,skStatListNode * stat) 
    : skStatNode(linenum),m_Id(*id),m_Qualifier(*qualifier),m_Expr(expr),m_Stats(stat){
  }
#else
  inline skForEachNode(int linenum,int id,skExprNode * expr,skStatListNode * stat) 
    : skStatNode(linenum),m_Id(id),m_Qualifier(NOT_PRESENT_INDEX),m_Expr(expr),m_Stats(stat){
  }
  inline skForEachNode(int linenum,int qualifier,int id,skExprNode * expr,skStatListNode * stat) 
    : skStatNode(linenum),m_Id(id),m_Qualifier(qualifier),m_Expr(expr),m_Stats(stat){
  }
#endif
  inline ~skForEachNode(){
    delete m_Expr;
    delete m_Stats;
  }
  inline void clear(){
    m_Expr=0;
    m_Stats=0;
  }
  inline int getType(){
    return s_ForEach;
  }
  inline skExprNode * getExpr(){
    return m_Expr;
  }
  inline skStatListNode * getStats(){
    return m_Stats;
  }
#ifdef EXECUTE_PARSENODES
  inline skString getId() const{
    return m_Id;
  }
  inline skString getQualifier() const{
    return m_Qualifier;
  }
#endif
#ifdef _DEBUG_SIZE
  virtual USize getSize() const {
    USize size=sizeof(this)+sizeof(void *)+m_Expr->getSize()+m_Stats->getSize();
#ifdef EXECUTE_PARSENODES
    size+=m_Id.length()+m_Qualifier.length();
#endif
    return size;
  }
#endif
#ifndef EXECUTE_PARSENODES
 public:
  /**
   * @exception Symbian - a leaving function
   */
  void compile(skCompiledCode& compiled_code);
#endif
 private:
#ifdef EXECUTE_PARSENODES
  skString m_Id;
  skString m_Qualifier;
#else
  int m_Id;
  int m_Qualifier;
#endif
  skExprNode * m_Expr;
  skStatListNode * m_Stats;
};
class  skForNode : public skStatNode {
 public:
#ifdef EXECUTE_PARSENODES
  inline skForNode(int linenum,skString * id,skExprNode * start_expr,skExprNode * end_expr,skStatListNode * stat) 
    : skStatNode(linenum),m_Id(*id),m_StartExpr(start_expr),m_EndExpr(end_expr),m_StepExpr(0){
    m_Stats=stat;
  }
  inline skForNode(int linenum,skString *id,skExprNode * start_expr,skExprNode * end_expr,skExprNode * step_expr,skStatListNode * stat) 
    : skStatNode(linenum),m_Id(*id),m_StartExpr(start_expr),m_EndExpr(end_expr),m_StepExpr(step_expr),m_Stats(stat){
  }
#else
  inline skForNode(int linenum,int id,skExprNode * start_expr,skExprNode * end_expr,skStatListNode * stat) 
    : skStatNode(linenum),m_Id(id),m_StartExpr(start_expr),m_EndExpr(end_expr),m_StepExpr(0){
    m_Stats=stat;
  }
  inline skForNode(int linenum,int id,skExprNode * start_expr,skExprNode * end_expr,skExprNode * step_expr,skStatListNode * stat) 
    : skStatNode(linenum),m_Id(id),m_StartExpr(start_expr),m_EndExpr(end_expr),m_StepExpr(step_expr),m_Stats(stat){
  }
#endif
  inline ~skForNode(){
    delete m_StartExpr;
    delete m_EndExpr;
    delete m_StepExpr;
    delete m_Stats;
  }
  inline void clear(){
    m_StartExpr=0;
    m_EndExpr=0;
    m_StepExpr=0;
    m_Stats=0;
  }
  inline int getType(){
    return s_For;
  }
  inline skExprNode * getStartExpr(){
    return m_StartExpr;
  }
  inline skExprNode * getEndExpr(){
    return m_EndExpr;
  }
  inline skExprNode * getStepExpr(){
    return m_StepExpr;
  }
  inline skStatListNode * getStats(){
    return m_Stats;
  }
#ifdef EXECUTE_PARSENODES
  inline skString getId() const{
    return m_Id;
  }
#endif
#ifdef _DEBUG_SIZE
  virtual USize getSize() const {
    USize size=sizeof(this)+sizeof(void *)+m_StartExpr->getSize()+m_EndExpr->getSize()+m_Stats->getSize();
#ifdef EXECUTE_PARSENODES
    size+=m_Id.length();
#endif
    if (m_StepExpr)
      size+=m_StepExpr->getSize();
    return size;
  }
#endif
#ifndef EXECUTE_PARSENODES
 public:
  /**
   * @exception Symbian - a leaving function
   */
  void compile(skCompiledCode& compiled_code);
#endif
 private:
#ifdef EXECUTE_PARSENODES
  skString m_Id;
#else
  int m_Id;
#endif
  skExprNode * m_StartExpr;
  skExprNode * m_EndExpr;
  skExprNode * m_StepExpr;
  skStatListNode * m_Stats;
};
class  skAssignNode : public skStatNode {
 public:
#ifdef EXECUTE_PARSENODES
  inline skAssignNode(int linenum,skIdListNode * idlist, skString * attribute,skExprNode * expr) 
    : skStatNode(linenum),m_Ids(idlist),m_Expr(expr){
    m_Ids->setAttribute(attribute);
  }
#else
  inline skAssignNode(int linenum,skIdListNode * idlist, int attribute,skExprNode * expr) 
    : skStatNode(linenum),m_Ids(idlist),m_Expr(expr){
    m_Ids->setAttribute(attribute);
  }
#endif
  inline skAssignNode(int linenum,skIdListNode * idlist, skExprNode * expr) 
    : skStatNode(linenum),m_Ids(idlist),m_Expr(expr){
  }
  inline ~skAssignNode(){
    delete m_Ids;
    delete m_Expr;
  }
  inline void clear(){
    m_Ids=0;
    m_Expr=0;
  }
  inline int getType(){
    return s_Assign;
  }
  inline skExprNode * getExpr(){
    return m_Expr;
  }
  inline skIdListNode * getIds(){
    return m_Ids;
  }
#ifdef _DEBUG_SIZE
  virtual USize getSize() const {
    return sizeof(this)+sizeof(void *)+m_Ids->getSize()+m_Expr->getSize();
  }
#endif
#ifndef EXECUTE_PARSENODES
 public:
  /**
   * @exception Symbian - a leaving function
   */
  void compile(skCompiledCode& compiled_code);
#endif
 private:
  skIdListNode * m_Ids;
  skExprNode * m_Expr;
};
class  skMethodStatNode : public skStatNode {
 public:
  inline skMethodStatNode(int linenum,skIdListNode * idlist) 
    : skStatNode(linenum),m_Ids(idlist){
  }
  inline ~skMethodStatNode(){
    delete m_Ids;
  }
  inline void clear(){
    m_Ids->clear();
    m_Ids=0;
  }
  inline int getType(){
    return s_Method;
  }
  inline skIdListNode * getIds(){
    return m_Ids;
  }
#ifdef _DEBUG_SIZE
  virtual USize getSize() const {
    return sizeof(this)+sizeof(void *)+m_Ids->getSize();
  }
#endif
#ifndef EXECUTE_PARSENODES
  /**
   * @exception Symbian - a leaving function
   */
  void compile(skCompiledCode& compiled_code);
#endif
 private:
  skIdListNode * m_Ids;
};
class  skLiteralNode : public skExprNode {
 public:
  inline skLiteralNode(int linenum,skString * s) 
    : skExprNode(linenum),m_String(s),m_Type(s_String){
  }
  inline skLiteralNode(int linenum,int i)
    : skExprNode(linenum),m_Int(i),m_Type(s_Integer){
  }
  inline skLiteralNode(int linenum,Char i)
    : skExprNode(linenum),m_Char(i),m_Type(s_Character){
  }
#ifdef USE_FLOATING_POINT
  inline skLiteralNode(int linenum,float f)
    : skExprNode(linenum),m_Float(f),m_Type(s_Float){
  }
#endif
  inline ~skLiteralNode(){
    if (m_Type==s_String)
      delete m_String;
  }
  inline void clear(){
    m_String=0;
  }
  inline int getType(){
    return m_Type;
  }
  inline skString getString() {
    return * m_String;
  }
#ifdef USE_FLOATING_POINT
  inline float getFloat(){
    return m_Float;
  }
#endif
  inline int getInt(){
    return m_Int;
  }
  inline Char getChar(){
    return m_Char;
  }
#ifdef _DEBUG_SIZE
  virtual USize getSize() const {
    USize size=sizeof(this)+sizeof(void *);
    if (m_Type==s_String)
      size+=sizeof(skString)+m_String->length();
    return size;
  }
#endif
#ifndef EXECUTE_PARSENODES
  /**
   * @exception Symbian - a leaving function
   */
  void compile(skCompiledCode& compiled_code);
#endif
 private:
  unsigned char m_Type;
  union {
    skString * m_String;
#ifdef USE_FLOATING_POINT
    float m_Float;
#endif
    int m_Int;
    Char m_Char;
  };
};
class  skOpNode : public skExprNode {
 public:
  inline skOpNode(int linenum,int type,skExprNode * expr1,skExprNode * expr2)
    : skExprNode(linenum),m_Expr1(expr1),m_Expr2(expr2),m_Type(type){
  }
  virtual ~skOpNode(){
    delete m_Expr1;
    delete m_Expr2;
  }
  inline void clear(){
    m_Expr1=0;
    m_Expr2=0;
  }
  inline int getType(){
    return m_Type;
  }
  inline skExprNode * getExpr1(){
    return m_Expr1;
  }
  inline skExprNode * getExpr2(){
    return m_Expr2;
  }
#ifdef _DEBUG_SIZE
  virtual USize getSize() const {
    USize size=sizeof(this)+sizeof(void *)+m_Expr1->getSize();
    if (m_Expr2)
      size+=m_Expr2->getSize();
    return size;
  }
#endif
#ifndef EXECUTE_PARSENODES
  /**
   * @exception Symbian - a leaving function
   */
  void compile(skCompiledCode& compiled_code);
#endif
 private:
  skExprNode * m_Expr1;
  skExprNode * m_Expr2;
  unsigned char m_Type;
};
/**
 * This class represents the parse tree for a whole method in Simkin.
 * You should not modify any of its data members.
 */
class  skMethodDefNode : public skParseNode {
 public:
  /** Constructor for Method node */
  inline skMethodDefNode(int linenum,skStatListNode * stats) 
    : skParseNode(linenum),m_Stats(stats),m_Params(0){
  }
  /** Constructor for Method node */
  inline skMethodDefNode(int linenum,skIdListNode * params,skStatListNode * stats) 
    : skParseNode(linenum),m_Stats(stats),m_Params(params){
  }
  /** Destructor for Method node */
  inline ~skMethodDefNode(){
    delete m_Stats;
    delete m_Params;
  }
  /** this method is called if an error occurs in the parse */
  inline void clear(){
    m_Stats=0;
    m_Params=0;
  }
  inline int getType(){
    return s_MethodDef;
  }
  inline skIdListNode * getParams(){
    return m_Params;
  }
  inline void setParams(skIdListNode * p){
    m_Params=p;
  }
  inline skStatListNode * getStats(){
    return m_Stats;
  }
 private:
  /** This statements in the method */
  skStatListNode * m_Stats;
  /** The parameters to this method */
  skIdListNode * m_Params;
  skMethodDefNode(const skMethodDefNode& ) 
    : skParseNode(0){
  }
  skMethodDefNode& operator=(const skMethodDefNode&){
    return *this;
  }
#ifdef _DEBUG_SIZE
  virtual USize getSize() const {
    return m_Stats->getSize();
  }
#endif
#ifndef EXECUTE_PARSENODES
 public:
  /**
   * @exception Symbian - a leaving function
   */
  void compile();
  skCompiledCode& getCompiledCode(){
    return m_Compiled;
  }
 private:
  skCompiledCode m_Compiled;
#endif
};
#include "skParseNode.inl"
#endif

