/************************************************************************************//**
 * @file CAreaChecker.cpp
 * @author Dan Valeo
 * @date Jul 2004
 *//*
 * Copyright (c) 2004 Dan Valeo; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"

/* ------------------------------------------------------------------------------------ */
//	Constructor
/* ------------------------------------------------------------------------------------ */
CAreaChecker::CAreaChecker()
{
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
/* ------------------------------------------------------------------------------------ */
CAreaChecker::~CAreaChecker()
{
}

// changed QD 12/15/05
/* ------------------------------------------------------------------------------------ */
//	IsCloseEnough
/* ------------------------------------------------------------------------------------ */
bool CAreaChecker::IsCloseEnough(const geVec3d &Pos1, const geVec3d &Pos2, float diameter)
{
	geVec3d B;

	geVec3d_Subtract(&Pos1, &Pos2, &B);

	return (geVec3d_DotProduct(&B, &B) < diameter*diameter);
}
// end change

/* ------------------------------------------------------------------------------------ */
//	Tick
/* ------------------------------------------------------------------------------------ */
void CAreaChecker::Tick(geFloat dwTicks)
{
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "AreaSwitch");

	if(!pSet)
	    return;

	geEntity *pEntity;
	geVec3d cPos;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		AreaSwitch *S = static_cast<AreaSwitch*>(geEntity_GetUserData(pEntity));

		if(!EffectC_IsStringNull(S->Entity))
		{
			CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(S->Entity), &cPos);
		}
		else
		{
			CCD->ActorManager()->GetPosition(CCD->Player()->GetActor(), &cPos);
		}

		if(IsCloseEnough(S->origin, cPos, S->Diameter))
		{
			CCD->Pawns()->AddEvent(S->Trigger, true);
		}
		else
		{
			CCD->Pawns()->AddEvent(S->Trigger, false);
		}
	}
}


/* ----------------------------------- END OF FILE ------------------------------------ */
