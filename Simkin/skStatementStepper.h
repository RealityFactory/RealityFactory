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

* $Id: skStatementStepper.h,v 1.10 2003/03/18 19:36:13 simkin_cvs Exp $
*/
#ifndef skSTATEMENTSTEPPER_H
#define skSTATEMENTSTEPPER_H

#include "skGeneral.h"

class CLASSEXPORT skStackFrame;
class CLASSEXPORT skException;

/**
 * This abstract class is used to provide an interface for the Interpreter to call
 * each time a statement is executed
 */

class CLASSEXPORT skStatementStepper {
 public:
  /** This method is called each time a statement is executed by the interpreter, just before the statement is performed
   * @return false to halt the execution of the current method
   * @param stack_frame the current stack frame
   * @param statement_type an integer indicating the type of statement, a constant s_ value from skParseNode.h 
   */
  virtual bool statementExecuted(const skStackFrame& stack_frame,int statement_type)=0;
  /** This method is called each time a compound statement is executed by the interpreter, just before the statement is performed
   * @param stack_frame the current stack frame
   * @return false to halt the execution of the current method
   */
  virtual bool compoundStatementExecuted(const skStackFrame& stack_frame)=0;
  /**
   * This method is called if an exception is encountered while executing a script.
   * @param stack_frame the current stack frame
   * @param e the exception that has occurred
   * @return true to throw the exception, false to ignore and continue
   */
  virtual bool exceptionEncountered(const skStackFrame * stack_frame,const skException& e)=0;
  /**
   * This method is called if the interpreter encounters a breakpoint command.
   * @param stack_frame the current stack frame
   */
  virtual void breakpoint(const skStackFrame * stack_frame)=0;
};
#endif
