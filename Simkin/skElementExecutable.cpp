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

* $Id: skElementExecutable.cpp,v 1.16 2003/05/14 22:00:10 simkin_cvs Exp $
*/

#include "skElementExecutable.h"
#include "skExpatParser.h"
#include "skInputSource.h"
#include "skOutputDestination.h"
//------------------------------------------
skElementExecutable::skElementExecutable() 
//------------------------------------------
{
}
//------------------------------------------
skElementExecutable::skElementExecutable(const skString& scriptLocation,  skInputSource& in,skExecutableContext& context)
//------------------------------------------
{
  load(scriptLocation,in,context);
}
//------------------------------------------
skElementExecutable::skElementExecutable(const skString& fileName,skExecutableContext& context)
//------------------------------------------
{
  load(fileName,context);
}
//------------------------------------------
void skElementExecutable::load(const skString& scriptLocation,skInputSource& in,skExecutableContext& context)
//------------------------------------------
{
  skExpatParser parser;
  skElement * elem=parser.parse(in,context);
  setElement(scriptLocation,elem,true);
}
//------------------------------------------
void skElementExecutable::load(const skString& fileName,skExecutableContext& context)
//------------------------------------------
{
  skInputFile in(fileName);
  load(fileName,in,context);
}
//------------------------------------------
void skElementExecutable::save(skOutputDestination& out) 
//------------------------------------------
{
  getElement()->write(out);
}
//------------------------------------------
void skElementExecutable::save(const skString& fileName) 
//------------------------------------------
{
  skOutputFile out(fileName);
  save(out);
}
//------------------------------------------
skElementExecutable::skElementExecutable(const skElementExecutable& other)
//------------------------------------------
{
}
//------------------------------------------
skElementExecutable& skElementExecutable::operator=(const skElementExecutable& other)
//------------------------------------------
{
  return *this;
}
