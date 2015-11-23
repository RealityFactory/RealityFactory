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

* $Id: skScriptedExecutable.h,v 1.14 2001/11/22 11:13:21 sdw Exp $
*/

#ifndef skSCRIPTEDEXECUTABLE_H
#define skSCRIPTEDEXECUTABLE_H

#include "skTreeNodeObject.h"

/**
 * This class provides an executable whose additional fields and methods are taken from a TreeNode file - most
 * of the behaviour is defined in the base class skTreeNodeObject
 */
class CLASSEXPORT skScriptedExecutable : public skTreeNodeObject
{ 
 public:
  /**
   * Constructor - takes the name of the file from which the treenode will be loaded
   */
  skScriptedExecutable(const skString& filename);
  /**
   * Virtual destructor - so that derived class destructors will be called
   */
  virtual ~skScriptedExecutable();
 private:
  /**
   * Executables can't be copied
   */
  skScriptedExecutable(const skScriptedExecutable&);
  /**
   * Executables can't be copied
   */
  skScriptedExecutable& operator=(const skScriptedExecutable&);
};
#endif
