/************************************************************************************//**
 * @file sxDialogState.h
 * @brief Class declaration for RFSX DialogState object
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_DIALOGSTATE_H_
#define _SX_DIALOGSTATE_H_

#include "RFSX.h"

// sxDialogState class definition
/*
	This class provides a method to end the Dialog Game state (required for modal messages).

	Methods:
		void UpdateLevel(bool update);
		void RenderLevel(bool render);
		void Enter();
		void Exit();
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxDialogState

#define SXDIALOGSTATE_METHODS 4

namespace RFSX
{

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE();
	virtual ~SX_IMPL_TYPE();
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual int executableType() const { return RFSXG_DIALOGSTATE; }

protected:
	SX_METHOD_DECL(UpdateLevel);
	SX_METHOD_DECL(RenderLevel);
	SX_METHOD_DECL(Enter);
	SX_METHOD_DECL(Exit);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_DIALOGSTATE_H_
