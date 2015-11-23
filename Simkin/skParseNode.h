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

 * $Id: skParseNode.h,v 1.25 2001/11/22 11:13:21 sdw Exp $
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

/**
 * This is the base class for Simkin parse nodes
 */
class  skParseNode {
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
  skStatNode(int linenum) : skParseNode(linenum){
  }
  virtual ~skStatNode(){
  }
};
class  skExprNode : public skParseNode {
 public:
  skExprNode(int linenum) : skParseNode(linenum){
  }
  virtual ~skExprNode(){
  }
};
typedef skTAList<skStatNode> skStatList;
typedef skTAListIterator<skStatNode> skStatListIterator;

class  skStatListNode : public skParseNode {
 public:
  skStatList m_Stats;
  skStatListNode(int linenum): skParseNode(linenum){
  }
  inline ~skStatListNode(){
    m_Stats.clearAndDestroy();
  }
  inline void addStat(skStatNode * stat){
    m_Stats.append(stat);
  }
  inline void clear(){
    m_Stats.clear();
  }
  inline int numStats(){
    return m_Stats.entries();
  }
};
typedef skTAList<skExprNode> skExprList;
typedef skTAListIterator<skExprNode> skExprListIterator;
class  skExprListNode : public skParseNode {
 public:
  skExprList m_Exprs;
  skExprListNode(int linenum): skParseNode(linenum){
  }
  inline ~skExprListNode(){
    m_Exprs.clearAndDestroy();
  }
  inline void addExpr(skExprNode * expr){
    m_Exprs.append(expr);
  }
  inline int numExprs(){
    return m_Exprs.entries();;
  }
  inline void clear(){
    m_Exprs.clear();
  }
};
class  skIdNode : public skExprNode {
 public:
  skString m_Id;
  skExprListNode * m_Exprs;
  skExprNode * m_ArrayIndex;
  inline skIdNode(int linenum,skString id,skExprNode * arrayIndex,skExprListNode * exprs) : skExprNode(linenum){
    m_Id=id;
    m_Exprs=exprs;
    m_ArrayIndex=arrayIndex;
  }
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
};
class  skIdNodeList : public skTAList<skIdNode>{
};
typedef  skTAListIterator<skIdNode> skIdNodeListIterator;
class  skIdListNode : public skExprNode {
 public:
  skIdNodeList m_Ids;
  skString m_Attribute;
  inline skIdListNode(int linenum) : skExprNode(linenum){
  }
  inline ~skIdListNode(){
    m_Ids.clearAndDestroy();
  }
  inline void addId(skIdNode * node){
    m_Ids.append(node);
  }
  inline void prependId(skIdNode * node){
    m_Ids.prepend(node);
  }
  inline int numIds(){
    return m_Ids.entries();
  }
  inline void clear(){
    m_Ids.clear();
  }
  inline skIdNode * getLastId(){
    return m_Ids[m_Ids.entries()-1];
  }
  inline int getType(){
    return s_IdList;
  }
};
class  skCaseNode : public skParseNode {
 public:
  skExprNode * m_Expr;
  skStatListNode * m_Stats;
  inline skCaseNode(int linenum,skExprNode * expr,skStatListNode * stat) : skParseNode(linenum){
    m_Expr=expr;
    m_Stats=stat;
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
};
typedef skTAList<skCaseNode> skCaseList;
typedef skTAListIterator<skCaseNode> skCaseListIterator;
class  skCaseListNode : public skParseNode {
 public:
  skCaseList m_Cases;
  inline skCaseListNode(int linenum) : skParseNode(linenum){
  }
  inline ~skCaseListNode(){
    m_Cases.clearAndDestroy();
  }
  inline void addCase(skCaseNode * expr){
    m_Cases.append(expr);
  }
  inline int numCases(){
    return m_Cases.entries();;
  }
  inline void clear(){
    m_Cases.clear();
  }
};
class  skSwitchNode : public skStatNode {
 public:
  skExprNode * m_Expr;
  skCaseListNode * m_Cases;
  skStatListNode * m_Default;
  inline skSwitchNode(int linenum,skExprNode * expr,skCaseListNode * cases,skStatListNode * defaultStat) : skStatNode(linenum){
    m_Expr=expr;
    m_Cases=cases;
    m_Default=defaultStat;
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
};
class  skIfNode : public skStatNode {
 public:
  skExprNode * m_Expr;
  skStatListNode * m_Stats;
  skStatListNode * m_Else;
  inline skIfNode(int linenum,skExprNode * expr,skStatListNode * stat,skStatListNode * elseStat) : skStatNode(linenum){
    m_Expr=expr;
    m_Stats=stat;
    m_Else=elseStat;
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
};
class  skReturnNode : public skStatNode {
 public:
  skExprNode * m_Expr;
  inline skReturnNode(int linenum,skExprNode * expr) : skStatNode(linenum){
    m_Expr=expr;
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
};
class  skWhileNode : public skStatNode {
 public:
  skExprNode * m_Expr;
  skStatListNode * m_Stats;
  inline skWhileNode(int linenum,skExprNode * expr,skStatListNode * stat) : skStatNode(linenum){
    m_Expr=expr;
    m_Stats=stat;
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
};
class  skForEachNode : public skStatNode {
 public:
  skString m_Id;
  skString m_Qualifier;
  skExprNode * m_Expr;
  skStatListNode * m_Stats;
  inline skForEachNode(int linenum,skString id,skExprNode * expr,skStatListNode * stat) : skStatNode(linenum){
    m_Id=id;
    m_Expr=expr;
    m_Stats=stat;
  }
  inline skForEachNode(int linenum,skString qualifier,skString id,skExprNode * expr,skStatListNode * stat) : skStatNode(linenum){
    m_Id=id;
    m_Qualifier=qualifier;
    m_Expr=expr;
    m_Stats=stat;
  }
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
};
class  skForNode : public skStatNode {
 public:
  skString m_Id;
  skExprNode * m_StartExpr;
  skExprNode * m_EndExpr;
  skExprNode * m_StepExpr;
  skStatListNode * m_Stats;
  inline skForNode(int linenum,skString id,skExprNode * start_expr,skExprNode * end_expr,skStatListNode * stat) : skStatNode(linenum){
    m_Id=id;
    m_StartExpr=start_expr;
    m_EndExpr=end_expr;
    m_StepExpr=0;
    m_Stats=stat;
  }
  inline skForNode(int linenum,skString id,skExprNode * start_expr,skExprNode * end_expr,skExprNode * step_expr,skStatListNode * stat) : skStatNode(linenum){
    m_Id=id;
    m_StartExpr=start_expr;
    m_EndExpr=end_expr;
    m_StepExpr=step_expr;
    m_Stats=stat;
  }
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
};
class  skAssignNode : public skStatNode {
 public:
  skIdListNode * m_Ids;
  skExprNode * m_Expr;
  inline skAssignNode(int linenum,skIdListNode * idlist, skString * attribute,skExprNode * expr) : skStatNode(linenum){
    m_Ids=idlist;
    if (attribute)
      m_Ids->m_Attribute=*attribute;
    delete attribute;
    m_Expr=expr;
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
};
class  skMethodStatNode : public skStatNode {
 public:
  skIdListNode * m_Ids;
  inline skMethodStatNode(int linenum,skIdListNode * idlist) : skStatNode(linenum){
    m_Ids=idlist;
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
};
class  skLiteralNode : public skExprNode {
 public:
  int m_Type;
  union {
    float m_Float;
    int m_Int;
    Char m_Char;
    skString * m_String;
  };
  inline skLiteralNode(int linenum,skString * s) : skExprNode(linenum){
    m_String=s;
    m_Type=s_String;
  }
  inline skLiteralNode(int linenum,int i): skExprNode(linenum){
    m_Int=i;
    m_Type=s_Integer;
  }
  inline skLiteralNode(int linenum,Char i): skExprNode(linenum){
    m_Char=i;
    m_Type=s_Character;
  }
  inline skLiteralNode(int linenum,float f): skExprNode(linenum){
    m_Float=f;
    m_Type=s_Float;
  }
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
};
class  skOpNode : public skExprNode {
 public:
  skExprNode * m_Expr1;
  skExprNode * m_Expr2;
  int m_Type;
  inline skOpNode(int linenum,int type,skExprNode * expr1,skExprNode * expr2): skExprNode(linenum){
    m_Expr1=expr1;
    m_Expr2=expr2;
    m_Type=type;
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
};
/**
 * This class represents the parse tree for a whole method in Simkin.
 * You should not modify any of its data members.
 */
class  skMethodDefNode : public skParseNode {
 public:
  /** This statements in the method */
  skStatListNode * m_Stats;
  /** The parameters to this method */
  skIdListNode * m_Params;
  /** Constructor for Method node */
  inline skMethodDefNode(int linenum,skStatListNode * stats) : skParseNode(linenum){
    m_Stats=stats;
    m_Params=0;
  }
  /** Constructor for Method node */
  inline skMethodDefNode(int linenum,skIdListNode * params,skStatListNode * stats) : skParseNode(linenum){
    m_Stats=stats;
    m_Params=params;
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
 private:
  skMethodDefNode(const skMethodDefNode& ) : skParseNode(0){
  }
  skMethodDefNode& operator=(const skMethodDefNode&){
    return *this;
  }
};
#endif

