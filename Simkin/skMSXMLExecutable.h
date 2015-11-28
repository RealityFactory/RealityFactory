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

* $Id: skMSXMLExecutable.h,v 1.1 2003/01/23 14:22:09 simkin_cvs Exp $
*/

#ifndef skMSXMLEXECUTABLE_H
#define skMSXMLEXECUTABLE_H

#include "skMSXMLElementObject.h"

/**
 * This class is a base class for objects which store their data and methods in an XML document
 * Each instance is associated with an XML document which is loaded on construction. 
 */
class CLASSEXPORT skMSXMLExecutable : public skMSXMLElementObject {
 public:
  /**
   * Default Constructor.
   */ 
  skMSXMLExecutable();
  /**
   * Constructor - passed the name of a file to load an XML document from
   * @param fileName name of XML file
   * @param context context object to receive errors
   */
  skMSXMLExecutable(const skString& fileName,skExecutableContext& context);
  /**
   * Constructor taking a String containing an XML document
   * @param scriptLocation the name of the XML source
   * @param in - string containing the XML
   * @param context context object to receive errors
   */
  skMSXMLExecutable(const skString& scriptLocation,const skString& in,skExecutableContext& context);
  /**
   * This method causes the XML to be reloaded. If there is an error, this method throws an skXMLParseException.
   * @param scriptLocation the name of the XML source
   * @param in - string containing the XML
   * @param context context object to receive errors
   */
  void load(const skString& scriptLocation,const skString& in,skExecutableContext& context);
  /**
   * This method causes the XML to be loaded from a file.If there is an error, this method throws an skXMLParseException.
   * @param fileName name of XML file
   * @param context context object to receive errors
   */
  void load(const skString& fileName,skExecutableContext& context);
  /**
   * This method saves the current state as an XML document out to the given file
   * @param fileName name of XML file
   */
  void save(const skString& fileName);
 private:
  /**
   * Executables can't be copied
   */
  skMSXMLExecutable(const skMSXMLExecutable&);
  /**
   * Executables can't be copied
   */
  skMSXMLExecutable& operator=(const skMSXMLExecutable&);

};
#endif
