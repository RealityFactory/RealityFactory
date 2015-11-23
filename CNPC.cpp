/*
CNPC.cpp:		Non-player character manager class

  This file contains the class implementation for all non-
  player characters in an RGF-based game.
*/

//	You only need the one, master include file.

#include "RabidFramework.h"

extern geSound_Def *SPool_Sound(char *SName);

#define BOT_DIR_AWAY    -1
#define BOT_DIR_TOWARD   1
#define BOT_DIR_NONE     0

#define BOT_SEARCH_Y_RANGE 40.0f

#define CLOSE_RANGE_DIST 100.0f
#define MED_RANGE_DIST   700.0f
#define LONG_RANGE_DIST  900.0f

#define TIMETODEACTIVATE	20.0f
#define BOTPICKTIME			 3.0f
#define PLAYERPICKTIME		10.0f
#define TIMENOTSEEN			10.0f
#define TIMESEEN			15.0f
#define MODETIMEOUT			15.0f
#define MODETIMESEEN		 5.0f
#define MODETIMENOTSEEN		 6.0f
#define MINMOVEDIST			20.0f

enum {	
	MODE_NULL = -1,
		MODE_ATTACK = 0, 
		MODE_RETREAT, 
		MODE_WANDER, 
		MODE_FIND_PLAYER, 
		MODE_FIND_PLAYER_QUICK, 
		MODE_CROWD_PLAYER,
		MODE_GOAL_POINT,
		MODE_ON_TRACK,
		MODE_RETREAT_ON_TRACK,
		MODE_UNSTICK,
		MODE_WANDER_ON_TRACK,
		MODE_WALK,
		MODE_WALK_ON_TRACK,
		MODE_MAX,
};

int32 ModeTable[MODE_MAX] =
{
	MODE_ATTACK, 
		MODE_RETREAT, 
		MODE_WANDER, 
		MODE_FIND_PLAYER, 
		MODE_FIND_PLAYER_QUICK,
		MODE_CROWD_PLAYER,
		MODE_GOAL_POINT,
		MODE_ON_TRACK,
		MODE_RETREAT_ON_TRACK,
		MODE_UNSTICK,
		MODE_WANDER_ON_TRACK,
		MODE_WALK,
		MODE_WALK_ON_TRACK,
};

// debug strings
char *ModeText[] = 
{
	"MODE_ATTACK", 
		"MODE_RETREAT", 
		"MODE_WANDER", 
		"MODE_FIND_PLAYER", 
		"MODE_FIND_PLAYER_QUICK",
		"MODE_CROWD_PLAYER",
		"MODE_GOAL_POINT",
		"MODE_ON_TRACK",
		"MODE_RETREAT_ON_TRACK",
		"MODE_UNSTICK",
		"MODE_WANDER_ON_TRACK",
		"MODE_WALK",
		"MODE_WALK_ON_TRACK",
};

geBoolean NPC_ModeThinkFindPlayer(void *Data, float dwTicks);
geBoolean NPC_ModeThinkRetreat(void *Data, float Time);
geBoolean NPC_ModeActionRetreat(void *Data, float Time);
geBoolean NPC_ModeThinkFindPlayerQuick(void *Data, float Time);
geBoolean NPC_ModeThinkFindPlayer(void *Data, float Time);
geBoolean NPC_ModeThinkWander(void *Data, float Time);
geBoolean NPC_ModeThinkWanderGoal(void *Data, float Time);
geBoolean NPC_ModeThinkAttack(void *Data, float Time);
geBoolean NPC_ModeThinkCrowdPlayer(void *Data, float Time);
geBoolean NPC_ModeThinkGoalPoint(void *Data, float Time);
geBoolean NPC_ModeThinkOnTrack(void *Data, float Time);
geBoolean NPC_ModeThinkUnstick(void *Data, float Time);
geBoolean NPC_ModeThinkWalk(void *Data, float Time);
geBoolean NPC_ModeThinkWalkOnTrack(void *Data, float Time);

CONTROLp ModeThinkFunc[MODE_MAX] = 
{
	NPC_ModeThinkAttack,
		NPC_ModeThinkRetreat,
		NPC_ModeThinkWander,
		NPC_ModeThinkFindPlayer,
		NPC_ModeThinkFindPlayerQuick,
		NPC_ModeThinkCrowdPlayer,
		NPC_ModeThinkGoalPoint,
		NPC_ModeThinkOnTrack,
		NPC_ModeThinkOnTrack,//RetreatOnTrack,
		NPC_ModeThinkUnstick,
		NPC_ModeThinkOnTrack,//WanderGoal,
		NPC_ModeThinkWalk,
		NPC_ModeThinkWalkOnTrack
};

CONTROLp ModeAction[MODE_MAX] = 
{
	NULL,//Attack,
		NPC_ModeActionRetreat,//Retreat,
		NULL,//Wander,
		NULL,//FindPlayer,
		NULL,//FindPlayerQuick,
		NULL,//CrowdPlayer,
		NULL,//GoalPoint,
		NULL,//OnTrack,
		NPC_ModeActionRetreat,//RetreatOnTrack,
		NULL,//Unstick,
		NULL,//WanderGoal,
		NULL,
		NULL,
};

typedef struct 
{
	short range;
	void *action;
} DECISION, *DECISIONp;

int32 WalkTrackList[] =
{
	TRACK_TYPE_SCAN_LOOP,
	TRACK_TYPE_SCAN,
	TRACK_TYPE_TRAVERSE,
		-1
};

int32 HideTrackList[] =
{
	TRACK_TYPE_HIDE,
		-1
};

char *ItemHealthList[] = {"health", NULL};
char *ItemWeaponList[] = {"health", NULL};

geBoolean NPC_ClearTrack(Bot_Var *DBot);
geBoolean NPC_Control(Bot_Var *DBot, float dwTicks);
geActor *NPC_PickTgtPlayer(Bot_Var *DBot, geBoolean ForcePick);
geBoolean NPC_TargetTest(Bot_Var *DBot, float dwTicks);
geBoolean NPC_ModeThink(void *Data, float dwTicks);
geBoolean NPC_ModeChange(void *Data, int32 NewMode, geBoolean Think, float Time);
geBoolean NPC_Reposition(void *Data, geVec3d *TgtPos, int32 Dir);
geBoolean NPC_InitFindMultiTrack(void *Data, geVec3d *DestPos, float Time);
geBoolean NPC_InitFindMultiTrackAway(void *Data, geVec3d *DestPos, float Time);
geBoolean NPC_InitMoveToPoint(void *Data, geVec3d *Pos);
geBoolean NPC_InitMoveFree(void *Data, geVec3d *Pos);
geBoolean NPC_InitGenericMove(void *Data, float Time);
geBoolean NPC_InitMoveCloser(void *Data, float Time);
geBoolean NPC_InitRunCloser(void *Data, float Time);
geBoolean NPC_InitTrackToGoal(void *Data, float Time);
geBoolean NPC_InitTrackAwayGoal(void *Data, float Time);
geBoolean NPC_InitHide(void *Data, float Time);
geBoolean NPC_InitMoveAway(void *Data, float Time);
geBoolean NPC_InitRunAway(void *Data, float Time);
geBoolean NPC_InitMaintain(void *Data, float Time);
geBoolean NPC_InitWaitForPlayer(void *Data, float Time);
geBoolean NPC_WaitForPlayer(void *Data, float Time);
geBoolean NPC_InitWaitForEntityVisible(void *Data, float Time);
geBoolean NPC_WaitForEntityVisible(void *Data, float Time);
geBoolean NPC_InitWaitForEntityDist(void *Data, float Time);
geBoolean NPC_WaitForEntityDist(void *Data, float Time);
geBoolean NPC_SetupXForm(void *Data, geVec3d *OrientVec);
geBoolean NPC_Animate(void *Data, float Time);
geActor *NPC_FindItem(geVec3d *Pos, char *ClassList[], float YDiff, float Range);
geActor *NPC_FindRandomItem(geVec3d *Pos, char *ClassList[]);
Track *NPC_FindRandomTrack(geVec3d *Pos, int32 *TrackTypeList);
geBoolean NPC_FindNewMoveVec(void *Data, const geVec3d *TargetPos, int32 dir, float DistToMove, geVec3d *save_vec);
geBoolean NPC_PastPoint(geVec3d *Pos, geVec3d *MoveVector, geVec3d *TgtPos);
geBoolean NPC_MoveToPoint(void *Data, float Time);
geBoolean NPC_ActionGetHealth(void *Data, float Range, float Time);
geBoolean NPC_ActionGetWeapon(void *Data, float Range, float Time);
int32 NPC_CompareWeapons(geActor *Actor1, geActor *Actor2);
Track *NPC_FindTrackToGoal(void *Data, float Time);
geBoolean NPC_ValidateTrackPoints(void *Data, Track* t);
geBoolean NPC_ValidateMultiTrackPoints(void *Data, Track* t);
Track *NPC_FindTrack(void *Data, int32 TrackArr[]);
geBoolean NPC_TrackAction(void *Data, const TrackPt* ThisPoint, const TrackPt* NextPoint, float Time);
geBoolean NPC_LeaveTrack(void *Data, float Time);
geBoolean NPC_FinishTrack(void *Data);
geBoolean NPC_MoveFree(void *Data, float Time);
geBoolean NPC_MoveScan(void *Data, geVec3d *vec_dir, float dist, geVec3d *stop_pos);
geBoolean NPC_Movement(void *Data, float Time);
geBoolean NPC_Jump(void *Data, float Time);
geBoolean NPC_Shoot(void *Data, geVec3d *ShootPosition, float Time);
geBoolean NPC_OverLedgeScan(void *Data, const geVec3d *StartPos, const geVec3d *EndPos, geVec3d *StopPos);
int NPC_Rank(void *Data);
geBoolean NPC_InitShootPoint(void *Data, float Time);
geBoolean NPC_ShootPoint(void *Data, float Time);
geBoolean PlayerDead(geActor *Actor);

CNPC::CNPC()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	//	Ok, check to see if there are NPC in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "NonPlayerCharacter");
	
	if(pSet) 
	{
		//	Ok, we have NonPlayerCharacters somewhere.  Dig through 'em all.
		
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
			NonPlayerCharacter *pSource = (NonPlayerCharacter*)geEntity_GetUserData(pEntity);
			if(EffectC_IsStringNull(pSource->szEntityName))
			{
				char szName[128];
				geEntity_GetName(pEntity, szName, 128);
				pSource->szEntityName = szName;
			}
			// Ok, put this entity into the Global Entity Registry
			CCD->EntityRegistry()->AddEntity(pSource->szEntityName, "NonPlayerCharacter");
			pSource->alive = GE_TRUE;
			pSource->active = GE_FALSE;
			pSource->bState = GE_FALSE;
			pSource->CallBack = GE_FALSE;
			pSource->Location = pSource->origin;
			pSource->ActorRotation.X = 0.0174532925199433f*pSource->ActorRotation.X;
			pSource->ActorRotation.Y = 0.0174532925199433f*pSource->ActorRotation.Y;
			pSource->ActorRotation.Z = 0.0174532925199433f*pSource->ActorRotation.Z;
			pSource->Actor = CCD->ActorManager()->SpawnActor(pSource->szActorName, 
				pSource->origin, pSource->ActorRotation, "", "", NULL);
			if(!pSource->Actor)
			{
				char szError[256];
				sprintf(szError,"%s : Missing Actor '%s'", pSource->szEntityName, pSource->szActorName);
				CCD->ReportError(szError, false);
				CCD->ShutdownLevel();
				delete CCD;
				MessageBox(NULL, szError,"NPC", MB_OK);
				exit(-333);
			}
			CCD->ActorManager()->RemoveActor(pSource->Actor);
			geActor_Destroy(&pSource->Actor);
			if(!EffectC_IsStringNull(pSource->DieSound))
				SPool_Sound(pSource->DieSound);
			if(!EffectC_IsStringNull(pSource->AttackSound))
				SPool_Sound(pSource->AttackSound);
			pSource->DBot = (void *)GE_RAM_ALLOCATE_STRUCT(Bot_Var);
			memset(pSource->DBot, 0, sizeof(Bot_Var));
		}
	}
	return;
}

//	Destructor
//

CNPC::~CNPC()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	//	Ok, check to see if there are NPC in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "NonPlayerCharacter");
	
	if(pSet) 
	{
		//	Ok, we have NonPlayerCharacters somewhere.  Dig through 'em all.
		
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
			NonPlayerCharacter *pSource = (NonPlayerCharacter*)geEntity_GetUserData(pEntity);
			Bot_Var *DBot = (Bot_Var *)pSource->DBot;
			if(DBot->Actor)
			{
				CCD->ActorManager()->RemoveActor(DBot->Actor);
				geActor_Destroy(&DBot->Actor);
			}
			if(DBot->TrackStack.Data)
				geRam_Free(DBot->TrackStack.Data);
			geRam_Free(DBot);
		}
	}
}


//	Tick
//

void CNPC::Tick(float dwTicks)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	Bot_Var *DBot;
	
	//	Ok, check to see if there are NonPlayerCharacters in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "NonPlayerCharacter");
	
	if(pSet) 
	{
		
		//	Ok, we have NonPlayerCharacters somewhere.  Dig through 'em all.
		
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
			NonPlayerCharacter *pSource = (NonPlayerCharacter*)geEntity_GetUserData(pEntity);
			if(!pSource->alive)
			{
				if(pSource->ReSpawn)
				{
					if(pSource->MaxNumber == -1 || pSource->MaxNumber>0)
					{
						pSource->Tick+=dwTicks;
						if(pSource->Tick>=(pSource->Delay*1000.0f))
						{
							pSource->alive=GE_TRUE;
							pSource->active = GE_FALSE;
							pSource->bState = GE_FALSE;
						}
					}
				}
				continue;
			}
			if(!EffectC_IsStringNull(pSource->TriggerName) && !pSource->active)
			{
				if(!GetTriggerState(pSource->TriggerName))
					continue;
			}
			if(!pSource->active)
			{
				pSource->active = GE_TRUE;
				pSource->CallBack = GE_TRUE;
				Spawn(pSource);
				pSource->MaxNumber-=1;
			}
			DBot = (Bot_Var *)pSource->DBot;
			if(DBot->theInv->Value(DBot->Attribute)<=0 && !DBot->Dying)
			{
				DBot->Dying = true;
				CCD->ActorManager()->SetNoCollide(DBot->Actor);
				CCD->ActorManager()->SetMotion(DBot->Actor, DBot->AnimDie);
				CCD->ActorManager()->SetHoldAtEnd(DBot->Actor, true);
				if(!EffectC_IsStringNull(DBot->DieSound))
				{
					Snd Sound;
					memset( &Sound, 0, sizeof( Sound ) );
					CCD->ActorManager()->GetPosition(DBot->Actor, &Sound.Pos);
					Sound.Min=kAudibleRadius;
					Sound.Loop=false;
					Sound.SoundDef = SPool_Sound(DBot->DieSound);
					CCD->EffectManager()->Item_Add(EFF_SND, (void *)&Sound);
				}
			}
			if(DBot->Dying)
			{
				if(CCD->ActorManager()->EndAnimation(DBot->Actor))
				{
// changed RF064
					if(DBot->DyingTime>-1.0f)
					{
						geFloat fAlpha;
						CCD->ActorManager()->GetAlpha(DBot->Actor, &fAlpha);
						fAlpha -= (DBot->AlphaStep*(dwTicks/1000.0f));
						CCD->ActorManager()->SetAlpha(DBot->Actor, fAlpha);
						DBot->DyingTime -= (dwTicks/1000.0f);
						if(DBot->DyingTime <=0.0f)
						{
							CCD->ActorManager()->GetPosition(DBot->Actor, &pSource->Location);
							if(DBot->Actor)
							{
								CCD->ActorManager()->RemoveActor(DBot->Actor);
								geActor_Destroy(&DBot->Actor);
							}
							if(DBot->TrackStack.Data)
								geRam_Free(DBot->TrackStack.Data);
							memset(DBot, 0, sizeof(Bot_Var));
							pSource->alive=GE_FALSE;
							pSource->bState = GE_TRUE;
							pSource->CallBack = GE_FALSE;
							pSource->Tick = 0.0f;
						}
					}
// end change RF064
				}
			}
			else
			{
				if(DBot->Attacking && CCD->ActorManager()->EndAnimation(DBot->Actor))
				{
					CCD->ActorManager()->SetHoldAtEnd(DBot->Actor, false);
					CCD->ActorManager()->SetMotion(DBot->Actor, DBot->LastAnimation);
					strcpy(DBot->Animation,DBot->LastAnimation);
					if(DBot->StopToAttack)
						DBot->Stopped = DBot->LastStopped;
					DBot->Attacking = false;
				}
				NPC_Control(DBot, dwTicks);
			}
		}
	}
	
	return;
}

void CNPC::Spawn(NonPlayerCharacter *pSource)
{
	Bot_Var *DBot;
	geExtBox theBox;
	
	DBot = (Bot_Var *)pSource->DBot;
	memset(DBot, 0, sizeof(Bot_Var));

	DBot->Actor = CCD->ActorManager()->SpawnActor(pSource->szActorName, 
		pSource->origin, pSource->ActorRotation, pSource->AnimIdle, pSource->AnimIdle, NULL);
	CCD->ActorManager()->SetScale(DBot->Actor, pSource->Scale);
	CCD->ActorManager()->SetType(DBot->Actor, ENTITY_NPC);
	CCD->ActorManager()->SetAutoStepUp(DBot->Actor, true);
// changed RF064
	CCD->ActorManager()->SetAlpha(DBot->Actor, pSource->ActorAlpha);
	CCD->ActorManager()->SetHideRadar(DBot->Actor, pSource->HideFromRadar);
	if(!EffectC_IsStringNull(pSource->ChangeMaterial))
		CCD->ActorManager()->ChangeMaterial(DBot->Actor, pSource->ChangeMaterial);
// end change RF064
	CCD->ActorManager()->SetBoxChange(DBot->Actor, false);
	CCD->ActorManager()->SetBoundingBox(DBot->Actor, pSource->AnimIdle);
	if(pSource->Gravity)
		CCD->ActorManager()->SetGravity(DBot->Actor, CCD->Player()->GetGravity());
	DBot->theInv = CCD->ActorManager()->Inventory(DBot->Actor);
	DBot->theInv->AddAndSet(pSource->Attribute,(int)pSource->AttributeAmt);
	DBot->theInv->SetValueLimits(pSource->Attribute, 0, (int)pSource->AttributeAmt);
	strcpy(DBot->Attribute, pSource->Attribute);
	DBot->RunSpeed = pSource->Speed/100.0f;
	DBot->Mode = MODE_WALK;
	DBot->Action = NPC_ModeThink;
	NPC_ClearTrack(DBot);
	DBot->Dir = 1;
    StackInit(&DBot->TrackStack);
	DBot->Stopped = true;
	DBot->NextWeaponTime = 0.0f;
	DBot->Active = false;
	if(!EffectC_IsStringNull(pSource->ActivateTriggerName))
		strcpy(DBot->Trigger,pSource->ActivateTriggerName);
	DBot->Dying = false;
	DBot->Attacking = false;
	CCD->ActorManager()->GetBoundingBox(DBot->Actor, &theBox);
	DBot->MeleeDistance = 2.0f * (((theBox.Max.X) < (theBox.Max.Z)) ? (theBox.Max.X) : (theBox.Max.Z));
	strcpy(DBot->Animation, pSource->AnimIdle);
	strcpy(DBot->AnimIdle, pSource->AnimIdle);
	strcpy(DBot->AnimWalk, pSource->AnimWalk);
	strcpy(DBot->AnimWalkAttack, pSource->AnimWalkAttack);
	strcpy(DBot->AnimAttack, pSource->AnimAttack);
	strcpy(DBot->AnimJump, pSource->AnimJump);
	strcpy(DBot->AnimDie, pSource->AnimDie);
	DBot->Rank = pSource->Aggresiveness;
	DBot->DyingTime = pSource->DyingTime;
	DBot->StopToAttack = pSource->StopToAttack;
	DBot->AttackDelay = pSource->AttackDelay;
	DBot->DamageAmt = pSource->DamageAmt;
	strcpy(DBot->DamageAttribute, pSource->DamageAttribute);
	strcpy(DBot->DieSound, pSource->DieSound);
	strcpy(DBot->AttackSound, pSource->AttackSound);
	DBot->Melee = pSource->Melee;
	strcpy(DBot->Projectile, pSource->Projectile);
	strcpy(DBot->FireBone, pSource->FireBone);
	DBot->FireOffset = pSource->FireOffset;
	if(pSource->AimingSkill>=10)
		DBot->AimSkill = 0.0f;
	else
	{
		if(pSource->AimingSkill<1)
			pSource->AimingSkill = 1;
		DBot->AimSkill = 1.0f/(float)pSource->AimingSkill;
	}
	DBot->AimDelay = 1.0f;
	DBot->Wander = true;
	DBot->AlphaStep = 255.0f/DBot->DyingTime;
}

void CNPC::Debug()
{
	geVec3d Pos;
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, check to see if there are NPC in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "NonPlayerCharacter");
	
	if(pSet) 
	{
		//	Ok, we have NonPlayerCharacters somewhere.  Dig through 'em all.
		
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
			NonPlayerCharacter *pSource = (NonPlayerCharacter*)geEntity_GetUserData(pEntity);
			if(pSource->alive && pSource->active)
			{
				Bot_Var *DBot = (Bot_Var *)pSource->DBot;
				
				char szData[256];
				sprintf(szData,"Mode : %s", ModeText[DBot->Mode]);
				CCD->MenuManager()->FontRect(szData, FONT10, 5, 10);
				
				CCD->ActorManager()->GetPosition(DBot->Actor, &Pos);
				sprintf(szData,"Position : %f %f %f", Pos.X, Pos.Y, Pos.Z);
				CCD->MenuManager()->FontRect(szData, FONT10, 5, 30);
				
				if(CCD->Track()->Track_OnTrack(&DBot->TrackInfo))
					sprintf(szData,"OnTrack : true");
				else
					sprintf(szData,"OnTrack : false");
				CCD->MenuManager()->FontRect(szData, FONT10, 5, 50);
				
				sprintf(szData,"Action : UnKnown");
				if(DBot->Action == NPC_ModeThink)
					sprintf(szData,"Action : NPC_ModeThink");
				if(DBot->Action == NPC_MoveToPoint)
					sprintf(szData,"Action : NPC_MoveToPoint");
				if(DBot->Action == NPC_MoveFree)
					sprintf(szData,"Action : NPC_MoveFree");
				if(DBot->Action == NPC_WaitForPlayer)
					sprintf(szData,"Action : NPC_WaitForPlayer");
				if(DBot->Action == NPC_WaitForEntityVisible)
					sprintf(szData,"Action : NPC_WaitForEntityVisible");
				if(DBot->Action == NPC_WaitForEntityDist)
					sprintf(szData,"Action : NPC_WaitForEntityDist");
				if(DBot->Action == NPC_Jump)
					sprintf(szData,"Action : NPC_Jump");
				CCD->MenuManager()->FontRect(szData, FONT10, 5, 70);
				
				sprintf(szData,"Velocity : %f %f %f", DBot->Velocity.X, DBot->Velocity.Y, DBot->Velocity.Z);
				CCD->MenuManager()->FontRect(szData, FONT10, 5, 90);
				
				sprintf(szData,"Direction : %f %f %f", DBot->MoveVec.X, DBot->MoveVec.Y, DBot->MoveVec.Z);
				CCD->MenuManager()->FontRect(szData, FONT10, 5, 110);
				
				sprintf(szData,"ModeTimeOut : %f", DBot->ModeTimeOut);
				CCD->MenuManager()->FontRect(szData, FONT10, 5, 130);
				
				if(CCD->ActorManager()->EndAnimation(DBot->Actor))
					sprintf(szData,"Dying Over : true");
				else
					sprintf(szData,"Dying Over : false");
				CCD->MenuManager()->FontRect(szData, FONT10, 5, 150);
				
				sprintf(szData,"Aim Position : %f %f %f", DBot->AimPos.X, DBot->AimPos.Y, DBot->AimPos.Z);
				CCD->MenuManager()->FontRect(szData, FONT10, 5, 170);
				
				sprintf(szData,"Distance to Goal : %f", geVec3d_DistanceBetween(&Pos, &DBot->GoalPos));
				CCD->MenuManager()->FontRect(szData, FONT10, 5, 190);
				
				sprintf(szData,"Current Health : %d", DBot->theInv->Value(DBot->Attribute));
				CCD->MenuManager()->FontRect(szData, FONT10, 5, 210);
			}
		}
	}
}

//	******************** CRGF Overrides ********************

//	LocateEntity
//
//	Given a name, locate the desired item in the currently loaded level
//	..and return it's user data.

int CNPC::LocateEntity(char *szName, void **pEntityData)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, check to see if there are static entity proxies in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "NonPlayerCharacter");
	
	if(!pSet) 
		return RGF_NOT_FOUND;									// No static entity proxies
	
	//	Ok, we have static entity proxies.  Dig through 'em all.
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		NonPlayerCharacter *pTheEntity = (NonPlayerCharacter*)geEntity_GetUserData(pEntity);
		if(strcmp(pTheEntity->szEntityName, szName) == 0)
		{
			*pEntityData = (void *)pTheEntity;
			return RGF_SUCCESS;
		}
	}
	
	return RGF_NOT_FOUND;								// Sorry, no such entity here
}

// start multiplayer
int CNPC::SaveTo(FILE *SaveFD, bool type)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, check to see if there are NonPlayerCharacter in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "NonPlayerCharacter");
	
	if(!pSet) 
		return RGF_NOT_FOUND;
	
	//	Ok, we have NonPlayerCharacter.  Dig through 'em all.
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		NonPlayerCharacter *pTheEntity = (NonPlayerCharacter*)geEntity_GetUserData(pEntity);
		WRITEDATA(&pTheEntity->alive, sizeof(geBoolean), 1, SaveFD);
		WRITEDATA(&pTheEntity->active, sizeof(geBoolean), 1, SaveFD);
		WRITEDATA(&pTheEntity->bState, sizeof(geBoolean), 1, SaveFD);
		WRITEDATA(&pTheEntity->CallBack, sizeof(geBoolean), 1, SaveFD);
		WRITEDATA(&pTheEntity->Tick, sizeof(geFloat), 1, SaveFD);
		WRITEDATA(&pTheEntity->MaxNumber, sizeof(int), 1, SaveFD);
		if(pTheEntity->alive)
		{
			Bot_Var *DBot = (Bot_Var *)pTheEntity->DBot;
			geVec3d Pos, Rotation;
			CCD->ActorManager()->GetPosition(DBot->Actor, &Pos);
			CCD->ActorManager()->GetRotate(DBot->Actor, &Rotation);
			WRITEDATA(&Pos, sizeof(geVec3d), 1, SaveFD);
			WRITEDATA(&Rotation, sizeof(geVec3d), 1, SaveFD);
			WRITEDATA(&DBot->Velocity, sizeof(geVec3d), 1, SaveFD);
			WRITEDATA(&DBot->Stopped, sizeof(geBoolean), 1, SaveFD);
			WRITEDATA(&DBot->LastStopped, sizeof(geBoolean), 1, SaveFD);
			WRITEDATA(DBot->Animation, sizeof(char), 128, SaveFD);
			WRITEDATA(DBot->LastAnimation, sizeof(char), 128, SaveFD);
			WRITEDATA(&DBot->Dir, sizeof(int), 1, SaveFD);
			WRITEDATA(&DBot->TgtPos, sizeof(geVec3d), 1, SaveFD);
			WRITEDATA(&DBot->MoveVec, sizeof(geVec3d), 1, SaveFD);
			WRITEDATA(&DBot->NextWeaponTime, sizeof(float), 1, SaveFD);
			WRITEDATA(&DBot->Rank, sizeof(int), 1, SaveFD);
			WRITEDATA(&DBot->Active, sizeof(bool), 1, SaveFD);
			WRITEDATA(&DBot->Dying, sizeof(bool), 1, SaveFD);
			WRITEDATA(&DBot->DyingTime, sizeof(float), 1, SaveFD);
			WRITEDATA(&DBot->Attacking, sizeof(bool), 1, SaveFD);
			WRITEDATA(&DBot->Wander, sizeof(bool), 1, SaveFD);
			int value = DBot->theInv->Value(DBot->Attribute);
			WRITEDATA(&value, sizeof(int), 1, SaveFD);
		}

	}
	return RGF_SUCCESS;
}

int CNPC::RestoreFrom(FILE *RestoreFD, bool type)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, check to see if there are NonPlayerCharacter in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "NonPlayerCharacter");
	
	if(!pSet) 
		return RGF_NOT_FOUND;
	
	//	Ok, we have NonPlayerCharacter.  Dig through 'em all.
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		NonPlayerCharacter *pTheEntity = (NonPlayerCharacter*)geEntity_GetUserData(pEntity);
		READDATA(&pTheEntity->alive, sizeof(geBoolean), 1, RestoreFD);
		READDATA(&pTheEntity->active, sizeof(geBoolean), 1, RestoreFD);
		READDATA(&pTheEntity->bState, sizeof(geBoolean), 1, RestoreFD);
		READDATA(&pTheEntity->CallBack, sizeof(geBoolean), 1, RestoreFD);
		READDATA(&pTheEntity->Tick, sizeof(geFloat), 1, RestoreFD);
		READDATA(&pTheEntity->MaxNumber, sizeof(int), 1, RestoreFD);
		if(pTheEntity->alive)
		{
			geVec3d Pos, Rotation;
			Spawn(pTheEntity);
			Bot_Var *DBot = (Bot_Var *)pTheEntity->DBot;
			READDATA(&Pos, sizeof(geVec3d), 1, RestoreFD);
			READDATA(&Rotation, sizeof(geVec3d), 1, RestoreFD);
			CCD->ActorManager()->Position(DBot->Actor, Pos);
			CCD->ActorManager()->Rotate(DBot->Actor, Rotation);
			READDATA(&DBot->Velocity, sizeof(geVec3d), 1, RestoreFD);
			READDATA(&DBot->Stopped, sizeof(geBoolean), 1, RestoreFD);
			READDATA(&DBot->LastStopped, sizeof(geBoolean), 1, RestoreFD);
			READDATA(DBot->Animation, sizeof(char), 128, RestoreFD);
			CCD->ActorManager()->SetMotion(DBot->Actor, DBot->Animation);
			READDATA(DBot->LastAnimation, sizeof(char), 128, RestoreFD);
			READDATA(&DBot->Dir, sizeof(int), 1, RestoreFD);
			READDATA(&DBot->TgtPos, sizeof(geVec3d), 1, RestoreFD);
			READDATA(&DBot->MoveVec, sizeof(geVec3d), 1, RestoreFD);
			READDATA(&DBot->NextWeaponTime, sizeof(float), 1, RestoreFD);
			READDATA(&DBot->Rank, sizeof(int), 1, RestoreFD);
			READDATA(&DBot->Active, sizeof(bool), 1, RestoreFD);
			READDATA(&DBot->Dying, sizeof(bool), 1, RestoreFD);
			READDATA(&DBot->DyingTime, sizeof(float), 1, RestoreFD);
			READDATA(&DBot->Attacking, sizeof(bool), 1, RestoreFD);
			READDATA(&DBot->Wander, sizeof(bool), 1, RestoreFD);
			int value;
			READDATA(&value, sizeof(int), 1, RestoreFD);
			DBot->theInv->Set(DBot->Attribute, value);
		}

	}
	return RGF_SUCCESS;
}
// end multiplayer

//=====================================================================================
//	NPC_ClearTrack - clearing a track should clear ALL including stack
//=====================================================================================
geBoolean NPC_ClearTrack(Bot_Var *DBot)
{
	CCD->Track()->Track_ClearTrack(&DBot->TrackInfoPrev);
	CCD->Track()->Track_ClearTrack(&DBot->TrackInfo);
    StackReset(&DBot->TrackStack);
	
	return GE_TRUE;
}

//=====================================================================================
//=====================================================================================
//
//	NPC_Control
//
//=====================================================================================
//=====================================================================================
geBoolean NPC_Control(Bot_Var *DBot, float dwTicks)
{
	geBoolean CanSee;

	DBot->ModeTimeOut += (dwTicks/1000.0f);
	DBot->TimeSinceTrack += (dwTicks/1000.0f);
	DBot->TimeOut -= (dwTicks/1000.0f);
	DBot->PickTimeout -= (dwTicks/1000.0f);
	DBot->NextWeaponTime -= (dwTicks/1000.0f);
	DBot->AimDelay += (dwTicks/1000.0f);

	if(DBot->Dying)
		return GE_TRUE;

	if(!DBot->Active)
	{
		if(!EffectC_IsStringNull(DBot->Trigger))
		{
			if(GetTriggerState(DBot->Trigger))
			{
				DBot->Active = true;
				DBot->Wander = false;
			}
		}
		else
		{
			DBot->Active = true;
			DBot->Wander = false;
		}
	}

	NPC_TargetTest(DBot, dwTicks);
	CanSee = CanSeeActorToActor(DBot->Actor, DBot->TgtPlayer);
	if (CanSee)
	{
		DBot->TimeNotSeen = 0.0f;
		DBot->TimeSeen += (dwTicks/1000.0f);
		DBot->ModeTimeSeen += (dwTicks/1000.0f);
		DBot->ActiveTime = TIMETODEACTIVATE;
	}
	else
	{
		DBot->TimeSeen = 0.0f;
		DBot->TimeNotSeen += (dwTicks/1000.0f);
		DBot->ModeTimeNotSeen += (dwTicks/1000.0f);
		if (DBot->ActiveTime <= 0.0f)
			return GE_TRUE;
		DBot->ActiveTime -= (dwTicks/1000.0f);
	}
	if(!CCD->Track()->Track_OnTrack(&DBot->TrackInfo) && !(DBot->Mode == MODE_GOAL_POINT))
		CCD->ActorManager()->GetPosition(DBot->TgtPlayer, &DBot->GoalPos); 

	if(DBot->AimDelay>=DBot->AimSkill)
	{
		geExtBox theBox;
		DBot->AimDelay = 0.0f;
		CCD->ActorManager()->GetPosition(DBot->TgtPlayer, &DBot->AimPos);
		CCD->ActorManager()->GetBoundingBox(DBot->TgtPlayer, &theBox);
		DBot->AimPos.Y += (theBox.Max.Y*0.75f);
	}

	if(DBot->Action)
		DBot->Action((void *)DBot, dwTicks);
	if (ModeAction[DBot->Mode])
		ModeAction[DBot->Mode]((void *)DBot, dwTicks);
	
	return GE_TRUE;
}

geActor *NPC_PickTgtPlayer(Bot_Var *DBot, geBoolean ForcePick)
{
	return CCD->Player()->GetActor();
}

geBoolean NPC_TargetTest(Bot_Var *DBot, float dwTicks)
{
	geActor *NewTgtPlayer;
	int Type;
	
	if(DBot->PickTimeout<=0.0f || !DBot->TgtPlayer)
	{
		NewTgtPlayer = NPC_PickTgtPlayer(DBot, GE_FALSE);
		CCD->ActorManager()->GetType(NewTgtPlayer, &Type);
		if(Type==ENTITY_NPC)
		{
			DBot->BotTgtPicked++;
			DBot->PickTimeout = BOTPICKTIME;
		}
		else
		{
			DBot->BotTgtPicked = 0;
			DBot->PickTimeout = PLAYERPICKTIME;
		}
		if (++DBot->BotTgtPicked > 2)
		{
			// Bot was picked too many times - force human player
			DBot->BotTgtPicked = 0;
			NewTgtPlayer = NPC_PickTgtPlayer(DBot, GE_TRUE);
			DBot->PickTimeout = PLAYERPICKTIME + 2.0f;
		}
		if (NewTgtPlayer != DBot->TgtPlayer)	//changed targets
		{
			DBot->TgtPlayer = NewTgtPlayer;
			DBot->TimeNotSeen = 0.0f;
			DBot->TimeSeen = 0.0f;
		}
	}
	return GE_TRUE;
}

//=====================================================================================
//	NPC_ModeThink - Run through the mode code
//=====================================================================================
geBoolean NPC_ModeThink(void *Data, float dwTicks)
{
	Bot_Var	*DBot;
	
	DBot = (Bot_Var *)Data;
	
	if(DBot->Mode != MODE_NULL)
		ModeThinkFunc[DBot->Mode]((void *)DBot, dwTicks);
	
    return GE_TRUE;
}

void NPC_SetMode(void *Data, int32 NewMode)
{
	Bot_Var	*DBot;
	
	DBot = (Bot_Var *)Data;
	
	switch (NewMode)
	{
	case MODE_RETREAT:
	case MODE_RETREAT_ON_TRACK:
		DBot->HealthCheck = DBot->theInv->Value(DBot->Attribute);
		DBot->FaceTgtPlayerOnRetreat = GE_FALSE;
		break;
	}
	
	DBot->Mode = NewMode;
}

//=====================================================================================
//	ChooseAction
//=====================================================================================
void *ChooseAction(DECISION decision[])
{
    int32 i, random_value;
    
    random_value = RandomRange(1000);
	
    for (i = 0; TRUE; i++)
	{
        if (random_value <= decision[i].range)
		{
            return (decision[i].action);
		}
	}
}

//=====================================================================================
//	NPC_ModeChange - This is the only place allowed to set modes
//=====================================================================================
geBoolean NPC_ModeChange(void *Data, int32 NewMode, geBoolean Think, float Time)
{
	Bot_Var	*DBot;
	int32	*iptr;
	int		Rank;
	
	DBot = (Bot_Var *)Data;
	
	// Reset Mode specific timers & counters
	DBot->ModeTimeSeen = 0.0f;
	DBot->ModeTimeNotSeen = 0.0f;
	DBot->ModeTimeOut = 0.0f;
	
	Rank = NPC_Rank(DBot);
	
	Think = false;

	// Set specific mode
	if (NewMode != MODE_NULL)
	{
		NPC_SetMode((void *)DBot, NewMode);
		if (Think)
			NPC_ModeThink((void *)DBot, Time);
		return GE_TRUE;
	}
	
	// CAN NOT SEE
	if (DBot->TimeNotSeen > TIMENOTSEEN)
	{
		if (RandomRange(1000) < 500)
			NPC_SetMode((void *)DBot, MODE_FIND_PLAYER_QUICK);
		else
			NPC_SetMode((void *)DBot, MODE_WANDER);
		if (Think)
			NPC_ModeThink((void *)DBot, Time);
		return GE_TRUE;
	}
	
	if (DBot->TimeNotSeen > (TIMENOTSEEN/2.0f))
	{
		DECISION *dptr;
		
		static DECISION d[] =
		{
			{200,  &ModeTable[MODE_FIND_PLAYER]},
			{500,  &ModeTable[MODE_FIND_PLAYER_QUICK]},
			{1000, &ModeTable[MODE_WANDER]},
		};
		
		static DECISION d2[] =
		{
			{200,  &ModeTable[MODE_FIND_PLAYER]},
			{900,  &ModeTable[MODE_FIND_PLAYER_QUICK]},
			{1000, &ModeTable[MODE_WANDER]},
		};

		if (Rank >= 8)
			dptr = d2;
		else
			dptr = d;

		iptr = (int32 *)ChooseAction(dptr);
		NPC_SetMode((void *)DBot, *iptr);
		if (Think)
			NPC_ModeThink((void *)DBot, Time);
		return GE_TRUE;
	}
	
	if (DBot->TimeNotSeen > 0.0f)
	{
		static DECISION d[] =
		{
			{600,  &ModeTable[MODE_FIND_PLAYER]},
			{1000,  &ModeTable[MODE_FIND_PLAYER_QUICK]},
		};
		
		iptr = (int32 *)ChooseAction(d);
		NPC_SetMode((void *)DBot, *iptr);
		if (Think)
			NPC_ModeThink((void *)DBot, Time);
		return GE_TRUE;
	}
	
	geVec3d Pos, TgtPos;
	CCD->ActorManager()->GetPosition(DBot->Actor, &Pos);
	CCD->ActorManager()->GetPosition(DBot->TgtPlayer, &TgtPos);
	if(geVec3d_DistanceBetween(&TgtPos, &Pos)>MED_RANGE_DIST)
	{
		NPC_SetMode((void *)DBot, MODE_WANDER);
		if (Think)
			NPC_ModeThink((void *)DBot, Time);
		return GE_TRUE;
	}

	// CANSEE
	if (DBot->TimeSeen > TIMESEEN)
	{
		if (Rank <= RandomRange(5))
			NPC_SetMode((void *)DBot, MODE_RETREAT);
		else
			NPC_SetMode((void *)DBot, MODE_ATTACK);
		if (Think)
			NPC_ModeThink((void *)DBot, Time);
		return GE_TRUE;
	}
	if (DBot->TimeSeen > (TIMESEEN/3.0f))
	{
		if (Rank <= RandomRange(4))
			NPC_SetMode((void *)DBot, MODE_RETREAT);
		else
			NPC_SetMode((void *)DBot, MODE_ATTACK);
		if (Think)
			NPC_ModeThink((void *)DBot, Time);
		return GE_TRUE;
	}
	if (DBot->TimeSeen > 0.0f && !DBot->Wander)
	{
		if (Rank <= RandomRange(2))
			NPC_SetMode((void *)DBot, MODE_RETREAT);
		else
			NPC_SetMode((void *)DBot, MODE_ATTACK);
		if (Think)
			NPC_ModeThink((void *)DBot, Time);
		return GE_TRUE;
	}

	return GE_FALSE;
}

//=====================================================================================
//	NPC_ModeThinkRetreat
//=====================================================================================
geBoolean NPC_ModeThinkRetreat(void *Data, float Time)
{
	Bot_Var	*DBot;
	geActor *Goal;
	geVec3d thePosition;
	
	DBot = (Bot_Var *)Data;
	
	DBot->HealthCheck = DBot->theInv->Value(DBot->Attribute);
	
	if (NPC_ActionGetHealth((void *)DBot, 500.0f, Time))
		return GE_TRUE;
	if (NPC_ActionGetWeapon((void *)DBot, 500.0f, Time))
		return GE_TRUE;
	
	// first try and find some item to run towards
	
	CCD->ActorManager()->GetPosition(DBot->Actor, &thePosition);
	if (Goal = NPC_FindRandomItem(&thePosition, ItemHealthList))
	{
		CCD->ActorManager()->GetPosition(Goal, &thePosition);
		if (NPC_InitFindMultiTrack((void *)DBot, &thePosition, Time))
		{
			NPC_ModeChange((void *)DBot, MODE_RETREAT, GE_FALSE, Time);
			return GE_TRUE;
		}
	}
	
	CCD->ActorManager()->GetPosition(DBot->TgtPlayer, &thePosition);
	if (NPC_InitFindMultiTrackAway((void *)DBot, &thePosition, Time))
	{
		NPC_ModeChange((void *)DBot, MODE_RETREAT_ON_TRACK, GE_FALSE, Time);
		return GE_TRUE;
	}
	
	// ModeTimeOut on this operation
	if (DBot->ModeTimeOut > MODETIMEOUT)
	{
		NPC_ModeChange((void *)DBot, MODE_NULL, GE_TRUE, Time);
        return GE_TRUE;
	}
	// cant get away
	if (DBot->ModeTimeSeen > MODETIMESEEN)
	{
		NPC_ModeChange((void *)DBot, MODE_ATTACK, GE_TRUE, Time);
        return GE_TRUE;
	}	
	
	if (DBot->ModeTimeNotSeen > MODETIMENOTSEEN)
	{
		NPC_ModeChange((void *)DBot, MODE_NULL, GE_TRUE, Time);
		return GE_TRUE;
	}	
	if (DBot->ModeTimeNotSeen > (MODETIMENOTSEEN/2.0f))
	{
		if (NPC_InitHide((void *)DBot, Time))
			return GE_TRUE;
	}	
	
	// this sets the Action
	NPC_InitRunAway((void *)DBot, Time);
	
	return GE_TRUE;
}

//=====================================================================================
//	NPC_ModeActionRetreat
//=====================================================================================
geBoolean NPC_ModeActionRetreat(void *Data, float Time)
{
	Bot_Var	*DBot;
	
	DBot = (Bot_Var *)Data;
	
	// if hit when retreating - stand and fight like a (wo)man
	if (DBot->HealthCheck < DBot->theInv->Value(DBot->Attribute) + 20)
	{
		DBot->FaceTgtPlayerOnRetreat = GE_TRUE;
		return GE_TRUE;
	}
	
	return GE_TRUE;
}

//=====================================================================================
//	NPC_ModeThinkFindPlayerQuick
//=====================================================================================
geBoolean NPC_ModeThinkFindPlayerQuick(void *Data, float Time)
{
	Bot_Var	*DBot;
	geVec3d thePosition;
	
	DBot = (Bot_Var *)Data;
	
	if(DBot->ModeTimeOut > (MODETIMEOUT*2.0f))
	{
		NPC_ModeChange((void *)DBot, MODE_NULL, GE_TRUE, Time);
        return GE_TRUE;
	}
	
	CCD->ActorManager()->GetPosition(DBot->TgtPlayer, &thePosition);
	if (NPC_InitFindMultiTrackAway((void *)DBot, &thePosition, Time))
	{
		NPC_ModeChange((void *)DBot, MODE_ON_TRACK, GE_FALSE, Time);
		return GE_TRUE;
	}
	else
	{
		geActor *Goal;
		if (Goal = NPC_FindRandomItem(&thePosition, ItemHealthList))
		{
			CCD->ActorManager()->GetPosition(Goal, &thePosition);
			if (NPC_InitFindMultiTrack((void *)DBot, &thePosition, Time))
			{
				NPC_ModeChange((void *)DBot, MODE_WANDER_ON_TRACK, GE_FALSE, Time);
				return GE_TRUE;
			}
		}
	}
	
	if (NPC_ActionGetHealth((void *)DBot, 400.0f, Time))
		return GE_TRUE;
	if (NPC_ActionGetWeapon((void *)DBot, 400.0f, Time))
		return GE_TRUE;
	
	if (NPC_InitHide((void *)DBot, Time))
		return GE_TRUE;
	
	// Found Player
	if (DBot->ModeTimeSeen > 1.0f)
	{
		NPC_ModeChange((void *)DBot, MODE_NULL, GE_TRUE, Time);
		return GE_TRUE;
	}
	
	NPC_InitRunCloser((void *)DBot, Time);
	
	return GE_TRUE;
}

//=====================================================================================
//	NPC_ModeThinkFindPlayer
//=====================================================================================
geBoolean NPC_ModeThinkFindPlayer(void *Data, float Time)
{
	Bot_Var	*DBot;
	
	DBot = (Bot_Var *)Data;
	
	DBot->WeaponDiff = NPC_CompareWeapons(DBot->Actor, DBot->TgtPlayer);
	
	// outgunned
	if (DBot->WeaponDiff < -40)
	{
		NPC_ModeChange((void *)DBot, MODE_WANDER, GE_FALSE, Time);
		return GE_TRUE;
	}
	
	if (DBot->ModeTimeOut > (MODETIMEOUT/2.0f))
	{
		NPC_ModeChange((void *)DBot, MODE_FIND_PLAYER_QUICK, GE_TRUE, Time);
        return GE_TRUE;
	}
	
	if (NPC_ActionGetHealth((void *)DBot, 600.0f, Time))
		return GE_TRUE;
	if (NPC_ActionGetWeapon((void *)DBot, 600.0f, Time))
		return GE_TRUE;
	
	// Found Player
	if (DBot->ModeTimeSeen > 0.0f)
	{
		NPC_ModeChange((void *)DBot, MODE_NULL, GE_TRUE, Time);
		return GE_TRUE;
	}
	
	if (DBot->ModeTimeNotSeen > (MODETIMENOTSEEN/2.0f))
	{
		if (NPC_InitHide((void *)DBot, Time))
			return GE_TRUE;
	}
	
	NPC_InitRunCloser((void *)DBot, Time);
	
	return GE_TRUE;
}

//=====================================================================================
//	NPC_ModeThinkWalk
//=====================================================================================
geBoolean NPC_ModeThinkWalk(void *Data, float Time)
{
	Bot_Var	*DBot;
	Track	*GoalTrack;
	geVec3d thePosition;
	
	DBot = (Bot_Var *)Data;

	if(!DBot->Wander)
	{
		NPC_ModeChange((void *)DBot, MODE_FIND_PLAYER, GE_FALSE, Time);
		return GE_TRUE;
	}
	
	CCD->ActorManager()->GetPosition(DBot->Actor, &thePosition);
	if (GoalTrack = NPC_FindRandomTrack(&thePosition, WalkTrackList))
	{
		if (NPC_InitFindMultiTrack((void *)DBot, GoalTrack->PointList[0].Pos, Time))
		{
			NPC_ModeChange((void *)DBot, MODE_WALK_ON_TRACK, GE_FALSE, Time);
			return GE_TRUE;
		}
	}

	return GE_TRUE;
}

//=====================================================================================
//	NPC_ModeThinkWalkOnTrack
//=====================================================================================
geBoolean NPC_ModeThinkWalkOnTrack(void *Data, float Time)
{
	Bot_Var	*DBot;
	
	DBot = (Bot_Var *)Data;
	
	// left the track?
	if (!CCD->Track()->Track_OnTrack(&DBot->TrackInfo))
	{
		NPC_ModeChange((void *)DBot, MODE_WALK, GE_TRUE, Time);
		return GE_TRUE;
	}
	
	// IF HERE THEN HIT SOMETHING!!!!! - if didn't hit then check logic!
	if (CCD->Track()->Track_OnTrack(&DBot->TrackInfo))
	{
		// This MAY push a track on the stack
		NPC_InitRunCloser((void *)DBot, Time);
		DBot->GoalPos = *CCD->Track()->Track_GetPoint(&DBot->TrackInfo)->Pos;
		return GE_TRUE;
	}
	
	return GE_TRUE;
}

//=====================================================================================
//	NPC_ModeThinkWander
//=====================================================================================
geBoolean NPC_ModeThinkWander(void *Data, float Time)
{
	geActor *Goal;
	Bot_Var	*DBot;
	Track	*GoalTrack;
	geVec3d thePosition;
	
	DBot = (Bot_Var *)Data;
	
	CCD->ActorManager()->GetPosition(DBot->Actor, &thePosition);
	if (GoalTrack = NPC_FindRandomTrack(&thePosition, HideTrackList))
	{
		if (NPC_InitFindMultiTrack((void *)DBot, GoalTrack->PointList[0].Pos, Time))
		{
			NPC_ModeChange((void *)DBot, MODE_ON_TRACK, GE_FALSE, Time);
			return GE_TRUE;
		}
	}
	
	// InitWanderGoal
	if (Goal = NPC_FindRandomItem(&thePosition, ItemHealthList))
	{
		CCD->ActorManager()->GetPosition(Goal, &thePosition);
		if (NPC_InitFindMultiTrack((void *)DBot, &thePosition, Time))
		{
			NPC_ModeChange((void *)DBot, MODE_WANDER_ON_TRACK, GE_FALSE, Time);
			return GE_TRUE;
		}
	}
	
	if (NPC_ActionGetHealth((void *)DBot, 1200.0f, Time))
		return GE_TRUE;
	if (NPC_ActionGetWeapon((void *)DBot, 1200.0f, Time))
		return GE_TRUE;
	
	NPC_ModeChange((void *)DBot, MODE_FIND_PLAYER, GE_FALSE, Time);

	return GE_TRUE;
}

//=====================================================================================
//	NPC_ModeThinkWanderGoal
//=====================================================================================
geBoolean NPC_ModeThinkWanderGoal(void *Data, float Time)
{
	Bot_Var	*DBot;
	
	DBot = (Bot_Var *)Data;
	
	if (!CCD->Track()->Track_OnTrack(&DBot->TrackInfo))
	{
		// Try and find an item
		if (NPC_ActionGetHealth((void *)DBot, 500.0f, Time))
			return GE_TRUE;
		if (NPC_ActionGetWeapon((void *)DBot, 500.0f, Time))
			return GE_TRUE;

		NPC_ModeChange((void *)DBot, MODE_FIND_PLAYER, GE_FALSE, Time);
	}
	else
	{
		// IF HERE THEN HIT SOMETHING!!!!! - if didn't hit then check logic!
		if (CCD->Track()->Track_OnTrack(&DBot->TrackInfo))
		{
			// This MAY push a track on the stack
			NPC_InitRunCloser((void *)DBot, Time);
			DBot->GoalPos = *CCD->Track()->Track_GetPoint(&DBot->TrackInfo)->Pos;
			return GE_TRUE;
		}
	}
	
	NPC_InitRunCloser((void *)DBot, Time);
	
	return GE_TRUE;
}

//=====================================================================================
//	NPC_ModeThinkAttack
//=====================================================================================
geBoolean NPC_ModeThinkAttack(void *Data, float Time)
{
	Bot_Var	*DBot;
	geVec3d	Pos,TgtPlayerPos;
	float	Dist;
	int Rank;
	
	DBot = (Bot_Var *)Data;
	
	Rank = NPC_Rank(DBot);

	DBot->WeaponDiff = NPC_CompareWeapons(DBot->Actor, DBot->TgtPlayer);
	
	CCD->ActorManager()->GetPosition(DBot->Actor, &Pos);
	CCD->ActorManager()->GetPosition(DBot->TgtPlayer, &TgtPlayerPos);
	if(geVec3d_DistanceBetween(&TgtPlayerPos, &Pos)>MED_RANGE_DIST)
	{
		NPC_SetMode((void *)DBot, MODE_WANDER);
		//NPC_ModeThink((void *)DBot, Time);
		return GE_TRUE;
	}

	// outgunned
	if (Rank < (DBot->Rank/2))
	{
		NPC_ModeChange((void *)DBot, MODE_RETREAT, GE_FALSE, Time);
		return GE_TRUE;
	}

	if(DBot->ModeTimeNotSeen > (MODETIMENOTSEEN/4.0f))
	{
		NPC_ModeChange((void *)DBot, MODE_FIND_PLAYER, GE_TRUE, Time);
        return GE_TRUE;
	}
	
    if (PlayerDead(DBot->TgtPlayer))
    {
		if (NPC_InitFindMultiTrackAway((void *)DBot, &TgtPlayerPos, Time))
		{
			NPC_ModeChange((void *)DBot, MODE_RETREAT_ON_TRACK, GE_FALSE, Time);
			return GE_TRUE;
        }
        else
        {
			NPC_ModeChange((void *)DBot, MODE_RETREAT, GE_TRUE, Time);
			return GE_TRUE;
        }
    }
	
    if (RandomRange(1000) < 500 && Rank >= 4)
	{
		NPC_ModeChange((void *)DBot, MODE_CROWD_PLAYER, GE_TRUE, Time);
        return GE_TRUE;
	}
	
	if (DBot->ModeTimeOut > MODETIMEOUT)// || DBot->ModeTimeSeen > MODETIMESEEN)
	{
		NPC_ModeChange((void *)DBot, MODE_NULL, GE_TRUE, Time);
        return GE_TRUE;
	} 
	
	if (NPC_ActionGetHealth((void *)DBot, 600.0f, Time))
		return GE_TRUE;
	if (NPC_ActionGetWeapon((void *)DBot, 600.0f, Time))
		return GE_TRUE;
	
	// keep a medium range distance
    Dist = geVec3d_DistanceBetween(&Pos, &TgtPlayerPos);
    if (Dist <= CLOSE_RANGE_DIST)
	{
		if(DBot->Melee)
			NPC_ModeChange((void *)DBot, MODE_CROWD_PLAYER, GE_TRUE, Time);
		else
			NPC_InitMaintain((void *)DBot, Time);
		return GE_TRUE;
	}
	else
	{
		if (Dist <= MED_RANGE_DIST)
		{
			if (RandomRange(1000) < 200)
				NPC_InitRunCloser((void *)DBot, Time);
			else
				NPC_InitMaintain((void *)DBot, Time);
			return GE_TRUE;
		}
		else
		{
			NPC_InitRunCloser((void *)DBot, Time);
			return GE_TRUE;
		}
	}
	
	// health assessment
	CPersistentAttributes *Inv = CCD->ActorManager()->Inventory(DBot->TgtPlayer);
    DBot->HealthDiff = DBot->theInv->Value(DBot->Attribute) - Inv->Value(DBot->Attribute);
	DBot->WeaponDiff = NPC_CompareWeapons(DBot->Actor, DBot->TgtPlayer);
	
	return GE_TRUE;
}

//=====================================================================================
//	NPC_ModeThinkCrowdPlayer
//=====================================================================================
geBoolean NPC_ModeThinkCrowdPlayer(void *Data, float Time)
{
	Bot_Var	*DBot;
	int Rank;
	geVec3d thePosition;
	
	DBot = (Bot_Var *)Data;
	
	Rank = NPC_Rank(DBot);
	DBot->WeaponDiff = NPC_CompareWeapons(DBot->Actor, DBot->TgtPlayer);
	
	// outgunned
	if (DBot->WeaponDiff < -40)
	{
		NPC_ModeChange((void *)DBot, MODE_RETREAT, GE_FALSE, Time);
		return GE_TRUE;
	}
	
    if (PlayerDead(DBot->TgtPlayer))
    {
		CCD->ActorManager()->GetPosition(DBot->TgtPlayer, &thePosition);
		if (NPC_InitFindMultiTrackAway((void *)DBot, &thePosition, Time))
		{
			NPC_ModeChange((void *)DBot, MODE_RETREAT_ON_TRACK, GE_FALSE, Time);
			return GE_TRUE;
        }
        else
        {
			NPC_ModeChange((void *)DBot, MODE_RETREAT, GE_TRUE, Time);
			return GE_TRUE;
        }
    }
	
	if (DBot->ModeTimeOut > (MODETIMEOUT/2.0f) || DBot->ModeTimeNotSeen > MODETIMENOTSEEN)
	{
		NPC_ModeChange((void *)DBot, MODE_NULL, GE_TRUE, Time);
        return GE_TRUE;
	}
	
	if (NPC_ActionGetHealth((void *)DBot, 400.0f, Time))
		return GE_TRUE;
	if (NPC_ActionGetWeapon((void *)DBot, 400.0f, Time))
		return GE_TRUE;
	
    if (Rank <= 4)
	{
		NPC_ModeChange((void *)DBot, MODE_NULL, GE_TRUE, Time);
        return GE_TRUE;
	}
	
    // crowd the player at all times
	NPC_InitRunCloser((void *)DBot, Time);
	
	return GE_TRUE;
}

//=====================================================================================
//	NPC_ModeThinkGoalPoint
//=====================================================================================
geBoolean NPC_ModeThinkGoalPoint(void *Data, float Time)
{
	Bot_Var	*DBot;
	
	DBot = (Bot_Var *)Data;
	
	// either hit something or made it to the point
	
	NPC_ModeChange((void *)DBot, MODE_NULL, GE_TRUE, Time);
	
	return GE_TRUE;
}

//=====================================================================================
//	NPC_ModeThinkOnTrack
//=====================================================================================
geBoolean NPC_ModeThinkOnTrack(void *Data, float Time)
{
	Bot_Var	*DBot;
	
	DBot = (Bot_Var *)Data;
	
	// left the track?
	if (!CCD->Track()->Track_OnTrack(&DBot->TrackInfo))
	{
		NPC_ModeChange((void *)DBot, MODE_NULL, GE_TRUE, Time);
		return GE_TRUE;
	}
	
	// IF HERE THEN HIT SOMETHING!!!!! - if didn't hit then check logic!
	if (CCD->Track()->Track_OnTrack(&DBot->TrackInfo))
	{
		// This MAY push a track on the stack
		NPC_InitRunCloser((void *)DBot, Time);
		DBot->GoalPos = *CCD->Track()->Track_GetPoint(&DBot->TrackInfo)->Pos;
		return GE_TRUE;
	}
	
	return GE_TRUE;
}

//=====================================================================================
//	NPC_ModeThinkUnstick
//=====================================================================================
geBoolean NPC_ModeThinkUnstick(void *Data, float Time)
{
	Bot_Var	*DBot;
	int32	dir;
	geVec3d thePosition;
	
	DBot = (Bot_Var *)Data;
	
	if (DBot->ModeTimeOut > (MODETIMEOUT/5.0f))
	{
		if(!DBot->Wander)
		{
			NPC_ModeChange((void *)DBot, MODE_NULL, GE_TRUE, Time);
			return GE_TRUE;
		}
	}
	
	dir = RandomRange(3) - 1;
	CCD->ActorManager()->GetPosition(DBot->TgtPlayer, &thePosition);
	NPC_Reposition((void *)DBot, &thePosition, dir);
	DBot->Action = NPC_MoveFree;

	return GE_TRUE;
}

//=====================================================================================
//	NPC_Reposition
//=====================================================================================
geBoolean NPC_Reposition(void *Data, geVec3d *TgtPos, int32 Dir)
{
	Bot_Var	*DBot;
	geVec3d			Vec2Player;
	
	DBot = (Bot_Var *)Data;
	
	geVec3d_Subtract(TgtPos, &DBot->GoalPos, &Vec2Player);
	
	// TgtPos is where we want to move to
	if (!NPC_FindNewMoveVec((void *)DBot, TgtPos, Dir, MINMOVEDIST, &DBot->MoveVec))
	{
		// Couldn't find a dir to move
		if (!NPC_FindNewMoveVec((void *)DBot, TgtPos, -Dir, MINMOVEDIST, &DBot->MoveVec))
			return GE_FALSE;
	}
	
    return GE_TRUE;
}

//=====================================================================================
//	NPC_InitFindMultiTrack
//=====================================================================================
geBoolean NPC_InitFindMultiTrack(void *Data, geVec3d *DestPos, float Time)
{
	Bot_Var	*DBot;
	TrackCB	CB;
	geVec3d thePosition;
	
	DBot = (Bot_Var *)Data;
	
	if (!StackIsEmpty(&DBot->TrackStack))
		return GE_FALSE;
	
	CB.Data = Data;
	CB.CB = NPC_ValidateMultiTrackPoints;
	CCD->ActorManager()->GetPosition(DBot->Actor, &thePosition);
	
	if (CCD->Track()->Track_FindMultiTrack (&thePosition,
		DestPos,
		BOT_DIR_TOWARD,
		&CB,
		&DBot->TrackStack))
	{
		DBot->Dir = BOT_DIR_TOWARD;
		CCD->Track()->Track_NextMultiTrack(&thePosition, StackTop(&DBot->TrackStack), &DBot->TrackInfo);
		DBot->PastFirstTrackPoint = GE_FALSE;
		DBot->GoalPos = *CCD->Track()->Track_GetPoint(&DBot->TrackInfo)->Pos;
		NPC_InitMoveToPoint((void *)DBot, &DBot->GoalPos);
		return GE_TRUE;
	}
	
	return GE_FALSE;
}

//=====================================================================================
//	NPC_InitFindMultiTrackAway
//=====================================================================================
geBoolean NPC_InitFindMultiTrackAway(void *Data, geVec3d *DestPos, float Time)
{
	Bot_Var			*DBot;
	TrackCB			CB;
	geVec3d thePosition;

	DBot = (Bot_Var *)Data;

	if (!StackIsEmpty(&DBot->TrackStack))
		return GE_FALSE;

	CB.Data = Data;
	CB.CB = NPC_ValidateMultiTrackPoints;
	CCD->ActorManager()->GetPosition(DBot->Actor, &thePosition);

	if (CCD->Track()->Track_FindMultiTrack(&thePosition,
						DestPos,
                        BOT_DIR_AWAY,
						&CB,
						&DBot->TrackStack))
		{
		DBot->Dir = BOT_DIR_AWAY;

		CCD->Track()->Track_NextMultiTrack(&thePosition, StackTop(&DBot->TrackStack), &DBot->TrackInfo);
		DBot->PastFirstTrackPoint = GE_FALSE;
		DBot->GoalPos = *CCD->Track()->Track_GetPoint(&DBot->TrackInfo)->Pos;
		NPC_InitMoveToPoint((void *)DBot, &DBot->GoalPos);
		return GE_TRUE;
		}

	return GE_FALSE;
}

//=====================================================================================
//	NPC_InitMoveToPoint
//=====================================================================================
geBoolean NPC_InitMoveToPoint(void *Data, geVec3d *Pos)
{
	Bot_Var	*DBot;
	geVec3d thePosition;
	
	DBot = (Bot_Var *)Data;
	
	DBot->TgtPos = *Pos;
	CCD->ActorManager()->GetPosition(DBot->Actor, &thePosition);
	
	geVec3d_Subtract(&DBot->TgtPos, &thePosition, &DBot->MoveVec);
    DBot->MoveVec.Y = 0.0f;
	geVec3d_Normalize(&DBot->MoveVec);
	
	DBot->Action = NPC_MoveToPoint;
	
	return GE_TRUE;
}

//=====================================================================================
//	NPC_InitMoveFree
//=====================================================================================
geBoolean NPC_InitMoveFree(void *Data, geVec3d *Pos)
{
	Bot_Var	*DBot;
	geVec3d thePosition;
	
	DBot = (Bot_Var *)Data;
	
	DBot->TgtPos = *Pos;
	CCD->ActorManager()->GetPosition(DBot->Actor, &thePosition);
	
	geVec3d_Subtract(&DBot->TgtPos, &thePosition, &DBot->MoveVec);
    DBot->MoveVec.Y = 0.0f;
	geVec3d_Normalize(&DBot->MoveVec);
	
	DBot->Action = NPC_MoveFree;
	
	return GE_TRUE;
}

//=====================================================================================
//	NPC_InitGenericMove
//=====================================================================================
geBoolean NPC_InitGenericMove(void *Data, float Time)
{
	Bot_Var	*DBot;
	
	DBot = (Bot_Var *)Data;
	
    // if still on a track
	if (CCD->Track()->Track_OnTrack(&DBot->TrackInfo))
    {
        // continue on the track
		DBot->Action = NPC_MoveToPoint;
        return GE_TRUE;
    }
	
    NPC_Reposition((void *)DBot, &DBot->GoalPos, DBot->Dir);
    DBot->Action = NPC_MoveToPoint;
	
    return GE_TRUE;
}

//=====================================================================================
//	NPC_InitMoveCloser
//=====================================================================================
geBoolean NPC_InitMoveCloser(void *Data, float Time)
{
	Bot_Var	*DBot;
	
	DBot = (Bot_Var *)Data;
	
    DBot->Dir = BOT_DIR_TOWARD;
	
	NPC_Reposition((void *)DBot, &DBot->GoalPos, DBot->Dir);
    DBot->Action = NPC_MoveToPoint;
	
    return GE_TRUE;
}

//=====================================================================================
//	NPC_InitRunCloser
//=====================================================================================
geBoolean NPC_InitRunCloser(void *Data, float Time)
{
	Bot_Var	*DBot;
	
	DBot = (Bot_Var *)Data;
	
    DBot->Dir = BOT_DIR_TOWARD;
	
    if (NPC_InitTrackToGoal((void *)DBot, Time))
        return GE_TRUE;
	
	NPC_Reposition((void *)DBot, &DBot->GoalPos, DBot->Dir);
    DBot->Action = NPC_MoveToPoint;
	
    return GE_TRUE;
}

//=====================================================================================
//	NPC_InitTrackToGoal
//=====================================================================================
geBoolean NPC_InitTrackToGoal(void *Data, float Time)
{
	Bot_Var	*DBot;
	
	DBot = (Bot_Var *)Data;
	
    DBot->Dir = BOT_DIR_TOWARD;
	
    if (NPC_FindTrackToGoal((void *)DBot, Time))
    {
        DBot->GoalPos = *CCD->Track()->Track_GetPoint(&DBot->TrackInfo)->Pos;
        NPC_InitMoveToPoint((void *)DBot, &DBot->GoalPos);
        return GE_TRUE;
    }
	
    return GE_FALSE;
}

//=====================================================================================
//	NPC_InitTrackAwayGoal
//=====================================================================================
geBoolean NPC_InitTrackAwayGoal(void *Data, float Time)
{
	Bot_Var	*DBot;
	
	DBot = (Bot_Var *)Data;
	
    DBot->Dir = BOT_DIR_AWAY;
	
    if (NPC_FindTrackToGoal((void *)DBot, Time))
    {
        DBot->GoalPos = *CCD->Track()->Track_GetPoint(&DBot->TrackInfo)->Pos;
        NPC_InitMoveToPoint((void *)DBot, &DBot->GoalPos);
        return GE_TRUE;
    }
	
    return GE_FALSE;
}

//=====================================================================================
//	NPC_InitHide
//=====================================================================================
geBoolean NPC_InitHide(void *Data, float Time)
{
	Bot_Var *DBot;
	
	DBot = (Bot_Var *)Data;
	
	DBot->Dir = BOT_DIR_NONE;
	
    if (NPC_FindTrack((void *)DBot, HideTrackList))
    {
        DBot->GoalPos = *CCD->Track()->Track_GetPoint(&DBot->TrackInfo)->Pos;
        NPC_InitMoveToPoint((void *)DBot, &DBot->GoalPos);
        NPC_InitMoveToPoint((void *)DBot, CCD->Track()->Track_GetPoint(&DBot->TrackInfo)->Pos);
        return GE_TRUE;
    }
	
    return GE_FALSE;
}

//=====================================================================================
//	NPC_InitMoveAway
//=====================================================================================
geBoolean NPC_InitMoveAway(void *Data, float Time)
{
	Bot_Var	*DBot;
	
	DBot = (Bot_Var *)Data;
	
    DBot->Dir = BOT_DIR_AWAY;
	
    NPC_Reposition((void *)DBot, &DBot->GoalPos, DBot->Dir);
    DBot->Action = NPC_MoveToPoint;
	
    return GE_TRUE;
}

//=====================================================================================
//	NPC_InitRunAway
//=====================================================================================
geBoolean NPC_InitRunAway(void *Data, float Time)
{
	Bot_Var	*DBot;
	geVec3d stop_pos, Pos;
	
	DBot = (Bot_Var *)Data;
	
    DBot->Dir = BOT_DIR_AWAY;
	
    if (NPC_InitTrackAwayGoal((void *)DBot, Time))
        return GE_TRUE;
	
    NPC_Reposition((void *)DBot, &DBot->GoalPos, DBot->Dir);
	NPC_MoveScan((void *)DBot, &DBot->MoveVec, MINMOVEDIST, &stop_pos);
	CCD->ActorManager()->GetPosition(DBot->Actor, &Pos);
	// hit something really close
	if (geVec3d_DistanceBetween(&Pos, &stop_pos) < MINMOVEDIST)
	{
		DBot->Dir = BOT_DIR_TOWARD;
		if (NPC_InitTrackToGoal((void *)DBot, Time))
			return GE_TRUE;
		CCD->ActorManager()->GetPosition(DBot->TgtPlayer, &Pos);
		NPC_Reposition((void *)DBot, &Pos, DBot->Dir);
		DBot->Action = NPC_MoveToPoint;
		NPC_ModeChange((void *)DBot, MODE_ATTACK, GE_TRUE, Time);
		return GE_TRUE;
	}
    DBot->Action = NPC_MoveToPoint;
	
    return GE_TRUE;
}

//=====================================================================================
//	NPC_InitMaintain
//=====================================================================================
geBoolean NPC_InitMaintain(void *Data, float Time)
{
	Bot_Var	*DBot;
	
	DBot = (Bot_Var *)Data;
	
    DBot->Dir = BOT_DIR_NONE;
	
    NPC_Reposition((void *)DBot, &DBot->GoalPos, DBot->Dir);
    DBot->Action = NPC_MoveToPoint;
	
    return GE_TRUE;
}

//=====================================================================================
//	NPC_InitWaitForPlayer
//=====================================================================================
geBoolean NPC_InitWaitForPlayer(void *Data, float Time)
{
	Bot_Var	*DBot;
	
	DBot = (Bot_Var *)Data;
	
	NPC_SetupXForm((void *)DBot, &DBot->MoveVec);
	
	// Stop here
	DBot->Stopped = true;
	
	DBot->Action = NPC_WaitForPlayer;
	
	return GE_TRUE;
}

//=====================================================================================
//	NPC_WaitForPlayer
//=====================================================================================
geBoolean NPC_WaitForPlayer(void *Data, float Time)
{
	Bot_Var	*DBot;
	
	DBot = (Bot_Var *)Data;
	
	NPC_Animate((void *)DBot, Time);
	
	if(DBot->TimeOut<=0.0f)
	{
        NPC_ClearTrack(DBot);
		NPC_InitGenericMove((void *)DBot, Time);
		return GE_TRUE;
	}
	
	if(CanSeeActorToActor(DBot->Actor, DBot->TgtPlayer))
	{
		NPC_InitGenericMove((void *)DBot, Time);
		return GE_TRUE;
	}
	
	return GE_TRUE;
}

//=====================================================================================
//	NPC_InitWaitForEntityVisible
//=====================================================================================
geBoolean NPC_InitWaitForEntityVisible(void *Data, float Time)
{
	Bot_Var	*DBot;
	
	DBot = (Bot_Var *)Data;
	
	NPC_SetupXForm((void *)DBot, &DBot->MoveVec);
	
	DBot->Stopped = true;	// Stop moving
	
	DBot->HealthCheck = DBot->theInv->Value(DBot->Attribute); // save health
	DBot->Action = NPC_WaitForEntityVisible;
	
	return GE_TRUE;
}

//=====================================================================================
//	NPC_WaitForEntityVisible
//=====================================================================================
geBoolean NPC_WaitForEntityVisible(void *Data, float Time)
{
	Bot_Var			*DBot;
    TrackPt         *Tp;
	geBoolean		CanSee;
	geVec3d			Vec2Player;
	geVec3d	Pos,TgtPlayerPos;
	
	DBot = (Bot_Var *)Data;
	
	// MOVEMENT
	NPC_Movement((void *)DBot, Time);

	// vec2player
	CCD->ActorManager()->GetPosition(DBot->Actor, &Pos);
	CCD->ActorManager()->GetPosition(DBot->TgtPlayer, &TgtPlayerPos);
	geVec3d_Subtract(&TgtPlayerPos, &Pos, &Vec2Player);
	NPC_SetupXForm((void *)DBot, &Vec2Player);       //Face Player
	
	// shooting
	if (!PlayerDead(DBot->TgtPlayer) && DBot->TimeSeen > 0.0f)
	{
		if(DBot->Melee)
		{
			if(geVec3d_DistanceBetween(&Pos, &TgtPlayerPos)<=DBot->MeleeDistance)
				NPC_Shoot((void *)DBot, &TgtPlayerPos, Time);
		}
		else
			NPC_Shoot((void *)DBot, &TgtPlayerPos, Time);
	}
	
	NPC_Animate((void *)DBot, Time);
	
    Tp = CCD->Track()->Track_GetPoint(&DBot->TrackInfoPrev);
    assert(Tp->WatchPos);
	
	if (CCD->Track()->Track_GetTrack(&DBot->TrackInfoPrev)->Type == TRACK_TYPE_TRAVERSE_DOOR)
		CanSee = CanSeePointToPoint(&Pos, Tp->WatchPos);
	else
		CanSee = CanSeeActorToPoint(DBot->Actor, Tp->WatchPos);
	
	if (CanSee)
	{
		if (CCD->Track()->Track_OnTrack(&DBot->TrackInfo))
			DBot->Action = NPC_MoveToPoint;
		else
			NPC_InitMoveToPoint((void *)DBot, &DBot->TgtPos);
		return GE_TRUE;
	}

	// could not see point
	if (DBot->TimeOut<=0.0f || DBot->HealthCheck > DBot->theInv->Value(DBot->Attribute))
	{
        NPC_ClearTrack(DBot);
		NPC_InitGenericMove((void *)DBot, Time);
		return GE_TRUE;
	}
	
	return GE_TRUE;
}

//=====================================================================================
//	NPC_InitWaitForEntityDist
//=====================================================================================
geBoolean NPC_InitWaitForEntityDist(void *Data, float Time)
{
	Bot_Var			*DBot;
	
	DBot = (Bot_Var *)Data;
	
	NPC_SetupXForm((void *)DBot, &DBot->MoveVec);
	
	// Stop
	DBot->Stopped = true;
	
	DBot->HealthCheck = DBot->theInv->Value(DBot->Attribute); // save health
	DBot->Action = NPC_WaitForEntityDist;
	
	return GE_TRUE;
}

//=====================================================================================
//	NPC_WaitForEntityDist
//=====================================================================================
geBoolean NPC_WaitForEntityDist(void *Data, float Time)
{
	Bot_Var	*DBot;
    TrackPt *Tp;
	geVec3d	Pos;
	
	DBot = (Bot_Var *)Data;
	
	// MOVEMENT
	NPC_Movement((void *)DBot, Time);

	NPC_Animate((void *)DBot, Time);
	
    Tp = CCD->Track()->Track_GetPoint(&DBot->TrackInfoPrev);
    assert(Tp->WatchPos);
	
	CCD->ActorManager()->GetPosition(DBot->Actor, &Pos);
	if (geVec3d_DistanceBetween(&Pos, Tp->WatchPos))
	{
		if (CCD->Track()->Track_OnTrack(&DBot->TrackInfo))
			DBot->Action = NPC_MoveToPoint;
		else
			NPC_InitMoveToPoint((void *)DBot, &DBot->TgtPos);
		return GE_TRUE;
	}
	
	if (DBot->TimeOut<=0.0f || DBot->HealthCheck > DBot->theInv->Value(DBot->Attribute))
	{
        NPC_ClearTrack(DBot);
		NPC_InitGenericMove((void *)DBot, Time);
		return GE_TRUE;
	}
	
	return GE_TRUE;
}

//=====================================================================================
//	NPC_SetupXForm
//=====================================================================================
geBoolean NPC_SetupXForm(void *Data, geVec3d *OrientVec)
{
	Bot_Var	*DBot;
	geVec3d	InVect;
	geFloat x,l;
	
	DBot = (Bot_Var *)Data;
	
	InVect = *OrientVec;
	InVect.Y = 0.0f; // Make it a 2D vector 
	l = geVec3d_Length(&InVect);
	
	// protect from Div by Zero
	if(l > 0.0f) 
	{
		x = InVect.X;
		InVect.X = (float)( GE_PI*0.5 ) - 
			(float)acos(InVect.Y / l);
		InVect.Y = (float)atan2( x , InVect.Z ) + GE_PI;
		// roll is zero - always!!?
		InVect.Z = 0.0;
	}
	CCD->ActorManager()->Rotate(DBot->Actor, InVect);
	
	return GE_TRUE;
}

//=====================================================================================
//	NPC_Animate
//=====================================================================================
geBoolean NPC_Animate(void *Data, float Time)
{
	Bot_Var			*DBot;
	
	DBot = (Bot_Var *)Data;

	if(DBot->Attacking && DBot->StopToAttack)
	{
		if(stricmp(DBot->Animation,DBot->AnimAttack))
		{
			strcpy(DBot->Animation,DBot->AnimAttack);
			CCD->ActorManager()->SetMotion(DBot->Actor, DBot->AnimAttack);
			CCD->ActorManager()->SetHoldAtEnd(DBot->Actor, true);
		}
		return GE_TRUE;
	}
	if(DBot->Attacking)
	{
		if(stricmp(DBot->Animation,DBot->AnimAttack))
		{
			strcpy(DBot->Animation,DBot->AnimWalkAttack);
			CCD->ActorManager()->SetMotion(DBot->Actor, DBot->AnimWalkAttack);
			CCD->ActorManager()->SetHoldAtEnd(DBot->Actor, true);
		}
		return GE_TRUE;
	}

	if(DBot->Stopped && stricmp(DBot->Animation,DBot->AnimIdle))
	{
		strcpy(DBot->Animation,DBot->AnimIdle);
		CCD->ActorManager()->SetMotion(DBot->Actor, DBot->AnimIdle);
		return GE_TRUE;
	}
	else
	{
		if(stricmp(DBot->Animation,DBot->AnimWalk))
		{
			strcpy(DBot->Animation,DBot->AnimWalk);
			CCD->ActorManager()->SetMotion(DBot->Actor, DBot->AnimWalk);
			return GE_TRUE;
		}
	}
	
	return GE_TRUE;
}

//=====================================================================================
//	NPC_FindItem
//=====================================================================================
geActor *NPC_FindItem(geVec3d *Pos, char *ClassList[], float YDiff, float Range)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	int i;
	geActor *FoundPlayerCanSeeNearDist = NULL;

	pSet = geWorld_GetEntitySet(CCD->World(), "Attribute");
	if(pSet) 
	{
		//	Ok, we have Attributes somewhere.  Dig through 'em all.
		
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
			Attribute *pSource = (Attribute*)geEntity_GetUserData(pEntity);
			if(pSource->active==GE_TRUE)
			{
				if (fabs(Pos->Y - pSource->origin.Y) > YDiff)
					continue;
				for (i=0; ClassList[i]; i++)
				{
					if (!strcmp(ClassList[i], pSource->AttributeName))   
						break;
				}
				if (!ClassList[i])
					continue;
				float Dist = geVec3d_DistanceBetween(Pos, &pSource->origin);
				if (Dist > Range)
					continue;
				if(CanSeePointToPoint(Pos, &pSource->origin))
					FoundPlayerCanSeeNearDist = pSource->Actor;
			}
		}
		if (FoundPlayerCanSeeNearDist)
			return  FoundPlayerCanSeeNearDist;
	}

	return NULL;
}

//=====================================================================================
//	NPC_FindRandomItem
//=====================================================================================
geActor *NPC_FindRandomItem(geVec3d *Pos, char *ClassList[])
{
	return NPC_FindItem(Pos, ItemHealthList, BOT_SEARCH_Y_RANGE, 500.0f);
}

//=====================================================================================
//	NPC_FindRandomTrack
//=====================================================================================
Track *NPC_FindRandomTrack(geVec3d *Pos, int32 *TrackTypeList)
{
	int             i,GoalCount;
	static Track	*GoalList[300];
	Track *t;
	
	GoalCount = 0;
	t = NULL;
	while (1)
	{
		t = CCD->Track()->Track_GetNextTrack(t);
		
		if (!t)
			break;
		
        for (i=0; TrackTypeList[i] >= 0; i++)
        {
			if (t->Type == TrackTypeList[i])
                break;
        }
		
        if (TrackTypeList[i] <= -1)
            continue;
		
		GoalList[GoalCount++] = t;
		if ( GoalCount >= sizeof(GoalList)/sizeof(GoalList[0]) )
			break;
		
	}
	
	if (!GoalCount)
		return NULL;
	
	return(GoalList[RandomRange(GoalCount)]);
}

//=====================================================================================
//	NPC_FindNewMoveVec
//=====================================================================================
geBoolean NPC_FindNewMoveVec(void *Data, const geVec3d *TargetPos, int32 dir, float DistToMove, geVec3d *save_vec)
{
	Bot_Var	*DBot;
	
    static float toward_angle_delta[4][10] = 
	{ 
        { -28.0f, -68.0f, 28.0f, 68.0f, -45.0f, 45.0f, -90.0f, 90.0f, -999.0f}, 
        { -18.0f, -28.0f, 18.0f, 28.0f, -45.0f, 45.0f, -90.0f, 90.0f, -999.0f}, 
        { 28.0f, 68.0f, -28.0f, -68.0f, 45.0f, -45.0f, 90.0f, -90.0f, -999.0f}, 
        { 18.0f, 28.0f, -18.0f, -28.0f, 45.0f, -45.0f, 90.0f, -90.0f, -999.0f}
	};
	
    static float away_angle_delta[4][10] = 
	{ 
        { -135.0f, 135.0f, -112.0f, 112.0f, -157.0f, 157.0f, 180.0f, -999.0f}, 
        { 135.0f, -135.0f, 112.0f, -112.0f, -157.0f, 157.0f, 180.0f, -999.0f}, 
        { 157.0f, -157.0f, -135.0f, 135.0f, -112.0f, 112.0f, 180.0f, -999.0f},
        { 157.0f, -157.0f, 135.0f, -135.0f, 112.0f, -112.0f, 180.0f, -999.0f}
	};
	
    static float evade_angle_delta[4][8] = 
	{ 
        { 90.0f,  -90.0f, -112.0f, 112.0f, -68.0f, 68.0f,  45.0f,  -999.0f}, 
        { -90.0f,  90.0f,  68.0f, -68.0f, -112.0f, 112.0f, 135.0f, -999.0f}, 
        { 68.0f, -68.0f,  -112.0f, 112.0f, -90.0f, 90.0f,  45.0f,  -999.0f},
        { 68.0f,  90.0f,  -68.0f, -90.0f, 112.0f, -112.0f, 135.0f, -999.0f}
	};
	
    float *adp;
	
    geBoolean ret;
	geBoolean found = GE_FALSE;
	
    float dist;
    // start out with mininum distance that will be accepted as a move
    float save_dist = MINMOVEDIST;
	
	geVec3d Vec2Tgt;
	geVec3d Pos, TgtPos;
	geVec3d vec_dir, stop_pos, save_stop_pos;
	float Dist2TgtPos,NewDist2TgtPos;
	
	DBot = (Bot_Var *)Data;
	
	CCD->ActorManager()->GetPosition(DBot->Actor, &Pos);
	TgtPos = *TargetPos;
	Dist2TgtPos = geVec3d_DistanceBetween(&TgtPos, &Pos);
	
	save_dist = Dist2TgtPos;
	
	// Find vector to the player
    geVec3d_Subtract(&TgtPos, &Pos, &Vec2Tgt);
	Vec2Tgt.Y = 0.0f;
    if (geVec3d_Length(&Vec2Tgt) == 0)
        Vec2Tgt.X = 1.0f;
	
	// choose a random angle array
	if (dir == BOT_DIR_TOWARD)
		adp = &toward_angle_delta[RandomRange(4)][0];
	else
		if (dir == BOT_DIR_AWAY)
			adp = &away_angle_delta[RandomRange(4)][0];
		else
			if (dir == BOT_DIR_NONE)
			{
				adp = &evade_angle_delta[RandomRange(4)][0];
				save_dist = MINMOVEDIST;
			}
			
			for (; *adp != -999; adp++)
			{
				Vec2Tgt.Y = 0.0f;
				VectorRotateY(&Vec2Tgt, (*adp * (PI_2/360)), &vec_dir);
				vec_dir.Y = 0.0f;
				geVec3d_Normalize(&vec_dir);
				
				// check to see how far we can move
				ret = NPC_MoveScan((void *)DBot, &vec_dir, DistToMove, &stop_pos);
				
				NewDist2TgtPos = geVec3d_DistanceBetween(&TgtPos, &stop_pos);
				
				dist = geVec3d_DistanceBetween(&Pos, &stop_pos);

				// hit something really close
				if(ret)
					continue;
				
				// MOVEMENT
				if (NPC_OverLedgeScan((void *)DBot, &Pos, &stop_pos, &stop_pos))
				{
					dist = geVec3d_DistanceBetween(&Pos, &stop_pos);
					// hit something really close
					if (dist < (MINMOVEDIST/10.0f))
						continue;
				}
				
				if (dir == BOT_DIR_TOWARD)
				{
					if (NewDist2TgtPos < save_dist)
					{
						save_stop_pos = stop_pos;
						*save_vec = vec_dir;
						save_dist = dist;
						found = GE_TRUE;
					}	
				}
				else
					if (dir == BOT_DIR_AWAY)
					{
						if (NewDist2TgtPos > save_dist)
						{
							save_stop_pos = stop_pos;
							*save_vec = vec_dir;
							save_dist = dist;
							found = GE_TRUE;
						}	
					}
					else
						if (dir == BOT_DIR_NONE)
						{
								save_stop_pos = stop_pos;
								*save_vec = vec_dir;
								save_dist = dist;
								found = GE_TRUE;
						}
						
						if (ret == GE_FALSE)
						{
							// cleanly moved in new direction without hitting something
							save_stop_pos = stop_pos;
							found = GE_TRUE;
							break;
						}
			}
			
			if (found)
			{
				geVec3d Vec2Point;
				
				// gimme the actual vector to the stop point
				geVec3d_Subtract(&save_stop_pos, &Pos, &Vec2Point);
				Vec2Point.Y = 0.0f;
				
				// scale it so that its a little shorter and find new stop point
				geVec3d_Scale(&Vec2Point, 0.8f, &Vec2Point);
				geVec3d_Add(&Pos, &Vec2Point, &save_stop_pos);
				
				DBot->TgtPos = save_stop_pos;
				
				return(GE_TRUE);
			}
			
			return(GE_FALSE);
}

//=====================================================================================
//	NPC_PastPoint
//=====================================================================================
geBoolean NPC_PastPoint(geVec3d *Pos, geVec3d *MoveVector, geVec3d *TgtPos)
{
	geVec3d Vec2Point, MoveVec = *MoveVector;
	
	// Temp hack
	if (geVec3d_Length(TgtPos) == 0.0f)
		return GE_TRUE;	
	
	// see if the bot went past the point
	geVec3d_Subtract(TgtPos, Pos, &Vec2Point);
	
	// make sure they are 2d for this test
    Vec2Point.Y = 0.0f;
	MoveVec.Y = 0.0f;
	
	geVec3d_Normalize(&Vec2Point);	
    geVec3d_Normalize(&MoveVec);	
	
	return (geVec3d_DotProduct(&Vec2Point, &MoveVec) < 0.0f);
}	


//=====================================================================================
//	NPC_MoveToPoint
//=====================================================================================
geBoolean NPC_MoveToPoint(void *Data, float Time)
{
	geVec3d			Pos, TgtPlayerPos, Vec2Player;
	Bot_Var			*DBot;
	geBoolean		ResetVelocity = GE_FALSE, BotShoot;
	TrackPt         *ThisPoint = NULL, *NextPoint = NULL;
	
	DBot = (Bot_Var *)Data;
	
	// save off the position
    CCD->ActorManager()->GetPosition(DBot->Actor, &Pos);
	CCD->ActorManager()->GetPosition(DBot->TgtPlayer, &TgtPlayerPos);
	
	geVec3d_Subtract(&TgtPlayerPos, &Pos, &Vec2Player);
	
	// decide whether to face player & shoot
	switch (DBot->Mode)
	{
	case MODE_UNSTICK:
		BotShoot = FALSE;
		NPC_SetupXForm((void *)DBot, &Vec2Player);       //Face Player
		break;
	case MODE_RETREAT:
	case MODE_RETREAT_ON_TRACK:
		if (DBot->FaceTgtPlayerOnRetreat)
		{
			BotShoot = TRUE;
			NPC_SetupXForm((void *)DBot, &Vec2Player);       //Face Player
		}
		else
		{
			BotShoot = FALSE;
			NPC_SetupXForm((void *)DBot, &DBot->MoveVec);     //Face run direction
		}
		break;
	case MODE_WANDER:
	case MODE_WANDER_ON_TRACK:
		BotShoot = FALSE;
		NPC_SetupXForm((void *)DBot, &DBot->MoveVec);     //Face run direction
		break;
	default:
		BotShoot = TRUE;
		NPC_SetupXForm((void *)DBot, &Vec2Player);       //Face Player
		break;
		
		  }

	//an attempt to counteract Velocity and zero in on the target position
	if (!NPC_PastPoint(&Pos, &DBot->MoveVec, &DBot->TgtPos))
	{
		geVec3d save = DBot->MoveVec;
					
		// get a new MoveVec
		geVec3d_Subtract(&DBot->TgtPos, &Pos, &DBot->MoveVec);
		DBot->MoveVec.Y = 0.0f;
		geVec3d_Normalize(&DBot->MoveVec);
					
		if (NPC_PastPoint(&Pos, &DBot->MoveVec, &DBot->TgtPos))
		{
				DBot->MoveVec = save;
		}
	}
	
	geVec3d_Clear(&DBot->Velocity);
	geVec3d_AddScaled(&DBot->Velocity, &DBot->MoveVec, DBot->RunSpeed, &DBot->Velocity);
				
	if (CCD->Track()->Track_OnTrack(&DBot->TrackInfo))
	{
		// MOVEMENT
		int Result = NPC_Movement((void *)DBot, Time);

		CCD->ActorManager()->GetPosition(DBot->Actor, &Pos);			
		if (DBot->TrackInfoPrev.TrackNdx == DBot->TrackInfo.TrackNdx &&
				CCD->Track()->Track_OnTrack(&DBot->TrackInfoPrev) && 
				CCD->Track()->Track_GetPoint(&DBot->TrackInfoPrev)->Action != POINT_TYPE_WAIT_POINT_VISIBLE)
		{
			if(!Result)
			{
				// get off of track
				NPC_ClearTrack(DBot);
				// pick another mode
				NPC_ModeChange((void *)DBot, MODE_NULL, GE_TRUE, Time);
				return GE_TRUE;
			}
		}
		else
		{
			if(!Result)
			{
				// get off of track
				NPC_ClearTrack(DBot);
				// pick another mode
				NPC_ModeChange((void *)DBot, MODE_NULL, GE_TRUE, Time);
				return GE_TRUE;
			}
		}
	}
	else
		// MOVEMENT
		if (!NPC_Movement((void *)DBot, Time))
		{
				if (DBot->Bumps++ > 3)
				{
					DBot->Bumps = 0;
					NPC_ModeChange((void *)DBot, MODE_UNSTICK, GE_TRUE, Time);
					return GE_TRUE;
				}			
				if (NPC_ModeThink((void *)DBot, Time))  //If couldn't move try a new action
					return GE_TRUE;
		}
		else
		{
			DBot->Bumps = 0;
		}	
		CCD->ActorManager()->GetPosition(DBot->Actor, &Pos);
/*		if (NPC_OverLedge((void *)DBot, &Pos))
		{
			if (DBot->LedgeBumps++ > 16)
			{
				DBot->LedgeBumps = 0;
				NPC_ModeChange((void *)DBot, MODE_UNSTICK, GE_TRUE, Time);
				return GE_TRUE;
			}				
			NPC_ModeThinkLedge((void *)DBot, Time);                                                                                                                                                                    
		}
		else 
			DBot->LedgeBumps = 0;
*/		
		DBot->Stopped = false;
		NPC_Animate((void *)DBot, Time);
					
		// shooting
		if (!PlayerDead(DBot->TgtPlayer) && BotShoot)
		{
			if (DBot->TimeSeen > 0.0f)
			{
				if(DBot->Melee)
				{
					if(geVec3d_DistanceBetween(&Pos, &TgtPlayerPos)<=DBot->MeleeDistance)
						NPC_Shoot((void *)DBot, &TgtPlayerPos, Time);
				}
				else
					NPC_Shoot((void *)DBot, &TgtPlayerPos, Time);
			}
		}
					
		// see about getting off of a path
		if (NPC_LeaveTrack((void *)DBot, Time))
		{
			CCD->ActorManager()->GetPosition(DBot->TgtPlayer, &DBot->GoalPos);
			NPC_ModeThink((void *)DBot, Time);
			return GE_TRUE;
		}
					
		// test current pos and MoveVec
		CCD->ActorManager()->GetPosition(DBot->Actor, &Pos);
		if (NPC_PastPoint(&Pos, &DBot->MoveVec, &DBot->TgtPos))    
		{
			if (DBot->Mode == MODE_GOAL_POINT &&
				NPC_PastPoint(&Pos, &DBot->MoveVec, &DBot->GoalPos))
			{
				// pick another mode
				NPC_ModeChange((void *)DBot, MODE_NULL, GE_TRUE, Time);
				return GE_TRUE;
			}		
			if (!CCD->Track()->Track_OnTrack(&DBot->TrackInfo))
			{
				// decide what to do next
				if (NPC_ModeThink((void *)DBot, Time))
					return GE_TRUE;
							
				// last resort - just reposition
				NPC_Reposition((void *)DBot, &DBot->GoalPos, DBot->Dir);
				return GE_TRUE;
			}
						
			// -=ON=- A TRACK PAST HERE
			DBot->PastFirstTrackPoint = GE_TRUE;
						
			if (CCD->Track()->Track_OnTrack(&DBot->TrackInfo) &&
					!NPC_PastPoint(&Pos, &DBot->MoveVec, &DBot->GoalPos))
			{
				return GE_TRUE;
			}
						
			ThisPoint = CCD->Track()->Track_GetPoint(&DBot->TrackInfo);
						
			DBot->TrackInfoPrev = DBot->TrackInfo;
			if (NextPoint = CCD->Track()->Track_NextPoint(&DBot->TrackInfo))
			{
				// if there is a next point - face that direction
				DBot->GoalPos = DBot->TgtPos = *NextPoint->Pos;
				geVec3d_Subtract(NextPoint->Pos, ThisPoint->Pos, &DBot->MoveVec);
				DBot->MoveVec.Y = 0.0f;
				geVec3d_Normalize(&DBot->MoveVec);
			}
						
			if (NPC_TrackAction((void *)DBot, ThisPoint, NextPoint, Time))
				return GE_TRUE;
						
			// Track was cleared?
			if (!CCD->Track()->Track_OnTrack(&DBot->TrackInfo))
			{
				NPC_ModeChange((void *)DBot, MODE_NULL, GE_FALSE, Time);
			}
						
			if (!NextPoint)
			{
				// Finish Track will pop stack if necessary
				NPC_FinishTrack((void *)DBot);
				// ONLY THINK IF YOU ARE DONE WITH THE STACK!
				if (StackIsEmpty(&DBot->TrackStack))
					NPC_ModeThink((void *)DBot, Time);
				return(GE_TRUE);
			}
						
		}
					
	return GE_TRUE;
}

//=====================================================================================
//	NPC_ActionGetHealth
//=====================================================================================
geBoolean NPC_ActionGetHealth(void *Data, float Range, float Time)
{
    geActor *HealthItem;
	Bot_Var	*DBot;
	geVec3d	Pos;
	
	DBot = (Bot_Var *)Data;
	CCD->ActorManager()->GetPosition(DBot->Actor, &Pos);
	
    if (HealthItem = NPC_FindItem(&Pos, ItemHealthList, BOT_SEARCH_Y_RANGE, Range))
    {
		CCD->ActorManager()->GetPosition(HealthItem, &DBot->GoalPos);
        NPC_InitMoveToPoint((void *)DBot, &DBot->GoalPos);
        NPC_ModeChange((void *)DBot, MODE_GOAL_POINT, GE_FALSE, Time);
        return GE_TRUE;
    }
	
    return GE_FALSE;
}

//=====================================================================================
//	NPC_ActionGetWeapon
//=====================================================================================
geBoolean NPC_ActionGetWeapon(void *Data, float Range, float Time)
{
    geActor *WeaponItem;
	Bot_Var	*DBot;
	geVec3d	Pos;
	
	DBot = (Bot_Var *)Data;
	CCD->ActorManager()->GetPosition(DBot->Actor, &Pos);
	
	if (WeaponItem = NPC_FindItem(&Pos, ItemWeaponList, BOT_SEARCH_Y_RANGE, Range))
    {
		CCD->ActorManager()->GetPosition(WeaponItem, &DBot->GoalPos);
        NPC_InitMoveToPoint((void *)DBot, &DBot->GoalPos);
        NPC_ModeChange((void *)DBot, MODE_GOAL_POINT, GE_FALSE, Time);
        return GE_TRUE;
    }
	
    return GE_FALSE;
}

//=====================================================================================
//	NPC_CompareWeapons
//=====================================================================================
int32 NPC_CompareWeapons(geActor *Actor1, geActor *Actor2)
{
	
	return 0;
}

//=====================================================================================
//	NPC_FindTrackToGoal
//=====================================================================================
Track *NPC_FindTrackToGoal(void *Data, float Time)
{
    Track *track;
    int32 *type;
    float ydiff;
	TrackData TrackInfo;
	
	geVec3d	Pos, TgtPos;
	Bot_Var	*DBot;
	
    static int32 Away[] =
	{
        TRACK_TYPE_UP,
			TRACK_TYPE_DOWN,
			TRACK_TYPE_ELEVATOR_UP,
			TRACK_TYPE_ELEVATOR_DOWN,
			TRACK_TYPE_TRAVERSE,
			TRACK_TYPE_TRAVERSE_ONEWAY,
			TRACK_TYPE_EXIT,
			TRACK_TYPE_TRAVERSE_DOOR,
			-1
	};
	
    static int32 PlayerAbove[] =
	{
        TRACK_TYPE_UP,
			TRACK_TYPE_ELEVATOR_UP,
			TRACK_TYPE_TRAVERSE,
			TRACK_TYPE_TRAVERSE_ONEWAY,
			-1
	};
	
    static int32 PlayerBelow[] =
	{
        TRACK_TYPE_DOWN,
			TRACK_TYPE_ELEVATOR_DOWN,
			TRACK_TYPE_TRAVERSE,
			TRACK_TYPE_TRAVERSE_ONEWAY,
			-1
	};
	
    static int32 PlayerOnLevel[] =
	{
        TRACK_TYPE_TRAVERSE,
			TRACK_TYPE_TRAVERSE_ONEWAY,
			TRACK_TYPE_EXIT,
			TRACK_TYPE_TRAVERSE_DOOR,
			-1
	};
	
    DBot = (Bot_Var *)Data;
	CCD->ActorManager()->GetPosition(DBot->Actor, &Pos);
	TgtPos = DBot->GoalPos;
	
    ydiff = TgtPos.Y - Pos.Y;
	
	if (DBot->Dir == BOT_DIR_TOWARD)
	{
		if (fabs(ydiff) <= BOT_SEARCH_Y_RANGE)
			type = PlayerOnLevel;
		else
		{
			if (ydiff < 0.0f)
				type = PlayerBelow;
			else
				type = PlayerAbove;
		}
	}
	else
		if (DBot->Dir == BOT_DIR_AWAY)
		{
			type = Away;
		}
		if (DBot->Dir == BOT_DIR_NONE)
		{
			type = Away;
		}
		
		CCD->Track()->Track_ClearTrack(&TrackInfo);
		while (1)
		{
			track = CCD->Track()->Track_FindTrack(&Pos, &TgtPos, DBot->Dir, type, &TrackInfo);
			
			if (!track)
				return NULL;
			
			if (NPC_ValidateTrackPoints((void *)DBot, track))
			{
				// can't add the same track!
				if (StackTop(&DBot->TrackStack) == TrackInfo.TrackNdx)
					continue;
				
				DBot->TrackInfo = TrackInfo;
				StackPush(&DBot->TrackStack, DBot->TrackInfo.TrackNdx);
				DBot->PastFirstTrackPoint = GE_FALSE;
				
				return (track);
			}
		}
		
		return NULL;
}

//=====================================================================================
//	NPC_ValidateTrackPoints
//=====================================================================================
geBoolean NPC_ValidateTrackPoints(void *Data, Track* t)
{
	TrackPt *tp;
	Bot_Var	*DBot;
	
	DBot = (Bot_Var *)Data;
	
    for (tp = t->PointList; tp < &t->PointList[t->PointCount]; tp++)
	{
		switch (tp->Action)
		{
		case POINT_TYPE_WAIT_POINT_VISIBLE:
			break;
			
		case POINT_TYPE_LOOK_FOR_ITEMS:
			{
				geActor *HealthItem;
				geActor *WeaponItem;
				
				if (!(HealthItem = NPC_FindItem(tp->Pos, ItemHealthList, BOT_SEARCH_Y_RANGE, 1000.0f)))
					return GE_FALSE;
				
				if (!(WeaponItem = NPC_FindItem(tp->Pos, ItemWeaponList, BOT_SEARCH_Y_RANGE, 1000.0f)))
					return GE_FALSE;
				
				break;
			}
		}
	}
	
	return GE_TRUE;
}

//=====================================================================================
//	NPC_ValidateMultiTrackPoints
//=====================================================================================
geBoolean NPC_ValidateMultiTrackPoints(void *Data, Track* t)
{
	TrackPt *tp;
	Bot_Var	*DBot;
	
	DBot = (Bot_Var *)Data;
	
    for (tp = t->PointList; tp < &t->PointList[t->PointCount]; tp++)
	{
		switch (tp->Action)
		{
		case POINT_TYPE_WAIT_FOR_PLAYER: // no hiding on multi-tracks
			return GE_FALSE;
			
		case POINT_TYPE_LOOK_FOR_ITEMS:
			return GE_FALSE;
		}
	}
	
	return GE_TRUE;
}

//=====================================================================================
//	NPC_FindTrack
//=====================================================================================
Track *NPC_FindTrack(void *Data, int32 TrackArr[])
{
    Track *track;
	TrackData TrackInfo;
    
	geVec3d			Pos, TgtPos;
	Bot_Var			*DBot;
	
	DBot = (Bot_Var *)Data;	
	
	CCD->ActorManager()->GetPosition(DBot->Actor, &Pos);
	TgtPos = DBot->GoalPos;
	
	CCD->Track()->Track_ClearTrack(&TrackInfo);
	while (1)
	{
		track = CCD->Track()->Track_FindTrack(&Pos, &TgtPos, 0, TrackArr, &TrackInfo);
		
		if (!track)
			break;
		
		if (NPC_ValidateTrackPoints((void *)DBot, track))
		{
			// can't add the same track!
			if (StackTop(&DBot->TrackStack) == TrackInfo.TrackNdx)
				continue;
			
			DBot->TrackInfo = TrackInfo;
			StackPush(&DBot->TrackStack, DBot->TrackInfo.TrackNdx);
			DBot->PastFirstTrackPoint = GE_FALSE;
			return (track);
		}
	}
	
    return (NULL);
}

//=====================================================================================
//	NPC_TrackAction
//=====================================================================================
geBoolean NPC_TrackAction(void *Data, const TrackPt* ThisPoint, const TrackPt* NextPoint, float Time)
{
    Bot_Var         *DBot;
    int32           DoAction;
	geVec3d	Pos;
	
    DBot = (Bot_Var *)Data;
	
    DoAction = ThisPoint->Action;
	
    // make sure your going the right direction
    if (ThisPoint->ActionDir != 0 && ThisPoint->ActionDir != CCD->Track()->Track_GetDir(&DBot->TrackInfo))
        DoAction = -1;
	
	switch (DoAction)
	{
	case POINT_TYPE_JUMP:
		{
			if (ThisPoint->VelocityScale > 0.0f)
				DBot->JumpSpeed = ThisPoint->VelocityScale;
			else
				DBot->JumpSpeed = 60.0f;

			DBot->Action = NPC_Jump;
		
			return(GE_TRUE);
		}
		
	case POINT_TYPE_LOOK_FOR_ITEMS:
		{
			geActor *HealthItem;
			geActor *WeaponItem;
							  
			// if multi-tracks
			if (DBot->TrackStack.TOS >= 1)
				break;

			CCD->ActorManager()->GetPosition(DBot->Actor, &Pos);
			if (HealthItem = NPC_FindItem(&Pos, ItemHealthList, BOT_SEARCH_Y_RANGE, 500.0f))
			{
				CCD->ActorManager()->GetPosition(HealthItem, &DBot->GoalPos);
				NPC_ClearTrack(DBot);
				NPC_InitMoveToPoint((void *)DBot, &DBot->GoalPos);
 			    NPC_ModeChange((void *)DBot, MODE_GOAL_POINT, GE_FALSE, Time);
	 		    return GE_TRUE;
			}	
			if (WeaponItem = NPC_FindItem(&Pos, ItemWeaponList, BOT_SEARCH_Y_RANGE, 500.0f))
			{
				CCD->ActorManager()->GetPosition(WeaponItem, &DBot->GoalPos);
				NPC_ClearTrack(DBot);
				NPC_InitMoveToPoint((void *)DBot, &DBot->GoalPos);
 			    NPC_ModeChange((void *)DBot, MODE_GOAL_POINT, GE_FALSE, Time);
	 		    return GE_TRUE;
			}	
		    break;
		}

	case POINT_TYPE_SHOOT:
		{
			NPC_InitShootPoint((void *)DBot, Time);
			return GE_TRUE;
		}

	case POINT_TYPE_WAIT_FOR_PLAYER:
		{
			// if multi-tracks
			if (DBot->TrackStack.TOS >= 1)
				break;
			
			if (!CanSeeActorToActor(DBot->Actor, DBot->TgtPlayer))
			{
				geVec3d TPos = *ThisPoint->Pos;
				CCD->ActorManager()->Position(DBot->Actor, TPos);

				// Set timeout
				if (ThisPoint->Time > 0.0f)
					DBot->TimeOut = ThisPoint->Time;
				else
					DBot->TimeOut = 20.0f;
				
				NPC_InitWaitForPlayer((void *)DBot, Time);
				// still on track at this point
				return GE_TRUE;
			}
			
			break;
		}
		
	case POINT_TYPE_WAIT_POINT_VISIBLE:
		{
			geVec3d TPos = *ThisPoint->Pos;
			geVec3d TVs;
			// nudge to point
			CCD->ActorManager()->GetPosition(DBot->Actor, &Pos);
			geVec3d_Subtract(&TPos, &Pos, &TVs);
			TVs.Y = 0.0f;
			geVec3d_Clear(&DBot->Velocity);
			geVec3d_AddScaled(&DBot->Velocity, &TVs, DBot->RunSpeed, &DBot->Velocity);

			NPC_Movement((void *)DBot, Time);
			// Set timeout	
			if (ThisPoint->Time > 0.0f)
				DBot->TimeOut = ThisPoint->Time;
			else
				DBot->TimeOut = 5.0f;
			
			geVec3d_Clear(&DBot->Velocity);
			
			NPC_InitWaitForEntityVisible((void *)DBot, Time);
			// still on track at this point
			return GE_TRUE;
		}
		
	case POINT_TYPE_WAIT_POINT_DIST:
		{
			// Set timeout
			if (ThisPoint->Time > 0.0f)
				DBot->TimeOut = ThisPoint->Time;
			else
				DBot->TimeOut = 5.0f;
			
			NPC_InitWaitForEntityDist((void *)DBot, Time);
			// still on track at this point
			return GE_TRUE;
		}
	}
	return GE_FALSE;
}

//=====================================================================================
//	NPC_LeaveTrack
//=====================================================================================
geBoolean NPC_LeaveTrack(void *Data, float Time)
{
	Bot_Var	*DBot;
	float	dist;
	geVec3d	Pos, TgtPos;
	
	DBot = (Bot_Var *)Data;	
	CCD->ActorManager()->GetPosition(DBot->Actor, &Pos);
	CCD->ActorManager()->GetPosition(DBot->TgtPlayer, &TgtPos);
	
    // run by when retreating
	if (DBot->Mode == MODE_RETREAT_ON_TRACK || DBot->Mode == MODE_RETREAT)
		return GE_FALSE;

    dist = geVec3d_DistanceBetween(&Pos, &TgtPos);
	
	if ((DBot->Mode == MODE_FIND_PLAYER || DBot->Mode == MODE_FIND_PLAYER_QUICK) &&
        dist < 500.0f && DBot->TimeSeen > 0.0f)
	{
        NPC_ClearTrack(DBot);
		return GE_TRUE;
	}
	
	if (dist < 150.0f && DBot->TimeSeen > 0.0f)
	{
        NPC_ClearTrack(DBot);
		return GE_TRUE;
	}
	
	return GE_FALSE;
}

//=====================================================================================
//	NPC_FinishTrack - finishing a track should allow movement to the next track
//=====================================================================================
geBoolean NPC_FinishTrack(void *Data)
{
	Bot_Var			*DBot;
	int32			NewTrack;
	geVec3d	Pos, TgtPos;
	
	DBot = (Bot_Var *)Data;	
	CCD->ActorManager()->GetPosition(DBot->Actor, &Pos);
	CCD->ActorManager()->GetPosition(DBot->TgtPlayer, &TgtPos);
	
	CCD->Track()->Track_ClearTrack(&DBot->TrackInfo);
	
	StackPop(&DBot->TrackStack);
	if (!StackIsEmpty(&DBot->TrackStack))
	{
		NewTrack = StackTop(&DBot->TrackStack);
		CCD->Track()->Track_NextMultiTrack(&Pos, NewTrack, &DBot->TrackInfo);
		DBot->PastFirstTrackPoint = GE_FALSE;
		// now on track
		// set goal to next point
		DBot->GoalPos = *CCD->Track()->Track_GetPoint(&DBot->TrackInfo)->Pos;
		NPC_InitMoveToPoint((void *)DBot, &DBot->GoalPos);
	}
    else 
	{
        DBot->GoalPos = TgtPos;
	}
	
	return GE_TRUE;
}

geBoolean PlayerDead(geActor *Actor)
{
	return GE_FALSE;
}

//=====================================================================================
//	NPC_MoveFree
//=====================================================================================
geBoolean NPC_MoveFree(void *Data, float Time)
{
	Bot_Var	*DBot;
	geVec3d	Pos, TgtPlayerPos, Vec2Player;

	DBot = (Bot_Var *)Data;
	CCD->ActorManager()->GetPosition(DBot->Actor, &Pos);
	CCD->ActorManager()->GetPosition(DBot->TgtPlayer, &TgtPlayerPos);
	geVec3d_Subtract(&TgtPlayerPos, &Pos, &Vec2Player);
    NPC_SetupXForm((void *)DBot, &DBot->MoveVec);
	geVec3d_Clear(&DBot->Velocity);
	geVec3d_AddScaled(&DBot->Velocity, &DBot->MoveVec, DBot->RunSpeed, &DBot->Velocity);
	if(!NPC_Movement((void *)DBot, Time))
	{
        // reposition again
        NPC_ModeThink((void *)DBot, Time);
		return GE_TRUE;
	} 
	DBot->Stopped = false;
	NPC_Animate((void *)DBot, Time);

	CCD->ActorManager()->GetPosition(DBot->Actor, &Pos);
	// test current pos and MoveVec
    if (NPC_PastPoint(&Pos, &DBot->MoveVec, &DBot->TgtPos))    
	{
		// decide what to do next
        if (NPC_ModeThink((void *)DBot, Time))
            return GE_TRUE;
	}

	return GE_TRUE;
}

//=====================================================================================
//	NPC_MoveScan
//=====================================================================================
geBoolean NPC_MoveScan(void *Data, geVec3d *vec_dir, float dist, geVec3d *stop_pos)
{
	Bot_Var	*DBot;
	geVec3d SaveVelocity;
	geVec3d	Pos, TgtPos;
	GE_Collision Collision;
	
	DBot = (Bot_Var *)Data;	
	CCD->ActorManager()->GetPosition(DBot->Actor, &Pos);
	// New Velocity in this direction
    geVec3d_Scale(vec_dir, dist, &SaveVelocity);
	geVec3d_Add(&Pos, &SaveVelocity, &TgtPos);
	*stop_pos = TgtPos;
	CCD->Collision()->IgnoreContents(false);
	CCD->Collision()->CheckLevel(RGF_COLLISIONLEVEL_1);
	bool Result = CCD->Collision()->CheckActorCollision(Pos, TgtPos, DBot->Actor, &Collision);
	if(Result)
	{
		*stop_pos = Collision.Impact;
	}

	return Result;
}

geBoolean NPC_Movement(void *Data, float Time)
{
	Bot_Var	*DBot;
	geVec3d	Pos, TgtPos, Distance;

	DBot = (Bot_Var *)Data;
	CCD->ActorManager()->GetPosition(DBot->Actor, &Pos);
	DBot->LastGoodPos = Pos;

	geVec3d_Scale( &DBot->Velocity, Time, &Distance ) ;
	if(DBot->Stopped || (DBot->Attacking && DBot->StopToAttack))
		geVec3d_Clear(&Distance);
	geVec3d_Add(&Pos, &Distance, &TgtPos) ;

	return CCD->ActorManager()->ValidateMove(Pos, TgtPos, DBot->Actor, false);
}

//=====================================================================================
//	NPC_Jump
//=====================================================================================
geBoolean NPC_Jump(void *Data, float Time)
{
	geVec3d theUp;
	Bot_Var			*DBot;
	
	DBot = (Bot_Var *)Data;

	NPC_SetupXForm((void *)DBot, &DBot->MoveVec);

	CCD->ActorManager()->UpVector(DBot->Actor, &theUp);
	CCD->ActorManager()->SetForce(DBot->Actor, 0, theUp, DBot->JumpSpeed, DBot->JumpSpeed);

	NPC_InitGenericMove((void *)DBot, Time);

    return GE_TRUE;
}

geBoolean NPC_Shoot(void *Data, geVec3d *ShootPosition, float Time)
{
	Bot_Var	*DBot;
	geVec3d	Pos, Direction, Orient;
	geExtBox theBox;

	DBot = (Bot_Var *)Data;

	if(DBot->NextWeaponTime>0.0f || DBot->Attacking)
		return GE_TRUE;

	DBot->NextWeaponTime = DBot->AttackDelay;
	CCD->ActorManager()->GetPosition(DBot->Actor, &Pos);
	CCD->ActorManager()->GetBoundingBox(DBot->Actor, &theBox);

	if(DBot->Melee)
	{
		geVec3d Forward, theRotation;
		GE_Collision Collision;
		geXForm3d Xf;
		Pos.Y += theBox.Max.Y/2.0f;
		float temp = (theBox.Max.Y-theBox.Min.Y)/4.0f;
		theBox.Min.Y = -temp;
		theBox.Max.Y = temp;
		CCD->ActorManager()->GetRotate(DBot->Actor, &theRotation);
		geXForm3d_SetIdentity(&Xf);
		geXForm3d_RotateZ(&Xf, theRotation.Z);
		geXForm3d_RotateX(&Xf, theRotation.X);
		geXForm3d_RotateY(&Xf, theRotation.Y);
		geXForm3d_Translate(&Xf, Pos.X, Pos.Y, Pos.Z);
		geXForm3d_GetIn (&Xf, &Forward);
		float distance = 2.0f * (((theBox.Max.X) < (theBox.Max.Z)) ? (theBox.Max.X) : (theBox.Max.Z));
		geVec3d_AddScaled (&Xf.Translation, &Forward, distance, &Pos);
		Collision.Actor = NULL;
		Collision.Model = NULL;
		bool result = false;
		geWorld_Model *pModel;
		geActor *pActor;
		theBox.Min.X += Pos.X;
		theBox.Min.Y += Pos.Y;
		theBox.Min.Z += Pos.Z;
		theBox.Max.X += Pos.X;
		theBox.Max.Y += Pos.Y;
		theBox.Max.Z += Pos.Z;
		if(CCD->ActorManager()->DoesBoxHitActor(Pos, theBox, &pActor, DBot->Actor) == GE_TRUE)
		{
			Collision.Model = NULL;
			Collision.Actor = pActor;		// Actor we hit
			result=true;
		}
		else if(CCD->ModelManager()->DoesBoxHitModel(Pos, theBox, &pModel) == GE_TRUE)
		{
			Collision.Actor = NULL;
			Collision.Model = pModel;		// Model we hit
			result=true;
		}

		if(result)
		{
			if(Collision.Actor!=NULL)
				CCD->Damage()->DamageActor(Collision.Actor, DBot->DamageAmt, DBot->DamageAttribute, DBot->DamageAmt, DBot->DamageAttribute);
			if(Collision.Model)
				CCD->Damage()->DamageModel(Collision.Model, DBot->DamageAmt, DBot->DamageAttribute, DBot->DamageAmt, DBot->DamageAttribute);
		}
	}
	else
	{
		geXForm3d Xf;
		geVec3d theRotation;
		geFloat x;
		if(EffectC_IsStringNull(DBot->FireBone))
			DBot->FireBone[0] = '\0';
		if(geActor_GetBoneTransform(DBot->Actor, DBot->FireBone, &Xf))
		{
			geVec3d_Copy(&(Xf.Translation), &Pos);
			CCD->ActorManager()->GetRotate(DBot->Actor, &theRotation);
			geXForm3d_SetIdentity(&Xf);
			geXForm3d_RotateZ(&Xf, theRotation.Z);
			geXForm3d_RotateX(&Xf, theRotation.X);
			geXForm3d_RotateY(&Xf, theRotation.Y);
			geXForm3d_Translate(&Xf, Pos.X, Pos.Y, Pos.Z);
			geXForm3d_GetUp(&Xf, &Direction);
			geVec3d_AddScaled (&Pos, &Direction, DBot->FireOffset.Y, &Pos);
			geXForm3d_GetLeft(&Xf, &Direction);
			geVec3d_AddScaled (&Pos, &Direction, DBot->FireOffset.X, &Pos);
			geXForm3d_GetIn(&Xf, &Direction);
			geVec3d_AddScaled (&Pos, &Direction, DBot->FireOffset.Z, &Pos);
			geVec3d_Subtract(&DBot->AimPos, &Pos, &Orient);
			float l = geVec3d_Length(&Orient);
			if(l > 0.0f) 
			{
				x = Orient.X;
				Orient.X = (float)( GE_PI*0.5 ) - (float)acos(Orient.Y / l);
				Orient.Y = (float)atan2( x , Orient.Z ) + GE_PI;
				// roll is zero - always!!?
				Orient.Z = 0.0;
				CCD->Weapons()->Add_Projectile(Pos, Pos, Orient, DBot->Projectile, DBot->DamageAttribute, DBot->DamageAttribute);
			}
		}
	}

	if(DBot->StopToAttack)
	{
		DBot->LastStopped = DBot->Stopped;
		DBot->Stopped = true;
	}
	DBot->Attacking = true;
	strcpy(DBot->LastAnimation, DBot->Animation);
	if(!EffectC_IsStringNull(DBot->AttackSound))
	{
		Snd Sound;
		memset( &Sound, 0, sizeof( Sound ) );
		CCD->ActorManager()->GetPosition(DBot->Actor, &Sound.Pos);
		Sound.Min=kAudibleRadius;
		Sound.Loop=false;
		Sound.SoundDef = SPool_Sound(DBot->AttackSound);
		CCD->EffectManager()->Item_Add(EFF_SND, (void *)&Sound);
	}
	return GE_TRUE;
}

//=====================================================================================
//	NPC_OverLedgeScan
//=====================================================================================
geBoolean NPC_OverLedgeScan(void *Data, const geVec3d *StartPos, const geVec3d *EndPos, geVec3d *StopPos)
{
	GE_Collision Collision;
	Bot_Var			*DBot;
	geVec3d Pos, Pos2, Vec, LastGoodPos;
	float len;
	int32 i;
	geBoolean ret_val;

	DBot = (Bot_Var *)Data;

	geVec3d_Subtract(EndPos, StartPos, &Vec);
	len = geVec3d_Length(&Vec);
	geVec3d_Normalize(&Vec);

	Pos = *StartPos;

	for (i = 4; i > 0; i--)
	{
		LastGoodPos = Pos;
		geVec3d_AddScaled(StartPos, &Vec, len/(i*2), &Pos);
	
		Pos2 = Pos;

		// Try and move down by this much
		Pos2.Y -= 100.0f;

		// just use a ray with no size - not the player box
		ret_val = geWorld_Collision(CCD->World(), NULL, NULL, 
			&Pos, &Pos2, GE_CONTENTS_CANNOT_OCCUPY, GE_COLLIDE_MODELS, 0xffffffff, NULL, NULL, &Collision);

		if (!ret_val)
		{
			*StopPos = LastGoodPos;
			return(GE_TRUE);
		}

	}

	return GE_FALSE;
}

int NPC_Rank(void *Data)
{
	Bot_Var	*DBot;

	DBot = (Bot_Var *)Data;

	int temp = (DBot->theInv->Value(DBot->Attribute)*DBot->Rank)/DBot->theInv->High(DBot->Attribute);
	return temp;
}

//=====================================================================================
//	NPC_InitShootPoint
//=====================================================================================
geBoolean NPC_InitShootPoint(void *Data, float Time)
{
	Bot_Var	*DBot;

	DBot = (Bot_Var *)Data;

	// Stop
	DBot->Stopped = true;

	DBot->Action = NPC_ShootPoint;
	
	return GE_TRUE;
}

//=====================================================================================
//	NPC_ShootPoint
//=====================================================================================
geBoolean NPC_ShootPoint(void *Data, float Time)
{
	Bot_Var			*DBot;
	TrackPt         *Tp;

	DBot = (Bot_Var *)Data;

	if(DBot->NextWeaponTime>0.0f || DBot->Attacking)
		return GE_TRUE;

	DBot->NextWeaponTime = DBot->AttackDelay;

	geVec3d ShootPos;

	Tp = CCD->Track()->Track_GetPoint(&DBot->TrackInfoPrev);
	assert(Tp->WatchPos);

	ShootPos = *Tp->WatchPos;

	NPC_Shoot((void *)DBot, &ShootPos, Time);

	DBot->ShootCount++;
	if (DBot->ShootCount >= Tp->ShootTimes)
	{
		DBot->ShootCount = 0;
		if (CCD->Track()->Track_OnTrack(&DBot->TrackInfo))
			DBot->Action = NPC_MoveToPoint;
		else
			NPC_InitMoveToPoint((void *)DBot, &DBot->TgtPos);
		return GE_TRUE;
	}

	// time out
	if(DBot->TimeOut<=0.0f)
	{
        NPC_ClearTrack(DBot);
		NPC_InitGenericMove((void *)DBot, Time);
		return GE_TRUE;
	}

	return GE_TRUE;
}
