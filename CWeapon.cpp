/*
CWeapon.cpp:		Weapon class implementation

  (c) 1999 Edward A. Averill, III
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
		}
	}
}

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
			while(CCD->Collision()->CheckForCollision(&d->ExtBox.Min, &d->ExtBox.Max,
				tempPos, tempPos1, &Collision, d->Actor))
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
				if(Collision.Actor)
					CCD->Damage()->DamageActor(Collision.Actor, d->Damage, d->Attribute);
				if(Collision.Model)
					CCD->Damage()->DamageModel(Collision.Model, d->Damage, d->Attribute);
				
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
			if(CCD->Collision()->CheckActorCollision(tempPos, tempPos1, d->Actor, &Collision))
			{
				//
				// Handle collision here
				//
				int nHitType = CCD->Collision()->ProcessCollision(Collision, d->Actor, false);
				if(nHitType != kNoCollision)
				{
					Alive = false;
					Actor = Collision.Actor;
					Model = Collision.Model;
					if(nHitType == kCollideNoMove)
					{
						geVec3d_AddScaled (&tempPos, &(d->Direction), 1000.0f, &tempPos1);
						CCD->Collision()->CheckForCollision(NULL, NULL,
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
							CCD->EffectManager()->Item_Modify(EFF_SPRAY, d->Effect[j], (void *)&Sp, SPRAY_SOURCE | SPRAY_DEST);
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
			
			if(Actor)
				CCD->Damage()->DamageActor(Actor, d->Damage, d->Attribute);
			if(Model)
				CCD->Damage()->DamageModel(Model, d->Damage, d->Attribute);
			
			//
			// create explosion here
			//
			
			CCD->Explosions()->AddExplosion(d->Explosion, d->Pos);
	
			//
			// Handle explosion damage here
			//
			if(d->RadiusDamage>0.0f)
			{
				CCD->Damage()->DamageActorInRange(d->Pos, d->Radius, d->RadiusDamage, d->Attribute);
				CCD->Damage()->DamageModelInRange(d->Pos, d->Radius, d->RadiusDamage, d->Attribute);
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
	if(CurrentWeapon==-1 || CurrentWeapon==11)
		return;
	
	if(ViewPoint==0)
	{
		geWorld_SetActorFlags(CCD->World(), WeaponD[CurrentWeapon].VActor, GE_ACTOR_RENDER_NORMAL);
		DisplayFirstPerson(CurrentWeapon);
	}
	else
	{
		geWorld_SetActorFlags(CCD->World(), WeaponD[CurrentWeapon].VActor, 0);
	}
}

void CWeapon::DisplayFirstPerson(int index)
{
	geVec3d theRotation;
	geVec3d thePosition;
	
	if(CCD->WeaponPosition()) // used to move actor around in setup mode
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
	
	geXForm3d XForm;
	geXForm3d_SetIdentity (&XForm);
	
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
	geXForm3d_Translate (&XForm, thePosition.X, thePosition.Y, thePosition.Z); 
	
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
	
	geFloat deltaTime, tStart, tEnd, bStart, bEnd;
	geMotion *ActorMotion;
	
	deltaTime = 0.001f * (geFloat)(CCD->FreeRunningCounter() - VAnimTime);
	VMCounter += (deltaTime*WeaponD[index].VAnimSpeed);
	
	if(VSequence == VWEPCHANGE)
		ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VArm);
	if(VSequence == VWEPIDLE)
		ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VIdle);
	if(VSequence == VWEPATTACK)
		ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VAttack);
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
		if(VMCounter > tEnd && VSequence == VWEPCHANGE) // switch from change to idle
		{
			VSequence = VWEPIDLE;
			ActorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VIdle);
		}
		if(VMCounter > tEnd && (VSequence == VWEPATTACK || VSequence == VWEPALTATTACK || VSequence == VWEPHIT || VSequence == VWEPALTHIT)) // switch from shoot to idle
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
			VBlend = GE_FALSE;
		}
		if(VMCounter > tEnd)
			VMCounter = 0.0f; // Wrap animation
		
		
		// blend from walk to idle animation
		if(VSequence == VWEPWALK && VBlend == GE_FALSE && (CCD->Player()->GetMoving() == MOVEIDLE) && VMCounter != tEnd)
		{
			VBactorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VIdle);
			geMotion_GetTimeExtents(VBactorMotion, &bStart, &bEnd);
			VBScale = bEnd/tEnd;
			VBDiff = (tEnd-VMCounter);
			VBOrigin = VMCounter;
			VBlend = GE_TRUE;
		}
		if(VSequence == VWEPWALK && VMCounter == 0.0f && (CCD->Player()->GetMoving() == MOVEIDLE))
		{
			VSequence = VWEPIDLE;
			VBlend = GE_FALSE; // all done blending
		}
		
		// blend from idle to walk animation
		if(VSequence == VWEPIDLE && VBlend == GE_FALSE && (CCD->Player()->GetMoving() != MOVEIDLE) && VMCounter != tEnd)
		{
			VBactorMotion = geActor_GetMotionByName(WeaponD[index].VActorDef, WeaponD[index].VWalk);
			geMotion_GetTimeExtents(VBactorMotion, &bStart, &bEnd);
			VBScale = bEnd/tEnd;
			VBDiff = (tEnd-VMCounter);
			VBOrigin = VMCounter;
			VBlend = GE_TRUE;
		}
		if(VSequence == VWEPIDLE && VMCounter == 0.0f && (CCD->Player()->GetMoving() != MOVEIDLE))
		{
			VSequence = VWEPWALK;
			VBlend = GE_FALSE; // all done blending
		}
		
		// set actor animation
		
		geActor_SetPose(WeaponD[index].VActor, ActorMotion, VMCounter, &XForm);
		// blend with other motion
		if(VBlend == GE_TRUE)
		{
			geFloat Amount = ((VMCounter-VBOrigin)/VBDiff);
			if(Amount < 0.0f)
				Amount = 0.0f;			// eaa3 05/28/2000 prevent negative blending amounts
			if(Amount > 1.0f)
				Amount = 1.0f;			// eaa3 05/28/2000 prevent more than 100% blending amount
			geActor_BlendPose(WeaponD[index].VActor, VBactorMotion, VMCounter*VBScale, &XForm, Amount);
			if(VMCounter == 0.0f)
				VBlend = GE_FALSE;
		}
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
			CCD->Explosions()->AddExplosion(WeaponD[CurrentWeapon].MuzzleFlash, Muzzle);
		}
		MFlash = false;
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
			geVec3d theRotation;
			geVec3d thePosition;
			CCD->CameraManager()->GetRotation(&theRotation);
			CCD->CameraManager()->GetPosition(&thePosition);
			MeleeAttack(CCD->Player()->GetActor(), theRotation, thePosition, true);
		}
		else if(WeaponD[CurrentWeapon].Catagory==BEAM)
		{
		}
	}
}

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
		if(WeaponD[CurrentWeapon].Catagory==PROJECTILE)
			CCD->HUD()->ActivateElement(WeaponD[CurrentWeapon].Ammunition, false);
	}
	CurrentWeapon = Slot[value]; // change to new weapon
	if(WeaponD[CurrentWeapon].Catagory==PROJECTILE)
		CCD->HUD()->ActivateElement(WeaponD[CurrentWeapon].Ammunition, true);
	geWorld_SetActorFlags(CCD->World(), WeaponD[CurrentWeapon].VActor, GE_ACTOR_RENDER_NORMAL);
	VSequence = VWEPCHANGE;
	VAnimTime = (float)CCD->FreeRunningCounter();
	AttackTime = 0;
	VMCounter = 0.0f;
	VBlend = GE_FALSE;
	AttackFlag = false;
}

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
	CurrentWeapon = value; // change to new weapon
	if(value==-1 || value==11)
		return;
	if(WeaponD[CurrentWeapon].Catagory==PROJECTILE)
		CCD->HUD()->ActivateElement(WeaponD[CurrentWeapon].Ammunition, true);
	geWorld_SetActorFlags(CCD->World(), WeaponD[CurrentWeapon].VActor, GE_ACTOR_RENDER_NORMAL);
	VSequence = VWEPCHANGE;
	VAnimTime = (float)CCD->FreeRunningCounter();
	AttackTime = 0;
	VMCounter = 0.0f;
	VBlend = GE_FALSE;
	AttackFlag = false;
}

bool CWeapon::CrossHair()
{
	if(CurrentWeapon==-1 || CurrentWeapon == 11)
		return false;
	if(WeaponD[CurrentWeapon].Catagory!=PROJECTILE || VSequence == VWEPCHANGE)
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

	if(!WeaponD[CurrentWeapon].CrossHair)
		theBmp = CCD->MenuManager()->GetFCrossHair();
	else
		theBmp = WeaponD[CurrentWeapon].CrossHair;
	if(geBitmap_GetInfo(theBmp, &BmpInfo, NULL) == GE_TRUE)
	{
		x = (CCD->Engine()->Width() - BmpInfo.Width) / 2;
		y = (CCD->Engine()->Height() - BmpInfo.Height) / 2;
		geEngine_DrawBitmap(CCD->Engine()->Engine(), theBmp, NULL, x, y );
	}
}

geBitmap *CWeapon::GetCrossHair()
{
	geBitmap *theBmp;

	if(!WeaponD[CurrentWeapon].CrossHair)
		theBmp = CCD->MenuManager()->GetCrossHair();
	else
		theBmp = WeaponD[CurrentWeapon].CrossHair;
	return theBmp;
}

void CWeapon::WeaponData()
{ 
	char szData[256];
	
	if(!(CurrentWeapon == -1 || CurrentWeapon == 11) && ViewPoint == 0)
	{
		if(CCD->WeaponPosition())
		{
			sprintf(szData,"Rotation : X= %.2f, Y= %.2f, Z= %.2f",
				WeaponD[CurrentWeapon].VActorRotation.X + WeaponD[CurrentWeapon].K, WeaponD[CurrentWeapon].VActorRotation.Y + WeaponD[CurrentWeapon].Z, WeaponD[CurrentWeapon].VActorRotation.Z + WeaponD[CurrentWeapon].L);
			CCD->MenuManager()->FontRect(szData, FONT8, 5, CCD->Engine()->Height()- 40);
			
			sprintf(szData,"Offset : X= %.2f, Y= %.2f, Z= %.2f, Scale : %.2f",
				WeaponD[CurrentWeapon].VActorOffset.X + WeaponD[CurrentWeapon].F, WeaponD[CurrentWeapon].VActorOffset.Y + WeaponD[CurrentWeapon].H, WeaponD[CurrentWeapon].VActorOffset.Z + WeaponD[CurrentWeapon].J,
				WeaponD[CurrentWeapon].VScale+WeaponD[CurrentWeapon].G);
			CCD->MenuManager()->FontRect(szData, FONT8, 5, CCD->Engine()->Height()- 30);
		}
	}
	return;
}


void CWeapon::Attack(bool Alternate)
{
	if(CurrentWeapon == -1 || CurrentWeapon == 11)
		return;
	
	AltAttack = Alternate;
	
	if(ViewPoint==0)
	{
		if(VSequence == VWEPCHANGE || (WeaponD[CurrentWeapon].Catagory==MELEE && (VSequence == VWEPATTACK || VSequence == VWEPALTATTACK || VSequence == VWEPHIT || VSequence == VWEPALTHIT)))
			return; // shooting or changing weapon
		if(AttackTime != 0)
		{
			int dtime = CCD->FreeRunningCounter()-AttackTime;
			if(dtime<(WeaponD[CurrentWeapon].FireRate*1000.0f))
				return; // too soon
		}
		AttackTime = CCD->FreeRunningCounter();
		VSequence = VWEPATTACK; // shooting animation
		if(WeaponD[CurrentWeapon].Catagory==MELEE)
		{
			if(((geFloat)((rand() % 1000) + 1) / 1000.0f * 10.0f)>=5.0f)
				VSequence = VWEPALTATTACK;
		}
		VMCounter = 0.0f;
		AttackFlag = true;
	}
}

void CWeapon::Sound(bool Attack, geVec3d Origin, bool Empty)
{
	Snd Sound;
	
	memset( &Sound, 0, sizeof( Sound ) );
    geVec3d_Copy( &(Origin), &( Sound.Pos ) );
    Sound.Min=kAudibleRadius;
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


void CWeapon::MeleeAttack(geActor *theActor, geVec3d theRotation, geVec3d thePosition, bool player)
{
	geExtBox theBox;
	geVec3d Forward, Up, Pos;
	GE_Collision Collision;
	geXForm3d Xf;

	CCD->ActorManager()->GetBoundingBox(theActor, &theBox);
	theBox.Min.Y += (theBox.Max.Y-theBox.Min.Y)/2.0f;
	
	geXForm3d_SetIdentity(&Xf);
	geXForm3d_RotateZ(&Xf, theRotation.Z);
	geXForm3d_RotateX(&Xf, theRotation.X);
	geXForm3d_RotateY(&Xf, theRotation.Y);
	geXForm3d_Translate(&Xf, thePosition.X, thePosition.Y, thePosition.Z);
	
	geXForm3d_GetUp (&Xf, &Up);
	geVec3d_AddScaled (&Xf.Translation, &Up, -(theBox.Max.Y-theBox.Min.Y)*1.7f, &Xf.Translation);
	geXForm3d_GetIn (&Xf, &Forward);
	float distance = 2.0f * (((theBox.Max.X) < (theBox.Max.Z)) ? (theBox.Max.X) : (theBox.Max.Z));
	geVec3d_AddScaled (&Xf.Translation, &Forward, distance, &Pos);
	
	Collision.Actor = NULL;
	Collision.Model = NULL;
	
	bool result = CCD->Collision()->CheckForCollision(&theBox.Min, &theBox.Max,
		Xf.Translation, Pos, &Collision, theActor);
				
	if(result)
	{
		bool Stamina = true;
		if(!EffectC_IsStringNull(WeaponD[CurrentWeapon].Ammunition))
		{
			CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(theActor);
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
				if(Collision.Actor!=NULL)
					CCD->Damage()->DamageActor(Collision.Actor, WeaponD[CurrentWeapon].MeleeDamage, WeaponD[CurrentWeapon].Attribute);
				if(Collision.Model)
					CCD->Damage()->DamageModel(Collision.Model, WeaponD[CurrentWeapon].MeleeDamage, WeaponD[CurrentWeapon].Attribute);
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
	
	CCD->CameraManager()->GetRotation(&theRotation);
	CCD->CameraManager()->GetPosition(&thePosition);
	
	geXForm3d_SetIdentity(&Xf);
	geXForm3d_RotateZ(&Xf, theRotation.Z);
	geXForm3d_RotateX(&Xf, theRotation.X);
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
		
		geVec3d_AddScaled (&Front, &Direction, WeaponD[CurrentWeapon].VOffset.Z, &Pos);
		CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(theActor);
		if(theInv->Value(WeaponD[CurrentWeapon].Ammunition)>=WeaponD[CurrentWeapon].AmmoPerShot)
		{
			theInv->Modify(WeaponD[CurrentWeapon].Ammunition, -WeaponD[CurrentWeapon].AmmoPerShot);
			Add_Projectile(Pos, Front, Orient, WeaponD[CurrentWeapon].Projectile, WeaponD[CurrentWeapon].Attribute);
			if(WeaponD[CurrentWeapon].MuzzleFlash[0] != '\0')
			{
				if(WeaponD[CurrentWeapon].VBone[0] != '\0')
					MFlash = true;
				else
					CCD->Explosions()->AddExplosion(WeaponD[CurrentWeapon].MuzzleFlash, Front);
			}
			Sound(true, thePosition, false);
		}
		else
			Sound(true, thePosition, true);
	}
}


void CWeapon::Add_Projectile(geVec3d Pos, geVec3d Front, geVec3d Orient, char *Projectile, char *PAttribute)
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
		d->Damage = ProjD[Type].Damage;
		d->RadiusDamage = ProjD[Type].RadiusDamage;
		d->Radius = ProjD[Type].Radius;
		d->Attribute = PAttribute;
		
		geVec3d_Set(&XForm.Translation, Pos.X,Pos.Y,Pos.Z);
		geXForm3d_SetIdentity(&XForm);
		geXForm3d_RotateZ(&XForm, Orient.Z);
		geXForm3d_RotateX(&XForm, Orient.X);
		geXForm3d_RotateY(&XForm, Orient.Y);
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
		CCD->ActorManager()->SetBBox(d->Actor, ProjD[Type].BoxSize);
		CCD->ActorManager()->GetBoundingBox(d->Actor, &d->ExtBox);
		CCD->ActorManager()->SetStepHeight(d->Actor, -1.0f);
		
		if(d->Gravity)
			CCD->ActorManager()->SetGravity(d->Actor, CCD->Player()->GetGravity());
		
		GE_Collision	Collision;
		CCD->Collision()->IgnoreContents(false);
		CCD->Collision()->CheckLevel(RGF_COLLISIONLEVEL_1);
		if(CCD->Collision()->CheckActorCollisionD(Front, d->Pos, d->Actor, &Collision))
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
				
				if(Collision.Actor)
					CCD->Damage()->DamageActor(Collision.Actor, d->Damage, d->Attribute);
				if(Collision.Model)
					CCD->Damage()->DamageModel(Collision.Model, d->Damage, d->Attribute);
				
				//
				// create explosion here
				//
				
				CCD->Explosions()->AddExplosion(d->Explosion, d->Pos);
				
				//
				// Handle explosion damage here
				//
				if(d->RadiusDamage>0.0f)
				{
					CCD->Damage()->DamageActorInRange(d->Pos, d->Radius, d->RadiusDamage, d->Attribute);
					CCD->Damage()->DamageModelInRange(d->Pos, d->Radius, d->RadiusDamage, d->Attribute);
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
    Sound.Min=kAudibleRadius;
    Sound.Loop=Loop;
	Sound.SoundDef=SoundDef;
    return CCD->EffectManager()->Item_Add(EFF_SND, (void *)&Sound);
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
			ProjD[projptr].BoxSize = (float)AttrFile.GetValueF(KeyName, "boundingbox");
			ProjD[projptr].Damage = (float)AttrFile.GetValueF(KeyName, "damage");
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
			strcpy(WeaponD[weapptr].Name,KeyName);
			WeaponD[weapptr].Slot = AttrFile.GetValueI(KeyName, "slot");
			WeaponD[weapptr].Catagory = MELEE;
			Type = AttrFile.GetValue(KeyName, "catagory");
			if(Type=="projectile")
				WeaponD[weapptr].Catagory = PROJECTILE;
			else if(Type=="beam")
				WeaponD[weapptr].Catagory = BEAM;
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
			Type = AttrFile.GetValue(KeyName, "ammunition");
			strcpy(WeaponD[weapptr].Ammunition,Type);
			WeaponD[weapptr].AmmoPerShot = AttrFile.GetValueI(KeyName, "ammopershot");
			WeaponD[weapptr].FireRate = (float)AttrFile.GetValueF(KeyName, "firerate");
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
						switch(WeaponD[weapptr].Catagory)
						{
						case MELEE:
							WeaponD[weapptr].MeleeDamage = (float)AttrFile.GetValueF(KeyName, "damage");
							Type = AttrFile.GetValue(KeyName, "hitsound");
							strcpy(WeaponD[weapptr].HitSound,Type);
							if(Type!="")
								SPool_Sound(WeaponD[weapptr].HitSound);
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
							Vector = AttrFile.GetValue(KeyName, "viewlaunchbone");
							if(Vector!="")
								strcpy(WeaponD[weapptr].VBone,Vector);
							Vector = AttrFile.GetValue(KeyName, "projectile");
							strcpy(WeaponD[weapptr].Projectile,Vector);
							Vector = AttrFile.GetValue(KeyName, "muzzleflash");
							strcpy(WeaponD[weapptr].MuzzleFlash,Vector);
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
							
							break;
						case BEAM:
							break;
						}

						WeaponD[weapptr].active = true;
						weapptr+=1;
					}	
					geVFile_Close(ActorFile);
				}
			}
		}
				
		KeyName = AttrFile.FindNextKey();
	}
}

