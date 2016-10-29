/************************************************************************************//**
 * @file sxCEGUIGlobalEventSet.h
 * @brief Class declaration for RFSX GlobalEventSet object
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_GLOBALEVENTSET_H_
#define _SX_GLOBALEVENTSET_H_

#include "RFSX.h"
#include "sxCEGUIEventSet.h"

// sxGlobalEventSet class definition
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
#define SX_IMPL_TYPE sxGlobalEventSet

namespace RFSX
{

class SX_IMPL_TYPE: public sxEventSet
{
public:
	SX_IMPL_TYPE();
	virtual ~SX_IMPL_TYPE();

	virtual int executableType() const { return RFSXU_GLOBALEVENTSET; }

	virtual CEGUI::EventSet* EventSet() { return CEGUI::GlobalEventSet::getSingletonPtr(); }
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_GLOBALEVENTSET_H_
