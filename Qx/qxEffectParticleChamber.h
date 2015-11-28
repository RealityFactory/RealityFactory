
//////////////////////////////////////////////////////////////////////
// qxEffectParticleChamber.h: interface for the qxEffectParticleChamber class.
//
// Author: Jeff Thelen
//
// Copyright 2000 Quixotic, Inc.  All Rights Reserved.
//////////////////////////////////////////////////////////////////////

#ifndef qxEffectParticleChamber_H
#define qxEffectParticleChamber_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "qxEffectBase.h"

#define MAX_PARTICLES 1000

class qxParticleBase;

class qxEffectParticleChamber  : public qxEffectBase
{

public:

	qxEffectParticleChamber(char* strName, void*& qxFromEditor);

	virtual ~qxEffectParticleChamber();

	virtual int Frame();
	virtual void Draw() { };

	static const char* GetQXTypeName() { return "qxEffectParticleChamber"; }

	virtual void SetOriginAndDest(geVec3d*, geVec3d* );

	// For randomizing bitmaps
	void AddBmp(const char* pBmp, const char* pAlpha);

	void SetAnimationMode();

	virtual bool		Init();

	// some classes may not want the original origin
	virtual geVec3d*	GetOrigin() { return &Origin; }

	int				GetParticlesAlive() { 	return m_nParticlesAlive; }

	bool			SetAngles( geVec3d* pAngles );


	virtual bool	ReInit();

	virtual void	KillInvisibleParticles( float fDistance );

	void			SetRenderStyle(int n) { m_nRenderStyle = n; }

	void			TranslateAllParticles(geVec3d* pV );

	void			KillAgedParticles( float fAge );


	// Array for storing random bitmaps or animation bitmaps
	std::vector<geBitmap*> m_BmpArray;

protected:


	void		LoadAnimation();

	void		InitQuad();

	void		KillAllParticles();

	geXForm3d	m_matXForm;

	GE_LVertex	m_Vertex;

	geVec3d		m_vDest;

	float		m_fTimeElapsed;

	// Animation mode allows for the particle's bmp to be animated
	bool		m_bAnimationMode;

	//For quads, store the rotation radians Min and Max
	geVec3d		m_vQuadRotateRadsMin;
	geVec3d		m_vQuadRotateRadsMax;

	//For quads, the random initial rotation max and min from player's view
	geVec3d		m_vQuadInitialRotateRadsMax;
	geVec3d		m_vQuadInitialRotateRadsMin;
	geVec3d		m_vQuadInitialRotateRads;

	//For quads, do we want to apply a random initial rotation?
	bool m_bQuadRandomInitialRotate;

	int		m_nParticlesAlive;

	// this is a value that allows us to emit very precise amounts of particles.
	// what if we want 0.5 particles per second ?
	// 0.5 is not a full particle ( we don't round up ) so we can't emit one yet
	// so we remember what we wanted to emit and add that next time we
	// try to emit some more particles.
	// Think about a faucet that is leaking very slowly.  The drop slowly gets
	// bigger and bigger till there is enough mass to allow
	// it to break free.
	float		m_fEmissionResidue;

	// array of particles.
	std::vector<qxParticleBase*> m_pParticles;

	bool		m_bUseGravity;
	bool		m_bUseVelocity;

	float		m_fAlphaTempEnd;

	int			m_nRenderStyle;

public:

	geVec3d		Angles;
	geBoolean	AnglesRandom;

	float		ParticlesPerSec;

	float		SizeVariation;
	float		SizeStart;
	float		SizeEnd;
	float		SpeedMin;
	float		SpeedMax;
	float		UnitLifeMin;
	float		UnitLifeMax;
	int			VarianceSource;
	int			VarianceDest;
	GE_RGBA		ColorStart;
	GE_RGBA		ColorEnd;
	geVec3d		Gravity;
	char		*BmpName;
	char		*BmpAlphaName;
	float		AlphaStart;
	float		AlphaEnd;

	geBoolean	RandomGravity;
	geVec3d		RandomGravityMin;
	geVec3d		RandomGravityMax;

	char		*AnimateBmpPrefix;
	geBoolean	AnimateLoop;
	geBoolean   AnimateKillNoLoop;
	int			AnimateFPS;

	geBoolean	Quad;
	geVec3d		QuadRotateDegreesMin;
	geVec3d		QuadRotateDegreesMax;
	geVec3d		QuadRandInitialRotateDegreesMax;
	geVec3d		QuadRandInitialRotateDegreesMin;

	float		AnglesRandomYaw;
	float		AnglesRandomPitch;
	int			ParticlesMax;
	geBoolean	AlphaFadeInFadeOut;

};


#endif// qxEffectParticleChamber_H
