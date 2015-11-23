/*
CWeapon.cpp:		Weapon class implementation

  (c) 2001 Ralph Deane
  All Rights Reserved
  
	This file contains the class declaration for Weapon
	implementation.
*/

//	Include the One True Header

#include "RabidFramework.h"

extern geSound_Def *SPool_Sound(char *SName);
extern geBitmap *TPool_Bitmap(char *DefaultBmp, char *DefaultAlpha, char *BName, char *AName);
extern "C" void	DrawBoundBox(geWorld *World, const geVec3d *Pos, const geVec3d *Min, const geVec3d *Max);

//
//	Constructor
//

CWeapon::CWeapon()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	int i;

	for(i=0;i<10;i++)
		Slot[i]=-1;

	CurrentWeapon = -1;
	Bottom =(Proj *)NULL;
	AttackFlag = false;
	MFlash = false;
// changed RF064
	dropflag = false;
// end change RF064
	
	//	Ok, check to see if there's a PlayerSetup around...
	pSet = geWorld_GetEntitySet(CCD->World(), "PlayerSetup");
	
	if(pSet == NULL)
	{
		CCD->ReportError("No player setup? Bad level", false);
		return;
	}
	
	pEntity = geEntity_EntitySetGetNextEntity(pSet, NULL);
	
	if(pEntity)
	{	
		PlayerSetup *theStart = (PlayerSetup *)geEntity_GetUserData(pEntity);
		ViewPoint = theStart->LevelViewPoint;
// changed RF064
		OldViewPoint = ViewPoint;
// end change RF064
		
	}
	else
	{
		CCD->ReportError("Couldn't locate PlayerSetup in world", false);
		return;
	}
	
	LoadDefaults();

}

//
//	Destructor
//

CWeapon::~CWeapon()
{

	Proj *d, *next;
	
	d = Bottom;
	
	while(d != NULL)
	{
		next = d->prev;
		CCD->ActorManager()->RemoveActor(d->Actor);
		geActor_Destroy(&d->Actor);
		delete d;
		d = next;
	}
	Bottom = (Proj *)NULL;
	
	for(int i=0;i<MAX_WEAPONS;i++)
	{
		if(WeaponD[i].active)
		{
			if(WeaponD[i].VActor)
			{
				geWorld_RemoveActor(CCD->World(), WeaponD[i].VActor);
				geActor_Destroy(&WeaponD[i].VActor);
				geActor_DefDestroy(&WeaponD[i].VActorDef);
			}
			if(WeaponD[i].CrossHair)
			{
				if(!WeaponD[i].CrossHairFixed)
					geWorld_RemoveBitmap(CCD->World(), WeaponD[i].CrossHair);
				geBitmap_Destroy(&WeaponD[i].CrossHair);
			}
			if(WeaponD[i].PActor)
			{
				geWorld_RemoveActor(CCD->World(), WeaponD[i].PActor);
				geActor_Destroy(&WeaponD[i].PActor);
				geActor_DefDestroy(&WeaponD[i].PActorDef);
			}
		}
	}
}

// changed Rf063
void CWeapon::ChangeWeapon(char *name)
{
	int i;
	for(i=0;i<MAX_WEAPONS;i++)
	{
		if(WeaponD[i].active)
		{
			if(!strcmp(WeaponD[i].Name, name))
			{
				SetWeapon(WeaponD[i].Slot);
				return;
			}
		}
	}
}
// end change RF063

//
// Tick
//

void CWeapon::Tick(float dwTicks)
{
		int i;
		for(i=0;i<MAX_WEAPONS;i++)
		{
			if(WeaponD[i].active)
			{
				CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
				if(theInv->Has(WeaponD[i].Name))
				{
					if(theInv->Value(WeaponD[i].Name)>0)
					{
						Slot[WeaponD[i].Slot] = i;
						if(CurrentWeapon == -1)
						{
							SetWeapon(WeaponD[i].Slot);
						}
					}
				}
			}
		}

	// take care of any projectiles that are active
	
	Proj *d, *next;
	
	d = Bottom;
	while(d != NULL)
	{
		geVec3d	tempPos;
		geVec3d	tempPos1;
		geVec3d	Distance;
		GE_Collision	Collision;
		geActor *Actor = NULL;
		geWorld_Model *Model = NULL;
// changed RF064
		char BoneHit[64];

		BoneHit[0] = '\0';
// end change RF064
		
		next = d->prev;
		float dwTick = (float)(dwTicks) * 0.001f;
		CCD->ActorManager()->GetPosition(d->Actor, &tempPos);
		
		// Movement
		geVec3d_Scale( &(d->Direction), dwTick, &Distance ) ;
		geVec3d_Add(&tempPos, &Distance, &tempPos1) ; 
		// LifeTime Checking
		bool Alive = true;
		d->LifeTime -= dwTicks *0.001f;
		if(d->LifeTime <= 0.0f)
			Alive = false;
		
		// Bouncing
		if(d->Bounce && Alive)
		{
			float totalTravelled = 1.0f ; // keeps track of fraction of path travelled (1.0=complete)
			float margin = 0.001f ; // margin to be satisfied with (1.0 - margin == 1.0)
			int loopCounter = 0 ; // safety valve for endless collisions in tight corners
			const int maxLoops = 10 ; // max. nr of collisions this frame
			bool PSound = true;
			
			CCD->Collision()->IgnoreContents(false);
			CCD->Collision()->CheckLevel(RGF_COLLISIONLEVEL_1);
			while(CCD->Collision()->CheckForBoneCollision(&d->ExtBox.Min, &d->ExtBox.Max,
				tempPos, tempPos1, &Collision, d->Actor, BoneHit, d->BoneLevel))
			{
				//
				// Process hit here
				//
				int nHitType = CCD->Collision()->ProcessCollision(Collision, d->Actor, false);
				if(nHitType == kNoCollision)
					break;
				//
				// impact damage
				//
// changed RF063
				if(Collision.Actor)
				{
					CCD->Damage()->DamageActor(Collision.Actor, d->Damage, d->Attribute, d->AltDamage, d->AltAttribute, d->Name);
					if(d->AttachActor && !EffectC_IsStringNull(BoneHit) && d->BoneLevel)
						CCD->Explosions()->AddExplosion(d->ActorExplosion, Collision.Impact, Collision.Actor, BoneHit);
					else
						CCD->Explosions()->AddExplosion(d->ActorExplosion, Collision.Impact, NULL, NULL);
				}

				if(Collision.Model)
					CCD->Damage()->DamageModel(Collision.Model, d->Damage, d->Attribute, d->AltDamage, d->AltAttribute);
// end change RF063
				if(PSound)
				{
					PlaySound(d->BounceSoundDef, Collision.Impact, false);
					PSound=false;
				}
				
				float ratio ;
				float elasticity = 1.3f ;
				float friction = 0.9f ; // loses (1 minus friction) of speed
				CollisionCalcRatio( Collision, tempPos, tempPos1, ratio ) ;
				CollisionCalcImpactPoint( Collision, tempPos, tempPos1, 1.0f, ratio, tempPos1 ) ;
				CollisionCalcVelocityImpact( Collision, d->Direction, elasticity, friction, d->Direction) ; 
				if( ratio >= 0 )
					totalTravelled += (1.0f - totalTravelled) * ratio ;
				if( totalTravelled >= 1.0f - margin )
					break ;
				if( ++ loopCounter >= maxLoops ) // safety check
					break ;
			}
		}
		// No Bouncing
		if(!d->Bounce && Alive)
		{
			CCD->Collision()->IgnoreContents(false);
			CCD->Collision()->CheckLevel(RGF_COLLISIONLEVEL_1);
			//if(CCD->Collision()->CheckForCollisionD(&d->ExtBox.Min, &d->ExtBox.Max,
			if(CCD->Collision()->CheckForBoneCollision(&d->ExtBox.Min, &d->ExtBox.Max,
				tempPos, tempPos1, &Collision, d->Actor, BoneHit, d->BoneLevel))
			{
				//
				// Handle collision here
				//
				int nHitType = CCD->Collision()->ProcessCollision(Collision, d->Actor, false);
				if(nHitType != kNoCollision)
				{
					Alive = false;
					d->Pos = Collision.Impact;
					Actor = Collision.Actor;
					Model = Collision.Model;
					if(nHitType == kCollideNoMove)
					{
						geVec3d_AddScaled (&tempPos, &(d->Direction), 1000.0f, &tempPos1);
						CCD->Collision()->CheckForWCollision(NULL, NULL,
							tempPos, tempPos1, &Collision, d->Actor);
						CCD->Decals()->AddDecal(d->Decal, &Collision.Impact, &Collision.Plane.Normal);
					}
				}
			} 
		}
		// Move and Pose
		if(Alive)
		{
			d->Pos = tempPos1;
			CCD->ActorManager()->Position(d->Actor, d->Pos);
			CCD->ActorManager()->GetBoundingBox(d->Actor, &d->ExtBox);
			
			if(d->MoveSoundEffect!=-1)
			{
				Snd Sound;
				geVec3d_Copy( &(d->Pos), &( Sound.Pos ) );
				CCD->EffectManager()->Item_Modify(EFF_SND, d->MoveSoundEffect, (void *)&Sound, SND_POS);
			}
			for(int j=0;j<5;j++)
			{
				if(d->Effect[j]!=-1)
				{
					geXForm3d thePosition;
					if(EffectC_IsStringNull(d->EffectBone[j]))
						geActor_GetBoneTransform(d->Actor, NULL, &(thePosition));
					else
					{
						if(!geActor_GetBoneTransform(d->Actor, d->EffectBone[j], &(thePosition)))
							geActor_GetBoneTransform(d->Actor, NULL, &(thePosition));
					}
					switch(d->EffectType[j])
					{
						case EFF_SPRAY:
							Spray	Sp;
							geVec3d_Copy(&(thePosition.Translation), &(Sp.Source));
							geVec3d_AddScaled(&(Sp.Source), &(d->In), 50.0f, &(Sp.Dest));
							CCD->EffectManager()->Item_Modify(EFF_SPRAY, d->Effect[j], (void *)&Sp, SPRAY_SOURCE | SPRAY_ACTUALDEST);
							break;
						case EFF_LIGHT:
							Glow  Gl;
							geVec3d_Copy(&(thePosition.Translation), &(Gl.Pos));
							CCD->EffectManager()->Item_Modify(EFF_LIGHT, d->Effect[j], (void *)&Gl, GLOW_POS);
							break;
						case EFF_SND:
							Snd Sound;
							geVec3d_Copy( &(thePosition.Translation), &( Sound.Pos) );
							CCD->EffectManager()->Item_Modify(EFF_SND, d->Effect[j], (void *)&Sound, SND_POS);
							break;
						case EFF_SPRITE:
							Sprite S;
							geVec3d_Copy( &(thePosition.Translation), &( S.Pos) );
							CCD->EffectManager()->Item_Modify(EFF_SPRITE, d->Effect[j], (void *)&S, SPRITE_POS);
							break;
						case EFF_CORONA:
							EffCorona C;
							C.Vertex.X = thePosition.Translation.X;
							C.Vertex.Y = thePosition.Translation.Y;
							C.Vertex.Z = thePosition.Translation.Z;
							CCD->EffectManager()->Item_Modify(EFF_CORONA, d->Effect[j], (void *)&C, CORONA_POS);
							break;
						case EFF_BOLT:
							EBolt Bl;
							geVec3d	Temp;
							geVec3d_Subtract( &(Bl.Start), &(Bl.End),&(Temp) );
							geVec3d_Copy( &(thePosition.Translation), &(Bl.Start) );
							geVec3d_Add( &(Bl.Start), &(Temp),&(Bl.End) );
							CCD->EffectManager()->Item_Modify(EFF_BOLT, d->Effect[j], (void *)&Bl, BOLT_START | BOLT_END);
							break;
					}
				}
			}
		}
		// Delete when done
		else
		{
			if(d->MoveSoundEffect!=-1)
				CCD->EffectManager()->Item_Delete(EFF_SND, d->MoveSoundEffect);
			for(int j=0;j<5;j++)
			{
				if(d->Effect[j]!=-1)
				{
					CCD->EffectManager()->Item_Delete(d->EffectType[j], d->Effect[j]);
				}
			}

			PlaySound(d->ImpactSoundDef, d->Pos, false);
			
			//
			// inflict damage
			//
// changed RF064
// changed RF063			
			if(Actor)
			{
				CCD->Damage()->DamageActor(Actor, d->Damage, d->Attribute, d->AltDamage, d->AltAttribute, d->Name);
				if(d->AttachActor && !EffectC_IsStringNull(BoneHit) && d->BoneLevel)
					CCD->Explosions()->AddExplosion(d->ActorExplosion, d->Pos, Actor, BoneHit);
				else
					CCD->Explosions()->AddExplosion(d->ActorExplosion, d->Pos, NULL, NULL);
				if(d->ShowBoth)
					CCD->Explosions()->AddExplosion(d->Explosion, d->Pos, NULL, NULL);
			}

			if(Model)
			{
				CCD->Damage()->DamageModel(Model, d->Damage, d->Attribute, d->AltDamage, d->AltAttribute);
				CCD->Explosions()->AddExplosion(d->Explosion, d->Pos, NULL, NULL);
			}
// end change RF063	
// end change RF064
			//
			// create explosion here
			//
// changed RF063			
			if(d->ShakeAmt>0.0f)
				CCD->CameraManager()->SetShake(d->ShakeAmt, d->ShakeDecay, d->Pos);
// changed RF063

			//
			// Handle explosion damage here
			//
			if(d->RadiusDamage>0.0f)
			{
				CCD->Damage()->DamageActorInRange(d->Pos, d->Radius, d->RadiusDamage, d->Attribute, d->RadiusDamage, d->Attribute, "Explosion");
				CCD->Damage()->DamageModelInRange(d->Pos, d->Radius, d->RadiusDamage, d->Attribute, d->RadiusDamage, d->Attribute);
			}
			
			if(Bottom == d)
				Bottom = d->prev;
			if(d->prev != NULL)
				d->prev->next = d->next;
			if(d->next != NULL)
				d->next->prev = d->prev;
			CCD->ActorManager()->RemoveActor(d->Actor);
			geActor_Destroy(&d->Actor);
			delete d;
			d = NULL;
		}
		
		d = next;
	}
	
}

void CWeapon::Display()
{
// changed RF064
	if(CCD->Player()->GetMonitorMode())
		return;
	if(CurrentWeapon==-1 || CurrentWeapon==11)
		return;
	
	if(ViewPoint==FIRSTPERSON)
	{
		if(WeaponD[CurrentWeapon].PActor)
			geWorld_SetActorFlags(CCD->World(), WeaponD[CurrentWeapon].PActor, CCD->Player()->GetMirror());
		geWorld_SetActorFlags(CCD->World(), WeaponD[CurrentWeapon].VActor, GE_ACTOR_RENDER_NORMAL);
		DisplayFirstPerson(CurrentWeapon);
		if(CCD->Player()->GetMirror()!=0 && WeaponD[CurrentWeapon].PActor)
			DisplayThirdPerson(CurrentWeapon);
// end change RF064
	}
	else
	{
		if(WeaponD[CurrentWeapon].PActor)
		{
			geWorld_SetActorFlags(CCD->World(), WeaponD[CurrentWeapon].VActor, 0);
			geWorld_SetActorFlags(CCD->World(), WeaponD[CurrentWeapon].PActor, GE_ACTOR_RENDER_NORMAL | GE_ACTOR_RENDER_MIRRORS);
			DisplayThirdPerson(CurrentWeapon);
		}
	}
}

void CWeapon::DisplayThirdPerson(int index)
{
	geXForm3d XForm;
	geVec3d theRotation;
	geVec3d thePosition;
	geMotion *ActorMotion;
	geMotion *ActorBMotion;

	geXForm3d_SetIdentity (&XForm);
	geActor *Actor = CCD->Player()->GetActor();
	CCD->ActorManager()->GetRotate(Actor, &theRotation);
	CCD->ActorManager()->GetPosition(Actor, &thePosition);
	geActor_SetScale(WeaponD[index].PActor,WeaponD[index].PScale,WeaponD[index].PScale,WeaponD[index].PScale);
	geXForm3d_RotateZ(&XForm, (0.0174532925199433f*(WeaponD[index].PActorRotation.Z)) + theRotation.Z);
	geXForm3d_RotateX(&XForm, (0.0174532925199433f*(WeaponD[index].PActorRotation.X)) + theRotation.X);
	geXForm3d_RotateY(&XForm, (0.0174532925199433f*(WeaponD[index].PActorRotation.Y)) + theRotation.Y);
	geXForm3d_Translate (&XForm, thePosition.X, thePosition.Y, thePosition.Z); 

	ActorMotion = CCD->ActorManager()->GetpMotion(Actor);
	ActorBMotion = CCD->ActorManager()->GetpBMotion(Actor);
// changed RF064
	if(ActorMotion)
	{
		if(CCD->ActorManager()->GetTransitionFlag(Actor))
		{
			geActor_SetPose(WeaponD[index].PActor, ActorMotion, 0.0f, &XForm);
			if(ActorBMotion)
			{
				float BM = (CCD->ActorManager()->GetBlendAmount(Actor)
					- CCD->ActorManager()->GetAnimationTime(Actor))
					/CCD->ActorManager()->GetBlendAmount(Actor);
				if(BM<0.0f)
					BM = 0.0f;
				geActor_BlendPose(WeaponD[index].PActor, ActorBMotion, 
					CCD->ActorManager()->GetStartTime(Actor), &XForm, 
					BM);
			}
		}
		else
		{
			geActor_SetPose(WeaponD[index].PActor, ActorMotion, CCD->ActorManager()->GetAnimationTime(Actor), &XForm);
			if(ActorBMotion)
			{
				geActor_BlendPose(WeaponD[index].PActor, ActorBMotion, 
					CCD->ActorManager()->GetAnimationTime(Actor), &XForm, 
					CCD->ActorManager()->GetBlendAmount(Actor));
			}
		}
	}
// end change RF064
	else
		geActor_ClearPose(WeaponD[index].PActor, &XForm);
// changed RF063
	geFloat fAlpha;
	CCD->ActorManager()->GetAlpha(Actor, &fAlpha);
	geActor_SetAlpha(WeaponD[index].PActor, fAlpha);
// changed RF064
	if(ViewPoint==FIRSTPERSON)
		return;
// end change RF064
	if(MFlash)
	{
		geVec3d Muzzle;
		geXForm3d Xf;
		if(geActor_GetBoneTransform(WeaponD[CurrentWeapon].PActor, WeaponD[CurrentWeapon].PBone, &Xf )==GE_TRUE)
		{
			geVec3d_Copy( &( Xf.Translation ), &Muzzle);
			CCD->Explosions()->AddExplosion(WeaponD[CurrentWeapon].MuzzleFlash3rd, Muzzle, WeaponD[CurrentWeapon].PActor, WeaponD[CurrentWeapon].PBone);
		}
		MFlash = false;
	}

	geXForm3d Xf;
	geVec3d Front, Back;
	geVec3d Direction, Pos;
	geExtBox theBox;
	GE_Collision Collision;
	geFloat CurrentDistance;
	
	theBox.Min.X = theBox.Min.Y = theBox.Min.Z = -1.0f;
	theBox.Max.X = theBox.Max.Y = theBox.Max.Z = 1.0f;
	geActor *theActor = CCD->Player()->GetActor();
	CurrentDistance = 4000.0f;
	
	if(WeaponD[CurrentWeapon].PBone[0]!='\0')
		geActor_GetBoneTransform(WeaponD[CurrentWeapon].PActor, WeaponD[CurrentWeapon].PBone, &Xf );
	else
		geActor_GetBoneTransform(WeaponD[CurrentWeapon].PActor, NULL, &Xf );
	thePosition = Xf.Translation;
	CCD->ActorManager()->GetRotate(theActor, &theRotation);
	geVec3d CRotation, CPosition;
	CCD->CameraManager()->GetCameraOffset(&CPosition, &CRotation);
	geXForm3d_SetIdentity(&Xf);
	geXForm3d_RotateZ(&Xf, theRotation.Z);
	geXForm3d_RotateX(&Xf, theRotation.X-CCD->CameraManager()->GetTilt());
	geXForm3d_RotateY(&Xf, theRotation.Y);
	geXForm3d_Translate(&Xf, thePosition.X, thePosition.Y, thePosition.Z);
	
	Pos = Xf.Translation;
	geXForm3d_GetIn(&Xf, &Direction);
	geVec3d_AddScaled (&Pos, &Direction, CurrentDistance, &Back);
	
	geVec3d_AddScaled (&Pos, &Direction, 0.0f, &Front);
	if(geWorld_Collision(CCD->World(), &theBox.Min, &theBox.Max, &Front, 
		&Back, GE_VISIBLE_CONTENTS, GE_COLLIDE_ALL, 0, NULL, NULL, &Collision))
    {
		CurrentDistance = (geFloat)fabs(geVec3d_DistanceBetween(&Collision.Impact, &Front));
		if(CurrentDistance < 0.0f)
			CurrentDistance = 0.0f;
		if(CurrentDistance > 4000.0f)
			CurrentDistance = 4000.0f;
		geVec3d_AddScaled (&Pos, &Direction, CurrentDistance, &Back);
	}
	
	geCamera_TransformAndProject(CCD->CameraManager()->Camera(), &Back, &ProjectedPoint); 

// end change RF063
}

void CWeapon::DisplayFirstPerson(int index)
{
	geVec3d theRotation;
	geVec3d thePosition;
	
	if(CCD->WeaponPosition()) // used to move actor around in setup mode
	{
// changed RF064
		if(!CCD->Player()->GetAlterKey())
		{
			if((GetAsyncKeyState(0x69) & 0x8000) != 0)
				WeaponD[index].G+=0.01f;
			if((GetAsyncKeyState(0x66) & 0x8000) != 0)
				WeaponD[index].G-=0.01f;
			
			if((GetAsyncKeyState(0x67) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) == 0)
				WeaponD[index].F+=0.1f;
			if((GetAsyncKeyState(0x68) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) == 0)
				WeaponD[index].F-=0.1f;
			
			if((GetAsyncKeyState(0x64) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) == 0)
				WeaponD[index].H+=0.1f;
			if((GetAsyncKeyState(0x65) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) == 0)
				WeaponD[index].H-=0.1f;
			
			if((GetAsyncKeyState(0x61) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) == 0)
				WeaponD[index].J+=0.1f;
			if((GetAsyncKeyState(0x62) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) == 0)
				WeaponD[index].J-=0.1f;
			
			if((GetAsyncKeyState(0x67) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) != 0)
				WeaponD[index].K+=0.1f;
			if((GetAsyncKeyState(0x68) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) != 0)
				WeaponD[index].K-=0.1f;
			
			if((GetAsyncKeyState(0x64) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) != 0)
				WeaponD[index].L+=0.1f;
			if((GetAsyncKeyState(0x65) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) != 0)
				WeaponD[index].L-=0.1f;
			
			if((GetAsyncKeyState(0x61) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) != 0)
				WeaponD[index].Z+=0.1f;
			if((GetAsyncKeyState(0x62) & 0x8000) != 0 && (GetAsyncKeyState(0x60) & 0x8000) != 0)
				WeaponD[index].Z-=0.1f;
		}
		else
			{
			if((GetAsyncKeyState(0x51) & 0x8000) != 0)
				WeaponD[index].G+=0.01f;
			if((GetAsyncKeyState(0x57) & 0x8000) != 0)
				WeaponD[index].G-=0.01f;
			
			if((GetAsyncKeyState(0x4A) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) == 0)
				WeaponD[index].F+=0.1f;
			if((GetAsyncKeyState(0x4B) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) == 0)
				WeaponD[index].F-=0.1f;
			
			if((GetAsyncKeyState(0x4D) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) == 0)
				WeaponD[index].H+=0.1f;
			if((GetAsyncKeyState(0x4E) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) == 0)
				WeaponD[index].H-=0.1f;
			
			if((GetAsyncKeyState(0x4F) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) == 0)
				WeaponD[index].J+=0.1f;
			if((GetAsyncKeyState(0x50) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) == 0)
				WeaponD[index].J-=0.1f;
			
			if((GetAsyncKeyState(0x4A) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) != 0)
				WeaponD[index].K+=0.1f;
			if((GetAsyncKeyState(0x4B) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) != 0)
				WeaponD[index].K-=0.1f;
			
			if((GetAsyncKeyState(0x4D) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) != 0)
				WeaponD[index].L+=0.1f;
			if((GetAsyncKeyState(0x4E) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) != 0)
				WeaponD[index].L-=0.1f;
			
			if((GetAsyncKeyState(0x4F) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) != 0)
				WeaponD[index].Z+=0.1f;
			if((GetAsyncKeyState(0x50) & 0x8000) != 0 && (GetAsyncKeyState(0x59) & 0x8000) != 0)
				WeaponD[index].Z-=0.1f;
		}
// end change RF064
	}
	
	geXForm3d XForm;
	geXForm3d_SetIdentity (&XForm);
	
	CCD->CameraManager()->TrackMotion();

	CCD->CameraManager()->GetRotation(&theRotation);
	if(CCD->WeaponPosition())
	{
		geActor_SetScale(WeaponD[index].VActor,WeaponD[index].VScale+WeaponD[index].G,WeaponD[index].VScale+WeaponD[index].G,WeaponD[index].VScale+WeaponD[index].G);
		geXForm3d_RotateX (&XForm, (0.0174532925199433f*(WeaponD[index].VActorRotation.X + WeaponD[index].K)));
		geXForm3d_RotateZ (&XForm, (0.0174532925199433f*(WeaponD[index].VActorRotation.Z + WeaponD[index].L)));
		geXForm3d_RotateY (&XForm, (0.0174532925199433f*(WeaponD[index].VActorRotation.Y + WeaponD[index].Z))); 
	}
	else
	{
		geActor_SetScale(WeaponD[index].VActor,WeaponD[index].VScale,WeaponD[index].VScale,WeaponD[index].VScale);
		// rotate actor into place
		geXForm3d_RotateX (&XForm, (0.0174532925199433f*(WeaponD[index].VActorRotation.X)));
		geXForm3d_RotateZ (&XForm, (0.0174532925199433f*(WeaponD[index].VActorRotation.Z)));
		geXForm3d_RotateY (&XForm, (0.0174532925199433f*(WeaponD[index].VActorRotation.Y))); 
	}
	
	geXForm3d_RotateX (&XForm, theRotation.Z + (0.0174532925199433f*(0)));  // rotate left/right
	geXForm3d_RotateZ (&XForm, theRotation.X + (0.0174532925199433f*(-5)));  // rotate up/down
	geXForm3d_RotateY (&XForm, theRotation.Y + (0.0174532925199433f*(90))); // swing side toside
	
	CCD->CameraManager()->GetPosition(&thePosition);
	float shx, shy, shz;
	CCD->CameraManager()->GetShake(&shx, &shy);
	shz = 0.0f;
	if(CCD->Player()->GetHeadBobbing())
	{
		if(WeaponD[index].BobAmt>0.0f)
			shz = CCD->CameraManager()->GetHeadBobOffset()/WeaponD[index].BobAmt;
	}
	geXForm3d_Translate (&XForm, thePosition.X+shx, thePosition.Y+shy, thePosition.Z); 

	geVec3d Forward;
	geVec3d Right;
	geVec3d Down; 
	
	geXForm3d_GetLeft (&XForm, &Forward);
	geXForm3d_GetUp (&XForm, &Right);
	geXForm3d_GetIn (&XForm, &Down); 
	
	if(CCD->WeaponPosition())
	{
		geVec3d_Scale (&Forward, WeaponD[index].VActorOffset.X + WeaponD[index].F, &Forward); // in 
		geVec3d_Scale (&Right, WeaponD[index].VActorOffset.Y + WeaponD[index].H, &Right);      // up 
		geVec3d_Scale (&Down, WeaponD[index].VActorOffset.Z + WeaponD[index].J, &Down);         // left 
	}
	else
	{
		geVec3d_Scale (&Forward, WeaponD[index].VActorOffset.X, &Forward); // in 
		geVec3d_Scale (&Right, WeaponD[index].VActorOffset.Y, &Right);      // up 
		geVec3d_Scale (&Down, WeaponD[index].VActorOffset.Z, &Down);         // left 
	}
	geXForm3d_Translate (&XForm, Forward.X, Forward.Y, Forward.Z);
	geXForm3d_Translate (&XForm, Right.X, Right.Y, Right.Z);
	geXForm3d_Translate (&XForm, Down.X, Down.Y, Down.Z); 

	geXForm3d_GetIn (&XForm, &Forward);
	geVec3d_AddScaled (&XForm.Translation, &Forward, shz, &XForm.Translation);

	geFloat deltaTime, tStart, tEnd; //, bStart, bEnd;
	geMotion *ActorMotion;
	
// changed RF063
	if(VAnimTime<0.0f)
		VAnimTime = (float)CCD->FreeRunningCounter();
// end change RF063
	deltaTime = 0.001f * (geFloat)(CCD->FreeRunningCounter() - VAnimTime);
	if(VSequence == VWEPHOLSTER)
		VMCounter -= (deltaTime*WeaponD[index].VAnimSpeed);
	else
		VMCounter += (deltaTime*WeaponD[index].VAnimSpeed);
	
	if(VSequence == VWEPCHANGE)
		ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VArm);
	if(VSequence == VWEPHOLSTER)
		ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VArm);
	if(VSequence == VWEPIDLE)
		ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VIdle);
	if(VSequence == VWEPATTACK)
		ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VAttack);
// changed RF064
	if(VSequence == VWEPRELOAD)
		ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VReload);
	if(VSequence == VWEPKEYRELOAD)
		ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VKeyReload);
	if(VSequence == VWEPATTACKEMPTY)
		ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VAttackEmpty);
// end change RF064
	if(VSequence == VWEPALTATTACK)
	{
		ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VAltAttack);
		if(!ActorMotion)
		{
			ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VAttack);
			VSequence = VWEPATTACK;
		}
	}
	if(VSequence == VWEPHIT)
		ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VHit);
	if(VSequence == VWEPALTHIT)
	{
		ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VAltHit);
		if(!ActorMotion)
		{
			ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VHit);
			VSequence = VWEPHIT;
		}
	}
	if(VSequence == VWEPWALK)
		ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VWalk);
	
	if(!ActorMotion) // no animation so use default
		ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VIdle);
	// test if at end of animation
	if(ActorMotion)
	{
		geMotion_GetTimeExtents(ActorMotion, &tStart, &tEnd);
		if(VMCounter <= tStart && VSequence == VWEPHOLSTER) // finish holster
		{
			CurrentWeapon = 11;
			geWorld_SetActorFlags(CCD->World(), WeaponD[index].VActor, 0);
// changed RF064
			if(dropflag)
			{
				CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
				theInv->Modify(WeaponD[index].Name, -1);
				Slot[WeaponD[index].Slot] = -1;
				dropflag = false;
			}
// end change RF064
		}
		if(VMCounter > tEnd && VSequence == VWEPCHANGE) // switch from change to idle
		{
			VSequence = VWEPIDLE;
			ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VIdle);
		}
// changed RF064
		if(VMCounter > tEnd && (VSequence == VWEPATTACK || VSequence == VWEPATTACKEMPTY
			|| VSequence == VWEPALTATTACK || VSequence == VWEPHIT 
			|| VSequence == VWEPALTHIT || VSequence == VWEPRELOAD
			 || VSequence == VWEPKEYRELOAD)) // switch from shoot to idle
		{
			geActor *theActor = CCD->Player()->GetActor();
			CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(theActor);
			if(VSequence == VWEPKEYRELOAD || VSequence == VWEPRELOAD)
			{
				WeaponD[index].ShotFired = 0;
				if(theInv->Value(WeaponD[index].Ammunition)>=WeaponD[index].ShotperMag)
					WeaponD[index].MagAmt = WeaponD[index].ShotperMag;
				else
					WeaponD[index].MagAmt = theInv->Value(WeaponD[index].Ammunition);
			}
			bool magflag = false;
			if(WeaponD[index].Catagory==PROJECTILE && VSequence == VWEPATTACK)
			{
				if(WeaponD[index].ShotperMag>0 && WeaponD[index].ShotFired>=WeaponD[index].ShotperMag)
				{
					if(theInv->Value(WeaponD[index].Ammunition)>=WeaponD[index].AmmoPerShot)
					{
						magflag = true;
						VSequence = VWEPRELOAD;
						ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VReload);
						if(!EffectC_IsStringNull(WeaponD[CurrentWeapon].ReloadSound))
						{
							Snd Sound;
							memset( &Sound, 0, sizeof( Sound ) );
							CCD->CameraManager()->GetPosition(&Sound.Pos);
							Sound.Min=CCD->GetAudibleRadius();
							Sound.Loop=GE_FALSE;
							Sound.SoundDef=SPool_Sound(WeaponD[CurrentWeapon].ReloadSound);
							CCD->EffectManager()->Item_Add(EFF_SND, (void *)&Sound);
						}
					}
				}
			}
			if(!magflag)
			{
				if(CCD->Player()->GetMoving() == MOVEIDLE)
				{
					VSequence = VWEPIDLE;
					ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VIdle);
				}
				else
				{
					VSequence = VWEPWALK;
					ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VWalk);
				}
			}
// end change RF064
			VBlend = GE_FALSE;
		}
		if(VMCounter > tEnd)
			VMCounter = 0.0f; // Wrap animation

		if(VSequence == VWEPWALK && VMCounter == 0.0f && (CCD->Player()->GetMoving() == MOVEIDLE))
		{
			VSequence = VWEPIDLE;
			VBlend = GE_FALSE; // all done blending
		}

		if(VSequence == VWEPIDLE && VMCounter == 0.0f && (CCD->Player()->GetMoving() != MOVEIDLE))
		{
			VSequence = VWEPWALK;
			VBlend = GE_FALSE; // all done blending
		}
		
		// set actor animation
		
		geActor_SetPose(WeaponD[index].VActor, ActorMotion, VMCounter, &XForm);
	}
	else
		geActor_ClearPose(WeaponD[index].VActor, &XForm);

	VAnimTime = (float)CCD->FreeRunningCounter();
	if(MFlash)
	{
		geVec3d Muzzle;
		geXForm3d Xf;
		
		if(geActor_GetBoneTransform(WeaponD[CurrentWeapon].VActor, WeaponD[CurrentWeapon].VBone, &Xf )==GE_TRUE)
		{
			geVec3d_Copy( &( Xf.Translation ), &Muzzle);
			CCD->Explosions()->AddExplosion(WeaponD[CurrentWeapon].MuzzleFlash, Muzzle, WeaponD[CurrentWeapon].VActor, WeaponD[CurrentWeapon].VBone);
		}
		
		MFlash = false;
	}
}

void CWeapon::Holster()
{
	if(CurrentWeapon==-1 || CurrentWeapon==11)
		return;
	
	if(ViewPoint==FIRSTPERSON)
	{
		if(VSequence == VWEPHOLSTER)
			return;
		geMotion *ActorMotion = ActorMotion = geActor_GetMotionByName(WeaponD[CurrentWeapon].VActorDef, WeaponD[CurrentWeapon].VArm);
		if(ActorMotion)
		{
			geFloat tStart, tEnd;
			geMotion_GetTimeExtents(ActorMotion, &tStart, &tEnd);
			VMCounter = tEnd;
			VSequence = VWEPHOLSTER;
// changed RF063
			VAnimTime = -1.0f;
// end change RF063
			AttackTime = 0;
			VBlend = GE_FALSE;
			AttackFlag = false;
		}
		else
			CurrentWeapon=11;
	}
	else
	{
			AttackFlag = false;
			geWorld_SetActorFlags(CCD->World(), WeaponD[CurrentWeapon].PActor, 0);
			CurrentWeapon=11;
	}
}

void CWeapon::DoAttack()
{
	if(AttackFlag)
	{
		AttackFlag = false;

		if(WeaponD[CurrentWeapon].Catagory==PROJECTILE)
			ProjectileAttack();
		else if(WeaponD[CurrentWeapon].Catagory==MELEE)
		{
			MeleeAttack();
		}
		else if(WeaponD[CurrentWeapon].Catagory==BEAM)
		{
		} 
	}
}

// changed RF063
int CWeapon::SaveTo(FILE *SaveFD)
{
	fwrite(&ViewPoint, sizeof(int), 1, SaveFD);
	fwrite(&Slot, sizeof(int), 10, SaveFD);
	fwrite(&CurrentWeapon, sizeof(int), 1, SaveFD);

	return RGF_SUCCESS;
}

int CWeapon::RestoreFrom(FILE *RestoreFD)
{
	fread(&ViewPoint, sizeof(int), 1, RestoreFD);
	fread(&Slot, sizeof(int), 10, RestoreFD);
	fread(&CurrentWeapon, sizeof(int), 1, RestoreFD);
	ReSetWeapon(CurrentWeapon);

	return RGF_SUCCESS;
}
// end change RF063

//---------------------
// change to new weapon
//---------------------

void CWeapon::SetWeapon(int value)
{ 
	if(Slot[value] == -1)
		return;

	if(!(CurrentWeapon == -1 || CurrentWeapon == 11))
	{
		if(WeaponD[CurrentWeapon].Slot == value) // same weapon?
			return;
		// stop displaying old weapon
		geWorld_SetActorFlags(CCD->World(), WeaponD[CurrentWeapon].VActor, 0);
		geWorld_SetActorFlags(CCD->World(), WeaponD[CurrentWeapon].PActor, 0);
		if(WeaponD[CurrentWeapon].Catagory==PROJECTILE)
			CCD->HUD()->ActivateElement(WeaponD[CurrentWeapon].Ammunition, false);
	}
// changed RF063
	char *Motion = CCD->ActorManager()->GetMotion(CCD->Player()->GetActor());
	int index = -1;
	for(int i=0;i<ANIMMAX;i++)
	{
		if(!strcmp(Motion, WeaponD[CurrentWeapon].Animations[i]))
		{
			index = i;
			break;
		}
	}
	CurrentWeapon = Slot[value]; // change to new weapon
	if(index!=-1)
	{
		CCD->ActorManager()->SetMotion(CCD->Player()->GetActor(), PlayerAnim(index));
	}
// end change RF063
// changed RF064
	if(WeaponD[CurrentWeapon].Catagory==PROJECTILE)
	{
		CCD->HUD()->ActivateElement(WeaponD[CurrentWeapon].Ammunition, true);
		if(WeaponD[CurrentWeapon].ShotperMag>0)
		{
			if(WeaponD[CurrentWeapon].MagAmt==-1)
			{
				geActor *theActor = CCD->Player()->GetActor();
				CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(theActor);
				if(theInv->Value(WeaponD[CurrentWeapon].Ammunition)>=WeaponD[CurrentWeapon].ShotperMag)
					WeaponD[CurrentWeapon].MagAmt = WeaponD[CurrentWeapon].ShotperMag;
				else
					WeaponD[CurrentWeapon].MagAmt = theInv->Value(WeaponD[CurrentWeapon].Ammunition);
			}
		}
	}
	DoChange();
// end change RF064
	VSequence = VWEPCHANGE;
// changed RF063
	VAnimTime = -1.0f;
// end change RF063
	AttackTime = 0;
	VMCounter = 0.0f;
	VBlend = GE_FALSE;
	AttackFlag = false;
}

// changed RF064
void CWeapon::DoChange()
{
	if(WeaponD[CurrentWeapon].FixedView!=-1)
	{
		if(ViewPoint!=WeaponD[CurrentWeapon].FixedView)
		{
			int TempView = OldViewPoint;
			CCD->Player()->SwitchCamera(WeaponD[CurrentWeapon].FixedView);
			OldViewPoint = TempView;
		}
	}
	else
	{
		if(ViewPoint!=OldViewPoint)
		{
			int TempView = OldViewPoint;
			CCD->Player()->SwitchCamera(OldViewPoint);
			OldViewPoint = TempView;
		}
	}
	if(ViewPoint==FIRSTPERSON)
	{
		geWorld_SetActorFlags(CCD->World(), WeaponD[CurrentWeapon].VActor, GE_ACTOR_RENDER_NORMAL);
	}
	else
	{
		geWorld_SetActorFlags(CCD->World(), WeaponD[CurrentWeapon].PActor, GE_ACTOR_RENDER_NORMAL | GE_ACTOR_RENDER_MIRRORS);
	}
}
// end change RF064

void CWeapon::ClearWeapon()
{ 
	if(!(CurrentWeapon == -1 || CurrentWeapon == 11))
	{
		if(WeaponD[CurrentWeapon].Catagory==PROJECTILE)
			CCD->HUD()->ActivateElement(WeaponD[CurrentWeapon].Ammunition, false);
	}
}

void CWeapon::ReSetWeapon(int value)
{ 
// changed RF063
	char *Motion = CCD->ActorManager()->GetMotion(CCD->Player()->GetActor());
	int index = -1;
	if(!(value==-1 || value==11))
	{
		for(int i=0;i<ANIMMAX;i++)
		{
			if(!strcmp(Motion, WeaponD[CurrentWeapon].Animations[i]))
			{
				index = i;
				break;
			}
		}
	}
	CurrentWeapon = value; // change to new weapon
	if(index!=-1)
	{
		CCD->ActorManager()->SetMotion(CCD->Player()->GetActor(), PlayerAnim(index));
	}
// end change RF063
	if(value==-1 || value==11)
		return;
// changed RF064
	if(WeaponD[CurrentWeapon].Catagory==PROJECTILE)
	{
		CCD->HUD()->ActivateElement(WeaponD[CurrentWeapon].Ammunition, true);
		if(WeaponD[CurrentWeapon].MagAmt==-1)
		{
			geActor *theActor = CCD->Player()->GetActor();
			CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(theActor);
			if(theInv->Value(WeaponD[CurrentWeapon].Ammunition)>=WeaponD[CurrentWeapon].ShotperMag)
				WeaponD[CurrentWeapon].MagAmt = WeaponD[CurrentWeapon].ShotperMag;
			else
				WeaponD[CurrentWeapon].MagAmt = theInv->Value(WeaponD[CurrentWeapon].Ammunition);
		}
	}
	DoChange();
// end change RF064
	VSequence = VWEPCHANGE;
// changed RF063
	VAnimTime = -1.0f;
// end change RF063
	AttackTime = 0;
	VMCounter = 0.0f;
	VBlend = GE_FALSE;
	AttackFlag = false;
}

bool CWeapon::CrossHair()
{
	if(CurrentWeapon==-1 || CurrentWeapon == 11)
		return false;
// changed RF063
	if(WeaponD[CurrentWeapon].Catagory!=PROJECTILE || !(ViewPoint==FIRSTPERSON || ViewPoint==THIRDPERSON))
		return false;
	return true;
	
}

bool CWeapon::CrossHairFixed()
{
	return WeaponD[CurrentWeapon].CrossHairFixed;
}

void CWeapon::DisplayCrossHair()
{
	geBitmap *theBmp;
	geBitmap_Info	BmpInfo;
	int x, y;
// changed RF063
	theBmp = WeaponD[CurrentWeapon].CrossHair;
	if(theBmp)
	{
		if(geBitmap_GetInfo(theBmp, &BmpInfo, NULL) == GE_TRUE)
		{
			x = (CCD->Engine()->Width() - BmpInfo.Width) / 2;
			y = (CCD->Engine()->Height() - BmpInfo.Height) / 2;
			geEngine_DrawBitmap(CCD->Engine()->Engine(), theBmp, NULL, x, y );
		}
	}
// end change RF063
}

geBitmap *CWeapon::GetCrossHair()
{
	geBitmap *theBmp;
// changed RF063
	theBmp = WeaponD[CurrentWeapon].CrossHair;
	return theBmp;
// end change RF063
}

char *CWeapon::PlayerAnim(int index)
{
	if(!(CurrentWeapon == -1 || CurrentWeapon == 11))
	{
		if(!EffectC_IsStringNull(WeaponD[CurrentWeapon].Animations[index]))
		{
			return WeaponD[CurrentWeapon].Animations[index];
		}
	}

	return CCD->Player()->GetAnimations(index);
}

// changed RF063
char *CWeapon::DieAnim()
{
	if(!(CurrentWeapon == -1 || CurrentWeapon == 11))
	{
		if(WeaponD[CurrentWeapon].DieAnimAmt>0)
		{
			return WeaponD[CurrentWeapon].DieAnim[rand()%WeaponD[CurrentWeapon].DieAnimAmt];
		}
	}

	return NULL;
}

char *CWeapon::InjuryAnim()
{
	if(!(CurrentWeapon == -1 || CurrentWeapon == 11))
	{
		if(WeaponD[CurrentWeapon].InjuryAnimAmt>0)
		{
			return WeaponD[CurrentWeapon].InjuryAnim[rand()%WeaponD[CurrentWeapon].InjuryAnimAmt];
		}
	}

	return NULL;
}
// end change RF063

void CWeapon::WeaponData()
{ 
	char szData[256];
	
	if(!(CurrentWeapon == -1 || CurrentWeapon == 11) && ViewPoint == FIRSTPERSON)
	{
		if(CCD->WeaponPosition())
		{
			sprintf(szData,"Rotation : X= %.2f, Y= %.2f, Z= %.2f",
				WeaponD[CurrentWeapon].VActorRotation.X + WeaponD[CurrentWeapon].K, WeaponD[CurrentWeapon].VActorRotation.Y + WeaponD[CurrentWeapon].Z, WeaponD[CurrentWeapon].VActorRotation.Z + WeaponD[CurrentWeapon].L);
			CCD->MenuManager()->WorldFontRect(szData, FONT8, 5, CCD->Engine()->Height()- 40);
			
			sprintf(szData,"Offset : X= %.2f, Y= %.2f, Z= %.2f, Scale : %.2f",
				WeaponD[CurrentWeapon].VActorOffset.X + WeaponD[CurrentWeapon].F, WeaponD[CurrentWeapon].VActorOffset.Y + WeaponD[CurrentWeapon].H, WeaponD[CurrentWeapon].VActorOffset.Z + WeaponD[CurrentWeapon].J,
				WeaponD[CurrentWeapon].VScale+WeaponD[CurrentWeapon].G);
			CCD->MenuManager()->WorldFontRect(szData, FONT8, 5, CCD->Engine()->Height()- 30);
		}
	}
	return;
}

// changed RF064

void CWeapon::KeyReload()
{
	if(CurrentWeapon == -1 || CurrentWeapon == 11)
		return;
	if(WeaponD[CurrentWeapon].Catagory!=PROJECTILE)
		return;
	if(WeaponD[CurrentWeapon].ShotperMag==0)
		return;

	if(ViewPoint==FIRSTPERSON)
	{
		if(VSequence == VWEPCHANGE || VSequence == VWEPHOLSTER || VSequence == VWEPATTACKEMPTY
			|| VSequence == VWEPRELOAD || VSequence == VWEPKEYRELOAD)
			return; // shooting or changing weapon
		geActor *theActor = CCD->Player()->GetActor();
		CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(theActor);
		if(theInv->Value(WeaponD[CurrentWeapon].Ammunition)>=WeaponD[CurrentWeapon].AmmoPerShot)
		{
			VSequence = VWEPRELOAD;
			if(!EffectC_IsStringNull(WeaponD[CurrentWeapon].VKeyReload))
				VSequence = VWEPKEYRELOAD;
			VMCounter = 0.0f;
			int LeftInMag = WeaponD[CurrentWeapon].ShotperMag - WeaponD[CurrentWeapon].ShotFired;
			if(LeftInMag>0 && WeaponD[CurrentWeapon].LooseMag)
				theInv->Modify(WeaponD[CurrentWeapon].Ammunition, -LeftInMag);
			if(!EffectC_IsStringNull(WeaponD[CurrentWeapon].ReloadSound))
			{
				Snd Sound;
				memset( &Sound, 0, sizeof( Sound ) );
				CCD->CameraManager()->GetPosition(&Sound.Pos);
				Sound.Min=CCD->GetAudibleRadius();
				Sound.Loop=GE_FALSE;
				Sound.SoundDef=SPool_Sound(WeaponD[CurrentWeapon].ReloadSound);
				CCD->EffectManager()->Item_Add(EFF_SND, (void *)&Sound);
			}
		}
	}
	else
	{
	}
}

void CWeapon::DropWeapon()
{
	if(CurrentWeapon == -1 || CurrentWeapon == 11)
		return;

	dropflag = true;
	Holster();
}

// end change RF064

void CWeapon::Attack(bool Alternate)
{
	if(CurrentWeapon == -1 || CurrentWeapon == 11)
		return;
	
	AltAttack = Alternate;
	
	if(ViewPoint==FIRSTPERSON)
	{
// changed RF064
		if(VSequence == VWEPCHANGE || VSequence == VWEPHOLSTER || VSequence == VWEPATTACKEMPTY
			|| VSequence == VWEPRELOAD || VSequence == VWEPKEYRELOAD
			|| (WeaponD[CurrentWeapon].Catagory==MELEE
			&& (VSequence == VWEPATTACK || VSequence == VWEPALTATTACK 
			|| VSequence == VWEPHIT || VSequence == VWEPALTHIT)))
			return; // shooting or changing weapon
// end change RF064
		if(AttackTime != 0)
		{
			int dtime = CCD->FreeRunningCounter()-AttackTime;
			if(dtime<(WeaponD[CurrentWeapon].FireRate*1000.0f))
				return; // too soon
		}
// changed RF064
		if(WeaponD[CurrentWeapon].ShotperMag>0 && WeaponD[CurrentWeapon].ShotFired>=WeaponD[CurrentWeapon].ShotperMag)
			return;
		AttackTime = CCD->FreeRunningCounter();
		VSequence = VWEPATTACK; // shooting animation
		if(WeaponD[CurrentWeapon].Catagory==MELEE)
		{
			if(((geFloat)((rand() % 1000) + 1) / 1000.0f * 10.0f)>=5.0f)
				VSequence = VWEPALTATTACK;
		}

		if(WeaponD[CurrentWeapon].Catagory==PROJECTILE)
		{
			geActor *theActor = CCD->Player()->GetActor();
			CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(theActor);
			if(theInv->Value(WeaponD[CurrentWeapon].Ammunition)<WeaponD[CurrentWeapon].AmmoPerShot)
			{
				if(!EffectC_IsStringNull(WeaponD[CurrentWeapon].VAttackEmpty))
					VSequence = VWEPATTACKEMPTY;
			}
		}
// end change RF064
		VMCounter = 0.0f;
		AttackFlag = true;
	}
	else
	{
		if(AttackTime != 0)
		{
// changed RF064
			if(WeaponD[CurrentWeapon].ShotperMag>0 && WeaponD[CurrentWeapon].ShotFired>=WeaponD[CurrentWeapon].ShotperMag)
			{
				WeaponD[CurrentWeapon].ShotFired = 0;
				geActor *theActor = CCD->Player()->GetActor();
				CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(theActor);
				if(theInv->Value(WeaponD[CurrentWeapon].Ammunition)>=WeaponD[CurrentWeapon].ShotperMag)
					WeaponD[CurrentWeapon].MagAmt = WeaponD[CurrentWeapon].ShotperMag;
				else
					WeaponD[CurrentWeapon].MagAmt = theInv->Value(WeaponD[CurrentWeapon].Ammunition);
			}
// end change RF064
			int dtime = CCD->FreeRunningCounter()-AttackTime;
			if(dtime<(WeaponD[CurrentWeapon].FireRate*1000.0f))
				return; // too soon
		}
		AttackTime = CCD->FreeRunningCounter();
		AttackFlag = true;
	}
}

void CWeapon::Sound(bool Attack, geVec3d Origin, bool Empty)
{
	Snd Sound;
	
	memset( &Sound, 0, sizeof( Sound ) );
    geVec3d_Copy( &(Origin), &( Sound.Pos ) );
    Sound.Min=CCD->GetAudibleRadius();
    Sound.Loop=GE_FALSE;
	if(Attack)
	{
		if(!Empty)
		{
			if(EffectC_IsStringNull(WeaponD[CurrentWeapon].AttackSound))
				return;
			Sound.SoundDef=SPool_Sound(WeaponD[CurrentWeapon].AttackSound);
		}
		else
		{
			if(EffectC_IsStringNull(WeaponD[CurrentWeapon].EmptySound))
				return;
			Sound.SoundDef=SPool_Sound(WeaponD[CurrentWeapon].EmptySound);
		}
	}
	else
	{
		if(EffectC_IsStringNull(WeaponD[CurrentWeapon].HitSound))
			return;
		Sound.SoundDef=SPool_Sound(WeaponD[CurrentWeapon].HitSound);
	}
	if(!Sound.SoundDef)
	{
		char szError[256];
		sprintf(szError,"Can't open audio file for Weapon %d\n", Slot[CurrentWeapon]);
		CCD->ReportError(szError, false);
		return;
	}
    CCD->EffectManager()->Item_Add(EFF_SND, (void *)&Sound);
}


void CWeapon::MeleeAttack()
{
	geExtBox theBox;
	geVec3d Forward, Pos;
	GE_Collision Collision;
	geXForm3d Xf;
	geVec3d theRotation, thePosition;

	CCD->ActorManager()->GetPosition(CCD->Player()->GetActor(), &thePosition);
	CCD->ActorManager()->GetBoundingBox(CCD->Player()->GetActor(), &theBox);
	thePosition.Y += theBox.Max.Y/2.0f;
	CCD->ActorManager()->GetRotate(CCD->Player()->GetActor(), &theRotation);

	float temp = (theBox.Max.Y-theBox.Min.Y)/4.0f;
	theBox.Min.Y = -temp;
	theBox.Max.Y = temp;
	float tiltp = (CCD->CameraManager()->GetTiltPercent()*2.0f)-1.0f;
	thePosition.Y += (temp*tiltp);
	
	geXForm3d_SetIdentity(&Xf);
	geXForm3d_RotateZ(&Xf, theRotation.Z);
	geXForm3d_RotateX(&Xf, theRotation.X);
	geXForm3d_RotateY(&Xf, theRotation.Y);
	geXForm3d_Translate(&Xf, thePosition.X, thePosition.Y, thePosition.Z);

	geXForm3d_GetIn (&Xf, &Forward);
	float distance = 1.25f*(((theBox.Max.X) < (theBox.Max.Z)) ? (theBox.Max.X) : (theBox.Max.Z));
	geVec3d_AddScaled (&Xf.Translation, &Forward, distance, &Pos);

	Collision.Actor = NULL;
	Collision.Model = NULL;
	
	//DrawBoundBox(CCD->World(), &Pos, &theBox.Min, &theBox.Max);
	//DrawBoundBox(CCD->World(), &Xf.Translation, &theBox.Min, &theBox.Max);

	bool result = false;
	geWorld_Model *pModel;
	geActor *pActor;
	theBox.Min.X += Pos.X;
	theBox.Min.Y += Pos.Y;
	theBox.Min.Z += Pos.Z;
	theBox.Max.X += Pos.X;
	theBox.Max.Y += Pos.Y;
	theBox.Max.Z += Pos.Z;
	if(CCD->ActorManager()->DoesBoxHitActor(Pos, theBox, &pActor, CCD->Player()->GetActor()) == GE_TRUE)
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
		bool Stamina = true;
		if(!EffectC_IsStringNull(WeaponD[CurrentWeapon].Ammunition))
		{
			CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
			if(theInv->Value(WeaponD[CurrentWeapon].Ammunition)<WeaponD[CurrentWeapon].AmmoPerShot)
				Stamina = false;
			else
				theInv->Modify(WeaponD[CurrentWeapon].Ammunition, -WeaponD[CurrentWeapon].AmmoPerShot);
		}

		if(Stamina)
		{
			int nHitType = CCD->Collision()->ProcessCollision(Collision, NULL, false);

			if(nHitType != kNoCollision)
			{
				if(VSequence == VWEPATTACK)
					VSequence = VWEPHIT;
				else
					VSequence = VWEPALTHIT;
				Sound(false, thePosition, false);
				
				//
				// Process damage here
				//
// changed RF063				
				if(Collision.Actor!=NULL)
				{
					CCD->Damage()->DamageActor(Collision.Actor, WeaponD[CurrentWeapon].MeleeDamage, WeaponD[CurrentWeapon].Attribute, WeaponD[CurrentWeapon].MeleeAltDamage, WeaponD[CurrentWeapon].AltAttribute, WeaponD[CurrentWeapon].Name);
					CCD->Explosions()->AddExplosion(WeaponD[CurrentWeapon].MeleeExplosion, Pos, NULL, NULL);
				}

				if(Collision.Model)
					CCD->Damage()->DamageModel(Collision.Model, WeaponD[CurrentWeapon].MeleeDamage, WeaponD[CurrentWeapon].Attribute, WeaponD[CurrentWeapon].MeleeAltDamage, WeaponD[CurrentWeapon].AltAttribute);
// end change RF063
			}
			else 
				Sound(true, thePosition, false);
		}
		else 
			Sound(true, thePosition, false);
	}
	else 
		Sound(true, thePosition, false);
	
}


void CWeapon::ProjectileAttack()
{
	geVec3d theRotation;
	geVec3d thePosition;
	geXForm3d Xf;
	geVec3d Front, Back;
	geVec3d Direction, Pos, Orient;
	geExtBox theBox;
	GE_Collision Collision;
	geFloat CurrentDistance, x;
	
	theBox.Min.X = theBox.Min.Y = theBox.Min.Z = -1.0f;
	theBox.Max.X = theBox.Max.Y = theBox.Max.Z = 1.0f;
	geActor *theActor = CCD->Player()->GetActor();
	CurrentDistance = 4000.0f;
	
	if(ViewPoint==FIRSTPERSON)
	{
		CCD->CameraManager()->GetRotation(&theRotation);
		CCD->CameraManager()->GetPosition(&thePosition);
		geXForm3d_SetIdentity(&Xf);
		geXForm3d_RotateZ(&Xf, theRotation.Z);
		geXForm3d_RotateX(&Xf, theRotation.X);
		geXForm3d_RotateY(&Xf, theRotation.Y);
		geXForm3d_Translate(&Xf, thePosition.X, thePosition.Y, thePosition.Z);
	}
	else
	{
// changed RF063
		if(WeaponD[CurrentWeapon].PBone[0]!='\0')
			geActor_GetBoneTransform(theActor, WeaponD[CurrentWeapon].PBone, &Xf );
		else
			geActor_GetBoneTransform(theActor, NULL, &Xf );
// end change RF063
		thePosition = Xf.Translation;
		CCD->ActorManager()->GetRotate(theActor, &theRotation);
		geVec3d CRotation, CPosition;
		CCD->CameraManager()->GetCameraOffset(&CPosition, &CRotation);
		geXForm3d_SetIdentity(&Xf);
		geXForm3d_RotateZ(&Xf, theRotation.Z);
		geXForm3d_RotateX(&Xf, theRotation.X-CCD->CameraManager()->GetTilt());
		geXForm3d_RotateY(&Xf, theRotation.Y);
		geXForm3d_Translate(&Xf, thePosition.X, thePosition.Y, thePosition.Z);
	}
	
	Pos = Xf.Translation;
	geXForm3d_GetIn(&Xf, &Direction);
	geVec3d_AddScaled (&Pos, &Direction, CurrentDistance, &Back);
	
	geVec3d_AddScaled (&Pos, &Direction, 0.0f, &Front);
	if(geWorld_Collision(CCD->World(), &theBox.Min, &theBox.Max, &Front, 
		&Back, GE_VISIBLE_CONTENTS, GE_COLLIDE_ALL, 0, NULL, NULL, &Collision))
    {
		CurrentDistance = (geFloat)fabs(geVec3d_DistanceBetween(&Collision.Impact, &Front));
		if(CurrentDistance < 0.0f)
			CurrentDistance = 0.0f;
		if(CurrentDistance > 4000.0f)
			CurrentDistance = 4000.0f;
		geVec3d_AddScaled (&Pos, &Direction, CurrentDistance, &Back);
	}

	if(ViewPoint==FIRSTPERSON)
	{
		if(WeaponD[CurrentWeapon].VBone[0] != '\0')
		{
			geXForm3d Xf;
			if(geActor_GetBoneTransform(WeaponD[CurrentWeapon].VActor, WeaponD[CurrentWeapon].VBone, &Xf )==GE_TRUE)
				geVec3d_Copy( &( Xf.Translation ), &Pos);
			else
			{
				geXForm3d_GetUp(&Xf, &Direction);
				geVec3d_AddScaled (&Pos, &Direction, WeaponD[CurrentWeapon].VOffset.Y, &Pos);
				geXForm3d_GetLeft(&Xf, &Direction);
				geVec3d_AddScaled (&Pos, &Direction, WeaponD[CurrentWeapon].VOffset.X, &Pos);
			}
		}
		else
		{
			geXForm3d_GetUp(&Xf, &Direction);
			geVec3d_AddScaled (&Pos, &Direction, WeaponD[CurrentWeapon].VOffset.Y, &Pos);
			geXForm3d_GetLeft(&Xf, &Direction);
			geVec3d_AddScaled (&Pos, &Direction, WeaponD[CurrentWeapon].VOffset.X, &Pos);
		}
	}
	else
	{
		geXForm3d_GetUp(&Xf, &Direction);
		geVec3d_AddScaled (&Pos, &Direction, WeaponD[CurrentWeapon].POffset.Y, &Pos);
		geXForm3d_GetLeft(&Xf, &Direction);
		geVec3d_AddScaled (&Pos, &Direction, WeaponD[CurrentWeapon].POffset.X, &Pos);
	}
	
	geVec3d_Subtract( &Back, &Pos, &Orient );
	float l = geVec3d_Length(&Orient);
	
	// protect from Div by Zero
	
	if(l > 0.0f) 
	{
		x = Orient.X;
		Orient.X = (float)( GE_PI*0.5 ) - (float)acos(Orient.Y / l);
		Orient.Y = (float)atan2( x , Orient.Z ) + GE_PI;
		// roll is zero - always!!?
		Orient.Z = 0.0;
		
		geXForm3d_SetIdentity(&Xf);
		geXForm3d_RotateZ(&Xf, Orient.Z);
		geXForm3d_RotateX(&Xf, Orient.X);
		geXForm3d_RotateY(&Xf, Orient.Y);
		geVec3d_Set(&Xf.Translation, Pos.X,Pos.Y,Pos.Z);
		geXForm3d_GetIn(&Xf, &Direction);
		
		geVec3d_AddScaled (&Pos, &Direction, 1.0f, &Front);
		Collision.Actor = (geActor *)NULL;
		CCD->Collision()->IgnoreContents(false);
		while(CCD->Collision()->CheckForCollision(&theBox.Min, &theBox.Max,
			Front, Front, &Collision))
		{
			if(Collision.Actor!=theActor)
				break;
			geVec3d_AddScaled (&Front, &Direction, 1.0f, &Front);
			Collision.Actor = (geActor *)NULL;
		}
		
		if(ViewPoint==FIRSTPERSON)
			geVec3d_AddScaled (&Front, &Direction, WeaponD[CurrentWeapon].VOffset.Z, &Pos);
		else
		{
			geVec3d_AddScaled (&Front, &Direction, WeaponD[CurrentWeapon].POffset.Z, &Pos);
		}
// changed RF063
		GE_Contents ZoneContents;
		if(geWorld_GetContents(CCD->World(), &Pos, &theBox.Min, 
		&theBox.Max, GE_COLLIDE_MODELS, 0, NULL, NULL, &ZoneContents) == GE_TRUE)
		{
			Liquid * LQ = CCD->Liquids()->IsLiquid(ZoneContents.Model);
			if(LQ)
			{
				if(!WeaponD[CurrentWeapon].WorksUnderwater)
				{
					Sound(true, thePosition, true);
					return;
				}
			}
		}
// end change RF063
		CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(theActor);
		if(theInv->Value(WeaponD[CurrentWeapon].Ammunition)>=WeaponD[CurrentWeapon].AmmoPerShot)
		{
			theInv->Modify(WeaponD[CurrentWeapon].Ammunition, -WeaponD[CurrentWeapon].AmmoPerShot);
// changed RF063
			Add_Projectile(Pos, Front, Orient, WeaponD[CurrentWeapon].Projectile, WeaponD[CurrentWeapon].Attribute, WeaponD[CurrentWeapon].AltAttribute);
// changed RF064
			WeaponD[CurrentWeapon].ShotFired += 1;
// end change RF064
			if(ViewPoint==FIRSTPERSON)
			{
				if(WeaponD[CurrentWeapon].JerkAmt>0.0f)
				{
					CCD->CameraManager()->SetJerk(WeaponD[CurrentWeapon].JerkAmt,WeaponD[CurrentWeapon].JerkDecay);
				}
				if(WeaponD[CurrentWeapon].MuzzleFlash[0] != '\0')
				{
					if(WeaponD[CurrentWeapon].VBone[0] != '\0')
					{
					/*	geVec3d Muzzle;
						geXForm3d Xf;
						
						if(geActor_GetBoneTransform(WeaponD[CurrentWeapon].VActor, WeaponD[CurrentWeapon].VBone, &Xf )==GE_TRUE)
						{
							geVec3d_Copy( &( Xf.Translation ), &Muzzle);
							CCD->Explosions()->AddExplosion(WeaponD[CurrentWeapon].MuzzleFlash, Muzzle, WeaponD[CurrentWeapon].VActor, WeaponD[CurrentWeapon].VBone);
						} */
						MFlash = true;
					}
					else
						CCD->Explosions()->AddExplosion(WeaponD[CurrentWeapon].MuzzleFlash, Front, NULL, NULL);
				}
			}
			else
			{
				if(WeaponD[CurrentWeapon].PMOffset>0.0f)
				{
					if(WeaponD[CurrentWeapon].PBone[0] != '\0')
						MFlash = true;
					else
					{
						geVec3d_AddScaled (&Front, &Direction, WeaponD[CurrentWeapon].PMOffset, &Front);
						CCD->Explosions()->AddExplosion(WeaponD[CurrentWeapon].MuzzleFlash3rd, Front, NULL, NULL);
					}
				}
				else
				{
					if(WeaponD[CurrentWeapon].MuzzleFlash3rd[0] != '\0')
					{
						if(WeaponD[CurrentWeapon].PBone[0] != '\0')
							MFlash = true;
					} 
				}
			}
// end change RF063
			Sound(true, thePosition, false);
		}
		else
			Sound(true, thePosition, true);
	}
}


void CWeapon::Add_Projectile(geVec3d Pos, geVec3d Front, geVec3d Orient, char *Projectile, char *PAttribute, char *PAltAttribute)
{
	Proj *d;
	int Type = -1;	

	for(int i=0;i<MAX_PROJD;i++)
	{
		if(ProjD[i].active)
		{
			if(!stricmp(ProjD[i].Name, Projectile))
			{
				Type = i;
				break;
			}
		}
	}
	
	if(Type ==-1)
		return;

	if(ProjD[Type].active)
	{
		geXForm3d XForm;
		d = new Proj;
		
		d->Rotation = ProjD[Type].Rotation;
		d->Angles = Orient;
		d->Pos = Pos;
		d->Gravity = ProjD[Type].Gravity;
		d->LifeTime = ProjD[Type].LifeTime;
		d->Bounce = ProjD[Type].Bounce;
		d->BounceSoundDef = NULL;
		if(!EffectC_IsStringNull(ProjD[Type].BounceSound))
			d->BounceSoundDef = SPool_Sound(ProjD[Type].BounceSound);
		d->MoveSoundDef = NULL;
		if(!EffectC_IsStringNull(ProjD[Type].MoveSound))
			d->MoveSoundDef = SPool_Sound(ProjD[Type].MoveSound);
		d->ImpactSoundDef = NULL;
		if(!EffectC_IsStringNull(ProjD[Type].ImpactSound))
			d->ImpactSoundDef = SPool_Sound(ProjD[Type].ImpactSound);
		d->MoveSoundEffect = PlaySound(d->MoveSoundDef, d->Pos, true);
		d->Decal = ProjD[Type].Decal;
		d->Explosion = ProjD[Type].Explosion;
		d->ActorExplosion = ProjD[Type].ActorExplosion;
// changed RF064
		d->ShowBoth = ProjD[Type].ShowBoth;
		d->AttachActor = ProjD[Type].AttachActor;
		d->BoneLevel = ProjD[Type].BoneLevel;
// end change RF064
		d->ShakeAmt = ProjD[Type].ShakeAmt;
		d->ShakeDecay = ProjD[Type].ShakeDecay;
		d->Damage = ProjD[Type].Damage;
// changed RF063
		d->AltDamage = ProjD[Type].AltDamage;
		d->AltAttribute = PAltAttribute;
// end change RF063
		d->RadiusDamage = ProjD[Type].RadiusDamage;
		d->Radius = ProjD[Type].Radius;
		d->Attribute = PAttribute;
		d->Name = ProjD[Type].Name;

		geXForm3d_SetIdentity(&XForm);
		geXForm3d_RotateZ(&XForm, Orient.Z);
		geXForm3d_RotateX(&XForm, Orient.X);
		geXForm3d_RotateY(&XForm, Orient.Y);
		geVec3d_Set(&XForm.Translation, Pos.X,Pos.Y,Pos.Z);
		geXForm3d_GetIn(&XForm, &d->Direction);
		geVec3d_Scale( &d->Direction, ProjD[Type].Speed, &d->Direction);
		geXForm3d_SetIdentity( &(XForm) );
		geXForm3d_RotateX( &(XForm), 0.0f);  
		geXForm3d_RotateY( &(XForm), (- 90.0f ) / 57.3f );  
		geXForm3d_RotateZ( &(XForm), 0.0f); 
		geXForm3d_GetIn( &(XForm), &d->In );
		geVec3d_Inverse(&d->In);
		
		d->Actor = CCD->ActorManager()->SpawnActor(ProjD[Type].Actorfile, 
			d->Pos, d->Rotation, ProjD[Type].ActorAnimation, ProjD[Type].ActorAnimation, NULL);
		CCD->ActorManager()->Rotate(d->Actor, d->Angles);
		CCD->ActorManager()->SetType(d->Actor, ENTITY_PROJECTILE);
		CCD->ActorManager()->SetScale(d->Actor, ProjD[Type].Scale);
		CCD->ActorManager()->SetBoxChange(d->Actor, false);
		CCD->ActorManager()->SetNoCollide(d->Actor);
		CCD->ActorManager()->SetBBox(d->Actor, ProjD[Type].BoxSize, ProjD[Type].BoxSize, ProjD[Type].BoxSize);
		CCD->ActorManager()->GetBoundingBox(d->Actor, &d->ExtBox);
		CCD->ActorManager()->SetStepHeight(d->Actor, -1.0f);
// changed RF064
		CCD->ActorManager()->SetHideRadar(d->Actor, true);
// end change RF064
		if(d->Gravity)
			CCD->ActorManager()->SetGravity(d->Actor, CCD->Player()->GetGravity());
		
		GE_Collision	Collision;
		CCD->Collision()->IgnoreContents(false);
		CCD->Collision()->CheckLevel(RGF_COLLISIONLEVEL_1);
		char BoneHit[64];
		BoneHit[0] = '\0';
		if(CCD->Collision()->CheckForBoneCollision(&d->ExtBox.Min, &d->ExtBox.Max,
				Front, d->Pos, &Collision, d->Actor, BoneHit, d->BoneLevel))
		{
			int nHitType = CCD->Collision()->ProcessCollision(Collision, d->Actor, false);

			if(nHitType != kNoCollision)
			{
				if(d->MoveSoundEffect!=-1)
					CCD->EffectManager()->Item_Delete(EFF_SND, d->MoveSoundEffect);
				PlaySound(d->ImpactSoundDef, d->Pos, false);
				
				//
				// inflict damage
				//
// changed RF063				
				if(Collision.Actor)
				{
					CCD->Damage()->DamageActor(Collision.Actor, d->Damage, d->Attribute, d->AltDamage, d->AltAttribute, d->Name);
				}

				if(Collision.Model)
					CCD->Damage()->DamageModel(Collision.Model, d->Damage, d->Attribute, d->AltDamage, d->AltAttribute);
// end change RF063				
				//
				// create explosion here
				//
// changed RF063				
				CCD->Explosions()->AddExplosion(d->Explosion, d->Pos, NULL, NULL);
				if(d->ShakeAmt>0.0f)
					CCD->CameraManager()->SetShake(d->ShakeAmt, d->ShakeDecay, d->Pos);
				if(Collision.Actor)
// changed RF063
					CCD->Explosions()->AddExplosion(d->ActorExplosion, d->Pos, NULL, NULL);
				
				//
				// Handle explosion damage here
				//
				if(d->RadiusDamage>0.0f)
				{
// changed RF063
					CCD->Damage()->DamageActorInRange(d->Pos, d->Radius, d->RadiusDamage, d->Attribute, d->RadiusDamage, d->AltAttribute, "Explosion");
					CCD->Damage()->DamageModelInRange(d->Pos, d->Radius, d->RadiusDamage, d->Attribute, d->RadiusDamage, d->AltAttribute);
// end change RF063
				}
				CCD->ActorManager()->RemoveActor(d->Actor);
				geActor_Destroy(&d->Actor);
				delete d;
				return;
			}
		}
		
		for (int i =0;i<5;i++)
		{
			d->Effect[i] = -1;
			geXForm3d thePosition;
			if(EffectC_IsStringNull(ProjD[Type].EffectBone[i]))
				geActor_GetBoneTransform(d->Actor, NULL, &(thePosition));
			else
			{
				if(!geActor_GetBoneTransform(d->Actor, ProjD[Type].EffectBone[i], &(thePosition)))
					geActor_GetBoneTransform(d->Actor, NULL, &(thePosition));
			}
			if(ProjD[Type].Effect[i][0] != '\0')
			{
				for(int k=0;k<MAXEXPITEM;k++)
				{
					if(CCD->Effect()->EffectActive(k))
					{
						if(!stricmp(ProjD[Type].Effect[i], CCD->Effect()->EffectName(k)))
						{
							geVec3d Zero = {0.0f, 0.0f, 0.0f};
							d->Effect[i] = CCD->Effect()->AddEffect(k, thePosition.Translation, Zero);
							d->EffectType[i] = CCD->Effect()->EffectType(k);
							d->EffectBone[i] = ProjD[Type].EffectBone[i];
							break;
						}
					}
				}
			}
		}
		
		d->next = NULL;
		d->prev = Bottom;
		if(Bottom != NULL) Bottom->next = d;
		Bottom = d;
		return;
	}
}

int CWeapon::PlaySound(geSound_Def *SoundDef, geVec3d Pos, bool Loop)
{
	Snd Sound;
	
	if(!SoundDef)
		return -1;
	
	memset( &Sound, 0, sizeof( Sound ) );
    geVec3d_Copy( &(Pos), &( Sound.Pos ) );
    Sound.Min=CCD->GetAudibleRadius();
    Sound.Loop=Loop;
	Sound.SoundDef=SoundDef;
    return CCD->EffectManager()->Item_Add(EFF_SND, (void *)&Sound);
}

int CWeapon::ZoomAmount()
{
	if(!(CurrentWeapon == -1 || CurrentWeapon == 11) && ViewPoint == FIRSTPERSON)
	{
		if(WeaponD[CurrentWeapon].ZoomAmt>1)
		{
			if(WeaponD[CurrentWeapon].ZoomAmt>20)
				return 20;
			return WeaponD[CurrentWeapon].ZoomAmt;
		}
	}
	return 0;
}

void CWeapon::LoadDefaults()
{
	int i;

	for(i=0;i<MAX_PROJD;i++)
	{
		memset(&ProjD[i], 0, sizeof(DefaultProj));
		ProjD[i].active = false;
	}

	for(i=0;i<MAX_WEAPONS;i++)
	{
		memset(&WeaponD[i], 0, sizeof(DefaultWeapons));
		WeaponD[i].active = false;
		WeaponD[i].VAnimSpeed = 1.0f;
	}

	CIniFile AttrFile("weapon.ini");
	if(!AttrFile.ReadFile())
	{
		CCD->ReportError("Can't open weapons initialization file", false);
		return;
	}
	CString KeyName = AttrFile.FindFirstKey();
	CString Type, Vector;
	geVFile *ActorFile;
	char szName[64], szAlpha[64];
	int projptr = 0;
	int weapptr = 0;
	while(KeyName != "")
	{
		Type = AttrFile.GetValue(KeyName, "type");
		if(Type=="projectile")
		{
			strcpy(ProjD[projptr].Name,KeyName);
			Type = AttrFile.GetValue(KeyName, "actor");
			strcpy(ProjD[projptr].Actorfile,Type);
			Vector = AttrFile.GetValue(KeyName, "rotation");
			if(Vector!="")
			{
				strcpy(szName,Vector);
				ProjD[projptr].Rotation = Extract(szName);
				ProjD[projptr].Rotation.X *= 0.0174532925199433f;
				ProjD[projptr].Rotation.Y *= 0.0174532925199433f;
				ProjD[projptr].Rotation.Z *= 0.0174532925199433f;
			}
			geActor *Actor = CCD->ActorManager()->SpawnActor(ProjD[projptr].Actorfile, 
							ProjD[projptr].Rotation, ProjD[projptr].Rotation, "", "", NULL);
			CCD->ActorManager()->RemoveActor(Actor);
			geActor_Destroy(&Actor);
			ProjD[projptr].Scale = (float)AttrFile.GetValueF(KeyName, "scale");
			Type = AttrFile.GetValue(KeyName, "animation");
			if(Type!="")
				strcpy(ProjD[projptr].ActorAnimation,Type);
			else
				ProjD[projptr].ActorAnimation[0] = '\0';
			Vector = AttrFile.GetValue(KeyName, "bounce");
			ProjD[projptr].Bounce = false;
			if(Vector=="true")
				ProjD[projptr].Bounce = true;
			Vector = AttrFile.GetValue(KeyName, "gravity");
			ProjD[projptr].Gravity = false;
			if(Vector=="true")
				ProjD[projptr].Gravity = true;
			ProjD[projptr].Speed = (float)AttrFile.GetValueF(KeyName, "speed");
			ProjD[projptr].LifeTime = (float)AttrFile.GetValueF(KeyName, "lifetime");
			Type = AttrFile.GetValue(KeyName, "explosion");
			strcpy(ProjD[projptr].Explosion,Type);
			Type = AttrFile.GetValue(KeyName, "actorexplosion");
			strcpy(ProjD[projptr].ActorExplosion,Type);
// changed RF064
			ProjD[projptr].ShowBoth = false;
			Vector = AttrFile.GetValue(KeyName, "showboth");
			if(Vector=="true")
				ProjD[projptr].ShowBoth = true;
			ProjD[projptr].AttachActor = false;
			Vector = AttrFile.GetValue(KeyName, "attachactor");
			if(Vector=="true")
				ProjD[projptr].AttachActor = true;
			ProjD[projptr].BoneLevel = true;
			Vector = AttrFile.GetValue(KeyName, "bonelevel");
			if(Vector=="false")
				ProjD[projptr].BoneLevel = false;
// end change RF064
			ProjD[projptr].ShakeAmt = (float)AttrFile.GetValueF(KeyName, "shakeamount");
			ProjD[projptr].ShakeDecay = (float)AttrFile.GetValueF(KeyName, "shakedecay");
			ProjD[projptr].BoxSize = (float)AttrFile.GetValueF(KeyName, "boundingbox");
			ProjD[projptr].Damage = (float)AttrFile.GetValueF(KeyName, "damage");
// changed RF063
			ProjD[projptr].AltDamage = (float)AttrFile.GetValueF(KeyName, "altdamage");
// end change RF063
			ProjD[projptr].Radius = (float)AttrFile.GetValueF(KeyName, "explosionradius");
			ProjD[projptr].RadiusDamage = (float)AttrFile.GetValueF(KeyName, "explosiondamage");
			ProjD[projptr].Decal = AttrFile.GetValueI(KeyName, "decal");
			Type = AttrFile.GetValue(KeyName, "movesound");
			strcpy(ProjD[projptr].MoveSound,Type);
			if(Type!="")
				SPool_Sound(ProjD[projptr].MoveSound);
			Type = AttrFile.GetValue(KeyName, "impactsound");
			strcpy(ProjD[projptr].ImpactSound,Type);
			if(Type!="")
				SPool_Sound(ProjD[projptr].ImpactSound);
			Type = AttrFile.GetValue(KeyName, "bouncesound");
			strcpy(ProjD[projptr].BounceSound,Type);
			if(Type!="")
				SPool_Sound(ProjD[projptr].BounceSound);
			Type = AttrFile.GetValue(KeyName, "effect0");
			if(Type!="")
			{
				Vector = AttrFile.GetValue(KeyName, "effectbone0");
				strcpy(ProjD[projptr].Effect[0],Type);
				if(Vector!="")
					strcpy(ProjD[projptr].EffectBone[0],Vector);
			}
			Type = AttrFile.GetValue(KeyName, "effect1");
			if(Type!="")
			{
				Vector = AttrFile.GetValue(KeyName, "effectbone1");
				strcpy(ProjD[projptr].Effect[1],Type);
				if(Vector!="")
					strcpy(ProjD[projptr].EffectBone[1],Vector);
			}
			Type = AttrFile.GetValue(KeyName, "effect2");
			if(Type!="")
			{
				Vector = AttrFile.GetValue(KeyName, "effectbone2");
				strcpy(ProjD[projptr].Effect[2],Type);
				if(Vector!="")
					strcpy(ProjD[projptr].EffectBone[2],Vector);
			}
			Type = AttrFile.GetValue(KeyName, "effect3");
			if(Type!="")
			{
				Vector = AttrFile.GetValue(KeyName, "effectbone3");
				strcpy(ProjD[projptr].Effect[3],Type);
				if(Vector!="")
					strcpy(ProjD[projptr].EffectBone[3],Vector);
			}
			Type = AttrFile.GetValue(KeyName, "effect4");
			if(Type!="")
			{
				Vector = AttrFile.GetValue(KeyName, "effectbone4");
				strcpy(ProjD[projptr].Effect[4],Type);
				if(Vector!="")
					strcpy(ProjD[projptr].EffectBone[4],Vector);
			}

			ProjD[projptr].active = true;
			projptr+=1;
		}
		else if(Type=="weapon")
		{
			WeaponD[weapptr].VActor = NULL;
			WeaponD[weapptr].PActor = NULL;

			for(int k=0;k<ANIMMAX;k++)
				WeaponD[weapptr].Animations[k][0] = '\0';

			strcpy(WeaponD[weapptr].Name,KeyName);
			WeaponD[weapptr].Slot = AttrFile.GetValueI(KeyName, "slot");
			WeaponD[weapptr].Catagory = MELEE;
			Type = AttrFile.GetValue(KeyName, "catagory");
			if(Type=="projectile")
				WeaponD[weapptr].Catagory = PROJECTILE;
			else if(Type=="beam")
				WeaponD[weapptr].Catagory = BEAM;
			switch(WeaponD[weapptr].Catagory)
			{
			case MELEE:
				WeaponD[weapptr].MeleeDamage = (float)AttrFile.GetValueF(KeyName, "damage");
// changed RF063
				WeaponD[weapptr].MeleeAltDamage = (float)AttrFile.GetValueF(KeyName, "altdamage");
// end change RF063
				Type = AttrFile.GetValue(KeyName, "hitsound");
				strcpy(WeaponD[weapptr].HitSound,Type);
				if(Type!="")
					SPool_Sound(WeaponD[weapptr].HitSound);
				Type = AttrFile.GetValue(KeyName, "meleexplosion");
				strcpy(WeaponD[weapptr].MeleeExplosion,Type);
				break;
			case PROJECTILE:
				Vector = AttrFile.GetValue(KeyName, "projectile");
				strcpy(WeaponD[weapptr].Projectile,Vector);
				Vector = AttrFile.GetValue(KeyName, "muzzleflash");
				strcpy(WeaponD[weapptr].MuzzleFlash,Vector);
// changed RF064
				Vector = AttrFile.GetValue(KeyName, "muzzleflash3rd");
				if(Vector!="")
					strcpy(WeaponD[weapptr].MuzzleFlash3rd,Vector);
				else
					strcpy(WeaponD[weapptr].MuzzleFlash3rd, WeaponD[weapptr].MuzzleFlash);
// end change RF064
// changed RF063
				WeaponD[weapptr].WorksUnderwater = true;
				Type = AttrFile.GetValue(KeyName, "worksunderwater");
				if(Type=="false")
					WeaponD[weapptr].WorksUnderwater = false;
// end change RF063
				break;
			case BEAM:
				break;
			}
// changed RF064
			WeaponD[weapptr].FixedView = -1;
			Type = AttrFile.GetValue(KeyName, "forceviewto");
			if(Type=="firstperson")
				WeaponD[weapptr].FixedView = FIRSTPERSON;
			if(Type=="thirdperson")
				WeaponD[weapptr].FixedView = THIRDPERSON;
			if(Type=="isometric")
				WeaponD[weapptr].FixedView = ISOMETRIC;
			if(Type=="fixed")
				WeaponD[weapptr].FixedView = FIXEDCAMERA;
// end change RF064
			Type = AttrFile.GetValue(KeyName, "attacksound");
			strcpy(WeaponD[weapptr].AttackSound,Type);
			if(Type!="")
				SPool_Sound(WeaponD[weapptr].AttackSound);
			Type = AttrFile.GetValue(KeyName, "emptysound");
			strcpy(WeaponD[weapptr].EmptySound,Type);
			if(Type!="")
				SPool_Sound(WeaponD[weapptr].EmptySound);
			Type = AttrFile.GetValue(KeyName, "attribute");
			strcpy(WeaponD[weapptr].Attribute,Type);
// changed RF064
			Type = AttrFile.GetValue(KeyName, "reloadsound");
			strcpy(WeaponD[weapptr].ReloadSound,Type);
			if(Type!="")
				SPool_Sound(WeaponD[weapptr].ReloadSound);
// end change RF064
// changed RF063
			Type = AttrFile.GetValue(KeyName, "altattribute");
			strcpy(WeaponD[weapptr].AltAttribute,Type);
// end change RF063
			Type = AttrFile.GetValue(KeyName, "ammunition");
			strcpy(WeaponD[weapptr].Ammunition,Type);
			WeaponD[weapptr].AmmoPerShot = AttrFile.GetValueI(KeyName, "ammopershot");
			WeaponD[weapptr].FireRate = (float)AttrFile.GetValueF(KeyName, "firerate");
			bool activeflag = false;
			Vector = AttrFile.GetValue(KeyName, "viewactor");
			if(Vector!="")
			{
				strcpy(szName,Vector);
				CCD->OpenRFFile(&ActorFile, kActorFile, szName, GE_VFILE_OPEN_READONLY);
				if(ActorFile)
				{
					WeaponD[weapptr].VActorDef = geActor_DefCreateFromFile(ActorFile);
					if(WeaponD[weapptr].VActorDef)
					{
						WeaponD[weapptr].VActor = geActor_Create(WeaponD[weapptr].VActorDef);
						geWorld_AddActor(CCD->World(), WeaponD[weapptr].VActor, 0, 0xffffffff);	
						geVec3d Fill = {0.0f, 1.0f, 0.0f};
						geVec3d_Normalize(&Fill);
						geXForm3d	Xf;
						geXForm3d	XfT;
						geVec3d FillColor = {255.0f, 255.0f, 255.0f};
						geVec3d AmbientColor = {255.0f, 255.0f, 255.0f};
						geVec3d NewFillNormal;
						geActor_GetBoneTransform(WeaponD[weapptr].VActor, NULL, &Xf );
						geXForm3d_GetTranspose( &Xf, &XfT );
						geXForm3d_Rotate( &XfT, &Fill, &NewFillNormal );
						Vector = AttrFile.GetValue(KeyName, "viewfillcolor");
						if(Vector!="")
						{
							strcpy(szName,Vector);
							FillColor = Extract(szName);
						}
						Vector = AttrFile.GetValue(KeyName, "viewambientcolor");
						if(Vector!="")
						{
							strcpy(szName,Vector);
							AmbientColor = Extract(szName);
						}

						geActor_SetStaticLightingOptions(WeaponD[weapptr].VActor, GE_TRUE, GE_TRUE, 3);

						geActor_SetLightingOptions(WeaponD[weapptr].VActor, GE_TRUE, &NewFillNormal, FillColor.X, FillColor.Y, FillColor.Z,
						AmbientColor.X, AmbientColor.Y, AmbientColor.Z, GE_TRUE, 6, NULL, GE_FALSE);
						Vector = AttrFile.GetValue(KeyName, "viewrotation");
						if(Vector!="")
						{
							strcpy(szName,Vector);
							WeaponD[weapptr].VActorRotation = Extract(szName);
						}
						Vector = AttrFile.GetValue(KeyName, "viewoffset");
						if(Vector!="")
						{
							strcpy(szName,Vector);
							WeaponD[weapptr].VActorOffset = Extract(szName);
						}
						WeaponD[weapptr].VScale = (float)AttrFile.GetValueF(KeyName, "viewscale");
						float tspeed = (float)AttrFile.GetValueF(KeyName, "viewanimspeed");
						if(tspeed>0.0f)
							WeaponD[weapptr].VAnimSpeed = tspeed;
						Vector = AttrFile.GetValue(KeyName, "viewarmanim");
						strcpy(WeaponD[weapptr].VArm,Vector);
						Vector = AttrFile.GetValue(KeyName, "viewidleanim");
						strcpy(WeaponD[weapptr].VIdle,Vector);
						Vector = AttrFile.GetValue(KeyName, "viewattackanim");
						strcpy(WeaponD[weapptr].VAttack,Vector);
						Vector = AttrFile.GetValue(KeyName, "viewwalkanim");
						strcpy(WeaponD[weapptr].VWalk,Vector);
						geVec3d LitColor = {255.0f, 255.0f, 255.0f};
						switch(WeaponD[weapptr].Catagory)
						{
						case MELEE:
							Vector = AttrFile.GetValue(KeyName, "viewaltattackanim");
							strcpy(WeaponD[weapptr].VAltAttack,Vector);
							Vector = AttrFile.GetValue(KeyName, "viewhitanim");
							strcpy(WeaponD[weapptr].VHit,Vector);
							Vector = AttrFile.GetValue(KeyName, "viewalthitanim");
							strcpy(WeaponD[weapptr].VAltHit,Vector);
							break;
						case PROJECTILE:
							Vector = AttrFile.GetValue(KeyName, "viewlaunchoffset");
							if(Vector!="")
							{
								strcpy(szName,Vector);
								WeaponD[weapptr].VOffset = Extract(szName);
							}
// changed RF064
							WeaponD[weapptr].VReload[0] = '\0';
							Vector = AttrFile.GetValue(KeyName, "viewreloadanim");
							strcpy(WeaponD[weapptr].VReload,Vector);
							WeaponD[weapptr].ShotperMag = AttrFile.GetValueI(KeyName, "shotpermagazine");
							WeaponD[weapptr].ShotFired = 0;
							WeaponD[weapptr].MagAmt = -1;
							WeaponD[weapptr].VKeyReload[0] = '\0';
							Vector = AttrFile.GetValue(KeyName, "viewkeyreloadanim");
							strcpy(WeaponD[weapptr].VKeyReload,Vector);
							WeaponD[weapptr].VAttackEmpty[0] = '\0';
							Vector = AttrFile.GetValue(KeyName, "viewattackemptyanim");
							strcpy(WeaponD[weapptr].VAttackEmpty,Vector);
							WeaponD[weapptr].LooseMag = false;
							Vector = AttrFile.GetValue(KeyName, "loosemag");
							if(Vector=="true")
								WeaponD[weapptr].LooseMag = true;
// end change RF064
							Vector = AttrFile.GetValue(KeyName, "viewlaunchbone");
							if(Vector!="")
								strcpy(WeaponD[weapptr].VBone,Vector);
							WeaponD[weapptr].CrossHairFixed = false;
							Vector = AttrFile.GetValue(KeyName, "crosshairfixed");
							if(Vector=="true")
								WeaponD[weapptr].CrossHairFixed = true;
							Vector = AttrFile.GetValue(KeyName, "crosshair");
							if(Vector!="")
							{
								strcpy(szName,Vector);
								Vector = AttrFile.GetValue(KeyName, "crosshairalpha");
								strcpy(szAlpha,Vector);
								WeaponD[weapptr].CrossHair = CreateFromFileAndAlphaNames(szName, szAlpha);
								if(WeaponD[weapptr].CrossHairFixed)
									geEngine_AddBitmap(CCD->Engine()->Engine(), WeaponD[weapptr].CrossHair);
								else
									geWorld_AddBitmap(CCD->World(), WeaponD[weapptr].CrossHair);
							}
							Vector = AttrFile.GetValue(KeyName, "crosshairlitcolor");
							if(Vector!="")
							{
								strcpy(szName,Vector);
								LitColor = Extract(szName);
							}
							WeaponD[weapptr].LitColor = LitColor;
							WeaponD[weapptr].AllowLit = false;
							Vector = AttrFile.GetValue(KeyName, "allowlitcrosshair");
							if(Vector=="true")
								WeaponD[weapptr].AllowLit = true;
							WeaponD[weapptr].ZoomAmt = AttrFile.GetValueI(KeyName, "zoomamount");
							WeaponD[weapptr].JerkAmt = (float)AttrFile.GetValueF(KeyName, "recoilamount");
							WeaponD[weapptr].JerkDecay = (float)AttrFile.GetValueF(KeyName, "recoildecay");
							WeaponD[weapptr].BobAmt = (float)AttrFile.GetValueF(KeyName, "bobamount");
							
							break;
						case BEAM:
							break;
						}
						activeflag = true;
					}	
					geVFile_Close(ActorFile);
				}
			}
			Vector = AttrFile.GetValue(KeyName, "playeractor");
			if(Vector!="")
			{
				strcpy(szName,Vector);
				CCD->OpenRFFile(&ActorFile, kActorFile, szName, GE_VFILE_OPEN_READONLY);
				if(ActorFile)
				{
					WeaponD[weapptr].PActorDef = geActor_DefCreateFromFile(ActorFile);
					if(WeaponD[weapptr].PActorDef)
					{
						WeaponD[weapptr].PActor = geActor_Create(WeaponD[weapptr].PActorDef);
						geWorld_AddActor(CCD->World(), WeaponD[weapptr].PActor, 0, 0xffffffff);	
						geVec3d Fill = {0.0f, 1.0f, 0.0f};
						geVec3d_Normalize(&Fill);
						geXForm3d	Xf;
						geXForm3d	XfT;
						geVec3d FillColor = {255.0f, 255.0f, 255.0f};
						geVec3d AmbientColor = {255.0f, 255.0f, 255.0f};
						geVec3d NewFillNormal;
						geActor_GetBoneTransform(WeaponD[weapptr].PActor, NULL, &Xf );
						geXForm3d_GetTranspose( &Xf, &XfT );
						geXForm3d_Rotate( &XfT, &Fill, &NewFillNormal );
						Vector = AttrFile.GetValue(KeyName, "playerfillcolor");
						if(Vector!="")
						{
							strcpy(szName,Vector);
							FillColor = Extract(szName);
						}
						Vector = AttrFile.GetValue(KeyName, "playerambientcolor");
						if(Vector!="")
						{
							strcpy(szName,Vector);
							AmbientColor = Extract(szName);
						}

						geActor_SetStaticLightingOptions(WeaponD[weapptr].PActor, GE_TRUE, GE_TRUE, 3);

						geActor_SetLightingOptions(WeaponD[weapptr].PActor, GE_TRUE, &NewFillNormal, FillColor.X, FillColor.Y, FillColor.Z,
						AmbientColor.X, AmbientColor.Y, AmbientColor.Z, GE_TRUE, 6, NULL, GE_FALSE);
						Vector = AttrFile.GetValue(KeyName, "playerrotation");
						if(Vector!="")
						{
							strcpy(szName,Vector);
							WeaponD[weapptr].PActorRotation = Extract(szName);
						}
						WeaponD[weapptr].PScale = (float)AttrFile.GetValueF(KeyName, "playerscale");
						WeaponD[weapptr].PMOffset = (float)AttrFile.GetValueF(KeyName, "playermuzzleoffset");
						WeaponD[weapptr].PBone[0] = '\0';
						Vector = AttrFile.GetValue(KeyName, "playerbone");
						if(Vector!="")
							strcpy(WeaponD[weapptr].PBone,Vector);
						Vector = AttrFile.GetValue(KeyName, "playerlaunchoffset");
						if(Vector!="")
						{
							strcpy(szName,Vector);
							WeaponD[weapptr].POffset = Extract(szName);
						}

						Type = AttrFile.GetValue(KeyName, "idle");
						strcpy(WeaponD[weapptr].Animations[IDLE],Type);
						Type = AttrFile.GetValue(KeyName, "walk");
						strcpy(WeaponD[weapptr].Animations[WALK],Type);
						Type = AttrFile.GetValue(KeyName, "run");
						strcpy(WeaponD[weapptr].Animations[RUN],Type);
						Type = AttrFile.GetValue(KeyName, "jump");
						strcpy(WeaponD[weapptr].Animations[JUMP],Type);
						Type = AttrFile.GetValue(KeyName, "starttojump");
						strcpy(WeaponD[weapptr].Animations[STARTJUMP],Type);
						Type = AttrFile.GetValue(KeyName, "fall");
						strcpy(WeaponD[weapptr].Animations[FALL],Type);
						Type = AttrFile.GetValue(KeyName, "land");
						strcpy(WeaponD[weapptr].Animations[LAND],Type);
						Type = AttrFile.GetValue(KeyName, "slidetoleft");
						strcpy(WeaponD[weapptr].Animations[SLIDELEFT],Type);
						Type = AttrFile.GetValue(KeyName, "slideruntoleft");
						strcpy(WeaponD[weapptr].Animations[RUNSLIDELEFT],Type);
						Type = AttrFile.GetValue(KeyName, "slidetoright");
						strcpy(WeaponD[weapptr].Animations[SLIDERIGHT],Type);
						Type = AttrFile.GetValue(KeyName, "slideruntoright");
						strcpy(WeaponD[weapptr].Animations[RUNSLIDERIGHT],Type);
						Type = AttrFile.GetValue(KeyName, "crawl");
						strcpy(WeaponD[weapptr].Animations[CRAWL],Type);
						Type = AttrFile.GetValue(KeyName, "crouchidle");
						strcpy(WeaponD[weapptr].Animations[CIDLE],Type);
						Type = AttrFile.GetValue(KeyName, "crouchstarttojump");
						strcpy(WeaponD[weapptr].Animations[CSTARTJUMP],Type);
						Type = AttrFile.GetValue(KeyName, "crouchland");
						strcpy(WeaponD[weapptr].Animations[CLAND],Type);
						Type = AttrFile.GetValue(KeyName, "crawlslidetoleft");
						strcpy(WeaponD[weapptr].Animations[SLIDECLEFT],Type);
						Type = AttrFile.GetValue(KeyName, "crawlslidetoright");
						strcpy(WeaponD[weapptr].Animations[SLIDECRIGHT],Type);
// changed RF064

// end change RF064		
						Type = AttrFile.GetValue(KeyName, "shootup");
						strcpy(WeaponD[weapptr].Animations[SHOOT1],Type);
						Type = AttrFile.GetValue(KeyName, "shootdwn");
						strcpy(WeaponD[weapptr].Animations[SHOOT],Type);
						Type = AttrFile.GetValue(KeyName, "aimup");
						strcpy(WeaponD[weapptr].Animations[AIM1],Type);
						Type = AttrFile.GetValue(KeyName, "aimdwn");
						strcpy(WeaponD[weapptr].Animations[AIM],Type);
						Type = AttrFile.GetValue(KeyName, "walkshootup");
						strcpy(WeaponD[weapptr].Animations[WALKSHOOT1],Type);
						Type = AttrFile.GetValue(KeyName, "walkshootdwn");
						strcpy(WeaponD[weapptr].Animations[WALKSHOOT],Type);
						Type = AttrFile.GetValue(KeyName, "runshootup");
						strcpy(WeaponD[weapptr].Animations[RUNSHOOT1],Type);
						Type = AttrFile.GetValue(KeyName, "runshootdwn");
						strcpy(WeaponD[weapptr].Animations[RUNSHOOT],Type);
						Type = AttrFile.GetValue(KeyName, "slidetoleftshootup");
						strcpy(WeaponD[weapptr].Animations[SLIDELEFTSHOOT1],Type);
						Type = AttrFile.GetValue(KeyName, "slidetoleftshootdwn");
						strcpy(WeaponD[weapptr].Animations[SLIDELEFTSHOOT],Type);
						Type = AttrFile.GetValue(KeyName, "slideruntoleftshootup");
						strcpy(WeaponD[weapptr].Animations[RUNSLIDELEFTSHOOT1],Type);
						Type = AttrFile.GetValue(KeyName, "slideruntoleftshootdwn");
						strcpy(WeaponD[weapptr].Animations[RUNSLIDELEFTSHOOT],Type);
						Type = AttrFile.GetValue(KeyName, "slidetorightshootup");
						strcpy(WeaponD[weapptr].Animations[SLIDERIGHTSHOOT1],Type);
						Type = AttrFile.GetValue(KeyName, "slidetorightshootdwn");
						strcpy(WeaponD[weapptr].Animations[SLIDERIGHTSHOOT],Type);
						Type = AttrFile.GetValue(KeyName, "slideruntorightshootup");
						strcpy(WeaponD[weapptr].Animations[RUNSLIDERIGHTSHOOT1],Type);
						Type = AttrFile.GetValue(KeyName, "slideruntorightshootdwn");
						strcpy(WeaponD[weapptr].Animations[RUNSLIDERIGHTSHOOT],Type);
						Type = AttrFile.GetValue(KeyName, "jumpshootup");
						strcpy(WeaponD[weapptr].Animations[JUMPSHOOT1],Type);
						Type = AttrFile.GetValue(KeyName, "jumpshootdwn");
						strcpy(WeaponD[weapptr].Animations[JUMPSHOOT],Type);
						Type = AttrFile.GetValue(KeyName, "fallshootup");
						strcpy(WeaponD[weapptr].Animations[FALLSHOOT1],Type);
						Type = AttrFile.GetValue(KeyName, "fallshootdwn");
						strcpy(WeaponD[weapptr].Animations[FALLSHOOT],Type);
						Type = AttrFile.GetValue(KeyName, "crouchaimup");
						strcpy(WeaponD[weapptr].Animations[CAIM1],Type);
						Type = AttrFile.GetValue(KeyName, "crouchaimdwn");
						strcpy(WeaponD[weapptr].Animations[CAIM],Type);
						Type = AttrFile.GetValue(KeyName, "crouchshootup");
						strcpy(WeaponD[weapptr].Animations[CSHOOT1],Type);
						Type = AttrFile.GetValue(KeyName, "crouchshootdwn");
						strcpy(WeaponD[weapptr].Animations[CSHOOT],Type);
						Type = AttrFile.GetValue(KeyName, "crawlshootup");
						strcpy(WeaponD[weapptr].Animations[CRAWLSHOOT1],Type);
						Type = AttrFile.GetValue(KeyName, "crawlshootdwn");
						strcpy(WeaponD[weapptr].Animations[CRAWLSHOOT],Type);
						Type = AttrFile.GetValue(KeyName, "crawlslidetoleftshootup");
						strcpy(WeaponD[weapptr].Animations[SLIDECLEFTSHOOT1],Type);
						Type = AttrFile.GetValue(KeyName, "crawlslidetoleftshootdwn");
						strcpy(WeaponD[weapptr].Animations[SLIDECLEFTSHOOT],Type);
						Type = AttrFile.GetValue(KeyName, "crawlslidetorightshootup");
						strcpy(WeaponD[weapptr].Animations[SLIDECRIGHTSHOOT1],Type);
						Type = AttrFile.GetValue(KeyName, "crawlslidetorightshootdwn");
						strcpy(WeaponD[weapptr].Animations[SLIDECRIGHTSHOOT],Type);
// changed RF063		
						Type = AttrFile.GetValue(KeyName, "swim");
						strcpy(WeaponD[weapptr].Animations[SWIM],Type);
						Type = AttrFile.GetValue(KeyName, "treadwater");
						strcpy(WeaponD[weapptr].Animations[TREADWATER],Type);
// changed RF064
						Type = AttrFile.GetValue(KeyName, "idletowalk");
						strcpy(WeaponD[weapptr].Animations[I2WALK],Type);
						Type = AttrFile.GetValue(KeyName, "idletorun");
						strcpy(WeaponD[weapptr].Animations[I2RUN],Type);
						Type = AttrFile.GetValue(KeyName, "walktoidle");
						strcpy(WeaponD[weapptr].Animations[W2IDLE],Type);
						Type = AttrFile.GetValue(KeyName, "crawltoidle");
						strcpy(WeaponD[weapptr].Animations[C2IDLE],Type);
						Type = AttrFile.GetValue(KeyName, "crouchtoidle");
						strcpy(WeaponD[weapptr].Animations[CROUCH2IDLE],Type);
						Type = AttrFile.GetValue(KeyName, "idletocrouch");
						strcpy(WeaponD[weapptr].Animations[IDLE2CROUCH],Type);
						Type = AttrFile.GetValue(KeyName, "swimtotread");
						strcpy(WeaponD[weapptr].Animations[SWIM2TREAD],Type);
						Type = AttrFile.GetValue(KeyName, "treadtoswim");
						strcpy(WeaponD[weapptr].Animations[TREAD2SWIM],Type);
						Type = AttrFile.GetValue(KeyName, "idletotread");
						strcpy(WeaponD[weapptr].Animations[IDLE2TREAD],Type);
						Type = AttrFile.GetValue(KeyName, "swimtowalk");
						strcpy(WeaponD[weapptr].Animations[SWIM2WALK],Type);
						Type = AttrFile.GetValue(KeyName, "walktoswim");
						strcpy(WeaponD[weapptr].Animations[WALK2SWIM],Type);
						Type = AttrFile.GetValue(KeyName, "treadtoidle");
						strcpy(WeaponD[weapptr].Animations[TREAD2IDLE],Type);
						Type = AttrFile.GetValue(KeyName, "jumptofall");
						strcpy(WeaponD[weapptr].Animations[JUMP2FALL],Type);
						Type = AttrFile.GetValue(KeyName, "jumptotread");
						strcpy(WeaponD[weapptr].Animations[JUMP2TREAD],Type);
						Type = AttrFile.GetValue(KeyName, "falltotread");
						strcpy(WeaponD[weapptr].Animations[FALL2TREAD],Type);
						Type = AttrFile.GetValue(KeyName, "falltocrawl");
						strcpy(WeaponD[weapptr].Animations[FALL2CRAWL],Type);
						Type = AttrFile.GetValue(KeyName, "falltowalk");
						strcpy(WeaponD[weapptr].Animations[FALL2WALK],Type);
						Type = AttrFile.GetValue(KeyName, "falltojump");
						strcpy(WeaponD[weapptr].Animations[FALL2JUMP],Type);
						Type = AttrFile.GetValue(KeyName, "walktojump");
						strcpy(WeaponD[weapptr].Animations[WALK2JUMP],Type);
						Type = AttrFile.GetValue(KeyName, "walktocrawl");
						strcpy(WeaponD[weapptr].Animations[WALK2CRAWL],Type);
						Type = AttrFile.GetValue(KeyName, "crawltowalk");
						strcpy(WeaponD[weapptr].Animations[CRAWL2WALK],Type);
						Type = AttrFile.GetValue(KeyName, "idletocrawl");
						strcpy(WeaponD[weapptr].Animations[IDLE2CRAWL],Type);
						Type = AttrFile.GetValue(KeyName, "aimtocrouch");
						strcpy(WeaponD[weapptr].Animations[AIM2CROUCH],Type);
						Type = AttrFile.GetValue(KeyName, "crouchtoaim");
						strcpy(WeaponD[weapptr].Animations[CROUCH2AIM],Type);
						Type = AttrFile.GetValue(KeyName, "walktotread");
						strcpy(WeaponD[weapptr].Animations[W2TREAD],Type);
						Type = AttrFile.GetValue(KeyName, "falltorun");
						strcpy(WeaponD[weapptr].Animations[FALL2RUN],Type);
						Type = AttrFile.GetValue(KeyName, "crawltorun");
						strcpy(WeaponD[weapptr].Animations[CRAWL2RUN],Type);
// end change RF064
						Type = AttrFile.GetValue(KeyName, "die");
						char strip[256], *temp;
						int i = 0;
						WeaponD[weapptr].DieAnimAmt = 0;
						if(Type!="")
						{
							strcpy(strip,Type);
							temp = strtok(strip," \n");
							while(temp)
							{
								strcpy(WeaponD[weapptr].DieAnim[i],temp);
								i+=1;
								WeaponD[weapptr].DieAnimAmt = i;
								if(i==5)
									break;
								temp = strtok(NULL," \n");
							}
						}
						Type = AttrFile.GetValue(KeyName, "injury");
						i = 0;
						WeaponD[weapptr].InjuryAnimAmt = 0;
						if(Type!="")
						{
							strcpy(strip,Type);
							temp = strtok(strip," \n");
							while(temp)
							{
								strcpy(WeaponD[weapptr].InjuryAnim[i],temp);
								i+=1;
								WeaponD[weapptr].InjuryAnimAmt = i;
								if(i==5)
									break;
								temp = strtok(NULL," \n");
							}
						}
// end change RF063
						activeflag = true;
					}
					geVFile_Close(ActorFile);
				}
			}

			if(activeflag)
			{
				WeaponD[weapptr].active = true;
				weapptr+=1;
			}
		}
				
		KeyName = AttrFile.FindNextKey();
	}
}

