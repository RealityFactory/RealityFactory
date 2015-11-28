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

* $Id: skXMLExecutable.h,v 1.14 2003/03/18 19:36:13 simkin_cvs Exp $
*/

#ifndef skXMLEXECUTABLE_H
#define skXMLEXECUTABLE_H

#include "skXMLElementObject.h"
#include <xercesc/sax/InputSource.hpp>
#include <iostream.h>

/**
 * This class is a base class for objects which store their data and methods in an XML document
 * Each instance is associated with an XML document which is loaded on construction. 
 */
class CLASSEXPORT skXMLExecutable : public skXMLElementObject {
 public:
  /**
   * Default Constructor.
   */ 
  skXMLExecutable();
  /**
   * Constructor - passed the name of a file to load an XML document from
   */
  skXMLExecutable(const skString& fileName);
  /**
   * Constructor taking an input stream containing an XML document
   * @param scriptLocation - the name of the XML document
   * @param in - stream containing the XML
   */
  skXMLExecutable(const skString& scriptLocation,InputSource& in);
  /**
   * Constructor taking a String containing an XML document
   * @param scriptLocation - the name of the XML document
   * @param in - string containing the XML
   */
  skXMLExecutable(const skString& scriptLocation,const skString& in);
  /**
   * This method causes the XML to be reloaded
   * @param scriptLocation - the name of the XML document
   * @param in - stream containing the XML
   */
  void load(const skString& scriptLocation,InputSource& in);
  /**
   * This method causes the XML to be reloaded
   * @param in - string containing the XML
   */
  void load(const skString& scriptLocation,const skString& in);
  /**
   * This method causes the XML to be loaded from a file
   */
  void load(const skString& fileName);
  /**
   * This method saves the current state as an XML document out to the given file
   */
  void save(const skString& fileName);
 private:
  /**
   * Executables can't be copied
   */
  skXMLExecutable(const skXMLExecutable&);
  /**
   * Executables can't be copied
   */
  skXMLExecutable& operator=(const skXMLExecutable&);

};
#endif
