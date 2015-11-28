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

  $Id: skExecutableContext.h,v 1.7 2003/04/14 15:24:57 simkin_cvs Exp $
*/
#ifndef SKEXECUTABLECONTEXT_H
#define SKEXECUTABLECONTEXT_H

#include "skGeneral.h"
#ifndef EXCEPTIONS_DEFINED
#include "skScriptError.h"
#endif

class CLASSEXPORT skInterpreter;
class CLASSEXPORT skStackFrame;

/**
* This class holds information passed to all methods via the siExecutable interface
*/
class CLASSEXPORT skExecutableContext
#ifdef __SYMBIAN32__
: public CBase
#endif
{
public:
  /** 
   * Constructs a new context
   * @param interp the interpreter within the context
   */
  skExecutableContext(skInterpreter * interp)
    : m_TopFrame(0),m_Interpreter(interp)
    {
    }
  skInterpreter *  getInterpreter();
#ifndef EXCEPTIONS_DEFINED
  /** holds a reference to an error encountered while a script was executing.
   * \remarks This is only available if EXCEPTIONS_DEFINED is undefined. It replaces the default mechanism of throwing exceptions
   */
  skScriptError&  getError();
#endif
  
  /**
   * called when a new stack frame is created
   */
  void pushStackFrame(skStackFrame * frame);
  /**
   * called when a method has finished
   */
  void popStackFrame();
  /**
   * returns the current top frame
   */
  skStackFrame *  getTopFrame();
  /**
   * set the top frame
   */
  void            setTopFrame(skStackFrame * frame);
private:
  skStackFrame *  m_TopFrame;
  /** holds a reference to the calling interpreter */
  skInterpreter *   m_Interpreter;
#ifndef EXCEPTIONS_DEFINED
  /** holds a reference to an error encountered - where exception support is not available */
  skScriptError   m_Error;
#endif
};

//------------------------------------------
inline skInterpreter *  skExecutableContext::getInterpreter()
//------------------------------------------
{
  return m_Interpreter;
}
//------------------------------------------
inline void skExecutableContext::setTopFrame(skStackFrame * frame)
//------------------------------------------
{
  m_TopFrame=frame;
}
//------------------------------------------
inline skStackFrame *  skExecutableContext::getTopFrame()
//------------------------------------------
{
  return m_TopFrame;
}
#ifndef EXCEPTIONS_DEFINED
//------------------------------------------
inline skScriptError&  skExecutableContext::getError()
//------------------------------------------
{
  return m_Error;
}
#endif

#endif
