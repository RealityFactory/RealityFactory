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

  $Id: skInterpreterByte.cpp,v 1.12 2004/12/17 21:31:17 sdw Exp $
*/
#include "skInterpreter.h"
#include "skRValueArray.h"
#include "skStackFrame.h"
#include "skStatementStepper.h"
#ifndef EXECUTE_PARSENODES
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
inline void skInterpreter::getIdNodes(skCompiledCode& code,USize& pc,int& num_ids,skString& attribute)
//---------------------------------------------------
{
  int attrib_id;
  skCompiledCode::skInstruction ins;
  code.getInstruction(pc++,ins,num_ids,attrib_id);
  assert(ins==skCompiledCode::b_IdList);
  attribute=code.getId(attrib_id);
}
//---------------------------------------------------
inline void skInterpreter::getIdNode(skCompiledCode& code,USize& pc,skString& id,bool& has_array,bool& is_method)
//---------------------------------------------------
{
  int id_index;
  skCompiledCode::skInstruction ins;
  code.getInstruction(pc++,ins,id_index,has_array);
  assert(ins==skCompiledCode::b_Id || ins==skCompiledCode::b_IdWithMethod);
  is_method=(ins==skCompiledCode::b_IdWithMethod);
  id=code.getId(id_index);
}
//---------------------------------------------------
skRValue skInterpreter::evaluate(skStackFrame& frame,skCompiledCode& code,USize& pc)
  //---------------------------------------------------
{ 
  skRValue r;
  SAVE_VARIABLE(r);
  skRValue item1;
  SAVE_VARIABLE(item1);
  skRValue item2;
  SAVE_VARIABLE(item2);
  int param1;
  int param2;
  skCompiledCode::skInstruction ins;
  code.getInstruction(pc++,ins,param1,param2);
  switch(ins){
  case skCompiledCode::b_IdList:{
    skString method_name;
    SAVE_VARIABLE(method_name);
    int num_ids=param1;
    skString attribute;
    SAVE_VARIABLE(attribute);
    attribute=code.getId(param2);
    if (num_ids==1){
      bool has_array_index;
      bool is_method;
      getIdNode(code,pc,method_name,has_array_index,is_method);
      if (is_method==false){
        if (has_array_index){
          skRValue array_index;
          SAVE_VARIABLE(array_index);
          array_index=evaluate(frame,code,pc);
          r=findValue(frame,method_name,&array_index,attribute);
          RELEASE_VARIABLE(array_index);
        }else{
          r=findValue(frame,method_name,0,attribute);
        }
      }else{
        skRValue caller;
        caller.assignObject(frame.getObject());
        makeMethodCall(frame,code,pc,caller,method_name,has_array_index,attribute,r);
      }
    }else{
      skRValue robject;
      SAVE_VARIABLE(robject);
      followIdList(frame,code,pc,num_ids,robject);
      bool has_array_index;
      bool is_method;
      skString blank;
      getIdNode(code,pc,method_name,has_array_index,is_method);
      if (is_method==false){
        if (has_array_index){
          skRValue array_field;
          SAVE_VARIABLE(array_field);
          skRValue array_index;
          SAVE_VARIABLE(array_index);
          array_index=evaluate(frame,code,pc);
          if (extractValue(frame,robject,method_name,blank,array_field))
            extractArrayValue(frame,array_field,array_index,attribute,r);
          else
            runtimeError(frame,skString::addStrings(s_FieldError,method_name.ptr(),s_cr));
          RELEASE_VARIABLE(array_index);
          RELEASE_VARIABLE(array_field);
        }else
          if (extractValue(frame,robject,method_name,attribute,r)==false)
            runtimeError(frame,skString::addStrings(s_FieldError,method_name.ptr(),s_cr));
      }else
        makeMethodCall(frame,code,pc,robject,method_name,has_array_index,attribute,r);
      RELEASE_VARIABLE(robject);
    }
    RELEASE_VARIABLE(attribute);
    RELEASE_VARIABLE(method_name);
    break;
  }
  case skCompiledCode::b_String:
    r=code.getString(param1);
    break;
  case skCompiledCode::b_IntInline:
    r=param1;
    break;
  case skCompiledCode::b_Int:
    r=code.getInt(param1);
    break;
  case skCompiledCode::b_Char:
    r=(Char)param1;
    break;
#ifdef USE_FLOATING_POINT
  case skCompiledCode::b_Float:
    r=code.getFloat(param1);
    break;
#endif
  default:{
    assert(ins==skCompiledCode::b_Op);
    item1=evaluate(frame,code,pc);
    if (param2)
      item2=evaluate(frame,code,pc);
    int item1Type=item1.type();
    switch(param1){
    case s_Not:
      r=bool(!item1.boolValue());
      break;
    case s_And:
      r=bool(item1.boolValue() && item2.boolValue());
      break;
    case s_Or:
      r=bool(item1.boolValue() || item2.boolValue());
      break;
    case s_Equals:
      r=(bool)(item1 == item2);
      break;
    case s_NotEquals:{
      bool equals=(item1 == item2);
      if (equals)
        r=false;
      else
        r=true;
      break;
    }
    case s_Minus:
#ifdef USE_FLOATING_POINT
      if (item1Type==skRValue::T_Float)
        r=skRValue(-item1.floatValue());
      else
#endif
        r=skRValue((-item1.intValue()));
      break;
    case s_Concat:
      r=skRValue(skString::addStrings(item1.str(),item2.str()));
      break;
    default:{
      int item2Type=item2.type();
#ifdef USE_FLOATING_POINT
      if (item1Type==skRValue::T_Int && item2Type==skRValue::T_Int){
#endif
        // we can use integer arithmetic if both objects are integer
        int val1=item1.intValue();
        int val2=item2.intValue();
        switch(param1){
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
#ifdef USE_FLOATING_POINT
      }else{
        // in all other cases we using floating point
        float val1=item1.floatValue();
        float val2=item2.floatValue();
        switch(param1){
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
#endif
    }
    }
  }
  }
  RELEASE_VARIABLE(item2);
  RELEASE_VARIABLE(item1);
  RELEASE_VARIABLE(r);
  return r;
}
//---------------------------------------------------
skRValue skInterpreter::findValue(skStackFrame& frame,const skString& name,const skRValue * array_index,const skString& attrib)
  //---------------------------------------------------
{              
  skRValue r;
  SAVE_VARIABLE(r);
  skString valueName;
  skString blank;
  SAVE_VARIABLE(valueName);
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
          extractArrayValue(frame,caller,*array_index,attrib,r);
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
            extractArrayValue(frame,r,*array_index,attrib,result);
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
          found=extractFieldArrayValue(frame,caller,valueName,*array_index,attrib,r);
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
                extractArrayValue(frame,r,*array_index,attrib,result);
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
bool skInterpreter::extractFieldArrayValue(skStackFrame& frame,skRValue& robject,const skString& field_name,const skRValue& array_index,const skString& attrib,skRValue& ret)
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
void skInterpreter::followIdList(skStackFrame& frame,skCompiledCode& code,USize& pc,int num_ids,skRValue& object)
  //---------------------------------------------------
{
  // skip down the id.id.id list, resolving each as we go along, we exclude the final id in the list
  skString name;
  SAVE_VARIABLE(name);
  skRValue array_index;
  SAVE_VARIABLE(array_index);
  bool has_array_index;
  bool is_method;
  skString blank;
  getIdNode(code,pc,name,has_array_index,is_method);
  if (has_array_index)
    array_index=evaluate(frame,code,pc);
  //  trace("followIdList: %s - %d ids\n",(const char *)name,idList->m_Ids.entries());
  if (is_method==false){
    if (has_array_index)
      object=findValue(frame,name,&array_index,blank);
    else
      object=findValue(frame,name,0,blank);
  }else{
    skRValue caller;
    SAVE_VARIABLE(caller);
    caller.assignObject(frame.getObject(),false);
    makeMethodCall(frame,code,pc,caller,name,has_array_index,blank,object);
    RELEASE_VARIABLE(caller);
  }
  for (int i=1;i<num_ids-1;i++){
#ifndef EXCEPTIONS_DEFINED
    if (frame.getContext().getError().getErrorCode()!=skScriptError::NONE)
      break;
#endif
    getIdNode(code,pc,name,has_array_index,is_method);
    //    trace("followIdList: %d: %s\n",i,(const char *)name);
    name=checkIndirectId(frame,name);
    skRValue result;
    SAVE_VARIABLE(result);
    if (is_method==false){
      if (has_array_index)
        extractFieldArrayValue(frame,object,name,array_index,blank,result);
      else
        if (extractValue(frame,object,name,blank,result)==false)
          runtimeError(frame,skString::addStrings(s_FieldError,name.ptr(),s_cr));
    }else
      makeMethodCall(frame,code,pc,object,name,has_array_index,blank,result);
    object=result;
    RELEASE_VARIABLE(result);
  }
  RELEASE_VARIABLE(array_index);
  RELEASE_VARIABLE(name);
}
//---------------------------------------------------
bool skInterpreter::insertArrayValue(skStackFrame& frame,skCompiledCode& code,USize& pc,skRValue& robject, const skString& attr,const skRValue& value)
  //---------------------------------------------------
{
  bool found=false;
  if (robject.type()==skRValue::T_Object){
    skRValue rExpr;
    SAVE_VARIABLE(rExpr);
    rExpr=evaluate(frame,code,pc);
    found=robject.obj()->setValueAt(rExpr,attr,value);
    if (found==false)
      runtimeError(frame,skSTR("Setting array member failed\n"));
    RELEASE_VARIABLE(rExpr);
  }else
    runtimeError(frame,skSTR("Cannot set an array member on a non-object\n"));
  return found;
}
//---------------------------------------------------
bool skInterpreter::extractArrayValue(skStackFrame& frame,skRValue& robject,const skRValue& array_index,const skString& attrib,skRValue& ret) 
  //---------------------------------------------------
{
  bool found=false;
  if (robject.type()==skRValue::T_Object){
    found=robject.obj()->getValueAt(array_index,attrib,ret);
    if (found==false)
      runtimeError(frame,skSTR("Cannot get an array member\n"));
  }else
    runtimeError(frame,skSTR("Cannot get an array member from a non-object\n"));
  return found;
}
//---------------------------------------------------
void skInterpreter::makeMethodCall(skStackFrame& frame,skCompiledCode& code,USize& pc,skRValue& robject,const skString& method_name,bool has_array_index,const skString& attribute, skRValue& ret)
  //---------------------------------------------------
{
  skString checked_method_name=checkIndirectId(frame,method_name);
  skRValue array_index;
  SAVE_VARIABLE(array_index);
  if (has_array_index)
    array_index=evaluate(frame,code,pc);
  if (robject.type()==skRValue::T_Object){
    skRValueArray args;
    SAVE_VARIABLE(robject);
    int dummy;
    int num_exprs;
    skCompiledCode::skInstruction ins;
    code.getInstruction(pc++,ins,dummy,num_exprs);
    assert(ins==skCompiledCode::b_ExprList);
    for (int i=0;i<num_exprs;i++){
#ifndef EXCEPTIONS_DEFINED
      if (frame.getContext().getError().getErrorCode()!=skScriptError::NONE)
        break;
#endif
      args.append(evaluate(frame,code,pc));
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
      if (has_array_index){
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
  RELEASE_VARIABLE(array_index);
}
//---------------------------------------------------
skRValue  skInterpreter::evalMethod(skStackFrame& frame,skCompiledCode& code,USize& pc)
//---------------------------------------------------
{
  skRValue ret;
  SAVE_VARIABLE(ret);
  int num_ids;
  skString attribute;
  SAVE_VARIABLE(attribute);
  getIdNodes(code,pc,num_ids,attribute);
  if (num_ids==1){
    skString method_name;
    SAVE_VARIABLE(method_name);
    bool has_array_index;
    bool is_method;
    getIdNode(code,pc,method_name,has_array_index,is_method);
    assert(is_method==true);
    skRValue caller;
    SAVE_VARIABLE(caller);
    caller.assignObject(frame.getObject());
    makeMethodCall(frame,code,pc,caller,method_name,has_array_index,attribute,ret);
    RELEASE_VARIABLE(caller);
    RELEASE_VARIABLE(method_name);
  }else{
    // follow the chain of Id's and then call the method
    skRValue robject;
    SAVE_VARIABLE(robject);
    followIdList(frame,code,pc,num_ids,robject);
    skString method_name;
    bool has_array_index;
    bool is_method;
    getIdNode(code,pc,method_name,has_array_index,is_method);
    assert(is_method==true);
    makeMethodCall(frame,code,pc,robject,method_name,has_array_index,attribute,ret);
    RELEASE_VARIABLE(robject);
  }
  RELEASE_VARIABLE(attribute);
  RELEASE_VARIABLE(ret);
  return ret;
}
//---------------------------------------------------
skInterpreter::StatementReturnCode skInterpreter::executeReturnStat(skStackFrame& frame,skCompiledCode& code,USize& pc,skRValue& r,bool has_expr)
  //---------------------------------------------------
{
  if (has_expr)
    r=evaluate(frame,code,pc);
  return SRC_RETURN;
}
//---------------------------------------------------
skInterpreter::StatementReturnCode skInterpreter::executeBreakStat(skStackFrame& frame,skCompiledCode& code,USize& pc,skRValue& r)
  //---------------------------------------------------
{
  return SRC_BREAK;
}
//---------------------------------------------------
skInterpreter::StatementReturnCode skInterpreter::executeWhileStat(skStackFrame& frame,skCompiledCode& code,USize& pc,skRValue& r)
  //---------------------------------------------------
{
  StatementReturnCode bRet=SRC_CONTINUE;
  int num_bytes=0;
  int dummy;
  skCompiledCode::skInstruction ins;
  code.getInstruction(pc++,ins,dummy,num_bytes);
  assert(ins==skCompiledCode::b_StatsSize);
  USize start_pc=pc;
  while(bRet==SRC_CONTINUE){
    // reset the PC
    pc=start_pc;
#ifndef EXCEPTIONS_DEFINED
    if (frame.getContext().getError().getErrorCode()!=skScriptError::NONE)
      break;
#endif
    skRValue rExpr=evaluate(frame,code,pc);
    if (rExpr.boolValue()){
      bRet=executeStats(frame,code,pc,num_bytes,true,r);
    }else{
      // advance to the end of the statements
      pc+=num_bytes;
      break;
    }
  }
  // once broken out, can continue executing
  if (bRet==SRC_BREAK)
    bRet=SRC_CONTINUE;
  return bRet;
}
//---------------------------------------------------
skInterpreter::StatementReturnCode  skInterpreter::executeIfStat(skStackFrame& frame,skCompiledCode& code,USize& pc,skRValue& r,bool has_else)
  //---------------------------------------------------
{
  StatementReturnCode bRet=SRC_CONTINUE;
  int num_bytes;
  skRValue rExpr;
  SAVE_VARIABLE(rExpr);
  rExpr=evaluate(frame,code,pc);
#ifndef EXCEPTIONS_DEFINED
  if (frame.getContext().getError().getErrorCode()==skScriptError::NONE){
#endif
    if (rExpr.boolValue()){
      bRet=executeStats(frame,code,pc,num_bytes,true,r);
      if (has_else)
        executeStats(frame,code,pc,num_bytes,false,r);
    }else{
      bRet=executeStats(frame,code,pc,num_bytes,false,r);
      if (has_else)
        bRet=executeStats(frame,code,pc,num_bytes,true,r);
    }
#ifndef EXCEPTIONS_DEFINED
  }
#endif
  RELEASE_VARIABLE(rExpr);
  return bRet;
}
//---------------------------------------------------
skInterpreter::StatementReturnCode skInterpreter::executeSwitchStat(skStackFrame& frame,skCompiledCode& code,USize& pc,skRValue& r,bool has_default)
  //---------------------------------------------------
{
  StatementReturnCode bRet=SRC_CONTINUE;
  skRValue expr;
  SAVE_VARIABLE(expr);
  expr=evaluate(frame,code,pc);
  USize start_pc=pc;
  int num_cases=0;
  int num_bytes=0;
  skCompiledCode::skInstruction ins;
  code.getInstruction(pc++,ins,num_cases,num_bytes);
  assert(ins==skCompiledCode::b_CaseList);
  skRValue testExpr;
  SAVE_VARIABLE(testExpr);
  bool case_found=false;
  for (int i=0;i<num_cases;i++){
#ifndef EXCEPTIONS_DEFINED
    if (frame.getContext().getError().getErrorCode()!=skScriptError::NONE)
      break;
#endif
    int num_stat_bytes=0;
    testExpr=evaluate(frame,code,pc);
    if (testExpr==expr){
      case_found=true;
      bRet=executeStats(frame,code,pc,num_stat_bytes,true,r);
      break;
    }else{
      // skip past the byte codes for this case statement
      bRet=executeStats(frame,code,pc,num_stat_bytes,false,r);
    }
  }
  // skip to the end of the case list
  pc=start_pc+num_bytes;
#ifndef EXCEPTIONS_DEFINED
  if (frame.getContext().getError().getErrorCode()==skScriptError::NONE){
#endif
    if (has_default){
      // execute or ignore the default clause, if it exists
      int num_stat_bytes=0;
      bRet=executeStats(frame,code,pc,num_stat_bytes,case_found==false,r);
    }
#ifndef EXCEPTIONS_DEFINED
  }
#endif
  RELEASE_VARIABLE(testExpr);
  RELEASE_VARIABLE(expr);
  return bRet;
}
//---------------------------------------------------
skInterpreter::StatementReturnCode skInterpreter::executeForEachStat(skStackFrame& frame,skCompiledCode& code,USize& pc,skRValue& r,USize id_index)
  //---------------------------------------------------
{
  StatementReturnCode bRet=SRC_CONTINUE;
  int num_bytes=0;
  skString checked_id;
  SAVE_VARIABLE(checked_id);
  checked_id=checkIndirectId(frame,code.getId(id_index));
  int qualifier_index;
  skCompiledCode::skInstruction ins;
  code.getInstruction(pc++,ins,qualifier_index,num_bytes);
  assert(ins==skCompiledCode::b_QualifierIndex);
  skString qualifier;
  SAVE_VARIABLE(qualifier);
  qualifier=code.getId(qualifier_index);
  skRValue expr;
  SAVE_VARIABLE(expr);
  expr=evaluate(frame,code,pc);
  if (expr.type()==skRValue::T_Object){
    skExecutableIterator * iterator=0;
    if (qualifier.length())
      iterator=expr.obj()->createIterator(qualifier);
    else
      iterator=expr.obj()->createIterator();
    if (iterator){
      SAVE_POINTER(iterator);
      skRValue value;
      SAVE_VARIABLE(value);
      USize loop_pc=pc;
      while ((iterator->next(value) && bRet==SRC_CONTINUE)){
#ifndef EXCEPTIONS_DEFINED
        if (frame.getContext().getError().getErrorCode()!=skScriptError::NONE)
          break;
#endif
        addLocalVariable(frame.getVars(),checked_id,value);
        // reset PC at each iteration
        pc=loop_pc;
        bRet=executeStats(frame,code,pc,num_bytes,true,r);
      }
      // advance beyond the statements
      pc=loop_pc+num_bytes;
      RELEASE_VARIABLE(value);
      RELEASE_POINTER(iterator);
      delete iterator;
    }else
      runtimeError(frame,skSTR("Object could not create an iterator, in a foreach statement\n"));
  }else
    runtimeError(frame,skSTR("Cannot apply foreach to a non-executable object\n"));
  RELEASE_VARIABLE(expr);
  RELEASE_VARIABLE(qualifier);
  RELEASE_VARIABLE(checked_id);
  // once broken out, can continue executing
  if (bRet==SRC_BREAK)
    bRet=SRC_CONTINUE;
  return bRet;
}
//---------------------------------------------------
skInterpreter::StatementReturnCode  skInterpreter::executeForStat(skStackFrame& frame,skCompiledCode& code,USize& pc,skRValue& r,bool has_step)
  //---------------------------------------------------
{
  StatementReturnCode bRet=SRC_CONTINUE;
  int num_bytes=0;
  int dummy;
  skCompiledCode::skInstruction ins;
  code.getInstruction(pc++,ins,dummy,num_bytes);
  assert(ins==skCompiledCode::b_StatsSize);
  skString checked_id;
  SAVE_VARIABLE(checked_id);
  bool has_array_index;
  bool is_method;
  getIdNode(code,pc,checked_id,has_array_index,is_method);
  assert(is_method==false);
  checked_id=checkIndirectId(frame,checked_id);
  skRValue start_expr;
  SAVE_VARIABLE(start_expr);
  start_expr=evaluate(frame,code,pc);
  int start_value=start_expr.intValue();
  skRValue end_expr;
  SAVE_VARIABLE(end_expr);
  end_expr=evaluate(frame,code,pc);
  int end_value=end_expr.intValue();
  int step_value=1;
  if (has_step){
    skRValue step_expr;
    SAVE_VARIABLE(step_expr);
    step_expr=evaluate(frame,code,pc);
    step_value=step_expr.intValue();
    RELEASE_VARIABLE(step_expr);
  }
  if (start_value!=end_value){
    if (step_value>0){
      if (end_value>start_value){
        USize loop_pc=pc;
        for (int i=start_value;i<end_value;i+=step_value){
#ifndef EXCEPTIONS_DEFINED
          if (frame.getContext().getError().getErrorCode()!=skScriptError::NONE)
            break;
#endif
          addLocalVariable(frame.getVars(),checked_id,i);
          // reset pc to start of the loop
          pc=loop_pc;
          bRet=executeStats(frame,code,pc,num_bytes,true,r);
          if (bRet!=SRC_CONTINUE)
            break;
        }
        // advance beyond the statements
        pc=loop_pc+num_bytes;
      }else
        runtimeError(frame,skSTR("End value is not greater than the start value in a for statement\n"));
    }else{
      if (step_value<0){
        if (start_value>end_value){
          USize loop_pc=pc;
          for (int i=start_value;i>end_value;i+=step_value){
#ifndef EXCEPTIONS_DEFINED
            if (frame.getContext().getError().getErrorCode()!=skScriptError::NONE)
              break;
#endif
            addLocalVariable(frame.getVars(),checked_id,i);
            // reset pc to start of the loop
            pc=loop_pc;
            bRet=executeStats(frame,code,pc,num_bytes,true,r);
            if (bRet!=SRC_CONTINUE)
              break;
          }
          // advance beyond the statements
          pc=loop_pc+num_bytes;
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
void skInterpreter::executeAssignStat(skStackFrame& frame,skCompiledCode& code,USize& pc,skRValue& r)
  //---------------------------------------------------
{
  skRValue value;
  SAVE_VARIABLE(value);
  value=evaluate(frame,code,pc);
#ifndef EXCEPTIONS_DEFINED
  if (frame.getContext().getError().getErrorCode()==skScriptError::NONE){
#endif
    int num_ids;
    skString attribute;
    SAVE_VARIABLE(attribute);
    getIdNodes(code,pc,num_ids,attribute);
    skString blank;
    skString field_name;
    SAVE_VARIABLE(field_name);
    if (num_ids==1){
      bool has_array_index;
      bool is_method;
      getIdNode(code,pc,field_name,has_array_index,is_method);
      assert(is_method==false);
      field_name=checkIndirectId(frame,field_name);
      // special case where there is a single id
      bool inserted=false;
      if (frame.getObject()!=0){
        skRValue caller;
        SAVE_VARIABLE(caller);
        caller.assignObject(frame.getObject());
        if (field_name==s_self){
          if (has_array_index)
            inserted=insertArrayValue(frame,code,pc,caller,attribute,value);
          else
            inserted=insertValue(frame,caller,blank,attribute,value);
        }else{
          if (has_array_index){
            caller=findValue(frame,field_name,0,blank);
            inserted=insertArrayValue(frame,code,pc,caller,attribute,value);
          }else{
            if (attribute.length()>0){
              // e.g. "field:name"
              caller.assignObject(frame.getObject());
              inserted=insertValue(frame,caller,field_name,attribute,value);
              if (inserted==false)
                runtimeError(frame,skString::addStrings(s_SettingAttribute,attribute.ptr(),s_OnMiddle,field_name.ptr(),s_Failed));
            }else
              inserted=insertValue(frame,caller,field_name,attribute,value);
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
      followIdList(frame,code,pc,num_ids,robject);
      bool has_array_index;
      bool is_method;
      getIdNode(code,pc,field_name,has_array_index,is_method);
      assert(is_method==false);
      field_name=checkIndirectId(frame,field_name);
      if (has_array_index){
        skRValue array_field;
        SAVE_VARIABLE(array_field);
        if (extractValue(frame,robject,field_name,blank,array_field)){
          insertArrayValue(frame,code,pc,array_field,attribute,value);
        }else{
          runtimeError(frame,skString::addStrings(s_CannotFindField,field_name.ptr(),s_cr));
        }
        RELEASE_VARIABLE(array_field);
      }else
        insertValue(frame,robject,field_name,attribute,value);
      RELEASE_VARIABLE(robject);
    }
    RELEASE_VARIABLE(field_name);
    RELEASE_VARIABLE(attribute);
#ifndef EXCEPTIONS_DEFINED
  }
#endif
  RELEASE_VARIABLE(value);
}
//------------------------------------------
skInterpreter::StatementReturnCode skInterpreter::executeStats(skStackFrame& frame,skCompiledCode& code,USize& pc,int& num_bytes,bool execute_stats,skRValue& r)
//------------------------------------------
{
  StatementReturnCode bRet=SRC_CONTINUE;
  skCompiledCode::skInstruction instruction;
  int num_stats;
  if (execute_stats){
    code.getInstruction(pc++,instruction,num_stats,num_bytes);
    assert(instruction==skCompiledCode::b_StatList);
    for (int i=0;i<num_stats;i++){
      int param1;
      int line_num;
      code.getInstruction(pc++,instruction,param1,line_num);
      frame.setLineNum(line_num);
      if (m_StatementStepper)
        if (m_StatementStepper->statementExecuted(frame,(int)instruction)==false)
          bRet=SRC_RETURN;
      if (bRet==false){
        switch(instruction){
        case skCompiledCode::b_NullStat:        // param1 = 0                 param2 = line
          break;
        case skCompiledCode::b_Switch:          // param1 = has default       param2 = line
          bRet=executeSwitchStat(frame,code,pc,r,param1==1);
          break;
        case skCompiledCode::b_If:              // param1 = has else          param2 = line
          bRet=executeIfStat(frame,code,pc,r,param1==1);
          break;
        case skCompiledCode::b_Return:          // param1 = has return expr   param2 = line
          bRet=executeReturnStat(frame,code,pc,r,param1==1);
          break;
        case skCompiledCode::b_Break:          // param1 = 0
          bRet=executeBreakStat(frame,code,pc,r);
          break;
        case skCompiledCode::b_While:           // param1 = 0                 param2 = line
          bRet=executeWhileStat(frame,code,pc,r);
          break;
        case skCompiledCode::b_ForEach:         // param1 = id index          param2 = line
          bRet=executeForEachStat(frame,code,pc,r,param1);
          break;
        case skCompiledCode::b_For:             // param1 = has step expr     param2 = line
          bRet=executeForStat(frame,code,pc,r,param1==1);
          break;
        case skCompiledCode::b_Assign:          // param1 = 0                 param2 = line
          executeAssignStat(frame,code,pc,r);
          break;
        case skCompiledCode::b_Method:          // param1 = 0                 param2 = line
          r=evalMethod(frame,code,pc);
          break;
        default:
          assert(skSTR("Unknown statement")==0);
          break;
        }
        // break out if stop has been passed back
        if (bRet!=SRC_CONTINUE)
          break;
      }
    }
  }else{
    // skip over this list of statements
    code.getInstruction(pc,instruction,num_stats,num_bytes);
    assert(instruction==skCompiledCode::b_StatList);
    pc+=num_bytes;
  }
  return bRet;
}
//------------------------------------------
void skInterpreter::evaluateExpression(const skString& location,skiExecutable * obj,
                                       skCompiledCode& code,skRValueTable&  vars,
                                       skRValue& return_value,skExecutableContext& ctxt)
  //------------------------------------------
{
  skStackFrame frame(location,obj,vars,ctxt);
  USize pc=0;
  return_value=evaluate(frame,code,pc);
}
#endif
