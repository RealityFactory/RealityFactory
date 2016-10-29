/************************************************************************************//**
 * @file sxCEGUIImagesetManager.h
 * @brief Class declaration for RFSX ImagesetManager object
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

// change these to your
#ifndef _SX_IMAGESETMANAGER_H_
#define _SX_IMAGESETMANAGER_H_

#include "RFSX.h"

// sxImagesetManager class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		sxImageset createImageset(skString name, sxTexture texture);
		sxImageset createImageset(skString filename, skString resourcegroup="");
		sxImageset createImagesetFromImageFile(skString name, skString filename, skString resourcegroup="");
		void destroyImageset(sxImageset imageset);
		void destroyImageset(skString name);
		void destroyAllImagesets();
		sxImageset getImageset(skString name);
		bool isImagesetPresent(skString name);
		void notifyScreenResolution(sxSize size);

	for each: ImagesetIterator
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxImagesetManager

#define SXIMAGESETMANAGER_METHODS 7

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
	virtual int executableType() const { return RFSXU_IMAGESETMANAGER; }

protected:
	SX_METHOD_DECL(createImageset);
	SX_METHOD_DECL(createImagesetFromImageFile);
	SX_METHOD_DECL(destroyImageset);
	SX_METHOD_DECL(destroyAllImagesets);
	SX_METHOD_DECL(getImageset);
	SX_METHOD_DECL(isImagesetPresent);
	SX_METHOD_DECL(notifyScreenResolution);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;

};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_IMAGESETMANAGER_H_
