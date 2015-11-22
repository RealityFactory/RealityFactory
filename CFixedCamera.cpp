/*
	CFixedCamera.cpp:		Fixed Camera class implementation

	(c) 2000 Ralph Deane
	All Rights Reserved

	This file contains the class implementation for Fixed Camera
handling.
*/

//	Include the One True Header

#include "RabidFramework.h"

//	Constructor
//
//	Load up all FixedCameras and set the
//	..entity values.

CFixedCamera::CFixedCamera()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	Number = 0;
	Camera = NULL;

//	Ok, check to see if there are FixedCameras in this world

	pSet = geWorld_GetEntitySet(CCD->World(), "FixedCamera");

	if(!pSet)
		return;	

//	Ok, we have FixedCameras somewhere.  Dig through 'em all.

	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		FixedCamera *pSource = (FixedCamera*)geEntity_GetUserData(pEntity);
		Number+=1;
		pSource->Rotation.Z = 0.0174532925199433f*(pSource->Angle.Z);
		pSource->Rotation.X = 0.0174532925199433f*(pSource->Angle.X);
		pSource->Rotation.Y = 0.0174532925199433f*(pSource->Angle.Y-90.0f);
	}

  return;
}


//	Destructor
//
//	Clean up.

CFixedCamera::~CFixedCamera()
{

}

void CFixedCamera::Tick()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	pSet = geWorld_GetEntitySet(CCD->World(), "FixedCamera");
	
	if(!pSet || !Camera)
		return;	
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		FixedCamera *pSource = (FixedCamera*)geEntity_GetUserData(pEntity);
		if(!EffectC_IsStringNull(pSource->ForceTrigger))
		{
			if(GetTriggerState(pSource->ForceTrigger))
			{
				Camera = pSource;
				return;
			}
		}
	}

	pSet = geWorld_GetEntitySet(CCD->World(), "FixedCamera");

	bool flag = true;
	if(!EffectC_IsStringNull(Camera->TriggerName))
	{
		if(!GetTriggerState(Camera->TriggerName))
			flag = false;;
	}
	if(CheckFieldofView(Camera) && flag)
		return;
	else
	{
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity))
		{
			FixedCamera *pSource = (FixedCamera*)geEntity_GetUserData(pEntity);
			if(!EffectC_IsStringNull(pSource->TriggerName))
			{
				if(!GetTriggerState(pSource->TriggerName))
					continue;
			}
			if(CheckFieldofView(pSource))
			{
				Camera = pSource;
				return;
			}
		}
	}
	
}

bool CFixedCamera::GetFirstCamera()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	pSet = geWorld_GetEntitySet(CCD->World(), "FixedCamera");
	
	if(!pSet)
		return false;
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		FixedCamera *pSource = (FixedCamera*)geEntity_GetUserData(pEntity);
		if(CheckFieldofView(pSource))
		{
			Camera = pSource;
			return true;
		}
	}
// changed RF063
	pSet = geWorld_GetEntitySet(CCD->World(), "FixedCamera");
	pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL);
	Camera = (FixedCamera*)geEntity_GetUserData(pEntity);
	return true;
// end change RF063
}

bool CFixedCamera::CheckFieldofView(FixedCamera *pSource)
{
	geXForm3d Xf;
	geVec3d Pos, Direction, TgtPos, temp;
	float dotProduct;
	geXForm3d_SetIdentity(&Xf);
	geXForm3d_RotateZ(&Xf, pSource->Rotation.Z);
	geXForm3d_RotateX(&Xf, pSource->Rotation.X);
	geXForm3d_RotateY(&Xf, pSource->Rotation.Y);
	geXForm3d_Translate(&Xf, pSource->origin.X, pSource->origin.Y, pSource->origin.Z);
	
	Pos = Xf.Translation;
	geXForm3d_GetIn(&Xf, &Direction);
	TgtPos = CCD->Player()->Position();
	geVec3d_Subtract(&TgtPos,&Pos,&temp);
	geVec3d_Normalize(&temp);
	dotProduct = geVec3d_DotProduct(&temp,&Direction);
	if(dotProduct > ((90.0f-(pSource->FieldofView*28.6f))/90.0f))
	{
		if(CanSeePointToActor(&pSource->origin, CCD->Player()->GetActor()))
			return true;
	}
	return false;
}