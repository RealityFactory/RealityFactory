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

* $Id: skStackFrame.h,v 1.7 2003/04/07 21:59:14 simkin_cvs Exp $
*/
#ifndef skSTACKFRAME_H
#define skSTACKFRAME_H

#include "skString.h"

class CLASSEXPORT skiExecutable;
class CLASSEXPORT skRValueTable;
class CLASSEXPORT skExecutableContext;
/**
 * This class stores info about the current stack frame - such the name of the current script 
 * and the line number being executed.
 * This class is used internally by the Interpreter, you do not normally need to use it.
 */

class CLASSEXPORT skStackFrame 
#ifdef __SYMBIAN32__
: public CBase
#endif
{
public:
  /** Constructor
   * @param location - the name of the source file
   * @param obj the object owning the script
   * @param vars the local variables at this part of the script
   * @param context context object to receive errors
   */
  skStackFrame(const skString& location,
            skiExecutable * obj,
            skRValueTable& vars,
            skExecutableContext& context);
  ~skStackFrame();
  /** returns the location of the code being executed in this stack frame 
   * @return the current name of the script
   */
  skString              getLocation() const;
  /** returns the current line number 
   * @return the current line number in the script
   */
  int                   getLineNum() const;
  /** updates the line number for the frame 
   * @param line_num the new line number
   */
  void                  setLineNum(int line_num);
  /** returns the context within which the stack frame is executing 
   * @return the owning ExecutableContext
   */
  skExecutableContext&  getContext() const;
  /** returns the object owning the stack frame 
   * @return the owning object
   */
  skiExecutable *       getObject() const;
  /** returns the local variables for this stack frame 
   * @return the current local variables
   */
  skRValueTable&        getVars() const;
  /** returns the parent frame of this one 
   * @return the StackFrame that called this one
   */
  skStackFrame *        getParentFrame() const;
  /** sets the parent frame of this frame 
   * @param frame set the parent stack frame
   */
  void                  setParentFrame(skStackFrame * frame);
private:
  /**
   * Copy constructor private - to prevent copying
   */
  skStackFrame(const skStackFrame& s)
    : m_Context(s.m_Context),m_Vars(s.m_Vars)
    {
  }
  /**
   * Assignment operator private - to prevent copying
   */
  skStackFrame& operator=(const skStackFrame&){
    return *this;
  }
  /** location during script execution */
  skString              m_Location; 
  /** line number during script execution */
  int                   m_LineNum; 
  /** the current executable context */
  skExecutableContext&  m_Context;
  /** the owner of the current code */
  skiExecutable *       m_Object;
  /** local variables for this frame */
  skRValueTable&        m_Vars;
  /** parent stack frame */
  skStackFrame *        m_ParentFrame;
};

//------------------------------------------
inline skString skStackFrame::getLocation() const
//------------------------------------------
{
  return m_Location;
}
//------------------------------------------
inline int skStackFrame::getLineNum() const
//------------------------------------------
{
  return m_LineNum;
}
//------------------------------------------
inline void skStackFrame::setLineNum(int line_num) 
//------------------------------------------
{
  m_LineNum=line_num;
}
//------------------------------------------
inline skExecutableContext& skStackFrame::getContext() const
//------------------------------------------
{
  return m_Context;
}
//------------------------------------------
inline skiExecutable * skStackFrame::getObject() const
//------------------------------------------
{
  return m_Object;
}
//------------------------------------------
inline skRValueTable& skStackFrame::getVars() const
//------------------------------------------
{
  return m_Vars;
}
//------------------------------------------
inline skStackFrame * skStackFrame::getParentFrame() const
//------------------------------------------
{
  return m_ParentFrame;
}
//------------------------------------------
inline void skStackFrame::setParentFrame(skStackFrame * frame)
//------------------------------------------
{
  m_ParentFrame=frame;
}
#endif

