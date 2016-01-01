/************************************************************************************//**
 * @file sxNew.h
 * @brief Class declaration for RFSX object generator.
 * @author D.R. Gunter
 *//*
 * Copyright (c) 2008 D.R. Gunter; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_NEW_H_
#define _SX_NEW_H_

#include "RFSX.h"

// sxNew class definition
/*
	This class provides methods to create typed objects in a script at runtime.

	Think of it as a c++ "new" command for simkin scripts.
	The objects returned will be properly destroyed when the scripted executable is
	destroyed, or when they go out of scope in the script.

	Methods:
		sxVector Vector(float x, float y, float z);

	Example of possible application:
		{
			VEC = New.Vector(x, y, z);
			SetPositon(VEC);
		}
		or, simply:
		{
			SetPosition(New.Vector(x, y, z));
		}
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxNew

#define SXNEW_METHODS 1

namespace RFSX
{

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE();
	virtual ~SX_IMPL_TYPE();
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual int executableType() const { return RFSXG_NEW; }

protected:
	SX_METHOD_DECL(Vector);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _RFSXNEW_H_
