////////////////////////////////////////////////////////////////////////////////////////
//  EffParticle.cpp  - particle system manager									                                        
//                                                                                      
//  Author:
//		Eli Boling		Created
//		Peter Siamidis	Added anchor point support.
//                                                                                      
//  The contents of this file are subject to the Genesis3D Public License               
//  Version 1.01 (the "License"); you may not use this file except in                   
//  compliance with the License. You may obtain a copy of the License at                
//  http://www.genesis3d.com                                                            
//                                                                                      
//  Software distributed under the License is distributed on an "AS IS"                 
//  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See                
//  the License for the specific language governing rights and limitations              
//  under the License.                                                                  
//                                                                                      
//  Copyright (C) 1996-1999 Eclipse Entertainment, L.L.C. All Rights Reserved           
//                                                                                      
////////////////////////////////////////////////////////////////////////////////////////
#include "RabidFramework.h"

#include <memory.h>
//#include <malloc.h>
#include "genesis.h"
#include <Ram.h>

#define	POLYQ

#define	PARTICLE_USER			( 1 << 0 )
#define	PARTICLE_HASVELOCITY	( 1 << 1 )
#define PARTICLE_HASGRAVITY		( 1 << 2 )
#define PARTICLE_RENDERSTYLE	GE_RENDER_DEPTH_SORT_BF | GE_RENDER_DO_NOT_OCCLUDE_OTHERS

typedef struct	Particle
{
	GE_LVertex		ptclVertex;
	geBitmap *		ptclTexture;
	gePoly *		ptclPoly;
	unsigned 	 	ptclFlags;
	Particle *		ptclNext;
	Particle *		ptclPrev;
	geFloat			Scale;
	geVec3d			Gravity;
	geFloat			Alpha;
	geVec3d			CurrentAnchorPoint;
	const geVec3d	*AnchorPoint;
	geFloat	 	 	ptclTime;
	geFloat			ptclTotalTime;
	geVec3d	 	    ptclVelocity;
	geBoolean		Bounce;
}	Particle;

typedef	struct	Particle_System
{
	Particle *		psParticles;
	geWorld *		psWorld;
	geFloat			psLastTime;
	geFloat			psQuantumSeconds;
}	Particle_System;

void Particle_SetTexture(Particle *p, geBitmap *Texture)
{
	p->ptclTexture = Texture;
}

GE_LVertex *  Particle_GetVertex(Particle *p)
{
	return &p->ptclVertex;
}

#define	QUANTUMSIZE	(1.0f / 30.0f)

Particle_System *  Particle_SystemCreate(geWorld *World)
{
	Particle_System *ps;

	ps = GE_RAM_ALLOCATE_STRUCT(Particle_System);
	if	(!ps)
		return ps;

	memset(ps, 0, sizeof(*ps));
	
	ps->psWorld	= World;
	ps->psQuantumSeconds = QUANTUMSIZE;
	ps->psLastTime = 0.0f;

	return ps;
}

static	void 	DestroyParticle(Particle_System *ps, Particle *p)
{
	if(p->ptclPoly)
		geWorld_RemovePoly(ps->psWorld, p->ptclPoly);

	geRam_Free(p);

}

void 	Particle_SystemDestroy(Particle_System *ps)
{
	Particle *	ptcl;

	ptcl = ps->psParticles;
	while	(ptcl)
	{
		Particle *	temp;

		temp = ptcl->ptclNext;
		DestroyParticle(ps, ptcl);
		ptcl = temp;
	}

	geRam_Free(ps);

}

static	void 	UnlinkParticle(Particle_System *ps, Particle *ptcl)
{
	if	(ptcl->ptclPrev)
		ptcl->ptclPrev->ptclNext = ptcl->ptclNext;

	if	(ptcl->ptclNext)
		ptcl->ptclNext->ptclPrev = ptcl->ptclPrev;

	if	(ps->psParticles == ptcl)
		ps->psParticles = ptcl->ptclNext;
}

void  Particle_SystemRemoveAll(Particle_System *ps)
{
	Particle *	ptcl;

	ptcl = ps->psParticles;
	while	(ptcl)
	{
		Particle *	temp;

		temp = ptcl->ptclNext;
		UnlinkParticle(ps, ptcl);
		DestroyParticle(ps, ptcl);
		ptcl = temp;
	}
}

int32 	Particle_GetCount(Particle_System *ps)
{

	// locals
	Particle	*ptcl;
	int32		TotalParticleCount = 0;

	// count up how many particles are active in this particle system
	ptcl = ps->psParticles;
	while ( ptcl )
	{
		ptcl = ptcl->ptclNext;
		TotalParticleCount++;
	}

	// return the active count
	return TotalParticleCount;

}

void 	Particle_SystemFrame(Particle_System *ps, geFloat DeltaTime)
{
	
	geVec3d	AnchorDelta;
	
	// the quick fix to the particle no-draw problem 
	ps->psQuantumSeconds = DeltaTime;
	{
		
		Particle *	ptcl;
		
		ptcl = ps->psParticles;
		while	(ptcl)
		{
			ptcl->ptclTime -= ps->psQuantumSeconds;
			if	(ptcl->ptclTime <= 0.0f)
			{
				Particle *	temp;
				
				temp = ptcl->ptclNext;
				UnlinkParticle(ps, ptcl);
				DestroyParticle(ps, ptcl);
				ptcl = temp;
				continue;
			}
			else
			{
				
				// locals
				geVec3d		DeltaPos = { 0.0f, 0.0f, 0.0f };
				
				// apply velocity
				if ( ptcl->ptclFlags & PARTICLE_HASVELOCITY )
				{
					geVec3d_Scale( &( ptcl->ptclVelocity ), ps->psQuantumSeconds, &DeltaPos );
				}
				
				// apply gravity
				if ( ptcl->ptclFlags & PARTICLE_HASGRAVITY )
				{
					// locals
					geVec3d	Gravity;
					
					// make gravity vector
					geVec3d_Scale( &( ptcl->Gravity ), ps->psQuantumSeconds, &Gravity );
					
					// apply gravity to built in velocity and DeltaPos
					geVec3d_Add( &( ptcl->ptclVelocity ), &Gravity, &( ptcl->ptclVelocity ) );
					geVec3d_Add( &DeltaPos, &Gravity, &DeltaPos );
				}
				
				// apply DeltaPos to particle position
				if (( ptcl->ptclFlags & PARTICLE_HASVELOCITY ) || ( ptcl->ptclFlags & PARTICLE_HASGRAVITY ) )
				{
					geVec3d temppos, temppos1;
					GE_Collision Collision;
					geVec3d_Copy((geVec3d *)&(ptcl->ptclVertex.X), &temppos);
					geVec3d_Add(&temppos, &DeltaPos, &temppos1);
					
					if(ptcl->Bounce)
					{
						float totalTravelled = 1.0f ; // keeps track of fraction of path travelled (1.0=complete)
						float margin = 0.001f ; // margin to be satisfied with (1.0 - margin == 1.0)
						int loopCounter = 0 ; // safety valve for endless collisions in tight corners
						const int maxLoops = 10 ;
						
						while(geWorld_Collision(CCD->World(), NULL, NULL, &temppos, 
							&temppos1, GE_VISIBLE_CONTENTS, GE_COLLIDE_ALL, 0, NULL, NULL, &Collision))
						{
							float ratio ;
							float elasticity = 1.3f ;
							float friction = 0.9f ; // loses (1 minus friction) of speed
							CollisionCalcRatio( Collision, temppos, temppos1, ratio ) ;
							CollisionCalcImpactPoint( Collision, temppos, temppos1, 1.0f, ratio, temppos1 ) ;
							CollisionCalcVelocityImpact( Collision, ptcl->ptclVelocity, elasticity, friction, ptcl->ptclVelocity) ; 
							if( ratio >= 0 )
								totalTravelled += (1.0f - totalTravelled) * ratio ;
							if( totalTravelled >= 1.0f - margin )
								break ;
							if( ++ loopCounter >= maxLoops ) // safety check
								break ;
						}
					}
					else
					{
						if(geWorld_Collision(CCD->World(), NULL, NULL, &temppos, 
							&temppos1, GE_VISIBLE_CONTENTS, GE_COLLIDE_ALL, 0, NULL, NULL, &Collision))
						{
							Particle *	temp;
							temp = ptcl->ptclNext;
							UnlinkParticle(ps, ptcl);
							DestroyParticle(ps, ptcl);
							ptcl = temp;
							continue;
						}
					}
					geVec3d_Copy(&temppos1, (geVec3d *)&(ptcl->ptclVertex.X));
				}
				
				// make the particle follow its anchor point if it has one
				if ( ptcl->AnchorPoint != (const geVec3d *)NULL )
				{
					geVec3d_Subtract( ptcl->AnchorPoint, &( ptcl->CurrentAnchorPoint ), &AnchorDelta );
					geVec3d_Add( (geVec3d *)&( ptcl->ptclVertex.X ), &AnchorDelta, (geVec3d *)&( ptcl->ptclVertex.X ) );
					geVec3d_Copy( ptcl->AnchorPoint, &( ptcl->CurrentAnchorPoint ) );
				}
				
			}
			
#ifndef	POLYQ
			// set particle alpha
			ptcl->ptclVertex.a = ptcl->Alpha * ( ptcl->ptclTime / ptcl->ptclTotalTime );
			geWorld_AddPolyOnce(ps->psWorld,
				&ptcl->ptclVertex,
				1,
				ptcl->ptclTexture,
				GE_TEXTURED_POINT,
				PARTICLE_RENDERSTYLE,
				ptcl->Scale );
#else
			// set particle alpha
			ptcl->ptclVertex.a = ptcl->Alpha * ( ptcl->ptclTime / ptcl->ptclTotalTime );
			
			if(ptcl->ptclPoly)
				gePoly_SetLVertex(ptcl->ptclPoly, 0, &ptcl->ptclVertex);
#endif
			
			ptcl = ptcl->ptclNext;
		}
		
		DeltaTime -= QUANTUMSIZE;
	}
	
	ps->psLastTime += DeltaTime;
}


static	Particle * CreateParticle(Particle_System *ps, geBitmap *Texture, const GE_LVertex *Vert )
{
	Particle *ptcl;

	ptcl = GE_RAM_ALLOCATE_STRUCT(Particle);
	if	(!ptcl)
		return ptcl;

	memset(ptcl, 0, sizeof(*ptcl));

	ptcl->ptclNext = ps->psParticles;
	ps->psParticles = ptcl;
	if(ptcl->ptclNext)
	  ptcl->ptclNext->ptclPrev = ptcl;
	ptcl->ptclTexture = Texture;
	ptcl->ptclVertex = *Vert;

	return ptcl;
}

// removes all references to an anchor point
geBoolean Particle_SystemRemoveAnchorPoint(Particle_System *ps, geVec3d	*AnchorPoint )
{
	// locals	
	Particle	*ptcl;
	geBoolean	AtLeastOneFound = GE_FALSE;

	// eliminate achnor point from all particles in this particle system
	ptcl = ps->psParticles;
	while ( ptcl != NULL )
	{
		if ( ptcl->AnchorPoint == AnchorPoint )
		{
			ptcl->AnchorPoint = (const geVec3d *)NULL;
			AtLeastOneFound = GE_TRUE;
		}
		ptcl = ptcl->ptclNext;
	}

	// all done
	return AtLeastOneFound;
}

void 	Particle_SystemAddParticle(
	Particle_System		*ps,
	geBitmap	*Texture,
	const GE_LVertex	*Vert,
	const geVec3d		*AnchorPoint,
	geFloat				Time,
	const geVec3d		*Velocity,
	geFloat				Scale,
	const geVec3d		*Gravity,
	geBoolean Bounce)
{

	// locals
	Particle	*ptcl;

	// create a new particle
	ptcl = CreateParticle( ps, Texture, Vert );
	if ( !ptcl )
	{
		return;
	}

	// setup gravity
	if ( Gravity != (const geVec3d *)NULL )
	{
		geVec3d_Copy( Gravity, &( ptcl->Gravity ) );
		ptcl->ptclFlags |= PARTICLE_HASGRAVITY;
	}

	// setup velocity
	if ( Velocity != (const geVec3d *)NULL )
	{
		geVec3d_Copy( Velocity, &( ptcl->ptclVelocity ) );
		ptcl->ptclFlags |= PARTICLE_HASVELOCITY;
	}

	// setup the anchor point
	if ( AnchorPoint != (const geVec3d *)NULL )
	{
		geVec3d_Copy( AnchorPoint, &( ptcl->CurrentAnchorPoint ) );
		ptcl->AnchorPoint = AnchorPoint;
	}

	// setup remaining data
	ptcl->Scale = Scale;
	ptcl->ptclTime = Time;
	ptcl->ptclTotalTime = Time;
	ptcl->Alpha = Vert->a;
	ptcl->Bounce = Bounce;

	// add the poly to the world
	#ifdef	POLYQ
	ptcl->ptclPoly = geWorld_AddPoly(ps->psWorld,
					&ptcl->ptclVertex,
					1,
					ptcl->ptclTexture,
					GE_TEXTURED_POINT,
					PARTICLE_RENDERSTYLE,
					ptcl->Scale );
	#endif
}

void  Particle_SystemReset(Particle_System *ps)
{
	ps->psLastTime = 0.0f;
	Particle_SystemRemoveAll(ps);
}

