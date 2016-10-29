/************************************************************************************//**
 * @file sxCEGUISchemeManager.h
 * @brief Class declaration for RFSX SchemeManager object
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_SCHEMEMANAGER_H_
#define _SX_SCHEMEMANAGER_H_

#include "RFSX.h"

// sxSchemeManager class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		sxScheme loadScheme(skString filename, skString resourcegroup="");
		void unloadScheme(skString name);
		bool isSchemePresent(skString name);
		sxScheme getScheme(skString name);
		void unloadAllSchemes();

	for each: SchemeIterator
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxSchemeManager

#define SXSCHEMEMANAGER_METHODS 5

namespace RFSX
{

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE();
	virtual ~SX_IMPL_TYPE();
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual skExecutableIterator* createIterator(const skString& qualifier) { return createIterator(); }
	virtual skExecutableIterator* createIterator();
	virtual int executableType() const { return RFSXU_SCHEMEMANAGER; }

protected:
	SX_METHOD_DECL(loadScheme);
	SX_METHOD_DECL(unloadScheme);
	SX_METHOD_DECL(isSchemePresent);
	SX_METHOD_DECL(getScheme);
	SX_METHOD_DECL(unloadAllSchemes);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_SCHEMEMANAGER_H_
