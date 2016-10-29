/************************************************************************************//**
 * @file sxCEGUIEvent.h
 * @brief Class declaration for RFSX Event object
 * @author Daniel Queteschiner
 * @date November 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_EVENT_H_
#define _SX_EVENT_H_

#include "RFSX.h"

// sxEvent class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		skString getName();
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxEvent
#define SXEVENT_METHODS 1

// Type-check and upcast macro
#define IS_EVENT(o) CheckType<sxEvent>(o, RFSXU_EVENT);

namespace RFSX
{

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE(CEGUI::Event* e);
	virtual ~SX_IMPL_TYPE();
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual int executableType() const { return RFSXU_EVENT; }

	CEGUI::Event* Event() { return m_Event; }

protected:
	SX_METHOD_DECL(getName);

private:
	CEGUI::Event* m_Event;

	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_EVENT_H_
