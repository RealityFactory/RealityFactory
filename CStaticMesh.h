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
	GE_LVertex*	Verts[4];
	int16*		MaterialI[4];
	int16*		FaceI[4];
	GE_RGBA*	Color[4];
	geBitmap**	Bitmaps[4];
	int			NumFaces[4];
	int			MaterialCount[4];
	int			NumVerts[4];
	int			NumVNormals[4];
	geVec3d*	VNormals[4];
	int16*		VNormalI[4];
	geBitmap*	LODBitmap;
	char*		szFilename;
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

	/**
	 * @brief Given a name, locate the desired entity in the currently loaded
	 * level and return its user data.
	 */
	int LocateEntity(const char *szName, void **pEntityData);

	/**
	 * @brief Correct internal timing to match current time, to make up for time
	 * lost when outside the game loop (typically in "menu mode").
	 */
	int ReSynchronize();

	bool CollisionCheck(geVec3d *Min, geVec3d *Max,
						const geVec3d &OldPosition, const geVec3d &NewPosition,
						GE_Collision *Collision);

	geBoolean RayTracing(StaticMesh *CallingMesh, int LOD,
						const geVec3d &OldPosition, const geVec3d &NewPosition,
						GE_Collision *Collision);

	void AABBofOBB(geVec3d *Min, geVec3d *Max,
					const geVec3d *AxisLength,
					const geVec3d *Axis0,
					const geVec3d *Axis1,
					const geVec3d *Axis2);

private:
	void ComputeLighting(StaticMesh *pMesh, void *pLight, int LType);

	void SetAmbientLight(StaticMesh *pMesh);

	bool IsInList(const char *szActorFile, int *Index);

	void CleanUp();

private:
	int m_MeshCount;
	LoadedMeshList *MeshList[MESH_LIST_SIZE];
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
