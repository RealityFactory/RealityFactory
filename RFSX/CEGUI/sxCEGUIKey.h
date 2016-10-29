/************************************************************************************//**
 * @file sxCEGUIKey.h
 * @brief Class declaration for RFSX Key object
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_KEY_H_
#define _SX_KEY_H_

#include "RFSX.h"

// sxKey class definition
/*
	Description:
		(brief description of object goes here)

	Fields:
		int key_value;(RO)
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxKey

#define IS_KEY(o)		CheckType<RFSX::sxKey>(o, RFSXU_KEY)

namespace RFSX
{

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE();
	virtual ~SX_IMPL_TYPE();

	virtual bool getValue(const skString& name, const skString& att, skRValue& val);
	virtual int executableType() const { return RFSXU_KEY; }

private:
	void InitKeyHT();

	skTSHashTable<int> h_Key;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_KEY_H_
