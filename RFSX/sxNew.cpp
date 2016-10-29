/************************************************************************************//**
 * @file sxNew.cpp
 * @brief Implementation for RFSX object generator.
 * @author D.R. Gunter
 *//*
 * Copyright (c) 2008 D.R. Gunter; All rights reserved.
 ****************************************************************************************/

#include "sxNew.h"
#include "sxVector.h"
#include "CEGUI/sxCEGUIVector2.h"
#include "CEGUI/sxCEGUISize.h"
#include "CEGUI/sxCEGUIRect.h"
#include "CEGUI/sxCEGUIColour.h"
#include "CEGUI/sxCEGUIColourRect.h"
#include "CEGUI/sxCEGUIUVector2.h"
#include "CEGUI/sxCEGUIUDim.h"
#include "CEGUI/sxCEGUIURect.h"
#include "CEGUI/elements/sxCEGUIListboxTextItem.h"
#include "CEGUI/elements/sxCEGUIMCLGridRef.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxNew

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxNew implementation
//////////////////////////////////////////////////////////////////////////////////////////

SX_IMPL_TYPE::SX_IMPL_TYPE()
{
}


SX_IMPL_TYPE::~SX_IMPL_TYPE()
{
}


bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return skExecutable::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxNew method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(Camera)
{
	// TODO
	return true;
}


SX_METHOD_IMPL(Vector)
{
	skRValue::RType rtypes[] = {	skRValue::T_Float,
									skRValue::T_Float,
									skRValue::T_Float };

	if(!CheckParams(3, 3, args, rtypes))
		return false;

	r_val.assignObject(new sxVector(args[0].floatValue(), args[1].floatValue(), args[2].floatValue()), true);
	return true;
}


SX_METHOD_IMPL(Vector2)
{
	skRValue::RType rtypes[] = { skRValue::T_Float, skRValue::T_Float };

	if(!CheckParams(2, 2, args, rtypes))
		return false;

	r_val.assignObject(new sxVector2(args[0].floatValue(), args[1].floatValue()), true);
	return true;
}


SX_METHOD_IMPL(Point)
{
	skRValue::RType rtypes[] = { skRValue::T_Float, skRValue::T_Float };

	if(!CheckParams(2, 2, args, rtypes))
		return false;

	r_val.assignObject(new sxPoint(args[0].floatValue(), args[1].floatValue()), true);
	return true;
}


SX_METHOD_IMPL(Size)
{
	skRValue::RType rtypes[] = { skRValue::T_Float, skRValue::T_Float };

	if(!CheckParams(2, 2, args, rtypes))
		return false;

	r_val.assignObject(new sxSize(args[0].floatValue(), args[1].floatValue()), true);
	return true;
}


SX_METHOD_IMPL(Rect)
{
	skRValue::RType rtypes[] = {	skRValue::T_Float, skRValue::T_Float,
									skRValue::T_Float, skRValue::T_Float };

	if(!CheckParams(4, 4, args, rtypes))
		return false;

	r_val.assignObject(new sxRect(args[0].floatValue(), args[1].floatValue(), args[2].floatValue(), args[3].floatValue()), true);
	return true;
}


SX_METHOD_IMPL(Colour)
{
	if(args.entries() == 0)
	{
		r_val.assignObject(new sxColour(), true);
		return true;
	}

	skRValue::RType rtypes[] = {	skRValue::T_Float, skRValue::T_Float,
									skRValue::T_Float, skRValue::T_Float };

	if(!CheckParams(3, 4, args, rtypes))
		return false;

	if(args.entries() == 3)
		r_val.assignObject(new sxColour(args[0].floatValue(), args[1].floatValue(), args[2].floatValue()), true);
	else
		r_val.assignObject(new sxColour(args[0].floatValue(), args[1].floatValue(), args[2].floatValue(), args[3].floatValue()), true);

	return true;
}


SX_METHOD_IMPL(ColourRect)
{
	switch(args.entries())
	{
	case 0:
		{
			r_val.assignObject(new sxColourRect(), true);
			return true;
		}
	case 1:
		{
			sxColour *col = IS_COLOUR(args[0]);
			if(!col)
				return false;
			r_val.assignObject(new sxColourRect(col->ColourConstRef()), true);
			return true;
		}
	case 4:
		{
			sxColour *top_left		= IS_COLOUR(args[0]);
			sxColour *top_right		= IS_COLOUR(args[1]);
			sxColour *bottom_left	= IS_COLOUR(args[2]);
			sxColour *bottom_right	= IS_COLOUR(args[3]);
			if(!top_left || !top_right || !bottom_left || !bottom_right)
				return false;

			r_val.assignObject(new sxColourRect(top_left->ColourConstRef(),
												top_right->ColourConstRef(),
												bottom_left->ColourConstRef(),
												bottom_right->ColourConstRef()), true);
			return true;
		}
	}

	return false;
}


SX_METHOD_IMPL(UDim)
{
	skRValue::RType rtypes[] = { skRValue::T_Float, skRValue::T_Float };

	if(!CheckParams(2, 2, args, rtypes))
		return false;

	r_val.assignObject(new sxUDim(args[0].floatValue(), args[1].floatValue()), true);
	return true;
}


SX_METHOD_IMPL(UVector2)
{
	if(args.entries() != 2)
		return false;

	sxUDim *scale	= IS_UDIM(args[0]);
	sxUDim *offset	= IS_UDIM(args[1]);
	if(!scale || !offset)
		return false;

	r_val.assignObject(new sxUVector2(scale->UDimConstRef(), offset->UDimConstRef()), true);
	return true;
}


SX_METHOD_IMPL(URect)
{
	if(args.entries() == 2)
	{
		sxUVector2 *min = IS_UVECTOR2(args[0]);
		sxUVector2 *max = IS_UVECTOR2(args[1]);
		if(!min || !max)
			return false;

		r_val.assignObject(new sxURect(min->UVector2ConstRef(), max->UVector2ConstRef()), true);
		return true;
	}
	else if(args.entries() == 4)
	{
		sxUDim *left	= IS_UDIM(args[0]);
		sxUDim *top		= IS_UDIM(args[1]);
		sxUDim *right	= IS_UDIM(args[2]);
		sxUDim *bottom	= IS_UDIM(args[3]);
		if(!left || !top || !right || !bottom)
			return false;

		r_val.assignObject(new sxURect(	left->UDimConstRef(),
										top->UDimConstRef(),
										right->UDimConstRef(),
										bottom->UDimConstRef()), true);
		return true;
	}

	return false;

}


SX_METHOD_IMPL(ListboxTextItem)
{
	skRValue::RType rtypes[] = {	skRValue::T_String,
									skRValue::T_Int,
									skRValue::T_Bool };

	if(!CheckParams(1, 3, args, rtypes))
		return false;

	CEGUI::String text(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	unsigned int item_id = 0;
	bool disabled = false;

	if(args.entries() > 1)
	{
		item_id = args[1].intValue();

		if(args.entries() > 2)
		{
			disabled = args[2].boolValue();
		}
	}

	CEGUI::ListboxTextItem *item = new CEGUI::ListboxTextItem(text, item_id, 0, disabled);

	r_val.assignObject(new sxListboxTextItem(item), true);
	return true;
}


SX_METHOD_IMPL(MCLGridRef)
{
	skRValue::RType rtypes[] = { skRValue::T_Int, skRValue::T_Int };

	if(!CheckParams(2, 2, args, rtypes))
		return false;

	r_val.assignObject(new sxMCLGridRef(args[0].intValue(), args[1].intValue()), true);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxNew initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXNEW_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(Vector);

	SX_ADD_METHOD(Vector2);
	SX_ADD_METHOD(Point);
	SX_ADD_METHOD(Size);
	SX_ADD_METHOD(Rect);
	SX_ADD_METHOD(Colour);
	SX_ADD_METHOD(ColourRect);
	SX_ADD_METHOD(UDim);
	SX_ADD_METHOD(UVector2);
	SX_ADD_METHOD(URect);

	SX_ADD_METHOD(ListboxTextItem);
	SX_ADD_METHOD(MCLGridRef);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
