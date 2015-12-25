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
#include "CFixedCamera.h"

/* ------------------------------------------------------------------------------------ */
//	Constructor
//
//	Load up all FixedCameras and set the entity values.
/* ------------------------------------------------------------------------------------ */
CFixedCamera::CFixedCamera() :
	m_EntityCount(0),
	m_Camera(NULL)
{
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

		++m_EntityCount;

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
	if(!m_Camera)
		return;

	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "FixedCamera");

	if(!pSet)
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
				m_Camera = pSource;
				return;
			}
		}
	}

	bool flag = true;

	if(!EffectC_IsStringNull(m_Camera->TriggerName))
	{
		if(!GetTriggerState(m_Camera->TriggerName))
			flag = false;
	}

	if(CheckFieldofView(m_Camera) && flag)
	{
		return;
	}
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
				m_Camera = pSource;
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
			m_Camera = pSource;
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
			m_Camera = pSource;
			return true;
		}
	}

// changed RF063
	pSet = geWorld_GetEntitySet(CCD->World(), "FixedCamera");
	pEntity = geEntity_EntitySetGetNextEntity(pSet, NULL);
	m_Camera = static_cast<FixedCamera*>(geEntity_GetUserData(pEntity));

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
	if(m_Camera)
		m_Camera->OriginOffset = Position;
}

/* ------------------------------------------------------------------------------------ */
//	Move
/* ------------------------------------------------------------------------------------ */
void CFixedCamera::Move(const geVec3d &Move)
{
	if(m_Camera)
		geVec3d_Add(&(m_Camera->OriginOffset), &Move, &(m_Camera->OriginOffset));
}

/* ------------------------------------------------------------------------------------ */
//	SetRotation
/* ------------------------------------------------------------------------------------ */
void CFixedCamera::SetRotation(const geVec3d &Rotation)
{
	if(m_Camera)
		m_Camera->Rotation = Rotation;
}

/* ------------------------------------------------------------------------------------ */
//	Rotate
/* ------------------------------------------------------------------------------------ */
void CFixedCamera::Rotate(const geVec3d &Rotate)
{
	if(m_Camera)
		geVec3d_Add(&(m_Camera->Rotation), &Rotate, &(m_Camera->Rotation));

}

/* ------------------------------------------------------------------------------------ */
//	SetFOV
/* ------------------------------------------------------------------------------------ */
void CFixedCamera::SetFOV(float FOV)
{
	if(m_Camera)
		m_Camera->FieldofView = FOV;
}
// end change

/* ----------------------------------- END OF FILE ------------------------------------ */
