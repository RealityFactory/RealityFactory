/************************************************************************************//**
 * @file sxCEGUILogger.h
 * @brief Class declaration for RFSX Template object
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _SX_LOGGER_H_
#define _SX_LOGGER_H_

#include "RFSX.h"

// sxLogger class definition
/*
	Description:
		(brief description of object goes here)

	Methods:
		void setLoggingLevel(int level);
		int getLoggingLevel();

		void logEvent(skString message, int level=GUI.Standard);
*/

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxLogger

#define SXLOGGER_METHODS 3

namespace RFSX
{

class SX_IMPL_TYPE: public skExecutable
{
public:
	SX_IMPL_TYPE();
	virtual ~SX_IMPL_TYPE();
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual int executableType() const { return RFSXU_LOGGER; }

protected:
	SX_METHOD_DECL(setLoggingLevel);
	SX_METHOD_DECL(getLoggingLevel);
	SX_METHOD_DECL(logEvent);

private:
	static sxMHT<SX_IMPL_TYPE> h_method;
};

} // namespace RFSX

#undef SX_IMPL_TYPE

#endif // _SX_LOGGER_H_
