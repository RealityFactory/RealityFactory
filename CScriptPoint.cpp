/****************************************************************************************/
/*																						*/
/*	CScriptPoint.cpp:		ScriptPoint class											*/
/*																						*/
/*																						*/
/****************************************************************************************/

#include "RabidFramework.h"

extern geBitmap *TPool_Bitmap(char *DefaultBmp, char *DefaultAlpha, char *BName, char *AName);

/* ------------------------------------------------------------------------------------ */
//	Constructor
/* ------------------------------------------------------------------------------------ */
CScriptPoint::CScriptPoint()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	// Ok, check to see if there are ScriptPoints in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "ScriptPoint");

	if(!pSet)
		return;

	// Ok, we have ScriptPoints somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		ScriptPoint *pSource = (ScriptPoint*)geEntity_GetUserData(pEntity);

		if(EffectC_IsStringNull(pSource->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pSource->szEntityName = szName;
		}

		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(pSource->szEntityName, "ScriptPoint");
	}

	Texture = TPool_Bitmap("corona.bmp", "corona.bmp", NULL, NULL);
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
/* ------------------------------------------------------------------------------------ */
CScriptPoint::~CScriptPoint()
{
}

/* ------------------------------------------------------------------------------------ */
//	LocateEntity
//
//	Given a name, locate the desired item in the currently loaded level
//	..and return it's user data.
/* ------------------------------------------------------------------------------------ */
int CScriptPoint::LocateEntity(char *szName, void **pEntityData)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	// Ok, check to see if there are ScriptPoints in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "ScriptPoint");

	if(!pSet)
		return RGF_NOT_FOUND;

	// Ok, we have ScriptPoints.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		ScriptPoint *pTheEntity = (ScriptPoint*)geEntity_GetUserData(pEntity);

		if(!strcmp(pTheEntity->szEntityName, szName))
		{
			*pEntityData = (void*)pTheEntity;
			return RGF_SUCCESS;
		}
	}

	return RGF_NOT_FOUND;	// Sorry, no such entity here
}

/* ------------------------------------------------------------------------------------ */
//	DrawLine3d
/* ------------------------------------------------------------------------------------ */
void CScriptPoint::DrawLine3d(const geVec3d *p1, const geVec3d *p2,
							  int r, int g, int b, int r1, int g1, int b1)
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

	for(i=0; i<4; i++)
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
// changed QD 12/15/05
// does nothing at all - zap it
	// geVec3d_Scale(&perp, 2.0f / 2.0f, &perp);

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

/* ------------------------------------------------------------------------------------ */
//	Render
/* ------------------------------------------------------------------------------------ */
void CScriptPoint::Render()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	geVec3d sp, ep;
	GE_Collision	Collision;
	GE_LVertex	Vert;
	geXForm3d XForm = CCD->CameraManager()->ViewPoint();

	SetOrigin();

	// Ok, check to see if there are ScriptPoints in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "ScriptPoint");

	if(pSet)
	{
		// Ok, we have ScriptPoints somewhere.  Dig through 'em all.
		for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
			pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
		{
			ScriptPoint *pSource = (ScriptPoint*)geEntity_GetUserData(pEntity);
			char *EntityName = pSource->NextPoint;
			sp = pSource->origin;

			if(!geWorld_Collision(CCD->World(),NULL, NULL,&sp,&XForm.Translation,
				GE_CONTENTS_CANNOT_OCCUPY,GE_COLLIDE_MODELS,0xffffffff, NULL, NULL,
				&Collision))
			{
				Vert.X = sp.X;
				Vert.Y = sp.Y;
				Vert.Z = sp.Z;

				Vert.r = 255.0f;
				Vert.g = 0.0f;
				Vert.b = 0.0f;
				Vert.a = 255.0f;
				Vert.u = Vert.v = 0.0f;

				geWorld_AddPolyOnce(CCD->World(),
					&Vert,
					1,
					Texture,
					GE_TEXTURED_POINT,
					GE_RENDER_DO_NOT_OCCLUDE_OTHERS | GE_RENDER_DO_NOT_OCCLUDE_SELF,
					0.05f);
			}

			if(!EffectC_IsStringNull(EntityName))
			{
				char *EntityType = CCD->EntityRegistry()->GetEntityType(EntityName);

				if(EntityType)
				{
					if(!stricmp(EntityType, "ScriptPoint"))
					{
						ep = GetOrigin(EntityName);
						DrawLine3d(&sp, &ep, 255, 255, 255, 0, 0, 255);
					}
				}
			}
		}
	}

	FreeOrigin();
}

/* ------------------------------------------------------------------------------------ */
//	SetOrigin
/* ------------------------------------------------------------------------------------ */
void CScriptPoint::SetOrigin()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	SPOrigin *pool;

	Bottom = (SPOrigin*)NULL;

	// Ok, check to see if there are ScriptPoints in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "ScriptPoint");

	if(!pSet)
		return;

	// Ok, we have ScriptPoints somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		ScriptPoint *pSource = (ScriptPoint*)geEntity_GetUserData(pEntity);

		if(EffectC_IsStringNull(pSource->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pSource->szEntityName = szName;
		}

		pool = GE_RAM_ALLOCATE_STRUCT(SPOrigin);
		memset(pool, 0x0, sizeof(SPOrigin));
		pool->next = Bottom;
		Bottom = pool;

		if(pool->next)
			pool->next->prev = pool;

		pool->Name = strdup(pSource->szEntityName);
		pool->origin = pSource->origin;
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
//	GetOrigin
/* ------------------------------------------------------------------------------------ */
geVec3d CScriptPoint::GetOrigin(char *Name)
{
	SPOrigin *pool;
	geVec3d zero = {0.0f, 0.0f, 0.0f};

	char *EntityType = CCD->EntityRegistry()->GetEntityType(Name);

	if(EntityType)
	{
		if(!stricmp(EntityType, "ScriptPoint"))
		{
			pool = Bottom;

			while(pool != NULL)
			{
				if(!stricmp(Name, pool->Name))
					return pool->origin;

				pool = pool->next;
			}

			char szError[256];
			sprintf(szError, "*WARNING* Invalid ScriptPoint Name '%s'\n", Name);
			CCD->ReportError(szError, false);
			return zero;
		}
	}

	return zero;
}

/* ------------------------------------------------------------------------------------ */
//	FreeOrigin
/* ------------------------------------------------------------------------------------ */
void CScriptPoint::FreeOrigin()
{
	SPOrigin *pool, *temp;
	pool = Bottom;

	while(pool != NULL)
	{
		temp = pool->next;
		free(pool->Name);
		geRam_Free(pool);
		pool = temp;
	}
}


/* ----------------------------------- END OF FILE ------------------------------------ */
