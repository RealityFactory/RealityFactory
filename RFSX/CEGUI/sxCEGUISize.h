/************************************************************************************//**
 * @file sxCEGUISize.h
 * @brief Class declaration for RFSX Size object
 * @author Daniel Queteschiner
 * @date November 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_SIZE_H_
#define _SX_SIZE_H_

#include "RFSX.h"

// sxSize class definition
/*
	Description:
		This class provides a wrapper for a CEGUI::Size object

	Operators:
		equals

	Fields:
		int width;(RW)
		int height;(RW)
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxSize

// Type-check and upcast macro
#define IS_SIZE(o)	CheckType<sxSize>(o, RFSXU_SIZE)

namespace RFSX
{

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE();
	SX_IMPL_TYPE(const CEGUI::Size& val);
	SX_IMPL_TYPE(float width, float height);
	virtual ~SX_IMPL_TYPE();

	virtual bool setValue(const skString& name, const skString& att, const skRValue& val);
	virtual bool getValue(const skString& name, const skString& att, skRValue& val);
	virtual bool equals(const skiExecutable *other_object) const;
	virtual int executableType() const { return RFSXU_SIZE; }

	CEGUI::Size* Size() { return &m_Size; }
	const CEGUI::Size& SizeConstRef() const { return m_Size; }

private:
	CEGUI::Size m_Size;

};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_SIZE_H_
