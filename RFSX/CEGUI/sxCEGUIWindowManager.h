/************************************************************************************//**
 * @file sxCEGUIWindowManager.h
 * @brief Class declaration for RFSX WindowManager object
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_WINDOWMANAGER_H_
#define _SX_WINDOWMANAGER_H_

#include "RFSX.h"

// sxWindowManager class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		sxWindow createWindow(skString type, skString name="");
		sxWindow loadWindowLayout(skString filename, skString name_prefix="", skString resourcegroup="");
		sxWindow loadWindowLayout(skString filename, bool generateRandomPrefix);

		//void writeWindowLayoutToStream(string window, OutStream& out, bool writeParent=false) const;
		//void writeWindowLayoutToStream(const Window& window, OutStream& out, bool writeParent=false) const;

		void destroyWindow(skString name);
		void destroyWindow(sxWindow window);
		void destroyAllWindows();

		sxWindow getWindow(skString name);
		bool isWindowPresent(skString name);

		bool isDeadPoolEmpty();
		void cleanDeadPool();

		void renameWindow(sxWindow window, skString new_name);
		void renameWindow(skString window, skString new_name);

		void setDefaultResourceGroup(skString resourceGroup);
		skString getDefaultResourceGroup();

		void lock();
		void unlock();
		bool isLocked();

	for each: WindowIterator
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxWindowManager

#define SXWINDOWMANAGER_METHODS 14

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
	virtual int executableType() const { return RFSXU_WINDOWMANAGER; }

protected:
	SX_METHOD_DECL(createWindow);
	SX_METHOD_DECL(loadWindowLayout);
	SX_METHOD_DECL(destroyWindow);
	SX_METHOD_DECL(destroyAllWindows);
	SX_METHOD_DECL(getWindow);
	SX_METHOD_DECL(isWindowPresent);
	SX_METHOD_DECL(isDeadPoolEmpty);
	SX_METHOD_DECL(cleanDeadPool);
	SX_METHOD_DECL(renameWindow);
	SX_METHOD_DECL(setDefaultResourceGroup);
	SX_METHOD_DECL(getDefaultResourceGroup);
	SX_METHOD_DECL(lock);
	SX_METHOD_DECL(unlock);
	SX_METHOD_DECL(isLocked);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_WINDOWMANAGER_H_
