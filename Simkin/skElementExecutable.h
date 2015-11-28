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

* $Id: skElementExecutable.h,v 1.10 2003/01/20 18:48:18 simkin_cvs Exp $
*/

#ifndef skELEMENTEXECUTABLE_H
#define skELEMENTEXECUTABLE_H

#include "skElementObject.h"
class CLASSEXPORT skInputSource;

/**
 * This class is a base class for objects which store their data and methods in an XML document
 * Each instance is associated with an XML document which is loaded on construction. 
 * This version uses the Expat XML parser to read documents.
 */
class CLASSEXPORT skElementExecutable : public skElementObject {
 public:
  /**
   * Default Constructor.
   */ 
  skElementExecutable();
  /**
   * Constructor - passed the name of a file to load an XML document from
   * @param fileName name of XML file
   * @param context context object to receive errors
   */
  skElementExecutable(const skString& fileName,skExecutableContext& context);
  /**
   * Constructor taking an input source containing an XML document
   * @param scriptLocation the name of the XML source
   * @param in an input source containing the document
   * @param context context object to receive errors
   */
  skElementExecutable(const skString& scriptLocation,skInputSource& in,skExecutableContext& context);
  /**
   * This method causes the  to be reloaded
   * @param scriptLocation the name of the XML source
   * @param in an input source containing the document
   * @param context context object to receive errors
   */
  void load(const skString& scriptLocation,skInputSource& in,skExecutableContext& context);
  /**
   * This method causes the  to be loaded from a file
   * @param fileName name of XML file
   * @param context context object to receive errors
   */
  void load(const skString& fileName,skExecutableContext& context);
  /**
   * Call this method to save the state of the object back to a destination
   */
  void save(skOutputDestination& out);
  /**
   * This method saves the current state as an  document out to the given file
   */
  void save(const skString& fileName);
 private:
  /**
   * ElementExecutables can't be copied
   */
  skElementExecutable(const skElementExecutable&);
  /**
   * ElementExecutables can't be copied
   */
  skElementExecutable& operator=(const skElementExecutable&);
};
#endif
