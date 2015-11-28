/************************************************************************************//**
 * @file CPolyShadow.cpp
 * @brief CPolyShadow class implementation
 * @author Dan Valeo
 *//*
 * Copyright (c) 2004 Dan Valeo; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"
#include <body._h>

/* ------------------------------------------------------------------------------------ */
//	Constructor
/* ------------------------------------------------------------------------------------ */
CPolyShadow::CPolyShadow()
{
	// set defaults
	m_Vertex[0].u = m_Vertex[1].u = m_Vertex[2].u = 0.f;
	m_Vertex[0].v = m_Vertex[1].v = m_Vertex[2].v = 0.f;
	m_Vertex[0].r = m_Vertex[1].r = m_Vertex[2].r = 0.f;
	m_Vertex[0].g = m_Vertex[1].g = m_Vertex[2].g = 0.f;
	m_Vertex[0].b = m_Vertex[1].b = m_Vertex[2].b = 0.f;
	m_Vertex[0].a = m_Vertex[1].a = m_Vertex[2].a = 255.f;
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
/* ------------------------------------------------------------------------------------ */
CPolyShadow::~CPolyShadow()
{
}

/* ------------------------------------------------------------------------------------ */
//	DrawShadow
/* ------------------------------------------------------------------------------------ */
bool CPolyShadow::DrawShadow(const geActor *Actor)
{
	int NumFace;
	int Verts;
	int Normals;
	geVec3d Point;
	GE_Collision Collision;
	int fc,x,i;
	geBody_Index BoneIndex;
	geXForm3d Transform;
	geBody *pBody;
	float scale;
	geVec3d Pos,NewPos;
	geVec3d LightRot, NewLPos;


	// get actor data
	pBody = geActor_GetBody(geActor_GetActorDef(Actor));
	geBody_GetGeometryStats(pBody, 0, &Verts, &NumFace, &Normals);
	CCD->ActorManager()->GetScale(Actor, &scale);
	CCD->ActorManager()->GetPosition(Actor, &Pos);

	// set default light source position
	NewLPos = Pos;
	NewLPos.Y += 512.f;
	NewLPos.Z += 256.f;

	// set collision position
	NewPos = Pos;
	NewPos.Y -= 512.f;

	// changed QD 12/15/05 - remove PathFollower dependency
	// get the angle from the light source to the actor - Cheat a bit. Should be done for every vertex.
	geVec3d_Subtract(&Pos, &NewLPos, &LightRot);
	geVec3d_Normalize(&LightRot);
	// end change

	// Get ground plane and distance
	BOOL Result = geWorld_Collision(CCD->Engine()->World(), NULL, NULL, &Pos, &NewPos,
			GE_CONTENTS_SOLID_CLIP , GE_COLLIDE_MESHES | GE_COLLIDE_MODELS, 0,
			NULL, NULL, &Collision);

	// draw shadow
	// cycle through faces
	for(i=0; i<NumFace; i++)
	{
		// cycle through the vertices of each face
		for(x=0; x<3; x++)
		{
			// get vertex pos and bone index for vertex
			fc = pBody->SkinFaces[0].FaceArray[i].VtxIndex[x];
			geVec3d_Copy(&(pBody->XSkinVertexArray[fc].XPoint), &(Point));
			BoneIndex = pBody->XSkinVertexArray[fc].BoneIndex;

			// scale the vertices by the RF defined scale
			geVec3d_Scale(&Point, scale, &Point);

			// transform the vertex by the bone position
			geActor_GetBoneTransformByIndex(Actor, BoneIndex, &Transform);
			geXForm3d_Transform(&Transform, &Point, &Point);

			// draw shadow against floor plane
			float t;
			geVec3d Intersection;
			t = (Collision.Plane.Dist - geVec3d_DotProduct(&Collision.Plane.Normal,&Point))/geVec3d_DotProduct(&Collision.Plane.Normal,&LightRot);
			geVec3d_AddScaled(&Point, &LightRot, t, &Intersection);
			m_Vertex[x].X = Intersection.X + Collision.Plane.Normal.X;
			m_Vertex[x].Y = Intersection.Y + Collision.Plane.Normal.Y;
			m_Vertex[x].Z = Intersection.Z + Collision.Plane.Normal.Z;
		}

		// add poly
		geWorld_AddPolyOnce(CCD->Engine()->World(), m_Vertex, 3, NULL, GE_GOURAUD_POLY,
				GE_RENDER_DO_NOT_OCCLUDE_OTHERS|GE_RENDER_DO_NOT_OCCLUDE_SELF, scale);
	}

	return true;
}


/* ----------------------------------- END OF FILE ------------------------------------ */
