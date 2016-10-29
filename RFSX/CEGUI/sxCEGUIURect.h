/************************************************************************************//**
 * @file sxCEGUIURect.h
 * @brief Class declaration for RFSX URect object
 * @author Daniel Queteschiner
 * @date November 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_URECT_H_
#define _SX_URECT_H_

#include "RFSX.h"

// sxURect class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		sxRect asAbsolute(sxSize base);
		sxRect asRelative(sxSize base);

		sxUVector2 getPosition();
		sxUVector2 getSize();
		sxUDim getWidth();
		sxUDim getHeight();

		void setPosition(sxUVector2 pos);
		void setSize(sxUVector2 sz);
		void setWidth(sxUDim w);
		void setHeight(sxUDim h);

		void offset(sxUVector2 sz);

	Fields:
		sxUVector2 min;(RW)
		sxUVector2 max;(RW)
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxURect
#define SXURECT_METHODS 11

// Type-check and upcast macro
#define IS_URECT(o)	 CheckType<sxURect>(o, RFSXU_URECT)

namespace RFSX
{

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE();
	SX_IMPL_TYPE(const CEGUI::URect& urect);
	SX_IMPL_TYPE(const CEGUI::UVector2& min, const CEGUI::UVector2& max);
	SX_IMPL_TYPE(const CEGUI::UDim& left, const CEGUI::UDim& top, const CEGUI::UDim& right, const CEGUI::UDim& bottom);
	virtual ~SX_IMPL_TYPE();
	static void InitMHT();

	virtual bool setValue(const skString& name, const skString& att, const skRValue& val);
	virtual bool getValue(const skString& name, const skString& att, skRValue& val);
	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual int executableType() const { return RFSXU_URECT; }

	CEGUI::URect* URect() { return &m_URect; }
	const CEGUI::URect& URectConstRef() const { return m_URect; }

protected:
	// Add methods here, by name.
	SX_METHOD_DECL(asAbsolute);
	SX_METHOD_DECL(asRelative);
	SX_METHOD_DECL(getPosition);
	SX_METHOD_DECL(getWidth);
	SX_METHOD_DECL(getHeight);
	SX_METHOD_DECL(getSize);
	SX_METHOD_DECL(setPosition);
	SX_METHOD_DECL(setWidth);
	SX_METHOD_DECL(setHeight);
	SX_METHOD_DECL(setSize);
	SX_METHOD_DECL(offset);

private:
	CEGUI::URect m_URect;

	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_URECT_H_
