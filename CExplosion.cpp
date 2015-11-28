/****************************************************************************************/
/*																						*/
/*	Explosion Manager.cpp               Explosion  Manager handler						*/
/*																						*/
/*	(c) 1999 Ralph Deane																*/
/*																						*/
/*	This file contains the class implementation for the Explosion						*/
/*	Manager that handles Explosions for RGF-based games.								*/
/*																						*/
/****************************************************************************************/

//	Include the One True Header
#include "RabidFramework.h"
#include <Ram.h>

/* ------------------------------------------------------------------------------------ */
//	Constructor
/* ------------------------------------------------------------------------------------ */
CExplosionInit::CExplosionInit()
{
	int i;

	for(i=0; i<MAXEXP; i++)
	{
		Explosions[i].Active = GE_FALSE;

		for(int j=0; j<10; j++)
		{
			Explosions[i].Effect[j][0] = '\0';
			Explosions[i].Delay[j] = 0.0f;
			Explosions[i].Offset[j].X = 0.0f;
			Explosions[i].Offset[j].Y = 0.0f;
			Explosions[i].Offset[j].Z = 0.0f;
		}
	}

	Bottom = NULL;

	CIniFile AttrFile("explosion.ini");

	if(!AttrFile.ReadFile())
	{
		CCD->ReportError("*WARNING* Failed to open explosion.ini file", false);
		return;
	}

	string KeyName = AttrFile.FindFirstKey();
	string Type;
	int effptr = 0;
	int expptr = 0;

	while(KeyName != "")
	{
		string Ename, Vector;
		char szName[64];

		strcpy(Explosions[expptr].Name, KeyName.c_str());
		Ename = AttrFile.GetValue(KeyName, "effect0");

		if(Ename != "")
		{
			strcpy(Explosions[expptr].Effect[0], Ename.c_str());
			Explosions[expptr].Delay[0] = (float)AttrFile.GetValueF(KeyName, "delay0");

			Vector = AttrFile.GetValue(KeyName, "offset0");

			if(Vector != "")
			{
				strcpy(szName, Vector.c_str());
				Explosions[expptr].Offset[0] = Extract(szName);
			}
		}

		Ename = AttrFile.GetValue(KeyName, "effect1");

		if(Ename != "")
		{
			strcpy(Explosions[expptr].Effect[1], Ename.c_str());
			Explosions[expptr].Delay[1] = (float)AttrFile.GetValueF(KeyName, "delay1");

			Vector = AttrFile.GetValue(KeyName, "offset1");

			if(Vector != "")
			{
				strcpy(szName, Vector.c_str());
				Explosions[expptr].Offset[1] = Extract(szName);
			}
		}

		Ename = AttrFile.GetValue(KeyName, "effect2");

		if(Ename != "")
		{
			strcpy(Explosions[expptr].Effect[2], Ename.c_str());
			Explosions[expptr].Delay[2] = (float)AttrFile.GetValueF(KeyName, "delay2");

			Vector = AttrFile.GetValue(KeyName, "offset2");

			if(Vector != "")
			{
				strcpy(szName, Vector.c_str());
				Explosions[expptr].Offset[2] = Extract(szName);
			}
		}

		Ename = AttrFile.GetValue(KeyName, "effect3");

		if(Ename != "")
		{
			strcpy(Explosions[expptr].Effect[3], Ename.c_str());
			Explosions[expptr].Delay[3] = (float)AttrFile.GetValueF(KeyName, "delay3");

			Vector = AttrFile.GetValue(KeyName, "offset3");

			if(Vector != "")
			{
				strcpy(szName, Vector.c_str());
				Explosions[expptr].Offset[3] = Extract(szName);
			}
		}

		Ename = AttrFile.GetValue(KeyName, "effect4");

		if(Ename != "")
		{
			strcpy(Explosions[expptr].Effect[4], Ename.c_str());
			Explosions[expptr].Delay[4] = (float)AttrFile.GetValueF(KeyName, "delay4");

			Vector = AttrFile.GetValue(KeyName, "offset4");

			if(Vector != "")
			{
				strcpy(szName, Vector.c_str());
				Explosions[expptr].Offset[4] = Extract(szName);
			}
		}

		Ename = AttrFile.GetValue(KeyName, "effect5");

		if(Ename != "")
		{
			strcpy(Explosions[expptr].Effect[5], Ename.c_str());
			Explosions[expptr].Delay[5] = (float)AttrFile.GetValueF(KeyName, "delay5");

			Vector = AttrFile.GetValue(KeyName, "offset5");

			if(Vector != "")
			{
				strcpy(szName, Vector.c_str());
				Explosions[expptr].Offset[5] = Extract(szName);
			}
		}

		Ename = AttrFile.GetValue(KeyName, "effect6");

		if(Ename != "")
		{
			strcpy(Explosions[expptr].Effect[6], Ename.c_str());
			Explosions[expptr].Delay[6] = (float)AttrFile.GetValueF(KeyName, "delay6");

			Vector = AttrFile.GetValue(KeyName, "offset6");

			if(Vector != "")
			{
				strcpy(szName, Vector.c_str());
				Explosions[expptr].Offset[6] = Extract(szName);
			}
		}

		Ename = AttrFile.GetValue(KeyName, "effect7");

		if(Ename != "")
		{
			strcpy(Explosions[expptr].Effect[7], Ename.c_str());
			Explosions[expptr].Delay[7] = (float)AttrFile.GetValueF(KeyName, "delay7");

			Vector = AttrFile.GetValue(KeyName, "offset7");

			if(Vector != "")
			{
				strcpy(szName, Vector.c_str());
				Explosions[expptr].Offset[7] = Extract(szName);
			}
		}

		Ename = AttrFile.GetValue(KeyName, "effect8");

		if(Ename != "")
		{
			strcpy(Explosions[expptr].Effect[8], Ename.c_str());
			Explosions[expptr].Delay[8] = (float)AttrFile.GetValueF(KeyName, "delay8");

			Vector = AttrFile.GetValue(KeyName, "offset8");

			if(Vector != "")
			{
				strcpy(szName, Vector.c_str());
				Explosions[expptr].Offset[8] = Extract(szName);
			}
		}

		Ename = AttrFile.GetValue(KeyName, "effect9");

		if(Ename != "")
		{
			strcpy(Explosions[expptr].Effect[9], Ename.c_str());
			Explosions[expptr].Delay[9] = (float)AttrFile.GetValueF(KeyName, "delay9");

			Vector = AttrFile.GetValue(KeyName, "offset9");

			if(Vector != "")
			{
				strcpy(szName, Vector.c_str());
				Explosions[expptr].Offset[9] = Extract(szName);
			}
		}

		Explosions[expptr].Active = true;
		expptr+=1;

		KeyName = AttrFile.FindNextKey();
	}
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
/* ------------------------------------------------------------------------------------ */
CExplosionInit::~CExplosionInit()
{
	DelayExp *pool, *temp;
	pool = Bottom;

	while(pool!= NULL)
	{
		temp = pool->prev;
		geRam_Free(pool);
		pool = temp;
	}
}

// changed RF063
/* ------------------------------------------------------------------------------------ */
//	AddExplosion
/* ------------------------------------------------------------------------------------ */
void CExplosionInit::AddExplosion(char *Name, const geVec3d &Position,
								  geActor *theActor, char *theBone)
{
	int i;

	for(i=0; i<MAXEXP; i++)
	{
		if(Explosions[i].Active)
		{
			if(!stricmp(Explosions[i].Name, Name))
			{
				for(int j=0; j<10; j++)
				{
					if(Explosions[i].Effect[j][0] != '\0')
					{
						for(int k=0; k<MAXEXPITEM; k++)
						{
							if(CCD->Effect()->EffectActive(k))
							{
								if(!stricmp(Explosions[i].Effect[j], CCD->Effect()->EffectName(k)))
								{
									DelayExp *pool;
									geXForm3d Xf;

									pool = GE_RAM_ALLOCATE_STRUCT(DelayExp);
									memset(pool, 0, sizeof(DelayExp));
									pool->next = NULL;
									pool->prev = Bottom;

									if(Bottom != NULL)
										Bottom->next = pool;

									Bottom = pool;
									pool->Type = k;
									pool->Offset = Explosions[i].Offset[j];
									pool->Delay = Explosions[i].Delay[j];
									geVec3d_Add(&Position, &(pool->Offset), &(pool->Offset));
									pool->Attached = false;
									pool->Actor = theActor;

									if(theActor)
									{
										pool->Bone[0] = '\0';

										if(theBone != NULL)
											strcpy(pool->Bone, theBone);

										if(pool->Bone[0] != '\0')
											geActor_GetBoneTransform(pool->Actor, pool->Bone, &Xf);
										else
											geActor_GetBoneTransform(pool->Actor, RootBoneName(pool->Actor), &Xf);

										pool->Position = Xf.Translation;
										pool->Tilt = false;
									}

									break;
								}
							}
						}
					}
				}
			}
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	AddExplosion
/* ------------------------------------------------------------------------------------ */
void CExplosionInit::AddExplosion(char *Name, const geVec3d &Position,
								  geActor *theActor, char *theBone, bool Tilt)
{
	int i;

	for(i=0; i<MAXEXP; i++)
	{
		if(Explosions[i].Active)
		{
			if(!stricmp(Explosions[i].Name, Name))
			{
				for(int j=0; j<10; j++)
				{
					if(Explosions[i].Effect[j][0] != '\0')
					{
						for(int k=0; k<MAXEXPITEM; k++)
						{
							if(CCD->Effect()->EffectActive(k))
							{
								if(!stricmp(Explosions[i].Effect[j], CCD->Effect()->EffectName(k)))
								{
									DelayExp *pool;
									geXForm3d Xf;

									pool = GE_RAM_ALLOCATE_STRUCT(DelayExp);
									memset(pool, 0, sizeof(DelayExp));
									pool->next = NULL;
									pool->prev = Bottom;

									if(Bottom != NULL)
										Bottom->next = pool;

									Bottom = pool;
									pool->Type = k;
									pool->Offset = Explosions[i].Offset[j];
									pool->Delay = Explosions[i].Delay[j];
									geVec3d_Add(&Position, &pool->Offset, &pool->Offset);
									pool->Attached = false;
									pool->Actor = theActor;

									if(theActor)
									{
										pool->Bone[0] = '\0';

										if(theBone != NULL)
											strcpy(pool->Bone, theBone);

										if(pool->Bone[0] != '\0')
											geActor_GetBoneTransform(pool->Actor, pool->Bone, &Xf);
										else
											geActor_GetBoneTransform(pool->Actor, RootBoneName(pool->Actor), &Xf);

										pool->Position = Xf.Translation;
										pool->Tilt = Tilt;
									}

									break;
								}
							}
						}
					}
				}
			}
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	AddExplosion
/* ------------------------------------------------------------------------------------ */
void CExplosionInit::AddExplosion(char *Name, const geVec3d &Position)
{
	int i;

	for(i=0; i<MAXEXP; i++)
	{
		if(Explosions[i].Active)
		{
			if(!stricmp(Explosions[i].Name, Name))
			{
				for(int j=0; j<10; j++)
				{
					if(Explosions[i].Effect[j][0] != '\0')
					{
						for(int k=0; k<MAXEXPITEM; k++)
						{
							if(CCD->Effect()->EffectActive(k))
							{
								if(!stricmp(Explosions[i].Effect[j], CCD->Effect()->EffectName(k)))
								{
									DelayExp *pool;
									pool = GE_RAM_ALLOCATE_STRUCT(DelayExp);
									memset(pool, 0, sizeof(DelayExp));
									pool->next = NULL;
									pool->prev = Bottom;

									if(Bottom != NULL)
										Bottom->next = pool;

									Bottom = pool;
									pool->Type = k;
									pool->Offset = Explosions[i].Offset[j];
									pool->Delay = Explosions[i].Delay[j];
									pool->Position = Position;
									pool->Attached = false;
									pool->Actor = NULL;
									pool->Bone[0] = '\0';

									break;
								}
							}
						}
					}
				}
			}
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	UnAttach
/* ------------------------------------------------------------------------------------ */
void CExplosionInit::UnAttach(geActor *Actor)
{
	DelayExp *pool, *temp;

	pool=Bottom;

	while(pool != NULL)
	{
		temp = pool->prev;

		if(pool->Attached && pool->Actor)
		{
			if(Actor == pool->Actor)
			{
				if(Bottom == pool)
					Bottom = pool->prev;

				if(pool->prev != NULL)
					pool->prev->next = pool->next;

				if(pool->next != NULL)
					pool->next->prev = pool->prev;

				geRam_Free(pool);
				pool = NULL;
			}
		}

		pool = temp;
	}
}

/* ------------------------------------------------------------------------------------ */
//	Tick
/* ------------------------------------------------------------------------------------ */
void CExplosionInit::Tick(geFloat dwTicks)
{
	DelayExp *pool, *temp;
	geVec3d Zero = {0.0f, 0.0f, 0.0f};

	int count = 0;
	pool = Bottom;

	while(pool != NULL)
	{
		temp = pool->prev;
		count += 1;
		pool = temp;
	}

	pool = Bottom;

	while(pool != NULL)
	{
		temp = pool->prev;
		pool->Delay -= (dwTicks*0.001f);

		if(!pool->Attached)
		{
			if(pool->Delay <= 0.0f)
			{
				if(pool->Actor)
				{
					geXForm3d Xf;

					if(pool->Bone[0] != '\0')
						geActor_GetBoneTransform(pool->Actor, pool->Bone, &Xf);
					else
						geActor_GetBoneTransform(pool->Actor, RootBoneName(pool->Actor), &Xf);

					geVec3d Position = Xf.Translation;

					geActor_GetBoneTransform(pool->Actor, NULL, &Xf);

					if(pool->Tilt)
						geXForm3d_RotateX(&Xf, CCD->CameraManager()->GetTilt());

					geVec3d Direction;
					geXForm3d_GetIn(&Xf, &Direction);

					if(!pool->Tilt)
						geVec3d_Inverse(&Direction);

					geVec3d_AddScaled(&Position, &Direction, pool->Offset.Z, &Position);

					geXForm3d_GetUp(&Xf, &Direction);
					geVec3d_AddScaled(&Position, &Direction, pool->Offset.Y, &Position);

					geXForm3d_GetLeft(&Xf, &Direction);
					geVec3d_AddScaled(&Position, &Direction, pool->Offset.X, &Position);

					pool->index = CCD->Effect()->AddEffect(pool->Type, Position, Zero);

					pool->Attached = true;

					int type = CCD->Effect()->EffectType(pool->Type);

					switch(type)
					{
					case EFF_SPRAY:
						Spray Sp;
						geVec3d_Copy(&Position, &(Sp.Source));
						geXForm3d_Copy(&Xf, &Sp.Xform);
						CCD->EffectManager()->Item_Modify(EFF_SPRAY, pool->index, &Sp, SPRAY_SOURCE | SPRAY_DEST);
						break;
					case EFF_ACTORSPRAY:
						ActorSpray aSp;
						geVec3d_Copy(&Position, &(aSp.Source));
						geXForm3d_Copy(&Xf, &aSp.Xform);
						CCD->EffectManager()->Item_Modify(EFF_ACTORSPRAY, pool->index, &aSp, SPRAY_SOURCE | SPRAY_DEST);
						break;
					case EFF_BOLT:
						EBolt Bl;
						geVec3d_Copy(&Position, &(Bl.Start));
						geXForm3d_Copy(&Xf, &Bl.Xform);
						CCD->EffectManager()->Item_Modify(EFF_BOLT, pool->index, &Bl, BOLT_START | BOLT_ENDOFFSET);
						break;
					}
				}
				else
				{
					pool->index = CCD->Effect()->AddEffect(pool->Type, pool->Position, pool->Offset);

					if(Bottom == pool)
						Bottom = pool->prev;

					if(pool->prev != NULL)
						pool->prev->next = pool->next;

					if(pool->next != NULL)
						pool->next->prev = pool->prev;

					geRam_Free(pool);
					pool = NULL;
				}
			}
		}
		else
		{
			if(CCD->EffectManager()->Item_Alive(pool->index) && pool->Actor)
			{
				geXForm3d Xf;

				if(pool->Bone[0] != '\0')
					geActor_GetBoneTransform(pool->Actor, pool->Bone, &Xf);
				else
					geActor_GetBoneTransform(pool->Actor, RootBoneName(pool->Actor), &Xf);

				geVec3d Position = Xf.Translation;

				geVec3d Direction;

				geActor_GetBoneTransform(pool->Actor, NULL, &Xf);

				if(pool->Tilt)
					geXForm3d_RotateX(&Xf, CCD->CameraManager()->GetTilt());

				geXForm3d_GetIn(&Xf, &Direction);

				if(!pool->Tilt)
					geVec3d_Inverse(&Direction);

				geVec3d_AddScaled(&Position, &Direction, pool->Offset.Z, &Position);

				geXForm3d_GetUp(&Xf, &Direction);
				geVec3d_AddScaled(&Position, &Direction, pool->Offset.Y, &Position);

				geXForm3d_GetLeft(&Xf, &Direction);
				geVec3d_AddScaled(&Position, &Direction, pool->Offset.X, &Position);

// changed RF064
				int type = CCD->Effect()->EffectType(pool->Type);
// end change RF064

				switch(type)
				{
				case EFF_SPRAY:
					Spray Sp;
					geVec3d_Copy(&Position, &(Sp.Source));
					geXForm3d_Copy(&Xf, &Sp.Xform);
					CCD->EffectManager()->Item_Modify(EFF_SPRAY, pool->index, &Sp, SPRAY_SOURCE | SPRAY_DEST);
					break;
				case EFF_LIGHT:
					Glow Lite;
					geVec3d_Copy(&Position, &(Lite.Pos));
					CCD->EffectManager()->Item_Modify(EFF_LIGHT, pool->index, &Lite, GLOW_POS);
					break;
				case EFF_SND:
					Snd Sound;
					geVec3d_Copy(&Position, &(Sound.Pos));
					CCD->EffectManager()->Item_Modify(EFF_SND, pool->index, &Sound, SND_POS);
					break;
				case EFF_SPRITE:
					Sprite S;
					geVec3d_Copy(&Position, &(S.Pos));
					CCD->EffectManager()->Item_Modify(EFF_SPRITE, pool->index, &S, SPRITE_POS);
					break;
				case EFF_CORONA:
					EffCorona C;
					C.Vertex.X = Position.X;
					C.Vertex.Y = Position.Y;
					C.Vertex.Z = Position.Z;
					CCD->EffectManager()->Item_Modify(EFF_CORONA, pool->index, &C, CORONA_POS);
					break;
				case EFF_BOLT:
					EBolt Bl;
					geVec3d_Copy(&Position, &(Bl.Start));
					geXForm3d_Copy(&Xf, &Bl.Xform);
					CCD->EffectManager()->Item_Modify(EFF_BOLT, pool->index, &Bl, BOLT_START | BOLT_ENDOFFSET);
					break;
// changed RF064
				case EFF_ACTORSPRAY:
					ActorSpray aSp;
					geVec3d_Copy(&Position, &(aSp.Source));
					geXForm3d_Copy(&Xf, &aSp.Xform);
					CCD->EffectManager()->Item_Modify(EFF_ACTORSPRAY, pool->index, &aSp, SPRAY_SOURCE | SPRAY_DEST);
					break;
// end change RF064
				}
			}
			else
			{
				if(Bottom == pool)
					Bottom = pool->prev;

				if(pool->prev != NULL)
					pool->prev->next = pool->next;

				if(pool->next != NULL)
					pool->next->prev = pool->prev;

				geRam_Free(pool);
				pool = NULL;
			}
		}

		pool = temp;
	}
}
// end change RF063

/* ------------------------------------------------------------------------------------ */
//	Constructor
/* ------------------------------------------------------------------------------------ */
CExplosion::CExplosion()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	// Ok, see if we have any Explosion entities at all
	pSet = geWorld_GetEntitySet(CCD->World(), "Explosion");

	if(!pSet)
		return;

	// Look through all of our Explosions
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Explosion *pSource = (Explosion*)geEntity_GetUserData(pEntity);

		pSource->OriginOffset = pSource->origin;

		if(pSource->Model)
		{
			geVec3d ModelOrigin;
	    	geWorld_GetModelRotationalCenter(CCD->World(), pSource->Model, &ModelOrigin);
			geVec3d_Subtract(&pSource->origin, &ModelOrigin, &pSource->OriginOffset);
  		}

		pSource->active = GE_FALSE;
	}
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
/* ------------------------------------------------------------------------------------ */
CExplosion::~CExplosion()
{
}

/* ------------------------------------------------------------------------------------ */
// Tick
/* ------------------------------------------------------------------------------------ */
void CExplosion::Tick(float dwTicks)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	//	Ok, check to see if there are Explosions in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "Explosion");

	if(pSet)
	{
		//	Ok, we have Explosions somewhere.  Dig through 'em all.
		for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
			pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
		{
			Explosion *pSource = (Explosion*)geEntity_GetUserData(pEntity);

			pSource->origin = pSource->OriginOffset;
			SetOriginOffset(pSource->EntityName, pSource->BoneName, pSource->Model, &(pSource->origin));

			if(!EffectC_IsStringNull(pSource->TriggerName))
			{
				if(GetTriggerState(pSource->TriggerName))
				{
					if(pSource->active == GE_FALSE)
					{
// changed RF063
						CCD->Explosions()->AddExplosion(pSource->ExplosionName, pSource->origin);

						if(pSource->ShakeAmt > 0.0f)
							CCD->CameraManager()->SetShake(pSource->ShakeAmt, pSource->ShakeDecay, pSource->origin);

						pSource->active = GE_TRUE;

						if(!EffectC_IsStringNull(pSource->DamageAttribute))
						{
							if(pSource->Radius > 0.0f)
							{
// changed RF063
								CCD->Damage()->DamageActorInRange(pSource->origin, pSource->Radius, pSource->DamageAmt, pSource->DamageAttribute, pSource->DamageAltAmt, pSource->DamageAltAttribute, "Explosion");
								CCD->Damage()->DamageModelInRange(pSource->origin, pSource->Radius, pSource->DamageAmt, pSource->DamageAttribute, pSource->DamageAltAmt, pSource->DamageAltAttribute);
// end change RF063
							}
						}
					}
				}
				else
				{
					if(pSource->active == GE_TRUE)
					{
						pSource->active = GE_FALSE;
					}
				}
			}
			else
			{
				if(pSource->active == GE_FALSE)
				{
// changed RF063
					CCD->Explosions()->AddExplosion(pSource->ExplosionName, pSource->origin);

					if(pSource->ShakeAmt > 0.0f)
						CCD->CameraManager()->SetShake(pSource->ShakeAmt, pSource->ShakeDecay, pSource->origin);

					pSource->active = GE_TRUE;

					if(!EffectC_IsStringNull(pSource->DamageAttribute))
					{
						if(pSource->Radius > 0.0f)
						{
// changed RF063
							CCD->Damage()->DamageActorInRange(pSource->origin, pSource->Radius, pSource->DamageAmt, pSource->DamageAttribute, pSource->DamageAltAmt, pSource->DamageAltAttribute, "Explosion");
							CCD->Damage()->DamageModelInRange(pSource->origin, pSource->Radius, pSource->DamageAmt, pSource->DamageAttribute, pSource->DamageAltAmt, pSource->DamageAltAttribute);
// end change RF063
						}
					}
				}
			}
		}
	}
}

/* ----------------------------------- END OF FILE ------------------------------------ */
