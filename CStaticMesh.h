/************************************************************************************//**
 * @file CStaticMesh.h
 * @brief Interface for the CStaticMesh class
 * @author Daniel Queteschiner
 *//*
 * Copyright (c) 2004 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CSTATICMESH_H_
#define __RGF_CSTATICMESH_H_

#define	MESH_LIST_SIZE		1024

struct OBB
{
	geVec3d ALength;
	geVec3d Axis[3];
	geVec3d Center;
};

struct LoadedMeshList
{
	OBB			OBBox[4];
	geVec3d		Max[4];
	geVec3d		Min[4];
	GE_LVertex	*Verts[4];
	int16		*MaterialI[4];
	int16		*FaceI[4];
	GE_RGBA		*Color[4];
	geBitmap	**Bitmaps[4];
	int			NumFaces[4];
	int			MaterialCount[4];
	int			NumVerts[4];
// changed QD 02/12/2004
	int			NumVNormals[4];
	geVec3d		*VNormals[4];
	int16		*VNormalI[4];
// end change
	geBitmap	*LODBitmap;
	char		*szFilename;
};

class CStaticMesh : public CRGFComponent
{
public:
	CStaticMesh();
	virtual ~CStaticMesh();

	void Tick(geFloat dwTicks);
	bool AddNewMesh(const char *szActorFile);
	void AddPoly(StaticMesh *pMesh, int LOD);

	int SaveTo(FILE *SaveFD, bool type);					// Save states to a supplied file
	int RestoreFrom(FILE *RestoreFD, bool type);			// Restore states from a supplied file
	int LocateEntity(const char *szName, void **pEntityData);
	int ReSynchronize();

	bool CollisionCheck(geVec3d *Min, geVec3d *Max,
						const geVec3d &OldPosition, const geVec3d &NewPosition,
						GE_Collision *Collision);
// changed QD 09/28/2004
//	void ComputeSunLight(StaticMesh *pMesh, geVec3d *Angles, GE_RGBA *Color);
	geBoolean RayTracing(StaticMesh *CallingMesh, int LOD,
						const geVec3d &OldPosition, const geVec3d &NewPosition,
						GE_Collision *Collision);
// end change
	void AABBofOBB(geVec3d *Min, geVec3d *Max,
					const geVec3d *AxisLength,
					const geVec3d *Axis0,
					const geVec3d *Axis1,
					const geVec3d *Axis2);


private:
// changed QD 09/28/2004
	void ComputeLighting(StaticMesh *pMesh, void *pLight, int LType);
	void SetAmbientLight(StaticMesh *pMesh);
// end change
// changed QD 12/15/05
	bool IsInList(const char *szActorFile, int *Index);
	void CleanUp();
// end change

private:
	int m_MeshCount;
	LoadedMeshList *MeshList[MESH_LIST_SIZE];
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
