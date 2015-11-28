/****************************************************************************************/
/*																						*/
/*	Edit History:																		*/
/*																						*/
/*	(c) 2004 Dan Valeo (Pickles Jul 04)													*/
/*																						*/
/*	Edit History:																		*/
/*																						*/
/****************************************************************************************/

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
	// float distsquared;
	geVec3d B;

	geVec3d_Subtract(&Pos1, &Pos2, &B);
	/*distsquared = geVec3d_DotProduct(&B, &B);

	if(distsquared < diameter * diameter)
		return true;

	return false;*/
	return (geVec3d_DotProduct(&B, &B) < diameter*diameter);
}
// end change

/* ------------------------------------------------------------------------------------ */
//	Tick
/* ------------------------------------------------------------------------------------ */
void CAreaChecker::Tick(float dwTicks)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	geVec3d cPos;

	pSet = geWorld_GetEntitySet(CCD->World(), "AreaSwitch");

	if(!pSet)
	    return;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
	    AreaSwitch *S = (AreaSwitch*)geEntity_GetUserData(pEntity);

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
