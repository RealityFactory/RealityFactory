/************************************************************************************//**
 * @file CCharacter.h
 * @brief Character class
 *
 * This file contains the declaration of the Character class.
 * @author Daniel Queteschiner
 * @date October 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CHARACTER_H_
#define __RGF_CHARACTER_H_


class CCharacter
{
public:
	CCharacter(const std::string& name, const std::string& startLevel);
	~CCharacter();

	inline const std::string& GetName() const				{ return m_Name;						}
	inline const std::string& GetImage() const				{ return m_Image;						}
	inline const std::string& GetIcon() const				{ return m_Icon;						}

	inline const std::string& GetActorFilename() const		{ return m_ActorFilename;				}
	inline float GetActorScale() const						{ return m_ActorScale;					}
	inline geVec3d GetRotation() const						{ return m_Rotation;					}
	inline float GetAnimationSpeed() const					{ return m_AnimationSpeed;				}

	inline GE_RGBA GetFillColor() const						{ return m_FillColor;					}
	inline GE_RGBA GetAmbientColor() const					{ return m_AmbientColor;				}
	inline geBoolean GetAmbientLightFromFloor() const		{ return m_AmbientLightFromFloor;		}

	inline float GetShadowSize() const						{ return m_ShadowSize;					}
	inline float GetShadowAlpha() const						{ return m_ShadowAlpha;					}
	inline const std::string& GetShadowBitmap() const		{ return m_ShadowBitmap;				}
	inline const std::string& GetShadowAlphamap() const		{ return m_ShadowAlphamap;				}
	inline geBoolean GetUseProjectedShadows() const			{ return m_UseProjectedShadows;			}
	inline geBoolean GetUseStencilShadows() const			{ return m_UseStencilShadows;			}

	inline const std::string& GetStartLevel() const			{ return m_StartLevel;					}
	inline const std::string& GetPlayerStart() const		{ return m_PlayerStart;					}

	inline float GetSpeed() const							{ return m_Speed;						}
	inline float GetJumpSpeed() const						{ return m_JumpSpeed;					}
	inline float GetSlopeSpeed() const						{ return m_SlopeSpeed;					}
	inline float GetSlopeSlide() const						{ return m_SlopeSlide;					}
	inline float GetStepHeight() const						{ return m_StepHeight;					}

	inline const std::string& GetPlayerConfigFilename() const		{ return m_PlayerConfigFilename;		}
	inline const std::string& GetAttributeConfigFilename() const	{ return m_AttributeConfigFilename;		}
	inline const std::string& GetWeaponConfigFilename() const		{ return m_WeaponConfigFilename;		}
	inline const std::string& GetEnvironmentConfigFilename() const	{ return m_EnvironmentConfigFilename;	}
	inline const std::string& GetHudConfigFilename() const			{ return m_HudConfigFilename;			}

	int LoadConfiguration(CIniFile& iniFile);

private:
	std::string	m_Name;
	std::string m_Image;
	std::string m_Icon;

	std::string	m_ActorFilename;
	float		m_ActorScale;
	geVec3d		m_Rotation;
	float		m_AnimationSpeed;

	GE_RGBA		m_FillColor;
	GE_RGBA		m_AmbientColor;
	geBoolean	m_AmbientLightFromFloor;

	float		m_ShadowSize;
	float		m_ShadowAlpha;
	std::string	m_ShadowBitmap;
	std::string	m_ShadowAlphamap;
	geBoolean	m_UseProjectedShadows;
	geBoolean	m_UseStencilShadows;

	std::string	m_StartLevel;
	std::string	m_PlayerStart;

	float		m_Speed;
	float		m_JumpSpeed;
	float		m_SlopeSpeed;
	float		m_SlopeSlide;
	float		m_StepHeight;

	std::string	m_PlayerConfigFilename;
	std::string	m_AttributeConfigFilename;
	std::string	m_WeaponConfigFilename;
	std::string	m_EnvironmentConfigFilename;
	std::string	m_HudConfigFilename;
};

#endif
