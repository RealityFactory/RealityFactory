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

* $Id: skInterpreter.h,v 1.49 2003/04/19 13:22:23 simkin_cvs Exp $
*/
#ifndef skINTERPRETER_H
#define skINTERPRETER_H

#include "skRValueTable.h"
#include "skiExecutable.h"
#include "skNull.h"
#include "skParseNode.h"
#include "skExecutableIterator.h"
#include "skRValueTable.h"

class CLASSEXPORT skRValueArray;
class CLASSEXPORT skExprNode;
class CLASSEXPORT skStringList;
class CLASSEXPORT skMethodDefNode;
class CLASSEXPORT skTraceCallback;
class CLASSEXPORT skStatementStepper;

#ifndef EXCEPTIONS_DEFINED
#include "skScriptError.h"
#endif

class CLASSEXPORT skStackFrame;

/**
 * This class parses and executes Simkin script, and holds a list of
 * global variables.
 *
 * There is one global Interpreter object which you should set up at
 * the start, although others can also be created.
*/
class CLASSEXPORT skInterpreter : public skExecutable
{ 
 public:
  //------------------------
  // Parsing and Executing methods
  //------------------------

  /**
   * this function parses the script in the code variable and returns
   * a parse tree if there are no syntax errors. 
   * If there are syntax errors the function throws an skParseException object
   * @param location - a string describing where this code is located, this will appear in any error messages
   * @param code - a string of Simkin code, which includes the parameter declarations
   * @param ctxt context object to receive errors
   * @return returns a parse tree, if the syntax was valid. The caller must free this tree.
   * @exception skParseException - if a syntax error is encountered
   * @exception Symbian - a leaving function
   */
  IMPORT_C skMethodDefNode * parseString(const skString& location,const skString& code,skExecutableContext& ctxt);       
  /**
   * this function parses the script in the code variable and returns
   * a parse tree if there are no syntax errors. It assumes that the script does *not* contain parameters and enclosing braces.
   * If there are syntax errors the function throws an skParseException object
   * @param location - a string describing where this code is located, this will appear in any error messages
   * @param paramNames - a list of parameter names (hence the name "ExternalParams"
   * @param code - a string of Simkin code, which does *not* include the parameter declarations
   * @param ctxt context object to receive errors
   * @return returns a parse tree, if the syntax was valid. The caller must free this tree.
   * @exception skParseException - if a syntax error is encountered
   * @exception Symbian - a leaving function
   */
  IMPORT_C skMethodDefNode * parseExternalParams(const skString& location,skStringList& paramNames,const skString& code,skExecutableContext& ctxt);
  /**
   * this function parses and executes script which is assumed to belong
   * to the object passed in.
   * @param location - a textual description of the location of the script (this appears in error messages)
   * @param obj - the executable object which "owns" the script
   * @param args - an array of arguments to the function, which are passed as parameters to the script
   * @param code - a string of Simkin script, including parameter declarations
   * @param return_value - an RValue which receives the result of the script
   * @param parseTree - if you supply this pointer, the parse tree is assigned to it, and you must delete it yourself. Without the parameter the parse tree will be deleted by the interpreter. The parse tree can be used in a cache and passed to executeParseTree later.
   * @param ctxt context object to receive errors
   * @exception skParseException - if a syntax error is encountered
   * @exception skRuntimeException - if an error occurs while the script is running
   * @exception Symbian - a leaving function
   */
  IMPORT_C void executeString(const skString& location,skiExecutable * obj,const skString& code,skRValueArray&  args,skRValue& return_value,skMethodDefNode ** parseTree,skExecutableContext& ctxt);

  /**
   * this function parses and executes script with externally declared parameters which is assumed to belong to the object passed in.
   * @param location - a textual description of the location of the script (this appears in error messages)
   * @param obj - the executable object which "owns" the script
   * @param paramNames - a list of parameter names (hence the name "ExternalParams"
   * @param args - an array of arguments to the function, which are passed as parameters to the script
   * @param code - a string of Simkin script, which does *not* include parameter declarations
   * @param r - an RValue which receives the result of the script
   * @param keepParseTree - if you supply this pointer, the parse tree is assigned to it, and you must delete it yourself. Without the parameter the parse tree will be deleted by the interpreter. The parse tree can be used in a cache and passed to executeParseTree later.
   * @param ctxt context object to receive errors
   * @exception skParseException - if a syntax error is encountered
   * @exception skRuntimeException - if an error occurs while the script is running
   * @exception Symbian - a leaving function
   */
  IMPORT_C void executeStringExternalParams(const skString& location,skiExecutable * obj,skStringList& paramNames,const skString& code,skRValueArray&  args,skRValue& r,skMethodDefNode ** keepParseTree,skExecutableContext& ctxt);

  /**
   * this function executes some script that has already been parsed into a parse tree. 
   * @param location - a textual description of the location of the script (this appears in error messages)
   * @param obj - the executable object which "owns" the script
   * @param parseTree - a parse tree that has been generated by one of the parse or execute functions of the Interpreter
   * @param args - an array of arguments to the function, which are passed as parameters to the script
   * @param ctxt context object to receive errors
   * @param return_value - an RValue which receives the result of the script
   * @exception skRuntimeException - if an error occurs while the script is running
   * @exception Symbian - a leaving function
   */
  IMPORT_C void executeParseTree(const skString& location,skiExecutable * obj,skMethodDefNode * parseTree,skRValueArray&  args,skRValue& return_value,skExecutableContext& ctxt);


  /**
   * this function parses and evaluates an expression within the given context
   * @param location - a textual description of the location of the expression (this appears in error messages)
   * @param obj - the executable object which "owns" the expression
   * @param vars - a table of local variables defined for the expression
   * @param expression - a single Simkin expression which must begin with "=", for example "=a+b"
   * @param return_value - an RValue which receives the result of the expression
   * @param ctxt context object to receive errors
   * @exception skParseException - if a syntax error is encountered
   * @exception skRuntimeException - if an error occurs while the expression is evaluated
   * @exception Symbian - a leaving function
   */
  IMPORT_C void evaluateExpression(const skString& location,skiExecutable * obj,
                      const skString& expression,skRValueTable&  vars,
                      skRValue& return_value,skExecutableContext& ctxt);
  /**
   * this function parses and evaluates an expression within the given context
   * @param location - a textual description of the location of the expression (this appears in error messages)
   * @param obj - the executable object which "owns" the expression
   * @param vars - a table of local variables defined for the expression
   * @param expression - a single Simkin expression in a parse tree created by parseExpression
   * @param return_value - an RValue which receives the result of the expression
   * @param ctxt context object to receive errors
   * @exception skParseException - if a syntax error is encountered
   * @exception skRuntimeException - if an error occurs while the expression is evaluated
   * @exception Symbian - a leaving function
   */
#ifdef EXECUTE_PARSENODES
  IMPORT_C void evaluateExpression(const skString& location,skiExecutable * obj,
                      skExprNode * expression,skRValueTable&  vars,
                      skRValue& return_value,skExecutableContext& ctxt);
#else
  IMPORT_C void evaluateExpression(const skString& location,skiExecutable * obj,
			  skCompiledCode& code,skRValueTable&  vars,
			  skRValue& return_value,skExecutableContext& ctxt);
#endif

  //------------------------
  // Global Variable methods
  //------------------------
   
  /**
   * this method adds a global variable to the list held by this interpreter. If the variable already has a value, it is replaced with the new one
   * @param name - the name of the global variable
   * @param value - the value of the global variable, which can be any RValue
   * @exception Symbian - a leaving function
   */
  IMPORT_C void addGlobalVariable(const skString& name,const skRValue& value);
  /**
   * this method removes a global variable from the list held by this interpreter
   * @param name - the name of the global variable
   */
  IMPORT_C void removeGlobalVariable(const skString& name);
#ifdef __SYMBIAN32__
  /**
   * this method adds a global variable to the list held by this interpreter. If the variable already has a value, it is replaced with the new one
   * \remarks only available in Symbian version
   * @param name - the name of the global variable
   * @param value - the value of the global variable, which can be any RValue
   * @exception Symbian - a leaving function
   */
  IMPORT_C void addGlobalVariable(const TDesC& name,const skRValue& value);
  /**
   * this method removes a global variable from the list held by this interpreter
   * \remarks only available in Symbian version
   * @param name - the name of the global variable
   */
  IMPORT_C void removeGlobalVariable(const TDesC& name);
#endif
  /**
   * this method finds the value of a global variable by name
   * @param name - the name of the global variable

   * @param return_value - an RValue which receives the value of the variable
   * @return true if the variable was found, otherwise false
   */
  IMPORT_C bool findGlobalVariable(const skString& name,skRValue& return_value);

  /**
  * this method returns the table of global variables
  * @return a hashtable of global variables mapping name to object
  */
  IMPORT_C const skRValueTable& getGlobalVariables() const;

  //--------------------------------------------------------
  // Interpreter is an Executable which exposes some fields
  //--------------------------------------------------------
    
  /**
   * this method is used to set the Tracing variable in the interpreter. This will show method calls as the script is executed
   * In Simkin call Interpreter.Tracing=true
   */
  virtual IMPORT_C bool setValue(const skString& s,const skString& attribute,const skRValue& v);
  /**
   * this method is used to retrieve the Tracing variable in the interpreter. 
   * If true, method calls will show in the trace as the script is executed
   * In Simkin call tracing=Interpreter.Tracing
   */
  virtual IMPORT_C bool getValue(const skString& s,const skString& attribute,skRValue& v);
  /**
   * Requests that the object execute the given method. Methods supported:
   * breakpoint - breaks to the debugger (if present)
   * @param method_name - the name of the method to execute
   * @param arguments - an array of RValue objects, which are the arguments to the method
   * @param return_value - an object to receive the return value of the method
   * @param ctxt context object to receive errors
   * @return true if the method could be executed, or false if the method is not supported
   * @exception skParseException - if a syntax error is encountered while the script is running
   * @exception skRuntimeException - if an error occurs while the script is running
   */
  virtual IMPORT_C bool method(const skString& method_name,skRValueArray& arguments,skRValue& return_value,skExecutableContext& ctxt);

  /**
  * This method returns the instance variables for this object
  * @param table a table to filled with references to the instance variables
  * @exception Symbian - a leaving function
  */
  virtual void getInstanceVariables(skRValueTable& table);
    
  //------------------------
  // Tracing methods
  //------------------------

  /** output a message to the current trace output
   * @param msg the message to show
   */
  IMPORT_C void trace(const skString& msg);
  /** output a message to the current trace output
   * @param msg the message to show
   */
  IMPORT_C void trace(const Char * msg);
  /** this method sets an object to receive trace messages. Pass 0 to fall back to standard mechanism (using skTracer)
   * @param callback the callback object, or 0 to fall back to using skTracer
   */
  IMPORT_C void setTraceCallback(skTraceCallback * callback);

  /** this method sets an object to be called each time a statement is executed.
   * @param stepper the stepper object, or 0 to clear
   */
  IMPORT_C void setStatementStepper(skStatementStepper * stepper);

  /**
   * This method reports a runtime error by throwing a skRuntimeException
   * @param ctxt - the current source code context
   * @param msg - a message describing the error
   * @exception Symbian - a leaving function
   */
  IMPORT_C void runtimeError(skStackFrame& ctxt,const skString& msg); 
  /**
   * This method reports a runtime error by throwing a skRuntimeException
   * @param ctxt - the current source code context
   * @param msg - a message describing the error
   * @exception Symbian - a leaving function
   */
  IMPORT_C void runtimeError(skStackFrame& ctxt,const Char * msg); 

  //---------------------------
  // Constructor and Destructor
  //---------------------------
    
  /**
   * Constructor - adds the interpreter as a global variable with the name "Interpreter"
   * \remarks in Symbian version - you should call init() separately to add the interpreter global variable, as this
   * may leave
   */
  IMPORT_C skInterpreter();
  /**
   * Destructor - deletes the global variable list
   */
  virtual IMPORT_C ~skInterpreter();
  /** 
   * init function
   * \remarks needs to be called explicitly in Symbian version
   * @exception Symbian - a leaving function
   */
  IMPORT_C void init();
    
  inline skNull& getNull();

 private:

  //--------------------
  // copying not allowed
  //--------------------

  /**
   * the interpreter cannot be copied, so the copy constructor is private
   */
  skInterpreter(const skInterpreter&);
  /**
   * the interpreter cannot be copied, so the assignment operator is private
   */
  skInterpreter& operator=(const skInterpreter&);

#ifdef EXECUTE_PARSENODES
  /**
   * this function parses the script in the code variable and returns
   * a parse tree if there are no syntax errors. 
   * If there are syntax errors the function throws an skParseException object
   * @param location - a string describing where this code is located, this will appear in any error messages
   * @param expression - a single Simkin expression
   * @param ctxt context object to receive errors
   * @return returns a parse tree, if the syntax was valid. The caller must free this tree.
   * @exception skParseException - if a syntax error is encountered
   * @exception Symbian - a leaving function
   */
  skExprNode * parseExpression(const skString& location,const skString& expression,skExecutableContext& ctxt);  
#else
  /**
   * this function parses the script in the code variable and returns
   * a parse tree if there are no syntax errors. 
   * If there are syntax errors the function throws an skParseException object
   * @param location - a string describing where this code is located, this will appear in any error messages
   * @param expression - a single Simkin expression
   * @param ctxt context object to receive errors
   * @return returns a parse tree, if the syntax was valid. The caller must free this tree.
   * @exception skParseException - if a syntax error is encountered
   * @exception Symbian - a leaving function
   */
  skCompiledExprNode * parseExpression(const skString& location,const skString& expression,skExecutableContext& ctxt);  
#endif
    // Expression evaluation
  
  /** this method evaluates an arbitrary Simkin expression 
   * @param frame - the current stack frame
   * @param n - the expression parse tree
   * @exception Symbian - a leaving function
   */
#ifdef EXECUTE_PARSENODES
  skRValue evaluate(skStackFrame& frame,skExprNode * n);
#else
  skRValue evaluate(skStackFrame& frame,skCompiledCode& code,USize& pc);
#endif
  /** this method evaluates the given method 
   * @param frame - the current stack frame
   * @param ids - the parse tree giving the method name
   * @exception Symbian - a leaving function
  */
#ifdef EXECUTE_PARSENODES
  skRValue evalMethod(skStackFrame& frame,skIdListNode * ids);
#else
  skRValue evalMethod(skStackFrame& frame,skCompiledCode& code,USize& pc);
#endif
  /** this method actually executes a method
   * @param frame - the current stack frame
   * @param robject - the object that the method is to be called on
   * @param method_name - the name of the method
   * @param array_index - the parse tree for the array index (if any)
   * @param attribute - the name of the attribute, if present
   * @param exprs - the method parameters
   * @param ret - the value to receive the return value from the method
   * @exception Symbian - a leaving function
   */
#ifdef EXECUTE_PARSENODES
  void makeMethodCall(skStackFrame& frame,skRValue& robject,const skString& method_name,skExprNode * array_index, const skString& attribute,skExprListNode * exprs,skRValue& ret);
#else
  void makeMethodCall(skStackFrame& frame,skCompiledCode& code,USize& pc,skRValue& robject,const skString& method_name,bool has_array_index,const skString& attribute, skRValue& ret);
#endif
  
    // Statement execution

  /** This method executes an assignment statement
   * @param frame - the current stack frame
   * @param n - the assignment statement parse tree
   * @exception Symbian - a leaving function
   */
#ifdef EXECUTE_PARSENODES
  void executeAssignStat(skStackFrame& frame,skAssignNode * n);
#else
  void executeAssignStat(skStackFrame& frame,skCompiledCode& code,USize& pc,skRValue& r);
#endif
  
    // the statements below return true to halt further processing (i.e. a return statement has been encountered)

  /** This method executes a list of statements
   * @param frame - the current stack frame
   * @param n - the parse tree for the list of statements
   * @param r - the value to receive the return value from the statements
   * @return true to halt further processing (i.e. a return statement has been executed)
   * @exception Symbian - a leaving function
   */
#ifdef EXECUTE_PARSENODES
  bool executeStats(skStackFrame& frame,skStatListNode * n,skRValue& r);
#else
  /** This method executes a list of statements
   * @param frame - the current stack frame
   * @param code - the compiled code
   * @param pc - the program counter
   * @param num_bytes - the number of byte codes in this list of statements is saved back to this variable
   * @param execute_bytes - if this is false, the statements are not executed, but the byte code position (PC) is updated
   * @param r - the value to receive the return value from the statements
   * @return true to halt further processing (i.e. a return statement has been executed)
   * @exception Symbian - a leaving function
   */
  bool executeStats(skStackFrame& frame,skCompiledCode& code,USize& pc,int& num_bytes,bool execute_bytes,skRValue& r);
#endif
  /** This method executes a return statement
   * @param frame - the current stack frame
   * @param n - the return statement parse tree
   * @param r - the value to receive the return value from the statement
   * @return true to halt further processing (i.e. a return statement has been executed)
   * @exception Symbian - a leaving function
   */
#ifdef EXECUTE_PARSENODES
  bool executeReturnStat(skStackFrame& frame,skReturnNode * n,skRValue& r);
#else
  bool executeReturnStat(skStackFrame& frame,skCompiledCode& code,USize& pc,skRValue& r,bool has_expr);
#endif

  /** This method executes an if statement
   * @param frame - the current stack frame
   * @param n - the if statement parse tree
   * @param r - the value to receive the return value from the statement
   * @return true to halt further processing (i.e. a return statement has been executed)
   * @exception Symbian - a leaving function
   */
#ifdef EXECUTE_PARSENODES
  bool executeIfStat(skStackFrame& frame,skIfNode * n,skRValue& r);
#else
  bool executeIfStat(skStackFrame& frame,skCompiledCode& code,USize& pc,skRValue& r,bool has_else);
#endif

  /** This method executes a while statement
   * @param frame - the current stack frame
   * @param n - the while statement parse tree
   * @param r - the value to receive the return value from the statement
   * @return true to halt further processing (i.e. a return statement has been executed)
   * @exception Symbian - a leaving function
   */
#ifdef EXECUTE_PARSENODES
  bool executeWhileStat(skStackFrame& frame,skWhileNode * n,skRValue& r);
#else
  bool executeWhileStat(skStackFrame& frame,skCompiledCode& code,USize& pc,skRValue& r);
#endif

  /** This method executes a switch statement
   * @param frame - the current stack frame
   * @param n - the switch statement parse tree
   * @param r - the value to receive the return value from the statement
   * @return true to halt further processing (i.e. a return statement has been executed)
   * @exception Symbian - a leaving function
   */
#ifdef EXECUTE_PARSENODES
  bool executeSwitchStat(skStackFrame& frame,skSwitchNode * n,skRValue& r);
#else
  bool executeSwitchStat(skStackFrame& frame,skCompiledCode& code,USize& pc,skRValue& r,bool has_default);
#endif

  /** This method executes a foreach statement
   * @param frame - the current stack frame
   * @param n - the foreach statement parse tree
   * @param r - the value to receive the return value from the statement
   * @return true to halt further processing (i.e. a return statement has been executed)
   * @exception Symbian - a leaving function
   */
#ifdef EXECUTE_PARSENODES
  bool executeForEachStat(skStackFrame& frame,skForEachNode * n,skRValue& r);
#else
  bool executeForEachStat(skStackFrame& frame,skCompiledCode& code,USize& pc,skRValue& r,USize id_index);
#endif

  /** This method executes a for statement
   * @param frame - the current stack frame
   * @param n - the for statement parse tree
   * @param r - the value to receive the return value from the statement
   * @return true to halt further processing (i.e. a return statement has been executed)
   * @exception Symbian - a leaving function
   */
#ifdef EXECUTE_PARSENODES
  bool executeForStat(skStackFrame& frame,skForNode * n,skRValue& r);
#else
  bool executeForStat(skStackFrame& frame,skCompiledCode& code,USize& pc,skRValue& r,bool has_step);
#endif

  // Misc runtime routines

  /** Adds a new local variable to the current list
   * @param var - the local variables
   * @param name - the name of the variable
   * @param value  - the value of the variable
   * @exception Symbian - a leaving function
   */
  void addLocalVariable(skRValueTable& var,const skString& name,skRValue value); 
  /** This method checks whether a field name includes the indirection character
   * @param ctxt - the current source code context
   * @param obj - the object owning the current method
   * @param var - the local variables
   * @param name - the name being checked
   * @exception Symbian - a leaving function
   */
  inline skString checkIndirectId(skStackFrame& frame,const skString& name); 
  /** This method finds a value associated with a given name
   * @param frame - the current stack frame
   * @param name - the name being checked
   * @param array_index - the parse tree for the array index (if any)
   * @param attribute - the name of the attribute, if present
   * @return the value associated with the name
   * @exception Symbian - a leaving function
   */
#ifdef EXECUTE_PARSENODES
  skRValue findValue(skStackFrame& frame,const skString& name,skExprNode * array_index,const skString& attribute); 
#else
  skRValue findValue(skStackFrame& frame,const skString& name,const skRValue * array_index,const skString& attribute); 
#endif
  /** This method follows a dotted list of id's to retrieve the associated value
   * @param frame - the current stack frame
   * @param idlist - the list of ids in the dotted name
   * @param object - this object receives the value corresponding to the id list
   * @exception Symbian - a leaving function
   */
#ifdef EXECUTE_PARSENODES
  void followIdList(skStackFrame& frame,skIdListNode * idList,skRValue& object); 
#else
  void followIdList(skStackFrame& frame,skCompiledCode& code,USize& pc,int num_ids,skRValue& object); 
#endif


  /** This method extracts a value of the form foo[1] - first dereferencing foo
   * @param frame - the current stack frame
   * @param robject - the object owning the field name
   * @param field_name - the field name being accessed
   * @param array_index - the parse tree for the array index (if any)
   * @param attrib - the name of the attribute, if present
   * @param ret - the value to receive the associated value
   * @exception Symbian - a leaving function
   */
#ifdef EXECUTE_PARSENODES
  bool extractFieldArrayValue(skStackFrame& frame,skRValue& robject,const skString& field_name,skExprNode * array_index,const skString& attrib,skRValue& ret);
#else
  bool extractFieldArrayValue(skStackFrame& frame,skRValue& robject,const skString& field_name,const skRValue& array_index,const skString& attrib,skRValue& ret);
#endif
  /** This method extracts a value of the form robject[1] - assumes robject is already a collection object
   * @param frame - the current stack frame
   * @param robject - the object owning the field name
   * @param array_index - the parse tree for the array index (if any)
   * @param attrib - the name of the attribute, if present
   * @param ret - the value to receive the associated value
   */
#ifdef EXECUTE_PARSENODES
  bool extractArrayValue(skStackFrame& frame,skRValue& robject,skExprNode * array_index,const skString& attrib,skRValue& ret) ;
#else
  bool extractArrayValue(skStackFrame& frame,skRValue& robject,const skRValue& array_index,const skString& attrib,skRValue& ret) ;
#endif
  /** This method extracts an instance variable with the given name
   * @param frame - the current stack frame
   * @param robject - the object owning the field name
   * @param name - the field name being accessed
   * @param attrib - the name of the attribute, if present
   * @param ret - the value to receive the associated value
   * @exception Symbian - a leaving function
   */
  bool extractValue(skStackFrame& frame,skRValue& robject,const skString& name,const skString& attrib,skRValue& ret) ;

  /** This method is a wrapper around calling the setValueAt method
   * @param frame - the current stack frame
   * @param robject - the object owning the field name
   * @param array_index - the parse tree for the array index (if any)
   * @param attr - the name of the attribute, if present
   * @param value - the value to be set
   * @exception Symbian - a leaving function
   */
#ifdef EXECUTE_PARSENODES
  bool insertArrayValue(skStackFrame& frame,skRValue& robject, skExprNode * array_index,const skString& attr,const skRValue& value);
#else
  bool insertArrayValue(skStackFrame& frame,skCompiledCode& code,USize& pc,skRValue& robject, const skString& attr,const skRValue& value);
#endif
  /** This method is a wrapper around calling the setValue method
   * @param ctxt - the current source code context
   * @param robject - the object owning the field name
   * @param name - the field name being accessed
   * @param attr - the name of the attribute, if present
   * @param value - the value to be set
   * @exception Symbian - a leaving function
   */
  bool insertValue(skStackFrame& frame,skRValue& robject,const skString& name, const skString& attr,const skRValue& value);

#ifndef EXECUTE_PARSENODES
  void getIdNode(skCompiledCode& code,USize& pc,skString& id,bool& has_array,bool& is_method);
  void getIdNodes(skCompiledCode& code,USize& pc,int& num_ids,skString& attribute);
#endif

  // Variables
  /** this is the list of global variables */
  skRValueTable m_GlobalVars; 
  /** this flag controls whether the interpreter outputs tracing information about the execution of statements */
  bool m_Tracing; 
  /** This variable points to an associated object for capturing tracing output */
  skTraceCallback * m_TraceCallback; 
  /** This variable points to an associated object which receives information about which statements are being executed */
  skStatementStepper * m_StatementStepper; // the statement stepper
  /**
   * null object
   */
  skNull m_Null;
};      
//---------------------------------------------------
inline skString skInterpreter::checkIndirectId(skStackFrame& frame,const skString& name)
//---------------------------------------------------
{
  // look for an initial "@" in a field name, and de-reference it if necessary
  skString ret=name;
  if (name.at(0)=='@'){
    ret=name.substr(1,name.length()-1);
    skRValue new_name=findValue(frame,ret,0,skString());
    ret=new_name.str();
  }
  return ret;
}       
//---------------------------------------------------
inline skNull& skInterpreter::getNull()
//---------------------------------------------------
{
  return m_Null;
}

xskNAMED_LITERAL(OnANonObject,skSTR(" on a non-object\n"));

#endif


