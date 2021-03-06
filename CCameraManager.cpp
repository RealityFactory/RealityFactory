/************************************************************************************//**
 * @file CCameraManager.cpp
 * @brief Camera positioning, rotation, and motion manager
 *
 * This file contains the class implementation for the Camera Manager class.
 * The Camera Manager is responsible for handling the motion of the player
 * viewpoint camera during the game.
 ****************************************************************************************/

#include "RabidFramework.h"				// The One True Include File
// changed RF063
extern geBitmap *TPool_Bitmap(const char *DefaultBmp, const char *DefaultAlpha,
							  const char *BName, const char *AName);

/* ------------------------------------------------------------------------------------ */
//	Default constructor
/* ------------------------------------------------------------------------------------ */
CCameraManager::CCameraManager()
{
	Defaults();
	memset(&CameraExtBox, 0, sizeof(geExtBox));
	CameraExtBox.Min.X = CameraExtBox.Min.Y = CameraExtBox.Min.Z = -4.0f;
	CameraExtBox.Max.X = CameraExtBox.Max.Y = CameraExtBox.Max.Z = 4.0f;

	// Set up our camera viewing rectangle
	theCameraRect.Left = 0;
	theCameraRect.Right = CCD->Engine()->Width() - 1;
	theCameraRect.Top = 0;
	theCameraRect.Bottom = CCD->Engine()->Height() - 1;

	// Create the Genesis3D camera
	EngineCamera = geCamera_Create(FOV, &theCameraRect);

	if(!EngineCamera)
	{
		CCD->ReportError("[ERROR] CCameraManager.cpp: geCamera_Create failed", false);
		return;
	}

	geCamera_SetZScale(EngineCamera, 1.0f);
	return;
}

/* ------------------------------------------------------------------------------------ */
//	Defaults
//
//	Set default values
/* ------------------------------------------------------------------------------------ */
void CCameraManager::Defaults()
{
	memset(&Translation, 0, sizeof(geVec3d));
	memset(&Rotation, 0, sizeof(geVec3d));

	bBindToActor	= false;
	theActor		= NULL;
	szActorBone[0]	= 0;
	memset(&CameraOffsetTranslation, 0, sizeof(geVec3d));
	memset(&CameraOffsetRotation, 0, sizeof(geVec3d));
	bUseExtBox		= false;
	theModel		= NULL;
	bBindToModel	= NULL;
	FarClipPlaneDistance = 0.0f;
	ClipEnable		= false;
	TrackingFlags	= kCameraTrackPosition;
// eaa3 12/18/2000 head bob setup
	m_HeadBobOffset	= 0.0f;					// No offset
	m_HeadBob		= false;				// Default to no bobbing
// eaa3 12/18/2000 end
	zooming			= false;
	zoommode		= false;
	shake			= false;
	shakex = shakey = shakemin = 0.0f;
// changed RF063
	jerkamt			= 0.0f;
	jerk			= false;
// end change RF063
// changed RF064
	DesiredHeight	= 0.0f;
	HeightSpeed		= 40.0f;
	OverlayDist		= 10.0f;

	geXForm3d_SetIdentity(&m_OldXForm);
	geVec3d_Clear(&m_vMoveDif);
	m_bViewChanged = m_bPositionMoved = false;
// end change RF064

	CIniFile AttrFile("camera.ini");
	if(!AttrFile.ReadFile())
	{
		CCD->ReportError("[ERROR] Failed to open camera.ini file", false);
		exit(-1);
	}

	std::string KeyName = AttrFile.FindFirstKey();
	std::string Type;

	while(KeyName != "")
	{
		if(KeyName == "General")
		{
			FOV = (float)AttrFile.GetValueF(KeyName, "fieldofview");

			if(FOV == 0.0f)
				FOV = DEFAULTFOV;

			m_LookUp = (float)AttrFile.GetValueF(KeyName, "lookupangle") * GE_PIOVER180;

			if(m_LookUp == 0.0f)
				m_LookUp = 1.0f;

			m_LookDwn = (float)AttrFile.GetValueF(KeyName, "lookdownangle") * GE_PIOVER180;

			if(m_LookDwn == 0.0f)
				m_LookDwn = 1.0f;

			switchallowed = false;
			Type = AttrFile.GetValue(KeyName, "viewswitchallowed");

			if(Type == "true")
				switchallowed = true;

			switch1st = false;
			Type = AttrFile.GetValue(KeyName, "switchto1stpersonallowed");

			if(Type == "true")
				switch1st = true;

			switch3rd = false;
			Type = AttrFile.GetValue(KeyName, "switchto3rdpersonallowed");

			if(Type == "true")
				switch3rd = true;

			switchiso = false;
			Type = AttrFile.GetValue(KeyName, "switchtoisopersonallowed");

			if(Type == "true")
				switchiso = true;

// changed RF064
			HeightSpeed = (float)AttrFile.GetValueF(KeyName, "heightspeed");

			if(HeightSpeed == 0.0f)
				HeightSpeed = 40.0f;

			OverlayDist = (float)AttrFile.GetValueF(KeyName, "overlaydistance");

			if(OverlayDist == 0.0f)
				OverlayDist = 10.0f;
// end change RF064
		}
		else if(KeyName == "FirstPerson")
		{
			Type = AttrFile.GetValue(KeyName, "height");

			if(Type == "auto" || Type == "")
			{
				viewheight = -1.0f;
			}
			else
			{
				viewheight = (float)AttrFile.GetValueF(KeyName, "height");
			}

			allowmouse1st = true;
			Type = AttrFile.GetValue(KeyName, "disablemouse");

			if(Type == "true")
				allowmouse1st = false;
		}
		else if(KeyName == "ThirdPerson")
		{
			Type = AttrFile.GetValue(KeyName, "height");

			if(Type == "auto" || Type == "")
			{
				playerheight = -1.0f;
			}
			else
			{
				playerheight = (float)AttrFile.GetValueF(KeyName, "height");
			}

			playerangleup = (float)AttrFile.GetValueF(KeyName, "angleup") * GE_PIOVER180;
			playerdistance = (float)AttrFile.GetValueF(KeyName, "distance");
			playerzoom = false;
			Type = AttrFile.GetValue(KeyName, "allowzoom");

			if(Type == "true")
			{
				playerzoom = true;
				playermindistance = (float)AttrFile.GetValueF(KeyName, "minimumdistance");
				playermaxdistance = (float)AttrFile.GetValueF(KeyName, "maximumdistance");
			}

			playermouserotation = true;
			Type = AttrFile.GetValue(KeyName, "mouserotation");

			if(Type == "false")
			{
				playermouserotation = false;
				playerallowlook = false;
				Type = AttrFile.GetValue(KeyName, "allowlook");

				if(Type == "true")
					playerallowlook = true;
			}

			allowmouse3rd = true;
			Type = AttrFile.GetValue(KeyName, "disablemouse");

			if(Type == "true")
				allowmouse3rd = false;

			allowtilt3rd = true;
			Type = AttrFile.GetValue(KeyName, "disabletilt");

			if(Type == "true")
				allowtilt3rd = false;
		}
		else if(KeyName == "Isometric")
		{
			Type = AttrFile.GetValue(KeyName, "height");
			if(Type == "auto" || Type == "")
			{
				isoheight = -1.0f;
			}
			else
			{
				isoheight = (float)AttrFile.GetValueF(KeyName, "height");
			}

			isoangleup = (float)AttrFile.GetValueF(KeyName, "angleup") * GE_PIOVER180;
			isoanglearound = (float)AttrFile.GetValueF(KeyName, "anglearound") * GE_PIOVER180;
			isodistance = (float)AttrFile.GetValueF(KeyName, "distance");
			IsoCollFlag = false;
			Type = AttrFile.GetValue(KeyName, "collisiondetection");

			if(Type == "true")
				IsoCollFlag = true;

			allowmouseiso = true;
			Type = AttrFile.GetValue(KeyName, "disablemouse");

			if(Type == "true")
				allowmouseiso = false;
		}
		else if(KeyName == "MultipleFixed")
		{
		}
		else if(KeyName == "Moving")
		{
		}

		KeyName = AttrFile.FindNextKey();
	}
}

/* ------------------------------------------------------------------------------------ */
//	Default destructor
/* ------------------------------------------------------------------------------------ */
CCameraManager::~CCameraManager()
{
	if(EngineCamera != NULL)
		geCamera_Destroy(&EngineCamera);

	EngineCamera = NULL;

	return;
}

// changed RF063
/* ------------------------------------------------------------------------------------ */
//	SaveTo
//
//	Save camera state to an open file
/* ------------------------------------------------------------------------------------ */
int CCameraManager::SaveTo(FILE *SaveFD)
{
	fwrite(&Translation,			sizeof(geVec3d),	1, SaveFD);
	fwrite(&Rotation,				sizeof(geVec3d),	1, SaveFD);
	fwrite(&CameraOffsetTranslation,sizeof(geVec3d),	1, SaveFD);
	fwrite(&CameraOffsetRotation,	sizeof(geVec3d),	1, SaveFD);
	fwrite(&m_defaultdistance,		sizeof(geFloat),	1, SaveFD);
	fwrite(&m_currentdistance,		sizeof(geFloat),	1, SaveFD);
	fwrite(&m_cameraX,				sizeof(geFloat),	1, SaveFD);
	fwrite(&m_cameraY,				sizeof(geFloat),	1, SaveFD);
	fwrite(&m_oldrotationx,			sizeof(geFloat),	1, SaveFD);
	fwrite(&shakemin,				sizeof(float),		1, SaveFD);
	fwrite(&DesiredHeight,			sizeof(float),		1, SaveFD);
	fwrite(&HeightSpeed,			sizeof(float),		1, SaveFD);
	fwrite(&OverlayDist,			sizeof(float),		1, SaveFD);
	fwrite(&m_OldXForm,				sizeof(geXForm3d),	1, SaveFD);
	fwrite(&m_vMoveDif,				sizeof(geVec3d),	1, SaveFD);
	fwrite(&m_bViewChanged,			sizeof(bool),		1, SaveFD);
	fwrite(&m_bPositionMoved,		sizeof(bool),		1, SaveFD);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	RestoreFrom
//
//	Restore camera state from an open file
/* ------------------------------------------------------------------------------------ */
int CCameraManager::RestoreFrom(FILE *RestoreFD)
{
	fread(&Translation,				sizeof(geVec3d),	1, RestoreFD);
	fread(&Rotation,				sizeof(geVec3d),	1, RestoreFD);
	fread(&CameraOffsetTranslation, sizeof(geVec3d),	1, RestoreFD);
	fread(&CameraOffsetRotation,	sizeof(geVec3d),	1, RestoreFD);
	fread(&m_defaultdistance,		sizeof(geFloat),	1, RestoreFD);
	fread(&m_currentdistance,		sizeof(geFloat),	1, RestoreFD);
	fread(&m_cameraX,				sizeof(geFloat),	1, RestoreFD);
	fread(&m_cameraY,				sizeof(geFloat),	1, RestoreFD);
	fread(&m_oldrotationx,			sizeof(geFloat),	1, RestoreFD);
	fread(&shakemin,				sizeof(float),		1, RestoreFD);
	fread(&DesiredHeight,			sizeof(float),		1, RestoreFD);
	fread(&HeightSpeed,				sizeof(float),		1, RestoreFD);
	fread(&OverlayDist,				sizeof(float),		1, RestoreFD);
	fread(&m_OldXForm,				sizeof(geXForm3d),	1, RestoreFD);
	fread(&m_vMoveDif,				sizeof(geVec3d),	1, RestoreFD);
	fread(&m_bViewChanged,			sizeof(bool),		1, RestoreFD);
	fread(&m_bPositionMoved,		sizeof(bool),		1, RestoreFD);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	SaveToS
/* ------------------------------------------------------------------------------------ */
void CCameraManager::SaveToS()
{
	sTranslation				= Translation;
	sRotation					= Rotation;
	sCameraOffsetTranslation	= CameraOffsetTranslation;
	sCameraOffsetRotation		= CameraOffsetRotation;
	sm_defaultdistance			= m_defaultdistance;
	sm_currentdistance			= m_currentdistance;
	sm_cameraX					= m_cameraX;
	sm_cameraY					= m_cameraY;
	sm_oldrotationx				= m_oldrotationx;
	sFOV = FOV;
}

/* ------------------------------------------------------------------------------------ */
//	RestoreFromS
/* ------------------------------------------------------------------------------------ */
void CCameraManager::RestoreFromS()
{
	Translation				= sTranslation;
	Rotation				= sRotation;
	CameraOffsetTranslation = sCameraOffsetTranslation;
	CameraOffsetRotation	= sCameraOffsetRotation;
	m_defaultdistance		= sm_defaultdistance;
	m_currentdistance		= sm_currentdistance;
	m_cameraX				= sm_cameraX;
	m_cameraY				= sm_cameraY;
	m_oldrotationx			= sm_oldrotationx;
	FOV = sFOV;
}

/* ------------------------------------------------------------------------------------ */
//	SetJerk
/* ------------------------------------------------------------------------------------ */
void CCameraManager::SetJerk(float MaxAmount, geFloat Decay)
{
	if(jerk)
	{
		if(jerkamt>MaxAmount)
			return;
	}

	jerkamt = MaxAmount;
	jerkdecay = Decay;
	jerk = true;
}

/* ------------------------------------------------------------------------------------ */
//	CancelZoom
/* ------------------------------------------------------------------------------------ */
void CCameraManager::CancelZoom()
{
	zooming = zoommode = false;
	FOV = DEFAULTFOV;
}
// end change RF063

/* ------------------------------------------------------------------------------------ */
//	Tick
/* ------------------------------------------------------------------------------------ */
void CCameraManager::Tick(geFloat dwTicks)
{
	int zoomamt = CCD->Weapons()->ZoomAmount();

	if(zoomamt > 0)
	{
		if(zooming)
		{
			if(zoommode)
			{
				if(FOV > (DEFAULTFOV/(float)zoomamt))
				{
					FOV -= ((dwTicks*0.001f) * 0.75f);
					if(FOV < (DEFAULTFOV/(float)zoomamt))
						FOV = (DEFAULTFOV/(float)zoomamt);
				}
			}
			else
			{
				FOV = DEFAULTFOV;
			}
		}
	}
	else
		FOV = DEFAULTFOV;

	if(shake)
	{
		shakeamt -= ((dwTicks*0.001f) * shakedecay);

		if(shakeamt <= 0.0f)
		{
			shakex = shakey = 0.0f;
			shake = false;
		}
		else
		{
			shakex = EffectC_Frand(-shakeamt, shakeamt);
			shakey = EffectC_Frand(-shakeamt, shakeamt);
		}
	}

// changed RF063
	if(jerk)
	{
		jerkamt -= ((dwTicks*0.001f) * jerkdecay);

		if(jerkamt <= 0.0f)
		{
			jerkamt = 0.0f;
			jerk = false;
		}
	}
// end change RF063

// changed RF064
	if(DesiredHeight < CameraOffsetTranslation.Y)
	{
		CameraOffsetTranslation.Y -= ((dwTicks*0.001f) * HeightSpeed);

		if(DesiredHeight > CameraOffsetTranslation.Y)
			DesiredHeight = CameraOffsetTranslation.Y;
	}
	else if(DesiredHeight > CameraOffsetTranslation.Y)
	{
		CameraOffsetTranslation.Y += ((dwTicks*0.001f) * HeightSpeed);

		if(DesiredHeight < CameraOffsetTranslation.Y)
			DesiredHeight = CameraOffsetTranslation.Y;
	}
// end change RF064
}

/* ------------------------------------------------------------------------------------ */
//	GetShake
/* ------------------------------------------------------------------------------------ */
void CCameraManager::GetShake(float *x, float *y)
{
	*x = shakex;
	*y = shakey;
}

/* ------------------------------------------------------------------------------------ */
//	SetZoom
/* ------------------------------------------------------------------------------------ */
void CCameraManager::SetZoom(bool flag)
{
	zooming = flag;

	if(flag)
		zoommode = !zoommode;
}

/* ------------------------------------------------------------------------------------ */
//	SetShake
/* ------------------------------------------------------------------------------------ */
void CCameraManager::SetShake(float MaxAmount, geFloat Decay, geVec3d Pos)
{
// changed QD 12/15/05
	// can't be negative (sqrt!)
	// float dist = (float)fabs(geVec3d_DistanceBetween(&Pos, &Translation));
	float dist = geVec3d_DistanceBetween(&Pos, &Translation);

	if(dist > shakemin)
		MaxAmount = MaxAmount * (shakemin/dist);

	if(shake)
	{
		if(shakeamt < MaxAmount)
		{
			shakeamt = MaxAmount;
			shakedecay = Decay;
		}
	}
	else
	{
		shakeamt = MaxAmount;
		shakedecay = Decay;
		shake = true;
	}
}

/* ------------------------------------------------------------------------------------ */
//	Position
//
//	Set the current camera position
/* ------------------------------------------------------------------------------------ */
int CCameraManager::Position(const geVec3d &thePosition)
{
	Translation = thePosition;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	Rotate
//
//	Set the camera rotation
/* ------------------------------------------------------------------------------------ */
int CCameraManager::Rotate(const geVec3d &theRotation)
{
	Rotation = theRotation;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	TurnLeft
//
//	Rotate the camera to look LEFT
/* ------------------------------------------------------------------------------------ */
int CCameraManager::TurnLeft(geFloat fAmount)
{
	Rotation.Y -= fAmount;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	TurnRight
//
//	Rotate the camera to look RIGHT
/* ------------------------------------------------------------------------------------ */
int CCameraManager::TurnRight(geFloat fAmount)
{
	Rotation.Y += fAmount;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	TiltUp
//
//	Rotate the camera to look UP
/* ------------------------------------------------------------------------------------ */
int CCameraManager::TiltUp(geFloat fAmount)
{
	Rotation.X += fAmount;
// Mode
	if(Rotation.X > m_LookUp)
		Rotation.X = m_LookUp;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	TiltDown
//
//	Rotate the camera to look DOWN
/* ------------------------------------------------------------------------------------ */
int CCameraManager::TiltDown(geFloat fAmount)
{
	Rotation.X -= fAmount;
// Mode
	if(Rotation.X < -m_LookDwn)
		Rotation.X = -m_LookDwn;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	Center
//
//	Reset the camera rotations to more or less straighten it up
/* ------------------------------------------------------------------------------------ */
int CCameraManager::Center()
{
	Rotation.X = 0.0f;
	Rotation.Y = 0.0f;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	TiltXUp
//
//	Rotate the camera to look UP
/* ------------------------------------------------------------------------------------ */
int CCameraManager::TiltXUp(geFloat fAmount)
{
	if((TrackingFlags & kCameraTrackFree) != 0)
	{
		CameraOffsetRotation.X += fAmount;

		if(CameraOffsetRotation.X > m_LookUp)
			CameraOffsetRotation.X = m_LookUp;
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	TiltXDown
//
//	Rotate the camera to look DOWN
/* ------------------------------------------------------------------------------------ */
int CCameraManager::TiltXDown(geFloat fAmount)
{
	if((TrackingFlags & kCameraTrackFree) != 0)
	{
		CameraOffsetRotation.X -= fAmount;

		if(CameraOffsetRotation.X < -m_LookDwn)
			CameraOffsetRotation.X = -m_LookDwn;
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	GetTiltPercent
/* ------------------------------------------------------------------------------------ */
float CCameraManager::GetTiltPercent()
{
	float Percent = 0.5f;

	if(allowtilt3rd)
	{
		if((TrackingFlags & kCameraTrackFree) != 0)
		{
			Percent = 1.0f - ((CameraOffsetRotation.X + m_LookDwn)/(m_LookDwn + m_LookUp));
		}
		else if((TrackingFlags & kCameraTrackPosition) != 0)
		{
			Percent = ((Rotation.X + m_LookDwn)/(m_LookDwn + m_LookUp));
		}
	}

	return Percent;
}

/* ------------------------------------------------------------------------------------ */
//	GetTilt
/* ------------------------------------------------------------------------------------ */
float CCameraManager::GetTilt()
{
	if((TrackingFlags & kCameraTrackFree) != 0)
	{
		if(allowtilt3rd)
			return CameraOffsetRotation.X;
		else
			return 0.0f;
	}
	else if((TrackingFlags & kCameraTrackPosition) != 0)
		return Rotation.X;

	return 0.0f;
}

/* ------------------------------------------------------------------------------------ */
//	GetPosition
//
//	Return the cameras current position
/* ------------------------------------------------------------------------------------ */
int CCameraManager::GetPosition(geVec3d *thePosition)
{
// changed RF063
	geXForm3d theViewPoint;

// changed QD 12/15/05
	//geXForm3d_SetIdentity(&theViewPoint); // Clear the matrix
	//geXForm3d_RotateZ(&theViewPoint, Rotation.Z); // Rotate then translate
	geXForm3d_SetZRotation(&theViewPoint, Rotation.Z);
// end change
	geXForm3d_RotateX(&theViewPoint, Rotation.X);
	geXForm3d_RotateY(&theViewPoint, Rotation.Y);

	geXForm3d_Translate(&theViewPoint, Translation.X, Translation.Y,
		Translation.Z);

	if(jerk)
	{
		geVec3d Direction;
		geXForm3d_GetIn(&theViewPoint, &Direction);
		geVec3d_AddScaled(&theViewPoint.Translation, &Direction, -jerkamt, &theViewPoint.Translation);
	}

	//*thePosition = Translation;
	*thePosition = theViewPoint.Translation;
// end change RF063
	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	GetRotation
//
//	Return the cameras current rotation
/* ------------------------------------------------------------------------------------ */
int CCameraManager::GetRotation(geVec3d *theRotation)
{
	*theRotation = Rotation;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	BindToActor
//
//	Bind the camera to an actor in the level.  This will force the camera
//	..to track the position and rotation of the actor as long as the
//	..binding is in force.
/* ------------------------------------------------------------------------------------ */
int CCameraManager::BindToActor(geActor *ActorToBindTo)
{
	bBindToActor = true;
	theActor = ActorToBindTo;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	SetCameraOffset
//
//	Defines an offset to be used whenever the camera is bound to an actor
//	..or a world model.  This offset will be added to the translation of
//	..the bound-to entity to offset the camera from the thing itself.
/* ------------------------------------------------------------------------------------ */
int CCameraManager::SetCameraOffset(const geVec3d &theTranslation,
									const geVec3d &theRotation)
{
	CameraOffsetTranslation = theTranslation;
	CameraOffsetRotation = theRotation;
	m_oldrotationx = CameraOffsetRotation.X;
// changed RF064
	DesiredHeight = theTranslation.Y;
// end change RF064

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	GetCameraOffset
/* ------------------------------------------------------------------------------------ */
int CCameraManager::GetCameraOffset(geVec3d *theTranslation, geVec3d *theRotation)
{
	*theTranslation = CameraOffsetTranslation;
	*theRotation = CameraOffsetRotation;
	m_oldrotationx = CameraOffsetRotation.X;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	BindToActorBone
//
//	Binds the camera to a specific bone on a specific actor, other than that,
//	..is just like BindToActor().  The camera offset is used here.
/* ------------------------------------------------------------------------------------ */
int CCameraManager::BindToActorBone(geActor *theBindActor, const char *szBoneName)
{
	theActor = theBindActor;
	strcpy(szActorBone, szBoneName);
	bBindToActor = true;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	BindToWorldModel
//
//	Binds the camera to track the position and rotation of the given world
//	..model.  The camera offset is used here.
/* ------------------------------------------------------------------------------------ */
int CCameraManager::BindToWorldModel(geWorld_Model *theBindModel)
{
	bBindToModel = true;
	theModel = theBindModel;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	SetBoundingBox
//
//	Defines a bounding box for use by the camera so camera collisions can be tracked.
/* ------------------------------------------------------------------------------------ */
int CCameraManager::SetBoundingBox(const geExtBox &theBox)
{
	CameraExtBox = theBox;
	bUseExtBox = true;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	ClearBoundingBox
//
//	Removes the camera bounding box
/* ------------------------------------------------------------------------------------ */
int CCameraManager::ClearBoundingBox()
{
	bUseExtBox = false;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	Unbind
//
//	Removes ALL CAMERA BINDINGS.
/* ------------------------------------------------------------------------------------ */
int CCameraManager::Unbind()
{
	theActor = NULL;
	szActorBone[0] = 0;
	bBindToActor = false;
	bBindToModel = false;
	theModel = NULL;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	TrackMotion
//
//	This routine forces the camera rotation and position to track whatever
//	..it is bound to, if anything.
/* ------------------------------------------------------------------------------------ */
int CCameraManager::TrackMotion()
{
	if(bBindToActor)
	{
		// Get the actors position and rotation
		if((TrackingFlags & kCameraTrackPosition) != 0)
		{
			if(szActorBone[0] != 0)
				CCD->ActorManager()->GetBonePosition(theActor, szActorBone, &Translation);
			else
				CCD->ActorManager()->GetPosition(theActor, &Translation);

			// Ok, add in the offset to line us up the way we want
			Translation.X += CameraOffsetTranslation.X;
			Translation.Y += CameraOffsetTranslation.Y;
			Translation.Z += CameraOffsetTranslation.Z;
		}

		if((TrackingFlags & kCameraTrackRotation) != 0)
		{
// Start Nov2003DCS
			geVec3d ActorRot;

			if(szActorBone[0] != 0)
				CCD->ActorManager()->GetBoneRotation(theActor,szActorBone, &ActorRot);
			else
				CCD->ActorManager()->GetRotation(theActor, &ActorRot);

			Rotation.Y = ActorRot.Y;
// End Nov2003DCS
		}

		Rotation.X += CameraOffsetRotation.X;
		Rotation.Y += CameraOffsetRotation.Y;
		Rotation.Z += CameraOffsetRotation.Z;

// eaa3 12/18/200 Start head bob code
		if((m_HeadBob) && (m_HeadBobOffset != 0.0f))
			Translation.Y += m_HeadBobOffset;			// Adjust for 'head bob'
// eaa3 12/18/2000 End head bob code

		if((TrackingFlags & kCameraTrackThirdPerson) != 0)
			DoThirdPersonTracking();

		if((TrackingFlags & kCameraTrackIso) != 0)
			DoIsoTracking();
	}
	else if(bBindToModel)
	{
		// Get the actors position and rotation
		if((TrackingFlags & kCameraTrackPosition) != 0)
		{
			CCD->ModelManager()->GetPosition(theModel, &Translation);
			// Ok, add in the offset to line us up the way we want
			Translation.X += CameraOffsetTranslation.X;
			Translation.Y += CameraOffsetTranslation.Y;
			Translation.Z += CameraOffsetTranslation.Z;
		}

		if((TrackingFlags & kCameraTrackRotation) != 0)
		{
			CCD->ModelManager()->GetRotation(theModel, &Rotation);
			// Ok, add in the offset to line us up the way we want
			Rotation.X += CameraOffsetRotation.X;
			Rotation.Y += CameraOffsetRotation.Y;
			Rotation.Z += CameraOffsetRotation.Z;
		}
	}
	else
	{
		if((TrackingFlags & kCameraTrackFixed) != 0)
		{
			FixedCamera *pSource = CCD->FixedCameras()->GetCamera();
			Translation.X = pSource->origin.X;
			Translation.Y = pSource->origin.Y;
			Translation.Z = pSource->origin.Z;
			Rotation.X = pSource->Rotation.X;
			Rotation.Y = pSource->Rotation.Y;
			Rotation.Z = pSource->Rotation.Z;
			FOV = pSource->FieldofView;
		}
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	RenderView
//
//	Render the view from this camera into the current window.
/* ------------------------------------------------------------------------------------ */
int CCameraManager::RenderView()
{
	geXForm3d theViewPoint;

// changed QD 12/15/05
	//geXForm3d_SetIdentity(&theViewPoint); // Clear the matrix
	//geXForm3d_RotateZ(&theViewPoint, Rotation.Z); // Rotate then translate
	geXForm3d_SetZRotation(&theViewPoint, Rotation.Z);
// end change
	geXForm3d_RotateX(&theViewPoint, Rotation.X);
	geXForm3d_RotateY(&theViewPoint, Rotation.Y);

	geXForm3d_Translate(&theViewPoint, Translation.X+shakex, Translation.Y+shakey, Translation.Z);

// changed RF063
	GE_Contents ZoneContents;
	geExtBox ExtBox;
	geVec3d Direction, Pos;
	GE_LVertex	Vertex;

	ExtBox.Min.X = ExtBox.Min.Z = -1.0f;
	ExtBox.Min.Y = 0.0f;
	ExtBox.Max.Y = 1.0f;
	ExtBox.Max.X = ExtBox.Max.Z = 1.0f;

	if(CCD->Collision()->GetContentsOf(Translation, &ExtBox, &ZoneContents) == RGF_SUCCESS)
	//if(geWorld_GetContents(CCD->World(), &Translation, &ExtBox.Min,
		//&ExtBox.Max, GE_COLLIDE_MODELS, 0, NULL, NULL, &ZoneContents) == GE_TRUE)
	{
		Liquid * LQ = CCD->Liquids()->IsLiquid(ZoneContents.Model);

		if(LQ)
		{
			geXForm3d_GetIn(&theViewPoint, &Direction);
			geVec3d_AddScaled(&Translation, &Direction, OverlayDist, &Pos);

			Vertex.r = LQ->TintColor.r;
			Vertex.g = LQ->TintColor.g;
			Vertex.b = LQ->TintColor.b;
			Vertex.a = LQ->Transparency;

			Vertex.u = 0.0f;
			Vertex.v = 0.0f;

			Vertex.X = Pos.X;
			Vertex.Y = Pos.Y;
			Vertex.Z = Pos.Z;

			geWorld_AddPolyOnce(CCD->World(), &Vertex, 1,
				LQ->Texture,
				GE_TEXTURED_POINT  , GE_RENDER_DO_NOT_OCCLUDE_SELF, 2.0f );
		}
// changed RF064
		else
		{
			Overlay * OL = CCD->Overlays()->IsOverlay(ZoneContents.Model);

			if(OL)
			{
				geXForm3d_GetIn(&theViewPoint, &Direction);
				geVec3d_AddScaled(&Translation, &Direction, OverlayDist, &Pos);

				Vertex.r = OL->TintColor.r;
				Vertex.g = OL->TintColor.g;
				Vertex.b = OL->TintColor.b;
				Vertex.a = OL->Transparency;

				Vertex.u = 0.0f;
				Vertex.v = 0.0f;

				Vertex.X = Pos.X;
				Vertex.Y = Pos.Y;
				Vertex.Z = Pos.Z;

				geWorld_AddPolyOnce(CCD->World(), &Vertex, 1, OL->Texture,
					GE_TEXTURED_POINT  , GE_RENDER_DO_NOT_OCCLUDE_SELF, 2.0f);
			}
		}
// end change RF064
	}

	if(jerk)
	{
		geVec3d Direction;
		geXForm3d_GetIn(&theViewPoint, &Direction);
		geVec3d_AddScaled(&theViewPoint.Translation, &Direction, -jerkamt, &theViewPoint.Translation);
	}

// end change RF063
	//	Set up camera attributes just prior to rendering the world.

	geRect Rect;
	geCamera_SetWorldSpaceXForm(EngineCamera, &theViewPoint);
	geCamera_GetClippingRect(EngineCamera, &Rect);
	geCamera_SetAttributes(EngineCamera, FOV, &Rect);
// changed RF064
	geVec3d_Subtract(&m_OldXForm.Translation, &theViewPoint.Translation, &m_vMoveDif );
	m_bPositionMoved = !geVec3d_IsZero(&m_vMoveDif);

	if( m_bPositionMoved  ||
		m_OldXForm.AX != theViewPoint.AX || m_OldXForm.AY != theViewPoint.AY || m_OldXForm.AZ != theViewPoint.AZ ||
		m_OldXForm.BX != theViewPoint.BX || m_OldXForm.BY != theViewPoint.BY || m_OldXForm.BZ != theViewPoint.BZ ||
		m_OldXForm.CX != theViewPoint.CX || m_OldXForm.CY != theViewPoint.CY || m_OldXForm.CZ != theViewPoint.CZ)
	{
		m_bViewChanged = true;
		m_OldXForm = theViewPoint;
	}
// end change RF064

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	ViewPoint
//
//	Return the viewpoint of the camera
/* ------------------------------------------------------------------------------------ */
geXForm3d CCameraManager::ViewPoint()
{
	geXForm3d theViewPoint;

	TrackMotion();

// changed QD 10715/05
	//geXForm3d_SetIdentity(&theViewPoint);			// Clear the matrix
	//geXForm3d_RotateZ(&theViewPoint, Rotation.Z);	// Rotate then translate
	geXForm3d_SetZRotation(&theViewPoint, Rotation.Z);
// end change
	geXForm3d_RotateX(&theViewPoint, Rotation.X);
	geXForm3d_RotateY(&theViewPoint, Rotation.Y);
	geXForm3d_Translate(&theViewPoint, Translation.X, Translation.Y, Translation.Z);

	return theViewPoint;
}

/* ------------------------------------------------------------------------------------ */
//	InCollision
//
//	Returns TRUE if the camera bounding box is in collision with anything.
//	..returns FALSE otherwise, or if no bounding box is set for use.
/* ------------------------------------------------------------------------------------ */
bool CCameraManager::InCollision()
{
	return false;
}

/* ------------------------------------------------------------------------------------ */
//	SetFarClipPlane
//
//	Set the far clip plane distance, in other words, nothing further
//	..than this from the camera will get rendered.
/* ------------------------------------------------------------------------------------ */
int CCameraManager::SetFarClipPlane(geFloat fDistance)
{
	FarClipPlaneDistance = fDistance;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	EnableFarClipPlane
//
//	Turn the far clip plane on and off.
/* ------------------------------------------------------------------------------------ */
void CCameraManager::EnableFarClipPlane(geBoolean ClipOn)
{
	geCamera_SetFarClipPlane(EngineCamera, ClipOn, FarClipPlaneDistance);
	ClipEnable = ClipOn;
	return;
}

/* ------------------------------------------------------------------------------------ */
//	CameraRotX
/* ------------------------------------------------------------------------------------ */
void CCameraManager::CameraRotX(bool direction)
{
// changed RF064
	if((TrackingFlags & kCameraTrackFree) != 0 || (TrackingFlags & kCameraTrackIso) != 0)
// end change RF064
	{
		if(direction)
		{
			m_cameraX += 2.0f;

			if(m_cameraX > (89.99f-(CameraOffsetRotation.X*GE_180OVERPI)))
				m_cameraX = 89.99f-(CameraOffsetRotation.X*GE_180OVERPI);
		}
		else
		{
			m_cameraX -= 2.0f;

			if(m_cameraX < 0.0f)
				m_cameraX = 0.0f;
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	CameraRotY
/* ------------------------------------------------------------------------------------ */
void CCameraManager::CameraRotY(bool direction, float Speed)
{
// changed RF064
	if((TrackingFlags & kCameraTrackFree) != 0 || (TrackingFlags & kCameraTrackIso) != 0)
// end change RF064
	{
		if(direction)
		{
			m_cameraY += Speed;
		}
		else
		{
			m_cameraY -= Speed;
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	ResetCamera
/* ------------------------------------------------------------------------------------ */
void CCameraManager::ResetCamera()
{
	m_currentdistance = m_defaultdistance = CameraOffsetTranslation.Z;
	m_cameraX = 0.0f;
	m_cameraY = 0.0f;
	Rotation.X = m_oldrotationx;
}

/* ------------------------------------------------------------------------------------ */
//	ChangeDistance
/* ------------------------------------------------------------------------------------ */
void CCameraManager::ChangeDistance(bool direction, float Speed)
{
	if((TrackingFlags & kCameraTrackFree) != 0)
	{
		if(playerzoom)
		{
			if(m_defaultdistance>m_currentdistance)
				m_defaultdistance = m_currentdistance;

			if(direction)
			{
				m_defaultdistance+=Speed;
				if(m_defaultdistance>playermaxdistance)
					m_defaultdistance = playermaxdistance;
			}
			else
			{
				m_defaultdistance-=Speed;
				if(m_defaultdistance<playermindistance)
					m_defaultdistance = playermindistance;
			}
		}
	}
}

//	********* PRIVATE MEMBER FUNCTIONS ************

/* ------------------------------------------------------------------------------------ */
//	DoThirdPersonTracking
//
//	We have to do some fancy footwork if we want to have decent third-person
//	..view handling when tracking an actor - so this does the job.
/* ------------------------------------------------------------------------------------ */
void CCameraManager::DoThirdPersonTracking()
{
	geXForm3d theViewPoint;
	geVec3d Pos;
	GE_Collision Collision;
	geVec3d Front, Back;
	geVec3d Direction;
	geFloat CurrentDistance;
	geExtBox ActorExtBox;
	geFloat ActorScale;
	geVec3d Orient;
	geFloat x;
// changed QD 01/2004
	geFloat PlayerScale = CCD->Player()->GetScale();
// end change

	CCD->ActorManager()->GetScale(theActor, &ActorScale);		// Get actor scale
	CCD->ActorManager()->GetBoundingBox(theActor, &ActorExtBox);
	CurrentDistance = m_defaultdistance * ActorScale;

	geVec3d ActorPosition, ActorRotation;
	CCD->ActorManager()->GetPosition(theActor, &ActorPosition);
	ActorPosition.Y += CameraOffsetTranslation.Y;
	CCD->ActorManager()->GetRotation(theActor, &ActorRotation);
// Start Nov2003DCS
	geVec3d_Add(&ActorRotation, &CameraOffsetRotation, &ActorRotation);
// End Nov2003DCS
	ActorRotation.X = 0.0f;

// changed QD 12/15/05
	//geXForm3d_SetIdentity(&theViewPoint);
	//geXForm3d_RotateZ(&theViewPoint, ActorRotation.Z+CameraOffsetRotation.Z);
	geXForm3d_SetZRotation(&theViewPoint, ActorRotation.Z+CameraOffsetRotation.Z);
// end change
	geXForm3d_RotateX(&theViewPoint, ActorRotation.X+CameraOffsetRotation.X+GE_PIOVER180*(m_cameraX));
	geXForm3d_RotateY(&theViewPoint, ActorRotation.Y+CameraOffsetRotation.Y+GE_PIOVER180*(m_cameraY));
	geXForm3d_Translate(&theViewPoint, ActorPosition.X, ActorPosition.Y, ActorPosition.Z);

	Pos = theViewPoint.Translation;
	geXForm3d_GetIn(&theViewPoint, &Direction);
	geVec3d_AddScaled(&Pos, &Direction, CurrentDistance, &Back);
// changed QD 12/15/05
	// geVec3d_AddScaled(&Pos, &Direction, 0.0f, &Front);
	geVec3d_Copy(&Pos, &Front);
// end change

	if(geWorld_Collision(CCD->World(), &CameraExtBox.Min, &CameraExtBox.Max, &Front,
		&Back, /*GE_VISIBLE_CONTENTS*/GE_CONTENTS_SOLID_CLIP, GE_COLLIDE_ALL, 0, NULL, NULL, &Collision))
    {
// changed QD 01/2004
		// can't be negative (sqrt!)
		// CurrentDistance = (geFloat)fabs(geVec3d_DistanceBetween(&Collision.Impact, &Front));
		// if(CurrentDistance < 0.0f)
		//	CurrentDistance = 0.0f;
		CurrentDistance = geVec3d_DistanceBetween(&Collision.Impact, &Front);
// end change

		if(CurrentDistance > (m_defaultdistance*ActorScale))
			CurrentDistance = m_defaultdistance*ActorScale;

		geVec3d_AddScaled(&Pos, &Direction, CurrentDistance, &Back);
	}

// changed QD 01/2004
	geVec3d_AddScaled(&Pos, &Direction, playermindistance*ActorScale, &Front);

	if(CCD->Meshes()->CollisionCheck(&CameraExtBox.Min, &CameraExtBox.Max, Front, Back, &Collision))
	{
// changed QD distance can't be negative
	//	geFloat CurrentDistance2 = (geFloat)fabs(geVec3d_DistanceBetween(&Collision.Impact, &Front));//-4.0f;
		geFloat CurrentDistance2 = geVec3d_DistanceBetween(&Collision.Impact, &Front);

		CurrentDistance2 += playermindistance*ActorScale;

		if(theActor == CCD->Player()->GetActor())
		{
			if(CurrentDistance2 < (playermindistance*ActorScale))
				CurrentDistance2 = playermindistance*ActorScale;
			if(CurrentDistance2 > (playermaxdistance*ActorScale))
				CurrentDistance2 = playermaxdistance*ActorScale;
		}
		else
		{
			if(CurrentDistance2 > (m_defaultdistance*ActorScale))
				CurrentDistance2 = m_defaultdistance*ActorScale;
		}

		if(CurrentDistance2 < CurrentDistance)
		{
			CurrentDistance = CurrentDistance2;
			geVec3d_AddScaled(&Pos, &Direction, CurrentDistance2, &Back);
		}
	}
// end change

	m_currentdistance = CurrentDistance/ActorScale;

	// Ok, here's the implementation of Ralph Deane's too-cool
	// ..third-person Actor Fading code. As the camera approaches
	// ..the player avatar, the AVATAR IS FADED OUT until finally
	// ..it's rendering is shut down. This is one Way Cool Effect.
	// 03/22/2000 eaa3 Added Ralph Deane's scaling fixes.

	geFloat fAlpha = 255.0f;

	if(CurrentDistance < (40.0f*ActorScale))
	{
		fAlpha = (10.0f*((CurrentDistance-((geFloat)fabs(ActorExtBox.Min.Z)+1.0f))/ActorScale))+30.0f;

		if(fAlpha < (15.0f*ActorScale))
			fAlpha = 0.0f;

		if(fAlpha > 255.0f)
			fAlpha = 255.0f;
	}

	CCD->ActorManager()->SetAlpha(theActor, fAlpha);			// Adjust actor alpha

	geVec3d_Subtract( &Pos, &Back, &Orient );

	// protect from Div by Zero
	if(CurrentDistance > 0.0f)
	{
		x = Orient.X;
		// changed QD 12/15/05
		// Orient.X = (geFloat)( GE_PI*0.5 ) - (geFloat)acos(Orient.Y / CurrentDistance);
		Orient.X = GE_PIOVER2 - (geFloat)acos(Orient.Y / CurrentDistance);
		Orient.Y = (geFloat)atan2(x, Orient.Z) + GE_PI;
		Orient.Z = 0.0f;	// roll is zero - always!!?
	}

	Rotation = Orient;										// Set camera orientation
	Translation = Back;										// Set camera translation

	return;
}

/* ------------------------------------------------------------------------------------ */
//	DoIsoTracking
//
//	We have to do some fancy footwork if we want to have decent isometric
//	..view handling when tracking an actor - so this does the job.
/* ------------------------------------------------------------------------------------ */
void CCameraManager::DoIsoTracking()
{
	geXForm3d theViewPoint;
	geVec3d Pos;
	GE_Collision Collision;
	geVec3d Front, Back;
	geVec3d Direction;
	geFloat CurrentDistance;
	geExtBox ActorExtBox;
	geFloat ActorScale;
	geVec3d Orient;
	geFloat x;

	CCD->ActorManager()->GetScale(theActor, &ActorScale);		// Get actor scale
	CCD->ActorManager()->GetBoundingBox(theActor, &ActorExtBox);
	CurrentDistance = m_defaultdistance * ActorScale;

	geVec3d ActorPosition, ActorRotation;
	CCD->ActorManager()->GetPosition(theActor, &ActorPosition);
	ActorPosition.Y += CameraOffsetTranslation.Y;
	CCD->ActorManager()->GetRotation(theActor, &ActorRotation);
	ActorRotation.X = 0.0f;

// changed QD 12/15/05
	//geXForm3d_SetIdentity(&theViewPoint);
	//geXForm3d_RotateZ(&theViewPoint, CameraOffsetRotation.Z);
	geXForm3d_SetZRotation(&theViewPoint, CameraOffsetRotation.Z);
// end change
	geXForm3d_RotateX(&theViewPoint, CameraOffsetRotation.X+GE_PIOVER180*(m_cameraX));
	geXForm3d_RotateY(&theViewPoint, CameraOffsetRotation.Y+GE_PIOVER180*(m_cameraY));
	geXForm3d_Translate(&theViewPoint, ActorPosition.X, ActorPosition.Y, ActorPosition.Z);

	Pos = theViewPoint.Translation;
	geXForm3d_GetIn(&theViewPoint, &Direction);
	geVec3d_AddScaled(&Pos, &Direction, CurrentDistance, &Back);
// changed QD 12/15/05
	// geVec3d_AddScaled(&Pos, &Direction, 0.0f, &Front);
	geVec3d_Copy(&Pos, &Front);
// end change

	if(IsoCollFlag)
	{
		if(geWorld_Collision(CCD->World(), &CameraExtBox.Min, &CameraExtBox.Max, &Front,
		&Back, /*GE_VISIBLE_CONTENTS*/GE_CONTENTS_SOLID_CLIP, GE_COLLIDE_ALL, 0, NULL, NULL, &Collision))
		{
// changed QD 01/2004
			// can't be negative (sqrt)
			// CurrentDistance = (geFloat)fabs(geVec3d_DistanceBetween(&Collision.Impact, &Front));
			// if(CurrentDistance < 0.0f)
			//	CurrentDistance = 0.0f;
			CurrentDistance = geVec3d_DistanceBetween(&Collision.Impact, &Front);
// end change

			if(CurrentDistance > (m_defaultdistance*ActorScale))
				CurrentDistance = m_defaultdistance*ActorScale;

			geVec3d_AddScaled(&Pos, &Direction, CurrentDistance, &Back);
		}

// changed QD 01/2004
		if(CCD->Meshes()->CollisionCheck(&CameraExtBox.Min, &CameraExtBox.Max, Front, Back, &Collision))
		{
			geFloat CurrentDistance2 = geVec3d_DistanceBetween(&Collision.Impact, &Front);

			if(CurrentDistance2 > (m_defaultdistance*ActorScale))
				CurrentDistance2 = m_defaultdistance*ActorScale;

			if(CurrentDistance2<CurrentDistance)
			{
				CurrentDistance = CurrentDistance2;
				geVec3d_AddScaled(&Pos, &Direction, CurrentDistance2, &Back);
			}
		}
// end change
	}

	m_currentdistance = CurrentDistance/ActorScale;

	if(IsoCollFlag)
	{
		geFloat fAlpha = 255.0f;

		if(CurrentDistance < (40.0f*ActorScale))
		{
			fAlpha = (10.0f*((CurrentDistance-((geFloat)fabs(ActorExtBox.Min.Z)+1.0f))/ActorScale))+30.0f;

			if(fAlpha < (15.0f*ActorScale))
				fAlpha = 0.0f;

			if(fAlpha > 255.0f)
				fAlpha = 255.0f;
		}

		CCD->ActorManager()->SetAlpha(theActor, fAlpha);			// Adjust actor alpha
	}

	geVec3d_Subtract( &Pos, &Back, &Orient );

	// protect from Div by Zero
	if(CurrentDistance > 0.0f)
	{
		x = Orient.X;
		// changed QD 12/15/05
		// Orient.X = GE_PI*0.5f - (geFloat)acos(Orient.Y / CurrentDistance);
		Orient.X = GE_PIOVER2 - (geFloat)acos(Orient.Y / CurrentDistance);
		Orient.Y = (geFloat)atan2(x, Orient.Z) + GE_PI;
		Orient.Z = 0.0f;	// roll is zero - always!!?

	}

	Rotation = Orient;										// Set camera orientation
	Translation = Back;										// Set camera translation

	return;
}

/* ------------------------------------------------------------------------------------ */
//	Get3rdData
/* ------------------------------------------------------------------------------------ */
void CCameraManager::Get3rdData(float *distance, float *camerax, float *cameray)
{
	*distance = m_defaultdistance;
	*camerax = m_cameraX;
	*cameray = m_cameraY;
}

/* ------------------------------------------------------------------------------------ */
//	Set3rdData
/* ------------------------------------------------------------------------------------ */
void CCameraManager::Set3rdData(float distance, float camerax, float cameray)
{
	m_defaultdistance = distance;
	m_cameraX = camerax;
	m_cameraY = cameray;
}


/* ----------------------------------- END OF FILE ------------------------------------ */
