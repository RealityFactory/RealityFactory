/************************************************************************************//**
 * @file CFixedCamera.cpp
 * @brief Fixed Camera class handler
 *
 * This file contains the class implementation for Fixed Camera handling.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2000 Ralph Deane; All rights reserved.
 ****************************************************************************************/

//	Include the One True Header
#include "RabidFramework.h"

/* ------------------------------------------------------------------------------------ */
//	Constructor
//
//	Load up all FixedCameras and set the entity values.
/* ------------------------------------------------------------------------------------ */
CFixedCamera::CFixedCamera()
{
	Number = 0;
	Camera = NULL;

	//	Ok, check to see if there are FixedCameras in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "FixedCamera");

	if(!pSet)
		return;

	geEntity *pEntity;

	//	Ok, we have FixedCameras somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		FixedCamera *pSource = static_cast<FixedCamera*>(geEntity_GetUserData(pEntity));

		Number += 1;

		pSource->Rotation.Z = GE_PIOVER180*(pSource->Angle.Z);
		pSource->Rotation.X = GE_PIOVER180*(pSource->Angle.X);
		pSource->Rotation.Y = GE_PIOVER180*(pSource->Angle.Y-90.0f);
		pSource->OriginOffset = pSource->origin;
		pSource->RealAngle = pSource->Rotation;
// changed QD 12/15/05
		if(pSource->FOVCheckRange < 0.0f)
			pSource->FOVCheckRange = 0.0f;
		else if(pSource->FOVCheckRange > 100.0f)
			pSource->FOVCheckRange = 100.0f;

		pSource->FOVCheckRange *= 0.01f;
// end change

// changed RF064
		if(pSource->Model)
		{
			geVec3d ModelOrigin;
	    	geWorld_GetModelRotationalCenter(CCD->World(), pSource->Model, &ModelOrigin);
			geVec3d_Subtract(&pSource->origin, &ModelOrigin, &pSource->OriginOffset);
  		}
// end change RF064
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
//
//	Clean up.
/* ------------------------------------------------------------------------------------ */
CFixedCamera::~CFixedCamera()
{

}

/* ------------------------------------------------------------------------------------ */
//	Tick
/* ------------------------------------------------------------------------------------ */
void CFixedCamera::Tick()
{

	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "FixedCamera");

	if(!pSet || !Camera)
		return;

	geEntity *pEntity;

// changed RF064
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		FixedCamera *pSource = static_cast<FixedCamera*>(geEntity_GetUserData(pEntity));

		pSource->origin = pSource->OriginOffset;
		SetOriginOffset(pSource->EntityName, pSource->BoneName, pSource->Model, &(pSource->origin));

		if(pSource->Model)
		{
			geXForm3d Xf;
			geVec3d  Tmp;
			geWorld_GetModelXForm(CCD->World(), pSource->Model, &Xf);
			geXForm3d_GetEulerAngles(&Xf, &Tmp);
			geVec3d_Add(&pSource->RealAngle, &Tmp, &pSource->Rotation);
		}
		else if(!EffectC_IsStringNull(pSource->EntityName) && !pSource->AngleRotation)
		{
			SetAngle(pSource->EntityName, pSource->BoneName, &pSource->Rotation);
		}
	}

	pSet = geWorld_GetEntitySet(CCD->World(), "FixedCamera");
// end change RF064

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		FixedCamera *pSource = static_cast<FixedCamera*>(geEntity_GetUserData(pEntity));

		if(!EffectC_IsStringNull(pSource->ForceTrigger))
		{
			if(GetTriggerState(pSource->ForceTrigger))
			{
				Camera = pSource;
				return;
			}
		}
	}

	bool flag = true;

	if(!EffectC_IsStringNull(Camera->TriggerName))
	{
		if(!GetTriggerState(Camera->TriggerName))
			flag = false;
	}

	if(CheckFieldofView(Camera) && flag)
		return;
	else
	{
		pSet = geWorld_GetEntitySet(CCD->World(), "FixedCamera");

		for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
			pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
		{
			FixedCamera *pSource = static_cast<FixedCamera*>(geEntity_GetUserData(pEntity));

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

/* ------------------------------------------------------------------------------------ */
//	GetFirstCamera
/* ------------------------------------------------------------------------------------ */
bool CFixedCamera::GetFirstCamera()
{
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "FixedCamera");

	if(!pSet)
		return false;

	geEntity *pEntity;

// changed RF064
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		FixedCamera *pSource = static_cast<FixedCamera*>(geEntity_GetUserData(pEntity));

		pSource->origin = pSource->OriginOffset;
		SetOriginOffset(pSource->EntityName, pSource->BoneName, pSource->Model, &(pSource->origin));

		if(pSource->Model)
		{
			geXForm3d Xf;
			geVec3d  Tmp;
			geWorld_GetModelXForm(CCD->World(), pSource->Model, &Xf);
			geXForm3d_GetEulerAngles(&Xf, &Tmp);
			geVec3d_Add(&pSource->RealAngle, &Tmp, &pSource->Rotation);
		}
		else if(!EffectC_IsStringNull(pSource->EntityName) && !pSource->AngleRotation)
		{
			SetAngle(pSource->EntityName, pSource->BoneName, &pSource->Rotation);
		}
	}

	pSet = geWorld_GetEntitySet(CCD->World(), "FixedCamera");

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		FixedCamera *pSource = static_cast<FixedCamera*>(geEntity_GetUserData(pEntity));

		if(pSource->UseFirst)
		{
			Camera = pSource;
			return true;
		}
	}

	pSet = geWorld_GetEntitySet(CCD->World(), "FixedCamera");
// end change RF064

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		FixedCamera *pSource = static_cast<FixedCamera*>(geEntity_GetUserData(pEntity));

		if(CheckFieldofView(pSource))
		{
			Camera = pSource;
			return true;
		}
	}

// changed RF063
	pSet = geWorld_GetEntitySet(CCD->World(), "FixedCamera");
	pEntity = geEntity_EntitySetGetNextEntity(pSet, NULL);
	Camera = (FixedCamera*)geEntity_GetUserData(pEntity);

	return true;
// end change RF063
}

/* ------------------------------------------------------------------------------------ */
//	CheckFieldofView
/* ------------------------------------------------------------------------------------ */
bool CFixedCamera::CheckFieldofView(FixedCamera *pSource)
{
	geXForm3d Xf;

// changed QD 12/15/05
	//geXForm3d_SetIdentity(&Xf);
	//geXForm3d_RotateZ(&Xf, pSource->Rotation.Z);
	geXForm3d_SetZRotation(&Xf, pSource->Rotation.Z);
	geXForm3d_RotateX(&Xf, pSource->Rotation.X);
	geXForm3d_RotateY(&Xf, pSource->Rotation.Y);
	// QD: translation doesn't influence the orientation
	//geXForm3d_Translate(&Xf, pSource->origin.X, pSource->origin.Y, pSource->origin.Z);

	geVec3d Pos = pSource->origin;

	geVec3d Direction, TgtPos, temp;

// end change
	geXForm3d_GetIn(&Xf, &Direction);
	TgtPos = CCD->Player()->Position();
	geVec3d_Subtract(&TgtPos, &Pos, &temp);
	geVec3d_Normalize(&temp);
	float dotProduct = geVec3d_DotProduct(&temp, &Direction);

// changed QD 12/15/05 - simplified right side, then added a CheckRange factor (0 - 1)
	// if(dotProduct > ((90.0f-(pSource->FieldofView*28.6f))/90.0f))
	if(dotProduct > (1.f - pSource->FieldofView*pSource->FOVCheckRange*GE_1OVERPI))
	{
		if(CanSeePointToActor(&pSource->origin, CCD->Player()->GetActor()))
			return true;
	}

	return false;
}

// changed QD 12/15/05
/* ------------------------------------------------------------------------------------ */
//	SetPosition
/* ------------------------------------------------------------------------------------ */
void CFixedCamera::SetPosition(const geVec3d &Position)
{
	if(Camera)
		Camera->OriginOffset = Position;
}

/* ------------------------------------------------------------------------------------ */
//	Move
/* ------------------------------------------------------------------------------------ */
void CFixedCamera::Move(const geVec3d &Move)
{
	if(Camera)
		geVec3d_Add(&(Camera->OriginOffset), &Move, &(Camera->OriginOffset));
}

/* ------------------------------------------------------------------------------------ */
//	SetRotation
/* ------------------------------------------------------------------------------------ */
void CFixedCamera::SetRotation(const geVec3d &Rotation)
{
	if(Camera)
		Camera->Rotation = Rotation;
}

/* ------------------------------------------------------------------------------------ */
//	Rotate
/* ------------------------------------------------------------------------------------ */
void CFixedCamera::Rotate(const geVec3d &Rotate)
{
	if(Camera)
		geVec3d_Add(&(Camera->Rotation), &Rotate, &(Camera->Rotation));

}

/* ------------------------------------------------------------------------------------ */
//	SetFOV
/* ------------------------------------------------------------------------------------ */
void CFixedCamera::SetFOV(float FOV)
{
	if(Camera)
		Camera->FieldofView = FOV;
}
// end change

/* ----------------------------------- END OF FILE ------------------------------------ */
