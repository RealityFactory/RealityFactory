/************************************************************************************//**
 * @file sxCEGUIMouseCursor.cpp
 * @brief Implementation for RFSX MouseCursor object.
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIMouseCursor.h"
#include "sxCEGUIImage.h"
#include "sxCEGUIVector2.h"
#include "sxCEGUIRect.h"
#include "sxCEGUIURect.h"
#include "../CGameStateManager.h"
#include "../CPlayState.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxMouseCursor

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxMouseCursor implementation
//////////////////////////////////////////////////////////////////////////////////////////

SX_IMPL_TYPE::SX_IMPL_TYPE()
{
}


SX_IMPL_TYPE::~SX_IMPL_TYPE()
{
}

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{

	if(m_name == "setImage")
	{
		if(args.entries() == 1)
		{
			sxImage *image = IS_IMAGE(args[0]);
			if(!image)
				return false;

			//void setImage(const Image* image);
			CEGUI::MouseCursor::getSingletonPtr()->setImage(image->Image());
			return true;
		}

		skRValue::RType rtypes[] = { skRValue::T_String, skRValue::T_String };

		if(!CheckParams(2, 2, args, rtypes))
			return false;

		CEGUI::MouseCursor::getSingletonPtr()->setImage(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()),
														reinterpret_cast<const CEGUI::utf8*>(args[1].str().c_str()));
		return true;
	}
	if(m_name == "getImage")
	{
		const CEGUI::Image* image = CEGUI::MouseCursor::getSingletonPtr()->getImage();
		if(image)
			r_val.assignObject(new sxImage(image), true);
		else
			r_val.assignObject(&ctxt.getInterpreter()->getNull());
		return true;
	}
	if(m_name == "setPosition")
	{
		if(args.entries() != 1)
			return false;

		sxVector2 *vec = IS_VECTOR2(args[0]);
		if(!vec)
			return false;

		CEGUI::MouseCursor::getSingletonPtr()->setPosition(vec->Vector2ConstRef());
		return true;
	}
	if(m_name == "getPosition")
	{
		r_val.assignObject(new sxVector2(CEGUI::MouseCursor::getSingletonPtr()->getPosition()), true);
		return true;
	}
	if(m_name == "offsetPosition")
	{
		if(args.entries() != 1)
			return false;

		sxVector2 *vec = IS_VECTOR2(args[0]);
		if(!vec)
			return false;

		CEGUI::MouseCursor::getSingletonPtr()->offsetPosition(vec->Vector2ConstRef());
		return true;
	}
	if(m_name == "setConstraintArea")
	{
		if(args.entries() != 1)
			return false;

		if(args[0].obj() == &ctxt.getInterpreter()->getNull())
		{
			CEGUI::MouseCursor::getSingletonPtr()->setConstraintArea(NULL);
			return true;
		}

		sxRect *rect = IS_RECT(args[0]);
		if(!rect)
			return false;

		CEGUI::MouseCursor::getSingletonPtr()->setConstraintArea(rect->Rect());
		return true;
	}
	if(m_name == "getConstraintArea")
	{
		r_val.assignObject(new sxRect(CEGUI::MouseCursor::getSingletonPtr()->getConstraintArea()), true);
		return true;
	}
	if(m_name == "getDisplayIndependantPosition")
	{
		r_val.assignObject(new sxVector2(CEGUI::MouseCursor::getSingletonPtr()->getDisplayIndependantPosition()), true);
		return true;
	}
	if(m_name == "setUnifiedConstraintArea")
	{
		if(args.entries() != 1)
			return false;

		if(args[0].obj() == &ctxt.getInterpreter()->getNull())
		{
			CEGUI::MouseCursor::getSingletonPtr()->setUnifiedConstraintArea(NULL);
			return true;
		}

		sxURect *urect = IS_URECT(args[0]);
		if(!urect)
			return false;

		CEGUI::MouseCursor::getSingletonPtr()->setUnifiedConstraintArea(urect->URect());
		return true;
	}
	if(m_name == "getUnifiedConstraintArea")
	{
		r_val.assignObject(new sxURect(CEGUI::MouseCursor::getSingletonPtr()->getUnifiedConstraintArea()), true);
		return true;
	}
	if(m_name == "hide")
	{
		CEGUI::MouseCursor::getSingletonPtr()->hide();
		if(CGameStateManager::GetSingletonPtr()->GetState() == CPlayState::GetSingletonPtr())
			CPlayState::GetSingletonPtr()->ShowCursor(false);
		return true;
	}
	if(m_name == "show")
	{
		CEGUI::MouseCursor::getSingletonPtr()->show();
		if(CGameStateManager::GetSingletonPtr()->GetState() == CPlayState::GetSingletonPtr())
			CPlayState::GetSingletonPtr()->ShowCursor(true);
		return true;
	}
	if(m_name == "setVisible")
	{
		if(args.entries() != 1)
			return false;

		if(args[0].type() != skRValue::T_Bool)
			return false;

		CEGUI::MouseCursor::getSingletonPtr()->setVisible(args[0].boolValue());

		if(CGameStateManager::GetSingletonPtr()->GetState() == CPlayState::GetSingletonPtr())
			CPlayState::GetSingletonPtr()->ShowCursor(args[0].boolValue());
		return true;
	}
	if(m_name == "isVisible")
	{
		r_val = CEGUI::MouseCursor::getSingletonPtr()->isVisible();
		return true;
	}

	return sxEventSet::method(m_name, args, r_val, ctxt);
}

} // namespace RFSX

#undef SX_IMPL_TYPE // And that's it in a nutshell
