// qxCloudMachine.h: 
//
// Author: Jeff Thelen
//
// Copyright 2000 Quixotic, Inc.  All Rights Reserved.
//////////////////////////////////////////////////////////////////////

#ifndef qxCloudMachine_H
#define qxCloudMachine_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "qxEffectParticleChamber.h"
#include "QxUser.h"

class qxCloudMachine : public qxEffectParticleChamber
{
public:

	qxCloudMachine();

	~qxCloudMachine();

	bool	Init();
	
	int		Frame();



	void DoWhiteFluffy();
	void DoGreyOvercast();
	void DoCloudless();
	void SetNightBlueColor();
	void Draw();
	void UpdateColors();

	//
	// Attributes
	//

	int m_nHeightMin;
	int m_nHeightMax;
		
	float		m_fTimeToKillParticles;
	float		m_fTimeToMoveOrigin;
	
	eDirection	m_eWindDir;

	// virtual override
	void KillInvisibleParticles( float fDistThreshold );


protected:

	void SetAngles();


	
};




#endif