/************************************************************************************//**
 * @file sxCEGUIEventConnection.h
 * @brief Class declaration for RFSX EventConnection object
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

// change these to your
#ifndef _SX_EVENTCONNECTION_H_
#define _SX_EVENTCONNECTION_H_

#include <RFSX.h>

// sxEventConnection class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		bool connected();
		void disconnect();
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxEventConnection

#define SXEVENTCONNECTION_METHODS 2

namespace RFSX
{

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE(CEGUI::RefCounted<CEGUI::BoundSlot> slot);
	virtual ~SX_IMPL_TYPE();
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual int executableType() const { return RFSXU_EVENTCONNECTION; }

	CEGUI::RefCounted<CEGUI::BoundSlot>& Slot() { return m_Slot; }

protected:
	SX_METHOD_DECL(connected);
	SX_METHOD_DECL(disconnect);

private:
	CEGUI::RefCounted<CEGUI::BoundSlot> m_Slot;

	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_EVENTCONNECTION_H_
