/************************************************************************************//**
 * @file CEGUISimkin.h
 * @brief Class declaration for Simkin Script Module for CEGUI
 * @author Daniel Queteschiner
 * @date November 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef _CEGUI_Simkin_h_
#define _CEGUI_Simkin_h_

#include <set>
#include "CEGUIScriptModule.h"

namespace CEGUI
{

class SimkinScriptModule : public CEGUI::ScriptModule
{
public:
	SimkinScriptModule();
	explicit SimkinScriptModule(skInterpreter* interpreter);

	~SimkinScriptModule();

	/**
	 * @brief Execute a script file.
	 * @param filename String object holding the filename of the script file that is to be executed
	 * @param resourceGroup	Resource group idendifier to be passed to the ResourceProvider when loading the script file.
	 */
	void executeScriptFile(const String& filename, const String& resourceGroup = "");
	void executeScriptFile(const String& filename, const String& resourceGroup, const String& function_name);

	/**
	 * @brief Execute a scripted global function. The function should not take any parameters and should return an integer.
	 * @param function_name	String object holding the name of the function, in the global script environment, that is to be executed.
	 * @return The integer value returned from the script function.
	 */
	int executeScriptGlobal(const String& function_name);

	/**
	 * @brief Execute a scripted global 'event handler' function. The function should take some kind of EventArgs like parameter
	 *      that the concrete implementation of this function can create from the passed EventArgs based object. The function
	 *      should not return anything.
	 * @param handler_name String object holding the name of the scripted handler function.
	 * @param e EventArgs based object that should be passed, by any appropriate means, to the scripted function.
	 * @return - true if the event was handled.
	 *         - false if the event was not handled.
	 */
	bool executeScriptedEventHandler(const String& handler_name, const EventArgs& e);

	/**
	 * @brief Execute script code contained in the given CEGUI::String object.
	 * @param str String object holding the valid script code that should be executed.
	 * @return Nothing.
	 */
	void executeString(const String& str);

	/**
	 * @brief Method called during system initialisation, prior to running any scripts via the ScriptModule, to enable the ScriptModule
	 *        to perform any operations required to complete initialisation or binding of the script language to the gui system objects.
	 * @return Nothing.
	 */
	void createBindings();

	/**
	 * @brief Method called during system destruction, after all scripts have been run via the ScriptModule, to enable the ScriptModule
	 *        to perform any operations required to cleanup bindings of the script language to the gui system objects, as set-up in the
	 *        earlier createBindings call.
	 * @return Nothing.
	 */
	void destroyBindings();

	/**
	 * @brief Subscribes the named Event to a scripted funtion
	 * @param target The target EventSet for the subscription.
	 * @param name String object containing the name of the Event to subscribe to.
	 * @param subscriber_name String object containing the name of the script funtion that is to be subscribed to the Event.
	 * @return Connection object that can be used to check the status of the Event connection and to disconnect (unsubscribe) from the Event.
	 * @exception UnknownObjectException Thrown if an Event named \a name is not in the EventSet
	 */
	Event::Connection subscribeEvent(EventSet* target, const String& name, const String& subscriber_name);

	/**
	 * @brief Subscribes the specified group of the named Event to a scripted funtion.
	 * @param target The target EventSet for the subscription.
	 * @param name String object containing the name of the Event to subscribe to.
	 * @param group Group which is to be subscribed to. Subscription groups are called in ascending order.
	 * @param subscriber_name String object containing the name of the script funtion that is to be subscribed to the Event.
	 * @return Connection object that can be used to check the status of the Event connection and to disconnect (unsubscribe) from the Event.
	 * @exception UnknownObjectException Thrown if an Event named \a name is not in the EventSet
	 */
	Event::Connection subscribeEvent(EventSet* target, const String& name, Event::Group group, const String& subscriber_name);

	void setInterpreter(skInterpreter* interpreter);

private:
	void setModuleIdentifierString();

private:
	skTreeNodeObject *d_pTreeNodeObject;
	skInterpreter *d_pInterpreter;
	bool d_ownsInterpreter;
	std::set<String, String::FastLessCompare> d_loadedScriptFiles;
};

}

#endif

