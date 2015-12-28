/****************************************************************************************/
/*	qxTerrainMgr.cpp: implementation of the qxTerrainMgr class.							*/
/*																						*/
/*	Author: Jeff Thelen																	*/
/*																						*/
/*	Copyright 2000 Quixotic, Inc.  All Rights Reserved.									*/
/*																						*/
/*	Edit History:																		*/
/*	=============																		*/
/*	02/01/07 QD:	- added skydome Y offset											*/
/*																						*/
/****************************************************************************************/

#include "..\\RabidFramework.h"
#include "qxTerrainUtils.h"
#include "qxTerrainMap.h"
#include "qxTerrainMapBase.h"
#include "qxColor.h"
#include "qxCloudMachine.h"
#include "qxSun.h"
#include "qxMoon.h"
#include "qxStars.h"
#include "qxEffectTextureFlow.h"
#include "qxTerrainDefinitionFile.h"
#include "qxSkyDome.h"
#include "QxUser.h"

#include "..\\Simkin\\skScriptedExecutable.h"
#include "..\\Simkin\\skRValue.h"
#include "..\\Simkin\\skRValueArray.h"
#include "..\\Simkin\\skRuntimeException.h"
#include "..\\Simkin\\skParseException.h"
#include "..\\Simkin\\skBoundsException.h"
#include "..\\Simkin\\skTreeNode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


TerrainObject::TerrainObject(char *fileName) : skScriptedExecutable(fileName, CCD->GetskContext()) //change simkin
{
	Order[0] = '\0';
	ElapseTime = 0.0f;
	strcpy(szName, "SkyDome");
}


TerrainObject::~TerrainObject()
{
}


bool TerrainObject::method(const skString& methodName, skRValueArray& arguments,
						   skRValue& returnValue, skExecutableContext& ctxt)
{
	char param0[128];
	float param1;
	int param3;
	param0[0] = '\0';

	if (IS_METHOD(methodName, "RenderWireframe"))
	{
		PARMCHECK(1);
		bool flag = arguments[0].boolValue();
		CCD->TerrainMgr()->SetRenderWireframe(flag);
		return true;
	}
	else if (IS_METHOD(methodName, "RenderLandscape"))
	{
		PARMCHECK(1);
		bool flag = arguments[0].boolValue();
		CCD->TerrainMgr()->SetRenderLandscape(flag);
		return true;
	}
	else if (IS_METHOD(methodName, "SetVertScale"))
	{
		PARMCHECK(1);
		param1 = arguments[0].floatValue();
		CCD->TerrainMgr()->SetScaleY(param1);
		return true;
	}
	else if (IS_METHOD(methodName, "SetHorizScale"))
	{
		PARMCHECK(1);
		param1 = arguments[0].floatValue();
		CCD->TerrainMgr()->SetScaleXZ(param1);
		return true;
	}
	else if (IS_METHOD(methodName, "SetHeightOrigin"))
	{
		PARMCHECK(1);
		param3 = arguments[0].intValue();
		CCD->TerrainMgr()->SetOffsetY(param3);
		return true;
	}
	else if (IS_METHOD(methodName, "SetSkyDomeHeightOffset"))
	{
		PARMCHECK(1);
		param3 = arguments[0].intValue();
		CCD->TerrainMgr()->SetSkyDomeOffsetY(param3);
		return true;
	}
	else if (IS_METHOD(methodName, "SetDesiredTriangles"))
	{
		PARMCHECK(1);
		param3 = arguments[0].intValue();
		CCD->TerrainMgr()->SetDesiredTriangles(param3);
		return true;
	}
	else if (IS_METHOD(methodName, "SetLandscapeSize"))
	{
		PARMCHECK(1);
		param3 = arguments[0].intValue();
		CCD->TerrainMgr()->SetLandscapeSize(param3);
		return true;
	}
	else if (IS_METHOD(methodName, "SetAmbientLightColor"))
	{
		PARMCHECK(4);
		float r = arguments[0].floatValue();
		float g = arguments[1].floatValue();
		float b = arguments[2].floatValue();
		float a = arguments[3].floatValue();
		CCD->TerrainMgr()->SetAmbientLightColor(r,g,b,a);
		return true;
	}
	else if (IS_METHOD(methodName, "SetTwilightDistanceFromHorizon"))
	{
		PARMCHECK(1);
		param1 = arguments[0].floatValue();
		CCD->TerrainMgr()->SetTwilightDistanceFromHorizon(param1);
		return true;
	}
	else if (IS_METHOD(methodName, "SetTimeScale"))
	{
		PARMCHECK(1);
		param1 = arguments[0].floatValue();
		CCD->TerrainMgr()->SetTimeScale(param1);
		return true;
	}
	else if (IS_METHOD(methodName, "SetSkyDome"))
	{
		PARMCHECK(2);
		int param6 = arguments[0].intValue();
		float param5 = arguments[1].floatValue();
		CCD->TerrainMgr()->SetSky(param6, param5);
		return true;
	}
	else if (IS_METHOD(methodName, "SetSkyTexture"))
	{
		PARMCHECK(1);
		strcpy(param0, "terrain\\");
		strcat(param0, arguments[0].str());
		CCD->TerrainMgr()->SetSkyTexture(param0);
		return true;
	}
	else if (IS_METHOD(methodName, "SetSkyColor"))
	{
		PARMCHECK(4);
		float r = arguments[0].floatValue();
		float g = arguments[1].floatValue();
		float b = arguments[2].floatValue();
		float a = arguments[3].floatValue();
		CCD->TerrainMgr()->SetSkyColor(r,g,b,a);
		return true;
	}
	else if (IS_METHOD(methodName, "SetTwilightColor"))
	{
		PARMCHECK(4);
		float r = arguments[0].floatValue();
		float g = arguments[1].floatValue();
		float b = arguments[2].floatValue();
		float a = arguments[3].floatValue();
		CCD->TerrainMgr()->SetTwilightColor(r,g,b,a);
		return true;
	}
	else if (IS_METHOD(methodName, "SetSunColor"))
	{
		PARMCHECK(4);
		float r = arguments[0].floatValue();
		float g = arguments[1].floatValue();
		float b = arguments[2].floatValue();
		float a = arguments[3].floatValue();
		CCD->TerrainMgr()->SetSunColor(r,g,b,a);
		return true;
	}
	else if (IS_METHOD(methodName, "SetDistanceFromSunFactor"))
	{
		PARMCHECK(1);
		param1 = arguments[0].floatValue();
		CCD->TerrainMgr()->SetDistanceFromSunFactor(param1);
		return true;
	}
	else if (IS_METHOD(methodName, "SetSunScale"))
	{
		PARMCHECK(1);
		param1 = arguments[0].floatValue();
		CCD->TerrainMgr()->SetSunScale(param1);
		return true;
	}
	else if (IS_METHOD(methodName, "SetMoonColor"))
	{
		PARMCHECK(4);
		float r = arguments[0].floatValue();
		float g = arguments[1].floatValue();
		float b = arguments[2].floatValue();
		float a = arguments[3].floatValue();
		CCD->TerrainMgr()->SetMoonColor(r,g,b,a);
		return true;
	}
	else if (IS_METHOD(methodName, "SetMoonPhase"))
	{
		PARMCHECK(1);
		int phase = arguments[0].intValue();
		CCD->TerrainMgr()->SetMoonPhase(phase);
		return true;
	}
	else if (IS_METHOD(methodName, "SetLocation"))
	{
		PARMCHECK(4);
		float lat = arguments[0].floatValue();
		int mon = arguments[1].intValue();
		int day = arguments[2].intValue();
		int hour = arguments[3].intValue();
		CCD->TerrainMgr()->SetLocation(lat, mon, day, hour);
		return true;
	}
	else if (IS_METHOD(methodName, "SetTerrain"))
	{
		PARMCHECK(2);
		strcpy(param0, "terrain\\");
		strcat(param0, arguments[0].str());
		char terr[128];
		strcpy(terr, "terrain\\");
		strcat(terr, arguments[1].str());
		CCD->TerrainMgr()->SetTerrain(param0, terr);
		return true;
	}
	else if (IS_METHOD(methodName, "DisableSun"))
	{
		PARMCHECK(1);
		bool flag = arguments[0].boolValue();
		CCD->TerrainMgr()->SetAllowSun(!flag);
		return true;
	}
	else if (IS_METHOD(methodName, "UseSkyFog"))
	{
		PARMCHECK(1);
		bool flag = arguments[0].boolValue();
		CCD->TerrainMgr()->SetAllowSkyFog(flag);
		return true;
	}
	else if (IS_METHOD(methodName, "DisableCloud"))
	{
		PARMCHECK(1);
		bool flag = arguments[0].boolValue();
		CCD->TerrainMgr()->SetAllowCloud(!flag);
		return true;
	}
	else if (IS_METHOD(methodName, "ToggleMoveWithCamera"))
	{
		CCD->TerrainMgr()->ToggleSkyFollow();
		return true;
	}
	else if (IS_METHOD(methodName, "SetWindDirection"))
	{
		PARMCHECK(1);
		strcpy(param0, arguments[0].str());
		eDirection dir = DIRECTION_S;
		if(!stricmp(param0, "northeast"))
			dir = DIRECTION_NE;
		else if(!stricmp(param0, "northwest"))
			dir = DIRECTION_NW;
		else if(!stricmp(param0, "north"))
			dir = DIRECTION_N;
		else if(!stricmp(param0, "southeast"))
			dir = DIRECTION_SE;
		else if(!stricmp(param0, "southwest"))
			dir = DIRECTION_SW;
		else if(!stricmp(param0, "south"))
			dir = DIRECTION_S;
		else if(!stricmp(param0, "east"))
			dir = DIRECTION_E;
		else if(!stricmp(param0, "west"))
			dir = DIRECTION_W;
		CCD->TerrainMgr()->SetWindDir(dir);
		return true;
	}
	else if (IS_METHOD(methodName, "debug"))
	{
		PARMCHECK(1);
		char szBug[128];
		strcpy(param0, arguments[0].str());
		sprintf(szBug, "Debug : %s", param0);
		CCD->ReportError(szBug, false);
		return true;
	}
	else
	{
		return skScriptedExecutable::method(methodName, arguments, returnValue,ctxt);
	}
}


bool TerrainObject::getValue(const skString& fieldName, const skString& attribute, skRValue& value)
{
	if (fieldName == "time")
	{
		value = CCD->TerrainMgr()->GetElapsedTime();
		return true;
	}
	else if (fieldName == "daytime")
	{
		value = CCD->TerrainMgr()->GetTerrainTime();
		return true;
	}
	else
		return skScriptedExecutable::getValue(fieldName, attribute, value);
}


bool TerrainObject::setValue(const skString& fieldName, const skString& attribute, const skRValue& value)
{
	if (fieldName == "think")
	{
		strcpy(Order, value.str());
		return true;
	}
	else
		return skScriptedExecutable::setValue(fieldName, attribute, value);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


qxTerrainMgr::qxTerrainMgr()
:m_bUpdateLandscape(true)
,m_bRenderWireframe(false)
,m_bRenderLandscape(true)
,m_nHeightFieldSourceType(1)
,m_nPolyIncrement(100)
,m_nDistanceDetail(8)
,m_fNormalDistanceToCamera(1000)
,m_pCloudMachine(0)
,m_pSun(0)
,m_pMoon(0)
,m_pStarField(0)
,m_pTerrainDefinitionFile(0)
,m_nWorldBoundsX(10)
,m_nWorldBoundsZ(10)
,m_pSkyDome(0)
,m_fTwilightPercent(0.0)
,TerrainTime(0.0)
,Initialized(false)
{
	m_fScaleY = 1.0f;
	m_fScaleXZ = 4.0f;
	m_OffsetY = 0;
	m_SkyDomeOffsetY = 0;
	m_nDesiredTriangles = 500;
	m_nLandscapeSize = 4096;
	m_nFarPlane = 9999999;
	m_nHeightFieldSourceType = 1;
	m_nShadingMethod = 0;
	m_fNormalDistanceToCamera = 500.0f;
	m_vLightSource.X = -1.0f;
	m_vLightSource.Y = 0.45f;
	m_vLightSource.Z = -1.0f;
	m_fLightAmbient = 0.3f;
	m_fLightIntensity = 1.0f;
	m_PixelFormatFinal = (gePixelFormat)5;
	m_AmbientLightColor.r = 255.0f;
	m_AmbientLightColor.g = 255.0f;
	m_AmbientLightColor.b = 255.0f;
	m_AmbientLightColor.a = 255.0f;
	m_fTwilightDistanceFromHorizon = 0.4f;
	m_TimeScale = 1;
	geVec3d_Normalize(&m_vLightSource);

	strcpy(m_strBmp, "terrain\\clouds_256.bmp");
	strcpy(heightmap, "terrain\\flat.bmp");
	strcpy(texmap, "terrain\\desert256.bmp");
	SkyMaxHeight = 400;
	fScaleXZ = 1.0f;
	fSkyLengthHeight = 32.0f;
	m_SunScale = 4.0f;
	AllowSun = true;
	AllowCloud = true;
	skyfog = false;
	WindDir = DIRECTION_S;
	rgba.r = 164.0f;
	rgba.g = 200.0f;
	rgba.b = 255.0f;
	rgba.a = 255.0f;
	m_fMinBlueSkyColor = 12.0f;
	m_fDistanceFromSunFactor = 900;
	color.r = 255.0f;
	color.g = 0.0f;
	color.b = 0.0f;
	color.a = 255.0f;
	suncolor.r = 255.0f;
	suncolor.g = 128.0f;
	suncolor.b = 0.0f;
	suncolor.a = 255.0f;
	mooncolor.r = 255.0f;
	mooncolor.g = 255.0f;
	mooncolor.b = 255.0f;
	mooncolor.a = 255.0f;
	MoonPhase = MOON_PHASE_FULL;
	m_fColorUpdateTime = 1.0f;
	TextureFlow = false;

	fPlayerLatitude = 49.0f;
	Hour = 12;
	Day = 24;
	Month = 10;
	ElapsedTime = 0.0f;

	Object = NULL;
}


qxTerrainMgr::~qxTerrainMgr()
{
	if(Object)
		delete Object;
	if(Initialized)
		Shutdown();
}


bool qxTerrainMgr::Init()
{
	geEntity_EntitySet* lEntitySet;
	geEntity* lEntity;

	lEntitySet = geWorld_GetEntitySet(CCD->World(), "SkyDome");

	if(lEntitySet != NULL)
	{
		lEntity = geEntity_EntitySetGetNextEntity(lEntitySet, NULL);
		if(lEntity)
		{
			SkyDome *pSource = (SkyDome*)geEntity_GetUserData(lEntity);

			if(!pSource->Enable)
				return false;

			if(!EffectC_IsStringNull(pSource->ScriptName))
			{
				char script[128] = "scripts\\";
				strcat(script, pSource->ScriptName);
				try
				{
					Object = new TerrainObject(script);
				}
				catch(skParseException e)
				{
					char szBug[256];
					sprintf(szBug, "Parse Script Error for SkyDome script");
					CCD->ReportError(szBug, false);
					strcpy(szBug, e.toString());
					CCD->ReportError(szBug, false);
					return false;
				}
				catch(skBoundsException e)
				{
					char szBug[256];
					sprintf(szBug, "Bounds Script Error for SkyDome script");
					CCD->ReportError(szBug, false);
					strcpy(szBug, e.toString());
					CCD->ReportError(szBug, false);
					return false;
				}
				catch(skRuntimeException e)
				{
					char szBug[256];
					sprintf(szBug, "Runtime Script Error for SkyDome script");
					CCD->ReportError(szBug, false);
					strcpy(szBug, e.toString());
					CCD->ReportError(szBug, false);
					return false;
				}
				catch(skTreeNodeReaderException e)
				{
					char szBug[256];
					sprintf(szBug, "Reader Script Error for SkyDome script");
					CCD->ReportError(szBug, false);
					strcpy(szBug, e.toString());
					CCD->ReportError(szBug, false);
					return false;
				}
				catch (...)
				{
					char szBug[256];
					sprintf(szBug, "Script Error for SkyDome script");
					CCD->ReportError(szBug, false);
					return false;
				}
				if(!EffectC_IsStringNull(pSource->InitOrder))
				{
					skRValueArray args;
					skRValue ret;

					strcpy(Object->Order, pSource->InitOrder);
					try
					{
						Object->method(skString(Object->Order), args, ret,CCD->GetskContext());//change simkin
					}
					catch(skRuntimeException e)
					{
						char szBug[256];
						sprintf(szBug, "Runtime Script Error for SkyDome script");
						CCD->ReportError(szBug, false);
						strcpy(szBug, e.toString());
						CCD->ReportError(szBug, false);
						return false;
					}
					catch(skParseException e)
					{
						char szBug[256];
						sprintf(szBug, "Parse Script Error for SkyDome script");
						CCD->ReportError(szBug, false);
						strcpy(szBug, e.toString());
						CCD->ReportError(szBug, false);
						return false;
					}
					catch (...)
					{
						char szBug[256];
						sprintf(szBug, "Script Error for SkyDome script");
						CCD->ReportError(szBug, false);
						return false;
					}
				}
				if(!EffectC_IsStringNull(pSource->StartOrder))
					strcpy(Object->Order, pSource->StartOrder);
				else
					Object->Order[0] = '\0';
			}

			m_pqxPolyPool = new qxPolyPool;
			m_pqxVertPool = new qxVertPool;

			qxTerrainDefinition Def;
			Def.m_strBmp = m_strBmp;
			Def.m_strAlpha	= m_strBmp;

			Def.m_nFarPlane = 9999999;

			m_pSkyDome = new qxSkyDome(	Def, (int)fSkyLengthHeight, (float)SkyMaxHeight,
				fScaleXZ);

			if( !m_pSkyDome->Init() )
			{
				char szError[256];
				sprintf(szError, "Failed to initialize SkyDome");
				CCD->ReportError(szError, false);
				CCD->ShutdownLevel();
				delete CCD;
				CCD = NULL;
				MessageBox(NULL, szError, "Terrain Manager", MB_OK);
				exit(-333);
			}

			if(AllowSun)
			{
				m_pSun = new qxSun;
				if( !m_pSun->Init())
				{
					char szError[256];
					sprintf(szError, "Failed to initialize Sun");
					CCD->ReportError(szError, false);
					CCD->ShutdownLevel();
					delete CCD;
					CCD = NULL;
					MessageBox(NULL, szError, "Terrain Manager", MB_OK);
					exit(-333);
				}

				m_pMoon = new qxMoon;
				if( !m_pMoon->Init())
				{
					char szError[256];
					sprintf(szError, "Failed to initialize Moon");
					CCD->ReportError(szError, false);
					CCD->ShutdownLevel();
					delete CCD;
					CCD = NULL;
					MessageBox(NULL, szError, "Terrain Manager", MB_OK);
					exit(-333);
				}

				m_pStarField = new qxStarField;
				if( !m_pStarField->Init())
				{
					char szError[256];
					sprintf(szError, "Failed to initialize Stars");
					CCD->ReportError(szError, false);
					CCD->ShutdownLevel();
					delete CCD;
					CCD = NULL;
					MessageBox(NULL, szError, "Terrain Manager", MB_OK);
					exit(-333);
				}
			}

			if(AllowCloud)
			{
				m_pCloudMachine = new qxCloudMachine;
				if( !m_pCloudMachine->Init())
				{
					char szError[256];
					sprintf(szError, "Failed to initialize Clouds");
					CCD->ReportError(szError, false);
					CCD->ShutdownLevel();
					delete CCD;
					CCD = NULL;
					MessageBox(NULL, szError, "Terrain Manager", MB_OK);
					exit(-333);
				}
				m_pCloudMachine->DoWhiteFluffy();
			}

			if(LoadTerrainDefinitionFile(""))
			{
				LoadMap(0, 0);
			}

			TerrainTime = (float)Hour * 3600.0f;
			CalculatePlayerLatitude();
			ElapsedTime = (CCD->LastElapsedTime_F()*0.001f);
			Initialized = true;
			return true;
		}
	}
	return false;
}


void qxTerrainMgr::Shutdown()
{

	delete m_pTerrainDefinitionFile;
	delete m_pSun;
	delete m_pMoon;
	delete m_pStarField;
	delete m_pSkyDome;
	delete m_pCloudMachine;

	for(unsigned int i = 0; i < m_pMaps.size(); i++)
	{
		qxTerrainMapBase* p = m_pMaps[i];
		delete p;
	}

	m_pMaps.erase(m_pMaps.begin(), m_pMaps.end());


	delete m_pqxPolyPool;
	delete m_pqxVertPool;
}


bool qxTerrainMgr::Frame()
{
	if(!Initialized)
		return true;

	ElapsedTime += (CCD->LastElapsedTime_F()*0.001f);

	if(Object)
	{
		if(!EffectC_IsStringNull(Object->Order))
		{
			skRValueArray args;
			skRValue ret;

			try
			{
				Object->method(skString(Object->Order), args, ret,CCD->GetskContext());//change simkin
			}
			catch(skRuntimeException e)
			{
				char szBug[256];
				sprintf(szBug, "Runtime Script Error for SkyDome script");
				CCD->ReportError(szBug, false);
				strcpy(szBug, e.toString());
				CCD->ReportError(szBug, false);
				return false;
			}
			catch(skParseException e)
			{
				char szBug[256];
				sprintf(szBug, "Parse Script Error for SkyDome script");
				CCD->ReportError(szBug, false);
				strcpy(szBug, e.toString());
				CCD->ReportError(szBug, false);
				return false;
			}
			catch (...)
			{
				char szBug[256];
				sprintf(szBug, "Script Error for SkyDome script");
				CCD->ReportError(szBug, false);
				return false;
			}
		}
	}

	CalculatePlayerLatitude();
	CCD->CameraManager()->TrackMotion();
	CCD->CameraManager()->RenderView();

	TerrainTime += (CCD->LastElapsedTime_F()*0.001f)*m_TimeScale;
	if(TerrainTime>86400.0f)
	{
		Day += 1;
		switch(Month)
		{
		case 1:
			if(Day>31)
			{
				Day = 1;
				Month +=1;
			}
			break;
		case 2:
			if(Day>28)
			{
				Day = 1;
				Month +=1;
			}
			break;
		case 3:
			if(Day>31)
			{
				Day = 1;
				Month +=1;
			}break;
		case 4:
			if(Day>30)
			{
				Day = 1;
				Month +=1;
			}
			break;
		case 5:
			if(Day>31)
			{
				Day = 1;
				Month +=1;
			}
			break;
		case 6:
			if(Day>30)
			{
				Day = 1;
				Month +=1;
			}
			break;
		case 7:
			if(Day>31)
			{
				Day = 1;
				Month +=1;
			}
			break;
		case 8:
			if(Day>31)
			{
				Day = 1;
				Month +=1;
			}
			break;
		case 9:
			if(Day>30)
			{
				Day = 1;
				Month +=1;
			}
			break;
		case 10:
			if(Day>31)
			{
				Day = 1;
				Month +=1;
			}
			break;
		case 11:
			if(Day>30)
			{
				Day = 1;
				Month +=1;
			}
			break;
		case 12:
			if(Day>31)
			{
				Day = 1;
				Month = 1;
			}
			break;
		}
		TerrainTime -= 86400.0f;
	}

	//
	// Rotate the planet
	//

	SetEarthAxis();

	if( m_pSun )
		m_pSun->Frame();

	UpdateTwilightPercent();

	if( m_pSkyDome )
		m_pSkyDome->Frame();

	if( m_pCloudMachine)
		m_pCloudMachine->Frame();

	if( m_pMoon )
		m_pMoon->Frame();

	if( m_pStarField )
		m_pStarField->Frame();

	UpdateFog();


	for(unsigned int i = 0; i < m_pMaps.size(); i++)
	{
		qxTerrainMapBase* p = m_pMaps[i];
		p->Frame();
	}

	return true;
}


bool qxTerrainMgr::Render()
{
	if(!Initialized)
		return true;

	if(m_pSkyDome)
	{
		if( m_bRenderWireframe )
			m_pSkyDome->RenderWireframe();
		else
			m_pSkyDome->Render();
	}


	if( m_bRenderLandscape)
	{
		if(!m_bRenderWireframe)
		{
			for(unsigned int i = 0; i < m_pMaps.size(); i++)
			{
				qxTerrainMapBase* p = m_pMaps[i];
				p->Render();
			}
		}
		else
		{
			for(unsigned int i = 0; i < m_pMaps.size(); i++)
			{
				qxTerrainMapBase* p = m_pMaps[i];
				p->RenderWireframe();
			}
		}
	}

	return true;
}


void qxTerrainMgr::Draw()
{
	if(!Initialized)
		return;

	if( m_pCloudMachine)
		m_pCloudMachine->Draw();

	for(unsigned int i = 0; i < m_pMaps.size(); i++)
	{
		qxTerrainMapBase* p = m_pMaps[i];
		p->Draw();
	}

	if(m_pSun)
		m_pSun->Draw();

	if(m_pSkyDome)
		m_pSkyDome->Draw();
}


void qxTerrainMgr::SetNormalDistanceToCamera( float f )
{
	m_fNormalDistanceToCamera = f;
}


void qxTerrainMgr::PolyCountRaise()
{
	m_nDesiredTriangles += m_nPolyIncrement;
}


void qxTerrainMgr::PolyCountLower()
{
	m_nDesiredTriangles -= m_nPolyIncrement;

	if(m_nDesiredTriangles < 0)
		m_nDesiredTriangles = m_nPolyIncrement;
}


void qxTerrainMgr::DistanceDetailLower()
{
	if(m_nDistanceDetail > 1)
		m_nDistanceDetail--;
}


void qxTerrainMgr::DistanceDetailRaise()
{
	if( m_nDistanceDetail == MAX_DISTANCE_DETAIL)
		return;

	m_nDistanceDetail++;
}


void qxTerrainMgr::ToggleRenderWireframe()
{
	m_bRenderWireframe = !m_bRenderWireframe;
}


//
// Load a new map if it's time
//


qxTerrainMapBase* qxTerrainMgr::GetEastNeighbor(qxTerrainMapBase* p)
{
	int OffsetX = p->GetMapOffsetIndexX();
	int OffsetZ = p->GetMapOffsetIndexZ();

	return GetMap(OffsetX, OffsetZ+1);
}


qxTerrainMapBase* qxTerrainMgr::GetWestNeighbor(qxTerrainMapBase* p)
{
	int OffsetX = p->GetMapOffsetIndexX();
	int OffsetZ = p->GetMapOffsetIndexZ();

	return GetMap(OffsetX, OffsetZ-1);
}


qxTerrainMapBase* qxTerrainMgr::GetNorthNeighbor(qxTerrainMapBase* p)
{
	int OffsetX = p->GetMapOffsetIndexX();
	int OffsetZ = p->GetMapOffsetIndexZ();

	return GetMap(OffsetX+1, OffsetZ);
}


qxTerrainMapBase* qxTerrainMgr::GetSouthNeighbor(qxTerrainMapBase* p)
{
	int OffsetX = p->GetMapOffsetIndexX();
	int OffsetZ = p->GetMapOffsetIndexZ();

	return GetMap(OffsetX-1, OffsetZ);
}


qxTerrainMapBase* qxTerrainMgr::GetNorthWestNeighbor(qxTerrainMapBase* p)
{
	int OffsetX = p->GetMapOffsetIndexX();
	int OffsetZ = p->GetMapOffsetIndexZ();

	return GetMap(OffsetX+1, OffsetZ-1);
}


qxTerrainMapBase* qxTerrainMgr::GetNorthEastNeighbor(qxTerrainMapBase* p)
{
	int OffsetX = p->GetMapOffsetIndexX();
	int OffsetZ = p->GetMapOffsetIndexZ();

	return GetMap(OffsetX+1, OffsetZ+1);
}


qxTerrainMapBase* qxTerrainMgr::GetSouthEastNeighbor(qxTerrainMapBase* p)
{
	int OffsetX = p->GetMapOffsetIndexX();
	int OffsetZ = p->GetMapOffsetIndexZ();

	return GetMap(OffsetX-1, OffsetZ+1);
}


qxTerrainMapBase* qxTerrainMgr::GetSouthWestNeighbor(qxTerrainMapBase* p)
{
	int OffsetX = p->GetMapOffsetIndexX();
	int OffsetZ = p->GetMapOffsetIndexZ();

	return GetMap(OffsetX-1, OffsetZ-1);
}


void qxTerrainMgr::SetEarthAxis(  )
{
	int nSeconds = (int)TerrainTime;
	float RotateY = RADIANS( .0041667f * nSeconds );

	geXForm3d_SetIdentity(&m_matEarthRotation);

	float RotateX  = m_fPlayerLatitudeRotation;

	geXForm3d_RotateY(&m_matEarthRotation, RotateY );
	geXForm3d_RotateX(&m_matEarthRotation, RotateX );
}


qxTerrainMapBase* qxTerrainMgr::GetMap( int OffsetX, int OffsetZ )
{
	if( OffsetX < 0 || OffsetZ < 0)
		return NULL;

	for(unsigned int i = 0; i < m_pMaps.size(); i++)
	{
		qxTerrainMapBase* p = m_pMaps[i];
		if(	p->GetMapOffsetIndexX() == OffsetX
			&& p->GetMapOffsetIndexZ() == OffsetZ)
			return p;
	}

	return NULL;
}


inline bool qxTerrainMgr::LoadMap( int OffsetX, int OffsetZ )
{
	// See if requested map is beyond the world boundaries
	if( OffsetX > m_nWorldBoundsX || OffsetZ > m_nWorldBoundsZ )
		return true;

	// See if it's loaded already
	for(unsigned int i = 0; i < m_pMaps.size(); i++)
	{
		qxTerrainMapBase* p = m_pMaps[i];
		if(	p->GetMapOffsetIndexX() == OffsetX
			&& p->GetMapOffsetIndexZ() == OffsetZ)
			return true;
	}

	// Go to the terrain definition file and find the coordinates
	QXASSERT( m_pTerrainDefinitionFile );

	qxTerrainDefinition TerrainDef;
	m_pTerrainDefinitionFile->m_pTerrainDef = &TerrainDef;
	m_pTerrainDefinitionFile->m_pTerrainDef->m_strHeightmap = heightmap;
	m_pTerrainDefinitionFile->m_pTerrainDef->m_strBmp = texmap;
	m_pTerrainDefinitionFile->m_pTerrainDef->MapOffsetIndexX = OffsetX;
	m_pTerrainDefinitionFile->m_pTerrainDef->MapOffsetIndexZ = OffsetZ;
	m_pTerrainDefinitionFile->m_pTerrainDef->m_nFarPlane = GetFarPlane();
	m_pTerrainDefinitionFile->m_pTerrainDef->m_eType = TT_LAND; // water mod
	m_pTerrainDefinitionFile->m_pTerrainDef->m_eHeightFieldSourceType = HEIGHTFIELD_GREY_BMP;
	m_pTerrainDefinitionFile->m_pTerrainDef->m_nFarPlane = CCD->TerrainMgr()->GetFarPlane();

	qxTerrainMapBase* pMap = 0;

	switch( TerrainDef.m_eType )
	{
	case TT_LAND:
		pMap	= (qxTerrainMapBase*) new qxTerrainMap(	TerrainDef );
		break;

	default:
		QXASSERT(0);
		return false;
	}

	QXASSERT( pMap );

	if(!pMap->Init())
		return false;

	pMap->SetSunLight(true);

	m_pMaps.push_back(pMap);

	return true;
}


bool qxTerrainMgr::LoadTerrainDefinitionFile(char* pStr)
{
	QXASSERT(pStr);

	delete m_pTerrainDefinitionFile;
	m_pTerrainDefinitionFile = new qxTerrainDefinitionFile(pStr);
	if( !m_pTerrainDefinitionFile )
		return false;
	return true;
}


void qxTerrainMgr::UpdateFog()
{
	qxSkyDome* pSky = GetSkyDome();

	if( pSky && skyfog)
	{
		float fFogEnd = CCD->Engine()->GetFogEnd();
		float fFogStart = CCD->Engine()->GetFogStart();

		qxColor	m_CurrentSkyColor = *pSky->GetCurrentSkyColor();

		float v = m_CurrentSkyColor.GetV();
		v *= 0.7f;
		m_CurrentSkyColor.SetV(v);

		float s = m_CurrentSkyColor.GetS();
		float delta = 1.0f-s;
		s += delta * (1.0f-0.5f);
		m_CurrentSkyColor.SetS(s);

		CCD->Engine()->SetFogParameters(m_CurrentSkyColor.rgba, fFogStart, fFogEnd);
		CCD->Player()->ShowFog();
	}
}


float qxTerrainMgr::GetSunPercentToZenith()
{
	if(m_pSun)
		return m_pSun->GetPercentToZenith();
	else
		return 1.0f;
}


void qxTerrainMgr::UpdateTwilightPercent()
{
	// -1.0 midnight, 1.0 noon
	float fSunPercentToZenith = GetSunPercentToZenith();

	// distance above and below horizon
	float fTwilightDist = GetTwilightDistanceFromHorizon();

	// Twilight distance is 0.0 to 1.0
	m_fTwilightPercent = (fSunPercentToZenith+fTwilightDist)/ (fTwilightDist*2.0f) ;
	m_fTwilightPercent = GE_CLAMP(m_fTwilightPercent, 0.0f, 1.0f);
}

//
// Player occupies a position on earth.
// We need to calculate a rotation factor for the
// terrain manager in radians from equator.
// This only needs to be done once a day in game time.
//
void qxTerrainMgr::CalculatePlayerLatitude()
{
	// Temp. We need to know where the player is on the planet.
	// 0.0 = equator
	// the Tropic of Cancer (23 1/2°N.)		= 23.5
	// the Tropic of Capricorn (23 1/2°S.)	= -23.5
	// the Arctic Circle (66 1/2°N.),		= 66.5
	// and the Antarctic Circle (66 1/2°S.)	= -66.5

	int nDay;
	switch(Month)
	{
	case 1:
		nDay = 0;
		break;
	case 2:
		nDay = 31;
		break;
	case 3:
		nDay = 59;
		break;
	case 4:
		nDay = 90;
		break;
	case 5:
		nDay = 120;
		break;
	case 6:
		nDay = 151;
		break;
	case 7:
		nDay = 181;
		break;
	case 8:
		nDay = 212;
		break;
	case 9:
		nDay = 243;
		break;
	case 10:
		nDay = 273;
		break;
	case 11:
		nDay = 304;
		break;
	case 12:
		nDay = 334;
		break;
	}
	nDay += Day;
	float fDayRads = RADIANS( nDay );
	float fDayFactor = (float) cos( fDayRads  );


	float f = fDayFactor*23.5f;
	float f2 = fPlayerLatitude + f + 90.0f; // Shift 90 since matrix assumes we are at north pole
	m_fPlayerLatitudeRotation = RADIANS( f2 );
}


void qxTerrainMgr::ToggleSkyFollow()
{
	if(m_pSkyDome)
	{
		m_pSkyDome->ToggleMoveWithCamera();
	}
}
