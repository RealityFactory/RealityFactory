//////////////////////////////////////////////////////////////////////////////
//
// This file contains code CONFIDENTIAL to Quixotic, Inc.
// Use or disclosure without permission is prohibited.
//
// Copyright 1999 Quixotic, Inc.  All Rights Reserved.
//
//////////////////////////////////////////////////////////////////////
//
//        FILE: qxParticle.cpp
//
//      AUTHOR: Jeff Thelen
//
//////////////////////////////////////////////////////////////////////////////

#include "..\\RabidFramework.h"
#include "qxParticle.h"
#include "QxUser.h"	//Genesis


qxParticleBase::qxParticleBase()
:
m_pPoly(0)
,m_fAge(-1)
,m_fLifetime(0.0f)
,m_fSize(0.0f)
,m_fSizeDelta(0.0f)
,m_nFlags(0)
,m_pParent(0)
,m_fAlphaEnd(0.0f)
{
	geVec3d_Clear((geVec3d*) &(m_vVertex.X));
	geVec3d_Clear(&m_vGravity);
	geVec3d_Clear(&m_vVelocity);
	m_vVertex.u = 0.0f;
	m_vVertex.v = 0.0f;
	m_vVertex.r = 255.0f;
	m_vVertex.g = 255.0f;
	m_vVertex.b = 255.0f;
	m_vVertex.a = 255.0f;
}

qxParticleQuad::qxParticleQuad()
:qxParticleBase()
{
	geVec3d_Clear(&m_vRotateRads);
	geXForm3d_SetIdentity(&m_matLocal);

	for(int i = 0; i < 4; i++)
	{
		geVec3d_Clear( (geVec3d*) &(m_vQuadVerts[i].X) );
		m_vQuadVerts[i].r = 255.0f;
		m_vQuadVerts[i].g = 255.0f;
		m_vQuadVerts[i].b = 255.0f;
		m_vQuadVerts[i].a = 255.0f;
	}
	m_vQuadVerts[0].u = 0.0f;
	m_vQuadVerts[0].v = 0.0f;
	m_vQuadVerts[1].u = 1.0f;
	m_vQuadVerts[1].v = 0.0f;
	m_vQuadVerts[2].u = 1.0f;
	m_vQuadVerts[2].v = 1.0f;
	m_vQuadVerts[3].u = 0.0f;
	m_vQuadVerts[3].v = 1.0f;
}

qxParticleBase::~qxParticleBase()
{
	Die();
}


void qxParticleQuad::TranslateQuad( float fElapsedTime )
{
	geVec3d Up, Left;
	float HalfWidth =  geBitmap_Width( m_pPoly->Bitmap ) * m_fSize * 0.5f;
	float HalfHeight = geBitmap_Height( m_pPoly->Bitmap ) * m_fSize * 0.5f;

	// Build rotation matrix
	if( !geVec3d_IsZero(&m_vRotateRads) )
	{

		geXForm3d matRotate;
		geXForm3d_SetIdentity(&matRotate);

		if( fabs(m_vRotateRads.X) > 0.0f)
			geXForm3d_RotateX(&matRotate, m_vRotateRads.X*fElapsedTime);
		if( fabs(m_vRotateRads.Y) > 0.0f)
			geXForm3d_RotateY(&matRotate, m_vRotateRads.Y*fElapsedTime);
		if( fabs(m_vRotateRads.Z) > 0.0f)
			geXForm3d_RotateZ(&matRotate, m_vRotateRads.Z*fElapsedTime);

		geXForm3d_Multiply(&m_matLocal, &matRotate, &m_matLocal);
	}


	geXForm3d_GetLeft( &m_matLocal, &Left );
	geXForm3d_GetUp( &m_matLocal, &Up );

	geVec3d_Scale( &Left, HalfWidth, &Left );
	geVec3d_Scale( &Up, HalfHeight, &Up );

	m_vQuadVerts[0].X = m_vVertex.X + Left.X + Up.X;
	m_vQuadVerts[0].Y = m_vVertex.Y + Left.Y + Up.Y;
	m_vQuadVerts[0].Z = m_vVertex.Z + Left.Z + Up.Z;
	m_vQuadVerts[1].X = m_vVertex.X - Left.X + Up.X;
	m_vQuadVerts[1].Y = m_vVertex.Y - Left.Y + Up.Y;
	m_vQuadVerts[1].Z = m_vVertex.Z - Left.Z + Up.Z;
	m_vQuadVerts[2].X = m_vVertex.X - Left.X - Up.X;
	m_vQuadVerts[2].Y = m_vVertex.Y - Left.Y - Up.Y;
	m_vQuadVerts[2].Z = m_vVertex.Z - Left.Z - Up.Z;
	m_vQuadVerts[3].X = m_vVertex.X + Left.X - Up.X;
	m_vQuadVerts[3].Y = m_vVertex.Y + Left.Y - Up.Y;
	m_vQuadVerts[3].Z = m_vVertex.Z + Left.Z - Up.Z;
}

