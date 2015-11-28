/************************************************************************************//**
 * @file CDecal.cpp
 * @brief Decal class implementation
 *
 * This file contains the class implementation for Decal handling.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

//	Include the One True Header
#include "RabidFramework.h"

extern geBitmap *TPool_Bitmap(char *DefaultBmp, char *DefaultAlpha, char *BName, char *AName);

/* ------------------------------------------------------------------------------------ */
//	Constructor
//
//	Load up all Decals and set the entities to default values.
/* ------------------------------------------------------------------------------------ */
CDecal::CDecal()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	Bottom = (Decal*)NULL;

	// Ok, check to see if there are Decals in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "DecalDefine");

	if(!pSet)
		return;

	// Ok, we have Decals somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		DecalDefine *pSource = (DecalDefine*)geEntity_GetUserData(pEntity);

		pSource->Bitmap = TPool_Bitmap(pSource->BmpName, pSource->AlphaName, NULL, NULL);

		if(!(pSource->Bitmap))
		{
			char szBug[256];
          	sprintf(szBug, "*WARNING* File %s - Line %d: CreateFromFileAndAlphaNames failed: %s, %s\n",
					__FILE__, __LINE__, pSource->BmpName, pSource->AlphaName);
		  	CCD->ReportError(szBug, false);
		}
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
//
//	Clean up.
/* ------------------------------------------------------------------------------------ */
CDecal::~CDecal()
{
	Decal *d, *next;

	d = Bottom;

	while(d != NULL)
	{
		next = d->prev;
		delete d;
		d = next;
	}

	Bottom = (Decal*)NULL;
}

/* ------------------------------------------------------------------------------------ */
//	Tick
/* ------------------------------------------------------------------------------------ */
void CDecal::Tick(float dwTicks)
{
	Decal *d, *next;

	d = Bottom;

	while(d != NULL)
	{
		next = d->prev;
		d->TimeToLive -= dwTicks;

		if(d->TimeToLive > 0.0f)
		{
			if(geWorld_MightSeeLeaf(CCD->World(), d->Leaf) == GE_TRUE)
			{
				geWorld_AddPolyOnce(CCD->World(),
									d->vertex,
									4,
									d->Bitmap,
									GE_TEXTURED_POLY,
									GE_RENDER_DEPTH_SORT_BF ,
									1.0f);
			}
		}
		else
		{
			if(Bottom == d)
				Bottom = d->prev;

			if(d->prev != NULL)
				d->prev->next = d->next;

			if(d->next != NULL)
				d->next->prev = d->prev;

			delete d;
			d = NULL;
		}

		d = next;
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
//	AddDecal
/* ------------------------------------------------------------------------------------ */
void CDecal::AddDecal(int type, geVec3d *impact, geVec3d *normal)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	Decal *d;
	geVec3d right, up;

	// Ok, check to see if there are Decals in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "DecalDefine");

	if(!pSet)
		return;

	// Ok, we have Decals somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		DecalDefine *pSource = (DecalDefine*)geEntity_GetUserData(pEntity);

		if(pSource->Type == type)
		{
			if (!(pSource->Bitmap))
				return;

			d = new Decal;
			d->Bitmap = pSource->Bitmap;
			d->TimeToLive = pSource->LifeTime*1000.0f;

			//Setup vertex 1,2,3,4
			for(int i=0; i<4; i++)
			{
				// texture coordinates
				d->vertex[i].u = 0.0f;
				d->vertex[i].v = 0.0f;
				// color
				d->vertex[i].r = pSource->Color.r;	//red
				d->vertex[i].g = pSource->Color.g;	//green
				d->vertex[i].b = pSource->Color.b;	//blue
				d->vertex[i].a = pSource->Alpha;	//alpha
			}

			d->vertex[3].u = 1.0f;
			d->vertex[2].u = 1.0f;
			d->vertex[2].v = 1.0f;
			d->vertex[1].v = 1.0f;

			geVec3d Axis[3] =
			{
				{1.0f, 0.0f, 0.0f},
				{0.0f, 1.0f, 0.0f},
				{0.0f, 0.0f, 1.0f}
			};

			int major = 0;

			#define fab(a) (a > 0 ? a : -a)

			if(fab(normal->Y) > fab(normal->X))
			{
				major = 1;

				if(fab(normal->Z) > fab(normal->Y))
					major = 2;
			}
			else
			{
				if(fab(normal->Z) > fab(normal->X))
					major = 2;
			}

			if(fab(normal->X) == 1.0f || fab(normal->Y) == 1.0f || fab(normal->Z) == 1.0f)
			{
				if((major == 0 && normal->X > 0.0f) || major == 1)
				{
					right.X = 0.0f;
					right.Y = 0.0f;
					right.Z = -1.0f;
				}
				else if(major == 0)
				{
					right.X = 0.0f;
					right.Y = 0.0f;
					right.Z = 1.0f;
				}
				else
				{
					right.X = normal->Z;
					right.Y = 0.0f;
					right.Z = 0.0f;
				}
			}
			else
				geVec3d_CrossProduct(&Axis[major], normal, &right);

			geVec3d_CrossProduct(normal, &right, &up);
			geVec3d_Normalize(&up);
			geVec3d_Normalize(&right);

			geVec3d_Scale(&right, pSource->Width*0.5f, &right);
			geVec3d_Scale(&up, pSource->Height*0.5f, &up);

			geVec3d_MA(impact, 0.1f, normal, impact);

			//calculate vertices from corners
			d->vertex[1].X = impact->X + ((-right.X - up.X));
			d->vertex[1].Y = impact->Y + ((-right.Y - up.Y));
			d->vertex[1].Z = impact->Z + ((-right.Z - up.Z));

			d->vertex[2].X = impact->X + ((right.X - up.X));
			d->vertex[2].Y = impact->Y + ((right.Y - up.Y));
			d->vertex[2].Z = impact->Z + ((right.Z - up.Z));

			d->vertex[3].X = impact->X + ((right.X + up.X));
			d->vertex[3].Y = impact->Y + ((right.Y + up.Y));
			d->vertex[3].Z = impact->Z + ((right.Z + up.Z));

			d->vertex[0].X = impact->X + ((-right.X + up.X));
			d->vertex[0].Y = impact->Y + ((-right.Y + up.Y));
			d->vertex[0].Z = impact->Z + ((-right.Z + up.Z));

			d->next = NULL;
			d->prev = Bottom;

			if(Bottom != NULL)
				Bottom->next = d;

			Bottom = d;

			geWorld_GetLeaf(CCD->World(), impact, &d->Leaf);
			return;
		}
	}

	return;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
