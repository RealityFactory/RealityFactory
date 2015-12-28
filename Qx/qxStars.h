// qxStars.h:
//
// Author: Jeff Thelen
//
// Copyright 2000 Quixotic, Inc.  All Rights Reserved.
//////////////////////////////////////////////////////////////////////

#ifndef qxStars_H
#define qxStars_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "qxEffectParticleChamber.h"


#define NUM_STARS 80

class qxStar
{
public:

	qxStar();
	~qxStar();

	gePoly*		m_pPoly;
	float		m_fScale;
	GE_RGBA		m_Color;
	geVec3d		m_vOrigin;
};


class qxStarField
{
public:

	qxStarField();
	~qxStarField();

	bool	Init();

	int		Frame();

	//
	// Attributes
	//

protected:

	bool InitStarsMajor();

	qxStar m_Stars[NUM_STARS];
	qxStar* m_pStarSparkle;
	float	m_fStarSparkleAlpha;
	float	m_fStarSparkleDir;
	float	m_fStarSparkleTime;

	geXForm3d	m_matLocal;
	void GetRandomStarColor(GE_RGBA* p);
};


#endif
