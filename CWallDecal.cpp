/*
	CWallDecal.cpp:		Decal class implementation

	(c) 2000 Ralph Deane
	All Rights Reserved

	This file contains the class implementation for WallDecal
handling.
*/

//	Include the One True Header

#include "RabidFramework.h"
#include <Ram.h>

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

      //Start Aug2003DCS - Added szEntityName
		if(EffectC_IsStringNull(pSource->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pSource->szEntityName = szName;
		}
      //End Aug2003DCS

// changed RF064
		pSource->FBitmap = NULL;
		pSource->vertex = NULL;
		if(!pSource->Animated)
			pSource->Bitmap = TPool_Bitmap(pSource->BmpName, pSource->AlphaName, NULL, NULL);
		else
		{
			pSource->vertex = (void *)geRam_AllocateClear( sizeof(GE_LVertex) * 4);
			pSource->FBitmap = (geBitmap **)geRam_AllocateClear( sizeof( *( pSource->FBitmap ) ) * pSource->BitmapCount );
			for (int i = 0; i < pSource->BitmapCount; i++ )
			{
				char BmpName[256];
				char AlphaName[256];
				// build bmp and alpha names
				sprintf( BmpName, "%s%d%s", pSource->BmpName, i, ".bmp" );
				if(!EffectC_IsStringNull(pSource->AlphaName))
				{
					sprintf( AlphaName, "%s%d%s", pSource->AlphaName, i, ".bmp" );
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
         geVec3d_Subtract(&pSource->origin, &ModelOrigin, &pSource->OriginOffset);
         //Start Aug2003DCS
         geVec3d_Copy(&pSource->Angle, &pSource->OriginalAngle);
         geVec3d_Copy(&pSource->Angle, &pSource->LastAngle);
         //End Aug2003DCS
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


//	Destructor
//
//	Clean up.

CWallDecal::~CWallDecal()
{
// changed RF064
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
		if(pSource->FBitmap)
			geRam_Free(pSource->FBitmap);
		if(pSource->vertex)
			geRam_Free(pSource->vertex);
	}
// end change RF064
}
// changed RF064
void CWallDecal::Tick(float dwTicks)
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

//Start Aug2003DCS  Moved this section up AND modified it
// changed RF064
		if(pSource->active==GE_TRUE && pSource->Animated)
		{
			pSource->Time += dwTicks;
			if(pSource->Time>=(1000.0f*(1.0f/pSource->Speed)))
			{
				pSource->Time = 0.0f;
				switch(pSource->Style)
				{
					case 0:
               case 5:
						break;
					case 2:
						pSource->CurTex += pSource->CycleDir;
						if(pSource->CurTex>=pSource->BitmapCount || pSource->CurTex<0)
						{
							pSource->CycleDir = -pSource->CycleDir;
							pSource->CurTex += pSource->CycleDir;
						}
                  pSource->BmpChanged = GE_TRUE;
						break;
					case 3:
						pSource->CurTex = ( rand() % pSource->BitmapCount );
                  pSource->BmpChanged = GE_TRUE;
						break;
					case 4:
						pSource->CurTex +=1;
						if(pSource->CurTex>=pSource->BitmapCount)
							pSource->CurTex = pSource->BitmapCount-1;
                  pSource->BmpChanged = GE_TRUE;
						break;
					default:
						pSource->CurTex +=1;
						if(pSource->CurTex>=pSource->BitmapCount)
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
//End Aug2003DCS

		if(!EffectC_IsStringNull(pSource->TriggerName))
		{
         //Start Aug2003DCS
         if( ((strcmp(pSource->TriggerName,"*Programmed*") == 0) && pSource->ProgrammedTrigger) ||
             ((strcmp(pSource->TriggerName,"*Programmed*") != 0) && GetTriggerState(pSource->TriggerName)) )
			{
				if(pSource->active==GE_FALSE)
				{
					AddDecal(pSource);
					pSource->active = GE_TRUE;
				}
            else
            {
               if (pSource->active)
               {
                  if (pSource->BmpChanged = GE_TRUE)
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
				if(pSource->active==GE_TRUE)
				{
					geWorld_RemovePoly(CCD->World(), pSource->Poly); 
					pSource->active = GE_FALSE;
				}
			}
         //End Aug2003DCS
// changed RF064
		}
		else
		{
			if(pSource->active!=GE_TRUE)
			{
				AddDecal(pSource);
				pSource->active = GE_TRUE;
			}
		}
      //Start Aug2003DCS
		if(pSource->active==GE_TRUE)
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
               if (pSource->OriginalAngle.Y != 0.0) geXForm3d_RotateY(&Xf, (pSource->OriginalAngle.Y - 90.0f) * 0.0174532925199433f);
               if (pSource->OriginalAngle.X != 0.0) geXForm3d_RotateX(&Xf, pSource->OriginalAngle.X * 0.0174532925199433f);
               //Rotate our unit vector about X and Y (OriginalAngle.Z should always be 0.0), put answer in Direction
               geVec3d_Set(&UnitVec, 0.0, 0.0, -1.0);
               geXForm3d_Rotate(&Xf, &UnitVec, &Direction);
               geVec3d_Copy(&Direction, &CopyOfDirection);
               //Now rotate Direction using the transform of the model we are attached to
               geXForm3d ModelXForm;
               geWorld_GetModelXForm(CCD->World(), pSource->Model, &ModelXForm);
               geXForm3d_Rotate(&ModelXForm, &Direction, &Direction);

               geVec3d_Set(&UnitVec, 1.0, 0.0, 0.0);
               geXForm3d_Rotate(&Xf, &UnitVec, &UnitVec);
               geXForm3d_Rotate(&ModelXForm, &UnitVec, &Direction2);

               if ((geVec3d_DistanceBetween(&Direction, &CopyOfDirection) != 0.0) || (geVec3d_DistanceBetween(&UnitVec, &Direction2) != 0.0))
               {
                  //Remove the old decal and place the new one
                  geWorld_RemovePoly(CCD->World(), pSource->Poly); 
					   AddDecal(pSource, &Direction, &Direction2);
               }
               else
               {
                  if (geVec3d_DistanceBetween(&OriginalOrigin, &pSource->origin) != 0.0)
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

	pSource->origin = pSource->OriginOffset;
	SetOriginOffset(pSource->EntityName, pSource->BoneName, pSource->Model, &(pSource->origin));
	
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

	impact = Collision.Impact;
	normal = Collision.Plane.Normal;
	
//Start Aug2003DCS
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

	right.X = (float) ( (normal.Z * cos(0.0174532925199433f*pSource->Angle.Z)) + ( normal.Y * cos(0.0174532925199433f*pSource->Angle.Z)));
	right.Y = (float) ( (normal.Z * sin(0.0174532925199433f*pSource->Angle.Z)) + ( normal.X * sin(0.0174532925199433f*pSource->Angle.Z)));
	right.Z = (float) ((-normal.X * cos(0.0174532925199433f*pSource->Angle.Z)) + (-normal.Y * sin(0.0174532925199433f*pSource->Angle.Z)));
//End Aug2003DCS

	geVec3d_CrossProduct(&normal, &right, &up);
	geVec3d_Normalize(&up);
	geVec3d_Normalize(&right);
	
	geVec3d_Scale(&right, pSource->Width/2.0f, &right); 
	geVec3d_Scale(&up, pSource->Height/2.0f, &up);
// changed RF064	
	geVec3d_MA(&impact, 0.5f, &normal, &impact);
// end change RF064	
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
      //Start Aug2003DCS
		pSource->Poly = geWorld_AddPoly(CCD->World(),
      //End Aug2003DCS
						(GE_LVertex *)pSource->vertex,
						4,
						pSource->FBitmap[pSource->CurTex],
						GE_TEXTURED_POLY,
						GE_RENDER_DO_NOT_OCCLUDE_OTHERS | GE_RENDER_DEPTH_SORT_BF ,
						1.0f); 
	}
// end change RF064
}

//Start Aug2003DCS - Added three functions
void CWallDecal::AddDecal(WallDecal *pSource, geVec3d *InVec, geVec3d *RightVec)
{
	geVec3d    up;
	GE_LVertex vertex[4];
	
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
	
	geVec3d impact, normal, Pos;
	geVec3d Front, Back;
	GE_Collision	Collision;
	
	geVec3d_Set(&Pos, pSource->origin.X, pSource->origin.Y, pSource->origin.Z);
	
	geVec3d_AddScaled (&Pos, InVec, 4000.0f, &Back);
	geVec3d_AddScaled (&Pos, InVec, 0.0f, &Front);
	geWorld_Collision(CCD->World(), NULL, NULL, &Front, 
		&Back, GE_CONTENTS_SOLID_CLIP, GE_COLLIDE_MODELS | GE_COLLIDE_MESHES , 0, NULL, NULL, &Collision);
	impact =Collision.Impact;
	normal = Collision.Plane.Normal;
	
	geVec3d_CrossProduct(&normal, RightVec, &up);
	geVec3d_Normalize(&up);
	geVec3d_Normalize(RightVec);
	
	geVec3d_Scale(RightVec, pSource->Width/2.0f, RightVec); 
	geVec3d_Scale(&up, pSource->Height/2.0f, &up);
	
	geVec3d_MA(&impact, 0.1f, &normal, &impact);
	
	//calculate vertices from corners
	vertex[1].X = impact.X + ((-RightVec->X - up.X));
	vertex[1].Y = impact.Y + ((-RightVec->Y - up.Y));
	vertex[1].Z = impact.Z + ((-RightVec->Z - up.Z));
	
	vertex[2].X = impact.X + ((RightVec->X - up.X));
	vertex[2].Y = impact.Y + ((RightVec->Y - up.Y));
	vertex[2].Z = impact.Z + ((RightVec->Z - up.Z));
	
	vertex[3].X = impact.X + ((RightVec->X + up.X));
	vertex[3].Y = impact.Y + ((RightVec->Y + up.Y));
	vertex[3].Z = impact.Z + ((RightVec->Z + up.Z));
	
	vertex[0].X = impact.X + ((-RightVec->X + up.X));
	vertex[0].Y = impact.Y + ((-RightVec->Y + up.Y));
	vertex[0].Z = impact.Z + ((-RightVec->Z + up.Z));
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
						(GE_LVertex *)pSource->vertex,
						4,
						pSource->FBitmap[pSource->CurTex],
						GE_TEXTURED_POLY,
						GE_RENDER_DO_NOT_OCCLUDE_OTHERS | GE_RENDER_DEPTH_SORT_BF ,
						1.0f); 
	}
}

//	SetProgrammedTrigger
//
//	Given a name, locate the desired item in the currently loaded level
//	..and set it's ProgrammedTrigger boolean.

int CWallDecal::SetProgrammedTrigger(char *szName, geBoolean Flag)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, check to see if there are  triggers in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "WallDecal");
	
	if(!pSet) 
		return RGF_NOT_FOUND;									// No WallDecals
	
	//	Ok, we have  triggers.  Dig through 'em all.
	
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet,NULL); pEntity; pEntity=geEntity_EntitySetGetNextEntity(pSet,pEntity)) 
	{
		WallDecal *pTheEntity = (WallDecal*)geEntity_GetUserData(pEntity);
		if(strcmp(pTheEntity->szEntityName, szName) == 0)
		{
         pTheEntity->ProgrammedTrigger = Flag;
         return RGF_SUCCESS;
		}
	}
	
	return RGF_NOT_FOUND;								// Sorry, no such entity here
}

//	SetCurrentBitmap
//
//	Given a name, locate the desired item in the currently loaded level
//	..and set it's CurBmp value.

int CWallDecal::SetCurrentBitmap(char *szName, int CurrentBitmap)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, check to see if there are  triggers in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "WallDecal");
	
	if(!pSet) 
		return RGF_NOT_FOUND;									// No WallDecals
	
	//	Ok, we have  WallDecals.  Dig through 'em all.
	
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet,NULL); pEntity; pEntity=geEntity_EntitySetGetNextEntity(pSet,pEntity)) 
	{
		WallDecal *pTheEntity = (WallDecal*)geEntity_GetUserData(pEntity);
		if(strcmp(pTheEntity->szEntityName, szName) == 0)
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
//End Aug2003DCS


