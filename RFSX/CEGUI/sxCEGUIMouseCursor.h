/************************************************************************************//**
 * @file sxCEGUIMouseCursor.h
 * @brief Class declaration for RFSX MouseCursor object
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_MOUSECURSOR_H_
#define _SX_MOUSECURSOR_H_

#include "RFSX.h"
#include "sxCEGUIEventSet.h"
// sxMouseCursor class definition
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

		void setImage(sxImage image);
		void setImage(skString imageset, skString image);
		sxImage getImage(); -- may return null!
		void setPosition(sxVector2 pos);
		void offsetPosition(sxVector2 offset);
		void setConstraintArea(sxRect area);
		sxVector2 getPosition();
		sxRect getConstraintArea();
		sxVector2 getDisplayIndependantPosition();
		void setUnifiedConstraintArea(sxURect area);
		sxURect getUnifiedConstraintArea();
		void hide();
		void show();
	    void setVisible(bool visible);
		bool isVisible();

	for each: EventIterator
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxMouseCursor

// Type-check and upcast macro
#define IS_MOUSECURSOR(o) CheckType<sxMouseCursor>(o, RFSXU_MOUSECURSOR);

namespace RFSX
{

class SX_IMPL_TYPE: public sxEventSet
{
public:
	SX_IMPL_TYPE();
	virtual ~SX_IMPL_TYPE();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual int executableType() const { return RFSXU_MOUSECURSOR; }

	virtual CEGUI::EventSet* EventSet() { return CEGUI::MouseCursor::getSingletonPtr(); }
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_MOUSECURSOR_H_
