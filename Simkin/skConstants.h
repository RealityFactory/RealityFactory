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

  * $Id: skConstants.h,v 1.10 2003/11/20 17:20:24 sdw Exp $
  */


#ifndef skCONSTANTS_H
#define skCONSTANTS_H

#include "skString.h"

/**
 * This file contains literal strings that are used in the Simkin library
 */

xskNAMED_LITERAL(one,skSTR("1"));
xskNAMED_LITERAL(colon,skSTR(":"));
xskNAMED_LITERAL(cr,skSTR("\n"));
xskNAMED_LITERAL(tab,skSTR("\t"));
xskLITERAL(true);
xskLITERAL(false);
xskLITERAL(function);
xskLITERAL(name);
xskLITERAL(null);
xskLITERAL(trace);
xskLITERAL(isObject);
xskLITERAL(length);
xskLITERAL(charAt);
xskLITERAL(next);
xskLITERAL(reset);
xskLITERAL(numChildren);
xskLITERAL(enumerate);
xskLITERAL(label);
xskLITERAL(self);
xskLITERAL(tracing);
xskLITERAL(Interpreter);
xskLITERAL(debugBreak);
xskLITERAL(breakpoint);
xskLITERAL(toInt);
xskLITERAL(toFloat);
xskLITERAL(toString);
xskLITERAL(toChar);
xskLITERAL(toBool);

xskNAMED_LITERAL(leftbracket,skSTR("["));
xskNAMED_LITERAL(rightbracket,skSTR("]"));

xskNAMED_LITERAL(left_bracket,skSTR(" ["));
xskNAMED_LITERAL(right_bracket,skSTR("] "));
xskNAMED_LITERAL(left_brace,skSTR(" {\n"));
xskNAMED_LITERAL(right_brace,skSTR(" }\n"));
xskLITERAL_STRING(Dash,"-");

#define c_colon ':'
#define c_leftbrace '{'
#define c_rightbrace '}'

#endif
