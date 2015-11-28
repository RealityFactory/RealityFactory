/************************************************************************************//**
 * @file CArmour.cpp
 * @author Ralph Deane
 * @brief CArmour class
 *//*
 * Copyright (c) Ralph Deane; All rights reserved
 ****************************************************************************************/

#include "RabidFramework.h"

/* ------------------------------------------------------------------------------------ */
//	Constructor
/* ------------------------------------------------------------------------------------ */
CArmour::CArmour()
{
	ListPtr = -1;
	CIniFile AttrFile("armour.ini");

	if(!AttrFile.ReadFile())
		return;

	std::string KeyName = AttrFile.FindFirstKey();
	std::string Type, Value;
	geVec3d Vector;
	char szName[64];

	while(KeyName != "")
	{
		ListPtr +=1;

		if(ListPtr >= MAXARMOUR)
		{
			ListPtr = MAXARMOUR-1;
			break;
		}

		List[ListPtr].Name = strdup(KeyName.c_str());

		for(int i=0; i<MAXDAMAGEBY; i++)
			List[ListPtr].DamageBy[i] = NULL;

		List[ListPtr].Attribute = NULL;
		Type = AttrFile.FindFirstName(KeyName);
		Value = AttrFile.FindFirstValue();

		int j = 0;

		while(Type != "")
		{
			if(j >= MAXDAMAGEBY)
				break;

			if(Type == "attribute")
			{
				List[ListPtr].Attribute = strdup(Value.c_str());
			}
			else
			{
				List[ListPtr].DamageBy[j] = strdup(Type.c_str());
// changed QD 07/15/06
				//strcpy(szName, Value);
				strncpy(szName, Value.c_str(), 63);
				szName[63] = 0;
// end change QD 07/15/06
				Vector = Extract(szName);
				List[ListPtr].Protection[j] = (int)Vector.X;
				List[ListPtr].DamageTo[j] = (int)Vector.Y;
				j += 1;
			}

			Type = AttrFile.FindNextName();
			Value = AttrFile.FindNextValue();
		}

		KeyName = AttrFile.FindNextKey();
	}
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
/* ------------------------------------------------------------------------------------ */
CArmour::~CArmour()
{
	if(ListPtr != -1)
	{
		for(int i=0; i<=ListPtr; i++)
		{
			if(List[i].Name)
			{
				free(List[i].Name);
				List[i].Name = 0;
			}

			if(List[i].Attribute)
			{
				free(List[i].Attribute);
				List[i].Attribute = 0;
			}

			for(int j=0; j<MAXDAMAGEBY; j++)
			{
				if(List[i].DamageBy[j])
				{
					free(List[i].DamageBy[j]);
					List[i].DamageBy[j] = 0;
				}
			}
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	DisableHud
/* ------------------------------------------------------------------------------------ */
void CArmour::DisableHud(const char *Attr)
{
	int i;

	if(ListPtr != -1)
	{
		for(i=0; i<=ListPtr; i++)
		{
			if(!strcmp(Attr, List[i].Name))
			{
				for(int j=0; j<=ListPtr; j++)
				{
					if(CCD->Player()->GetUseAttribute(List[j].Name))
					{
						CCD->Player()->DelUseAttribute(List[j].Name);
						CCD->HUD()->ActivateElement(List[j].Name, false);
					}
				}

				break;
			}
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	AdjustDamage
/* ------------------------------------------------------------------------------------ */
int CArmour::AdjustDamage(int Amount, const char *name, const char *Attr)
{
	if(ListPtr == -1)
		return Amount;

	for(int i=0; i<=ListPtr; i++)
	{
		if(CCD->Player()->GetUseAttribute(List[i].Name) && List[i].Attribute)
		{
			if(!strcmp(Attr, List[i].Attribute))
			{
				for(int j=0; j<MAXDAMAGEBY; j++)
				{
					if(List[i].DamageBy[j])
					{
						if(!strcmp(name, List[i].DamageBy[j]))
						{
							int Actual;
							CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
							int Protect = ((Amount*List[i].Protection[j])/100);

							if(theInv->Value(List[i].Name)<Protect)
								Protect = theInv->Value(List[i].Name);

							Actual = Amount - Protect;

							if(Actual < 0)
								Actual = 0;

							theInv->Modify(List[i].Name, -((Amount*List[i].DamageTo[j])/100));

							if(theInv->Value(List[i].Name) <= theInv->Low(List[i].Name))
							{
								CCD->Player()->DelUseAttribute(List[i].Name);
								CCD->HUD()->ActivateElement(List[i].Name, false);
							}

							return Actual;
						}
					}
				}
			}
		}
	}

	return Amount;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
