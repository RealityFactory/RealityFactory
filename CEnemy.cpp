
#ifdef RF063

#include "RabidFramework.h"

int ValidateTrackPoints(void *Data, Track* t);
int ValidateMultiTrackPoints(void *Data, Track* t);
static Track *FindRandomTrack(geVec3d *Pos, int32 *TrackTypeList);

typedef enum
{
	NPCIDLE = 0,
	NPCALERT,
	NPCLINJURY,
	NPCMINJURY,
	NPCHINJURY,
	NPCSEARCHROTATE,
	NPCSEARCHWALK,
	NPCSEARCHBACKUP,
	NPCSEARCHUNSTICK,
	NPCSEARCHNOGOAL,
	NPCJUMP
} NPCAction;

char *ActionText[] = 
{
	"NPCIdle",
	"NPCAlert",
	"NPCLight injury",
	"NPCMedium injury",
	"NPCHeavy injury",
	"NPCSearchRotate",
	"NPCSearchWalk",
	"NPCSearchBackup",
	"NPCSearchUnStick",
	"NPCSearchNoGoal",
	"NPCJump"
};

typedef enum
{
	SUBNULL = 0,
	SUBWALK,
	SUBSLIDELEFT,
	SUBSLIDERIGHT,
	SUBROTATE
} SUBAction;

enum
{
	AIIDLE = 0,
	AIALERT,
	AISEARCH,
	AIPATROL,
	AIATTACK,
	AIRETREAT,
    AITHREATEN,
	AILINJURY,
	AIMINJURY,
	AIHINJURY,
	AIMAX
};

char *StateText[] = 
{
	"idle",
	"alert",
	"search",
	"patrol",
	"attack",
	"retreat",
	"threaten",
	"light injury",
	"medium injury",
	"heavy injury"
};

char *Reaction[] =
{
	"ignore",
	"hostile",
	"random"
};

enum
{
	RIGNORE =0,
	RHOSTILE,
	RRANDOM,
	REACTMAX
};

enum
{
	STATETRIGGER = 0,
	STATEMODETIME,
	STATEATTACKED,
	STATESEETARGET,
	STATELOOSETARGET,
	STATETARGETDIST,
	STATEMAX
};

typedef union
{
	bool trigger;
	float modetime;
	int healthdiff;
	float notseentime;
	bool target;
	float distance;
} InputValue;

typedef struct StateItem
{
	bool active;
	int input;
	InputValue Value;
	int nextstate;
} StateItem;

typedef struct StateMachine
{
	StateItem Item[STATEMAX];
} StateMachine;

typedef struct StateInput
{
	bool active;
	int Input;
	InputValue Value;
} StateInput;

typedef struct AIData
{
	int		LastState;
	int		InitialState;
	int		State;
	int		InjuryNextState;
	float	ModeTime; 
	int		LastHealth;
	int		SameReaction;
	int		OtherReaction;
	int		PlayerReaction;
	float	SightDistance;
	float	FOV;
	float	RotationAmt;
	float	RotationSpeed;
	float	CurrentRotation;
	bool	RotateLeft;
	float	RotateAmt;
	geActor	*TargetActor;
	bool	GotGoal;
	geVec3d GoalPos;
	int		Dir;
	float	YRange;
	float	SeenTime;
	float	NotSeenTime;
	float	GoalTime;
	float	GoalTimeOut;
	int		BackupState;
	int		NextBackupState;
	bool	SearchMove;
	int		SearchCnt;
	float	BackupDist;
	float	UnStickTime;
	float	UnStickTimeOut;
	bool	SlideDir;
	SUBAction	SubAction;
	Stack   TrackStack;
	TrackData   TrackInfo;
	TrackData   TrackInfoPrev;
	bool	PastFirstTrackPoint;
	StateInput Inputs[STATEMAX];
	StateMachine FSM[AIMAX];
} AIData;

#define DIR_AWAY    -1
#define DIR_TOWARD   1
#define DIR_NONE     0

CEnemy::CEnemy()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	//	Ok, check to see if there are NPC in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Enemy");
	
	if(pSet) 
	{
		AttrFile.SetPath("npc.ini");
		if(!AttrFile.ReadFile())
		{
			CCD->ReportError("Can't open NPC initialization file", false);
			return;
		}
		//	Ok, we have NonPlayerCharacters somewhere.  Dig through 'em all.
		
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
			Enemy *pSource = (Enemy*)geEntity_GetUserData(pEntity);
			if(EffectC_IsStringNull(pSource->szEntityName))
			{
				char szName[128];
				geEntity_GetName(pEntity, szName, 128);
				pSource->szEntityName = szName;
			}
			// Ok, put this entity into the Global Entity Registry
			CCD->EntityRegistry()->AddEntity(pSource->szEntityName, "Enemy");
			pSource->alive = GE_TRUE;
			pSource->active = GE_FALSE;
			pSource->bState = GE_FALSE;
			pSource->CallBack = GE_FALSE;
			pSource->Dying = GE_FALSE;
			pSource->Location = pSource->origin;
			pSource->AlphaStep = 255.0f/pSource->DyingTime;
			pSource->Data = (void *)GE_RAM_ALLOCATE_STRUCT(AIData);
			memset(pSource->Data, 0, sizeof(AIData));
			BuildBase(pSource);
			BuildAI(pSource);
		}
	}
	return;
}

//	Destructor
//

CEnemy::~CEnemy()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	//	Ok, check to see if there are NPC in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Enemy");
	
	if(pSet) 
	{
		//	Ok, we have NonPlayerCharacters somewhere.  Dig through 'em all.
		
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
			Enemy *pSource = (Enemy*)geEntity_GetUserData(pEntity);
			AIData *Data = (AIData *)pSource->Data;
			if(pSource->Actor)
			{
				CCD->ActorManager()->RemoveActor(pSource->Actor);
				geActor_Destroy(&pSource->Actor);
			}
			if(pSource->ActorName)
				free(pSource->ActorName);
			if(pSource->AnimIdle)
				free(pSource->AnimIdle);
			if(pSource->AnimAlert)
				free(pSource->AnimAlert);
			if(pSource->AnimRotate)
				free(pSource->AnimRotate);
			if(pSource->AnimWalk)
				free(pSource->AnimWalk);
			if(pSource->AnimBackup)
				free(pSource->AnimBackup);
			if(pSource->AnimStrafeLeft)
				free(pSource->AnimStrafeLeft);
			if(pSource->AnimStrafeRight)
				free(pSource->AnimStrafeRight);
			if(pSource->AnimJump)
				free(pSource->AnimJump);
			if(pSource->AnimLInjury)
				free(pSource->AnimLInjury);
			if(pSource->AnimMInjury)
				free(pSource->AnimMInjury);
			if(pSource->AnimHInjury)
				free(pSource->AnimHInjury);
			if(pSource->AnimDie)
				free(pSource->AnimDie);
			if(Data->TrackStack.Data)
				geRam_Free(Data->TrackStack.Data);
			geRam_Free(pSource->Data);
		}
	}
}

void CEnemy::BuildBase(Enemy *pSource)
{
	CString KeyName = AttrFile.FindFirstKey();
	CString Type, Vector;
	char szName[64];
	while(KeyName != "")
	{
		if(!strcmp(KeyName,pSource->BaseName))
		{
			Type = AttrFile.GetValue(KeyName, "type");
			if(Type=="base")
			{
				Type = AttrFile.GetValue(KeyName, "actorname");
				pSource->ActorName = strdup(Type);
				Vector = AttrFile.GetValue(KeyName, "actorrotation");
				if(Vector!="")
				{
					strcpy(szName,Vector);
					pSource->Rotation = Extract(szName);
					pSource->Rotation.X *= 0.0174532925199433f;
					pSource->Rotation.Y *= 0.0174532925199433f;
					pSource->Rotation.Z *= 0.0174532925199433f;
				}
				geActor *Actor = CCD->ActorManager()->SpawnActor(pSource->ActorName, 
							pSource->Rotation, pSource->Rotation, "", "", NULL);
				CCD->ActorManager()->RemoveActor(Actor);
				geActor_Destroy(&Actor);
				pSource->Scale = (float)AttrFile.GetValueF(KeyName, "actorscale");
				geVec3d FillColor = {255.0f, 255.0f, 255.0f};
				geVec3d AmbientColor = {255.0f, 255.0f, 255.0f};
				Vector = AttrFile.GetValue(KeyName, "fillcolor");
				if(Vector!="")
				{
					strcpy(szName,Vector);
					FillColor = Extract(szName);
				}
				Vector = AttrFile.GetValue(KeyName, "ambientcolor");
				if(Vector!="")
				{
					strcpy(szName,Vector);
					AmbientColor = Extract(szName);
				}
				pSource->FillColor.r = FillColor.X;
				pSource->FillColor.g = FillColor.Y;
				pSource->FillColor.b = FillColor.Z;
				pSource->FillColor.a = 255.0f;
				pSource->AmbientColor.r = AmbientColor.X;
				pSource->AmbientColor.g = AmbientColor.Y;
				pSource->AmbientColor.b = AmbientColor.Z;
				pSource->AmbientColor.a = 255.0f;
				CCD->ActorManager()->SetActorDynamicLighting(pSource->Actor, pSource->FillColor, pSource->AmbientColor);
				Vector = AttrFile.GetValue(KeyName, "idle");
				pSource->AnimIdle = NULL;
				if(Vector!="")
					pSource->AnimIdle = strdup(Vector);
				Vector = AttrFile.GetValue(KeyName, "alert");
				pSource->AnimAlert = NULL;
				if(Vector!="")
					pSource->AnimAlert = strdup(Vector);
				Vector = AttrFile.GetValue(KeyName, "rotate");
				pSource->AnimRotate = NULL;
				if(Vector!="")
					pSource->AnimRotate = strdup(Vector);
				Vector = AttrFile.GetValue(KeyName, "walk");
				pSource->AnimWalk = NULL;
				if(Vector!="")
					pSource->AnimWalk = strdup(Vector);
				Vector = AttrFile.GetValue(KeyName, "backup");
				pSource->AnimBackup = NULL;
				if(Vector!="")
					pSource->AnimBackup = strdup(Vector);
				Vector = AttrFile.GetValue(KeyName, "strafeleft");
				pSource->AnimStrafeLeft = NULL;
				if(Vector!="")
					pSource->AnimStrafeLeft = strdup(Vector);
				Vector = AttrFile.GetValue(KeyName, "strafeleft");
				pSource->AnimStrafeRight = NULL;
				if(Vector!="")
					pSource->AnimStrafeRight = strdup(Vector);
				Vector = AttrFile.GetValue(KeyName, "jump");
				pSource->AnimJump = NULL;
				if(Vector!="")
					pSource->AnimJump = strdup(Vector);
				Vector = AttrFile.GetValue(KeyName, "lightinjury");
				pSource->AnimLInjury = NULL;
				if(Vector!="")
					pSource->AnimLInjury = strdup(Vector);
				Vector = AttrFile.GetValue(KeyName, "mediuminjury");
				pSource->AnimMInjury = NULL;
				if(Vector!="")
					pSource->AnimMInjury = strdup(Vector);
				Vector = AttrFile.GetValue(KeyName, "heavyinjury");
				pSource->AnimHInjury = NULL;
				if(Vector!="")
					pSource->AnimHInjury = strdup(Vector);
				Vector = AttrFile.GetValue(KeyName, "die");
				pSource->AnimDie = NULL;
				if(Vector!="")
					pSource->AnimDie = strdup(Vector);
				pSource->LInjuryLimit = (float)AttrFile.GetValueF(KeyName, "lightinjurylimit");
				pSource->MInjuryLimit = (float)AttrFile.GetValueF(KeyName, "mediuminjurylimit");
			}
			return;
		}
		KeyName = AttrFile.FindNextKey();
	}
}

void CEnemy::BuildAI(Enemy *pSource)
{
	CString KeyName = AttrFile.FindFirstKey();
	CString Type, Vector;
	int i, j;
	AIData *Data = (AIData *)pSource->Data;
	for(i = 0;i < AIMAX; i++)
	{
		for(j = 0;j < STATEMAX; j++)
			Data->FSM[i].Item[j].active = false;
	}
	while(KeyName != "")
	{
		if(!strcmp(KeyName,pSource->AIName))
		{
			Type = AttrFile.GetValue(KeyName, "type");
			if(Type=="ai")
			{
				Type = AttrFile.GetValue(KeyName, "initialstate");
				Data->InitialState = AIIDLE;
				for(i = 0;i < AIMAX; i++)
				{
					if(!strcmp(Type, StateText[i]))
					{
						Data->InitialState = i;
						break;
					}
				}
				Type = AttrFile.GetValue(KeyName, "samereaction");
				Data->SameReaction = RIGNORE;
				for(i = 0;i < REACTMAX; i++)
				{
					if(!strcmp(Type, Reaction[i]))
					{
						Data->SameReaction = i;
						break;
					}
				}
				Type = AttrFile.GetValue(KeyName, "otherreaction");
				Data->SameReaction = RIGNORE;
				for(i = 0;i < REACTMAX; i++)
				{
					if(!strcmp(Type, Reaction[i]))
					{
						Data->OtherReaction = i;
						break;
					}
				}
				Type = AttrFile.GetValue(KeyName, "playerreaction");
				Data->SameReaction = RIGNORE;
				for(i = 0;i < REACTMAX; i++)
				{
					if(!strcmp(Type, Reaction[i]))
					{
						Data->PlayerReaction = i;
						break;
					}
				}
				Data->SightDistance = (float)AttrFile.GetValueF(KeyName, "sightdistance");

				Data->FOV = ((float)AttrFile.GetValueF(KeyName, "fieldofview"))/2.0f;
				Data->FOV = (90.0f-Data->FOV)/90.0f;

				Data->RotationAmt = (float)AttrFile.GetValueF(KeyName, "alertrotation");
				Data->RotationSpeed = (float)AttrFile.GetValueF(KeyName, "rotationspeed");
				Data->GoalTimeOut = (float)AttrFile.GetValueF(KeyName, "goaltimeout");
				Data->UnStickTimeOut = (float)AttrFile.GetValueF(KeyName, "unsticktimeout");
				Data->YRange = (float)AttrFile.GetValueF(KeyName, "trackheightrange");

				i = AIIDLE;
				if(AINextState((void *)Data, i, STATETRIGGER, "idletriggerstate", KeyName))
					Data->FSM[i].Item[STATETRIGGER].Value.trigger = true;
				if(AINextState((void *)Data, i, STATEMODETIME, "idlemodetimestate", KeyName))
					Data->FSM[i].Item[STATEMODETIME].Value.modetime = (float)AttrFile.GetValueF(KeyName, "idlemodetime");
				AINextState((void *)Data, i, STATEATTACKED, "idleattackedstate", KeyName);
				if(AINextState((void *)Data, i, STATESEETARGET, "idleseetarget", KeyName))
					Data->FSM[i].Item[STATESEETARGET].Value.modetime = (float)AttrFile.GetValueF(KeyName, "idleseetargettime");
				if(AINextState((void *)Data, i, STATELOOSETARGET, "idleloosetarget", KeyName))
					Data->FSM[i].Item[STATELOOSETARGET].Value.modetime = (float)AttrFile.GetValueF(KeyName, "idleloosetargettime");
				if(AINextState((void *)Data, i, STATETARGETDIST, "idletargetrange", KeyName))
					Data->FSM[i].Item[STATETARGETDIST].Value.distance = (float)AttrFile.GetValueF(KeyName, "idletargetrangedist");

				i = AIALERT;
				if(AINextState((void *)Data, i, STATETRIGGER, "alerttriggerstate", KeyName))
					Data->FSM[i].Item[STATETRIGGER].Value.trigger = true;
				if(AINextState((void *)Data, i, STATEMODETIME, "alertmodetimestate", KeyName))
					Data->FSM[i].Item[STATEMODETIME].Value.modetime = (float)AttrFile.GetValueF(KeyName, "alertmodetime");
				AINextState((void *)Data, i, STATEATTACKED, "alertattackedstate", KeyName);
				if(AINextState((void *)Data, i, STATESEETARGET, "alertseetarget", KeyName))
					Data->FSM[i].Item[STATESEETARGET].Value.modetime = (float)AttrFile.GetValueF(KeyName, "alertseetargettime");
				if(AINextState((void *)Data, i, STATELOOSETARGET, "alertloosetarget", KeyName))
					Data->FSM[i].Item[STATELOOSETARGET].Value.modetime = (float)AttrFile.GetValueF(KeyName, "alertloosetargettime");
				if(AINextState((void *)Data, i, STATETARGETDIST, "alerttargetrange", KeyName))
					Data->FSM[i].Item[STATETARGETDIST].Value.distance = (float)AttrFile.GetValueF(KeyName, "alerttargetrangedist");

				i = AISEARCH;
				if(AINextState((void *)Data, i, STATETRIGGER, "searchtriggerstate", KeyName))
					Data->FSM[i].Item[STATETRIGGER].Value.trigger = true;
				if(AINextState((void *)Data, i, STATEMODETIME, "searchmodetimestate", KeyName))
					Data->FSM[i].Item[STATEMODETIME].Value.modetime = (float)AttrFile.GetValueF(KeyName, "searchmodetime");
				AINextState((void *)Data, i, STATEATTACKED, "searchattackedstate", KeyName);
				if(AINextState((void *)Data, i, STATESEETARGET, "searchseetarget", KeyName))
					Data->FSM[i].Item[STATESEETARGET].Value.modetime = (float)AttrFile.GetValueF(KeyName, "searchseetargettime");
				if(AINextState((void *)Data, i, STATELOOSETARGET, "searchloosetarget", KeyName))
					Data->FSM[i].Item[STATELOOSETARGET].Value.modetime = (float)AttrFile.GetValueF(KeyName, "searchloosetargettime");
				if(AINextState((void *)Data, i, STATETARGETDIST, "searchtargetrange", KeyName))
					Data->FSM[i].Item[STATETARGETDIST].Value.distance = (float)AttrFile.GetValueF(KeyName, "searchtargetrangedist");

				i = AIPATROL;
				if(AINextState((void *)Data, i, STATETRIGGER, "patroltriggerstate", KeyName))
					Data->FSM[i].Item[STATETRIGGER].Value.trigger = true;
				if(AINextState((void *)Data, i, STATEMODETIME, "patrolmodetimestate", KeyName))
					Data->FSM[i].Item[STATEMODETIME].Value.modetime = (float)AttrFile.GetValueF(KeyName, "patrolmodetime");
				AINextState((void *)Data, i, STATEATTACKED, "patrolattackedstate", KeyName);
				if(AINextState((void *)Data, i, STATESEETARGET, "patrolseetarget", KeyName))
					Data->FSM[i].Item[STATESEETARGET].Value.modetime = (float)AttrFile.GetValueF(KeyName, "patrolseetargettime");
				if(AINextState((void *)Data, i, STATELOOSETARGET, "patrolloosetarget", KeyName))
					Data->FSM[i].Item[STATELOOSETARGET].Value.modetime = (float)AttrFile.GetValueF(KeyName, "patrolloosetargettime");
				if(AINextState((void *)Data, i, STATETARGETDIST, "patroltargetrange", KeyName))
					Data->FSM[i].Item[STATETARGETDIST].Value.distance = (float)AttrFile.GetValueF(KeyName, "patroltargetrangedist");
			}
			return;
		}
		KeyName = AttrFile.FindNextKey();
	}
}

bool CEnemy::AINextState(void *VData, int state, int input, char *key, CString KeyName)
{
	AIData *Data = (AIData *)VData;
	CString Type = AttrFile.GetValue(KeyName, key);
	if(Type!="")
	{
		Data->FSM[state].Item[input].nextstate = state;
		for(int i = 0;i < AIMAX; i++)
		{
			if(!strcmp(Type, StateText[i]))
			{
				Data->FSM[state].Item[input].nextstate = i;
				break;
			}
		}
		Data->FSM[state].Item[input].active = true;
		return true;
	}
	return false;
}

//	Tick
//

void CEnemy::Tick(float dwTicks)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	EnemyList *pool;

	//	Ok, check to see if there are NonPlayerCharacters in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Enemy");
	
	if(pSet) 
	{
		Bottom = NULL;
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
			Enemy *pSource = (Enemy*)geEntity_GetUserData(pEntity);
			if(pSource->active)
			{
				pool = GE_RAM_ALLOCATE_STRUCT(EnemyList);
				memset(pool, 0x0, sizeof(EnemyList));
				pool->next = Bottom;
				Bottom = pool;
				if(pool->next)
					pool->next->prev = pool;
				pool->Actor = pSource->Actor;
				pool->AIName=strdup(pSource->AIName);
			}
		}
		
		pSet = geWorld_GetEntitySet(CCD->World(), "Enemy");
		
		if(pSet) 
		{
			
			//	Ok, we have NonPlayerCharacters somewhere.  Dig through 'em all.
			
			for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
			pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
			{
				Enemy *pSource = (Enemy*)geEntity_GetUserData(pEntity);
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
				CPersistentAttributes *Inv = (CPersistentAttributes *)pSource->theInv;
				if(Inv->Value(pSource->Attribute)<=0 && !pSource->Dying)
				{
					pSource->Dying = true;
					CCD->ActorManager()->SetNoCollide(pSource->Actor);
					CCD->ActorManager()->SetMotion(pSource->Actor, pSource->AnimDie);
					CCD->ActorManager()->SetHoldAtEnd(pSource->Actor, true);
				}
				if(pSource->Dying)
				{
					if(CCD->ActorManager()->EndAnimation(pSource->Actor))
					{
						geFloat fAlpha;
						CCD->ActorManager()->GetAlpha(pSource->Actor, &fAlpha);
						fAlpha -= (pSource->AlphaStep*(dwTicks/1000.0f));
						CCD->ActorManager()->SetAlpha(pSource->Actor, fAlpha);
						pSource->DyingTime -= (dwTicks/1000.0f);
						if(pSource->DyingTime <=0.0f)
						{
							CCD->ActorManager()->GetPosition(pSource->Actor, &pSource->Location);
							if(pSource->Actor)
							{
								CCD->ActorManager()->RemoveActor(pSource->Actor);
								geActor_Destroy(&pSource->Actor);
							}
							pSource->Actor = NULL;
							pSource->alive=GE_FALSE;
							pSource->bState = GE_TRUE;
							pSource->CallBack = GE_FALSE;
							pSource->Dying = GE_FALSE;
							pSource->Tick = 0.0f;
						}
					}
				}
				else
				{
					ProcessAI(pSource, dwTicks);
					Animate(pSource, dwTicks);
				}
				
			}
		}
		
		EnemyList *temp;
		pool = Bottom;
		while	(pool!= NULL)
		{
			temp = pool->next;
			free(pool->AIName);
			geRam_Free(pool);
			pool = temp;
		}
	}

	return;
}

void CEnemy::Spawn(Enemy *pSource)
{
	pSource->Actor = CCD->ActorManager()->SpawnActor(pSource->ActorName, 
		pSource->origin, pSource->Rotation, pSource->AnimIdle, pSource->AnimIdle, NULL);
	CCD->ActorManager()->SetActorDynamicLighting(pSource->Actor, pSource->FillColor, pSource->AmbientColor);
	CCD->ActorManager()->SetShadow(pSource->Actor, pSource->ShadowSize);
	CCD->ActorManager()->SetScale(pSource->Actor, pSource->Scale);
	CCD->ActorManager()->SetType(pSource->Actor, ENTITY_NPC);
	CCD->ActorManager()->SetAutoStepUp(pSource->Actor, true);
	CCD->ActorManager()->SetBoxChange(pSource->Actor, false);
	CCD->ActorManager()->SetBoundingBox(pSource->Actor, pSource->AnimIdle);
	CCD->ActorManager()->SetGravity(pSource->Actor, CCD->Player()->GetGravity());
	pSource->theInv = (void *)CCD->ActorManager()->Inventory(pSource->Actor);
	CPersistentAttributes *Inv = (CPersistentAttributes *)pSource->theInv;
	Inv->AddAndSet(pSource->Attribute,pSource->AttributeAmt);
	Inv->SetValueLimits(pSource->Attribute, 0, pSource->AttributeAmt);
	AIData *Data = (AIData *)pSource->Data;
	Data->State = Data->InitialState;
	Data->ModeTime = 0.0f;
	Data->NotSeenTime = 0.0f;
	Data->SeenTime = 0.0f;
	Data->GoalTime = 0.0f;
	Data->LastHealth = pSource->AttributeAmt;
	Data->TargetActor = NULL;
	Data->CurrentRotation = 0.0f;
	Data->RotateLeft = true;
	Data->GotGoal = false;
	Data->NextBackupState = 1;
	Data->Dir = DIR_NONE;
	//StackInit(&Data->TrackStack);
	ClearTrack(pSource);
	StackInit(&Data->TrackStack);
}

void CEnemy::Animate(Enemy *pSource, float dwTicks)
{
	char *Motion;
	AIData *Data = (AIData *)pSource->Data;

	Motion = CCD->ActorManager()->GetMotion(pSource->Actor);
	switch(pSource->Action)
	{
	case NPCIDLE:
		if(strcmp(Motion, pSource->AnimIdle))
		{
			CCD->ActorManager()->SetMotion(pSource->Actor, pSource->AnimIdle);
		}
		break;

	case NPCALERT:
		if(strcmp(Motion, pSource->AnimAlert))
		{
			CCD->ActorManager()->SetMotion(pSource->Actor, pSource->AnimAlert);
		}
		break;

	case NPCLINJURY:
		if(strcmp(Motion, pSource->AnimLInjury))
		{
			CCD->ActorManager()->SetMotion(pSource->Actor, pSource->AnimLInjury);
			CCD->ActorManager()->SetHoldAtEnd(pSource->Actor, true);
			break;
		}
		if(CCD->ActorManager()->EndAnimation(pSource->Actor))
		{
			pSource->EndAnimFlag = true;
			CCD->ActorManager()->SetHoldAtEnd(pSource->Actor, false);
		}
		break;
	case NPCMINJURY:
		if(strcmp(Motion, pSource->AnimMInjury))
		{
			CCD->ActorManager()->SetMotion(pSource->Actor, pSource->AnimMInjury);
			CCD->ActorManager()->SetHoldAtEnd(pSource->Actor, true);
			break;
		}
		if(CCD->ActorManager()->EndAnimation(pSource->Actor))
		{
			pSource->EndAnimFlag = true;
			CCD->ActorManager()->SetHoldAtEnd(pSource->Actor, false);
		}
		break;
	case NPCHINJURY:
		if(strcmp(Motion, pSource->AnimHInjury))
		{
			CCD->ActorManager()->SetMotion(pSource->Actor, pSource->AnimHInjury);
			CCD->ActorManager()->SetHoldAtEnd(pSource->Actor, true);
			break;
		}
		if(CCD->ActorManager()->EndAnimation(pSource->Actor))
		{
			pSource->EndAnimFlag = true;
			CCD->ActorManager()->SetHoldAtEnd(pSource->Actor, false);
		}
		break;

	case NPCSEARCHWALK:
		if(strcmp(Motion, pSource->AnimWalk))
		{
			CCD->ActorManager()->SetMotion(pSource->Actor, pSource->AnimWalk);
		}
		break;
	case NPCSEARCHROTATE:
		if(strcmp(Motion, pSource->AnimRotate))
		{
			CCD->ActorManager()->SetMotion(pSource->Actor, pSource->AnimRotate);
		}
		break;
	case NPCSEARCHBACKUP:
		if(strcmp(Motion, pSource->AnimBackup) && Data->BackupState == 0)
		{
			CCD->ActorManager()->SetMotion(pSource->Actor, pSource->AnimBackup);
			break;
		}
		if(strcmp(Motion, pSource->AnimStrafeLeft) && Data->BackupState == 1)
		{
			CCD->ActorManager()->SetMotion(pSource->Actor, pSource->AnimStrafeLeft);
			break;
		}
		if(strcmp(Motion, pSource->AnimStrafeRight) && Data->BackupState == 2)
		{
			CCD->ActorManager()->SetMotion(pSource->Actor, pSource->AnimStrafeRight);
			break;
		}
		break;
	case NPCSEARCHNOGOAL:
	case NPCSEARCHUNSTICK:
		if(strcmp(Motion, pSource->AnimBackup) && Data->SubAction == SUBWALK)
		{
			CCD->ActorManager()->SetMotion(pSource->Actor, pSource->AnimBackup);
			break;
		}
		if(strcmp(Motion, pSource->AnimStrafeLeft) && Data->SubAction == SUBSLIDELEFT)
		{
			CCD->ActorManager()->SetMotion(pSource->Actor, pSource->AnimStrafeLeft);
			break;
		}
		if(strcmp(Motion, pSource->AnimStrafeRight) && Data->SubAction == SUBSLIDERIGHT)
		{
			CCD->ActorManager()->SetMotion(pSource->Actor, pSource->AnimStrafeRight);
			break;
		}
		if(strcmp(Motion, pSource->AnimRotate) && Data->SubAction == SUBROTATE)
		{
			CCD->ActorManager()->SetMotion(pSource->Actor, pSource->AnimRotate);
			break;
		}
		break;
	}
}

void CEnemy::ProcessAI(Enemy *pSource, float dwTicks)
{
	int i;
	float amount;
	bool flag;
	AIData *Data = (AIData *)pSource->Data;
	Data->ModeTime += (dwTicks/1000.0f);
	Data->SeenTime += (dwTicks/1000.0f);
	Data->NotSeenTime += (dwTicks/1000.0f);
	Data->GoalTime += (dwTicks/1000.0f);
	Data->UnStickTime += (dwTicks/1000.0f);

	for(i=0;i<STATEMAX;i++)
	{
		Data->Inputs[i].active = false;
	}

	if(!EffectC_IsStringNull(pSource->ActivateTrigger))
	{
		Data->Inputs[STATETRIGGER].Value.trigger = GetTriggerState(pSource->ActivateTrigger);
		Data->Inputs[STATETRIGGER].active = true;
	}

	Data->Inputs[STATEMODETIME].Value.modetime = Data->ModeTime;
	Data->Inputs[STATEMODETIME].active = true;

	CPersistentAttributes *Inv = (CPersistentAttributes *)pSource->theInv;
	int health = Inv->Value(pSource->Attribute);
	if(health<Data->LastHealth)
	{
		Data->Inputs[STATEATTACKED].Value.healthdiff = (Data->LastHealth-health);
		Data->Inputs[STATEATTACKED].active = true;
	}
	Data->LastHealth = health;

	CheckTarget(pSource);

	if(!(Data->State==AILINJURY || Data->State==AIMINJURY || Data->State==AIHINJURY))
	{
		Data->LastState = Data->State;
		ProcessStateMachine(pSource);
	}

	if(Data->State==AIIDLE)
	{
		pSource->Action = NPCIDLE;
		
		return;
	}
	if(Data->State==AIALERT)
	{
		if(pSource->Action != NPCALERT)
			Data->CurrentRotation = 0.0f;
		pSource->Action = NPCALERT;
		if(Data->RotationAmt>0.0f && Data->RotationSpeed>0.0f)
		{
			amount = Data->RotationSpeed * (dwTicks/1000.0f);
			flag = false;
			if((Data->CurrentRotation+amount)>Data->RotationAmt)
			{
				amount = Data->RotationAmt - Data->CurrentRotation;
				flag = true;
			}
			Data->CurrentRotation += amount;
			if(Data->RotateLeft)
			{	
				CCD->ActorManager()->TurnLeft(pSource->Actor,0.0174532925199433f*amount);
				if(flag)
				{
					Data->RotateLeft = false;
					Data->CurrentRotation = -Data->CurrentRotation;
				}
			}
			else
			{		
				CCD->ActorManager()->TurnRight(pSource->Actor,0.0174532925199433f*amount);
				if(flag)
				{
					Data->RotateLeft = true;
					Data->CurrentRotation = -Data->CurrentRotation;
				}
			}
		}
		return;
	}
	if(Data->State==AILINJURY)
	{
		if(pSource->Action != NPCLINJURY)
		{
			pSource->EndAnimFlag = false;
			pSource->Action = NPCLINJURY;
		}
		if(pSource->EndAnimFlag)
		{
			Data->State = Data->InjuryNextState;
			Data->ModeTime = 0.0f;
		}
		return;
	}
	if(Data->State==AIMINJURY)
	{
		if(pSource->Action != NPCMINJURY)
		{
			pSource->EndAnimFlag = false;
			pSource->Action = NPCMINJURY;
		}
		if(pSource->EndAnimFlag)
		{
			Data->State = Data->InjuryNextState;
			Data->ModeTime = 0.0f;
		}
		return;
	}
	if(Data->State==AIHINJURY)
	{
		if(pSource->Action != NPCHINJURY)
		{
			pSource->EndAnimFlag = false;
			pSource->Action = NPCHINJURY;
		}
		if(pSource->EndAnimFlag)
		{
			Data->State = Data->InjuryNextState;
			Data->ModeTime = 0.0f;
		}
		return;
	}
	if(Data->State==AISEARCH)
	{
		// initalize search state
		if(Data->LastState!=Data->State)
		{
			pSource->Action = NPCSEARCHROTATE;
			Data->SlideDir = false;
			Data->SubAction = SUBNULL;
			if(Data->NextBackupState==1)
				Data->NextBackupState=2;
			else
				Data->NextBackupState=1;
			Data->SearchMove = false;
			Data->SearchCnt = 0;
		}
		// have a goal
		if(AquireGoal(pSource) && pSource->Action!=NPCSEARCHUNSTICK)
		{
			if(pSource->Action==NPCSEARCHNOGOAL)
				pSource->Action=NPCSEARCHROTATE;
			RotateToGoal(pSource, (pSource->Action==NPCSEARCHROTATE || pSource->Action==NPCSEARCHWALK), dwTicks);
			// see if finished rotate to face goal mode
			if(Data->RotateAmt == 0.0f && pSource->Action==NPCSEARCHROTATE)
				pSource->Action = NPCSEARCHWALK;
			// walk toward goal
			if(pSource->Action==NPCSEARCHWALK)
			{
				Data->Dir = DIR_TOWARD;
				geVec3d Direction;
				CCD->ActorManager()->InVector(pSource->Actor, &Direction);
				// test if can move
				if(MoveScan(pSource, Direction, pSource->Speed))
				{
					CCD->ActorManager()->MoveForward(pSource->Actor, pSource->Speed*(dwTicks/1000.0f));
				}
				else
				{
					// can't move so switch to backup and slide
					CCD->ActorManager()->SetSlide(pSource->Actor, false);
					pSource->Action = NPCSEARCHBACKUP;
					Data->Dir = DIR_AWAY;
					Data->BackupState = 0;
					Data->BackupDist = 0.0f;
					Data->SearchCnt += 1;
					// if can't slide one way switch to other
					if(Data->SearchCnt>1)
					{
						if(Data->NextBackupState==1)
							Data->NextBackupState=2;
						else
							Data->NextBackupState=1;
						Data->SearchCnt = 0;
					}
					return;
				}
			}
			// backup and slide to get get to goal
			if(pSource->Action==NPCSEARCHBACKUP)
			{
				// backup 1/2 second
				if(Data->BackupDist<=(pSource->Speed/2.0f))
				{
					Data->BackupDist += pSource->Speed*(dwTicks/1000.0f);
					CCD->ActorManager()->MoveBackward(pSource->Actor, pSource->Speed*(dwTicks/1000.0f));
					return;
				}
				// switch to sliding
				if(Data->BackupState==0)
				{
					Data->BackupState = Data->NextBackupState;
				}
				// slide for 1.5 seconds
				if(Data->BackupDist<=(pSource->Speed+(pSource->Speed/2.0f)))
				{
					int result;
					Data->BackupDist += pSource->Speed*(dwTicks/1000.0f);
					if(Data->BackupState==1)
						result = CCD->ActorManager()->MoveLeft(pSource->Actor, pSource->Speed*(dwTicks/1000.0f));
					else
						result = CCD->ActorManager()->MoveRight(pSource->Actor, pSource->Speed*(dwTicks/1000.0f));
					// unable to slide in direction
					if(result==RGF_FAILURE)
					{
						CCD->ActorManager()->SetSlide(pSource->Actor, true);
						// stopped in both directions
						if(Data->SearchMove)
						{
							// go to unstick mode
							pSource->Action = NPCSEARCHUNSTICK;
							Data->Dir = DIR_NONE;
							Data->SubAction = SUBNULL;
							Data->UnStickTime = 0.0f;
							Data->SearchMove = false;
							Data->SearchCnt = 0;
							return;
						}
						// face goal and try other direction
						pSource->Action = NPCSEARCHROTATE;
						Data->SearchMove = true;
					}
				}
				else
				{
					// face goal and try to move toward it again
					pSource->Action = NPCSEARCHROTATE;
					Data->SearchCnt = 0;
					CCD->ActorManager()->SetSlide(pSource->Actor, true);
				}
			}
		}
		else
		{
			if(pSource->Action==NPCSEARCHUNSTICK)
			{
				// timed out, retry searching
				if(Data->UnStickTime>Data->UnStickTimeOut)
				{
					pSource->Action = NPCSEARCHROTATE;
					Data->SubAction = SUBNULL;
					return;
				}
				CoffeeMove(pSource, dwTicks);
				Data->GoalTime = 0.0f;
				return;
			}
			// move random direction with no goal
			pSource->Action = NPCSEARCHNOGOAL;
			Data->Dir = DIR_NONE;
			CoffeeMove(pSource, dwTicks);
		}
		
		return;
	}
	if(Data->State==AIPATROL)
	{
		// initalize search state
		if(Data->LastState!=Data->State)
		{
			Track	*GoalTrack;
			geVec3d thePosition;

			int32 WalkTrackList[] =
			{
				TRACK_TYPE_SCAN_LOOP,
				TRACK_TYPE_SCAN,
				TRACK_TYPE_TRAVERSE,
				-1
			};

			pSource->Action = NPCSEARCHROTATE;
			CCD->ActorManager()->GetPosition(pSource->Actor, &thePosition);
			if (GoalTrack = FindRandomTrack(&thePosition, WalkTrackList))
			{
				thePosition = *GoalTrack->PointList[0].Pos;
				char szBug[256];
				sprintf(szBug, "Goal %f %f %f", thePosition.X,thePosition.Y,thePosition.Z);
				CCD->ReportError(szBug, false);	
				if(!InitFindMultiTrack(pSource, GoalTrack->PointList[0].Pos))
				{
					//Data->GoalPos = *GoalTrack->PointList[0].Pos;
					pSource->Action = NPCSEARCHNOGOAL;
				}
			}
			else
				pSource->Action = NPCSEARCHNOGOAL;
		}

		RotateToGoal(pSource, (pSource->Action==NPCSEARCHROTATE || pSource->Action==NPCSEARCHWALK), dwTicks);
		// see if finished rotate to face goal mode
		if(Data->RotateAmt == 0.0f && pSource->Action==NPCSEARCHROTATE)
			pSource->Action = NPCSEARCHWALK;
		if(pSource->Action == NPCSEARCHWALK)
		{
			Data->Dir = DIR_TOWARD;
			geVec3d Direction, thePosition;
			CCD->ActorManager()->InVector(pSource->Actor, &Direction);
			CCD->ActorManager()->GetPosition(pSource->Actor, &thePosition);
			if(PastPoint(&thePosition, &Direction, &Data->GoalPos))
			{
				TrackPt *NextPoint = NULL;
				TrackPt *ThisPoint = CCD->Track()->Track_GetPoint(&Data->TrackInfo);
				Data->TrackInfoPrev = Data->TrackInfo;
				if (NextPoint = CCD->Track()->Track_NextPoint(&Data->TrackInfo))
				{
					Data->GoalPos = *NextPoint->Pos;
					pSource->Action = NPCSEARCHROTATE;
				}
				else
				{
					FinishTrack(pSource);
					if (StackIsEmpty(&Data->TrackStack))
					{
						pSource->Action = NPCSEARCHNOGOAL;
					}
				}
			}
			else
			{
				CCD->ActorManager()->MoveForward(pSource->Actor, pSource->Speed*(dwTicks/1000.0f));
			}
		}
		if(pSource->Action == NPCSEARCHNOGOAL)
		{
			Data->Dir = DIR_NONE;
			CoffeeMove(pSource, dwTicks);
		}
	}
}

void CEnemy::RotateToGoal(Enemy *pSource, bool CanRotate, float dwTicks)
{
	AIData *Data = (AIData *)pSource->Data;
	geVec3d Pos, Orient;
	CCD->ActorManager()->GetPosition(pSource->Actor, &Pos);
	geVec3d_Subtract(&Data->GoalPos,&Pos,&Orient);
	float l = geVec3d_Length(&Orient);
	Data->RotateAmt = 0.0f;
	// rotate to face goal if moving or in face goal mode
	if(l > 0.0f && CanRotate) 
	{
		Orient.Y = (float)atan2(Orient.X , Orient.Z ) + GE_PI;
		CCD->ActorManager()->GetRotate(pSource->Actor, &Pos);
		while(Pos.Y<0.0f)
		{
			Pos.Y+=(GE_PI*2);
		}
		while(Pos.Y>(GE_PI*2))
		{
			Pos.Y-=(GE_PI*2);
		}
		Data->RotateLeft = false;
		Data->RotateAmt = Pos.Y - Orient.Y;
		if(Data->RotateAmt<0.0f)
		{
			Data->RotateAmt = -Data->RotateAmt;
			Data->RotateLeft = true;
		}
		if(Data->RotateAmt>GE_PI)
		{
			Data->RotateAmt-=GE_PI;
			if(Data->RotateLeft)
				Data->RotateLeft=false;
			else
				Data->RotateLeft=true;
		}
		Data->RotateAmt /= 0.0174532925199433f;
		float amount = Data->RotationSpeed * (dwTicks/1000.0f) * 2.0f;
		if(amount>Data->RotateAmt)
		{
			amount = Data->RotateAmt;
		}
		Data->RotateAmt -= amount;
		if(Data->RotateLeft)
			CCD->ActorManager()->TurnRight(pSource->Actor,0.0174532925199433f*amount);
		else
			CCD->ActorManager()->TurnLeft(pSource->Actor,0.0174532925199433f*amount);
	}
}

void CEnemy::CoffeeMove(Enemy *pSource, float dwTicks)
{
	AIData *Data = (AIData *)pSource->Data;

	if(Data->SubAction==SUBROTATE)
	{
		float amount = Data->RotationSpeed * (dwTicks/1000.0f) * 2.0f;
		if(amount>Data->CurrentRotation)
			amount =  Data->CurrentRotation;
		Data->CurrentRotation -= amount;
		if(Data->SlideDir)
			CCD->ActorManager()->TurnLeft(pSource->Actor,0.0174532925199433f*amount);
		else
			CCD->ActorManager()->TurnRight(pSource->Actor,0.0174532925199433f*amount);
		if(Data->CurrentRotation<=0.0f)
		{
			Data->SubAction = SUBNULL;
		}
		return; 
	}

	geVec3d Direction;
	CCD->ActorManager()->InVector(pSource->Actor, &Direction);
	geVec3d_Inverse(&Direction);
	// test if can move
	if(MoveScan(pSource, Direction, pSource->Speed))
	{
		CCD->ActorManager()->MoveBackward(pSource->Actor, pSource->Speed*(dwTicks/1000.0f));
		Data->SubAction = SUBWALK;
	}
	else
	{
		int result;
		if(Data->SlideDir)
		{
			result = CCD->ActorManager()->MoveLeft(pSource->Actor, pSource->Speed*(dwTicks/1000.0f));
			Data->SubAction = SUBSLIDELEFT;
		}
		else
		{
			result = CCD->ActorManager()->MoveRight(pSource->Actor, pSource->Speed*(dwTicks/1000.0f));
			Data->SubAction = SUBSLIDERIGHT;
		}
		if(result==RGF_FAILURE)
		{
			Data->CurrentRotation = 45.0f;
			Data->SubAction=SUBROTATE;
			return;
		}
		if(rand()%100<5)
			Data->SlideDir = !Data->SlideDir;
	}
}

bool CEnemy::MoveScan(Enemy *pSource, geVec3d dir, float dist)
{
	geVec3d	Pos, TgtPos;
	GE_Collision Collision;

	CCD->ActorManager()->GetPosition(pSource->Actor, &Pos);
	geVec3d_AddScaled(&Pos, &dir, dist, &TgtPos);
	CCD->Collision()->IgnoreContents(false);
	CCD->Collision()->CheckLevel(RGF_COLLISIONLEVEL_1);
	bool Result = CCD->Collision()->CheckActorCollision(Pos, TgtPos, pSource->Actor, &Collision);
	if(Result)
		return false;
	for (int i = 4; i > 0; i--)
	{
		geVec3d_AddScaled(&Pos, &dir, dist/(i*2), &TgtPos);
		if(CCD->ActorManager()->CheckReallyFall(pSource->Actor, TgtPos)==RGF_SUCCESS)
			return false;
	} 
	return true;
}

void CEnemy::CheckTarget(Enemy *pSource)
{
		
	geActor	*TargetActor = NULL;
	float Distance = 99999.0f;
	geVec3d Pos, TgtPos;
	EnemyList *pool;

	AIData *Data = (AIData *)pSource->Data;
	
	CCD->ActorManager()->GetPosition(pSource->Actor, &Pos);
	
	if(Data->TargetActor)
	{
		if(InRange(pSource->Actor, Data->TargetActor, Data->SightDistance, Data->FOV))
		{
			Data->NotSeenTime = 0.0f;
			Data->Inputs[STATESEETARGET].Value.modetime = Data->SeenTime;
			Data->Inputs[STATESEETARGET].active = true;
			CCD->ActorManager()->GetPosition(Data->TargetActor, &TgtPos);
			Distance = geVec3d_DistanceBetween(&Pos, &TgtPos);
			Data->Inputs[STATETARGETDIST].Value.distance = Distance;
			Data->Inputs[STATETARGETDIST].active = true;
		}
		else
		{
			Data->SeenTime = 0.0f;
			Data->TargetActor = NULL;
			Data->Inputs[STATELOOSETARGET].Value.modetime = 0.0f;
			Data->Inputs[STATELOOSETARGET].active = true;
		}
		return;
	}

	switch(Data->PlayerReaction)
	{
	case RIGNORE:
		break;
	case RRANDOM:
		break;
	case RHOSTILE:
		if(InRange(pSource->Actor, CCD->Player()->GetActor(), Data->SightDistance, Data->FOV))
		{
			TargetActor = CCD->Player()->GetActor();
			CCD->ActorManager()->GetPosition(TargetActor, &TgtPos);
			Distance = geVec3d_DistanceBetween(&Pos, &TgtPos);
		}
		break;
	}

	switch(Data->OtherReaction)
	{
	case RIGNORE:
		break;
	case RRANDOM:
		break;
	case RHOSTILE:
		if(Bottom)
		{
			pool=Bottom;
			while ( pool != NULL )
			{
				if(stricmp(pSource->AIName, pool->AIName))
				{
					if(InRange(pSource->Actor, pool->Actor, Data->SightDistance, Data->FOV))
					{
						CCD->ActorManager()->GetPosition(pool->Actor, &TgtPos);
						if(Distance > geVec3d_DistanceBetween(&Pos, &TgtPos))
						{
							Distance = geVec3d_DistanceBetween(&Pos, &TgtPos);
							TargetActor = pool->Actor;
						}
					}
				}
				pool = pool->next;
			}
		}
		break;
	}

	switch(Data->SameReaction)
	{
	case RIGNORE:
		break;
	case RRANDOM:
		break;
	case RHOSTILE:
		if(Bottom)
		{
			pool=Bottom;
			while ( pool != NULL )
			{
				if(!stricmp(pSource->AIName, pool->AIName) && pSource->Actor!=pool->Actor)
				{
					if(InRange(pSource->Actor, pool->Actor, Data->SightDistance, Data->FOV))
					{
						CCD->ActorManager()->GetPosition(pool->Actor, &TgtPos);
						if(Distance > geVec3d_DistanceBetween(&Pos, &TgtPos))
						{
							Distance = geVec3d_DistanceBetween(&Pos, &TgtPos);
							TargetActor = pool->Actor;
						}
					}
				}
				pool = pool->next;
			}
		}
		break;
	}

	if(TargetActor)
	{
		Data->NotSeenTime = 0.0f;
		Data->TargetActor = TargetActor;
		Data->Inputs[STATESEETARGET].Value.modetime = Data->SeenTime;
		Data->Inputs[STATESEETARGET].active = true;
		Data->Inputs[STATETARGETDIST].Value.distance = Distance;
		Data->Inputs[STATETARGETDIST].active = true;
	}
	else
	{
		Data->SeenTime = 0.0f;
		Data->TargetActor = NULL;
		Data->Inputs[STATELOOSETARGET].Value.modetime = Data->NotSeenTime;
		Data->Inputs[STATELOOSETARGET].active = true;
	}
}

bool CEnemy::InRange(geActor *Actor, geActor *Target, float distance, float FOV)
{
	geVec3d Pos, TgtPos, temp, In;
	float dotProduct;

	CCD->ActorManager()->GetPosition(Actor, &Pos);
	CCD->ActorManager()->GetPosition(Target, &TgtPos);

	geVec3d_Subtract(&TgtPos,&Pos,&temp);
	geVec3d_Normalize(&temp);
	CCD->ActorManager()->InVector(Actor, &In);
	dotProduct = geVec3d_DotProduct(&temp,&In);

	if(dotProduct > FOV)
	{
		if(geVec3d_DistanceBetween(&TgtPos, &Pos)>distance)
			return false;
		if(CanSeeActorToActor(Actor, Target))
			return true;
	}
	return false;
}

bool CEnemy::AquireGoal(Enemy *pSource)
{
	AIData *Data = (AIData *)pSource->Data;

	if(Data->TargetActor)
	{
		Data->GotGoal = false;
		CCD->ActorManager()->GetPosition(Data->TargetActor, &Data->GoalPos);
		return true;
	}

	if(Data->GotGoal)
	{
		if(Data->GoalTime<Data->GoalTimeOut)
			return true;
	}
	Data->GoalTime = 0.0f;

	return false;
}

void CEnemy::ProcessStateMachine(Enemy *pSource)
{
	AIData *Data = (AIData *)pSource->Data;

	if(Data->Inputs[STATETRIGGER].active)
	{
		if(Data->FSM[Data->State].Item[STATETRIGGER].active)
		{
			if(Data->Inputs[STATETRIGGER].Value.trigger==Data->FSM[Data->State].Item[STATETRIGGER].Value.trigger)
			{
				Data->State = Data->FSM[Data->State].Item[STATETRIGGER].nextstate;
				Data->ModeTime = 0.0f;
				return;
			}
		}
	}

	if(Data->Inputs[STATEMODETIME].active)
	{
		if(Data->FSM[Data->State].Item[STATEMODETIME].active)
		{
			if(Data->Inputs[STATEMODETIME].Value.modetime>=Data->FSM[Data->State].Item[STATEMODETIME].Value.modetime)
			{
				Data->State = Data->FSM[Data->State].Item[STATEMODETIME].nextstate;
				Data->ModeTime = 0.0f;
				return;
			}
		}
	}
 
	if(Data->Inputs[STATEATTACKED].active)
	{
		if(Data->FSM[Data->State].Item[STATEATTACKED].active)
		{
			if(Data->Inputs[STATEATTACKED].Value.healthdiff>0)
			{
				if(rand()%100>=70)
				{
					Data->InjuryNextState = Data->FSM[Data->State].Item[STATEATTACKED].nextstate;
					if(Data->Inputs[STATEATTACKED].Value.healthdiff<=pSource->LInjuryLimit)
						Data->State = AILINJURY;
					else if(Data->Inputs[STATEATTACKED].Value.healthdiff<=pSource->MInjuryLimit)
						Data->State = AILINJURY;
					else
						Data->State = AIHINJURY;
				}
				else
					Data->State = Data->FSM[Data->State].Item[STATEATTACKED].nextstate;
				Data->ModeTime = 0.0f;
				Data->TargetActor = NULL;
				Data->SeenTime = 0.0f;
				return;
			}
		}
	}
	
	if(Data->Inputs[STATESEETARGET].active)
	{
		if(Data->FSM[Data->State].Item[STATESEETARGET].active)
		{
			if(Data->Inputs[STATESEETARGET].Value.modetime>=Data->FSM[Data->State].Item[STATESEETARGET].Value.modetime)
			{
				Data->State = Data->FSM[Data->State].Item[STATESEETARGET].nextstate;
				Data->ModeTime = 0.0f;
				return;
			}
		}
	}
	
	if(Data->Inputs[STATELOOSETARGET].active)
	{
		if(Data->FSM[Data->State].Item[STATELOOSETARGET].active)
		{
			if(Data->Inputs[STATELOOSETARGET].Value.modetime>=Data->FSM[Data->State].Item[STATELOOSETARGET].Value.modetime)
			{
				Data->State = Data->FSM[Data->State].Item[STATELOOSETARGET].nextstate;
				Data->ModeTime = 0.0f;
				return;
			}
		}
	}

	if(Data->Inputs[STATETARGETDIST].active)
	{
		if(Data->FSM[Data->State].Item[STATETARGETDIST].active)
		{
			if(Data->FSM[Data->State].Item[STATETARGETDIST].Value.distance<0.0f)
			{
				if(Data->Inputs[STATETARGETDIST].Value.distance>=fabs(Data->FSM[Data->State].Item[STATETARGETDIST].Value.distance))
				{
					Data->State = Data->FSM[Data->State].Item[STATETARGETDIST].nextstate;
					Data->ModeTime = 0.0f;
					return;
				}
			}
			if(Data->Inputs[STATETARGETDIST].Value.distance<=Data->FSM[Data->State].Item[STATETARGETDIST].Value.distance)
			{
				Data->State = Data->FSM[Data->State].Item[STATETARGETDIST].nextstate;
				Data->ModeTime = 0.0f;
				return;
			}
		}
	}

}

void CEnemy::ClearTrack(Enemy *pSource)
{
	AIData *Data = (AIData *)pSource->Data;

	CCD->Track()->Track_ClearTrack(&Data->TrackInfoPrev);
	CCD->Track()->Track_ClearTrack(&Data->TrackInfo);
    StackReset(&Data->TrackStack);
}

Track *CEnemy::FindTrack(Enemy *pSource, int32 TrackArr[])
{
	AIData *Data = (AIData *)pSource->Data;
	Track *track;
	TrackData TrackInfo;
	geVec3d			Pos, TgtPos;

	CCD->ActorManager()->GetPosition(pSource->Actor, &Pos);
	TgtPos = Data->GoalPos;
	
	CCD->Track()->Track_ClearTrack(&TrackInfo);
	while (1)
	{
		track = CCD->Track()->Track_FindTrack(&Pos, &TgtPos, 0, TrackArr, &TrackInfo);
		
		if (!track)
			break;
		
		if (ValidateTrackPoints(pSource->Data, track))
		{
			// can't add the same track!
			if (StackTop(&Data->TrackStack) == TrackInfo.TrackNdx)
				continue;
			
			Data->TrackInfo = TrackInfo;
			StackPush(&Data->TrackStack, Data->TrackInfo.TrackNdx);
			Data->PastFirstTrackPoint = false;
			return (track);
		}
	}
	
    return (NULL);
}

int ValidateTrackPoints(void *Data, Track* t)
{
	TrackPt *tp;

    for (tp = t->PointList; tp < &t->PointList[t->PointCount]; tp++)
	{
		switch (tp->Action)
		{
		case POINT_TYPE_WAIT_POINT_VISIBLE:
			break;
			
		case POINT_TYPE_LOOK_FOR_ITEMS:
			{
				// false if can find item in range
				break;
			}
		}
	}
	return GE_TRUE;
}

int ValidateMultiTrackPoints(void *Data, Track* t)
{
	TrackPt *tp;

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

Track *CEnemy::FindTrackToGoal(Enemy *pSource)
{
	AIData *Data = (AIData *)pSource->Data;
    Track *track;
    int32 *type;
    float ydiff;
	TrackData TrackInfo;
	
	geVec3d	Pos, TgtPos;

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
	
	CCD->ActorManager()->GetPosition(pSource->Actor, &Pos);
	TgtPos = Data->GoalPos;
	
    ydiff = TgtPos.Y - Pos.Y;
	
	if (Data->Dir == DIR_TOWARD)
	{
		if (fabs(ydiff) <= Data->YRange)
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
		if (Data->Dir == DIR_AWAY)
		{
			type = Away;
		}
		if (Data->Dir == DIR_NONE)
		{
			type = Away;
		}
		
		CCD->Track()->Track_ClearTrack(&TrackInfo);
		while (1)
		{
			track = CCD->Track()->Track_FindTrack(&Pos, &TgtPos, Data->Dir, type, &TrackInfo);
			
			if (!track)
				return NULL;
			
			if (ValidateTrackPoints(pSource->Data, track))
			{
				// can't add the same track!
				if (StackTop(&Data->TrackStack) == TrackInfo.TrackNdx)
					continue;
				
				Data->TrackInfo = TrackInfo;
				StackPush(&Data->TrackStack, Data->TrackInfo.TrackNdx);
				Data->PastFirstTrackPoint = false;
				return (track);
			}
		}
		return NULL;
}

bool CEnemy::FinishTrack(Enemy *pSource)
{
	AIData *Data = (AIData *)pSource->Data;
	int32			NewTrack;
	geVec3d	Pos;

	CCD->ActorManager()->GetPosition(pSource->Actor, &Pos);
	
	CCD->Track()->Track_ClearTrack(&Data->TrackInfo);
	
	StackPop(&Data->TrackStack);
	if (!StackIsEmpty(&Data->TrackStack))
	{
		NewTrack = StackTop(&Data->TrackStack);
		CCD->Track()->Track_NextMultiTrack(&Pos, NewTrack, &Data->TrackInfo);
		Data->PastFirstTrackPoint = false;
		// now on track
		// set goal to next point
		Data->GoalPos = *CCD->Track()->Track_GetPoint(&Data->TrackInfo)->Pos;
		return true;
	}
	return false;
}

static Track *FindRandomTrack(geVec3d *Pos, int32 *TrackTypeList)
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

bool CEnemy::InitFindMultiTrack(Enemy *pSource, geVec3d *DestPos)
{
	AIData *Data = (AIData *)pSource->Data;
	TrackCB	CB;
	geVec3d thePosition;

	if (!StackIsEmpty(&Data->TrackStack))
		return false;
	
	CB.Data = pSource->Data;
	CB.CB = ValidateMultiTrackPoints;
	CCD->ActorManager()->GetPosition(pSource->Actor, &thePosition);

	CCD->ReportError("Getting MultiTrack", false);

	if (CCD->Track()->Track_FindMultiTrack (&thePosition,
		DestPos,
		DIR_TOWARD,
		&CB,
		&Data->TrackStack))
	{
		CCD->ReportError("Got MultiTrack", false);
		Data->Dir = DIR_TOWARD;
		CCD->Track()->Track_NextMultiTrack(&thePosition, StackTop(&Data->TrackStack), &Data->TrackInfo);
		Data->PastFirstTrackPoint = false;
		Data->GoalPos = *CCD->Track()->Track_GetPoint(&Data->TrackInfo)->Pos;
		return true;
	}
	return false;
}

bool CEnemy::InitFindMultiTrackAway(Enemy *pSource, geVec3d *DestPos)
{
	AIData *Data = (AIData *)pSource->Data;
	TrackCB	CB;
	geVec3d thePosition;

	if (!StackIsEmpty(&Data->TrackStack))
		return false;
	
	CB.Data = pSource->Data;
	CB.CB = ValidateMultiTrackPoints;
	CCD->ActorManager()->GetPosition(pSource->Actor, &thePosition);
	
	if (CCD->Track()->Track_FindMultiTrack (&thePosition,
		DestPos,
		DIR_AWAY,
		&CB,
		&Data->TrackStack))
	{
		Data->Dir = DIR_AWAY;
		CCD->Track()->Track_NextMultiTrack(&thePosition, StackTop(&Data->TrackStack), &Data->TrackInfo);
		Data->PastFirstTrackPoint = false;
		Data->GoalPos = *CCD->Track()->Track_GetPoint(&Data->TrackInfo)->Pos;
		return true;
	}
	return false;
}

bool CEnemy::PastPoint(geVec3d *Pos, geVec3d *MoveVector, geVec3d *TgtPos)
{
	geVec3d Vec2Point, MoveVec = *MoveVector;
	
	// Temp hack
	if (geVec3d_Length(TgtPos) == 0.0f)
		return true;	
	
	// see if went past the point
	geVec3d_Subtract(TgtPos, Pos, &Vec2Point);
	
	// make sure they are 2d for this test
    Vec2Point.Y = 0.0f;
	MoveVec.Y = 0.0f;
	
	geVec3d_Normalize(&Vec2Point);	
    geVec3d_Normalize(&MoveVec);	
	
	return (geVec3d_DotProduct(&Vec2Point, &MoveVec) < 0.0f);
}	

void CEnemy::Debug()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, check to see if there are NPC in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Enemy");
	
	if(pSet) 
	{
		//	Ok, we have NonPlayerCharacters somewhere.  Dig through 'em all.
		
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
			Enemy *pSource = (Enemy*)geEntity_GetUserData(pEntity);
			if(pSource->alive && pSource->active)
			{
				AIData *Data = (AIData *)pSource->Data;
				CPersistentAttributes *Inv = (CPersistentAttributes *)pSource->theInv;
				geVec3d Pos;

				char szData[256];
				sprintf(szData,"Mode : %s", StateText[Data->State]);
				CCD->MenuManager()->FontRect(szData, FONT10, 5, 10);

				sprintf(szData,"ModeTime : %f", Data->ModeTime);
				CCD->MenuManager()->FontRect(szData, FONT10, 5, 30);

				sprintf(szData,"Target Seen Time : %f", Data->SeenTime);
				CCD->MenuManager()->FontRect(szData, FONT10, 5, 50);

				sprintf(szData,"Target NotSeen Time : %f", Data->NotSeenTime);
				CCD->MenuManager()->FontRect(szData, FONT10, 5, 70);

				sprintf(szData,"Action : %s", ActionText[pSource->Action]);
				CCD->MenuManager()->FontRect(szData, FONT10, 5, 90);

				sprintf(szData,"Search Count : %d", Data->SearchCnt);
				CCD->MenuManager()->FontRect(szData, FONT10, 5, 110);

				if(Data->SearchMove)
					sprintf(szData,"Can Move : true");
				else
					sprintf(szData,"Can Move : false");
				CCD->MenuManager()->FontRect(szData, FONT10, 5, 130);

				CCD->ActorManager()->GetPosition(pSource->Actor, &Pos);
				sprintf(szData,"Position : %f %f %f", Pos.X, Pos.Y, Pos.Z);
				CCD->MenuManager()->FontRect(szData, FONT10, 5, 150);
			
				sprintf(szData,"Health : %d", Inv->Value(pSource->Attribute));
				CCD->MenuManager()->FontRect(szData, FONT10, 5, 170);

			}
		}
	}
}

#endif