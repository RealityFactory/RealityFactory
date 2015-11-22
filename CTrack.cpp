#ifdef RF063

#include "RabidFramework.h"

extern geBitmap *TPool_Bitmap(char *DefaultBmp, char *DefaultAlpha, char *BName, char *AName);

CTrackPoint::CTrackPoint()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	//	Ok, check to see if there are TrackPoints in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "TrackPoint");
	
	if(pSet) 
	{
		//	Ok, we have TrackPoints somewhere.  Dig through 'em all.
		
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
			TrackPoint *pSource = (TrackPoint*)geEntity_GetUserData(pEntity);
			if(EffectC_IsStringNull(pSource->szEntityName))
			{
				char szName[128];
				geEntity_GetName(pEntity, szName, 128);
				pSource->szEntityName = szName;
			}
			// Ok, put this entity into the Global Entity Registry
			CCD->EntityRegistry()->AddEntity(pSource->szEntityName, "TrackPoint");
		}
	}
}

CTrackPoint::~CTrackPoint()
{
}

//	******************** CRGF Overrides ********************

//	LocateEntity
//
//	Given a name, locate the desired item in the currently loaded level
//	..and return it's user data.

int CTrackPoint::LocateEntity(char *szName, void **pEntityData)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, check to see if there are TrackPoints in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "TrackPoint");
	
	if(!pSet) 
		return RGF_NOT_FOUND;
	
	//	Ok, we have TrackPoints.  Dig through 'em all.
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		TrackPoint *pTheEntity = (TrackPoint*)geEntity_GetUserData(pEntity);
		if(strcmp(pTheEntity->szEntityName, szName) == 0)
		{
			*pEntityData = (void *)pTheEntity;
			return RGF_SUCCESS;
		}
	}
	
	return RGF_NOT_FOUND;								// Sorry, no such entity here
}




CTrackStart::CTrackStart()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	//	Ok, check to see if there are TrackStarts in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "TrackStart");
	
	if(pSet) 
	{
		//	Ok, we have TrackStarts somewhere.  Dig through 'em all.
		
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
			TrackStart *pSource = (TrackStart*)geEntity_GetUserData(pEntity);
			if(EffectC_IsStringNull(pSource->szEntityName))
			{
				char szName[128];
				geEntity_GetName(pEntity, szName, 128);
				pSource->szEntityName = szName;
			}
			// Ok, put this entity into the Global Entity Registry
			CCD->EntityRegistry()->AddEntity(pSource->szEntityName, "TrackStart");
			char *EntityName = pSource->TrackPointName;
			while(!EffectC_IsStringNull(EntityName))
			{
				bool error = true;
				char *EntityType = CCD->EntityRegistry()->GetEntityType(EntityName);
				if(EntityType)
				{
					if(!stricmp(EntityType, "TrackPoint"))
					{
						TrackPoint *pProxy;
						CCD->TrackPoints()->LocateEntity(EntityName, (void**)&pProxy);
						EntityName = pProxy->NextTrackPoint;
						error = false;
					}
					else if(!stricmp(EntityType, "TrackStart"))
					{
						if(!stricmp(EntityName, pSource->szEntityName))
							break;
					}
				}
				if(error)
				{
					char szError[256];
					sprintf(szError,"TrackStart : Invalid TrackPoint '%s'", EntityName);
					CCD->ReportError(szError, false);
					CCD->ShutdownLevel();
					delete CCD;
					MessageBox(NULL, szError,"Invalid TrackPoint", MB_OK);
					exit(-333);
				}
			}
		}
		TrackStartTexture = TPool_Bitmap("corona.bmp", "corona.bmp", NULL, NULL);
	}
}

CTrackStart::~CTrackStart()
{
}

//	******************** CRGF Overrides ********************

//	LocateEntity
//
//	Given a name, locate the desired item in the currently loaded level
//	..and return it's user data.

int CTrackStart::LocateEntity(char *szName, void **pEntityData)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, check to see if there are TrackStarts in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "TrackStart");
	
	if(!pSet) 
		return RGF_NOT_FOUND;
	
	//	Ok, we have TrackStarts.  Dig through 'em all.
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		TrackStart *pTheEntity = (TrackStart*)geEntity_GetUserData(pEntity);
		if(strcmp(pTheEntity->szEntityName, szName) == 0)
		{
			*pEntityData = (void *)pTheEntity;
			return RGF_SUCCESS;
		}
	}
	
	return RGF_NOT_FOUND;								// Sorry, no such entity here
}


void CTrackStart::DrawLine3d(const geVec3d *p1, const geVec3d *p2, int r, int g, int b, int r1, int g1, int b1)
{
	GE_LVertex	v[4];
	int			i;
	geVec3d		perp;
	geVec3d		in;
	
	geXForm3d M = CCD->CameraManager()->ViewPoint();
	
	v[0].u = 0.0f;
	v[0].v = 0.0f;
	v[1].u = 1.0f;
	v[1].v = 0.0f;
	v[2].u = 1.0f;
	v[2].v = 1.0f;
	v[3].u = 0.0f;
	v[3].v = 1.0f;
	
	for	(i = 0; i < 4; i++)
	{
		v[i].r = (geFloat)r;
		v[i].g = (geFloat)g;
		v[i].b = (geFloat)b;
		v[i].a = 255.0f;
	}
	
	geVec3d_Subtract(p1, p2, &perp);
	geXForm3d_GetIn(&M, &in);
	geVec3d_CrossProduct(&perp, &in, &perp);
	geVec3d_Normalize(&perp);
	geVec3d_Scale(&perp, 2.0f / 2.0f, &perp);
	
	v[0].X = p2->X - perp.X;
	v[0].Y = p2->Y - perp.Y;
	v[0].Z = p2->Z - perp.Z;
	v[1].X = p2->X + perp.X;
	v[1].Y = p2->Y + perp.Y;
	v[1].Z = p2->Z + perp.Z;
	v[2].X = p1->X + perp.X;
	v[2].Y = p1->Y + perp.Y;
	v[2].Z = p1->Z + perp.Z;
	v[3].X = p1->X - perp.X;
	v[3].Y = p1->Y - perp.Y;
	v[3].Z = p1->Z - perp.Z;
	
	v[0].r = (float)r1;
	v[0].g = (float)g1;
	v[0].b = (float)b1;
	v[1].r = (float)r1;
	v[1].g = (float)g1;
	v[1].b = (float)b1;
	v[2].r = (float)r1;
	v[2].g = (float)g1;
	v[2].b = (float)b1;
	v[3].r = (float)r1;
	v[3].g = (float)g1;
	v[3].b = (float)b1;
	
	geWorld_AddPolyOnce(CCD->World(),
		v,
		4,
		NULL,
		GE_GOURAUD_POLY,
		GE_RENDER_DO_NOT_OCCLUDE_OTHERS,
		1.0f);
}

void CTrackStart::Render()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	geVec3d sp, ep;

	//	Ok, check to see if there are TrackStarts in this world
	
	pSet = geWorld_GetEntitySet(CCD->World(), "TrackStart");
	
	if(pSet) 
	{
		//	Ok, we have TrackStarts somewhere.  Dig through 'em all.
		
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
			TrackStart *pSource = (TrackStart*)geEntity_GetUserData(pEntity);
			char *EntityName = pSource->TrackPointName;
			sp = pSource->origin;
			while(!EffectC_IsStringNull(EntityName))
			{
				char *EntityType = CCD->EntityRegistry()->GetEntityType(EntityName);
				if(EntityType)
				{
					if(!stricmp(EntityType, "TrackPoint"))
					{
						TrackPoint *pProxy;
						CCD->TrackPoints()->LocateEntity(EntityName, (void**)&pProxy);
						ep = pProxy->origin;
						EntityName = pProxy->NextTrackPoint;
					}
					else if(!stricmp(EntityType, "TrackStart"))
					{
						ep = pSource->origin;
						EntityName = NULL;
					}
					DrawLine3d(&sp,&ep,255, 255, 255,0, 0, 255);

				}

			}
		}
	}
}

#endif