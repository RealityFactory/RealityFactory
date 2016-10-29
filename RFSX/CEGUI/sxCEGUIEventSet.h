/************************************************************************************//**
 * @file sxCEGUIEventSet.h
 * @brief Class declaration for RFSX EventSet object
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_EVENTSET_H_
#define _SX_EVENTSET_H_

#include "RFSX.h"

// sxEventSet class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		void addEvent(skString name);
		void removeEvent(skString name);
		void removeAllEvents();
		bool isEventPresent(skString name);
		sxEventConnection subscribeEvent(skString name, skString callback_name);
		void fireEvent(skString name, sxEventArgs args, skString eventnamespace="");
		bool isMuted();
		void setMutedState(bool setting);

	for each: EventIterator
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxEventSet

#define SXEVENTSET_METHODS 8

namespace RFSX
{

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE(CEGUI::EventSet* eventSet);
	virtual ~SX_IMPL_TYPE();
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual skExecutableIterator* createIterator(const skString& qualifier) { return createIterator(); }
	virtual skExecutableIterator* createIterator();
	virtual int executableType() const { return RFSXU_EVENTSET; }

	virtual CEGUI::EventSet* EventSet() { return m_EventSet; }

protected:
	SX_METHOD_DECL(addEvent);
	SX_METHOD_DECL(removeEvent);
	SX_METHOD_DECL(removeAllEvents);
	SX_METHOD_DECL(isEventPresent);
	SX_METHOD_DECL(subscribeEvent);
	SX_METHOD_DECL(fireEvent);
	SX_METHOD_DECL(isMuted);
	SX_METHOD_DECL(setMutedState);

	SX_IMPL_TYPE() : m_EventSet(NULL) {}

private:
	CEGUI::EventSet*	m_EventSet;

	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_EVENTSET_H_
