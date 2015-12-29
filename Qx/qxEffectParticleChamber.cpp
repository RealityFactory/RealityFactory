// qxEffectParticleChamber.cpp: implementation of the qxEffectParticleChamber class.
//
// Author: Jeff Thelen
//
// Copyright 1999 Quixotic, Inc.  All Rights Reserved.
//////////////////////////////////////////////////////////////////////

#include "..\\RabidFramework.h"
#include "qxEffectParticleChamber.h"
#include "qxParticle.h"
#include "QxUser.h"	//Genesis

extern geBitmap *TPool_Bitmap(const char *DefaultBmp, const char *DefaultAlpha,
							  const char *BName, const char *AName);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Not needed except for protection against absurd values.
#define MAX_ANIMATION_BMPS	256

#define M_PI 3.14159

//////////////////////////////////////////////////////////////////////
// Constructor for those entities not coming from editor
//////////////////////////////////////////////////////////////////////


qxEffectParticleChamber::qxEffectParticleChamber(char* strName, void*& /*qxFromEditor*/)
:
qxEffectBase(strName)
,m_fEmissionResidue(0.0f)
,m_fTimeElapsed(0.0f)
,m_bAnimationMode(false)
,m_bQuadRandomInitialRotate(false)
,m_bUseGravity(false)
,m_bUseVelocity(false)

//QX VARIABLES
,AnglesRandom(false)
,ParticlesPerSec(25.0)
,SpeedMin(0.0f)
,SpeedMax(0.0f)
,UnitLifeMin(0.0f)
,UnitLifeMax(0.0f)
,VarianceSource(0)
,VarianceDest(0)
,BmpName(NULL)
,BmpAlphaName(NULL)
,RandomGravity(0)
,AnimateBmpPrefix(NULL)
,AnimateLoop(0)
,AnimateKillNoLoop(0)
,AnimateFPS(0)
,Quad(0)
,AnglesRandomPitch(180.0f)
,AnglesRandomYaw(360.0f)
,AlphaStart(128.f)
,AlphaEnd(0.0f)
,SizeStart(4.0f)
,SizeEnd(8.0f)
,SizeVariation(.5f)
,ParticlesMax(100)
,AlphaFadeInFadeOut(false)
,m_fAlphaTempEnd(0.0f)
,m_nRenderStyle(QX_PARTICLE_RENDER_STYLE)
//QX VARIABLES
{

	geVec3d_Clear(&Gravity);
	geVec3d_Clear(&RandomGravityMin);
	geVec3d_Clear(&RandomGravityMax);
	geVec3d_Clear(&Angles);
	geVec3d_Clear(&QuadRotateDegreesMin);
	geVec3d_Clear(&QuadRotateDegreesMax);
	geVec3d_Clear(&QuadRandInitialRotateDegreesMax);
	geVec3d_Clear(&QuadRandInitialRotateDegreesMin);

	ColorInit(&ColorStart);
	ColorInit(&ColorEnd);
}


qxEffectParticleChamber::~qxEffectParticleChamber()
{
	std::vector<qxParticleBase*>::const_iterator it = m_pParticles.begin();
	for(;it != m_pParticles.end(); ++it)
	{
		delete *it;
	}
}


bool qxEffectParticleChamber::Init()
{
	if(!qxEffectBase::Init() )
		return false;

	QXASSERT( IsColorGood( &ColorStart ));
	QXASSERT( IsColorGood( &ColorEnd ));
	QXASSERT( ParticlesPerSec > 0.0 );
	QXASSERT( SpeedMax >= SpeedMin );
	QXASSERT( UnitLifeMin > 0.0f );
	QXASSERT( UnitLifeMax >= UnitLifeMin );
	QXASSERT( VarianceSource>= 0);
	QXASSERT( VarianceDest >= 0);

	QXASSERT(RandomGravityMin.X <= RandomGravityMax.X);
	QXASSERT(RandomGravityMin.Y <= RandomGravityMax.Y);
	QXASSERT(RandomGravityMin.Z <= RandomGravityMax.Z);


	if( !EffectC_IsStringNull( AnimateBmpPrefix) )
	{
		QXASSERT( AnimateFPS > 0 );

		LoadAnimation();

		QXASSERT( MAX_ANIMATION_BMPS > m_BmpArray.size() );
	}
	// else, normal mode
	else
	{
		m_pBmp = TPool_Bitmap(BmpName, BmpAlphaName, NULL, NULL);
	}

	QXASSERT(m_pBmp);


	m_pParticles.resize(ParticlesMax);

	for( int i = 0; i < ParticlesMax; i++)
	{
		qxParticleBase* p = 0;

		if( !Quad )
			p = new qxParticleBase;
		else
			p = (qxParticleBase*) new qxParticleQuad;

		p->m_pParent  = this;
		m_pParticles[i] = p;
	}

	// setup orientation
	SetAngles( &Angles );

	// setup default vertex data
	m_Vertex.u = 0.0f;
	m_Vertex.v = 0.0f;

	if(Quad)
	{
		InitQuad();
	}

	m_bUseGravity = !geVec3d_IsZero(&Gravity);
	m_bUseVelocity = (SpeedMax > 0.0f);


	m_fAlphaTempEnd = AlphaEnd;

	if( AlphaFadeInFadeOut )
	{
		QXASSERT(AlphaEnd > AlphaStart);
		m_fAlphaTempEnd = AlphaEnd*2.0f;
	}

	return true;
}

bool qxEffectParticleChamber::SetAngles(geVec3d* pAngles)
{
	Angles = *pAngles;

	geXForm3d_SetIdentity( &m_matXForm );
	geXForm3d_RotateX( &m_matXForm, RADIANS(pAngles->X));
	geXForm3d_RotateY( &m_matXForm, RADIANS( pAngles->Y));
	geXForm3d_RotateZ( &m_matXForm, RADIANS(pAngles->Z));

	if(m_pActor)
	{
		if(!geActor_GetBoneTransform( m_pActor, BoneName, &m_matXForm ))
			return false;

		geVec3d_Copy( &m_matXForm.Translation, &Origin);
	}
	else
	{
		geVec3d		In;

		// setup dest position
		geXForm3d_GetIn( &m_matXForm, &In );
		geVec3d_AddScaled( &Origin, &In, 50.0f, &m_vDest );
	}

	// save the transform
	EffectC_XFormFromVector( &Origin, &m_vDest, &m_matXForm );

	return true;
}

void qxEffectParticleChamber::SetOriginAndDest(geVec3d* pPos, geVec3d* pDest)
{
	if( pDest )
	{
		m_vDest  = *pDest;
	}

	qxEffectBase::SetOriginAndDest(pPos, pDest);

	EffectC_XFormFromVector( &Origin, &m_vDest, &m_matXForm );

	SetAngles( &Angles );
}

////////////////////////////////////////////////////////////////////////////////////////
//
// qxEffectParticleChamber::Frame()
//
////////////////////////////////////////////////////////////////////////////////////////
int qxEffectParticleChamber::Frame()
{
	// return -1 if we should continue in this function.
	int ret = qxEffectBase::Frame();
	if( ret == 0 ) // 0 == kill effect
		return 0;

	float fTimeElapsed = (CCD->LastElapsedTime_F()*0.001f);

	// clear our counter variable before counting how many particles are alive
	m_nParticlesAlive = 0;

	// update all particles that we own
	for ( int i=0; i<ParticlesMax; i++ )
	{
		if ( m_pParticles[i]->m_fAge >= 0.0f )
		{
			// Note that the update function returns FALSE if the particle died
			if ( m_pParticles[i]->Frame( fTimeElapsed ) )
			{
				m_pParticles[i]->UpdatePoly();
				m_nParticlesAlive++;
			}
		}
	}

	SoundFrame();

	if( ret != -1 )
	{
		return ret; // else, 0 == kill effect, 1 == continue effect
	}


	// adjust position if it's hooked to an actor
	if ( m_pActor )
	{
		// locals
		geVec3d		In;

		// get bone location
		if(!geActor_GetBoneTransform( m_pActor, BoneName, &m_matXForm ))
			return 0;

		geVec3d_Copy( &m_matXForm.Translation, &Origin);
		geXForm3d_GetIn( &m_matXForm, &In );
		geVec3d_Inverse( &In );
		geVec3d_AddScaled( &Origin, &In, 50.0f, &m_vDest );


		EffectC_XFormFromVector( &Origin, &m_vDest, &m_matXForm );
		if(!geWorld_GetLeaf(CCD->Engine()->World(), &Origin, &m_nLeaf ))
			return 0;
	}//if actor attached


	if(!IsVisible())
	{
		//Printer()->AddDebugStringBottom("NOTVISIBLE");
		return 1;
	}

	//Printer()->AddDebugStringBottom("VISIBLE");

	float fDistanceAdjustment = 1.0f;

	// perform level of detail processing if required
	if ( DistanceMax > 0.0f )
	{
		float fDistance = CheckVisibleDistance();
		if(fDistance < 0.0f)
			return 1;


		// determine polygon adjustment amount
		if ( fDistance > DistanceMin )
		{
			// Distance min determines the minimum needed for full intensity
			// .67 = ( 1.0f - ( (1000-100)/(3000-100) )
			fDistanceAdjustment = ( 1.0f - ( ( fDistance - DistanceMin ) / ( DistanceMax - DistanceMin ) ) );
		}
	}


	// calculate how many particles we should create from ParticlesPerSec and
	// time elapsed taking the
	// previous frame's EmissionResidue into account.
	float fParticlesNeeded = (ParticlesPerSec * fTimeElapsed * fDistanceAdjustment)+ m_fEmissionResidue;

	// cast the float fparticlesNeeded to an INT to see
	// how many particles we really need to create.
	int ParticlesCreated = (unsigned int)fParticlesNeeded;

	// remember the difference between what we wanted to create and how many we created.
	m_fEmissionResidue = fParticlesNeeded - ParticlesCreated;

	// see if we actually have any to create
	if ( ParticlesCreated > 0 )
	{
		// loop through all the particles to see if any are available
		for (int i=0; i<ParticlesMax; i++ )
		{
			// if we have created enough particles,
			// this value will be 0 and we can skip the rest
			// alternatively you could mantain a list of free particles and used particles
			if ( !ParticlesCreated )
				break;

			// if the age is -1.0f then this particle is not in use
			if ( m_pParticles[i]->m_fAge < 0.0f )
			{
				// New particle so its age is 0.0f
				m_pParticles[i]->m_fAge = 0.0f;
				m_pParticles[i]->m_fLifetime = Frand( UnitLifeMin, UnitLifeMax );

				// our start color is going to be the System's StartColor
				// + the System's color variation
				m_pParticles[i]->m_vVertex.r = ColorStart.r;
				m_pParticles[i]->m_vVertex.g = ColorStart.g;
				m_pParticles[i]->m_vVertex.b = ColorStart.b;
				m_pParticles[i]->m_vVertex.a = AlphaStart;


				if( AlphaFadeInFadeOut )
				{
					m_pParticles[i]->m_fAlphaEnd = AlphaEnd;
					m_pParticles[i]->m_nFlags |= QX_PARTICLE_ALPHA_FADEIN_FADEOUT;
				}

				// calculate a delta so that by the time the particle dies,
				// it will have reached its "ColorEnd"
				m_pParticles[i]->m_ColorDelta.r = (ColorEnd.r - m_pParticles[i]->m_vVertex.r ) / m_pParticles[i]->m_fLifetime;
				m_pParticles[i]->m_ColorDelta.g = (ColorEnd.g - m_pParticles[i]->m_vVertex.g) / m_pParticles[i]->m_fLifetime;
				m_pParticles[i]->m_ColorDelta.b = (ColorEnd.b - m_pParticles[i]->m_vVertex.b) / m_pParticles[i]->m_fLifetime;
				m_pParticles[i]->m_ColorDelta.a = (m_fAlphaTempEnd - m_pParticles[i]->m_vVertex.a) / m_pParticles[i]->m_fLifetime;

				m_pParticles[i]->m_fSize = SizeStart + RandFloat() * SizeVariation;
				m_pParticles[i]->m_fSizeDelta = (SizeEnd - m_pParticles[i]->m_fSize) / m_pParticles[i]->m_fLifetime;

				if(RandomGravity)
				{
					m_pParticles[i]->m_vGravity.X = Frand( RandomGravityMin.X, RandomGravityMax.X );
					m_pParticles[i]->m_vGravity.Y = Frand( RandomGravityMin.Y, RandomGravityMax.Y );
					m_pParticles[i]->m_vGravity.Z = Frand( RandomGravityMin.Z, RandomGravityMax.Z );
					m_pParticles[i]->m_nFlags |= QX_PARTICLE_HAS_GRAVITY;
				}
				//else gravity is set already
				else if( m_bUseGravity )
				{
					m_pParticles[i]->m_vGravity = Gravity;
					m_pParticles[i]->m_nFlags |= QX_PARTICLE_HAS_GRAVITY;
				}


				geVec3d Source;

				// pick a source point
				if ( VarianceSource > 0 )
				{
					geVec3d	Left, Up;
					geXForm3d_GetLeft( &m_matXForm, &Left );
					geXForm3d_GetUp( &m_matXForm, &Up );
					geVec3d_Scale( &Left, (float)VarianceSource * Frand( -1.0f, 1.0f ), &Left );
					geVec3d_Scale( &Up, (float)VarianceSource * Frand( -1.0f, 1.0f ), &Up );
					geVec3d_Add( &Left, &Up, &Source );
					geVec3d_Add( &Origin, &Source, &Source );
				}
				else
				{
					// Use GetOrigin(). Some classes may not want to
					// use the original origin
					geVec3d_Copy( GetOrigin(), &Source );
				}

				m_pParticles[i]->m_vVertex.X = Source.X;
				m_pParticles[i]->m_vVertex.Y = Source.Y;
				m_pParticles[i]->m_vVertex.Z = Source.Z;

				// The emitter has a Direction.  This code adds some randomness to that direction so
				// that we don't emit a Line of particles.
				// For the demo I always assume a randomess of
				// 360 degrees in the Yaw direction.  In your code, you really should allow
				// this to be modified
				// separately from the Pitch.  Normally I would define PI 3.14159 and have a DEG_2_RAD macro
				// but it is used so briefly in this app I didn't
				if( AnglesRandom )
				{
					float RandomYaw = (geFloat)(RANDOM_ONE_TO_ZERO * AnglesRandomYaw * M_PI * 2.0f);
					float RandomPitch = (geFloat)(RANDOM_ONE_TO_ZERO *  AnglesRandomPitch * M_PI / 180.0f);

					// this uses spherical coordinates to randomize the velocity vector
					// ( or the direction ) of the particle
					m_pParticles[i]->m_vVelocity.Y = cosf( RandomPitch );
					m_pParticles[i]->m_vVelocity.X = sinf(RandomPitch) * cosf(RandomYaw);
					m_pParticles[i]->m_vVelocity.Z = sinf(RandomPitch) * sinf(RandomYaw);

				}
				// Not using random angles. Use the object matrix.
				else
				{
					geVec3d Dest;

					// pick a destination point
					if ( VarianceDest > 0 )
					{
						geVec3d			Left, Up;

						geXForm3d_GetLeft( &m_matXForm, &Left );
						geXForm3d_GetUp( &m_matXForm, &Up );
						geVec3d_Scale( &Left, (float)VarianceDest * Frand( -1.0f, 1.0f ), &Left );
						geVec3d_Scale( &Up, (float)VarianceDest * Frand( -1.0f, 1.0f ), &Up );
						geVec3d_Add( &Left, &Up, &Dest );
						geVec3d_Add( &m_vDest, &Dest, &Dest );
					}
					else
					{
						geVec3d_Copy( &m_vDest, &Dest );
					}

					geVec3d_Subtract( &Dest, &Source, &m_pParticles[i]->m_vVelocity );
					geVec3d_Normalize( &m_pParticles[i]->m_vVelocity );

				}// if not using random angles

				if( m_bUseVelocity )
				{
					float fNewSpeed = Frand( SpeedMin, SpeedMax );
					geVec3d_Scale(&m_pParticles[i]->m_vVelocity, fNewSpeed, &m_pParticles[i]->m_vVelocity);
					m_pParticles[i]->m_nFlags |= QX_PARTICLE_HAS_VELOCITY;
				}


				if( m_bAnimationMode )
					m_pParticles[i]->m_nFlags |= QX_PARTICLE_ANIMATION;


				//
				// QUADS
				//
				if( Quad )
				{
					qxParticleQuad* pQuad = (qxParticleQuad*)m_pParticles[i];

					pQuad->m_vRotateRads.X = Frand( m_vQuadRotateRadsMin.X, m_vQuadRotateRadsMax.X );
					pQuad->m_vRotateRads.Y = Frand( m_vQuadRotateRadsMin.Y, m_vQuadRotateRadsMax.Y );
					pQuad->m_vRotateRads.Z = Frand( m_vQuadRotateRadsMin.Z, m_vQuadRotateRadsMax.Z );


					for ( int i = 0; i < 4; i++ )
					{
						pQuad->m_vQuadVerts[i].r = pQuad->m_vVertex.r;
						pQuad->m_vQuadVerts[i].g = pQuad->m_vVertex.g;
						pQuad->m_vQuadVerts[i].b = pQuad->m_vVertex.b;
						pQuad->m_vQuadVerts[i].a = pQuad->m_vVertex.a;
					}
					// Get original orientation
					geVec3d vOrientation;
					CCD->CameraManager()->GetPosition(&vOrientation);
					//Make it 2D
					vOrientation.Y = pQuad->m_vVertex.Y;

					// Get the local mat from the current position and the player's view.
					EffectC_XFormFromVector((geVec3d *)&(pQuad->m_vVertex.X), &vOrientation, &pQuad->m_matLocal );
					// Move it to local origin (zero)
					geXForm3d_Translate(&pQuad->m_matLocal, -pQuad->m_vVertex.X,
						-pQuad->m_vVertex.Y, -pQuad->m_vVertex.Z);

					//If there's an *initial* rotation
					if( m_bQuadRandomInitialRotate )
					{
						m_vQuadInitialRotateRads.X = Frand( m_vQuadInitialRotateRadsMin.X, m_vQuadInitialRotateRadsMax.X );
						m_vQuadInitialRotateRads.Y = Frand( m_vQuadInitialRotateRadsMin.Y, m_vQuadInitialRotateRadsMax.Y );
						m_vQuadInitialRotateRads.Z = Frand( m_vQuadInitialRotateRadsMin.Z, m_vQuadInitialRotateRadsMax.Z );

						//STRVECTOR(m_vQuadInitialRotateRads);

						geXForm3d matRotate;
						geXForm3d_SetIdentity(&matRotate);

						if( fabs(m_vQuadInitialRotateRads.X) > 0.0f)
							geXForm3d_RotateX( &matRotate, m_vQuadInitialRotateRads.X );
						if( fabs(m_vQuadInitialRotateRads.Y) > 0.0f)
							geXForm3d_RotateY(&matRotate, m_vQuadInitialRotateRads.Y );
						if( fabs(m_vQuadInitialRotateRads.Z) > 0.0f)
							geXForm3d_RotateZ(&matRotate, m_vQuadInitialRotateRads.Z );

						geXForm3d_Multiply(&pQuad->m_matLocal, &matRotate, &pQuad->m_matLocal);

					}

					pQuad->m_pPoly = geWorld_AddPoly(CCD->Engine()->World(),
														&pQuad->m_vQuadVerts[0],
														4,
														m_pBmp,
														GE_TEXTURED_POLY,
														m_nRenderStyle,
														pQuad->m_fSize );

					// Due to bug somewhere...
					pQuad->Frame(0.0001f);
					pQuad->UpdatePoly();
					// Due to bug somewhere...

				}
				//
				//QUAD
				//
				else
				{
					m_pParticles[i]->m_pPoly = geWorld_AddPoly(CCD->Engine()->World(),
										&m_pParticles[i]->m_vVertex,
										1,
										m_pBmp,
										GE_TEXTURED_POINT,
										m_nRenderStyle,
										m_pParticles[i]->m_fSize );
				}

				// decrement the number of needed particles
				ParticlesCreated--;
			}
		}
	}

	return 1;
} // Frame()


void qxEffectParticleChamber::AddBmp(const char* pBmp, const char* pAlpha)
{
	// LoadBitmaps
	char Bmp[128], Alpha[128];
	strcpy(Bmp, pBmp);
	strcpy(Alpha, pAlpha);
	geBitmap* p = TPool_Bitmap(Bmp, Alpha, NULL, NULL);
	QXASSERT(p);
	m_BmpArray.push_back(p);
}


// This can only be called once.
// Not necessary if coming from the editor
void qxEffectParticleChamber::SetAnimationMode()
{
	if( m_bAnimationMode )
		return;

	m_bAnimationMode = true;
}



void qxEffectParticleChamber::LoadAnimation()
{
	m_bAnimationMode = true;

	//BmpMgr()->LoadAnimation(AnimateBmpPrefix, &m_BmpArray);

	//m_pBmp = m_BmpArray[0];

	//QXASSERT(m_pBmp);
}

bool qxEffectParticleChamber::ReInit()
{
	QXASSERT( IsColorGood( &ColorStart ));
	QXASSERT( IsColorGood( &ColorEnd ));
	QXASSERT( ParticlesPerSec > 0.0 );
	QXASSERT( SpeedMax >= SpeedMin );
	QXASSERT( UnitLifeMin > 0.0f );
	QXASSERT( UnitLifeMax >= UnitLifeMin );
	QXASSERT( VarianceSource>= 0);
	QXASSERT( VarianceDest >= 0);

	QXASSERT(RandomGravityMin.X <= RandomGravityMax.X);
	QXASSERT(RandomGravityMin.Y <= RandomGravityMax.Y);
	QXASSERT(RandomGravityMin.Z <= RandomGravityMax.Z);

	// Animation can't be changed now.
	if(!EffectC_IsStringNull( AnimateBmpPrefix) )
	{
	}
	else
	{
		m_pBmp = TPool_Bitmap(BmpName, BmpAlphaName, NULL, NULL);
	}

	QXASSERT(m_pBmp);

	std::vector<qxParticleBase*>::const_iterator it = m_pParticles.begin();
	for(;it != m_pParticles.end(); ++it)
	{
		delete *it;
	}

	m_pParticles.clear();
	m_pParticles.resize(ParticlesMax);

	for(int j = 0; j < ParticlesMax; ++j)
	{
		qxParticleBase* p = 0;

		if( !Quad )
			p = new qxParticleBase;
		else
			p = (qxParticleBase*) new qxParticleQuad;

		p->m_pParent  = this;
		m_pParticles[j] = p;
	}

	// setup orientation
	SetAngles( &Angles );

	if(Quad)
	{
		InitQuad();
	}

	m_bUseGravity = !geVec3d_IsZero(&Gravity);
	m_bUseVelocity = (SpeedMax > 0.0f);

	m_fAlphaTempEnd = AlphaEnd;

	if( AlphaFadeInFadeOut )
	{
		QXASSERT(AlphaEnd > AlphaStart);
		m_fAlphaTempEnd = AlphaEnd*2.0f;
	}

	return true;
}


void qxEffectParticleChamber::KillAllParticles( )
{
	for ( int i=0; i < ParticlesMax; i++ )
	{
		m_pParticles[i]->Die();
	}
}


void qxEffectParticleChamber::TranslateAllParticles(geVec3d* pV )
{
	// update all particles that we own
	for ( int i=0; i < ParticlesMax; i++ )
	{
		if ( m_pParticles[i]->m_fAge < 0.0f )
			continue;

		geVec3d_Add(	(geVec3d *)&( m_pParticles[i]->m_vVertex.X ),
						pV, (geVec3d *)&( m_pParticles[i]->m_vVertex.X ) );
	}
}


void qxEffectParticleChamber::KillAgedParticles( float fAge )
{
	m_nParticlesAlive = 0;

	// update all particles that we own
	for ( int i=0; i < ParticlesMax; i++ )
	{
		if ( m_pParticles[i]->m_fAge < 0.0f )
			continue;

		if ( m_pParticles[i]->m_fLifetime - m_pParticles[i]->m_fAge < fAge  )
		{
			m_pParticles[i]->Die();
			continue;
		}

		m_nParticlesAlive++;
	}
}

//
// Helper to kill distant invisible particles
//
void qxEffectParticleChamber::KillInvisibleParticles( float fDistThreshold )
{
	m_nParticlesAlive = 0;

	// update all particles that we own
	for ( int i=0; i < ParticlesMax; i++ )
	{
		if ( m_pParticles[i]->m_fAge < 0.0f )
			continue;

		geVec3d* p = (geVec3d*)&(m_pParticles[i]->m_vVertex.X);

		geVec3d thePosition;
		CCD->CameraManager()->GetPosition(&thePosition);
		float fDistSq = geVec3d_DistanceBetweenSquared(p, &thePosition);

		if ( fDistSq >  fDistThreshold*fDistThreshold )
		{
			m_pParticles[i]->Die();
			//QXSTR("DIE");
			continue;
		}

		m_nParticlesAlive++;
	}
}


void qxEffectParticleChamber::InitQuad()
{
	QXASSERT(QuadRotateDegreesMin.X <= QuadRotateDegreesMax.X);
	QXASSERT(QuadRotateDegreesMin.Y <= QuadRotateDegreesMax.Y);
	QXASSERT(QuadRotateDegreesMin.Z <= QuadRotateDegreesMax.Z);

	// Convert degrees to rads
	m_vQuadRotateRadsMin.X = RADIANS(QuadRotateDegreesMin.X);
	m_vQuadRotateRadsMin.Y = RADIANS(QuadRotateDegreesMin.Y);
	m_vQuadRotateRadsMin.Z = RADIANS(QuadRotateDegreesMin.Z);
	m_vQuadRotateRadsMax.X = RADIANS(QuadRotateDegreesMax.X);
	m_vQuadRotateRadsMax.Y = RADIANS(QuadRotateDegreesMax.Y);
	m_vQuadRotateRadsMax.Z = RADIANS(QuadRotateDegreesMax.Z);

	if( !geVec3d_IsZero(&QuadRandInitialRotateDegreesMax ) ||
		!geVec3d_IsZero(&QuadRandInitialRotateDegreesMin ) )
	{
		m_bQuadRandomInitialRotate = true;

		m_vQuadInitialRotateRadsMax.X = RADIANS(QuadRandInitialRotateDegreesMax.X);
		m_vQuadInitialRotateRadsMax.Y = RADIANS(QuadRandInitialRotateDegreesMax.Y);
		m_vQuadInitialRotateRadsMax.Z = RADIANS(QuadRandInitialRotateDegreesMax.Z);
		m_vQuadInitialRotateRadsMin.X = RADIANS(QuadRandInitialRotateDegreesMin.X);
		m_vQuadInitialRotateRadsMin.Y = RADIANS(QuadRandInitialRotateDegreesMin.Y);
		m_vQuadInitialRotateRadsMin.Z = RADIANS(QuadRandInitialRotateDegreesMin.Z);

		QXASSERT(m_vQuadInitialRotateRadsMin.X <= m_vQuadInitialRotateRadsMax.X);
		QXASSERT(m_vQuadInitialRotateRadsMin.Y <= m_vQuadInitialRotateRadsMax.Y);
		QXASSERT(m_vQuadInitialRotateRadsMin.Z <= m_vQuadInitialRotateRadsMax.Z);
	}
}
