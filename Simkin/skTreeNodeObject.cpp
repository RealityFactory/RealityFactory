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

  $Id: skTreeNodeObject.cpp,v 1.56 2003/04/23 14:34:51 simkin_cvs Exp $
*/

#include "skStringTokenizer.h"
#include "skTreeNodeObject.h"
#include "skTreeNode.h"
#include "skTreeNodeObjectEnumerator.h"
#include "skRValueArray.h"
#include "skInterpreter.h"
#include "skMethodTable.h"
#include "skRValueTable.h"


//------------------------------------------
EXPORT_C skTreeNodeObject::skTreeNodeObject()
//------------------------------------------
  : m_Node(0),m_Created(false),m_MethodCache(0),m_AddIfNotPresent(false)

{
}
//------------------------------------------
EXPORT_C skTreeNodeObject::skTreeNodeObject(const skString& location,skTreeNode * node,bool created)
//------------------------------------------
  : m_Location(location),m_Node(node),m_Created(created),m_MethodCache(0),m_AddIfNotPresent(false)
{
}
//------------------------------------------
EXPORT_C skTreeNodeObject::~skTreeNodeObject()
//------------------------------------------
{
  if (m_Created)
    delete m_Node;
  delete m_MethodCache;
}
//------------------------------------------
EXPORT_C int skTreeNodeObject::executableType() const
//------------------------------------------
{
  return TREENODE_TYPE;
}
//------------------------------------------
EXPORT_C int skTreeNodeObject::intValue() const
//------------------------------------------
{
  return m_Node->intData();
}
#ifdef USE_FLOATING_POINT
//------------------------------------------
EXPORT_C float skTreeNodeObject::floatValue() const
//------------------------------------------
{
  return m_Node->floatData();
}
#endif
//------------------------------------------
EXPORT_C bool skTreeNodeObject::boolValue() const
//------------------------------------------
{
  return m_Node->boolData();
}
//------------------------------------------
EXPORT_C Char skTreeNodeObject::charValue() const
//------------------------------------------
{
  return m_Node->data().at(0);
}
//------------------------------------------
EXPORT_C skString skTreeNodeObject::strValue() const
//------------------------------------------
{
  return m_Node->data();
}
//------------------------------------------
EXPORT_C bool skTreeNodeObject::setValueAt(const skRValue& array_index,const skString& attribute,const skRValue& v)
//------------------------------------------
{
  bool bRet=false;
  int index=array_index.intValue();
  skTreeNode * child=0;
  if (index<(int)m_Node->numChildren())
    child=m_Node->nthChild(index);
  if (child==0 && m_AddIfNotPresent){
    // add some nodes if necessary
    if (index>=(int)m_Node->numChildren()){
      int num_to_add=index-m_Node->numChildren()+1;
      for (int i=0;i<num_to_add;i++){
        child=skNEW(skTreeNode);
        m_Node->addChild(child);
      }
    }
  }
  if (child){
    bRet=true;
    skiExecutable * other=v.obj();
    if (other && other->executableType()==TREENODE_TYPE)
      child->copyItems(*((skTreeNodeObject *)other)->m_Node);
    else
      child->data(v.str());
  }else
    bRet=skExecutable::setValueAt(array_index,attribute,v);
  return bRet;
}
//------------------------------------------
EXPORT_C bool skTreeNodeObject::setValue(const skString& s,const skString& attrib,const skRValue& v)
//------------------------------------------
{
  bool bRet=false;
  skTreeNode * child=m_Node->findChild(s);
  if (child==0 && m_AddIfNotPresent){
    child=skNEW(skTreeNode(s));
    m_Node->addChild(child);
  }
  if (child){
    bRet=true;
    skiExecutable * other=v.obj();
    if (other && other->executableType()==TREENODE_TYPE)
      child->copyItems(*((skTreeNodeObject *)other)->m_Node);
    else
      child->data(v.str());
  }else
    bRet=skExecutable::setValue(s,attrib,v);
  return bRet;
}
//------------------------------------------
EXPORT_C skTreeNode * skTreeNodeObject::getNode()
//------------------------------------------
{
  return m_Node;
}
//------------------------------------------
EXPORT_C void skTreeNodeObject::setNode(const skString& location,skTreeNode * node,bool created)
//------------------------------------------
{
  if (m_Node){
    if (m_Created)
      delete m_Node;
  }
  m_Created=created;
  m_Location=location;
  m_Node=node;
  // clear up the current method cache
  delete m_MethodCache;
  m_MethodCache=0;
}
#ifdef __SYMBIAN32__
//------------------------------------------
EXPORT_C void skTreeNodeObject::setNode(const TDesC& location,skTreeNode * node,bool created)
//------------------------------------------
{
  skString s_location;
  s_location=location;
  setNode(s_location,node,created);
}
#endif
//------------------------------------------
EXPORT_C bool skTreeNodeObject::getValueAt(const skRValue& array_index,const skString& attribute,skRValue& value)
//------------------------------------------
{
  bool bRet=false;
  int index=array_index.intValue();
  skTreeNode * child=0;
  if (index<(int)m_Node->numChildren())
    child=m_Node->nthChild(index);
  if (child==0 && m_AddIfNotPresent){
    // add some nodes if necessary
    if (index>=(int)m_Node->numChildren()){
      int num_to_add=index-m_Node->numChildren()+1;
      for (int i=0;i<num_to_add;i++){
        child=skNEW(skTreeNode);
        m_Node->addChild(child);
      }
    }
  }
  if (child){
    bRet=true;
    value.assignObject(createTreeNodeObject(skString::addStrings(m_Location.ptr(),s_leftbracket,skString::from(index).ptr(),s_rightbracket),child,false),true);
  }else
    bRet=skExecutable::getValueAt(array_index,attribute,value);
  return bRet;
}
//------------------------------------------
EXPORT_C bool skTreeNodeObject::getValue(const skString& name,const skString& attrib,skRValue& v)
//------------------------------------------
{
  bool bRet=false;
  if (name == s_numChildren){
    bRet=true;
    v=(int)m_Node->numChildren();
  }else
    if (name == s_label){
      bRet=true;
      v=m_Node->label();
    }else{
      skTreeNode * child=m_Node->findChild(name);
      if (child==0 && m_AddIfNotPresent){
        child=skNEW(skTreeNode(name));
        m_Node->addChild(child);
      }
      if (child){
        bRet=true;
        v.assignObject(createTreeNodeObject(skString::addStrings(m_Location.ptr(),s_colon,name.ptr()),child,false),true);
      }else
        bRet=skExecutable::getValue(name,attrib,v);
  }
  return bRet;
}
//------------------------------------------
EXPORT_C bool skTreeNodeObject::method(const skString& s,skRValueArray& args,skRValue& ret,skExecutableContext& ctxt)
//------------------------------------------
{
  bool bRet=false;
  if (s==s_enumerate && (args.entries()==0 || args.entries()==1)){
    // return an enumeration object for this element
    bRet=true;
    if (args.entries()==0){
       skTreeNodeObjectEnumerator * enumerator=skNEW(skTreeNodeObjectEnumerator(this,getLocation()));
      ret.assignObject(enumerator,true);
    }else{
       skTreeNodeObjectEnumerator * enumerator=skNEW(skTreeNodeObjectEnumerator(this,getLocation(),args[0].str()));
      ret.assignObject(enumerator,true);
    }
  }else{
    skString location=skString::addStrings(m_Location.ptr(),s_colon,s.ptr());
    if (m_Node){
      skMethodDefNode * methNode=0;
      if (m_MethodCache!=0)
        methNode=m_MethodCache->value(s);
      if (methNode==0){
        // if no parse tree is found - try and build one
        skTreeNode * func_node=0;
        func_node=m_Node->findChild(s);
        if (func_node){
          bRet=true;
          // extract the params etc..
          skString code=func_node->data();
          SAVE_VARIABLE(code);
          int first_bracket=code.indexOf('(');
          if (first_bracket!=-1){
            int next_bracket=code.indexOf(')');
            if (next_bracket!=-1 && next_bracket>first_bracket){
              int first_brace=code.indexOf('{');
              if (first_brace!=-1 && first_brace>next_bracket){
                // parse the parameters between the brackets
                skString params=code.substr(first_bracket+1,next_bracket-first_bracket-1);
                SAVE_VARIABLE(params);
                skStringList paramList;
                SAVE_VARIABLE(paramList);
                if (params.length()>0){
                  skString comma;
                  comma=skSTR(", ");
                  skStringTokenizer tokenizer;
                  tokenizer.init(params,comma,false);
                  while (tokenizer.hasMoreTokens())
                    paramList.append(tokenizer.nextToken());
                }
                code=code.substr(first_brace+1);
                code=code.removeInitialBlankLines();
                ctxt.getInterpreter()->executeStringExternalParams(location,this,paramList,code,args,ret,&methNode,ctxt);
                if (methNode){
                  if (m_MethodCache==0)
                    m_MethodCache=skNEW(skMethodTable());
                  m_MethodCache->insertKeyAndValue(s,methNode);
                }
                
                RELEASE_VARIABLE(paramList);
                RELEASE_VARIABLE(params);
              }else
                bRet=skExecutable::method(s,args,ret,ctxt);
            }else
              bRet=skExecutable::method(s,args,ret,ctxt);
          }else
            bRet=skExecutable::method(s,args,ret,ctxt);
          RELEASE_VARIABLE(code);
        }else
          bRet=skExecutable::method(s,args,ret,ctxt);
      }else{
        // otherwise execute the parse tree immediately
        ctxt.getInterpreter()->executeParseTree(location,this,methNode,args,ret,ctxt);
        bRet=true;
      }
    }else
      bRet=skExecutable::method(s,args,ret,ctxt);
  }
  return bRet;
}
//------------------------------------------
skTreeNodeObject::skTreeNodeObject(const skTreeNodeObject& other)
//------------------------------------------
{
}
//------------------------------------------
skTreeNodeObject& skTreeNodeObject::operator=(const skTreeNodeObject& other)
//------------------------------------------
{
  return *this;
} 
//------------------------------------------
EXPORT_C skString skTreeNodeObject::getLocation() const
//------------------------------------------
{
  return m_Location;
}
//------------------------------------------
EXPORT_C skExecutableIterator * skTreeNodeObject::createIterator(const skString& qualifier)
//------------------------------------------
{
  return skNEW(skTreeNodeObjectEnumerator(this,getLocation(),qualifier));
}
//------------------------------------------
EXPORT_C skExecutableIterator * skTreeNodeObject::createIterator()
//------------------------------------------
{
  return skNEW(skTreeNodeObjectEnumerator(this,getLocation()));
}
//------------------------------------------
EXPORT_C skString skTreeNodeObject::getSource(const skString& location)
//------------------------------------------
{
  skString src;
  if (m_Node){
    // extract the object name
    int index=location.indexOfLast(c_colon);
    skString name=location;
    SAVE_VARIABLE(name);
    if (index!=-1)
      name=location.substr(index+1);
    src=m_Node->findChildData(name);
    // strip out the parameter and opening/closing braces
    int first_brace=src.indexOf(c_leftbrace);
    if (first_brace!=-1)
      src=src.substr(first_brace+1);
    int last_brace=src.indexOfLast(c_rightbrace);
    if (last_brace!=-1)
      src=src.substr(0,last_brace);
    src=src.removeInitialBlankLines();
    RELEASE_VARIABLE(name);
  }
  return src;
}
//------------------------------------------
EXPORT_C void skTreeNodeObject::getInstanceVariables(skRValueTable& table)
//------------------------------------------
{
  if (m_Node){
    for (unsigned int i=0;i<m_Node->numChildren();i++){
      skTreeNode * var=m_Node->nthChild(i);
      skRValue * value=skNEW(skRValue());
      SAVE_POINTER(value);
      value->assignObject(new skTreeNodeObject(var->label(),var,false),true);
      table.insertKeyAndValue(var->label(),value);
      RELEASE_POINTER(value);
    }
  }
}
//------------------------------------------
EXPORT_C void skTreeNodeObject::setAddIfNotPresent(bool enable)
//------------------------------------------
{
  m_AddIfNotPresent=enable;
}
//------------------------------------------
EXPORT_C bool skTreeNodeObject::getAddIfNotPresent()
//------------------------------------------
{
  return m_AddIfNotPresent;
}
//------------------------------------------
EXPORT_C skTreeNodeObject * skTreeNodeObject::createTreeNodeObject(const skString& location,skTreeNode * node,bool created)
//------------------------------------------
{
  skTreeNodeObject *  obj=skNEW(skTreeNodeObject(location,node,created));
  obj->setAddIfNotPresent(getAddIfNotPresent());
  return obj;
}
//------------------------------------------
EXPORT_C bool skTreeNodeObject::equals(const skiExecutable * o) const
//------------------------------------------
{
  bool equals=false;
  // is the other a TreeNodeObject?
  if (o->executableType()==executableType() && m_Node){
    skTreeNode * other=((skTreeNodeObject *)o)->getNode();
    if (other)
      // do a deep comparison on the nodes
      equals=(*m_Node==*other);
  }else
    // otherwise just check the string value
    equals=(strValue()==o->strValue());
  return equals;
}
