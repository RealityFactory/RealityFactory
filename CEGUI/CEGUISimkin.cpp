/************************************************************************************//**
 * @file CEGUISimkin.cpp
 * @brief Implementation of Simkin Script Module for CEGUI
 * @author Daniel Queteschiner
 * @date November 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include <CEGUISystem.h>
#include <CEGUILogger.h>
#include "Simkin\\skInterpreter.h"
#include "Simkin\\skTreeNode.h"
#include "Simkin\\skScriptedExecutable.h"
#include "Simkin\\skParseException.h"
#include "Simkin\\skRuntimeException.h"
#include "Simkin\\skRValueArray.h"
#include "CEGUISimkin.h"
#include "CEGUIGenesis3DResourceProvider.h"
#include "RFSX\\RFSX.h"
#include "RFSX\\CEGUI\\sxCEGUI.h"

namespace CEGUI
{

class SimkinFunctor
{
public:
	SimkinFunctor(const String& functionName) : scriptFunctionName(functionName) {}
	SimkinFunctor(const SimkinFunctor& obj) : scriptFunctionName(obj.scriptFunctionName) {}

	bool operator()(const EventArgs& e) const
	{
		ScriptModule* scriptModule = System::getSingleton().getScriptingModule();

		if(scriptModule)
		{
			return scriptModule->executeScriptedEventHandler(scriptFunctionName, e);
		}
		else
		{
			Logger::getSingleton().logEvent("Scripted event handler '" + scriptFunctionName + "' could not be called as no ScriptModule is available.", Errors);
			return false;
		}
	}
private:
	// no assignment possible
	SimkinFunctor& operator=(const SimkinFunctor& rhs);

	const String	scriptFunctionName;
};


SimkinScriptModule::SimkinScriptModule(void)
{
	d_pInterpreter = new skInterpreter();
	d_pTreeNodeObject = new skTreeNodeObject("ScriptModule", new skTreeNode, true);
	d_ownsInterpreter = true;
	setModuleIdentifierString();
}


SimkinScriptModule::SimkinScriptModule(skInterpreter* interpreter)
	: d_pInterpreter(interpreter), d_ownsInterpreter(false)
{
	d_pTreeNodeObject = new skTreeNodeObject("ScriptModule", new skTreeNode, true);
	setModuleIdentifierString();
}


SimkinScriptModule::~SimkinScriptModule(void)
{
	if(d_ownsInterpreter)
		delete d_pInterpreter;

	delete d_pTreeNodeObject;
}


void SimkinScriptModule::setInterpreter(skInterpreter* interpreter)
{
	if(d_ownsInterpreter)
		delete d_pInterpreter;

	d_pInterpreter = interpreter;
}


void SimkinScriptModule::executeScriptFile(const String& filename, const String& resourceGroup)
{
	executeScriptFile(filename, resourceGroup, "main");
}


void SimkinScriptModule::executeScriptFile(const String& filename, const String& resourceGroup, const String& function_name)
{
	skRValueArray args;
	skRValue ret;
	skExecutableContext ctxt(d_pInterpreter);

	String final_filename;
	final_filename = ((Genesis3DResourceProvider*)(CEGUI::System::getSingleton().getResourceProvider()))->getResourceGroupDirectory(resourceGroup.empty() ? d_defaultResourceGroup : resourceGroup);
	final_filename += filename;

	// check if this script file has already been loaded
	if(d_loadedScriptFiles.find(final_filename) != d_loadedScriptFiles.end())
		return;

	// load script file
	skScriptedExecutable script(final_filename.c_str(), ctxt);

	// try to execute function
	try
	{
		script.method(function_name.c_str(), args, ret, ctxt);
	}
	catch(skRuntimeException &e)
	{
		CEGUI::Logger::getSingleton().logEvent(e.toString().c_str(), Errors);
	}
	catch(skParseException &e)
	{
		CEGUI::Logger::getSingleton().logEvent(e.toString().c_str(), Errors);
	}
	catch(...)
	{
		String message =
			"Script error in file '"
			+ filename
			+ "' in method '"
			+ function_name
			+ "'";
		CEGUI::Logger::getSingleton().logEvent(message, Errors);
	}

	skTreeNode *main = script.getNode()->findChild(function_name.c_str());
	if(main) script.getNode()->removeChild(main);

	// copy node objects

	for(unsigned int i=0; i<script.getNode()->numChildren(); ++i)
		d_pTreeNodeObject->getNode()->addChild(new skTreeNode(*(script.getNode()->nthChild(i))));

	d_loadedScriptFiles.insert(final_filename);
}


int	SimkinScriptModule::executeScriptGlobal(const String& function_name)
{
	skRValueArray args;
	skRValue ret(0);
	skExecutableContext ctxt(d_pInterpreter);

	try
	{
		if(!d_pTreeNodeObject->method(function_name.c_str(), args, ret, ctxt))
			Logger::getSingleton().logEvent("Unable to evaluate the Simkin function: '" + function_name + "'", Warnings);
	}
	catch(skRuntimeException &e)
	{
		Logger::getSingleton().logEvent(e.toString().c_str(), Errors);
	}
	catch(skParseException &e)
	{
		Logger::getSingleton().logEvent(e.toString().c_str(), Errors);
	}
	catch(...)
	{
		String message =
			"Script error in method '"
			+ function_name
			+ "' of global object '"
			+ String(d_pTreeNodeObject->getLocation().c_str())
			+ "'";
		Logger::getSingleton().logEvent(message, Errors);
	}

	return ret.intValue();
}


bool SimkinScriptModule::executeScriptedEventHandler(const String& handler_name, const EventArgs& e)
{
	skRValueArray args;
	skRValue ret;
	skExecutableContext ctxt(d_pInterpreter);
	args.append(skRValue(new RFSX::sxEventArgs(const_cast<EventArgs*>(&e)), true));
	bool handled = false;

	// Executing scripted event handler
	try
	{
		handled = d_pTreeNodeObject->method(handler_name.c_str(), args, ret, ctxt);
	}
	catch(skRuntimeException &e)
	{
		Logger::getSingleton().logEvent(e.toString().c_str(), Errors);
	}
	catch(skParseException &e)
	{
		Logger::getSingleton().logEvent(e.toString().c_str(), Errors);
	}
	catch(...)
	{
		String message =
			"Script error in method '"
			+ handler_name
			+ "' of global object '"
			+ String(d_pTreeNodeObject->getLocation().c_str())
			+ "'";
		Logger::getSingleton().logEvent(message, Errors);
	}

	return handled;
}


void SimkinScriptModule::executeString(const String& str)
{
	skString location("CEGUI ScriptModule");
	skRValueArray args;
	skRValue ret;
	skExecutableContext ctxt(d_pInterpreter);
	skExecutable obj;

	try
	{
		d_pInterpreter->executeString(location, &obj, str.c_str(), args, ret, NULL, ctxt);
	}
	catch(skRuntimeException &e)
	{
		Logger::getSingleton().logEvent(e.toString().c_str(), Errors);
	}
	catch(skParseException &e)
	{
		Logger::getSingleton().logEvent(e.toString().c_str(), Errors);
	}
	catch(...)
	{
		String message = "Failed to execute string '" + str + "'";
		Logger::getSingleton().logEvent(message, Errors);
	}
}


void SimkinScriptModule::createBindings(void)
{
}


void SimkinScriptModule::destroyBindings(void)
{
}


/*************************************************************************
	Set the ID string for the module
*************************************************************************/
void SimkinScriptModule::setModuleIdentifierString()
{
	// set ID string
	d_identifierString = "CEGUI::SimkinScriptModule - Simkin based scripting module for CEGUI";
	d_language = "Simkin";
}


Event::Connection SimkinScriptModule::subscribeEvent(
	EventSet* target,
	const String& name,
	const String& subscriber_name)
{
	SimkinFunctor functor(subscriber_name);
	return target->subscribeEvent(name, Event::Subscriber(functor));
}


Event::Connection SimkinScriptModule::subscribeEvent(
	EventSet* target,
	const String& name,
	Event::Group group,
	const String& subscriber_name)
{
	SimkinFunctor functor(subscriber_name);
	return target->subscribeEvent(name, group, Event::Subscriber(functor));
}

}
