/************************************************************************************//**
 * @file CInputAction.h
 * @brief Class declaration for RFSX Input Action object
 * @author Daniel Queteschiner
 * @date January 2014
 *//*
 * Copyright (c) 2014 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_INPUT_ACTION_H_
#define _SX_INPUT_ACTION_H_

#include "RFSX\\RFSX.h"

// sxAction class definition
/*
	Description:
		(brief description of object goes here)

	Fields:
		int action_value;(RO)
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxAction

#define IS_ACTION(o)		CheckType<RFSX::sxAction>(o, RFSXU_ACTION)

namespace RFSX
{

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE();
	virtual ~SX_IMPL_TYPE();

	virtual bool getValue(const skString& name, const skString& att, skRValue& val);
	virtual int executableType() const { return RFSXU_ACTION; }

private:
	void InitActionHT();

	skTSHashTable<int> h_Action;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_INPUT_ACTION_H_
