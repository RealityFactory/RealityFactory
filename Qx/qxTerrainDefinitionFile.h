
#ifndef qxTERRAIN_DEFINITION_FILE_H
#define qxTERRAIN_DEFINITION_FILE_H


enum eTerrainType
{
	TT_NONE,
	TT_OCEAN,
	TT_BEACH,
	TT_LAND
};

enum eHeightFieldSourceType
{
	HEIGHTFIELD_FRACTAL,
	HEIGHTFIELD_GREY_BMP,
	HEIGHTFIELD_TER_FILE

};


class qxTerrainDefinition
{

public:


	qxTerrainDefinition()
	:
	MapOffsetIndexX(0)
	,MapOffsetIndexZ(0)
	,m_fLatitude(0.0f)
	,m_fLongitude(0.0f)
	,m_eType( TT_LAND )
	,m_eHeightFieldSourceType(HEIGHTFIELD_GREY_BMP)
	,m_nFarPlane(9999999)

	,m_nWaterTileCountX(8)
	,m_nWaterTileCountZ(8)
	,m_nWaterMapHeight(1025)
	,m_nWaterMapLength(1025)
	,m_nWaterLevel(0)
	{};

	~qxTerrainDefinition(){};

	int				MapOffsetIndexX;
	int				MapOffsetIndexZ;
	float			m_fLatitude;
	float			m_fLongitude;
	eTerrainType	m_eType;
	string			m_strHeightmap;
	string			m_strBmp;
	string			m_strAlpha;
	int				m_nFarPlane;

	eHeightFieldSourceType m_eHeightFieldSourceType;

	// The following apply to TT_OCEAN only
	// since we don't have a heightmap
	int				m_nWaterTileCountX;
	int				m_nWaterTileCountZ;
	int				m_nWaterMapHeight;
	int				m_nWaterMapLength;
	int				m_nWaterLevel;


};


class qxTerrainDefinitionFile// : public CStdioFile
{

public:

	qxTerrainDefinitionFile( const char* pFileName );
	~qxTerrainDefinitionFile();
	bool Load( int X, int Y);
	bool GetNextHeader();
	bool ParseDefinition();
	void CheckWorldBounds();
	qxTerrainDefinition* m_pTerrainDef;

	string m_strCurrent;

};


#endif