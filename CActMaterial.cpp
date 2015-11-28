/****************************************************************************************/
/*																						*/
/*	CActMaterial.cpp			CActMaterial handler									*/
/*																						*/
/*	(c) 2002 Ralph Deane																*/
/*																						*/
/*	This file contains the class implementation for the									*/
/*	CActMaterial enitity  for RGF-based games.											*/
/*																						*/
/*	Programming provided by QD															*/
/*																						*/
/*	Edit History:																		*/
/*	=============																		*/
/*	07/15/06 QD:	- make missing actor not a fatal error								*/
/*																						*/
/****************************************************************************************/

// Include the One True Header
#include "RabidFramework.h"

/* ------------------------------------------------------------------------------------ */
//	Constructor
/* ------------------------------------------------------------------------------------ */
CActMaterial::CActMaterial()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	//	Ok, check to see if there are ActMaterial entities in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "ActMaterial");

// changed QD 12/15/05
//	Bottom = Top = NULL;

	if(!pSet)
		return;

	//	Ok, we have ActMaterials somewhere.  Dig through 'em all.
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

		//	Ok, put this entity into the Global Entity Registry
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
			sprintf(szError,"*WARNING* File %s - Line %d: %s: Missing actor '%s'\n",
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
// changed QD 12/15/05
/*
	if(Bottom)
	{
		MaterialList *pool, *temp;

		pool = Bottom;

		while(pool!= NULL)
		{
			temp = pool->next;
			delete pool;
			pool = temp;
		}

		Bottom = NULL;
	}
*/
// end change
}

/* ------------------------------------------------------------------------------------ */
//	Tick
/* ------------------------------------------------------------------------------------ */
void CActMaterial::Tick(float dwTicks)
{

	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	pSet = geWorld_GetEntitySet(CCD->World(), "ActMaterial");

	if(!pSet)
		return;         //	Not on this level.

	//  Ok, we have ActorMaterial entities somewhere. Dig through 'em all.
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
// changed QD 12/15/05
/*
			MaterialList *pool;

			pool = new MaterialList;
			pool->next = NULL;
			pool->prev = Top;

			if(!Bottom)
				Bottom = pool;

			Top = pool;

			if(pool->prev)
				pool->prev->next = pool;

			strcpy(pool->Entity, pMaterial->EntityName);
			strcpy(pool->Material, pMaterial->ChangeMaterial);
			pool->ChangeLighting = pMaterial->ChangeLighting;
*/
// end change
			pMaterial->Time += dwTicks;

			if(pMaterial->Time >= (1000.0f/pMaterial->Speed)) //(1000.0f*(1.0f/pMaterial->Speed))) // changed QD 12/15/05
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

			//	build material  name
			sprintf(MaterialName, "%s%d", pMaterial->ChangeMaterial, pMaterial->CurMat);

			if(pMaterial->Style != 0)
				CCD->ActorManager()->ChangeMaterial(pMaterial->Actor, MaterialName);
			else
				CCD->ActorManager()->ChangeMaterial(pMaterial->Actor, pMaterial->ChangeMaterial);

			if(pMaterial->ChangeLighting)
			{
// changed QD 12/15/05
//				pool->FillColor = pMaterial->FillColor;
//				pool->AmbientColor = pMaterial->AmbientColor;
// changed QD 07/21/04
//				pool->AmbientLightFromFloor = pMaterial->AmbientLightFromFloor;
				CCD->ActorManager()->SetActorDynamicLighting(pMaterial->Actor, pMaterial->FillColor,
					pMaterial->AmbientColor, pMaterial->AmbientLightFromFloor);
// end change
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
// changed QD 12/15/05
/*	bool Any = false;

	if(Bottom)
		Any = true;

	WRITEDATA(&Any, sizeof(bool), 1, SaveFD);

	if(Any)
	{
		MaterialList *pool, *temp;
		int count = 0;

		pool = Bottom;

		while	(pool!= NULL)
		{
			temp = pool->next;
			count += 1;
			pool = temp;
		}

		WRITEDATA(&count, sizeof(int), 1, SaveFD);
		pool = Bottom;

		while(pool!= NULL)
		{
			temp = pool->next;
			WRITEDATA(pool->Entity,					sizeof(char),	64, SaveFD);
			WRITEDATA(pool->Material,				sizeof(char),	64, SaveFD);
			WRITEDATA(&pool->ChangeLighting,		sizeof(bool),	1,	SaveFD);
			WRITEDATA(&pool->AmbientColor,			sizeof(GE_RGBA),1,	SaveFD);
			WRITEDATA(&pool->FillColor,				sizeof(GE_RGBA),1,	SaveFD);
// changed QD 07/21/04
			WRITEDATA(&pool->AmbientLightFromFloor, sizeof(bool),	1,	SaveFD);
// end change
			pool = temp;
		}
	}
*/
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

		WRITEDATA(&(pMaterial->active),		sizeof(geBoolean),	1, SaveFD);
		WRITEDATA(&(pMaterial->CurMat),		sizeof(int),		1, SaveFD);
		WRITEDATA(&(pMaterial->CycleDir),	sizeof(int),		1, SaveFD);
	}
// end change

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	RestoreFrom
//
//	Restore the state of every ActMaterial in the current world from an	open file.
/* ------------------------------------------------------------------------------------ */
int CActMaterial::RestoreFrom(FILE *RestoreFD, bool type)
{
// changed QD 12/15/05
/*
	bool Any;
	int count;

	Bottom = Top = NULL;

	READDATA(&Any, sizeof(bool), 1, RestoreFD);

	if(Any)
	{
		READDATA(&count, sizeof(int), 1, RestoreFD);

		for(int i=0; i<count; i++)
		{
			MaterialList *pool;
			pool = new MaterialList;
			pool->next = NULL;
			pool->prev = Top;

			if(!Bottom)
				Bottom = pool;

			Top = pool;

			if(pool->prev)
				pool->prev->next = pool;

			READDATA(pool->Entity,					sizeof(char),	64, RestoreFD);
			READDATA(pool->Material,				sizeof(char),	64, RestoreFD);
			READDATA(&pool->ChangeLighting,			sizeof(bool),	1,	RestoreFD);
			READDATA(&pool->AmbientColor,			sizeof(GE_RGBA),1,	RestoreFD);
			READDATA(&pool->FillColor,				sizeof(GE_RGBA),1,	RestoreFD);
// changed QD 07/21/04
			READDATA(&pool->AmbientLightFromFloor, sizeof(bool),	1,	RestoreFD);
// end change
			geActor *Actor = GetEntityActor(pool->Entity);
			CCD->ActorManager()->ChangeMaterial(Actor, pool->Material);

			if(pool->ChangeLighting)
// changed QD 07/21/04
				CCD->ActorManager()->SetActorDynamicLighting(Actor, pool->FillColor, pool->AmbientColor, pool->AmbientLightFromFloor);
// end change
		}
	}
*/
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

		READDATA(&(pMaterial->active),		sizeof(geBoolean),	1, RestoreFD);
		READDATA(&(pMaterial->CurMat),		sizeof(int),		1, RestoreFD);
		READDATA(&(pMaterial->CycleDir),	sizeof(int),		1, RestoreFD);
	}
// end change

	return RGF_SUCCESS;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
