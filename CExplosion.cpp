/*

    Explosion Manager.cpp               Explosion  Manager handler

	(c) 1999 Ralph Deane

	This file contains the class implementation for the Explosion 
Manager that handles Explosions for RGF-based games.
*/

//	Include the One True Header

#include "RabidFramework.h"
#include <Ram.h>

CExplosionInit::CExplosionInit()
{
	int i;
	
	for(i=0;i<MAXEXP;i++)
	{
		Explosions[i].Active = false;
		for(int j=0;j<10;j++)
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
		CCD->ReportError("Can't open explosion initialization file", false);
		return;
	}
	CString KeyName = AttrFile.FindFirstKey();
	CString Type;
	int effptr = 0;
	int expptr = 0;
	while(KeyName != "")
	{
		CString Ename, Vector;
		char szName[64];
		
		strcpy(Explosions[expptr].Name,KeyName);
		Ename = AttrFile.GetValue(KeyName, "effect0");
		if(Ename!="")
		{
			strcpy(Explosions[expptr].Effect[0],Ename);
			Explosions[expptr].Delay[0] = (float)AttrFile.GetValueF(KeyName, "delay0");
			Vector = AttrFile.GetValue(KeyName, "offset0");
			if(Vector!="")
			{
				strcpy(szName,Vector);
				Explosions[expptr].Offset[0] = Extract(szName);
			}
		}
		Ename = AttrFile.GetValue(KeyName, "effect1");
		if(Ename!="")
		{
			strcpy(Explosions[expptr].Effect[1],Ename);
			Explosions[expptr].Delay[1] = (float)AttrFile.GetValueF(KeyName, "delay1");
			Vector = AttrFile.GetValue(KeyName, "offset1");
			if(Vector!="")
			{
				strcpy(szName,Vector);
				Explosions[expptr].Offset[1] = Extract(szName);
			}
		}
		Ename = AttrFile.GetValue(KeyName, "effect2");
		if(Ename!="")
		{
			strcpy(Explosions[expptr].Effect[2],Ename);
			Explosions[expptr].Delay[2] = (float)AttrFile.GetValueF(KeyName, "delay2");
			Vector = AttrFile.GetValue(KeyName, "offset2");
			if(Vector!="")
			{
				strcpy(szName,Vector);
				Explosions[expptr].Offset[2] = Extract(szName);
			}
		}
		Ename = AttrFile.GetValue(KeyName, "effect3");
		if(Ename!="")
		{
			strcpy(Explosions[expptr].Effect[3],Ename);
			Explosions[expptr].Delay[3] = (float)AttrFile.GetValueF(KeyName, "delay3");
			Vector = AttrFile.GetValue(KeyName, "offset3");
			if(Vector!="")
			{
				strcpy(szName,Vector);
				Explosions[expptr].Offset[3] = Extract(szName);
			}
		}
		Ename = AttrFile.GetValue(KeyName, "effect4");
		if(Ename!="")
		{
			strcpy(Explosions[expptr].Effect[4],Ename);
			Explosions[expptr].Delay[4] = (float)AttrFile.GetValueF(KeyName, "delay4");
			Vector = AttrFile.GetValue(KeyName, "offset4");
			if(Vector!="")
			{
				strcpy(szName,Vector);
				Explosions[expptr].Offset[4] = Extract(szName);
			}
		}
		Ename = AttrFile.GetValue(KeyName, "effect5");
		if(Ename!="")
		{
			strcpy(Explosions[expptr].Effect[5],Ename);
			Explosions[expptr].Delay[5] = (float)AttrFile.GetValueF(KeyName, "delay5");
			Vector = AttrFile.GetValue(KeyName, "offset5");
			if(Vector!="")
			{
				strcpy(szName,Vector);
				Explosions[expptr].Offset[5] = Extract(szName);
			}
		}
		Ename = AttrFile.GetValue(KeyName, "effect6");
		if(Ename!="")
		{
			strcpy(Explosions[expptr].Effect[6],Ename);
			Explosions[expptr].Delay[6] = (float)AttrFile.GetValueF(KeyName, "delay6");
			Vector = AttrFile.GetValue(KeyName, "offset6");
			if(Vector!="")
			{
				strcpy(szName,Vector);
				Explosions[expptr].Offset[6] = Extract(szName);
			}
		}
		Ename = AttrFile.GetValue(KeyName, "effect7");
		if(Ename!="")
		{
			strcpy(Explosions[expptr].Effect[7],Ename);
			Explosions[expptr].Delay[7] = (float)AttrFile.GetValueF(KeyName, "delay7");
			Vector = AttrFile.GetValue(KeyName, "offset7");
			if(Vector!="")
			{
				strcpy(szName,Vector);
				Explosions[expptr].Offset[7] = Extract(szName);
			}
		}
		Ename = AttrFile.GetValue(KeyName, "effect8");
		if(Ename!="")
		{
			strcpy(Explosions[expptr].Effect[8],Ename);
			Explosions[expptr].Delay[8] = (float)AttrFile.GetValueF(KeyName, "delay8");
			Vector = AttrFile.GetValue(KeyName, "offset8");
			if(Vector!="")
			{
				strcpy(szName,Vector);
				Explosions[expptr].Offset[8] = Extract(szName);
			}
		}
		Ename = AttrFile.GetValue(KeyName, "effect9");
		if(Ename!="")
		{
			strcpy(Explosions[expptr].Effect[9],Ename);
			Explosions[expptr].Delay[9] = (float)AttrFile.GetValueF(KeyName, "delay9");
			Vector = AttrFile.GetValue(KeyName, "offset9");
			if(Vector!="")
			{
				strcpy(szName,Vector);
				Explosions[expptr].Offset[9] = Extract(szName);
			}
		}
		Explosions[expptr].Active = true;
		expptr+=1;
		
		KeyName = AttrFile.FindNextKey();
	}
}

CExplosionInit::~CExplosionInit()
{
	DelayExp *pool, *temp;
	pool = Bottom;
	while	(pool!= NULL)
	{
		temp = pool->next;
		geRam_Free(pool);
		pool = temp;
	}
}

void CExplosionInit::AddExplosion(char *Name, geVec3d Position)
{
	int i;

	for(i=0;i<MAXEXP;i++)
	{
		if(Explosions[i].Active)
		{
			if(!stricmp(Explosions[i].Name, Name))
			{
				for(int j=0;j<10;j++)
				{
					if(Explosions[i].Effect[j][0] != '\0')
					{
						for(int k=0;k<MAXEXPITEM;k++)
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
									if(Bottom != NULL) Bottom->next = pool;
										Bottom = pool;
									pool->Type = k;
									pool->Offset = Explosions[i].Offset[j];
									pool->Delay = Explosions[i].Delay[j];
									pool->Position = Position;
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


void CExplosionInit::Tick(geFloat dwTicks)
{
	DelayExp *pool, *temp;

	pool=Bottom;
	while ( pool != NULL )
	{
		temp = pool->prev;
		pool->Delay-=(dwTicks*0.001f);
		if(pool->Delay<=0.0f)
		{
			CCD->Effect()->AddEffect(pool->Type, pool->Position, pool->Offset);
			if(Bottom == pool)
				Bottom = pool->prev;
			if(pool->prev != NULL)
				pool->prev->next = pool->next;
			if(pool->next != NULL)
				pool->next->prev = pool->prev;
			geRam_Free(pool);
			pool = NULL;
		}
		pool = temp;
	} 
}


CExplosion::CExplosion()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, see if we have any Explosion entities at all
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Explosion");
	
	if(!pSet) 
		return;	
	
	//	Look through all of our Explosions
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
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

CExplosion::~CExplosion()
{
}

//
// Tick
//

void CExplosion::Tick(float dwTicks)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, check to see if there are Explosions in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "Explosion");
	
	if(pSet) 
	{
		
		//	Ok, we have Explosions somewhere.  Dig through 'em all.
		
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
			Explosion *pSource = (Explosion*)geEntity_GetUserData(pEntity);

			pSource->origin = pSource->OriginOffset;
			SetOriginOffset(pSource->EntityName, pSource->BoneName, pSource->Model, &(pSource->origin));
			if(!EffectC_IsStringNull(pSource->TriggerName))
			{
				if(GetTriggerState(pSource->TriggerName))
				{
					if(pSource->active==GE_FALSE)
					{
						CCD->Explosions()->AddExplosion(pSource->ExplosionName, pSource->origin);
						pSource->active=GE_TRUE;
						if(!EffectC_IsStringNull(pSource->DamageAttribute))
						{
							if(pSource->Radius>0.0f)
							{
								CCD->Damage()->DamageActorInRange(pSource->origin, pSource->Radius, pSource->DamageAmt, pSource->DamageAttribute);
								CCD->Damage()->DamageModelInRange(pSource->origin, pSource->Radius, pSource->DamageAmt, pSource->DamageAttribute);
							}
						}
					}
				}
				else
				{
					if(pSource->active==GE_TRUE)
					{
						pSource->active=GE_FALSE;
					}
				}
			}
			else
			{
				if(pSource->active==GE_FALSE)
				{
					CCD->Explosions()->AddExplosion(pSource->ExplosionName, pSource->origin);
					pSource->active=GE_TRUE;
					if(!EffectC_IsStringNull(pSource->DamageAttribute))
					{
						if(pSource->Radius>0.0f)
						{
							CCD->Damage()->DamageActorInRange(pSource->origin, pSource->Radius, pSource->DamageAmt, pSource->DamageAttribute);
							CCD->Damage()->DamageModelInRange(pSource->origin, pSource->Radius, pSource->DamageAmt, pSource->DamageAttribute);
						}
					}
				}
			}
		}
	}
}
