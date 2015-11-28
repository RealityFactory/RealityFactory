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

  $Id: skInterpreter.cpp,v 1.83 2003/04/14 15:24:57 simkin_cvs Exp $
*/

#include "skParseNode.h"
#include "skInterpreter.h"
#include "skString.h"
#include <stdio.h>
#include "skRValueArray.h"
#include "skStringList.h"
#include "skParseException.h"
#include "skRuntimeException.h"
#include "skTracer.h"
#include "skParser.h"
#include "skTraceCallback.h"
#include "skStatementStepper.h"
#include "skStackFrame.h"
#include "skConstants.h"

skNAMED_LITERAL(CannotGetField,skSTR("Cannot get field "));
skNAMED_LITERAL(FromANonObject,skSTR("  from a non-object\n"));
skNAMED_LITERAL(CannotSetField,skSTR("Cannot set field "));
skNAMED_LITERAL(OnANonObject,skSTR(" on a non-object\n"));

// switch on additional info about functioning of the interpreter
//#define TRACING_EXECUTION 1

//---------------------------------------------------
EXPORT_C skInterpreter::skInterpreter()
  //---------------------------------------------------
  : m_Tracing(false),m_TraceCallback(0),m_StatementStepper(0)
{
#ifndef __SYMBIAN32__
// don't call this function in Symbian, as it could leave
  init();
#endif
}
//---------------------------------------------------
EXPORT_C void skInterpreter::init()
  //---------------------------------------------------
{
  skRValue r;
  r.assignObject(this,false);
  SAVE_VARIABLE(r);
  addGlobalVariable(s_Interpreter,r);
  RELEASE_VARIABLE(r);
}
//---------------------------------------------------
skInterpreter::~skInterpreter()
  //---------------------------------------------------
{
}
//---------------------------------------------------
void skInterpreter::addLocalVariable(skRValueTable& var,const skString& name,skRValue value)
  //---------------------------------------------------
{
  skRValue * pvalue=var.value(name);
  if (pvalue)
    *pvalue=value;
  else{
    skRValue * value_copy=skNEW(skRValue(value));
    SAVE_POINTER(value_copy);
    var.insertKeyAndValue(name,value_copy);
    RELEASE_POINTER(value_copy);
  }
}
//------------------------------------------
EXPORT_C bool skInterpreter::findGlobalVariable(const skString& s,skRValue& r)
  //------------------------------------------
{
  bool bRet=false;
  skRValue * pvalue=m_GlobalVars.value(s);
  if (pvalue){
    r=*pvalue;
    bRet=true;
  }
  return bRet;
}
//------------------------------------------
EXPORT_C void skInterpreter::addGlobalVariable(const skString& name,const skRValue& value)
  //------------------------------------------
{
  skRValue * pvalue=m_GlobalVars.value(name);
  if (pvalue)
    *pvalue=value;
  else{
    skRValue * value_copy=skNEW(skRValue(value));
    SAVE_POINTER(value_copy);
    m_GlobalVars.insertKeyAndValue(name,value_copy);
    RELEASE_POINTER(value_copy);
  }
}
//------------------------------------------
EXPORT_C void skInterpreter::removeGlobalVariable(const skString& name)
  //------------------------------------------
{
  m_GlobalVars.del(name);
}
#ifdef __SYMBIAN32__
// Symbian-friendly version of addGlobalVariable
//------------------------------------------
EXPORT_C void skInterpreter::addGlobalVariable(const TDesC& s_name,const skRValue& value)
  //------------------------------------------
{
  skString name;
  name=s_name;
  addGlobalVariable(name,value);
}
// Symbian-friendly version of removeGlobalVariable
//------------------------------------------
EXPORT_C void skInterpreter::removeGlobalVariable(const TDesC& s)
  //------------------------------------------
{
  skString str;
  str=s;
  removeGlobalVariable(str);
}
#endif
//------------------------------------------
EXPORT_C const skRValueTable& skInterpreter::getGlobalVariables() const
  //------------------------------------------
{
  return m_GlobalVars;
}
//---------------------------------------------------
bool skInterpreter::extractValue(skStackFrame& frame,skRValue& obj,const skString& name,const skString& attrib,skRValue& ret) 
  //---------------------------------------------------
{
  bool found=false;
  if (obj.type()==skRValue::T_Object){
    found=obj.obj()->getValue(name,attrib,ret);
  }else
    runtimeError(frame,skString::addStrings(s_CannotGetField,name.ptr(),s_FromANonObject));
  return found;
}
//---------------------------------------------------
bool skInterpreter::insertValue(skStackFrame& frame,skRValue& robject,const skString& name, const skString& attr,const skRValue& value)
  //---------------------------------------------------
{
  bool found=false;
  if (robject.type()==skRValue::T_Object){
    found=robject.obj()->setValue(name,attr,value);
  }else
    runtimeError(frame,skString::addStrings(s_CannotSetField,name.ptr(),s_OnANonObject));
  return found;
}
//---------------------------------------------------
EXPORT_C void  skInterpreter::executeParseTree(const skString& location,skiExecutable * obj,skMethodDefNode * pExecuteNode,skRValueArray& args,skRValue& r,skExecutableContext& ctxt)
  //---------------------------------------------------
{      
#ifdef TRACING_EXECUTION
  trace(skSTR("skInterpreter::executeParseTree"));
#endif
  skRValueTable vars;
  SAVE_VARIABLE(vars);
  skStackFrame frame(location,obj,vars,ctxt);
  if (pExecuteNode){
    // fix up parameters
    if (pExecuteNode->getParams()){
      for (unsigned int i=0;i<pExecuteNode->getParams()->numIds();i++)
        if (i<args.entries())
#ifdef EXECUTE_PARSENODES
          addLocalVariable(vars,pExecuteNode->getParams()->getId(i)->getId(),args[i]);
#else
          addLocalVariable(vars,pExecuteNode->getCompiledCode().getId(pExecuteNode->getParams()->getId(i)->getId()),args[i]);
#endif
    }
#ifdef EXECUTE_PARSENODES
    executeStats(frame,pExecuteNode->getStats(),r);
#else    
    USize pc=0;
    int num_bytes=0;
    executeStats(frame,pExecuteNode->getCompiledCode(),pc,num_bytes,true,r);
#endif
  }
  RELEASE_VARIABLE(vars);
}
//---------------------------------------------------
EXPORT_C skMethodDefNode * skInterpreter::parseString(const skString& location,
                                             const skString& code,skExecutableContext& ctxt)
  //---------------------------------------------------
{ 
#ifdef TRACING_EXECUTION
  trace(skSTR("skInterpreter::parseString"));
#endif
  skMethodDefNode * methNode=0;
  skParser parser(code,location);
  SAVE_VARIABLE(parser);
  methNode=parser.parseMethod();
  if (methNode==0){
    bool throw_exception=true;
    skParseException e;
    e.setErrors(parser.getErrList());
    if (m_StatementStepper)
      throw_exception=m_StatementStepper->exceptionEncountered(ctxt.getTopFrame(),e);
    if (throw_exception){
#ifdef EXCEPTIONS_DEFINED
      throw e;
#else
      // inefficient - takes another copy of the error
      skParseException * exception=skNEW(skParseException);
      SAVE_POINTER(exception);
      exception->setErrors(parser.getErrList());
      ctxt.getError().setError(skScriptError::PARSE_ERROR,exception);
      RELEASE_POINTER(exception);
#endif
    }
  }
  RELEASE_VARIABLE(parser);
  return methNode;
}
//---------------------------------------------------
EXPORT_C skMethodDefNode * skInterpreter::parseExternalParams(const skString& location,skStringList& paramNames,const skString& code,skExecutableContext& user_ctxt)
  //---------------------------------------------------
{                                     
#ifdef TRACING_EXECUTION
  trace(skSTR("skInterpreter::parseExternalParams"));
#endif
  skMethodDefNode * methNode=parseString(location,code,user_ctxt);
  // fix up the parameters
  if (methNode){
    SAVE_POINTER(methNode);
    if (methNode->getParams()==0)
      methNode->setParams(skNEW(skIdListNode(0)));
    else
      methNode->getParams()->clear();
    for (unsigned int i=0;i<paramNames.entries();i++)
#ifdef EXECUTE_PARSENODES
      methNode->getParams()->addId(skNEW(skIdNode(0,&paramNames[i],0,0)));
#else
      methNode->getParams()->addId(skNEW(skIdNode(0,methNode->getCompiledCode().addId(paramNames[i]),0,0)));
#endif
    RELEASE_POINTER(methNode);
  }
  return methNode;
}
//---------------------------------------------------
EXPORT_C void skInterpreter::executeStringExternalParams(const skString& location,skiExecutable * obj,skStringList& paramNames,const skString& code,skRValueArray& args,skRValue& r,skMethodDefNode** keepParseNode,skExecutableContext& user_ctxt)
  //---------------------------------------------------
{      
#ifdef TRACING_EXECUTION
  trace(skSTR("skInterpreter::executeStringExternalParams"));
#endif
  if (keepParseNode)
    *keepParseNode=0;
#ifdef EXCEPTIONS_DEFINED
  skMethodDefNode * parseNode=parseExternalParams(location,paramNames,code,user_ctxt);
  if (parseNode){
    try{
      executeParseTree(location,obj,parseNode,args,r,user_ctxt);
    }catch(skRuntimeException e){
      delete parseNode;
      throw e;
    }catch(skParseException e1){
      delete parseNode;
      throw e1;
    }
  }
  // give the parse node back to the caller if they want it
  if (keepParseNode)
    *keepParseNode=parseNode;
  else
    delete parseNode;
#else
  skMethodDefNode * parseNode=parseExternalParams(location,paramNames,code,user_ctxt);
  if (parseNode){
    SAVE_POINTER(parseNode);
    executeParseTree(location,obj,parseNode,args,r,user_ctxt);
    if (user_ctxt.getError().getErrorCode()==skScriptError::NONE){
      // give the parse node back to the caller if they want it
      if (keepParseNode)
        *keepParseNode=parseNode;
      else
        delete parseNode;
    }else
      delete parseNode;
    RELEASE_POINTER(parseNode);
  }
#endif
}
//---------------------------------------------------
EXPORT_C void skInterpreter::executeString(const skString& location,skiExecutable * obj,const skString& code,skRValueArray& args,skRValue& r,skMethodDefNode** keepParseNode,skExecutableContext& user_ctxt)
  //---------------------------------------------------
{      
#ifdef TRACING_EXECUTION
  trace(skSTR("skInterpreter::executeString"));
#endif
  if (keepParseNode)
    *keepParseNode=0;
#ifdef EXCEPTIONS_DEFINED
  skMethodDefNode * parseNode=parseString(location,code,user_ctxt);
  if (parseNode){
    try{
      executeParseTree(location,obj,parseNode,args,r,user_ctxt);
    }catch(skRuntimeException e){
      delete parseNode;
      throw e;
    }catch(skParseException e1){
      delete parseNode;
      throw e1;
    }
  }
  // give the parse node back to the caller if they want it
  if (keepParseNode)
    *keepParseNode=parseNode;
  else
    delete parseNode;
#else
  skMethodDefNode * parseNode=parseString(location,code,user_ctxt);
  if (parseNode){
    SAVE_POINTER(parseNode);
    executeParseTree(location,obj,parseNode,args,r,user_ctxt);
    if (user_ctxt.getError().getErrorCode()==skScriptError::NONE){
      // give the parse node back to the caller if they want it
      if (keepParseNode)
        *keepParseNode=parseNode;
      else
        delete parseNode;
    }else
      delete parseNode;
    RELEASE_POINTER(parseNode);
  }
#endif
}
//------------------------------------------
bool skInterpreter::method(const skString& method_name,skRValueArray& arguments,skRValue& return_value,skExecutableContext& ctxt)
  //------------------------------------------
{
  bool bRet=false;
  if (method_name==s_breakpoint){
    bRet=true;
    if (m_StatementStepper)
      m_StatementStepper->breakpoint(ctxt.getTopFrame());
  }else
    bRet=skExecutable::method(method_name,arguments,return_value,ctxt);
  return bRet;
}
//------------------------------------------
void skInterpreter::getInstanceVariables(skRValueTable& table)
  //------------------------------------------
{
  skString str;
  SAVE_VARIABLE(str);
  str=s_tracing;
  skRValue * value=skNEW(skRValue());
  SAVE_POINTER(value);
  *value=m_Tracing;
  table.insertKeyAndValue(str,value);
  RELEASE_POINTER(value);
  RELEASE_VARIABLE(str);
}
//------------------------------------------
bool skInterpreter::getValue(const skString& s,const skString& attribute,skRValue& v)
  //------------------------------------------
{
  bool bRet=false;
  if (s==s_tracing){
    v=m_Tracing;
    bRet=true;
  }else
    bRet=skExecutable::getValue(s,attribute,v);
  return bRet;
}
//------------------------------------------
bool skInterpreter::setValue(const skString& s,const skString& attrib,const skRValue& v)
  //------------------------------------------
{
  bool bRet=false;
  if (s==s_tracing){
    m_Tracing=v.boolValue();
    bRet=true;
  }else
#ifdef WIN32
#ifndef _WIN32_WCE
    // on desktop Windows systems only, setting Interpreter.debugBreak=1 will cause
    // a debug break interrupt trap
    if (s==s_debugBreak){
      _asm int 03h;
      bRet=true;
    }else
#endif
#endif
      bRet=skExecutable::setValue(s,attrib,v);
  return bRet;
}
//------------------------------------------
EXPORT_C void skInterpreter::runtimeError(skStackFrame& frame,const skString& msg)
  //------------------------------------------
{
  bool throw_exception=true;
  skRuntimeException e(frame.getLocation(),frame.getLineNum(),msg);
  if (m_StatementStepper)
    throw_exception=m_StatementStepper->exceptionEncountered(&frame,e);
  if (throw_exception){
#ifdef EXCEPTIONS_DEFINED
    throw e;
#else
    skRuntimeException * exception=skNEW(skRuntimeException(frame.getLocation(),frame.getLineNum(),msg));
    frame.getContext().getError().setError(skScriptError::RUNTIME_ERROR,exception);
#endif
  }
}
//------------------------------------------
EXPORT_C void skInterpreter::runtimeError(skStackFrame& frame,const Char * msg)
  //------------------------------------------
{
  skString s_msg;
  s_msg=msg;
  runtimeError(frame,s_msg);
}
//------------------------------------------
EXPORT_C void skInterpreter::trace(const skString& msg)
  //------------------------------------------
{
  if (m_TraceCallback!=0)
    m_TraceCallback->trace(msg);
  else
    skTracer::trace(msg);
}
//------------------------------------------
EXPORT_C void skInterpreter::trace(const Char * msg)
  //------------------------------------------
{
  skString s_msg;
  SAVE_VARIABLE(s_msg);
  s_msg=msg;
  if (m_TraceCallback!=0)
    m_TraceCallback->trace(s_msg);
  else
    skTracer::trace(s_msg);
  RELEASE_VARIABLE(s_msg);
}
//------------------------------------------
EXPORT_C void skInterpreter::setTraceCallback(skTraceCallback * callback)
  //------------------------------------------
{
  m_TraceCallback=callback;
}
//------------------------------------------
EXPORT_C void skInterpreter::setStatementStepper(skStatementStepper * stepper)
  //------------------------------------------
{
  m_StatementStepper=stepper;
}
//------------------------------------------
#ifdef EXECUTE_PARSENODES
skExprNode * skInterpreter::parseExpression(const skString& location,const skString& expression,skExecutableContext& ctxt)
#else
skCompiledExprNode * skInterpreter::parseExpression(const skString& location,const skString& expression,skExecutableContext& ctxt)
#endif
  //------------------------------------------
{
  skParser parser(expression,location);
  SAVE_VARIABLE(parser);
#ifdef EXECUTE_PARSENODES
  skExprNode * exprNode=parser.parseExpression();
#else
  skCompiledExprNode * exprNode=parser.parseExpression();
#endif
  if (exprNode==0){
    bool throw_exception=true;
    skParseException e;
    e.setErrors(parser.getErrList());
    if (m_StatementStepper)
      throw_exception=m_StatementStepper->exceptionEncountered(ctxt.getTopFrame(),e);
    if (throw_exception){
#ifdef EXCEPTIONS_DEFINED
      throw e;
#else
      skParseException * exception_copy=skNEW(skParseException);
      SAVE_POINTER(exception_copy);
      // inefficient - another copy of the exception is generated
      exception_copy->setErrors(parser.getErrList());
      ctxt.getError().setError(skScriptError::PARSE_ERROR,exception_copy);
      RELEASE_POINTER(exception_copy);
#endif
    }
  }
  RELEASE_VARIABLE(parser);
  return exprNode;
}
//------------------------------------------
EXPORT_C void skInterpreter::evaluateExpression(const skString& location,skiExecutable * obj,
                                       const skString& expression,skRValueTable&  vars,
                                       skRValue& return_value,skExecutableContext& ctxt)
  //------------------------------------------
{
#ifdef EXECUTE_PARSENODES
  skExprNode * exprNode=parseExpression(location,expression,ctxt);
#else
  skCompiledExprNode * exprNode=parseExpression(location,expression,ctxt);
#endif
  if (exprNode){
#ifdef EXCEPTIONS_DEFINED
    try{
#ifdef EXECUTE_PARSENODES
      evaluateExpression(location,obj,exprNode,vars,return_value,ctxt);
#else
      evaluateExpression(location,obj,exprNode->getCompiledCode(),vars,return_value,ctxt);
#endif
    }catch(skRuntimeException e){
      delete exprNode;
      throw e;
    }catch(skParseException e1){
      delete exprNode;
      throw e1;
    }
#else
    SAVE_POINTER(exprNode);
#ifdef EXECUTE_PARSENODES
    evaluateExpression(location,obj,exprNode,vars,return_value,ctxt);
#else
    evaluateExpression(location,obj,exprNode->getCompiledCode(),vars,return_value,ctxt);
#endif
    RELEASE_POINTER(exprNode);
#endif
    delete exprNode;
  }
}
