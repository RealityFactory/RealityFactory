// qxEffectBase.cpp: implementation of the qxEffectBase class.
//
// Author: Jeff Thelen
//
// Copyright 1999 Quixotic, Inc.  All Rights Reserved.
//////////////////////////////////////////////////////////////////////
#include "..\\RabidFramework.h"
#include "qxEffectBase.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern geBitmap *TPool_Bitmap(const char *DefaultBmp, const char *DefaultAlpha,
							  const char *BName, const char *AName);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

qxEffectBase::qxEffectBase(char* strName)
:m_strName(strName)
,m_bPaused(false)
,m_fPauseTime(0.0)
,m_fTimeLeft(0.0)
,m_pBmp(0)
,m_pActor(0)
,m_bTriggered(false)
,m_bSelfDelete(false)
,m_bActive(true)
,m_nVisibilityFlags( EFFECTC_CLIP_LEAF | EFFECTC_CLIP_SEMICIRCLE )

//QXVARIABLES
,ActorName(0)
,BoneName(NULL)
,PauseTimeMin(0.0f)
,PauseTimeMax(0.0f)
,TotalTime(0.0f)
,DistanceMin(500.0f)
,DistanceMax(1000.0f)
,SoundFile(NULL)
,SoundMinDistance(0.0f)
//QXVARIABLES
{
	geVec3d_Clear(&Origin);
}

qxEffectBase::~qxEffectBase()
{
}

bool qxEffectBase::Init()
{
	QXASSERT( DistanceMax >= 0.0f);
	QXASSERT( DistanceMin >= 0.0f);
	QXASSERT( DistanceMin <= DistanceMax);
	QXASSERT( PauseTimeMin <= PauseTimeMax);

	if ( PauseTimeMax > 0.0f )
	{
		m_fPauseTime = Frand( PauseTimeMin, PauseTimeMax );
	}


	m_fTimeLeft = TotalTime;

	AttachActor(ActorName);

	geWorld_GetLeaf(CCD->Engine()->World(), &Origin, &m_nLeaf );


	return true;
}


bool qxEffectBase::TriggerStop()
{
	return true;
}

void qxEffectBase::SetActive(bool b)
{
	m_bActive = b;
}

void qxEffectBase::SetPause(bool b)
{
	m_bPaused = b;
}

bool qxEffectBase::TestTrigger()
{
	return true;
}

//
// Common frame functions.
// Returns: 0 to kill effect immediately
//			1 to return from derived class function
//			-1 to continue in derived class function
//
int qxEffectBase::Frame()
{
	float fTime = CCD->LastElapsedTime_F()*0.001f;

	// kill the effect if its time has run out
	if ( m_fTimeLeft > 0.0f )
	{
		m_fTimeLeft -= fTime;
		if ( m_fTimeLeft <= 0.0f )
		{
			m_fTimeLeft = 0.0f;
			SetActive(false);
			return 0;
		}
	}

	// adjust pause time
	if ( m_fPauseTime > 0.0f )
	{
		m_fPauseTime -=  fTime;
		if ( m_fPauseTime <= 0.0f )
		{
			m_fPauseTime = Frand( PauseTimeMin, PauseTimeMax );
			SetPause(!m_bPaused);
		}
	}

	if(m_bPaused)
		return 1;

	return -1;
}


//
// The base effect has no destination. This is virtual, so those
// effects with destinations can handle it.
//
void  qxEffectBase::SetOriginAndDest(geVec3d* pOrig, geVec3d* /*pDest*/)
{
	Origin = *pOrig;
	geWorld_GetLeaf(CCD->Engine()->World(), &Origin, &m_nLeaf);
}



bool qxEffectBase::ActorFrame( )
{
	// adjust position if it's hooked to an actor
	if ( m_pActor )
	{
		geXForm3d	XForm;

		// get bone location
		if(!geActor_GetBoneTransform( m_pActor, BoneName, &XForm ))
			return 0; //Error

		geVec3d_Copy( &XForm.Translation, &Origin);

		if(!geWorld_GetLeaf(CCD->Engine()->World(), &Origin, &m_nLeaf ))
			return 0;//Error

	}//if actor attached


	return true; //Carry on
}

void qxEffectBase::SoundFrame()
{
}

//Return visible distance between camera and origin, or -1.0f if not visible
float qxEffectBase::CheckVisibleDistance()
{
	float fDistance = 0.0f;

	if(DistanceMax > 0.0f)
	{
		// do nothing if it's too far away
		geVec3d thePosition;
		CCD->CameraManager()->GetPosition(&thePosition);
		fDistance = geVec3d_DistanceBetween( &Origin, &thePosition);

		if ( fDistance > DistanceMax )
		{
			return -1.0f;
		}
	}

	return fDistance;
}



void qxEffectBase::AttachTrigger(char* /*pTriggerName*/)
{
}


bool qxEffectBase::AttachWorldBmp(char* pBmpName)
{
	QXASSERT(!EffectC_IsStringNull(pBmpName));

	//Genesis doesn't want extension
	std::string s = pBmpName;
	if(s.find(".bmp") != -1) s.erase(s.find(".bmp"));
	m_pBmp = geWorld_GetBitmapByName(CCD->Engine()->World(), s.c_str());
	QXASSERT(m_pBmp);

	return true;
}

void qxEffectBase::AttachBmp(char* pBmpName, char* pAlphaName)
{
	if( EffectC_IsStringNull( pBmpName ) )
		return;

	m_pBmp = TPool_Bitmap(pBmpName, pAlphaName, NULL, NULL);
}


bool qxEffectBase::AttachActor(char* /*pActorName*/)
{
	return true;
}

bool qxEffectBase::AttachActorBmp(char* pBmpName)
{
	QXASSERT(m_pActor);

	geActor_Def	*ActorDef;
	geBody		*Body;
	int			MaterialCount;
	int			i;
	const char	*MaterialName;
	float		R, G, B;

	// get actor material count
	ActorDef = geActor_GetActorDef(m_pActor);
	QXASSERT(ActorDef);

	Body = geActor_GetBody( ActorDef );
	QXASSERT(Body);

	MaterialCount = geActor_GetMaterialCount( m_pActor );

	// get bitmap pointer
	for ( i = 0; i < MaterialCount; i++ )
	{
		if ( !geBody_GetMaterial( Body, i, &MaterialName, &m_pBmp, &R, &G, &B ))
			continue;

		if ( !strnicmp( pBmpName, MaterialName, strlen( pBmpName ) ))
			break;
	}
	if ( i == MaterialCount )
	{
		return false;
	}

	return true;
}
