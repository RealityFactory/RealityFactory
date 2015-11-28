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

  $Id: skStringList.h,v 1.10 2003/03/24 19:02:16 simkin_cvs Exp $
*/
#ifndef STRINGLIST_H
#define STRINGLIST_H

#include "skString.h"
#include "skValist.h"

#ifdef INSTANTIATE_TEMPLATES
EXTERN_TEMPLATE template class CLASSEXPORT skTVAList<skString>;
#endif

/**
 * This class is a list of Strings, held by value
 */
class  skStringList : public  skTVAList<skString> {
};
#endif
