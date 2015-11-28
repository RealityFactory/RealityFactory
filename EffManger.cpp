/*

    EffManager.cpp               Effects Manager handler

	(c) 1999 Ralph Deane

	This file contains the class implementation for the Effects
Manager that handles special effects for RGF-based games.
*/

//	Include the One True Header

#include "RabidFramework.h"
#include <Ram.h>
#include <string.h>
#include <stdlib.h>

extern Particle_System *  Particle_SystemCreate(geWorld *World);
extern void 	Particle_SystemDestroy(Particle_System *ps);
extern void 	Particle_SystemFrame(Particle_System *ps, geFloat DeltaTime);
extern geBoolean Particle_SystemRemoveAnchorPoint(Particle_System *ps, geVec3d	*AnchorPoint );
extern void 	Particle_SystemAddParticle(Particle_System *ps, geBitmap *Texture, const GE_LVertex	*Vert,const geVec3d	*AnchorPoint, geFloat Time,
	const geVec3d *Velocity,float Scale, const geVec3d *Gravity, geBoolean Bounce );
// changed RF064
extern ActorParticle_System *  ActorParticle_SystemCreate();
extern void 	ActorParticle_SystemDestroy(ActorParticle_System *ps);
extern void 	ActorParticle_SystemFrame(ActorParticle_System *ps, geFloat DeltaTime);
extern void 	ActorParticle_SystemAddParticle(ActorParticle_System *ps,
							char *ActorName, geVec3d Position,
							geVec3d	BaseRotation, geVec3d RotationSpeed,
							GE_RGBA	FillColor, GE_RGBA	AmbientColor,
							float Alpha, float AlphaRate, geFloat Time, const geVec3d *Velocity,
							geFloat	Scale, bool Gravity, geBoolean Bounce, geBoolean Solid,
							bool EnvironmentMapping, bool AllMaterial, float PercentMapping, float PercentMaterial);
// end change RF064
extern void TPool_Initalize();
extern void TPool_Delete();
extern void SPool_Initalize();
extern void SPool_Delete();

extern Electric_BoltEffect * Electric_BoltEffectCreate(
	geBitmap				*Texture,	/* The texture we map onto the bolt */
	geBitmap				*Texture2,	/* The texture we map onto the bolt */
	int 					NumPolys,	/* Number of polys, must be power of 2 */
	int						Width,		/* Width in world units of the bolt */
	geFloat 				Wildness);	/* How wild the bolt is (0 to 1 inclusive) */

extern void Electric_BoltEffectSetColorInfo(
	Electric_BoltEffect *	Effect,
	GE_RGBA *				BaseColor,		/* Base color of the bolt (2 colors should be the same */
	int						DominantColor);	/* Which color is the one to leave fixed */

extern void Electric_BoltEffectDestroy(Electric_BoltEffect *Effect);

extern void Electric_BoltEffectAnimate(
	Electric_BoltEffect *	Effect,
	const geVec3d *			start,		/* Starting point of the bolt */
	const geVec3d *			end);		/* Ending point of the bolt */

extern void Electric_BoltEffectRender(
	Electric_BoltEffect *	Effect,		/* Bolt to render */
	const geXForm3d *		XForm);		/* Transform of our point of view */

//
// Effect Manager Constructor
//

EffManager::EffManager()
{
  int i;

  TPool_Initalize(); // initalize the texture manager
  SPool_Initalize(); // initalize the sound manager

  // clear out all items
  for (i=0;i<MAX_EFF_ITEMS;i++)
  {
    Item[i].Active=GE_FALSE;
    Item[i].Pause=GE_FALSE;
  }

  // create the particle system handler
  Ps=Particle_SystemCreate(CCD->World());
// change RF064
  APs=ActorParticle_SystemCreate();
// end change RF064

}

//
// Effect Manager Destructor
//

EffManager::~EffManager()
{
  int i;

  for (i=0;i<MAX_EFF_ITEMS;i++)
  {
    // do only if item is currently active
    if(Item[i].Active==GE_TRUE)
    {
      // have each effect type remove any data
      switch(Item[i].Type)
      {
        case EFF_LIGHT:
          Glow_Remove((Glow *)Item[i].Data);
          Item[i].Active=GE_FALSE;
          Item[i].Pause=GE_FALSE;
          break;
        case EFF_SPRAY:
          Spray_Remove((Spray *)Item[i].Data);
          Item[i].Active=GE_FALSE;
          Item[i].Pause=GE_FALSE;
          break;
        case EFF_SPRITE:
          Sprite_Remove((Sprite *)Item[i].Data);
          Item[i].Active=GE_FALSE;
          Item[i].Pause=GE_FALSE;
          break;
        case EFF_SND:
          Snd_Remove((Snd *)Item[i].Data);
          Item[i].Active=GE_FALSE;
          Item[i].Pause=GE_FALSE;
          break;
        case EFF_BOLT:
          Bolt_Remove((EBolt *)Item[i].Data);
          Item[i].Active=GE_FALSE;
          Item[i].Pause=GE_FALSE;
          break;
        case EFF_CORONA:
          Corona_Remove((EffCorona *)Item[i].Data);
          Item[i].Active=GE_FALSE;
          Item[i].Pause=GE_FALSE;
          break;
// changed RF064
		case EFF_ACTORSPRAY:
          ActorSpray_Remove((ActorSpray *)Item[i].Data);
          Item[i].Active=GE_FALSE;
          Item[i].Pause=GE_FALSE;
          break;
// end change RF064
        default:
          break;
      }
    }
  }

  // remove the particle system handler
  Particle_SystemDestroy(Ps);
// change RF064
  ActorParticle_SystemDestroy(APs);
// end change RF064
  TPool_Delete(); // delete the textures
  SPool_Delete(); // delete the sounds
}

//
// Effect Manager Tick  - Process all effects
//

void EffManager::Tick(float dwTicksIn)
{
  int i, j, L, SP, Sr, Sn, Bl, C, AS;
  j = L = SP = Sr = Sn = Bl = C = AS = 0;

  // get time in seconds since last pass
  float dwTicks = (float)(dwTicksIn)*0.001f;

  for (i=0;i<MAX_EFF_ITEMS;i++)
  {
    // process if active and not paused
    if(Item[i].Active==GE_TRUE && Item[i].RemoveNext==GE_TRUE)
    {
		j+=1;
      // if effect process returns false then remove it
      switch(Item[i].Type)
      {
        case EFF_LIGHT:
          {
            Glow_Remove((Glow *)Item[i].Data);
            Item[i].Active=GE_FALSE;
			Item[i].RemoveNext = GE_FALSE;
          }
		  L+=1;
          break;
        case EFF_SPRAY:
          {
            Spray_Remove((Spray *)Item[i].Data);
            Item[i].Active=GE_FALSE;
			Item[i].RemoveNext = GE_FALSE;
          }
		  SP+=1;
          break;
        case EFF_SPRITE:
          {
            Sprite_Remove((Sprite *)Item[i].Data);
            Item[i].Active=GE_FALSE;
			Item[i].RemoveNext = GE_FALSE;
          }
		  Sr+=1;
          break;
        case EFF_SND:
          {
            Snd_Remove((Snd *)Item[i].Data);
            Item[i].Active=GE_FALSE;
			Item[i].RemoveNext = GE_FALSE;
          }
		  Sn +=1;
          break;
		  case EFF_BOLT:
          {
            Bolt_Remove((EBolt *)Item[i].Data);
            Item[i].Active=GE_FALSE;
			Item[i].RemoveNext = GE_FALSE;
          }
		  Bl+=1;
          break;
		  case EFF_CORONA:
          {
            Corona_Remove((EffCorona *)Item[i].Data);
            Item[i].Active=GE_FALSE;
			Item[i].RemoveNext = GE_FALSE;
          }
		  C+=1;
          break;
// changed RF064
		case EFF_ACTORSPRAY:
          {
            ActorSpray_Remove((ActorSpray *)Item[i].Data);
            Item[i].Active=GE_FALSE;
			Item[i].RemoveNext = GE_FALSE;
          }
		  AS+=1;
          break;
// end change RF064
        default:
          break;
      }
    }
  }

  for (i=0;i<MAX_EFF_ITEMS;i++)
  {
    // process if active and not paused
    if(Item[i].Active==GE_TRUE && Item[i].Pause==GE_FALSE)
    {
		j+=1;
      // if effect process returns false then remove it
      switch(Item[i].Type)
      {
        case EFF_LIGHT:
          if(Glow_Process((Glow  *)Item[i].Data,  dwTicks)==GE_FALSE)
          {
			  Item[i].RemoveNext = GE_TRUE;
          }
		  L+=1;
          break;
        case EFF_SPRAY:
          if(Spray_Process((Spray  *)Item[i].Data,  dwTicks)==GE_FALSE)
          {
            Item[i].RemoveNext = GE_TRUE;
          }
		  SP+=1;
          break;
        case EFF_SPRITE:
          if(Sprite_Process((Sprite  *)Item[i].Data,  dwTicks)==GE_FALSE)
          {
            Item[i].RemoveNext = GE_TRUE;
          }
		  Sr+=1;
          break;
        case EFF_SND:
          if(Snd_Process((Snd  *)Item[i].Data,  dwTicks)==GE_FALSE)
          {
            Item[i].RemoveNext = GE_TRUE;
          }
		  Sn +=1;
          break;
		  case EFF_BOLT:
          if(Bolt_Process((EBolt  *)Item[i].Data,  dwTicks)==GE_FALSE)
          {
            Item[i].RemoveNext = GE_TRUE;
          }
		  Bl+=1;
          break;
		  case EFF_CORONA:
          if(Corona_Process((EffCorona  *)Item[i].Data,  dwTicks)==GE_FALSE)
          {
            Item[i].RemoveNext = GE_TRUE;
          }
		  C+=1;
          break;
// changed RF064
		case EFF_ACTORSPRAY:
          if(ActorSpray_Process((ActorSpray  *)Item[i].Data,  dwTicks)==GE_FALSE)
          {
            Item[i].RemoveNext = GE_TRUE;
          }
		  AS+=1;
          break;
// end change RF064
        default:
          break;
      }
    }
  }

  //geEngine_Printf(CCD->Engine()->Engine(), 0,450,"Eff Count %d %d %d %d %d %d %d %d",j, L, SP, Sr, Sn, Bl, C, AS);

  // process the particles
  Particle_SystemFrame(Ps, dwTicks);
// changed RF064
  ActorParticle_SystemFrame(APs, dwTicks);
// end change RF064

}

//
// Effect Manager Add new Effect item
//     - Itype is effect type
//     - Idata is the effect data
//

int EffManager::Item_Add(int Itype, void *Idata)
{
  int i;

  for (i=0;i<MAX_EFF_ITEMS;i++)
  {
    // find a empty slot to store it in
    if(Item[i].Active==GE_FALSE)
    {
      // save effect type
      Item[i].Type=Itype;
      // process and save the data, set item active
      // returns the slot index used so that the effect
      // can be modified later
      switch(Itype)
      {
        case EFF_LIGHT:
          Item[i].Data=Glow_Add(Idata);
          Item[i].Active=GE_TRUE;
		  Item[i].RemoveNext = GE_FALSE;
          return i;
          break;
        case EFF_SPRAY:
          Item[i].Data=Spray_Add(Idata);
          Item[i].Active=GE_TRUE;
		  Item[i].RemoveNext = GE_FALSE;
          return i;
          break;
        case EFF_SPRITE:
          Item[i].Data=Sprite_Add(Idata);
          Item[i].Active=GE_TRUE;
		  Item[i].RemoveNext = GE_FALSE;
          return i;
          break;
        case EFF_SND:
          Item[i].Data=Snd_Add(Idata);
          Item[i].Active=GE_TRUE;
		  Item[i].RemoveNext = GE_FALSE;
          return i;
          break;
		  case EFF_BOLT:
          Item[i].Data=Bolt_Add(Idata);
          Item[i].Active=GE_TRUE;
		  Item[i].RemoveNext = GE_FALSE;
          return i;
          break;
		  case EFF_CORONA:
          Item[i].Data=Corona_Add(Idata);
          Item[i].Active=GE_TRUE;
		  Item[i].RemoveNext = GE_FALSE;
          return i;
          break;
// changed RF064
		  case EFF_ACTORSPRAY:
          Item[i].Data=ActorSpray_Add(Idata);
          Item[i].Active=GE_TRUE;
		  Item[i].RemoveNext = GE_FALSE;
          return i;
          break;
// end change RF064
        default:
          break;
      }
    }
  }
  // index of -1 is failure
  return -1;
}

//
// Effect Manager Modify effect item
//     - Itype is effect type
//     - Index is slot index
//     - Data is new effect data
//     - Flags indicate which parts of the data is used
//           to update effect data
//

void EffManager::Item_Modify(int Itype, int Index, void *Data, uint32 Flags)
{
    // make sure that item types match and that it is active
    if(Item[Index].Type==Itype && Item[Index].Active==GE_TRUE)
    {
      // modify effect data
      switch(Itype)
      {
        case EFF_LIGHT:
          Glow_Modify((Glow *)Item[Index].Data, (Glow *)Data, Flags);
          break;
        case EFF_SPRAY:
          Spray_Modify((Spray *)Item[Index].Data, (Spray *)Data, Flags);
          break;
        case EFF_SPRITE:
          Sprite_Modify((Sprite *)Item[Index].Data, (Sprite *)Data, Flags);
          break;
        case EFF_SND:
          Snd_Modify((Snd *)Item[Index].Data, (Snd *)Data, Flags);
          break;
		case EFF_BOLT:
          Bolt_Modify((EBolt *)Item[Index].Data, (EBolt *)Data, Flags);
          break;
		case EFF_CORONA:
          Corona_Modify((EffCorona *)Item[Index].Data, (EffCorona *)Data, Flags);
          break;
// changed RF064
		  case EFF_ACTORSPRAY:
          ActorSpray_Modify((ActorSpray *)Item[Index].Data, (ActorSpray *)Data, Flags);
          break;
// end change RF064
        default:
          break;
      }
    }
}

//
// Effect Manager Delete effect item
//

void EffManager::Item_Delete(int Itype, int Index)
{
    // delete only if types match and is active
    if(Item[Index].Type==Itype && Item[Index].Active==GE_TRUE)
    {
      // remove the effects data
      switch(Itype)
      {
        case EFF_LIGHT:
          Glow_Remove((Glow *)Item[Index].Data);
          Item[Index].Active=GE_FALSE;
          break;
        case EFF_SPRAY:
          Spray_Remove((Spray *)Item[Index].Data);
          Item[Index].Active=GE_FALSE;
          break;
        case EFF_SPRITE:
          Sprite_Remove((Sprite *)Item[Index].Data);
          Item[Index].Active=GE_FALSE;
		  Item[Index].Data = NULL;
          break;
        case EFF_SND:
          Snd_Remove((Snd *)Item[Index].Data);
          Item[Index].Active=GE_FALSE;
          break;
	case EFF_BOLT:
          Bolt_Remove((EBolt *)Item[Index].Data);
          Item[Index].Active=GE_FALSE;
          break;
	case EFF_CORONA:
          Corona_Remove((EffCorona *)Item[Index].Data);
          Item[Index].Active=GE_FALSE;
          break;
// changed RF064
		case EFF_ACTORSPRAY:
          ActorSpray_Remove((ActorSpray *)Item[Index].Data);
          Item[Index].Active=GE_FALSE;
          break;
// end change RF064
        default:
          break;
      }
    }
}

//
// Effect Manager Pause/Unpause an effect
//     - Flag is state of pause flag
//

void EffManager::Item_Pause(int Itype, int Index, geBoolean Flag)
{
    // change only if types match and is active
    if(Item[Index].Type==Itype && Item[Index].Active==GE_TRUE)
      Item[Index].Pause=Flag;
    // do processing for bolt if it is paused
    //if(Itype==EFF_BOLT)
      //Bolt_Pause((Bolt *)Item[Index].Data, Flag);
}

// changed RF063
bool EffManager::Item_Alive(int Index)
{
    if(Item[Index].Active==GE_TRUE && Item[Index].RemoveNext==GE_FALSE)
		return true;
	return false;
}
// end change RF063

//---------------------------------------------------------------------
//   Private Functions to handle each effect
//
//---------------------------------------------------------------------

//---------------------------------------------------------------------
// Spray
//---------------------------------------------------------------------
void *EffManager::Spray_Add(void *Data)
{
  Spray	*NewData;

  NewData = GE_RAM_ALLOCATE_STRUCT(Spray);
  if ( NewData == NULL )
	return NULL;

  	memcpy( NewData, Data, sizeof( *NewData ) );

	// setup defaults
	NewData->TimeRemaining = 0.0f;
	NewData->PolyCount = 0.0f;

	// setup particle gravity
	if ( geVec3d_Length( &( NewData->Gravity ) ) > 0.0f )
	{
		NewData->ParticleGravity = &( NewData->Gravity );
	}
	else
	{
		NewData->ParticleGravity = (geVec3d *)NULL;
	}

	// save the transform
	EffectC_XFormFromVector( &( NewData->Source ), &( NewData->Dest ), &( NewData->Xf ) );

	// setup default vertex data
	NewData->Vertex.u = 0.0f;
	NewData->Vertex.v = 0.0f;
	NewData->Vertex.r = 255.0f;
	NewData->Vertex.g = 255.0f;
	NewData->Vertex.b = 255.0f;

	// calculate leaf value
	geWorld_GetLeaf(CCD->World(), &( NewData->Source ), &( NewData->Leaf ) );

  return (void *)NewData;
}

void EffManager::Spray_Remove(Spray *Data)
{
	if (Data->AnchorPoint != NULL )
  	  Particle_SystemRemoveAnchorPoint( Ps, Data->AnchorPoint );

	// free effect data
	geRam_Free( Data );

}

geBoolean EffManager::Spray_Process(Spray  *Data,  float  TimeDelta)
{

	// locals
	geVec3d			Velocity;
	geVec3d			Left, Up, In;
	geVec3d			Source, Dest;
	const geXForm3d		*CameraXf;
	float			Scale;
	float			UnitLife;
	float			Distance;
	float			Adjustment = 1.0f;
	float			NewPolyCount = 0.0f;

	// adjust spray life, killing it if required
	if ( Data->SprayLife > 0.0f )
	{
		Data->SprayLife -= TimeDelta;
		if ( Data->SprayLife <= 0.0f )
		{
			return GE_FALSE;
		}
	}

	// do nothing if its paused
	if ( Data->Paused == GE_TRUE )
	{
		return GE_TRUE;
	}

	// do nothing if it isn't visible
	if(Data->ShowAlways==GE_FALSE)
	{
  	  if ( EffectC_IsPointVisible(CCD->World(),
					CCD->CameraManager()->Camera(),
					&( Data->Source ),
					Data->Leaf,
					EFFECTC_CLIP_LEAF | EFFECTC_CLIP_SEMICIRCLE ) == GE_FALSE )
 		return GE_TRUE;
	}

	// get camera xform
	CameraXf = geCamera_GetWorldSpaceXForm(CCD->CameraManager()->Camera() );

	// perform level of detail processing if required
	if ( Data->DistanceMax > 0.0f )
	{

		// do nothing if its too far away
		Distance = geVec3d_DistanceBetween( &( Data->Source ), &( CameraXf->Translation ) );
		if ( Distance > Data->DistanceMax )
		{
			return GE_TRUE;
		}

		// determine polygon adjustment amount
		if ( ( Data->DistanceMin > 0.0f ) && ( Distance > Data->DistanceMin ) )
		{
			Adjustment = ( 1.0f - ( ( Distance - Data->DistanceMin ) / ( Data->DistanceMax - Data->DistanceMin ) ) );
		}
	}

	// determine how many polys need to be added taking level fo detail into account
	Data->TimeRemaining += TimeDelta;
	while ( Data->TimeRemaining >= Data->Rate )
	{
		Data->TimeRemaining -= Data->Rate;
		NewPolyCount += 1.0f;
	}
	NewPolyCount *= Adjustment;
	Data->PolyCount += NewPolyCount;

	// add new textures
	while ( Data->PolyCount > 0 )
	{

		// adjust poly remaining count
		Data->PolyCount -= 1.0f;

		// pick a source point
		if ( Data->SourceVariance > 0 )
		{
			geXForm3d_GetLeft( &( Data->Xf ), &Left );
			geXForm3d_GetUp( &( Data->Xf ), &Up );
			geXForm3d_GetIn( &( Data->Xf ), &In );
			geVec3d_Scale( &Left, (float)Data->SourceVariance * EffectC_Frand( -1.0f, 1.0f ), &Left );
			geVec3d_Scale( &Up, (float)Data->SourceVariance * EffectC_Frand( -1.0f, 1.0f ), &Up );
			geVec3d_Scale( &In, (float)Data->SourceVariance * EffectC_Frand( -1.0f, 1.0f ), &In );
			geVec3d_Add( &Left, &In, &Left );
			geVec3d_Add( &Left, &Up, &Source );
			geVec3d_Add( &( Data->Source ), &Source, &Source );
		}
		else
		{
			geVec3d_Copy( &( Data->Source ), &Source );
		}
		Data->Vertex.X = Source.X;
		Data->Vertex.Y = Source.Y;
		Data->Vertex.Z = Source.Z;

		// pick a destination point
		if ( Data->DestVariance > 0 )
		{
			geXForm3d_GetLeft( &( Data->Xf ), &Left );
			geXForm3d_GetUp( &( Data->Xf ), &Up );
			geXForm3d_GetIn( &( Data->Xf ), &In );
			geVec3d_Scale( &Left, (float)Data->DestVariance * EffectC_Frand( -1.0f, 1.0f ), &Left );
			geVec3d_Scale( &Up, (float)Data->DestVariance * EffectC_Frand( -1.0f, 1.0f ), &Up );
			geVec3d_Scale( &In, (float)Data->DestVariance * EffectC_Frand( -1.0f, 1.0f ), &In );
			geVec3d_Add( &Left, &In, &Left );
			geVec3d_Add( &Left, &Up, &Dest );
			geVec3d_Add( &( Data->Dest ), &Dest, &Dest );
		}
		else
		{
			geVec3d_Copy( &( Data->Dest ), &Dest );
		}

		// set velocity
		if ( Data->MinSpeed > 0.0f )
		{
			geVec3d_Subtract( &Dest, &Source, &Velocity );
			geVec3d_Normalize( &Velocity );
			geVec3d_Scale( &Velocity, EffectC_Frand( Data->MinSpeed, Data->MaxSpeed ), &Velocity );
		}
		else
		{
			geVec3d_Set( &Velocity, 0.0f, 0.0f, 0.0f );
		}

		// set scale
		Scale = EffectC_Frand( Data->MinScale, Data->MaxScale );

		// set life
		UnitLife = EffectC_Frand( Data->MinUnitLife, Data->MaxUnitLife );

		// setup color
		Data->Vertex.r = EffectC_Frand( Data->ColorMin.r, Data->ColorMax.r );
		Data->Vertex.g = EffectC_Frand( Data->ColorMin.g, Data->ColorMax.g );
		Data->Vertex.b = EffectC_Frand( Data->ColorMin.b, Data->ColorMax.b );
		Data->Vertex.a = EffectC_Frand( Data->ColorMin.a, Data->ColorMax.a );

		// add the new particle
		Particle_SystemAddParticle(Ps,
						Data->Texture,
						&( Data->Vertex ),
						Data->AnchorPoint,
						UnitLife,
						&Velocity,
						Scale,
						Data->ParticleGravity,
						Data->Bounce);
	}

	// all done
	return GE_TRUE;

} // Spray_Process()

geBoolean EffManager::Spray_Modify(Spray *Data, Spray *NewData, uint32 Flags)
{
  geBoolean RecalculateLeaf = GE_FALSE;

  // adjust source and dest together
  if ( Flags & SPRAY_FOLLOWTAIL )
  {
	geVec3d_Copy( &( Data->Source ), &( Data->Dest ) );
	geVec3d_Copy( &( NewData->Source ), &( Data->Source ) );
	RecalculateLeaf = GE_TRUE;
  }

  // adjust source
  if ( Flags & SPRAY_SOURCE )
  {
	geVec3d_Copy( &( NewData->Source ), &( Data->Source ) );
	RecalculateLeaf = GE_TRUE;
  }

  // adjust source
  if ( Flags & SPRAY_DEST )
  {
	  geVec3d	In;
	  geXForm3d_RotateX( &(NewData->Xform), Data->Angle.X / 57.3f );  
	  geXForm3d_RotateY( &(NewData->Xform), Data->Angle.Y / 57.3f );  
	  geXForm3d_RotateZ( &(NewData->Xform), Data->Angle.Z / 57.3f ); 
	  geXForm3d_GetIn( &(NewData->Xform), &In );
	  geVec3d_Inverse( &In );
	  geVec3d_AddScaled( &( Data->Source ), &In, 50.0f, &( Data->Dest ) );
	  RecalculateLeaf = GE_TRUE;
  }

  if ( Flags & SPRAY_ACTUALDEST )
  {
	  geVec3d_Copy( &( NewData->Dest ), &( Data->Dest ) );
	  RecalculateLeaf = GE_TRUE;
  }

  // calculate leaf value
  if ( RecalculateLeaf == GE_TRUE )
  {
	EffectC_XFormFromVector( &( Data->Source ), &( Data->Dest ), &( Data->Xf ) );
	geWorld_GetLeaf(CCD->World(), &( Data->Source ), &( Data->Leaf ) );
  }

  // all done
  return GE_TRUE;
}


//---------------------------------------------------------------------
// Glow
//---------------------------------------------------------------------
void *EffManager::Glow_Add(void *Data)
{
  Glow	*NewData;

  NewData = GE_RAM_ALLOCATE_STRUCT(Glow);
  if ( NewData == NULL )
	return NULL;

  memcpy( NewData, Data, sizeof( *NewData ) );

  if ( NewData->RadiusMin < 0.0f )
    NewData->RadiusMin = 0.0f;
  if(NewData->RadiusMax <= 0.0f )
    NewData->RadiusMax = 0.1f;
  if( NewData->RadiusMax < NewData->RadiusMin )
    NewData->RadiusMax = NewData->RadiusMin;
  if (NewData->Intensity < 0.0f )
    NewData->Intensity = 0.0f;
  if (NewData->Intensity > 1.0f )
    NewData->Intensity = 1.0f;

  geWorld_GetLeaf(CCD->World(), &( NewData->Pos ), &( NewData->Leaf ) );

  return (void *)NewData;
}

void EffManager::Glow_Remove(Glow *Data)
{
  if ( Data->Light != NULL )
  {
    geWorld_RemoveLight(CCD->World(), Data->Light );
    Data->Light = (geLight *)NULL;
  }
  geRam_Free( Data );
}

geBoolean EffManager::Glow_Process(Glow  *Data,  float  TimeDelta)
{
  float	Radius;
  GE_RGBA Color;

  // adjust light life, killing it if required
	if ( Data->LightLife > 0.0f )
	{
		Data->LightLife -= TimeDelta;
		if ( Data->LightLife <= 0.0f )
		{
			return GE_FALSE;
		}
	}
  // do nothing if it isn't visible
  if ( Data->DoNotClip == GE_FALSE )
  {
    if ( EffectC_IsPointVisible(CCD->World(),
					CCD->CameraManager()->Camera(),
					&( Data->Pos ),
					Data->Leaf,
					EFFECTC_CLIP_LEAF ) == GE_FALSE )
    {
	  if ( Data->Light != NULL )
	  {
	     geWorld_RemoveLight(CCD->World(), Data->Light );
	     Data->Light = (geLight *)NULL;
	  }
	  return GE_TRUE;
    }
  }

  // recreate the light if required
  if ( Data->Light == NULL )
  {
    Data->Light = geWorld_AddLight(CCD->World() );
    if ( Data->Light == NULL )
	return GE_TRUE;

  }

	// set color
	if( Data->ColorMin.r < -255.0f )
          Data->ColorMin.r = -255.0f;
	if( Data->ColorMax.r > 255.0f )
          Data->ColorMax.r = 255.0f;
	if( Data->ColorMin.r > Data->ColorMax.r )
          Data->ColorMin.r = Data->ColorMax.r;
	Color.r = EffectC_Frand( Data->ColorMin.r, Data->ColorMax.r ) * Data->Intensity;
	if( Data->ColorMin.g < -255.0f )
          Data->ColorMin.g = -255.0f;
	if( Data->ColorMax.g > 255.0f )
          Data->ColorMax.g = 255.0f;
	if( Data->ColorMin.g > Data->ColorMax.g )
          Data->ColorMin.g = Data->ColorMax.g;
	Color.g = EffectC_Frand( Data->ColorMin.g, Data->ColorMax.g ) * Data->Intensity;
	if( Data->ColorMin.b < -255.0f )
          Data->ColorMin.b = -255.0f;
	if( Data->ColorMax.b > 255.0f )
          Data->ColorMax.b = 255.0f;
	if( Data->ColorMin.b > Data->ColorMax.b )
          Data->ColorMin.b = Data->ColorMax.b;
	Color.b = EffectC_Frand( Data->ColorMin.b, Data->ColorMax.b ) * Data->Intensity;
	if( Data->ColorMin.a < -255.0f )
          Data->ColorMin.a = -255.0f;
	if( Data->ColorMax.a > 255.0f )
          Data->ColorMax.a = 255.0f;
	if( Data->ColorMin.a > Data->ColorMax.a )
          Data->ColorMin.a = Data->ColorMax.a;
	Color.a = EffectC_Frand( Data->ColorMin.a, Data->ColorMax.a ) * Data->Intensity;

	// set radius
	if( Data->RadiusMin < 0.0f )
          Data->RadiusMin = 0.0f;
	if( Data->RadiusMax <= 0.0f )
          Data->RadiusMax = 0.1f;
	if( Data->RadiusMin > Data->RadiusMax )
          Data->RadiusMin = Data->RadiusMax;
	Radius = EffectC_Frand( Data->RadiusMin, Data->RadiusMax );

// changed QuestOfDreams
	// adjust the lights parameters
	if(Data->Spot)
		geWorld_SetSpotLightAttributes(CCD->World(),
								   Data->Light,
								   &(Data->Pos),
								   &Color,
								   Radius,
								   Data->Arc,
								   &Data->Direction,
								   Data->Style,
								   Data->CastShadows);
		
	else	
		geWorld_SetLightAttributes(CCD->World(),
						Data->Light,
						&( Data->Pos ),
						&Color,
						Radius,
						Data->CastShadows );
// end change QuestOfDreams

	// all done
	return GE_TRUE;
}

geBoolean EffManager::Glow_Modify(Glow *Data, Glow *NewData, uint32 Flags)
{
  geBoolean RecalculateLeaf = GE_FALSE;

  // adjust the source
  if ( Flags & GLOW_POS )
  {
	  // changed QuestOfDreams
	if(Data->Spot)
		geVec3d_Copy(&(NewData->Direction), &(Data->Direction));
	// end change QuestOfDreams
	geVec3d_Copy( &( NewData->Pos ), &( Data->Pos ) );
	RecalculateLeaf = GE_TRUE;
  }

  // adjust the min radius
  if ( Flags & GLOW_RADIUSMIN )
  {
  	if ( ( NewData->RadiusMin < 0.0f ) || ( NewData->RadiusMin > NewData->RadiusMax ) )
		return GE_FALSE;
	Data->RadiusMin = NewData->RadiusMin;
	if ( Data->RadiusMin > Data->RadiusMax )
		Data->RadiusMax = Data->RadiusMin;
  }

  // adjust the max radius
  if ( Flags & GLOW_RADIUSMAX )
  {
	if ( ( NewData->RadiusMax <= 0.0f ) || ( NewData->RadiusMax < NewData->RadiusMin ) )
  		return GE_FALSE;
	Data->RadiusMax = NewData->RadiusMax;
	if ( Data->RadiusMax < Data->RadiusMin )
		Data->RadiusMin = Data->RadiusMax;

  }

  // adjust intensity
  if ( Flags & GLOW_INTENSITY )
  {
	if ( ( NewData->Intensity < 0.0f ) || ( NewData->Intensity > 1.0f ) )
		return GE_FALSE;
	Data->Intensity = NewData->Intensity;
  }

  // calculate leaf value
  if ( RecalculateLeaf == GE_TRUE )
	geWorld_GetLeaf(CCD->World(), &( Data->Pos ), &( Data->Leaf ) );

  // all done
  return GE_TRUE;
}

//---------------------------------------------------------------------
// Sprite
//---------------------------------------------------------------------
void *EffManager::Sprite_Add(void *Data)
{
  Sprite	*NewData;
  int i;

  NewData = GE_RAM_ALLOCATE_STRUCT(Sprite);
  if ( NewData == NULL )
	return NULL;

  	memcpy( NewData, Data, sizeof( *NewData ) );

	NewData->Rotation = 0.0f;
	for ( i = 0; i < 4; i++ )
	{
		NewData->Vertex[i].r = NewData->Color.r;
		NewData->Vertex[i].g = NewData->Color.g;
		NewData->Vertex[i].b = NewData->Color.b;
		NewData->Vertex[i].a = NewData->Color.a;
	}
	NewData->Vertex[0].u = 0.0f;
	NewData->Vertex[0].v = 0.0f;
	NewData->Vertex[1].u = 1.0f;
	NewData->Vertex[1].v = 0.0f;
	NewData->Vertex[2].u = 1.0f;
	NewData->Vertex[2].v = 1.0f;
	NewData->Vertex[3].u = 0.0f;
	NewData->Vertex[3].v = 1.0f;
	NewData->CurrentTexture = 0;
	NewData->ElapsedTime = 0.0f;
// changed RF064
	NewData->CurrentLife = 0.0f;
// end change RF064
	NewData->Direction = 1;

	// calculate leaf value
	geWorld_GetLeaf(CCD->World(), &( NewData->Pos ), &( NewData->Leaf ) );

  return (void *)NewData;
}

void EffManager::Sprite_Remove(Sprite *Data)
{
	// free effect data
	geRam_Free( Data );

}

geBoolean EffManager::Sprite_Process(Sprite  *Data,  float  TimeDelta)
{
	int i;

	if ( Data->Pause==GE_TRUE)
		return GE_TRUE;

	if ( Data->ScaleRate != 0.0f )
	{

		// eliminate the effect if the scale has reached zero
		Data->Scale -= ( Data->ScaleRate * TimeDelta );
		if ( Data->Scale <= 0.0f )
		{
			Data->Scale = 0.0f;
			return GE_FALSE;
		}
	}
	// adjust alpha
	if ( Data->AlphaRate != 0.0f )
	{

		// eliminate the effect if the alpha has reached zero
		Data->Color.a -= ( Data->AlphaRate * TimeDelta );
		if ( Data->Color.a <= 0.0f )
		{
			Data->Color.a = 0.0f;
			return GE_FALSE;
		}
		if ( Data->Color.a > 255.0f )
		{
			Data->Color.a = 255.0f;
			return GE_FALSE;
		}

		// adjust all verts with new alpha
		for ( i = 0; i < 4; i++ )
		{
			Data->Vertex[i].a = Data->Color.a;
		}
	}
	// do nothing else if it isn't visible
	if(Data->ShowAlways==GE_FALSE)
	{
	  if ( EffectC_IsPointVisible(CCD->World(),
					CCD->CameraManager()->Camera(),
					&( Data->Pos ),
					Data->Leaf,
					EFFECTC_CLIP_LEAF | EFFECTC_CLIP_SEMICIRCLE ) == GE_FALSE )
		return GE_TRUE;
	}
// changed RF064
	if(Data->LifeTime>0.0f)
	{
		Data->CurrentLife += TimeDelta;
		if(Data->CurrentLife>Data->LifeTime)
		{
			return GE_FALSE;
		}
	}
// end change RF064
	// adjust art
	if ( Data->TotalTextures > 1 )
	{
		Data->ElapsedTime += TimeDelta;
		while ( Data->ElapsedTime > Data->TextureRate )
		{

			// adjust elapsed time counter
			Data->ElapsedTime -= Data->TextureRate;

			// pick the next nexture based on the cycle style
			if ( Data->Style == SPRITE_CYCLE_RANDOM )
			{
				Data->CurrentTexture = ( rand() % Data->TotalTextures );
			}
			else
			{

				// pick new texture number
				Data->CurrentTexture += Data->Direction;

				// adjust texture number if it has exceeded limits
				if (	( Data->CurrentTexture < 0 ) ||
						( Data->CurrentTexture >= Data->TotalTextures ) )
				{
					if ( Data->Style == SPRITE_CYCLE_REVERSE )
					{
						Data->Direction = -Data->Direction;
						Data->CurrentTexture += Data->Direction;
						Data->CurrentTexture += Data->Direction;
					}
					else if ( Data->Style == SPRITE_CYCLE_RESET )
					{
						Data->CurrentTexture = 0;
					}
					else
					{
						return GE_FALSE;
					}
				}
			}
		}
	}
	// adjust current rotation amount
	if ( Data->RotationRate != 0.0f )
	{
		Data->Rotation += ( Data->RotationRate * TimeDelta );
		if ( Data->Rotation > GE_PI )
		{
			Data->Rotation = -GE_PI;
		}
		else if ( Data->Rotation < -GE_PI )
		{
			Data->Rotation = GE_PI;
		}
	}

	// if there is no rotation, then process the sprite this way
	if ( Data->Rotation == 0.0f )
	{

		// setup vert
		Data->Vertex[0].X = Data->Pos.X;
		Data->Vertex[0].Y = Data->Pos.Y;
		Data->Vertex[0].Z = Data->Pos.Z;

		// update the art
		long nLeafID;
		geWorld_GetLeaf(CCD->World(), &Data->Pos, &nLeafID);
		// If the BSP leaf the entity is in might be visible,
		// ..go ahead and add it.
		if(geWorld_MightSeeLeaf(CCD->World(), nLeafID) == GE_TRUE)
		  geWorld_AddPolyOnce(CCD->World(), Data->Vertex, 1,
		  Data->Texture[Data->CurrentTexture], GE_TEXTURED_POINT,
		  GE_RENDER_DEPTH_SORT_BF | GE_RENDER_DO_NOT_OCCLUDE_OTHERS,
		  Data->Scale ); 
	}
	// ...otherwise process it this way
	else 
	{

		// locals
		const geXForm3d	*CameraXf;
		geXForm3d		NewCameraXf;
		geVec3d			Left, Up, In;
		geQuaternion	Quat;
		float			HalfWidth, HalfHeight;

		// determine half width and half height
		HalfWidth = geBitmap_Width( Data->Texture[Data->CurrentTexture] ) * Data->Scale * 0.5f;
		HalfHeight = geBitmap_Height( Data->Texture[Data->CurrentTexture] ) * Data->Scale * 0.5f;

		// get left and up vectors from camera transform
		CameraXf = geCamera_GetWorldSpaceXForm(CCD->CameraManager()->Camera());
		geXForm3d_GetIn( CameraXf, &In );
		geQuaternion_SetFromAxisAngle( &Quat, &In, Data->Rotation );
		geQuaternion_ToMatrix( &Quat, &NewCameraXf );
		geXForm3d_Multiply( &NewCameraXf, CameraXf, &NewCameraXf );
		geXForm3d_GetLeft( &NewCameraXf, &Left );
		geXForm3d_GetUp( &NewCameraXf, &Up );
		geVec3d_Scale( &Left, HalfWidth, &Left );
		geVec3d_Scale( &Up, HalfHeight, &Up );

		// setup verticies
		Data->Vertex[0].X = Data->Pos.X + Left.X + Up.X;
		Data->Vertex[0].Y = Data->Pos.Y + Left.Y + Up.Y;
		Data->Vertex[0].Z = Data->Pos.Z + Left.Z + Up.Z;
		Data->Vertex[1].X = Data->Pos.X - Left.X + Up.X;
		Data->Vertex[1].Y = Data->Pos.Y - Left.Y + Up.Y;
		Data->Vertex[1].Z = Data->Pos.Z - Left.Z + Up.Z;
		Data->Vertex[2].X = Data->Pos.X - Left.X - Up.X;
		Data->Vertex[2].Y = Data->Pos.Y - Left.Y - Up.Y;
		Data->Vertex[2].Z = Data->Pos.Z - Left.Z - Up.Z;
		Data->Vertex[3].X = Data->Pos.X + Left.X - Up.X;
		Data->Vertex[3].Y = Data->Pos.Y + Left.Y - Up.Y;
		Data->Vertex[3].Z = Data->Pos.Z + Left.Z - Up.Z;

		// update the art
		long nLeafID;
		geWorld_GetLeaf(CCD->World(), &Data->Pos, &nLeafID);
		// If the BSP leaf the entity is in might be visible,
		// ..go ahead and add it.
		if(geWorld_MightSeeLeaf(CCD->World(), nLeafID) == GE_TRUE)
		  geWorld_AddPolyOnce(CCD->World(), Data->Vertex, 4,
		  Data->Texture[Data->CurrentTexture],
		  GE_TEXTURED_POLY, GE_RENDER_DEPTH_SORT_BF | GE_RENDER_DO_NOT_OCCLUDE_OTHERS,
		  1.0f );
	}

	// all done
	return GE_TRUE;
}

geBoolean EffManager::Sprite_Modify(Sprite *Data, Sprite *NewData, uint32 Flags)
{
  geBoolean RecalculateLeaf = GE_FALSE;

  	// adjust pause
	if ( Flags & SPRITE_PAUSE )
	{
		Data->Pause = NewData->Pause;
	}

	// adjust location
	if ( Flags & SPRITE_POS )
	{
		geVec3d_Copy( &( NewData->Pos ), &( Data->Pos ) );
		RecalculateLeaf = GE_TRUE;
	}

	// adjust scale
	if ( Flags & SPRITE_SCALE )
	{
		if ( NewData->Scale <= 0.0f )
			return GE_FALSE;
		Data->Scale = NewData->Scale;
	}

	// adjust rotation
	if ( Flags & SPRITE_ROTATION )
	{
		Data->Rotation = NewData->Rotation;
	}

	// adjust color
	if ( Flags & SPRITE_COLOR )
	{

		// locals
		int	i;

		// fail if a bad color was provided
		if ( ( NewData->Color.a < 0.0f ) || ( NewData->Color.a > 255.0f ) )
			return GE_FALSE;
		if ( ( NewData->Color.r < 0.0f ) || ( NewData->Color.r > 255.0f ) )
			return GE_FALSE;
		if ( ( NewData->Color.g < 0.0f ) || ( NewData->Color.g > 255.0f ) )
			return GE_FALSE;
		if ( ( NewData->Color.b < 0.0f ) || ( NewData->Color.b > 255.0f ) )
			return GE_FALSE;

		// save new color value
		Data->Color.r = NewData->Color.r;
		Data->Color.g = NewData->Color.g;
		Data->Color.b = NewData->Color.b;
		Data->Color.a = NewData->Color.a;

		// apply it to all verts
		for ( i = 0; i < 4; i++ )
		{
			Data->Vertex[i].r = NewData->Color.r;
			Data->Vertex[i].g = NewData->Color.g;
			Data->Vertex[i].b = NewData->Color.b;
			Data->Vertex[i].a = NewData->Color.a;
		}
	}

	// recalculate leaf value if required
	if ( RecalculateLeaf == GE_TRUE )
	  geWorld_GetLeaf(CCD->World(), &( Data->Pos ), &( Data->Leaf ) );

	// all done
	return GE_TRUE;
}

//---------------------------------------------------------------------
// Sound
//---------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////////
//
//	Snd_Get3dSoundValues()
//
////////////////////////////////////////////////////////////////////////////////////////
static geBoolean Snd_Get3dSoundValues(Snd *Data,
	geFloat	*Volume,
	geFloat	*Pan,
	geFloat	*Frequency )
{

	// locals
	const geXForm3d	*SoundXf;
// changed RF063
	geXForm3d CamXf;
	geFloat	VolDelta, PanDelta;

	// get the camera xform
	CamXf = CCD->CameraManager()->ViewPoint();
	SoundXf = &CamXf;
// end change RF063
	// get 3d sound values
	geSound3D_GetConfig(
			CCD->World(),
			SoundXf, 
			&( Data->Pos ), 
			Data->Min, 
			0,
			Volume,
			Pan,
			Frequency );

	// return true or false depending on whether or not its worth modifying the sound
	VolDelta = Data->LastVolume - *Volume;
	if ( VolDelta < 0.0f )
	{
		VolDelta = -VolDelta;
	}
	PanDelta = Data->LastPan - *Pan;
	if ( PanDelta < 0.0f )
	{
		PanDelta = -PanDelta;
	}
	if ( ( VolDelta > 0.03f ) || ( PanDelta > 0.02f ) )
	{
		return GE_TRUE;
	}
	return GE_FALSE;

} // Snd_Get3dSoundValues()

////////////////////////////////////////////////////////////////////////////////////////
//
//	Snd_Add()
//
////////////////////////////////////////////////////////////////////////////////////////
void *EffManager::Snd_Add(void *Data)
{

	// locals
	Snd		*NewData;
// EFFECTS
//	geFloat	Volume;
//	geFloat	Pan;
//	geFloat	Frequency;

	// don't create anything if there is no sound system
	if (CCD->Engine()->AudioSystem() == NULL )
		return NULL;

	// allocate Snd data
        NewData = GE_RAM_ALLOCATE_STRUCT(Snd);
        if ( NewData == NULL )
	  return NULL;

  	memcpy( NewData, Data, sizeof( *NewData ) );

	// fail if we have any bad data
	if ( NewData->SoundDef == NULL )
		return NULL;
	if ( NewData->Min < 0.0f )
		return NULL;

	// play the sound
// EFFECTS
//	Snd_Get3dSoundValues(NewData, &Volume, &Pan, &Frequency );
//	Volume=0.0f;
	NewData->Sound = CCD->MenuManager()->PlaySoundDef(CCD->Engine()->AudioSystem(),
						NewData->SoundDef,
						0.0f, 0.0f, 0.0f,
						NewData->Loop );
	if ( NewData->Sound == NULL )
	{
	  Snd_Remove(NewData );
	  return NULL;
	}
// EFFECT
	NewData->LastVolume = 0.0f;
	NewData->LastPan = 0.0f;

	// all done
	return NewData;
} // Snd_Add()

////////////////////////////////////////////////////////////////////////////////////////
//
//	Snd_Remove()
//
////////////////////////////////////////////////////////////////////////////////////////
void EffManager::Snd_Remove(Snd *Data )
{
	// stop the sound
	if ( Data->Sound != NULL )
	{
		CCD->MenuManager()->StopSound(CCD->Engine()->AudioSystem(), Data->Sound );
	}

	// free effect data
	geRam_Free( Data );

} // Snd_Remove()

////////////////////////////////////////////////////////////////////////////////////////
//
//	Snd_Process()
//
//	Perform processing on an indivual effect. A return of GE_FALSE means that the
//	effect needs to be removed.
//
////////////////////////////////////////////////////////////////////////////////////////
geBoolean EffManager::Snd_Process(Snd *Data, float TimeDelta)
{

	// locals
	geBoolean	Result;
	geFloat		Volume;
	geFloat		Pan;
	geFloat		Frequency;

	// stop the sound if required...
	if ( Data->Sound != NULL )
	{
		// if the sound is done then zap this effect
		if (( Data->Loop == GE_FALSE ) && ( geSound_SoundIsPlaying(CCD->Engine()->AudioSystem(), Data->Sound ) == GE_FALSE ) )
			return GE_FALSE;

		// adjust the sound if required
		if ( Snd_Get3dSoundValues(Data, &Volume, &Pan, &Frequency ) == GE_TRUE )
		{
			Result = geSound_ModifySound(CCD->Engine()->AudioSystem(),
							Data->Sound,
							Volume, Pan, Frequency );
			if ( Result == GE_FALSE )
				return GE_TRUE;
			Data->LastVolume = Volume;
			Data->LastPan = Pan;
		}

		// stop the sound if its volume is out of hearing range
		if ( Data->Loop == GE_TRUE )
		{
			if ( Data->LastVolume < SND_MINAUDIBLEVOLUME )
			{
				CCD->MenuManager()->StopSound(CCD->Engine()->AudioSystem(), Data->Sound );
				Data->Sound = NULL;
				Data->LastVolume = 0;
				Data->LastPan = 0;
			}
		}
	}
	// ...or restart it
	else
	{

		// only restart looping non paused sounds
		if (	( Data->Loop == GE_TRUE ) &&
				( Data->Paused == GE_FALSE ) )
		{

			// restart it if its volume is now in hearing range
			Snd_Get3dSoundValues(Data, &( Data->LastVolume ), &( Data->LastPan ), &Frequency );
			if ( Data->LastVolume >= SND_MINAUDIBLEVOLUME )
			{
				Data->Sound = CCD->MenuManager()->PlaySoundDef(CCD->Engine()->AudioSystem(),
									Data->SoundDef,
									Data->LastVolume, Data->LastPan, Frequency,
									Data->Loop );
				if( Data->Sound == NULL )
					return GE_TRUE;

			}
			else
			{
				Data->LastVolume = 0;
				Data->LastPan = 0;
			}
		}
	}

	// all done
	return GE_TRUE;

} // Snd_Process()

////////////////////////////////////////////////////////////////////////////////////////
//
//	Snd_Modify()
//
//	Adjust the effect.
//
////////////////////////////////////////////////////////////////////////////////////////
geBoolean EffManager::Snd_Modify(Snd *Data, Snd	*NewData, uint32 Flags )
{

	// adjust the position
	if ( Flags & SND_POS )
	{

		// save new position
		geVec3d_Copy( &( NewData->Pos ), &( Data->Pos ) );

		// adjust the sound
		if ( Data->Sound != NULL )
		{

			// locals
			geBoolean	Result;
			geFloat		Volume;
			geFloat		Pan;
			geFloat		Frequency;

			// adjust the sound
			if ( Snd_Get3dSoundValues(Data, &Volume, &Pan, &Frequency ) == GE_TRUE )
			{
				Result = geSound_ModifySound(CCD->Engine()->AudioSystem(),
								Data->Sound,
								Volume, Pan, Frequency );
				if( Result == GE_FALSE )
					return GE_FALSE;

				Data->LastVolume = Volume;
				Data->LastPan = Pan;
			}
		}
	}

	// all done
	return GE_TRUE;

} // Snd_Modify()


//---------------------------------------------------------------------
// Corona
//---------------------------------------------------------------------
void *EffManager::Corona_Add(void *Data)
{
  EffCorona	*NewData;
  geVec3d	Pos;

  NewData = GE_RAM_ALLOCATE_STRUCT(EffCorona);
  if ( NewData == NULL )
	return NULL;

  memcpy( NewData, Data, sizeof( *NewData ) );

	if ( NewData->Texture == NULL )
		return NULL;
	if( NewData->FadeTime < 0.0f )
		return NULL;
	if ( NewData->MaxVisibleDistance <= 0.0f )
		return NULL;
	if( NewData->MinRadius < 0.0f )
		return NULL;
	if ( NewData->MaxRadius < NewData->MinRadius )
		return NULL;
	if ( NewData->MinRadiusDistance < 0.0f )
		return NULL;
	if ( ( NewData->MaxRadiusDistance <= 0.0f ) || ( NewData->MaxRadiusDistance < NewData->MinRadiusDistance ) )
		return NULL;

	// setup defaultdata
	NewData->Vertex.u = NewData->Vertex.v = 0.0f;
	NewData->LastVisibleRadius = 0.0f;
	NewData->Vertex.a = 255.0f;

	// calculate leaf value
	geVec3d_Set( &Pos, NewData->Vertex.X, NewData->Vertex.Y, NewData->Vertex.Z );
	geWorld_GetLeaf(CCD->World(), &Pos, &( NewData->Leaf ) );

	// all done
	return (void *)NewData;
}

void EffManager::Corona_Remove(EffCorona *Data)
{
	// free effect data
	geRam_Free( Data );

}

geBoolean EffManager::Corona_Process(EffCorona  *Data,  float  TimeDelta)
{
	// locals
	const geXForm3d	*CameraXf;
	geVec3d			Pos;
	geVec3d			Delta;
	geFloat			DistanceToCorona;
	geBoolean		Visible;

	// adjust corona life, killing it if required
	if ( Data->LifeTime > 0.0f )
	{
		Data->LifeTime -= TimeDelta;
		if ( Data->LifeTime <= 0.0f )
		{
			return GE_FALSE;
		}
	}

	// get camera xform
	CameraXf = geCamera_GetWorldSpaceXForm(CCD->CameraManager()->Camera());

	// determine distance to corona
	geVec3d_Set( &Pos, Data->Vertex.X, Data->Vertex.Y, Data->Vertex.Z );
	geVec3d_Subtract( &Pos, &( CameraXf->Translation ), &Delta);
	DistanceToCorona = geVec3d_Length( &Delta );

	// determine distance to corona
	Visible = EffectC_IsPointVisible(CCD->World(),
					CCD->CameraManager()->Camera(),
					&Pos,
					Data->Leaf,
					EFFECTC_CLIP_LEAF | EFFECTC_CLIP_LINEOFSIGHT | EFFECTC_CLIP_SEMICIRCLE );
	// process the corona if required
	if ( Data->Paused == GE_FALSE )
	{
		// set new radius
		if ( Visible )
		{
			// locals
			float	DesiredRadius;

			// determine desired radius
			if ( DistanceToCorona >= Data->MaxRadiusDistance )
			{
				DesiredRadius = Data->MaxRadius;
			}
			else if	( DistanceToCorona <= Data->MinRadiusDistance )
			{
				DesiredRadius = Data->MinRadius;
			}
			else
			{

				// locals
				geFloat	Slope;

				// determine radius
				Slope = ( Data->MaxRadius - Data->MinRadius ) / ( Data->MaxRadiusDistance - Data->MinRadiusDistance );
				DesiredRadius = Data->MinRadius + Slope * ( DistanceToCorona - Data->MinRadiusDistance );
			}

			// scale radius upwards
			if ( Data->FadeTime > 0.0f )
			{
				Data->LastVisibleRadius += ( ( TimeDelta * Data->MaxRadius ) / Data->FadeTime );
				if ( Data->LastVisibleRadius > DesiredRadius )
				{
					Data->LastVisibleRadius = DesiredRadius;
				}
			}
			else
			{
				Data->LastVisibleRadius = DesiredRadius;
			}
		}
		else if ( Data->LastVisibleRadius > 0.0f )
		{

			// scale radius down
			if ( Data->FadeTime > 0.0f )
			{
				Data->LastVisibleRadius -= ( ( TimeDelta * Data->MaxRadius ) / Data->FadeTime );
				if ( Data->LastVisibleRadius < 0.0f )
				{
					Data->LastVisibleRadius = 0.0f;
				}
			}
			else
			{
				Data->LastVisibleRadius = 0.0f;
			}
		}

		// update the art
		if ( Data->LastVisibleRadius > 0.0f )
		{
		long nLeafID;
		geWorld_GetLeaf(CCD->World(), &Pos, &nLeafID);
		// If the BSP leaf the entity is in might be visible,
		// ..go ahead and add it.
		if(geWorld_MightSeeLeaf(CCD->World(), nLeafID) == GE_TRUE)
			geWorld_AddPolyOnce(CCD->World(),
						&( Data->Vertex ),
						1,
						Data->Texture,
						GE_TEXTURED_POINT,
						GE_RENDER_DO_NOT_OCCLUDE_SELF,
						Data->LastVisibleRadius );
		}
	}

	// all done
	return GE_TRUE;
}

geBoolean EffManager::Corona_Modify(EffCorona *Data, EffCorona *NewData, uint32 Flags)
{
	// locals
	geBoolean	RecalculateLeaf = GE_FALSE;

	// adjust location
	if ( Flags & CORONA_POS )
	{
		Data->Vertex.X = NewData->Vertex.X;
		Data->Vertex.Y = NewData->Vertex.Y;
		Data->Vertex.Z = NewData->Vertex.Z;
		RecalculateLeaf = GE_TRUE;
	}

	// recalculate leaf value if required
	if ( RecalculateLeaf == GE_TRUE )
	{

		// locals
		geVec3d		Pos;

		// recalc leaf
		geVec3d_Set( &Pos, NewData->Vertex.X, NewData->Vertex.Y, NewData->Vertex.Z );
		geWorld_GetLeaf(CCD->World(), &Pos, &( Data->Leaf ) );
	}

	// all done
	return GE_TRUE;
}

// EFFECT

//---------------------------------------------------------------------
// Bolt
//---------------------------------------------------------------------
void *EffManager::Bolt_Add(void *Data)
{
  EBolt	*NewData;

  NewData = GE_RAM_ALLOCATE_STRUCT(EBolt);
  if ( NewData == NULL )
	return NULL;

	memcpy( NewData, Data, sizeof( *NewData ) );
	NewData->Bolt = Electric_BoltEffectCreate(NewData->Bitmap,
							   NULL,
							   NewData->NumPoints,
							   NewData->Width,
							   NewData->Wildness);
	Electric_BoltEffectSetColorInfo(NewData->Bolt, &NewData->Color, NewData->DominantColor);

	// all done
	return (void *)NewData;
}


void EffManager::Bolt_Remove(EBolt *Data)
{
	// free effect data
	if(Data->Bolt)
		Electric_BoltEffectDestroy(Data->Bolt);
	geRam_Free( Data );

}

geBoolean EffManager::Bolt_Process(EBolt  *Data,  float  TimeDelta)
{
	geVec3d			MidPoint;
	int32			Leaf;

	// kill it if required
	if ( Data->CompleteLife > 0.0f )
	{
		Data->CompleteLife -= TimeDelta;
		if ( Data->CompleteLife <= 0.0f )
		{
			return GE_FALSE;
		}
	}

	geVec3d_Subtract(&Data->End, &Data->Start, &MidPoint);
	geVec3d_AddScaled(&Data->Start, &MidPoint, 0.5f, &MidPoint);
	geWorld_GetLeaf(CCD->World(), &MidPoint, &Leaf);

	if (geWorld_MightSeeLeaf(CCD->World(), Leaf) == GE_TRUE)
	{
		Data->LastTime += TimeDelta;
		if	((Data->Intermittent == GE_FALSE) ||
			(Data->LastTime - Data->LastBoltTime > EffectC_Frand(Data->MaxFrequency, Data->MinFrequency)))
		{
			Electric_BoltEffectAnimate(Data->Bolt,
				&Data->Start,
				&Data->End);
			Data->LastBoltTime = Data->LastTime;
		}

		geXForm3d XForm = CCD->CameraManager()->ViewPoint();
		if(Data->LastTime - Data->LastBoltTime <= LIGHTNINGSTROKEDURATION)
		{
			Electric_BoltEffectRender(Data->Bolt, &XForm);
		}
	}
	// all done
	return GE_TRUE;
}


geBoolean EffManager::Bolt_Modify(EBolt *Data, EBolt *NewData, uint32 Flags)
{
	// adjust start
	if ( Flags & BOLT_START )
	{
		geVec3d_Copy( &( NewData->Start ), &( Data->Start ) );
	}

	// adjust end
	if ( Flags & BOLT_END )
	{
		geVec3d_Copy( &( NewData->End ), &( Data->End ) );
	}

	// adjust end using offset
	if ( Flags & BOLT_ENDOFFSET )
	{
		geVec3d Direction;
		Data->End = Data->Start;
		geXForm3d_GetIn(&NewData->Xform, &Direction);
		geVec3d_Inverse( &Direction);
		geVec3d_AddScaled (&Data->End, &Direction, Data->EndOffset.Z, &Data->End);
		geXForm3d_GetUp(&NewData->Xform, &Direction);
		geVec3d_AddScaled (&Data->End, &Direction, Data->EndOffset.Y, &Data->End);
		geXForm3d_GetLeft(&NewData->Xform, &Direction);
		geVec3d_AddScaled (&Data->End, &Direction, Data->EndOffset.X, &Data->End);
	}

	// all done
	return GE_TRUE;
}

//---------------------------------------------------------------------
// ActorSpray
//---------------------------------------------------------------------
void *EffManager::ActorSpray_Add(void *Data)
{
  ActorSpray	*NewData;

  NewData = GE_RAM_ALLOCATE_STRUCT(ActorSpray);
  if ( NewData == NULL )
	return NULL;

  	memcpy( NewData, Data, sizeof( *NewData ) );

	// setup defaults
	NewData->TimeRemaining = 0.0f;
	NewData->PolyCount = 0.0f;
	NewData->ActorNum = 0;
	NewData->Direction = true;

	// save the transform
	EffectC_XFormFromVector( &( NewData->Source ), &( NewData->Dest ), &( NewData->Xf ) );

  return (void *)NewData;
}

void EffManager::ActorSpray_Remove(ActorSpray *Data)
{
	// free effect data
	geRam_Free( Data );

}

geBoolean EffManager::ActorSpray_Process(ActorSpray  *Data,  float  TimeDelta)
{

	// locals
	geVec3d			Velocity, Speed;
	geVec3d			Left, Up, In;
	geVec3d			Source, Dest;
	const geXForm3d		*CameraXf;
	float			Scale;
	float			UnitLife;
	float			Distance;
	float			Adjustment = 1.0f;
	float			NewPolyCount = 0.0f;

	// adjust spray life, killing it if required
	if ( Data->SprayLife > 0.0f )
	{
		Data->SprayLife -= TimeDelta;
		if ( Data->SprayLife <= 0.0f )
		{
			return GE_FALSE;
		}
	}

	// do nothing if its paused
	if ( Data->Paused == GE_TRUE )
	{
		return GE_TRUE;
	}

	// get camera xform
	CameraXf = geCamera_GetWorldSpaceXForm(CCD->CameraManager()->Camera() );

	// perform level of detail processing if required
	if ( Data->DistanceMax > 0.0f )
	{

		// do nothing if its too far away
		Distance = geVec3d_DistanceBetween( &( Data->Source ), &( CameraXf->Translation ) );
		if ( Distance > Data->DistanceMax )
		{
			return GE_TRUE;
		}

		// determine polygon adjustment amount
		if ( ( Data->DistanceMin > 0.0f ) && ( Distance > Data->DistanceMin ) )
		{
			Adjustment = ( 1.0f - ( ( Distance - Data->DistanceMin ) / ( Data->DistanceMax - Data->DistanceMin ) ) );
		}
	}

	// determine how many polys need to be added taking level fo detail into account
	Data->TimeRemaining += TimeDelta;
	while ( Data->TimeRemaining >= Data->Rate )
	{
		Data->TimeRemaining -= Data->Rate;
		NewPolyCount += 1.0f;
	}
	NewPolyCount *= Adjustment;
	Data->PolyCount += NewPolyCount;

	// add new textures
	while ( Data->PolyCount > 0 )
	{

		// adjust poly remaining count
		Data->PolyCount -= 1.0f;

		// pick a source point
		if ( Data->SourceVariance > 0 )
		{
			geXForm3d_GetLeft( &( Data->Xf ), &Left );
			geXForm3d_GetUp( &( Data->Xf ), &Up );
			geXForm3d_GetIn( &( Data->Xf ), &In );
			geVec3d_Scale( &Left, (float)Data->SourceVariance * EffectC_Frand( -1.0f, 1.0f ), &Left );
			geVec3d_Scale( &Up, (float)Data->SourceVariance * EffectC_Frand( -1.0f, 1.0f ), &Up );
			geVec3d_Scale( &In, (float)Data->SourceVariance * EffectC_Frand( -1.0f, 1.0f ), &In );
			geVec3d_Add( &Left, &Up, &Source );
			geVec3d_Add( &Source, &In, &Source );
			geVec3d_Add( &( Data->Source ), &Source, &Source );
		}
		else
		{
			geVec3d_Copy( &( Data->Source ), &Source );
		}

		// pick a destination point
		if ( Data->DestVariance > 0 )
		{
			geXForm3d_GetLeft( &( Data->Xf ), &Left );
			geXForm3d_GetUp( &( Data->Xf ), &Up );
			geXForm3d_GetIn( &( Data->Xf ), &In );
			geVec3d_Scale( &Left, (float)Data->DestVariance * EffectC_Frand( -1.0f, 1.0f ), &Left );
			geVec3d_Scale( &Up, (float)Data->DestVariance * EffectC_Frand( -1.0f, 1.0f ), &Up );
			geVec3d_Scale( &In, (float)Data->DestVariance * EffectC_Frand( -1.0f, 1.0f ), &In );
			geVec3d_Add( &Left, &Up, &Dest );
			geVec3d_Add( &Dest, &In, &Dest );
			geVec3d_Add( &( Data->Dest ), &Dest, &Dest );
		}
		else
		{
			geVec3d_Copy( &( Data->Dest ), &Dest );
		}

		// set velocity
		if ( Data->MinSpeed > 0.0f )
		{
			geVec3d_Subtract( &Dest, &Source, &Velocity );
			geVec3d_Normalize( &Velocity );
			geVec3d_Scale( &Velocity, EffectC_Frand( Data->MinSpeed, Data->MaxSpeed ), &Velocity );
		}
		else
		{
			geVec3d_Set( &Velocity, 0.0f, 0.0f, 0.0f );
		}

		// set scale
		Scale = EffectC_Frand( Data->MinScale, Data->MaxScale );

		// set life
		UnitLife = EffectC_Frand( Data->MinUnitLife, Data->MaxUnitLife );

		Speed.X = EffectC_Frand( Data->MinRotationSpeed.X, Data->MaxRotationSpeed.X);
		Speed.Y = EffectC_Frand( Data->MinRotationSpeed.Y, Data->MaxRotationSpeed.Y);
		Speed.Z = EffectC_Frand( Data->MinRotationSpeed.Z, Data->MaxRotationSpeed.Z);

		switch(Data->Style)
		{
		case 0:
			Data->ActorNum +=1;
			if(Data->ActorNum>=Data->NumActors)
				Data->ActorNum = 0;
			break;
		case 1:
			if(Data->Direction)
			{
				Data->ActorNum +=1;
				if(Data->ActorNum>=Data->NumActors)
				{
					Data->ActorNum = Data->NumActors-1;
					Data->Direction = false;
				}
			}
			else
			{
				Data->ActorNum -=1;
				if(Data->ActorNum<0)
				{
					Data->ActorNum = 0;
					Data->Direction = true;
				}
			}
			break;
		case 2:
		default:
			Data->ActorNum = ( rand() % Data->NumActors );
			break;
		}

		char ActorName[64];
		sprintf(ActorName, "%s%d%s", Data->BaseName, Data->ActorNum, ".act" );

		// add the new particle
		ActorParticle_SystemAddParticle(APs,
						ActorName,
						Source,
						Data->BaseRotation,
						Speed,
						Data->FillColor, 
						Data->AmbientColor,
						Data->Alpha,
						Data->AlphaRate,
						UnitLife,
						&Velocity,
						Scale,
						Data->Gravity,
						Data->Bounce,
						Data->Solid,
						Data->EnvironmentMapping,
						Data->AllMaterial,
						Data->PercentMapping,
						Data->PercentMaterial);
	}

	// all done
	return GE_TRUE;

} // Spray_Process()

geBoolean EffManager::ActorSpray_Modify(ActorSpray *Data, ActorSpray *NewData, uint32 Flags)
{

  // adjust source and dest together
  if ( Flags & SPRAY_FOLLOWTAIL )
  {
	geVec3d_Copy( &( Data->Source ), &( Data->Dest ) );
	geVec3d_Copy( &( NewData->Source ), &( Data->Source ) );
  }

  // adjust source
  if ( Flags & SPRAY_SOURCE )
  {
	geVec3d_Copy( &( NewData->Source ), &( Data->Source ) );
  }

  // adjust source
  if ( Flags & SPRAY_DEST )
  {
	  geVec3d	In;
	  geXForm3d_RotateZ( &(NewData->Xform), Data->Angle.Z / 57.3f );
	  geXForm3d_RotateX( &(NewData->Xform), Data->Angle.X / 57.3f );  
	  geXForm3d_RotateY( &(NewData->Xform), Data->Angle.Y / 57.3f );  
 
	  geXForm3d_GetIn( &(NewData->Xform), &In );
	  geVec3d_Inverse( &In );
	  geVec3d_AddScaled( &( Data->Source ), &In, 50.0f, &( Data->Dest ) );
  }

  if ( Flags & SPRAY_ACTUALDEST )
  {
	  geVec3d_Copy( &( NewData->Dest ), &( Data->Dest ) );
  }

  EffectC_XFormFromVector( &( Data->Source ), &( Data->Dest ), &( Data->Xf ) );

  // all done
  return GE_TRUE;
}

