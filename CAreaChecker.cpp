// Pickles Jul 04

#include "RabidFramework.h"


CAreaChecker::CAreaChecker()
{
	return;
}

CAreaChecker::~CAreaChecker()
{
	return;
}

bool CAreaChecker::IsCloseEnough(geVec3d Pos1, geVec3d Pos2, float diameter)
{
	float dist;
	geVec3d B;
	geVec3d_Subtract(&Pos1,&Pos2,&B);
	dist = geVec3d_DotProduct(&B,&B);
	if(dist < diameter * diameter)
		return true;
	return false;
}

void CAreaChecker::Tick(float dwTicks)
{
	geEntity_EntitySet *	Set;
	geEntity *		Entity;
	geVec3d cPos;
	bool state;

	Set = geWorld_GetEntitySet(CCD->World(), "AreaSwitch");
	if (Set == NULL)
	    return;

	Entity = geEntity_EntitySetGetNextEntity(Set, NULL);

	while(Entity)
	{
	    AreaSwitch *	S;

		S = (AreaSwitch *)geEntity_GetUserData(Entity);

		if(!EffectC_IsStringNull(S->Entity))
			CCD->ActorManager()->GetPosition(CCD->ActorManager()->GetByEntityName(S->Entity),&cPos);
		else
			CCD->ActorManager()->GetPosition(CCD->Player()->GetActor(),&cPos);

		if(IsCloseEnough(S->origin,cPos,S->Diameter))
		{
			state = true;
		}
		else
		{
			state = false;
		}

		CCD->Pawns()->AddEvent(S->Trigger, state);
		
		Entity = geEntity_EntitySetGetNextEntity(Set, Entity);
	}

}


