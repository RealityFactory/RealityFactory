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

  $Id: skInterpreter.cpp,v 1.48 2001/11/22 11:13:21 sdw Exp $
*/

#include "skInterpreter.h"
#include "skInterpreterp.h"
#include "skParseNode.h"
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


skLITERAL(true);
skLITERAL(false);
skLITERAL(null);
skLITERAL(self);
skLITERAL(tracing);
skLITERAL(Interpreter);
skLITERAL(debugBreak);


skInterpreter * P_Interpreter::g_GlobalInterpreter;	//	used by client
skString g_BlankString;
skNull skInterpreter::g_Null;


//------------------------------------------
inline void P_Interpreter::trace(const skString& msg)
  //------------------------------------------
{
  if (m_TraceCallback!=0)
    m_TraceCallback->trace(msg);
  else
    skTracer::trace(msg);
}
//---------------------------------------------------
skInterpreter::skInterpreter()
  //---------------------------------------------------
{
  //  P_Interpreter::g_Interpreter=this;
  pimp=new P_Interpreter;
  addGlobalVariable(s_Interpreter,this);
}
//---------------------------------------------------
P_Interpreter::P_Interpreter()
  //---------------------------------------------------
  : m_Tracing(false),m_TraceCallback(0),m_StatementStepper(0)
{
}
//---------------------------------------------------
P_Interpreter::~P_Interpreter()
  //---------------------------------------------------
{
}
//---------------------------------------------------
skInterpreter::~skInterpreter()
  //---------------------------------------------------
{
  delete pimp;
}
//---------------------------------------------------
skInterpreter * skInterpreter::getInterpreter()
  //---------------------------------------------------
{
  return P_Interpreter::g_GlobalInterpreter;
}
//---------------------------------------------------
void skInterpreter::setInterpreter(skInterpreter * i)
  //---------------------------------------------------
{
  P_Interpreter::g_GlobalInterpreter=i;
}
//---------------------------------------------------
bool P_Interpreter::executeStat(skContext& ctxt,skiExecutable * obj,skRValueTable& var,skStatNode * pstat,skRValue& r)
  //---------------------------------------------------
{
  bool stop=false;
  ctxt.m_LineNum=pstat->m_LineNum;
  int stat_type=pstat->getType();
  if (m_StatementStepper)
    stop=(m_StatementStepper->statementExecuted(ctxt.m_Location,ctxt.m_LineNum,obj,var,stat_type)==false);
  if (stop==false)
    switch(stat_type){
    case s_If:
      stop=executeIfStat(ctxt,obj,var,(skIfNode*)pstat,r);break;
    case s_While:
      stop=executeWhileStat(ctxt,obj,var,(skWhileNode*)pstat,r);break;
    case s_Return:
      stop=executeReturnStat(ctxt,obj,var,(skReturnNode*)pstat,r);break;
    case s_Assign:
      executeAssignStat(ctxt,obj,var,(skAssignNode*)pstat);break;
    case s_Method:
      r=evalMethod(ctxt,obj,var,((skMethodStatNode*)pstat)->m_Ids);break;
    case s_Switch:
      stop=executeSwitchStat(ctxt,obj,var,(skSwitchNode*)pstat,r);break;
    case s_ForEach:
      stop=executeForEachStat(ctxt,obj,var,(skForEachNode*)pstat,r);break;
    case s_For:
      stop=executeForStat(ctxt,obj,var,(skForNode*)pstat,r);break;
    }
  return stop;
}
//---------------------------------------------------
bool P_Interpreter::executeStats(skContext& ctxt,skiExecutable * obj,skRValueTable& var,skStatListNode * n,skRValue& r)
  //---------------------------------------------------
{
  bool bRet=false;
  if (n){
    if (m_StatementStepper)
      bRet=(m_StatementStepper->compoundStatementExecuted(ctxt.m_Location,ctxt.m_LineNum,obj,var)==false);
    if (bRet==false){
      skStatListIterator iter(n->m_Stats);
      skStatNode * node=0;
      while ((node=iter())!=0){
	bRet=executeStat(ctxt,obj,var,node,r);
	// break out if stop has been passed back
	if (bRet)
	  break;
      }
    }
  }
  return bRet;
}
//---------------------------------------------------
void P_Interpreter::addLocalVariable(skRValueTable& var,const skString& name,skRValue value)
  //---------------------------------------------------
{
  skRValue * pvalue=var.value(&name);
  if (pvalue)
    *pvalue=value;
  else
    var.insertKeyAndValue(new skString(name),new skRValue(value));
}
//------------------------------------------
bool skInterpreter::findGlobalVariable(const skString& s,skRValue& r)
  //------------------------------------------
{
  bool bRet=false;
  skRValue * pvalue=pimp->m_GlobalVars.value(&s);
  if (pvalue){
    r=*pvalue;
    bRet=true;
  }
  return bRet;
}
//------------------------------------------
void skInterpreter::addGlobalVariable(const skString& s,skRValue r)
  //------------------------------------------
{
  skRValue * pvalue=pimp->m_GlobalVars.value(&s);
  if (pvalue)
    *pvalue=r;
  else
    pimp->m_GlobalVars.insertKeyAndValue(new skString(s),new skRValue(r));
}
//------------------------------------------
void skInterpreter::removeGlobalVariable(const skString& s)
  //------------------------------------------
{
  pimp->m_GlobalVars.del(&s);
}
//---------------------------------------------------
void P_Interpreter::followIdList(skContext& ctxt,skiExecutable * obj,skRValueTable& var,skIdListNode * idList,skRValue& object)
  //---------------------------------------------------
{
  // skip down the id.id.id list, resolving each as we go along, we exclude the final id in the list
  skIdNode * idNode=idList->m_Ids[0];
  skString name=idNode->m_Id;
  //  trace("followIdList: %s - %d ids\n",(const char *)name,idList->m_Ids.entries());
  if (idNode->m_Exprs==0)
    object=findValue(ctxt,obj,var,name,idNode->m_ArrayIndex,g_BlankString);
  else{
    skRValue caller(obj);
    makeMethodCall(ctxt,obj,var,caller,name,idNode->m_ArrayIndex,g_BlankString,idNode->m_Exprs,object);
  }
  for (unsigned int i=1;i<idList->m_Ids.entries()-1;i++){
    idNode=idList->m_Ids[i];
    name=idNode->m_Id;
    //    trace("followIdList: %d: %s\n",i,(const char *)name);
    skRValue result;
    if (idNode->m_Exprs==0){
      if (idNode->m_ArrayIndex)
	extractFieldArrayValue(ctxt,obj,var,object,name,idNode->m_ArrayIndex,g_BlankString,result);
      else
	if (extractValue(ctxt,object,name,g_BlankString,result)==false)
	  runtimeError(ctxt,skString(skSTR("Cannot get field "))+name+skSTR("\n"));
    }else
      makeMethodCall(ctxt,obj,var,object,name,idNode->m_ArrayIndex,g_BlankString,idNode->m_Exprs,result);
    object=result;
  }
}
//---------------------------------------------------
skRValue  P_Interpreter::evalMethod(skContext& ctxt,skiExecutable * obj,skRValueTable& var,skIdListNode * ids)
  //---------------------------------------------------
{
  skRValue ret;
  skIdNode * idNode=ids->getLastId();
  skString method_name=idNode->m_Id;
  if (ids->numIds()==1){
    skRValue caller(obj);
    makeMethodCall(ctxt,obj,var,caller,method_name,idNode->m_ArrayIndex,ids->m_Attribute,idNode->m_Exprs,ret);
  }else{
    // follow the chain of Id's and then call the method
    skRValue robject;
    followIdList(ctxt,obj,var,ids,robject);
    makeMethodCall(ctxt,obj,var,robject,method_name,idNode->m_ArrayIndex,ids->m_Attribute,idNode->m_Exprs,ret);
  }
  return ret;
}
//---------------------------------------------------
void  P_Interpreter::makeMethodCall(skContext& ctxt,skiExecutable * obj,skRValueTable& var,skRValue& robject,const skString& method_name,skExprNode * array_index,const skString& attribute, skExprListNode * exprs,skRValue& ret)
  //---------------------------------------------------
{
  skString checked_method_name=checkIndirectId(ctxt,obj,var,method_name);
  if (robject.type()==skRValue::T_Object){
    skRValueArray args;
    skExprListIterator iter(exprs->m_Exprs);
    skExprNode * expr=0;
    while ((expr=iter())!=0)
      args.append(evaluate(ctxt,obj,var,expr));
    if (m_Tracing)
      trace(ctxt.m_Location+skSTR(":")+skString::from(ctxt.m_LineNum)+skSTR(": ")+checked_method_name+skSTR("()\n"));
    // call to this object
    bool bRet=robject.obj()->method(checked_method_name,args,ret);	
    if (bRet==false)
      runtimeError(ctxt,skString(skSTR("Method "))+ checked_method_name+skSTR(" not found\n"));
    // case foo()[i]
    if (array_index){
      skRValue array_value;
      bRet=extractArrayValue(ctxt,obj,var,ret,array_index,attribute,array_value);
      ret=array_value;
    }else{
      // case foo():attr
      if (attribute.length()){
	skRValue attr_value;
	bRet=extractValue(ctxt,ret,g_BlankString,attribute,attr_value);
	ret=attr_value;
      }
    }
  }else{
    runtimeError(ctxt,skString(skSTR("Cannot call Method "))+checked_method_name+skSTR(" on a non-object\n"));
  }
}
//---------------------------------------------------
bool P_Interpreter::executeReturnStat(skContext& ctxt,skiExecutable * obj,skRValueTable& var,skReturnNode * n,skRValue& r)
  //---------------------------------------------------
{
  r=evaluate(ctxt,obj,var,n->m_Expr);
  return true;
}
//---------------------------------------------------
bool P_Interpreter::executeWhileStat(skContext& ctxt,skiExecutable * obj,skRValueTable& var,skWhileNode * n,skRValue& r)
  //---------------------------------------------------
{
  bool bRet=false;
  while(bRet==false){
    skRValue rExpr=evaluate(ctxt,obj,var,n->m_Expr);
    if (rExpr.boolValue()){
      if (n->m_Stats)
	bRet=executeStats(ctxt,obj,var,n->m_Stats,r);
    }else
      break;
  }
  return bRet;
}
//---------------------------------------------------
bool P_Interpreter::executeIfStat(skContext& ctxt,skiExecutable * obj,skRValueTable& var,skIfNode * n,skRValue& r)
  //---------------------------------------------------
{
  bool bRet=false;
  skRValue rExpr=evaluate(ctxt,obj,var,n->m_Expr);
  if (rExpr.boolValue()){
    if (n->m_Stats)
      bRet=executeStats(ctxt,obj,var,n->m_Stats,r);
  }else
    if (n->m_Else)		
      bRet=executeStats(ctxt,obj,var,n->m_Else,r);
  return bRet;
}
//---------------------------------------------------
bool P_Interpreter::executeSwitchStat(skContext& ctxt,skiExecutable * obj,skRValueTable& var,skSwitchNode * n,skRValue& r)
  //---------------------------------------------------
{
  bool bRet=false;
  skRValue expr=evaluate(ctxt,obj,var,n->m_Expr);
  skCaseNode * caseFound=0;
  skCaseListIterator iter(n->m_Cases->m_Cases);
  skCaseNode * caseNode=0;
  while ((caseNode=iter())!=0){
    skRValue testExpr=evaluate(ctxt,obj,var,caseNode->m_Expr);
    if (testExpr==expr){
      caseFound=caseNode;
      break;
    }
  }
  if (caseFound!=0)
    bRet=executeStats(ctxt,obj,var,caseFound->m_Stats,r);
  else if (n->m_Default!=0)
    bRet=executeStats(ctxt,obj,var,n->m_Default,r);
  return bRet;
}
//---------------------------------------------------
bool P_Interpreter::executeForEachStat(skContext& ctxt,skiExecutable * obj,skRValueTable& var,skForEachNode * n,skRValue& r)
  //---------------------------------------------------
{
  bool bRet=false;
  skString checked_id=checkIndirectId(ctxt,obj,var,n->m_Id);
  skRValue expr=evaluate(ctxt,obj,var,n->m_Expr);
  if (expr.type()==skRValue::T_Object){
    skExecutableIterator * iterator=0;
    if (n->m_Qualifier.length())
      iterator=expr.obj()->createIterator(n->m_Qualifier);
    else
      iterator=expr.obj()->createIterator();
    if (iterator){
      skRValue value;
      while ((iterator->next(value) && bRet==false)){
	addLocalVariable(var,checked_id,value);
	if (n->m_Stats){
	  bRet=executeStats(ctxt,obj,var,n->m_Stats,r);
	}
      }
      delete iterator;
    }else
      runtimeError(ctxt,skSTR("Object could not create an iterator, in a foreach statement\n"));
  }else
    runtimeError(ctxt,skSTR("Cannot apply foreach to a non-executable object\n"));
  return bRet;
}
//---------------------------------------------------
bool P_Interpreter::executeForStat(skContext& ctxt,skiExecutable * obj,skRValueTable& var,skForNode * n,skRValue& r)
  //---------------------------------------------------
{
  bool bRet=false;
  skString checked_id=checkIndirectId(ctxt,obj,var,n->m_Id);
  skRValue start_expr=evaluate(ctxt,obj,var,n->m_StartExpr);
  int start_value=start_expr.intValue();
  skRValue end_expr=evaluate(ctxt,obj,var,n->m_EndExpr);
  int end_value=end_expr.intValue();
  int step_value=1;
  if (n->m_StepExpr){
    skRValue step_expr=evaluate(ctxt,obj,var,n->m_StepExpr);
    step_value=step_expr.intValue();
  }
  if (start_value!=end_value){
    if (step_value>0){
      if (end_value>start_value){
	for (int i=start_value;i<end_value;i+=step_value){
	  addLocalVariable(var,checked_id,i);
	  if (n->m_Stats){
	    bRet=executeStats(ctxt,obj,var,n->m_Stats,r);
	    if (bRet)
	      break;
	}
	}
      }else
	runtimeError(ctxt,skSTR("End value is not greater than the start value in a for statement\n"));
    }else{
      if (step_value<0){
	if (start_value>end_value){
	  for (int i=start_value;i>end_value;i+=step_value){
	    addLocalVariable(var,checked_id,i);
	    if (n->m_Stats){
	      bRet=executeStats(ctxt,obj,var,n->m_Stats,r);
	      if (bRet)
		break;
	    }
	  }
	}else
	  runtimeError(ctxt,skSTR("Start value is not greater than the end value in a for statement\n"));
      }else
	runtimeError(ctxt,skSTR("Cannot use a zero step increment in a for statement\n"));
    }
  }// else do nothing
  return bRet;
}
//---------------------------------------------------
skRValue P_Interpreter::findValue(skContext& ctxt,skiExecutable * obj,skRValueTable& var,const skString& name,skExprNode * array_index,const skString& attrib)
  //---------------------------------------------------
{              
  skRValue r;
  skString valueName=checkIndirectId(ctxt,obj,var,name);
  if (valueName.length()){
    // first check some built-ins: true, false and self
    if (valueName==s_true)
      r=skRValue(true);
    else if (valueName==s_false)
      r=skRValue(false);
    else if (valueName==s_null)
      r=skRValue(&skInterpreter::g_Null);
    else if (valueName==s_self){
      if (attrib.length() || array_index){
	skRValue caller(obj);
	if (array_index){
	  extractArrayValue(ctxt,obj,var,caller,array_index,attrib,r);
	}else
	  if (extractValue(ctxt,caller,g_BlankString,attrib,r)==false)
	    runtimeError(ctxt,skString(skSTR("Cannot get attribute "))+attrib+skSTR("\n"));
      }else
	r=obj;
    }else{
      // otherwise look up the scope hierarchy
      skRValue * pvalue=0;
      // first in the local variables
      pvalue=var.value(&valueName);
      if (pvalue){
	r=*pvalue;
	if (attrib.length() || array_index){
	  skRValue result;
	  if (array_index){
	    extractArrayValue(ctxt,obj,var,r,array_index,attrib,result);
	  }else
	    if (extractValue(ctxt,r,g_BlankString,attrib,result)==false)
	      runtimeError(ctxt,skString(skSTR("Cannot get attribute "))+attrib+skSTR("\n"));
	  r=result;
	}
      }else{
	// then in the instance fields
	skRValue caller(obj);
	bool found=false;
	if (array_index)
	  found=extractFieldArrayValue(ctxt,obj,var,caller,valueName,array_index,attrib,r);
	else
	  found=extractValue(ctxt,caller,valueName,attrib,r);
	if (found==false){
	  // and finally in the global variables
	  pvalue=m_GlobalVars.value(&valueName);
	  if (pvalue){
	    r=*pvalue;
	    if (attrib.length() || array_index){
	      skRValue result;
	      if (array_index){
		extractArrayValue(ctxt,obj,var,r,array_index,attrib,result);
		r=result;
	      }else
		if (extractValue(ctxt,r,g_BlankString,attrib,result)==false)
		  runtimeError(ctxt,skString(skSTR("Cannot get attribute "))+attrib+skSTR("\n"));
	      r=result;
	    }
	  }else
	    runtimeError(ctxt,skString(skSTR("Field "))+valueName+skSTR("  not found\n"));
	}	
      }	
    }
  }
  return r;
}
//---------------------------------------------------
bool P_Interpreter::extractFieldArrayValue(skContext& ctxt,skiExecutable * obj,skRValueTable& var,skRValue& robject,const skString& field_name,skExprNode * array_index,const skString& attrib,skRValue& ret) 
  //---------------------------------------------------
{
  skRValue array_field;
  bool found=extractValue(ctxt,robject,field_name,g_BlankString,array_field);
  if (found){
    extractArrayValue(ctxt,obj,var,array_field,array_index,attrib,ret);
  }
  return found;
}
//---------------------------------------------------
bool P_Interpreter::extractArrayValue(skContext& ctxt,skiExecutable * obj,skRValueTable& var,skRValue& robject,skExprNode * array_index,const skString& attrib,skRValue& ret) 
  //---------------------------------------------------
{
  bool found=false;
  if (robject.type()==skRValue::T_Object){
    skRValue rExpr=evaluate(ctxt,obj,var,array_index);
    found=robject.obj()->getValueAt(rExpr,attrib,ret);
    if (found==false)
      runtimeError(ctxt,skSTR("Cannot get an array member\n"));
  }else
    runtimeError(ctxt,skSTR("Cannot get an array member from a non-object\n"));
  return found;
}
//---------------------------------------------------
bool P_Interpreter::extractValue(skContext& ctxt,skRValue& obj,const skString& name,const skString& attrib,skRValue& ret) 
  //---------------------------------------------------
{
  bool found=false;
  if (obj.type()==skRValue::T_Object){
    found=obj.obj()->getValue(name,attrib,ret);
  }else
    runtimeError(ctxt,skString(skSTR("Cannot get field "))+name+skSTR("  from a non-object\n"));
  return found;
}
//---------------------------------------------------
void P_Interpreter::executeAssignStat(skContext& ctxt,skiExecutable * obj,skRValueTable& var,skAssignNode * n)
  //---------------------------------------------------
{
  skRValue value=evaluate(ctxt,obj,var,n->m_Expr);
  skIdNode * idNode=n->m_Ids->getLastId();
  skString field_name=checkIndirectId(ctxt,obj,var,idNode->m_Id);
  if (n->m_Ids->numIds()==1){
    // special case where there is a single id
    bool inserted=false;
    if (obj!=0){
      skRValue caller(obj);
      if (field_name==s_self){
	if (idNode->m_ArrayIndex)
	  inserted=insertArrayValue(ctxt,obj,var,caller,idNode->m_ArrayIndex,n->m_Ids->m_Attribute,value);
	else
	  inserted=insertValue(ctxt,caller,g_BlankString,n->m_Ids->m_Attribute,value);
      }else{
	if (idNode->m_ArrayIndex){
	  caller=findValue(ctxt,obj,var,field_name,0,g_BlankString);
	  inserted=insertArrayValue(ctxt,obj,var,caller,idNode->m_ArrayIndex,n->m_Ids->m_Attribute,value);
	}else{
	  if (n->m_Ids->m_Attribute.length()>0){
	    // e.g. "field:name"
	    caller=obj;
	    inserted=insertValue(ctxt,caller,field_name,n->m_Ids->m_Attribute,value);
	    if (inserted==false)
	      runtimeError(ctxt,skString(skSTR("Setting attribute "))+n->m_Ids->m_Attribute+skSTR(" on ")+field_name+skSTR(" failed\n"));
	  }else
	    inserted=insertValue(ctxt,caller,field_name,n->m_Ids->m_Attribute,value);
	}
      }
    }
    if (inserted==false)
      // if the object doesn't want this variable, we add it as a local variable
      addLocalVariable(var,field_name,value);
  }else{
    // otherwise follow the id's to the penultimate one
    skRValue robject;
    followIdList(ctxt,obj,var,n->m_Ids,robject);
    if (idNode->m_ArrayIndex){
      skRValue array_field;
      if (extractValue(ctxt,robject,field_name,g_BlankString,array_field)){
	insertArrayValue(ctxt,obj,var,array_field,idNode->m_ArrayIndex,n->m_Ids->m_Attribute,value);
      }else{
	runtimeError(ctxt,skString(skSTR("Cannot find field "))+field_name+skSTR("\n"));
      }
    }else
      insertValue(ctxt,robject,field_name,n->m_Ids->m_Attribute,value);
  }
}
//---------------------------------------------------
bool P_Interpreter::insertArrayValue(skContext& ctxt,skiExecutable * obj,skRValueTable& var,skRValue& robject, skExprNode * array_index,const skString& attr,const skRValue& value)
  //---------------------------------------------------
{
  bool found=false;
  if (robject.type()==skRValue::T_Object){
    skRValue rExpr=evaluate(ctxt,obj,var,array_index);
    found=robject.obj()->setValueAt(rExpr,attr,value);
    if (found==false)
      runtimeError(ctxt,skSTR("Setting array member failed\n"));
  }else
    runtimeError(ctxt,skSTR("Cannot set an array member on a non-object\n"));
  return found;
}
//---------------------------------------------------
bool P_Interpreter::insertValue(skContext& ctxt,skRValue& robject,const skString& name, const skString& attr,const skRValue& value)
  //---------------------------------------------------
{
  bool found=false;
  if (robject.type()==skRValue::T_Object){
    found=robject.obj()->setValue(name,attr,value);
  }else
    runtimeError(ctxt,skString(skSTR("Cannot set field "))+name+skSTR("  on a non-object\n"));
  return found;
}
//---------------------------------------------------
void  skInterpreter::executeParseTree(const skString& location,skiExecutable * obj,skMethodDefNode * pExecuteNode,skRValueArray& args,skRValue& r)
  //---------------------------------------------------
{      
  skContext ctxt(location);
  if (pExecuteNode){
    skRValueTable vars;
    // fix up parameters
    if (pExecuteNode->m_Params){
      for (unsigned int i=0;i<pExecuteNode->m_Params->m_Ids.entries();i++)
	if (i<args.entries())
	  pimp->addLocalVariable(vars,pExecuteNode->m_Params->m_Ids[i]->m_Id,args[i]);
    }
    pimp->executeStats(ctxt,obj,vars,pExecuteNode->m_Stats,r);
  }
}
//---------------------------------------------------
skMethodDefNode * skInterpreter::parseString(const skString& location,const skString& code)
  //---------------------------------------------------
{ 
  skMethodDefNode * methNode=0;                                    
  skParser parser(code,location);
  parser.parse();
  if (parser.getTopNode()){
    methNode=parser.getTopNode();
    parser.clearTempNodes();
  }else{
    parser.cleanupTempNodes();
    THROW( skParseException(parser.getErrList()),skBoundsException_Code);
  }
  return methNode;
}
//---------------------------------------------------
skMethodDefNode * skInterpreter::parseExternalParams(const skString& location,skStringList& paramNames,const skString& code)
  //---------------------------------------------------
{                                     
  skMethodDefNode * methNode=parseString(location,code);
  // fix up the parameters
  if (methNode){
    if (methNode->m_Params==0)
      methNode->m_Params=new skIdListNode(0);
    else
      methNode->m_Params->m_Ids.clear();
    for (unsigned int i=0;i<paramNames.entries();i++)
      methNode->m_Params->addId(new skIdNode(0,paramNames[i],0,0));
  }
  return methNode;
}
//---------------------------------------------------
void skInterpreter::executeStringExternalParams(const skString& location,skiExecutable * obj,skStringList& paramNames,const skString& code,skRValueArray& args,skRValue& r,skMethodDefNode** keepParseNode)
  //---------------------------------------------------
{      
  skMethodDefNode * parseNode=parseExternalParams(location,paramNames,code);
  if (parseNode)
    executeParseTree(location,obj,parseNode,args,r);
  // give the parse node back to the caller if they want it
  if (keepParseNode)
    *keepParseNode=parseNode;
  else
    delete parseNode;
}
//---------------------------------------------------
void skInterpreter::executeString(const skString& location,skiExecutable * obj,const skString& code,skRValueArray& args,skRValue& r,skMethodDefNode** keepParseNode)
  //---------------------------------------------------
{      
  skMethodDefNode * parseNode=parseString(location,code);
  if (parseNode)
    executeParseTree(location,obj,parseNode,args,r);
  // give the parse node back to the caller if they want it
  if (keepParseNode)
    *keepParseNode=parseNode;
  else
    delete parseNode;
}
//---------------------------------------------------
skRValue P_Interpreter::evaluate(skContext& ctxt,skiExecutable * obj,skRValueTable& var,skExprNode * n)
  //---------------------------------------------------
{ 
  skRValue r;
  switch(n->getType()){
  case s_IdList:{
    skIdListNode * ids=(skIdListNode *)n;
    skIdNode * idNode=ids->getLastId();
    skString method_name=idNode->m_Id;
    if (ids->numIds()==1){
      if (idNode->m_Exprs==0)
	r=findValue(ctxt,obj,var,method_name,idNode->m_ArrayIndex,ids->m_Attribute);
      else{
	skRValue caller(obj);
	makeMethodCall(ctxt,obj,var,caller,method_name,idNode->m_ArrayIndex,ids->m_Attribute,idNode->m_Exprs,r);
      }
    }else{
      method_name=checkIndirectId(ctxt,obj,var,method_name);
      skRValue robject;
      followIdList(ctxt,obj,var,ids,robject);
      if (idNode->m_Exprs==0){
	if (idNode->m_ArrayIndex){
	  skRValue array_field;
	  if (extractValue(ctxt,robject,method_name,g_BlankString,array_field))
	    extractArrayValue(ctxt,obj,var,array_field,idNode->m_ArrayIndex,ids->m_Attribute,r);
	  else
	    runtimeError(ctxt,skString(skSTR("Cannot get field "))+method_name+skSTR("\n"));
	}else
	  if (extractValue(ctxt,robject,method_name,ids->m_Attribute,r)==false)
	    runtimeError(ctxt,skString(skSTR("Cannot get field "))+method_name+skSTR("\n"));
      }else
	makeMethodCall(ctxt,obj,var,robject,method_name,idNode->m_ArrayIndex,ids->m_Attribute,idNode->m_Exprs,r);
    }
    break;
  }
  case s_String:
    r=skRValue(*((skLiteralNode *)n)->m_String);
    break;
  case s_Integer:
    r=skRValue(((skLiteralNode *)n)->m_Int);
    break;
  case s_Character:
    r=skRValue(((skLiteralNode *)n)->m_Char);
    break;
  case s_Float:
    r=skRValue(((skLiteralNode *)n)->m_Float);
    break;
  default:
    skOpNode * opNode=(skOpNode *)n;
    skRValue item1=evaluate(ctxt,obj,var,opNode->m_Expr1);
    int item1Type=item1.type();
    switch(n->getType()){
    case s_Not:
      r=skRValue((bool)(!item1.boolValue()));
      break;
    case s_And:
      r=skRValue((bool)(item1.boolValue() && evaluate(ctxt,obj,var,opNode->m_Expr2).boolValue()));
      break;
    case s_Or:
      r=skRValue((bool)(item1.boolValue() || evaluate(ctxt,obj,var,opNode->m_Expr2).boolValue()));
      break;
    case s_Equals:
      r=skRValue((bool)(item1 == evaluate(ctxt,obj,var,opNode->m_Expr2)));
      break;
    case s_NotEquals:{
      bool equals=item1 == evaluate(ctxt,obj,var,opNode->m_Expr2);
      if (equals)
	r=false;
      else
	r=true;
      break;
    }
    case s_Minus:
      if (item1Type==skRValue::T_Float)
	r=skRValue(-item1.floatValue());
      else
	r=skRValue((-item1.intValue()));
      break;
    case s_Concat:
      r=skRValue(item1.str() + evaluate(ctxt,obj,var,opNode->m_Expr2).str());
      break;
    default:{
	skRValue item2=evaluate(ctxt,obj,var,opNode->m_Expr2);
	int item2Type=item2.type();
	if (item1Type==skRValue::T_Int && item2Type==skRValue::T_Int){
	  // we can use integer arithmetic if both objects are integer
	  int val1=item1.intValue();
	  int val2=item2.intValue();
	  switch(opNode->getType()){
	  case s_Plus:
	    r=skRValue(val1+val2);
	    break;
	  case s_More:
	    if (val1>val2)
	      r=skRValue(true);
	    else
	      r=skRValue(false);
	    break;
	  case s_MoreEqual:
	    if (val1>=val2)
	      r=skRValue(true);
	    else
	      r=skRValue(false);
	    break;
	  case s_Less:
	    if (val1<val2)
	      r=skRValue(true);
	    else
	      r=skRValue(false);
	    break;
	  case s_LessEqual:
	    if (val1<=val2)
	      r=skRValue(true);
	    else
	      r=skRValue(false);
	    break;
	  case s_Subtract:
	    r=skRValue(val1-val2);
	    break;
	  case s_Divide:{
	    long top=val1;
	    long bottom=val2;
	    if (bottom)
	      r=skRValue((int)(top/bottom));
	    else
	      runtimeError(ctxt,skSTR("Divide by zero error"));
	    break;
	  }
	  case s_Mult:
	    r=skRValue(val1*val2);
	    break;
	  case s_Mod:
	    r=skRValue(val1 % val2);
	    break;
	  }
	}else{
	  // in all other cases we using floating point
	  float val1=item1.floatValue();
	  float val2=item2.floatValue();
	  switch(opNode->getType()){
	  case s_Plus:
	    r=skRValue(val1+val2);
	    break;
	  case s_More:
	    if (val1>val2)
	      r=skRValue(true);
	    else
	      r=skRValue(false);
	    break;
	  case s_Less:
	    if (val1<val2)
	      r=skRValue(true);
	    else
	      r=skRValue(false);
	    break;
	  case s_Subtract:
	    r=skRValue(val1-val2);
	    break;
	  case s_Divide:{
	    double top=val1;
	    double bottom=val2;
	    if (bottom)
	      r=skRValue((float)(top/bottom));
	    else
	      runtimeError(ctxt,skSTR("Divide by zero error"));
	    break;
	  }
	  case s_Mult:
	    r=skRValue(val1*val2);
	    break;
	  case s_Mod:
	    r=skRValue((int)((long)val1 % (long)val2));
	    break;
	  }
	}
      }
    }
  }
  return r;
}
//------------------------------------------
bool skInterpreter::setValue(const skString& s,const skString& attrib,const skRValue& v)
  //------------------------------------------
{
  bool bRet=false;
  if (s==s_tracing){
    pimp->m_Tracing=v.boolValue();
    bRet=true;
  }else
#ifdef WIN32
#ifndef _WIN32_WCE
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
void skInterpreter::runtimeError(skContext& ctxt,const skString& msg)
  //------------------------------------------
{
  pimp->runtimeError(ctxt,msg);
}
//------------------------------------------
void P_Interpreter::runtimeError(skContext& ctxt,const skString& msg)
  //------------------------------------------
{
  THROW(skRuntimeException(ctxt.m_Location,ctxt.m_LineNum,msg),skBoundsException_Code);
}
//------------------------------------------
void skInterpreter::trace(const skString& msg)
  //------------------------------------------
{
  pimp->trace(msg);
}
//------------------------------------------
void skInterpreter::setTraceCallback(skTraceCallback * callback)
  //------------------------------------------
{
  pimp->m_TraceCallback=callback;
}
//------------------------------------------
void skInterpreter::setStatementStepper(skStatementStepper * stepper)
  //------------------------------------------
{
  pimp->m_StatementStepper=stepper;
}



