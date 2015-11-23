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

* $Id: skTraceCallback.h,v 1.3 2001/11/22 11:13:21 sdw Exp $
*/
#ifndef skTRACECALLBACK_H
#define skTRACECALLBACK_H
#include "skString.h"

/**
 * This abstract class is used to provide an interface for the Interpreter to pass
 * trace messages to
 */

class CLASSEXPORT skTraceCallback {
 public:
  /** This method receives trace messages from the interpreter and Simkin scripts
   * @param msg  the trace message
   */
  virtual void trace(const skString& msg)=0;
};
#endif
