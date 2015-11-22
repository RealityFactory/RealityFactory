/*
CPlayer.cpp:		Player character encapsulation class

  (c) 2001 Ralph Deane
  
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
	m_PlayerViewPoint = FIRSTPERSON;
	m_Gravity = -4.6f;
	m_Running = m_run = false;
	m_Scale = 1.0f;
	m_lastdirection = 0;
	Alive = true;
	
	m_oldcrouch = m_crouch = GE_FALSE;
	
	m_Moving = MOVEIDLE;
	m_SlideWalk = MOVEIDLE;
	
	EnvAudio = NULL;
	lighton = false;
	lightactive = false;
	lighteffect = -1;
	LiteDecayTime = 0.0f;
	LiteTime = 0.0f;
	CurrentLiteLife = -1.0f;
	StaminaDelay = 1.0f;
	StaminaTime = 0.0f;
	Allow3rdLook = true;
	LastHealth = -1;
	Injured = false;
	Dying = false;
	OldFalling = Falling = FallInjure = false;
	MinFallDist = MaxFallDist = FallDamage = 0.0f;
// changed RF063
	UseAttribute[0] = '\0';
	RealJumping = false;
	LiquidTime = 0.0f;
	InLiquid = -1;
	SwimSound = -1;
	SurfaceSound = -1;
	InLiquidSound = -1;
// end change RF063
	DefaultMotion[0] = NULL;
	DefaultMotion[1] = NULL;
	DefaultMotion[2] = NULL;
	
	for(int nTemp = 0; nTemp < 16; nTemp++)
		for(int j=0;j<3;j++)
			Contents[nTemp][j] = NULL;
		
		CIniFile AttrFile("playersetup.ini");
		if(!AttrFile.ReadFile())
		{
			CCD->ReportError("Can't open playersetup initialization file", false);
			return;
		}
		CString KeyName = AttrFile.FindFirstKey();
		CString Type, Vector;
		char szName[64];
		while(KeyName != "")
		{
			if(KeyName=="Animations")
			{
				Type = AttrFile.GetValue(KeyName, "idle");
				strcpy(Animations[IDLE],Type);
				Type = AttrFile.GetValue(KeyName, "idletowalk");
				strcpy(Animations[I2WALK],Type);
				Type = AttrFile.GetValue(KeyName, "idletorun");
				strcpy(Animations[I2RUN],Type);
				Type = AttrFile.GetValue(KeyName, "walk");
				strcpy(Animations[WALK],Type);
				Type = AttrFile.GetValue(KeyName, "walktoidle");
				strcpy(Animations[W2IDLE],Type);
				Type = AttrFile.GetValue(KeyName, "run");
				strcpy(Animations[RUN],Type);
				Type = AttrFile.GetValue(KeyName, "jump");
				strcpy(Animations[JUMP],Type);
				Type = AttrFile.GetValue(KeyName, "starttojump");
				strcpy(Animations[STARTJUMP],Type);
				Type = AttrFile.GetValue(KeyName, "fall");
				strcpy(Animations[FALL],Type);
				Type = AttrFile.GetValue(KeyName, "land");
				strcpy(Animations[LAND],Type);
				Type = AttrFile.GetValue(KeyName, "slidetoleft");
				strcpy(Animations[SLIDELEFT],Type);
				Type = AttrFile.GetValue(KeyName, "slideruntoleft");
				strcpy(Animations[RUNSLIDELEFT],Type);
				Type = AttrFile.GetValue(KeyName, "slidetoright");
				strcpy(Animations[SLIDERIGHT],Type);
				Type = AttrFile.GetValue(KeyName, "slideruntoright");
				strcpy(Animations[RUNSLIDERIGHT],Type);
				Type = AttrFile.GetValue(KeyName, "crawl");
				strcpy(Animations[CRAWL],Type);
				Type = AttrFile.GetValue(KeyName, "crouchidle");
				strcpy(Animations[CIDLE],Type);
				Type = AttrFile.GetValue(KeyName, "crawltoidle");
				strcpy(Animations[C2IDLE],Type);
				Type = AttrFile.GetValue(KeyName, "crouchstarttojump");
				strcpy(Animations[CSTARTJUMP],Type);
				Type = AttrFile.GetValue(KeyName, "crouchland");
				strcpy(Animations[CLAND],Type);
				Type = AttrFile.GetValue(KeyName, "crawlslidetoleft");
				strcpy(Animations[SLIDECLEFT],Type);
				Type = AttrFile.GetValue(KeyName, "crawlslidetoright");
				strcpy(Animations[SLIDECRIGHT],Type);
				
				Type = AttrFile.GetValue(KeyName, "shootup");
				strcpy(Animations[SHOOT1],Type);
				Type = AttrFile.GetValue(KeyName, "shootdwn");
				strcpy(Animations[SHOOT],Type);
				Type = AttrFile.GetValue(KeyName, "aimup");
				strcpy(Animations[AIM1],Type);
				Type = AttrFile.GetValue(KeyName, "aimdwn");
				strcpy(Animations[AIM],Type);
				Type = AttrFile.GetValue(KeyName, "walkshootup");
				strcpy(Animations[WALKSHOOT1],Type);
				Type = AttrFile.GetValue(KeyName, "walkshootdwn");
				strcpy(Animations[WALKSHOOT],Type);
				Type = AttrFile.GetValue(KeyName, "runshootup");
				strcpy(Animations[RUNSHOOT1],Type);
				Type = AttrFile.GetValue(KeyName, "runshootdwn");
				strcpy(Animations[RUNSHOOT],Type);
				Type = AttrFile.GetValue(KeyName, "slidetoleftshootup");
				strcpy(Animations[SLIDELEFTSHOOT1],Type);
				Type = AttrFile.GetValue(KeyName, "slidetoleftshootdwn");
				strcpy(Animations[SLIDELEFTSHOOT],Type);
				Type = AttrFile.GetValue(KeyName, "slideruntoleftshootup");
				strcpy(Animations[RUNSLIDELEFTSHOOT1],Type);
				Type = AttrFile.GetValue(KeyName, "slideruntoleftshootdwn");
				strcpy(Animations[RUNSLIDELEFTSHOOT],Type);
				Type = AttrFile.GetValue(KeyName, "slidetorightshootup");
				strcpy(Animations[SLIDERIGHTSHOOT1],Type);
				Type = AttrFile.GetValue(KeyName, "slidetorightshootdwn");
				strcpy(Animations[SLIDERIGHTSHOOT],Type);
				Type = AttrFile.GetValue(KeyName, "slideruntorightshootup");
				strcpy(Animations[RUNSLIDERIGHTSHOOT1],Type);
				Type = AttrFile.GetValue(KeyName, "slideruntorightshootdwn");
				strcpy(Animations[RUNSLIDERIGHTSHOOT],Type);
				Type = AttrFile.GetValue(KeyName, "jumpshootup");
				strcpy(Animations[JUMPSHOOT1],Type);
				Type = AttrFile.GetValue(KeyName, "jumpshootdwn");
				strcpy(Animations[JUMPSHOOT],Type);
				Type = AttrFile.GetValue(KeyName, "fallshootup");
				strcpy(Animations[FALLSHOOT1],Type);
				Type = AttrFile.GetValue(KeyName, "fallshootdwn");
				strcpy(Animations[FALLSHOOT],Type);
				Type = AttrFile.GetValue(KeyName, "crouchaimup");
				strcpy(Animations[CAIM1],Type);
				Type = AttrFile.GetValue(KeyName, "crouchaimdwn");
				strcpy(Animations[CAIM],Type);
				Type = AttrFile.GetValue(KeyName, "crouchshootup");
				strcpy(Animations[CSHOOT1],Type);
				Type = AttrFile.GetValue(KeyName, "crouchshootdwn");
				strcpy(Animations[CSHOOT],Type);
				Type = AttrFile.GetValue(KeyName, "crawlshootup");
				strcpy(Animations[CRAWLSHOOT1],Type);
				Type = AttrFile.GetValue(KeyName, "crawlshootdwn");
				strcpy(Animations[CRAWLSHOOT],Type);
				Type = AttrFile.GetValue(KeyName, "crawlslidetoleftshootup");
				strcpy(Animations[SLIDECLEFTSHOOT1],Type);
				Type = AttrFile.GetValue(KeyName, "crawlslidetoleftshootdwn");
				strcpy(Animations[SLIDECLEFTSHOOT],Type);
				Type = AttrFile.GetValue(KeyName, "crawlslidetorightshootup");
				strcpy(Animations[SLIDECRIGHTSHOOT1],Type);
				Type = AttrFile.GetValue(KeyName, "crawlslidetorightshootdwn");
				strcpy(Animations[SLIDECRIGHTSHOOT],Type);
// changed RF063
				Type = AttrFile.GetValue(KeyName, "swim");
				strcpy(Animations[SWIM],Type);
				Type = AttrFile.GetValue(KeyName, "treadwater");
				strcpy(Animations[TREADWATER],Type);
// end change RF063				
				Type = AttrFile.GetValue(KeyName, "die");
				char strip[256], *temp;
				int i = 0;
				DieAnimAmt = 0;
// changed RF063
				if(Type!="")
				{
					strcpy(strip,Type);
					temp = strtok(strip," \n");
					while(temp)
					{
						strcpy(DieAnim[i],temp);
						i+=1;
						DieAnimAmt = i;
						if(i==5)
							break;
						temp = strtok(NULL," \n");
					}
				}
				Type = AttrFile.GetValue(KeyName, "injury");
				i = 0;
				InjuryAnimAmt = 0;
				if(Type!="")
				{
					strcpy(strip,Type);
					temp = strtok(strip," \n");
					while(temp)
					{
						strcpy(InjuryAnim[i],temp);
						i+=1;
						InjuryAnimAmt = i;
						if(i==5)
							break;
						temp = strtok(NULL," \n");
					}
				}
// end change RF063
		}
		if(KeyName=="Sounds")
		{
			Type = AttrFile.GetValue(KeyName, "die");
			char strip[256], *temp;
			int i = 0;
			DieSoundAmt = 0;
			strcpy(strip,Type);
			temp = strtok(strip," \n");
			while(temp)
			{
				DieSound[i]=SPool_Sound(temp);
				i+=1;
				DieSoundAmt = i;
				if(i==5)
					break;
				temp = strtok(NULL," \n");
			}
			Type = AttrFile.GetValue(KeyName, "injury");
			i = 0;
			InjurySoundAmt = 0;
			strcpy(strip,Type);
			temp = strtok(strip," \n");
			while(temp)
			{
				InjurySound[i]=SPool_Sound(temp);
				i+=1;
				InjurySoundAmt = i;
				if(i==5)
					break;
				temp = strtok(NULL," \n");
			}
			Type = AttrFile.GetValue(KeyName, "land");
			i = 0;
			LandSoundAmt = 0;
			strcpy(strip,Type);
			temp = strtok(strip," \n");
			while(temp)
			{
				LandSound[i]=SPool_Sound(temp);
				i+=1;
				LandSoundAmt = i;
				if(i==5)
					break;
				temp = strtok(NULL," \n");
			}
		}
		if(KeyName=="Light")
		{
			geVec3d convert;
			LiteIntensity = (float)AttrFile.GetValueF(KeyName, "intensity");
			LiteRadiusMin = (float)AttrFile.GetValueF(KeyName, "minimumradius");
			LiteRadiusMax = (float)AttrFile.GetValueF(KeyName, "maximumradius");
			LiteLife = (float)AttrFile.GetValueF(KeyName, "lifetime");
			LiteDecay = (float)AttrFile.GetValueF(KeyName, "decaytime");
			Vector = AttrFile.GetValue(KeyName, "minimumcolor");
			if(Vector!="")
			{
				strcpy(szName,Vector);
				convert = Extract(szName);
				LiteColorMin.r = convert.X;
				LiteColorMin.g = convert.Y;
				LiteColorMin.b = convert.Z;
			}
			Vector = AttrFile.GetValue(KeyName, "maximumcolor");
			if(Vector!="")
			{
				strcpy(szName,Vector);
				convert = Extract(szName);
				LiteColorMax.r = convert.X;
				LiteColorMax.g = convert.Y;
				LiteColorMax.b = convert.Z;
			}
			LiteBone[0] = '\0';
			Type = AttrFile.GetValue(KeyName, "attachtobone");
			if(Type!="")
				strcpy(LiteBone,Type);
		}
		if(KeyName=="Misc")
		{
			strcpy(StaminaName,"stamina");
			Type = AttrFile.GetValue(KeyName, "attributename");
			if(Type!="")
				strcpy(StaminaName,Type);
			StaminaDelay = (float)AttrFile.GetValueF(KeyName, "recoverytime");
			MinFallDist = (float)AttrFile.GetValueF(KeyName, "minimumfalldistance");
			MaxFallDist = (float)AttrFile.GetValueF(KeyName, "maximumfalldistance");
			FallDamage = (float)AttrFile.GetValueF(KeyName, "falldamage");
// changed RF063
			UseRange = (float)AttrFile.GetValueF(KeyName, "userange");
			BoxWidth = (float)AttrFile.GetValueF(KeyName, "boxsize");
// end change RF063
			geVec3d TFillColor = {255.0f, 255.0f, 255.0f};
			geVec3d TAmbientColor = {255.0f, 255.0f, 255.0f};
			Vector = AttrFile.GetValue(KeyName, "fillcolor");
			if(Vector!="")
			{
				strcpy(szName,Vector);
				TFillColor = Extract(szName);
			}
			Vector = AttrFile.GetValue(KeyName, "ambientcolor");
			if(Vector!="")
			{
				strcpy(szName,Vector);
				TAmbientColor = Extract(szName);
			}
			FillColor.r = TFillColor.X;
			FillColor.g = TFillColor.Y;
			FillColor.b = TFillColor.Z;
			FillColor.a = 255.0f;
			AmbientColor.r = TAmbientColor.X;
			AmbientColor.g = TAmbientColor.Y;
			AmbientColor.b = TAmbientColor.Z;
			AmbientColor.a = 255.0f;
		}
		
		KeyName = AttrFile.FindNextKey();
	};
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
		if(DefaultMotion[0] != NULL)
			geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), DefaultMotion[0]);
		if(DefaultMotion[1] != NULL)
			geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), DefaultMotion[1]);
		if(DefaultMotion[2] != NULL)
			geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), DefaultMotion[2]);
		for(int nTemp = 0; nTemp < 16; nTemp++)
		{
			for(int j=0;j<3;j++)
			{
				if(Contents[nTemp][j] != NULL)
					geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), Contents[nTemp][j]);
			}
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
// changed RF063
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

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
	pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL);
	PlayerSetup *pSetup = (PlayerSetup*)geEntity_GetUserData(pEntity);

	if(EffectC_IsStringNull(pSetup->ActorName))
		Actor = CCD->ActorManager()->LoadActor(szFile, NULL);
	else
		Actor = CCD->ActorManager()->LoadActor(pSetup->ActorName, NULL);

	if(!Actor)
	{
		char szError[256];
		if(EffectC_IsStringNull(pSetup->ActorName))
			sprintf(szError,"Missing Player Actor '%s'", szFile);
		else
			sprintf(szError,"Missing Player Actor '%s'", pSetup->ActorName);
		CCD->ReportError(szError, false);
		CCD->ShutdownLevel();
		delete CCD;
		MessageBox(NULL, szError,"Player", MB_OK);
		exit(-333);
	}
// end change RF063
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
	CCD->ActorManager()->SetDefaultMotion(Actor, Animations[IDLE]);
	CCD->ActorManager()->SetMotion(Actor, Animations[IDLE]);
	CCD->ActorManager()->SetScale(Actor, m_Scale*actscale);
	CCD->ActorManager()->SetBoundingBox(Actor, Animations[IDLE]);
	CCD->ActorManager()->SetType(Actor, ENTITY_ACTOR);
	CCD->ActorManager()->SetActorDynamicLighting(Actor, FillColor, AmbientColor);
	CCD->ActorManager()->SetShadow(Actor, pSetup->ShadowSize);
	
	geExtBox theBox;
	CCD->ActorManager()->GetBoundingBox(Actor, &theBox);
	m_CurrentHeight = theBox.Max.Y;
// changed RF063
	if(BoxWidth>0.0f)
		CCD->ActorManager()->SetBBox(Actor, BoxWidth, -m_CurrentHeight, BoxWidth);
// end change RF063
 
	int nFlags = 0;
	// Mode
	geVec3d theRotation = {0.0f, 0.0f, 0.0f};
	CCD->ActorManager()->Rotate(Actor, theRotation);
	geVec3d theTranslation = {0.0f, 0.0f, 0.0f};
	switch(m_PlayerViewPoint)
	{
	case FIRSTPERSON:
	default:
		nFlags = kCameraTrackPosition;
		theTranslation.Y = m_CurrentHeight*0.75f;
		if(CCD->CameraManager()->GetViewHeight()!=-1.0f)
			theTranslation.Y = CCD->CameraManager()->GetViewHeight()*0.75f*m_Scale;
		CCD->CameraManager()->SetCameraOffset(theTranslation, 
			theRotation);				// Set offset
		SwitchToFirstPerson();
		break;
	case THIRDPERSON:
		nFlags = kCameraTrackThirdPerson | kCameraTrackFree;
		theRotation.X = CCD->CameraManager()->GetPlayerAngleUp();
		theTranslation.Y = m_CurrentHeight;
		if(CCD->CameraManager()->GetPlayerHeight()!=-1.0f)
			theTranslation.Y = CCD->CameraManager()->GetPlayerHeight()*m_Scale;
		theTranslation.Z = CCD->CameraManager()->GetPlayerDistance();
		CCD->CameraManager()->SetCameraOffset(theTranslation, 
			theRotation);				// Set offset
		Allow3rdLook = CCD->CameraManager()->GetPlayerAllowLook();
		SwitchToThirdPerson();
		CCD->CameraManager()->ResetCamera();
		break;
	case ISOMETRIC:
		nFlags = kCameraTrackIso;
		theRotation.X = CCD->CameraManager()->GetIsoAngleUp();
		theRotation.Y = CCD->CameraManager()->GetIsoAngleAround();
		theTranslation.Y = m_CurrentHeight;
		if(CCD->CameraManager()->GetIsoHeight()!=-1.0f)
			theTranslation.Y = CCD->CameraManager()->GetIsoHeight()*m_Scale;
		theTranslation.Z = CCD->CameraManager()->GetIsoDistance();
		CCD->CameraManager()->SetCameraOffset(theTranslation, 
			theRotation);				// Set offset
		SwitchToThirdPerson();
		CCD->CameraManager()->ResetCamera();
		break;
	case FIXEDCAMERA:
		if(CCD->FixedCameras()->GetNumber()==0)
		{
			char szError[256];
			sprintf(szError,"No Fixed Cameras in Level");
			CCD->ReportError(szError, false);
			CCD->ShutdownLevel();
			delete CCD;
			MessageBox(NULL, szError,"Fixed Camera", MB_OK);
			exit(-333);
		}
		nFlags = kCameraTrackFixed;
		CCD->CameraManager()->Unbind();
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
	
	DefaultMotion[0] = NULL;
	DefaultMotion[1] = NULL;
	DefaultMotion[2] = NULL;
	DefaultMotionHandle = NULL;
	
	for(int nTemp = 0; nTemp < 16; nTemp++)
	{
		for(int j=0;j<3;j++)
			Contents[nTemp][j] = NULL;
		ContentsHandles[nTemp] = NULL;
	}
	
	DefaultMotion[0] = LoadAudioClip(EnvAudio->DefaultMotionSound);
	DefaultMotion[1] = LoadAudioClip(EnvAudio->DefaultMotionSound1);
	DefaultMotion[2] = LoadAudioClip(EnvAudio->DefaultMotionSound2);
	Contents[0][0] = LoadAudioClip(EnvAudio->SoundWhenInWater);
	Contents[0][1] = LoadAudioClip(EnvAudio->SoundWhenInWater1);
	Contents[0][2] = LoadAudioClip(EnvAudio->SoundWhenInWater2);
	Contents[1][0] = LoadAudioClip(EnvAudio->SoundWhenInLava);
	Contents[1][1] = LoadAudioClip(EnvAudio->SoundWhenInLava1);
	Contents[1][2] = LoadAudioClip(EnvAudio->SoundWhenInLava2);
	Contents[2][0] = LoadAudioClip(EnvAudio->SoundWhenInToxicGas);
	Contents[2][1] = LoadAudioClip(EnvAudio->SoundWhenInToxicGas1);
	Contents[2][2] = LoadAudioClip(EnvAudio->SoundWhenInToxicGas2);
	Contents[3][0] = LoadAudioClip(EnvAudio->SoundWhenInZeroG);
	Contents[3][1] = LoadAudioClip(EnvAudio->SoundWhenInZeroG1);
	Contents[3][2] = LoadAudioClip(EnvAudio->SoundWhenInZeroG2);
	Contents[4][0] = LoadAudioClip(EnvAudio->SoundWhenInFrozen);
	Contents[4][1] = LoadAudioClip(EnvAudio->SoundWhenInFrozen1);
	Contents[4][2] = LoadAudioClip(EnvAudio->SoundWhenInFrozen2);
	Contents[5][0] = LoadAudioClip(EnvAudio->SoundWhenInSludge);
	Contents[5][1] = LoadAudioClip(EnvAudio->SoundWhenInSludge1);
	Contents[5][2] = LoadAudioClip(EnvAudio->SoundWhenInSludge2);
	Contents[6][0] = LoadAudioClip(EnvAudio->SoundWhenInSlowMotion);
	Contents[6][1] = LoadAudioClip(EnvAudio->SoundWhenInSlowMotion1);
	Contents[6][2] = LoadAudioClip(EnvAudio->SoundWhenInSlowMotion2);
	Contents[7][0] = LoadAudioClip(EnvAudio->SoundWhenInFastMotion);
	Contents[7][1] = LoadAudioClip(EnvAudio->SoundWhenInFastMotion1);
	Contents[7][2] = LoadAudioClip(EnvAudio->SoundWhenInFastMotion2);
	Contents[8][0] = LoadAudioClip(EnvAudio->SoundWhenUsingLadders);
	Contents[8][1] = LoadAudioClip(EnvAudio->SoundWhenUsingLadders1);
	Contents[8][2] = LoadAudioClip(EnvAudio->SoundWhenUsingLadders2);
// changed RF063
	Contents[9][0] = LoadAudioClip(EnvAudio->SoundWhenInUnclimbable);
	Contents[9][1] = LoadAudioClip(EnvAudio->SoundWhenInUnclimbable1);
	Contents[9][2] = LoadAudioClip(EnvAudio->SoundWhenInUnclimbable2);
// end change RF063	
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
			CCD->CameraManager()->SetShakeMin(theState->MinShakeDist);	
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
		if(m_PlayerViewPoint==FIRSTPERSON)
		{
			CCD->CameraManager()->Rotate(theRotation);
		}
		
		CCD->ActorManager()->SetStepHeight(Actor, m_StepHeight);
		CCD->ActorManager()->SetAutoStepUp(Actor, true);
		CCD->ActorManager()->SetGravity(Actor, m_Gravity);			// Default gravity
		CCD->ActorManager()->Position(Actor, m_Translation);
		
		if(m_PlayerViewPoint==FIXEDCAMERA)
		{
			if(!CCD->FixedCameras()->GetFirstCamera())
			{
				char szError[256];
				sprintf(szError,"No Active Fixed Camera Can See Player");
				CCD->ReportError(szError, false);
				CCD->ShutdownLevel();
				delete CCD;
				MessageBox(NULL, szError,"Fixed Camera", MB_OK);
				exit(-333);
			}
		}
		CCD->CameraManager()->TrackMotion();
		
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
		if(keyrotate & 1) // is it to the top?
		{
			if(Allow3rdLook)
				CCD->CameraManager()->TiltXDown(UPDOWN_SPEED);
			CCD->ActorManager()->TiltDown(Actor, UPDOWN_SPEED);
		}
		if(keyrotate & 8 && Allow3rdLook) // is it to the bottom?
		{
			if(Allow3rdLook)
				CCD->CameraManager()->TiltXUp(UPDOWN_SPEED);
			CCD->ActorManager()->TiltUp(Actor, UPDOWN_SPEED);
		}
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
		else			// Third-person mode
		{
			if(m_PlayerViewPoint==THIRDPERSON)
			{
				if(CCD->CameraManager()->GetPlayerMouseRotation())
				{
					if(temppos.x > pos.x) // is it to the left?
						CCD->CameraManager()->CameraRotY(true, TURN_SPEED*57.0f);
					else if (temppos.x < pos.x) // is it to the right?
						CCD->CameraManager()->CameraRotY(false, TURN_SPEED*57.0f);
					if(temppos.y < pos.y) // is it to the top?
					{
						CCD->CameraManager()->ChangeDistance(false, UPDOWN_SPEED*57.0f);
					}
					else if (temppos.y > pos.y) // is it to the bottom?
					{
						CCD->CameraManager()->ChangeDistance(true, UPDOWN_SPEED*57.0f);
					}
				}
				else
				{
					// In third-person the camera handles all the hard work for us.
					if(temppos.x > pos.x) // is it to the left?
						CCD->ActorManager()->TurnLeft(Actor, TURN_SPEED);
					else if (temppos.x < pos.x) // is it to the right?
						CCD->ActorManager()->TurnRight(Actor, TURN_SPEED);
					if(temppos.y < pos.y) // is it to the top?
					{
						if(Allow3rdLook)
							CCD->CameraManager()->TiltXDown(UPDOWN_SPEED);
						CCD->ActorManager()->TiltDown(Actor, UPDOWN_SPEED);
					}
					else if (temppos.y > pos.y) // is it to the bottom?
					{
						if(Allow3rdLook)
							CCD->CameraManager()->TiltXUp(UPDOWN_SPEED);
						CCD->ActorManager()->TiltUp(Actor, UPDOWN_SPEED);
					}
				}
			}
			else if(m_PlayerViewPoint==ISOMETRIC)
			{
				// In third-person the camera handles all the hard work for us.
				if(temppos.x > pos.x) // is it to the left?
					CCD->ActorManager()->TurnLeft(Actor, TURN_SPEED);
				else if (temppos.x < pos.x) // is it to the right?
					CCD->ActorManager()->TurnRight(Actor, TURN_SPEED);
				if(temppos.y < pos.y) // is it to the top?
				{
					CCD->CameraManager()->TiltXDown(UPDOWN_SPEED);
					CCD->ActorManager()->TiltDown(Actor, UPDOWN_SPEED);
				}
				else if (temppos.y > pos.y) // is it to the bottom?
				{
					CCD->CameraManager()->TiltXUp(UPDOWN_SPEED);
					CCD->ActorManager()->TiltUp(Actor, UPDOWN_SPEED);
				}
			}
			else if(m_PlayerViewPoint==FIXEDCAMERA)
			{
				// In third-person the camera handles all the hard work for us.
				if(temppos.x > pos.x) // is it to the left?
					CCD->ActorManager()->TurnLeft(Actor, TURN_SPEED);
				else if (temppos.x < pos.x) // is it to the right?
					CCD->ActorManager()->TurnRight(Actor, TURN_SPEED);
				if(temppos.y < pos.y) // is it to the top?
				{
					CCD->ActorManager()->TiltDown(Actor, UPDOWN_SPEED);
				}
				else if (temppos.y > pos.y) // is it to the bottom?
				{
					CCD->ActorManager()->TiltUp(Actor, UPDOWN_SPEED);
				}
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
// changed RF063	
	int ZoneType, ConType, OldZone; 
	bool liq = false;
	
	CCD->ActorManager()->GetActorZone(Actor, &ZoneType);
	CCD->ActorManager()->GetActorOldZone(Actor, &OldZone);
	
	if(OldZone==1 && bPlayerMoved)
	{
		if(SurfaceSound==-1)
		{
			Liquid *LQ = CCD->ActorManager()->GetLiquid(Actor);
			if(!EffectC_IsStringNull(LQ->SurfaceSound))
			{
				Snd Sound;
				geVec3d Origin = Position();
				memset( &Sound, 0, sizeof( Sound ) );
				geVec3d_Copy( &(Origin), &( Sound.Pos ) );
				Sound.Min=kAudibleRadius;
				Sound.Loop=true;
				Sound.SoundDef = SPool_Sound(LQ->SurfaceSound);
				if(Sound.SoundDef)
					SurfaceSound = CCD->EffectManager()->Item_Add(EFF_SND, (void *)&Sound);
			}	
		}
		liq = true;
	} 
	else
	{
		if(SurfaceSound!=-1)
		{
			CCD->EffectManager()->Item_Delete(EFF_SND, SurfaceSound);
			SurfaceSound=-1;
		}
	}
	if(OldZone==2 && bPlayerMoved)
	{
		if(SwimSound==-1)
		{
			Liquid *LQ = CCD->ActorManager()->GetLiquid(Actor);
			if(!EffectC_IsStringNull(LQ->SwimSound))
			{
				Snd Sound;
				geVec3d Origin = Position();
				memset( &Sound, 0, sizeof( Sound ) );
				geVec3d_Copy( &(Origin), &( Sound.Pos ) );
				Sound.Min=kAudibleRadius;
				Sound.Loop=true;
				Sound.SoundDef = SPool_Sound(LQ->SwimSound);
				if(Sound.SoundDef)
					SwimSound = CCD->EffectManager()->Item_Add(EFF_SND, (void *)&Sound);
			}
		}
		liq = true;
	} 
	else
	{
		if(SwimSound!=-1)
		{
			CCD->EffectManager()->Item_Delete(EFF_SND, SwimSound);
			SwimSound=-1;
		}
	}
	if(OldZone==0 && (ZoneType & kInLiquidZone) && bPlayerMoved)
	{
		if(InLiquidSound==-1)
		{
			Liquid *LQ = CCD->ActorManager()->GetLiquid(Actor);
			if(!EffectC_IsStringNull(LQ->InLiquidSound))
			{
				Snd Sound;
				geVec3d Origin = Position();
				memset( &Sound, 0, sizeof( Sound ) );
				geVec3d_Copy( &(Origin), &( Sound.Pos ) );
				Sound.Min=kAudibleRadius;
				Sound.Loop=true;
				Sound.SoundDef = SPool_Sound(LQ->InLiquidSound);
				if(Sound.SoundDef)
					InLiquidSound = CCD->EffectManager()->Item_Add(EFF_SND, (void *)&Sound);
			}
		}
		liq = true;
	} 
	else
	{
		if(InLiquidSound!=-1)
		{
			CCD->EffectManager()->Item_Delete(EFF_SND, InLiquidSound);
			InLiquidSound=-1;
		}
	}
	if(liq)
		return RGF_SUCCESS;
// end change RF063
	if(ZoneType & kWaterZone)
	{
		ConType = 0;
	}
	else if(ZoneType & kLavaZone)
	{
		ConType = 1;
	}
	else if(ZoneType & kToxicGasZone)
	{
		ConType = 2;
	}
	else if(ZoneType & kNoGravityZone)
	{
		ConType = 3;
	}
	else if(ZoneType & kFrozenZone)
	{
		ConType = 4;
	}
	else if(ZoneType & kSludgeZone)
	{
		ConType = 5;
	}
	else if(ZoneType & kSlowMotionZone)
	{
		ConType = 6;
	}
	else if(ZoneType & kFastMotionZone)
	{
		ConType = 7;
	}
	else if(ZoneType & kClimbLaddersZone)
	{
		ConType = 8;
	}
// changed RF063
	else if(ZoneType & kUnclimbableZone)
	{
		ConType = 9;
	}
// end change RF063
	else 
	{
		ConType = -1;
	}
	if(!(CCD->ActorManager()->ForceActive(Actor, 0) || CCD->ActorManager()->Falling(Actor)))
	{
		if(ConType==-1)
		{
			if(geSound_SoundIsPlaying(CCD->Engine()->AudioSystem(),
				DefaultMotionHandle) != GE_TRUE && bPlayerMoved)
			{
				if(DefaultMotion[0])
				{
					int i = rand()%3;
					while(DefaultMotion[i]==NULL)
					{
						i = rand()%3;
					}
					DefaultMotionHandle = geSound_PlaySoundDef(
						CCD->Engine()->AudioSystem(), DefaultMotion[i], 
						1.00f, 0.0f, 1.0f, GE_FALSE);
				}
			}
			else
			{
				if(!bPlayerMoved && DefaultMotionHandle)
					geSound_StopSound(CCD->Engine()->AudioSystem(), DefaultMotionHandle);
			}
		}
		else
		{
			if(geSound_SoundIsPlaying(CCD->Engine()->AudioSystem(),
				ContentsHandles[ConType]) != GE_TRUE && bPlayerMoved)
			{
				if(Contents[ConType][0])
				{
					int i = rand()%3;
					while(Contents[ConType][i]==NULL)
					{
						i = rand()%3;
					}
					ContentsHandles[ConType] = geSound_PlaySoundDef(
						CCD->Engine()->AudioSystem(), Contents[ConType][i], 
						1.00f, 0.0f, 1.0f, GE_FALSE);
				}
			} 
			else
			{
				if(!bPlayerMoved && ContentsHandles[ConType])
					geSound_StopSound(CCD->Engine()->AudioSystem(), ContentsHandles[ConType]);
			}
		}
	}
	else
	{
		if(ConType==-1)
		{
			if(DefaultMotionHandle)
				geSound_StopSound(CCD->Engine()->AudioSystem(), DefaultMotionHandle);
		}
		else
		{
			if(ContentsHandles[ConType])
				geSound_StopSound(CCD->Engine()->AudioSystem(), ContentsHandles[ConType]);
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
	if(Injured)
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
// changed RF063	
	CCD->ActorManager()->SetMoving(Actor);
	int ZoneType; 
	float Step;
	CCD->ActorManager()->GetStepHeight(Actor, &Step);
	CCD->ActorManager()->GetActorZone(Actor, &ZoneType);
	if(ZoneType & kLiquidZone)
		CCD->ActorManager()->SetStepHeight(Actor, Step*3.0f);

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
	CCD->ActorManager()->SetStepHeight(Actor, Step);
// end change RF063
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
// changed RF063
	int OldZone;
	CCD->ActorManager()->GetActorOldZone(Actor, &OldZone);
	if(((OldZone>0) && CCD->ActorManager()->GetMoving(Actor)) || (theZone & kLavaZone)
		|| (theZone & kNoGravityZone) || (theZone & kUnclimbableZone))
		return;					// Sorry, no jumping allowed
	
	m_JumpActive = true;
	return;
// end change RF063
}

void CPlayer::SetJump()
{
	//	Jumping removes us as a passenger from any vehicle we're on.
	
	geActor *theVehicle = CCD->ActorManager()->GetVehicle(Actor);
	if(theVehicle != NULL)
		CCD->ActorManager()->RemovePassenger(Actor);
	
	if(CCD->ActorManager()->Falling(Actor) == GE_TRUE)
		return;						// No jumping in mid-air, heh heh heh
// changed RF063
	int theZone;
	CCD->ActorManager()->GetActorZone(Actor, &theZone);

	int OldZone;
	CCD->ActorManager()->GetActorOldZone(Actor, &OldZone);
	if(((OldZone>0) && CCD->ActorManager()->GetMoving(Actor)) || (theZone & kLavaZone)
		|| (theZone & kNoGravityZone) || (theZone & kUnclimbableZone))
		return;

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
		CCD->ActorManager()->SetForce(Actor, 1, theDir, m_JumpSpeed, m_JumpSpeed);
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
	default:
		mode = FIRSTPERSON;
	case FIRSTPERSON:
		CCD->CameraManager()->BindToActor(Actor);
		nFlags = kCameraTrackPosition;
		theTranslation.Y = m_CurrentHeight*0.75f;
		if(CCD->CameraManager()->GetViewHeight()!=-1.0f)
			theTranslation.Y = CCD->CameraManager()->GetViewHeight()*0.75f*m_Scale;
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
//changed RF063
		CCD->CameraManager()->ResetCamera();
// end change RF063
		CCD->Weapons()->SetView(0);
		break;
	case THIRDPERSON:
		CCD->CameraManager()->BindToActor(Actor);
		nFlags = kCameraTrackThirdPerson | kCameraTrackFree;
		theRotation.X = CCD->CameraManager()->GetPlayerAngleUp();
		theTranslation.Y = m_CurrentHeight;
		if(CCD->CameraManager()->GetPlayerHeight()!=-1.0f)
			theTranslation.Y = CCD->CameraManager()->GetPlayerHeight()*m_Scale;
		theTranslation.Z = CCD->CameraManager()->GetPlayerDistance();
		CCD->CameraManager()->SetCameraOffset(theTranslation, 
			theRotation);				// Set offset
		SwitchToThirdPerson();
		CCD->CameraManager()->ResetCamera();
		Allow3rdLook = CCD->CameraManager()->GetPlayerAllowLook();
		CCD->Weapons()->SetView(1);
		break;
	case ISOMETRIC:
		CCD->CameraManager()->BindToActor(Actor);
		nFlags = kCameraTrackIso;
		theRotation.X = CCD->CameraManager()->GetIsoAngleUp();
		theRotation.Y = CCD->CameraManager()->GetIsoAngleAround();
		theTranslation.Y = m_CurrentHeight;
		if(CCD->CameraManager()->GetIsoHeight()!=-1.0f)
			theTranslation.Y = CCD->CameraManager()->GetIsoHeight()*m_Scale;
		theTranslation.Z = CCD->CameraManager()->GetIsoDistance();
		CCD->CameraManager()->SetCameraOffset(theTranslation, 
			theRotation);				// Set offset
		SwitchToThirdPerson();
		CCD->CameraManager()->ResetCamera();
		CCD->Weapons()->SetView(1);
		break;
	case FIXEDCAMERA:
		if(CCD->FixedCameras()->GetNumber()==0)
			return;
		if(!CCD->FixedCameras()->GetFirstCamera())
			return;
		nFlags = kCameraTrackFixed;
		CCD->CameraManager()->Unbind();
		SwitchToThirdPerson();
		CCD->CameraManager()->ResetCamera();
		CCD->FixedCameras()->Tick();
		CCD->Weapons()->SetView(1);
		break;
	}
	m_PlayerViewPoint = mode;
	CCD->CameraManager()->SetTrackingFlags(nFlags);
	CCD->CameraManager()->TrackMotion();
	
}

// changed RF063
void CPlayer::ModifyLight(int amount)
{
	if(DecayLite)
		CurrentLiteLife = (float)amount;
	else
		CurrentLiteLife+=(float)amount;
	if(CurrentLiteLife>LiteLife)
		CurrentLiteLife = LiteLife;
	DecayLite = false;
}
// end change RF063

char *CPlayer::GetDieAnim()
{
	char *DA = CCD->Weapons()->DieAnim();
	if(DA)
		return DA;
	return DieAnim[rand()%DieAnimAmt];
}

char *CPlayer::GetInjuryAnim()
{
	char *DA = CCD->Weapons()->InjuryAnim();
	if(DA)
		return DA;
	return InjuryAnim[rand()%InjuryAnimAmt];
}

// Tick
// Process actor animation changes
//

void CPlayer::Tick(geFloat dwTicks)
{
	float distance;
	geVec3d FallEnd;
	int Zone, OldZone;

	CCD->ActorManager()->GetActorZone(Actor, &Zone);
	CCD->ActorManager()->GetActorOldZone(Actor, &OldZone);

// play out dying animation
	if(Dying)
	{
		if(CCD->ActorManager()->EndAnimation(Actor))
		{
			Alive = false;
			Dying = false;
		}
		return;
	}
// play out injury animation
	if(Injured)
	{
		if(!CCD->ActorManager()->EndAnimation(Actor))
			return;
		Injured = false;
		CCD->ActorManager()->SetMotion(Actor, LastMotion);
		CCD->ActorManager()->SetHoldAtEnd(Actor, false);
	}
// check health for death
	CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(Actor);
	if(theInv->Value("health")<=0)
	{
// changed RF063
		bool Switch = true;
		char *DA = CCD->Weapons()->DieAnim();
		if(!DA)
		{
			if(DieAnimAmt==0)
				Switch = false;
		}

		if(!CCD->CameraManager()->GetSwitchAllowed() || !CCD->CameraManager()->GetSwitch3rd())
			Switch = false;

		if(m_PlayerViewPoint==FIRSTPERSON && Switch)
			SwitchCamera(THIRDPERSON);
		
		if(!Switch)
// end change RF063
		{
			Alive = false;
		}
		else
		{
			if(DieSoundAmt>0)
			{
				Snd Sound;
				memset( &Sound, 0, sizeof( Sound ) );
				CCD->ActorManager()->GetPosition(Actor, &( Sound.Pos ));
				Sound.Min=kAudibleRadius;
				Sound.Loop=false;
				Sound.SoundDef = DieSound[rand()%DieSoundAmt];
				CCD->EffectManager()->Item_Add(EFF_SND, (void *)&Sound);
			}
			Dying = true;
			CCD->ActorManager()->SetNoCollide(Actor);
			CCD->ActorManager()->SetMotion(Actor, ANIMDIE);
			CCD->ActorManager()->SetHoldAtEnd(Actor, true);
		}
		
		return;
	}
// check health for injury
	if(LastHealth==-1)
	{
		LastHealth = theInv->Value("health");
	}
	else
	{
		if((LastHealth > theInv->Value("health") && !Injured) && (!m_JumpActive && CCD->ActorManager()->Falling(Actor) != GE_TRUE))
		{
// changed RF063
			if(InjurySoundAmt>0 && OldZone==0)
			{
				Snd Sound;
				memset( &Sound, 0, sizeof( Sound ) );
				CCD->ActorManager()->GetPosition(Actor, &( Sound.Pos ));
				Sound.Min=kAudibleRadius;
				Sound.Loop=false;
				Sound.SoundDef = InjurySound[rand()%InjurySoundAmt];
				CCD->EffectManager()->Item_Add(EFF_SND, (void *)&Sound);
			}
			if(m_PlayerViewPoint!=FIRSTPERSON && !FallInjure)
			{
				if(OldZone==0)
				{
					bool Switch = true;
					char *DA = CCD->Weapons()->InjuryAnim();
					if(!DA)
					{
						if(InjuryAnimAmt==0)
							Switch = false;
					}
					if(Switch && (rand()%10)>7)
					{
						strcpy(LastMotion, CCD->ActorManager()->GetMotion(Actor));
						CCD->ActorManager()->SetMotion(Actor, ANIMINJURY);
						CCD->ActorManager()->SetHoldAtEnd(Actor, true);
						Injured = true;
					}
				}
// end change RF063
			}
			FallInjure = false;
		}
		LastHealth = theInv->Value("health");
	}
// check for fall damage	
	if(!Falling && OldFalling)
	{
		CCD->ActorManager()->GetPosition(Actor, &(FallEnd));
		distance = (float)fabs(FallEnd.Y-FallStart.Y);
// changed RF063
		if(Zone!=kLiquidZone)
		{
			if(LandSoundAmt>0 && distance>MinFallDist)
			{
				Snd Sound;
				memset( &Sound, 0, sizeof( Sound ) );
				CCD->ActorManager()->GetPosition(Actor, &( Sound.Pos ));
				Sound.Min=kAudibleRadius;
				Sound.Loop=false;
				Sound.SoundDef = LandSound[rand()%LandSoundAmt];
				CCD->EffectManager()->Item_Add(EFF_SND, (void *)&Sound);
			}
			if(FallDamage>0.0f && distance>=MinFallDist)
			{
				if(distance>MaxFallDist)
					distance=MaxFallDist;
				float damage = ((distance-MinFallDist)/(MaxFallDist-MinFallDist))*FallDamage;
				CCD->Damage()->DamageActor(Actor, damage, "health", damage, "health");
				FallInjure = true;
			}
		}
// end change RF063
	}
	OldFalling = Falling;

// check auto increase attribute

// changed RF063
	if(!strcmp(StaminaName, "LightValue"))
	{
		
		StaminaTime += dwTicks*0.001f;
		if(StaminaTime>=StaminaDelay)
		{
			StaminaTime = 0.0f;
			if(CurrentLiteLife != -1.0f && !lightactive)
			{
				CurrentLiteLife += 1.0f;
				if(CurrentLiteLife > LiteLife)
					CurrentLiteLife = LiteLife;
			}
		}
	}
	else
	{
		if(theInv->Has(StaminaName))
		{
			StaminaTime += dwTicks*0.001f;
			if(StaminaTime>=StaminaDelay)
			{
				StaminaTime = 0.0f;
				theInv->Modify(StaminaName, 1);
			}
		}
	}
// restore oxygen if out of liquid
	if(theInv->Has("oxygen"))
	{
		if(OldZone<2)
			theInv->Modify("oxygen", theInv->High("oxygen"));
	}
// check for damage in liquid
	if(OldZone>0 || Zone==kLiquidZone || Zone==kInLiquidZone)
	{
		Liquid *LQ = CCD->ActorManager()->GetLiquid(Actor);
		if(LQ)
		{
			if(LiquidTime==0.0f || InLiquid==-1)
			{
				if(LQ->DamageIn || OldZone>1)
				{
					if(!EffectC_IsStringNull(LQ->DamageAttr))
					{
						if(theInv->Has(LQ->DamageAttr) && theInv->Value(LQ->DamageAttr)>theInv->Low(LQ->DamageAttr))
						{
							InLiquid = 1;
							theInv->Modify(LQ->DamageAttr, -LQ->DamageAmt);
						}
					}
					if(InLiquid==-1)
					{
						if(!EffectC_IsStringNull(LQ->DamageAltAttr))
						{
							if(theInv->Has(LQ->DamageAltAttr) && theInv->Value(LQ->DamageAltAttr)>theInv->Low(LQ->DamageAltAttr))
							{
								theInv->Modify(LQ->DamageAltAttr, -LQ->DamageAltAmt);
								InLiquid = 2;
							}
						}
					}
				}
				else
				{
					LiquidTime = 0.0f;
					InLiquid = -1;
				}
			}
			LiquidTime += dwTicks*0.001f;
			if(InLiquid>0)
			{
				if(InLiquid==1)
				{
					if(LiquidTime>=LQ->DamageDelay)
					{
						LiquidTime = 0.0f;
						InLiquid = -1;
					}
				}
				else
				{
					if(LiquidTime>=LQ->DamageAltDelay)
					{
						LiquidTime = 0.0f;
						InLiquid = -1;
					}
				}
			}
		}
	}
	else
	{
		LiquidTime = 0.0f;
		InLiquid = -1;
	}

// end change RF063	
	if(theInv->Has("light"))
	{
		if(theInv->Value("light")!=0 && lighton)
		{
			if(lightactive==false)
			{
				Glow	Gl;
				geExtBox theBox;
				geVec3d Pos = Position();
				CCD->ActorManager()->GetBoundingBox(Actor, &theBox);
				Pos.Y += theBox.Max.Y;
				if(LiteBone[0] != '\0')
				{
					geXForm3d thePosition;
					if(geActor_GetBoneTransform(Actor, LiteBone, &(thePosition)))
						Pos = thePosition.Translation;
				}
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
				Gl.DoNotClip = false;
				Gl.CastShadows = true;
				lightactive=true;
				Gl.Intensity = LiteIntensity;
				if(CurrentLiteLife == -1.0f)
				{
					CurrentLiteLife = LiteLife;
					DecayLite = false;
				}
				else if(DecayLite)
					Gl.Intensity = LiteIntensity*(CurrentLiteDecay/LiteDecay);
				lighteffect = CCD->EffectManager()->Item_Add(EFF_LIGHT, (void *)&Gl);
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
		if(LiteBone[0] != '\0')
		{
			geXForm3d thePosition;
			if(geActor_GetBoneTransform(Actor, LiteBone, &(thePosition)))
				Pos = thePosition.Translation;
		}
		geVec3d_Copy(&(Pos), &(Gl.Pos));
		if(!DecayLite)
		{
			CurrentLiteLife -= dwTicks*0.001f;
			if(CurrentLiteLife<=0.0f)
			{
				DecayLite = true;
				CurrentLiteDecay = LiteDecay;
			}
			CCD->EffectManager()->Item_Modify(EFF_LIGHT, lighteffect, (void *)&Gl, GLOW_POS);
		}
		else
		{
			CurrentLiteDecay -= dwTicks*0.001f;
			if(CurrentLiteDecay<=0.0f)
			{
				CCD->EffectManager()->Item_Delete(EFF_LIGHT, lighteffect);
				lighteffect = -1;
				lightactive=false;
				CurrentLiteLife = -1.0f;
				lighton = false;
				theInv->Modify("light", -1);
			}
			else
			{
				Gl.Intensity = LiteIntensity*(CurrentLiteDecay/LiteDecay);
				CCD->EffectManager()->Item_Modify(EFF_LIGHT, lighteffect, (void *)&Gl, GLOW_POS | GLOW_INTENSITY);
			}
		}
	}
	
	char *Motion;
	
	Motion = CCD->ActorManager()->GetMotion(Actor);

//	geEngine_Printf(CCD->Engine()->Engine(), 0,10,"Motion = %s",Motion);
//	geEngine_Printf(CCD->Engine()->Engine(), 0,20,"OldZone = %d",OldZone);
//	geEngine_Printf(CCD->Engine()->Engine(), 0,30,"Zone = %x",Zone);

	switch(m_Moving)
	{
	case MOVEIDLE:	
		// jump at idle
		if(m_JumpActive && strcmp(Motion, ANIMSTARTJUMP)
			&& strcmp(Motion, ANIMJUMP) && strcmp(Motion, ANIMCSTARTJUMP)
			&& strcmp(Motion, ANIMLAND) && strcmp(Motion, ANIMCLAND)
			&& strcmp(Motion, ANIMFALL) && strcmp(Motion, ANIMJUMPSHOOT)
			&& strcmp(Motion, ANIMFALLSHOOT))
		{
			if(CCD->ActorManager()->CheckAnimation(Actor, ANIMJUMP)==GE_TRUE)
			{
				CCD->ActorManager()->SetAnimationHeight(Actor, ANIMJUMP, false);
				if(m_PlayerViewPoint!=FIRSTPERSON)
				{
					if(m_crouch)
						CCD->ActorManager()->SetMotion(Actor, ANIMCSTARTJUMP);
					else
						CCD->ActorManager()->SetMotion(Actor, ANIMSTARTJUMP);
					CCD->ActorManager()->SetNextMotion(Actor, ANIMJUMP);
					CCD->Weapons()->SetAttackFlag(false);
				}
				else
					CCD->ActorManager()->SetMotion(Actor, ANIMJUMP);
				break;
			}
			else
				m_JumpActive = false; 
		}
		
		if(!strcmp(Motion, ANIMSTARTJUMP) || !strcmp(Motion, ANIMCSTARTJUMP)
			|| !strcmp(Motion, ANIMLAND) || !strcmp(Motion, ANIMCLAND))
		{
			CCD->Weapons()->SetAttackFlag(false);
		}
		
		if(!strcmp(Motion, ANIMJUMP) && (CCD->ActorManager()->ForceActive(Actor, 0)==GE_FALSE) && m_JumpActive)
		{
			SetJump();
			m_JumpActive = false;
			break;
		}
		
		if((!strcmp(Motion, ANIMJUMP) || !strcmp(Motion, ANIMJUMPSHOOT)) && (CCD->ActorManager()->ForceActive(Actor, 0)==GE_FALSE) && !m_JumpActive)
		{
			if(!Falling)
			{
				CCD->ActorManager()->GetPosition(Actor, &(FallStart));
				Falling = true;
			}
			if(OldZone>0)
			{
				CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
				CCD->ActorManager()->SetMotion(Actor, ANIMTREADWATER);
			}
			else
				CCD->ActorManager()->SetMotion(Actor, ANIMFALL);
			break;
		}
// changed RF063
		if(!strcmp(Motion, ANIMSWIM))
		{
			if(CCD->ActorManager()->CheckAnimation(Actor, ANIMIDLE)==GE_TRUE)
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMTREADWATER);
				CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
			}
			break;
		}

		if(CCD->ActorManager()->Falling(Actor)==GE_TRUE)
		{
			if(CCD->ActorManager()->ReallyFall(Actor)==RGF_SUCCESS || !strcmp(Motion, ANIMJUMP))
			{
				if(!Falling)
				{
					CCD->ActorManager()->GetPosition(Actor, &(FallStart));
					Falling = true;
				}

				if(!strcmp(Motion, ANIMFALL) || !strcmp(Motion, ANIMFALLSHOOT)
					|| !strcmp(Motion, ANIMTREADWATER))
					break;

				if(OldZone>0 || Zone & kLiquidZone)
				{
					CCD->ActorManager()->SetMotion(Actor, ANIMTREADWATER);
					CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
				}
				else 
				{
					CCD->ActorManager()->SetMotion(Actor, ANIMFALL);
					CCD->ActorManager()->SetAnimationHeight(Actor, ANIMFALL, false);
				}
// end change RF063
				break;
			}
		}
		
		if((!strcmp(Motion, ANIMFALL) || !strcmp(Motion, ANIMFALLSHOOT)) && (CCD->ActorManager()->Falling(Actor)==GE_FALSE))
		{
			Falling=false;
			
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
// changed RF063
				if(OldZone>0)
				{
					CCD->ActorManager()->SetNextMotion(Actor, ANIMTREADWATER);
					CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
				}
				else
				{
					CCD->ActorManager()->SetNextMotion(Actor, ANIMIDLE);
					CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
				}
			}
			CCD->Weapons()->SetAttackFlag(false);
			break;
		}
// changed RF063
		if(!(!strcmp(Motion, ANIMJUMP) || !strcmp(Motion, ANIMJUMPSHOOT) 
			|| !strcmp(Motion, ANIMSTARTJUMP) || !strcmp(Motion, ANIMLAND)))
		{
		if(OldZone>0 && m_crouch)
		{
			m_crouch = false;
			CCD->ActorManager()->SetMotion(Actor, ANIMIDLE);
			CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
			break;
		}
		if(OldZone>0)
		{
			if(strcmp(Motion, ANIMTREADWATER))
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMTREADWATER);
				CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
				m_crouch = false;
				break;
			}
		}
		if(OldZone==0)
		{
			if(!strcmp(Motion, ANIMTREADWATER))
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMIDLE);
				CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
				break;
			}
		}
		}
// end change RF063
		// crouch at idle
		if(!strcmp(Motion, ANIMIDLE) && m_crouch && OldZone==0)
		{
			CCD->ActorManager()->SetMotion(Actor, ANIMCIDLE);
			CCD->ActorManager()->SetAnimationHeight(Actor, ANIMCIDLE, true);
		}
		if(!strcmp(Motion, ANIMSHOOT) && m_crouch && OldZone==0)
		{
			CCD->ActorManager()->SetBlendMotion(Actor, ANIMCSHOOT, ANIMCSHOOT1, CCD->CameraManager()->GetTiltPercent());
			CCD->ActorManager()->SetBlendNextMotion(Actor, ANIMCAIM, ANIMCAIM1, CCD->CameraManager()->GetTiltPercent());
			CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
		}
		if(!strcmp(Motion, ANIMAIM) && m_crouch && OldZone==0)
		{
			CCD->ActorManager()->SetMotion(Actor, ANIMCAIM);
			CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
		}
		// uncrouch at idle
		if((!strcmp(Motion, ANIMCIDLE) || !strcmp(Motion, ANIMC2IDLE))&& !m_crouch)
		{
			if(CCD->ActorManager()->CheckAnimation(Actor, ANIMIDLE)==GE_TRUE)
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMIDLE);
				CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
			}
		}
		if(!strcmp(Motion, ANIMCSHOOT) && !m_crouch)
		{
			if(CCD->ActorManager()->CheckAnimation(Actor, ANIMIDLE)==GE_TRUE)
			{
				CCD->ActorManager()->SetBlendMotion(Actor, ANIMSHOOT, ANIMSHOOT1, CCD->CameraManager()->GetTiltPercent());
				CCD->ActorManager()->SetBlendNextMotion(Actor, ANIMAIM, ANIMAIM1, CCD->CameraManager()->GetTiltPercent());
				CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
			}
		}
		if(!strcmp(Motion, ANIMCAIM) && !m_crouch)
		{
			if(CCD->ActorManager()->CheckAnimation(Actor, ANIMIDLE)==GE_TRUE)
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMAIM);
				CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
			}
		}
		// attack
// changed RF063
		if(CCD->Weapons()->GetAttackFlag()) 
		{
			if(!strcmp(Motion, ANIMIDLE) || !strcmp(Motion, ANIMW2IDLE) || !strcmp(Motion, ANIMAIM))
			{
				CCD->ActorManager()->SetBlendMotion(Actor, ANIMSHOOT, ANIMSHOOT1, CCD->CameraManager()->GetTiltPercent());
				CCD->ActorManager()->SetBlendNextMotion(Actor, ANIMAIM, ANIMAIM1, CCD->CameraManager()->GetTiltPercent());
			}
			if(!strcmp(Motion, ANIMCIDLE) || !strcmp(Motion, ANIMC2IDLE) || !strcmp(Motion, ANIMCAIM))
			{
				CCD->ActorManager()->SetBlendMotion(Actor, ANIMCSHOOT, ANIMCSHOOT1, CCD->CameraManager()->GetTiltPercent());
				CCD->ActorManager()->SetBlendNextMotion(Actor, ANIMCAIM, ANIMCAIM1, CCD->CameraManager()->GetTiltPercent());
			}
			if(!strcmp(Motion, ANIMJUMP))
			{
				CCD->ActorManager()->SetBlendMotion(Actor, ANIMJUMPSHOOT, ANIMJUMPSHOOT1, CCD->CameraManager()->GetTiltPercent());
				CCD->ActorManager()->SetNextMotion(Actor, ANIMJUMP);
			}
			if(!strcmp(Motion, ANIMFALL))
			{
				CCD->ActorManager()->SetBlendMotion(Actor, ANIMFALLSHOOT, ANIMFALLSHOOT1, CCD->CameraManager()->GetTiltPercent());
				CCD->ActorManager()->SetNextMotion(Actor, ANIMFALL);
			}

			if(!strcmp(Motion, ANIMSHOOT))
			{
				CCD->ActorManager()->SetBlendMot(Actor, ANIMSHOOT, ANIMSHOOT1, CCD->CameraManager()->GetTiltPercent());
				CCD->ActorManager()->SetBlendNextMotion(Actor, ANIMAIM, ANIMAIM1, CCD->CameraManager()->GetTiltPercent());
			}
			if(!strcmp(Motion, ANIMCSHOOT))
			{
				CCD->ActorManager()->SetBlendMot(Actor, ANIMCSHOOT, ANIMCSHOOT1, CCD->CameraManager()->GetTiltPercent());
				CCD->ActorManager()->SetBlendNextMotion(Actor, ANIMCAIM, ANIMCAIM1, CCD->CameraManager()->GetTiltPercent());
			}
			if(!strcmp(Motion, ANIMJUMPSHOOT))
			{
				CCD->ActorManager()->SetBlendMot(Actor, ANIMJUMPSHOOT, ANIMJUMPSHOOT1, CCD->CameraManager()->GetTiltPercent());
				CCD->ActorManager()->SetNextMotion(Actor, ANIMJUMP);
			}
			if(!strcmp(Motion, ANIMFALLSHOOT))
			{
				CCD->ActorManager()->SetBlendMot(Actor, ANIMFALLSHOOT, ANIMFALLSHOOT1, CCD->CameraManager()->GetTiltPercent());
				CCD->ActorManager()->SetNextMotion(Actor, ANIMFALL);
			}
// end change RF063
			break;
		}
		if(!strcmp(Motion, ANIMAIM))
		{
			CCD->ActorManager()->SetNextMotion(Actor, ANIMIDLE);
			break;
		}
		if(!strcmp(Motion, ANIMCAIM))
		{
			CCD->ActorManager()->SetNextMotion(Actor, ANIMCIDLE);
			break;
		}
		
		// change to idle from moving
		if(!strcmp(Motion, ANIMWALK) || !strcmp(Motion, ANIMI2WALK)
			|| !strcmp(Motion, ANIMRUN) || !strcmp(Motion, ANIMI2RUN)
			|| !strcmp(Motion, ANIMRUNSHOOT) || !strcmp(Motion, ANIMWALKSHOOT)
			|| !strcmp(Motion, ANIMSWIM))
		{
			if(OldZone>0)
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMTREADWATER);
				CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
			}
			else
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMW2IDLE);
				CCD->ActorManager()->SetNextMotion(Actor, ANIMIDLE);
				CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
			}
			break;
		}
		
		if(!strcmp(Motion, ANIMCRAWL) || !strcmp(Motion, ANIMCRAWLSHOOT))
		{
			CCD->ActorManager()->SetMotion(Actor, ANIMC2IDLE);
			CCD->ActorManager()->SetNextMotion(Actor, ANIMCIDLE);
			break;
		}
		
		if(!strcmp(Motion, ANIMSLIDECLEFT) || !strcmp(Motion, ANIMSLIDECRIGHT)
			|| !strcmp(Motion, ANIMSLIDECLEFTSHOOT) || !strcmp(Motion, ANIMSLIDECRIGHTSHOOT))
		{
			CCD->ActorManager()->SetMotion(Actor, ANIMCIDLE);
			break;
		}
		
		if(!strcmp(Motion, ANIMSLIDELEFT) || !strcmp(Motion, ANIMRUNSLIDELEFT)
			|| !strcmp(Motion, ANIMSLIDERIGHT) || !strcmp(Motion, ANIMRUNSLIDERIGHT)
			|| !strcmp(Motion, ANIMSLIDELEFTSHOOT) || !strcmp(Motion, ANIMRUNSLIDELEFTSHOOT)
			|| !strcmp(Motion, ANIMSLIDERIGHTSHOOT) || !strcmp(Motion, ANIMRUNSLIDERIGHTSHOOT))
		{
			if(OldZone>0)
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMTREADWATER);
				CCD->ActorManager()->SetAnimationHeight(Actor, ANIMTREADWATER, false);
			}
			else
				CCD->ActorManager()->SetMotion(Actor, ANIMIDLE);
			CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, false);
			break;
		}
		
		break;
	case MOVEWALKFORWARD:
	case MOVEWALKBACKWARD:
		if(strcmp(Motion, ANIMFALL) && (CCD->ActorManager()->Falling(Actor)==GE_TRUE))
		{
			if(CCD->ActorManager()->ReallyFall(Actor)==RGF_SUCCESS || !strcmp(Motion, ANIMJUMP))
			{
				if(!Falling)
				{
					CCD->ActorManager()->GetPosition(Actor, &(FallStart));
					Falling = true;
				}
// changed RF063
				if(!(Zone & kInLiquidZone || Zone & kLiquidZone) || !strcmp(Motion, ANIMJUMP))
				{
					CCD->ActorManager()->SetMotion(Actor, ANIMFALL);
					CCD->ActorManager()->SetAnimationHeight(Actor, ANIMFALL, false);
				}
// end change RF063
				CCD->Weapons()->SetAttackFlag(false);
				break;
			}
		}
		
		if((!strcmp(Motion, ANIMFALL) || !strcmp(Motion, ANIMFALLSHOOT)) && (CCD->ActorManager()->Falling(Actor)==GE_FALSE))
		{
			Falling=false;
			
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
			CCD->Weapons()->SetAttackFlag(false);
			break;
		}
		
		if(m_JumpActive && strcmp(Motion, ANIMJUMP) && strcmp(Motion, ANIMFALL) 
			&& (CCD->ActorManager()->ForceActive(Actor, 0)==GE_FALSE))
		{
			m_JumpActive = false;
// changed RF063
			if(strcmp(Motion, ANIMSWIM))
			{
				if(CCD->ActorManager()->CheckAnimation(Actor, ANIMJUMP)==GE_TRUE)
				{
					CCD->ActorManager()->SetAnimationHeight(Actor, ANIMJUMP, false);
					CCD->ActorManager()->SetMotion(Actor, ANIMJUMP);
					SetJump();
					CCD->Weapons()->SetAttackFlag(false);
					break;
				}
			}
// end change RF063
		}
	
		if(!strcmp(Motion, ANIMJUMP))
		{
			m_JumpActive = false;
			if(CCD->Weapons()->GetAttackFlag())
			{
				CCD->ActorManager()->SetBlendMotion(Actor, ANIMJUMPSHOOT, ANIMJUMPSHOOT1, CCD->CameraManager()->GetTiltPercent());
				CCD->ActorManager()->SetNextMotion(Actor, ANIMJUMP);
			}
			break;
		}
		if(!strcmp(Motion, ANIMFALL))
		{
			m_JumpActive = false;
			if(CCD->Weapons()->GetAttackFlag())
			{
				CCD->ActorManager()->SetBlendMotion(Actor, ANIMFALLSHOOT, ANIMFALLSHOOT1, CCD->CameraManager()->GetTiltPercent());
				CCD->ActorManager()->SetNextMotion(Actor, ANIMFALL);
			}
			break;
		}
// changed RF063		
		if(OldZone>0)
		{
			if(strcmp(Motion, ANIMSWIM))
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMSWIM);
				CCD->ActorManager()->SetAnimationHeight(Actor, ANIMSWIM, true);
			}
		}
		else
		{
			if(!strcmp(Motion, ANIMSWIM) || !strcmp(Motion, ANIMTREADWATER))
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMWALK);
				CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
			}
		}
// changed RF063	
		// walk from idle
		if(!strcmp(Motion, ANIMIDLE) || !strcmp(Motion, ANIMW2IDLE))
		{
			if(!m_run)
			{
				if(!CCD->Weapons()->GetAttackFlag())
				{
					CCD->ActorManager()->SetMotion(Actor, ANIMI2WALK);
					CCD->ActorManager()->SetNextMotion(Actor, ANIMWALK);
				}
				else
				{
					CCD->ActorManager()->SetBlendMotion(Actor, ANIMWALKSHOOT, ANIMWALKSHOOT1, CCD->CameraManager()->GetTiltPercent());
					CCD->ActorManager()->SetNextMotion(Actor, ANIMWALK);
				}
			}
			else
			{
				if(!CCD->Weapons()->GetAttackFlag())
				{
					CCD->ActorManager()->SetMotion(Actor, ANIMI2RUN);
					CCD->ActorManager()->SetNextMotion(Actor, ANIMRUN);
				}
				else
				{
					CCD->ActorManager()->SetBlendMotion(Actor, ANIMRUNSHOOT, ANIMRUNSHOOT1, CCD->CameraManager()->GetTiltPercent());
					CCD->ActorManager()->SetNextMotion(Actor, ANIMRUN);
				}
			}
			break;
		}
		// walk from attack
		if(!strcmp(Motion, ANIMSHOOT) || !strcmp(Motion, ANIMAIM))
		{
			if(!m_run)
			{
				if(!CCD->Weapons()->GetAttackFlag())
					CCD->ActorManager()->SetMotion(Actor, ANIMWALK);
				else
				{
					CCD->ActorManager()->SetBlendMotion(Actor, ANIMWALKSHOOT, ANIMWALKSHOOT1, CCD->CameraManager()->GetTiltPercent());
					CCD->ActorManager()->SetNextMotion(Actor, ANIMWALK);
				}
			}
			else
			{
				if(!CCD->Weapons()->GetAttackFlag())
					CCD->ActorManager()->SetMotion(Actor, ANIMRUN);
				else
				{
					CCD->ActorManager()->SetBlendMotion(Actor, ANIMRUNSHOOT, ANIMRUNSHOOT1, CCD->CameraManager()->GetTiltPercent());
					CCD->ActorManager()->SetNextMotion(Actor, ANIMRUN);
				}
			}
			break;
		}
		// change to crawl	
		if((!strcmp(Motion, ANIMI2WALK) || !strcmp(Motion, ANIMWALK)
			|| !strcmp(Motion, ANIMI2RUN) || !strcmp(Motion, ANIMRUN) 
			|| !strcmp(Motion, ANIMWALKSHOOT) || !strcmp(Motion, ANIMRUNSHOOT)) 
			&& m_crouch && OldZone==0)
		{
			CCD->ActorManager()->SetAnimationHeight(Actor, ANIMCIDLE, true);
			CCD->ActorManager()->SetMotion(Actor, ANIMCRAWL);
			break;
		}
		// crawl
		if(!strcmp(Motion, ANIMCIDLE) || !strcmp(Motion, ANIMC2IDLE) 
			|| !strcmp(Motion, ANIMCSHOOT) || !strcmp(Motion, ANIMCAIM))
		{
			CCD->ActorManager()->SetMotion(Actor, ANIMCRAWL);
			break;
		}
		// change to walk from crawl
		if((!strcmp(Motion, ANIMCRAWL) || !strcmp(Motion, ANIMCRAWLSHOOT)) && !m_crouch)
		{
			if(CCD->ActorManager()->CheckAnimation(Actor, ANIMIDLE)==GE_TRUE)
			{
				CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
				if(!m_run)
				{
					if(!CCD->Weapons()->GetAttackFlag())
						CCD->ActorManager()->SetMotion(Actor, ANIMWALK);
					else
					{
						CCD->ActorManager()->SetBlendMotion(Actor, ANIMWALKSHOOT, ANIMWALKSHOOT1, CCD->CameraManager()->GetTiltPercent());
						CCD->ActorManager()->SetNextMotion(Actor, ANIMWALK);
					}
				}
				else
				{
					if(!CCD->Weapons()->GetAttackFlag())
						CCD->ActorManager()->SetMotion(Actor, ANIMRUN);
					else
					{
						CCD->ActorManager()->SetBlendMotion(Actor, ANIMRUNSHOOT, ANIMRUNSHOOT1, CCD->CameraManager()->GetTiltPercent());
						CCD->ActorManager()->SetNextMotion(Actor, ANIMRUN);
					}
				}
				break;
			}
		}
		
		if(CCD->Weapons()->GetAttackFlag() && !m_crouch)
		{
			if(!m_run)
			{
// changed RF063
				if(OldZone==0)
				{
					if(!strcmp(Motion, ANIMWALKSHOOT))
						CCD->ActorManager()->SetBlendMot(Actor, ANIMWALKSHOOT, ANIMWALKSHOOT1, CCD->CameraManager()->GetTiltPercent());
					else
						CCD->ActorManager()->SetBlendMotion(Actor, ANIMWALKSHOOT, ANIMWALKSHOOT1, CCD->CameraManager()->GetTiltPercent());
					CCD->ActorManager()->SetNextMotion(Actor, ANIMWALK);
				}
			}
			else
			{
				if(!strcmp(Motion, ANIMRUNSHOOT))
					CCD->ActorManager()->SetBlendMot(Actor, ANIMRUNSHOOT, ANIMRUNSHOOT1, CCD->CameraManager()->GetTiltPercent());
				else
					CCD->ActorManager()->SetBlendMotion(Actor, ANIMRUNSHOOT, ANIMRUNSHOOT1, CCD->CameraManager()->GetTiltPercent());
				CCD->ActorManager()->SetNextMotion(Actor, ANIMRUN);
			}
			break;
		}
		
		if(CCD->Weapons()->GetAttackFlag() && m_crouch)
		{
			if(!strcmp(Motion, ANIMCRAWLSHOOT))
				CCD->ActorManager()->SetBlendMot(Actor, ANIMCRAWLSHOOT, ANIMCRAWLSHOOT1, CCD->CameraManager()->GetTiltPercent());
			else
				CCD->ActorManager()->SetBlendMotion(Actor, ANIMCRAWLSHOOT, ANIMCRAWLSHOOT1, CCD->CameraManager()->GetTiltPercent());
// end change RF063
			CCD->ActorManager()->SetNextMotion(Actor, ANIMCRAWL);
			break;
		}
		
		if(!strcmp(Motion, ANIMSLIDELEFT) || !strcmp(Motion, ANIMRUNSLIDELEFT)
			|| !strcmp(Motion, ANIMSLIDERIGHT) || !strcmp(Motion, ANIMRUNSLIDERIGHT)
			|| !strcmp(Motion, ANIMSLIDELEFTSHOOT) || !strcmp(Motion, ANIMRUNSLIDELEFTSHOOT)
			|| !strcmp(Motion, ANIMSLIDERIGHTSHOOT) || !strcmp(Motion, ANIMRUNSLIDERIGHTSHOOT))
		{
			if(!m_run)
			{
				if(!CCD->Weapons()->GetAttackFlag())
					CCD->ActorManager()->SetMotion(Actor, ANIMWALK);
				else
				{
					CCD->ActorManager()->SetBlendMotion(Actor, ANIMWALKSHOOT, ANIMWALKSHOOT1, CCD->CameraManager()->GetTiltPercent());
					CCD->ActorManager()->SetNextMotion(Actor, ANIMWALK);
				}
			}
			else
			{
				if(!CCD->Weapons()->GetAttackFlag())
					CCD->ActorManager()->SetMotion(Actor, ANIMRUN);
				else
				{
					CCD->ActorManager()->SetBlendMotion(Actor, ANIMRUNSHOOT, ANIMRUNSHOOT1, CCD->CameraManager()->GetTiltPercent());
					CCD->ActorManager()->SetNextMotion(Actor, ANIMRUN);
				}
			}
			break;
		}
		
		if(!strcmp(Motion, ANIMSLIDECLEFT) || !strcmp(Motion, ANIMSLIDECRIGHT)
			|| !strcmp(Motion, ANIMSLIDECLEFTSHOOT) || !strcmp(Motion, ANIMSLIDECRIGHTSHOOT))
		{
			CCD->ActorManager()->SetMotion(Actor, ANIMCRAWL);
			break;
		}
		
		if(!strcmp(Motion, ANIMWALK) && m_run)
		{
			CCD->ActorManager()->SetMotion(Actor, ANIMRUN);
			break;
		}
		if(!strcmp(Motion, ANIMRUN) && !m_run)
		{
			CCD->ActorManager()->SetMotion(Actor, ANIMWALK);
			break;
		}
		break;
	case MOVESLIDERIGHT:
		m_JumpActive = false;
		if(!strcmp(Motion, ANIMIDLE) || !strcmp(Motion, ANIMW2IDLE)
			|| !strcmp(Motion, ANIMSHOOT) || !strcmp(Motion, ANIMAIM)
			|| !strcmp(Motion, ANIMWALK) || !strcmp(Motion, ANIMI2WALK)
			|| !strcmp(Motion, ANIMWALKSHOOT) || !strcmp(Motion, ANIMRUNSHOOT)
			|| !strcmp(Motion, ANIMRUN) || !strcmp(Motion, ANIMI2RUN)
			|| !strcmp(Motion, ANIMSLIDELEFTSHOOT) || !strcmp(Motion, ANIMRUNSLIDELEFTSHOOT)
			|| !strcmp(Motion, ANIMSLIDELEFT) || !strcmp(Motion, ANIMRUNSLIDELEFT))
		{
			if(!m_run)
			{
				if(!CCD->Weapons()->GetAttackFlag())
					CCD->ActorManager()->SetMotion(Actor, ANIMSLIDERIGHT);
				else
				{
					CCD->ActorManager()->SetBlendMotion(Actor, ANIMSLIDERIGHTSHOOT, ANIMSLIDERIGHTSHOOT1, CCD->CameraManager()->GetTiltPercent());
					CCD->ActorManager()->SetNextMotion(Actor, ANIMSLIDERIGHT);
				}
			}
			else
			{
				if(!CCD->Weapons()->GetAttackFlag())
					CCD->ActorManager()->SetMotion(Actor, ANIMRUNSLIDERIGHT);
				else
				{
					CCD->ActorManager()->SetMotion(Actor, ANIMRUNSLIDERIGHTSHOOT);
					CCD->ActorManager()->SetNextMotion(Actor, ANIMRUNSLIDERIGHT);
				}
			}
			break;
		}
		
		if(!strcmp(Motion, ANIMCIDLE) || !strcmp(Motion, ANIMC2IDLE)
			|| !strcmp(Motion, ANIMSLIDECLEFT) || !strcmp(Motion, ANIMSLIDECLEFTSHOOT)
			|| !strcmp(Motion, ANIMCRAWL) || !strcmp(Motion, ANIMCRAWLSHOOT)
			|| !strcmp(Motion, ANIMCSHOOT) || !strcmp(Motion, ANIMCAIM))
		{
			if(!CCD->Weapons()->GetAttackFlag())
				CCD->ActorManager()->SetMotion(Actor, ANIMSLIDECRIGHT);
			else
			{
				CCD->ActorManager()->SetBlendMotion(Actor, ANIMSLIDECRIGHTSHOOT, ANIMSLIDECRIGHTSHOOT1, CCD->CameraManager()->GetTiltPercent());
				CCD->ActorManager()->SetNextMotion(Actor, ANIMSLIDECRIGHT);
			}
			break;
		}
		
		if((!strcmp(Motion, ANIMSLIDERIGHT) || !strcmp(Motion, ANIMRUNSLIDERIGHT)) && m_crouch)
		{
			if(!CCD->Weapons()->GetAttackFlag())
				CCD->ActorManager()->SetMotion(Actor, ANIMSLIDECRIGHT);
			else
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMSLIDECRIGHTSHOOT);
				CCD->ActorManager()->SetNextMotion(Actor, ANIMSLIDECRIGHT);
			}
			CCD->ActorManager()->SetAnimationHeight(Actor, ANIMCIDLE, true);
			break;
		}
		
		if((!strcmp(Motion, ANIMSLIDECRIGHT) || !strcmp(Motion, ANIMSLIDECRIGHTSHOOT))&& !m_crouch)
		{
			if(CCD->ActorManager()->CheckAnimation(Actor, ANIMIDLE)==GE_TRUE)
			{
				if(!m_run)
				{
					if(!CCD->Weapons()->GetAttackFlag())
						CCD->ActorManager()->SetMotion(Actor, ANIMSLIDERIGHT);
					else
					{
						CCD->ActorManager()->SetBlendMotion(Actor, ANIMSLIDERIGHTSHOOT, ANIMSLIDERIGHTSHOOT1, CCD->CameraManager()->GetTiltPercent());
						CCD->ActorManager()->SetNextMotion(Actor, ANIMSLIDERIGHT);
					}
				}
				else
				{
					if(!CCD->Weapons()->GetAttackFlag())
						CCD->ActorManager()->SetMotion(Actor, ANIMRUNSLIDERIGHT);
					else
					{
						CCD->ActorManager()->SetBlendMotion(Actor, ANIMRUNSLIDERIGHTSHOOT, ANIMRUNSLIDERIGHTSHOOT1, CCD->CameraManager()->GetTiltPercent());
						CCD->ActorManager()->SetNextMotion(Actor, ANIMRUNSLIDERIGHT);
					}
				}
				CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
				break;
			}
		}
		
		if(CCD->Weapons()->GetAttackFlag() && !m_crouch)
		{
			if(!m_run)
			{
// changed RF063
				if(!strcmp(Motion, ANIMSLIDERIGHTSHOOT))
					CCD->ActorManager()->SetBlendMot(Actor, ANIMSLIDERIGHTSHOOT, ANIMSLIDERIGHTSHOOT1, CCD->CameraManager()->GetTiltPercent());
				else
					CCD->ActorManager()->SetBlendMotion(Actor, ANIMSLIDERIGHTSHOOT, ANIMSLIDERIGHTSHOOT1, CCD->CameraManager()->GetTiltPercent());
				CCD->ActorManager()->SetNextMotion(Actor, ANIMSLIDERIGHT);
			}
			else
			{
				if(!strcmp(Motion, ANIMRUNSLIDERIGHTSHOOT))
					CCD->ActorManager()->SetBlendMot(Actor, ANIMRUNSLIDERIGHTSHOOT, ANIMRUNSLIDERIGHTSHOOT1, CCD->CameraManager()->GetTiltPercent());
				else
					CCD->ActorManager()->SetBlendMotion(Actor, ANIMRUNSLIDERIGHTSHOOT, ANIMRUNSLIDERIGHTSHOOT1, CCD->CameraManager()->GetTiltPercent());
				CCD->ActorManager()->SetNextMotion(Actor, ANIMRUNSLIDERIGHT);
			}
			break;
		}
		
		if(CCD->Weapons()->GetAttackFlag() && m_crouch)
		{
			if(!strcmp(Motion, ANIMSLIDECRIGHTSHOOT))
				CCD->ActorManager()->SetBlendMot(Actor, ANIMSLIDECRIGHTSHOOT, ANIMSLIDECRIGHTSHOOT1, CCD->CameraManager()->GetTiltPercent());
			else
				CCD->ActorManager()->SetBlendMotion(Actor, ANIMSLIDECRIGHTSHOOT, ANIMSLIDECRIGHTSHOOT1, CCD->CameraManager()->GetTiltPercent());
// end change RF063
			CCD->ActorManager()->SetNextMotion(Actor, ANIMSLIDECRIGHT);
			break;
		}
		
		if(!strcmp(Motion, ANIMSLIDERIGHT) && m_run)
		{
			CCD->ActorManager()->SetMotion(Actor, ANIMRUNSLIDERIGHT);
			break;
		}
		if(!strcmp(Motion, ANIMRUNSLIDERIGHT) && !m_run)
		{
			CCD->ActorManager()->SetMotion(Actor, ANIMSLIDERIGHT);
			break;
		}
		break;
	case MOVESLIDELEFT:
		m_JumpActive = false;
		if(!strcmp(Motion, ANIMIDLE) || !strcmp(Motion, ANIMW2IDLE)
			|| !strcmp(Motion, ANIMSHOOT) || !strcmp(Motion, ANIMAIM)
			|| !strcmp(Motion, ANIMWALK) || !strcmp(Motion, ANIMI2WALK)
			|| !strcmp(Motion, ANIMWALKSHOOT) || !strcmp(Motion, ANIMRUNSHOOT)
			|| !strcmp(Motion, ANIMRUN) || !strcmp(Motion, ANIMI2RUN)
			|| !strcmp(Motion, ANIMSLIDERIGHTSHOOT) || !strcmp(Motion, ANIMRUNSLIDERIGHTSHOOT)
			|| !strcmp(Motion, ANIMSLIDERIGHT) || !strcmp(Motion, ANIMRUNSLIDERIGHT))
		{
			if(!m_run)
			{
				if(!CCD->Weapons()->GetAttackFlag())
					CCD->ActorManager()->SetMotion(Actor, ANIMSLIDELEFT);
				else
				{
					CCD->ActorManager()->SetBlendMotion(Actor, ANIMSLIDELEFTSHOOT, ANIMSLIDELEFTSHOOT1, CCD->CameraManager()->GetTiltPercent());
					CCD->ActorManager()->SetNextMotion(Actor, ANIMSLIDELEFT);
				}
			}
			else
			{
				if(!CCD->Weapons()->GetAttackFlag())
					CCD->ActorManager()->SetMotion(Actor, ANIMRUNSLIDELEFT);
				else
				{
					CCD->ActorManager()->SetBlendMotion(Actor, ANIMRUNSLIDELEFTSHOOT, ANIMRUNSLIDELEFTSHOOT1, CCD->CameraManager()->GetTiltPercent());
					CCD->ActorManager()->SetNextMotion(Actor, ANIMRUNSLIDELEFT);
				}
			}
			break;
		}
		
		if(!strcmp(Motion, ANIMCIDLE) || !strcmp(Motion, ANIMC2IDLE)
			|| !strcmp(Motion, ANIMSLIDECRIGHT) || !strcmp(Motion, ANIMSLIDECRIGHTSHOOT)
			|| !strcmp(Motion, ANIMCRAWL) || !strcmp(Motion, ANIMCRAWLSHOOT)
			|| !strcmp(Motion, ANIMCSHOOT) || !strcmp(Motion, ANIMCAIM))
		{
			if(!CCD->Weapons()->GetAttackFlag())
				CCD->ActorManager()->SetMotion(Actor, ANIMSLIDECLEFT);
			else
			{
				CCD->ActorManager()->SetBlendMotion(Actor, ANIMSLIDECLEFTSHOOT, ANIMSLIDECLEFTSHOOT1, CCD->CameraManager()->GetTiltPercent());
				CCD->ActorManager()->SetNextMotion(Actor, ANIMSLIDECLEFT);
			}
			break;
		}
		
		if((!strcmp(Motion, ANIMSLIDELEFT) || !strcmp(Motion, ANIMRUNSLIDELEFT)) && m_crouch)
		{
			if(!CCD->Weapons()->GetAttackFlag())
				CCD->ActorManager()->SetMotion(Actor, ANIMSLIDECLEFT);
			else
			{
				CCD->ActorManager()->SetBlendMotion(Actor, ANIMSLIDECLEFTSHOOT, ANIMSLIDECLEFTSHOOT1, CCD->CameraManager()->GetTiltPercent());
				CCD->ActorManager()->SetNextMotion(Actor, ANIMSLIDECLEFT);
			}
			CCD->ActorManager()->SetAnimationHeight(Actor, ANIMCIDLE, true);
			break;
		}
		
		if((!strcmp(Motion, ANIMSLIDECLEFT) || !strcmp(Motion, ANIMSLIDECLEFTSHOOT)) && !m_crouch)
		{
			if(CCD->ActorManager()->CheckAnimation(Actor, ANIMIDLE)==GE_TRUE)
			{
				if(!m_run)
				{
					if(!CCD->Weapons()->GetAttackFlag())
						CCD->ActorManager()->SetMotion(Actor, ANIMSLIDELEFT);
					else
					{
						CCD->ActorManager()->SetBlendMotion(Actor, ANIMSLIDELEFTSHOOT, ANIMSLIDELEFTSHOOT1, CCD->CameraManager()->GetTiltPercent());
						CCD->ActorManager()->SetNextMotion(Actor, ANIMSLIDELEFT);
					}
				}
				else
				{
					if(!CCD->Weapons()->GetAttackFlag())
						CCD->ActorManager()->SetMotion(Actor, ANIMRUNSLIDELEFT);
					else
					{
						CCD->ActorManager()->SetBlendMotion(Actor, ANIMRUNSLIDELEFTSHOOT, ANIMRUNSLIDELEFTSHOOT1, CCD->CameraManager()->GetTiltPercent());
						CCD->ActorManager()->SetNextMotion(Actor, ANIMRUNSLIDELEFT);
					}
				}
				CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
				break;
			}
		}
		
		if(CCD->Weapons()->GetAttackFlag() && !m_crouch)
		{
			if(!m_run)
			{
// changed RF063
				if(!strcmp(Motion, ANIMSLIDELEFTSHOOT))
					CCD->ActorManager()->SetBlendMot(Actor, ANIMSLIDELEFTSHOOT, ANIMSLIDELEFTSHOOT1, CCD->CameraManager()->GetTiltPercent());
				else
					CCD->ActorManager()->SetBlendMotion(Actor, ANIMSLIDELEFTSHOOT, ANIMSLIDELEFTSHOOT1, CCD->CameraManager()->GetTiltPercent());
				CCD->ActorManager()->SetNextMotion(Actor, ANIMSLIDELEFT);
			}
			else
			{
				if(!strcmp(Motion, ANIMRUNSLIDELEFTSHOOT))
					CCD->ActorManager()->SetBlendMot(Actor, ANIMRUNSLIDELEFTSHOOT, ANIMRUNSLIDELEFTSHOOT1, CCD->CameraManager()->GetTiltPercent());
				else
					CCD->ActorManager()->SetBlendMotion(Actor, ANIMRUNSLIDELEFTSHOOT, ANIMRUNSLIDELEFTSHOOT1, CCD->CameraManager()->GetTiltPercent());
				CCD->ActorManager()->SetNextMotion(Actor, ANIMRUNSLIDELEFT);
			}
			break;
		}
		
		if(CCD->Weapons()->GetAttackFlag() && m_crouch)
		{
			if(!strcmp(Motion, ANIMSLIDECLEFTSHOOT))
				CCD->ActorManager()->SetBlendMot(Actor, ANIMSLIDECLEFTSHOOT, ANIMSLIDECLEFTSHOOT1, CCD->CameraManager()->GetTiltPercent());
			else
				CCD->ActorManager()->SetBlendMotion(Actor, ANIMSLIDECLEFTSHOOT, ANIMSLIDECLEFTSHOOT1, CCD->CameraManager()->GetTiltPercent());
// changed RF063
			CCD->ActorManager()->SetNextMotion(Actor, ANIMSLIDECLEFT);
			break;
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
// changed RF063
	fwrite(&m_PlayerViewPoint, sizeof(int), 1, SaveFD);
	geVec3d position = Position();
	fwrite(&position, sizeof(geVec3d), 1, SaveFD);
	CCD->ActorManager()->GetRotate(Actor, &position);
	fwrite(&position, sizeof(geVec3d), 1, SaveFD);
	fwrite(&m_JumpActive, sizeof(bool), 1, SaveFD);
	fwrite(&m_Jumping, sizeof(geBoolean), 1, SaveFD);
	fwrite(&m_run, sizeof(bool), 1, SaveFD);
	fwrite(&m_crouch, sizeof(bool), 1, SaveFD);
	fwrite(&m_oldcrouch, sizeof(bool), 1, SaveFD);
	fwrite(&m_lastdirection, sizeof(int), 1, SaveFD);
	fwrite(&Falling, sizeof(bool), 1, SaveFD);
	fwrite(&OldFalling, sizeof(bool), 1, SaveFD);
	fwrite(&FallStart, sizeof(geVec3d), 1, SaveFD);
	fwrite(&LastHealth, sizeof(int), 1, SaveFD);
	fwrite(&StaminaTime, sizeof(geFloat), 1, SaveFD);
	fwrite(&lighton, sizeof(bool), 1, SaveFD);
	fwrite(&CurrentLiteLife, sizeof(geFloat), 1, SaveFD);
	fwrite(&DecayLite, sizeof(bool), 1, SaveFD);
	fwrite(&CurrentLiteDecay, sizeof(geFloat), 1, SaveFD);
	fwrite(&m_LastMovementSpeed, sizeof(geFloat), 1, SaveFD);
	geFloat Level, Decay;
	for(int i=0;i<4;i++)
	{
		CCD->ActorManager()->GetForce(Actor, i, &position, &Level, &Decay);
		fwrite(&position, sizeof(geVec3d), 1, SaveFD);
		fwrite(&Level, sizeof(geFloat), 1, SaveFD);
		fwrite(&Decay, sizeof(geFloat), 1, SaveFD);
	}

// end change RF063	
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
// changed RF063
	fread(&m_PlayerViewPoint, sizeof(int), 1, RestoreFD);
	SwitchCamera(m_PlayerViewPoint);
	geVec3d position;
	fread(&position, sizeof(geVec3d), 1, RestoreFD);
	CCD->ActorManager()->Position(Actor, position);
	fread(&position, sizeof(geVec3d), 1, RestoreFD);
	CCD->ActorManager()->Rotate(Actor, position);
	fread(&m_JumpActive, sizeof(bool), 1, RestoreFD);
	fread(&m_Jumping, sizeof(geBoolean), 1, RestoreFD);
	fread(&m_run, sizeof(bool), 1, RestoreFD);
	fread(&m_crouch, sizeof(bool), 1, RestoreFD);
	fread(&m_oldcrouch, sizeof(bool), 1, RestoreFD);
	fread(&m_lastdirection, sizeof(int), 1, RestoreFD);
	fread(&Falling, sizeof(bool), 1, RestoreFD);
	fread(&OldFalling, sizeof(bool), 1, RestoreFD);
	fread(&FallStart, sizeof(geVec3d), 1, RestoreFD);
	fread(&LastHealth, sizeof(int), 1, RestoreFD);
	fread(&StaminaTime, sizeof(geFloat), 1, RestoreFD);
	fread(&lighton, sizeof(bool), 1, RestoreFD);
	fread(&CurrentLiteLife, sizeof(geFloat), 1, RestoreFD);
	fread(&DecayLite, sizeof(bool), 1, RestoreFD);
	fread(&CurrentLiteDecay, sizeof(geFloat), 1, RestoreFD);
	fread(&m_LastMovementSpeed, sizeof(geFloat), 1, RestoreFD);
	geFloat Level, Decay;
	for(int i=0;i<4;i++)
	{
		fread(&position, sizeof(geVec3d), 1, RestoreFD);
		fread(&Level, sizeof(geFloat), 1, RestoreFD);
		fread(&Decay, sizeof(geFloat), 1, RestoreFD);
		CCD->ActorManager()->SetForce(Actor, i, position, Level, Decay);	
	} 
// end change RF063	

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
// changed RF063
	int theZone, OldZone;
	geFloat fTemp;
	
	CCD->ActorManager()->GetActorZone(Actor, &theZone);
	CCD->ActorManager()->GetActorOldZone(Actor, &OldZone);
	if(OldZone>0 || theZone==kLiquidZone || theZone==kInLiquidZone)
	{
		Liquid *LQ = CCD->ActorManager()->GetLiquid(Actor);
		if(LQ)
		{
			m_CoeffSpeed = (float)LQ->SpeedCoeff/100.0f;
		}
	}
	else if(theZone & kLavaZone)
		m_CoeffSpeed = 0.55f;
	else if(theZone & kSlowMotionZone)
		m_CoeffSpeed = 0.25f;
	else if(theZone & kFastMotionZone)
		m_CoeffSpeed = 2.0f;
	
	//A little run wanted ?
	if ((m_Running) && !GetCrouch() && OldZone==0)		//Don't run when crouch
	{
		if(CCD->ActorManager()->Falling(Actor) != GE_TRUE)	//If we are not falling, adjust speed modifications
			m_CoeffSpeed *= 2.0f;
	}
// end change RF063	
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
// changed RF063
			if(OldZone==0)
			{
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
			}
			else
				m_Moving=MOVEIDLE;
			break;
		case MOVESLIDERIGHT:
			if(OldZone==0)
			{
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
			}
			else
				m_Moving=MOVEIDLE;
// end change RF063
			break;
		default:
			break;
		}
		
	}
	else
		Move(RGF_K_FORWARD, 0);
	
	return bPlayerMoved;
}

// changed RF063

void CPlayer::UseItem()
{
	geVec3d theRotation;
	geVec3d thePosition;
	geXForm3d Xf;
	geExtBox theBox;
	geVec3d Direction, Pos, Front, Back;
	GE_Collision Collision;

	theBox.Min.X = theBox.Min.Y = theBox.Min.Z = -10.0f * m_Scale;
	theBox.Max.X = theBox.Max.Y = theBox.Max.Z = 10.0f * m_Scale;

	if(m_PlayerViewPoint==FIRSTPERSON)
	{
		CCD->CameraManager()->GetRotation(&theRotation);
		CCD->CameraManager()->GetPosition(&thePosition);
		geXForm3d_SetIdentity(&Xf);
		geXForm3d_RotateZ(&Xf, theRotation.Z);
		geXForm3d_RotateX(&Xf, theRotation.X);
		geXForm3d_RotateY(&Xf, theRotation.Y);
		geXForm3d_Translate(&Xf, thePosition.X, thePosition.Y, thePosition.Z);
	}
	else
	{
		geActor_GetBoneTransform(Actor, NULL, &Xf );
		thePosition = Xf.Translation;
		CCD->ActorManager()->GetRotate(Actor, &theRotation);
		geVec3d CRotation, CPosition;
		CCD->CameraManager()->GetCameraOffset(&CPosition, &CRotation);
		geXForm3d_SetIdentity(&Xf);
		geXForm3d_RotateZ(&Xf, theRotation.Z);
		geXForm3d_RotateX(&Xf, theRotation.X-CCD->CameraManager()->GetTilt());
		geXForm3d_RotateY(&Xf, theRotation.Y);
		geXForm3d_Translate(&Xf, thePosition.X, thePosition.Y, thePosition.Z);
	}

	Pos = Xf.Translation;
	geXForm3d_GetIn(&Xf, &Direction);
	geVec3d_AddScaled (&Pos, &Direction, UseRange, &Back);
	geVec3d_AddScaled (&Pos, &Direction, 0.0f, &Front);

	Collision.Actor = NULL;
	Collision.Model = NULL;
	CCD->Collision()->IgnoreContents(false);
	if(CCD->Collision()->CheckForWCollision(&theBox.Min, &theBox.Max,
			Front, Back, &Collision, Actor))
	{
		if(Collision.Model)
		{
			if(CCD->Changelevel()->CheckChangeLevel(Collision.Model, true))
			{
				CCD->SetChangeLevel(true);	// We hit a change level
				return;
			}
			if(CCD->Doors()->HandleCollision(Collision.Model, false, true, Actor))
				return;
			if(CCD->Platforms()->HandleCollision(Collision.Model, false, true, Actor))
				return;
			if(CCD->Triggers()->HandleCollision(Collision.Model, false, true, Actor) == RGF_SUCCESS)
				return;
		}
		if(Collision.Actor)
		{
			if(CCD->Props()->HandleCollision(Collision.Actor, Actor, true, true) == RGF_SUCCESS)
				return;
		}
	}
}

// end change RF063