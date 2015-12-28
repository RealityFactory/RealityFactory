
#include "..\\RabidFramework.h"
#include "qxTerrainDefinitionFile.h"

qxTerrainDefinitionFile::qxTerrainDefinitionFile( const char* pFileName )
{
	m_pTerrainDef = 0;
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


