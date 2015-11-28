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

* $Id: skMethodTable.h,v 1.9 2003/04/03 16:21:17 simkin_cvs Exp $
*/
#ifndef skMETHODTABLE_H
#define skMETHODTABLE_H

#include "skSHashTable.h"
#include "skParseNode.h"

#ifdef INSTANTIATE_TEMPLATES
EXTERN_TEMPLATE template class CLASSEXPORT skTSHashTable<skMethodDefNode>;
#endif

/**
 * This class can be used to cache parse trees. It is a hashtable mapping method name to parse tree
 */
class CLASSEXPORT skMethodTable : public skTSHashTable<skMethodDefNode>
{
};

#endif

