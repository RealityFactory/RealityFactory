/************************************************************************************//**
 * @file CLog.h
 * @brief Logging class
 *
 * This file contains the class declaration for the Logging class.
 * @author Daniel Queteschiner
 * @date May 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_LOG_H_
#define __RGF_LOG_H_

#include <fstream>
#include "RFSX\\RFSX.h"

typedef enum
{
	LP_NOTSET = 0,
	LP_DEBUG,
	LP_INFO,
	LP_NOTICE,
	LP_WARNING,
	LP_ERROR,
	LP_CRITICAL,
	LP_UNKNOWN
} LOGPRIORITY;


#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxLog

#define SXLOG_METHODS 12

/**
 * @brief Logging class
 */
class SX_IMPL_TYPE : public skExecutable
{
public:
	~SX_IMPL_TYPE();

	static void InitMHT();

	virtual bool getValue(const skString& name, const skString& att, skRValue& val);
	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual int executableType() const { return RFSXG_LOG; }

	/**
	 * @brief Sets the specified file as log file.
	 * @param filename Name of the file to put log messages
	 */
	bool SetFile(const std::string& filename, bool append = true);

	/**
	 * @brief Check if log file is open.
	 */
	bool IsOpen();

	/**
	 * @brief Set lowest priority that will get logged.
	 */
	void SetPriority(int priority) { m_Priority = priority; }

	/**
	 * @brief Set lowest priority that will get message boxed.
	 * If priority is larger then no message box will be displayed.
	 */
	void SetMBPriority(int priority) { m_MBPriority = priority; }

	/**
	 * @brief Log a message without priority checking.
	 * @param format Format specifier for the string to write to the log file.
	 * @param ... The arguments for format
	 */
	void Print(const char* format, ...);

	/**
	 * @brief Log a message without priority checking.
	 * @param message The string to write to the log file.
	 */
	void Print(const std::string& message);

	/**
	 * @brief Log a message with debug priority.
	 * @param format Format specifier for the string to write to the log file.
	 * @param ... The arguments for format
	 */
	void Debug(const char* format, ...);

	/**
	 * @brief Log a message with debug priority.
	 * @param message The string to write to the log file.
	 */
	void Debug(const std::string& message);

	/**
	 * @brief Log a message with info priority.
	 * @param format Format specifier for the string to write to the log file.
	 * @param ... The arguments for format
	 */
	void Info(const char* format, ...);

	/**
	 * @brief Log a message with info priority.
	 * @param message The string to write to the log file.
	 */
	void Info(const std::string& message);

	/**
	 * @brief Log a message with notice priority.
	 * @param format Format specifier for the string to write to the log file.
	 * @param ... The arguments for format
	 */
	void Notice(const char* format, ...);

	/**
	 * @brief Log a message with notice priority.
	 * @param message The string to write to the log file.
	 */
	void Notice(const std::string& message);

	/**
	 * @brief Log a message with warning priority.
	 * @param format Format specifier for the string to write to the log file.
	 * @param ... The arguments for format
	 */
	void Warning(const char* format, ...);

	/**
	 * @brief Log a message with warning priority.
	 * @param message The string to write to the log file.
	 */
	void Warning(const std::string& message);

	/**
	 * @brief Log a message with error priority.
	 * @param format Format specifier for the string to write to the log file.
	 * @param ... The arguments for format
	 */
	void Error(const char* format, ...);

	/**
	 * @brief Log a message with error priority.
	 * @param message The string to write to the log file.
	 */
	void Error(const std::string& message);

	/**
	 * @brief Log a message with critical priority.
	 * @param format Format specifier for the string to write to the log file.
	 * @param ... The arguments for format
	 */
	void Critical(const char* format, ...);

	/**
	 * @brief Log a message with critical priority.
	 * @param message The string to write to the log file.
	 */
	void Critical(const std::string& message);

	/**
	 * @brief Log a message with the specified priority.
	 * @param priority The priority of the log message.
	 * @param format Format specifier for the string to write to the log file.
	 * @param ... The arguments for format
	 **/
	void Log(int priority, const char* format, ...);

	/**
	 * @brief Log a message with the specified priority.
	 * @param priority The priority of this log message.
	 * @param message The string to write to the log file.
	 */
	void Log(int priority, const std::string& message);

	/**
	 * @brief Get a human readable string for a given priority.
	 * @param priority The priority whose name is asked for.
	 * @return A human readable string for the specified priority.
	 */
	static const char* GetPriorityName(int priority)
	{ return m_PriorityNames[(priority>LP_UNKNOWN || priority<0)?LP_UNKNOWN:priority]; }

	static SX_IMPL_TYPE* GetSingletonPtr();
	static void Destroy();

protected:
	SX_METHOD_DECL(SetFile);
	SX_METHOD_DECL(IsOpen);
	SX_METHOD_DECL(SetPriority);
	SX_METHOD_DECL(SetMBPriority);
	SX_METHOD_DECL(Print);
	SX_METHOD_DECL(Debug);
	SX_METHOD_DECL(Info);
	SX_METHOD_DECL(Notice);
	SX_METHOD_DECL(Warning);
	SX_METHOD_DECL(Error);
	SX_METHOD_DECL(Critical);
	SX_METHOD_DECL(Log);

private:
	SX_IMPL_TYPE();
	// The Log cannot be copied, so the copy constructor is private
	SX_IMPL_TYPE(const SX_IMPL_TYPE&) { }
	// The Log cannot be copied, so the assignment operator is private
	SX_IMPL_TYPE& operator=(const SX_IMPL_TYPE&);

	const char* vform(const char* format, va_list args);
	void WriteLine(int priority, const char* message);
	void PopupMessageBox(int priority, const char* message);

private:
	std::ofstream	m_Stream;
	int				m_Priority;
	int				m_MBPriority;
	char			m_Buffer[1024];
	std::vector<std::pair<std::string, int> > m_Cache;    //!< Used to cache log entries before log file is created.

private:
	static RFSX::sxMHT<SX_IMPL_TYPE> h_method;

	static const char*		m_PriorityNames[];
	static SX_IMPL_TYPE*	m_Log;
};

#undef SX_IMPL_TYPE

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
