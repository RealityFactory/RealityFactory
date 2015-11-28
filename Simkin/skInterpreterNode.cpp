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

  $Id: skInterpreterNode.cpp,v 1.9 2004/12/17 21:31:17 sdw Exp $
*/
#include "skInterpreter.h"
#ifdef EXECUTE_PARSENODES
#include "skRValueArray.h"
#include "skStackFrame.h"
#include "skStatementStepper.h"
skLITERAL_STRING(FieldError,"Cannot get field ");
skLITERAL_STRING(FunctionEnd,"()\n");
skLITERAL_STRING(NotFound," not found\n");
skLITERAL_STRING(MethodStart,"Method ");
skLITERAL_STRING(CannotCallMethod,"Cannot call Method ");
skLITERAL_STRING(CannotGetAttribute,"Cannot get attribute ");
skLITERAL_STRING(FieldStart,"Field ");
skLITERAL_STRING(SettingAttribute,"Setting attribute ");
skLITERAL_STRING(Failed," failed\n");
skLITERAL_STRING(OnMiddle," on ");
skLITERAL_STRING(CannotFindField,"Cannot find field ");
//---------------------------------------------------
skInterpreter::StatementReturnCode skInterpreter::executeStats(skStackFrame& frame,skStatListNode * n,skRValue& r)
  //---------------------------------------------------
{
  StatementReturnCode bRet=SRC_CONTINUE;
  if (n){
    for (USize i=0;i<n->numStats();i++){
      skStatNode * pstat=n->getStat(i);
#ifndef EXCEPTIONS_DEFINED
      if (frame.getContext().getError().getErrorCode()!=skScriptError::NONE)
        break;
#endif
      frame.setLineNum(pstat->getLineNum());
      int stat_type=pstat->getType();
      if (m_StatementStepper)
        if (m_StatementStepper->statementExecuted(frame,stat_type)==false)
          bRet=SRC_RETURN;
      if (bRet==false){
        switch(stat_type){
        case s_If:
          bRet=executeIfStat(frame,(skIfNode*)pstat,r);break;
        case s_While:
          bRet=executeWhileStat(frame,(skWhileNode*)pstat,r);break;
        case s_Return:
          bRet=executeReturnStat(frame,(skReturnNode*)pstat,r);break;
        case s_Break:
          bRet=executeBreakStat(frame,(skBreakNode*)pstat,r);break;
        case s_Assign:
          executeAssignStat(frame,(skAssignNode*)pstat);break;
        case s_Method:
          r=evalMethod(frame,((skMethodStatNode*)pstat)->getIds());break;
        case s_Switch:
          bRet=executeSwitchStat(frame,(skSwitchNode*)pstat,r);break;
        case s_ForEach:
          bRet=executeForEachStat(frame,(skForEachNode*)pstat,r);break;
        case s_For:
          bRet=executeForStat(frame,(skForNode*)pstat,r);break;
        }
        // break out if stop has been passed back
        if (bRet!=SRC_CONTINUE)
          break;
      }
    }
    if (m_StatementStepper)
      if (m_StatementStepper->compoundStatementExecuted(frame)==false)
        bRet=SRC_RETURN;
  }
  return bRet;
}
//---------------------------------------------------
void skInterpreter::followIdList(skStackFrame& frame,skIdListNode * idList,skRValue& object)
  //---------------------------------------------------
{
   skString blank;
  // skip down the id.id.id list, resolving each as we go along, we exclude the final id in the list
  skIdNode * idNode=idList->getId(0);
  skString name=idNode->getId();
  //  trace("followIdList: %s - %d ids\n",(const char *)name,idList->m_Ids.entries());
  if (idNode->getExprs()==0)
    object=findValue(frame,name,idNode->getArrayIndex(),blank);
  else{
    skRValue caller;
    SAVE_VARIABLE(caller);
    caller.assignObject(frame.getObject(),false);
    makeMethodCall(frame,caller,name,idNode->getArrayIndex(),blank,idNode->getExprs(),object);
    RELEASE_VARIABLE(caller);
  }
  for (unsigned int i=1;i<idList->numIds()-1;i++){
#ifndef EXCEPTIONS_DEFINED
    if (frame.getContext().getError().getErrorCode()!=skScriptError::NONE)
      break;
#endif
    idNode=idList->getId(i);
    name=idNode->getId();
    name=checkIndirectId(frame,name);
    //    trace("followIdList: %d: %s\n",i,(const char *)name);
    skRValue result;
    SAVE_VARIABLE(result);
    if (idNode->getExprs()==0){
      if (idNode->getArrayIndex())
        extractFieldArrayValue(frame,object,name,idNode->getArrayIndex(),blank,result);
      else
        if (extractValue(frame,object,name,blank,result)==false)
          runtimeError(frame,skString::addStrings(s_FieldError,name.ptr(),s_cr));
    }else
      makeMethodCall(frame,object,name,idNode->getArrayIndex(),blank,idNode->getExprs(),result);
    object=result;
    RELEASE_VARIABLE(result);
  }
}
//---------------------------------------------------
skRValue  skInterpreter::evalMethod(skStackFrame& frame,skIdListNode * ids)
  //---------------------------------------------------
{
  skRValue ret;
  SAVE_VARIABLE(ret);
  skIdNode * idNode=ids->getLastId();
  skString method_name=idNode->getId();
  if (ids->numIds()==1){
    skRValue caller;
    SAVE_VARIABLE(caller);
    caller.assignObject(frame.getObject());
    makeMethodCall(frame,caller,method_name,idNode->getArrayIndex(),ids->getAttribute(),idNode->getExprs(),ret);
    RELEASE_VARIABLE(caller);
  }else{
    // follow the chain of Id's and then call the method
    skRValue robject;
    SAVE_VARIABLE(robject);
    followIdList(frame,ids,robject);
    makeMethodCall(frame,robject,method_name,idNode->getArrayIndex(),ids->getAttribute(),idNode->getExprs(),ret);
    RELEASE_VARIABLE(robject);
  }
  RELEASE_VARIABLE(ret);
  return ret;
}
//---------------------------------------------------
void  skInterpreter::makeMethodCall(skStackFrame& frame,skRValue& robject,const skString& method_name,skExprNode * array_index,const skString& attribute, skExprListNode * exprs,skRValue& ret)
  //---------------------------------------------------
{
  skString checked_method_name=checkIndirectId(frame,method_name);
  if (robject.type()==skRValue::T_Object){
    skRValueArray args;
    SAVE_VARIABLE(robject);
    skExprListIterator iter(exprs->getExprs());
    skExprNode * expr=0;
    while ((expr=iter())!=0){
#ifndef EXCEPTIONS_DEFINED
      if (frame.getContext().getError().getErrorCode()!=skScriptError::NONE)
        break;
#endif
      args.append(evaluate(frame,expr));
    }
#ifndef EXCEPTIONS_DEFINED
    if (frame.getContext().getError().getErrorCode()==skScriptError::NONE){
#endif
      if (m_Tracing)
        trace(skString::addStrings(frame.getLocation().ptr(),s_colon,skString::from(frame.getLineNum()+1).ptr(),s_colon,checked_method_name.ptr(),s_FunctionEnd));
      // call to this object
      bool bRet=robject.obj()->method(checked_method_name,args,ret,frame.getContext());
      if (bRet==false)
        runtimeError(frame,skString::addStrings(s_MethodStart,checked_method_name.ptr(),s_NotFound));
      // case foo()[i]
      if (array_index){
        skRValue array_value;
        bRet=extractArrayValue(frame,ret,array_index,attribute,array_value);
        ret=array_value;
      }else{
        // case foo():attr
        if (attribute.length()){
          skRValue attr_value;
          skString blank;
          bRet=extractValue(frame,ret,blank,attribute,attr_value);
          ret=attr_value;
        }
      }
#ifndef EXCEPTIONS_DEFINED
    }
#endif
    RELEASE_VARIABLE(robject);
  }else{
    runtimeError(frame,skString::addStrings(s_CannotCallMethod,checked_method_name.ptr(),s_OnANonObject));
  }
}
//---------------------------------------------------
skInterpreter::StatementReturnCode skInterpreter::executeReturnStat(skStackFrame& frame,skReturnNode * n,skRValue& r)
  //---------------------------------------------------
{
  if (n->getExpr())
    r=evaluate(frame,n->getExpr());
  return SRC_RETURN;
}
//---------------------------------------------------
skInterpreter::StatementReturnCode skInterpreter::executeBreakStat(skStackFrame& frame,skBreakNode * n,skRValue& r)
  //---------------------------------------------------
{
  return SRC_BREAK;
}
//---------------------------------------------------
skInterpreter::StatementReturnCode skInterpreter::executeWhileStat(skStackFrame& frame,skWhileNode * n,skRValue& r)
  //---------------------------------------------------
{
  StatementReturnCode bRet=SRC_CONTINUE;
  while(bRet==SRC_CONTINUE){
#ifndef EXCEPTIONS_DEFINED
    if (frame.getContext().getError().getErrorCode()!=skScriptError::NONE)
      break;
#endif
    skRValue rExpr=evaluate(frame,n->getExpr());
    if (rExpr.boolValue()){
      if (n->getStats())
        bRet=executeStats(frame,n->getStats(),r);
    }else
      break;
  }
  // once broken out, can continue executing
  if (bRet==SRC_BREAK)
    bRet=SRC_CONTINUE;
  return bRet;
}
//---------------------------------------------------
skInterpreter::StatementReturnCode skInterpreter::executeIfStat(skStackFrame& frame,skIfNode * n,skRValue& r)
  //---------------------------------------------------
{
  StatementReturnCode bRet=SRC_CONTINUE;
  skRValue rExpr;
  SAVE_VARIABLE(rExpr);
  rExpr=evaluate(frame,n->getExpr());
#ifndef EXCEPTIONS_DEFINED
  if (frame.getContext().getError().getErrorCode()==skScriptError::NONE){
#endif
    if (rExpr.boolValue()){
      if (n->getStats())
        bRet=executeStats(frame,n->getStats(),r);
    }else
      if (n->getElse())         
        bRet=executeStats(frame,n->getElse(),r);
#ifndef EXCEPTIONS_DEFINED
  }
#endif
  RELEASE_VARIABLE(rExpr);
  return bRet;
}
//---------------------------------------------------
skInterpreter::StatementReturnCode skInterpreter::executeSwitchStat(skStackFrame& frame,skSwitchNode * n,skRValue& r)
  //---------------------------------------------------
{
  StatementReturnCode bRet=SRC_CONTINUE;
  skRValue expr;
  SAVE_VARIABLE(expr);
  expr=evaluate(frame,n->getExpr());
  skCaseNode * caseFound=0;
  skCaseListIterator iter(n->getCases()->getCases());
  skCaseNode * caseNode=0;
  skRValue testExpr;
  SAVE_VARIABLE(testExpr);
  while ((caseNode=iter())!=0){
#ifndef EXCEPTIONS_DEFINED
    if (frame.getContext().getError().getErrorCode()!=skScriptError::NONE)
      break;
#endif
    testExpr=evaluate(frame,caseNode->getExpr());
    if (testExpr==expr){
      caseFound=caseNode;
      break;
    }
  }
#ifndef EXCEPTIONS_DEFINED
  if (frame.getContext().getError().getErrorCode()==skScriptError::NONE){
#endif
    if (caseFound!=0)
      bRet=executeStats(frame,caseFound->getStats(),r);
    else if (n->getDefault()!=0)
      bRet=executeStats(frame,n->getDefault(),r);
#ifndef EXCEPTIONS_DEFINED
  }
#endif
  RELEASE_VARIABLE(testExpr);
  RELEASE_VARIABLE(expr);
  return bRet;
}
//---------------------------------------------------
skInterpreter::StatementReturnCode skInterpreter::executeForEachStat(skStackFrame& frame,skForEachNode * n,skRValue& r)
  //---------------------------------------------------
{
  StatementReturnCode bRet=SRC_CONTINUE;
  skString checked_id;
  SAVE_VARIABLE(checked_id);
     checked_id=checkIndirectId(frame,n->getId());
  skRValue expr;
  SAVE_VARIABLE(expr);
  expr=evaluate(frame,n->getExpr());
  if (expr.type()==skRValue::T_Object){
    skExecutableIterator * iterator=0;
    if (n->getQualifier().length())
      iterator=expr.obj()->createIterator(n->getQualifier());
    else
      iterator=expr.obj()->createIterator();
    if (iterator){
      SAVE_POINTER(iterator);
      skRValue value;
      SAVE_VARIABLE(value);
      while ((iterator->next(value) && bRet==SRC_CONTINUE)){
#ifndef EXCEPTIONS_DEFINED
        if (frame.getContext().getError().getErrorCode()!=skScriptError::NONE)
          break;
#endif
        addLocalVariable(frame.getVars(),checked_id,value);
        if (n->getStats()){
          bRet=executeStats(frame,n->getStats(),r);
        }
      }
      RELEASE_VARIABLE(value);
      RELEASE_POINTER(iterator);
      delete iterator;
    }else
      runtimeError(frame,skSTR("Object could not create an iterator, in a foreach statement\n"));
  }else
    runtimeError(frame,skSTR("Cannot apply foreach to a non-executable object\n"));
  RELEASE_VARIABLE(expr);
  RELEASE_VARIABLE(checked_id);
  // once broken out, can continue executing
  if (bRet==SRC_BREAK)
    bRet=SRC_CONTINUE;
  return bRet;
}
//---------------------------------------------------
skInterpreter::StatementReturnCode skInterpreter::executeForStat(skStackFrame& frame,skForNode * n,skRValue& r)
  //---------------------------------------------------
{
  StatementReturnCode bRet=SRC_CONTINUE;
  skString checked_id;
  SAVE_VARIABLE(checked_id);
  checked_id=checkIndirectId(frame,n->getId());
  skRValue start_expr;
  SAVE_VARIABLE(start_expr);
  start_expr=evaluate(frame,n->getStartExpr());
  int start_value=start_expr.intValue();
  skRValue end_expr;
  SAVE_VARIABLE(end_expr);
  end_expr=evaluate(frame,n->getEndExpr());
  int end_value=end_expr.intValue();
  int step_value=1;
  if (n->getStepExpr()){
    skRValue step_expr=evaluate(frame,n->getStepExpr());
    step_value=step_expr.intValue();
  }
  if (start_value!=end_value){
    if (step_value>0){
      if (end_value>start_value){
        for (int i=start_value;i<end_value;i+=step_value){
#ifndef EXCEPTIONS_DEFINED
          if (frame.getContext().getError().getErrorCode()!=skScriptError::NONE)
            break;
#endif
          addLocalVariable(frame.getVars(),checked_id,i);
          if (n->getStats()){
            bRet=executeStats(frame,n->getStats(),r);
            if (bRet!=SRC_CONTINUE)
              break;
          }
        }
      }else
        runtimeError(frame,skSTR("End value is not greater than the start value in a for statement\n"));
    }else{
      if (step_value<0){
        if (start_value>end_value){
          for (int i=start_value;i>end_value;i+=step_value){
#ifndef EXCEPTIONS_DEFINED
            if (frame.getContext().getError().getErrorCode()!=skScriptError::NONE)
              break;
#endif
            addLocalVariable(frame.getVars(),checked_id,i);
            if (n->getStats()){
              bRet=executeStats(frame,n->getStats(),r);
              if (bRet!=SRC_CONTINUE)
                break;
            }
          }
        }else
          runtimeError(frame,skSTR("Start value is not greater than the end value in a for statement\n"));
      }else
        runtimeError(frame,skSTR("Cannot use a zero step increment in a for statement\n"));
    }
  }// else do nothing
  RELEASE_VARIABLE(end_expr);
  RELEASE_VARIABLE(start_expr);
  RELEASE_VARIABLE(checked_id);
  // once broken out, can continue executing
  if (bRet==SRC_BREAK)
    bRet=SRC_CONTINUE;
  return bRet;
}
//---------------------------------------------------
skRValue skInterpreter::findValue(skStackFrame& frame,const skString& name,skExprNode * array_index,const skString& attrib)
  //---------------------------------------------------
{              
  skRValue r;
  SAVE_VARIABLE(r);
  skString valueName;
  SAVE_VARIABLE(valueName);
  skString blank;
  valueName=checkIndirectId(frame,name);
  if (valueName.length()){
    // first check some built-ins: true, false and self
    if (valueName==s_true)
      r=true;
    else if (valueName==s_false)
      r=false;
    else if (valueName==s_null)
      r.assignObject(&m_Null);
    else if (valueName==s_self){
      if (attrib.length() || array_index){
        skRValue caller;
        SAVE_VARIABLE(caller);
        caller.assignObject(frame.getObject());
        if (array_index){
          extractArrayValue(frame,caller,array_index,attrib,r);
        }else
          if (extractValue(frame,caller,blank,attrib,r)==false)
            runtimeError(frame,skString::addStrings(s_CannotGetAttribute,attrib.ptr(),s_cr));
        RELEASE_VARIABLE(caller);
      }else
        r.assignObject(frame.getObject());
    }else{
      // otherwise look up the scope hierarchy
      skRValue * pvalue=0;
      // first in the local variables
      pvalue=frame.getVars().value(valueName);
      if (pvalue){
        r=*pvalue;
        if (attrib.length() || array_index){
          skRValue result;
          SAVE_VARIABLE(result);
          if (array_index){
            extractArrayValue(frame,r,array_index,attrib,result);
          }else
            if (extractValue(frame,r,blank,attrib,result)==false)
              runtimeError(frame,skString::addStrings(s_CannotGetAttribute,attrib.ptr(),s_cr));
          r=result;
          RELEASE_VARIABLE(result);
        }
      }else{
        // then in the instance fields
        skRValue caller;
        SAVE_VARIABLE(caller);
        caller.assignObject(frame.getObject());
        bool found=false;
        if (array_index)
          found=extractFieldArrayValue(frame,caller,valueName,array_index,attrib,r);
        else
          found=extractValue(frame,caller,valueName,attrib,r);
        if (found==false){
          // and finally in the global variables
          pvalue=m_GlobalVars.value(valueName);
          if (pvalue){
            r=*pvalue;
            if (attrib.length() || array_index){
              skRValue result;
              SAVE_VARIABLE(result);
              if (array_index){
                extractArrayValue(frame,r,array_index,attrib,result);
                r=result;
              }else
                if (extractValue(frame,r,blank,attrib,result)==false)
                  runtimeError(frame,skString::addStrings(s_CannotGetAttribute,attrib.ptr(),s_cr));
              r=result;
              RELEASE_VARIABLE(result);
            }
          }else
            runtimeError(frame,skString::addStrings(s_FieldStart,valueName.ptr(),s_NotFound));
        }       
        RELEASE_VARIABLE(caller);
      } 
    }
  }
  RELEASE_VARIABLE(valueName);
  RELEASE_VARIABLE(r);
  return r;
}
//---------------------------------------------------
bool skInterpreter::extractFieldArrayValue(skStackFrame& frame,skRValue& robject,const skString& field_name,skExprNode * array_index,const skString& attrib,skRValue& ret) 
  //---------------------------------------------------
{
  skRValue array_field;
  SAVE_VARIABLE(array_field);
  skString blank;
  bool found=extractValue(frame,robject,field_name,blank,array_field);
  if (found){
    extractArrayValue(frame,array_field,array_index,attrib,ret);
  }
  RELEASE_VARIABLE(array_field);
  return found;
}
//---------------------------------------------------
bool skInterpreter::extractArrayValue(skStackFrame& frame,skRValue& robject,skExprNode * array_index,const skString& attrib,skRValue& ret) 
  //---------------------------------------------------
{
  bool found=false;
  if (robject.type()==skRValue::T_Object){
    skRValue rExpr;
    SAVE_VARIABLE(rExpr);
    rExpr=evaluate(frame,array_index);
    found=robject.obj()->getValueAt(rExpr,attrib,ret);
    if (found==false)
      runtimeError(frame,skSTR("Cannot get an array member\n"));
    RELEASE_VARIABLE(rExpr);
  }else
    runtimeError(frame,skSTR("Cannot get an array member from a non-object\n"));
  return found;
}
//---------------------------------------------------
void skInterpreter::executeAssignStat(skStackFrame& frame,skAssignNode * n)
  //---------------------------------------------------
{
  skRValue value;
  SAVE_VARIABLE(value);
  value=evaluate(frame,n->getExpr());
    skString blank;
#ifndef EXCEPTIONS_DEFINED
  if (frame.getContext().getError().getErrorCode()==skScriptError::NONE){
#endif
    skIdNode * idNode=n->getIds()->getLastId();
    skString field_name;
    SAVE_VARIABLE(field_name);
    field_name=checkIndirectId(frame,idNode->getId());
    if (n->getIds()->numIds()==1){
      // special case where there is a single id
      bool inserted=false;
      if (frame.getObject()!=0){
        skRValue caller;
        SAVE_VARIABLE(caller);
        caller.assignObject(frame.getObject());
        if (field_name==s_self){
          if (idNode->getArrayIndex())
            inserted=insertArrayValue(frame,caller,idNode->getArrayIndex(),n->getIds()->getAttribute(),value);
          else
            inserted=insertValue(frame,caller,blank,n->getIds()->getAttribute(),value);
        }else{
          if (idNode->getArrayIndex()){
            caller=findValue(frame,field_name,0,blank);
            inserted=insertArrayValue(frame,caller,idNode->getArrayIndex(),n->getIds()->getAttribute(),value);
          }else{
            if (n->getIds()->getAttribute().length()>0){
              // e.g. "field:name"
              caller.assignObject(frame.getObject());
              inserted=insertValue(frame,caller,field_name,n->getIds()->getAttribute(),value);
              if (inserted==false)
                runtimeError(frame,skString::addStrings(s_SettingAttribute,n->getIds()->getAttribute().ptr(),s_OnMiddle,field_name.ptr(),s_Failed));
            }else
              inserted=insertValue(frame,caller,field_name,n->getIds()->getAttribute(),value);
          }
        }
        RELEASE_VARIABLE(caller);
      }
      if (inserted==false)
        // if the object doesn't want this variable, we add it as a local variable
        addLocalVariable(frame.getVars(),field_name,value);
    }else{
      // otherwise follow the id's to the penultimate one
      skRValue robject;
      SAVE_VARIABLE(robject);
      followIdList(frame,n->getIds(),robject);
      if (idNode->getArrayIndex()){
        skRValue array_field;
        SAVE_VARIABLE(array_field);
        if (extractValue(frame,robject,field_name,blank,array_field)){
          insertArrayValue(frame,array_field,idNode->getArrayIndex(),n->getIds()->getAttribute(),value);
        }else{
          runtimeError(frame,skString::addStrings(s_CannotFindField,field_name.ptr(),s_cr));
        }
        RELEASE_VARIABLE(array_field);
      }else
        insertValue(frame,robject,field_name,n->getIds()->getAttribute(),value);
      RELEASE_VARIABLE(robject);
    }
    RELEASE_VARIABLE(field_name);
#ifndef EXCEPTIONS_DEFINED
  }
#endif
  RELEASE_VARIABLE(value);
}
//---------------------------------------------------
bool skInterpreter::insertArrayValue(skStackFrame& frame,skRValue& robject, skExprNode * array_index,const skString& attr,const skRValue& value)
  //---------------------------------------------------
{
  bool found=false;
  if (robject.type()==skRValue::T_Object){
    skRValue rExpr;
    SAVE_VARIABLE(rExpr);
    rExpr=evaluate(frame,array_index);
    found=robject.obj()->setValueAt(rExpr,attr,value);
    if (found==false)
      runtimeError(frame,skSTR("Setting array member failed\n"));
    RELEASE_VARIABLE(rExpr);
  }else
    runtimeError(frame,skSTR("Cannot set an array member on a non-object\n"));
  return found;
}
//---------------------------------------------------
skRValue skInterpreter::evaluate(skStackFrame& frame,skExprNode * n)
  //---------------------------------------------------
{ 
  skRValue r;
  skString blank;
  SAVE_VARIABLE(r);
  skRValue item1;
  SAVE_VARIABLE(item1);
  skRValue item2;
  SAVE_VARIABLE(item2);
  switch(n->getType()){
  case s_IdList:{
    skIdListNode * ids=(skIdListNode *)n;
    skIdNode * idNode=ids->getLastId();
    skString method_name;
    SAVE_VARIABLE(method_name);
    method_name=idNode->getId();
    if (ids->numIds()==1){
      if (idNode->getExprs()==0)
        r=findValue(frame,method_name,idNode->getArrayIndex(),ids->getAttribute());
      else{
        skRValue caller;
        caller.assignObject(frame.getObject());
        makeMethodCall(frame,caller,method_name,idNode->getArrayIndex(),ids->getAttribute(),idNode->getExprs(),r);
      }
    }else{
      method_name=checkIndirectId(frame,method_name);
      skRValue robject;
      SAVE_VARIABLE(robject);
      followIdList(frame,ids,robject);
      if (idNode->getExprs()==0){
        if (idNode->getArrayIndex()){
          skRValue array_field;
          if (extractValue(frame,robject,method_name,blank,array_field))
            extractArrayValue(frame,array_field,idNode->getArrayIndex(),ids->getAttribute(),r);
          else
            runtimeError(frame,skString::addStrings(s_FieldError,method_name.ptr(),s_cr));
        }else
          if (extractValue(frame,robject,method_name,ids->getAttribute(),r)==false)
            runtimeError(frame,skString::addStrings(s_FieldError,method_name.ptr(),s_cr));
      }else
        makeMethodCall(frame,robject,method_name,idNode->getArrayIndex(),ids->getAttribute(),idNode->getExprs(),r);
      RELEASE_VARIABLE(robject);
    }
    RELEASE_VARIABLE(method_name);
    break;
  }
  case s_String:
    r=skRValue(((skLiteralNode *)n)->getString());
    break;
  case s_Integer:
    r=skRValue(((skLiteralNode *)n)->getInt());
    break;
  case s_Character:
    r=skRValue(((skLiteralNode *)n)->getChar());
    break;
#ifdef USE_FLOATING_POINT
  case s_Float:
    r=skRValue(((skLiteralNode *)n)->getFloat());
    break;
#endif
  default:{
    skOpNode * opNode=(skOpNode *)n;
    item1=evaluate(frame,opNode->getExpr1());
    int item1Type=item1.type();
    switch(n->getType()){
    case s_Not:
      r=bool(!item1.boolValue());
      break;
    case s_And:
      r=bool(item1.boolValue() && evaluate(frame,opNode->getExpr2()).boolValue());
      break;
    case s_Or:
      r=bool(item1.boolValue() || evaluate(frame,opNode->getExpr2()).boolValue());
      break;
    case s_Equals:
      r=bool(item1 == evaluate(frame,opNode->getExpr2()));
      break;
    case s_NotEquals:{
      bool equals=item1 == evaluate(frame,opNode->getExpr2());
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
      r=skRValue(skString::addStrings(item1.str(),evaluate(frame,opNode->getExpr2()).str()));
      break;
    default:{
      item2=evaluate(frame,opNode->getExpr2());
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
          r=bool(val1>val2);
          break;
        case s_MoreEqual:
          r=bool(val1>=val2);
          break;
        case s_Less:
          r=bool(val1<val2);
          break;
        case s_LessEqual:
          r=bool(val1<=val2);
          break;
        case s_Subtract:
          r=skRValue(val1-val2);
          break;
        case s_Divide:{
          long top=val1;
          long bottom=val2;
          if (bottom)
            r=skRValue((int)(top/bottom));
          else{
            r=skRValue((int)1);
            runtimeError(frame,skSTR("Divide by zero error"));
          }
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
          r=bool(val1>val2);
          break;
        case s_MoreEqual:
          r=bool(val1>=val2);
          break;
        case s_Less:
          r=bool(val1<val2);
          break;
        case s_LessEqual:
          r=bool(val1<=val2);
          break;
        case s_Subtract:
          r=skRValue(val1-val2);
          break;
        case s_Divide:{
          double top=val1;
          double bottom=val2;
          if (bottom)
            r=skRValue((float)(top/bottom));
          else{
            r=skRValue(1.0f);
            runtimeError(frame,skSTR("Divide by zero error"));
          }
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
  }
  RELEASE_VARIABLE(item2);
  RELEASE_VARIABLE(item1);
  RELEASE_VARIABLE(r);
  return r;
}
//------------------------------------------
void skInterpreter::evaluateExpression(const skString& location,skiExecutable * obj,
                                       skExprNode * expression,skRValueTable&  vars,
                                       skRValue& return_value,skExecutableContext& ctxt)
  //------------------------------------------
{
  skStackFrame frame(location,obj,vars,ctxt);
  return_value=evaluate(frame,expression);
}
#endif
