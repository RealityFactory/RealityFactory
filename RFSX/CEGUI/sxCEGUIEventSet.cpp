/************************************************************************************//**
 * @file sxCEGUIEventSet.cpp
 * @brief Implementation for RFSX EventSet object.
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUIEventSet.h"
#include "sxCEGUIEventArgs.h"
#include "sxCEGUIEventConnection.h"
#include "sxCEGUIIterators.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxEventSet

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxEventSet implementation
//////////////////////////////////////////////////////////////////////////////////////////

SX_IMPL_TYPE::SX_IMPL_TYPE(CEGUI::EventSet* eventSet)
	: m_EventSet(eventSet)
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

skExecutableIterator* SX_IMPL_TYPE::createIterator()
{
	return new sxEventIterator(this->EventSet());
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxEventSet method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(addEvent)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	caller->EventSet()->addEvent(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(removeEvent)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	caller->EventSet()->removeEvent(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(removeAllEvents)
{
	caller->EventSet()->removeAllEvents();
	return true;
}


SX_METHOD_IMPL(isEventPresent)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	r_val = caller->EventSet()->isEventPresent(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	return true;
}


SX_METHOD_IMPL(subscribeEvent)
{
	skRValue::RType rtypes[] = { skRValue::T_String, skRValue::T_String };

	if(!CheckParams(2, 2, args, rtypes))
		return false;

	CEGUI::String name(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	CEGUI::String callback_name(reinterpret_cast<const CEGUI::utf8*>(args[1].str().c_str()));

	try
	{
		r_val.assignObject(new sxEventConnection(caller->EventSet()->subscribeScriptedEvent(name, callback_name)), true);
	}
	catch(CEGUI::UnknownObjectException CEGUIDeadException(&e))
	{
		return false;
	}

	return true;
}


SX_METHOD_IMPL(fireEvent)
{
	skRValue::RType rtypes[] = {	skRValue::T_String,
									skRValue::T_Object,
									skRValue::T_String };

	if(!CheckParams(2, 3, args, rtypes))
		return false;

	CEGUI::String name(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	sxEventArgs *eargs = (sxEventArgs*)(args[1].obj());
	CEGUI::String eventnamespace;

	if(args.entries() == 3)
		eventnamespace = reinterpret_cast<const CEGUI::utf8*>(args[2].str().c_str());

	caller->EventSet()->fireEvent(name, eargs->EventArgsRef(), eventnamespace);
	return true;
}


SX_METHOD_IMPL(isMuted)
{
	r_val = caller->EventSet()->isMuted();
	return true;
}


SX_METHOD_IMPL(setMutedState)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	caller->EventSet()->setMutedState(args[0].boolValue());
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxEventSet initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXEVENTSET_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(addEvent);
	SX_ADD_METHOD(removeEvent);
	SX_ADD_METHOD(removeAllEvents);
	SX_ADD_METHOD(isEventPresent);
	SX_ADD_METHOD(subscribeEvent);
	SX_ADD_METHOD(fireEvent);
	SX_ADD_METHOD(isMuted);
	SX_ADD_METHOD(setMutedState);
}

} // namespace RFSX

#undef SX_IMPL_TYPE // And that's it in a nutshell
