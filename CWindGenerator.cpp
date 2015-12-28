/************************************************************************************//**
 * @file CWindGenerator.cpp
 * @brief WindGenerator handling class
 *
 * This file contains the implementation of the CWindGenerator	class
 * @author Jan Eisenkolb
 *//*
 * Copyright (c) 2007 Jan Eisenkolb; All right reserved.
 ****************************************************************************************/

#include "RabidFramework.h"
#include "CWindGenerator.h"

geFloat geVec3d_GetMaxElement(const geVec3d *V)
{
	geFloat X = fabs(V->X);
	geFloat Y = fabs(V->Y);
	geFloat Z = fabs(V->Z);

	if(X > Y){ if(X > Z) return X; return Z; }
	if(Y > Z) return Y; return Z;
}

/* ------------------------------------------------------------------------------------ */
// Constructor
//
// Init variables
/* ------------------------------------------------------------------------------------ */
CWindGenerator::CWindGenerator()
{
	m_bEnabled		= false;
	m_bInitialized	= false;
	m_CurrentTime	= 0.0f;
	m_ThinkTime		= 0.0f;
	m_ChangeTime	= 0.0f;
	m_MaxWindSpeed	= CCD->Player()->GetWind();
	m_MinWindSpeed	= CCD->Player()->GetWind();
	m_DestWindSpeed	= CCD->Player()->GetWind();
	m_CurrentState	= 0;
	m_MinPauseTime	= 0.0f;
	m_MaxPauseTime	= 0.0f;
	m_PauseTime		= 0.0f;
	m_PauseTimer	= 0.0f;
	m_Tolerance		= 0.1f;
	geVec3d_Clear(&m_AnchorValue);
	geVec3d_Clear(&m_ChangeSpeed);

	// get EntitySet and Entity (i assume there is only one Entity in the world)
	// also check if Entity is there, if not, then abort
	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "WindGenerator");

	if(!pSet) //Entity does not exist
		return;

	geEntity *pEntity = geEntity_EntitySetGetNextEntity(pSet, NULL);

	// get EntityData
	WindGenerator *pWG = static_cast<WindGenerator*>(geEntity_GetUserData(pEntity));

	// copy the relevant data
	m_ThinkTime = pWG->ThinkTime*1000.f;
	m_ChangeTime = pWG->ChangeSpeed*1000.f;

	if(m_ChangeTime <= 0.0f)
		m_ChangeTime = 0.1f;

	m_MaxWindSpeed = pWG->MaxWindSpeed;
	m_MinWindSpeed = pWG->MinWindSpeed;
	m_MaxPauseTime = pWG->MaxPauseTime*1000.f;
	m_MinPauseTime = pWG->MinPauseTime*1000.f;
	m_AnchorValue = pWG->AnchorValue;

	m_bInitialized = true;
	m_bEnabled = true;
}

/* ------------------------------------------------------------------------------------ */
// Destructor
/* ------------------------------------------------------------------------------------ */
CWindGenerator::~CWindGenerator()
{
}

/* ------------------------------------------------------------------------------------ */
// Tick(): Update Wind value when ThinkTime is reached
/* ------------------------------------------------------------------------------------ */
void CWindGenerator::Tick(geFloat dwTicks)
{
	if(!m_bEnabled || !m_bInitialized)
		return;

	m_CurrentTime += dwTicks;

	if(m_CurrentTime > m_ThinkTime)
	{
		geVec3d Wind = CCD->Player()->GetWind();

		switch(m_CurrentState)
		{
		case 0: // start
			//increment the current WindSpeed value
			CCD->Player()->ModifyWind(&m_ChangeSpeed);

			//Now lets look if have (almost) reached the destination wind speed
			if(geVec3d_Compare(&(CCD->Player()->GetWind()), &m_DestWindSpeed, m_Tolerance))
			{
				//Set a pause time and set the state to 1 (=pause)
				m_PauseTime = EffectC_Frand(m_MinPauseTime, m_MaxPauseTime);

				//Set a new destination wind speed, add the initial wind speed to it.
				m_DestWindSpeed.X = EffectC_Frand(m_MinWindSpeed.X, m_MaxWindSpeed.X) + CCD->Player()->GetInitialWind().X;
				m_DestWindSpeed.Y = EffectC_Frand(m_MinWindSpeed.Y, m_MaxWindSpeed.Y) + CCD->Player()->GetInitialWind().Y;
				m_DestWindSpeed.Z = EffectC_Frand(m_MinWindSpeed.Z, m_MaxWindSpeed.Z) + CCD->Player()->GetInitialWind().Z;

				//Set the Changing speed
				geVec3d_Subtract(&m_DestWindSpeed, &Wind, &m_ChangeSpeed);
				geVec3d_Scale(&m_ChangeSpeed, m_CurrentTime/m_ChangeTime, &m_ChangeSpeed);
				m_Tolerance = geVec3d_GetMaxElement(&m_ChangeSpeed);

				m_CurrentState = 1;
			}
			break;
		case 1: // pause
			// increment pause timer
			m_PauseTimer += m_CurrentTime;
			if(m_PauseTimer > m_PauseTime)
			{
				m_CurrentState = 2;
				m_PauseTimer = 0.0f;
			}
			break;
		case 2: // end
			// increment the current WindSpeed value
			CCD->Player()->ModifyWind(&m_ChangeSpeed);

			// lets look if have (almost) reached the destination wind speed
			if(geVec3d_Compare(&Wind, &m_DestWindSpeed, m_Tolerance))
			{
				//Set destination WindSpeed to AnchorValue
				geVec3d_Add(&m_AnchorValue, &CCD->Player()->GetInitialWind(), &m_DestWindSpeed);
				//Set the Changing speed
				geVec3d_Subtract(&m_DestWindSpeed, &Wind, &m_ChangeSpeed);
				geVec3d_Scale(&m_ChangeSpeed, m_CurrentTime/m_ChangeTime, &m_ChangeSpeed);
				m_Tolerance = geVec3d_GetMaxElement(&m_ChangeSpeed);

				m_CurrentState = 0;
			}
			break;
		}

		m_CurrentTime = 0;
	}
}


void CWindGenerator::SetEnabled(bool enable)
{
	if(m_bInitialized)
		m_bEnabled = enable;
}
