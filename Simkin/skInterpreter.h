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

* $Id: skInterpreter.h,v 1.24 2001/11/22 11:13:21 sdw Exp $
*/
#ifndef skINTERPRETER_H
#define skINTERPRETER_H

#include "skRValue.h"
#include "skiExecutable.h"
#include "skNull.h"

class CLASSEXPORT skRValueArray;
class CLASSEXPORT skParseNode;
class CLASSEXPORT skStringList;
class CLASSEXPORT skMethodDefNode;
class CLASSEXPORT skTraceCallback;
class CLASSEXPORT skStatementStepper;

/**
 * This class stores info about the current execution context - such the name of the current script and the line number being executed.
 */

class CLASSEXPORT skContext 
{
 public:
  /** Constructor
   * @param location - the name of the source file
   */
  skContext(const skString& location):
    m_Location(location)
    {};
  /** location during script execution */
  skString m_Location; 
  /** line number during script execution */
  int m_LineNum; 
};

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
   * @return returns a parse tree, if the syntax was valid. The caller must free this tree.
   * @exception skParseException - if a syntax error is encountered
   */
  skMethodDefNode * parseString(const skString& location,const skString& code);	
  /**
   * this function parses the script in the code variable and returns
   * a parse tree if there are no syntax errors. It assumes that the script does *not* contain parameters and enclosing braces.
   * If there are syntax errors the function throws an skParseException object
   * @param location - a string describing where this code is located, this will appear in any error messages
   * @param paramNames - a list of parameter names (hence the name "ExternalParams"
   * @param code - a string of Simkin code, which does *not* include the parameter declarations
   * @return returns a parse tree, if the syntax was valid. The caller must free this tree.
   * @exception skParseException - if a syntax error is encountered
   */
  skMethodDefNode * parseExternalParams(const skString& location,skStringList& paramNames,const skString& code);
  /**
   * this function parses and executes script which is assumed to belong
   * to the object passed in.
   * @param location - a textual description of the location of the script (this appears in error messages)
   * @param obj - the executable object which "owns" the script
   * @param args - an array of arguments to the function, which are passed as parameters to the script
   * @param code - a string of Simkin script, including parameter declarations
   * @param return_value - an RValue which receives the result of the method call
   * @param parseTree - if you supply this pointer, the parse tree is assigned to it, and you must delete it yourself. Without the parameter the parse tree will be deleted by the interpreter. The parse tree can be used in a cache and passed to executeParseTree later.
   * @exception skParseException - if a syntax error is encountered
   * @exception skRuntimeException - if an error occurs while the script is running
   */
  void executeString(const skString& location,skiExecutable * obj,const skString& code,skRValueArray&  args,skRValue& return_value,skMethodDefNode ** parseTree);

  /**
   * this function parses and executes script with externally declared parameters which is assumed to belong to the object passed in.
   * @param location - a textual description of the location of the script (this appears in error messages)
   * @param obj - the executable object which "owns" the script
   * @param paramNames - a list of parameter names (hence the name "ExternalParams"
   * @param args - an array of arguments to the function, which are passed as parameters to the script
   * @param code - a string of Simkin script, which does *not* include parameter declarations
   * @param return_value - an RValue which receives the result of the method call
   * @param parseTree - if you supply this pointer, the parse tree is assigned to it, and you must delete it yourself. Without the parameter the parse tree will be deleted by the interpreter. The parse tree can be used in a cache and passed to executeParseTree later.
   * @exception skParseException - if a syntax error is encountered
   * @exception skRuntimeException - if an error occurs while the script is running
   */
  void executeStringExternalParams(const skString& location,skiExecutable * obj,skStringList& paramNames,const skString& code,skRValueArray&  args,skRValue& r,skMethodDefNode ** keepParseTree);

  /**
   * this function executes some script that has already been parsed into a parse tree. 
   * @param location - a textual description of the location of the script (this appears in error messages)
   * @param obj - the executable object which "owns" the script
   * @param parseTree - a parse tree that has been generated by one of the parse or execute functions of the Interpreter
   * @param args - an array of arguments to the function, which are passed as parameters to the script
   * @param return_value - an RValue which receives the result of the method call
   * @exception skRuntimeException - if an error occurs while the script is running
   */
  void executeParseTree(const skString& location,skiExecutable * obj,skMethodDefNode * parseTree,skRValueArray&  args,skRValue& return_value);
  //------------------------
  // Global Variable methods
  //------------------------
   
  /**
   * this method adds a global variable to the list held by this interpreter. If the variable already has a value, it is replaced with the new one
   * @param name - the name of the global variable
   * @param value - the value of the global variable, which can be any RValue
   */
  void addGlobalVariable(const skString& name,skRValue value);
  /**
   * this method removes a global variable from the list held by this interpreter
   * @param name - the name of the global variable
   */
  void removeGlobalVariable(const skString& name);
  /**
   * this method finds the value of a global variable by name
   * @param name - the name of the global variable
   * @param return_value - an RValue which receives the value of the variable
   * @return true if the variable was found, otherwise false
   */
  bool findGlobalVariable(const skString& name,skRValue& return_value);

  //--------------------------------------------------------
  // Interpreter is an Executable which exposes some fields
  //--------------------------------------------------------
    
  /**
   * this method is used to set the Tracing variable in the interpreter. This will show method calls as the script is executed
   * In Simkin call Interpreter.Tracing=true
   */
  bool setValue(const skString& s,const skString& attribute,const skRValue& v);
    
  //---------------------------------
  // accessing the global interpreter
  //---------------------------------

  /**
   * this method returns the global interpreter which was previously set with setInterpreter
   */
  static skInterpreter * getInterpreter();
  /**
   * this method constructs the global interpreter used by the process
   */
  static void setInterpreter(skInterpreter *);
    
  //------------------------
  // Tracing methods
  //------------------------

  /** output a message to the current trace output
   * @param msg the message to show
   */
  void trace(const skString& msg);
  /** this method sets an object to receive trace messages. Pass 0 to fall back to standard mechanism (using skTracer)
   * @param callback the callback object, or 0 to fall back to using skTracer
   */
  void setTraceCallback(skTraceCallback * callback);

  /** this method sets an object to be called each time a statement is executed.
   * @param stepper the stepper object, or 0 to clear
   */
  void setStatementStepper(skStatementStepper * stepper);

  /** creates and throws a skRuntimeException */
  void runtimeError(skContext& ctxt,const skString& msg); 

  //---------------------------
  // Constructor and Destructor
  //---------------------------
    
  /**
   * Constructor - adds the interpreter as a global variable with the name "Interpreter"
   */
  skInterpreter();
  /**
   * Destructor - deletes the global variable list
   */
  ~skInterpreter();
    
  /** This variable points to the private implementation object */
  class P_Interpreter *	pimp;
  
  /**
   * public null object
   */
  static skNull g_Null;

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
};	


#endif


