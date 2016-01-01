/************************************************************************************//**
 * @file CLog.cpp
 * @brief Logging class
 *
 * This file contains the implementation of the Logging class.
 * @author Daniel Queteschiner
 * @date May 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "CLog.h"
#include <cstdio>
#include <cstdarg>
#include <windows.h> // OutputDebugString
#include "Simkin\\skInterpreter.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxLog

SX_IMPL_TYPE* SX_IMPL_TYPE::m_Log = NULL;

const char* SX_IMPL_TYPE::m_PriorityNames[] =
{
	"",
	"[debug]   ",
	"[info]    ",
	"[notice]  ",
	"[warning] ",
	"[error]   ",
	"[crit]    ",
	"[unknown] "
};


SX_IMPL_TYPE::SX_IMPL_TYPE() :
	m_Priority(LP_NOTICE),
	m_MBPriority(LP_ERROR)
{
}


SX_IMPL_TYPE::~SX_IMPL_TYPE()
{
	if(m_Stream.is_open())
		m_Stream.close();
}


SX_IMPL_TYPE* SX_IMPL_TYPE::GetSingletonPtr()
{
	if(m_Log == NULL)
	{
		m_Log = new SX_IMPL_TYPE();
	}

	return m_Log;
}


void SX_IMPL_TYPE::Destroy()
{
	delete m_Log;
	m_Log = NULL;
}


bool SX_IMPL_TYPE::SetFile(const std::string& filename, bool append)
{
	if(m_Stream.is_open())
		m_Stream.close();

	int mode = std::ios::out;
	if(append) mode |= std::ios::app;

	m_Stream.open(filename.c_str(), mode);

	if(m_Stream.is_open())
	{
		// we have an open stream now, see if we need to output any cached messages
		if(!m_Cache.empty())
		{
			std::vector<std::pair<std::string, int>>::iterator iter = m_Cache.begin();
			while(iter != m_Cache.end())
			{
				m_Stream << GetPriorityName(iter->second) << iter->first << std::endl;
				++iter;
			}
			m_Cache.clear();
		}

		return true;
	}

	OutputDebugString("Failed to open log file!\n");
	return false;
}


bool SX_IMPL_TYPE::IsOpen()
{
	return m_Stream.is_open();
}


void SX_IMPL_TYPE::Print(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	WriteLine(LP_NOTSET, vform(format, args));
	va_end(args);
}


void SX_IMPL_TYPE::Print(const std::string& message)
{
	WriteLine(LP_NOTSET, message.c_str());
}


void SX_IMPL_TYPE::Debug(const char* format, ...)
{
	if(m_Priority > LP_DEBUG) return;
	va_list args;
	va_start(args, format);
	WriteLine(LP_DEBUG, vform(format, args));
	va_end(args);
}


void SX_IMPL_TYPE::Debug(const std::string& message)
{
	if(m_Priority > LP_DEBUG) return;
	WriteLine(LP_DEBUG, message.c_str());
}


void SX_IMPL_TYPE::Info(const char *format, ...)
{
	if(m_Priority > LP_INFO) return;
	va_list args;
	va_start(args, format);
	WriteLine(LP_INFO, vform(format, args));
	va_end(args);
}


void SX_IMPL_TYPE::Info(const std::string& message)
{
	if(m_Priority > LP_INFO) return;
	WriteLine(LP_INFO, message.c_str());
}


void SX_IMPL_TYPE::Notice(const char* format, ...)
{
	if(m_Priority > LP_NOTICE) return;
	va_list args;
	va_start(args, format);
	WriteLine(LP_NOTICE, vform(format, args));
	va_end(args);
}


void SX_IMPL_TYPE::Notice(const std::string& message)
{
	if(m_Priority > LP_NOTICE) return;
	WriteLine(LP_NOTICE, message.c_str());
}


void SX_IMPL_TYPE::Warning(const char* format, ...)
{
	if(m_Priority > LP_WARNING) return;
	va_list args;
	va_start(args, format);
	WriteLine(LP_WARNING, vform(format, args));
	va_end(args);
}


void SX_IMPL_TYPE::Warning(const std::string& message)
{
	if(m_Priority > LP_WARNING) return;
	WriteLine(LP_WARNING, message.c_str());
}


void SX_IMPL_TYPE::Error(const char* format, ...)
{
	if(m_Priority > LP_ERROR) return;
	va_list args;
	va_start(args, format);
	WriteLine(LP_ERROR, vform(format, args));
	va_end(args);
}


void SX_IMPL_TYPE::Error(const std::string& message)
{
	if(m_Priority > LP_ERROR) return;
	WriteLine(LP_ERROR, message.c_str());
}


void SX_IMPL_TYPE::Critical(const char* format, ...)
{
	if(m_Priority > LP_CRITICAL) return;
	va_list args;
	va_start(args, format);
	WriteLine(LP_CRITICAL, vform(format, args));
	va_end(args);
}


void SX_IMPL_TYPE::Critical(const std::string& message)
{
	if(m_Priority > LP_CRITICAL) return;
	WriteLine(LP_CRITICAL, message.c_str());
}


void SX_IMPL_TYPE::Log(int priority, const char* format, ...)
{
	if(m_Priority > priority) return;
	va_list args;
	va_start(args, format);
	WriteLine(priority, vform(format, args));
	va_end(args);
}


void SX_IMPL_TYPE::Log(int priority, const std::string& message)
{
	if(m_Priority > priority) return;
	WriteLine(priority, message.c_str());
}


// private
const char* SX_IMPL_TYPE::vform(const char* format, va_list args)
{
	vsnprintf(m_Buffer, 1024, format, args);
	return m_Buffer;
}


void SX_IMPL_TYPE::WriteLine(int priority, const char* message)
{
	if(m_Stream.is_open())
	{
		m_Stream << GetPriorityName(priority) << message << std::endl;
	}
	else
	{
		m_Cache.push_back(std::make_pair(message, priority));
	}

	OutputDebugString(message);
	OutputDebugString("\n");

	if(m_MBPriority > priority) return;

	PopupMessageBox(priority, message);
}


void SX_IMPL_TYPE::PopupMessageBox(int priority, const char* message)
{
	UINT uType = MB_OK | MB_TOPMOST;

	switch(priority)
	{
	default:
	case LP_UNKNOWN:
	case LP_NOTSET:
	case LP_DEBUG:
	case LP_INFO:
		uType |= MB_ICONINFORMATION;
		break;
	case LP_NOTICE:
	case LP_WARNING:
		uType |= MB_ICONWARNING;
		break;
	case LP_ERROR:
	case LP_CRITICAL:
		uType |= MB_ICONERROR;
		break;
	}

	ShowCursor(TRUE);

	MessageBox(NULL, message, "RF Error Report", uType);
}


// Simkin
bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return skExecutable::method(m_name, args, r_val, ctxt);
}


bool SX_IMPL_TYPE::getValue(const skString& name, const skString& att, skRValue& val)
{
	if(name == "DEBUG")
	{
		val = LP_DEBUG;
		return true;
	}
	if(name == "INFO")
	{
		val = LP_INFO;
		return true;
	}
	if(name == "NOTICE")
	{
		val = LP_NOTICE;
		return true;
	}
	if(name == "WARNING")
	{
		val = LP_WARNING;
		return true;
	}
	if(name == "ERROR")
	{
		val = LP_ERROR;
		return true;
	}
	if(name == "CRITICAL")
	{
		val = LP_CRITICAL;
		return true;
	}

	return skExecutable::getValue(name, att, val);
}


//////////////////////////////////////////////////////////////////////////////////////////
// sxLog method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(SetFile)
{
	skRValue::RType rtypes[] = { skRValue::T_String, skRValue::T_Bool };

	if(!RFSX::CheckParams(1, 2, args, rtypes))
		return false;

	bool append = (args.entries() > 1) ? args[1].boolValue() : true;

	r_val = SX_IMPL_TYPE::GetSingletonPtr()->SetFile(args[0].str().c_str(), append);

	return true;
}


SX_METHOD_IMPL(IsOpen)
{
	r_val = SX_IMPL_TYPE::GetSingletonPtr()->IsOpen();
	return true;
}


SX_METHOD_IMPL(SetPriority)
{
	if(args.entries() != 1)
		return false;

	if(args[0].intValue() < LP_DEBUG || args[0].intValue() > LP_CRITICAL)
		return false;

	SX_IMPL_TYPE::GetSingletonPtr()->SetPriority(args[0].intValue());

	return true;
}


SX_METHOD_IMPL(SetMBPriority)
{
	if(args.entries() != 1)
		return false;

	if(args[0].intValue() < LP_DEBUG || args[0].intValue() > LP_CRITICAL)
		return false;

	SX_IMPL_TYPE::GetSingletonPtr()->SetMBPriority(args[0].intValue());

	return true;
}


SX_METHOD_IMPL(Print)
{
	if(args.entries() != 1)
		return false;

	SX_IMPL_TYPE::GetSingletonPtr()->Print(args[0].str().c_str());

	return true;
}


SX_METHOD_IMPL(Debug)
{
	if(args.entries() != 1)
		return false;

	SX_IMPL_TYPE::GetSingletonPtr()->Debug(args[0].str().c_str());

	return true;
}


SX_METHOD_IMPL(Info)
{
	if(args.entries() != 1)
		return false;

	SX_IMPL_TYPE::GetSingletonPtr()->Info(args[0].str().c_str());

	return true;
}


SX_METHOD_IMPL(Notice)
{
	if(args.entries() != 1)
		return false;

	SX_IMPL_TYPE::GetSingletonPtr()->Notice(args[0].str().c_str());

	return true;
}


SX_METHOD_IMPL(Warning)
{
	if(args.entries() != 1)
		return false;

	SX_IMPL_TYPE::GetSingletonPtr()->Warning(args[0].str().c_str());

	return true;
}


SX_METHOD_IMPL(Error)
{
	if(args.entries() != 1)
		return false;

	SX_IMPL_TYPE::GetSingletonPtr()->Error(args[0].str().c_str());

	return true;
}


SX_METHOD_IMPL(Critical)
{
	if(args.entries() != 1)
		return false;

	SX_IMPL_TYPE::GetSingletonPtr()->Critical(args[0].str().c_str());

	return true;
}


SX_METHOD_IMPL(Log)
{
	skRValue::RType rtypes[] = { skRValue::T_Int, skRValue::T_String };

	if(!RFSX::CheckParams(2, 2, args, rtypes))
		return false;

	if(args[0].intValue() < LP_DEBUG || args[0].intValue() > LP_CRITICAL)
		return false;

	SX_IMPL_TYPE::GetSingletonPtr()->Log(args[0].intValue(), args[1].str().c_str());

	return true;
}


//////////////////////////////////////////////////////////////////////////////////////////
// sxLog initialization
//////////////////////////////////////////////////////////////////////////////////////////

RFSX::SX_INIT_MHT(SXLOG_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	// Add all methods here
	SX_ADD_METHOD(SetFile);
	SX_ADD_METHOD(IsOpen);
	SX_ADD_METHOD(SetPriority);
	SX_ADD_METHOD(SetMBPriority);
	SX_ADD_METHOD(Print);
	SX_ADD_METHOD(Debug);
	SX_ADD_METHOD(Info);
	SX_ADD_METHOD(Notice);
	SX_ADD_METHOD(Warning);
	SX_ADD_METHOD(Error);
	SX_ADD_METHOD(Critical);
	SX_ADD_METHOD(Log);
}

#undef SX_IMPL_TYPE
