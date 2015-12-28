//////////////////////////////////////////////////////////////////////
// qxEffectBase.h: interface for the qxEffectBase class.
//
// Author: Jeff Thelen
//
// Copyright 2000 Quixotic, Inc.  All Rights Reserved.
//////////////////////////////////////////////////////////////////////

#ifndef QX_EFFECTBASE_H
#define QX_EFFECTBASE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define EFFECT_FRAMES_PER_SEC 10 //per sec


enum TRIGGER_FLAG
{
	NO_TRIGGER,
	NOT_TRIGGERED,
	START_TRIGGERED,
	STOP_TRIGGERED
};


class qxEffectBase
{
public:

	qxEffectBase() {}
	qxEffectBase(char* strName);
	virtual			~qxEffectBase();
	virtual int		Frame();
	virtual void	Draw() {}
	virtual bool	Init();


	std::string&	GetName()	{ return m_strName; }
	geBitmap*		GetBitmap() { return m_pBmp; }

	virtual void	SetPause(bool b);
	virtual void	SetActive(bool b);
	void			SetTotalTime(float f)	{ TotalTime = f; m_fTimeLeft = f; }
	void			SetTimeLeft(float f)	{ m_fTimeLeft = f; }

	// Not all effects have a destination, but all have an origin.
	virtual void	SetOriginAndDest(geVec3d* pOrig, geVec3d* pDest);

	bool			GetPaused() { return m_bPaused; }
	void			SetVisibilityFlags(int n) { m_nVisibilityFlags = n; }

protected:

	float			CheckVisibleDistance();
	bool			ActorFrame();
	inline bool		IsVisible();
	void			SoundFrame();

	bool			AttachActor(char*);
	bool			AttachActorBmp(char* pBmpName);
	bool			AttachWorldBmp(char* pBmpName);
	void			AttachBmp(char* pBmpName, char* pAlphaName);
	void			AttachTrigger(char*);
	bool			TestTrigger();

	virtual bool	TriggerStart(){return true;}
	virtual bool	TriggerStop();
	virtual bool	TriggerFrame(){ return m_bTriggered;}


	bool			m_bPaused;
	bool			m_bSelfDelete;

	std::string		m_strName;
	bool			m_bTriggered;
	geBitmap*		m_pBmp;

	int32			m_nLeaf;
	geActor	*		m_pActor;

	bool			m_bActive;

	float			m_fTimeLeft;
	float			m_fPauseTime;

	int				m_nVisibilityFlags;

public:
	geVec3d		Origin;
	char		*ActorName;
	char		*BoneName;
	float		PauseTimeMin;
	float		PauseTimeMax;
	float		TotalTime;
	float		DistanceMin;
	float		DistanceMax;
	char		*SoundFile;
	float		SoundMinDistance;

};

inline bool qxEffectBase::IsVisible()
{
	// do nothing if it isn't visible
	if ( !EffectC_IsPointVisible(CCD->Engine()->World(),
							CCD->CameraManager()->Camera(),
									&Origin,
									m_nLeaf,
									m_nVisibilityFlags ) )
	{
		return false;
	}

	return true;
}

#endif // !defined(QX_EFFECTBASE_H)
