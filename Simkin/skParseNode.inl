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

  * $Id: skParseNode.inl,v 1.6 2003/04/19 13:22:24 simkin_cvs Exp $
  */
#include "skParseNode.h"
#ifndef EXECUTE_PARSENODES
#include "skTracer.h"
extern const Char * g_ByteCodeNames[skCompiledCode::b_NUMCODES];
//#define _DEBUG_BYTECODES
//---------------------------------------------------
inline void skCompiledCode::getInstruction(USize pc,skInstruction& instruction,int& parameter1,int& parameter2)
//---------------------------------------------------
{
  USize inst=m_Instructions[pc];
  instruction=(skInstruction)(inst>>24);
  parameter1=(int)(((inst & PARAM1_MASK))>>12);
  parameter2=(int)(inst & PARAM2_MASK);
#ifdef _DEBUG_BYTECODES
  skTracer::trace(skSTR("Retrieving "));
  skTracer::trace(skString::from(pc));
  skTracer::trace(skSTR(":"));
  skTracer::trace(g_ByteCodeNames[instruction]);
  skTracer::trace(s_cr);
#endif
}
//---------------------------------------------------
inline void skCompiledCode::moveIdentifiers(skStringList& identifiers)
//---------------------------------------------------
{
  m_Identifiers.moveFrom(identifiers);
}
//---------------------------------------------------
inline void skCompiledCode::getInstruction(USize pc,skInstruction& instruction,int& parameter1,bool& parameter2)
//---------------------------------------------------
{

  USize inst=m_Instructions[pc];
  instruction=(skInstruction)(inst>>24);
  parameter1=(int)(((inst & PARAM1_MASK))>>12);
  parameter2=(inst & PARAM2_MASK)==1;
#ifdef _DEBUG_BYTECODES
  skTracer::trace(skSTR("Retrieving "));
  skTracer::trace(skString::from(pc));
  skTracer::trace(skSTR(":"));
  skTracer::trace(g_ByteCodeNames[instruction]);
  skTracer::trace(s_cr);
#endif
}
  /** parameter stored in lower 24 bits (16mb)*/
//---------------------------------------------------
inline skString skCompiledCode::getId(int id)
//---------------------------------------------------
{
  skString s;
  if (id!=NOT_PRESENT_INDEX)
    s=m_Identifiers[id];
  return s;
}
//---------------------------------------------------
inline int skCompiledCode::getInt(int id)
//---------------------------------------------------
{
  int i=0;
  if (id!=NOT_PRESENT_INDEX)
    i=m_LiteralInts[id];
  return i;
}
//---------------------------------------------------
inline skString skCompiledCode::getString(int id)
//---------------------------------------------------
{
  skString s;
  if (id!=NOT_PRESENT_INDEX)
    s=m_LiteralStrings[id];
  return s;
}
#ifdef USE_FLOATING_POINT
//---------------------------------------------------
inline float skCompiledCode::getFloat(int id)
//---------------------------------------------------
{
  float f=0.0f;
  if (id!=NOT_PRESENT_INDEX)
    f=m_LiteralFloats[id];
  return f;
}
#endif
//---------------------------------------------------
inline USize skCompiledCode::getPC() const
//---------------------------------------------------
{
  return m_Instructions.entries();
}
//---------------------------------------------------
inline void skCompiledCode::addInstruction(skInstruction instruction,int parameter1,int parameter2)
//---------------------------------------------------
{
  USize ins=(USize)(((long)(instruction)<<24)+(((long)parameter1)<<12)+(long)parameter2);
  m_Instructions.append(ins);
#ifdef _DEBUG_BYTECODES
  skTracer::trace(skSTR("Adding "));
  skTracer::trace(skString::from(m_Instructions.entries()-1));
  skTracer::trace(skSTR(":"));
  skTracer::trace(g_ByteCodeNames[instruction]);
  skTracer::trace(s_cr);
#endif
}
//---------------------------------------------------
inline void skCompiledCode::setInstruction(USize pc,skInstruction instruction,int parameter1,int parameter2)
//---------------------------------------------------
{
  USize ins=(USize)(((USize)(instruction)<<24)|((USize)(parameter1)<<12|(USize)parameter2));
  m_Instructions[pc]=ins;
#ifdef _DEBUG_BYTECODES
  skTracer::trace(skSTR("Setting "));
  skTracer::trace(skString::from(pc));
  skTracer::trace(skSTR(":"));
  skTracer::trace(g_ByteCodeNames[instruction]);
  skTracer::trace(s_cr);
#endif
}
//---------------------------------------------------
inline int skCompiledCode::addId(const skString& id)
//---------------------------------------------------
{
  int index=NOT_PRESENT_INDEX;
  if (id.length()>0){
  	USize i=0;
    for (i=0;i<m_Identifiers.entries();i++){
      if (m_Identifiers[i]==id){
        index=(int)i;
        break;
      }
    }
    if (index==NOT_PRESENT_INDEX){
      index=(int)i;
      m_Identifiers.append(id);
    }
  }
  return index;
}
//---------------------------------------------------
inline int skCompiledCode::addString(const skString& id)
//---------------------------------------------------
{
  int i=(int)m_LiteralStrings.entries();
  m_LiteralStrings.append(id);
  return i;
}
//---------------------------------------------------
inline int skCompiledCode::addInt(int integer)
//---------------------------------------------------
{
  int i=(int)m_LiteralInts.entries();
  m_LiteralInts.append(integer);
  return i;
}
#ifdef USE_FLOATING_POINT
//---------------------------------------------------
inline int skCompiledCode::addFloat(float f)
//---------------------------------------------------
{
  int index=NOT_PRESENT_INDEX;
  USize i=0;
  for (i=0;i<m_LiteralFloats.entries();i++){
    if (m_LiteralFloats[i]==f){
      index=(int)i;
      break;
    }
  }
  if (index==NOT_PRESENT_INDEX){
    index=(int)i;
    m_LiteralFloats.append(f);
  }
  return index;
}
#endif
#ifdef _DEBUG_SIZE
//---------------------------------------------------
inline USize skCompiledCode::getSize() const
//---------------------------------------------------
{
  USize size=sizeof(this);
  size+=m_Instructions.getArraySize()*sizeof(USize);
  size+=m_LiteralInts.getArraySize()*sizeof(int);
#ifdef USE_FLOATING_POINT
  size+=m_LiteralFloats.getArraySize()*sizeof(float);
#endif
  size+=m_LiteralStrings.getArraySize()*sizeof(skString);
  USize i=0;
  for (i=0;i<m_LiteralStrings.entries();i++)
    size+=m_LiteralStrings[i].length();
  size+=m_Identifiers.getArraySize()*sizeof(skString);
  for (i=0;i<m_Identifiers.entries();i++)
    size+=m_Identifiers[i].length();
  return size;
}
#endif
//---------------------------------------------------
inline void skMethodDefNode::compile()
//---------------------------------------------------
{
  if (m_Stats)
    m_Stats->compile(m_Compiled);
#ifdef _DEBUG_SIZE
  skTracer::trace(skSTR("Size of parse tree:"));
  skTracer::trace(skString::from(m_Stats->getSize()));
  skTracer::trace(s_cr);
  skTracer::trace(skSTR("Size of compiled code:"));
  skTracer::trace(skString::from(m_Compiled.getSize()));
  skTracer::trace(s_cr);
#endif
  // clear the parse tree - it is no longer required
  delete m_Stats;
  m_Stats=0;
}
//---------------------------------------------------
inline void skStatListNode::compile(skCompiledCode& compiled_code)
//---------------------------------------------------
{
  USize start_pc=compiled_code.getPC();
  // store placeholder
  compiled_code.addInstruction(skCompiledCode::b_StatList,0,0);
  for (USize i=0;i<m_Stats.entries();i++)
    m_Stats[i]->compile(compiled_code);
  // update placeholder with num entries and num bytes
  compiled_code.setInstruction(start_pc,skCompiledCode::b_StatList,m_Stats.entries(),compiled_code.getPC()-start_pc);
}
//---------------------------------------------------
inline void skStatNode::compile(skCompiledCode& compiled_code)
//---------------------------------------------------
{
  // store a null statement - line number is the parameter
  compiled_code.addInstruction(skCompiledCode::b_NullStat,0,m_LineNum);
}
//---------------------------------------------------
inline void skSwitchNode::compile(skCompiledCode& compiled_code)
//---------------------------------------------------
{
  // store as a parameter whether or not there is a default case and the line number
  compiled_code.addInstruction(skCompiledCode::b_Switch,(m_Default!=0),m_LineNum);
  m_Expr->compile(compiled_code);
  m_Cases->compile(compiled_code);
  if (m_Default)
    m_Default->compile(compiled_code);
}
//---------------------------------------------------
inline void skIfNode::compile(skCompiledCode& compiled_code)
//---------------------------------------------------
{
  // store as a parameter whether or not there are else statements and the line number
  compiled_code.addInstruction(skCompiledCode::b_If,(m_Else!=0),m_LineNum);
  m_Expr->compile(compiled_code);
  m_Stats->compile(compiled_code);
  if (m_Else)
    m_Else->compile(compiled_code);
}
//---------------------------------------------------
inline void skReturnNode::compile(skCompiledCode& compiled_code)
//---------------------------------------------------
{
  // store as a parameter whether or not there is a return expression (for future syntax...) and the line number
  compiled_code.addInstruction(skCompiledCode::b_Return,(m_Expr!=0),m_LineNum);
  if (m_Expr)
    m_Expr->compile(compiled_code);
}
//---------------------------------------------------
inline void skWhileNode::compile(skCompiledCode& compiled_code)
//---------------------------------------------------
{
  // store the line number as parameter
  compiled_code.addInstruction(skCompiledCode::b_While,0,m_LineNum);
  USize size_pc=compiled_code.getPC();
  compiled_code.addInstruction(skCompiledCode::b_StatsSize,0,0);
  m_Expr->compile(compiled_code);
  USize start_pc=compiled_code.getPC();
  m_Stats->compile(compiled_code);
  // update the stat size instruction with the number of bytes in the stats list
  compiled_code.setInstruction(size_pc,skCompiledCode::b_StatsSize,0,compiled_code.getPC()-start_pc);
}
//---------------------------------------------------
inline void skForEachNode::compile(skCompiledCode& compiled_code)
//---------------------------------------------------
{
  // this instruction includes the id and qualifier. n.b. this means there is a restriction of 4k different identifiers within a script
  compiled_code.addInstruction(skCompiledCode::b_ForEach,m_Id,m_LineNum);
  // now store qualifier index
  USize qualifier_pc=compiled_code.getPC();
  compiled_code.addInstruction(skCompiledCode::b_QualifierIndex,m_Qualifier,0);
  m_Expr->compile(compiled_code);
  USize start_pc=compiled_code.getPC();
  m_Stats->compile(compiled_code);
  // update the qualifier instruction with the number of bytes in the stats list
  compiled_code.setInstruction(qualifier_pc,skCompiledCode::b_QualifierIndex,m_Qualifier,compiled_code.getPC()-start_pc);
}
//---------------------------------------------------
inline void skForNode::compile(skCompiledCode& compiled_code)
//---------------------------------------------------
{
  // store as a parameter whether or not there is a step expression and the line number
  compiled_code.addInstruction(skCompiledCode::b_For,(m_StepExpr!=0),m_LineNum);
  USize size_pc=compiled_code.getPC();
  compiled_code.addInstruction(skCompiledCode::b_StatsSize,0,0);
  // store the loop variable as an id
  compiled_code.addInstruction(skCompiledCode::b_Id,m_Id,false);
  m_StartExpr->compile(compiled_code);
  m_EndExpr->compile(compiled_code);
  if (m_StepExpr!=0)
    m_StepExpr->compile(compiled_code);
  USize start_pc=compiled_code.getPC();
  m_Stats->compile(compiled_code);
  // update the stat size instruction with the number of bytes in the stats list
  compiled_code.setInstruction(size_pc,skCompiledCode::b_StatsSize,0,compiled_code.getPC()-start_pc);
}
//---------------------------------------------------
inline void skAssignNode::compile(skCompiledCode& compiled_code)
//---------------------------------------------------
{
  // store the line number as parameter
  compiled_code.addInstruction(skCompiledCode::b_Assign,0,m_LineNum);
  m_Expr->compile(compiled_code);
  m_Ids->compile(compiled_code);
}
//---------------------------------------------------
inline void skMethodStatNode::compile(skCompiledCode& compiled_code)
//---------------------------------------------------
{
  // store the line number as parameter
  compiled_code.addInstruction(skCompiledCode::b_Method,0,m_LineNum);
  m_Ids->compile(compiled_code);
}
//---------------------------------------------------
inline void skIdListNode::compile(skCompiledCode& compiled_code)
//---------------------------------------------------
{
  USize num_ids=m_Ids.entries();
  // store as a parameter the number of ids and the attribute
  compiled_code.addInstruction(skCompiledCode::b_IdList,num_ids,m_Attribute);
  for (USize i=0;i<num_ids;i++)
    m_Ids[i]->compile(compiled_code);
}
//---------------------------------------------------
inline void skIdNode::compile(skCompiledCode& compiled_code)
//---------------------------------------------------
{
  if (m_Exprs){
    // parameters: id name index, and whether there is an array index
    compiled_code.addInstruction(skCompiledCode::b_IdWithMethod,m_Id,m_ArrayIndex!=0);
    if (m_ArrayIndex)
      m_ArrayIndex->compile(compiled_code);
    m_Exprs->compile(compiled_code);
  }else{
    // parameters: id name index, and whether there is an array index
    compiled_code.addInstruction(skCompiledCode::b_Id,m_Id,m_ArrayIndex!=0);
    if (m_ArrayIndex)
      m_ArrayIndex->compile(compiled_code);
  }
}
//---------------------------------------------------
inline void skExprListNode::compile(skCompiledCode& compiled_code)
//---------------------------------------------------
{
  USize num_exprs=m_Exprs.entries();
  compiled_code.addInstruction(skCompiledCode::b_ExprList,0,num_exprs);
  for (USize i=0;i<num_exprs;i++)
    m_Exprs[i]->compile(compiled_code);
}
//---------------------------------------------------
inline void skLiteralNode::compile(skCompiledCode& compiled_code)
//---------------------------------------------------
{
  switch(m_Type){
  case s_String:
    compiled_code.addInstruction(skCompiledCode::b_String,compiled_code.addString(*m_String),0);
    break;
  case s_Integer:{
    if (abs(m_Int)<INLINE_INT)
      compiled_code.addInstruction(skCompiledCode::b_IntInline,m_Int,0);
    else
      compiled_code.addInstruction(skCompiledCode::b_Int,compiled_code.addInt(m_Int),0);
    break;
  }
  case s_Character:
    compiled_code.addInstruction(skCompiledCode::b_Char,m_Char,0);
    break;
#ifdef USE_FLOATING_POINT
  case s_Float:
    compiled_code.addInstruction(skCompiledCode::b_Float,compiled_code.addFloat(m_Float),0);
    break;
#endif
  }
}
//---------------------------------------------------
inline void skOpNode::compile(skCompiledCode& compiled_code)
//---------------------------------------------------
{
  // parameter: op code number and whether the op has 2 expressions
  compiled_code.addInstruction(skCompiledCode::b_Op,m_Type,m_Expr2!=0);
  m_Expr1->compile(compiled_code);
  if (m_Expr2)
    m_Expr2->compile(compiled_code);
}
//---------------------------------------------------
inline void skCaseListNode::compile(skCompiledCode& compiled_code)
//---------------------------------------------------
{
  USize start_pc=compiled_code.getPC();
  // store placeholder
  compiled_code.addInstruction(skCompiledCode::b_CaseList,0,0);
  for (USize i=0;i<m_Cases.entries();i++)
    m_Cases[i]->compile(compiled_code);
  // update placeholder with num cases and num bytes
  compiled_code.setInstruction(start_pc,skCompiledCode::b_CaseList,m_Cases.entries(),compiled_code.getPC()-start_pc);
}
//---------------------------------------------------
inline void skCaseNode::compile(skCompiledCode& compiled_code)
//---------------------------------------------------
{
  m_Expr->compile(compiled_code);
  m_Stats->compile(compiled_code);
}
//---------------------------------------------------
inline void skCompiledExprNode::compile()
//---------------------------------------------------
{
  if (m_Node)
    m_Node->compile(m_Compiled);
  // delete the parse tree, it's no longer necessary
  delete m_Node;
  m_Node=0;
}
#endif
