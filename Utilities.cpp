/****************************************************************************************/
/*																						*/
/*	Utilities.cpp:		Utilities code													*/
/*																						*/
/*	(c) 1999 Ralph Deane																*/
/*	All Rights Reserved																	*/
/*																						*/
/*	This file contains the utilities functions											*/
/*																						*/
/****************************************************************************************/

//	Include the One True Header
#include "RabidFramework.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//MOD010124 - Added the SetOriginOffset function.
/* ------------------------------------------------------------------------------------ */
// SetOriginOffset
//
// If attached to an entity or model set Origin and return true
//   return false otherwise
// If attached to a model, this function will transform the
//   origin offset vector by the same transform (including
//   translation) applied to the model.
/* ------------------------------------------------------------------------------------ */
bool SetOriginOffset(char *EntityName, char *BoneName, geWorld_Model *Model, geVec3d *OriginOffset)
{
	if(!EffectC_IsStringNull(EntityName))
	{
// changed RF064
		if(!stricmp(EntityName, "Player"))
		{
			geXForm3d Xf;

			if(EffectC_IsStringNull(BoneName))
				BoneName = NULL;

			if(geActor_GetBoneTransform(CCD->Player()->GetActor(), BoneName, &Xf )==GE_TRUE)
			{
				geVec3d_Copy( &( Xf.Translation ), OriginOffset );
				return true;
			}

			return false;
		}
// end change RF064

		char *EntityType = CCD->EntityRegistry()->GetEntityType(EntityName);

		if(EntityType)
		{
			if(!stricmp(EntityType, "StaticEntityProxy"))
			{
				StaticEntityProxy *pProxy;
				CCD->Props()->LocateEntity(EntityName, (void**)&pProxy);

				if(pProxy->Actor)
				{
					geXForm3d Xf;

					if(EffectC_IsStringNull(BoneName))
						BoneName = NULL;

					if(geActor_GetBoneTransform(pProxy->Actor, BoneName, &Xf )==GE_TRUE)
					{
						geVec3d_Copy( &( Xf.Translation ), OriginOffset );
						return true;
					}

					return false;
				}
			}

			if(!stricmp(EntityType, "Attribute"))
			{
				Attribute *pProxy;
				CCD->Attributes()->LocateEntity(EntityName, (void**)&pProxy);

				if(pProxy->Actor)
				{
					geXForm3d Xf;

					if(EffectC_IsStringNull(BoneName))
						BoneName = NULL;

					if(geActor_GetBoneTransform(pProxy->Actor, BoneName, &Xf )==GE_TRUE)
					{
						geVec3d_Copy( &( Xf.Translation ), OriginOffset );
						return true;
					}

					return false;
				}
			}
// changed RF064

			if(!stricmp(EntityType, "Pawn"))
			{
				Pawn *pProxy;
				CCD->Pawns()->LocateEntity(EntityName, (void**)&pProxy);
				ScriptedObject *Object = (ScriptedObject *)pProxy->Data;

				if(Object->Actor)
				{
					geXForm3d Xf;

					if(EffectC_IsStringNull(BoneName))
						BoneName = NULL;

					if(geActor_GetBoneTransform(Object->Actor, BoneName, &Xf )==GE_TRUE)
					{
						geVec3d_Copy( &( Xf.Translation ), OriginOffset );
						return true;
					}

					return false;
				}
				else
				{
					geVec3d_Copy( &Object->Location, OriginOffset );
					return true;
				}
			}
// end change RF064
		}
	}
	else if(Model)
	{
		geXForm3d Xf1;
		geWorld_GetModelXForm(CCD->World(), Model, &Xf1);

		if(Xf1.Translation.X == 10000.0f && Xf1.Translation.Y == 10000.0f && Xf1.Translation.Z == 10000.0f)
			return false;

		geXForm3d_Transform(&Xf1, OriginOffset, OriginOffset);

		geVec3d ModelOrigin;
		geWorld_GetModelRotationalCenter(CCD->World(), Model, &ModelOrigin);
		geVec3d_Add(OriginOffset, &ModelOrigin, OriginOffset);
		return true;
  	}

	return false;
}

/* ------------------------------------------------------------------------------------ */
// SetAngle
//
// If attached to an entity  set bone angle and return true
// return false otherwise
/* ------------------------------------------------------------------------------------ */
bool SetAngle(char *EntityName, char *BoneName, geVec3d *Angle)
{
	if(!EffectC_IsStringNull(EntityName))
	{
// changed RF064
		if(!stricmp(EntityName, "Player"))
		{
			if(EffectC_IsStringNull(BoneName))
				BoneName = NULL;

			if(CCD->ActorManager()->GetBoneRotation(CCD->Player()->GetActor(), BoneName, Angle) == RGF_SUCCESS)
				return true;

			return false;
		}
// end change RF064

		char *EntityType = CCD->EntityRegistry()->GetEntityType(EntityName);

		if(EntityType)
		{
			if(!stricmp(EntityType, "StaticEntityProxy"))
			{
				StaticEntityProxy *pProxy;
				CCD->Props()->LocateEntity(EntityName, (void**)&pProxy);

				if(pProxy->Actor)
				{
					if(CCD->ActorManager()->GetBoneRotation(pProxy->Actor, BoneName, Angle) == RGF_SUCCESS)
						return true;

					return false;
				}
			}

			if(!stricmp(EntityType, "Attribute"))
			{
				Attribute *pProxy;
				CCD->Attributes()->LocateEntity(EntityName, (void**)&pProxy);

				if(pProxy->Actor)
				{
					if(CCD->ActorManager()->GetBoneRotation(pProxy->Actor, BoneName, Angle) == RGF_SUCCESS)
						return true;

					return false;
				}
			}

// changed RF064
			if(!stricmp(EntityType, "Pawn"))
			{
				Pawn *pProxy;
				CCD->Pawns()->LocateEntity(EntityName, (void**)&pProxy);
				ScriptedObject *Object = (ScriptedObject *)pProxy->Data;

				if(Object->Actor)
				{
					if(CCD->ActorManager()->GetRotation(Object->Actor, Angle) == RGF_SUCCESS)
					{
						Angle->Y += GE_PI;
						return true;
					}

					return false;
				}
			}
// end change RF064
		}
	}

	return false;
}

/* ------------------------------------------------------------------------------------ */
// GetTriggerState
//
// return state of trigger
// return false if trigger invalid
/* ------------------------------------------------------------------------------------ */
bool GetTriggerState(char *TriggerName)
{
// changed RF064
	if(!stricmp(TriggerName, "MovieMode"))
	{
		return CCD->Player()->GetMonitorState();
	}

	if(!stricmp(TriggerName, "PlayerDeath"))
	{
		return CCD->Player()->GetDying();
	}

	if(!stricmp(TriggerName, "DeathESC"))
	{
		return CCD->Player()->GetMonitorState();
	}

	if(!stricmp(TriggerName, "DeathSpace"))
	{
		return CCD->Player()->GetDeathSpace();
	}

	if(!stricmp(TriggerName, "InFirstPerson"))
	{
		return CCD->Player()->InFirstPerson();
	}

/*	if(!strnicmp(TriggerName, "Zoom", 4))
	{
		if(!CCD->CameraManager()->GetZooming())
			return false;
		if(strlen(TriggerName)==4)
			return true;
		char *Weaponname;
		Weaponname = (TriggerName+4);
		if(!stricmp(Weaponname, CCD->Weapons()->GetWeaponName()))
			return true;
		return false;
	} */
// end change RF064

	char *EntityType = CCD->EntityRegistry()->GetEntityType(TriggerName);

	if(EntityType)
	{
		if(!stricmp(EntityType, "Trigger"))
		{
			Trigger *pProxy;
			CCD->Triggers()->LocateEntity(TriggerName, (void**)&pProxy);
			return pProxy->bState;
		}

		if(!stricmp(EntityType, "LogicGate"))
		{
			LogicGate *pProxy;
			CCD->Logic()->LocateEntity(TriggerName, (void**)&pProxy);
			return pProxy->bState;
		}

		if(!stricmp(EntityType, "StaticEntityProxy"))
		{
			StaticEntityProxy *pProxy;
			CCD->Props()->LocateEntity(TriggerName, (void**)&pProxy);
			return pProxy->bState;
		}

		if(!stricmp(EntityType, "Attribute"))
		{
			Attribute *pProxy;
			CCD->Attributes()->LocateEntity(TriggerName, (void**)&pProxy);
			return pProxy->bState;
		}

		if(!stricmp(EntityType, "DestroyableModel"))
		{
			DestroyableModel *pProxy;
			CCD->Damage()->LocateEntity(TriggerName, (void**)&pProxy);
			return pProxy->bState;
		}

		if(!stricmp(EntityType, "ElectricBolt"))
		{
			ElectricBolt *pProxy = NULL;
			CCD->ElectricEffects()->LocateEntity(TriggerName, (void**)&pProxy);
			return pProxy->bState;
		}

// changed RF064
		if(!stricmp(EntityType, "CountDownTimer"))
		{
			CountDownTimer *pProxy;
			CCD->CountDownTimers()->LocateEntity(TriggerName, (void**)&pProxy);
			return pProxy->bState;
		}

		if(!stricmp(EntityType, "LiftBelt"))
		{
			LiftBelt *pProxy;
			CCD->LiftBelts()->LocateEntity(TriggerName, (void**)&pProxy);
			return pProxy->bState;
		}
	}

	return CCD->Pawns()->GetEventState(TriggerName);
// end change RF064
}

/* ------------------------------------------------------------------------------------ */
// GetCallBackState
//
// return state of Callback flag
// return false if Callback invalid
/* ------------------------------------------------------------------------------------ */
bool GetCallBackState(char *CallBackName)
{
	char *EntityType = CCD->EntityRegistry()->GetEntityType(CallBackName);

	if(EntityType)
	{
		if(!stricmp(EntityType, "Trigger"))
		{
			Trigger *pProxy;
			CCD->Triggers()->LocateEntity(CallBackName, (void**)&pProxy);
			return pProxy->CallBack;
		}

		if(!stricmp(EntityType, "Door"))
		{
			Door *pProxy;
			CCD->Doors()->LocateEntity(CallBackName, (void**)&pProxy);
			return pProxy->CallBack;
		}

		if(!stricmp(EntityType, "Attribute"))
		{
			Attribute *pProxy;
			CCD->Attributes()->LocateEntity(CallBackName, (void**)&pProxy);
			return pProxy->CallBack;
		}

		if(!stricmp(EntityType, "StaticEntityProxy"))
		{
			StaticEntityProxy *pProxy;
			CCD->Props()->LocateEntity(CallBackName, (void**)&pProxy);
			return pProxy->CallBack;
		}

		if(!stricmp(EntityType, "DestroyableModel"))
		{
			DestroyableModel *pProxy;
			CCD->Damage()->LocateEntity(CallBackName, (void**)&pProxy);
			return pProxy->CallBack;
		}

		if(!stricmp(EntityType, "ChangeLevel"))
		{
			ChangeLevel *pProxy;
			CCD->Changelevel()->LocateEntity(CallBackName, (void**)&pProxy);
			return pProxy->CallBack;
		}

// changed QD 12/15/05
		if(!stricmp(EntityType, "Teleporter"))
		{
			Teleporter *pProxy;
			CCD->Teleporters()->LocateEntity(CallBackName, (void**)&pProxy);
			return pProxy->CallBack;
		}
// end change
	}

	return false;
}

/* ------------------------------------------------------------------------------------ */
//	EffectC_IsStringNull()
//
//	Determines if a string is NULL, accounting for additional editor posibilities.
/* ------------------------------------------------------------------------------------ */
geBoolean EffectC_IsStringNull(char *String )
{

	// first way
	if(String == NULL)
	{
		return GE_TRUE;
	}

	// second way
	if(strlen(String) < 1)
	{
		return GE_TRUE;
	}

	// third way
	if(strnicmp(String, "<null>", 6) == 0)
	{
		return GE_TRUE;
	}

	// fourth way
	if(strnicmp(String, "NULL", 4) == 0)
	{
		return GE_TRUE;
	}

	// if we got to here then the string is not null
	return GE_FALSE;

} // EffectC_IsStringNull()

/* ------------------------------------------------------------------------------------ */
//	EffectC_IsPointVisible()
//
//	Returns true if point is visible, false if it isn't.
/* ------------------------------------------------------------------------------------ */
geBoolean EffectC_IsPointVisible(geWorld *World, geCamera *Camera, geVec3d *Target, int32 Leaf,	uint32 ClipStyle )
{
	// leaf check
	if(ClipStyle & EFFECTC_CLIP_LEAF)
	{
		if(geWorld_MightSeeLeaf(World, Leaf) == GE_FALSE)
		{
			return GE_FALSE;
		}
	}

	// semi circle check
	if(ClipStyle & EFFECTC_CLIP_SEMICIRCLE)
	{
		// locals
		const geXForm3d	*CameraXf;
		geVec3d			In;
		geVec3d			Delta;
		float			Dot;

		// get camera xform
		CameraXf = geCamera_GetWorldSpaceXForm( Camera );

		// get angle between camera in vector and vector to target
		geVec3d_Subtract(Target, &(CameraXf->Translation), &Delta);
		geVec3d_Normalize(&Delta );
		geXForm3d_GetIn(CameraXf, &In);
		Dot = geVec3d_DotProduct(&In, &Delta);

		// check if its visible
		if(Dot < 0.0f)
		{
			return GE_FALSE;
		}

		if(CCD->CameraManager()->GetClipEnable())
		{
			float dist = geVec3d_DistanceBetween(Target, &CameraXf->Translation);

			if(dist > CCD->CameraManager()->GetFarClipPlane())
				return GE_FALSE;
		}
	}

	// line of sight check
	if(ClipStyle & EFFECTC_CLIP_LINEOFSIGHT)
	{
		// locals
		GE_Collision	Collision;
		const geXForm3d	*CameraXf;

		// get camera xform
		CameraXf = geCamera_GetWorldSpaceXForm( Camera );

		// check if its visible
		if ( geWorld_Collision( World, (const geVec3d *)NULL, (const geVec3d *)NULL, &( CameraXf->Translation ), Target, GE_CONTENTS_SOLID, GE_COLLIDE_MODELS, 0, (GE_CollisionCB *)NULL, NULL, &Collision ) == GE_TRUE )
		{
			return GE_FALSE;
		}

// changed QD 09/23/03
		geActor *pActor;
		geFloat T;
		geVec3d Normal;

		if(CCD->ActorManager()->DoesRayHitActor(CameraXf->Translation, *Target, &pActor, NULL, &T, &Normal) == GE_TRUE)
			return GE_FALSE;
// end change QD
	}

	// if we got to here then its visible
	return GE_TRUE;

} // EffectC_IsPointVisible()


// changed QD 01/2004
/* ------------------------------------------------------------------------------------ */
//	this is a non-conservative test
//	check all box corners, then shrink the box and check the corners again...
/* ------------------------------------------------------------------------------------ */
geBoolean EffectC_IsBoxVisible(geWorld *World, geCamera *Camera, geExtBox* TestBox)
{
	// Box corners
	geVec3d BoxCorners[8];

	BoxCorners[0] = TestBox->Min;
	BoxCorners[1] = BoxCorners[0];  BoxCorners[1].X = TestBox->Max.X;
	BoxCorners[2] = BoxCorners[0];  BoxCorners[2].Y = TestBox->Max.Y;
	BoxCorners[3] = BoxCorners[0];  BoxCorners[3].Z = TestBox->Max.Z;
	BoxCorners[4] = TestBox->Max;
	BoxCorners[5] = BoxCorners[4];  BoxCorners[5].X = TestBox->Min.X;
	BoxCorners[6] = BoxCorners[4];  BoxCorners[6].Y = TestBox->Min.Y;
	BoxCorners[7] = BoxCorners[4];  BoxCorners[7].Z = TestBox->Min.Z;

	// is the camera inside the box? then we can definitly see it
	geVec3d CamPos;
	CCD->CameraManager()->GetPosition(&CamPos);

	if(geExtBox_ContainsPoint(TestBox, &CamPos))
		return GE_TRUE;

	geVec3d temp;
	geVec3d_Subtract(&TestBox->Max, &TestBox->Min, &temp);
	geVec3d_Scale(&temp, 0.1f, &temp);

	for(int step=1;step<6;step++)
	{
		for(int i=0;i<8;i++)
		{
			// check 1
			int32 Leaf;
			geWorld_GetLeaf(World, &(BoxCorners[i]), &Leaf);

			if(geWorld_MightSeeLeaf(World, Leaf) == GE_FALSE)
				continue;

			// check 2
			GE_Collision	Collision;
			const geXForm3d	*CameraXf;

			// get camera xform
			CameraXf = geCamera_GetWorldSpaceXForm(Camera);

			if(geWorld_Collision(World, (const geVec3d*)NULL, (const geVec3d *)NULL, &(CameraXf->Translation),
				&(BoxCorners[i]), GE_CONTENTS_SOLID, GE_COLLIDE_MODELS, 0, (GE_CollisionCB*)NULL, NULL, &Collision) == GE_FALSE)
			{
				return GE_TRUE;
			}
		}

		BoxCorners[0].X = TestBox->Min.X+temp.X*step;
		BoxCorners[0].Y = TestBox->Min.Y+temp.Y*step;
		BoxCorners[0].Z = TestBox->Min.Z+temp.Z*step;
		BoxCorners[1] = BoxCorners[0];  BoxCorners[1].X = TestBox->Max.X-temp.X*step;
		BoxCorners[2] = BoxCorners[0];  BoxCorners[2].Y = TestBox->Max.Y-temp.Y*step;
		BoxCorners[3] = BoxCorners[0];  BoxCorners[3].Z = TestBox->Max.Z-temp.Z*step;
		BoxCorners[4].X = TestBox->Max.X-temp.X*step;
		BoxCorners[4].Y = TestBox->Max.Y-temp.Y*step;
		BoxCorners[4].Z = TestBox->Max.Z-temp.Z*step;
		BoxCorners[5] = BoxCorners[4];  BoxCorners[5].X = TestBox->Min.X+temp.X*step;
		BoxCorners[6] = BoxCorners[4];  BoxCorners[6].Y = TestBox->Min.Y+temp.Y*step;
		BoxCorners[7] = BoxCorners[4];  BoxCorners[7].Z = TestBox->Min.Z+temp.Z*step;
	}

	return GE_FALSE;
}
// end change

/* ------------------------------------------------------------------------------------ */
//	EffectC_Frand()
//
//	Picks a random float within the supplied range.
/* ------------------------------------------------------------------------------------ */
float EffectC_Frand(float Low, float High)
{
	// locals
	float	Range;

	// if they are the same then just return one of them
	if(High == Low)
	{
		return Low;
	}

	// pick a random float from whithin the range
	Range = High - Low;
	return ((float)(((rand() % 1000) + 1))) / 1000.0f * Range + Low;

} // EffectC_Frand()


/* ------------------------------------------------------------------------------------ */
//	EffectC_XFormFromVector()
//
//	Create a transform from two vectors.
/* ------------------------------------------------------------------------------------ */
void EffectC_XFormFromVector(geVec3d *Source, geVec3d *Target, geXForm3d *Out )
{
	// locals
	geVec3d	Temp, Vertical, Vect;

	// create a straight up vector
	Vertical.X = 0.0f;
	Vertical.Y = 1.0f;
	Vertical.Z = 0.0f;

	// create the source vector, fudging it if its coplanar to the comparison vector
	geVec3d_Subtract( Source, Target, &Vect );

	if((Vertical.X == Vect.X) && (Vertical.Z == Vect.Z))
	{
		Vertical.X += 1.0f;
	}

	// set the IN vector
	geXForm3d_SetIdentity(Out);
	geVec3d_Normalize(&Vect);
	Out->AZ = Vect.X;
	Out->BZ = Vect.Y;
	Out->CZ = Vect.Z;

	// use it with the in vector to get the RIGHT vector
	geVec3d_CrossProduct(&Vertical, &Vect, &Temp);
	geVec3d_Normalize(&Temp);

	// put the RIGHT vector in the matrix
	Out->AX = Temp.X;
	Out->BX = Temp.Y;
	Out->CX = Temp.Z;

	// use the RIGHT vector with the IN vector to get the real UP vector
	geVec3d_CrossProduct(&Vect, &Temp, &Vertical);
	geVec3d_Normalize(&Vertical);

	// put the UP vector in the matrix
	Out->AY = Vertical.X;
	Out->BY = Vertical.Y;
	Out->CY = Vertical.Z;

	// put the translation in
	Out->Translation = *Source;

} // EffectC_XFormFromVector()


/*----------------------------------------------------------------------------------------------------*/
//	Utility functions to load bitmaps
//	from gebmutil.c
/* ------------------------------------------------------------------------------------ */
geBitmap *CreateFromFileAndAlphaNames(char * BmName, char *AlphaName)
{
	geBitmap *Bmp, *AlphaBmp;

	Bmp = CreateFromFileName(BmName);

	if(!Bmp)
		return NULL;

// changed RF064
	if(!EffectC_IsStringNull(AlphaName))
	{
		if(stricmp(BmName, AlphaName))
		{
			AlphaBmp = CreateFromFileName(AlphaName);

			if(!AlphaBmp)
			{
				geBitmap_Destroy(&Bmp);
				return NULL;
			}

			if(!geBitmap_SetAlpha(Bmp, AlphaBmp))
			{
				geBitmap_Destroy(&Bmp);
				geBitmap_Destroy(&AlphaBmp);
				return NULL;
			}

			geBitmap_Destroy(&AlphaBmp);
		}
	}
// end change RF064

	//geBitmap_SetPreferredFormat(Bmp,GE_PIXELFORMAT_16BIT_4444_ARGB);
	geBitmap_SetPreferredFormat(Bmp,GE_PIXELFORMAT_32BIT_ARGB);

	return Bmp;
}

/* ------------------------------------------------------------------------------------ */
//	CreateFromFileName
/* ------------------------------------------------------------------------------------ */
geBitmap *CreateFromFileName(char *BmName)
{
	geVFile *File;
	geBitmap *Bmp;

	if(EffectC_IsStringNull(BmName))
		return NULL;

	CCD->OpenRFFile(&File, kBitmapFile, BmName, GE_VFILE_OPEN_READONLY);

	if(!File)
	{
		char szError[256];
		sprintf(szError, "*WARNING* File %s - Line %d: Missing Bitmap %s or Missing Path",
				__FILE__, __LINE__, BmName);
		CCD->ReportError(szError, false);
		// changed QD 07/15/06
		return NULL;
		/*
		CCD->ShutdownLevel();
		delete CCD;
		CCD = NULL;
		MessageBox(NULL, szError,"Bitmap Manager", MB_OK);
		exit(-333);
		*/
		// end change
	}

	Bmp = geBitmap_CreateFromFile(File);

	if(!Bmp)
	{
		FreeImageIO io;
		FIBITMAP *Fbmp32;
		geBitmap *LockedBMP;
		geBitmap_Info Info;
		unsigned char *gptr ,*fptr;
		int bpp, nFormat;
		int FStride;
		int width, height;

		io.read_proc = VFS_Read;
		io.write_proc = NULL;
		io.seek_proc = VFS_Seek;
		io.tell_proc = VFS_Tell;
		FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromHandle(&io, (fi_handle)File, 16);

		if(fif >= 0)
		{
			FIBITMAP *Fbmp = FreeImage_LoadFromHandle(fif, &io, (fi_handle)File, 0);
			bpp = (int)FreeImage_GetBPP(Fbmp);
			width = (int)FreeImage_GetWidth(Fbmp);
			height = (int)FreeImage_GetHeight(Fbmp);

			if(bpp == 32)
			{
				Fbmp32 = FreeImage_Clone(Fbmp);
				nFormat = GE_PIXELFORMAT_32BIT_BGRA;
			}
			else
			{
				if(bpp != 24)
					Fbmp32 = FreeImage_ConvertTo24Bits(Fbmp);
				else
					Fbmp32 = FreeImage_Clone(Fbmp);

				nFormat = GE_PIXELFORMAT_24BIT_BGR;
			}

			Bmp = geBitmap_Create(width, height, 0, (gePixelFormat)nFormat);

			if(Bmp)
			{
				geBitmap_GetInfo(Bmp,&Info,NULL);
				geBitmap_LockForWriteFormat(Bmp, &LockedBMP, 0, 0, (gePixelFormat)nFormat);

				if(LockedBMP == NULL)
				{
					geBitmap_SetFormat(Bmp,(gePixelFormat)nFormat,GE_TRUE,0,NULL);
					geBitmap_LockForWriteFormat(Bmp, &LockedBMP, 0, 0, (gePixelFormat)nFormat);
				}

				if(LockedBMP)
				{
					FStride = (int)FreeImage_GetPitch(Fbmp32);
					gptr = (LPBYTE)geBitmap_GetBits(LockedBMP);

					if(nFormat == GE_PIXELFORMAT_32BIT_BGRA)
						gptr += (height-1)*(Info.Stride*4);
					else
						gptr += (height-1)*(Info.Stride*3);

					fptr = FreeImage_GetBits(Fbmp32);

					for(int y=0; y<Info.Height; y++)
					{
						if(nFormat == GE_PIXELFORMAT_32BIT_BGRA)
						{
							__asm
							{
								mov esi, fptr
								mov edi, gptr
								mov ecx, width
								rep movsd
							}

							gptr -= (Info.Stride*4);
						}
						else
						{
							int nTemp = width * 3;
							__asm
							{
								mov esi, fptr
								mov edi, gptr
								mov ecx, nTemp
								rep movs
							}

							gptr -= (Info.Stride*3);
						}

						fptr += FStride;
					}

					geBitmap_UnLock(LockedBMP);
				}
				else
				{
					geBitmap_Destroy(&Bmp);
					Bmp = NULL;
				}
			}
// 09/30/2003 Wendell Buckner
// updated freeimage library for Show save game image add Nout
/*			FreeImage_Free(Fbmp32);
			FreeImage_Free(Fbmp); */
			FreeImage_Unload(Fbmp32);
			FreeImage_Unload(Fbmp);
		}
	}

	geVFile_Close(File);

	return Bmp;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
long DLL_CALLCONV VFS_Tell(fi_handle handle)
{
	long position;
	geVFile *File = (geVFile *)handle;
	geVFile_Tell(File, &position);
	return position;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
int DLL_CALLCONV VFS_Seek(fi_handle handle, long offset, int origin)
{
	geVFile *File = (geVFile *)handle;
	geVFile_Whence whence;

	if(origin == SEEK_SET)
		whence = GE_VFILE_SEEKSET;
	else if(origin == SEEK_CUR)
		whence = GE_VFILE_SEEKCUR;
	else
		whence = GE_VFILE_SEEKEND;

	if(geVFile_Seek(File, (int)offset, whence) == GE_TRUE)
		return 0;
	else
		return 1;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
unsigned DLL_CALLCONV VFS_Read(void *buffer, unsigned size, unsigned count, fi_handle handle)
{
	geVFile *File = (geVFile *)handle;
	unsigned csize = size*count;
	geVFile_Read(File, buffer, (int)csize);
	return csize;
}

// Weapon
/* ------------------------------------------------------------------------------------ */
// Routines to calculate bouncing off walls
/* ------------------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------------------ */
// Use this function to calculate the "range" travelled before the collision.
//
// Returns a number between 0 .. 1.0f indicating the fraction of the path from
// a_OldPos to a_NewPos that was actually travelled.
//
// NB: based on comment in sourcecode of GTest.
/* ------------------------------------------------------------------------------------ */
void CollisionCalcRatio(const GE_Collision &a_Collision, const geVec3d &a_OldPos,
                        const geVec3d &a_NewPos, float *a_Ratio)
{
	float fd = geVec3d_DotProduct(&a_OldPos, &(a_Collision.Plane.Normal)) - a_Collision.Plane.Dist;
	float bd = geVec3d_DotProduct(&a_NewPos, &(a_Collision.Plane.Normal)) - a_Collision.Plane.Dist;

	if(fd == bd)
		*a_Ratio = 0.0f;
	else
		*a_Ratio = fd / (fd - bd);
}

/* ------------------------------------------------------------------------------------ */
//	Calculates the point of impact based on the collision results.
/* ------------------------------------------------------------------------------------ */
void CollisionCalcImpactPoint(const GE_Collision &a_Collision, const geVec3d &a_OldPos,
                              const geVec3d &a_NewPos, float a_MinDistanceFromPlane,
                              float a_Ratio, geVec3d *a_Impact)
{
	geVec3d vecTravelled ;
	geVec3d vecPath ;

	geVec3d_Subtract(&a_NewPos, &a_OldPos, &vecPath);
	geVec3d_Scale(&vecPath, a_Ratio, &vecTravelled);
	geVec3d_Add(&a_OldPos, &vecTravelled, a_Impact);

	if(a_MinDistanceFromPlane != 0.0f)
		geVec3d_AddScaled(a_Impact, &(a_Collision.Plane.Normal), a_MinDistanceFromPlane, a_Impact) ;
}

/* ------------------------------------------------------------------------------------ */
//	SqueezeVector
/* ------------------------------------------------------------------------------------ */
static void SqueezeVector(geVec3d *Vect, float Epsilon)
{
	if (Vect->X > -Epsilon && Vect->X < Epsilon)
		Vect->X = 0.0f;

	if (Vect->Y > -Epsilon && Vect->Y < Epsilon)
		Vect->Y = 0.0f;

	if (Vect->Z > -Epsilon && Vect->Z < Epsilon)
		Vect->Z = 0.0f;
}

/* ------------------------------------------------------------------------------------ */
//	ReflectVelocity
/* ------------------------------------------------------------------------------------ */
static void ReflectVelocity(geVec3d *In, const geVec3d *Normal, geVec3d *Out, float Scale)
{
	float	Reflect;

	Reflect = geVec3d_DotProduct(In, Normal) * Scale;

	Out->X = In->X - Normal->X*Reflect;
	Out->Y = In->Y - Normal->Y*Reflect;
	Out->Z = In->Z - Normal->Z*Reflect;

	SqueezeVector(Out, 0.1f);
}

/* ------------------------------------------------------------------------------------ */
// Calculates the velocity *after* the collision (a_NewVelocity).
/* ------------------------------------------------------------------------------------ */
void CollisionCalcVelocityImpact(const GE_Collision &a_Collision, const geVec3d &a_OldVelocity,
                                 float a_Elasticity, float a_Friction, geVec3d *a_NewVelocity)
{
	geVec3d normalizedVelocity;
	float length;

	// we do this using normalized velocity vector
	// so we have to save its length
	normalizedVelocity = a_OldVelocity;
	length = geVec3d_Normalize(&normalizedVelocity);

	// reflect normalized velocity against the plane
	ReflectVelocity(&normalizedVelocity, &a_Collision.Plane.Normal, &normalizedVelocity, a_Elasticity);

	// new velocity = scaled( reflected normalized velocity )
	geVec3d_Scale(&normalizedVelocity, length * a_Friction, a_NewVelocity);
}

/* ------------------------------------------------------------------------------------ */
//	Extract
/* ------------------------------------------------------------------------------------ */
geVec3d Extract(char *Vector)
{
	geVec3d values = {0.0f, 0.0f, 0.0f};
	char *temp = NULL;

	temp = strtok(Vector, " ");

	if(temp)
	{
// changed RF063 - atoi -> atof
		values.X = (float)atof(temp);
		temp = strtok(NULL," \n");

		if(temp)
		{
			values.Y = (float)atof(temp);
			temp = strtok(NULL," \n");

			if(temp)
			{
				values.Z = (float)atof(temp);
			}
// end change RF063
		}
	}

	return values;
}

/* ------------------------------------------------------------------------------------ */
//	Ang2Vec
/* ------------------------------------------------------------------------------------ */
void Ang2Vec(float ang, geVec3d *vec)
{
	vec->X = (float)cos(ang);
	vec->Z = (float)sin(ang);
	vec->Y = 0.0f;

	SqueezeVector(vec, 0.0001f);

	geVec3d_Normalize(vec);
}

/* ------------------------------------------------------------------------------------ */
//	CanSeePointToPoint
/* ------------------------------------------------------------------------------------ */
geBoolean CanSeePointToPoint(geVec3d *Pos1, geVec3d *Pos2)
{
	GE_Collision Collision;
    int32 Leaf1, Leaf2;
	geVec3d RayMins = {-1.0f, -1.0f, -1.0f}, RayMaxs = {1.0f, 1.0f, 1.0f};

	geWorld_GetLeaf(CCD->World(), Pos1, &Leaf1);
	geWorld_GetLeaf(CCD->World(), Pos2, &Leaf2);

	if(geWorld_LeafMightSeeLeaf(CCD->World(), Leaf1, Leaf2, 0))
		return !geWorld_Collision(CCD->World(), &RayMins, &RayMaxs, Pos1, Pos2, GE_CONTENTS_CANNOT_OCCUPY, GE_COLLIDE_MODELS, 0, NULL, NULL, &Collision);

	return(GE_FALSE);
}

/* ------------------------------------------------------------------------------------ */
//	CanSeeActorToPoint
/* ------------------------------------------------------------------------------------ */
geBoolean CanSeeActorToPoint(geActor *Actor, geVec3d *Pos2)
{
	geExtBox theBox;
	geVec3d thePosition;

	CCD->ActorManager()->GetBoundingBox(Actor, &theBox);
	CCD->ActorManager()->GetPosition(Actor, &thePosition);
	thePosition.Y += (theBox.Max.Y*0.75f);
	return CanSeePointToPoint(&thePosition, Pos2);
}

/* ------------------------------------------------------------------------------------ */
//	CanSeePointToActor
/* ------------------------------------------------------------------------------------ */
geBoolean CanSeePointToActor(geVec3d *Pos2, geActor *Actor)
{
	geExtBox theBox;
	geVec3d thePosition;

	CCD->ActorManager()->GetBoundingBox(Actor, &theBox);
	CCD->ActorManager()->GetPosition(Actor, &thePosition);
	thePosition.Y += (theBox.Max.Y*0.5f);
	return CanSeePointToPoint(Pos2, &thePosition);
}

/* ------------------------------------------------------------------------------------ */
//	CanSeeActorToActor
/* ------------------------------------------------------------------------------------ */
geBoolean CanSeeActorToActor(geActor *Actor1, geActor *Actor2)
{
	geExtBox theBox;
	geVec3d thePosition1, thePosition2;
// changed RF064
	geBoolean flag;

	CCD->ActorManager()->GetBoundingBox(Actor1, &theBox);
	CCD->ActorManager()->GetPosition(Actor1, &thePosition1);

	thePosition1.Y += (theBox.Max.Y*0.75f);

	CCD->ActorManager()->GetBoundingBox(Actor2, &theBox);
	CCD->ActorManager()->GetPosition(Actor2, &thePosition2);

	flag = CanSeePointToPoint(&thePosition1, &thePosition2);

	thePosition2.Y += (theBox.Max.Y*0.37f);

	flag |= CanSeePointToPoint(&thePosition1, &thePosition2);

	thePosition2.Y += (theBox.Max.Y*0.37f);

	return (flag | CanSeePointToPoint(&thePosition1, &thePosition2));
// end change RF064
}

// changed RF064
/* ------------------------------------------------------------------------------------ */
// Get actor from entity name
//
// added by QD
/* ------------------------------------------------------------------------------------ */
geActor * GetEntityActor(char *EntityName)
{
	if(!EffectC_IsStringNull(EntityName))
	{
		if(!stricmp(EntityName, "Player"))
			return CCD->Player()->GetActor();

		char *EntityType = CCD->EntityRegistry()->GetEntityType(EntityName);

		if(EntityType)
		{
			if(!stricmp(EntityType, "StaticEntityProxy"))
			{
				StaticEntityProxy *pProxy;
				CCD->Props()->LocateEntity(EntityName, (void**)&pProxy);

				if(pProxy->Actor)
					return pProxy->Actor;

				return NULL;
			}

			if(!stricmp(EntityType, "Attribute"))
			{
				Attribute *pProxy;
				CCD->Attributes()->LocateEntity(EntityName, (void**)&pProxy);

				if(pProxy->Actor)
					return pProxy->Actor;

				return NULL;
			}

			if(!stricmp(EntityType, "Pawn"))
			{
				Pawn *pProxy;
				CCD->Pawns()->LocateEntity(EntityName, (void**)&pProxy);
				ScriptedObject *Object = (ScriptedObject*)pProxy->Data;

				if(Object->Actor)
					return Object->Actor;

				return NULL;
			}
		}
	}

	return NULL;
}

/* ------------------------------------------------------------------------------------ */
//	geVec3d_IsZero
/* ------------------------------------------------------------------------------------ */
bool geVec3d_IsZero(geVec3d *pVect)
{
	if( pVect->X < 0.0f || pVect->X > 0.0f ||
		pVect->Y < 0.0f || pVect->Y > 0.0f ||
		pVect->Z < 0.0f || pVect->Z > 0.0f )
		return false;

	return true;
}

/* ------------------------------------------------------------------------------------ */
//	Length
/* ------------------------------------------------------------------------------------ */
geFloat Length(geVec3d &vec)
{
// changed QD 12/15/05 - length can't be negative (sqrt!)
/*	geFloat Len = geVec3d_Length(&vec);

	if(Len < 0)
		Len *= -1;

	return Len;
*/
	return geVec3d_Length(&vec);
}

/* ------------------------------------------------------------------------------------ */
//	SetEnvironmentMapping
/* ------------------------------------------------------------------------------------ */
void SetEnvironmentMapping(geActor *Actor, bool Enable, bool AllMaterial, float Percent, float PercentMaterial)
{
	geEnvironmentOptions Options;
	Options.UseEnvironmentMapping = Enable;
	Options.PercentPuppet = PercentMaterial*0.01f;// /100.0f;

	if(!Enable)
	{
		Options.Supercede = false;
		Options.PercentEnvironment = 1.0f;
		Options.PercentMaterial = 1.0f;
	}
	else
	{
		if(!AllMaterial)
		{
			Options.Supercede = true;
			Options.PercentMaterial = Percent*0.01f;// /100.0f;
			Options.PercentEnvironment = 1.0f;
		}
		else
		{
			Options.Supercede = false;
			Options.PercentEnvironment = Percent*0.01f;// /100.0f;
			Options.PercentMaterial = 1.0f;
		}
	}

	geActor_SetEnvironOptions(Actor, &Options);
}
// end change RF064

/* ------------------------------------------------------------------------------------ */
//	RootBoneName
/* ------------------------------------------------------------------------------------ */
const char *RootBoneName(geActor *Actor)
{
	geXForm3d XForm;
	const char *RootBone;
	int ParentBoneIndex;
	geBody *Body;
	geActor_Def *ActorDef = geActor_GetActorDef(Actor);

	Body = geActor_GetBody(ActorDef);
	geBody_GetBone(Body, 0, &RootBone, &XForm, &ParentBoneIndex);
	return RootBone;
}


/* ----------------------------------- END OF FILE ------------------------------------ */
