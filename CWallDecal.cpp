/************************************************************************************//**
 * @file CWallDecal.cpp
 * @brief WallDecal class implementation
 *
 * This file contains the class implementation for WallDecal handling.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2000 Ralph Deane; All rights reserved.
 ****************************************************************************************/

//	Include the One True Header
#include "RabidFramework.h"
#include <Ram.h>
#include "CWallDecal.h"

extern geBitmap *TPool_Bitmap(const char *DefaultBmp, const char *DefaultAlpha,
							  const char *BName, const char *AName);

/* ------------------------------------------------------------------------------------ */
//	Constructor
//
//	Load up all Decals and set the	entities to default values.
/* ------------------------------------------------------------------------------------ */
CWallDecal::CWallDecal()
{
	// Ok, check to see if there are Decals in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "WallDecal");

	if(!pSet)
		return;

	geEntity *pEntity;

	// Ok, we have Decals somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		WallDecal *pSource = static_cast<WallDecal*>(geEntity_GetUserData(pEntity));

// Start Aug2003DCS - Added szEntityName
		if(EffectC_IsStringNull(pSource->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pSource->szEntityName = szName;
		}
// End Aug2003DCS

// changed RF064
		pSource->FBitmap = NULL;
		pSource->vertex = NULL;

		if(!pSource->Animated)
		{
			pSource->Bitmap = TPool_Bitmap(pSource->BmpName, pSource->AlphaName, NULL, NULL);
		}
		else
		{
			pSource->vertex = static_cast<void*>(geRam_AllocateClear(sizeof(GE_LVertex) * 4));
			pSource->FBitmap = static_cast<geBitmap**>(geRam_AllocateClear(sizeof(geBitmap*) * pSource->BitmapCount));

			for(int i=0; i<pSource->BitmapCount; ++i)
			{
				char BmpName[256];
				// build bmp and alpha names
				sprintf( BmpName, "%s%d%s", pSource->BmpName, i, ".bmp");

				if(!EffectC_IsStringNull(pSource->AlphaName))
				{
					char AlphaName[256];
					sprintf( AlphaName, "%s%d%s", pSource->AlphaName, i, ".bmp");
					pSource->FBitmap[i] = TPool_Bitmap(BmpName, AlphaName, NULL, NULL);
				}
				else
				{
					pSource->FBitmap[i] = TPool_Bitmap(BmpName, BmpName, NULL, NULL);
				}
			}

			pSource->CurTex = 0;
			pSource->Time = 0.0f;
			pSource->CycleDir = 1;
		}

		pSource->OriginOffset = pSource->origin;

		if(pSource->Model)
		{
			geVec3d ModelOrigin;
			geWorld_GetModelRotationalCenter(CCD->World(), pSource->Model, &ModelOrigin);
			geVec3d_Subtract(&(pSource->origin), &ModelOrigin, &(pSource->OriginOffset));
// Start Aug2003DCS
			geVec3d_Copy(&(pSource->Angle), &(pSource->OriginalAngle));
			geVec3d_Copy(&(pSource->Angle), &(pSource->LastAngle));
// End Aug2003DCS
		}
// end change RF064

		pSource->active = GE_FALSE;

//Start Aug2003DCS
		pSource->BmpChanged = GE_FALSE;

		if(EffectC_IsStringNull(pSource->TriggerName))
		{
			AddDecal(pSource);
			pSource->active = GE_TRUE;
			pSource->ProgrammedTrigger = GE_FALSE;
		}
//End Nov2001DCS
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
//
//	Clean up.
/* ------------------------------------------------------------------------------------ */
CWallDecal::~CWallDecal()
{
// changed RF064
	// Ok, check to see if there are Decals in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "WallDecal");

	if(!pSet)
		return;

	geEntity *pEntity;

	// Ok, we have Decals somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		WallDecal *pSource = static_cast<WallDecal*>(geEntity_GetUserData(pEntity));

		if(pSource->FBitmap)
			geRam_Free(pSource->FBitmap);

		if(pSource->vertex)
			geRam_Free(pSource->vertex);
	}
// end change RF064
}

// changed RF064
/* ------------------------------------------------------------------------------------ */
//	Tick
/* ------------------------------------------------------------------------------------ */
void CWallDecal::Tick(geFloat dwTicks)
{
	// Ok, check to see if there are Decals in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "WallDecal");

	if(!pSet)
		return;

	geEntity *pEntity;

	// Ok, we have Decals somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	    pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		WallDecal *pSource = static_cast<WallDecal*>(geEntity_GetUserData(pEntity));

// Start Aug2003DCS  Moved this section up AND modified it
// changed RF064
		if(pSource->active == GE_TRUE && pSource->Animated)
		{
			pSource->Time += dwTicks;

			if(pSource->Time >= (1000.0f/pSource->Speed))
			{
				pSource->Time = 0.0f;

				switch(pSource->Style)
				{
				case 0:
				case 5:
					break;
				case 2:
					pSource->CurTex += pSource->CycleDir;
					if(pSource->CurTex >= pSource->BitmapCount || pSource->CurTex < 0)
					{
						pSource->CycleDir = -pSource->CycleDir;
						pSource->CurTex += pSource->CycleDir;
					}
					pSource->BmpChanged = GE_TRUE;
					break;
				case 3:
					pSource->CurTex = (rand() % pSource->BitmapCount);
					pSource->BmpChanged = GE_TRUE;
					break;
				case 4:
					pSource->CurTex += 1;
					if(pSource->CurTex >= pSource->BitmapCount)
						pSource->CurTex = pSource->BitmapCount-1;
					pSource->BmpChanged = GE_TRUE;
					break;
				default:
					pSource->CurTex += 1;
					if(pSource->CurTex >= pSource->BitmapCount)
						pSource->CurTex = 0;
					pSource->BmpChanged = GE_TRUE;
					break;
				}
			}
//			geWorld_AddPolyOnce(CCD->World(),
//						(GE_LVertex *)pSource->vertex,
//						4,
//						pSource->FBitmap[pSource->CurTex],
//						GE_TEXTURED_POLY,
//						GE_RENDER_DO_NOT_OCCLUDE_OTHERS | GE_RENDER_DEPTH_SORT_BF ,
//						1.0f);
		}
// end change RF064
// End Aug2003DCS

		if(!EffectC_IsStringNull(pSource->TriggerName))
		{
// Start Aug2003DCS
			if(((strcmp(pSource->TriggerName,"*Programmed*") == 0) && pSource->ProgrammedTrigger) ||
				((strcmp(pSource->TriggerName,"*Programmed*") != 0) && GetTriggerState(pSource->TriggerName)))
			{
				if(pSource->active == GE_FALSE)
				{
					AddDecal(pSource);
					pSource->active = GE_TRUE;
				}
				else
				{
					if(pSource->active)
					{
						if(pSource->BmpChanged == GE_TRUE)
						{
							geWorld_RemovePoly(CCD->World(), pSource->Poly);
							AddDecal(pSource);
							pSource->BmpChanged = GE_FALSE;
						}
					}
				}
			}
			else
			{
				if(pSource->active == GE_TRUE)
				{
					geWorld_RemovePoly(CCD->World(), pSource->Poly);
					pSource->active = GE_FALSE;
				}
			}
// End Aug2003DCS
// changed RF064
		}
		else
		{
			if(pSource->active != GE_TRUE)
			{
				AddDecal(pSource);
				pSource->active = GE_TRUE;
			}
		}

// Start Aug2003DCS
		if(pSource->active == GE_TRUE)
		{
			if (pSource->Model)
			{
				//Move origin with the model
				geVec3d OriginalOrigin = pSource->origin;
				pSource->origin = pSource->OriginOffset;

				if(SetOriginOffset(0, 0, pSource->Model, &(pSource->origin)))
				{
					//Rotate wall decal angle to match model's rotation
					geVec3d Direction, Direction2, CopyOfDirection, UnitVec;
					geXForm3d Xf;

					geXForm3d_SetIdentity(&Xf);

					if(pSource->OriginalAngle.Y != 0.0f)
						geXForm3d_RotateY(&Xf, (pSource->OriginalAngle.Y - 90.0f) * GE_PIOVER180);

					if(pSource->OriginalAngle.X != 0.0f)
						geXForm3d_RotateX(&Xf, pSource->OriginalAngle.X * GE_PIOVER180);

					// Rotate our unit vector about X and Y (OriginalAngle.Z should always be 0.0),
					// put answer in Direction
					geVec3d_Set(&UnitVec, 0.0f, 0.0f, -1.0f);
					geXForm3d_Rotate(&Xf, &UnitVec, &Direction);
					geVec3d_Copy(&Direction, &CopyOfDirection);

					// Now rotate Direction using the transform of the model we are attached to
					geXForm3d ModelXForm;
					geWorld_GetModelXForm(CCD->World(), pSource->Model, &ModelXForm);
					geXForm3d_Rotate(&ModelXForm, &Direction, &Direction);

					geVec3d_Set(&UnitVec, 1.0f, 0.0f, 0.0f);
					geXForm3d_Rotate(&Xf, &UnitVec, &UnitVec);
					geXForm3d_Rotate(&ModelXForm, &UnitVec, &Direction2);

					// changed QD 12/15/05 - use squared distance
					geVec3d temp1, temp2;
					geVec3d_Subtract(&Direction, &CopyOfDirection, &temp1);
					geVec3d_Subtract(&UnitVec, &Direction2, &temp2);

					//if((geVec3d_DistanceBetween(&Direction, &CopyOfDirection) != 0.0f) ||
					//	(geVec3d_DistanceBetween(&UnitVec, &Direction2) != 0.0f))
					if((geVec3d_DotProduct(&temp1, &temp1) != 0.0f) ||
						(geVec3d_DotProduct(&temp2, &temp2) != 0.0f))
					// end change
					{
						//Remove the old decal and place the new one
						geWorld_RemovePoly(CCD->World(), pSource->Poly);
						AddDecal(pSource, &Direction, &Direction2);
					}
					else
					{
						// changed QD 12/15/05 - use squared distance
						geVec3d temp;
						geVec3d_Subtract(&OriginalOrigin, &(pSource->origin), &temp);

						//if(geVec3d_DistanceBetween(&OriginalOrigin, &(pSource->origin)) != 0.0f)
						if(geVec3d_DotProduct(&temp, &temp) != 0.0f)
						// end change
						{
							geWorld_RemovePoly(CCD->World(), pSource->Poly);
							AddDecal(pSource);
						}
					}
				}
			}
		}
//End Aug2003DCS
	}
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CWallDecal::AddDecal(WallDecal *pSource)
{
	geVec3d Axis[3] =
	{
		{1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f}
	};

	pSource->origin = pSource->OriginOffset;
	SetOriginOffset(pSource->EntityName, pSource->BoneName, pSource->Model, &(pSource->origin));

	geVec3d  Direction;

	Direction.Z = GE_PIOVER180*(pSource->Angle.Z);
	Direction.X = GE_PIOVER180*(pSource->Angle.X);
	Direction.Y = GE_PIOVER180*(pSource->Angle.Y-90.0f);

	geXForm3d Xf;
	// changed QD 12/15/05
	//geXForm3d_SetIdentity(&Xf);
	//geXForm3d_RotateZ(&Xf, Direction.Z);
	geXForm3d_SetZRotation(&Xf, Direction.Z);
	geXForm3d_RotateX(&Xf, Direction.X);
	geXForm3d_RotateY(&Xf, Direction.Y);
	// geXForm3d_Translate(&Xf, pSource->origin.X, pSource->origin.Y, pSource->origin.Z);
	// end change

	//Pos = Xf.Translation;

	geXForm3d_GetIn(&Xf, &Direction);

	geVec3d Front, Back;
	geVec3d_AddScaled(&(pSource->origin), &Direction, 4000.0f, &Back);
// changed QD 12/15/05
	//geVec3d_AddScaled(&Pos, &Direction, 0.0f, &Front);
	geVec3d_Copy(&(pSource->origin), &Front);
// end change

	GE_Collision Collision;
	geWorld_Collision(CCD->World(), NULL, NULL, &Front, &Back,
		GE_CONTENTS_SOLID_CLIP, GE_COLLIDE_MODELS | GE_COLLIDE_MESHES , 0, NULL, NULL, &Collision);

	geVec3d impact = Collision.Impact;
	geVec3d normal = Collision.Plane.Normal;
	geVec3d right, up;

//	int major = 0;
//
//#define fab(a) (a > 0 ? a : -a)
//
//	if(fab(normal.Y) > fab(normal.X))
//	{
//		major = 1;
//		if(fab(normal.Z) > fab(normal.Y))
//			major = 2;
//	}
//	else
//	{
//		if(fab(normal.Z) > fab(normal.X))
//			major = 2;
//	}
//
//	if(fab(normal.X)==1.0f || fab(normal.Y)==1.0f || fab(normal.Z)==1.0f)
//	{
//		if ((major == 0 && normal.X > 0) || major == 1)
//		{
//			right.X =  0.0f;
//			right.Y =  0.0f;
//			right.Z = -1.0f;
//		}
//		else if (major == 0)
//		{
//			right.X =  0.0f;
//			right.Y =  0.0f;
//			right.Z =  1.0f;
//		}
//		else
//		{
//			right.X =  normal.Z;
//			right.Y =  0.0f;
//			right.Z =  0.0f;
//		}
//	}
//	else
//		geVec3d_CrossProduct(&Axis[major], &normal, &right);

	right.X = ( normal.Z * cos(GE_PIOVER180*pSource->Angle.Z)) + ( normal.Y * cos(GE_PIOVER180*pSource->Angle.Z));
	right.Y = ( normal.Z * sin(GE_PIOVER180*pSource->Angle.Z)) + ( normal.X * sin(GE_PIOVER180*pSource->Angle.Z));
	right.Z = (-normal.X * cos(GE_PIOVER180*pSource->Angle.Z)) + (-normal.Y * sin(GE_PIOVER180*pSource->Angle.Z));

	geVec3d_CrossProduct(&normal, &right, &up);
	geVec3d_Normalize(&up);
	geVec3d_Normalize(&right);

	geVec3d_Scale(&right, pSource->Width*0.5f, &right);
	geVec3d_Scale(&up, pSource->Height*0.5f, &up);
// changed RF064
	geVec3d_MA(&impact, 0.5f, &normal, &impact);

	GE_LVertex	vertex[4];

	// Setup vertex 1,2,3,4
	for(int i=0; i<4; ++i)
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
// end change RF064

	//calculate vertices from corners
	vertex[1].X = impact.X + (-right.X - up.X);
	vertex[1].Y = impact.Y + (-right.Y - up.Y);
	vertex[1].Z = impact.Z + (-right.Z - up.Z);

	vertex[2].X = impact.X + (right.X - up.X);
	vertex[2].Y = impact.Y + (right.Y - up.Y);
	vertex[2].Z = impact.Z + (right.Z - up.Z);

	vertex[3].X = impact.X + (right.X + up.X);
	vertex[3].Y = impact.Y + (right.Y + up.Y);
	vertex[3].Z = impact.Z + (right.Z + up.Z);

	vertex[0].X = impact.X + (-right.X + up.X);
	vertex[0].Y = impact.Y + (-right.Y + up.Y);
	vertex[0].Z = impact.Z + (-right.Z + up.Z);

// changed RF064
	if(!pSource->Animated)
	{
		pSource->Poly = geWorld_AddPoly(CCD->World(),
							vertex,
							4,
							pSource->Bitmap,
							GE_TEXTURED_POLY,
							GE_RENDER_DO_NOT_OCCLUDE_OTHERS | GE_RENDER_DEPTH_SORT_BF ,
							1.0f);
	}
	else
	{
		memcpy(pSource->vertex, vertex, sizeof(GE_LVertex)*4);
		pSource->Poly = geWorld_AddPoly(CCD->World(),
										static_cast<GE_LVertex*>(pSource->vertex),
										4,
										pSource->FBitmap[pSource->CurTex],
										GE_TEXTURED_POLY,
										GE_RENDER_DO_NOT_OCCLUDE_OTHERS | GE_RENDER_DEPTH_SORT_BF,
										1.0f);
	}
// end change RF064
}

// Start Aug2003DCS - Added three functions
/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CWallDecal::AddDecal(WallDecal *pSource, const geVec3d *InVec, const geVec3d *RightVec)
{
	geVec3d Axis[3] =
	{
		{1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f}
	};

	geVec3d Front, Back;

// changed QD 12/15/05
	geVec3d_AddScaled(&(pSource->origin), InVec, 4000.0f, &Back);
	geVec3d_Copy(&(pSource->origin), &Front);
// end change

	GE_Collision Collision;
	geWorld_Collision(CCD->World(), NULL, NULL, &Front, &Back,
		GE_CONTENTS_SOLID_CLIP, GE_COLLIDE_MODELS | GE_COLLIDE_MESHES , 0, NULL, NULL, &Collision);

	geVec3d impact = Collision.Impact;
	geVec3d normal = Collision.Plane.Normal;

	geVec3d up, Right;
	geVec3d_CrossProduct(&normal, RightVec, &up);
	geVec3d_Normalize(&up);
	geVec3d_Copy(RightVec, &Right);
	geVec3d_Normalize(&Right);

	geVec3d_Scale(&Right, pSource->Width*0.5f, &Right);
	geVec3d_Scale(&up, pSource->Height*0.5f, &up);

	geVec3d_MA(&impact, 0.1f, &normal, &impact);

	GE_LVertex	vertex[4];

	//Setup vertex 1,2,3,4
	for(int i=0; i<4; ++i)
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

	//calculate vertices from corners
	vertex[1].X = impact.X + (-Right.X - up.X);
	vertex[1].Y = impact.Y + (-Right.Y - up.Y);
	vertex[1].Z = impact.Z + (-Right.Z - up.Z);

	vertex[2].X = impact.X + (Right.X - up.X);
	vertex[2].Y = impact.Y + (Right.Y - up.Y);
	vertex[2].Z = impact.Z + (Right.Z - up.Z);

	vertex[3].X = impact.X + (Right.X + up.X);
	vertex[3].Y = impact.Y + (Right.Y + up.Y);
	vertex[3].Z = impact.Z + (Right.Z + up.Z);

	vertex[0].X = impact.X + (-Right.X + up.X);
	vertex[0].Y = impact.Y + (-Right.Y + up.Y);
	vertex[0].Z = impact.Z + (-Right.Z + up.Z);

	if(!pSource->Animated)
	{
		pSource->Poly = geWorld_AddPoly(CCD->World(),
						vertex,
						4,
						pSource->Bitmap,
						GE_TEXTURED_POLY,
						GE_RENDER_DO_NOT_OCCLUDE_OTHERS | GE_RENDER_DEPTH_SORT_BF ,
						1.0f);
	}
	else
	{
		memcpy(pSource->vertex, vertex, sizeof(GE_LVertex)*4);
		pSource->Poly = geWorld_AddPoly(CCD->World(),
										static_cast<GE_LVertex*>(pSource->vertex),
										4,
										pSource->FBitmap[pSource->CurTex],
										GE_TEXTURED_POLY,
										GE_RENDER_DO_NOT_OCCLUDE_OTHERS | GE_RENDER_DEPTH_SORT_BF ,
										1.0f);
	}
}

/* ------------------------------------------------------------------------------------ */
//	SetProgrammedTrigger
//
//	Given a name, locate the desired item in the currently loaded level
//	..and set it's ProgrammedTrigger boolean.
/* ------------------------------------------------------------------------------------ */
int CWallDecal::SetProgrammedTrigger(const char *szName, geBoolean Flag)
{
	// Ok, check to see if there are  triggers in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "WallDecal");

	if(!pSet)
		return RGF_NOT_FOUND;									// No WallDecals

	geEntity *pEntity;

	// Ok, we have  triggers.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet,NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet,pEntity))
	{
		WallDecal *pTheEntity = static_cast<WallDecal*>(geEntity_GetUserData(pEntity));

		if(!strcmp(pTheEntity->szEntityName, szName))
		{
			pTheEntity->ProgrammedTrigger = Flag;
			return RGF_SUCCESS;
		}
	}

	return RGF_NOT_FOUND;								// Sorry, no such entity here
}

/* ------------------------------------------------------------------------------------ */
//	SetCurrentBitmap
//
//	Given a name, locate the desired item in the currently loaded level
//	..and set it's CurBmp value.
/* ------------------------------------------------------------------------------------ */
int CWallDecal::SetCurrentBitmap(const char *szName, int CurrentBitmap)
{
	// Ok, check to see if there are  triggers in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "WallDecal");

	if(!pSet)
		return RGF_NOT_FOUND;									// No WallDecals

	geEntity *pEntity;

	// Ok, we have  WallDecals.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet,NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet,pEntity))
	{
		WallDecal *pTheEntity = static_cast<WallDecal*>(geEntity_GetUserData(pEntity));

		if(!strcmp(pTheEntity->szEntityName, szName))
		{
			if ((CurrentBitmap >= 0) && (CurrentBitmap < pTheEntity->BitmapCount))
				pTheEntity->CurTex = CurrentBitmap;
			else
				pTheEntity->CurTex = 0;

			pTheEntity->BmpChanged = GE_TRUE;
			return RGF_SUCCESS;
		}
	}

	return RGF_NOT_FOUND;								// Sorry, no such entity here
}
// End Aug2003DCS



/* ----------------------------------- END OF FILE ------------------------------------ */
