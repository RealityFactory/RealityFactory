/************************************************************************************//**
 * @file CCameraManager.cpp
 * @brief Camera positioning, rotation, and motion manager
 *
 * This file contains the class implementation for the Camera Manager class.
 * The Camera Manager is responsible for handling the motion of the player
 * viewpoint camera during the game.
 ****************************************************************************************/

#include "RabidFramework.h"				// The One True Include File
#include "IniFile.h"
#include "CScriptManager.h"
#include "CLevel.h"
#include "CFixedCamera.h"
#include "CLiquid.h"
#include "COverlay.h"
#include "CStaticMesh.h"
#include "Simkin/skExecutable.h"
#include "RFSX/sxVector.h"
#include "RFSX/CEGUI/sxCEGUIRect.h"

#define DEFAULTFOV 2.0f

extern geBitmap *TPool_Bitmap(const char *DefaultBmp, const char *DefaultAlpha,
							  const char *BName, const char *AName);

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE CCameraManager

#ifdef SX_METHOD_ARGS
#undef SX_METHOD_ARGS
#endif
#define SX_METHOD_ARGS sxCameraMethodArgs

typedef bool (SX_IMPL_TYPE::*CameraMemFn)(float timeElapsed, skRValueArray& args);

class SX_METHOD_ARGS
{
private:
	SX_METHOD_ARGS() {}
public:
	explicit SX_METHOD_ARGS(CameraMemFn m) : method(m) {}
	SX_METHOD_ARGS(CameraMemFn m, const skRValueArray& a) : method(m), args(a) {}
	~SX_METHOD_ARGS() {}

	CameraMemFn method;
	skRValueArray args;
};


SX_IMPL_TYPE* SX_IMPL_TYPE::m_CameraManager = NULL;

/* ------------------------------------------------------------------------------------ */
// Default constructor
/* ------------------------------------------------------------------------------------ */
CCameraManager::CCameraManager()
{
	Defaults();

	memset(&m_CameraExtBox, 0, sizeof(geExtBox));
	m_CameraExtBox.Min.X = m_CameraExtBox.Min.Y = m_CameraExtBox.Min.Z = -4.0f;
	m_CameraExtBox.Max.X = m_CameraExtBox.Max.Y = m_CameraExtBox.Max.Z = 4.0f;

	// Set up our camera viewing rectangle
	m_CameraRect.Left = 0;
	m_CameraRect.Right = CCD->Engine()->Width() - 1;
	m_CameraRect.Top = 0;
	m_CameraRect.Bottom = CCD->Engine()->Height() - 1;

	// Create the Genesis3D camera
	m_EngineCamera = geCamera_Create(m_FOV, &m_CameraRect);

	if(!m_EngineCamera)
	{
		CCD->Log()->Critical("File %s - Line %d: geCamera_Create failed!", __FILE__, __LINE__);
		return;
	}

	geCamera_SetZScale(m_EngineCamera, 1.0f);
}


SX_IMPL_TYPE* SX_IMPL_TYPE::GetSingletonPtr()
{
	if(!m_CameraManager)
	{
		m_CameraManager = new SX_IMPL_TYPE();
#ifdef _DEBUG
		InitMHT();
#endif
		ScriptManager::AddGlobalVariable("Camera", m_CameraManager);
	}

	return m_CameraManager;
}

/* ------------------------------------------------------------------------------------ */
// Defaults
//
// Set default values
/* ------------------------------------------------------------------------------------ */
void CCameraManager::Defaults()
{
	m_SyncPoints = 0;

	memset(&m_Translation, 0, sizeof(geVec3d));
	memset(&m_Rotation, 0, sizeof(geVec3d));

	m_bBindToActor		= false;
	m_Actor				= NULL;
	m_szActorBone[0]	= 0;

	memset(&m_CameraOffsetTranslation, 0, sizeof(geVec3d));
	memset(&m_CameraOffsetRotation, 0, sizeof(geVec3d));

	m_bUseExtBox		= false;
	m_Model				= NULL;
	m_bBindToModel		= NULL;
	m_FarClipPlaneDistance = 0.0f;
	m_ClipEnable		= false;
	m_TrackingFlags		= kCameraTrackPosition;

	m_HeadBobOffset		= 0.0f;				// No offset
	m_HeadBob			= false;			// Default to no bobbing

	m_Zooming			= false;
	m_ZoomMode			= false;
	m_Shake				= false;
	m_ShakeX = m_ShakeY = m_ShakeMin = 0.0f;

	m_JerkAmount		= 0.0f;
	m_Jerk				= false;

	m_DesiredHeight		= 0.0f;
	m_HeightSpeed		= 40.0f;
	m_OverlayDist		= 10.0f;

	geXForm3d_SetIdentity(&m_OldXForm);
	geVec3d_Clear(&m_vMoveDif);
	m_bViewChanged = m_bPositionMoved = false;

	CIniFile AttrFile("camera.ini");
	if(!AttrFile.ReadFile())
	{
		CCD->Log()->Critical("Failed to open camera.ini file!");
		exit(-1);
	}

	std::string section = AttrFile.FindFirstKey();
	std::string value;

	while(!section.empty())
	{
		if(section == "General")
		{
			m_DefaultFOV = m_FOV = static_cast<float>(AttrFile.GetValueF(section, "fieldofview"));

			if(m_FOV <= 0.0f)
				m_DefaultFOV = m_FOV = DEFAULTFOV;

			m_LookUp = static_cast<float>(AttrFile.GetValueF(section, "lookupangle")) * GE_PIOVER180;

			if(m_LookUp == 0.0f)
				m_LookUp = 1.0f;

			m_LookDwn = static_cast<float>(AttrFile.GetValueF(section, "lookdownangle")) * GE_PIOVER180;

			if(m_LookDwn == 0.0f)
				m_LookDwn = 1.0f;

			m_SwitchAllowed = false;
			value = AttrFile.GetValue(section, "viewswitchallowed");

			if(value == "true")
				m_SwitchAllowed = true;

			m_Switch1st = false;
			value = AttrFile.GetValue(section, "switchto1stpersonallowed");

			if(value == "true")
				m_Switch1st = true;

			m_Switch3rd = false;
			value = AttrFile.GetValue(section, "switchto3rdpersonallowed");

			if(value == "true")
				m_Switch3rd = true;

			m_SwitchIso = false;
			value = AttrFile.GetValue(section, "switchtoisopersonallowed");

			if(value == "true")
				m_SwitchIso = true;

			m_HeightSpeed = static_cast<float>(AttrFile.GetValueF(section, "heightspeed"));

			if(m_HeightSpeed == 0.0f)
				m_HeightSpeed = 40.0f;

			m_OverlayDist = static_cast<float>(AttrFile.GetValueF(section, "overlaydistance"));

			if(m_OverlayDist == 0.0f)
				m_OverlayDist = 10.0f;
		}
		else if(section == "FirstPerson")
		{
			value = AttrFile.GetValue(section, "height");

			if(value == "auto" || value == "")
			{
				m_ViewHeight = -1.0f;
			}
			else
			{
				m_ViewHeight = static_cast<float>(AttrFile.GetValueF(section, "height"));
			}

			m_AllowMouse1st = true;
			value = AttrFile.GetValue(section, "disablemouse");

			if(value == "true")
				m_AllowMouse1st = false;
		}
		else if(section == "ThirdPerson")
		{
			value = AttrFile.GetValue(section, "height");

			if(value == "auto" || value == "")
			{
				m_PlayerHeight = -1.0f;
			}
			else
			{
				m_PlayerHeight = static_cast<float>(AttrFile.GetValueF(section, "height"));
			}

			m_PlayerAngleUp = static_cast<float>(AttrFile.GetValueF(section, "angleup")) * GE_PIOVER180;
			m_PlayerDistance = static_cast<float>(AttrFile.GetValueF(section, "distance"));
			m_PlayerZoom = false;
			value = AttrFile.GetValue(section, "allowzoom");

			if(value == "true")
			{
				m_PlayerZoom = true;
				m_PlayerMinDistance = static_cast<float>(AttrFile.GetValueF(section, "minimumdistance"));
				m_PlayerMaxDistance = static_cast<float>(AttrFile.GetValueF(section, "maximumdistance"));
			}

			m_PlayerMouseRotation = true;
			value = AttrFile.GetValue(section, "mouserotation");

			if(value == "false")
			{
				m_PlayerMouseRotation = false;
				m_PlayerAllowLook = false;
				value = AttrFile.GetValue(section, "allowlook");

				if(value == "true")
					m_PlayerAllowLook = true;
			}

			m_AllowMouse3rd = true;
			value = AttrFile.GetValue(section, "disablemouse");

			if(value == "true")
				m_AllowMouse3rd = false;

			m_AllowTilt3rd = true;
			value = AttrFile.GetValue(section, "disabletilt");

			if(value == "true")
				m_AllowTilt3rd = false;
		}
		else if(section == "Isometric")
		{
			value = AttrFile.GetValue(section, "height");
			if(value == "auto" || value == "")
			{
				m_IsoHeight = -1.0f;
			}
			else
			{
				m_IsoHeight = static_cast<float>(AttrFile.GetValueF(section, "height"));
			}

			m_IsoAngleUp = static_cast<float>(AttrFile.GetValueF(section, "angleup")) * GE_PIOVER180;
			m_IsoAngleAround = static_cast<float>(AttrFile.GetValueF(section, "anglearound")) * GE_PIOVER180;
			m_IsoDistance = static_cast<float>(AttrFile.GetValueF(section, "distance"));

			m_IsoCollFlag = false;
			value = AttrFile.GetValue(section, "collisiondetection");

			if(value == "true")
				m_IsoCollFlag = true;

			m_AllowMouseIso = true;
			value = AttrFile.GetValue(section, "disablemouse");

			if(value == "true")
				m_AllowMouseIso = false;
		}

		section = AttrFile.FindNextKey();
	}
}


/* ------------------------------------------------------------------------------------ */
// Default destructor
/* ------------------------------------------------------------------------------------ */
CCameraManager::~CCameraManager()
{
	ScriptManager::RemoveGlobalVariable("Camera");

	if(m_EngineCamera != NULL)
		geCamera_Destroy(&m_EngineCamera);

	m_EngineCamera = NULL;

	DequeueAll();
}


/* ------------------------------------------------------------------------------------ */
// SaveTo
//
// Save camera state to an open file
/* ------------------------------------------------------------------------------------ */
int CCameraManager::SaveTo(FILE *saveFD)
{
	fwrite(&m_Translation,				sizeof(geVec3d),	1, saveFD);
	fwrite(&m_Rotation,					sizeof(geVec3d),	1, saveFD);
	fwrite(&m_CameraOffsetTranslation,	sizeof(geVec3d),	1, saveFD);
	fwrite(&m_CameraOffsetRotation,		sizeof(geVec3d),	1, saveFD);
	fwrite(&m_DefaultDistance,			sizeof(float),		1, saveFD);
	fwrite(&m_CurrentDistance,			sizeof(float),		1, saveFD);
	fwrite(&m_CameraX,					sizeof(float),		1, saveFD);
	fwrite(&m_CameraY,					sizeof(float),		1, saveFD);
	fwrite(&m_OldRotationX,				sizeof(float),		1, saveFD);
	fwrite(&m_ShakeMin,					sizeof(float),		1, saveFD);
	fwrite(&m_DesiredHeight,			sizeof(float),		1, saveFD);
	fwrite(&m_HeightSpeed,				sizeof(float),		1, saveFD);
	fwrite(&m_OverlayDist,				sizeof(float),		1, saveFD);
	fwrite(&m_OldXForm,					sizeof(geXForm3d),	1, saveFD);
	fwrite(&m_vMoveDif,					sizeof(geVec3d),	1, saveFD);
	fwrite(&m_bViewChanged,				sizeof(bool),		1, saveFD);
	fwrite(&m_bPositionMoved,			sizeof(bool),		1, saveFD);

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// RestoreFrom
//
// Restore camera state from an open file
/* ------------------------------------------------------------------------------------ */
int CCameraManager::RestoreFrom(FILE *restoreFD)
{
	fread(&m_Translation,				sizeof(geVec3d),	1, restoreFD);
	fread(&m_Rotation,					sizeof(geVec3d),	1, restoreFD);
	fread(&m_CameraOffsetTranslation,	sizeof(geVec3d),	1, restoreFD);
	fread(&m_CameraOffsetRotation,		sizeof(geVec3d),	1, restoreFD);
	fread(&m_DefaultDistance,			sizeof(float),		1, restoreFD);
	fread(&m_CurrentDistance,			sizeof(float),		1, restoreFD);
	fread(&m_CameraX,					sizeof(float),		1, restoreFD);
	fread(&m_CameraY,					sizeof(float),		1, restoreFD);
	fread(&m_OldRotationX,				sizeof(float),		1, restoreFD);
	fread(&m_ShakeMin,					sizeof(float),		1, restoreFD);
	fread(&m_DesiredHeight,				sizeof(float),		1, restoreFD);
	fread(&m_HeightSpeed,				sizeof(float),		1, restoreFD);
	fread(&m_OverlayDist,				sizeof(float),		1, restoreFD);
	fread(&m_OldXForm,					sizeof(geXForm3d),	1, restoreFD);
	fread(&m_vMoveDif,					sizeof(geVec3d),	1, restoreFD);
	fread(&m_bViewChanged,				sizeof(bool),		1, restoreFD);
	fread(&m_bPositionMoved,			sizeof(bool),		1, restoreFD);

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// SaveToS
/* ------------------------------------------------------------------------------------ */
void CCameraManager::SaveToS()
{
	m_sTranslation				= m_Translation;
	m_sRotation					= m_Rotation;
	m_sCameraOffsetTranslation	= m_CameraOffsetTranslation;
	m_sCameraOffsetRotation		= m_CameraOffsetRotation;
	m_sDefaultDistance			= m_DefaultDistance;
	m_sCurrentDistance			= m_CurrentDistance;
	m_sCameraX					= m_CameraX;
	m_sCameraY					= m_CameraY;
	m_sOldRotationX				= m_OldRotationX;
	m_sFOV						= m_FOV;
}


/* ------------------------------------------------------------------------------------ */
// RestoreFromS
/* ------------------------------------------------------------------------------------ */
void CCameraManager::RestoreFromS()
{
	m_Translation				= m_sTranslation;
	m_Rotation					= m_sRotation;
	m_CameraOffsetTranslation	= m_sCameraOffsetTranslation;
	m_CameraOffsetRotation		= m_sCameraOffsetRotation;
	m_DefaultDistance			= m_sDefaultDistance;
	m_CurrentDistance			= m_sCurrentDistance;
	m_CameraX					= m_sCameraX;
	m_CameraY					= m_sCameraY;
	m_OldRotationX				= m_sOldRotationX;
	m_FOV						= m_sFOV;
}


/* ------------------------------------------------------------------------------------ */
// SetJerk
/* ------------------------------------------------------------------------------------ */
void CCameraManager::SetJerk(float maxAmount, float decay)
{
	if(m_Jerk)
	{
		if(m_JerkAmount > maxAmount)
			return;
	}

	m_JerkAmount = maxAmount;
	m_JerkDecay = decay;
	m_Jerk = true;
}


/* ------------------------------------------------------------------------------------ */
// CancelZoom
/* ------------------------------------------------------------------------------------ */
void CCameraManager::CancelZoom()
{
	m_Zooming = m_ZoomMode = false;
	m_FOV = m_DefaultFOV;
}


/* ------------------------------------------------------------------------------------ */
// Tick
/* ------------------------------------------------------------------------------------ */
void CCameraManager::Update(float timeElapsed)
{
	int zoomAmount = CCD->Weapons()->ZoomAmount();

	if(zoomAmount > 0)
	{
		if(m_Zooming)
		{
			if(m_ZoomMode)
			{
				if(m_FOV > m_DefaultFOV / static_cast<float>(zoomAmount))
				{
					m_FOV -= timeElapsed * 0.001f * 0.75f;
					if(m_FOV < m_DefaultFOV / static_cast<float>(zoomAmount))
						m_FOV = m_DefaultFOV / static_cast<float>(zoomAmount);
				}
			}
			else
			{
				m_FOV = m_DefaultFOV;
			}
		}
	}
	else
	{
		m_FOV = m_DefaultFOV;
	}


	if(m_Shake)
	{
		m_ShakeAmount -= timeElapsed * 0.001f * m_ShakeDecay;

		if(m_ShakeAmount <= 0.0f)
		{
			m_ShakeX = m_ShakeY = 0.0f;
			m_Shake = false;
		}
		else
		{
			m_ShakeX = EffectC_Frand(-m_ShakeAmount, m_ShakeAmount);
			m_ShakeY = EffectC_Frand(-m_ShakeAmount, m_ShakeAmount);
		}
	}


	if(m_Jerk)
	{
		m_JerkAmount -= timeElapsed * 0.001f * m_JerkDecay;

		if(m_JerkAmount <= 0.0f)
		{
			m_JerkAmount = 0.0f;
			m_Jerk = false;
		}
	}


	if(m_DesiredHeight < m_CameraOffsetTranslation.Y)
	{
		m_CameraOffsetTranslation.Y -= timeElapsed * 0.001f * m_HeightSpeed;

		if(m_DesiredHeight > m_CameraOffsetTranslation.Y)
			m_DesiredHeight = m_CameraOffsetTranslation.Y;
	}
	else if(m_DesiredHeight > m_CameraOffsetTranslation.Y)
	{
		m_CameraOffsetTranslation.Y += timeElapsed * 0.001f * m_HeightSpeed;

		if(m_DesiredHeight < m_CameraOffsetTranslation.Y)
			m_DesiredHeight = m_CameraOffsetTranslation.Y;
	}


	while(!m_MethodQueue.empty())
	{
		if((this->*(m_MethodQueue.front()->method))(timeElapsed, m_MethodQueue.front()->args))
		{
			delete m_MethodQueue.front();
			m_MethodQueue.pop();
		}
		else
		{
			break;
		}
	}
}


/* ------------------------------------------------------------------------------------ */
// GetShake
/* ------------------------------------------------------------------------------------ */
void CCameraManager::GetShake(float *x, float *y) const
{
	if(x) *x = m_ShakeX;
	if(y) *y = m_ShakeY;
}


/* ------------------------------------------------------------------------------------ */
// SetZoom
/* ------------------------------------------------------------------------------------ */
void CCameraManager::SetZoom(bool flag)
{
	m_Zooming = flag;

	if(flag)
		m_ZoomMode = !m_ZoomMode;
}


/* ------------------------------------------------------------------------------------ */
// SetShake
/* ------------------------------------------------------------------------------------ */
void CCameraManager::SetShake(float maxAmount, float decay, const geVec3d& pos)
{
	float dist = geVec3d_DistanceBetween(&pos, &m_Translation);

	if(dist > m_ShakeMin)
		maxAmount = maxAmount * (m_ShakeMin / dist);

	if(m_Shake)
	{
		if(m_ShakeAmount < maxAmount)
		{
			m_ShakeAmount = maxAmount;
			m_ShakeDecay = decay;
		}
	}
	else
	{
		m_ShakeAmount = maxAmount;
		m_ShakeDecay = decay;
		m_Shake = true;
	}
}


/* ------------------------------------------------------------------------------------ */
// Position
//
// Set the current camera position
/* ------------------------------------------------------------------------------------ */
int CCameraManager::SetPosition(const geVec3d& position)
{
	m_Translation = position;

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// Rotate
//
// Set the camera rotation
/* ------------------------------------------------------------------------------------ */
int CCameraManager::SetRotation(const geVec3d& rotation)
{
	m_Rotation = rotation;

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// TurnLeft
//
// Rotate the camera to look LEFT
/* ------------------------------------------------------------------------------------ */
int CCameraManager::TurnLeft(float amount)
{
	m_Rotation.Y -= amount;

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// TurnRight
//
// Rotate the camera to look RIGHT
/* ------------------------------------------------------------------------------------ */
int CCameraManager::TurnRight(float amount)
{
	m_Rotation.Y += amount;

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// TiltUp
//
// Rotate the camera to look UP
/* ------------------------------------------------------------------------------------ */
int CCameraManager::TiltUp(float amount)
{
	m_Rotation.X += amount;

	if(m_Rotation.X > m_LookUp)
		m_Rotation.X = m_LookUp;

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// TiltDown
//
// Rotate the camera to look DOWN
/* ------------------------------------------------------------------------------------ */
int CCameraManager::TiltDown(float amount)
{
	m_Rotation.X -= amount;

	if(m_Rotation.X < -m_LookDwn)
		m_Rotation.X = -m_LookDwn;

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// Center
//
// Reset the camera rotations to more or less straighten it up
/* ------------------------------------------------------------------------------------ */
int CCameraManager::Center()
{
	m_Rotation.X = 0.0f;
	m_Rotation.Y = 0.0f;

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// TiltXUp
//
// Rotate the camera to look UP
/* ------------------------------------------------------------------------------------ */
int CCameraManager::TiltXUp(float amount)
{
	if((m_TrackingFlags & kCameraTrackFree) != 0)
	{
		m_CameraOffsetRotation.X += amount;

		if(m_CameraOffsetRotation.X > m_LookUp)
			m_CameraOffsetRotation.X = m_LookUp;
	}

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// TiltXDown
//
// Rotate the camera to look DOWN
/* ------------------------------------------------------------------------------------ */
int CCameraManager::TiltXDown(float amount)
{
	if((m_TrackingFlags & kCameraTrackFree) != 0)
	{
		m_CameraOffsetRotation.X -= amount;

		if(m_CameraOffsetRotation.X < -m_LookDwn)
			m_CameraOffsetRotation.X = -m_LookDwn;
	}

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// GetTiltPercent
/* ------------------------------------------------------------------------------------ */
float CCameraManager::GetTiltPercent() const
{
	float percent = 0.5f;

	if(m_AllowTilt3rd)
	{
		if((m_TrackingFlags & kCameraTrackFree) != 0)
		{
			percent = 1.0f - ((m_CameraOffsetRotation.X + m_LookDwn) / (m_LookDwn + m_LookUp));
		}
		else if((m_TrackingFlags & kCameraTrackPosition) != 0)
		{
			percent = (m_Rotation.X + m_LookDwn) / (m_LookDwn + m_LookUp);
		}
	}

	return percent;
}


/* ------------------------------------------------------------------------------------ */
// GetTilt
/* ------------------------------------------------------------------------------------ */
float CCameraManager::GetTilt() const
{
	if((m_TrackingFlags & kCameraTrackFree) != 0)
	{
		if(m_AllowTilt3rd)
			return m_CameraOffsetRotation.X;
		else
			return 0.0f;
	}
	else if((m_TrackingFlags & kCameraTrackPosition) != 0)
	{
		return m_Rotation.X;
	}

	return 0.0f;
}


/* ------------------------------------------------------------------------------------ */
// GetPosition
//
// Return the cameras current position
/* ------------------------------------------------------------------------------------ */
int CCameraManager::GetPosition(geVec3d* position) const
{
	if(!position)
		return RGF_FAILURE;

	geXForm3d viewPoint;

	geXForm3d_SetZRotation(&viewPoint, m_Rotation.Z);
	geXForm3d_RotateX(&viewPoint, m_Rotation.X);
	geXForm3d_RotateY(&viewPoint, m_Rotation.Y);

	geXForm3d_Translate(&viewPoint, m_Translation.X, m_Translation.Y, m_Translation.Z);

	if(m_Jerk)
	{
		geVec3d direction;
		geXForm3d_GetIn(&viewPoint, &direction);
		geVec3d_AddScaled(&viewPoint.Translation, &direction, -m_JerkAmount, &viewPoint.Translation);
	}

	*position = viewPoint.Translation;

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// GetRotation
//
// Return the cameras current rotation
/* ------------------------------------------------------------------------------------ */
int CCameraManager::GetRotation(geVec3d* rotation) const
{
	if(!rotation)
		return RGF_FAILURE;

	*rotation = m_Rotation;

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// BindToActor
//
// Bind the camera to an actor in the level.  This will force the camera
// ..to track the position and rotation of the actor as long as the
// ..binding is in force.
/* ------------------------------------------------------------------------------------ */
int CCameraManager::BindToActor(geActor *actorToBindTo)
{
	m_bBindToActor = true;
	m_Actor = actorToBindTo;

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// SetCameraOffset
//
// Defines an offset to be used whenever the camera is bound to an actor
// ..or a world model.  This offset will be added to the translation of
// ..the bound-to entity to offset the camera from the thing itself.
/* ------------------------------------------------------------------------------------ */
int CCameraManager::SetCameraOffset(const geVec3d &translation,
									const geVec3d &rotation)
{
	m_CameraOffsetTranslation = translation;
	m_CameraOffsetRotation = rotation;
	m_OldRotationX = m_CameraOffsetRotation.X;

	m_DesiredHeight = translation.Y;

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// GetCameraOffset
/* ------------------------------------------------------------------------------------ */
int CCameraManager::GetCameraOffset(geVec3d* translation, geVec3d* rotation)
{
	*translation = m_CameraOffsetTranslation;
	*rotation = m_CameraOffsetRotation;
	m_OldRotationX = m_CameraOffsetRotation.X;

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// BindToActorBone
//
// Binds the camera to a specific bone on a specific actor, other than that,
// ..is just like BindToActor().  The camera offset is used here.
/* ------------------------------------------------------------------------------------ */
int CCameraManager::BindToActorBone(geActor *bindActor, const char* szBoneName)
{
	m_Actor = bindActor;
	strcpy(m_szActorBone, szBoneName);
	m_bBindToActor = true;

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// BindToWorldModel
//
// Binds the camera to track the position and rotation of the given world
// ..model.  The camera offset is used here.
/* ------------------------------------------------------------------------------------ */
int CCameraManager::BindToWorldModel(geWorld_Model* bindModel)
{
	m_bBindToModel = true;
	m_Model = bindModel;

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// SetBoundingBox
//
// Defines a bounding box for use by the camera so camera collisions can be tracked.
/* ------------------------------------------------------------------------------------ */
int CCameraManager::SetBoundingBox(const geExtBox& box)
{
	m_CameraExtBox = box;
	m_bUseExtBox = true;

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// ClearBoundingBox
//
// Removes the camera bounding box
/* ------------------------------------------------------------------------------------ */
int CCameraManager::ClearBoundingBox()
{
	m_bUseExtBox = false;

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// Unbind
//
// Removes ALL CAMERA BINDINGS.
/* ------------------------------------------------------------------------------------ */
int CCameraManager::Unbind()
{
	m_Actor = NULL;
	m_szActorBone[0] = 0;
	m_bBindToActor = false;
	m_bBindToModel = false;
	m_Model = NULL;

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// TrackMotion
//
// This routine forces the camera rotation and position to track whatever
// ..it is bound to, if anything.
/* ------------------------------------------------------------------------------------ */
int CCameraManager::TrackMotion()
{
	if(m_bBindToActor)
	{
		// Get the actors position and rotation
		if((m_TrackingFlags & kCameraTrackPosition) != 0)
		{
			if(m_szActorBone[0] != 0)
				CCD->ActorManager()->GetBonePosition(m_Actor, m_szActorBone, &m_Translation);
			else
				CCD->ActorManager()->GetPosition(m_Actor, &m_Translation);

			// Ok, add in the offset to line us up the way we want
			m_Translation.X += m_CameraOffsetTranslation.X;
			m_Translation.Y += m_CameraOffsetTranslation.Y;
			m_Translation.Z += m_CameraOffsetTranslation.Z;
		}

		if((m_TrackingFlags & kCameraTrackRotation) != 0)
		{
			geVec3d actorRotation;

			if(m_szActorBone[0] != 0)
				CCD->ActorManager()->GetBoneRotation(m_Actor, m_szActorBone, &actorRotation);
			else
				CCD->ActorManager()->GetRotation(m_Actor, &actorRotation);

			m_Rotation.Y = actorRotation.Y;
		}

		m_Rotation.X += m_CameraOffsetRotation.X;
		m_Rotation.Y += m_CameraOffsetRotation.Y;
		m_Rotation.Z += m_CameraOffsetRotation.Z;

		if((m_HeadBob) && (m_HeadBobOffset != 0.0f))
			m_Translation.Y += m_HeadBobOffset;			// Adjust for 'head bob'

		if((m_TrackingFlags & kCameraTrackThirdPerson) != 0)
			DoThirdPersonTracking();

		if((m_TrackingFlags & kCameraTrackIso) != 0)
			DoIsoTracking();
	}
	else if(m_bBindToModel)
	{
		// Get the model's position and rotation
		if((m_TrackingFlags & kCameraTrackPosition) != 0)
		{
			CCD->ModelManager()->GetPosition(m_Model, &m_Translation);
			// Ok, add in the offset to line us up the way we want
			m_Translation.X += m_CameraOffsetTranslation.X;
			m_Translation.Y += m_CameraOffsetTranslation.Y;
			m_Translation.Z += m_CameraOffsetTranslation.Z;
		}

		if((m_TrackingFlags & kCameraTrackRotation) != 0)
		{
			CCD->ModelManager()->GetRotation(m_Model, &m_Rotation);
			// Ok, add in the offset to line us up the way we want
			m_Rotation.X += m_CameraOffsetRotation.X;
			m_Rotation.Y += m_CameraOffsetRotation.Y;
			m_Rotation.Z += m_CameraOffsetRotation.Z;
		}
	}
	else
	{
		if((m_TrackingFlags & kCameraTrackFixed) != 0)
		{
			FixedCamera *fixedCamera = CCD->Level()->FixedCameras()->GetCamera();
			m_Translation.X = fixedCamera->origin.X;
			m_Translation.Y = fixedCamera->origin.Y;
			m_Translation.Z = fixedCamera->origin.Z;
			m_Rotation.X = fixedCamera->Rotation.X;
			m_Rotation.Y = fixedCamera->Rotation.Y;
			m_Rotation.Z = fixedCamera->Rotation.Z;
			m_FOV = fixedCamera->FieldofView;
		}
	}

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// RenderView
//
// Render the view from this camera into the current window.
/* ------------------------------------------------------------------------------------ */
int CCameraManager::RenderView()
{
	geXForm3d viewPoint;

	geXForm3d_SetZRotation(&viewPoint, m_Rotation.Z);
	geXForm3d_RotateX(&viewPoint, m_Rotation.X);
	geXForm3d_RotateY(&viewPoint, m_Rotation.Y);

	geXForm3d_Translate(&viewPoint,
						m_Translation.X + m_ShakeX,
						m_Translation.Y + m_ShakeY,
						m_Translation.Z);

	GE_Contents zoneContents;
	geExtBox extBox;

	extBox.Min.X = extBox.Min.Z = -1.0f;
	extBox.Min.Y = 0.0f;
	extBox.Max.Y = 1.0f;
	extBox.Max.X = extBox.Max.Z = 1.0f;

	if(CCD->Collision()->GetContentsOf(m_Translation, &extBox, &zoneContents) == RGF_SUCCESS)
	{
		GE_LVertex vertex;
		geVec3d direction, position;
		Liquid *liquid = CCD->Level()->Liquids()->IsLiquid(zoneContents.Model);

		if(liquid)
		{
			geXForm3d_GetIn(&viewPoint, &direction);
			geVec3d_AddScaled(&m_Translation, &direction, m_OverlayDist, &position);

			vertex.r = liquid->TintColor.r;
			vertex.g = liquid->TintColor.g;
			vertex.b = liquid->TintColor.b;
			vertex.a = liquid->Transparency;

			vertex.u = 0.0f;
			vertex.v = 0.0f;

			vertex.X = position.X;
			vertex.Y = position.Y;
			vertex.Z = position.Z;

			geWorld_AddPolyOnce(CCD->World(),
								&vertex,
								1,
								liquid->Texture,
								GE_TEXTURED_POINT,
								GE_RENDER_DO_NOT_OCCLUDE_SELF,
								2.0f);
		}
		else
		{
			Overlay *overlay = CCD->Level()->Overlays()->IsOverlay(zoneContents.Model);

			if(overlay)
			{
				geXForm3d_GetIn(&viewPoint, &direction);
				geVec3d_AddScaled(&m_Translation, &direction, m_OverlayDist, &position);

				vertex.r = overlay->TintColor.r;
				vertex.g = overlay->TintColor.g;
				vertex.b = overlay->TintColor.b;
				vertex.a = overlay->Transparency;

				vertex.u = 0.0f;
				vertex.v = 0.0f;

				vertex.X = position.X;
				vertex.Y = position.Y;
				vertex.Z = position.Z;

				geWorld_AddPolyOnce(CCD->World(),
									&vertex,
									1,
									overlay->Texture,
									GE_TEXTURED_POINT,
									GE_RENDER_DO_NOT_OCCLUDE_SELF,
									2.0f);
			}
		}
	}

	if(m_Jerk)
	{
		geVec3d direction;
		geXForm3d_GetIn(&viewPoint, &direction);
		geVec3d_AddScaled(&viewPoint.Translation, &direction, -m_JerkAmount, &viewPoint.Translation);
	}

	// Set up camera attributes just prior to rendering the world.

	geRect rect;
	geCamera_SetWorldSpaceXForm(m_EngineCamera, &viewPoint);
	geCamera_GetClippingRect(m_EngineCamera, &rect);
	geCamera_SetAttributes(m_EngineCamera, m_FOV, &rect);
	geVec3d_Subtract(&m_OldXForm.Translation, &viewPoint.Translation, &m_vMoveDif);
	m_bPositionMoved = !geVec3d_IsZero(&m_vMoveDif);

	if( m_bPositionMoved ||
		m_OldXForm.AX != viewPoint.AX || m_OldXForm.AY != viewPoint.AY || m_OldXForm.AZ != viewPoint.AZ ||
		m_OldXForm.BX != viewPoint.BX || m_OldXForm.BY != viewPoint.BY || m_OldXForm.BZ != viewPoint.BZ ||
		m_OldXForm.CX != viewPoint.CX || m_OldXForm.CY != viewPoint.CY || m_OldXForm.CZ != viewPoint.CZ)
	{
		m_bViewChanged = true;
		m_OldXForm = viewPoint;
	}

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// ViewPoint
//
// Return the viewpoint of the camera
/* ------------------------------------------------------------------------------------ */
geXForm3d CCameraManager::ViewPoint()
{
	geXForm3d viewPoint;

	TrackMotion();

	geXForm3d_SetZRotation(&viewPoint, m_Rotation.Z);
	geXForm3d_RotateX(&viewPoint, m_Rotation.X);
	geXForm3d_RotateY(&viewPoint, m_Rotation.Y);
	geXForm3d_Translate(&viewPoint, m_Translation.X, m_Translation.Y, m_Translation.Z);

	return viewPoint;
}


/* ------------------------------------------------------------------------------------ */
// InCollision
//
// Returns TRUE if the camera bounding box is in collision with anything.
// ..returns FALSE otherwise, or if no bounding box is set for use.
/* ------------------------------------------------------------------------------------ */
bool CCameraManager::InCollision()
{
	return false;
}


/* ------------------------------------------------------------------------------------ */
// SetFarClipPlane
//
// Set the far clip plane distance, in other words, nothing further
// ..than this from the camera will get rendered.
/* ------------------------------------------------------------------------------------ */
int CCameraManager::SetFarClipPlane(float distance)
{
	m_FarClipPlaneDistance = distance;
	geCamera_SetFarClipPlane(m_EngineCamera, m_ClipEnable, m_FarClipPlaneDistance);

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// EnableFarClipPlane
//
// Turn the far clip plane on and off.
/* ------------------------------------------------------------------------------------ */
void CCameraManager::EnableFarClipPlane(geBoolean clipOn)
{
	geCamera_SetFarClipPlane(m_EngineCamera, clipOn, m_FarClipPlaneDistance);
	m_ClipEnable = clipOn;
}


/* ------------------------------------------------------------------------------------ */
// CameraRotX
/* ------------------------------------------------------------------------------------ */
void CCameraManager::CameraRotX(bool direction)
{
	if((m_TrackingFlags & kCameraTrackFree) != 0 || (m_TrackingFlags & kCameraTrackIso) != 0)
	{
		if(direction)
		{
			m_CameraX += GE_PIOVER180 * 2.0f;

			if(m_CameraX > (89.99f * GE_PIOVER180 - m_CameraOffsetRotation.X))
				m_CameraX = 89.99f * GE_PIOVER180 - m_CameraOffsetRotation.X;
		}
		else
		{
			m_CameraX -= GE_PIOVER180 * 2.0f;

			if(m_CameraX < 0.0f)
				m_CameraX = 0.0f;
		}
	}
}


/* ------------------------------------------------------------------------------------ */
// CameraRotY
/* ------------------------------------------------------------------------------------ */
void CCameraManager::CameraRotY(bool direction, float speed)
{
	if((m_TrackingFlags & kCameraTrackFree) != 0 || (m_TrackingFlags & kCameraTrackIso) != 0)
	{
		if(direction)
		{
			m_CameraY += speed;
		}
		else
		{
			m_CameraY -= speed;
		}
	}
}


/* ------------------------------------------------------------------------------------ */
// ResetCamera
/* ------------------------------------------------------------------------------------ */
void CCameraManager::ResetCamera()
{
	m_CurrentDistance = m_DefaultDistance = m_CameraOffsetTranslation.Z;
	m_CameraX = 0.0f;
	m_CameraY = 0.0f;
	m_Rotation.X = m_OldRotationX;
}


/* ------------------------------------------------------------------------------------ */
// ChangeDistance
/* ------------------------------------------------------------------------------------ */
void CCameraManager::ChangeDistance(bool direction, float speed)
{
	if((m_TrackingFlags & kCameraTrackFree) != 0)
	{
		if(m_PlayerZoom)
		{
			if(m_DefaultDistance > m_CurrentDistance)
				m_DefaultDistance = m_CurrentDistance;

			if(direction)
			{
				m_DefaultDistance += speed;
				if(m_DefaultDistance > m_PlayerMaxDistance)
					m_DefaultDistance = m_PlayerMaxDistance;
			}
			else
			{
				m_DefaultDistance -= speed;
				if(m_DefaultDistance < m_PlayerMinDistance)
					m_DefaultDistance = m_PlayerMinDistance;
			}
		}
	}
}


// ********* PRIVATE MEMBER FUNCTIONS ************

/* ------------------------------------------------------------------------------------ */
// DoThirdPersonTracking
//
// We have to do some fancy footwork if we want to have decent third-person
// ..view handling when tracking an actor - so this does the job.
/* ------------------------------------------------------------------------------------ */
void CCameraManager::DoThirdPersonTracking()
{
	geVec3d actorPosition, actorRotation;

	CCD->ActorManager()->GetPosition(m_Actor, &actorPosition);
	actorPosition.Y += m_CameraOffsetTranslation.Y;

	CCD->ActorManager()->GetRotation(m_Actor, &actorRotation);
	geVec3d_Add(&actorRotation, &m_CameraOffsetRotation, &actorRotation);
	actorRotation.X = 0.0f;

	geXForm3d viewPoint;

	geXForm3d_SetZRotation(&viewPoint, actorRotation.Z + m_CameraOffsetRotation.Z);
	geXForm3d_RotateX(&viewPoint, actorRotation.X + m_CameraOffsetRotation.X + m_CameraX);
	geXForm3d_RotateY(&viewPoint, actorRotation.Y + m_CameraOffsetRotation.Y + m_CameraY);
	geXForm3d_Translate(&viewPoint, actorPosition.X, actorPosition.Y, actorPosition.Z);

	geVec3d direction;
	geXForm3d_GetIn(&viewPoint, &direction);

	float actorScale;
	CCD->ActorManager()->GetScale(m_Actor, &actorScale);		// Get actor scale

	float currentDistance = m_DefaultDistance * actorScale;
	geVec3d position = viewPoint.Translation;
	geVec3d front, back;

	geVec3d_AddScaled(&position, &direction, currentDistance, &back);
	geVec3d_Copy(&position, &front);

	GE_Collision collision;

	if(geWorld_Collision(CCD->World(), &m_CameraExtBox.Min, &m_CameraExtBox.Max, &front,
		&back, /*GE_VISIBLE_CONTENTS*/GE_CONTENTS_SOLID_CLIP, GE_COLLIDE_ALL, 0, NULL, NULL, &collision))
	{
		currentDistance = geVec3d_DistanceBetween(&collision.Impact, &front);

		if(currentDistance > (m_DefaultDistance * actorScale))
			currentDistance = m_DefaultDistance * actorScale;

		geVec3d_AddScaled(&position, &direction, currentDistance, &back);
	}

	geVec3d_AddScaled(&position, &direction, m_PlayerMinDistance * actorScale, &front);

	if(CCD->Level()->Meshes()->CollisionCheck(&m_CameraExtBox.Min, &m_CameraExtBox.Max, front, back, &collision))
	{
		float currentDistance2 = geVec3d_DistanceBetween(&collision.Impact, &front);

		currentDistance2 += m_PlayerMinDistance * actorScale;

		if(m_Actor == CCD->Player()->GetActor())
		{
			if(currentDistance2 < (m_PlayerMinDistance * actorScale))
				currentDistance2 = m_PlayerMinDistance * actorScale;
			if(currentDistance2 > (m_PlayerMaxDistance * actorScale))
				currentDistance2 = m_PlayerMaxDistance * actorScale;
		}
		else
		{
			if(currentDistance2 > (m_DefaultDistance * actorScale))
				currentDistance2 = m_DefaultDistance * actorScale;
		}

		if(currentDistance2 < currentDistance)
		{
			currentDistance = currentDistance2;
			geVec3d_AddScaled(&position, &direction, currentDistance, &back);
		}
	}

	m_CurrentDistance = currentDistance / actorScale;

	// Ok, here's the implementation of Ralph Deane's too-cool
	// ..third-person Actor Fading code. As the camera approaches
	// ..the player avatar, the AVATAR IS FADED OUT until finally
	// ..it's rendering is shut down. This is one Way Cool Effect.

	float alpha = 255.0f;

	if(currentDistance < (40.0f * actorScale))
	{
		geExtBox actor_ext_box;
		CCD->ActorManager()->GetBoundingBox(m_Actor, &actor_ext_box);

		alpha = 10.0f * (currentDistance - (fabs(actor_ext_box.Min.Z) + 1.0f)) / actorScale + 30.0f;

		if(alpha < (15.0f * actorScale))
			alpha = 0.0f;

		if(alpha > 255.0f)
			alpha = 255.0f;
	}

	CCD->ActorManager()->SetAlpha(m_Actor, alpha);			// Adjust actor alpha

	geVec3d orient;
	geVec3d_Subtract(&position, &back, &orient);

	// protect from Div by Zero
	if(currentDistance > 0.0f)
	{
		float x = orient.X;

		orient.X = GE_PIOVER2 - acos(orient.Y / currentDistance);
		orient.Y = atan2(x, orient.Z) + GE_PI;
		orient.Z = 0.0f;	// roll is zero - always!!?
	}

	m_Rotation = orient;									// Set camera orientation
	m_Translation = back;									// Set camera translation
}


/* ------------------------------------------------------------------------------------ */
// DoIsoTracking
//
// We have to do some fancy footwork if we want to have decent isometric
// ..view handling when tracking an actor - so this does the job.
/* ------------------------------------------------------------------------------------ */
void CCameraManager::DoIsoTracking()
{
	geVec3d actorPosition, actorRotation;

	CCD->ActorManager()->GetPosition(m_Actor, &actorPosition);
	actorPosition.Y += m_CameraOffsetTranslation.Y;

	CCD->ActorManager()->GetRotation(m_Actor, &actorRotation);
	actorRotation.X = 0.0f;

	geXForm3d viewPoint;

	geXForm3d_SetZRotation(&viewPoint, m_CameraOffsetRotation.Z);
	geXForm3d_RotateX(&viewPoint, m_CameraOffsetRotation.X + m_CameraX);
	geXForm3d_RotateY(&viewPoint, m_CameraOffsetRotation.Y + m_CameraY);
	geXForm3d_Translate(&viewPoint, actorPosition.X, actorPosition.Y, actorPosition.Z);

	geVec3d direction;
	geXForm3d_GetIn(&viewPoint, &direction);

	float actorScale;
	CCD->ActorManager()->GetScale(m_Actor, &actorScale);		// Get actor scale

	float currentDistance = m_DefaultDistance * actorScale;
	geVec3d position = viewPoint.Translation;
	geVec3d front, back;

	geVec3d_AddScaled(&position, &direction, currentDistance, &back);
	geVec3d_Copy(&position, &front);

	if(m_IsoCollFlag)
	{
		GE_Collision collision;

		if(geWorld_Collision(CCD->World(), &m_CameraExtBox.Min, &m_CameraExtBox.Max, &front,
			&back, /*GE_VISIBLE_CONTENTS*/GE_CONTENTS_SOLID_CLIP, GE_COLLIDE_ALL, 0, NULL, NULL, &collision))
		{
			currentDistance = geVec3d_DistanceBetween(&collision.Impact, &front);

			if(currentDistance > (m_DefaultDistance * actorScale))
				currentDistance = m_DefaultDistance * actorScale;

			geVec3d_AddScaled(&position, &direction, currentDistance, &back);
		}

		if(CCD->Level()->Meshes()->CollisionCheck(&m_CameraExtBox.Min, &m_CameraExtBox.Max, front, back, &collision))
		{
			float currentDistance2 = geVec3d_DistanceBetween(&collision.Impact, &front);

			if(currentDistance2 > (m_DefaultDistance * actorScale))
				currentDistance2 = m_DefaultDistance * actorScale;

			if(currentDistance2 < currentDistance)
			{
				currentDistance = currentDistance2;
				geVec3d_AddScaled(&position, &direction, currentDistance, &back);
			}
		}
	}

	m_CurrentDistance = currentDistance / actorScale;

	if(m_IsoCollFlag)
	{
		// fade out actor
		float alpha = 255.0f;

		if(currentDistance < (40.0f * actorScale))
		{
			geExtBox actor_ext_box;
			CCD->ActorManager()->GetBoundingBox(m_Actor, &actor_ext_box);

			alpha = 10.0f * (currentDistance - (fabs(actor_ext_box.Min.Z) + 1.0f)) / actorScale + 30.0f;

			if(alpha < (15.0f * actorScale))
				alpha = 0.0f;

			if(alpha > 255.0f)
				alpha = 255.0f;
		}

		CCD->ActorManager()->SetAlpha(m_Actor, alpha);			// Adjust actor alpha
	}

	geVec3d orient;
	geVec3d_Subtract(&position, &back, &orient);

	// protect from Div by Zero
	if(currentDistance > 0.0f)
	{
		float x = orient.X;
		orient.X = GE_PIOVER2 - acos(orient.Y / currentDistance);
		orient.Y = atan2(x, orient.Z) + GE_PI;
		orient.Z = 0.0f;	// roll is zero - always!!?
	}

	m_Rotation = orient;									// Set camera orientation
	m_Translation = back;									// Set camera translation
}


/* ------------------------------------------------------------------------------------ */
// Get3rdData
/* ------------------------------------------------------------------------------------ */
void CCameraManager::Get3rdData(float* distance, float* cameraX, float* cameraY)
{
	if(distance)	*distance = m_DefaultDistance;
	if(cameraX)		*cameraX = m_CameraX;
	if(cameraY)		*cameraY = m_CameraY;
}


/* ------------------------------------------------------------------------------------ */
// Set3rdData
/* ------------------------------------------------------------------------------------ */
void CCameraManager::Set3rdData(float distance, float cameraX, float cameraY)
{
	m_DefaultDistance = distance;
	m_CameraX = cameraX;
	m_CameraY = cameraY;
}


/* ------------------------------------------------------------------------------------ */
// Scripting
/* ------------------------------------------------------------------------------------ */
void SX_IMPL_TYPE::Enqueue(SX_METHOD_ARGS* ma)
{
	if(ma)
	{
		m_MethodQueue.push(ma);
	}
}


void SX_IMPL_TYPE::Dequeue()
{
	if(!m_MethodQueue.empty())
	{
		delete m_MethodQueue.front();
		m_MethodQueue.pop();
	}
}


void SX_IMPL_TYPE::DequeueAll()
{
	while(!m_MethodQueue.empty())
	{
		delete m_MethodQueue.front();
		m_MethodQueue.pop();
	}
}


void SX_IMPL_TYPE::InsertScriptSyncPoint()
{
	SX_METHOD_ARGS *ma = new SX_METHOD_ARGS(Q_P_FUNC(SyncPoint));
	Enqueue(ma);
}


/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */

Q_METHOD_IMPL(SyncPoint)
{
	if(m_SyncPoints > 0)
	{
		--m_SyncPoints;
		return true;
	}

	return false;
}


Q_METHOD_IMPL(SetFOV)
{
	SetFOV(args[0].floatValue());
	return true;
}


Q_METHOD_IMPL(SetClippingRect)
{

	RFSX::sxRect *sxrect = RFSX::IS_RECT(args[0]);
	if(!sxrect)
		return true;

	geRect rect = { static_cast<int>(sxrect->Rect()->d_left),
					static_cast<int>(sxrect->Rect()->d_right),
					static_cast<int>(sxrect->Rect()->d_top),
					static_cast<int>(sxrect->Rect()->d_bottom) };

	geCamera_SetAttributes(m_EngineCamera, m_FOV, &rect);
	return true;
}


Q_METHOD_IMPL(SetPosition)
{
	RFSX::sxVector *vec = RFSX::IS_VECTOR(args[0]);
	if(!vec)
		return true;

	SetPosition(vec->VectorConstRef());
	return true;
}


Q_METHOD_IMPL(SetFarClipPlane)
{
	SetFarClipPlane(args[0].floatValue());
	return true;
}


Q_METHOD_IMPL(EnableFarClipPlane)
{
	EnableFarClipPlane(GE_TRUE);
	return true;
}


Q_METHOD_IMPL(DisableFarClipPlane)
{
	EnableFarClipPlane(GE_FALSE);
	return true;
}


enum
{
	MOVE_FORWARD = 0,
	MOVE_BACKWARD,
	MOVE_LEFT,
	MOVE_RIGHT,
	MOVE_UP,
	MOVE_DOWN
};


Q_METHOD_IMPL(Move)
{
	// args = {dir, dist, time, timeElapsed, dir}
	if(args[0].obj() == &ScriptManager::GetContext().getInterpreter()->getNull())
	{
		geXForm3d xForm = ViewPoint();
		geVec3d dir;

		switch(args[4].intValue())
		{
		case MOVE_FORWARD:
			geXForm3d_GetIn(&xForm, &dir);
			break;
		case MOVE_BACKWARD:
			geXForm3d_GetIn(&xForm, &dir);
			geVec3d_Inverse(&dir);
			break;
		case MOVE_LEFT:
			geXForm3d_GetLeft(&xForm, &dir);
			break;
		case MOVE_RIGHT:
			geXForm3d_GetLeft(&xForm, &dir);
			geVec3d_Inverse(&dir);
			break;
		case MOVE_UP:
			geXForm3d_GetUp(&xForm, &dir);
			break;
		case MOVE_DOWN:
			geXForm3d_GetUp(&xForm, &dir);
			break;
		}
		args[0].assignObject(new RFSX::sxVector(dir), true);
	}

	RFSX::sxVector *vec = RFSX::IS_VECTOR(args[0]);
	if(!vec)
		return true;

	if(args[2].floatValue() > 0.f)
	{
		geVec3d pov;
		GetPosition(&pov);
		geVec3d_AddScaled(&pov, vec->Vector(), timeElapsed * (args[1].floatValue() / args[2].floatValue()), &pov);
		SetPosition(pov);

		args[3] = args[3].floatValue() + timeElapsed;

		if(args[3].floatValue() < args[2].floatValue())
			return false;
	}
	else
	{
		geVec3d pov;
		GetPosition(&pov);
		geVec3d_AddScaled(&pov, vec->Vector(), args[1].floatValue(), &pov);
		SetPosition(pov);
	}

	return true;
}


Q_METHOD_IMPL(Rotate)
{
	return true;
}


Q_METHOD_IMPL(LookAt)
{
	RFSX::sxVector *vec = RFSX::IS_VECTOR(args[0]);
	if(!vec)
		return true;

	geVec3d pov;
	GetPosition(&pov);
	geVec3d lookRotation;

	geVec3d_Subtract(vec->Vector(), &pov, &lookRotation);

	if(geVec3d_Normalize(&lookRotation) > 0.f)
	{
		float x = lookRotation.X;

		lookRotation.X = GE_PIOVER2 - acos(lookRotation.Y);
		lookRotation.Y = atan2(x, lookRotation.Z) + GE_PI;
		lookRotation.Z = 0.0f;	// roll is zero - always!!?

		SetRotation(lookRotation);
	}
	return true;
}


/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */

bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return skExecutable::method(m_name, args, r_val, ctxt);
}


SX_METHOD_IMPL(SetFOV)
{
	if(args.entries() != 1)
		return false;

	SX_METHOD_ARGS *ma = new SX_METHOD_ARGS(Q_P_FUNC(SetFOV), args);

	caller->Enqueue(ma);
	return true;
}


SX_METHOD_IMPL(GetFOV)
{
	r_val = caller->GetFOV();
	return true;
}


SX_METHOD_IMPL(SetClippingRect)
{
	if(args.entries() != 1)
		return false;

	SX_METHOD_ARGS *ma = new SX_METHOD_ARGS(Q_P_FUNC(SetClippingRect), args);

	caller->Enqueue(ma);
	return true;
}


SX_METHOD_IMPL(GetClippingRect)
{
	geRect rect;
	geCamera_GetClippingRect(caller->Camera(), &rect);

	r_val.assignObject(new RFSX::sxRect(static_cast<float>(rect.Left),
										static_cast<float>(rect.Top),
										static_cast<float>(rect.Right),
										static_cast<float>(rect.Bottom)),
						true);
	return true;
}


SX_METHOD_IMPL(SetPosition)
{
	if(args.entries() != 1)
		return false;

	SX_METHOD_ARGS *ma = new SX_METHOD_ARGS(Q_P_FUNC(SetPosition), args);

	caller->Enqueue(ma);
	return true;
}


SX_METHOD_IMPL(GetPosition)
{
	geVec3d pov;
	caller->GetPosition(&pov);

	r_val.assignObject(new RFSX::sxVector(pov), true);
	return true;
}


SX_METHOD_IMPL(SetFarClipPlane)
{
	if(args.entries() != 1)
		return false;

	SX_METHOD_ARGS *ma = new SX_METHOD_ARGS(Q_P_FUNC(SetFarClipPlane), args);

	caller->Enqueue(ma);
	return true;
}


SX_METHOD_IMPL(GetFarClipPlane)
{
	r_val = caller->GetFarClipPlane();
	return true;
}


SX_METHOD_IMPL(EnableFarClipPlane)
{
	SX_METHOD_ARGS *ma = new SX_METHOD_ARGS(Q_P_FUNC(EnableFarClipPlane));
	caller->Enqueue(ma);
	return true;
}


SX_METHOD_IMPL(DisableFarClipPlane)
{
	SX_METHOD_ARGS *ma = new SX_METHOD_ARGS(Q_P_FUNC(DisableFarClipPlane));
	caller->Enqueue(ma);
	return true;
}


SX_METHOD_IMPL(Move)
{
	if(args.entries() > 2 ||  args.entries() < 1)
		return false;

	RFSX::sxVector *vec = RFSX::IS_VECTOR(args[0]);
	if(!vec)
		return false;

	geVec3d dir;
	geVec3d_Copy(vec->Vector(), &dir);

	float dist = geVec3d_Normalize(&dir);

	float time = 0.f;
	if(args.entries() == 2)
	{
		time = args[1].floatValue() * 1000.f; // seconds to milliseconds
	}

	SX_METHOD_ARGS *ma = new SX_METHOD_ARGS(Q_P_FUNC(Move), args);
	ma->args.append(skRValue(new RFSX::sxVector(dir), true));
	ma->args.append(dist);
	ma->args.append(time);
	ma->args.append(0.f); // elapsed time

	caller->Enqueue(ma);
	return true;
}


SX_METHOD_IMPL(MoveForward)
{
	// void MoveForward(float distance, float time);
	if(args.entries() > 2 ||  args.entries() < 1)
		return false;

	float time = 0.f;
	if(args.entries() == 2)
	{
		time = args[1].floatValue() * 1000.f; // seconds to milliseconds
	}

	SX_METHOD_ARGS *ma = new SX_METHOD_ARGS(Q_P_FUNC(Move));
	ma->args.append(&ctxt.getInterpreter()->getNull());
	ma->args.append(args[0]);
	ma->args.append(time);
	ma->args.append(0.f); // elapsed time
	ma->args.append(MOVE_FORWARD);

	caller->Enqueue(ma);
	return true;
}


SX_METHOD_IMPL(MoveBackward)
{
	// void MoveBackward(float distance, float time);
	if(args.entries() > 2 ||  args.entries() < 1)
		return false;

	float time = 0.f;
	if(args.entries() == 2)
	{
		time = args[1].floatValue() * 1000.f; // seconds to milliseconds
	}

	SX_METHOD_ARGS *ma = new SX_METHOD_ARGS(Q_P_FUNC(Move));
	ma->args.append(&ctxt.getInterpreter()->getNull());
	ma->args.append(args[0]);
	ma->args.append(time);
	ma->args.append(0.f); // elapsed time
	ma->args.append(MOVE_BACKWARD);

	caller->Enqueue(ma);
	return true;
}


SX_METHOD_IMPL(MoveLeft)
{
	// void MoveLeft(float distance, float time);
	if(args.entries() > 2 ||  args.entries() < 1)
		return false;

	float time = 0.f;
	if(args.entries() == 2)
	{
		time = args[1].floatValue() * 1000.f; // seconds to milliseconds
	}

	SX_METHOD_ARGS *ma = new SX_METHOD_ARGS(Q_P_FUNC(Move));
	ma->args.append(&ctxt.getInterpreter()->getNull());
	ma->args.append(args[0]);
	ma->args.append(time);
	ma->args.append(0.f); // elapsed time
	ma->args.append(MOVE_LEFT);

	caller->Enqueue(ma);
	return true;
}


SX_METHOD_IMPL(MoveRight)
{
	// void MoveRight(float distance, float time);
	if(args.entries() > 2 ||  args.entries() < 1)
		return false;

	float time = 0.f;
	if(args.entries() == 2)
	{
		time = args[1].floatValue() * 1000.f; // seconds to milliseconds
	}

	SX_METHOD_ARGS *ma = new SX_METHOD_ARGS(Q_P_FUNC(Move));
	ma->args.append(&ctxt.getInterpreter()->getNull());
	ma->args.append(args[0]);
	ma->args.append(time);
	ma->args.append(0.f); // elapsed time
	ma->args.append(MOVE_RIGHT);

	caller->Enqueue(ma);
	return true;
}


SX_METHOD_IMPL(MoveUp)
{
	// void MoveUp(float distance, float time);
	if(args.entries() > 2 ||  args.entries() < 1)
		return false;

	float time = 0.f;
	if(args.entries() == 2)
	{
		time = args[1].floatValue() * 1000.f; // seconds to milliseconds
	}

	SX_METHOD_ARGS *ma = new SX_METHOD_ARGS(Q_P_FUNC(Move));
	ma->args.append(&ctxt.getInterpreter()->getNull());
	ma->args.append(args[0]);
	ma->args.append(time);
	ma->args.append(0.f); // elapsed time
	ma->args.append(MOVE_UP);

	caller->Enqueue(ma);
	return true;
}


SX_METHOD_IMPL(MoveDown)
{
	// void MoveDown(float distance, float time);
	if(args.entries() > 2 ||  args.entries() < 1)
		return false;

	float time = 0.f;
	if(args.entries() == 2)
	{
		time = args[1].floatValue() * 1000.f; // seconds to milliseconds
	}

	SX_METHOD_ARGS *ma = new SX_METHOD_ARGS(Q_P_FUNC(Move));
	ma->args.append(&ctxt.getInterpreter()->getNull());
	ma->args.append(args[0]);
	ma->args.append(time);
	ma->args.append(0.f); // elapsed time
	ma->args.append(MOVE_DOWN);

	caller->Enqueue(ma);
	return true;
}


SX_METHOD_IMPL(Rotate)
{
	// TODO
	if(args.entries() != 1)
		return false;

	return true;
}


SX_METHOD_IMPL(LookAt)
{
	if(args.entries() != 1)
		return false;

	SX_METHOD_ARGS *ma = new SX_METHOD_ARGS(Q_P_FUNC(LookAt), args);
	caller->Enqueue(ma);
	return true;
}


RFSX::SX_INIT_MHT(SXCAMERAMANAGER_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(SetFOV);
	SX_ADD_METHOD(GetFOV);
	SX_ADD_METHOD(SetClippingRect);
	SX_ADD_METHOD(GetClippingRect);
	SX_ADD_METHOD(SetPosition);
	SX_ADD_METHOD(GetPosition);
	SX_ADD_METHOD(SetFarClipPlane);
	SX_ADD_METHOD(GetFarClipPlane);
	SX_ADD_METHOD(EnableFarClipPlane);
	SX_ADD_METHOD(DisableFarClipPlane);
	SX_ADD_METHOD(Move);
	SX_ADD_METHOD(MoveForward);
	SX_ADD_METHOD(MoveBackward);
	SX_ADD_METHOD(MoveLeft);
	SX_ADD_METHOD(MoveRight);
	SX_ADD_METHOD(MoveUp);
	SX_ADD_METHOD(MoveDown);
	SX_ADD_METHOD(Rotate);
	SX_ADD_METHOD(LookAt);
}

#undef SX_IMPL_TYPE

/* ----------------------------------- END OF FILE ------------------------------------ */
