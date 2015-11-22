/*
CPlayer.cpp:		Player character encapsulation class

  (c) 1999 Edward A. Averill, III
  
	This file contains the class implementation for the player
	character (avatar) in an RGF-based game.
*/

//	You only need the one, master include file.

#include "RabidFramework.h"

extern "C" void	DrawBoundBox(geWorld *World, const geVec3d *Pos, const geVec3d *Min, const geVec3d *Max);
extern geSound_Def *SPool_Sound(char *SName);

//	CPlayer
//
//	Default constructor

CPlayer::CPlayer()
{
	//	Initialize all state variables.  Note that the player avatar is not
	//	..scaled up at all!  Why?  This means the designer can make a level
	//	..that is, effectively, _immense_ in the level editor without running
	//	..up against the G3D 1.0 limit of 4096x4096x4096 maximum size.  The
	//	..player CAN be scaled when the environment is loaded, though.
	
	//	03/22/2000 eaa3 Add Ralph Deane's scaling fixes
	
	m_Speed = m_CurrentSpeed = 3.0f;
	m_JumpSpeed = 60.0f;
	m_JumpActive = false;
	m_LastMovementSpeed = 0.0f;
	m_StepHeight = 16.0f;
	m_FirstPersonView = true;
	m_CoeffSpeed = 1.0f;
	m_LookMode = false;
	m_PositionHistoryPtr = 0;
	m_FogActive = false;
	m_ClipActive = false;
	m_PlayerViewPoint = 0;
	m_Gravity = -4.6f;
	m_run = false;
	m_Scale = 1.0f;
	m_lastdirection = 0;
	Alive = true;
	
	m_oldcrouch = m_crouch = GE_FALSE;

	m_Moving = MOVEIDLE;
	m_SlideWalk = MOVEIDLE;

	EnvAudio = NULL;
	lightactive = false;
	lighteffect = -1;
	StaminaDelay = 1.0f;
	StaminaTime = 0.0f;
	Allow3rdLook = true;
	
	DefaultMotion = NULL;
	
	for(int nTemp = 0; nTemp < 16; nTemp++)
		Contents[nTemp] = NULL;
	
	return;
}

//	~CPlayer
//
//	Default destructor

CPlayer::~CPlayer()
{
	//	Release all the environmental audio sounds
	
	if(EnvAudio != NULL)
	{
		if(DefaultMotion != NULL)
			geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), DefaultMotion);
		for(int nTemp = 0; nTemp < 16; nTemp++)
		{
			if(Contents[nTemp] != NULL)
				geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), Contents[nTemp]);
		}
	}
	geActor_Destroy(&Actor);
	return;
}

//	LoadAvatar
//
//	Load the actor file (.ACT) to be used as the players avatar.
//	..In normal, first-person use this won't actually be seen
//	..except in mirrors.

int CPlayer::LoadAvatar(char *szFile)
{
	Actor = CCD->ActorManager()->LoadActor(szFile, NULL);				// Load it.
	if(!Actor)
	{
		char szError[256];
		sprintf(szError,"Missing Player Actor '%s'", szFile);
		CCD->ReportError(szError, false);
		CCD->ShutdownLevel();
		delete CCD;
		MessageBox(NULL, szError,"Player", MB_OK);
		exit(-333);
	}
	CCD->CameraManager()->BindToActor(Actor);							// Bind the camera

	return RGF_SUCCESS;
}

//	LoadConfiguration
//
//	Loads the attribute configuration from the player configuration file
//	..defined in the PlayerSetup entity.

int CPlayer::LoadConfiguration()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Load environmental audio
	
	LoadEnvironmentalAudio();
	
	pSet = geWorld_GetEntitySet(CCD->World(), "PlayerSetup");
	
	if(!pSet) 
	{
		char szError[256];
		sprintf(szError,"Missing PlayerSetup");
		CCD->ReportError(szError, false);
		CCD->ShutdownLevel();
		delete CCD;
		MessageBox(NULL, szError,"Fatal Error", MB_OK);
		exit(-333);
	}
	
	//	Ok, get the setup information.  There should only be one, so
	//	we'll just take the first one we run into.
	
	pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL);
	PlayerSetup *pSetup = (PlayerSetup*)geEntity_GetUserData(pEntity);

	m_PlayerViewPoint = pSetup->LevelViewPoint;
	m_Scale = pSetup->PlayerScaleFactor;			// Get scale factor
	float actscale = pSetup->ActorScaleFactor;
	geVec3d m_Rotation;
	m_Rotation.X = 0.0174532925199433f*pSetup->ActorRotation.X;
	m_Rotation.Y = 0.0174532925199433f*pSetup->ActorRotation.Y;
	m_Rotation.Z = 0.0174532925199433f*pSetup->ActorRotation.Z;
	CCD->ActorManager()->SetAligningRotation(Actor, m_Rotation);
	CCD->ActorManager()->SetTilt(Actor, false);
	CCD->ActorManager()->SetBoxChange(Actor, false);	
	CCD->ActorManager()->SetDefaultMotion(Actor, ANIMIDLE);
	CCD->ActorManager()->SetMotion(Actor, ANIMIDLE);
	CCD->ActorManager()->SetScale(Actor, m_Scale*actscale);
	CCD->ActorManager()->SetBoundingBox(Actor, ANIMIDLE);
	CCD->ActorManager()->SetType(Actor, ENTITY_ACTOR);
	
	geExtBox theBox;
	CCD->ActorManager()->GetBoundingBox(Actor, &theBox);
	m_CurrentHeight = theBox.Max.Y;
	
	int nFlags = 0;
	// Mode
	geVec3d theRotation = {0.0f, 0.0f, 0.0f};
	CCD->ActorManager()->Rotate(Actor, theRotation);
	geVec3d theTranslation = {0.0f, 0.0f, 0.0f};
	switch(m_PlayerViewPoint)
	{
	case 0:
	default:
		nFlags = kCameraTrackPosition;
		theTranslation.Y = m_CurrentHeight*0.75f;
		if(pSetup->Level1stHeight!=0.0f)
			theTranslation.Y = pSetup->Level1stHeight*0.75f*m_Scale;
		CCD->CameraManager()->SetCameraOffset(theTranslation, 
			theRotation);				// Set offset
		SwitchToFirstPerson();
		break;
	case 1:
		nFlags = kCameraTrackThirdPerson | kCameraTrackFree;
		theRotation.X = 0.0174532925199433f*(5);
		if(pSetup->Level3rdAngle!=0.0f)
			theRotation.X = 0.0174532925199433f*pSetup->Level3rdAngle;
		theTranslation.Y = m_CurrentHeight;
		if(pSetup->Level3rdHeight!=0.0f)
			theTranslation.Y = pSetup->Level3rdHeight*m_Scale;
		theTranslation.Z = 133.0f;
		if(pSetup->Level3rdDistance!=0.0f)
			theTranslation.Z = pSetup->Level3rdDistance;
		CCD->CameraManager()->SetCameraOffset(theTranslation, 
			theRotation);				// Set offset
		SwitchToThirdPerson();
		CCD->CameraManager()->ResetCamera();
		break;
	case 2:
		nFlags = kCameraTrackIso;
		theRotation.X = 0.0174532925199433f*(45);
		if(pSetup->LevelIsoAngleX!=0.0f)
			theRotation.X = 0.0174532925199433f*pSetup->LevelIsoAngleX;
		if(pSetup->LevelIsoAngleY!=0.0f)
			theRotation.Y = 0.0174532925199433f*pSetup->LevelIsoAngleY;
		theTranslation.Y = m_CurrentHeight;
		if(pSetup->LevelIsoHeight!=0.0f)
			theTranslation.Y = pSetup->LevelIsoHeight*m_Scale;
		theTranslation.Z = 266.0f;
		if(pSetup->LevelIsoDistance!=0.0f)
			theTranslation.Z = pSetup->LevelIsoDistance;
		CCD->CameraManager()->SetCameraOffset(theTranslation, 
			theRotation);				// Set offset
		SwitchToThirdPerson();
		CCD->CameraManager()->ResetCamera();
		break;
	}

	// eaa3 12/18/2000 Head bob setup
	m_HeadBobSpeed = pSetup->HeadBobSpeed;
	m_HeadBobLimit = pSetup->HeadBobLimit;
	if(pSetup->HeadBobbing == GE_TRUE)
		CCD->CameraManager()->EnableHeadBob(true);
	else
		CCD->CameraManager()->EnableHeadBob(false);
	// eaa3 12/18/2000 end

	CCD->CameraManager()->SetTrackingFlags(nFlags);		// Set tracking flags
	CCD->ActorManager()->SetAutoStepUp(Actor, true);	// Allow stepping up
	CCD->ActorManager()->SetStepHeight(Actor, 32.0f);	// Set step height

	LiteRadiusMin = pSetup->LiteRadiusMin;
	LiteRadiusMax = pSetup->LiteRadiusMax;
	LiteColorMin = pSetup->LiteColorMin;
	LiteColorMax = pSetup->LiteColorMax;
	LiteIntensity = pSetup->LiteIntensity;
	StaminaDelay = pSetup->StaminaDelay;
	StaminaTime = 0.0f;
	Allow3rdLook = pSetup->Allow3rdLook;

	m_Speed = m_CurrentSpeed*= m_Scale;
	m_JumpSpeed *= m_Scale;

	CIniFile AttrFile(pSetup->AttributeInfoFile);
	if(!AttrFile.ReadFile())
	{
		char szAttrError[256];
		sprintf(szAttrError,"Can't open player config file '%s'", pSetup->AttributeInfoFile);
		CCD->ReportError(szAttrError, false);
		return RGF_FAILURE;
	}
	m_Attr = CCD->ActorManager()->Inventory(Actor);
	CString KeyName = AttrFile.FindFirstKey();
	char szAttr[64];
	int InitialValue, LowValue, HighValue;
	while(KeyName != "")
	{
		strcpy(szAttr,KeyName);
		InitialValue = AttrFile.GetValueI(KeyName, "initial");
		m_Attr->AddAndSet(szAttr, InitialValue);
		LowValue = AttrFile.GetValueI(KeyName, "low");
		HighValue = AttrFile.GetValueI(KeyName, "high");
		m_Attr->SetValueLimits(szAttr, LowValue, HighValue);
		KeyName = AttrFile.FindNextKey();
	}
	
	return RGF_SUCCESS;
}

//	LoadEnvironmentalAudio
//
//	Look up the environmental audio entity in the level, if there is one,
//	..and load up all the sounds required for player motion audio.

int CPlayer::LoadEnvironmentalAudio()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	//	Ok, check to see if there's a PlayerSetup around...
	
	pSet = geWorld_GetEntitySet(CCD->World(), "EnvAudioSetup");
	
	if(!pSet) 
		return RGF_FAILURE;									// No setup?
	
	//	Ok, get the setup information.  There should only be one, so
	//	..we'll just take the first one we run into.
	
	pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL);
	EnvAudio = (EnvAudioSetup *)geEntity_GetUserData(pEntity);
	
	//	Cool, now load up all the audio and prep the motion audio for
	//	..playback.
	
	DefaultMotion = NULL;
	DefaultMotionHandle = NULL;
	
	for(int nTemp = 0; nTemp < 16; nTemp++)
	{
		Contents[nTemp] = NULL;
		ContentsHandles[nTemp] = NULL;
	}
	
	DefaultMotion = LoadAudioClip(EnvAudio->DefaultMotionSound);
	Contents[0] = LoadAudioClip(EnvAudio->SoundWhenInWater);
	Contents[1] = LoadAudioClip(EnvAudio->SoundWhenInLava);
	Contents[2] = LoadAudioClip(EnvAudio->SoundWhenInToxicGas);
	Contents[3] = LoadAudioClip(EnvAudio->SoundWhenInZeroG);
	Contents[4] = LoadAudioClip(EnvAudio->SoundWhenInFrozen);
	Contents[5] = LoadAudioClip(EnvAudio->SoundWhenInSludge);
	Contents[6] = LoadAudioClip(EnvAudio->SoundWhenInSlowMotion);
	Contents[7] = LoadAudioClip(EnvAudio->SoundWhenInFastMotion);
	Contents[8] = LoadAudioClip(EnvAudio->SoundWhenUsingLadders);
	
	return RGF_SUCCESS;
}

//	MoveToStart
//
//	Move the player avatar to the first PlayerStart we find
//	..in the world.

int CPlayer::MoveToStart()
{
	geEntity_EntitySet* lEntitySet;
	geEntity* lEntity;
	PlayerStart* theStart;
	
	lEntitySet = geWorld_GetEntitySet(CCD->World(), "EnvironmentSetup");
	
	if(lEntitySet != NULL)
	{
		lEntity = geEntity_EntitySetGetNextEntity(lEntitySet, NULL);
		if(lEntity)
		{	
			EnvironmentSetup *theState = (EnvironmentSetup*)geEntity_GetUserData(lEntity);
			EnvSetup = theState;
			RealJumping = theState->RealJumping;
			if(theState->Gravity != 0.0f)
				m_Gravity = theState->Gravity;				// Over-ride default gravity
			if(theState->JumpSpeed != 0.0f)
				m_JumpSpeed = theState->JumpSpeed;		// Over-ride default jump speed
			if(theState->Speed != 0.0f)
				m_Speed = m_CurrentSpeed = theState->Speed;	// Over-ride default speed
			if(theState->StepHeight != 0.0f)
				m_StepHeight = theState->StepHeight;	// Over-ride default step-up height
			float detailevel = (float)CCD->MenuManager()->GetDetail();
			if(theState->EnableDistanceFog == GE_TRUE)
			{
				float start = theState->FogStartDistLow + ((theState->FogStartDistHigh - theState->FogStartDistLow)*(detailevel/100));
				float end = theState->TotalFogDistLow + ((theState->TotalFogDistHigh - theState->TotalFogDistLow)*(detailevel/100));
				CCD->Engine()->SetFogParameters(theState->DistanceFogColor,
					start, end);
				m_FogActive = true;
			}
			if(theState->UseFarClipPlane == GE_TRUE)
			{
				float dist = theState->FarClipPlaneDistLow + ((theState->FarClipPlaneDistHigh - theState->FarClipPlaneDistLow)*(detailevel/100));
				CCD->CameraManager()->SetFarClipPlane(dist);
				m_ClipActive = true;
			}
			if(theState->MaxLookUpAngle > 0.0f || theState->MaxLookDwnAngle > 0.0f)
				CCD->CameraManager()->SetLook(0.0174532925199433f*theState->MaxLookUpAngle, 0.0174532925199433f*theState->MaxLookDwnAngle);
		}
	}
	
	lEntitySet = geWorld_GetEntitySet(CCD->World(), "PlayerStart");
	
	if(lEntitySet == NULL)
	{
		char szError[256];
		sprintf(szError,"Missing PlayerStart");
		CCD->ReportError(szError, false);
		CCD->ShutdownLevel();
		delete CCD;
		MessageBox(NULL, szError,"Fatal Error", MB_OK);
		exit(-333);
	}
	
	for(lEntity= geEntity_EntitySetGetNextEntity(lEntitySet, NULL); lEntity;
	lEntity= geEntity_EntitySetGetNextEntity(lEntitySet, lEntity)) 
	{
		theStart = (PlayerStart *)geEntity_GetUserData(lEntity);
		if(CCD->ChangeLevel() && !EffectC_IsStringNull(CCD->StartName()))
		{
			if(!stricmp(CCD->StartName(),theStart->szEntityName))
				break;
		}
		else
		{
			if(EffectC_IsStringNull(theStart->szEntityName))
				break;
		}
	}
	
	if(!lEntity && CCD->ChangeLevel() && !EffectC_IsStringNull(CCD->StartName()))
	{
		for(lEntity= geEntity_EntitySetGetNextEntity(lEntitySet, NULL); lEntity;
		lEntity= geEntity_EntitySetGetNextEntity(lEntitySet, lEntity)) 
		{
			theStart = (PlayerStart *)geEntity_GetUserData(lEntity);
			if(EffectC_IsStringNull(theStart->szEntityName))
				break;
		}
	}

	if(lEntity)
	{	
		theStart = (PlayerStart *)geEntity_GetUserData(lEntity);

		m_Translation.X = theStart->origin.X;
		m_Translation.Y = theStart->origin.Y;
		m_Translation.Z = theStart->origin.Z;

		geVec3d theRotation;
		theRotation.X = 0.0f; 
		theRotation.Y = 0.0174532925199433f*(theStart->Angle.Y-90.0f);
		theRotation.Z = 0.0f; 
		CCD->ActorManager()->Rotate(Actor, theRotation);
		if(m_PlayerViewPoint==0)
		{
			CCD->CameraManager()->Rotate(theRotation);
		}
		CCD->CameraManager()->TrackMotion();

		CCD->ActorManager()->SetStepHeight(Actor, m_StepHeight);
		CCD->ActorManager()->SetAutoStepUp(Actor, true);
		CCD->ActorManager()->SetGravity(Actor, m_Gravity);			// Default gravity
		CCD->ActorManager()->Position(Actor, m_Translation);
		
		if(!EffectC_IsStringNull(theStart->szCDTrack))
		{
			// Start CD player running, if we have one
			if(CCD->CDPlayer())
			{
				CCD->CDPlayer()->SetCdOn(true);
				CCD->CDPlayer()->Play(atoi(theStart->szCDTrack), theStart->bSoundtrackLoops);
			}
		}
		if(!EffectC_IsStringNull(theStart->szMIDIFile))
		{
			// Start MIDI file playing
			if(CCD->MIDIPlayer())
				CCD->MIDIPlayer()->Play(theStart->szMIDIFile, theStart->bSoundtrackLoops);
		}
		if(!EffectC_IsStringNull(theStart->szStreamingAudio))
		{
			// Start streaming audio file playing
			if(CCD->AudioStreams())
				CCD->AudioStreams()->Play(theStart->szStreamingAudio, theStart->bSoundtrackLoops,
				true);
		}
	}
	else
	{
		char szError[256];
		sprintf(szError,"Missing PlayerStart");
		CCD->ReportError(szError, false);
		CCD->ShutdownLevel();
		delete CCD;
		MessageBox(NULL, szError,"Fatal Error", MB_OK);
		exit(-333);
	}
	
	//	Fill the position history with the new location
	
	for(int nCtr = 0; nCtr < 50; nCtr++)
		AddPosition();
	
	return RGF_SUCCESS;
}

// 
//	CheckKeyLook
//
//	Adjust the various rotation angles depending on the current
//	..key rotation.

void CPlayer::CheckKeyLook(int keyrotate)
{
		
	geFloat TURN_SPEED = 40.0f * CCD->MenuManager()->GetMouseSen();
	geFloat UPDOWN_SPEED = 40.0f * CCD->MenuManager()->GetMouseSen();
	
	if(RealJumping && (CCD->ActorManager()->ForceActive(Actor, 0) || CCD->ActorManager()->Falling(Actor)==GE_TRUE))
		return;

	if(m_LookMode)
	{
		if(keyrotate & 8)
				CCD->CameraManager()->TiltDown(UPDOWN_SPEED);
		if(keyrotate & 1)
			CCD->CameraManager()->TiltUp(UPDOWN_SPEED);
		if(keyrotate & 16)
			CCD->CameraManager()->Center();
		if(keyrotate & 4)
			CCD->CameraManager()->TurnLeft(TURN_SPEED);
		if(keyrotate & 2)
			CCD->CameraManager()->TurnRight(TURN_SPEED);
		return;
	}

	if(m_FirstPersonView)
	{
		if(keyrotate & 8)
		{
			if(CCD->ActorManager()->TiltDown(Actor, UPDOWN_SPEED) == RGF_SUCCESS)
				CCD->CameraManager()->TiltDown(UPDOWN_SPEED);
		}
		if(keyrotate & 1)
		{
			if(CCD->ActorManager()->TiltUp(Actor, UPDOWN_SPEED) == RGF_SUCCESS)
				CCD->CameraManager()->TiltUp(UPDOWN_SPEED);
		}
		if(keyrotate & 16)
			CCD->CameraManager()->Center();
		if(keyrotate & 4)
		{
			if(CCD->ActorManager()->TurnLeft(Actor, TURN_SPEED) == RGF_SUCCESS)
				CCD->CameraManager()->TurnLeft(TURN_SPEED);
		}
		if(keyrotate & 2)
		{
			if(CCD->ActorManager()->TurnRight(Actor, TURN_SPEED) == RGF_SUCCESS)
				CCD->CameraManager()->TurnRight(TURN_SPEED);
		}
	}
    else
	{
		if(keyrotate & 4)
		{
			if(CCD->ActorManager()->TurnLeft(Actor, TURN_SPEED) == RGF_SUCCESS)
				CCD->CameraManager()->TurnLeft(TURN_SPEED);
		}
		if(keyrotate & 2)
		{
			if(CCD->ActorManager()->TurnRight(Actor, TURN_SPEED) == RGF_SUCCESS)
				CCD->CameraManager()->TurnRight(TURN_SPEED);
		}
		if(keyrotate & 1 && Allow3rdLook) // is it to the top?
			CCD->CameraManager()->TiltXDown(UPDOWN_SPEED);
		if(keyrotate & 8 && Allow3rdLook) // is it to the bottom?
			CCD->CameraManager()->TiltXUp(UPDOWN_SPEED); 
	}
	
	return;
}

//	CheckMouseLook
//
//	Adjust the various rotation angles depending on the current
//	..position of the mouse.

void CPlayer::CheckMouseLook()
{
	POINT temppos, tempfilter;
	RECT client;
	geFloat TURN_SPEED;				// speed camera will move if turning left/right
	geFloat UPDOWN_SPEED;			// speed camera will move if looking up/down
	POINT pos;

	GetCursorPos(&temppos);	// get the mouse position in SCREEN coordinates
	if(CCD->MenuManager()->GetMouseFilter())
	{
		tempfilter = CCD->MenuManager()->GetFilter();
		CCD->MenuManager()->SetFilter(temppos);
		if(tempfilter.x == -1)
			tempfilter = temppos;
		temppos.x = (temppos.x+tempfilter.x)/2;
		temppos.y = (temppos.y+tempfilter.y)/2;
	}
	
	if(CCD->Engine()->FullScreen())
    {
		pos.x = CCD->Engine()->Width()/2;			// calculate the center of the screen
		pos.y = CCD->Engine()->Height()/2;			// calculate the center of the screen
		SetCursorPos(pos.x, pos.y);	// set the cursor in the center of the screen
		TURN_SPEED   = abs(temppos.x-pos.x) * CCD->MenuManager()->GetMouseSen();	// calculate the turning speed
		UPDOWN_SPEED = abs(temppos.y-pos.y) * CCD->MenuManager()->GetMouseSen();	// calculate the up/down speed
    }
	else
    {
		GetClientRect(CCD->Engine()->WindowHandle(),&client);	// get the client area of the window
		pos.x = client.right/2;						// calculate the center of the client area
		pos.y = client.bottom/2;					// calculate the center of the client area
		ClientToScreen(CCD->Engine()->WindowHandle(),&pos);		// convert to SCREEN coordinates
		SetCursorPos(pos.x,pos.y);					// put the cursor in the middle of the window
		TURN_SPEED   = abs(temppos.x-pos.x) * CCD->MenuManager()->GetMouseSen();	// calculate the turning speed
		UPDOWN_SPEED = abs(temppos.y-pos.y) * CCD->MenuManager()->GetMouseSen();	// calculate the up/down speed
    }
	
		
	if(RealJumping && (CCD->ActorManager()->ForceActive(Actor, 0) || CCD->ActorManager()->Falling(Actor)==GE_TRUE))
		return;

    if(CCD->MenuManager()->GetMouseReverse())
	{
		temppos.y = pos.y+(pos.y-temppos.y);
	}
	
	if((temppos.x != pos.x) || (temppos.y != pos.y))
	{
		if(m_LookMode)
		{
			// First person we run both the camera AND the actor
			if(temppos.y > pos.y) // is it to the top?
				CCD->CameraManager()->TiltDown(UPDOWN_SPEED);
			else if (temppos.y < pos.y) // is it to the bottom?
				CCD->CameraManager()->TiltUp(UPDOWN_SPEED);

			if(temppos.x > pos.x) // is it to the left?
				CCD->CameraManager()->TurnLeft(TURN_SPEED);
			else if (temppos.x < pos.x) // is it to the right?
				CCD->CameraManager()->TurnRight(TURN_SPEED);
			return;
		}
		if(m_FirstPersonView)
		{
			// First person we run both the camera AND the actor
			if(temppos.y > pos.y) // is it to the top?
			{
				if(CCD->ActorManager()->TiltDown(Actor, UPDOWN_SPEED) == RGF_SUCCESS)
					CCD->CameraManager()->TiltDown(UPDOWN_SPEED);
			}
			else if (temppos.y < pos.y) // is it to the bottom?
			{
				if(CCD->ActorManager()->TiltUp(Actor, UPDOWN_SPEED) == RGF_SUCCESS)
					CCD->CameraManager()->TiltUp(UPDOWN_SPEED);
			}
			if(temppos.x > pos.x) // is it to the left?
			{
				if(CCD->ActorManager()->TurnLeft(Actor, TURN_SPEED) == RGF_SUCCESS)
					CCD->CameraManager()->TurnLeft(TURN_SPEED);
			}
			else if (temppos.x < pos.x) // is it to the right?
			{
				if(CCD->ActorManager()->TurnRight(Actor, TURN_SPEED) == RGF_SUCCESS)
					CCD->CameraManager()->TurnRight(TURN_SPEED);
			}
		}
		else			// Third-person mode, no camera tilt...
		{
			// In third-person the camera handles all the hard work for us.
			if(temppos.x > pos.x) // is it to the left?
				CCD->ActorManager()->TurnLeft(Actor, TURN_SPEED);
			else if (temppos.x < pos.x) // is it to the right?
				CCD->ActorManager()->TurnRight(Actor, TURN_SPEED);
			if(temppos.y < pos.y && Allow3rdLook) // is it to the top?
			{
				CCD->CameraManager()->TiltXDown(UPDOWN_SPEED);
				CCD->ActorManager()->TiltDown(Actor, UPDOWN_SPEED);
			}
			else if (temppos.y > pos.y && Allow3rdLook) // is it to the bottom?
			{
				CCD->CameraManager()->TiltXUp(UPDOWN_SPEED);
				CCD->ActorManager()->TiltUp(Actor, UPDOWN_SPEED);
			}
		}
	}
	
	return;
}

//	ProcessMove
//
//	Check the contents of the
//	..current area to see if we need to play Something Special.

int CPlayer::ProcessMove(bool bPlayerMoved)
{
	static bool BobUp = true;							// eaa3 12/18/2000 head bob control
	
	float nBobValue = CCD->CameraManager()->GetHeadBobOffset();
	
	if(!bPlayerMoved || CCD->ActorManager()->ForceActive(Actor, 0) || CCD->ActorManager()->Falling(Actor))
	{
		// eaa3 12/18/2000 We need to "settle" the viewpoint down to a neutral
		// ..position if we're bobbing.
		if(nBobValue != 0.0f)
		{
			nBobValue -= m_HeadBobSpeed;
			if(nBobValue <= 0.0f)
			{
				nBobValue = 0.0f;
				BobUp = true;
			}
			CCD->CameraManager()->SetHeadBobOffset(nBobValue);	// Adjust it!
		}
		// eaa3 12/18/2000 - End head settling code
	}
	else
	{
		//	eaa3 12/18/2000
		//	Process "head bobbing"
		//	..Note that head bobbing only occurs when the camera is bound to an
		//	..actor, and has NOT been tested in third-person mode.
		
		if(BobUp)
		{
			nBobValue += m_HeadBobSpeed;
			if(nBobValue > m_HeadBobLimit)
				BobUp = false;
		}
		else
		{
			nBobValue -= m_HeadBobSpeed;
			if(nBobValue <= 0.0f)
				BobUp = true;
		}
		
		CCD->CameraManager()->SetHeadBobOffset(nBobValue);
	}
	
	//	End of "head bob" processing
	//	eaa3 12/18/2000
	
	int ZoneType; 
	
	CCD->ActorManager()->GetActorZone(Actor, &ZoneType);

		  if(ZoneType & kWaterZone)
		  {
			  if(geSound_SoundIsPlaying(CCD->Engine()->AudioSystem(),
				  ContentsHandles[0]) != GE_TRUE && bPlayerMoved)
				  ContentsHandles[0] = geSound_PlaySoundDef(
				  CCD->Engine()->AudioSystem(), Contents[0], 
				  1.0f, 0.0f, 1.0f, GE_FALSE);
			  else
			  {
				  if(!bPlayerMoved && ContentsHandles[0])
					  geSound_StopSound(CCD->Engine()->AudioSystem(), ContentsHandles[0]);
			  }
		  }
		  else if(ZoneType & kLavaZone)
		  {
			  if(geSound_SoundIsPlaying(CCD->Engine()->AudioSystem(),
				  ContentsHandles[1]) != GE_TRUE && bPlayerMoved)
				  ContentsHandles[1] = geSound_PlaySoundDef(
				  CCD->Engine()->AudioSystem(), Contents[1], 
				  1.0f, 0.0f, 1.0f, GE_FALSE);
			  else
			  {
				  if(!bPlayerMoved && ContentsHandles[1])
					  geSound_StopSound(CCD->Engine()->AudioSystem(), ContentsHandles[1]);
			  }
		  }
		  else if(ZoneType & kToxicGasZone)
		  {
			  if(geSound_SoundIsPlaying(CCD->Engine()->AudioSystem(),
				  ContentsHandles[2]) != GE_TRUE && bPlayerMoved)
				  ContentsHandles[2] = geSound_PlaySoundDef(
				  CCD->Engine()->AudioSystem(), Contents[2], 
				  1.00f, 0.0f, 1.0f, GE_FALSE);
			  else
			  {
				  if(!bPlayerMoved && ContentsHandles[2])
					  geSound_StopSound(CCD->Engine()->AudioSystem(), ContentsHandles[2]);
			  }
		  }
		  else if(ZoneType & kNoGravityZone)
		  {
			  if(geSound_SoundIsPlaying(CCD->Engine()->AudioSystem(),
				  ContentsHandles[3]) != GE_TRUE && bPlayerMoved)
				  ContentsHandles[3] = geSound_PlaySoundDef(
				  CCD->Engine()->AudioSystem(), Contents[3], 
				  1.00f, 0.0f, 1.0f, GE_FALSE);
			  else
			  {
				  if(!bPlayerMoved && ContentsHandles[3])
					  geSound_StopSound(CCD->Engine()->AudioSystem(), ContentsHandles[3]);
			  }
		  }
		  else if(ZoneType & kFrozenZone)
		  {
			  if(geSound_SoundIsPlaying(CCD->Engine()->AudioSystem(),
				  ContentsHandles[4]) != GE_TRUE && bPlayerMoved)
				  ContentsHandles[4] = geSound_PlaySoundDef(
				  CCD->Engine()->AudioSystem(), Contents[4], 
				  1.0f, 0.0f, 1.0f, GE_FALSE);
			  else
			  {
				  if(!bPlayerMoved && ContentsHandles[4])
					  geSound_StopSound(CCD->Engine()->AudioSystem(), ContentsHandles[4]);
			  }
		  }
		  else if(ZoneType & kSludgeZone)
		  {
			  if(geSound_SoundIsPlaying(CCD->Engine()->AudioSystem(),
				  ContentsHandles[5]) != GE_TRUE && bPlayerMoved)
				  ContentsHandles[5] = geSound_PlaySoundDef(
				  CCD->Engine()->AudioSystem(), Contents[5], 
				  1.0f, 0.0f, 1.0f, GE_FALSE);
			  else
			  {
				  if(!bPlayerMoved && ContentsHandles[5])
					  geSound_StopSound(CCD->Engine()->AudioSystem(), ContentsHandles[5]);
			  }
		  }
		  else if(ZoneType & kSlowMotionZone)
		  {
			  if(geSound_SoundIsPlaying(CCD->Engine()->AudioSystem(),
				  ContentsHandles[6]) != GE_TRUE && bPlayerMoved)
				  ContentsHandles[6] = geSound_PlaySoundDef(
				  CCD->Engine()->AudioSystem(), Contents[6], 
				  1.0f, 0.0f, 1.0f, GE_FALSE);
			  else
			  {
				  if(!bPlayerMoved && ContentsHandles[6])
					  geSound_StopSound(CCD->Engine()->AudioSystem(), ContentsHandles[6]);
			  }
		  }
		  else if(ZoneType & kFastMotionZone)
		  {
			  if(geSound_SoundIsPlaying(CCD->Engine()->AudioSystem(),
				  ContentsHandles[7]) != GE_TRUE && bPlayerMoved)
				  ContentsHandles[7] = geSound_PlaySoundDef(
				  CCD->Engine()->AudioSystem(), Contents[7], 
				  1.00f, 0.0f, 1.0f, GE_FALSE);
			  else
			  {
				  if(!bPlayerMoved && ContentsHandles[7])
					  geSound_StopSound(CCD->Engine()->AudioSystem(), ContentsHandles[7]);
			  }
		  }
		  else if(ZoneType & kClimbLaddersZone)
		  {
			  if(geSound_SoundIsPlaying(CCD->Engine()->AudioSystem(),
				  ContentsHandles[8]) != GE_TRUE && bPlayerMoved)
				  ContentsHandles[8] = geSound_PlaySoundDef(
				  CCD->Engine()->AudioSystem(), Contents[8], 
				  1.00f, 0.0f, 1.0f, GE_FALSE);
			  else
			  {
				  if(!bPlayerMoved && ContentsHandles[8])
					  geSound_StopSound(CCD->Engine()->AudioSystem(), ContentsHandles[8]);
			  }
		  }
		  else 
		  {
			  if(geSound_SoundIsPlaying(CCD->Engine()->AudioSystem(),
				  DefaultMotionHandle) != GE_TRUE && bPlayerMoved)
				  DefaultMotionHandle = geSound_PlaySoundDef(
				  CCD->Engine()->AudioSystem(), DefaultMotion, 
				  1.00f, 0.0f, 1.0f, GE_FALSE);
			  else
			  {
				  if(!bPlayerMoved && DefaultMotionHandle)
					  geSound_StopSound(CCD->Engine()->AudioSystem(), DefaultMotionHandle);
			  }
		  }
		  
		  return RGF_SUCCESS;
}

//	Move
//
//	Move the player forward/backward/left/right at a specific
//	..speed.

int CPlayer::Move(int nHow, geFloat fSpeed)
{
	if(m_LookMode)
		return RGF_SUCCESS;

	m_LastMovementSpeed = fSpeed;
	m_lastdirection = nHow;


	if(RealJumping && CCD->ActorManager()->ForceActive(Actor, 0))
		return RGF_SUCCESS;

	if(RealJumping && CCD->ActorManager()->Falling(Actor)==GE_TRUE)
	{
		geVec3d theDir;
		if(m_LastMovementSpeed!=0.0f && !CCD->ActorManager()->ForceActive(Actor, 1) )
		{
			switch(m_lastdirection)
			{
			case RGF_K_FORWARD:
				CCD->ActorManager()->InVector(Actor, &theDir);
				break;
			case RGF_K_BACKWARD:
				CCD->ActorManager()->InVector(Actor, &theDir);
				geVec3d_Inverse(&theDir);
				break;
			case RGF_K_RIGHT:
				CCD->ActorManager()->LeftVector(Actor, &theDir);
				geVec3d_Inverse(&theDir);
				break;
			case RGF_K_LEFT:
				CCD->ActorManager()->LeftVector(Actor, &theDir);
				break;
			}
			CCD->ActorManager()->SetForce(Actor, 1, theDir, m_JumpSpeed, m_JumpSpeed/2.0f);
		}
		return RGF_SUCCESS;
	}

	if(fSpeed == 0.0f)
		return RGF_SUCCESS;
	
	m_run = m_Running;

	switch(nHow)
	{
	case RGF_K_FORWARD:
		CCD->ActorManager()->MoveForward(Actor, fSpeed);
		break;
	case RGF_K_BACKWARD:
		CCD->ActorManager()->MoveBackward(Actor, fSpeed);
		break;
	case RGF_K_RIGHT:
		CCD->ActorManager()->MoveRight(Actor, fSpeed);
		break;
	case RGF_K_LEFT:
		CCD->ActorManager()->MoveLeft(Actor, fSpeed);
		break;
	}
	
	return RGF_SUCCESS;
}

//	StartJump
//
//	Initiates a jump of the player.

void CPlayer::StartJump()
{
	int theZone;
	
	if(m_LookMode || m_JumpActive)
		return;

	CCD->ActorManager()->GetActorZone(Actor, &theZone);
	
	//	eaa 03/28/2000 You shouldn't be able to jump in certain
	//	..environments, like water, lava, or zero-G.
	
	switch(theZone)
	{
	case Water:
	case Lava:
	case ZeroG:
		return;					// Sorry, no jumping allowed
		break;
	}
	
	//if(!m_FirstPersonView)
	{
		m_JumpActive = true;
		return;
	}
	
	if(CCD->ActorManager()->ForceActive(Actor, 0))
		return;

	SetJump();
}

void CPlayer::SetJump()
{
	//	Jumping removes us as a passenger from any vehicle we're on.
	
	geActor *theVehicle = CCD->ActorManager()->GetVehicle(Actor);
	if(theVehicle != NULL)
		CCD->ActorManager()->RemovePassenger(Actor);
	
	if(CCD->ActorManager()->Falling(Actor) == GE_TRUE)
		return;						// No jumping in mid-air, heh heh heh
	
	//	Add a force to the player actor, aiming upwards, of our jump speed,
	//	..decaying 10 units over time.
	
	geVec3d theUp, theDir;
	CCD->ActorManager()->UpVector(Actor, &theUp);
	if(RealJumping && m_LastMovementSpeed!=0.0f)
	{
		switch(m_lastdirection)
		{
		case RGF_K_FORWARD:
			CCD->ActorManager()->InVector(Actor, &theDir);
			break;
		case RGF_K_BACKWARD:
			CCD->ActorManager()->InVector(Actor, &theDir);
			geVec3d_Inverse(&theDir);
			break;
		case RGF_K_RIGHT:
			CCD->ActorManager()->LeftVector(Actor, &theDir);
			geVec3d_Inverse(&theDir);
			break;
		case RGF_K_LEFT:
			CCD->ActorManager()->LeftVector(Actor, &theDir);
			break;
		}
		CCD->ActorManager()->SetForce(Actor, 1, theDir, m_JumpSpeed, m_JumpSpeed/2.0f);
	}
	CCD->ActorManager()->SetForce(Actor, 0, theUp, m_JumpSpeed, m_JumpSpeed);
}


geVec3d CPlayer::Position()
{
	CCD->ActorManager()->GetPosition(Actor, &m_Translation);
	return m_Translation;
}


void CPlayer::SwitchCamera(int mode)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	pSet = geWorld_GetEntitySet(CCD->World(), "PlayerSetup");
	pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL);
	PlayerSetup *pSetup = (PlayerSetup*)geEntity_GetUserData(pEntity);

	if(!pSetup->LevelViewAllowed)
		return;
	if(!pSetup->Level1stAllowed && mode ==0)
		return;
	if(!pSetup->Level3rdAllowed && mode ==1)
		return;
	if(!pSetup->LevelIsoAllowed && mode ==2)
		return;

	int nFlags = 0;
	
	// Mode
	geVec3d theRotation;
	CCD->ActorManager()->GetRotate(Actor, &theRotation);
	CCD->CameraManager()->Rotate(theRotation);
	theRotation.X = 0.0f;
	theRotation.Y = 0.0f;
	theRotation.Z = 0.0f;
	geVec3d theTranslation = {0.0f, 0.0f, 0.0f};
	switch(mode)
	{
	case 0:
	default:
		nFlags = kCameraTrackPosition;
		theTranslation.Y = m_CurrentHeight*0.75f;
		if(pSetup->Level1stHeight!=0.0f)
			theTranslation.Y = pSetup->Level1stHeight*0.75f*m_Scale;
		CCD->CameraManager()->SetCameraOffset(theTranslation, 
			theRotation);				// Set offset
		SwitchToFirstPerson();
		CCD->CameraManager()->ResetCamera();
		m_HeadBobSpeed = pSetup->HeadBobSpeed;
		m_HeadBobLimit = pSetup->HeadBobLimit;
		if(pSetup->HeadBobbing == GE_TRUE)
			CCD->CameraManager()->EnableHeadBob(true);
		else
			CCD->CameraManager()->EnableHeadBob(false);
		CCD->Weapons()->SetView(0);
		break;
	case 1:
		nFlags = kCameraTrackThirdPerson | kCameraTrackFree;
		theRotation.X = 0.0174532925199433f*(5);
		if(pSetup->Level3rdAngle!=0.0f)
			theRotation.X = 0.0174532925199433f*pSetup->Level3rdAngle;
		theTranslation.Y = m_CurrentHeight;
		if(pSetup->Level3rdHeight!=0.0f)
			theTranslation.Y = pSetup->Level3rdHeight*m_Scale;
		theTranslation.Z = 133.0f;
		if(pSetup->Level3rdDistance!=0.0f)
			theTranslation.Z = pSetup->Level3rdDistance;
		CCD->CameraManager()->SetCameraOffset(theTranslation, 
			theRotation);				// Set offset
		SwitchToThirdPerson();
		CCD->CameraManager()->ResetCamera();
		CCD->Weapons()->SetView(1);
		break;
	case 2:
		nFlags = kCameraTrackIso;
		theRotation.X = 0.0174532925199433f*(45);
		if(pSetup->LevelIsoAngleX!=0.0f)
			theRotation.X = 0.0174532925199433f*pSetup->LevelIsoAngleX;
		if(pSetup->LevelIsoAngleY!=0.0f)
			theRotation.Y = 0.0174532925199433f*pSetup->LevelIsoAngleY;
		theTranslation.Y = m_CurrentHeight;
		if(pSetup->LevelIsoHeight!=0.0f)
			theTranslation.Y = pSetup->LevelIsoHeight*m_Scale;
		theTranslation.Z = 266.0f;
		if(pSetup->LevelIsoDistance!=0.0f)
			theTranslation.Z = pSetup->LevelIsoDistance;
		CCD->CameraManager()->SetCameraOffset(theTranslation, 
			theRotation);				// Set offset
		SwitchToThirdPerson();
		CCD->CameraManager()->ResetCamera();
		CCD->Weapons()->SetView(1);
		break;
	}
	CCD->CameraManager()->SetTrackingFlags(nFlags);
	CCD->CameraManager()->TrackMotion();

}
// Tick
// Process actor animation changes
//

void CPlayer::Tick(geFloat dwTicks)
{
	CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(Actor);
	if(theInv->Value("health")<=0)
	{
		Alive = false;
		return;
	}

	if(theInv->Has("stamina"))
	{
		StaminaTime += dwTicks*0.001f;
		if(StaminaTime>=StaminaDelay)
		{
			StaminaTime = 0.0f;
			theInv->Modify("stamina", 1);
		}
	}

	if(theInv->Has("light"))
	{
		if(theInv->Value("light")!=0)
		{
			if(lightactive==false)
			{
				Glow	Gl;
				geExtBox theBox;
				geVec3d Pos = Position();
				CCD->ActorManager()->GetBoundingBox(Actor, &theBox);
				Pos.Y += theBox.Max.Y;
			    memset( &Gl, 0, sizeof(Gl));
				geVec3d_Copy(&(Pos), &(Gl.Pos));
				Gl.RadiusMin = LiteRadiusMin;
				Gl.RadiusMax = LiteRadiusMax;
				Gl.ColorMin.r = LiteColorMin.r;
				Gl.ColorMax.r = LiteColorMax.r;
				Gl.ColorMin.g = LiteColorMin.g;
				Gl.ColorMax.g = LiteColorMax.g;
				Gl.ColorMin.b = LiteColorMin.b;
				Gl.ColorMax.b = LiteColorMax.b;
				Gl.ColorMin.a = 255.0f;
				Gl.ColorMax.a = 255.0f;
				Gl.Intensity = LiteIntensity;
				Gl.DoNotClip = false;
				Gl.CastShadows = true;
				lighteffect = CCD->EffectManager()->Item_Add(EFF_LIGHT, (void *)&Gl);
				lightactive=true;
			}
		}
		else
		{
			if(lightactive)
			{
				if(lighteffect!=-1)
					CCD->EffectManager()->Item_Delete(EFF_LIGHT, lighteffect);
				lighteffect = -1;
				lightactive=false;
			}
		}
	}

	if(lightactive && lighteffect!=-1)
	{
		Glow Gl;
		geExtBox theBox;
		geVec3d Pos = Position();
		CCD->ActorManager()->GetBoundingBox(Actor, &theBox);
		Pos.Y += theBox.Max.Y;
		geVec3d_Copy(&(Pos), &(Gl.Pos));
		CCD->EffectManager()->Item_Modify(EFF_LIGHT, lighteffect, (void *)&Gl, GLOW_POS);
	}

	char *Motion;

	Motion = CCD->ActorManager()->GetMotion(Actor);
	switch(m_Moving)
	{
	case MOVEIDLE:	
		if(!strcmp(Motion, ANIMWALK) || !strcmp(Motion, ANIMI2WALK)
			|| !strcmp(Motion, ANIMRUN) || !strcmp(Motion, ANIMI2RUN))
		{
			CCD->ActorManager()->SetMotion(Actor, ANIMW2IDLE);
			CCD->ActorManager()->SetNextMotion(Actor, ANIMIDLE);
		}
		
		if(!strcmp(Motion, ANIMCRAWL))
		{
			CCD->ActorManager()->SetMotion(Actor, ANIMC2IDLE);
			CCD->ActorManager()->SetNextMotion(Actor, ANIMCIDLE);
		}

		if(!strcmp(Motion, ANIMSLIDECLEFT) || !strcmp(Motion, ANIMSLIDECRIGHT))
		{
			CCD->ActorManager()->SetMotion(Actor, ANIMCIDLE);
		}

		if(!strcmp(Motion, ANIMSLIDELEFT) || !strcmp(Motion, ANIMRUNSLIDELEFT)
			|| !strcmp(Motion, ANIMSLIDERIGHT) || !strcmp(Motion, ANIMRUNSLIDERIGHT))
		{
			CCD->ActorManager()->SetMotion(Actor, ANIMIDLE);
		}
		
		if(m_JumpActive && strcmp(Motion, ANIMSTARTJUMP) && strcmp(Motion, ANIMJUMP) && strcmp(Motion, ANIMCSTARTJUMP) && strcmp(Motion, ANIMLAND) && strcmp(Motion, ANIMCLAND) && strcmp(Motion, ANIMFALL))
		{
			if(CCD->ActorManager()->CheckAnimation(Actor, ANIMJUMP)==GE_TRUE)
			{
				CCD->ActorManager()->SetAnimationHeight(Actor, ANIMJUMP, false);
				if(!m_FirstPersonView)
				{
					if(m_crouch)
						CCD->ActorManager()->SetMotion(Actor, ANIMCSTARTJUMP);
					else
						CCD->ActorManager()->SetMotion(Actor, ANIMSTARTJUMP);
					CCD->ActorManager()->SetNextMotion(Actor, ANIMJUMP);
				}
				else
					CCD->ActorManager()->SetMotion(Actor, ANIMJUMP);
			}
			else
				m_JumpActive = false;
		}
		
		if(!strcmp(Motion, ANIMJUMP) && (CCD->ActorManager()->ForceActive(Actor, 0)==GE_FALSE) && m_JumpActive)
		{
			SetJump();
			m_JumpActive = false;
		}

		if(!strcmp(Motion, ANIMJUMP) && (CCD->ActorManager()->ForceActive(Actor, 0)==GE_FALSE) && !m_JumpActive)
			CCD->ActorManager()->SetMotion(Actor, ANIMFALL);
		
		if(strcmp(Motion, ANIMFALL) && (CCD->ActorManager()->Falling(Actor)==GE_TRUE))
		{
			if(CCD->ActorManager()->ReallyFall(Actor)==RGF_SUCCESS || !strcmp(Motion, ANIMJUMP))
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMFALL);
				CCD->ActorManager()->SetAnimationHeight(Actor, ANIMFALL, false);
			}
		}
		
		if(!strcmp(Motion, ANIMFALL) && (CCD->ActorManager()->Falling(Actor)==GE_FALSE))
		{
			m_JumpActive = false;
			if(m_crouch)
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMCLAND);
				CCD->ActorManager()->SetNextMotion(Actor, ANIMCIDLE);
				CCD->ActorManager()->SetAnimationHeight(Actor, ANIMCIDLE, true);
			}
			else
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMLAND);
				CCD->ActorManager()->SetNextMotion(Actor, ANIMIDLE);
				CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
			}
		}

		if(!strcmp(Motion, ANIMIDLE) && m_crouch)
		{
			CCD->ActorManager()->SetMotion(Actor, ANIMCIDLE);
			CCD->ActorManager()->SetAnimationHeight(Actor, ANIMCIDLE, true);
		}

		if((!strcmp(Motion, ANIMCIDLE) || !strcmp(Motion, ANIMC2IDLE))&& !m_crouch)
		{
			if(CCD->ActorManager()->CheckAnimation(Actor, ANIMIDLE)==GE_TRUE)
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMIDLE);
				CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
			}
		}
		break;
	case MOVEWALKFORWARD:
	case MOVEWALKBACKWARD:
		if(!strcmp(Motion, ANIMIDLE) || !strcmp(Motion, ANIMW2IDLE))
		{
			if(!m_run)
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMI2WALK);
				CCD->ActorManager()->SetNextMotion(Actor, ANIMWALK);
			}
			else
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMI2RUN);
				CCD->ActorManager()->SetNextMotion(Actor, ANIMRUN);
			}
		}
		
		if((!strcmp(Motion, ANIMI2WALK) || !strcmp(Motion, ANIMWALK)
			|| !strcmp(Motion, ANIMI2RUN) || !strcmp(Motion, ANIMRUN)) && m_crouch)
		{
			CCD->ActorManager()->SetAnimationHeight(Actor, ANIMCIDLE, true);
			CCD->ActorManager()->SetMotion(Actor, ANIMCRAWL);
		}

		if(!strcmp(Motion, ANIMCIDLE) || !strcmp(Motion, ANIMC2IDLE))
			CCD->ActorManager()->SetMotion(Actor, ANIMCRAWL);

		if(!strcmp(Motion, ANIMCRAWL) && !m_crouch)
		{
			if(CCD->ActorManager()->CheckAnimation(Actor, ANIMIDLE)==GE_TRUE)
			{
				CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
				if(!m_run)
					CCD->ActorManager()->SetMotion(Actor, ANIMWALK);
				else
					CCD->ActorManager()->SetMotion(Actor, ANIMRUN);
			}
		}

		if(!strcmp(Motion, ANIMSLIDELEFT) || !strcmp(Motion, ANIMRUNSLIDELEFT)
			|| !strcmp(Motion, ANIMSLIDERIGHT) || !strcmp(Motion, ANIMRUNSLIDERIGHT))
		{
			if(!m_run)
				CCD->ActorManager()->SetMotion(Actor, ANIMWALK);
			else
				CCD->ActorManager()->SetMotion(Actor, ANIMRUN);
		}
		
		if(!strcmp(Motion, ANIMSLIDECLEFT) || !strcmp(Motion, ANIMSLIDECRIGHT))
		{
			CCD->ActorManager()->SetMotion(Actor, ANIMCRAWL);
		}

		if(!strcmp(Motion, ANIMWALK) && m_run)
			CCD->ActorManager()->SetMotion(Actor, ANIMRUN);
		if(!strcmp(Motion, ANIMRUN) && !m_run)
			CCD->ActorManager()->SetMotion(Actor, ANIMWALK);

		if(strcmp(Motion, ANIMFALL) && (CCD->ActorManager()->Falling(Actor)==GE_TRUE))
		{
			if(CCD->ActorManager()->ReallyFall(Actor)==RGF_SUCCESS || !strcmp(Motion, ANIMJUMP))
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMFALL);
				CCD->ActorManager()->SetAnimationHeight(Actor, ANIMFALL, false);
			}
		}
		
		if(!strcmp(Motion, ANIMFALL) && (CCD->ActorManager()->Falling(Actor)==GE_FALSE))
		{
			if(!m_JumpActive)
			{
				if(m_crouch)
				{
					CCD->ActorManager()->SetAnimationHeight(Actor, ANIMCIDLE, true);
					CCD->ActorManager()->SetMotion(Actor, ANIMCRAWL);
				}
				else
				{
					CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
					if(!m_run)
						CCD->ActorManager()->SetMotion(Actor, ANIMWALK);
					else
						CCD->ActorManager()->SetMotion(Actor, ANIMRUN);
				}
			}
			else
			{
				if(CCD->ActorManager()->CheckAnimation(Actor, ANIMJUMP)==GE_TRUE)
				{
					CCD->ActorManager()->SetAnimationHeight(Actor, ANIMJUMP, false);
					CCD->ActorManager()->SetMotion(Actor, ANIMJUMP);
					SetJump();
				}
				m_JumpActive = false;
			}
		}
				
		if(m_JumpActive && strcmp(Motion, ANIMJUMP) && strcmp(Motion, ANIMFALL) && (CCD->ActorManager()->ForceActive(Actor, 0)==GE_FALSE))
		{
			if(CCD->ActorManager()->CheckAnimation(Actor, ANIMJUMP)==GE_TRUE)
			{
				CCD->ActorManager()->SetAnimationHeight(Actor, ANIMJUMP, false);
				CCD->ActorManager()->SetMotion(Actor, ANIMJUMP);
				SetJump();
			}
			m_JumpActive = false;
		}
		
		if(!strcmp(Motion, ANIMJUMP) || !strcmp(Motion, ANIMFALL))
			m_JumpActive = false;

		break;
	case MOVESLIDERIGHT:
		m_JumpActive = false;
		if(!strcmp(Motion, ANIMIDLE) || !strcmp(Motion, ANIMW2IDLE)
			|| !strcmp(Motion, ANIMWALK) || !strcmp(Motion, ANIMI2WALK)
			|| !strcmp(Motion, ANIMRUN) || !strcmp(Motion, ANIMI2RUN)
			|| !strcmp(Motion, ANIMSLIDELEFT) || !strcmp(Motion, ANIMRUNSLIDELEFT))
		{
			if(!m_run)
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMSLIDERIGHT);
			}
			else
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMRUNSLIDERIGHT);
			}
		}
		
		if(!strcmp(Motion, ANIMCIDLE) || !strcmp(Motion, ANIMC2IDLE)
			|| !strcmp(Motion, ANIMCRAWL))
		{
			CCD->ActorManager()->SetMotion(Actor, ANIMSLIDECRIGHT);
		}

		if((!strcmp(Motion, ANIMSLIDERIGHT) || !strcmp(Motion, ANIMRUNSLIDERIGHT)) && m_crouch)
		{
			CCD->ActorManager()->SetMotion(Actor, ANIMSLIDECRIGHT);
			CCD->ActorManager()->SetAnimationHeight(Actor, ANIMCIDLE, true);
		}

		if(!strcmp(Motion, ANIMSLIDECRIGHT) && !m_crouch)
		{
			if(CCD->ActorManager()->CheckAnimation(Actor, ANIMIDLE)==GE_TRUE)
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMSLIDERIGHT);
				CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
			}
		}

		if(!strcmp(Motion, ANIMSLIDERIGHT) && m_run)
		{
			CCD->ActorManager()->SetMotion(Actor, ANIMRUNSLIDERIGHT);
		}
		if(!strcmp(Motion, ANIMRUNSLIDERIGHT) && !m_run)
		{
			CCD->ActorManager()->SetMotion(Actor, ANIMSLIDERIGHT);
		}
		break;
	case MOVESLIDELEFT:
		m_JumpActive = false;
		if(!strcmp(Motion, ANIMIDLE) || !strcmp(Motion, ANIMW2IDLE) 
			|| !strcmp(Motion, ANIMWALK) || !strcmp(Motion, ANIMI2WALK) 
			|| !strcmp(Motion, ANIMRUN) || !strcmp(Motion, ANIMI2RUN) 
			|| !strcmp(Motion, ANIMSLIDERIGHT) || !strcmp(Motion, ANIMRUNSLIDERIGHT))
		{
			if(!m_run)
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMSLIDELEFT);
			}
			else
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMRUNSLIDELEFT);
			}
		}
		
				
		if(!strcmp(Motion, ANIMCIDLE) || !strcmp(Motion, ANIMC2IDLE)
			|| !strcmp(Motion, ANIMCRAWL))
		{
			CCD->ActorManager()->SetMotion(Actor, ANIMSLIDECLEFT);
		}

		if((!strcmp(Motion, ANIMSLIDELEFT) || !strcmp(Motion, ANIMRUNSLIDELEFT)) && m_crouch)
		{
			CCD->ActorManager()->SetMotion(Actor, ANIMSLIDECLEFT);
			CCD->ActorManager()->SetAnimationHeight(Actor, ANIMCIDLE, true);
		}

		if(!strcmp(Motion, ANIMSLIDECLEFT) && !m_crouch)
		{
			if(CCD->ActorManager()->CheckAnimation(Actor, ANIMIDLE)==GE_TRUE)
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMSLIDELEFT);
				CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
			}
		}

		if(!strcmp(Motion, ANIMSLIDELEFT) && m_run)
		{
			CCD->ActorManager()->SetMotion(Actor, ANIMRUNSLIDELEFT);
		}
		if(!strcmp(Motion, ANIMRUNSLIDELEFT) && !m_run)
		{
			CCD->ActorManager()->SetMotion(Actor, ANIMSLIDELEFT);
		}
		break;
		
	}

}


//	ViewPoint
//
//	Compute the players viewpoint and return it as a transform

geXForm3d CPlayer::ViewPoint()
{
	geXForm3d theViewPoint;
	geVec3d Translation;
	geVec3d Rotation;
	
	geXForm3d_SetIdentity(&theViewPoint); // Clear the matrix
	CCD->ActorManager()->GetPosition(Actor, &Translation);
	CCD->ActorManager()->GetRotation(Actor, &Rotation);
	
	geXForm3d_RotateZ(&theViewPoint, Rotation.Z); // Rotate then translate
	geXForm3d_RotateX(&theViewPoint, Rotation.X);
	geXForm3d_RotateY(&theViewPoint, Rotation.Y);
	
	geXForm3d_Translate(&theViewPoint, Translation.X, Translation.Y, Translation.Z);
	
	return theViewPoint;
}

//	GetExtBoxWS
//
//	Get the players ExtBox values (axially-aligned bounding box)
//	..in WORLD SPACE.

void CPlayer::GetExtBoxWS(geExtBox *theBox)
{
	CCD->ActorManager()->GetBoundingBox(Actor, theBox);
}

//	GetIn
//
//	Get the direction the player avatar is facing

void CPlayer::GetIn(geVec3d *In)
{
	CCD->ActorManager()->InVector(Actor, In);
}

void CPlayer::GetLeft(geVec3d *Left)
{
	CCD->ActorManager()->LeftVector(Actor, Left);
}

//	SaveTo
//
//	Save the current player state to an open file.

int CPlayer::SaveTo(FILE *SaveFD)
{
	fwrite(&m_JumpActive, sizeof(bool), 1, SaveFD);
	fwrite(&m_JumpSpeed, sizeof(geFloat), 1, SaveFD);
	fwrite(&m_CurrentSpeed, sizeof(geFloat), 1, SaveFD);
	fwrite(&m_Speed, sizeof(geFloat), 1, SaveFD);
	fwrite(&m_StepHeight, sizeof(geFloat), 1, SaveFD);
	fwrite(&m_Translation, sizeof(geVec3d), 1, SaveFD);
	fwrite(&m_Gravity, sizeof(geFloat), 1, SaveFD);
	fwrite(&m_LastMovementSpeed, sizeof(geFloat), 1, SaveFD);
	fwrite(&m_FirstPersonView, sizeof(bool), 1, SaveFD);
	
	fwrite(&m_LookMode, sizeof(bool), 1, SaveFD);
	fwrite(&m_Moving, sizeof(int), 1, SaveFD);
	fwrite(&m_PositionHistory, sizeof(geVec3d), 50, SaveFD);
	fwrite(&m_PositionHistoryPtr, sizeof(int), 1, SaveFD);
	fwrite(&m_Scale, sizeof(geFloat), 1, SaveFD);
	
	fwrite(&m_SlideWalk, sizeof(int), 1, SaveFD);
	fwrite(&m_crouch, sizeof(bool), 1, SaveFD);
	
	//	If we have attributes, save them.
	
	bool bAttributes = false;
	
	m_Attr = CCD->ActorManager()->Inventory(Actor);

	if(m_Attr)
	{
		bAttributes = true;
		fwrite(&bAttributes, sizeof(bool), 1, SaveFD);
		m_Attr->SaveTo(SaveFD);
	}
	else
		fwrite(&bAttributes, sizeof(bool), 1, SaveFD);
	
	return RGF_SUCCESS;
}

//	RestoreFrom
//
//	Restore the player state from an open file.

int CPlayer::RestoreFrom(FILE *RestoreFD)
{
	fread(&m_JumpActive, sizeof(bool), 1, RestoreFD);
	fread(&m_JumpSpeed, sizeof(geFloat), 1, RestoreFD);
	fread(&m_CurrentSpeed, sizeof(geFloat), 1, RestoreFD);
	fread(&m_Speed, sizeof(geFloat), 1, RestoreFD);
	fread(&m_StepHeight, sizeof(geFloat), 1, RestoreFD);
	fread(&m_Translation, sizeof(geVec3d), 1, RestoreFD);
	fread(&m_Gravity, sizeof(geFloat), 1, RestoreFD);
	fread(&m_LastMovementSpeed, sizeof(geFloat), 1, RestoreFD);
	fread(&m_FirstPersonView, sizeof(bool), 1, RestoreFD);
	
	fread(&m_LookMode, sizeof(bool), 1, RestoreFD);
	fread(&m_Moving, sizeof(int), 1, RestoreFD);
	fread(&m_PositionHistory, sizeof(geVec3d), 50, RestoreFD);
	fread(&m_PositionHistoryPtr, sizeof(int), 1, RestoreFD);
	fread(&m_Scale, sizeof(geFloat), 1, RestoreFD);
	
	fread(&m_SlideWalk, sizeof(int), 1, RestoreFD);
	fread(&m_crouch, sizeof(bool), 1, RestoreFD);
	
	bool bAttributes;
	fread(&bAttributes, sizeof(bool), 1, RestoreFD);
	if(bAttributes)
	{
		m_Attr = CCD->ActorManager()->Inventory(Actor);
		m_Attr->RestoreFrom(RestoreFD);
	}
	
	return RGF_SUCCESS;
}

//	LookMode
//
//	If in third-person mode, set flags such that (temporarily) the
//	..camera locks into first-person perspective, although movement
//	..is NOT permitted while in LookMode.

void CPlayer::LookMode(bool bLookOn)
{

	//	First, this is effective ONLY if we're in third-person mode..
	
	if(m_FirstPersonView)
		return;								// Ignore the call.
	
	if(bLookOn == m_LookMode)
		return;								// Mode already set.
	
	if(bLookOn)
	{
		m_LookMode = true;
		geWorld_SetActorFlags(CCD->World(), Actor, GE_ACTOR_COLLIDE | GE_ACTOR_RENDER_MIRRORS);
		geExtBox theBox;
		CCD->ActorManager()->GetBoundingBox(Actor, &theBox);
		CCD->CameraManager()->GetCameraOffset(&LookPosition, 
			&LookRotation);
		LookFlags = CCD->CameraManager()->GetTrackingFlags();
		CCD->CameraManager()->SetTrackingFlags(kCameraTrackPosition);
		geVec3d theRotation = {0.0f, 0.0f, 0.0f};
		geVec3d theTranslation = {0.0f, 0.0f, 0.0f};
		theTranslation.Y = theBox.Max.Y*0.75f;
		CCD->CameraManager()->SetCameraOffset(theTranslation, theRotation);	
	}
	else
	{
		m_LookMode = false;
		geWorld_SetActorFlags(CCD->World(), Actor, 
			GE_ACTOR_RENDER_NORMAL | GE_ACTOR_COLLIDE | GE_ACTOR_RENDER_MIRRORS);
		CCD->CameraManager()->SetTrackingFlags(LookFlags);
		CCD->CameraManager()->SetCameraOffset(LookPosition, LookRotation);
	}
	
	return;
}

//	SwitchToFirstPerson
//
//	If the player is not in a first-person view already, switch
//	..to first-person mode.

void CPlayer::SwitchToFirstPerson()
{ 
	m_FirstPersonView = true;
	m_LookMode = false;
	geWorld_SetActorFlags(CCD->World(), Actor, GE_ACTOR_COLLIDE);
	
	
	return;
}

//	SwitchToThirdPerson
//
//	If the player is not in a third-person view already, switch
//	..to third-person mode.

void CPlayer::SwitchToThirdPerson()
{ 
	m_FirstPersonView = false;
	m_LookMode = false;
	geWorld_SetActorFlags(CCD->World(), Actor, 
		GE_ACTOR_RENDER_NORMAL | GE_ACTOR_COLLIDE | GE_ACTOR_RENDER_MIRRORS);
	return;
}

//	SaveAttributes
//
//	Save off all attributes and inventory to a file.  Mainly used to
//	..preserve player data across level changes.

int CPlayer::SaveAttributes(char *szSaveFile)
{
	FILE *theFile = CCD->OpenRFFile(kInstallFile, szSaveFile, "wb");
	
	if(theFile == NULL)
	{
		char szError[256];
		sprintf(szError,"Can't SaveAttributes to '%s'", szSaveFile);
		CCD->ReportError(szError, false);
		return RGF_FAILURE;
	}
	
	m_Attr = CCD->ActorManager()->Inventory(Actor);
	m_Attr->SaveTo(theFile);
	
	fclose(theFile);
	
	return RGF_SUCCESS;
}

//	LoadAttributes
//
//	Load all attributes and inventory from a file.  Mainly used to
//	..restore player data after a level change.

int CPlayer::LoadAttributes(char *szSaveFile)
{
	FILE *theFile = CCD->OpenRFFile(kInstallFile, szSaveFile, "rb");
	
	if(theFile == NULL)
	{
		char szError[256];
		sprintf(szError,"Can't LoadAttributes from '%s'", szSaveFile);
		CCD->ReportError(szError, false);
		return RGF_FAILURE;
	}

	m_Attr = CCD->ActorManager()->Inventory(Actor);
	m_Attr->RestoreFrom(theFile);
	
	fclose(theFile);
	
	return RGF_SUCCESS;
}

//	Backtrack
//
//	Move player to the previous position in the position history.

void CPlayer::Backtrack()
{
	m_PositionHistoryPtr--;
	
	if(m_PositionHistoryPtr < 0)
		m_PositionHistoryPtr = 49;
	
	m_Translation = m_PositionHistory[m_PositionHistoryPtr];
	
	return;
}

//	AddPosition
//
//	Add the players current position to the position history ring buffer

void CPlayer::AddPosition()
{
	m_PositionHistoryPtr++;
	
	if(m_PositionHistoryPtr >= 50)
		m_PositionHistoryPtr = 0;
	
	m_PositionHistory[m_PositionHistoryPtr] = m_Translation; 
	
	return;
}

//	LoadAudioClip
//
//	Load an audio clip and return the geSound pointer

geSound_Def *CPlayer::LoadAudioClip(char *szFilename)
{
	if(!EffectC_IsStringNull(szFilename))
	{
		geSound_Def *SoundPointer = SPool_Sound(szFilename);
		return SoundPointer;
	}
	return NULL;

}

//	ShowFog
//
//	If fogging is enabled for this player, activate it.

void CPlayer::ShowFog()
{ 
	CCD->Engine()->EnableFog(m_FogActive);
	
	return;
}

//	DisableFog
//
//	Unconditionally disable fog for this player.

void CPlayer::DisableFog()
{ 
	CCD->Engine()->EnableFog(GE_FALSE);
	
	return;
}

//	ActivateClipPlane
//
//	Activate the far clip plane, if used.

void CPlayer::ActivateClipPlane()
{
	CCD->CameraManager()->EnableFarClipPlane(m_ClipActive);
	
	return;
}

//	DisableClipPlane
//
//	Unconditionally disable the far clip plane.

void CPlayer::DisableClipPlane()
{
	CCD->CameraManager()->EnableFarClipPlane(GE_FALSE);
	
	return;
}

void CPlayer::SetCrouch(bool value)
{ 
	m_crouch = value;
	
}

bool CPlayer::DoMovements()
{
	//	Ok, if the player moved, handle any processing necessary to reflect
	//	..that fact in the world.
	//	03/22/2000 eaa3 Added Ralph Deane bug fixes.

	bool bPlayerMoved = false;
	m_CoeffSpeed = 1.0f;		//Coef de course
	int theZone;
	geFloat fTemp;

	CCD->ActorManager()->GetActorZone(Actor, &theZone);
	if(theZone & kWaterZone)
		m_CoeffSpeed = 0.6f;
	else if(theZone & kLavaZone)
		m_CoeffSpeed = 0.55f;
	else if(theZone & kSlowMotionZone)
		m_CoeffSpeed = 0.25f;
	else if(theZone & kFastMotionZone)
		m_CoeffSpeed = 2.0f;

	//A little run wanted ?
	if ((m_Running) && !GetCrouch())		//Don't run when crouch
	{
		if(CCD->ActorManager()->Falling(Actor) != GE_TRUE)	//If we are not falling, adjust speed modifications
			m_CoeffSpeed *= 2.0f;
	}

	//Want a jump ?
	if (m_Jumping)
		StartJump();

	if(GetMoving()!=MOVEIDLE)
	{
		switch(GetMoving())
		{
		case MOVEWALKFORWARD:
			fTemp = Speed() * ((CCD->LastElapsedTime_F()/20.0f) * m_CoeffSpeed);
			if(GetSlideWalk()==MOVESLIDELEFT)
				Move(RGF_K_LEFT, fTemp);
			if(GetSlideWalk()==MOVESLIDERIGHT)
				Move(RGF_K_RIGHT, fTemp);
			Move(RGF_K_FORWARD, fTemp);
			bPlayerMoved = true;
			break;
		case MOVEWALKBACKWARD:
			fTemp = Speed() * ((CCD->LastElapsedTime_F()/20.0f) * (m_CoeffSpeed-0.33f));	//Some hard coding. todo : make configurable
			if(GetSlideWalk()==MOVESLIDELEFT)
				Move(RGF_K_LEFT, fTemp);
			if(GetSlideWalk()==MOVESLIDERIGHT)
				Move(RGF_K_RIGHT, fTemp);
			Move(RGF_K_BACKWARD, fTemp);
			bPlayerMoved = true;
			break;
		case MOVESLIDELEFT:
			fTemp = Speed() * ((CCD->LastElapsedTime_F()/20.0f) * (m_CoeffSpeed-0.25f));	//Some hard coding. todo : make configurable
			Move(RGF_K_LEFT, fTemp);
			if(GetSlideWalk()==MOVEWALKFORWARD)
			{
				Move(RGF_K_FORWARD, fTemp);
				Moving(MOVEWALKFORWARD);
			}
			if(GetSlideWalk()==MOVEWALKBACKWARD)
			{
				Move(RGF_K_BACKWARD, fTemp);
				Moving(MOVEWALKBACKWARD);
			}
			bPlayerMoved = true;
			break;
		case MOVESLIDERIGHT:
			fTemp = Speed() * ((CCD->LastElapsedTime_F()/20.0f) * (m_CoeffSpeed-0.25f));	//Some hard coding. todo : make configurable
			Move(RGF_K_RIGHT, fTemp);
			if(GetSlideWalk()==MOVEWALKFORWARD)
			{
				Move(RGF_K_FORWARD, fTemp);
				Moving(MOVEWALKFORWARD);
			}
			if(GetSlideWalk()==MOVEWALKBACKWARD)
			{
				Move(RGF_K_BACKWARD, fTemp);
				Moving(MOVEWALKBACKWARD);
			}
			bPlayerMoved = true;
			break;
		default:
			break;
		}
		
	}
	else
		Move(RGF_K_FORWARD, 0);

	return bPlayerMoved;
}
