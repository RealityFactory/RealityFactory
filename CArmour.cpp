/************************************************************************************//**
 * @file CArmour.cpp
 * @author Ralph Deane
 * @brief CArmour class
 *//*
 * Copyright (c) Ralph Deane; All rights reserved
 ****************************************************************************************/

#include "RabidFramework.h"
#include "IniFile.h"
#include "CArmour.h"
#include "CHeadsUpDisplay.h"
#include "CInventory.h"

struct ArmourType
{
	int			Protection[MAXDAMAGEBY]; // percentage of protection
	int			DamageTo[MAXDAMAGEBY]; // percentage of damage taken
	std::string	DamageBy[MAXDAMAGEBY]; // effective against these damage dealers
	std::string	Attribute; // protected attribute
};


/* ------------------------------------------------------------------------------------ */
// Constructor
/* ------------------------------------------------------------------------------------ */
CArmour::CArmour()
{
	CIniFile iniFile("armour.ini");

	if(!iniFile.ReadFile())
		return;

	std::string armourName = iniFile.FindFirstKey();
	std::string type, value;
	geVec3d vector;

	while(!armourName.empty())
	{
		if(m_Armours.find(armourName) != m_Armours.end())
		{
			// already defined
			sxLog::GetSingletonPtr()->Warning("Armour type [" + armourName + "] already defined");
			armourName = iniFile.FindNextKey();
			continue;
		}

		if(iniFile.GetValue(armourName, "attribute").empty())
		{
			// no attribute specified
			sxLog::GetSingletonPtr()->Warning("No attribute defined for armour type [" + armourName + "]");
			continue;
		}

		m_Armours[armourName] = new ArmourType;
		memset(m_Armours[armourName]->Protection, 0, sizeof(int)*MAXDAMAGEBY);
		memset(m_Armours[armourName]->DamageTo,   0, sizeof(int)*MAXDAMAGEBY);

		type = iniFile.FindFirstName(armourName);
		value = iniFile.FindFirstValue();

		int j = 0;

		while(!type.empty())
		{
			if(j >= MAXDAMAGEBY)
			{
				sxLog::GetSingletonPtr()->Warning("Too many damage name entries defined for armour type [" + armourName + "]");
				break;
			}

			if(type == "attribute")
			{
				m_Armours[armourName]->Attribute = value;
			}
			else
			{
				m_Armours[armourName]->DamageBy[j] = type;

				vector = ToVec3d(value);
				m_Armours[armourName]->Protection[j] = static_cast<int>(vector.X);
				m_Armours[armourName]->DamageTo[j] = static_cast<int>(vector.Y);
				++j;
			}

			type = iniFile.FindNextName();
			value = iniFile.FindNextValue();
		}

		armourName = iniFile.FindNextKey();
	}
}


/* ------------------------------------------------------------------------------------ */
// Destructor
/* ------------------------------------------------------------------------------------ */
CArmour::~CArmour()
{
	stdext::hash_map<std::string, ArmourType*>::iterator it = m_Armours.begin();
	for(; it!=m_Armours.end(); ++it)
		delete it->second;
}


/* ------------------------------------------------------------------------------------ */
// IsArmour
/* ------------------------------------------------------------------------------------ */
bool CArmour::IsArmour(const std::string &attr) const
{
	return (m_Armours.find(attr) != m_Armours.end());
}


/* ------------------------------------------------------------------------------------ */
// DisableHud
/* ------------------------------------------------------------------------------------ */
void CArmour::DisableHud(const std::string& attr) const
{
	// if 'attr' is an armour, disable all armour
	// (the 'attr' armour will be enabled by the code calling this function afterwards)
	if(m_Armours.find(attr) != m_Armours.end())
	{
		stdext::hash_map<std::string, ArmourType*>::const_iterator it = m_Armours.begin();
		for(; it!=m_Armours.end(); ++it)
		{
			if(CCD->Player()->GetUseAttribute(it->first))
			{
				CCD->Player()->DelUseAttribute(it->first);
				CCD->HUD()->ActivateElement(it->first, false);
			}
		}
	}
}


/* ------------------------------------------------------------------------------------ */
// AdjustDamage
/* ------------------------------------------------------------------------------------ */
int CArmour::AdjustDamage(int amount, const std::string& name, const std::string& attr) const
{
	stdext::hash_map<std::string, ArmourType*>::const_iterator it = m_Armours.begin();
	for(; it!=m_Armours.end(); ++it)
	{
		if(it->second->Attribute != attr)
			continue;

		if(CCD->Player()->GetUseAttribute(it->first))
		{
			for(int j=0; j<MAXDAMAGEBY; ++j)
			{
				if(it->second->DamageBy[j] != name)
					continue;

				CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
				int protect = (amount * it->second->Protection[j]) / 100;

				if(theInv->Value(it->first) < protect)
					protect = theInv->Value(it->first);

				int actual = amount - protect;

				if(actual < 0)
					actual = 0;

				theInv->Modify(it->first, -(amount * it->second->DamageTo[j]) / 100);

				if(theInv->Value(it->first) <= theInv->Low(it->first))
				{
					CCD->Player()->DelUseAttribute(it->first);
					CCD->HUD()->ActivateElement(it->first, false);
				}

				sxInventory::GetSingletonPtr()->UpdateItem(it->first, true);

				return actual;
			}
		}
	}

	return amount;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
