/************************************************************************************//**
 * @file CStaticMesh.cpp
 * @brief Implementation of the CStaticMesh class
 * @author Daniel Queteschiner
 *//*
 * Copyright (c) 2004 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

//	Include the One True Header
#include "RabidFramework.h"
#include <body._h>
extern "C" void	DrawBoundBox(geWorld *World, const geVec3d *Pos, const geVec3d *Min, const geVec3d *Max);

// changed QD 09/28/2004
#define LIGHT_SUNLIGHT		0
#define LIGHT_POINTLIGHT	1
#define LIGHT_SPOTLIGHT		2

// light entity
typedef struct	tag_light
{
	int		light;
	GE_RGBA	color;
	int		style;
	geVec3d	origin;

}	light;

// spotlight entity
typedef struct	tag_spotlight
{
	int		DummyRadius;
	int		light;
	GE_RGBA	color;
	int		style;
	geVec3d	origin;
	geVec3d	angles;
	int		arc;

}	spotlight;
// end change


/* ------------------------------------------------------------------------------------ */
// Construction/Destruction
/* ------------------------------------------------------------------------------------ */
CStaticMesh::CStaticMesh()
{
	m_MeshCount = 0;

	for(int nTemp = 0; nTemp < MESH_LIST_SIZE; nTemp++)
		MeshList[nTemp] = NULL;

	// Ok, see if we have any static entity proxies we need to set up.
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "StaticMesh");

	if(!pSet)
		return;				// Don't waste CPU time.

	geEntity *pEntity;

	//	Ok, we have static mesh entities somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		StaticMesh *pMesh = static_cast<StaticMesh*>(geEntity_GetUserData(pEntity));

		if(EffectC_IsStringNull(pMesh->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pMesh->szEntityName = szName;
		}

		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(pMesh->szEntityName, "StaticMesh");

		int Index = 0;

		// let's check if we have this actor's poly already in the MeshList
		if(!IsInList(pMesh->szActorFile, &Index))
		{
			if(CCD->FileExist(kActorFile, pMesh->szActorFile))
			{
				// add a new entry to the meshlist
				if(!AddNewMesh(pMesh->szActorFile))
				{
					// changed QD 12/15/05
					CleanUp();
					// end change
					char szError[256];
					sprintf(szError, "File %s - Line %d: %s: AddNewMesh '%s' failed",
							__FILE__, __LINE__, pMesh->szEntityName, pMesh->szActorFile);
					CCD->ReportError(szError, false);
					CCD->ShutdownLevel();
					delete CCD;
					CCD = NULL;
					MessageBox(NULL, szError,"StaticMesh", MB_OK);
					exit(-333);
				}

				pMesh->ListIndex = m_MeshCount;
				++m_MeshCount;
			}
			// the file doesn't exist, report error and shutdown
			else
			{
				// changed QD 12/15/05
				CleanUp();
				// end change
				char szError[256];
				sprintf(szError, "File %s - Line %d: %s: file '%s' doesn't exist",
						__FILE__, __LINE__, pMesh->szEntityName, pMesh->szActorFile);
				CCD->ReportError(szError, false);
				CCD->ShutdownLevel();
				delete CCD;
				CCD = NULL;
				MessageBox(NULL, szError,"StaticMesh", MB_OK);
				exit(-333);
			}
		}
		else
			pMesh->ListIndex = Index;

// changed QD 09/28/2004
/*		if(pMesh->CompSunLightMin>100)
			pMesh->CompSunLightMin=100;
		if(pMesh->CompSunLightMin<0)
			pMesh->CompSunLightMin=0;*/

		pMesh->ColorLOD0 = NULL;
		pMesh->ColorLOD1 = NULL;
		pMesh->ColorLOD2 = NULL;
		pMesh->ColorLOD3 = NULL;

		// validate the alpha value
		if(pMesh->Alpha > 255.0f)
			pMesh->Alpha = 255.0f;
		else if(pMesh->Alpha < 0.0f)
			pMesh->Alpha = 0.0f;

		pMesh->RenderFlags = 0;//GE_RENDER_BACKFACED; //flag has no effect

		if(pMesh->ZSort)
			pMesh->RenderFlags |= GE_RENDER_DEPTH_SORT_BF;

		geVec3d_Scale(&(pMesh->ActorRotation), GE_PIOVER180, &(pMesh->ActorRotation));

		pMesh->OriginOffset = pMesh->origin;
		pMesh->Rotation = pMesh->ActorRotation;

		if(pMesh->Model)
		{
            geVec3d ModelOrigin;
	    	geWorld_GetModelRotationalCenter(CCD->World(), pMesh->Model, &ModelOrigin);
            geVec3d_Subtract(&(pMesh->origin), &ModelOrigin, &(pMesh->OriginOffset));
  		}
	}

// changed QD 09/28/2004
// compute static lighting for the static meshes

	geEntity_EntitySet *pSetLight, *pSetSpotlight, *pSetSunLight;
	geEntity *pEntityLight, *pEntitySpotlight, *pEntitySunLight;

	// find sunlight entity for lighting computation
	pSetSunLight = geWorld_GetEntitySet(CCD->World(), "SunLight");

	// lighting from static omni-light
	pSetLight = geWorld_GetEntitySet(CCD->World(), "light");

	// lighting from static spotlights
	pSetSpotlight= geWorld_GetEntitySet(CCD->World(), "spotlight");

	// if there's no light at all return -
	// no ambientlight is used if there's no light -> use fillcolor instead
	if(!pSetSunLight && !pSetLight && !pSetSpotlight)
		return;

	geBoolean bSunLight, bLight, bSpotlight, bAmbientLight;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		StaticMesh *pMesh = static_cast<StaticMesh*>(geEntity_GetUserData(pEntity));

		bSunLight =	bLight = bSpotlight	= bAmbientLight = GE_FALSE;

		if(pSetSunLight && pMesh->CompSunLight)
			bSunLight = GE_TRUE;
		if(pSetLight && pMesh->CompLight)
			bLight = GE_TRUE;
		if(pSetSpotlight && pMesh->CompLight)
			bSpotlight = GE_TRUE;

		if((pMesh->AmbientColor.r > 0.f || pMesh->AmbientColor.g > 0.f || pMesh->AmbientColor.b > 0.f) &&
			(pMesh->CompSunLight || pMesh->CompLight))
			bAmbientLight = GE_TRUE;

		geBoolean bLights = (bSunLight || bLight || bSpotlight);

		if(bLights || bAmbientLight)
		{
			// alocate color-array
			pMesh->ColorLOD0 = (GE_RGBA*)malloc(sizeof(GE_RGBA)*3*(MeshList[pMesh->ListIndex]->NumFaces[0]));
			if(!pMesh->ColorLOD0)
				continue;
			memset(pMesh->ColorLOD0, 0, sizeof(GE_RGBA)*3*(MeshList[pMesh->ListIndex]->NumFaces[0]));

			if(MeshList[pMesh->ListIndex]->Verts[1])
			{
				pMesh->ColorLOD1 = (GE_RGBA*)malloc(sizeof(GE_RGBA)*3*(MeshList[pMesh->ListIndex]->NumFaces[1]));
				if(!pMesh->ColorLOD1)
				{
					free(pMesh->ColorLOD0);
					continue;
				}
				memset(pMesh->ColorLOD1, 0, sizeof(GE_RGBA)*3*(MeshList[pMesh->ListIndex]->NumFaces[1]));
			}
			if(MeshList[pMesh->ListIndex]->Verts[2])
			{
				pMesh->ColorLOD2 = (GE_RGBA*)malloc(sizeof(GE_RGBA)*3*(MeshList[pMesh->ListIndex]->NumFaces[2]));
				if(!pMesh->ColorLOD2)
				{
					free(pMesh->ColorLOD0);
					free(pMesh->ColorLOD1);
					continue;
				}
				memset(pMesh->ColorLOD2, 0, sizeof(GE_RGBA)*3*(MeshList[pMesh->ListIndex]->NumFaces[2]));
			}
			if(MeshList[pMesh->ListIndex]->Verts[3])
			{
				pMesh->ColorLOD3 = (GE_RGBA*)malloc(sizeof(GE_RGBA)*3*(MeshList[pMesh->ListIndex]->NumFaces[3]));
				if(!pMesh->ColorLOD3)
				{
					free(pMesh->ColorLOD0);
					free(pMesh->ColorLOD1);
					free(pMesh->ColorLOD2);
					continue;
				}
				memset(pMesh->ColorLOD3, 0, sizeof(GE_RGBA)*3*(MeshList[pMesh->ListIndex]->NumFaces[3]));
			}
		}
		else
			continue;

		if(bAmbientLight)
		{
			SetAmbientLight(pMesh);
			if(CCD->GetLogging())
			{
				char Info[512];
				sprintf(Info, "[INFO] Set Ambient light for %s", pMesh->szEntityName);
				CCD->ReportError(Info, false);
			}
		}

		if(bSunLight)
		{
			pEntitySunLight = geEntity_EntitySetGetNextEntity(pSetSunLight, NULL);
			SunLight *pSun = (SunLight*)geEntity_GetUserData(pEntitySunLight);
			ComputeLighting(pMesh, (void*)pSun, LIGHT_SUNLIGHT);
			if(CCD->GetLogging())
			{
				char Info[512];
				sprintf(Info, "[INFO] Set Sunlight light for %s", pMesh->szEntityName);
				CCD->ReportError(Info, false);
			}
		}

		if(bLight)
		{
			for(pEntityLight=geEntity_EntitySetGetNextEntity(pSetLight, NULL); pEntityLight;
				pEntityLight=geEntity_EntitySetGetNextEntity(pSetLight, pEntityLight))
			{
				light *pLight = (light*)geEntity_GetUserData(pEntityLight);
				ComputeLighting(pMesh, (void*)pLight, LIGHT_POINTLIGHT);
			}
			if(CCD->GetLogging())
			{
				char Info[512];
				sprintf(Info, "[INFO] Set Light for %s", pMesh->szEntityName);
				CCD->ReportError(Info, false);
			}
		}

		if(bSpotlight)
		{
			for(pEntitySpotlight=geEntity_EntitySetGetNextEntity(pSetSpotlight, NULL);pEntitySpotlight;
				pEntitySpotlight=geEntity_EntitySetGetNextEntity(pSetSpotlight, pEntitySpotlight))
			{
				spotlight *pSpotlight = (spotlight*)geEntity_GetUserData(pEntitySpotlight);
				ComputeLighting(pMesh, (void*)pSpotlight, LIGHT_SPOTLIGHT);
			}

			if(CCD->GetLogging())
			{
				char Info[512];
				sprintf(Info, "[INFO] Set Spotlight for %s", pMesh->szEntityName);
				CCD->ReportError(Info, false);
			}
		}

// changed QD 02/12/2005
		//if(pMesh->CompLightSmooth && bLights)
		//	FreeVertexNormals();
	}
// end change
}

/* ------------------------------------------------------------------------------------ */
//	dig through the meshlist to see if the szActorfile is already in there
//	if it's there set Index
//	in: filename
//	out: found?,  index
/* ------------------------------------------------------------------------------------ */
bool CStaticMesh::IsInList(const char *szActorFile, int *Index)
{
	for(int i=0; i<m_MeshCount; ++i)
	{
		if(!strcmp(MeshList[i]->szFilename, szActorFile))
		{
			*Index = i;
			return true;
		}
	}

	*Index=0;
	return false;
}

/* ------------------------------------------------------------------------------------ */
//	add a new entry to the meshlist
//	in: filename
//	out: success?
/* ------------------------------------------------------------------------------------ */
bool CStaticMesh::AddNewMesh(const char *szActorFile)
{
	MeshList[m_MeshCount] = new LoadedMeshList;
	memset(MeshList[m_MeshCount], 0, sizeof(LoadedMeshList));

	char LodName[128], Name[128];
	geBoolean CleanUp = GE_FALSE;

	for(int LOD=0; LOD<4; ++LOD)
	{
		geVFile *ActorFile = NULL;
		geActor *theActor = NULL;

		// build the actorfilename
		if(!LOD)
		{
			strcpy(Name, szActorFile);
			MeshList[m_MeshCount]->szFilename = new char[strlen(szActorFile)+1];
			memcpy(MeshList[m_MeshCount]->szFilename, szActorFile, strlen(szActorFile)+1);
		}
		else
		{
			int len = strlen(szActorFile);
			strncpy(LodName, szActorFile, len-4);
			LodName[len-4] = '\0';
			strcat(LodName, "_LOD");
			sprintf(Name, "%s%d.act", LodName, LOD);
		}

// changed QD 12/15/05
		if(!CCD->FileExist(kActorFile, Name))
		{
			if(!LOD)
			{
				if(MeshList[m_MeshCount]->szFilename != NULL)
					delete MeshList[m_MeshCount]->szFilename;

				delete MeshList[m_MeshCount];
				MeshList[m_MeshCount] = NULL;
				return false;
			}
			else
				continue;
		}
// end change

		//open the actor file
		CCD->OpenRFFile(&ActorFile, kActorFile, Name, GE_VFILE_OPEN_READONLY);

		geActor_Def *ActorDef = NULL;

		if(ActorFile)
		{
			// Create a definition of the actor
			ActorDef = geActor_DefCreateFromFile(ActorFile);

			if(!ActorDef)
			{
				geVFile_Close(ActorFile);							// Clean up in case of error
				char szError[256];
				sprintf(szError,"File %s - Line %d: Failed to create geActor_Def from file '%s'",
						__FILE__, __LINE__, Name);
				CCD->ReportError(szError, false);

				if(!LOD)
				{
					if(MeshList[m_MeshCount]->szFilename != NULL)
						delete MeshList[m_MeshCount]->szFilename;

					delete MeshList[m_MeshCount];
					MeshList[m_MeshCount] = NULL;
					return false;
				}
				else
					continue;
			}
		}
		else							// File didn't open, error out.
		{
			if(!LOD)
			{
				char szError[256];
				sprintf(szError,"File %s - Line %d: Failed to load actor '%s'",
						__FILE__, __LINE__, Name);
				CCD->ReportError(szError, false);

				if(MeshList[m_MeshCount]->szFilename != NULL)
					delete MeshList[m_MeshCount]->szFilename;

				delete MeshList[m_MeshCount];
				MeshList[m_MeshCount] = NULL;
				return false;
			}
			else
				continue;
		}

		geVFile_Close(ActorFile);

		theActor = geActor_Create(ActorDef);

		geXForm3d thePosition;
		geXForm3d_SetIdentity(&thePosition);

		geActor_ClearPose(theActor, &thePosition);

		MeshList[m_MeshCount]->Bitmaps[LOD] = NULL;
		MeshList[m_MeshCount]->Verts[LOD] = NULL;
		MeshList[m_MeshCount]->FaceI[LOD] = NULL;
		MeshList[m_MeshCount]->Color[LOD] = NULL;
		MeshList[m_MeshCount]->MaterialI[LOD] = NULL;
		// changed QD 02/12/2005
		MeshList[m_MeshCount]->VNormals[LOD] = NULL;
		MeshList[m_MeshCount]->VNormalI[LOD] = NULL;
		// end change


		// get the body information of this actor
		geBody *B = geActor_GetBody(ActorDef);

		if(!B)
		{
			CleanUp=GE_TRUE;

			char szError[256];
			sprintf(szError,"File %s - Line %d: Failed to get body from actor '%s'",
					__FILE__, __LINE__, Name);
			CCD->ReportError(szError, false);

			goto CLEAN_UP;
		}

		// extracting the data of the body
		{
			int i,j;
			geBody_Index BoneIndex;
			geXForm3d Transform;
			char BoneName[256];
			const char *BName;
			const char *MaterialName;
			geFloat Red, Green, Blue;

			// get the number of materials in the body
			MeshList[m_MeshCount]->MaterialCount[LOD] = geBody_GetMaterialCount(B);

			// allocate memory for the bitmaps
			MeshList[m_MeshCount]->Bitmaps[LOD] = (geBitmap**)malloc(sizeof(geBitmap*)*(MeshList[m_MeshCount]->MaterialCount[LOD]));
			if(!(MeshList[m_MeshCount]->Bitmaps[LOD]))
			{
				// error - clean up
				CleanUp = GE_TRUE;

				char szError[256];
				sprintf(szError,"File %s - Line %d: Failed to allocate bitmaps");
				CCD->ReportError(szError, false);

				goto CLEAN_UP;
			}

			// get the bitmaps
			for(i=0;i<(MeshList[m_MeshCount]->MaterialCount[LOD]);i++)
			{
				geBody_GetMaterial(B, i, &MaterialName, &(MeshList[m_MeshCount]->Bitmaps[LOD][i]),
									&Red, &Green, &Blue);

				// if this material has a bitmap, add it to the world
				if(MeshList[m_MeshCount]->Bitmaps[LOD][i])
				{
					if(!geWorld_AddBitmap(CCD->World(), MeshList[m_MeshCount]->Bitmaps[LOD][i]))
					{
						char szError[256];
						sprintf(szError, "File %s - Line %d: AddBitmap %s failed",
								__FILE__, __LINE__, MaterialName);
						CCD->ReportError(szError, false);
					}
				}
			}

			MeshList[m_MeshCount]->NumVerts[LOD] = B->XSkinVertexCount;
			// allocate memory for the vertexlist information
			MeshList[m_MeshCount]->Verts[LOD] = (GE_LVertex*)malloc(sizeof(GE_LVertex)*(MeshList[m_MeshCount]->NumVerts[LOD]));//NumFaces[LOD]));
			if(!MeshList[m_MeshCount]->Verts[LOD])
			{
				// error - clean up
				CleanUp = GE_TRUE;

				char szError[256];
				sprintf(szError, "File %s - Line %d: Failed to allocate vertices", __FILE__, __LINE__);
				CCD->ReportError(szError, false);

				goto CLEAN_UP;
			}

			// get the number of faces in the body
			MeshList[m_MeshCount]->NumFaces[LOD] = B->SkinFaces[0].FaceCount;
			// allocate memory for the vertexlist information, each face has 3 indices for its 3 vertices
			MeshList[m_MeshCount]->FaceI[LOD] = (int16*)malloc(sizeof(int16)*3*(MeshList[m_MeshCount]->NumFaces[LOD]));
			if(!MeshList[m_MeshCount]->FaceI[LOD])
			{
				// error - clean up
				CleanUp = GE_TRUE;

				char szError[256];
				sprintf(szError, "File %s - Line %d: Failed to allocate faceindices", __FILE__, __LINE__);
				CCD->ReportError(szError, false);

				goto CLEAN_UP;
			}

			// changed QD 02/12/2005
			// get the number of VertexNormals in the body
			MeshList[m_MeshCount]->NumVNormals[LOD] = B->SkinNormalCount;
			// allocate memory for the normalindices, each face has 3 indices for its 3 vertexnormals
			MeshList[m_MeshCount]->VNormalI[LOD] = (int16*)malloc(sizeof(int16)*3*(MeshList[m_MeshCount]->NumFaces[LOD]));
			if(!MeshList[m_MeshCount]->VNormalI[LOD])
			{
				// error - clean up
				CleanUp = GE_TRUE;

				char szError[256];
				sprintf(szError, "File %s - Line %d: Failed to allocate normalindices", __FILE__, __LINE__);
				CCD->ReportError(szError, false);

				goto CLEAN_UP;
			}
			// allocate memory for the normals
			MeshList[m_MeshCount]->VNormals[LOD] = (geVec3d*)malloc(sizeof(geVec3d)*(MeshList[m_MeshCount]->NumVNormals[LOD]));
			if(!MeshList[m_MeshCount]->VNormals[LOD])
			{
				// error - clean up
				CleanUp = GE_TRUE;

				char szError[256];
				sprintf(szError, "File %s - Line %d: Failed to allocate normals", __FILE__, __LINE__);
				CCD->ReportError(szError, false);

				goto CLEAN_UP;
			}
			// end change

			// allocate memory for the materialindices
			MeshList[m_MeshCount]->Color[LOD] = (GE_RGBA*)malloc(sizeof(GE_RGBA)*(MeshList[m_MeshCount]->NumFaces[LOD]));
			if(!MeshList[m_MeshCount]->Color[LOD])
			{
				// error - clean up
				CleanUp = GE_TRUE;

				char szError[256];
				sprintf(szError, "File %s - Line %d: Failed to allocate colorarray", __FILE__, __LINE__);
				CCD->ReportError(szError, false);

				goto CLEAN_UP;
			}

			// allocate memory for the materialindices
			MeshList[m_MeshCount]->MaterialI[LOD] = (int16*)malloc(sizeof(int16)*(MeshList[m_MeshCount]->NumFaces[LOD]));
			if(!MeshList[m_MeshCount]->MaterialI[LOD])
			{
				// error - clean up
				CleanUp = GE_TRUE;

				char szError[256];
				sprintf(szError, "File %s - Line %d: Failed to allocate materialindices", __FILE__, __LINE__);
				CCD->ReportError(szError, false);

				goto CLEAN_UP;
			}

			// now rip out the verts
			for(i=0;i<MeshList[m_MeshCount]->NumVerts[LOD];i++)//NumFaces[LOD];i++)
			{
				geVec3d Point;

				geVec3d_Copy(&(B->XSkinVertexArray[i].XPoint), &(Point));//s[i]));

				// transform vertex point by bone transformation
				BoneIndex = B->XSkinVertexArray[i].BoneIndex;
				BName = geStrBlock_GetString(B->BoneNames, BoneIndex);
				strcpy(BoneName, BName);
				geActor_GetBoneTransform(theActor, BoneName, &Transform);
				geXForm3d_Transform(&Transform, &(Point), &(Point));

				MeshList[m_MeshCount]->Verts[LOD][i].X = Point.X;
				MeshList[m_MeshCount]->Verts[LOD][i].Y = Point.Y;
				MeshList[m_MeshCount]->Verts[LOD][i].Z = Point.Z;

				// get the texture uv coordinates
				MeshList[m_MeshCount]->Verts[LOD][i].u=B->XSkinVertexArray[i].XU;
				MeshList[m_MeshCount]->Verts[LOD][i].v=B->XSkinVertexArray[i].XV;

				// give each vertex a default value for now
				MeshList[m_MeshCount]->Verts[LOD][i].r=128.0f;//B->MaterialArray[MeshList[m_MeshCount]->MaterialI[LOD][i]].Red;
				MeshList[m_MeshCount]->Verts[LOD][i].g=128.0f;//B->MaterialArray[MeshList[m_MeshCount]->MaterialI[LOD][i]].Green;
				MeshList[m_MeshCount]->Verts[LOD][i].b=128.0f;//B->MaterialArray[MeshList[m_MeshCount]->MaterialI[LOD][i]].Blue;
				MeshList[m_MeshCount]->Verts[LOD][i].a=255.0f;

				// get the maximum and minimum extension of the mesh
				if(!i)
				{
					MeshList[m_MeshCount]->Max[LOD].X = MeshList[m_MeshCount]->Min[LOD].X= Point.X;
					MeshList[m_MeshCount]->Max[LOD].Y = MeshList[m_MeshCount]->Min[LOD].Y= Point.Y;
					MeshList[m_MeshCount]->Max[LOD].Z = MeshList[m_MeshCount]->Min[LOD].Z= Point.Z;
				}
				else
				{
					if(MeshList[m_MeshCount]->Max[LOD].X < Point.X)
						MeshList[m_MeshCount]->Max[LOD].X = Point.X;
					if(MeshList[m_MeshCount]->Max[LOD].Y < Point.Y)
						MeshList[m_MeshCount]->Max[LOD].Y = Point.Y;
					if(MeshList[m_MeshCount]->Max[LOD].Z < Point.Z)
						MeshList[m_MeshCount]->Max[LOD].Z = Point.Z;
					if(MeshList[m_MeshCount]->Min[LOD].X > Point.X)
						MeshList[m_MeshCount]->Min[LOD].X = Point.X;
					if(MeshList[m_MeshCount]->Min[LOD].Y > Point.Y)
						MeshList[m_MeshCount]->Min[LOD].Y = Point.Y;
					if(MeshList[m_MeshCount]->Min[LOD].Z > Point.Z)
						MeshList[m_MeshCount]->Min[LOD].Z = Point.Z;
				}
			}

			// make an obb from the min and max values (AABBs can't be rotated)
			{
				MeshList[m_MeshCount]->OBBox[LOD].Axis[0].X=1.0f;
				MeshList[m_MeshCount]->OBBox[LOD].Axis[0].Y=0.0f;
				MeshList[m_MeshCount]->OBBox[LOD].Axis[0].Z=0.0f;

				MeshList[m_MeshCount]->OBBox[LOD].Axis[1].X=0.0f;
				MeshList[m_MeshCount]->OBBox[LOD].Axis[1].Y=1.0f;
				MeshList[m_MeshCount]->OBBox[LOD].Axis[1].Z=0.0f;

				MeshList[m_MeshCount]->OBBox[LOD].Axis[2].X=0.0f;
				MeshList[m_MeshCount]->OBBox[LOD].Axis[2].Y=0.0f;
				MeshList[m_MeshCount]->OBBox[LOD].Axis[2].Z=1.0f;

				MeshList[m_MeshCount]->OBBox[LOD].ALength.X=(MeshList[m_MeshCount]->Max[LOD].X-MeshList[m_MeshCount]->Min[LOD].X)*0.5f;
				MeshList[m_MeshCount]->OBBox[LOD].ALength.Y=(MeshList[m_MeshCount]->Max[LOD].Y-MeshList[m_MeshCount]->Min[LOD].Y)*0.5f;
				MeshList[m_MeshCount]->OBBox[LOD].ALength.Z=(MeshList[m_MeshCount]->Max[LOD].Z-MeshList[m_MeshCount]->Min[LOD].Z)*0.5f;

				MeshList[m_MeshCount]->OBBox[LOD].Center.X=(MeshList[m_MeshCount]->Max[LOD].X+MeshList[m_MeshCount]->Min[LOD].X)*0.5f;
				MeshList[m_MeshCount]->OBBox[LOD].Center.Y=(MeshList[m_MeshCount]->Max[LOD].Y+MeshList[m_MeshCount]->Min[LOD].Y)*0.5f;
				MeshList[m_MeshCount]->OBBox[LOD].Center.Z=(MeshList[m_MeshCount]->Max[LOD].Z+MeshList[m_MeshCount]->Min[LOD].Z)*0.5f;

			}

			// we have the verts, now get the normals
			for(i=0;i<MeshList[m_MeshCount]->NumVNormals[LOD];i++)
			{
				geVec3d_Copy(&(B->SkinNormalArray[i].Normal), &(MeshList[m_MeshCount]->VNormals[LOD][i]));
			}


			for(i=0;i<MeshList[m_MeshCount]->NumFaces[LOD];i++)
			{
				// get the material index of this face
				MeshList[m_MeshCount]->MaterialI[LOD][i] = B->SkinFaces[0].FaceArray[i].MaterialIndex;

				// get the material color value, this is per face and not per vertex - ugh!
				MeshList[m_MeshCount]->Color[LOD][i].r=B->MaterialArray[MeshList[m_MeshCount]->MaterialI[LOD][i]].Red;
				MeshList[m_MeshCount]->Color[LOD][i].g=B->MaterialArray[MeshList[m_MeshCount]->MaterialI[LOD][i]].Green;
				MeshList[m_MeshCount]->Color[LOD][i].b=B->MaterialArray[MeshList[m_MeshCount]->MaterialI[LOD][i]].Blue;
				MeshList[m_MeshCount]->Color[LOD][i].a=255.0f;

				for(j=0; j<3; ++j)
				{
					// fill in VertexIndices
					MeshList[m_MeshCount]->FaceI[LOD][i*3+j]=B->SkinFaces[0].FaceArray[i].VtxIndex[j];
					// changed QD 02/12/2005
					// fill in NormalIndices
					MeshList[m_MeshCount]->VNormalI[LOD][i*3+j]=B->SkinFaces[0].FaceArray[i].NormalIndex[j];
					// end change
				}
			}
		}

CLEAN_UP:
		if(ActorDef != NULL)
		{
			geActor_DefDestroy(&ActorDef);
			ActorDef = NULL;
		}
		if(theActor != NULL)
		{
			geActor_Destroy(&theActor);
			theActor = NULL;
		}

		// error - clean up
		if(CleanUp)
		{
			CleanUp = GE_FALSE;

			for(int i=0;i<MeshList[m_MeshCount]->MaterialCount[LOD];i++)
			{
				if(MeshList[m_MeshCount]->Bitmaps[LOD][i])
					geWorld_RemoveBitmap(CCD->World(), MeshList[m_MeshCount]->Bitmaps[LOD][i]);
			}
			free(MeshList[m_MeshCount]->Bitmaps[LOD]);
			MeshList[m_MeshCount]->Bitmaps[LOD] = NULL;

			free(MeshList[m_MeshCount]->Verts[LOD]);
			MeshList[m_MeshCount]->Verts[LOD] = NULL;

			free(MeshList[m_MeshCount]->FaceI[LOD]);
			MeshList[m_MeshCount]->FaceI[LOD] = NULL;

			// changed QD 02/12/2005
			free(MeshList[m_MeshCount]->VNormals[LOD]);
			MeshList[m_MeshCount]->VNormals[LOD] = NULL;

			free(MeshList[m_MeshCount]->VNormalI[LOD]);
			MeshList[m_MeshCount]->VNormalI[LOD] = NULL;
			// end change

			free(MeshList[m_MeshCount]->Color[LOD]);
			MeshList[m_MeshCount]->Color[LOD] = NULL;

			if(!LOD)
			{
				if(MeshList[m_MeshCount]->szFilename != NULL)
					delete MeshList[m_MeshCount]->szFilename;
				delete MeshList[m_MeshCount];
				MeshList[m_MeshCount] = NULL;
				return false;
			}
		}
	}

	// add the LODBitmap if it exists
	MeshList[m_MeshCount]->LODBitmap=NULL;
	sprintf(Name, "%s.tga", LodName);

	if(CCD->FileExist(kActorFile, Name))
	{
		geVFile *ImageFile;
		CCD->OpenRFFile(&ImageFile, kActorFile, Name, GE_VFILE_OPEN_READONLY);
		if(ImageFile)
		{
			MeshList[m_MeshCount]->LODBitmap = geBitmap_CreateFromFile(ImageFile);

			if(MeshList[m_MeshCount]->LODBitmap)
			{
				geBitmap_SetPreferredFormat(MeshList[m_MeshCount]->LODBitmap,GE_PIXELFORMAT_32BIT_ARGB);
				geWorld_AddBitmap(CCD->World(), MeshList[m_MeshCount]->LODBitmap);
			}

			geVFile_Close(ImageFile);
		}
	}

	return true;
}

// changed QD 09/28/2004
/* ------------------------------------------------------------------------------------ */
//							COMPUTE LIGHTING
/* ------------------------------------------------------------------------------------ */
void CStaticMesh::SetAmbientLight(StaticMesh *pMesh)
{
	for(int LOD=0; LOD<4; ++LOD)
	{
		if(!MeshList[pMesh->ListIndex]->Verts[LOD])
			continue;

		GE_RGBA* pColor;

		switch(LOD)
		{
		case 0:
			pColor = pMesh->ColorLOD0;
			break;
		case 1:
			pColor = pMesh->ColorLOD1;
			break;
		case 2:
			pColor = pMesh->ColorLOD2;
			break;
		case 3:
			pColor = pMesh->ColorLOD3;
			break;
		}

		for(int iFace=0;iFace<MeshList[pMesh->ListIndex]->NumFaces[LOD];iFace++)
		{
			for(int j=0; j<3; ++j)
			{
				pColor[iFace*3+j].r += pMesh->AmbientColor.r;
				pColor[iFace*3+j].g += pMesh->AmbientColor.g;
				pColor[iFace*3+j].b += pMesh->AmbientColor.b;

				if(pColor[iFace*3+j].r > 255.f)
					pColor[iFace*3+j].r = 255.f;
				if(pColor[iFace*3+j].g > 255.f)
					pColor[iFace*3+j].g = 255.f;
				if(pColor[iFace*3+j].b > 255.f)
					pColor[iFace*3+j].b = 255.f;
			}
		}
	}
}

// changed QD 02/12/2005
// removed PrepareVertexNormals and FreeVertexNormals, using normals of act file instead

/* ------------------------------------------------------------------------------------ */
//	actually computing the lighting for 1 light
//	in: calling mesh, light-data, type of light (SunLight, light, spotlight supported)
/* ------------------------------------------------------------------------------------ */
void CStaticMesh::ComputeLighting(StaticMesh *pMesh, void* pLight, int LType)
{
	geXForm3d thePosition, theRotation;
	geVec3d Angles, LDirection;
	geFloat arc;
	GE_RGBA *Color, *pColor;

	if(!(LType == LIGHT_SUNLIGHT || LType == LIGHT_SPOTLIGHT || LType == LIGHT_POINTLIGHT))
		return; // unsupported lighttype

	if(LType == LIGHT_SUNLIGHT || LType == LIGHT_SPOTLIGHT)
	{
		geXForm3d temp;

		if(LType == LIGHT_SUNLIGHT)
		{
			geVec3d_Copy(&(static_cast<SunLight*>(pLight)->angles), &Angles);
			Color = &(static_cast<SunLight*>(pLight)->color);
		}
		else if(LType == LIGHT_SPOTLIGHT)
		{
			geVec3d_Copy(&(static_cast<spotlight*>(pLight)->angles), &Angles);
			arc = cos(static_cast<spotlight*>(pLight)->arc/360.0f*GE_PI); // we just need half of the arc...
			Color = &(static_cast<spotlight*>(pLight)->color);
		}

		// get the direction of the light
		geVec3d_Set(&LDirection, 1.f, 0.f, 0.f);

		geXForm3d_SetZRotation(&temp, Angles.Z*GE_PIOVER180);
		geXForm3d_RotateX(&temp, Angles.X*GE_PIOVER180);
		geXForm3d_RotateY(&temp, Angles.Y*GE_PIOVER180);

		geXForm3d_Rotate(&temp, &LDirection, &LDirection);
	}
	else if(LType == LIGHT_POINTLIGHT)
	{
		Color = &(static_cast<light*>(pLight)->color);

	// prepare the transformation matrix
	geXForm3d_SetScaling(&thePosition, pMesh->Scale, pMesh->Scale, pMesh->Scale);
	geXForm3d_RotateZ(&thePosition, pMesh->Rotation.Z);
	geXForm3d_RotateX(&thePosition, pMesh->Rotation.X);
	geXForm3d_RotateY(&thePosition, pMesh->Rotation.Y);
	geXForm3d_Translate(&thePosition, pMesh->origin.X, pMesh->origin.Y, pMesh->origin.Z);

	// changed QD 12/15/05
	//geXForm3d_SetIdentity(&theRotation);
	//geXForm3d_RotateZ(&theRotation, pMesh->Rotation.Z);
	geXForm3d_SetZRotation(&theRotation, pMesh->Rotation.Z);
	// end change
	geXForm3d_RotateX(&theRotation, pMesh->Rotation.X);
	geXForm3d_RotateY(&theRotation, pMesh->Rotation.Y);

	for(int LOD=0; LOD<4; ++LOD)
	{
		if(!MeshList[pMesh->ListIndex]->Verts[LOD])
			continue;

		switch(LOD)
		{
		case 0:
			pColor=pMesh->ColorLOD0;
			break;
		case 1:
			pColor=pMesh->ColorLOD1;
			break;
		case 2:
			pColor=pMesh->ColorLOD2;
			break;
		case 3:
			pColor=pMesh->ColorLOD3;
			break;
		}


		for(int iFace=0;iFace<MeshList[pMesh->ListIndex]->NumFaces[LOD];iFace++)
		{
			geVec3d Vertex[3];
			int j;

			for(j=0; j<3; ++j)
			{
				int VertIndex = MeshList[pMesh->ListIndex]->FaceI[LOD][iFace*3+j];

				Vertex[j].X=MeshList[pMesh->ListIndex]->Verts[LOD][VertIndex].X;
				Vertex[j].Y=MeshList[pMesh->ListIndex]->Verts[LOD][VertIndex].Y;
				Vertex[j].Z=MeshList[pMesh->ListIndex]->Verts[LOD][VertIndex].Z;

				geXForm3d_Transform(&thePosition, &Vertex[j], &Vertex[j]);
			}

			// check if the triangle faces the light
			geVec3d Normal;

			if(!pMesh->CompLightSmooth) // if we want a blocky world-like lighting
			{
				geVec3d v1, v2;

				geVec3d_Subtract(&Vertex[1], &Vertex[0], &v1);
				geVec3d_Subtract(&Vertex[2], &Vertex[0], &v2);
				geVec3d_CrossProduct(&v1, &v2, &Normal);

				geVec3d_Normalize(&Normal);
			}

			for(j=0; j<3; ++j)
			{
				GE_Collision Collision;
				geVec3d LOrigin, Vect;
				geFloat Val;
				int Intensity;

				if(pMesh->CompLightSmooth) // if we want a smooth actor-like lighting
				{
					int NormalIndex = MeshList[pMesh->ListIndex]->VNormalI[LOD][iFace*3+j];

					geXForm3d_Transform(&theRotation, &(MeshList[pMesh->ListIndex]->VNormals[LOD][NormalIndex]), &Normal);

					//geVec3d_Normalize(&Normal);
				}

				if(LType == LIGHT_SUNLIGHT)
				{
					Val = -geVec3d_DotProduct(&LDirection, &Normal);

					// light source is far, far away
					geVec3d_AddScaled(&Vertex[j], &LDirection, -10000.f, &LOrigin);
				}
				else if(LType == LIGHT_SPOTLIGHT)
				{
					geVec3d_Copy(&(((spotlight*)pLight)->origin), &LOrigin);
					Intensity = ((spotlight*)pLight)->light;
				}
				else if(LType == LIGHT_POINTLIGHT)
				{
					// changed QD 02/12/2005 fixed bug
					geVec3d_Copy(&(((light*)pLight)->origin), &LOrigin);
					Intensity = ((light*)pLight)->light;
				}
				else
					Val = 0.f;

				if(LType == LIGHT_SPOTLIGHT || LType == LIGHT_POINTLIGHT)
				{
					// Find the angle between the light, and the face normal
					geVec3d_Subtract(&LOrigin, &Vertex[j], &Vect);

					geFloat Dist = geVec3d_Normalize (&Vect);
					geFloat Angle = geVec3d_DotProduct(&Vect, &Normal);

					if(Angle <= 0.f)
						continue;

					if(LType == LIGHT_SPOTLIGHT)
					{
						geFloat Angle2 = -geVec3d_DotProduct(&Vect, &LDirection);
						if(Angle2 < arc)
							continue;
					}

					Val = ((Intensity - Dist)/Intensity)*Angle;
				}

				if(Val <= 0.f)
					continue;

				// check if there's something between the light source and the vertex
				geBoolean Result=RayTracing(pMesh, LOD, Vertex[j], LOrigin, &Collision);

				if(Result && !geVec3d_Compare(&Vertex[j], &Collision.Impact, 0.0001f))
					continue;

				pColor[iFace*3+j].r += (Color->r*Val);
				pColor[iFace*3+j].g += (Color->g*Val);
				pColor[iFace*3+j].b += (Color->b*Val);

				// avoid overflow
				if(pColor[iFace*3+j].r > 255.f)
					pColor[iFace*3+j].r = 255.f;
				if(pColor[iFace*3+j].g > 255.f)
					pColor[iFace*3+j].g = 255.f;
				if(pColor[iFace*3+j].b > 255.f)
					pColor[iFace*3+j].b = 255.f;
			}
		}
	}
}
// end change 09/28/2004


/* TODO exclude skybox from collisin test - only works if you turn all world geometry into models
but then they don't cast shadows on other world geometry...
#define		TEXINFO_SKY					(1<<2)

static geBoolean ColCB(geWorld_Model *Model, geActor *Actor, void *Context)
{
	if(Model)
	{
		int32				TexFlags=0;
		GFX_Face			*pFace;
		GFX_TexInfo			*pTexInfo;

		int32 Face = Model->BSPModel->FirstFace;
		int NumFaces = Model->BSPModel->NumFaces;
		GBSP_BSPData	*BSPData=&(CCD->World()->CurrentBSP->BSPData);

		for(int i=0;i<NumFaces;++i)
		{
			pFace = &BSPData->GFXFaces[Face+i];
			pTexInfo = &BSPData->GFXTexInfo[pFace->TexInfo];
			TexFlags = pTexInfo->Flags;
			if(!(TexFlags & TEXINFO_SKY))
				return GE_TRUE;
		}
		return GE_FALSE;
	}
	return GE_TRUE;
}*/

// changed QD 09/28/2004
/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
geBoolean CStaticMesh::RayTracing(StaticMesh *CallingMesh, int LOD,
								  const geVec3d &OldPosition, const geVec3d &NewPosition,
								  GE_Collision *Collision)
{
	if(m_MeshCount==0) // don't waste precious time
		return GE_FALSE;

	// Ok, see if we have any static meshes we need check
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "StaticMesh");

	if(!pSet)
		return GE_FALSE;

	geVec3d Line, NewPos;

	geVec3d_Subtract(&NewPosition, &OldPosition, &Line);
	geVec3d_AddScaled(&OldPosition, &Line, 0.05f, &NewPos);

	// TODO
	// buhh - geWorld_Collision function doesn't let me exclude the skybox from the collision test...
	if(geWorld_Collision(CCD->World(), NULL, NULL, &OldPosition, &NewPos, GE_VISIBLE_CONTENTS,
						GE_COLLIDE_ALL, 0, NULL, NULL, Collision))
		return GE_TRUE;

	geEntity *pEntity;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		StaticMesh *pMesh = static_cast<StaticMesh*>(geEntity_GetUserData(pEntity));

		if(!pMesh->StaticShadowCaster)
			continue;

// changed QD 09/28/2004
		int LODIndex = 0;
		if(pMesh == CallingMesh)
			LODIndex = LOD;
// end change

		geXForm3d thePosition, thePositionT, theScale;

		// set up the transformation matrix
		geXForm3d_SetZRotation(&thePosition, pMesh->Rotation.Z);
		geXForm3d_RotateX(&thePosition, pMesh->Rotation.X);
		geXForm3d_RotateY(&thePosition, pMesh->Rotation.Y);

		geXForm3d_Translate(&thePosition, pMesh->origin.X, pMesh->origin.Y, pMesh->origin.Z);

		// get the transpose, we will transform the line/ min&max instead of the whole mesh
		// this should be faster
		geXForm3d_GetTranspose(&thePosition, &thePositionT); //(M, MTranspose)

		// now we can scale it (the matrix had to be orthonormal (=without scale) to transpose it)
		geXForm3d_SetScaling(&theScale, pMesh->Scale, pMesh->Scale, pMesh->Scale);
		geXForm3d_Multiply(&thePosition, &theScale, &thePosition);

		geFloat s;
		geVec3d m_vDir;
		geVec3d OldPos;
		geXForm3d theRotationT;

		// also scale the transposed matrix
		geFloat InvScale = 1.f/pMesh->Scale;
		geXForm3d_Scale(&thePositionT, InvScale, InvScale, InvScale);

		geXForm3d_Transform(&thePositionT, &OldPosition, &OldPos);

		geXForm3d_SetYRotation(&theRotationT, -pMesh->Rotation.Y);
		geXForm3d_RotateX(&theRotationT, -pMesh->Rotation.X);
		geXForm3d_RotateZ(&theRotationT, -pMesh->Rotation.Z);

		// don't translate the direction!!
		geXForm3d_Rotate(&theRotationT, &Line, &m_vDir);
		geVec3d_Scale(&m_vDir, InvScale, &m_vDir);
		geFloat fL = geVec3d_Normalize(&m_vDir);
// changed QD 04/23/2004
		for(int i=0; i<MeshList[pMesh->ListIndex]->NumFaces[LODIndex]; i++)
		{
			geVec3d Vertex[3];
			int VertIndex, j;

			for(j=0; j<3; ++j)
			{
				VertIndex = MeshList[pMesh->ListIndex]->FaceI[LODIndex][i*3+j];

				Vertex[j].X=MeshList[pMesh->ListIndex]->Verts[LODIndex][VertIndex].X;
				Vertex[j].Y=MeshList[pMesh->ListIndex]->Verts[LODIndex][VertIndex].Y;
				Vertex[j].Z=MeshList[pMesh->ListIndex]->Verts[LODIndex][VertIndex].Z;
			}
// end change
			//*********************************************************************
			// Fast, Minimum Storage Ray-Triangle Intersection by Möller & Trumbore
			//*********************************************************************

			geVec3d edge1, edge2;
			geVec3d pvec, tvec, qvec, PlaneNormal;

			// find vectors for 2 edges sharing vert0
			geVec3d_Subtract(&Vertex[1], &Vertex[0], &edge1);
			geVec3d_Subtract(&Vertex[2], &Vertex[0], &edge2);

			// begin calculating determinant - also used to calculate U parameter
			geVec3d_CrossProduct(&m_vDir, &edge2, &pvec);
			geFloat det = edge1.X*pvec.X+edge1.Y*pvec.Y+edge1.Z*pvec.Z;// geVec3d_DotProduct(&edge1, &pvec);

			// the line and the plane are almost parallel, if determinant is near zero
			if(det < 0.0001f && det > -0.0001f)
				continue;

			geFloat inv_det = 1.0f/det;

			// calculate distance from vert0 to ray origin
			geVec3d_Subtract(&OldPos, &Vertex[0], &tvec);
			// calculate U parameter and test bounds
			geFloat u = (tvec.X*pvec.X+tvec.Y*pvec.Y+tvec.Z*pvec.Z)*inv_det;

			if(u < 0.0f || u > 1.0f)
				continue;

			// prepare to test V parameter
			geVec3d_CrossProduct(&tvec, &edge1, &qvec);
			// calculate V parameter and test bounds
			geFloat v = (m_vDir.X*qvec.X+m_vDir.Y*qvec.Y+m_vDir.Z*qvec.Z)*inv_det;

			if(v < 0.0f || u+v > 1.0f)
				continue;

			// calculate s, ray intersects triangle
			s = (edge2.X*qvec.X + edge2.Y*qvec.Y + edge2.Z*qvec.Z)*inv_det;

			// collision but not on segment?
			if(s <= 0.0f)
				continue;
			else
			{
				if(Collision)
				{
					geVec3d_AddScaled(&OldPos, &m_vDir, s, &(Collision->Impact));
					geXForm3d_Transform(&thePosition, &(Collision->Impact), &(Collision->Impact));

					geVec3d_CrossProduct(&edge1, &edge2, &PlaneNormal);
					geXForm3d_Transform(&thePosition, &PlaneNormal, &(Collision->Plane.Normal));
					geVec3d_Normalize(&(Collision->Plane.Normal));

// changed QD 09/28/2004
					Collision->Plane.Dist = geVec3d_DotProduct(&(Collision->Plane.Normal), &(Collision->Impact));
// end change
				}

				return GE_TRUE;
			}
		}//for faces
	}//for entities

	return GE_FALSE;
}
/* ------------------------------------------------------------------------------------ */
//							END COMPUTE LIGHTING
/* ------------------------------------------------------------------------------------ */

// changed QD 12/15/05
/* ------------------------------------------------------------------------------------ */
//	Destructor
/* ------------------------------------------------------------------------------------ */
CStaticMesh::~CStaticMesh()
{
	CleanUp();
}

/* ------------------------------------------------------------------------------------ */
//	CleanUp
/* ------------------------------------------------------------------------------------ */
void CStaticMesh::CleanUp()
{
	// Ok, see if we have any static entity proxies we need to set up.
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "StaticMesh");

	if(!pSet)
		return;									// Don't waste CPU time.

	geEntity *pEntity;

	//	Ok, we have static mesh entities somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		StaticMesh *pMesh = static_cast<StaticMesh*>(geEntity_GetUserData(pEntity));

		if(pMesh->ColorLOD0)
			free(pMesh->ColorLOD0);

		if(pMesh->ColorLOD1)
			free(pMesh->ColorLOD1);

		if(pMesh->ColorLOD2)
			free(pMesh->ColorLOD2);

		if(pMesh->ColorLOD3)
			free(pMesh->ColorLOD3);
	}

	for(int i=0; i<m_MeshCount; ++i)
	{
		for(int LOD=0; LOD<4; ++LOD)
		{
			free(MeshList[i]->Verts[LOD]);

			free(MeshList[i]->FaceI[LOD]);

// changed QD 02/12/2005
			if(MeshList[i]->VNormals[LOD])
				free(MeshList[i]->VNormals[LOD]);

			if(MeshList[i]->VNormalI[LOD])
				free(MeshList[i]->VNormalI[LOD]);
// end change
			free(MeshList[i]->Color[LOD]);

			free(MeshList[i]->MaterialI[LOD]);

			for(int j=0;j<MeshList[i]->MaterialCount[LOD];j++)
			{
				if(MeshList[i]->Bitmaps[LOD][j])
				{
					geWorld_RemoveBitmap(CCD->World(), MeshList[i]->Bitmaps[LOD][j]);
				//		geBitmap_Destroy(&(pMesh->Bitmaps0[i]));
				}
			}
			free(MeshList[i]->Bitmaps[LOD]);
		}

		if(MeshList[i]->LODBitmap)
		{
			geWorld_RemoveBitmap(CCD->World(), MeshList[i]->LODBitmap);
			geBitmap_Destroy(&(MeshList[i]->LODBitmap));
		}

		if(MeshList[i]->szFilename != NULL)
		{
			delete MeshList[i]->szFilename;
		}

		delete MeshList[i];
		MeshList[i] = NULL;
	}

	m_MeshCount = 0;
}
// end change

/* ------------------------------------------------------------------------------------ */
//  entity tick function
/* ------------------------------------------------------------------------------------ */
void CStaticMesh::Tick(geFloat dwTicks)
{
	if(m_MeshCount == 0) // don't waste precious time
		return;

	// Ok, see if we have any static meshes we need to set up.
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "StaticMesh");

	if(!pSet)
		return;

	geEntity *pEntity;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		StaticMesh *pMesh = static_cast<StaticMesh*>(geEntity_GetUserData(pEntity));

		int LOD = 0;

		pMesh->origin = pMesh->OriginOffset;
		SetOriginOffset(pMesh->EntityName, pMesh->BoneName, pMesh->Model, &(pMesh->origin));

		{
			if(pMesh->Model)
			{
				geXForm3d Xf;
				geVec3d  Tmp;
				geWorld_GetModelXForm(CCD->World(), pMesh->Model, &Xf);
				geXForm3d_GetEulerAngles(&Xf, &Tmp);
				geVec3d_Add(&(pMesh->ActorRotation), &Tmp, &(pMesh->Rotation));

			}
			else if(!EffectC_IsStringNull(pMesh->EntityName))
			{

				if(!EffectC_IsStringNull(pMesh->BoneName))
				{
					SetAngle(pMesh->EntityName, pMesh->BoneName, &(pMesh->Rotation));
				}
				else
				{
					geActor	*theActor;

					theActor = GetEntityActor(pMesh->EntityName);
					CCD->ActorManager()->GetRotation(theActor, &(pMesh->Rotation));
				}
				geVec3d_Add(&(pMesh->ActorRotation), &(pMesh->Rotation), &(pMesh->Rotation));
			}
		}

		// get the distance between the entity and the camera
		geVec3d CamPosition;
		CCD->CameraManager()->GetPosition(&CamPosition);
// changed QD 02/01/07
// use center of bounding box for LOD determination instead enity origin ( = root bone position of actor)
		geVec3d Center;
		geXForm3d thePosition;

		geXForm3d_SetScaling(&thePosition, pMesh->Scale, pMesh->Scale, pMesh->Scale);
		geXForm3d_RotateZ(&thePosition, pMesh->Rotation.Z);
		geXForm3d_RotateX(&thePosition, pMesh->Rotation.X);
		geXForm3d_RotateY(&thePosition, pMesh->Rotation.Y);
		geXForm3d_Translate(&thePosition, pMesh->origin.X, pMesh->origin.Y, pMesh->origin.Z);
		geXForm3d_Transform(&thePosition, &(MeshList[pMesh->ListIndex]->OBBox[0].Center), &Center);
		float dist = (geVec3d_DistanceBetween(&CamPosition, &Center) / CCD->CameraManager()->AmtZoom());
// end change

		// which LOD do we have to render?
		if(CCD->GetLODdistance(0) == 0 && CCD->GetLODdistance(1) == 0 && CCD->GetLODdistance(2) == 0
			&& CCD->GetLODdistance(3) == 0 && CCD->GetLODdistance(4) == 0)
		{
			AddPoly(pMesh, 0);
		}
		else
		{
			if(CCD->GetLODdistance(0) != 0 && dist>CCD->GetLODdistance(0))
			{
				if(MeshList[pMesh->ListIndex]->Verts[1])
					LOD = 1;
			}
			if(CCD->GetLODdistance(1) != 0 && dist>CCD->GetLODdistance(1))
			{
				if(MeshList[pMesh->ListIndex]->Verts[2])
					LOD = 2;
			}
			if(CCD->GetLODdistance(2) != 0 && dist>CCD->GetLODdistance(2))
			{
				if(MeshList[pMesh->ListIndex]->Verts[3])
					LOD = 3;
			}
			if(dist<CCD->GetLODdistance(4) || CCD->GetLODdistance(4) == 0)
			{
				if(CCD->GetLODdistance(3) != 0 && dist > CCD->GetLODdistance(3) && MeshList[pMesh->ListIndex]->LODBitmap)
					LOD = 4;
			}
			else
				LOD = 5;


			if(LOD < 4)
				AddPoly(pMesh, LOD);
			else if(LOD == 4)
			{
				GE_LVertex	Vertex;
				Vertex.r = 255.0f;
				Vertex.g = 255.0f;
				Vertex.b = 255.0f;
				Vertex.a = 255.0f;
				if(CCD->GetLODdistance(4) != 0 && CCD->GetLODdistance(4) > CCD->GetLODdistance(3))
				{
					float diff = CCD->GetLODdistance(4) - CCD->GetLODdistance(3);
					float alpha = 1.0f - ((dist-CCD->GetLODdistance(3))/diff);
					Vertex.a = 255.0f*alpha;
				}
				Vertex.u = 0.0f;
				Vertex.v = 0.0f;
				float HalfHeight = geBitmap_Height(MeshList[pMesh->ListIndex]->LODBitmap) * pMesh->Scale * 0.5f;
				Vertex.X = pMesh->origin.X;
				Vertex.Y = pMesh->origin.Y+HalfHeight;
				Vertex.Z = pMesh->origin.Z;
				geWorld_AddPolyOnce(CCD->World(), &Vertex, 1, MeshList[pMesh->ListIndex]->LODBitmap, GE_TEXTURED_POINT,
					GE_RENDER_DEPTH_SORT_BF | GE_RENDER_DO_NOT_OCCLUDE_OTHERS, pMesh->Scale);
			}
		}
	}
}

// TODO: only transform vertices if position or rotation has changed...
/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CStaticMesh::AddPoly(StaticMesh *pMesh, int LOD)
{
	geExtBox Box;
	geVec3d Axis[3], ALength, Center;
	int i;
	int Index = pMesh->ListIndex;

	geXForm3d thePosition, theRotation;

// changed QD 04/23/2004
	geXForm3d CameraXf;
	geFloat clipsq = 10000.f;
	//if(!pMesh->Backfaced || CCD->CameraManager()->GetClipEnable())
	//	CameraXf = CCD->CameraManager()->ViewPoint();
	if(CCD->CameraManager()->GetClipEnable())
	{
		CameraXf = CCD->CameraManager()->ViewPoint();
		clipsq = CCD->CameraManager()->GetFarClipPlane();
		clipsq *= clipsq;
	}
// end change

	geXForm3d_SetScaling(&thePosition, pMesh->Scale, pMesh->Scale, pMesh->Scale);

	geXForm3d_RotateZ(&thePosition, pMesh->Rotation.Z);
	geXForm3d_RotateX(&thePosition, pMesh->Rotation.X);
	geXForm3d_RotateY(&thePosition, pMesh->Rotation.Y);

	geXForm3d_Translate(&thePosition, pMesh->origin.X, pMesh->origin.Y, pMesh->origin.Z);

	// check the visibility at an overall bounding box level
	if(pMesh->VisCheckLevel > 0 && pMesh->VisCheckLevel < 3)
	{
// changed QD 12/15/05
		//geXForm3d_SetIdentity(&theRotation);
		//geXForm3d_RotateZ(&theRotation, pMesh->Rotation.Z);
		geXForm3d_SetZRotation(&theRotation, pMesh->Rotation.Z);
		geXForm3d_RotateX(&theRotation, pMesh->Rotation.X);
		geXForm3d_RotateY(&theRotation, pMesh->Rotation.Y);

		// transform the obb
		for(i=0; i<3; ++i)
			geXForm3d_Rotate(&theRotation, &(MeshList[Index]->OBBox[LOD].Axis[i]), &Axis[i]);

		geXForm3d_Transform(&thePosition, &(MeshList[Index]->OBBox[LOD].Center), &Center);

		geVec3d_Scale(&(MeshList[Index]->OBBox[LOD].ALength), pMesh->Scale, &ALength);

		AABBofOBB(&(Box.Min), &(Box.Max), &ALength, &(Axis[0]), &(Axis[1]), &(Axis[2]));

		geVec3d_Add(&Center, &(Box.Max), &(Box.Max));
		geVec3d_Add(&Center, &(Box.Min), &(Box.Min));

		if(CCD->MenuManager()->GetSEBoundBox())
		{
			Box.Min.X -= pMesh->origin.X;
			Box.Min.Y -= pMesh->origin.Y;
			Box.Min.Z -= pMesh->origin.Z;
			Box.Max.X -= pMesh->origin.X;
			Box.Max.Y -= pMesh->origin.Y;
			Box.Max.Z -= pMesh->origin.Z;
			DrawBoundBox(CCD->World(), &pMesh->origin, &Box.Min, &Box.Max);
		}

		if(!EffectC_IsBoxVisible(CCD->World(), CCD->CameraManager()->Camera(), &Box))
			return;
	}

	GE_RGBA *pColor;
	switch(LOD)
	{
	case 0:
		pColor = pMesh->ColorLOD0;
		break;
	case 1:
		pColor = pMesh->ColorLOD1;
		break;
	case 2:
		pColor = pMesh->ColorLOD2;
		break;
	case 3:
		pColor = pMesh->ColorLOD3;
		break;
	}

	for(i=0; i<MeshList[Index]->NumFaces[LOD]; i++)
	{
		GE_LVertex	Vertex[3];
		geVec3d temp[3];
		int j;

		for(j=0; j<3; ++j)
		{
			int VertIndex = MeshList[pMesh->ListIndex]->FaceI[LOD][i*3+j];
			temp[j].X = MeshList[Index]->Verts[LOD][VertIndex].X;
			temp[j].Y = MeshList[Index]->Verts[LOD][VertIndex].Y;
			temp[j].Z = MeshList[Index]->Verts[LOD][VertIndex].Z;

			geXForm3d_Transform(&thePosition, &temp[j], &temp[j]);

			Vertex[j].X = temp[j].X;
			Vertex[j].Y = temp[j].Y;
			Vertex[j].Z = temp[j].Z;
		}

// changed QD 04/23/2004
		// backface culling - using GE_RENDER_BACKFACED or not doesn't make any difference
		if(!pMesh->Backfaced)
		{
			//geVec3d	CameraNormal[3];
			geVec3d FaceNormal, edge[2];
			geFloat Dist;

			//for(j=0;j<3;j++)
			//	geVec3d_Subtract(&(temp[j]), &(CameraXf.Translation), &CameraNormal[j]);

			// direction of the triangle normal
			geVec3d_Subtract(&temp[1], &temp[0], &edge[0]);
			geVec3d_Subtract(&temp[2], &temp[0], &edge[1]);
			geVec3d_CrossProduct(&edge[0], &edge[1], &FaceNormal);

			/*if(geVec3d_DotProduct(&CameraNormal[0], &FaceNormal) > 0.0f
				&& geVec3d_DotProduct(&CameraNormal[1], &FaceNormal)> 0.0f
				&& geVec3d_DotProduct(&CameraNormal[2], &FaceNormal)> 0.0f)
				continue;*/

			geVec3d_Normalize(&FaceNormal);

			Dist = geVec3d_DotProduct(&FaceNormal, &temp[0]);

			if(geVec3d_DotProduct(&FaceNormal, geCamera_GetPov(CCD->CameraManager()->Camera())) <= Dist)
				continue;
		}
		// far clipping
		if(CCD->CameraManager()->GetClipEnable())
		{
			geFloat distsq[3];

			for(j=0; j<3; ++j)
			{
				geVec3d B;
				geVec3d_Subtract(&temp[j], &(CameraXf.Translation), &B);
				distsq[j]= geVec3d_LengthSquared(&B);
			}

			if((distsq[0] > clipsq) && (distsq[1] > clipsq) && (distsq[2] > clipsq))
				continue;
		}
// end change

		// check the visibility at a per face bounding box level
		// rather slow to be honest...
		if(pMesh->VisCheckLevel > 1)
		{
			Box.Max.X = Box.Min.X = Vertex[0].X;
			Box.Max.Y = Box.Min.Y = Vertex[0].Y;
			Box.Max.Z = Box.Min.Z = Vertex[0].Z;

			for(j=1; j<3; ++j)
			{
				if(Box.Max.X < Vertex[j].X)
					Box.Max.X = Vertex[j].X;
				if(Box.Max.Y < Vertex[j].Y)
					Box.Max.Y = Vertex[j].Y;
				if(Box.Max.Z < Vertex[j].Z)
					Box.Max.Z = Vertex[j].Z;
				if(Box.Min.X > Vertex[j].X)
					Box.Min.X = Vertex[j].X;
				if(Box.Min.Y > Vertex[j].Y)
					Box.Min.Y = Vertex[j].Y;
				if(Box.Min.Z > Vertex[j].Z)
					Box.Min.Z = Vertex[j].Z;
			}

			if(!EffectC_IsBoxVisible(CCD->World(), CCD->CameraManager()->Camera(), &Box))
				continue;
		}

		for(j=0; j<3; ++j)
		{
			int VertIndex = MeshList[pMesh->ListIndex]->FaceI[LOD][i*3+j];

			if(pMesh->UseFillColor)
			{
				Vertex[j].r = pMesh->FillColor.r;
				Vertex[j].g = pMesh->FillColor.g;
				Vertex[j].b = pMesh->FillColor.b;
			}
			else if((pMesh->CompSunLight || pMesh->CompLight) && pColor) // changed QD 09/28/2004
			{
				// changed QD 02/12/2004
				/*
				if(pMesh->CompLightSmooth)
				{
					Vertex[j].r=pColor[VertIndex].r;
					Vertex[j].g=pColor[VertIndex].g;
					Vertex[j].b=pColor[VertIndex].b;
				}
				else
				*/
				{
					Vertex[j].r = pColor[i*3+j].r;
					Vertex[j].g = pColor[i*3+j].g;
					Vertex[j].b = pColor[i*3+j].b;
				}
			}
			else //default color of the actor
			{
				Vertex[j].r = MeshList[Index]->Color[LOD][i].r;
				Vertex[j].g = MeshList[Index]->Color[LOD][i].g;
				Vertex[j].b = MeshList[Index]->Color[LOD][i].b;
			}

			Vertex[j].a = pMesh->Alpha;
			Vertex[j].u = MeshList[Index]->Verts[LOD][VertIndex].u;
			Vertex[j].v = MeshList[Index]->Verts[LOD][VertIndex].v;
		}

		if(MeshList[Index]->Bitmaps[LOD][MeshList[Index]->MaterialI[LOD][i]])
		{
			geWorld_AddPolyOnce(CCD->World(), Vertex, 3, MeshList[Index]->Bitmaps[LOD][MeshList[Index]->MaterialI[LOD][i]],
								GE_TEXTURED_POLY, (uint32)pMesh->RenderFlags, 1.f);
		}
		else // material without bitmap
		{
			geWorld_AddPolyOnce(CCD->World(),
								Vertex,
								3,
								NULL,
								GE_GOURAUD_POLY,
								static_cast<uint32>(pMesh->RenderFlags),
								1.f);
		}
	}
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
static bool NoIntersect(geFloat Speed, geFloat Min0, geFloat Max0,
						geFloat Min1, geFloat Max1, geFloat* TFirst, geFloat* TLast)
{
    geFloat InvSpeed, T;

    if(Max1 < Min0)  // C1 initially on left of C0
    {
        if (Speed <= 0.0f)
        {
            // intervals moving apart
            return true;
        }

        InvSpeed = 1.0f/Speed;

        T = (Min0 - Max1)*InvSpeed;
        if(T > *TFirst)
            *TFirst = T;
        if(*TFirst > 1.0f)
            return true;

        T = (Max0 - Min1)*InvSpeed;
        if(T < *TLast)
            *TLast = T;
        if(*TFirst > *TLast)
            return true;
    }
    else if(Max0 < Min1)  // C1 initially on right of C0
    {
        if (Speed >= 0.0f)
        {
            // intervals moving apart
            return true;
        }

        InvSpeed = 1.0f/Speed;

        T = (Max0 - Min1)*InvSpeed;
        if(T > *TFirst)
            *TFirst = T;
        if(*TFirst > 1.0f)
            return true;

        T = (Min0 - Max1)*InvSpeed;
        if(T < *TLast)
            *TLast = T;
        if(*TFirst > *TLast)
            return true;
    }
    else  // C0 and C1 overlap
    {
        if(Speed > 0.0f)
        {
            T = (Max0 - Min1)/Speed;
            if(T < *TLast)
                *TLast = T;
            if(*TFirst > *TLast)
                return true;
        }
        else if(Speed < 0.0f)
        {
            T = (Min0 - Max1)/Speed;
            if(T < *TLast)
                *TLast = T;
            if(*TFirst > *TLast)
                return true;
        }
    }

    return false;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
bool CStaticMesh::CollisionCheck(geVec3d *Min, geVec3d *Max,
								 const geVec3d &OldPosition, const geVec3d &NewPosition,
								 GE_Collision *Collision)
{
	if(m_MeshCount == 0) // don't waste precious time
		return GE_FALSE;

	// Ok, see if we have any static meshes we need check
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "StaticMesh");

	if(!pSet)
		return GE_FALSE;

	geEntity *pEntity;
	geBoolean ColResult = GE_FALSE;
	geBoolean GColResult = GE_FALSE;
	geFloat Gs=1.0f;
	geExtBox TestBox;
	geVec3d Line;

	// OBB
	geVec3d OBBaxis[3];
	geFloat OBBHalfAxisLength[3];
	geVec3d OBBcenter;

	geVec3d_Subtract(&NewPosition, &OldPosition, &Line);

	if(Min && Max)
	{
		TestBox.Min = *Min;
		TestBox.Max = *Max;

		geExtBox_LinearSweep(&TestBox, &OldPosition, &NewPosition, &TestBox);

		geVec3d_Add(Min, Max, &OBBcenter);
		geVec3d_Scale(&OBBcenter, 0.5f, &OBBcenter);
		geVec3d_Add(&OBBcenter, &OldPosition, &OBBcenter);

		OBBaxis[0].X=1.0f;
		OBBaxis[0].Y=0.0f;
		OBBaxis[0].Z=0.0f;

		OBBaxis[1].X=0.0f;
		OBBaxis[1].Y=1.0f;
		OBBaxis[1].Z=0.0f;

		OBBaxis[2].X=0.0f;
		OBBaxis[2].Y=0.0f;
		OBBaxis[2].Z=1.0f;

		OBBHalfAxisLength[0]=(Max->X-Min->X)*0.5f;
		OBBHalfAxisLength[1]=(Max->Y-Min->Y)*0.5f;
		OBBHalfAxisLength[2]=(Max->Z-Min->Z)*0.5f;
/*
// it's possible to rotate the box with the actor... but it doesn't make a big difference,
// so stick with the simple method above
		geFloat x, y, z;
		x=Line.X;
		if(x<0.0f)	x=-x;
		y=Line.Y;
		if(y<0.0f)	y=-y;
		z=Line.Z;
		if(z<0.0f)	z=-z;

		if(y>0.0f)
		{
			OBBaxis[0].X=1.0f;
			OBBaxis[0].Y=0.0f;
			OBBaxis[0].Z=0.0f;

			OBBaxis[2].X=0.0f;
			OBBaxis[2].Y=0.0f;
			OBBaxis[2].Z=1.0f;
		}
		else
		{
			if(x>z)
			{
				OBBaxis[0].X=Line.X;
				OBBaxis[0].Y=0.0f;
				OBBaxis[0].Z=Line.Z;

				OBBaxis[2].X=-Line.Z;
				OBBaxis[2].Y=0.0f;
				OBBaxis[2].Z=Line.X;
			}
			else
			{
				OBBaxis[2].X=Line.X;
				OBBaxis[2].Y=0.0f;
				OBBaxis[2].Z=Line.Z;

				OBBaxis[0].X=-Line.Z;
				OBBaxis[0].Y=0.0f;
				OBBaxis[0].Z=Line.X;
			}
			geVec3d_Normalize(&OBBaxis[0]);
			geVec3d_Normalize(&OBBaxis[2]);
		}*/
	}

	// loop through the entities
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		StaticMesh *pMesh = static_cast<StaticMesh*>(geEntity_GetUserData(pEntity));

		/*
		ColCheckLevel:
		0 - no collision
		1 - overall bounding box collision
		2 - overall bounding box collision and if true, triangle collison
		3 - triangle collision, with line or box
		4 - triangle collision, with line only - useful for terain
		*/
		if(!pMesh->ColCheckLevel)
			continue;

		geXForm3d thePosition, thePositionT, theScale;
		geExtBox Box;
		//int Index = pMesh->ListIndex;

// changed QD 09/08/2004
		// get current LOD
		int LOD = 0;

		if(CCD->GetLODdistance(0) != 0 || CCD->GetLODdistance(1) != 0 || CCD->GetLODdistance(2) != 0
			|| CCD->GetLODdistance(3) != 0 || CCD->GetLODdistance(4) != 0)
		{
			// get the distance between the entity and the camera
			geVec3d CamPosition;
			CCD->CameraManager()->GetPosition(&CamPosition);
// changed QD 02/01/07
// use center of bounding box for LOD determination instead enity origin ( = root bone position of actor)
			geVec3d Center;
			geXForm3d thePosition;

			geXForm3d_SetScaling(&thePosition, pMesh->Scale, pMesh->Scale, pMesh->Scale);
			geXForm3d_RotateZ(&thePosition, pMesh->Rotation.Z);
			geXForm3d_RotateX(&thePosition, pMesh->Rotation.X);
			geXForm3d_RotateY(&thePosition, pMesh->Rotation.Y);
			geXForm3d_Translate(&thePosition, pMesh->origin.X, pMesh->origin.Y, pMesh->origin.Z);
			geXForm3d_Transform(&thePosition, &(MeshList[pMesh->ListIndex]->OBBox[0].Center), &Center);
			float dist = (geVec3d_DistanceBetween(&CamPosition, &Center) / CCD->CameraManager()->AmtZoom());
// end change

			if(CCD->GetLODdistance(0) != 0 && dist > CCD->GetLODdistance(0))
			{
				if(MeshList[pMesh->ListIndex]->Verts[1])
					LOD = 1;
			}
			if(CCD->GetLODdistance(1) != 0 && dist > CCD->GetLODdistance(1))
			{
				if(MeshList[pMesh->ListIndex]->Verts[2])
					LOD = 2;
			}
			if(CCD->GetLODdistance(2) != 0 && dist > CCD->GetLODdistance(2))
			{
				if(MeshList[pMesh->ListIndex]->Verts[3])
					LOD = 3;
			}
			if(dist < CCD->GetLODdistance(4) || CCD->GetLODdistance(4) == 0)
			{
				if(CCD->GetLODdistance(3) != 0 && dist > CCD->GetLODdistance(3) && MeshList[pMesh->ListIndex]->LODBitmap)
					LOD = 4;
			}
			else
				LOD = 5;
		}

		if(LOD > 3)
			continue;
// end change

		// set up the transformation matrix
		geXForm3d_SetZRotation(&thePosition, pMesh->Rotation.Z);
		geXForm3d_RotateX(&thePosition, pMesh->Rotation.X);
		geXForm3d_RotateY(&thePosition, pMesh->Rotation.Y);

		geXForm3d_Translate(&thePosition, pMesh->origin.X, pMesh->origin.Y, pMesh->origin.Z);

		// get the transpose, we will transform the line/box instead of the whole mesh - this should be faster
		geXForm3d_GetTranspose(&thePosition, &thePositionT); //(M, MTranspose)

		// now we can scale it (the matrix had to be orthonormal (=without scale) to transpose it)
		geXForm3d_SetScaling(&theScale, pMesh->Scale, pMesh->Scale, pMesh->Scale);
		geXForm3d_Multiply(&thePosition, &theScale, &thePosition);

		geFloat s;
		geVec3d PlaneNormal;

		// bounding box collision test - start
		if(pMesh->ColCheckLevel > 0 && pMesh->ColCheckLevel < 3)
		{
			geVec3d Axis[3], ALength, Center;
			geXForm3d theRotation;
			int i;

			geXForm3d_SetZRotation(&theRotation, pMesh->Rotation.Z);
			geXForm3d_RotateX(&theRotation, pMesh->Rotation.X);
			geXForm3d_RotateY(&theRotation, pMesh->Rotation.Y);

			for(i=0; i<3; ++i)
				geXForm3d_Rotate(&theRotation, &(MeshList[pMesh->ListIndex]->OBBox[LOD].Axis[i]), &Axis[i]);

			geXForm3d_Transform(&thePosition, &(MeshList[pMesh->ListIndex]->OBBox[LOD].Center), &Center);

			geVec3d_Scale(&(MeshList[pMesh->ListIndex]->OBBox[LOD].ALength), pMesh->Scale, &ALength);
			AABBofOBB(&(Box.Min), &(Box.Max), &ALength, &(Axis[0]), &(Axis[1]), &(Axis[2]));

			geVec3d_Add(&Center, &(Box.Max), &(Box.Max));
			geVec3d_Add(&Center, &(Box.Min), &(Box.Min));

			if(Min && Max) // there's a boundingbox, do a box-box collision
			{
				// using same method as genesis actor collision now
				for (i=0; i<3; ++i)
				{
					if(geVec3d_GetElement(&TestBox.Max, i) < geVec3d_GetElement(&Box.Min, i))
						break;

					if (geVec3d_GetElement(&TestBox.Min, i) > geVec3d_GetElement(&Box.Max, i))
						break;
				}

				ColResult = (i!=3)? GE_FALSE:GE_TRUE;

				if(ColResult && pMesh->ColCheckLevel < 2)
				{
					if(Collision)
					{
						geVec3d_Subtract(&Box.Min, Max, &Box.Min);
						geVec3d_Subtract(&Box.Max, Min, &Box.Max);
						if(!geExtBox_RayCollision(&Box, &OldPosition, &NewPosition, &s, &(Collision->Plane.Normal)))
							continue;

						if(s < 0.0f || s >= 1.0f)
							continue;

						if(s < Gs)
						{
							Gs = s;
							geVec3d_AddScaled(&OldPosition, &Line, s, &(Collision->Impact));

							Collision->Plane.Dist = geVec3d_DotProduct(&(Collision->Plane.Normal), &(Collision->Impact));
						}
					}
					else // just want to know if we collide at all
						return GE_TRUE;
				}
			}
			else //no boundingbox was specified, do a simple ray-box collision
			{
				if(!geExtBox_RayCollision(&Box, &OldPosition, &NewPosition, &s, &PlaneNormal))
					ColResult = geExtBox_RayCollision(&Box, &NewPosition, &OldPosition, &s, &PlaneNormal);

				if(ColResult && pMesh->ColCheckLevel < 2)
				{
					if(Collision)
					{
						if(s < Gs)
						{
							Gs = s;
							geVec3d_AddScaled(&OldPosition, &Line, Gs, &Collision->Impact);
							Collision->Plane.Normal=PlaneNormal;
// changed QD 10/01/2004
							Collision->Plane.Dist = geVec3d_DotProduct(&(Collision->Plane.Normal), &(Collision->Impact));
						}
					}
					else // just want to know if we collide at all
						return GE_TRUE;
// end change
				}
			}

			if(!ColResult)
				continue;
		}
		// bounding box collision test - end

		/////////////////////////////////////////////////////////////////////////////
		// face collision test - collision level > 1
		if((pMesh->ColCheckLevel == 2 && ColResult == GE_TRUE)||(pMesh->ColCheckLevel > 2))
		{
			ColResult = GE_FALSE;
			geVec3d m_vDir;
			geVec3d OldPos;
			geXForm3d theRotationT;

			// scale the transposed matrix
			geFloat InvScale = 1.f/pMesh->Scale;
			geXForm3d_Scale(&thePositionT, InvScale, InvScale, InvScale);

			geXForm3d_Transform(&thePositionT, &OldPosition, &OldPos);

			// get a xform for the rotation only
			geXForm3d_SetYRotation(&theRotationT, -pMesh->Rotation.Y);
			geXForm3d_RotateX(&theRotationT, -pMesh->Rotation.X);
			geXForm3d_RotateZ(&theRotationT, -pMesh->Rotation.Z);

			//	don't translate the direction!
			geXForm3d_Rotate(&theRotationT, &Line, &m_vDir);
			geVec3d_Scale(&m_vDir, InvScale, &m_vDir);

			if(Min && Max && (pMesh->ColCheckLevel < 4))
			{
				geVec3d center, axis[3];
				geFloat HalfAxisLength[3];
				int i;

				// transform OBB into object space
				for(i=0; i<3; ++i)
				{
					geXForm3d_Rotate(&theRotationT, &OBBaxis[i], &axis[i]);
					HalfAxisLength[i] = OBBHalfAxisLength[i]*InvScale;
				}
				geXForm3d_Transform(&thePositionT, &OBBcenter, &center);

				// test each triangle
				for(i=0; i<MeshList[pMesh->ListIndex]->NumFaces[LOD]; i++)
				{
					geVec3d Vertex[3];
					int VertIndex, j;

					// get the vertices of the actual triangle
					for(j=0; j<3; ++j)
					{
						VertIndex = MeshList[pMesh->ListIndex]->FaceI[LOD][i*3+j];

						Vertex[j].X = MeshList[pMesh->ListIndex]->Verts[LOD][VertIndex].X;
						Vertex[j].Y = MeshList[pMesh->ListIndex]->Verts[LOD][VertIndex].Y;
						Vertex[j].Z = MeshList[pMesh->ListIndex]->Verts[LOD][VertIndex].Z;
					}

					//************************************************************************
					// collision test using the seperating axis theorem
					// for more information look at David Eberly's site www.magic-software.com
					// Dynamic Collision Detection Using Oriented Bounding Boxes
					//************************************************************************

					geFloat Min0, Max0, Min1, Max1, Speed, TFirst, TLast;
					geFloat D_C;
					geVec3d V, edge[3];

					TFirst = 0.0f;
					TLast = 1.01f;

					// direction of the triangle normal
					geVec3d_Subtract(&Vertex[1], &Vertex[0], &edge[0]);
					geVec3d_Subtract(&Vertex[2], &Vertex[0], &edge[1]);

					geVec3d_CrossProduct(&edge[0], &edge[1], &V);

					Min0 = V.X*Vertex[0].X+V.Y*Vertex[0].Y+V.Z*Vertex[0].Z;
					Max0 = Min0;

					//ObbProject
					{
						geFloat DP = V.X*center.X+V.Y*center.Y+V.Z*center.Z;
						geFloat R = 0.0f;

						for(j=0; j<3; ++j)
						{
							R += HalfAxisLength[j]*fabs(V.X*axis[j].X + V.Y*axis[j].Y + V.Z*axis[j].Z);
						}

						Min1 = DP-R;
						Max1 = DP+R;
					}

					Speed = V.X*m_vDir.X+V.Y*m_vDir.Y+V.Z*m_vDir.Z;
					if(NoIntersect(Speed, Min0, Max0, Min1, Max1, &TFirst, &TLast))
						goto NO_COLLISION;//continue;

					//direction of the OBB planes
					for(j=0; j<3; ++j)
					{
						V = axis[j];
						// TriProject
						{
							Min0 = V.X*Vertex[0].X+V.Y*Vertex[0].Y+V.Z*Vertex[0].Z;
							Max0 = Min0;

							geFloat DP = V.X*Vertex[1].X+V.Y*Vertex[1].Y+V.Z*Vertex[1].Z;;
							if(DP < Min0)
								Min0 = DP;
							else if(DP > Max0)
								Max0 = DP;

							DP = V.X*Vertex[2].X+V.Y*Vertex[2].Y+V.Z*Vertex[2].Z;
							if(DP < Min0)
								Min0 = DP;
							else if(DP > Max0)
								Max0 = DP;
						}

						D_C = V.X*center.X+V.Y*center.Y+V.Z*center.Z;
						Min1 = D_C-HalfAxisLength[j];
						Max1 = D_C+HalfAxisLength[j];

						Speed = V.X*m_vDir.X+V.Y*m_vDir.Y+V.Z*m_vDir.Z;
						if(NoIntersect(Speed, Min0, Max0, Min1, Max1, &TFirst, &TLast))
							goto NO_COLLISION;//continue;
					}

					geVec3d_Subtract(&edge[1], &edge[0], &edge[2]);
					for(j=0; j<3; ++j)
					{
						for(int k=0; k<3; ++k)
						{
							geVec3d_CrossProduct(&edge[j], &axis[k], &V);

							//TriProject
							{
								Min0 = V.X*Vertex[0].X+V.Y*Vertex[0].Y+V.Z*Vertex[0].Z;
								Max0 = Min0;

								geFloat DP = V.X*Vertex[1].X+V.Y*Vertex[1].Y+V.Z*Vertex[1].Z;;
								if(DP < Min0)
									Min0 = DP;
								else if(DP > Max0)
									Max0 = DP;

								DP = V.X*Vertex[2].X+V.Y*Vertex[2].Y+V.Z*Vertex[2].Z;
								if(DP < Min0)
									Min0 = DP;
								else if(DP > Max0)
									Max0 = DP;
							}
							//ObbProj
							{
								geFloat DP = V.X*center.X+V.Y*center.Y+V.Z*center.Z;
								geFloat R = 0.0f;

								for(int m=0; m<3; ++m)
								{
									R += HalfAxisLength[m]*fabs(V.X*axis[m].X + V.Y*axis[m].Y + V.Z*axis[m].Z);
								}

								Min1 = DP-R;
								Max1 = DP+R;
							}

							Speed = V.X*m_vDir.X+V.Y*m_vDir.Y+V.Z*m_vDir.Z;
							if(NoIntersect(Speed, Min0, Max0, Min1, Max1, &TFirst, &TLast))
								goto NO_COLLISION;//continue;
						}
					}

					// there is no seperating axis, the objects collide
					ColResult = GE_TRUE;
					if(Collision)
					{
						if(TFirst < Gs)
						{
							Gs = TFirst;
							geVec3d_AddScaled(&OldPos, &m_vDir, Gs, &(Collision->Impact));
							geXForm3d_Transform(&thePosition, &(Collision->Impact), &(Collision->Impact));

							geVec3d_CrossProduct(&edge[0], &edge[1], &(Collision->Plane.Normal));
							geXForm3d_Transform(&thePosition, &(Collision->Plane.Normal), &(Collision->Plane.Normal));
							geVec3d_Normalize(&(Collision->Plane.Normal));

// changed QD 09/28/2004
							Collision->Plane.Dist = geVec3d_DotProduct(&(Collision->Plane.Normal), &(Collision->Impact));
// end change
						}
					}
// changed QD 10/01/2004
					else // just want to know if we collide at all
						return GE_TRUE;
// end change
NO_COLLISION:;
				}

			}
			else
			{
				geFloat fL = geVec3d_Normalize(&m_vDir);

				for(int i=0; i<MeshList[pMesh->ListIndex]->NumFaces[LOD]; i++)
				{
					geVec3d Vertex[3];
					int VertIndex, j;

					for(j=0; j<3; ++j)
					{
						VertIndex = MeshList[pMesh->ListIndex]->FaceI[LOD][i*3+j];

						Vertex[j].X = MeshList[pMesh->ListIndex]->Verts[LOD][VertIndex].X;
						Vertex[j].Y = MeshList[pMesh->ListIndex]->Verts[LOD][VertIndex].Y;
						Vertex[j].Z = MeshList[pMesh->ListIndex]->Verts[LOD][VertIndex].Z;
					}

					//*********************************************************************
					// Fast, Minimum Storage Ray-Triangle Intersection by Möller & Trumbore
					//*********************************************************************

					geVec3d edge1, edge2;
					geVec3d pvec, tvec, qvec, PlaneNormal;

					// find vectors for 2 edges sharing vert0
					geVec3d_Subtract(&Vertex[1], &Vertex[0], &edge1);
					geVec3d_Subtract(&Vertex[2], &Vertex[0], &edge2);

					// begin calculating determinant - also used to calculate U parameter
					geVec3d_CrossProduct(&m_vDir, &edge2, &pvec);
					geFloat det = edge1.X*pvec.X+edge1.Y*pvec.Y+edge1.Z*pvec.Z;// geVec3d_DotProduct(&edge1, &pvec);

					// the line and the plane are almost parallel, if determinant is near zero
					if(det < 0.0001f && det > -0.0001f)
						continue;

					geFloat inv_det = 1.0f/det;

					// calculate distance from vert0 to ray origin
					geVec3d_Subtract(&OldPos, &Vertex[0], &tvec);
					// calculate U parameter and test bounds
					geFloat u = (tvec.X*pvec.X+tvec.Y*pvec.Y+tvec.Z*pvec.Z)*inv_det;//geVec3d_DotProduct(&tvec, &pvec);

					if(u < 0.0f || u > 1.0f)
						continue;

					// prepare to test V parameter
					geVec3d_CrossProduct(&tvec, &edge1, &qvec);
					// calculate V parameter and test bounds
					geFloat v = (m_vDir.X*qvec.X+m_vDir.Y*qvec.Y+m_vDir.Z*qvec.Z)*inv_det;//geVec3d_DotProduct(&m_vDir, &qvec);

					if(v < 0.0f || u+v > 1.0f)
						continue;

					// calculate s, ray intersects triangle
					s = (edge2.X*qvec.X + edge2.Y*qvec.Y + edge2.Z*qvec.Z)*inv_det;//geVec3d_DotProduct(&edge2, &qvec);

					// collision but not on segment?
					if(s >= fL||s < 0.0f)
						continue;
					else
					{
						if(Collision)
						{
							if((s/fL) < Gs)
							{
								Gs = s/fL;
								geVec3d_AddScaled(&OldPos, &m_vDir, s, &(Collision->Impact));
								geXForm3d_Transform(&thePosition, &(Collision->Impact), &(Collision->Impact));

								geVec3d_CrossProduct(&edge1, &edge2, &PlaneNormal);
								geXForm3d_Transform(&thePosition, &PlaneNormal, &(Collision->Plane.Normal));
								geVec3d_Normalize(&(Collision->Plane.Normal));

// changed QD 09/28/2004
								Collision->Plane.Dist = geVec3d_DotProduct(&(Collision->Plane.Normal), &(Collision->Impact));
// end change

							}
						}
// changed QD 10/01/2004
						else // just want to know if we collide at all
							return GE_TRUE;
// end change
						ColResult = GE_TRUE;
					}
				}//for(all triangles)
			}//if(Min&&Max)
		}//if(ColLevel)

		if(ColResult)
			GColResult = GE_TRUE;
	}
	return GColResult;
}

/* ------------------------------------------------------------------------------------ */
//	Obb center is supposed to be at {0, 0, 0}
/* ------------------------------------------------------------------------------------ */
void CStaticMesh::AABBofOBB(geVec3d *Min, geVec3d *Max, const geVec3d *AxisLength,
			   const geVec3d *Axis0, const geVec3d *Axis1, const geVec3d *Axis2)
{
	geVec3d A0, A1, A2;

	A0.X = fabs(Axis0->X*AxisLength->X);
	A0.Y = fabs(Axis0->Y*AxisLength->X);
	A0.Z = fabs(Axis0->Z*AxisLength->X);

	A1.X = fabs(Axis1->X*AxisLength->Y);
	A1.Y = fabs(Axis1->Y*AxisLength->Y);
	A1.Z = fabs(Axis1->Z*AxisLength->Y);

	A2.X = fabs(Axis2->X*AxisLength->Z);
	A2.Y = fabs(Axis2->Y*AxisLength->Z);
	A2.Z = fabs(Axis2->Z*AxisLength->Z);

	// MinX, MaxX
	if(A0.X > A1.X)
	{
		if(A0.X > A2.X)
		{
			Max->X = A0.X;
			Min->X = -A0.X;
		}
		else
		{
			Max->X = A2.X;
			Min->X = -A2.X;
		}
	}
	else
	{
		if(A1.X > A2.X)
		{
			Max->X = A1.X;
			Min->X = -A1.X;
		}
		else
		{
			Max->X = A2.X;
			Min->X = -A2.X;
		}
	}

	// MinY, MaxY
	if(A0.Y > A1.Y)
	{
		if(A0.Y > A2.Y)
		{
			Max->Y = A0.Y;
			Min->Y = -A0.Y;
		}
		else
		{
			Max->Y = A2.Y;
			Min->Y = -A2.Y;
		}
	}
	else
	{
		if(A1.Y > A2.Y)
		{
			Max->Y = A1.Y;
			Min->Y = -A1.Y;
		}
		else
		{
			Max->Y = A2.Y;
			Min->Y = -A2.Y;
		}
	}

	// MinZ, MaxZ
	if(A0.Z > A1.Z)
	{
		if(A0.Z > A2.Z)
		{
			Max->Z = A0.Z;
			Min->Z = -A0.Z;
		}
		else
		{
			Max->Z = A2.Z;
			Min->Z = -A2.Z;
		}
	}
	else
	{
		if(A1.Z > A2.Z)
		{
			Max->Z = A1.Z;
			Min->Z = -A1.Z;
		}
		else
		{
			Max->Z = A2.Z;
			Min->Z = -A2.Z;
		}
	}
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
int CStaticMesh::SaveTo(FILE *SaveFD, bool type)
{
	if(m_MeshCount == 0)
		return RGF_SUCCESS;							// No meshes in world, bail early

	// Ok, see if we have any static mesh entities
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "StaticMesh");

	if(!pSet)
		return RGF_SUCCESS;

	geEntity *pEntity;

	//	Ok, we have StaticMesh entities somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		StaticMesh *pMesh = static_cast<StaticMesh*>(geEntity_GetUserData(pEntity));

		WRITEDATA(type, &pMesh->origin,		sizeof(geVec3d), 1, SaveFD);
		WRITEDATA(type, &pMesh->Rotation,	sizeof(geVec3d), 1, SaveFD);
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
int CStaticMesh::RestoreFrom(FILE *RestoreFD, bool type)
{
	if(m_MeshCount == 0)
		return RGF_SUCCESS;							// No meshes in world, bail early

	// Ok, see if we have any static entity proxies
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "StaticMesh");

	if(!pSet)
		return RGF_SUCCESS;

	geEntity *pEntity;

	//	Ok, we have StaticMesh entities somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		StaticMesh *pMesh = static_cast<StaticMesh*>(geEntity_GetUserData(pEntity));
		READDATA(type, &pMesh->origin,		sizeof(geVec3d), 1, RestoreFD);
		READDATA(type, &pMesh->Rotation,	sizeof(geVec3d), 1, RestoreFD);
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
int CStaticMesh::LocateEntity(const char *szName, void **pEntityData)
{
	// Ok, check to see if there are static mesh entities in this world
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "StaticMesh");

	if(!pSet)
		return RGF_NOT_FOUND;									// No static entity proxies

	geEntity *pEntity;

	//	Ok, we have static meshes.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		StaticMesh *pTheEntity = static_cast<StaticMesh*>(geEntity_GetUserData(pEntity));

		if(!strcmp(pTheEntity->szEntityName, szName))
		{
			*pEntityData = static_cast<void*>(pTheEntity);
			return RGF_SUCCESS;
		}
	}

	return RGF_NOT_FOUND;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
int CStaticMesh::ReSynchronize()
{
	return RGF_SUCCESS;
}

/* ----------------------------------- END OF FILE ------------------------------------ */
