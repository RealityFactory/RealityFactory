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

* $Id: skStackFrame.cpp,v 1.3 2003/04/14 15:24:57 simkin_cvs Exp $
*/

#include "skStackFrame.h"
#include "skExecutableContext.h"

//---------------------------------------------------
skStackFrame::skStackFrame(const skString& location,
            skiExecutable * obj,
            skRValueTable& vars,
            skExecutableContext& context)
//---------------------------------------------------
       : m_Location(location),m_LineNum(0),m_Context(context),m_Object(obj),
        m_Vars(vars),m_ParentFrame(0)
{
  m_Context.pushStackFrame(this);
}
//---------------------------------------------------
skStackFrame::~skStackFrame()
//---------------------------------------------------
{
  m_Context.popStackFrame();
}
