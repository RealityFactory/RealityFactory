/************************************************************************************//**
 * @file CWindGenerator.h
 * @brief WindGenerator handling class
 *
 * This file contains the declaration of the CWindGenerator	class
 * @author Jan Eisenkolb
 *//*
 * Copyright (c) 2007 Jan Eisenkolb; All right reserved.
 ****************************************************************************************/

#ifndef __RGF_WINDGENERATOR_H_
#define __RGF_WINDGENERATOR_H_

/**
 * @brief CWindGenerator handles wind effect and WindGenerator entity
 *
 * The WindGenerator can produce random but still realistic wind effects
 */
class CWindGenerator : public CRGFComponent
{
public:
	CWindGenerator();
	~CWindGenerator();
	void Tick(geFloat dwTicks);	///< Sets CCD->Player()->m_Wind for every frame
	void SetEnabled(bool enable);
	bool GetEnabled() { return m_bEnabled; };
private:
	bool	m_bEnabled;
	bool	m_bInitialized;
	geFloat m_CurrentTime;
	geFloat m_ThinkTime;
	geFloat m_ChangeTime;
	geFloat m_MaxPauseTime;
	geFloat m_MinPauseTime;
	geFloat m_PauseTime;
	geFloat m_PauseTimer;
	geVec3d m_MaxWindSpeed;
	geVec3d m_MinWindSpeed;
	geVec3d m_DestWindSpeed;
	geVec3d m_ChangeSpeed;
	geVec3d m_AnchorValue;
	char	m_CurrentState;
	geFloat m_Tolerance;
};

#endif
