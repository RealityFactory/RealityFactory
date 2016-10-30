/************************************************************************************//**
 * @file CInventoryItem.cpp
 * @brief InventoryItem class
 *
 * This file contains the implementation of the InventoryItem class.
 * @author Daniel Queteschiner
 * @date March 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"
#include "IniFile.h"
#include "CHeadsUpDisplay.h"
#include "CGUIManager.h"
#include "CInventoryItem.h"
#include "CInventory.h"
#include "RFSX/CEGUI/sxCEGUIWindow.h"


class ItemCombination
{
public:
	ItemCombination(
		const std::string& result,
		int increaseResult = 1,
		int decrease = 1,
		int decreaseOther = 1) :
	m_Result(result),
	m_IncreaseResult(increaseResult),
	m_Decrease(decrease),
	m_DecreaseOther(decreaseOther)
	{}

	~ItemCombination() {}

	const std::string& Result() const { return m_Result; }
	int IncreaseResult() const { return m_IncreaseResult; }
	int Decrease() const { return m_Decrease; }
	int DecreaseOther() const { return m_DecreaseOther; }

private:
	std::string m_Result;
	int m_IncreaseResult;
	int m_Decrease;
	int m_DecreaseOther;
};


#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxInventoryItem


SX_IMPL_TYPE::SX_IMPL_TYPE(const std::string& attribute) :
	m_Attribute(attribute),
	m_Category("default"),
	m_ParentWindow(NULL),
	m_Window(NULL),
	m_TextWindow(NULL),
	m_ValueWindow(NULL),
	m_SelectDecrease(0)
{
#ifdef _DEBUG
	InitMHT();
#endif
}


SX_IMPL_TYPE::~SX_IMPL_TYPE()
{
	stdext::hash_map<std::string, ItemCombination*>::iterator iter = m_Combinations.begin();

	for(; iter!=m_Combinations.end(); ++iter)
	{
		SAFE_DELETE(iter->second);
	}

	m_Combinations.clear();

	if(m_Window)
		m_Window->setDestroyedByParent(true);
}


const std::string& SX_IMPL_TYPE::GetText() const
{
	return sxInventory::GetSingletonPtr()->GetText(m_Attribute);
}


const std::string& SX_IMPL_TYPE::GetTooltipText() const
{
	return sxInventory::GetSingletonPtr()->GetText(m_TooltipSection);
}


void SX_IMPL_TYPE::SelectForUse(int decrease)
{
	if(!CCD->MenuManager()->GetInGame())
		return;

	if(CCD->Player()->GetAttributes()->Value(m_Attribute) >=
		CCD->Player()->GetAttributes()->Low(m_Attribute) + decrease)
	{
		if(!CCD->Player()->GetUseAttribute(m_Attribute))
		{
			if(CCD->Player()->SetUseAttribute(m_Attribute))
			{
				CCD->HUD()->ActivateElement(m_Attribute, true);

				if(decrease)
				{
					CCD->Player()->GetAttributes()->Modify(m_Attribute, -decrease);
					m_SelectDecrease = decrease;
					Update(true);
				}
			}
		}
	}
}


void SX_IMPL_TYPE::DeselectFromUse()
{
	if(!CCD->MenuManager()->GetInGame())
		return;

	if(CCD->Player()->GetUseAttribute(m_Attribute))
	{
		CCD->HUD()->ActivateElement(m_Attribute, false);

		CCD->Player()->DelUseAttribute(m_Attribute);

		if(m_SelectDecrease)
		{
			CCD->Player()->GetAttributes()->Modify(m_Attribute, m_SelectDecrease);
			m_SelectDecrease = 0;
			Update(true);
		}
	}
}


void SX_IMPL_TYPE::Modify(const std::string& attribute, int amount, int decrease)
{
	if(!CCD->MenuManager()->GetInGame())
		return;

	if(CCD->Player()->GetAttributes()->Value(m_Attribute) >=
		CCD->Player()->GetAttributes()->Low(m_Attribute) + decrease )
	{
		if(CCD->Player()->GetAttributes()->Has(attribute))
		{
			if(attribute == "light")
			{
				if(CCD->Player()->GetAttributes()->Value("light") > 0)
				{
					CCD->Player()->GetAttributes()->Modify(m_Attribute, -decrease);
					CCD->Player()->GetAttributes()->Modify(attribute, amount);
					CCD->Player()->ModifyLight(CCD->Player()->GetAttributes()->GetModifyAmt("light"));

					sxInventory::GetSingletonPtr()->UpdateItem(attribute, true);
					Update(true);
				}
			}
			else
			{
				CCD->Player()->GetAttributes()->Modify(m_Attribute, -decrease);
				CCD->Player()->GetAttributes()->Modify(attribute, amount);

				sxInventory::GetSingletonPtr()->UpdateItem(attribute, true);
				Update(true);
			}
		}
	}
}


bool SX_IMPL_TYPE::HasCombination(const std::string& other)
{
	if(m_Combinations.find(other) != m_Combinations.end())
		return true;

	return false;
}


int SX_IMPL_TYPE::AddCombination(
	const std::string& other,
	const std::string& result,
	int increaseResult,
	int decrease,
	int decreaseOther)
{
	if(m_Combinations.find(other) != m_Combinations.end())
		return RGF_FAILURE; // already in list

	ItemCombination *combination = new ItemCombination(result, increaseResult, decrease, decreaseOther);
	m_Combinations[other] = combination;

	// also add this combination to the other item
	SX_IMPL_TYPE *otherItem = sxInventory::GetSingletonPtr()->GetItem(other);

	if(!otherItem->HasCombination(m_Attribute))
	{
		otherItem->AddCombination(m_Attribute, result, increaseResult, decreaseOther, decrease);
	}

	return RGF_SUCCESS;
}


int SX_IMPL_TYPE::RemoveCombination(const std::string& other)
{
	if(m_Combinations.find(other) != m_Combinations.end())
	{
		SAFE_DELETE(m_Combinations[other]);
		m_Combinations.erase(other);

		// also remove this combination from the other item
		SX_IMPL_TYPE *otherItem = sxInventory::GetSingletonPtr()->GetItem(other);

		if(otherItem->HasCombination(m_Attribute))
		{
			otherItem->RemoveCombination(m_Attribute);
		}
	}

	return RGF_SUCCESS;
}


int SX_IMPL_TYPE::Combine(const std::string& other)
{
	if(m_Combinations.find(other) == m_Combinations.end())
		return RGF_FAILURE;

	return Combine(
				other,
				m_Combinations[other]->Result(),
				m_Combinations[other]->IncreaseResult(),
				m_Combinations[other]->Decrease(),
				m_Combinations[other]->DecreaseOther());
}


int SX_IMPL_TYPE::Combine(
	const std::string& other,
	const std::string& result,
	int increaseResult,
	int decrease,
	int decreaseOther)
{
	if(!CCD->MenuManager()->GetInGame())
		return RGF_FAILURE;

	SX_IMPL_TYPE *otherItem = sxInventory::GetSingletonPtr()->GetItem(other);
	if(!otherItem)
		return RGF_FAILURE;

	if(CCD->Player()->GetAttributes()->Value(m_Attribute) <
		CCD->Player()->GetAttributes()->Low(m_Attribute) + decrease)
		return -1;

	if(CCD->Player()->GetAttributes()->Value(other) <
		CCD->Player()->GetAttributes()->Low(other) + decreaseOther)
		return -2;

	CCD->Player()->GetAttributes()->Modify(m_Attribute, -decrease);
	CCD->Player()->GetAttributes()->Modify(other, -decreaseOther);

	if(CCD->Player()->GetAttributes()->Has(result))
		CCD->Player()->GetAttributes()->Modify(result, increaseResult);
	else
		CCD->Player()->GetAttributes()->AddAndSet(result, increaseResult);

	Update(true);
	sxInventory::GetSingletonPtr()->UpdateItem(other, true);
	sxInventory::GetSingletonPtr()->UpdateItem(result, true);

	return RGF_SUCCESS;
}


void SX_IMPL_TYPE::Update(bool valueOnly)
{
	if(CCD->Player())
	{
		if(m_ValueWindow)
		{
			char value[32];
			sprintf(value, "%d", CCD->Player()->GetAttributes()->Value(m_Attribute));
			m_ValueWindow->setText((CEGUI::utf8*)value);
		}

		if(CCD->Player()->GetAttributes()->Value(m_Attribute) > 0)
		{
			if(m_Window->testClassName("ItemEntry"))
			{
				//sxLog::GetSingletonPtr()->Debug("->Show m_Window class: ItemEntry");

				CEGUI::ItemListBase *list = static_cast<CEGUI::ItemEntry*>(m_Window)->getOwnerList();
				if(list == NULL && m_ParentWindow != NULL)
				{
					static_cast<CEGUI::ItemListBase*>(m_ParentWindow)->addItem(static_cast<CEGUI::ItemEntry*>(m_Window));
					m_Window->show();
				}
			}
			else if(m_Window->testClassName("DragContainer"))
			{
				if(!m_Window->isVisible(true))
				{
					// search for empty slot
					if(!m_ParentWindowBaseName.empty())
					{
						int iSlotNumber = 0;
						char szSlotNumber[32];
						sprintf(szSlotNumber, "%d", iSlotNumber);

						std::string slot = m_ParentWindowBaseName + szSlotNumber;

						while(CCD->GUIManager()->IsWindowPresent(slot))
						{
							m_ParentWindow = CCD->GUIManager()->GetWindow(slot);

							// check if slot is available
							if(!m_ParentWindow->isVisible(true))
							{
								if(sxInventory::GetSingletonPtr()->GetSlotManagement()) //autoshow
								{
									if(m_ParentWindow->getParent())
									{
										m_ParentWindow->show();
									}
									else if(m_ParentWindow->isUserStringDefined("parent"))
									{
										CEGUI::Window *parent = CCD->GUIManager()->GetWindow(m_ParentWindow->getUserString("parent").c_str());
										if(parent)
										{
											parent->addChildWindow(m_ParentWindow);
											m_ParentWindow->show();
										}
									}
									else
									{
										break;
									}
								}
								else
								{
									break;
								}
							}

							// check if slot is occupied
							if(m_ParentWindow->getChildCount() == 0)
							{
								//sxLog::GetSingletonPtr()->Debug("--> Show m_Window class: DragContainer: added to slot " + slot);
								m_ParentWindow->addChildWindow(m_Window);
								m_Window->show();
								break;
							}

							++iSlotNumber;
							sprintf(szSlotNumber, "%d", iSlotNumber);
							slot = m_ParentWindowBaseName + szSlotNumber;
						}

						// TODO?
						/*
						if(!m_Window->isVisible())
						{
							// no empty slot found
							sxLog::GetSingletonPtr()->Debug("->Show m_Window class: DragContainer: no empty slot");
						}*/

					}
				}
			}
			else
			{
				m_Window->show();
			}
		}
		else
		{
			if(m_Window->testClassName("ItemEntry"))
			{
				CEGUI::ItemListBase *list = static_cast<CEGUI::ItemEntry*>(m_Window)->getOwnerList();
				if(list != NULL)
				{
					m_Window->hide();
					list->removeItem(static_cast<CEGUI::ItemEntry*>(m_Window));
				}
			}
			else if(m_Window->testClassName("DragContainer"))
			{
				if(m_Window->isVisible(true))
				{
					m_ParentWindow = m_Window->getParent();

					if(m_ParentWindow)
					{
						m_ParentWindow->removeChildWindow(m_Window);

						if(sxInventory::GetSingletonPtr()->GetSlotManagement()) // autohide
						{
							if(m_ParentWindow->getParent())
							{
								m_ParentWindow->setUserString("parent", m_ParentWindow->getParent()->getName());
								m_ParentWindow->getParent()->removeChildWindow(m_ParentWindow);
							}

							m_ParentWindow->hide();
						}
					}

					m_Window->hide();
				}
			}
			else
			{
				m_Window->hide();
			}
		}
	}

	if(valueOnly)
		return;

	if(m_TextWindow)
	{
		if(!GetText().empty())
		{
			m_TextWindow->setText((CEGUI::utf8*)GetText().c_str());
		}
	}

	if(!GetTooltipText().empty())
	{
		m_Window->setTooltipText((CEGUI::utf8*)GetTooltipText().c_str());
	}
}


int SX_IMPL_TYPE::LoadConfiguration(CIniFile& iniFile)
{
	if(m_Attribute.empty())
		return RGF_FAILURE;

	if(CCD->Player())
		if(!CCD->Player()->GetAttributes()->Has(m_Attribute))
			sxLog::GetSingletonPtr()->Debug("No corresponding player attribute found for inventory item [" + m_Attribute + "]");

	std::string value;

	value = iniFile.GetValue(m_Attribute, "category");
	if(!value.empty())
	{
		m_Category = value;
	}

	value = iniFile.GetValue(m_Attribute, "tooltip");
	if(!value.empty())
	{
		m_TooltipSection = value;
	}

	value = iniFile.GetValue(m_Attribute, "window");
	if(!value.empty())
	{
		m_Window = CCD->GUIManager()->GetWindow(value);

		if(!m_Window)
		{
			sxLog::GetSingletonPtr()->Warning("Failed to find window " + value + " for inventory item [" + m_Attribute + "]");
			return RGF_FAILURE;
		}

		m_Window->setDestroyedByParent(false);
	}
	else
	{
		sxLog::GetSingletonPtr()->Debug("No window specified for inventory item [" + m_Attribute + "]");
		return RGF_FAILURE;
	}

	value = iniFile.GetValue(m_Attribute, "textwindow");
	if(!value.empty())
	{
		m_TextWindow = CCD->GUIManager()->GetWindow(value);

		if(!m_TextWindow)
		{
			sxLog::GetSingletonPtr()->Warning("Failed to find textwindow " + value + " for inventory item [" + m_Attribute + "]");
		}
	}

	value = iniFile.GetValue(m_Attribute, "valuewindow");
	if(!value.empty())
	{
		m_ValueWindow = CCD->GUIManager()->GetWindow(value);

		if(!m_ValueWindow)
		{
			sxLog::GetSingletonPtr()->Warning("Failed to find valuewindow " + value + " for inventory item [" + m_Attribute + "]");
		}
	}

	value = iniFile.GetValue(m_Attribute, "parentwindow");
	if(!value.empty())
	{
		if(m_Window->testClassName("ItemEntry"))
		{
			m_ParentWindow = CCD->GUIManager()->GetWindow(value);

			if(!m_ParentWindow)
			{
				sxLog::GetSingletonPtr()->Warning("Failed to find parentwindow " + value + " for inventory item [" + m_Attribute + "]");

				m_ParentWindow = static_cast<CEGUI::ItemEntry*>(m_Window)->getOwnerList();

				if(!m_ParentWindow)
				{
					return RGF_FAILURE;
				}
			}
		}
		else if(m_Window->testClassName("DragContainer"))
		{
			if(!CCD->GUIManager()->IsWindowPresent(value + "0"))
			{
				sxLog::GetSingletonPtr()->Warning("Failed to find parentwindow " + value + " for inventory item [" + m_Attribute + "]");
			}
			else
			{
				m_ParentWindowBaseName = value;
				CEGUI::Window *parent = m_Window->getParent();

				if(parent)
				{
					parent->removeChildWindow(m_Window);
				}

				if(sxInventory::GetSingletonPtr()->GetSlotManagement()) // autohide
				{
					int iSlotNumber = 0;
					char szSlotNumber[32];
					sprintf(szSlotNumber, "%d", iSlotNumber);

					std::string slot = m_ParentWindowBaseName + szSlotNumber;

					while(CCD->GUIManager()->IsWindowPresent(slot))
					{
						CEGUI::Window *slotWindow = CCD->GUIManager()->GetWindow(slot);
						if(slotWindow->getParent())
						{
							slotWindow->setUserString("parent", slotWindow->getParent()->getName());
							slotWindow->getParent()->removeChildWindow(slotWindow);
						}
						slotWindow->hide();

						++iSlotNumber;
						sprintf(szSlotNumber, "%d", iSlotNumber);
						slot = m_ParentWindowBaseName + szSlotNumber;
					}
				}
			}
		}
	}

	m_Window->hide();

	return RGF_SUCCESS;
}


bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return skExecutable::method(m_name, args, r_val, ctxt);
}


SX_METHOD_IMPL(GetName)
{
	r_val = skString(caller->GetName().c_str());
	return true;
}


SX_METHOD_IMPL(SetCategory)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	caller->SetCategory(args[0].str().c_str());
	return true;
}


SX_METHOD_IMPL(GetCategory)
{
	r_val = skString(caller->GetCategory().c_str());
	return true;
}


SX_METHOD_IMPL(GetWindow)
{
	r_val.assignObject(new RFSX::sxWindow(caller->GetWindow()), true);
	return true;
}


SX_METHOD_IMPL(GetTextWindow)
{
	if(caller->GetTextWindow())
		r_val.assignObject(new RFSX::sxWindow(caller->GetTextWindow()), true);
	else
		r_val.assignObject(&ctxt.getInterpreter()->getNull());

	return true;
}


SX_METHOD_IMPL(GetValueWindow)
{
	if(caller->GetValueWindow())
		r_val.assignObject(new RFSX::sxWindow(caller->GetValueWindow()), true);
	else
		r_val.assignObject(&ctxt.getInterpreter()->getNull());
	return true;
}


SX_METHOD_IMPL(GetText)
{
	r_val = skString(caller->GetText().c_str());
	return true;
}


SX_METHOD_IMPL(SelectForUse)
{
	if(args.entries() > 1)
		return false;

	int decrease = 1;

	if(args.entries() == 1)
	{
		if(args[0].type() != skRValue::T_Int)
			return false;

		decrease = args[0].intValue();
	}

	caller->SelectForUse(decrease);
	return true;
}


SX_METHOD_IMPL(DeselectFromUse)
{
	caller->DeselectFromUse();
	return true;
}


SX_METHOD_IMPL(Modify)
{
	skRValue::RType rtypes[] = { skRValue::T_String, skRValue::T_Int, skRValue::T_Int };

	if(!RFSX::CheckParams(2, 3, args, rtypes))
		return false;

	int decrease = 1;

	if(args.entries() == 3)
	{
		decrease = args[2].intValue();
	}

	caller->Modify(args[0].str().c_str(), args[1].intValue(), decrease);
	return true;
}


SX_METHOD_IMPL(Combine)
{
	skRValue::RType rtypes[] = {	skRValue::T_String, skRValue::T_String,
									skRValue::T_Int, skRValue::T_Int, skRValue::T_Int };

	if(RFSX::CheckParams(1, 5, args, rtypes))
	{
		if(args.entries() > 1)
		{
			int increaseResult = 1;
			int decrease = 1;
			int decreaseOther = 1;

			if(args.entries() > 2)
			{
				increaseResult = args[2].intValue();

				if(args.entries() > 3)
				{
					decrease = args[3].intValue();

					if(args.entries() > 4)
					{
						decreaseOther = args[4].intValue();
					}
				}
			}

			r_val = caller->Combine(args[0].str().c_str(), args[1].str().c_str(), increaseResult, decrease, decreaseOther);
		}
		else
		{
			r_val = caller->Combine(args[0].str().c_str());
		}

		return true;
	}
	else if(args.entries() == 1)
	{
		sxInventoryItem *other = (sxInventoryItem*)IS_INVENTORYITEM(args[0]);
		if(!other)
			return false;

		r_val = caller->Combine(other->GetName());
		return true;
	}

	return false;
}


SX_METHOD_IMPL(AddCombination)
{
	skRValue::RType rtypes[] = {	skRValue::T_String, skRValue::T_String,
									skRValue::T_Int, skRValue::T_Int, skRValue::T_Int };

	if(!RFSX::CheckParams(2, 5, args, rtypes))
		return false;

	int increaseResult = 1;
	int decrease = 1;
	int decreaseOther = 1;

	if(args.entries() > 2)
	{
		increaseResult = args[2].intValue();

		if(args.entries() > 3)
		{
			decrease = args[3].intValue();

			if(args.entries() > 4)
			{
				decreaseOther = args[4].intValue();
			}
		}
	}

	r_val = caller->AddCombination(args[0].str().c_str(), args[1].str().c_str(), increaseResult, decrease, decreaseOther);

	return true;
}


SX_METHOD_IMPL(RemoveCombination)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	caller->RemoveCombination(args[0].str().c_str());
	return true;
}


RFSX::SX_INIT_MHT(SXINVENTORYITEM_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(GetName);
	SX_ADD_METHOD(SetCategory);
	SX_ADD_METHOD(GetCategory);
	SX_ADD_METHOD(GetWindow);
	SX_ADD_METHOD(GetTextWindow);
	SX_ADD_METHOD(GetValueWindow);
	SX_ADD_METHOD(GetText);
	SX_ADD_METHOD(SelectForUse);
	SX_ADD_METHOD(DeselectFromUse);
	SX_ADD_METHOD(Modify);
	SX_ADD_METHOD(Combine);
	SX_ADD_METHOD(AddCombination);
	SX_ADD_METHOD(RemoveCombination);
}

#undef SX_IMPL_TYPE

/* ----------------------------------- END OF FILE ------------------------------------ */
