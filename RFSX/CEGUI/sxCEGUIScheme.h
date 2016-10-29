/************************************************************************************//**
 * @file sxCEGUIScheme.h
 * @brief Class declaration for RFSX Scheme object
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_SCHEME_H_
#define _SX_SCHEME_H_

#include "RFSX.h"

// sxScheme class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		void loadResources();
		void unloadResources();
		bool resourcesLoaded();
		skString getName();
		void setDefaultResourceGroup(skString resourceGroup);
		skString getDefaultResourceGroup();
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxScheme

#define SXSCHEME_METHODS 6

namespace RFSX
{

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE(CEGUI::Scheme *scheme);
	virtual ~SX_IMPL_TYPE();
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual int executableType() const { return RFSXU_SCHEME; }

	CEGUI::Scheme* Scheme() { return m_Scheme; }

protected:
	SX_METHOD_DECL(loadResources);
	SX_METHOD_DECL(unloadResources);
	SX_METHOD_DECL(resourcesLoaded);
	SX_METHOD_DECL(getName);
	SX_METHOD_DECL(setDefaultResourceGroup);
	SX_METHOD_DECL(getDefaultResourceGroup);

private:
	CEGUI::Scheme*	m_Scheme;

	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_SCHEME_H_
