/************************************************************************************//**
 * @file CScriptManager.h
 * @brief ScriptManager class
 *
 * This file contains the class declaration for the ScriptManager class.
 * @author Daniel Queteschiner
 * @date March 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_SCRIPTMANAGER_H_
#define __RGF_SCRIPTMANAGER_H_


#define CHECK_PARAMETERS

#ifdef CHECK_PARAMETERS
	#define PARMCHECK(x) ScriptManager::ParmCheck(arguments.entries(), x, Order, szName, methodName)
#else
	#define PARMCHECK(x)
#endif


/**
 * @brief ScriptManager class
 */
class ScriptManager
{
public:
	static void InitMHT();

	static skExecutableContext GetContext();

	static void AddGlobalVariable(const std::string& name, skRValue value);

	static void RemoveGlobalVariable(const std::string& name);

	static long GetHashMethod(const skString& name);

	static void ParmCheck(int entries, int desired, const std::string& order,
							const std::string& script, const skString& method);

public:
	static skInterpreter Interpreter;

private:
	static skTSHashTable<long> h_method;

private:
	ScriptManager() {}
	ScriptManager(const ScriptManager&) {}
	ScriptManager& operator=(const ScriptManager&) { return *this; }
	~ScriptManager() {}
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
