/*
	CWallDecal.cpp:		Decal class implementation

	(c) 2000 Ralph Deane
	All Rights Reserved

	This file contains the class implementation for WallDecal
handling.
*/

//	Include the One True Header

#include "RabidFramework.h"

extern geBitmap *TPool_Bitmap(char *DefaultBmp, char *DefaultAlpha, char *BName, char *AName);

//	Constructor
//
//	Load up all Decals and set the
//	..entities to default values.

CWallDecal::CWallDecal()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

//	Ok, check to see if there are Decals in this world

	pSet = geWorld_GetEntitySet(CCD->World(), "WallDecal");

	if(!pSet)
		return;	

//	Ok, we have Decals somewhere.  Dig through 'em all.

	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		WallDecal *pSource = (WallDecal*)geEntity_GetUserData(pEntity);

		pSource->Bitmap = TPool_Bitmap(pSource->BmpName, pSource->AlphaName, NULL, NULL);

		pSource->active = GE_FALSE;
		if(EffectC_IsStringNull(pSource->TriggerName))
		{
			AddDecal(pSource);
			pSource->active = GE_TRUE;
		}
	}

  return;
}


//	Destructor
//
//	Clean up.

CWallDecal::~CWallDecal()
{

}

void CWallDecal::Tick()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

//	Ok, check to see if there are Decals in this world

	pSet = geWorld_GetEntitySet(CCD->World(), "WallDecal");

	if(!pSet)
		return;	

//	Ok, we have Decals somewhere.  Dig through 'em all.

	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		WallDecal *pSource = (WallDecal*)geEntity_GetUserData(pEntity);

		if(!EffectC_IsStringNull(pSource->TriggerName))
		{
			if(GetTriggerState(pSource->TriggerName))
			{
				if(pSource->active==GE_FALSE)
				{
					AddDecal(pSource);
					pSource->active = GE_TRUE;
				}
			}
			else
			{
				if(pSource->active==GE_TRUE)
				{
					geWorld_RemovePoly(CCD->World(), pSource->Poly); 
					pSource->active = GE_FALSE;
				}
			}
		}
	}
}

void CWallDecal::AddDecal(WallDecal *pSource)
{
	geVec3d right, up;
	GE_LVertex	vertex[4];
	
	//Setup vertex 1,2,3,4
	for(int i = 0; i < 4; i++)
	{
		// texture coordinates
		vertex[i].u = 0.0f;
		vertex[i].v = 0.0f;
		// color
		vertex[i].r = pSource->Color.r;	//red
		vertex[i].g = pSource->Color.g;	//green
		vertex[i].b = pSource->Color.b;	//blue
		vertex[i].a = pSource->Alpha;	//alpha
	}
	
	vertex[3].u = 1.0f;
	vertex[2].u = 1.0f;
	vertex[2].v = 1.0f;
	vertex[1].v = 1.0f;
	
	geVec3d Axis[3] =
	{
		{1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f}
	};
	
	geVec3d impact, normal, Pos, Direction;
	geVec3d Front, Back;
	GE_Collision	Collision;
	
	Direction.Z = 0.0174532925199433f*(pSource->Angle.Z);
	Direction.X = 0.0174532925199433f*(pSource->Angle.X);
	Direction.Y = 0.0174532925199433f*(pSource->Angle.Y-90.0f);
	
	geXForm3d Xf;
	geXForm3d_SetIdentity(&Xf);
	geXForm3d_RotateZ(&Xf, Direction.Z);
	geXForm3d_RotateX(&Xf, Direction.X);
	geXForm3d_RotateY(&Xf, Direction.Y);
	geXForm3d_Translate(&Xf, pSource->origin.X, pSource->origin.Y, pSource->origin.Z);
	
	Pos = Xf.Translation;
	geXForm3d_GetIn(&Xf, &Direction);
	
	geVec3d_AddScaled (&Pos, &Direction, 4000.0f, &Back);
	geVec3d_AddScaled (&Pos, &Direction, 0.0f, &Front);
	geWorld_Collision(CCD->World(), NULL, NULL, &Front, 
		&Back, GE_CONTENTS_SOLID_CLIP, GE_COLLIDE_MODELS | GE_COLLIDE_MESHES , 0, NULL, NULL, &Collision);
	impact =Collision.Impact;
	normal = Collision.Plane.Normal;
	
	int major = 0;
	
#define fab(a) (a > 0 ? a : -a)
	
	if(fab(normal.Y) > fab(normal.X))
	{
		major = 1;
		if(fab(normal.Z) > fab(normal.Y))
			major = 2;
	}
	else
	{
		if(fab(normal.Z) > fab(normal.X))
			major = 2;
	}
	
	if(fab(normal.X)==1.0f || fab(normal.Y)==1.0f || fab(normal.Z)==1.0f)
	{
		if ((major == 0 && normal.X > 0) || major == 1)
		{
			right.X = 0.0f;
			right.Y = 0.0f;
			right.Z = -1.0f;
		}
		else if (major == 0)
		{
			right.X = 0.0f;
			right.Y = 0.0f;
			right.Z = 1.0f;
		}
		else 
		{
			right.X = normal.Z;
			right.Y = 0.0f;
			right.Z = 0.0f;
		}
	}
	else 
		geVec3d_CrossProduct(&Axis[major], &normal, &right);
	
	geVec3d_CrossProduct(&normal, &right, &up);
	geVec3d_Normalize(&up);
	geVec3d_Normalize(&right);
	
	geVec3d_Scale(&right, pSource->Width/2.0f, &right); 
	geVec3d_Scale(&up, pSource->Height/2.0f, &up);
	
	geVec3d_MA(&impact, 0.1f, &normal, &impact);
	
	//calculate vertices from corners
	vertex[1].X = impact.X + ((-right.X - up.X));
	vertex[1].Y = impact.Y + ((-right.Y - up.Y));
	vertex[1].Z = impact.Z + ((-right.Z - up.Z));
	
	vertex[2].X = impact.X + ((right.X - up.X));
	vertex[2].Y = impact.Y + ((right.Y - up.Y));
	vertex[2].Z = impact.Z + ((right.Z - up.Z));
	
	vertex[3].X = impact.X + ((right.X + up.X));
	vertex[3].Y = impact.Y + ((right.Y + up.Y));
	vertex[3].Z = impact.Z + ((right.Z + up.Z));
	
	vertex[0].X = impact.X + ((-right.X + up.X));
	vertex[0].Y = impact.Y + ((-right.Y + up.Y));
	vertex[0].Z = impact.Z + ((-right.Z + up.Z));
	
	pSource->Poly = geWorld_AddPoly(CCD->World(),
						vertex,
						4,
						pSource->Bitmap,
						GE_TEXTURED_POLY,
						GE_RENDER_DO_NOT_OCCLUDE_OTHERS | GE_RENDER_DEPTH_SORT_BF ,
						1.0f); 
}

