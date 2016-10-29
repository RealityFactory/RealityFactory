/************************************************************************************//**
 * @file sxCEGUILogger.cpp
 * @brief Implementation for RFSX Logger object.
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUILogger.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxLogger

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxLogger implementation
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
// sxLogger method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(setLoggingLevel)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	int level = args[0].intValue();
	if(level < CEGUI::Errors || level > CEGUI::Insane)
		return false;

	CEGUI::Logger::getSingletonPtr()->setLoggingLevel(static_cast<CEGUI::LoggingLevel>(level));
	return true;
}


SX_METHOD_IMPL(getLoggingLevel)
{
	r_val = static_cast<int>(CEGUI::Logger::getSingletonPtr()->getLoggingLevel());
	return true;
}


SX_METHOD_IMPL(logEvent)
{
	skRValue::RType rtypes[] = { skRValue::T_String, skRValue::T_Int };

	if(!CheckParams(1, 2, args, rtypes))
		return false;

	CEGUI::String message(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	int level = CEGUI::Standard;

	if(args.entries() == 2)
	{
		level = args[1].intValue();
		if(level < CEGUI::Errors || level > CEGUI::Insane)
			return false;
	}

	CEGUI::Logger::getSingletonPtr()->logEvent(message, static_cast<CEGUI::LoggingLevel>(level));
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxLogger initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXLOGGER_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(setLoggingLevel);
	SX_ADD_METHOD(getLoggingLevel);
	SX_ADD_METHOD(logEvent);
}

} // namespace RFSX

#undef SX_IMPL_TYPE
