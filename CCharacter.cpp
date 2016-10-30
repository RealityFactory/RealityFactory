/************************************************************************************//**
 * @file CCharacter.cpp
 * @brief Character class
 *
 * This file contains the implementation of the Character class.
 * @author Daniel Queteschiner
 * @date October 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"
#include "IniFile.h"
#include "CGUIManager.h"
#include "CCharacter.h"


CCharacter::CCharacter(const std::string& name, const std::string& startLevel) :
	m_Name(name),
	m_ActorScale(1.0f),
	m_AnimationSpeed(1.0f),
	m_AmbientLightFromFloor(GE_TRUE),
	m_ShadowSize(0.0f),
	m_ShadowAlpha(128.0f),
	m_UseProjectedShadows(GE_FALSE),
	m_UseStencilShadows(GE_FALSE),
	m_StartLevel(startLevel),
	m_Speed(-1.0f),
	m_JumpSpeed(-1.0f),
	m_SlopeSpeed(-1.0f),
	m_SlopeSlide(-1.0f),
	m_StepHeight(-1.0f),
	m_WeaponConfigFilename("weapon.ini")
{
	geVec3d_Clear(&m_Rotation);
	m_FillColor.r = m_FillColor.g = m_FillColor.b = m_FillColor.a = 255.0f;
	m_AmbientColor.r = m_AmbientColor.g = m_AmbientColor.b = m_AmbientColor.a = 255.0f;
}

CCharacter::~CCharacter()
{
}

int CCharacter::LoadConfiguration(CIniFile& iniFile)
{
	if(m_Name.empty())
		return RGF_FAILURE;

	std::string value;

	value = iniFile.GetValue(m_Name, "actorname");
	if(value.empty())
	{
		CCD->Log()->Critical("File %s - Line %d: Missing character [%s] actor filename!",
								__FILE__, __LINE__, m_Name.c_str());
		return RGF_FAILURE;
	}
	m_ActorFilename = value;

	m_Image = iniFile.GetValue(m_Name, "image");
	if(m_Image.empty())
	{
		CCD->Log()->Critical("File %s - Line %d: Missing character [%s] image name!",
								__FILE__, __LINE__, m_Name.c_str());
		return RGF_FAILURE;
	}

	m_Icon = iniFile.GetValue(m_Name, "icon");

	m_ActorScale = static_cast<float>(iniFile.GetValueF(m_Name, "actorscale"));

	value = iniFile.GetValue(m_Name, "actorrotation");
	if(!value.empty())
	{
		m_Rotation = ToVec3d(value);
		geVec3d_Scale(&m_Rotation, GE_PIOVER180, &m_Rotation);
	}

	value = iniFile.GetValue(m_Name, "animationspeed");
	if(!value.empty())
		m_AnimationSpeed = static_cast<float>(iniFile.GetValueF(m_Name, "animationspeed"));


	value = iniFile.GetValue(m_Name, "fillcolor");
	if(!value.empty())
		m_FillColor = ToRGBA(value);

	value = iniFile.GetValue(m_Name, "ambientcolor");
	if(!value.empty())
		m_AmbientColor = ToRGBA(value);

	value = iniFile.GetValue(m_Name, "ambientlightfromfloor");
	if(value == "false")
		m_AmbientLightFromFloor = GE_FALSE;


	value = iniFile.GetValue(m_Name, "shadowsize");
	if(!value.empty())
		m_ShadowSize = static_cast<float>(iniFile.GetValueF(m_Name, "shadowsize"));

	value = iniFile.GetValue(m_Name, "shadowalpha");
	if(!value.empty())
		m_ShadowAlpha = static_cast<float>(iniFile.GetValueF(m_Name, "shadowalpha"));

	m_ShadowBitmap = iniFile.GetValue(m_Name, "shadowbitmap");
	m_ShadowAlphamap = iniFile.GetValue(m_Name, "shadowalphamap");

	value = iniFile.GetValue(m_Name, "projectedshadows");
	if(value == "true")
		m_UseProjectedShadows = GE_TRUE;

	value = iniFile.GetValue(m_Name, "stencilshadows");
	if(value == "true")
		m_UseStencilShadows = GE_TRUE;


	value = iniFile.GetValue(m_Name, "startlevel");
	if(!value.empty())
		m_StartLevel = value;

	value = iniFile.GetValue(m_Name, "playerstartname");
	if(!value.empty())
		m_PlayerStart = value;


	value = iniFile.GetValue(m_Name, "speed");
	if(!value.empty())
		m_Speed = static_cast<float>(iniFile.GetValueF(m_Name, "speed"));

	value = iniFile.GetValue(m_Name, "jumpspeed");
	if(!value.empty())
		m_JumpSpeed = static_cast<float>(iniFile.GetValueF(m_Name, "jumpspeed"));

	value = iniFile.GetValue(m_Name, "slopespeed");
	if(!value.empty())
		m_SlopeSpeed = static_cast<float>(iniFile.GetValueF(m_Name, "slopespeed"));

	value = iniFile.GetValue(m_Name, "slopeslide");
	if(!value.empty())
		m_SlopeSlide = static_cast<float>(iniFile.GetValueF(m_Name, "slopeslide"));

	value = iniFile.GetValue(m_Name, "stepheight");
	if(!value.empty())
		m_StepHeight = static_cast<float>(iniFile.GetValueF(m_Name, "stepheight"));

	m_PlayerConfigFilename = iniFile.GetValue(m_Name, "playersetupfile");

	m_AttributeConfigFilename = iniFile.GetValue(m_Name, "attributefile");

	value = iniFile.GetValue(m_Name, "weaponfile");
	if(!value.empty())
		m_WeaponConfigFilename = value;

	m_EnvironmentConfigFilename = iniFile.GetValue(m_Name, "environmentfile");

	m_HudConfigFilename = iniFile.GetValue(m_Name, "hudfile");

	return RGF_SUCCESS;
}
