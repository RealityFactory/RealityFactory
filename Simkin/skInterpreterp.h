/*
	Interpreter implementation class
	$Id: skInterpreterp.h,v 1.29 2001/11/22 11:13:21 sdw Exp $
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

*/

#include "skParseNode.h"
#include "skExecutableIterator.h"
#include "skRValueTable.h"

/**
 * Interpreter implementation class
 */
class P_Interpreter 
{   
 public:
  /** Constructor for implementation class */
  P_Interpreter();
  /** Destructor for implementation class */
  ~P_Interpreter();
						

    // Expression evaluation
  
  /** this method evaluates an arbitrary Simkin expression 
   * @param ctxt - the current source code context
   * @param obj - the object owning the current method
   * @param var - the local variables
   * @param n - the expression parse tree
   */
  skRValue evaluate(skContext& ctxt,skiExecutable * obj,skRValueTable& var,skExprNode * n);
  /** this method evaluates the given method 
   * @param ctxt - the current source code context
   * @param obj - the object owning the current method
   * @param var - the local variables
   * @param ids - the parse tree giving the method name
*/
  skRValue evalMethod(skContext& ctxt,skiExecutable * obj,skRValueTable& var,skIdListNode * ids);
  /** this method actually executes a method
   * @param ctxt - the current source code context
   * @param obj - the object owning the current method
   * @param robject - the object that the method is to be called on
   * @param method_name - the name of the method
   * @param array_index - the parse tree for the array index (if any)
   * @param attribute - the name of the attribute, if present
   * @param exprs - the method parameters
   * @param ret - the value to receive the return value from the method
   */
  void makeMethodCall(skContext& ctxt,skiExecutable * obj,skRValueTable& var,skRValue& robject,const skString& method_name,skExprNode * array_index, const skString& attribute,skExprListNode * exprs,skRValue& ret);
  
    // Statement execution

  /** This method executes an assignment statement
   * @param ctxt - the current source code context
   * @param obj - the object owning the current method
   * @param var - the local variables
   * @param n - the assignment statement parse tree
   */
  void executeAssignStat(skContext& ctxt,skiExecutable * obj,skRValueTable& var,skAssignNode * n);
  
    // the statements below return true to halt further processing (i.e. a return statement has been encountered)

  /** This method executes a statement
   * @param ctxt - the current source code context
   * @param obj - the object owning the current method
   * @param var - the local variables
   * @param pstat - the assignment statement parse tree
   * @param r - the value to receive the return value from the statement
   * @return true to halt further processing (i.e. a return statement has been executed)
   */
  bool executeStat(skContext& ctxt,skiExecutable * obj,skRValueTable& var,skStatNode * pstat,skRValue& r);

  /** This method executes a list of statements
   * @param ctxt - the current source code context
   * @param obj - the object owning the current method
   * @param var - the local variables
   * @param n - the parse tree for the list of statements
   * @param r - the value to receive the return value from the statements
   * @return true to halt further processing (i.e. a return statement has been executed)
   */
  bool executeStats(skContext& ctxt,skiExecutable * obj,skRValueTable& var,skStatListNode * n,skRValue& r);

  /** This method executes a return statement
   * @param ctxt - the current source code context
   * @param obj - the object owning the current method
   * @param var - the local variables
   * @param n - the return statement parse tree
   * @param r - the value to receive the return value from the statement
   * @return true to halt further processing (i.e. a return statement has been executed)
   */
  bool executeReturnStat(skContext& ctxt,skiExecutable * obj,skRValueTable& var,skReturnNode * n,skRValue& r);

  /** This method executes an if statement
   * @param ctxt - the current source code context
   * @param obj - the object owning the current method
   * @param var - the local variables
   * @param n - the if statement parse tree
   * @param r - the value to receive the return value from the statement
   * @return true to halt further processing (i.e. a return statement has been executed)
   */
  bool executeIfStat(skContext& ctxt,skiExecutable * obj,skRValueTable& var,skIfNode * n,skRValue& r);

  /** This method executes a while statement
   * @param ctxt - the current source code context
   * @param obj - the object owning the current method
   * @param var - the local variables
   * @param n - the while statement parse tree
   * @param r - the value to receive the return value from the statement
   * @return true to halt further processing (i.e. a return statement has been executed)
   */
  bool executeWhileStat(skContext& ctxt,skiExecutable * obj,skRValueTable& var,skWhileNode * n,skRValue& r);

  /** This method executes a switch statement
   * @param ctxt - the current source code context
   * @param obj - the object owning the current method
   * @param var - the local variables
   * @param n - the switch statement parse tree
   * @param r - the value to receive the return value from the statement
   * @return true to halt further processing (i.e. a return statement has been executed)
   */
  bool executeSwitchStat(skContext& ctxt,skiExecutable * obj,skRValueTable& var,skSwitchNode * n,skRValue& r);

  /** This method executes a foreach statement
   * @param ctxt - the current source code context
   * @param obj - the object owning the current method
   * @param var - the local variables
   * @param n - the foreach statement parse tree
   * @param r - the value to receive the return value from the statement
   * @return true to halt further processing (i.e. a return statement has been executed)
   */
  bool executeForEachStat(skContext& ctxt,skiExecutable * obj,skRValueTable& var,skForEachNode * n,skRValue& r);

  /** This method executes a for statement
   * @param ctxt - the current source code context
   * @param obj - the object owning the current method
   * @param var - the local variables
   * @param n - the for statement parse tree
   * @param r - the value to receive the return value from the statement
   * @return true to halt further processing (i.e. a return statement has been executed)
   */
  bool executeForStat(skContext& ctxt,skiExecutable * obj,skRValueTable& var,skForNode * n,skRValue& r);

    // Misc runtime routines

  /** Adds a new local variable to the current list
   * @param var - the local variables
   * @param name - the name of the variable
   * @param value  - the value of the variable
   */
    void addLocalVariable(skRValueTable& var,const skString& name,skRValue value); 
    /** This method checks whether a field name includes the indirection character
   * @param ctxt - the current source code context
   * @param obj - the object owning the current method
   * @param var - the local variables
   * @param name - the name being checked
   */
    skString checkIndirectId(skContext& ctxt,skiExecutable * obj,skRValueTable& var,const skString& name); 
    /** This method finds a value associated with a given name
   * @param ctxt - the current source code context
   * @param obj - the object owning the current method
   * @param var - the local variables
   * @param name - the name being checked
   * @param array_index - the parse tree for the array index (if any)
   * @param attribute - the name of the attribute, if present
   * @return the value associated with the name
   */
    skRValue findValue(skContext& ctxt,skiExecutable * obj,skRValueTable& var,const skString& name,skExprNode * array_index,const skString& attribute); 
    /**
     * This method reports a runtime error by throwing a skRuntimeException
     * @param ctxt - the current source code context
     * @param s - a message describing the error
     */
    void runtimeError(skContext& ctxt,const skString& s); 
    /** This method follows a dotted list of id's to retrieve the associated value
     * @param ctxt - the current source code context
     * @param obj - the object owning the current method
     * @param var - the local variables
     * @param idlist - the list of ids in the dotted name
     * @param object - this object receives the value corresponding to the id list
     */
    void followIdList(skContext& ctxt,skiExecutable * obj,skRValueTable& var,skIdListNode * idList,skRValue& object); 

    /** This method sends a message to the tracer output
     * @param s - the message to sent to the tracer output
     */
    void trace(const skString& s);

    /** This method extracts a value of the form foo[1] - first dereferencing foo
     * @param ctxt - the current source code context
     * @param obj - the object owning the current method
     * @param var - the local variables
     * @param robject - the object owning the field name
     * @param field_name - the field name being accessed
     * @param array_index - the parse tree for the array index (if any)
     * @param attrib - the name of the attribute, if present
     * @param ret - the value to receive the associated value
     */
    bool extractFieldArrayValue(skContext& ctxt,skiExecutable * obj,skRValueTable& var,skRValue& robject,const skString& field_name,skExprNode * array_index,const skString& attrib,skRValue& ret);
    /** This method extracts a value of the form robject[1] - assumes robject is already a collection object
     * @param ctxt - the current source code context
     * @param obj - the object owning the current method
     * @param var - the local variables
     * @param robject - the object owning the field name
     * @param array_index - the parse tree for the array index (if any)
     * @param attrib - the name of the attribute, if present
     * @param ret - the value to receive the associated value
     */
    bool extractArrayValue(skContext& ctxt,skiExecutable * obj,skRValueTable& var,skRValue& robject,skExprNode * array_index,const skString& attrib,skRValue& ret) ;
    /** This method extracts an instance variable with the given name
     * @param ctxt - the current source code context
     * @param robject - the object owning the field name
     * @param name - the field name being accessed
     * @param attrib - the name of the attribute, if present
     * @param ret - the value to receive the associated value
     */
    bool extractValue(skContext& ctxt,skRValue& robject,const skString& name,const skString& attrib,skRValue& ret) ;
    
    /** This method is a wrapper around calling the setValueAt method
     * @param ctxt - the current source code context
     * @param obj - the object owning the current method
     * @param robject - the object owning the field name
     * @param array_index - the parse tree for the array index (if any)
     * @param attr - the name of the attribute, if present
     * @param value - the value to be set
     */
    bool insertArrayValue(skContext& ctxt,skiExecutable * obj,skRValueTable& var,skRValue& robject, skExprNode * array_index,const skString& attr,const skRValue& value);
    /** This method is a wrapper around calling the setValue method
     * @param ctxt - the current source code context
     * @param robject - the object owning the field name
     * @param name - the field name being accessed
     * @param attr - the name of the attribute, if present
     * @param value - the value to be set
     */
    bool insertValue(skContext& ctxt,skRValue& robject,const skString& name, const skString& attr,const skRValue& value);

    // Variables
    /** this is the list of global variables */
    skRValueTable m_GlobalVars; 
    /** this flag controls whether the interpreter outputs tracing information about the execution of statements */
    bool m_Tracing; 
    /** This variable points to an associated object for capturing tracing output */
    skTraceCallback * m_TraceCallback; 
    /** This variable points to an associated object which receives information about which statements are being executed */
    skStatementStepper * m_StatementStepper; // the statement stepper
    /** This variable holds a global instance of the interpreter. */
    THREAD static skInterpreter * g_GlobalInterpreter;	//	used by clients - one per thread (on Windows only at the moment)

};    

//---------------------------------------------------
inline skString P_Interpreter::checkIndirectId(skContext& ctxt,skiExecutable * obj,skRValueTable& var,const skString& name)
//---------------------------------------------------
{
  // look for an initial "@" in a field name, and de-reference it if necessary
    skString ret=name;
    if (name.at(0)=='@'){
	ret=name.substr(1,name.length()-1);
	skRValue new_name=findValue(ctxt,obj,var,ret,0,skString());
	ret=new_name.str();
    }
    return ret;
}	
