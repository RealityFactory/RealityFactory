/************************************************************************************//**
 * @file CActMaterial.cpp
 * @brief CActMaterial handler
 *
 * This file contains the class implementation for the CActMaterial enitity
 * for RGF-based games.
 * @author Daniel Queteschiner
 *//*
 * Copyright (c) 2002 Ralph Deane; All rights reserved
 ****************************************************************************************/

// Include the One True Header
#include "RabidFramework.h"

/* ------------------------------------------------------------------------------------ */
//	Constructor
/* ------------------------------------------------------------------------------------ */
CActMaterial::CActMaterial()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	// Ok, check to see if there are ActMaterial entities in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "ActMaterial");

	if(!pSet)
		return;

	// Ok, we have ActMaterials somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		ActMaterial *pMaterial = (ActMaterial*)geEntity_GetUserData(pEntity);

		if(EffectC_IsStringNull(pMaterial->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pMaterial->szEntityName = szName;
		}

		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(pMaterial->szEntityName, "ActMaterial");

		pMaterial->active = GE_FALSE;
		pMaterial->Time = 0.0f;
		pMaterial->CurMat = 0;
		pMaterial->CycleDir = 1;
		pMaterial->Actor = NULL;

		if(!EffectC_IsStringNull(pMaterial->EntityName))
			pMaterial->Actor = GetEntityActor(pMaterial->EntityName);

		if(pMaterial->Actor == NULL)
		{
			char szError[256];
			sprintf(szError,"[WARNING] File %s - Line %d: %s: Missing actor '%s'\n",
					__FILE__, __LINE__, pMaterial->szEntityName, pMaterial->EntityName);
			CCD->ReportError(szError, false);
			// changed QD 07/15/06 - make missing actor not a fatal error
			/*
			CCD->ShutdownLevel();
			delete CCD;
			CCD = NULL;
			MessageBox(NULL, szError, "ActMaterial", MB_OK);
			exit(-333);
			*/
			// end change
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
/* ------------------------------------------------------------------------------------ */
CActMaterial::~CActMaterial()
{
}

/* ------------------------------------------------------------------------------------ */
//	Tick
/* ------------------------------------------------------------------------------------ */
void CActMaterial::Tick(geFloat dwTicks)
{

	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	pSet = geWorld_GetEntitySet(CCD->World(), "ActMaterial");

	if(!pSet)
		return;         //	Not on this level.

	// Ok, we have ActorMaterial entities somewhere. Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		ActMaterial *pMaterial = (ActMaterial*)geEntity_GetUserData(pEntity);

		// changed QD 07/15/06 - make missing actor not a fatal error
		if(pMaterial->Actor == NULL)
			continue;
		// end change

		if(EffectC_IsStringNull(pMaterial->ChangeMaterial))
			continue;

		if(!EffectC_IsStringNull(pMaterial->TriggerName))
		{
			if(GetTriggerState(pMaterial->TriggerName))
			{
				if(pMaterial->active == GE_FALSE)
				{
					pMaterial->active = GE_TRUE;
				}
			}
			else
			{
				if(pMaterial->active == GE_TRUE)
				{
					pMaterial->active = GE_FALSE;
				}
			}
		}
		else
		{
			if(pMaterial->active == GE_FALSE)
			{
				pMaterial->active = GE_TRUE;
			}
		}

		if(pMaterial->active == GE_TRUE)
		{
			pMaterial->Time += dwTicks;

			if(pMaterial->Time >= (1000.0f/pMaterial->Speed))
			{
				pMaterial->Time = 0.0f;
				switch(pMaterial->Style)
				{
				case 0:
					break;
				case 2:
					pMaterial->CurMat += pMaterial->CycleDir;
					if(pMaterial->CurMat >= pMaterial->MaterialCount || pMaterial->CurMat < 0)
					{
						pMaterial->CycleDir = -pMaterial->CycleDir;
						pMaterial->CurMat += pMaterial->CycleDir;
					}
					break;
				case 3:
					pMaterial->CurMat = ( rand() % pMaterial->MaterialCount );
					break;
				case 4:
					if(pMaterial->CurMat < (pMaterial->MaterialCount-1))
						pMaterial->CurMat +=1;
					break;
				default:
					pMaterial->CurMat +=1;
					if(pMaterial->CurMat>=pMaterial->MaterialCount)
						pMaterial->CurMat = 0;
					break;
				}
			}

			char MaterialName[256];

			// build material  name
			sprintf(MaterialName, "%s%d", pMaterial->ChangeMaterial, pMaterial->CurMat);

			if(pMaterial->Style != 0)
				CCD->ActorManager()->ChangeMaterial(pMaterial->Actor, MaterialName);
			else
				CCD->ActorManager()->ChangeMaterial(pMaterial->Actor, pMaterial->ChangeMaterial);

			if(pMaterial->ChangeLighting)
			{
				CCD->ActorManager()->SetActorDynamicLighting(pMaterial->Actor, pMaterial->FillColor,
					pMaterial->AmbientColor, pMaterial->AmbientLightFromFloor);
			}
		}
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
//	SaveTo
//
//	Save the current state of every ActMaterial in the current world off to an open file.
/* ------------------------------------------------------------------------------------ */
int CActMaterial::SaveTo(FILE *SaveFD, bool type)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	// Ok, check to see if there are ActMaterial entities in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "ActMaterial");

	if(!pSet)
		return RGF_SUCCESS;

	// Ok, we have ActMaterial entities somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		ActMaterial *pMaterial = (ActMaterial*)geEntity_GetUserData(pEntity);

		WRITEDATA(type, &(pMaterial->active),	sizeof(geBoolean),	1, SaveFD);
		WRITEDATA(type, &(pMaterial->CurMat),	sizeof(int),		1, SaveFD);
		WRITEDATA(type, &(pMaterial->CycleDir),	sizeof(int),		1, SaveFD);
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	RestoreFrom
//
//	Restore the state of every ActMaterial in the current world from an	open file.
/* ------------------------------------------------------------------------------------ */
int CActMaterial::RestoreFrom(FILE *RestoreFD, bool type)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	// Ok, check to see if there are  ElectricBolt in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "ActMaterial");

	if(!pSet)
		return RGF_SUCCESS;

	// Ok, we have logic gates somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		ActMaterial *pMaterial = (ActMaterial*)geEntity_GetUserData(pEntity);

		READDATA(type, &(pMaterial->active),	sizeof(geBoolean),	1, RestoreFD);
		READDATA(type, &(pMaterial->CurMat),	sizeof(int),		1, RestoreFD);
		READDATA(type, &(pMaterial->CycleDir),	sizeof(int),		1, RestoreFD);
	}

	return RGF_SUCCESS;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
