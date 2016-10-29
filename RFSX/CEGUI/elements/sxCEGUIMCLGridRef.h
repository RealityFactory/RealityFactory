/************************************************************************************//**
 * @file sxCEGUIMCLGridRef.h
 * @brief Class declaration for RFSX MCLGridRef object
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_MCLGRIDREF_H_
#define _SX_MCLGRIDREF_H_

#include "RFSX.h"

// sxMCLGridRef class definition
/*
	Description:
		Simple grid index structure.

	Operators:
		equals

	Fields:
		int row;(RW)	-- Zero based row index.
		int column;(RW)	-- Zero based column index.
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxMCLGridRef

// Type-check and upcast macro
#define IS_MCLGRIDREF(o)	CheckType<sxMCLGridRef>(o, RFSXU_MCLGRIDREF);

namespace RFSX
{

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE(unsigned int r, unsigned int c);
	SX_IMPL_TYPE(const CEGUI::MCLGridRef& gridRef);
	virtual ~SX_IMPL_TYPE();

	virtual bool setValue(const skString& name, const skString& att, const skRValue& val);
	virtual bool getValue(const skString& name, const skString& att, skRValue& val);
	virtual bool equals(const skiExecutable *other_object) const;
	virtual int executableType() const { return RFSXU_MCLGRIDREF; }

	CEGUI::MCLGridRef* MCLGridRef() { return &m_MCLGridRef; }
	const CEGUI::MCLGridRef& MCLGridRefConstRef() const { return m_MCLGridRef; }

private:
	CEGUI::MCLGridRef	m_MCLGridRef;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_MCLGRIDREF_H_
