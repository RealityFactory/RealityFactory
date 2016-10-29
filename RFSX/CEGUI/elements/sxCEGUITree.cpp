/************************************************************************************//**
 * @file sxCEGUITree.cpp
 * @brief Implementation for RFSX Tree object.
 * @author Daniel Queteschiner
 * @date January 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "sxCEGUITree.h"
#include "sxCEGUITreeItem.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxTree

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxTree implementation
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return sxWindow::method(m_name, args, r_val, ctxt);
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxTree method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(getItemCount)
{
	r_val = static_cast<int>(caller->Tree()->getItemCount());
	return true;
}


SX_METHOD_IMPL(getSelectedCount)
{
	r_val = static_cast<int>(caller->Tree()->getSelectedCount());
	return true;
}


SX_METHOD_IMPL(getFirstSelectedItem)
{
	CEGUI::TreeItem *item = caller->Tree()->getFirstSelectedItem();

	if(item)
		r_val.assignObject(new sxTreeItem(item), true);
	else
		r_val.assignObject(&ctxt.getInterpreter()->getNull());

	return true;
}


SX_METHOD_IMPL(getLastSelectedItem)
{
	CEGUI::TreeItem *item = caller->Tree()->getLastSelectedItem();

	if(item)
		r_val.assignObject(new sxTreeItem(item), true);
	else
		r_val.assignObject(&ctxt.getInterpreter()->getNull());

	return true;
}


SX_METHOD_IMPL(isSortEnabled)
{
	r_val = caller->Tree()->isSortEnabled();
	return true;
}


SX_METHOD_IMPL(isMultiselectEnabled)
{
	r_val = caller->Tree()->isMultiselectEnabled();
	return true;
}


SX_METHOD_IMPL(isItemTooltipsEnabled)
{
	r_val = caller->Tree()->isItemTooltipsEnabled();
	return true;
}


SX_METHOD_IMPL(findFirstItemWithText)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CEGUI::String text(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));

	try
	{
		CEGUI::TreeItem *item = caller->Tree()->findFirstItemWithText(text);

		if(item)
			r_val.assignObject(new sxTreeItem(item), true);
		else
			r_val.assignObject(&ctxt.getInterpreter()->getNull());
	}
	catch(CEGUI::Exception CEGUIDeadException(&e))
	{
		return false;
	}
	catch(std::exception& e)
	{
		char errorBuffer[512];
		snprintf(errorBuffer, 512, "Exception of type 'std::exception' was thrown by function 'findFirstItemWithText'\nMessage: %s", e.what());
		CEGUI::Logger::getSingleton().logEvent(errorBuffer, CEGUI::Errors);
		return false;
	}
	catch(...)
	{
		CEGUI::Logger::getSingleton().logEvent("Unknown exception thrown by function 'findFirstItemWithText'", CEGUI::Errors);
		return false;
	}

	return true;
}


SX_METHOD_IMPL(findNextItemWithText)
{
	skRValue::RType rtypes[] = { skRValue::T_String, skRValue::T_Object };

	if(!CheckParams(1, 2, args, rtypes))
		return false;

	CEGUI::String text(reinterpret_cast<const CEGUI::utf8*>(args[0].str().c_str()));
	CEGUI::TreeItem *start_item = NULL;

	if(args.entries() == 2)
	{
		if(args[1].obj() != &ctxt.getInterpreter()->getNull())
		{
			sxTreeItem *item = IS_TREEITEM(args[1]);

			if(!item)
				return false;

			start_item = item->TreeItem();
		}
	}

	try
	{
		CEGUI::TreeItem *item = caller->Tree()->findNextItemWithText(text, start_item);

		if(item)
			r_val.assignObject(new sxTreeItem(item), true);
		else
			r_val.assignObject(&ctxt.getInterpreter()->getNull());
	}
	catch(CEGUI::Exception CEGUIDeadException(&e))
	{
		return false;
	}
	catch(std::exception& e)
	{
		char errorBuffer[512];
		snprintf(errorBuffer, 512, "Exception of type 'std::exception' was thrown by function 'findNextItemWithText'\nMessage: %s", e.what());
		CEGUI::Logger::getSingleton().logEvent(errorBuffer, CEGUI::Errors);
		return false;
	}
	catch(...)
	{
		CEGUI::Logger::getSingletonPtr()->logEvent("Unknown exception thrown by function 'findNextItemWithText'", CEGUI::Errors);
		return false;
	}

	return true;
}


SX_METHOD_IMPL(findFirstItemWithID)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Int)
		return false;

	try
	{
		CEGUI::TreeItem *item = caller->Tree()->findFirstItemWithID(args[0].intValue());

		if(item)
			r_val.assignObject(new sxTreeItem(item), true);
		else
			r_val.assignObject(&ctxt.getInterpreter()->getNull());
	}
	catch(CEGUI::Exception CEGUIDeadException(&e))
	{
		return false;
	}
	catch(std::exception& e)
	{
		char errorBuffer[512];
		snprintf(errorBuffer, 512, "Exception of type 'std::exception' was thrown by function 'findFirstItemWithID'\nMessage: %s", e.what());
		CEGUI::Logger::getSingleton().logEvent(errorBuffer, CEGUI::Errors);
		return false;
	}
	catch(...)
	{
		CEGUI::Logger::getSingleton().logEvent("Unknown exception thrown by function 'findFirstItemWithID'", CEGUI::Errors);
		return false;
	}

	return true;
}


SX_METHOD_IMPL(findNextItemWithID)
{
	skRValue::RType rtypes[] = { skRValue::T_Int, skRValue::T_Object };

	if(!CheckParams(1, 2, args, rtypes))
		return false;

	CEGUI::TreeItem *start_item = NULL;

	if(args.entries() == 2)
	{
		if(args[1].obj() != &ctxt.getInterpreter()->getNull())
		{
			sxTreeItem *item = IS_TREEITEM(args[1]);

			if(!item)
				return false;

			start_item = item->TreeItem();
		}
	}

	try
	{
		CEGUI::TreeItem *item = caller->Tree()->findNextItemWithID(args[0].intValue(), start_item);

		if(item)
			r_val.assignObject(new sxTreeItem(item), true);
		else
			r_val.assignObject(&ctxt.getInterpreter()->getNull());
	}
	catch(CEGUI::Exception CEGUIDeadException(&e))
	{
		return false;
	}
	catch(std::exception& e)
	{
		char errorBuffer[512];
		snprintf(errorBuffer, 512, "Exception of type 'std::exception' was thrown by function 'findNextItemWithID'\nMessage: %s", e.what());
		CEGUI::Logger::getSingleton().logEvent(errorBuffer, CEGUI::Errors);
		return false;
	}
	catch(...)
	{
		CEGUI::Logger::getSingletonPtr()->logEvent("Unknown exception thrown by function 'findNextItemWithID'", CEGUI::Errors);
		return false;
	}

	return true;
}


SX_METHOD_IMPL(isTreeItemInList)
{
	if(args.entries() != 1)
		return false;

	sxTreeItem* item = IS_TREEITEM(args[0]);
	if(!item)
		return false;

	r_val = caller->Tree()->isTreeItemInList(item->TreeItem());
	return true;
}


SX_METHOD_IMPL(resetList)
{
	caller->Tree()->resetList();
	return true;
}


SX_METHOD_IMPL(addItem)
{
	if(args.entries() != 1)
		return false;

	sxTreeItem* item = IS_TREEITEM(args[0]);
	if(!item)
		return false;

	caller->Tree()->addItem(item->TreeItem());
	return true;
}


SX_METHOD_IMPL(insertItem)
{
	if(args.entries() != 2)
		return false;

	sxTreeItem *item		= IS_TREEITEM(args[0]);
	sxTreeItem *position	= IS_TREEITEM(args[1]);
	if(!item || !position)
		return false;

	try
	{
		caller->Tree()->insertItem(item->TreeItem(), position->TreeItem());
	}
	catch(CEGUI::Exception CEGUIDeadException(&e))
	{
		return false;
	}
	catch(std::exception& e)
	{
		char errorBuffer[512];
		snprintf(errorBuffer, 512, "Exception of type 'std::exception' was thrown by function 'insertItem'\nMessage: %s", e.what());
		CEGUI::Logger::getSingleton().logEvent(errorBuffer, CEGUI::Errors);
		return false;
	}
	catch(...)
	{
		CEGUI::Logger::getSingletonPtr()->logEvent("Unknown exception thrown by function 'insertItem'", CEGUI::Errors);
		return false;
	}

	return true;
}


SX_METHOD_IMPL(removeItem)
{
	if(args.entries() != 1)
		return false;

	sxTreeItem* item = IS_TREEITEM(args[0]);
	if(!item)
		return false;

	caller->Tree()->removeItem(item->TreeItem());
	return true;
}


SX_METHOD_IMPL(clearAllSelections)
{
	caller->Tree()->clearAllSelections();
	return true;
}


SX_METHOD_IMPL(setSortingEnabled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	caller->Tree()->setSortingEnabled(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setMultiselectEnabled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	caller->Tree()->setMultiselectEnabled(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(setItemSelectState)
{
	if(args.entries() != 2)
		return false;

	if(args[1].type() != skRValue::T_Bool)
		return false;

	if(args[0].type() == skRValue::T_Int)
	{
		try
		{
			caller->Tree()->setItemSelectState(args[0].intValue(), args[1].boolValue());
		}
		catch(CEGUI::Exception CEGUIDeadException(&e))
		{
			return false;
		}
		catch(std::exception& e)
		{
			char errorBuffer[512];
			snprintf(errorBuffer, 512, "Exception of type 'std::exception' was thrown by function 'setItemSelectState'\nMessage: %s", e.what());
			CEGUI::Logger::getSingleton().logEvent(errorBuffer, CEGUI::Errors);
			return false;
		}
		catch(...)
		{
			CEGUI::Logger::getSingletonPtr()->logEvent("Unknown exception thrown by function 'setItemSelectState'", CEGUI::Errors);
			return false;
		}

		return true;
	}

	sxTreeItem* item = IS_TREEITEM(args[0]);
	if(!item)
		return false;

	caller->Tree()->setItemSelectState(item->TreeItem(), args[1].boolValue());
	return true;
}


SX_METHOD_IMPL(ensureItemIsVisible)
{
	if(args.entries() != 1)
		return false;

	sxTreeItem* item = IS_TREEITEM(args[0]);
	if(!item)
		return false;

	try
	{
		caller->Tree()->ensureItemIsVisible(item->TreeItem());
	}
	catch(CEGUI::Exception CEGUIDeadException(&e))
	{
		return false;
	}
	catch(std::exception& e)
	{
		char errorBuffer[512];
		snprintf(errorBuffer, 512, "Exception of type 'std::exception' was thrown by function 'ensureItemIsVisible'\nMessage: %s", e.what());
		CEGUI::Logger::getSingleton().logEvent(errorBuffer, CEGUI::Errors);
		return false;
	}
	catch(...)
	{
		CEGUI::Logger::getSingletonPtr()->logEvent("Unknown exception thrown by function 'ensureItemIsVisible'", CEGUI::Errors);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxTree initialization
//////////////////////////////////////////////////////////////////////////////////////////

SX_INIT_MHT(SXTREE_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(getItemCount);
	SX_ADD_METHOD(getSelectedCount);
	SX_ADD_METHOD(getFirstSelectedItem);
	SX_ADD_METHOD(getLastSelectedItem);
	SX_ADD_METHOD(isSortEnabled);
	SX_ADD_METHOD(isMultiselectEnabled);
	SX_ADD_METHOD(isItemTooltipsEnabled);
	SX_ADD_METHOD(findFirstItemWithText);
	SX_ADD_METHOD(findNextItemWithText);
	SX_ADD_METHOD(findFirstItemWithID);
	SX_ADD_METHOD(findNextItemWithID);
	SX_ADD_METHOD(isTreeItemInList);
	SX_ADD_METHOD(resetList);
	SX_ADD_METHOD(addItem);
	SX_ADD_METHOD(insertItem);
	SX_ADD_METHOD(removeItem);
	SX_ADD_METHOD(clearAllSelections);
	SX_ADD_METHOD(setSortingEnabled);
	SX_ADD_METHOD(setMultiselectEnabled);
	SX_ADD_METHOD(setItemSelectState);
	SX_ADD_METHOD(ensureItemIsVisible);
}

} // namespace RFSX

#undef SX_IMPL_TYPE // And that's it in a nutshell
