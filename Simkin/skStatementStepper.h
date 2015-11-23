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

* $Id: skStatementStepper.h,v 1.3 2001/11/22 11:13:21 sdw Exp $
*/
#ifndef skSTATEMENTSTEPPER_H
#define skSTATEMENTSTEPPER_H

#include "skRValueTable.h"

/**
 * This abstract class is used to provide an interface for the Interpreter to call
 * each time a statement is executed
 */

class CLASSEXPORT skStatementStepper {
 public:
  /** This method is called each time a statement is executed by the interpreter, just before the statement is performed
   * @return false to halt the execution of the current method
   * @param location the location of the script
   * @param line the line the statement was on
   * @param obj the object owning the current method
   * @param local_vars the local variables for the current method
   * @param statement_type an integer indicating the type of statement, a constant s_ value from skParseNode.h 
   */
  virtual bool statementExecuted(const skString& location, int line_num,skiExecutable * obj,skRValueTable& local_vars,int statement_type)=0;
  /** This method is called each time a compound statement is executed by the interpreter, just before the statement is performed
   * @return false to halt the execution of the current method
   * @param location the location of the script
   * @param line the line the statement was on
   * @param obj the object owning the current method
   * @param local_vars the local variables for the current method
   */
  virtual bool compoundStatementExecuted(const skString& location, int line_num,skiExecutable * obj,skRValueTable& local_vars)=0;
};
#endif
