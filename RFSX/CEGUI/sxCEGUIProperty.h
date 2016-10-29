/************************************************************************************//**
 * @file sxCEGUIProperty.h
 * @brief Class declaration for RFSX Property object
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_PROPERTY_H_
#define _SX_PROPERTY_H_

#include "RFSX.h"

// sxProperty class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
	    skString getHelp();
		skString getName();
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxProperty

#define SXPROPERTY_METHODS 2

namespace RFSX
{

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE(CEGUI::Property* prop);
	virtual ~SX_IMPL_TYPE();
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual int executableType() const { return RFSXU_PROPERTY; }

	CEGUI::Property* Property() { return m_Property; }

protected:
	SX_METHOD_DECL(getHelp);
	SX_METHOD_DECL(getName);

private:
	CEGUI::Property* m_Property;

	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_PROPERTY_H_
