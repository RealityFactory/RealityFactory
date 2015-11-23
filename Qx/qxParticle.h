#ifndef QX_PARTICLE_H
#define QX_PARTICLE_H


#include "..\\RabidFramework.h"
#include "QxUser.h" //genesis
#include "qxEffectParticleChamber.h"

#define	QX_PARTICLE_HAS_VELOCITY	( 1 << 1 )
#define QX_PARTICLE_HAS_GRAVITY		( 1 << 2 )
#define QX_PARTICLE_ANIMATION		( 1 << 3 )
#define QX_PARTICLE_ALPHA_FADEIN_FADEOUT ( 1<<4 )
#define QX_PARTICLE_RENDER_STYLE	( GE_RENDER_DO_NOT_OCCLUDE_OTHERS)


class qxEffectParticleChamber;

class qxParticleBase
{

public:

	qxParticleBase();
	virtual ~qxParticleBase();

	gePoly*					m_pPoly;

	GE_LVertex			 	m_vVertex;
	geVec3d					m_vVelocity;

	// Current Color and how much color to add over time
	GE_RGBA					m_ColorDelta;

	// Age of particle in seconds
	float					m_fAge;
	
	// Age at which particle dies
	float					m_fLifetime;
	
	// size of particle in world units and delta to add over time	
	float					m_fSize;
	float					m_fSizeDelta;
	float					m_fAlphaEnd;
	
	geVec3d					m_vGravity;

	int						m_nFlags;
	
	virtual bool			Frame( float fTimeDelta );
	virtual void			UpdatePoly();
	inline	void			Die();


	qxEffectParticleChamber* m_pParent;


};

//Quad version of particle
class qxParticleQuad : public qxParticleBase
{

public:

	qxParticleQuad();
	~qxParticleQuad(){};
	
	GE_LVertex				m_vQuadVerts[4];
	geVec3d					m_vRotateRads;
	geXForm3d				m_matLocal;
	
	bool Frame( float fTimeDelta);
	void TranslateQuad( float fTimeDelta );
	void UpdatePoly();

};

inline bool qxParticleQuad::Frame( float fTimeDelta )
{
	if( !qxParticleBase::Frame( fTimeDelta) )
		return false;

	TranslateQuad( fTimeDelta );
	
	
	
	return true;
}

inline bool qxParticleBase::Frame( float fTimeDelta )
{

	geVec3d		DeltaPos;

	// Some systems may have particles with no Lifetime
	// i.e. they could have to collide with terrain 
	// or they could just always be alive if the system
	// had a fixed # of particles alive

	// everything ages so add the time elapsed
	m_fAge += fTimeDelta;

	if ( m_fAge >= m_fLifetime ) 
	{
		goto Death;
		
	}

	
	// Animation
	if ( m_nFlags & QX_PARTICLE_ANIMATION )
	{
		int nBmpSize = m_pParent->m_BmpArray.GetSize();
	
		int frame = (int)(m_pParent->AnimateFPS * m_fAge);
	
		if( m_pParent->AnimateFPS > nBmpSize)
		{
			frame = frame % m_pParent->AnimateFPS;
			frame = frame % nBmpSize;
		}
		else
		{
			frame = frame % nBmpSize;	
		}

		m_pPoly->Bitmap = m_pParent->m_BmpArray[frame];
		
		if( frame == nBmpSize-1 && !m_pParent->AnimateLoop )
		{
			
			if(m_pParent->AnimateKillNoLoop)
			{
				goto Death;
			
			}

			m_nFlags &= ~(QX_PARTICLE_ANIMATION);
		}
	}

		

	geVec3d_Clear(&DeltaPos);

	// apply velocity
	if ( m_nFlags & QX_PARTICLE_HAS_VELOCITY )
	{
		geVec3d_Scale( &m_vVelocity, fTimeDelta, &DeltaPos );
	}
	
	// apply gravity
	if ( m_nFlags & QX_PARTICLE_HAS_GRAVITY )
	{
		geVec3d	Gravity;
		
		// make gravity vector
		geVec3d_Scale( &m_vGravity, fTimeDelta, &Gravity );
		
		// apply gravity to built in velocity and DeltaPos
		geVec3d_Add( &m_vVelocity, &Gravity, &m_vVelocity);
		geVec3d_Add( &DeltaPos, &Gravity, &DeltaPos );
	}


	// apply DeltaPos to particle position
	geVec3d_Add( (geVec3d *)&( m_vVertex.X ), &DeltaPos, (geVec3d *)&( m_vVertex.X ) );
	
	
	// adjust current Color from calculated Deltas and time elapsed.
	m_vVertex.r += m_ColorDelta.r * fTimeDelta;
	m_vVertex.g += m_ColorDelta.g * fTimeDelta;
	m_vVertex.b += m_ColorDelta.b * fTimeDelta;
	
	// adjust current Alpha 
	
	m_vVertex.a += m_ColorDelta.a * fTimeDelta;
	
	if( m_nFlags & QX_PARTICLE_ALPHA_FADEIN_FADEOUT)
	{
		if( m_vVertex.a > m_fAlphaEnd)
		{
			m_vVertex.a		= m_fAlphaEnd;
			m_ColorDelta.a	= -m_ColorDelta.a;
		}
	}

	// adjust current Size
	if( m_fSizeDelta != 0.0f)
	{
		m_fSize += m_fSizeDelta * fTimeDelta;
		m_pPoly->Scale = m_fSize;
	}

	return true;

Death:
		Die();
		return false;

	
}

inline void qxParticleBase::Die()
{
	m_fAge = -1.0f;
	if(m_pPoly)
	{
		geWorld_RemovePoly(CCD->Engine()->World(), m_pPoly);
		m_pPoly = 0;
	}
}

inline void qxParticleQuad::UpdatePoly()
{
	for(int i = 0; i < 4; i++)
	{
		
		m_vQuadVerts[i].r = m_vVertex.r;
		m_vQuadVerts[i].g = m_vVertex.g;
		m_vQuadVerts[i].b = m_vVertex.b;
		m_vQuadVerts[i].a = m_vVertex.a;
		
		gePoly_SetLVertex(m_pPoly, i, &m_vQuadVerts[i]);
	}
}

inline void qxParticleBase::UpdatePoly()
{
	gePoly_SetLVertex(m_pPoly, 0, &m_vVertex);
}


#endif
