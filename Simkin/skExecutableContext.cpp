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

  $Id: skExecutableContext.cpp,v 1.3 2003/01/20 18:48:18 simkin_cvs Exp $
*/

#include "skExecutableContext.h"
#include "skStackFrame.h"
//------------------------------------------
void skExecutableContext::pushStackFrame(skStackFrame * frame)
//------------------------------------------
{
  if (m_TopFrame)
    frame->setParentFrame(m_TopFrame);
  m_TopFrame=frame;
}
//------------------------------------------
void skExecutableContext::popStackFrame()
//------------------------------------------
{
  if (m_TopFrame){
    skStackFrame * parent=m_TopFrame->getParentFrame();
    m_TopFrame->setParentFrame(0);
    m_TopFrame=parent;
  }
}
