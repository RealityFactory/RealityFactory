// CStaticMesh.h: interface for the CStaticMesh class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CSTATICMESH_H__05862756_3BC3_4A6D_9E13_7615355493E7__INCLUDED_)
#define AFX_CSTATICMESH_H__05862756_3BC3_4A6D_9E13_7615355493E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CRGFComponent.h"

#define	MESH_LIST_SIZE		1024

struct OBB
{
	geVec3d ALength;
	geVec3d Axis[3];
	geVec3d Center;
};

struct LoadedMeshList
{
	OBB OBBox[4];
	geVec3d Max[4];
	geVec3d Min[4];
	GE_LVertex *Verts[4];
	int16 *MaterialI[4];
	int16 *FaceI[4];
	GE_RGBA *Color[4];
	geBitmap **Bitmaps[4];
	int		NumFaces[4];
	int		MaterialCount[4];
	int		NumVerts[4];
// changed QD 02/12/2004
	int		NumVNormals[4];
	geVec3d *VNormals[4];
	int16	*VNormalI[4];
// end change
	geBitmap *LODBitmap;
	char * szFilename;
};

class CStaticMesh : public CRGFComponent  
{
public:
	CStaticMesh();
	virtual ~CStaticMesh();

	void Tick(geFloat dwTicks);

	bool AddNewMesh(char *szActorFile);

	void AddPoly(StaticMesh *pMesh, int LOD);
		

	bool IsInList(char *szActorFile, int *Index);
	int SaveTo(FILE *SaveFD, bool type);					// Save states to a supplied file
	int RestoreFrom(FILE *RestoreFD, bool type);			// Restore states from a supplied file
	int LocateEntity(char *szName, void **pEntityData);
	int ReSynchronize();
	bool CollisionCheck(geVec3d *Min, geVec3d *Max, geVec3d OldPosition, geVec3d NewPosition, GE_Collision *Collision);
// changed QD 09/28/2004
//	void ComputeSunLight(StaticMesh *pMesh, geVec3d *Angles, GE_RGBA *Color);
	geBoolean RayTracing(StaticMesh *CallingMesh, int LOD, geVec3d OldPosition, geVec3d NewPosition, GE_Collision *Collision);
// end change
	void AABBofOBB(geVec3d *Min, geVec3d *Max, const geVec3d *AxisLength, const geVec3d *Axis0, const geVec3d *Axis1, const geVec3d *Axis2);
private:
	int m_MeshCount;
	LoadedMeshList *MeshList[MESH_LIST_SIZE];
// changed QD 09/28/2004
	void ComputeLighting(StaticMesh *pMesh, void* pLight, int LType);
	void SetAmbientLight(StaticMesh *pMesh);
// end change

};

#endif // !defined(AFX_CSTATICMESH_H__05862756_3BC3_4A6D_9E13_7615355493E7__INCLUDED_)
