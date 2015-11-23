#include "..\\RabidFramework.h"
#include "qxTerrainDefinitionFile.h"

qxTerrainDefinitionFile::qxTerrainDefinitionFile( const char* pFileName )
:CStdioFile()
,m_pTerrainDef(0)
{

}

qxTerrainDefinitionFile::~qxTerrainDefinitionFile()
{
}

//
// CheckWorldBounds()
//
// Parse the file and determine the defined limits of the world
// based on what is there
void qxTerrainDefinitionFile::CheckWorldBounds()
{

}

bool qxTerrainDefinitionFile::Load( int X, int Z )
{
	m_pTerrainDef->m_eType = TT_LAND;
	//m_pTerrainDef->m_strHeightmap = "terrain\\kauai.bmp";
	m_pTerrainDef->m_strHeightmap = "terrain\\flat.bmp";
	m_pTerrainDef->m_strBmp = "terrain\\desert256.bmp";
	//m_pTerrainDef->m_strBmp = "terrain\\waterg.bmp";
	m_pTerrainDef->m_eHeightFieldSourceType = HEIGHTFIELD_GREY_BMP;
	//m_pTerrainDef->m_eHeightFieldSourceType = HEIGHTFIELD_FRACTAL;
	m_pTerrainDef->MapOffsetIndexX = X;
	m_pTerrainDef->MapOffsetIndexZ = Z;
	m_pTerrainDef->m_nFarPlane = CCD->TerrainMgr()->GetFarPlane();
	return true;
}

inline bool qxTerrainDefinitionFile::GetNextHeader()
{

	return false;
}

inline bool qxTerrainDefinitionFile::ParseDefinition()
{

	return true;
}

	


