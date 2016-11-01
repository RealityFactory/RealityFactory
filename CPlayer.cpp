/************************************************************************************//**
 * @file CPlayer.cpp
 * @brief Player character encapsulation class
 *
 * This file contains the class implementation for the player character
 * (avatar) in an RGF-based game.
 * @author Ralph Deane
 * @bug Get rid of crashing due to missing entity or entity parameters
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"
#include "CFileManager.h"
#include "IniFile.h"
#include "CArmour.h"
#include "CHeadsUpDisplay.h"
#include "CCharacter.h"
#include "CCDAudio.h"
#include "CMIDIAudio.h"
#include "CLevel.h"
#include "CScriptManager.h"
#include "CLanguageManager.h"
#include "CPawnManager.h"

#include "CInventory.h"

#include "CAttribute.h"
#include "CAutoDoors.h"
#include "CChange.h"
#include "CChangeLevel.h"
#include "CDamage.h"
#include "CFixedCamera.h"
#include "CFlipBook.h"
#include "CLiftBelt.h"
#include "CMovingPlatforms.h"
#include "CStaticEntity.h"
#include "CTriggers.h"
#include "CConversationManager.h"

#include "RFSX/sxVector.h"
#include "RFSX/CEGUI/sxCEGUIVector2.h"

extern "C" void	DrawBoundBox(geWorld *World, const geVec3d *Pos, const geVec3d *Min, const geVec3d *Max);
extern geSound_Def *SPool_Sound(const char *SName);
extern geBitmap *TPool_Bitmap(const char *DefaultBmp, const char *DefaultAlpha,
							  const char *BName, const char *AName);

#define ANIMDIE					GetDieAnim()
#define ANIMINJURY				GetInjuryAnim()

#define ANIMI2WALKTIME			m_TranTime[I2WALKTIME]
#define ANIMI2RUNTIME			m_TranTime[I2RUNTIME]
#define ANIMW2IDLETIME			m_TranTime[W2IDLETIME]
#define ANIMC2IDLETIME			m_TranTime[C2IDLETIME]
#define ANIMCROUCH2IDLETIME		m_TranTime[CROUCH2IDLETIME]
#define ANIMIDLE2CROUCHTIME		m_TranTime[IDLE2CROUCHTIME]
#define ANIMSWIM2TREADTIME		m_TranTime[SWIM2TREADTIME]
#define ANIMTREAD2SWIMTIME		m_TranTime[TREAD2SWIMTIME]
#define ANIMIDLE2TREADTIME		m_TranTime[IDLE2TREADTIME]
#define ANIMTREAD2IDLETIME		m_TranTime[TREAD2IDLETIME]
#define ANIMSWIM2WALKTIME		m_TranTime[SWIM2WALKTIME]
#define ANIMWALK2SWIMTIME		m_TranTime[WALK2SWIMTIME]
#define ANIMJUMP2FALLTIME		m_TranTime[JUMP2FALLTIME]
#define ANIMJUMP2TREADTIME		m_TranTime[JUMP2TREADTIME]
#define ANIMFALL2TREADTIME		m_TranTime[FALL2TREADTIME]
#define ANIMSLIDE2CROUCHTIME	m_TranTime[SLIDE2CROUCHTIME]
#define ANIMSLIDE2IDLETIME		m_TranTime[SLIDE2IDLETIME]
#define ANIMFALL2CRAWLTIME		m_TranTime[FALL2CRAWLTIME]
#define ANIMFALL2WALKTIME		m_TranTime[FALL2WALKTIME]
#define ANIMFALL2JUMPTIME		m_TranTime[FALL2JUMPTIME]
#define ANIMWALK2JUMPTIME		m_TranTime[WALK2JUMPTIME]
#define ANIMWALK2CRAWLTIME		m_TranTime[WALK2CRAWLTIME]
#define ANIMCRAWL2WALKTIME		m_TranTime[CRAWL2WALKTIME]
#define ANIMIDLE2CRAWLTIME		m_TranTime[IDLE2CRAWLTIME]

/* ------------------------------------------------------------------------------------ */
// CPlayer
//
// Default constructor
/* ------------------------------------------------------------------------------------ */
CPlayer::CPlayer() :
	m_ViewPoint(FIRSTPERSON),
	m_JumpActive(false),
	m_FirstPersonView(true),
	m_LookMode(false),
	m_Moving(MOVEIDLE),
	m_Running(GE_FALSE),
	m_JumpSpeed(60.f),
	m_CurrentHeight(0.f),
	m_CurrentSpeed(3.f),
	m_SpeedCoeff(1.f),
	m_SpeedCoeffBackward(0.67f),
	m_SpeedCoeffSideward(0.75f),
	m_SpeedCoeffRun(2.f),
	m_Speed(3.f),
	m_StepHeight(16.f),
	m_LastMovementSpeed(0.f),
	m_Attr(NULL),
	m_PositionHistoryPtr(0),
	m_DefaultMotionHandle(NULL),
	m_Scale(1.f),
	m_SlideWalk(MOVEIDLE),
	m_Crouch(false),
	m_OldCrouch(false),
	m_Run(false),
	m_HeadBobbing(false),
	m_HeadBobSpeed(0.f),
	m_HeadBobLimit(0.f),
	m_LookFlags(0),
	m_LastDirection(0),
	m_Alive(true),
	m_RealJumping(false),
	m_LightActive(false),
	m_LightOn(true),
	m_LightEffect(-1),
	m_CurrentLiteLife(-1.0f),
	m_DecayLite(false),
	m_CurrentLiteDecay(0.f),
	m_LiteTime(0.f),
	m_LiteDecayTime(0.f),
	m_StaminaDelay(1.f),
	m_StaminaTime(0.f),
	m_Allow3rdLook(true),
	m_LastHealth(-1),
	m_Injured(false),
	m_Dying(false),
	m_Falling(false),
	m_OldFalling(false),
	m_FallInjure(false),
	m_MinFallDist(0.f),
	m_MaxFallDist(0.f),
	m_FallDamage(0.f),
	m_LiquidTime(0.f),
	m_InLiquid(-1),
	m_SwimSound(-1),
	m_SurfaceSound(-1),
	m_InLiquidSound(-1),
	m_OnLadder(false),
	m_PlayerName("Unnamed"),
	m_AlwaysRun(false),
	m_LockView(false),
	m_MonitorState(false),
	m_MonitorMode(false),
	m_RestoreOxy(true),
	m_DeathSpace(false),
	m_ContinueAfterDeath(true),
	m_SlideSlope(0.8f),
	m_SlideSpeed(40.f),
	m_StreamingAudio(NULL),
	m_FirstFrame(true),
	m_bSoundtrackLoops(false)
{
	// Initialize all state variables.  Note that the player avatar is not
	// ..scaled up at all!  Why?  This means the designer can make a level
	// ..that is, effectively, _immense_ in the level editor without running
	// ..up against the G3D 1.0 limit of 4096x4096x4096 maximum size.  The
	// ..player CAN be scaled when the environment is loaded, though.

	memset(m_ContentsHandles, 0, sizeof(geSound*)*16);
	m_ActorName[0] = 0;

	for(int sn=0; sn<20; ++sn)
	{
		m_StaminaDelay1[sn] = 1.0f;
		m_StaminaTime1[sn] = 0.0f;
	}

	m_DefaultMotion[0]		= NULL;
	m_DefaultMotion[1]		= NULL;
	m_DefaultMotion[2]		= NULL;

	for(int nTemp=0; nTemp<16; ++nTemp)
		for(int j=0; j<3; ++j)
			m_Contents[nTemp][j] = NULL;

	std::string pSetup("playersetup.ini");

	if(CCD->MenuManager()->GetUseSelect())
	{
		if(!CCD->MenuManager()->GetSelectedCharacter()->GetPlayerConfigFilename().empty())
			pSetup = CCD->MenuManager()->GetSelectedCharacter()->GetPlayerConfigFilename();
	}

	CIniFile iniFile(pSetup);

	if(!iniFile.ReadFile())
	{
		CCD->Log()->Error("Failed to open playersetup configuration file '" + pSetup + "'!");
		return;
	}

	std::string KeyName = iniFile.FindFirstKey();
	std::string value, vector;

	while(!KeyName.empty())
	{
		if(KeyName == "Animations")
		{
			m_Animations[IDLE]			= iniFile.GetValue(KeyName, "idle");
			m_Animations[WALK]			= iniFile.GetValue(KeyName, "walk");
			m_Animations[RUN]			= iniFile.GetValue(KeyName, "run");
			m_Animations[JUMP]			= iniFile.GetValue(KeyName, "jump");
			m_Animations[STARTJUMP]		= iniFile.GetValue(KeyName, "starttojump");
			m_Animations[FALL]			= iniFile.GetValue(KeyName, "fall");
			m_Animations[LAND]			= iniFile.GetValue(KeyName, "land");
			m_Animations[CLIMBIDLE]		= iniFile.GetValue(KeyName, "climbidle");
			m_Animations[CLIMBDOWN]		= iniFile.GetValue(KeyName, "climbdown");
			m_Animations[CLIMBUP]		= iniFile.GetValue(KeyName, "climbup");
			m_Animations[SLIDELEFT]		= iniFile.GetValue(KeyName, "slidetoleft");
			m_Animations[RUNSLIDELEFT]	= iniFile.GetValue(KeyName, "slideruntoleft");
			m_Animations[SLIDERIGHT]	= iniFile.GetValue(KeyName, "slidetoright");
			m_Animations[RUNSLIDERIGHT] = iniFile.GetValue(KeyName, "slideruntoright");
			m_Animations[CRAWL]			= iniFile.GetValue(KeyName, "crawl");
			m_Animations[CIDLE]			= iniFile.GetValue(KeyName, "crouchidle");
			m_Animations[CSTARTJUMP]	= iniFile.GetValue(KeyName, "crouchstarttojump");
			m_Animations[CLAND]			= iniFile.GetValue(KeyName, "crouchland");
			m_Animations[SLIDECLEFT]	= iniFile.GetValue(KeyName, "crawlslidetoleft");
			m_Animations[SLIDECRIGHT]	= iniFile.GetValue(KeyName, "crawlslidetoright");

			m_Animations[SHOOT1] = iniFile.GetValue(KeyName, "shootup");
			m_Animations[SHOOT] = iniFile.GetValue(KeyName, "shootdwn");
			m_Animations[AIM1] = iniFile.GetValue(KeyName, "aimup");
			m_Animations[AIM] = iniFile.GetValue(KeyName, "aimdwn");
			m_Animations[WALKSHOOT1] = iniFile.GetValue(KeyName, "walkshootup");
			m_Animations[WALKSHOOT] = iniFile.GetValue(KeyName, "walkshootdwn");
			m_Animations[RUNSHOOT1] = iniFile.GetValue(KeyName, "runshootup");
			m_Animations[RUNSHOOT] = iniFile.GetValue(KeyName, "runshootdwn");
			m_Animations[SLIDELEFTSHOOT1] = iniFile.GetValue(KeyName, "slidetoleftshootup");
			m_Animations[SLIDELEFTSHOOT] = iniFile.GetValue(KeyName, "slidetoleftshootdwn");
			m_Animations[RUNSLIDELEFTSHOOT1] = iniFile.GetValue(KeyName, "slideruntoleftshootup");
			m_Animations[RUNSLIDELEFTSHOOT] = iniFile.GetValue(KeyName, "slideruntoleftshootdwn");
			m_Animations[SLIDERIGHTSHOOT1] = iniFile.GetValue(KeyName, "slidetorightshootup");
			m_Animations[SLIDERIGHTSHOOT] = iniFile.GetValue(KeyName, "slidetorightshootdwn");
			m_Animations[RUNSLIDERIGHTSHOOT1] = iniFile.GetValue(KeyName, "slideruntorightshootup");
			m_Animations[RUNSLIDERIGHTSHOOT] = iniFile.GetValue(KeyName, "slideruntorightshootdwn");
			m_Animations[JUMPSHOOT1] = iniFile.GetValue(KeyName, "jumpshootup");
			m_Animations[JUMPSHOOT] = iniFile.GetValue(KeyName, "jumpshootdwn");
			m_Animations[FALLSHOOT1] = iniFile.GetValue(KeyName, "fallshootup");
			m_Animations[FALLSHOOT] = iniFile.GetValue(KeyName, "fallshootdwn");
			m_Animations[CAIM1] = iniFile.GetValue(KeyName, "crouchaimup");
			m_Animations[CAIM] = iniFile.GetValue(KeyName, "crouchaimdwn");
			m_Animations[CSHOOT1] = iniFile.GetValue(KeyName, "crouchshootup");
			m_Animations[CSHOOT] = iniFile.GetValue(KeyName, "crouchshootdwn");
			m_Animations[CRAWLSHOOT1] = iniFile.GetValue(KeyName, "crawlshootup");
			m_Animations[CRAWLSHOOT] = iniFile.GetValue(KeyName, "crawlshootdwn");
			m_Animations[SLIDECLEFTSHOOT1] = iniFile.GetValue(KeyName, "crawlslidetoleftshootup");
			m_Animations[SLIDECLEFTSHOOT] = iniFile.GetValue(KeyName, "crawlslidetoleftshootdwn");
			m_Animations[SLIDECRIGHTSHOOT1] = iniFile.GetValue(KeyName, "crawlslidetorightshootup");
			m_Animations[SLIDECRIGHTSHOOT] = iniFile.GetValue(KeyName, "crawlslidetorightshootdwn");

			m_Animations[SWIM] = iniFile.GetValue(KeyName, "swim");
			m_Animations[TREADWATER] = iniFile.GetValue(KeyName, "treadwater");

			m_Animations[I2WALK]		= iniFile.GetValue(KeyName, "idletowalk");
			m_Animations[I2RUN]			= iniFile.GetValue(KeyName, "idletorun");
			m_Animations[W2IDLE]		= iniFile.GetValue(KeyName, "walktoidle");
			m_Animations[C2IDLE]		= iniFile.GetValue(KeyName, "crawltoidle");
			m_Animations[CROUCH2IDLE]	= iniFile.GetValue(KeyName, "crouchtoidle");
			m_Animations[IDLE2CROUCH]	= iniFile.GetValue(KeyName, "idletocrouch");
			m_Animations[SWIM2TREAD]	= iniFile.GetValue(KeyName, "swimtotread");
			m_Animations[TREAD2SWIM]	= iniFile.GetValue(KeyName, "treadtoswim");
			m_Animations[IDLE2TREAD]	= iniFile.GetValue(KeyName, "idletotread");
			m_Animations[SWIM2WALK]		= iniFile.GetValue(KeyName, "swimtowalk");
			m_Animations[WALK2SWIM]		= iniFile.GetValue(KeyName, "walktoswim");
			m_Animations[TREAD2IDLE]	= iniFile.GetValue(KeyName, "treadtoidle");
			m_Animations[JUMP2FALL]		= iniFile.GetValue(KeyName, "jumptofall");
			m_Animations[JUMP2TREAD]	= iniFile.GetValue(KeyName, "jumptotread");
			m_Animations[FALL2TREAD]	= iniFile.GetValue(KeyName, "falltotread");
			m_Animations[FALL2CRAWL]	= iniFile.GetValue(KeyName, "falltocrawl");
			m_Animations[FALL2WALK]		= iniFile.GetValue(KeyName, "falltowalk");
			m_Animations[FALL2JUMP]		= iniFile.GetValue(KeyName, "falltojump");
			m_Animations[WALK2JUMP]		= iniFile.GetValue(KeyName, "walktojump");
			m_Animations[WALK2CRAWL]	= iniFile.GetValue(KeyName, "walktocrawl");
			m_Animations[CRAWL2WALK]	= iniFile.GetValue(KeyName, "crawltowalk");
			m_Animations[IDLE2CRAWL]	= iniFile.GetValue(KeyName, "idletocrawl");
			m_Animations[AIM2CROUCH]	= iniFile.GetValue(KeyName, "aimtocrouch");
			m_Animations[CROUCH2AIM]	= iniFile.GetValue(KeyName, "crouchtoaim");
			m_Animations[W2TREAD]		= iniFile.GetValue(KeyName, "walktotread");
			m_Animations[FALL2RUN]		= iniFile.GetValue(KeyName, "falltorun");
			m_Animations[CRAWL2RUN]		= iniFile.GetValue(KeyName, "crawltorun");

			m_Animations[WALKBACK]			= iniFile.GetValue(KeyName, "walkback");
			m_Animations[RUNBACK]			= iniFile.GetValue(KeyName, "runback");
			m_Animations[CRAWLBACK]			= iniFile.GetValue(KeyName, "crawlback");
			m_Animations[WALKSHOOT1BACK]	= iniFile.GetValue(KeyName, "walkshootupback");
			m_Animations[WALKSHOOTBACK]		= iniFile.GetValue(KeyName, "walkshootdwnback");
			m_Animations[RUNSHOOT1BACK]		= iniFile.GetValue(KeyName, "runshootupback");
			m_Animations[RUNSHOOTBACK]		= iniFile.GetValue(KeyName, "runshootdwnback");
			m_Animations[CRAWLSHOOT1BACK]	= iniFile.GetValue(KeyName, "crawlshootupback");
			m_Animations[CRAWLSHOOTBACK]	= iniFile.GetValue(KeyName, "crawlshootdwnback");
			m_Animations[SWIMBACK]			= iniFile.GetValue(KeyName, "swimback");

			char strip[256], *temp;

			value = iniFile.GetValue(KeyName, "die");
			m_DieAnimAmt = 0;
			if(!value.empty())
			{
				strcpy(strip, value.c_str());
				temp = strtok(strip, " \n");

				while(temp)
				{
					m_DieAnim[m_DieAnimAmt] = temp;
					++m_DieAnimAmt;

					if(m_DieAnimAmt == 5)
						break;

					temp = strtok(NULL, " \n");
				}
			}

			value = iniFile.GetValue(KeyName, "injury");
			m_InjuryAnimAmt = 0;
			if(!value.empty())
			{
				strcpy(strip, value.c_str());
				temp = strtok(strip, " \n");

				while(temp)
				{
					m_InjuryAnim[m_InjuryAnimAmt] = temp;
					++m_InjuryAnimAmt;

					if(m_InjuryAnimAmt == 5)
						break;
					temp = strtok(NULL, " \n");
				}
			}

			m_TranTime[I2WALKTIME]		= static_cast<float>(iniFile.GetValueF(KeyName, "idletowalktime"));
			m_TranTime[I2RUNTIME]		= static_cast<float>(iniFile.GetValueF(KeyName, "idletoruntime"));
			m_TranTime[W2IDLETIME]		= static_cast<float>(iniFile.GetValueF(KeyName, "walktoidletime"));
			m_TranTime[C2IDLETIME]		= static_cast<float>(iniFile.GetValueF(KeyName, "crawltoidletime"));
			m_TranTime[CROUCH2IDLETIME]	= static_cast<float>(iniFile.GetValueF(KeyName, "crouchtoidletime"));
			m_TranTime[IDLE2CROUCHTIME]	= static_cast<float>(iniFile.GetValueF(KeyName, "idletocrouchtime"));
			m_TranTime[SWIM2TREADTIME]	= static_cast<float>(iniFile.GetValueF(KeyName, "swimtotreadtime"));
			m_TranTime[TREAD2SWIMTIME]	= static_cast<float>(iniFile.GetValueF(KeyName, "treadtoswimtime"));
			m_TranTime[IDLE2TREADTIME]	= static_cast<float>(iniFile.GetValueF(KeyName, "idletotreadtime"));
			m_TranTime[TREAD2IDLETIME]	= static_cast<float>(iniFile.GetValueF(KeyName, "treadtoidletime"));
			m_TranTime[SWIM2WALKTIME]	= static_cast<float>(iniFile.GetValueF(KeyName, "swimtowalktime"));
			m_TranTime[WALK2SWIMTIME]	= static_cast<float>(iniFile.GetValueF(KeyName, "walktoswimtime"));
			m_TranTime[JUMP2FALLTIME]	= static_cast<float>(iniFile.GetValueF(KeyName, "jumptofalltime"));
			m_TranTime[JUMP2TREADTIME]	= static_cast<float>(iniFile.GetValueF(KeyName, "jumptotreadtime"));
			m_TranTime[FALL2TREADTIME]	= static_cast<float>(iniFile.GetValueF(KeyName, "falltotreadtime"));
			m_TranTime[SLIDE2CROUCHTIME]= static_cast<float>(iniFile.GetValueF(KeyName, "slidetocrouchtime"));
			m_TranTime[SLIDE2IDLETIME]	= static_cast<float>(iniFile.GetValueF(KeyName, "slidetoidletime"));
			m_TranTime[FALL2CRAWLTIME]	= static_cast<float>(iniFile.GetValueF(KeyName, "falltocrawltime"));
			m_TranTime[FALL2WALKTIME]	= static_cast<float>(iniFile.GetValueF(KeyName, "falltowalktime"));
			m_TranTime[FALL2JUMPTIME]	= static_cast<float>(iniFile.GetValueF(KeyName, "falltojumptime"));
			m_TranTime[WALK2JUMPTIME]	= static_cast<float>(iniFile.GetValueF(KeyName, "walktojumptime"));
			m_TranTime[WALK2CRAWLTIME]	= static_cast<float>(iniFile.GetValueF(KeyName, "walktocrawltime"));
			m_TranTime[CRAWL2WALKTIME]	= static_cast<float>(iniFile.GetValueF(KeyName, "crawltowalktime"));
			m_TranTime[IDLE2CRAWLTIME]	= static_cast<float>(iniFile.GetValueF(KeyName, "idletocrawltime"));
		}

		if(KeyName == "Sounds")
		{
			char strip[256], *temp;

			m_SpeakBone = iniFile.GetValue(KeyName, "attachtobone");

			value = iniFile.GetValue(KeyName, "die");
			m_DieSoundAmt = 0;
			strcpy(strip, value.c_str());
			temp = strtok(strip, " \n");
			while(temp)
			{
				m_DieSound[m_DieSoundAmt] = SPool_Sound(temp);
				++m_DieSoundAmt;
				if(m_DieSoundAmt == 5)
					break;
				temp = strtok(NULL, " \n");
			}

			value = iniFile.GetValue(KeyName, "injury");
			m_InjurySoundAmt = 0;
			strcpy(strip, value.c_str());
			temp = strtok(strip, " \n");
			while(temp)
			{
				m_InjurySound[m_InjurySoundAmt] = SPool_Sound(temp);
				++m_InjurySoundAmt;
				if(m_InjurySoundAmt == 5)
					break;
				temp = strtok(NULL, " \n");
			}

			value = iniFile.GetValue(KeyName, "land");
			m_LandSoundAmt = 0;
			strcpy(strip, value.c_str());
			temp = strtok(strip, " \n");
			while(temp)
			{
				m_LandSound[m_LandSoundAmt] = SPool_Sound(temp);
				++m_LandSoundAmt;
				if(m_LandSoundAmt == 5)
					break;
				temp = strtok(NULL," \n");
			}
		}

		if(KeyName == "Light")
		{
			//geVec3d convert;
			m_LiteIntensity	= static_cast<float>(iniFile.GetValueF(KeyName, "intensity"));
			m_LiteRadiusMin	= static_cast<float>(iniFile.GetValueF(KeyName, "minimumradius"));
			m_LiteRadiusMax	= static_cast<float>(iniFile.GetValueF(KeyName, "maximumradius"));
			m_LiteLife		= static_cast<float>(iniFile.GetValueF(KeyName, "lifetime"));
			m_LiteDecay		= static_cast<float>(iniFile.GetValueF(KeyName, "decaytime"));

			vector = iniFile.GetValue(KeyName, "minimumcolor");
			if(vector != "")
			{
				m_LiteColorMin = ToRGBA(vector);
			}
			vector = iniFile.GetValue(KeyName, "maximumcolor");
			if(!vector.empty())
			{
				m_LiteColorMax = ToRGBA(vector);
			}

			vector = iniFile.GetValue(KeyName, "offsetangles");
			if(!vector.empty())
			{
				m_LiteOffset = ToVec3d(vector);
			}
			geVec3d_Scale(&m_LiteOffset, GE_PIOVER180, &m_LiteOffset);
			m_LiteSpot = false;
			if(iniFile.GetValue(KeyName, "spotlight") == "true")
				m_LiteSpot = true;
			m_LiteArc = static_cast<float>(iniFile.GetValueF(KeyName, "arc"));
			m_LiteStyle = iniFile.GetValueI(KeyName, "style");

			m_LiteBone = iniFile.GetValue(KeyName, "attachtobone");
		}

		if(KeyName == "Misc")
		{
			m_Icon = iniFile.GetValue(KeyName, "icon");

			m_StaminaName = "stamina";
			value = iniFile.GetValue(KeyName, "attributename");
			if(!value.empty())
				m_StaminaName = value;

			char szSName[64];
			for(int sn=0; sn<20; ++sn)
			{
				sprintf(szSName, "attributename%d", sn+1);
				value = iniFile.GetValue(KeyName, szSName);
				if(!value.empty())
					m_StaminaName1[sn] = value;
				sprintf(szSName, "recoverytime%d", sn+1);
				m_StaminaDelay1[sn] = static_cast<float>(iniFile.GetValueF(KeyName, szSName));
			}

			m_FallDamageAttr = "health";
			value = iniFile.GetValue(KeyName, "falldamageattribute");
			if(!value.empty())
				m_FallDamageAttr = value;

			m_StaminaDelay	= static_cast<float>(iniFile.GetValueF(KeyName, "recoverytime"));
			m_MinFallDist	= static_cast<float>(iniFile.GetValueF(KeyName, "minimumfalldistance"));
			m_MaxFallDist	= static_cast<float>(iniFile.GetValueF(KeyName, "maximumfalldistance"));
			m_FallDamage	= static_cast<float>(iniFile.GetValueF(KeyName, "falldamage"));

			m_UseRange = static_cast<float>(iniFile.GetValueF(KeyName, "userange"));
			m_BoxWidth = static_cast<float>(iniFile.GetValueF(KeyName, "boxsize"));

			m_AlwaysRun = false;
			if(iniFile.GetValue(KeyName, "alwaysrun") == "true")
				m_AlwaysRun = true;

			m_NoCrouchJump = false;
			if(iniFile.GetValue(KeyName, "allowcrouchjump") == "false")
				m_NoCrouchJump = true;

			if(iniFile.GetValue(KeyName, "restartafterdeath") == "false")
				m_ContinueAfterDeath = false;

			m_JumpOnDamageAttr = "health";
			value = iniFile.GetValue(KeyName, "jumpondamageattribute");
			if(!value.empty())
				m_JumpOnDamageAttr = value;

			m_MinJumpOnDist	= static_cast<float>(iniFile.GetValueF(KeyName, "minimumjumpondistance"));
			m_MaxJumpOnDist	= static_cast<float>(iniFile.GetValueF(KeyName, "maximumjumpondistance"));
			m_JumpOnDamage	= static_cast<float>(iniFile.GetValueF(KeyName, "jumpondamage"));

			m_Mirror = 0;
			if(iniFile.GetValue(KeyName, "mirror1st") == "true")
				m_Mirror = GE_ACTOR_RENDER_MIRRORS;

			m_ChangeMaterial = iniFile.GetValue(KeyName, "changematerial");

			m_FillColor.r = m_FillColor.g = m_FillColor.b = 0.0f;
			m_FillColor.a = 255.0f;
			m_AmbientColor.r = m_AmbientColor.g = m_AmbientColor.b = 0.0f;
			m_AmbientColor.a = 255.0f;

			vector = iniFile.GetValue(KeyName, "fillcolor");
			if(!vector.empty())
			{
				m_FillColor = ToRGBA(vector);
			}
			vector = iniFile.GetValue(KeyName, "ambientcolor");
			if(!vector.empty())
			{
				m_AmbientColor = ToRGBA(vector);
			}

			m_AmbientLightFromFloor = true;
			if(iniFile.GetValue(KeyName, "ambientlightfromfloor") == "false")
				m_AmbientLightFromFloor = false;

			m_EnvironmentMapping = false;
			if(iniFile.GetValue(KeyName, "environmentmapping") == "true")
			{
				m_EnvironmentMapping = true;
				m_AllMaterial = false;
				if(iniFile.GetValue(KeyName, "allmaterial") == "true")
					m_AllMaterial = true;
				m_PercentMapping = static_cast<float>(iniFile.GetValueF(KeyName, "percentmapping"));
				m_PercentMaterial = static_cast<float>(iniFile.GetValueF(KeyName, "percentmaterial"));
			}

			if(!iniFile.GetValue(KeyName, "speedcoeffbackward").empty())
				m_SpeedCoeffBackward = static_cast<float>(iniFile.GetValueF(KeyName, "speedcoeffbackward")) * 0.01f;

			if(!iniFile.GetValue(KeyName, "speedcoeffsideward").empty())
				m_SpeedCoeffSideward = static_cast<float>(iniFile.GetValueF(KeyName, "speedcoeffsideward")) * 0.01f;

			if(!iniFile.GetValue(KeyName, "speedcoeffrun").empty())
				m_SpeedCoeffRun = static_cast<float>(iniFile.GetValueF(KeyName, "speedcoeffrun")) * 0.01f;
		}

		KeyName = iniFile.FindNextKey();
	}

	m_Actor = NULL;

#ifdef _DEBUG
	InitMHT();
#endif
}

/* ------------------------------------------------------------------------------------ */
// ~CPlayer
//
// Default destructor
/* ------------------------------------------------------------------------------------ */
CPlayer::~CPlayer()
{
	// Release all the environmental audio sounds
	if(m_DefaultMotion[0] != NULL)
		geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), m_DefaultMotion[0]);

	if(m_DefaultMotion[1] != NULL)
		geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), m_DefaultMotion[1]);

	if(m_DefaultMotion[2] != NULL)
		geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), m_DefaultMotion[2]);

	for(int nTemp=0; nTemp<16; ++nTemp)
	{
		for(int j=0; j<3; ++j)
		{
			if(m_Contents[nTemp][j] != NULL)
				geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), m_Contents[nTemp][j]);
		}
	}

	if(m_StreamingAudio)
	{
		m_StreamingAudio->Delete();
		delete m_StreamingAudio;
	}

	if(m_Actor)
	{
		geActor_Destroy(&m_Actor);
		m_Actor = NULL;
	}
}

/* ------------------------------------------------------------------------------------ */
// LoadAvatar
//
// Load the actor file (.ACT) to be used as the players avatar.
// ..In normal, first-person use this won't actually be seen except in mirrors.
/* ------------------------------------------------------------------------------------ */
int CPlayer::LoadAvatar(const std::string& file, const std::string& name)
{
	if(CCD->MenuManager()->GetUseSelect())
	{
		strcpy(m_ActorName, CCD->MenuManager()->GetSelectedCharacter()->GetActorFilename().c_str());
		m_Actor = CCD->ActorManager()->LoadActor(m_ActorName, NULL);

		SetPlayerName(CCD->MenuManager()->GetSelectedCharacter()->GetName());
		m_Icon = CCD->MenuManager()->GetSelectedCharacter()->GetIcon();

		if(!m_Actor)
		{
			CCD->Log()->Critical("File %s - Line %d: Missing Character Actor '%s'",
									__FILE__, __LINE__, m_ActorName);
			delete CCD;
			exit(-333);
		}
	}
	else
	{
		geEntity_EntitySet *pSet;
		geEntity *pEntity;

		pSet = geWorld_GetEntitySet(CCD->World(), "PlayerSetup");

		if(!pSet)
		{
			CCD->Log()->Critical("File %s - Line %d: Missing PlayerSetup", __FILE__, __LINE__);
			delete CCD;
			exit(-333);
		}

		pEntity = geEntity_EntitySetGetNextEntity(pSet, NULL);
		PlayerSetup *pSetup = static_cast<PlayerSetup*>(geEntity_GetUserData(pEntity));

		if(EffectC_IsStringNull(pSetup->ActorName))
		{
			strcpy(m_ActorName, file.c_str());
			m_Actor = CCD->ActorManager()->LoadActor(file.c_str(), NULL);
		}
		else
		{
			strcpy(m_ActorName, pSetup->ActorName);
			m_Actor = CCD->ActorManager()->LoadActor(pSetup->ActorName, NULL);
		}

		if(!m_Actor)
		{
			if(EffectC_IsStringNull(pSetup->ActorName))
				CCD->Log()->Critical("File %s - Line %d: Missing Player Actor '%s'",
										__FILE__, __LINE__, file.c_str());
			else
				CCD->Log()->Critical("File %s - Line %d: Missing Player Actor '%s'",
										__FILE__, __LINE__, pSetup->ActorName);

			delete CCD;
			exit(-333);
		}

		SetPlayerName(name);
	}

	if(CCD->MenuManager()->GetUseNameSelect())
		SetPlayerName(CCD->MenuManager()->GetSelectedName());

	if(!m_ChangeMaterial.empty())
		CCD->ActorManager()->ChangeMaterial(m_Actor, m_ChangeMaterial);

	if(m_EnvironmentMapping)
		SetEnvironmentMapping(m_Actor, true, m_AllMaterial, m_PercentMapping, m_PercentMaterial);

	CCD->CameraManager()->BindToActor(m_Actor);							// Bind the camera

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// LoadConfiguration
//
// Loads the attribute configuration from the player configuration file
// ..defined in the PlayerSetup entity.
/* ------------------------------------------------------------------------------------ */
int CPlayer::LoadConfiguration()
{
	CCD->Log()->Debug("Loading Attributes and Player Configuration from PlayerSetup.ini");

	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	// Load environmental audio
	CCD->Log()->Debug("Loading Environmental Audio");
	LoadEnvironmentalAudio();

	CCD->Log()->Debug("Parsing PlayerSetup Entity");

	pSet = geWorld_GetEntitySet(CCD->World(), "PlayerSetup");

	if(!pSet)
	{
		CCD->Log()->Critical("File %s - Line %d: Missing PlayerSetup", __FILE__, __LINE__);
		delete CCD;
		exit(-333);
	}

	// Ok, get the setup information. There should only be one, so
	// we'll just take the first one we run into.
	pEntity = geEntity_EntitySetGetNextEntity(pSet, NULL);
	PlayerSetup *pSetup = static_cast<PlayerSetup*>(geEntity_GetUserData(pEntity));

	m_ViewPoint		= pSetup->LevelViewPoint;
	m_LockView		= pSetup->LockView;
	m_MonitorMode	= pSetup->MovieMode;
	m_Scale			= pSetup->PlayerScaleFactor;

	if(m_MonitorMode)
		CCD->SetKeyPaused(true);

	geVec3d rotation;
	float shadowSize;
	float shadowAlpha;
	std::string shadowBitmap;
	std::string shadowAlphamap;
	geBoolean useProjectedShadows, useStencilShadows;

	CCD->Log()->Debug("Initialize Player Data");

	float actScale;

	if(CCD->MenuManager()->GetUseSelect())
	{
		actScale			= CCD->MenuManager()->GetSelectedCharacter()->GetActorScale();
		rotation			= CCD->MenuManager()->GetSelectedCharacter()->GetRotation();
		shadowSize			= CCD->MenuManager()->GetSelectedCharacter()->GetShadowSize();
		m_FillColor			= CCD->MenuManager()->GetSelectedCharacter()->GetFillColor();
		m_AmbientColor		= CCD->MenuManager()->GetSelectedCharacter()->GetAmbientColor();
		m_AmbientLightFromFloor = CCD->MenuManager()->GetSelectedCharacter()->GetAmbientLightFromFloor();
		CCD->ActorManager()->SetAnimationSpeed(m_Actor, CCD->MenuManager()->GetSelectedCharacter()->GetAnimationSpeed());
		shadowAlpha			= CCD->MenuManager()->GetSelectedCharacter()->GetShadowAlpha();
		shadowBitmap		= CCD->MenuManager()->GetSelectedCharacter()->GetShadowBitmap();
		shadowAlphamap		= CCD->MenuManager()->GetSelectedCharacter()->GetShadowAlphamap();
		useProjectedShadows = CCD->MenuManager()->GetSelectedCharacter()->GetUseProjectedShadows();
		useStencilShadows	= CCD->MenuManager()->GetSelectedCharacter()->GetUseStencilShadows();
	}
	else
	{
		actScale			= pSetup->ActorScaleFactor;
		rotation.X			= pSetup->ActorRotation.X * GE_PIOVER180;
		rotation.Y			= pSetup->ActorRotation.Y * GE_PIOVER180;
		rotation.Z			= pSetup->ActorRotation.Z * GE_PIOVER180;
		shadowSize			= pSetup->ShadowSize;
		shadowAlpha			= pSetup->ShadowAlpha;
		shadowBitmap		= pSetup->ShadowBitmap;
		shadowAlphamap		= pSetup->ShadowAlphamap;
		useProjectedShadows = pSetup->UseProjectedShadows;
		useStencilShadows	= pSetup->UseStencilShadows;
	}

	CCD->ActorManager()->SetAligningRotation(m_Actor, rotation);
	CCD->ActorManager()->SetTilt(m_Actor, false);
	CCD->ActorManager()->SetBoxChange(m_Actor, false);
	CCD->ActorManager()->SetDefaultMotion(m_Actor, m_Animations[IDLE]);
	CCD->ActorManager()->SetMotion(m_Actor, m_Animations[IDLE]);
	CCD->ActorManager()->SetScale(m_Actor, m_Scale * actScale);
	CCD->ActorManager()->SetBoundingBox(m_Actor, m_Animations[IDLE]);
	CCD->ActorManager()->SetType(m_Actor, ENTITY_ACTOR);
	CCD->ActorManager()->SetActorDynamicLighting(m_Actor, m_FillColor, m_AmbientColor, m_AmbientLightFromFloor);
	CCD->ActorManager()->SetShadow(m_Actor, shadowSize);

	if(pSetup->ShadowAlpha > 0.0f)
		CCD->ActorManager()->SetShadowAlpha(m_Actor, shadowAlpha);

	if(!shadowBitmap.empty())
		CCD->ActorManager()->SetShadowBitmap(m_Actor, TPool_Bitmap(shadowBitmap.c_str(), shadowAlphamap.c_str(), NULL, NULL));

	geExtBox box;
	CCD->ActorManager()->GetBoundingBox(m_Actor, &box);
	m_CurrentHeight = box.Max.Y;

	if(m_BoxWidth > 0.0f)
		CCD->ActorManager()->SetBBox(m_Actor, m_BoxWidth, -m_CurrentHeight*2.0f, m_BoxWidth);
	else
		m_BoxWidth = box.Max.X - box.Min.X;

	int nFlags = 0;
	geVec3d theRotation = {0.0f, 0.0f, 0.0f};
	CCD->ActorManager()->Rotate(m_Actor, theRotation);
	geVec3d theTranslation = {0.0f, 0.0f, 0.0f};

	switch(m_ViewPoint)
	{
	case FIRSTPERSON:
	default:
		nFlags = kCameraTrackPosition + kCameraTrackRotation;
		theTranslation.Y = m_CurrentHeight;
		if(CCD->CameraManager()->GetViewHeight() != -1.0f)
			theTranslation.Y = CCD->CameraManager()->GetViewHeight() * m_Scale;
		rotation.X = rotation.Z = 0.0f;
		// Set offset
		CCD->CameraManager()->SetCameraOffset(theTranslation, rotation);
		// no projected shadows in 1st person
		CCD->ActorManager()->SetProjectedShadows(m_Actor, false);
		CCD->ActorManager()->SetStencilShadows(m_Actor, GE_FALSE);
		SwitchToFirstPerson();
		break;
	case THIRDPERSON:
		nFlags = kCameraTrackThirdPerson | kCameraTrackFree;
		rotation.X = CCD->CameraManager()->GetPlayerAngleUp();
		theTranslation.Y = m_CurrentHeight;
		if(CCD->CameraManager()->GetPlayerHeight() != -1.0f)
			theTranslation.Y = CCD->CameraManager()->GetPlayerHeight()*m_Scale;
		theTranslation.Z = CCD->CameraManager()->GetPlayerDistance();
		// Set offset
		CCD->CameraManager()->SetCameraOffset(theTranslation, rotation);
		CCD->ActorManager()->SetProjectedShadows(m_Actor, useProjectedShadows);
		CCD->ActorManager()->SetStencilShadows(m_Actor, useStencilShadows);

		m_Allow3rdLook = CCD->CameraManager()->GetPlayerAllowLook();
		SwitchToThirdPerson();
		CCD->CameraManager()->ResetCamera();
		break;
	case ISOMETRIC:
		nFlags = kCameraTrackIso;
		theRotation.X = CCD->CameraManager()->GetIsoAngleUp();
		theRotation.Y = CCD->CameraManager()->GetIsoAngleAround();
		theTranslation.Y = m_CurrentHeight;
		if(CCD->CameraManager()->GetIsoHeight() != -1.0f)
			theTranslation.Y = CCD->CameraManager()->GetIsoHeight() * m_Scale;
		theTranslation.Z = CCD->CameraManager()->GetIsoDistance();
		// Set offset
		CCD->CameraManager()->SetCameraOffset(theTranslation, theRotation);
		CCD->ActorManager()->SetProjectedShadows(m_Actor, useProjectedShadows);
		CCD->ActorManager()->SetStencilShadows(m_Actor, useStencilShadows);
		SwitchToThirdPerson();
		CCD->CameraManager()->ResetCamera();
		break;
	case FIXEDCAMERA:
		if(CCD->Level()->FixedCameras()->GetNumber() == 0)
		{
			CCD->Log()->Warning("File %s - Line %d: No Fixed Cameras in Level", __FILE__, __LINE__);

			m_ViewPoint = THIRDPERSON;
			nFlags = kCameraTrackThirdPerson | kCameraTrackFree;
			theTranslation.Y = m_CurrentHeight;
			if(CCD->CameraManager()->GetPlayerHeight() != -1.0f)
				theTranslation.Y = CCD->CameraManager()->GetPlayerHeight() * m_Scale;
			theTranslation.Z = CCD->CameraManager()->GetPlayerDistance();
			// Set offset
			CCD->CameraManager()->SetCameraOffset(theTranslation, rotation);
			m_Allow3rdLook = CCD->CameraManager()->GetPlayerAllowLook();
		}
		else
		{
			nFlags = kCameraTrackFixed;
			CCD->CameraManager()->Unbind();
		}

		CCD->ActorManager()->SetProjectedShadows(m_Actor, useProjectedShadows);
		CCD->ActorManager()->SetStencilShadows(m_Actor, useStencilShadows);

		SwitchToThirdPerson();
		CCD->CameraManager()->ResetCamera();
		break;
	}

	// Head bob setup
	m_HeadBobSpeed = pSetup->HeadBobSpeed;
	m_HeadBobLimit = pSetup->HeadBobLimit;
	m_HeadBobbing = pSetup->HeadBobbing;

	if(pSetup->HeadBobbing == GE_TRUE)
		CCD->CameraManager()->EnableHeadBob(true);
	else
		CCD->CameraManager()->EnableHeadBob(false);

	CCD->CameraManager()->SetTrackingFlags(nFlags);		// Set tracking flags
	CCD->ActorManager()->SetAutoStepUp(m_Actor, true);	// Allow stepping up
	CCD->ActorManager()->SetStepHeight(m_Actor, 32.0f);	// Set step height

	m_Speed = m_CurrentSpeed *= m_Scale;
	m_JumpSpeed *= m_Scale;

	if(m_MonitorMode)
	{
		CCD->ActorManager()->SetShadow(m_Actor, 0.0f);
		CCD->ActorManager()->SetNoCollide(m_Actor);
		CCD->ActorManager()->SetActorFlags(m_Actor, 0);
		CCD->HUD()->Deactivate();
	}

	m_RealJumping = pSetup->RealJumping;

	if(pSetup->JumpSpeed != 0.0f)
		m_JumpSpeed = pSetup->JumpSpeed;			// Over-ride default jump speed

	if(pSetup->Speed != 0.0f)
		m_Speed = m_CurrentSpeed = pSetup->Speed;	// Over-ride default speed

	if(pSetup->StepHeight != 0.0f)
		m_StepHeight = pSetup->StepHeight;		// Over-ride default step-up height

	if(pSetup->SlideSlope != 0.0f)
		m_SlideSlope = pSetup->SlideSlope;

	if(pSetup->SlideSpeed != 0.0f)
		m_SlideSpeed = pSetup->SlideSpeed;


	CCD->ActorManager()->SetStepHeight(m_Actor, m_StepHeight);
	CCD->ActorManager()->SetGravity(m_Actor, CCD->Level()->GetGravity());

	std::string AttrInfo = pSetup->AttributeInfoFile;

	if(CCD->MenuManager()->GetUseSelect())
	{
		if(!CCD->MenuManager()->GetSelectedCharacter()->GetAttributeConfigFilename().empty())
			AttrInfo = CCD->MenuManager()->GetSelectedCharacter()->GetAttributeConfigFilename();

		if(CCD->MenuManager()->GetSelectedCharacter()->GetSpeed() != -1.0f)
			m_Speed = m_CurrentSpeed = CCD->MenuManager()->GetSelectedCharacter()->GetSpeed();

		if(CCD->MenuManager()->GetSelectedCharacter()->GetJumpSpeed() != -1.0f)
			m_JumpSpeed = CCD->MenuManager()->GetSelectedCharacter()->GetJumpSpeed();

		if(CCD->MenuManager()->GetSelectedCharacter()->GetStepHeight() != -1.0f)
		{
			m_StepHeight = CCD->MenuManager()->GetSelectedCharacter()->GetStepHeight();
			CCD->ActorManager()->SetStepHeight(m_Actor, m_StepHeight);
		}

		if(CCD->MenuManager()->GetSelectedCharacter()->GetSlopeSlide() != -1.0f)
			m_SlideSlope = CCD->MenuManager()->GetSelectedCharacter()->GetSlopeSlide();

		if(CCD->MenuManager()->GetSelectedCharacter()->GetSlopeSpeed() != -1.0f)
			m_SlideSpeed = CCD->MenuManager()->GetSelectedCharacter()->GetSlopeSpeed();
	}

	return LoadAttributesConfiguration(AttrInfo);
}


int CPlayer::LoadAttributesConfiguration(const std::string& filename)
{
	CIniFile AttrFile(filename);

	if(!AttrFile.ReadFile())
	{
		CCD->Log()->Warning("File %s - Line %d: Failed to open Attribute Info file '%s'",
							__FILE__, __LINE__, filename.c_str());
		return RGF_FAILURE;
	}

	m_Attr = CCD->ActorManager()->Inventory(m_Actor);
	std::string KeyName = AttrFile.FindFirstKey();

	while(!KeyName.empty())
	{
		int InitialValue = AttrFile.GetValueI(KeyName, "initial");
		m_Attr->AddAndSet(KeyName.c_str(), InitialValue);
		int LowValue = AttrFile.GetValueI(KeyName, "low");
		int HighValue = AttrFile.GetValueI(KeyName, "high");
		m_Attr->SetValueLimits(KeyName.c_str(), LowValue, HighValue);
		KeyName = AttrFile.FindNextKey();
	}

	sxInventory::GetSingletonPtr()->Update();

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// LoadEnvironmentalAudio
//
// Look up the environmental audio entity in the level, if there is one,
// ..and load up all the sounds required for player motion audio.
/* ------------------------------------------------------------------------------------ */
int CPlayer::LoadEnvironmentalAudio()
{
	// Cool, now load up all the audio and prep the motion audio for playback.
	m_DefaultMotion[0] = NULL;
	m_DefaultMotion[1] = NULL;
	m_DefaultMotion[2] = NULL;
	m_DefaultMotionHandle = NULL;

	for(int nTemp=0; nTemp<16; ++nTemp)
	{
		for(int j=0; j<3; ++j)
			m_Contents[nTemp][j] = NULL;

		m_ContentsHandles[nTemp] = NULL;
	}

	std::string envir = "environment.ini";

	if(CCD->MenuManager()->GetUseSelect())
	{
		if(!CCD->MenuManager()->GetSelectedCharacter()->GetEnvironmentConfigFilename().empty())
			envir = CCD->MenuManager()->GetSelectedCharacter()->GetEnvironmentConfigFilename();
	}

	CIniFile iniFile(envir);

	if(iniFile.ReadFile())
	{
		std::string KeyName = iniFile.FindFirstKey();
		std::string Type, Value;
		int count;

		while(!KeyName.empty())
		{
			if(KeyName == "Default")
			{
				count = 0;
				Type = iniFile.FindFirstName(KeyName);
				Value = iniFile.FindFirstValue();

				while(!Type.empty() && count < 3)
				{
					m_DefaultMotion[count] = LoadAudioClip(Type);
					++count;
					Type = iniFile.FindNextName();
					Value = iniFile.FindNextValue();
				}
			}
			else if(KeyName == "Water")
			{
				count = 0;
				Type = iniFile.FindFirstName(KeyName);
				Value = iniFile.FindFirstValue();

				while(!Type.empty() && count < 3)
				{
					m_Contents[0][count] = LoadAudioClip(Type);
					++count;
					Type = iniFile.FindNextName();
					Value = iniFile.FindNextValue();
				}
			}
			else if(KeyName == "Lava")
			{
				count = 0;
				Type = iniFile.FindFirstName(KeyName);
				Value = iniFile.FindFirstValue();

				while(!Type.empty() && count < 3)
				{
					m_Contents[1][count] = LoadAudioClip(Type);
					++count;
					Type = iniFile.FindNextName();
					Value = iniFile.FindNextValue();
				}
			}
			else if(KeyName == "ToxicGas")
			{
				count = 0;
				Type = iniFile.FindFirstName(KeyName);
				Value = iniFile.FindFirstValue();

				while(!Type.empty() && count < 3)
				{
					m_Contents[2][count] = LoadAudioClip(Type);
					++count;
					Type = iniFile.FindNextName();
					Value = iniFile.FindNextValue();
				}
			}
			else if(KeyName == "ZeroG")
			{
				count = 0;
				Type = iniFile.FindFirstName(KeyName);
				Value = iniFile.FindFirstValue();

				while(!Type.empty() && count < 3)
				{
					m_Contents[3][count] = LoadAudioClip(Type);
					++count;
					Type = iniFile.FindNextName();
					Value = iniFile.FindNextValue();
				}
			}
			else if(KeyName == "Frozen")
			{
				count = 0;
				Type = iniFile.FindFirstName(KeyName);
				Value = iniFile.FindFirstValue();

				while(!Type.empty() && count < 3)
				{
					m_Contents[4][count] = LoadAudioClip(Type);
					++count;
					Type = iniFile.FindNextName();
					Value = iniFile.FindNextValue();
				}
			}
			else if(KeyName == "Sludge")
			{
				count = 0;
				Type = iniFile.FindFirstName(KeyName);
				Value = iniFile.FindFirstValue();

				while(!Type.empty() && count < 3)
				{
					m_Contents[5][count] = LoadAudioClip(Type);
					++count;
					Type = iniFile.FindNextName();
					Value = iniFile.FindNextValue();
				}
			}
			else if(KeyName == "SlowMotion")
			{
				count = 0;
				Type = iniFile.FindFirstName(KeyName);
				Value = iniFile.FindFirstValue();

				while(!Type.empty() && count < 3)
				{
					m_Contents[6][count] = LoadAudioClip(Type);
					++count;
					Type = iniFile.FindNextName();
					Value = iniFile.FindNextValue();
				}
			}
			else if(KeyName == "FastMotion")
			{
				count = 0;
				Type = iniFile.FindFirstName(KeyName);
				Value = iniFile.FindFirstValue();

				while(!Type.empty() && count < 3)
				{
					m_Contents[7][count] = LoadAudioClip(Type);
					++count;
					Type = iniFile.FindNextName();
					Value = iniFile.FindNextValue();
				}
			}
			else if(KeyName == "Ladders")
			{
				count = 0;
				Type = iniFile.FindFirstName(KeyName);
				Value = iniFile.FindFirstValue();

				while(!Type.empty() && count < 3)
				{
					m_Contents[8][count] = LoadAudioClip(Type);
					++count;
					Type = iniFile.FindNextName();
					Value = iniFile.FindNextValue();
				}
			}
			else if(KeyName == "Unclimbable")
			{
				count = 0;
				Type = iniFile.FindFirstName(KeyName);
				Value = iniFile.FindFirstValue();

				while(!Type.empty() && count < 3)
				{
					m_Contents[9][count] = LoadAudioClip(Type);
					++count;
					Type = iniFile.FindNextName();
					Value = iniFile.FindNextValue();
				}
			}

			KeyName = iniFile.FindNextKey();
		}
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// MoveToStart
//
// Move the player avatar to the first PlayerStart we find	in the world.
/* ------------------------------------------------------------------------------------ */
int CPlayer::MoveToStart()
{
	geEntity_EntitySet *lEntitySet;
	geEntity *lEntity;
	PlayerStart *theStart;

	lEntitySet = geWorld_GetEntitySet(CCD->World(), "PlayerStart");

	if(lEntitySet == NULL)
	{
		CCD->Log()->Critical("File %s - Line %d: Missing PlayerStart", __FILE__, __LINE__);
		delete CCD;
		exit(-333);
	}

	bool validPlayerStart = false;

	for(lEntity=geEntity_EntitySetGetNextEntity(lEntitySet, NULL); lEntity;
		lEntity=geEntity_EntitySetGetNextEntity(lEntitySet, lEntity))
	{
		theStart = static_cast<PlayerStart*>(geEntity_GetUserData(lEntity));

		if(CCD->ChangeLevel() && !EffectC_IsStringNull(CCD->StartName()))
		{
			if(!stricmp(CCD->StartName(), theStart->szEntityName))
			{
				validPlayerStart = true;
				break;
			}
		}
		else if(CCD->MenuManager()->GetUseSelect())
		{
			if(!CCD->MenuManager()->GetSelectedCharacter()->GetPlayerStart().empty())
			{
				if(CCD->MenuManager()->GetSelectedCharacter()->GetPlayerStart() == theStart->szEntityName)
				{
					validPlayerStart = true;
					break;
				}
			}
			else
			{
				if(EffectC_IsStringNull(theStart->szEntityName))
				{
					validPlayerStart = true;
					break;
				}
			}
		}
		else
		{
			if(EffectC_IsStringNull(theStart->szEntityName))
			{
				validPlayerStart = true;
				break;
			}
		}
	}

	if(!validPlayerStart)
	{
		for(lEntity=geEntity_EntitySetGetNextEntity(lEntitySet, NULL); lEntity;
			lEntity=geEntity_EntitySetGetNextEntity(lEntitySet, lEntity))
		{
			theStart = static_cast<PlayerStart*>(geEntity_GetUserData(lEntity));

			if(EffectC_IsStringNull(theStart->szEntityName))
			{
				validPlayerStart = true;
				break;
			}
		}
	}

	if(validPlayerStart)
	{
		theStart = static_cast<PlayerStart*>(geEntity_GetUserData(lEntity));

		m_Translation.X = theStart->origin.X;
		m_Translation.Y = theStart->origin.Y;
		m_Translation.Z = theStart->origin.Z;

		geVec3d theRotation;
		theRotation.X = 0.0f;
		theRotation.Y = GE_PIOVER180 * (theStart->Angle.Y - 90.0f);
		theRotation.Z = 0.0f;

		CCD->ActorManager()->Rotate(m_Actor, theRotation);

		if(m_ViewPoint == FIRSTPERSON)
		{
			CCD->CameraManager()->SetRotation(theRotation);
		}

		CCD->ActorManager()->Position(m_Actor, m_Translation);

		if(m_ViewPoint == FIXEDCAMERA)
		{
			if(!CCD->Level()->FixedCameras()->GetFirstCamera())
			{
				CCD->Log()->Critical("File %s - Line %d: No active Fixed Camera can see Player"
										__FILE__, __LINE__);
				delete CCD;
				exit(-333);
			}
		}

		CCD->CameraManager()->TrackMotion();

		if(!EffectC_IsStringNull(theStart->szCDTrack))
			m_szCDTrack = theStart->szCDTrack;

		if(!EffectC_IsStringNull(theStart->szMIDIFile))
			m_szMIDIFile = theStart->szMIDIFile;

		if(!EffectC_IsStringNull(theStart->szStreamingAudio))
			m_szStreamingAudio = theStart->szStreamingAudio;

		m_bSoundtrackLoops = theStart->bSoundtrackLoops;
	}
	else
	{
		CCD->Log()->Critical("File %s - Line %d: Missing PlayerStart", __FILE__, __LINE__);
		delete CCD;
		exit(-333);
	}

	// Fill the position history with the new location
	for(int nCtr=0; nCtr<50; ++nCtr)
		m_PositionHistory[nCtr] = m_Translation;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// CheckKeyLook
//
// Adjust the various rotation angles depending on the current key rotation.
/* ------------------------------------------------------------------------------------ */
void CPlayer::CheckKeyLook(int keyRotate, float timeElapsed)
{
	float turnSpeed = CCD->MenuManager()->GetMouseSen() * timeElapsed;
	float upDownSpeed = CCD->MenuManager()->GetMouseSen() * timeElapsed;

	if(m_RealJumping && (CCD->ActorManager()->ForceActive(m_Actor, 0)
		|| CCD->ActorManager()->Falling(m_Actor) == GE_TRUE))
		return;

	if(m_LookMode)
	{
		if(keyRotate & 8)
			CCD->CameraManager()->TiltDown(upDownSpeed);

		if(keyRotate & 1)
			CCD->CameraManager()->TiltUp(upDownSpeed);

		if(keyRotate & 16)
			CCD->CameraManager()->Center();

		if(keyRotate & 4)
			CCD->CameraManager()->TurnLeft(turnSpeed);

		if(keyRotate & 2)
			CCD->CameraManager()->TurnRight(turnSpeed);

		return;
	}

	if(m_FirstPersonView)
	{
		if(keyRotate & 8)
		{
			if(CCD->ActorManager()->TiltDown(m_Actor, upDownSpeed) == RGF_SUCCESS)
				CCD->CameraManager()->TiltDown(upDownSpeed);
		}

		if(keyRotate & 1)
		{
			if(CCD->ActorManager()->TiltUp(m_Actor, upDownSpeed) == RGF_SUCCESS)
				CCD->CameraManager()->TiltUp(upDownSpeed);
		}

		if(keyRotate & 16)
			CCD->CameraManager()->Center();

		if(keyRotate & 4)
		{
			if(CCD->ActorManager()->TurnLeft(m_Actor, turnSpeed) == RGF_SUCCESS)
				CCD->CameraManager()->TurnLeft(turnSpeed);
		}

		if(keyRotate & 2)
		{
			if(CCD->ActorManager()->TurnRight(m_Actor, turnSpeed) == RGF_SUCCESS)
				CCD->CameraManager()->TurnRight(turnSpeed);
		}
	}
	else
	{
		if(keyRotate & 4)
		{
			if(CCD->ActorManager()->TurnLeft(m_Actor, turnSpeed) == RGF_SUCCESS)
				CCD->CameraManager()->TurnLeft(turnSpeed);
		}

		if(keyRotate & 2)
		{
			if(CCD->ActorManager()->TurnRight(m_Actor, turnSpeed) == RGF_SUCCESS)
				CCD->CameraManager()->TurnRight(turnSpeed);
		}

		if(keyRotate & 1) // is it to the top?
		{
			if(m_Allow3rdLook)
				CCD->CameraManager()->TiltXDown(upDownSpeed);

			CCD->ActorManager()->TiltDown(m_Actor, upDownSpeed);
		}

		if((keyRotate & 8) && m_Allow3rdLook) // is it to the bottom?
		{
			if(m_Allow3rdLook)
				CCD->CameraManager()->TiltXUp(upDownSpeed);

			CCD->ActorManager()->TiltUp(m_Actor, upDownSpeed);
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// CheckMouseLook
//
// Adjust the various rotation angles depending on the current position of the mouse.
/* ------------------------------------------------------------------------------------ */
void CPlayer::CheckMouseLook()
{
	POINT temppos, tempfilter;
	float turnSpeed;				// speed camera will move if turning left/right
	float upDownSpeed;				// speed camera will move if looking up/down
	POINT pos;

	GetCursorPos(&temppos);			// get the mouse position in SCREEN coordinates

	if(CCD->MenuManager()->IsMouseFilterEnabled())
	{
		tempfilter = CCD->MenuManager()->GetFilter();
		CCD->MenuManager()->SetFilter(temppos);

		if(tempfilter.x == -1)
			tempfilter = temppos;

		temppos.x = (temppos.x + tempfilter.x)/2;
		temppos.y = (temppos.y + tempfilter.y)/2;
	}

	CCD->Input()->CenterMouse(&pos.x, &pos.y);

	turnSpeed   = abs(temppos.x - pos.x) * CCD->MenuManager()->GetMouseSen();	// calculate the turning speed
	upDownSpeed = abs(temppos.y - pos.y) * CCD->MenuManager()->GetMouseSen();	// calculate the up/down speed

	if(m_RealJumping && (CCD->ActorManager()->ForceActive(m_Actor, 0)
		|| CCD->ActorManager()->Falling(m_Actor) == GE_TRUE))
		return;

	if(CCD->MenuManager()->IsMouseReversed())
	{
		temppos.y = pos.y+(pos.y-temppos.y);
	}

	if((temppos.x != pos.x) || (temppos.y != pos.y))
	{
		if(m_LookMode)
		{
			// First person we run both the camera AND the actor
			if(temppos.y > pos.y)								// is it to the top?
				CCD->CameraManager()->TiltDown(upDownSpeed);
			else if (temppos.y < pos.y)							// is it to the bottom?
				CCD->CameraManager()->TiltUp(upDownSpeed);

			if(temppos.x > pos.x)								// is it to the left?
				CCD->CameraManager()->TurnLeft(turnSpeed);
			else if (temppos.x < pos.x)							// is it to the right?
				CCD->CameraManager()->TurnRight(turnSpeed);

			return;
		}

		if(m_FirstPersonView)
		{
			if(!CCD->CameraManager()->GetAllowMouse1st())
				return;

			// First person we run both the camera AND the actor
			if(temppos.y > pos.y)							// is it to the top?
			{
				if(CCD->ActorManager()->TiltDown(m_Actor, upDownSpeed) == RGF_SUCCESS)
					CCD->CameraManager()->TiltDown(upDownSpeed);
			}
			else if(temppos.y < pos.y)						// is it to the bottom?
			{
				if(CCD->ActorManager()->TiltUp(m_Actor, upDownSpeed) == RGF_SUCCESS)
					CCD->CameraManager()->TiltUp(upDownSpeed);
			}

			if(temppos.x > pos.x)							// is it to the left?
			{
				if(CCD->ActorManager()->TurnLeft(m_Actor, turnSpeed) == RGF_SUCCESS)
					CCD->CameraManager()->TurnLeft(turnSpeed);
			}
			else if(temppos.x < pos.x)						// is it to the right?
			{
				if(CCD->ActorManager()->TurnRight(m_Actor, turnSpeed) == RGF_SUCCESS)
					CCD->CameraManager()->TurnRight(turnSpeed);
			}
		}
		else	// Third-person mode
		{
			if(m_ViewPoint == THIRDPERSON)
			{
				if(!CCD->CameraManager()->GetAllowMouse3rd())
					return;

				if(CCD->CameraManager()->GetPlayerMouseRotation())
				{
					if(temppos.x > pos.x)				// is it to the left?
						CCD->CameraManager()->CameraRotY(true, turnSpeed);
					else if (temppos.x < pos.x)			// is it to the right?
						CCD->CameraManager()->CameraRotY(false, turnSpeed);

					if(temppos.y < pos.y)				// is it to the top?
						CCD->CameraManager()->ChangeDistance(false, upDownSpeed * 57.0f);
					else if (temppos.y > pos.y)			// is it to the bottom?
						CCD->CameraManager()->ChangeDistance(true, upDownSpeed * 57.0f);
				}
				else
				{
					// In third-person the camera handles all the hard work for us.
					if(temppos.x > pos.x)				// is it to the left?
						CCD->ActorManager()->TurnLeft(m_Actor, turnSpeed);
					else if (temppos.x < pos.x)			// is it to the right?
						CCD->ActorManager()->TurnRight(m_Actor, turnSpeed);

					if(temppos.y < pos.y)				// is it to the top?
					{
						if(m_Allow3rdLook)
							CCD->CameraManager()->TiltXDown(upDownSpeed);

						CCD->ActorManager()->TiltDown(m_Actor, upDownSpeed);
					}
					else if (temppos.y > pos.y)			// is it to the bottom?
					{
						if(m_Allow3rdLook)
							CCD->CameraManager()->TiltXUp(upDownSpeed);

						CCD->ActorManager()->TiltUp(m_Actor, upDownSpeed);
					}
				}
			}
			else if(m_ViewPoint == ISOMETRIC)
			{
				if(!CCD->CameraManager()->GetAllowMouseIso())
					return;

				// In third-person the camera handles all the hard work for us.
				if(temppos.x > pos.x)					// is it to the left?
					CCD->ActorManager()->TurnLeft(m_Actor, turnSpeed);
				else if (temppos.x < pos.x)				// is it to the right?
					CCD->ActorManager()->TurnRight(m_Actor, turnSpeed);

				if(temppos.y < pos.y)					// is it to the top?
				{
					CCD->CameraManager()->TiltXDown(upDownSpeed);
					CCD->ActorManager()->TiltDown(m_Actor, upDownSpeed);
				}
				else if (temppos.y > pos.y)				// is it to the bottom?
				{
					CCD->CameraManager()->TiltXUp(upDownSpeed);
					CCD->ActorManager()->TiltUp(m_Actor, upDownSpeed);
				}
			}
			else if(m_ViewPoint == FIXEDCAMERA)
			{
				// In third-person the camera handles all the hard work for us.
				if(temppos.x > pos.x)					// is it to the left?
					CCD->ActorManager()->TurnLeft(m_Actor, turnSpeed);
				else if (temppos.x < pos.x)				// is it to the right?
					CCD->ActorManager()->TurnRight(m_Actor, turnSpeed);

				if(temppos.y < pos.y)					// is it to the top?
					CCD->ActorManager()->TiltDown(m_Actor, upDownSpeed);
				else if (temppos.y > pos.y)				// is it to the bottom?
					CCD->ActorManager()->TiltUp(m_Actor, upDownSpeed);
			}
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// ProcessMove
//
// Check the contents of the current area to see if we need to play Something Special.
/* ------------------------------------------------------------------------------------ */
int CPlayer::ProcessMove(bool playerMoved)
{
	// head bob control
	static bool BobUp = true;

	float nBobValue = CCD->CameraManager()->GetHeadBobOffset();

	if(!playerMoved || CCD->ActorManager()->ForceActive(m_Actor, 0) || CCD->ActorManager()->Falling(m_Actor))
	{
		// We need to "settle" the viewpoint down to a neutral
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
	}
	else
	{
		// Process "head bobbing"
		// ..Note that head bobbing only occurs when the camera is bound to an
		// ..actor, and has NOT been tested in third-person mode.
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

	int ZoneType, ConType, OldZone;
	bool liq = false;

	CCD->ActorManager()->GetActorZone(m_Actor, &ZoneType);
	CCD->ActorManager()->GetActorOldZone(m_Actor, &OldZone);

	if(OldZone == 1 && playerMoved)
	{
		if(m_SurfaceSound == -1)
		{
			Liquid *LQ = CCD->ActorManager()->GetLiquid(m_Actor);

			if(!EffectC_IsStringNull(LQ->SurfaceSound))
			{
				Snd Sound;
				geVec3d Origin = Position();
				memset(&Sound, 0, sizeof(Sound));
				geVec3d_Copy(&Origin, &(Sound.Pos));
				Sound.Min = CCD->Level()->GetAudibleRadius();
				Sound.Loop = GE_TRUE;
				Sound.SoundDef = SPool_Sound(LQ->SurfaceSound);

				if(Sound.SoundDef)
					m_SurfaceSound = CCD->EffectManager()->Item_Add(EFF_SND, static_cast<void*>(&Sound));
			}
		}

		liq = true;
	}
	else
	{
		if(m_SurfaceSound != -1)
		{
			CCD->EffectManager()->Item_Delete(EFF_SND, m_SurfaceSound);
			m_SurfaceSound = -1;
		}
	}

	if(OldZone == 2 && playerMoved)
	{
		if(m_SwimSound == -1)
		{
			Liquid *LQ = CCD->ActorManager()->GetLiquid(m_Actor);

			if(!EffectC_IsStringNull(LQ->SwimSound))
			{
				Snd Sound;
				geVec3d Origin = Position();
				memset(&Sound, 0, sizeof(Sound));
				geVec3d_Copy(&Origin, &(Sound.Pos));
				Sound.Min = CCD->Level()->GetAudibleRadius();
				Sound.Loop = GE_TRUE;
				Sound.SoundDef = SPool_Sound(LQ->SwimSound);

				if(Sound.SoundDef)
					m_SwimSound = CCD->EffectManager()->Item_Add(EFF_SND, static_cast<void*>(&Sound));
			}
		}

		liq = true;
	}
	else
	{
		if(m_SwimSound != -1)
		{
			CCD->EffectManager()->Item_Delete(EFF_SND, m_SwimSound);
			m_SwimSound = -1;
		}
	}

	if(OldZone == 0 && (ZoneType & kInLiquidZone) && playerMoved)
	{
		if(m_InLiquidSound == -1)
		{
			Liquid *LQ = CCD->ActorManager()->GetLiquid(m_Actor);

			if(!EffectC_IsStringNull(LQ->InLiquidSound))
			{
				Snd Sound;
				geVec3d Origin = Position();
				memset(&Sound, 0, sizeof(Sound));
				geVec3d_Copy(&Origin, &(Sound.Pos));
				Sound.Min = CCD->Level()->GetAudibleRadius();
				Sound.Loop = GE_TRUE;
				Sound.SoundDef = SPool_Sound(LQ->InLiquidSound);

				if(Sound.SoundDef)
					m_InLiquidSound = CCD->EffectManager()->Item_Add(EFF_SND, static_cast<void*>(&Sound));
			}
		}

		liq = true;
	}
	else
	{
		if(m_InLiquidSound != -1)
		{
			CCD->EffectManager()->Item_Delete(EFF_SND, m_InLiquidSound);
			m_InLiquidSound = -1;
		}
	}

	if(liq)
		return RGF_SUCCESS;

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
	else if(ZoneType & kUnclimbableZone)
	{
		ConType = 9;
	}
	else
	{
		ConType = -1;
	}

	if(!(CCD->ActorManager()->ForceActive(m_Actor, 0) || CCD->ActorManager()->ReallyFall(m_Actor)))
	{
		if(ConType == -1)
		{
			if(geSound_SoundIsPlaying(CCD->Engine()->AudioSystem(),
				m_DefaultMotionHandle) != GE_TRUE && playerMoved)
			{
				if(m_DefaultMotion[0])
				{
					int i = rand()%3;
					while(m_DefaultMotion[i] == NULL)
					{
						i = rand()%3;
					}

					m_DefaultMotionHandle = geSound_PlaySoundDef(
						CCD->Engine()->AudioSystem(), m_DefaultMotion[i],
						1.0f, 0.0f, 1.0f, GE_FALSE);
				}
			}
			else
			{
				if(!playerMoved && m_DefaultMotionHandle)
					geSound_StopSound(CCD->Engine()->AudioSystem(), m_DefaultMotionHandle);
			}
		}
		else
		{
			if(geSound_SoundIsPlaying(CCD->Engine()->AudioSystem(),
				m_ContentsHandles[ConType]) != GE_TRUE && playerMoved)
			{
				if(m_Contents[ConType][0])
				{
					int i = rand()%3;

					while(m_Contents[ConType][i] == NULL)
					{
						i = rand()%3;
					}

					m_ContentsHandles[ConType] = geSound_PlaySoundDef(
						CCD->Engine()->AudioSystem(), m_Contents[ConType][i],
						1.0f, 0.0f, 1.0f, GE_FALSE);
				}
			}
			else
			{
				if(!playerMoved && m_ContentsHandles[ConType])
					geSound_StopSound(CCD->Engine()->AudioSystem(), m_ContentsHandles[ConType]);
			}
		}
	}
	else
	{
		if(ConType == -1)
		{
			if(m_DefaultMotionHandle)
				geSound_StopSound(CCD->Engine()->AudioSystem(), m_DefaultMotionHandle);
		}
		else
		{
			if(m_ContentsHandles[ConType])
				geSound_StopSound(CCD->Engine()->AudioSystem(), m_ContentsHandles[ConType]);
		}
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// Move
//
// Move the player forward/backward/left/right at a specific speed.
/* ------------------------------------------------------------------------------------ */
int CPlayer::Move(int how, float speed)
{
	if(m_LookMode)
		return RGF_SUCCESS;

	if(m_Injured)
		return RGF_SUCCESS;

	if(CCD->ActorManager()->CheckTransitionMotion(m_Actor, ANIMSWIM))
		return RGF_SUCCESS;

	m_LastMovementSpeed = speed;
	m_LastDirection = how;


	if(m_RealJumping && CCD->ActorManager()->ForceActive(m_Actor, 0))
		return RGF_SUCCESS;

	if(m_RealJumping && CCD->ActorManager()->Falling(m_Actor) == GE_TRUE)
	{
		geVec3d theDir;

		if(m_LastMovementSpeed != 0.0f && !CCD->ActorManager()->ForceActive(m_Actor, 1))
		{
			switch(m_LastDirection)
			{
			case RGF_K_FORWARD:
				CCD->ActorManager()->InVector(m_Actor, &theDir);
				break;
			case RGF_K_BACKWARD:
				CCD->ActorManager()->InVector(m_Actor, &theDir);
				geVec3d_Inverse(&theDir);
				break;
			case RGF_K_RIGHT:
				CCD->ActorManager()->LeftVector(m_Actor, &theDir);
				geVec3d_Inverse(&theDir);
				break;
			case RGF_K_LEFT:
				CCD->ActorManager()->LeftVector(m_Actor, &theDir);
				break;
			}

			CCD->ActorManager()->SetForce(m_Actor, 1, theDir, m_JumpSpeed, m_JumpSpeed*0.5f);
		}

		return RGF_SUCCESS;
	}

	if(speed == 0.0f)
		return RGF_SUCCESS;

	m_Run = (m_Running || m_AlwaysRun);

	CCD->ActorManager()->SetMoving(m_Actor);
	int zoneType;
	float step;
	CCD->ActorManager()->GetStepHeight(m_Actor, &step);
	CCD->ActorManager()->GetActorZone(m_Actor, &zoneType);

	if(zoneType & kLiquidZone)
		CCD->ActorManager()->SetStepHeight(m_Actor, step * 3.0f);

	switch(how)
	{
	case RGF_K_FORWARD:
		CCD->ActorManager()->MoveForward(m_Actor, speed);
		break;
	case RGF_K_BACKWARD:
		CCD->ActorManager()->MoveBackward(m_Actor, speed);
		break;
	case RGF_K_RIGHT:
		CCD->ActorManager()->MoveRight(m_Actor, speed);
		break;
	case RGF_K_LEFT:
		CCD->ActorManager()->MoveLeft(m_Actor, speed);
		break;
	}

	CCD->ActorManager()->SetStepHeight(m_Actor, step);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// StartJump
//
// Initiates a jump of the player.
/* ------------------------------------------------------------------------------------ */
void CPlayer::StartJump()
{
	int zone;

	if(m_LookMode || m_JumpActive)
		return;

	CCD->ActorManager()->GetActorZone(m_Actor, &zone);

	// You shouldn't be able to jump in certain
	// ..environments, like water, lava, or zero-G.
	int oldZone;
	CCD->ActorManager()->GetActorOldZone(m_Actor, &oldZone);

	if(((oldZone > 0) && CCD->ActorManager()->GetMoving(m_Actor))
		|| (zone & kLavaZone)
		|| (zone & kNoGravityZone)
		|| (zone & kUnclimbableZone)
		|| (m_Crouch && m_NoCrouchJump))
		return;					// Sorry, no jumping allowed

	m_JumpActive = true;
}

/* ------------------------------------------------------------------------------------ */
// SetJump
/* ------------------------------------------------------------------------------------ */
void CPlayer::SetJump()
{
	// Jumping removes us as a passenger from any vehicle we're on.
	geActor *vehicle = CCD->ActorManager()->GetVehicle(m_Actor);

	if(vehicle != NULL)
		CCD->ActorManager()->RemovePassenger(m_Actor);

	if(CCD->ActorManager()->Falling(m_Actor) == GE_TRUE)
		return;						// No jumping in mid-air, heh heh heh

	int zone;
	CCD->ActorManager()->GetActorZone(m_Actor, &zone);

	int oldZone;
	CCD->ActorManager()->GetActorOldZone(m_Actor, &oldZone);

	if(((oldZone>0) && CCD->ActorManager()->GetMoving(m_Actor))
		|| (zone & kLavaZone)
		|| (zone & kNoGravityZone)
		|| (zone & kUnclimbableZone)
		|| (m_Crouch && m_NoCrouchJump))
		return;

	// Add a force to the player actor, aiming upwards, of our jump speed,
	// ..decaying 10 units over time.
	geVec3d up, dir;
	CCD->ActorManager()->UpVector(m_Actor, &up);

	if(m_RealJumping && m_LastMovementSpeed != 0.0f)
	{
		switch(m_LastDirection)
		{
		case RGF_K_FORWARD:
			CCD->ActorManager()->InVector(m_Actor, &dir);
			break;
		case RGF_K_BACKWARD:
			CCD->ActorManager()->InVector(m_Actor, &dir);
			geVec3d_Inverse(&dir);
			break;
		case RGF_K_RIGHT:
			CCD->ActorManager()->LeftVector(m_Actor, &dir);
			geVec3d_Inverse(&dir);
			break;
		case RGF_K_LEFT:
			CCD->ActorManager()->LeftVector(m_Actor, &dir);
			break;
		}

		CCD->ActorManager()->SetForce(m_Actor, 1, dir, m_JumpSpeed, m_JumpSpeed);
	}

	if(m_ViewPoint != FIRSTPERSON && m_OnLadder)
	{
		// jump backwards if on ladder
		geVec3d back;
		CCD->ActorManager()->InVector(m_Actor, &back);
		geVec3d_Inverse(&back);
		CCD->ActorManager()->SetForce(m_Actor, 2, back, m_JumpSpeed * 0.3f, m_JumpSpeed * 0.2f);
	}
	else
	{
		CCD->ActorManager()->SetForce(m_Actor, 0, up, m_JumpSpeed, m_JumpSpeed);
	}
}

/* ------------------------------------------------------------------------------------ */
// Position
/* ------------------------------------------------------------------------------------ */
geVec3d CPlayer::Position()
{
	CCD->ActorManager()->GetPosition(m_Actor, &m_Translation);
	return m_Translation;
}

/* ------------------------------------------------------------------------------------ */
// SwitchCamera
/* ------------------------------------------------------------------------------------ */
void CPlayer::SwitchCamera(int mode)
{
	if(m_LockView)
		return;

	int fixedView = CCD->Weapons()->GetFixedView();

	if(fixedView != -1 && !m_MonitorMode)
	{
		if(mode != fixedView)
			return;
	}

	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	pSet = geWorld_GetEntitySet(CCD->World(), "PlayerSetup");
	pEntity = geEntity_EntitySetGetNextEntity(pSet, NULL);
	PlayerSetup *pSetup = static_cast<PlayerSetup*>(geEntity_GetUserData(pEntity));

	// Mode
	geVec3d theRotation, theAlignRotation;
	CCD->ActorManager()->GetAligningRotation(m_Actor, &theAlignRotation);
	CCD->ActorManager()->GetRotate(m_Actor, &theRotation);
	if(mode != FREEFLOATING)
	{
		CCD->CameraManager()->SetRotation(theRotation);
	}
	theAlignRotation.X = 0.0f;
	theAlignRotation.Z = 0.0f;
	geVec3d theTranslation = {0.0f, 0.0f, 0.0f};

	int nFlags = 0;
	float height;

	switch(mode)
	{
	default:
		mode = FIRSTPERSON;
	case FIRSTPERSON:
		CCD->CameraManager()->BindToActor(m_Actor);
		nFlags = kCameraTrackPosition + kCameraTrackRotation;

		CCD->ActorManager()->GetAnimationHeight(m_Actor, CCD->ActorManager()->GetMotion(m_Actor), &height);
		theTranslation.Y = height;
		if(CCD->CameraManager()->GetViewHeight() != -1.0f)
			theTranslation.Y = height * CCD->CameraManager()->GetViewHeight() * m_Scale / m_CurrentHeight;

		CCD->CameraManager()->SetCameraOffset(theTranslation, theAlignRotation);
		SwitchToFirstPerson();
		CCD->CameraManager()->ResetCamera();

		m_HeadBobSpeed = pSetup->HeadBobSpeed;
		m_HeadBobLimit = pSetup->HeadBobLimit;
		if(pSetup->HeadBobbing == GE_TRUE)
			CCD->CameraManager()->EnableHeadBob(true);
		else
			CCD->CameraManager()->EnableHeadBob(false);

		CCD->CameraManager()->ResetCamera();

		CCD->Weapons()->SetView(FIRSTPERSON);
		break;
	case THIRDPERSON:
		CCD->CameraManager()->BindToActor(m_Actor);
		nFlags = kCameraTrackThirdPerson | kCameraTrackFree;
		theAlignRotation.X = CCD->CameraManager()->GetPlayerAngleUp();

		CCD->ActorManager()->GetAnimationHeight(m_Actor, CCD->ActorManager()->GetMotion(m_Actor), &height);
		theTranslation.Y = height;
		if(CCD->CameraManager()->GetPlayerHeight() != -1.0f)
			theTranslation.Y = height * CCD->CameraManager()->GetPlayerHeight() * m_Scale / m_CurrentHeight;

		theTranslation.Z = CCD->CameraManager()->GetPlayerDistance();
		CCD->CameraManager()->SetCameraOffset(theTranslation, theAlignRotation);
		SwitchToThirdPerson();
		CCD->CameraManager()->ResetCamera();
		m_Allow3rdLook = CCD->CameraManager()->GetPlayerAllowLook();
		CCD->Weapons()->SetView(THIRDPERSON);
		break;
	case ISOMETRIC:
		CCD->CameraManager()->BindToActor(m_Actor);
		nFlags = kCameraTrackIso;
		theRotation.X = CCD->CameraManager()->GetIsoAngleUp();
		theRotation.Y = CCD->CameraManager()->GetIsoAngleAround();

		CCD->ActorManager()->GetAnimationHeight(m_Actor, CCD->ActorManager()->GetMotion(m_Actor), &height);
		theTranslation.Y = height;
		if(CCD->CameraManager()->GetIsoHeight() != -1.0f)
			theTranslation.Y = height * CCD->CameraManager()->GetIsoHeight() * m_Scale / m_CurrentHeight;

		theTranslation.Z = CCD->CameraManager()->GetIsoDistance();
		CCD->CameraManager()->SetCameraOffset(theTranslation, theRotation);
		SwitchToThirdPerson();
		CCD->CameraManager()->ResetCamera();
		CCD->Weapons()->SetView(THIRDPERSON);
		break;
	case FIXEDCAMERA:
		if(CCD->Level()->FixedCameras()->GetNumber() == 0)
			return;
		if(!CCD->Level()->FixedCameras()->GetFirstCamera())
			return;
		nFlags = kCameraTrackFixed;
		CCD->CameraManager()->Unbind();
		SwitchToThirdPerson();
		CCD->CameraManager()->ResetCamera();
		CCD->Level()->FixedCameras()->Tick();
		CCD->Weapons()->SetView(THIRDPERSON);
		break;
	case FREEFLOATING:
		nFlags = kCameraTrackFree;
		CCD->CameraManager()->Unbind();
		SwitchToThirdPerson();
		CCD->Weapons()->SetView(THIRDPERSON);
		break;
	}

	m_ViewPoint = mode;
	CCD->CameraManager()->SetTrackingFlags(nFlags);
	CCD->CameraManager()->TrackMotion();
}

/* ------------------------------------------------------------------------------------ */
// ModifyLight
/* ------------------------------------------------------------------------------------ */
void CPlayer::ModifyLight(int amount)
{
	if(m_DecayLite)
		m_CurrentLiteLife = static_cast<float>(amount);
	else
		m_CurrentLiteLife += static_cast<float>(amount);

	if(m_CurrentLiteLife > m_LiteLife)
		m_CurrentLiteLife = m_LiteLife;

	m_DecayLite = false;
}

/* ------------------------------------------------------------------------------------ */
// GetDieAnim
/* ------------------------------------------------------------------------------------ */
std::string CPlayer::GetDieAnim()
{
	std::string DA = CCD->Weapons()->DieAnim();

	if(!DA.empty())
		return DA;

	return m_DieAnim[rand() % m_DieAnimAmt];
}

/* ------------------------------------------------------------------------------------ */
// GetInjuryAnim
/* ------------------------------------------------------------------------------------ */
std::string CPlayer::GetInjuryAnim()
{
	std::string IA = CCD->Weapons()->InjuryAnim();

	if(!IA.empty())
		return IA;

	return m_InjuryAnim[rand() % m_InjuryAnimAmt];
}

/* ------------------------------------------------------------------------------------ */
// Update
//
// Process actor animation changes
/* ------------------------------------------------------------------------------------ */
void CPlayer::Tick(float timeElapsed)
{
	if(m_MonitorMode)
	{
		CCD->HUD()->Deactivate();

		// TODO:
		if(CCD->Input()->GetKeyboardInputNoWait() == OIS::KC_ESCAPE)
		{
			m_MonitorState = true;
		}
	}

	// TODO: move to better place
	if(m_FirstFrame)
	{
		if(!m_szCDTrack.empty())
		{
			// Start CD player running, if we have one
			if(CCD->CDPlayer())
			{
				CCD->CDPlayer()->SetCdOn(true);
				CCD->CDPlayer()->Play(atoi(m_szCDTrack.c_str()), m_bSoundtrackLoops);
			}
		}

		if(!m_szMIDIFile.empty())
		{
			// Start MIDI file playing
			if(CCD->MIDIPlayer())
				CCD->MIDIPlayer()->Play(m_szMIDIFile, m_bSoundtrackLoops);
		}

		if(!m_szStreamingAudio.empty())
		{
			// Start streaming audio file playing
			if(CCD->Level()->AudioStreams())
				CCD->Level()->AudioStreams()->Play(m_szStreamingAudio.c_str(), m_bSoundtrackLoops, true);
		}

		m_FirstFrame = false;
	}

	AddPosition();
	UpdateSounds();

	int Zone, OldZone;

	CCD->ActorManager()->GetActorZone(m_Actor, &Zone);
	CCD->ActorManager()->GetActorOldZone(m_Actor, &OldZone);

	// play out dying animation
	if(m_Dying || !m_Alive)
	{
		return;
	}

	// play out injury animation
	if(m_Injured)
	{
		if(!CCD->ActorManager()->EndAnimation(m_Actor))
			return;

		m_Injured = false;
		CCD->ActorManager()->SetMotion(m_Actor, m_LastMotion);
		CCD->ActorManager()->SetHoldAtEnd(m_Actor, false);
	}

	// check health for death
	CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(m_Actor);
	if(theInv->Value("health") <= 0)
	{
		bool Switch = true;
		std::string DA = CCD->Weapons()->DieAnim();

		if(DA.empty())
		{
			if(m_DieAnimAmt == 0)
				Switch = false;
		}

		if(!CCD->CameraManager()->GetSwitchAllowed() || !CCD->CameraManager()->GetSwitch3rd())
			Switch = false;

		if(m_ViewPoint == FIRSTPERSON && Switch)
			SwitchCamera(THIRDPERSON);

		if(!Switch)
		{
			m_Alive = false;
		}
		else
		{
			if(m_DieSoundAmt > 0)
			{
				Snd Sound;
				memset(&Sound, 0, sizeof(Sound));
				Sound.Pos = GetSpeakBonePosition();
				Sound.Min = CCD->Level()->GetAudibleRadius();
				Sound.Loop = GE_FALSE;
				Sound.SoundDef = m_DieSound[rand() % m_DieSoundAmt];
				CCD->EffectManager()->Item_Add(EFF_SND, static_cast<void*>(&Sound));
			}

			m_Dying = true;
			CCD->ActorManager()->SetNoCollide(m_Actor);
			CCD->ActorManager()->SetMotion(m_Actor, ANIMDIE);
			CCD->ActorManager()->SetHoldAtEnd(m_Actor, true);
		}

		return;
	}

	// check health for injury
	if(m_LastHealth == -1)
	{
		m_LastHealth = theInv->Value("health");
	}
	else
	{
		if((m_LastHealth > theInv->Value("health") && !m_Injured)
			&& (!m_JumpActive && CCD->ActorManager()->Falling(m_Actor) != GE_TRUE)
			&& !CCD->Level()->ChangeAttributes()->GetActive())
		{
			if(m_InjurySoundAmt > 0 && OldZone == 0)
			{
				Snd Sound;
				memset(&Sound, 0, sizeof(Sound));
				Sound.Pos = GetSpeakBonePosition();
				Sound.Min = CCD->Level()->GetAudibleRadius();
				Sound.Loop = GE_FALSE;
				Sound.SoundDef = m_InjurySound[rand() % m_InjurySoundAmt];
				CCD->EffectManager()->Item_Add(EFF_SND, static_cast<void*>(&Sound));
			}

			if(m_ViewPoint != FIRSTPERSON && !m_FallInjure)
			{
				if(OldZone == 0)
				{
					bool Switch = true;
					std::string IA = CCD->Weapons()->InjuryAnim();

					if(IA.empty())
					{
						if(m_InjuryAnimAmt == 0)
							Switch = false;
					}

					if(Switch && (rand()%10) > 7)
					{
						m_LastMotion = CCD->ActorManager()->GetMotion(m_Actor);
						CCD->ActorManager()->SetMotion(m_Actor, ANIMINJURY);
						CCD->ActorManager()->SetHoldAtEnd(m_Actor, true);
						m_Injured = true;
					}
				}
			}

			m_FallInjure = false;
		}

		m_LastHealth = theInv->Value("health");
	}

	// check for fall damage
	if(!m_Falling && m_OldFalling)
	{
		geVec3d FallEnd;
		CCD->ActorManager()->GetPosition(m_Actor, &FallEnd);
		float distance = fabs(FallEnd.Y - m_FallStart.Y);

		if(Zone != kLiquidZone)
		{
			if(m_LandSoundAmt > 0 && distance > m_MinFallDist)
			{
				Snd Sound;
				memset(&Sound, 0, sizeof(Sound));
				Sound.Pos = GetSpeakBonePosition();
				Sound.Min = CCD->Level()->GetAudibleRadius();
				Sound.Loop = GE_FALSE;
				Sound.SoundDef = m_LandSound[rand() % m_LandSoundAmt];
				CCD->EffectManager()->Item_Add(EFF_SND, static_cast<void*>(&Sound));
			}

			if(m_FallDamage > 0.0f && distance >= m_MinFallDist)
			{
				if(distance > m_MaxFallDist)
					distance = m_MaxFallDist;

				float damage = ((distance - m_MinFallDist)/(m_MaxFallDist - m_MinFallDist))*m_FallDamage;

				CCD->Level()->Damage()->DamageActor(m_Actor, damage, m_FallDamageAttr, damage, m_FallDamageAttr, "Fall");
				m_FallInjure = true;
			}

			if(m_JumpOnDamage > 0.0f && distance >= m_MinJumpOnDist)
			{
				if(distance > m_MaxJumpOnDist)
					distance = m_MaxJumpOnDist;

				float damage = ((distance - m_MinJumpOnDist)/(m_MaxJumpOnDist - m_MinJumpOnDist))*m_JumpOnDamage;
				GE_Collision Collision;
				CCD->ActorManager()->GetGravityCollision(m_Actor, &Collision);

				if(Collision.Actor)
					CCD->Level()->Damage()->DamageActor(Collision.Actor, damage, m_JumpOnDamageAttr, damage, m_JumpOnDamageAttr, "JumpOn");

				if(Collision.Model)
					CCD->Level()->Damage()->DamageModel(Collision.Model, damage, m_JumpOnDamageAttr, damage, m_JumpOnDamageAttr);
			}
		}
	}

	m_OldFalling = m_Falling;

	// check auto increase attribute
	if(m_StaminaName == "LightValue")
	{
		m_StaminaTime += timeElapsed*0.001f;

		if(m_StaminaTime >= m_StaminaDelay)
		{
			m_StaminaTime = 0.0f;

			if(m_CurrentLiteLife != -1.0f && !m_LightActive)
			{
				m_CurrentLiteLife += 1.0f;
				if(m_CurrentLiteLife > m_LiteLife)
					m_CurrentLiteLife = m_LiteLife;
			}
		}
	}
	else
	{
		if(theInv->Has(m_StaminaName))
		{
			m_StaminaTime += timeElapsed*0.001f;
			if(m_StaminaTime >= m_StaminaDelay)
			{
				m_StaminaTime = 0.0f;
				theInv->Modify(m_StaminaName, 1);
				sxInventory::GetSingletonPtr()->UpdateItem(m_StaminaName, true);
			}
		}
	}

	for(int sn=0; sn<20; ++sn)
	{
		if(m_StaminaName1[sn] == "LightValue")
		{
			m_StaminaTime1[sn] += timeElapsed*0.001f;

			if(m_StaminaTime1[sn] >= m_StaminaDelay1[sn])
			{
				m_StaminaTime1[sn] = 0.0f;
				if(m_CurrentLiteLife != -1.0f && !m_LightActive)
				{
					m_CurrentLiteLife += 1.0f;
					if(m_CurrentLiteLife > m_LiteLife)
						m_CurrentLiteLife = m_LiteLife;
				}
			}
		}
		else
		{
			if(theInv->Has(m_StaminaName1[sn]))
			{
				m_StaminaTime1[sn] += timeElapsed*0.001f;
				if(m_StaminaTime1[sn] >= m_StaminaDelay1[sn])
				{
					m_StaminaTime1[sn] = 0.0f;
					theInv->Modify(m_StaminaName1[sn], 1);
					sxInventory::GetSingletonPtr()->UpdateItem(m_StaminaName1[sn], true);
				}
			}
		}
	}

	// restore oxygen if out of liquid
	if(m_RestoreOxy && theInv->Has("oxygen"))
	{
		if(OldZone < 2)
		{
			theInv->Modify("oxygen", theInv->High("oxygen"));
			sxInventory::GetSingletonPtr()->UpdateItem("oxygen", true);
		}
	}
	m_RestoreOxy = true;

	// check for damage in liquid
	if(OldZone > 0 || Zone & kLiquidZone || Zone & kInLiquidZone)
	{
		Liquid *LQ = CCD->ActorManager()->GetLiquid(m_Actor);

		if(LQ)
		{
			if(m_LiquidTime == 0.0f || m_InLiquid == -1)
			{
				if(LQ->DamageIn || OldZone > 1)
				{
					if(!EffectC_IsStringNull(LQ->DamageAttr))
					{
						if(theInv->Has(LQ->DamageAttr) && theInv->Value(LQ->DamageAttr) > theInv->Low(LQ->DamageAttr))
						{
							m_InLiquid = 1;
							theInv->Modify(LQ->DamageAttr, -LQ->DamageAmt);
							sxInventory::GetSingletonPtr()->UpdateItem(LQ->DamageAttr, true);
						}
					}

					if(m_InLiquid == -1)
					{
						if(!EffectC_IsStringNull(LQ->DamageAltAttr))
						{
							if(theInv->Has(LQ->DamageAltAttr) && theInv->Value(LQ->DamageAltAttr) > theInv->Low(LQ->DamageAltAttr))
							{
								theInv->Modify(LQ->DamageAltAttr, -LQ->DamageAltAmt);
								sxInventory::GetSingletonPtr()->UpdateItem(LQ->DamageAltAttr, true);
								m_InLiquid = 2;
							}
						}
					}
				}
				else
				{
					m_LiquidTime = 0.0f;
					m_InLiquid = -1;
				}
			}

			m_LiquidTime += timeElapsed*0.001f;

			if(m_InLiquid > 0)
			{
				if(m_InLiquid == 1)
				{
					if(m_LiquidTime >= LQ->DamageDelay)
					{
						m_LiquidTime = 0.0f;
						m_InLiquid = -1;
					}
				}
				else
				{
					if(m_LiquidTime >= LQ->DamageAltDelay)
					{
						m_LiquidTime = 0.0f;
						m_InLiquid = -1;
					}
				}
			}
		}
	}
	else
	{
		m_LiquidTime = 0.0f;
		m_InLiquid = -1;
	}

	// TODO: rotate player to face the ladder
	if(Zone & kClimbLaddersZone)
		m_OnLadder = true;
	else
		m_OnLadder = false;

	if(theInv->Has("light"))
	{
		if(m_LightOn && theInv->Value("light") != 0)
		{
			if(m_LightActive == false)
			{
				Glow Gl;
				geExtBox theBox;
				geXForm3d thePosition;
				geVec3d Pos = Position();
				CCD->ActorManager()->GetBoundingBox(m_Actor, &theBox);
				Pos.Y += theBox.Max.Y;

				if(m_LiteBone[0] != '\0')
				{
					if(geActor_GetBoneTransform(m_Actor, m_LiteBone.c_str(), &(thePosition)))
						Pos = thePosition.Translation;
				}
				else
					geActor_GetBoneTransform(m_Actor, RootBoneName(m_Actor), &thePosition);

				memset(&Gl, 0, sizeof(Gl));
				geVec3d_Copy(&Pos, &(Gl.Pos));
				Gl.Spot = m_LiteSpot;
				Gl.Arc = m_LiteArc;
				Gl.Style = m_LiteStyle;

				geXForm3d_RotateY(&thePosition, m_LiteOffset.Y);
				geXForm3d_GetEulerAngles(&thePosition, &Gl.Direction);
				Gl.Direction.Z += m_LiteOffset.Z;
				geVec3d_Scale(&(Gl.Direction), GE_180OVERPI, &(Gl.Direction));

				Gl.RadiusMin = m_LiteRadiusMin;
				Gl.RadiusMax = m_LiteRadiusMax;
				Gl.ColorMin.r = m_LiteColorMin.r;
				Gl.ColorMax.r = m_LiteColorMax.r;
				Gl.ColorMin.g = m_LiteColorMin.g;
				Gl.ColorMax.g = m_LiteColorMax.g;
				Gl.ColorMin.b = m_LiteColorMin.b;
				Gl.ColorMax.b = m_LiteColorMax.b;
				Gl.ColorMin.a = 255.0f;
				Gl.ColorMax.a = 255.0f;
				Gl.DoNotClip = false;
				Gl.CastShadows = true;
				m_LightActive = true;
				Gl.Intensity = m_LiteIntensity;

				if(m_CurrentLiteLife == -1.0f)
				{
					m_CurrentLiteLife = m_LiteLife;
					m_DecayLite = false;
				}
				else if(m_DecayLite)
				{
					Gl.Intensity = m_LiteIntensity*(m_CurrentLiteDecay/m_LiteDecay);
				}

				m_LightEffect = CCD->EffectManager()->Item_Add(EFF_LIGHT, static_cast<void*>(&Gl));
			}
		}
		else
		{
			if(m_LightActive)
			{
				if(m_LightEffect != -1)
					CCD->EffectManager()->Item_Delete(EFF_LIGHT, m_LightEffect);
				m_LightEffect = -1;
				m_LightActive = false;
			}
		}
	}

	if(m_LightActive && m_LightEffect != -1)
	{
		Glow Gl;
		geExtBox theBox;
		geXForm3d thePosition;
		geVec3d Pos = Position();
		CCD->ActorManager()->GetBoundingBox(m_Actor, &theBox);
		Pos.Y += theBox.Max.Y;

		if(m_LiteBone[0] != '\0')
		{
			if(geActor_GetBoneTransform(m_Actor, m_LiteBone.c_str(), &thePosition))
				Pos = thePosition.Translation;
		}
		else
		{
			geActor_GetBoneTransform(m_Actor, RootBoneName(m_Actor), &thePosition);
		}

		geVec3d_Copy(&Pos, &(Gl.Pos));
		geXForm3d_RotateY(&thePosition, m_LiteOffset.Y);
		geXForm3d_GetEulerAngles(&thePosition, &(Gl.Direction));
		// move the light up/down when looking up/down
		Gl.Direction.Z += m_LiteOffset.Z;
		geVec3d_Scale(&(Gl.Direction), GE_180OVERPI, &(Gl.Direction));

		if(!m_DecayLite)
		{
			m_CurrentLiteLife -= timeElapsed * 0.001f;

			if(m_CurrentLiteLife <= 0.0f)
			{
				m_DecayLite = true;
				m_CurrentLiteDecay = m_LiteDecay;
			}

			CCD->EffectManager()->Item_Modify(EFF_LIGHT, m_LightEffect, static_cast<void*>(&Gl), GLOW_POS);
		}
		else
		{
			m_CurrentLiteDecay -= timeElapsed * 0.001f;

			if(m_CurrentLiteDecay <= 0.0f)
			{
				CCD->EffectManager()->Item_Delete(EFF_LIGHT, m_LightEffect);
				m_LightEffect = -1;
				m_LightActive = false;
				m_CurrentLiteLife = -1.0f;
				m_LightOn = false;
				theInv->Modify("light", -1);
				sxInventory::GetSingletonPtr()->UpdateItem("light", true);
			}
			else
			{
				Gl.Intensity = m_LiteIntensity * (m_CurrentLiteDecay / m_LiteDecay);
				CCD->EffectManager()->Item_Modify(EFF_LIGHT, m_LightEffect, static_cast<void*>(&Gl), GLOW_POS | GLOW_INTENSITY);
			}
		}
	}

#define CLIMBING ((Motion == ANIMCLIMBUP) || (Motion == ANIMCLIMBDOWN))
#define JUMPING ((Motion == ANIMJUMP) || (Motion == ANIMJUMPSHOOT))
#define FALLING ((Motion == ANIMFALL) || (Motion == ANIMFALLSHOOT))
#define STARTLAND ((Motion == ANIMSTARTJUMP) || (Motion == ANIMLAND))
#define CSTARTLAND ((Motion == ANIMCSTARTJUMP) || (Motion == ANIMCLAND))
#define TREADING ((Motion == ANIMTREADWATER))
#define SWIMING ((Motion == ANIMSWIM) || (Motion == ANIMSWIMBACK))
#define IDLING ((Motion == ANIMIDLE) || (Motion == ANIMAIM))
#define CIDLING ((Motion == ANIMCIDLE) || (Motion == ANIMCAIM))
#define TRANS1 ((Motion == ANIMW2IDLE) || (Motion == ANIMC2IDLE) || (Motion == ANIMCROUCH2IDLE) || (Motion == ANIMIDLE2CROUCH) || (Motion == ANIMSWIM2TREAD) || (Motion == ANIMIDLE2TREAD) || (Motion == ANIMTREAD2IDLE))
#define TRANS2 ((Motion == ANIMI2WALK) || (Motion == ANIMI2RUN) || (Motion == ANIMTREAD2SWIM) || (Motion == ANIMSWIM2WALK) || (Motion == ANIMWALK2SWIM) || (Motion == ANIMJUMP2FALL) || (Motion == ANIMJUMP2TREAD))
#define TRANS3 ((Motion == ANIMFALL2TREAD) || (Motion == ANIMFALL2CRAWL) || (Motion == ANIMFALL2WALK) || (Motion == ANIMFALL2JUMP) || (Motion == ANIMWALK2JUMP) || (Motion == ANIMWALK2CRAWL) || (Motion == ANIMCRAWL2WALK))
#define TRANS4 ((Motion == ANIMIDLE2CRAWL) || (Motion == ANIMAIM2CROUCH) || (Motion == ANIMCROUCH2AIM) || (Motion == ANIMW2TREAD) || (Motion == ANIMFALL2RUN) || (Motion == ANIMCRAWL2RUN))
#define ALLBACK ( (Motion == ANIMWALKBACK) || (Motion == ANIMRUNBACK) || (Motion == ANIMCRAWLBACK) || (Motion == ANIMWALKSHOOTBACK) || (Motion == ANIMRUNSHOOTBACK) || (Motion == ANIMCRAWLSHOOTBACK))
#define ALLTRANS (TRANS1 || TRANS2 || TRANS3 || TRANS4)
#define ALLIDLE (ALLTRANS || JUMPING || FALLING || STARTLAND || CSTARTLAND || TREADING || IDLING || CIDLING || (Motion == ANIMCLIMB) || (Motion == ANIMSHOOT) || (Motion == ANIMCSHOOT))
#define ALLWALK (CLIMBING || SWIMING || ALLTRANS || TREADING || JUMPING || FALLING || ALLBACK || (Motion == ANIMWALK) || (Motion == ANIMRUN) || (Motion == ANIMCRAWL) || (Motion == ANIMWALKSHOOT) || (Motion == ANIMRUNSHOOT) || (Motion == ANIMCRAWLSHOOT))
#define ALLSLIDERIGHT (ALLTRANS || JUMPING || FALLING || (Motion == ANIMSLIDERIGHT) || (Motion == ANIMRUNSLIDERIGHT) || (Motion == ANIMSLIDECRIGHT) || (Motion == ANIMSLIDERIGHTSHOOT) || (Motion == ANIMRUNSLIDERIGHTSHOOT) || (Motion == ANIMSLIDECRIGHTSHOOT))
#define ALLSLIDELEFT (ALLTRANS || JUMPING || FALLING || (Motion == ANIMSLIDELEFT) || (Motion == ANIMRUNSLIDELEFT) || (Motion == ANIMSLIDECLEFT) || (Motion == ANIMSLIDELEFTSHOOT) || (Motion == ANIMRUNSLIDELEFTSHOOT) || (Motion == ANIMSLIDECLEFTSHOOT))

	std::string Motion = CCD->ActorManager()->GetMotion(m_Actor);

	//geEngine_Printf(CCD->Engine()->Engine(), 0, 310, "Motion = %s", Motion.c_str());
	//geEngine_Printf(CCD->Engine()->Engine(), 0, 20,  "OldZone = %d", OldZone);
	//geEngine_Printf(CCD->Engine()->Engine(), 0, 30,  "Zone = %x", Zone);

	switch(m_Moving)
	{
	case MOVEIDLE:
		{
			// jump at idle
			if(m_JumpActive && !STARTLAND && !JUMPING && !CSTARTLAND && !FALLING)
			{
				if(CCD->ActorManager()->CheckAnimation(m_Actor, ANIMJUMP) == GE_TRUE)
				{
					CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMJUMP, false);

					if(m_ViewPoint != FIRSTPERSON)
					{
						if(m_Crouch)
							CCD->ActorManager()->SetMotion(m_Actor, ANIMCSTARTJUMP);
						else
							CCD->ActorManager()->SetMotion(m_Actor, ANIMSTARTJUMP);

						CCD->ActorManager()->SetNextMotion(m_Actor, ANIMJUMP);
						CCD->Weapons()->SetAttackFlag(false);
					}
					else
					{
						CCD->ActorManager()->SetMotion(m_Actor, ANIMJUMP);
					}

					break;
				}
				else
				{
					m_JumpActive = false;
				}
			}

			if(STARTLAND || CSTARTLAND)
			{
				CCD->Weapons()->SetAttackFlag(false);
			}

			if(Motion == ANIMJUMP && CCD->ActorManager()->ForceActive(m_Actor, 0) == GE_FALSE && m_JumpActive)
			{
				SetJump();
				m_JumpActive = false;
				break;
			}

			if(JUMPING && (CCD->ActorManager()->ForceActive(m_Actor, 0) == GE_FALSE) && !m_JumpActive)
			{
				if(!m_Falling)
				{
					CCD->ActorManager()->GetPosition(m_Actor, &(m_FallStart));
					m_Falling = true;
				}

				if(OldZone > 0)
				{
					CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMIDLE, true);
					CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMTREADWATER, ANIMJUMP2TREADTIME, ANIMJUMP2TREAD);
					CCD->ActorManager()->SetNextMotion(m_Actor, ANIMTREADWATER);
				}
				else
				{
					CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMFALL, ANIMJUMP2FALLTIME, ANIMJUMP2FALL);
					CCD->ActorManager()->SetNextMotion(m_Actor, ANIMFALL);
				}

				break;
			}

			if(SWIMING)
			{
				if(CCD->ActorManager()->CheckAnimation(m_Actor, ANIMIDLE) == GE_TRUE)
				{
					CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMTREADWATER, ANIMSWIM2TREADTIME, ANIMSWIM2TREAD);
					CCD->ActorManager()->SetNextMotion(m_Actor, ANIMTREADWATER);
					CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMIDLE, true);
				}

				break;
			}

			if(CCD->ActorManager()->Falling(m_Actor) == GE_TRUE)
			{
				if(CCD->ActorManager()->ReallyFall(m_Actor) == RGF_SUCCESS || (Motion == ANIMJUMP))
				{
					if(!m_Falling)
					{
						CCD->ActorManager()->GetPosition(m_Actor, &m_FallStart);
						m_Falling = true;
					}

					if(FALLING || TREADING)
						break;

					if(OldZone > 0 || Zone & kLiquidZone)
					{
						CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMTREADWATER, ANIMFALL2TREADTIME, ANIMFALL2TREAD);
						CCD->ActorManager()->SetNextMotion(m_Actor, ANIMTREADWATER);
						CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMIDLE, true);
					}
					else
					{
						CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMFALL, ANIMJUMP2FALLTIME, ANIMJUMP2FALL);
						CCD->ActorManager()->SetNextMotion(m_Actor, ANIMFALL);
						CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMFALL, false);
					}

					break;
				}
			}

			if(FALLING && (CCD->ActorManager()->Falling(m_Actor) == GE_FALSE))
			{
				m_Falling = false;

				m_JumpActive = false;

				if(m_OnLadder)
				{
					CCD->ActorManager()->SetMotion(m_Actor, ANIMCLIMB);
					CCD->ActorManager()->SetNextMotion(m_Actor, ANIMCLIMB);
					CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMIDLE, true);
				}
				else
				{
					if(m_Crouch)
					{
						CCD->ActorManager()->SetMotion(m_Actor, ANIMCLAND);
						CCD->ActorManager()->SetNextMotion(m_Actor, ANIMCIDLE);
						CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMCIDLE, true);
					}
					else
					{
						CCD->ActorManager()->SetMotion(m_Actor, ANIMLAND);

						if(OldZone > 0)
						{
							CCD->ActorManager()->SetNextMotion(m_Actor, ANIMTREADWATER);
							CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMIDLE, true);
						}
						else
						{
							CCD->ActorManager()->SetNextMotion(m_Actor, ANIMIDLE);
							CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMIDLE, true);
						}
					}
				}

				CCD->Weapons()->SetAttackFlag(false);
				break;
			}

			if(!(JUMPING || STARTLAND))
			{
				if(OldZone > 0)
				{
					if(CIDLING)
					{
						if(CCD->ActorManager()->CheckAnimation(m_Actor, ANIMIDLE) == GE_TRUE)
						{
							CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMIDLE, ANIMCROUCH2IDLETIME, ANIMCROUCH2IDLE);
							CCD->ActorManager()->SetNextMotion(m_Actor, ANIMIDLE);
							CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMIDLE, true);
							m_Crouch = false;
						}
					}
					else if(!TREADING)
					{
						CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMTREADWATER, ANIMIDLE2TREADTIME, ANIMIDLE2TREAD);
						CCD->ActorManager()->SetNextMotion(m_Actor, ANIMTREADWATER);
						CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMIDLE, true);
						m_Crouch = false;
						break;
					}
				}

				if(OldZone == 0)
				{
					if(TREADING)
					{
						CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMIDLE, ANIMTREAD2IDLETIME, ANIMTREAD2IDLE);
						CCD->ActorManager()->SetMotion(m_Actor, ANIMIDLE);
						CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMIDLE, true);
						break;
					}
				}
			}

			// crouch at idle
			if((Motion == ANIMIDLE) && m_Crouch && OldZone == 0)
			{
				CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMCIDLE, ANIMIDLE2CROUCHTIME, ANIMIDLE2CROUCH);
				CCD->ActorManager()->SetNextMotion(m_Actor, ANIMCIDLE);
				CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMCIDLE, true);
			}

			if((Motion == ANIMSHOOT) && m_Crouch && OldZone == 0)
			{
				CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMCSHOOT, ANIMCSHOOT1, CCD->CameraManager()->GetTiltPercent());
				CCD->ActorManager()->SetBlendNextMotion(m_Actor, ANIMCAIM, ANIMCAIM1, CCD->CameraManager()->GetTiltPercent());
				CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMCIDLE, true);
			}

			if((Motion == ANIMAIM) && m_Crouch && OldZone == 0)
			{
				CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMCAIM, ANIMIDLE2CROUCHTIME, ANIMAIM2CROUCH);
				CCD->ActorManager()->SetNextMotion(m_Actor, ANIMCAIM);
				CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMCIDLE, true);
			}

			// uncrouch at idle
			if((Motion == ANIMCIDLE) && !m_Crouch)
			{
				if(CCD->ActorManager()->CheckAnimation(m_Actor, ANIMIDLE) == GE_TRUE)
				{
					CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMIDLE, ANIMCROUCH2IDLETIME, ANIMCROUCH2IDLE);
					CCD->ActorManager()->SetNextMotion(m_Actor, ANIMIDLE);
					CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMIDLE, true);
				}
			}

			if((Motion == ANIMCSHOOT) && !m_Crouch)
			{
				if(CCD->ActorManager()->CheckAnimation(m_Actor, ANIMIDLE) == GE_TRUE)
				{
					CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMSHOOT, ANIMSHOOT1, CCD->CameraManager()->GetTiltPercent());
					CCD->ActorManager()->SetBlendNextMotion(m_Actor, ANIMAIM, ANIMAIM1, CCD->CameraManager()->GetTiltPercent());
					CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMIDLE, true);
				}
			}

			if((Motion == ANIMCAIM) && !m_Crouch)
			{
				if(CCD->ActorManager()->CheckAnimation(m_Actor, ANIMIDLE) == GE_TRUE)
				{
					CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMAIM, ANIMCROUCH2IDLETIME, ANIMCROUCH2AIM);
					CCD->ActorManager()->SetNextMotion(m_Actor, ANIMAIM);
					CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMIDLE, true);
				}
			}

			// attack
			if(CCD->Weapons()->GetAttackFlag())
			{
				if(IDLING)
				{
					CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMSHOOT, ANIMSHOOT1, CCD->CameraManager()->GetTiltPercent());
					CCD->ActorManager()->SetBlendNextMotion(m_Actor, ANIMAIM, ANIMAIM1, CCD->CameraManager()->GetTiltPercent());
				}

				if(CIDLING)
				{
					CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMCSHOOT, ANIMCSHOOT1, CCD->CameraManager()->GetTiltPercent());
					CCD->ActorManager()->SetBlendNextMotion(m_Actor, ANIMCAIM, ANIMCAIM1, CCD->CameraManager()->GetTiltPercent());
				}

				if((Motion == ANIMJUMP))
				{
					CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMJUMPSHOOT, ANIMJUMPSHOOT1, CCD->CameraManager()->GetTiltPercent());
					CCD->ActorManager()->SetNextMotion(m_Actor, ANIMJUMP);
				}

				if((Motion == ANIMFALL))
				{
					CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMFALLSHOOT, ANIMFALLSHOOT1, CCD->CameraManager()->GetTiltPercent());
					CCD->ActorManager()->SetNextMotion(m_Actor, ANIMFALL);
				}

				if((Motion == ANIMSHOOT))
				{
					CCD->ActorManager()->SetBlendMot(m_Actor, ANIMSHOOT, ANIMSHOOT1, CCD->CameraManager()->GetTiltPercent());
					CCD->ActorManager()->SetBlendNextMotion(m_Actor, ANIMAIM, ANIMAIM1, CCD->CameraManager()->GetTiltPercent());
				}

				if((Motion == ANIMCSHOOT))
				{
					CCD->ActorManager()->SetBlendMot(m_Actor, ANIMCSHOOT, ANIMCSHOOT1, CCD->CameraManager()->GetTiltPercent());
					CCD->ActorManager()->SetBlendNextMotion(m_Actor, ANIMCAIM, ANIMCAIM1, CCD->CameraManager()->GetTiltPercent());
				}

				if((Motion == ANIMJUMPSHOOT))
				{
					CCD->ActorManager()->SetBlendMot(m_Actor, ANIMJUMPSHOOT, ANIMJUMPSHOOT1, CCD->CameraManager()->GetTiltPercent());
					CCD->ActorManager()->SetNextMotion(m_Actor, ANIMJUMP);
				}

				if((Motion == ANIMFALLSHOOT))
				{
					CCD->ActorManager()->SetBlendMot(m_Actor, ANIMFALLSHOOT, ANIMFALLSHOOT1, CCD->CameraManager()->GetTiltPercent());
					CCD->ActorManager()->SetNextMotion(m_Actor, ANIMFALL);
				}

				break;
			}

			if((Motion == ANIMAIM))
			{
				CCD->ActorManager()->SetNextMotion(m_Actor, ANIMIDLE);
				break;
			}

			if((Motion == ANIMCAIM))
			{
				CCD->ActorManager()->SetNextMotion(m_Actor, ANIMCIDLE);
				break;
			}

			// change to idle from moving
			if((Motion == ANIMWALK) || (Motion == ANIMI2WALK)
				|| (Motion == ANIMRUN) || (Motion == ANIMI2RUN)
				|| (Motion == ANIMRUNSHOOT) || (Motion == ANIMWALKSHOOT)
				|| (Motion == ANIMWALKBACK)|| (Motion == ANIMRUNBACK)
				|| (Motion == ANIMRUNSHOOTBACK) || (Motion == ANIMWALKSHOOTBACK))
			{
				if(m_OnLadder && !JUMPING)
				{
					CCD->ActorManager()->SetMotion(m_Actor, ANIMCLIMB);
				}
				else
				{
					if(OldZone > 0)
					{
						CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMTREADWATER, ANIMW2IDLETIME, ANIMW2TREAD);
						CCD->ActorManager()->SetMotion(m_Actor, ANIMTREADWATER);
						CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMIDLE, true);
					}
					else
					{
						CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMIDLE, ANIMW2IDLETIME, ANIMW2IDLE);
						CCD->ActorManager()->SetNextMotion(m_Actor, ANIMIDLE);
						CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMIDLE, true);
					}
				}

				break;
			}

			if((Motion == ANIMCRAWL) || (Motion == ANIMCRAWLSHOOT) ||
				(Motion == ANIMCRAWLBACK) || (Motion == ANIMCRAWLSHOOTBACK))
			{
				CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMCIDLE, ANIMC2IDLETIME, ANIMC2IDLE);
				CCD->ActorManager()->SetNextMotion(m_Actor, ANIMCIDLE);
				break;
			}

			if((Motion == ANIMSLIDECLEFT) || (Motion == ANIMSLIDECRIGHT) ||
				(Motion == ANIMSLIDECLEFTSHOOT) || (Motion == ANIMSLIDECRIGHTSHOOT))
			{
				CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMCIDLE, ANIMSLIDE2CROUCHTIME, "");
				CCD->ActorManager()->SetNextMotion(m_Actor, ANIMCIDLE);
				break;
			}

			if((Motion == ANIMSLIDELEFT) || (Motion == ANIMRUNSLIDELEFT)
				|| (Motion == ANIMSLIDERIGHT) || (Motion == ANIMRUNSLIDERIGHT)
				|| (Motion == ANIMSLIDELEFTSHOOT) || (Motion == ANIMRUNSLIDELEFTSHOOT)
				|| (Motion == ANIMSLIDERIGHTSHOOT) || (Motion == ANIMRUNSLIDERIGHTSHOOT))
			{
				if(m_OnLadder)
				{
					CCD->ActorManager()->SetMotion(m_Actor, ANIMCLIMB);
					break;
				}
				else
				{
					if(OldZone > 0)
					{
						CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMTREADWATER, ANIMSLIDE2IDLETIME, "");
						CCD->ActorManager()->SetNextMotion(m_Actor, ANIMTREADWATER);
					}
					else
					{
						CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMIDLE, ANIMSLIDE2IDLETIME, "");
						CCD->ActorManager()->SetNextMotion(m_Actor, ANIMIDLE);
					}
				}

				CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMIDLE, false);

				break;
			}

			if(m_OnLadder && CLIMBING)
			{
				CCD->ActorManager()->SetMotion(m_Actor, ANIMCLIMB);
				break;
			}

			if(!ALLIDLE)
			{
				CCD->ActorManager()->SetMotion(m_Actor, ANIMIDLE);
			}

			break;
		}
	case MOVEWALKFORWARD:
		{
			if((Motion != ANIMFALL) && (CCD->ActorManager()->Falling(m_Actor) == GE_TRUE))
			{
				if(CCD->ActorManager()->ReallyFall(m_Actor) == RGF_SUCCESS || (Motion == ANIMJUMP))
				{
					if(!m_Falling)
					{
						CCD->ActorManager()->GetPosition(m_Actor, &m_FallStart);
						m_Falling = true;
					}

					if(!(Zone & kInLiquidZone || Zone & kLiquidZone) || (Motion == ANIMJUMP))
					{
						CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMFALL, ANIMJUMP2FALLTIME, ANIMJUMP2FALL);
						CCD->ActorManager()->SetNextMotion(m_Actor, ANIMFALL);
						CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMFALL, false);
					}

					CCD->Weapons()->SetAttackFlag(false);
					break;
				}
			}

			if(FALLING && (CCD->ActorManager()->Falling(m_Actor) == GE_FALSE))
			{
				m_Falling = false;

				if(!m_JumpActive)
				{
					if(m_OnLadder)
					{
						CCD->ActorManager()->SetMotion(m_Actor, ANIMCLIMBUP);
						CCD->ActorManager()->SetNextMotion(m_Actor, ANIMCLIMBUP);
					}
					else
					{
						if(m_Crouch)
						{
							CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMCIDLE, true);
							CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMCRAWL, ANIMFALL2CRAWLTIME, ANIMFALL2CRAWL);
							CCD->ActorManager()->SetNextMotion(m_Actor, ANIMCRAWL);
						}
						else
						{
							CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMIDLE, true);
							if(!m_Run)
							{
								CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMWALK, ANIMFALL2WALKTIME, ANIMFALL2WALK);
								CCD->ActorManager()->SetNextMotion(m_Actor, ANIMWALK);
							}
							else
							{
								CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMRUN, ANIMFALL2WALKTIME, ANIMFALL2RUN);
								CCD->ActorManager()->SetNextMotion(m_Actor, ANIMRUN);
							}
						}
					}
				}
				else
				{
					if(CCD->ActorManager()->CheckAnimation(m_Actor, ANIMJUMP) == GE_TRUE)
					{
						CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMJUMP, false);
						CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMJUMP, ANIMFALL2JUMPTIME, ANIMFALL2JUMP);
						CCD->ActorManager()->SetNextMotion(m_Actor, ANIMJUMP);
						SetJump();
					}

					m_JumpActive = false;
				}

				CCD->Weapons()->SetAttackFlag(false);
				break;
			}

			if(m_JumpActive && (Motion != ANIMJUMP) && (Motion != ANIMFALL)
				&& (CCD->ActorManager()->ForceActive(m_Actor, 0) == GE_FALSE))
			{
				m_JumpActive = false;

				if(!SWIMING)
				{
					if(CCD->ActorManager()->CheckAnimation(m_Actor, ANIMJUMP) == GE_TRUE)
					{
						CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMJUMP, false);
						CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMJUMP, ANIMWALK2JUMPTIME, ANIMWALK2JUMP);
						CCD->ActorManager()->SetNextMotion(m_Actor, ANIMJUMP);
						SetJump();
						CCD->Weapons()->SetAttackFlag(false);
						break;
					}
				}
			}

			if((Motion == ANIMJUMP))
			{
				m_JumpActive = false;

				if(CCD->Weapons()->GetAttackFlag())
				{
					CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMJUMPSHOOT, ANIMJUMPSHOOT1, CCD->CameraManager()->GetTiltPercent());
					CCD->ActorManager()->SetNextMotion(m_Actor, ANIMJUMP);
				}
				break;
			}

			if((Motion == ANIMFALL))
			{
				m_JumpActive = false;

				if(CCD->Weapons()->GetAttackFlag())
				{
					CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMFALLSHOOT, ANIMFALLSHOOT1, CCD->CameraManager()->GetTiltPercent());
					CCD->ActorManager()->SetNextMotion(m_Actor, ANIMFALL);
				}
				break;
			}

			if(OldZone > 0)
			{
				if(!SWIMING)
				{
					if((Motion == ANIMTREADWATER))
						CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMSWIM, ANIMTREAD2SWIMTIME, ANIMTREAD2SWIM);
					else
						CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMSWIM, ANIMWALK2SWIMTIME, ANIMWALK2SWIM);

					CCD->ActorManager()->SetNextMotion(m_Actor, ANIMSWIM);
					CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMSWIM, true);
				}
			}
			else
			{
				if(SWIMING)
				{
					CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMWALK, ANIMSWIM2WALKTIME, ANIMSWIM2WALK);
					CCD->ActorManager()->SetNextMotion(m_Actor, ANIMWALK);
					CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMIDLE, true);
				}
			}
			// walk from idle
			if((Motion == ANIMCLIMB) || (Motion == ANIMIDLE) || (Motion == ANIMW2IDLE) || (Motion == ANIMCROUCH2IDLE))
			{
				if(m_OnLadder)
				{
					CCD->ActorManager()->SetMotion(m_Actor, ANIMCLIMBUP);
				}
				else
				{
					if(!m_Run)
					{
						if(!CCD->Weapons()->GetAttackFlag())
						{
							CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMWALK, ANIMI2WALKTIME, ANIMI2WALK);
							CCD->ActorManager()->SetNextMotion(m_Actor, ANIMWALK);
						}
						else
						{
							CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMWALKSHOOT, ANIMWALKSHOOT1, CCD->CameraManager()->GetTiltPercent());
							CCD->ActorManager()->SetNextMotion(m_Actor, ANIMWALK);
						}
					}
					else
					{
						if(!CCD->Weapons()->GetAttackFlag())
						{
							CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMRUN, ANIMI2RUNTIME, ANIMI2RUN);
							CCD->ActorManager()->SetNextMotion(m_Actor, ANIMRUN);
						}
						else
						{
							CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMRUNSHOOT, ANIMRUNSHOOT1, CCD->CameraManager()->GetTiltPercent());
							CCD->ActorManager()->SetNextMotion(m_Actor, ANIMRUN);
						}
					}
				}

				break;
			}

			// walk from attack
			if((Motion == ANIMSHOOT) || (Motion == ANIMAIM))
			{
				if(!m_Run)
				{
					if(!CCD->Weapons()->GetAttackFlag())
						CCD->ActorManager()->SetMotion(m_Actor, ANIMWALK);
					else
					{
						CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMWALKSHOOT, ANIMWALKSHOOT1, CCD->CameraManager()->GetTiltPercent());
						CCD->ActorManager()->SetNextMotion(m_Actor, ANIMWALK);
					}
				}
				else
				{
					if(!CCD->Weapons()->GetAttackFlag())
						CCD->ActorManager()->SetMotion(m_Actor, ANIMRUN);
					else
					{
						CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMRUNSHOOT, ANIMRUNSHOOT1, CCD->CameraManager()->GetTiltPercent());
						CCD->ActorManager()->SetNextMotion(m_Actor, ANIMRUN);
					}
				}
				break;
			}

			// change to crawl
			if(((Motion == ANIMI2WALK) || (Motion == ANIMWALK)
				|| (Motion == ANIMI2RUN) || (Motion == ANIMRUN)
				|| (Motion == ANIMWALKSHOOT) || (Motion == ANIMRUNSHOOT))
				&& m_Crouch && OldZone == 0)
			{
				CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMCIDLE, true);
				CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMCRAWL, ANIMWALK2CRAWLTIME, ANIMWALK2CRAWL);
				CCD->ActorManager()->SetNextMotion(m_Actor, ANIMCRAWL);
				break;
			}

			// crawl
			if((Motion == ANIMCIDLE) || (Motion == ANIMC2IDLE)
				|| (Motion == ANIMCSHOOT) || (Motion == ANIMCAIM) || (Motion == ANIMIDLE2CROUCH))
			{
				CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMCRAWL, ANIMIDLE2CRAWLTIME, ANIMIDLE2CRAWL);
				CCD->ActorManager()->SetNextMotion(m_Actor, ANIMCRAWL);
				break;
			}

			// change to walk from crawl
			if(((Motion == ANIMCRAWL) || (Motion == ANIMCRAWLSHOOT)) && !m_Crouch)
			{
				if(CCD->ActorManager()->CheckAnimation(m_Actor, ANIMIDLE) == GE_TRUE)
				{
					CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMIDLE, true);

					if(!m_Run)
					{
						if(!CCD->Weapons()->GetAttackFlag())
						{
							CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMWALK, ANIMCRAWL2WALKTIME, ANIMCRAWL2WALK);
							CCD->ActorManager()->SetNextMotion(m_Actor, ANIMWALK);
						}
						else
						{
							CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMWALKSHOOT, ANIMWALKSHOOT1, CCD->CameraManager()->GetTiltPercent());
							CCD->ActorManager()->SetNextMotion(m_Actor, ANIMWALK);
						}
					}
					else
					{
						if(!CCD->Weapons()->GetAttackFlag())
						{
							CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMRUN, ANIMCRAWL2WALKTIME, ANIMCRAWL2RUN);
							CCD->ActorManager()->SetNextMotion(m_Actor, ANIMRUN);
						}
						else
						{
							CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMRUNSHOOT, ANIMRUNSHOOT1, CCD->CameraManager()->GetTiltPercent());
							CCD->ActorManager()->SetNextMotion(m_Actor, ANIMRUN);
						}
					}
					break;
				}
			}

			if(CCD->Weapons()->GetAttackFlag() && !m_Crouch)
			{
				if(!m_Run)
				{
					if(OldZone == 0)
					{
						if((Motion == ANIMWALKSHOOT))
							CCD->ActorManager()->SetBlendMot(m_Actor, ANIMWALKSHOOT, ANIMWALKSHOOT1, CCD->CameraManager()->GetTiltPercent());
						else
							CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMWALKSHOOT, ANIMWALKSHOOT1, CCD->CameraManager()->GetTiltPercent());

						CCD->ActorManager()->SetNextMotion(m_Actor, ANIMWALK);
					}
				}
				else
				{
					if((Motion == ANIMRUNSHOOT))
						CCD->ActorManager()->SetBlendMot(m_Actor, ANIMRUNSHOOT, ANIMRUNSHOOT1, CCD->CameraManager()->GetTiltPercent());
					else
						CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMRUNSHOOT, ANIMRUNSHOOT1, CCD->CameraManager()->GetTiltPercent());

					CCD->ActorManager()->SetNextMotion(m_Actor, ANIMRUN);
				}
				break;
			}

			if(CCD->Weapons()->GetAttackFlag() && m_Crouch)
			{
				if((Motion == ANIMCRAWLSHOOT))
					CCD->ActorManager()->SetBlendMot(m_Actor, ANIMCRAWLSHOOT, ANIMCRAWLSHOOT1, CCD->CameraManager()->GetTiltPercent());
				else
					CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMCRAWLSHOOT, ANIMCRAWLSHOOT1, CCD->CameraManager()->GetTiltPercent());

				CCD->ActorManager()->SetNextMotion(m_Actor, ANIMCRAWL);
				break;
			}

			if((Motion == ANIMSLIDELEFT) || (Motion == ANIMRUNSLIDELEFT)
				|| (Motion == ANIMSLIDERIGHT) || (Motion == ANIMRUNSLIDERIGHT)
				|| (Motion == ANIMSLIDELEFTSHOOT) || (Motion == ANIMRUNSLIDELEFTSHOOT)
				|| (Motion == ANIMSLIDERIGHTSHOOT) || (Motion == ANIMRUNSLIDERIGHTSHOOT))
			{
				if(m_OnLadder)
				{
					CCD->ActorManager()->SetMotion(m_Actor, ANIMCLIMBUP);
					CCD->ActorManager()->SetNextMotion(m_Actor, ANIMCLIMBUP);
				}
				else
				{
					if(!m_Run)
					{
						if(!CCD->Weapons()->GetAttackFlag())
							CCD->ActorManager()->SetMotion(m_Actor, ANIMWALK);
						else
						{
							CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMWALKSHOOT, ANIMWALKSHOOT1, CCD->CameraManager()->GetTiltPercent());
							CCD->ActorManager()->SetNextMotion(m_Actor, ANIMWALK);
						}
					}
					else
					{
						if(!CCD->Weapons()->GetAttackFlag())
							CCD->ActorManager()->SetMotion(m_Actor, ANIMRUN);
						else
						{
							CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMRUNSHOOT, ANIMRUNSHOOT1, CCD->CameraManager()->GetTiltPercent());
							CCD->ActorManager()->SetNextMotion(m_Actor, ANIMRUN);
						}
					}
				}

				break;
			}

			if((Motion == ANIMSLIDECLEFT) || (Motion == ANIMSLIDECRIGHT)
				|| (Motion == ANIMSLIDECLEFTSHOOT) || (Motion == ANIMSLIDECRIGHTSHOOT))
			{
				CCD->ActorManager()->SetMotion(m_Actor, ANIMCRAWL);
				break;
			}

			if((Motion == ANIMCLIMB))
			{
				CCD->ActorManager()->SetMotion(m_Actor, ANIMCLIMBUP);
				break;
			}

			if(((Motion == ANIMWALK) || (Motion == ANIMRUN) || (Motion == ANIMCRAWL)) && m_OnLadder)
			{
				CCD->ActorManager()->SetMotion(m_Actor, ANIMCLIMBUP);
				break;
			}

			if(CLIMBING && !m_OnLadder)
				CCD->ActorManager()->SetMotion(m_Actor, ANIMWALK);

			if((Motion == ANIMWALK) && m_Run)
			{
				CCD->ActorManager()->SetMotion(m_Actor, ANIMRUN);
				break;
			}

			if((Motion == ANIMRUN) && !m_Run)
			{
				CCD->ActorManager()->SetMotion(m_Actor, ANIMWALK);
				break;
			}

			if((Motion == ANIMWALKBACK))
			{
				CCD->ActorManager()->SetMotion(m_Actor, ANIMWALK);
				break;
			}

			if((Motion == ANIMRUNBACK))
			{
				CCD->ActorManager()->SetMotion(m_Actor, ANIMRUN);
				break;
			}

			if((Motion == ANIMCRAWLBACK))
			{
				CCD->ActorManager()->SetMotion(m_Actor, ANIMCRAWL);
				break;
			}

			if((Motion == ANIMSWIMBACK))
			{
				CCD->ActorManager()->SetMotion(m_Actor, ANIMSWIM);
				break;
			}

			if(!ALLWALK && !m_Run)
			{
				CCD->ActorManager()->SetMotion(m_Actor, ANIMWALK);
				break;
			}

			if(!ALLWALK && m_Run)
			{
				CCD->ActorManager()->SetMotion(m_Actor, ANIMRUN);
				break;
			}

			break;
		}
	case MOVEWALKBACKWARD:
		{
			if((Motion != ANIMFALL) && (CCD->ActorManager()->Falling(m_Actor) == GE_TRUE))
			{
				if(CCD->ActorManager()->ReallyFall(m_Actor) == RGF_SUCCESS || (Motion == ANIMJUMP))
				{
					if(!m_Falling)
					{
						CCD->ActorManager()->GetPosition(m_Actor, &m_FallStart);
						m_Falling = true;
					}

					if(!(Zone & kInLiquidZone || Zone & kLiquidZone) || (Motion == ANIMJUMP))
					{
						CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMFALL, ANIMJUMP2FALLTIME, ANIMJUMP2FALL);
						CCD->ActorManager()->SetNextMotion(m_Actor, ANIMFALL);
						CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMFALL, false);
					}

					CCD->Weapons()->SetAttackFlag(false);
					break;
				}
			}

			if(FALLING && (CCD->ActorManager()->Falling(m_Actor) == GE_FALSE))
			{
				m_Falling = false;

				if(!m_JumpActive)
				{
					if(m_OnLadder)
					{
						CCD->ActorManager()->SetMotion(m_Actor, ANIMCLIMBDOWN);
					}
					else
					{
						if(m_Crouch)
						{
							CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMCIDLE, true);
							CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMCRAWLBACK, ANIMFALL2CRAWLTIME, ANIMFALL2CRAWL);
							CCD->ActorManager()->SetNextMotion(m_Actor, ANIMCRAWLBACK);
						}
						else
						{
							CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMIDLE, true);
							if(!m_Run)
							{
								CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMWALKBACK, ANIMFALL2WALKTIME, ANIMFALL2WALK);
								CCD->ActorManager()->SetNextMotion(m_Actor, ANIMWALKBACK);
							}
							else
							{
								CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMRUNBACK, ANIMFALL2WALKTIME, ANIMFALL2RUN);
								CCD->ActorManager()->SetNextMotion(m_Actor, ANIMRUNBACK);
							}
						}
					}
				}
				else
				{
					if(CCD->ActorManager()->CheckAnimation(m_Actor, ANIMJUMP) == GE_TRUE)
					{
						CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMJUMP, false);
						CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMJUMP, ANIMFALL2JUMPTIME, ANIMFALL2JUMP);
						CCD->ActorManager()->SetNextMotion(m_Actor, ANIMJUMP);
						SetJump();
					}

					m_JumpActive = false;
				}

				CCD->Weapons()->SetAttackFlag(false);
				break;
			}

			if(m_JumpActive && (Motion != ANIMJUMP) && (Motion != ANIMFALL)
				&& (CCD->ActorManager()->ForceActive(m_Actor, 0) == GE_FALSE))
			{
				m_JumpActive = false;

				if(!SWIMING)
				{
					if(CCD->ActorManager()->CheckAnimation(m_Actor, ANIMJUMP) == GE_TRUE)
					{
						CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMJUMP, false);
						CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMJUMP, ANIMWALK2JUMPTIME, ANIMWALK2JUMP);
						CCD->ActorManager()->SetNextMotion(m_Actor, ANIMJUMP);
						SetJump();
						CCD->Weapons()->SetAttackFlag(false);
						break;
					}
				}
			}

			if((Motion == ANIMJUMP))
			{
				m_JumpActive = false;

				if(CCD->Weapons()->GetAttackFlag())
				{
					CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMJUMPSHOOT, ANIMJUMPSHOOT1, CCD->CameraManager()->GetTiltPercent());
					CCD->ActorManager()->SetNextMotion(m_Actor, ANIMJUMP);
				}

				break;
			}

			if((Motion == ANIMFALL))
			{
				m_JumpActive = false;

				if(CCD->Weapons()->GetAttackFlag())
				{
					CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMFALLSHOOT, ANIMFALLSHOOT1, CCD->CameraManager()->GetTiltPercent());
					CCD->ActorManager()->SetNextMotion(m_Actor, ANIMFALL);
				}

				break;
			}

			if(OldZone > 0)
			{
				if(!SWIMING)
				{
					if((Motion == ANIMTREADWATER))
						CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMSWIMBACK, ANIMTREAD2SWIMTIME, ANIMTREAD2SWIM);
					else
						CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMSWIMBACK, ANIMWALK2SWIMTIME, ANIMWALK2SWIM);

					CCD->ActorManager()->SetNextMotion(m_Actor, ANIMSWIMBACK);
					CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMSWIMBACK, true);
				}
			}
			else
			{
				if(SWIMING)
				{
					CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMWALKBACK, ANIMSWIM2WALKTIME, ANIMSWIM2WALK);
					CCD->ActorManager()->SetNextMotion(m_Actor, ANIMWALKBACK);
					CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMIDLE, true);
				}
			}

			// walk from idle
			if((Motion == ANIMCLIMB) || (Motion == ANIMIDLE) || (Motion == ANIMW2IDLE) || (Motion == ANIMCROUCH2IDLE))
			{
				if(m_OnLadder)
				{
					CCD->ActorManager()->SetMotion(m_Actor, ANIMCLIMBDOWN);
					CCD->ActorManager()->SetNextMotion(m_Actor, ANIMCLIMBDOWN);
				}
				else
				{
					if(!m_Run)
					{
						if(!CCD->Weapons()->GetAttackFlag())
						{
							CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMWALKBACK, ANIMI2WALKTIME, ANIMI2WALK);
							CCD->ActorManager()->SetNextMotion(m_Actor, ANIMWALKBACK);
						}
						else
						{
							CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMWALKSHOOTBACK, ANIMWALKSHOOT1BACK, CCD->CameraManager()->GetTiltPercent());
							CCD->ActorManager()->SetNextMotion(m_Actor, ANIMWALKBACK);
						}
					}
					else
					{
						if(!CCD->Weapons()->GetAttackFlag())
						{
							CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMRUNBACK, ANIMI2RUNTIME, ANIMI2RUN);
							CCD->ActorManager()->SetNextMotion(m_Actor, ANIMRUNBACK);
						}
						else
						{
							CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMRUNSHOOTBACK, ANIMRUNSHOOT1BACK, CCD->CameraManager()->GetTiltPercent());
							CCD->ActorManager()->SetNextMotion(m_Actor, ANIMRUNBACK);
						}
					}
				}

				break;
			}

			// walk from attack
			if((Motion == ANIMSHOOT) || (Motion == ANIMAIM))
			{
				if(!m_Run)
				{
					if(!CCD->Weapons()->GetAttackFlag())
						CCD->ActorManager()->SetMotion(m_Actor, ANIMWALKBACK);
					else
					{
						CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMWALKSHOOTBACK, ANIMWALKSHOOT1BACK, CCD->CameraManager()->GetTiltPercent());
						CCD->ActorManager()->SetNextMotion(m_Actor, ANIMWALKBACK);
					}
				}
				else
				{
					if(!CCD->Weapons()->GetAttackFlag())
						CCD->ActorManager()->SetMotion(m_Actor, ANIMRUNBACK);
					else
					{
						CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMRUNSHOOTBACK, ANIMRUNSHOOT1BACK, CCD->CameraManager()->GetTiltPercent());
						CCD->ActorManager()->SetNextMotion(m_Actor, ANIMRUNBACK);
					}
				}
				break;
			}

			// change to crawl
			if(((Motion == ANIMI2WALK) || (Motion == ANIMWALKBACK)
				|| (Motion == ANIMI2RUN) || (Motion == ANIMRUNBACK)
				|| (Motion == ANIMWALKSHOOTBACK) || (Motion == ANIMRUNSHOOTBACK))
				&& m_Crouch && OldZone == 0)
			{
				CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMCIDLE, true);
				CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMCRAWLBACK, ANIMWALK2CRAWLTIME, ANIMWALK2CRAWL);
				CCD->ActorManager()->SetNextMotion(m_Actor, ANIMCRAWLBACK);
				break;
			}

			// crawl
			if((Motion == ANIMCIDLE) || (Motion == ANIMC2IDLE)
				|| (Motion == ANIMCSHOOT) || (Motion == ANIMCAIM) || (Motion == ANIMIDLE2CROUCH))
			{
				CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMCRAWLBACK, ANIMIDLE2CRAWLTIME, ANIMIDLE2CRAWL);
				CCD->ActorManager()->SetNextMotion(m_Actor, ANIMCRAWLBACK);
				break;
			}

			// change to walk from crawl
			if(((Motion == ANIMCRAWLBACK) || (Motion == ANIMCRAWLSHOOTBACK)) && !m_Crouch)
			{
				if(CCD->ActorManager()->CheckAnimation(m_Actor, ANIMIDLE) == GE_TRUE)
				{
					CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMIDLE, true);

					if(!m_Run)
					{
						if(!CCD->Weapons()->GetAttackFlag())
						{
							CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMWALKBACK, ANIMCRAWL2WALKTIME, ANIMCRAWL2WALK);
							CCD->ActorManager()->SetNextMotion(m_Actor, ANIMWALKBACK);
						}
						else
						{
							CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMWALKSHOOTBACK, ANIMWALKSHOOT1BACK, CCD->CameraManager()->GetTiltPercent());
							CCD->ActorManager()->SetNextMotion(m_Actor, ANIMWALKBACK);
						}
					}
					else
					{
						if(!CCD->Weapons()->GetAttackFlag())
						{
							CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMRUNBACK, ANIMCRAWL2WALKTIME, ANIMCRAWL2RUN);
							CCD->ActorManager()->SetNextMotion(m_Actor, ANIMRUNBACK);
						}
						else
						{
							CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMRUNSHOOTBACK, ANIMRUNSHOOT1BACK, CCD->CameraManager()->GetTiltPercent());
							CCD->ActorManager()->SetNextMotion(m_Actor, ANIMRUNBACK);
						}
					}
					break;
				}
			}

			if(CCD->Weapons()->GetAttackFlag() && !m_Crouch)
			{
				if(!m_Run)
				{
					if(OldZone == 0)
					{
						if((Motion == ANIMWALKSHOOTBACK))
							CCD->ActorManager()->SetBlendMot(m_Actor, ANIMWALKSHOOTBACK, ANIMWALKSHOOT1BACK, CCD->CameraManager()->GetTiltPercent());
						else
							CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMWALKSHOOTBACK, ANIMWALKSHOOT1BACK, CCD->CameraManager()->GetTiltPercent());

						CCD->ActorManager()->SetNextMotion(m_Actor, ANIMWALKBACK);
					}
				}
				else
				{
					if((Motion == ANIMRUNSHOOTBACK))
						CCD->ActorManager()->SetBlendMot(m_Actor, ANIMRUNSHOOTBACK, ANIMRUNSHOOT1BACK, CCD->CameraManager()->GetTiltPercent());
					else
						CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMRUNSHOOTBACK, ANIMRUNSHOOT1BACK, CCD->CameraManager()->GetTiltPercent());

					CCD->ActorManager()->SetNextMotion(m_Actor, ANIMRUNBACK);
				}

				break;
			}

			if(CCD->Weapons()->GetAttackFlag() && m_Crouch)
			{
				if((Motion == ANIMCRAWLSHOOTBACK))
					CCD->ActorManager()->SetBlendMot(m_Actor, ANIMCRAWLSHOOTBACK, ANIMCRAWLSHOOT1BACK, CCD->CameraManager()->GetTiltPercent());
				else
					CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMCRAWLSHOOTBACK, ANIMCRAWLSHOOT1BACK, CCD->CameraManager()->GetTiltPercent());

				CCD->ActorManager()->SetNextMotion(m_Actor, ANIMCRAWLBACK);
				break;
			}

			if((Motion == ANIMSLIDELEFT) || (Motion == ANIMRUNSLIDELEFT)
				|| (Motion == ANIMSLIDERIGHT) || (Motion == ANIMRUNSLIDERIGHT)
				|| (Motion == ANIMSLIDELEFTSHOOT) || (Motion == ANIMRUNSLIDELEFTSHOOT)
				|| (Motion == ANIMSLIDERIGHTSHOOT) || (Motion == ANIMRUNSLIDERIGHTSHOOT))
			{
				if(m_OnLadder)
				{
					CCD->ActorManager()->SetMotion(m_Actor, ANIMCLIMBUP);
					CCD->ActorManager()->SetNextMotion(m_Actor, ANIMCLIMBUP);
				}
				else
				{
					if(!m_Run)
					{
						if(!CCD->Weapons()->GetAttackFlag())
							CCD->ActorManager()->SetMotion(m_Actor, ANIMWALKBACK);
						else
						{
							CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMWALKSHOOTBACK, ANIMWALKSHOOT1BACK, CCD->CameraManager()->GetTiltPercent());
							CCD->ActorManager()->SetNextMotion(m_Actor, ANIMWALKBACK);
						}
					}
					else
					{
						if(!CCD->Weapons()->GetAttackFlag())
							CCD->ActorManager()->SetMotion(m_Actor, ANIMRUNBACK);
						else
						{
							CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMRUNSHOOTBACK, ANIMRUNSHOOT1BACK, CCD->CameraManager()->GetTiltPercent());
							CCD->ActorManager()->SetNextMotion(m_Actor, ANIMRUNBACK);
						}
					}
				}

				break;
			}

			if((Motion == ANIMSLIDECLEFT) || (Motion == ANIMSLIDECRIGHT)
				|| (Motion == ANIMSLIDECLEFTSHOOT) || (Motion == ANIMSLIDECRIGHTSHOOT))
			{
				CCD->ActorManager()->SetMotion(m_Actor, ANIMCRAWLBACK);
				break;
			}

			if((Motion == ANIMCLIMB))
			{
				CCD->ActorManager()->SetMotion(m_Actor, ANIMCLIMBDOWN);
				break;
			}

			if(((Motion == ANIMWALKBACK) || (Motion == ANIMRUNBACK) || (Motion == ANIMCRAWLBACK)) && m_OnLadder)
			{
				CCD->ActorManager()->SetMotion(m_Actor, ANIMCLIMBDOWN);
				break;
			}

			if(CLIMBING && !m_OnLadder)
				CCD->ActorManager()->SetMotion(m_Actor, ANIMWALKBACK);

			if((Motion == ANIMWALKBACK) && m_Run)
			{
				CCD->ActorManager()->SetMotion(m_Actor, ANIMRUNBACK);
				break;
			}

			if((Motion == ANIMRUNBACK) && !m_Run)
			{
				CCD->ActorManager()->SetMotion(m_Actor, ANIMWALKBACK);
				break;
			}

			if((Motion == ANIMWALK))
			{
				CCD->ActorManager()->SetMotion(m_Actor, ANIMWALKBACK);
				break;
			}

			if((Motion == ANIMRUN))
			{
				CCD->ActorManager()->SetMotion(m_Actor, ANIMRUNBACK);
				break;
			}

			if((Motion == ANIMCRAWL))
			{
				CCD->ActorManager()->SetMotion(m_Actor, ANIMCRAWLBACK);
				break;
			}

			if((Motion == ANIMSWIM))
			{
				CCD->ActorManager()->SetMotion(m_Actor, ANIMSWIMBACK);
				break;
			}

			if(!ALLWALK && !m_Run)
			{
				CCD->ActorManager()->SetMotion(m_Actor, ANIMWALKBACK);
				break;
			}

			if(!ALLWALK && m_Run)
			{
				CCD->ActorManager()->SetMotion(m_Actor, ANIMRUNBACK);
				break;
			}

			break;
		}
	case MOVESLIDERIGHT:
		{
			m_JumpActive = false;

			if((Motion != ANIMFALL) && (CCD->ActorManager()->Falling(m_Actor) == GE_TRUE))
			{
				if(CCD->ActorManager()->ReallyFall(m_Actor) == RGF_SUCCESS || (Motion == ANIMJUMP))
				{
					if(!m_Falling)
					{
						CCD->ActorManager()->GetPosition(m_Actor, &(m_FallStart));
						m_Falling = true;
					}

					if(!(Zone & kInLiquidZone || Zone & kLiquidZone) || (Motion == ANIMJUMP))
					{
						CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMFALL, ANIMJUMP2FALLTIME, ANIMJUMP2FALL);
						CCD->ActorManager()->SetNextMotion(m_Actor, ANIMFALL);
						CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMFALL, false);
					}

					CCD->Weapons()->SetAttackFlag(false);
					break;
				}
			}

			if(FALLING && (CCD->ActorManager()->Falling(m_Actor) == GE_FALSE))
			{
				m_Falling = false;

				if(m_Crouch)
				{
					CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMCIDLE, true);
					CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMSLIDECRIGHT, ANIMFALL2CRAWLTIME, ANIMFALL2CRAWL);
					CCD->ActorManager()->SetNextMotion(m_Actor, ANIMSLIDECRIGHT);
				}
				else
				{
					CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMIDLE, true);

					if(!m_Run)
					{
						CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMSLIDERIGHT, ANIMFALL2WALKTIME, ANIMFALL2WALK);
						CCD->ActorManager()->SetNextMotion(m_Actor, ANIMSLIDERIGHT);
					}
					else
					{
						CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMRUNSLIDERIGHT, ANIMFALL2WALKTIME, ANIMFALL2RUN);
						CCD->ActorManager()->SetNextMotion(m_Actor, ANIMRUNSLIDERIGHT);
					}
				}

				CCD->Weapons()->SetAttackFlag(false);
				break;
			}

			if((Motion == ANIMIDLE) || (Motion == ANIMW2IDLE)
				|| (Motion == ANIMSHOOT) || (Motion == ANIMAIM)
				|| (Motion == ANIMWALK) || (Motion == ANIMI2WALK)
				|| (Motion == ANIMWALKSHOOT) || (Motion == ANIMRUNSHOOT)
				|| (Motion == ANIMRUN) || (Motion == ANIMI2RUN)
				|| (Motion == ANIMSLIDELEFTSHOOT) || (Motion == ANIMRUNSLIDELEFTSHOOT)
				|| (Motion == ANIMSLIDELEFT) || (Motion == ANIMRUNSLIDELEFT))
			{
				if(!m_Run)
				{
					if(!CCD->Weapons()->GetAttackFlag())
						CCD->ActorManager()->SetMotion(m_Actor, ANIMSLIDERIGHT);
					else
					{
						CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMSLIDERIGHTSHOOT, ANIMSLIDERIGHTSHOOT1, CCD->CameraManager()->GetTiltPercent());
						CCD->ActorManager()->SetNextMotion(m_Actor, ANIMSLIDERIGHT);
					}
				}
				else
				{
					if(!CCD->Weapons()->GetAttackFlag())
						CCD->ActorManager()->SetMotion(m_Actor, ANIMRUNSLIDERIGHT);
					else
					{
						CCD->ActorManager()->SetMotion(m_Actor, ANIMRUNSLIDERIGHTSHOOT);
						CCD->ActorManager()->SetNextMotion(m_Actor, ANIMRUNSLIDERIGHT);
					}
				}
				break;
			}

			if((Motion == ANIMCIDLE) || (Motion == ANIMC2IDLE)
				|| (Motion == ANIMSLIDECLEFT) || (Motion == ANIMSLIDECLEFTSHOOT)
				|| (Motion == ANIMCRAWL) || (Motion == ANIMCRAWLSHOOT)
				|| (Motion == ANIMCSHOOT) || (Motion == ANIMCAIM))
			{
				if(!CCD->Weapons()->GetAttackFlag())
					CCD->ActorManager()->SetMotion(m_Actor, ANIMSLIDECRIGHT);
				else
				{
					CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMSLIDECRIGHTSHOOT, ANIMSLIDECRIGHTSHOOT1, CCD->CameraManager()->GetTiltPercent());
					CCD->ActorManager()->SetNextMotion(m_Actor, ANIMSLIDECRIGHT);
				}
				break;
			}

			if(((Motion == ANIMSLIDERIGHT) || (Motion == ANIMRUNSLIDERIGHT)) && m_Crouch)
			{
				if(!CCD->Weapons()->GetAttackFlag())
					CCD->ActorManager()->SetMotion(m_Actor, ANIMSLIDECRIGHT);
				else
				{
					CCD->ActorManager()->SetMotion(m_Actor, ANIMSLIDECRIGHTSHOOT);
					CCD->ActorManager()->SetNextMotion(m_Actor, ANIMSLIDECRIGHT);
				}

				CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMCIDLE, true);
				break;
			}

			if(((Motion == ANIMSLIDECRIGHT) || (Motion == ANIMSLIDECRIGHTSHOOT))&& !m_Crouch)
			{
				if(CCD->ActorManager()->CheckAnimation(m_Actor, ANIMIDLE) == GE_TRUE)
				{
					if(!m_Run)
					{
						if(!CCD->Weapons()->GetAttackFlag())
							CCD->ActorManager()->SetMotion(m_Actor, ANIMSLIDERIGHT);
						else
						{
							CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMSLIDERIGHTSHOOT, ANIMSLIDERIGHTSHOOT1, CCD->CameraManager()->GetTiltPercent());
							CCD->ActorManager()->SetNextMotion(m_Actor, ANIMSLIDERIGHT);
						}
					}
					else
					{
						if(!CCD->Weapons()->GetAttackFlag())
							CCD->ActorManager()->SetMotion(m_Actor, ANIMRUNSLIDERIGHT);
						else
						{
							CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMRUNSLIDERIGHTSHOOT, ANIMRUNSLIDERIGHTSHOOT1, CCD->CameraManager()->GetTiltPercent());
							CCD->ActorManager()->SetNextMotion(m_Actor, ANIMRUNSLIDERIGHT);
						}
					}

					CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMIDLE, true);
					break;
				}
			}

			if(CCD->Weapons()->GetAttackFlag() && !m_Crouch)
			{
				if(!m_Run)
				{
					if((Motion == ANIMSLIDERIGHTSHOOT))
						CCD->ActorManager()->SetBlendMot(m_Actor, ANIMSLIDERIGHTSHOOT, ANIMSLIDERIGHTSHOOT1, CCD->CameraManager()->GetTiltPercent());
					else
						CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMSLIDERIGHTSHOOT, ANIMSLIDERIGHTSHOOT1, CCD->CameraManager()->GetTiltPercent());

					CCD->ActorManager()->SetNextMotion(m_Actor, ANIMSLIDERIGHT);
				}
				else
				{
					if((Motion == ANIMRUNSLIDERIGHTSHOOT))
						CCD->ActorManager()->SetBlendMot(m_Actor, ANIMRUNSLIDERIGHTSHOOT, ANIMRUNSLIDERIGHTSHOOT1, CCD->CameraManager()->GetTiltPercent());
					else
						CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMRUNSLIDERIGHTSHOOT, ANIMRUNSLIDERIGHTSHOOT1, CCD->CameraManager()->GetTiltPercent());

					CCD->ActorManager()->SetNextMotion(m_Actor, ANIMRUNSLIDERIGHT);
				}

				break;
			}

			if(CCD->Weapons()->GetAttackFlag() && m_Crouch)
			{
				if((Motion == ANIMSLIDECRIGHTSHOOT))
					CCD->ActorManager()->SetBlendMot(m_Actor, ANIMSLIDECRIGHTSHOOT, ANIMSLIDECRIGHTSHOOT1, CCD->CameraManager()->GetTiltPercent());
				else
					CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMSLIDECRIGHTSHOOT, ANIMSLIDECRIGHTSHOOT1, CCD->CameraManager()->GetTiltPercent());

				CCD->ActorManager()->SetNextMotion(m_Actor, ANIMSLIDECRIGHT);
				break;
			}

			if((Motion == ANIMSLIDERIGHT) && m_Run)
			{
				CCD->ActorManager()->SetMotion(m_Actor, ANIMRUNSLIDERIGHT);
				break;
			}

			if((Motion == ANIMRUNSLIDERIGHT) && !m_Run)
			{
				CCD->ActorManager()->SetMotion(m_Actor, ANIMSLIDERIGHT);
				break;
			}

			if(!ALLSLIDERIGHT && !m_Run)
			{
				CCD->ActorManager()->SetMotion(m_Actor, ANIMSLIDERIGHT);
				break;
			}

			if(!ALLSLIDERIGHT && m_Run)
			{
				CCD->ActorManager()->SetMotion(m_Actor, ANIMRUNSLIDERIGHT);
				break;
			}

			break;
		}
	case MOVESLIDELEFT:
		{
			m_JumpActive = false;

			if((Motion != ANIMFALL) && (CCD->ActorManager()->Falling(m_Actor) == GE_TRUE))
			{
				if(CCD->ActorManager()->ReallyFall(m_Actor) == RGF_SUCCESS || (Motion == ANIMJUMP))
				{
					if(!m_Falling)
					{
						CCD->ActorManager()->GetPosition(m_Actor, &(m_FallStart));
						m_Falling = true;
					}

					if(!(Zone & kInLiquidZone || Zone & kLiquidZone) || (Motion == ANIMJUMP))
					{
						CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMFALL, ANIMJUMP2FALLTIME, ANIMJUMP2FALL);
						CCD->ActorManager()->SetNextMotion(m_Actor, ANIMFALL);
						CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMFALL, false);
					}

					CCD->Weapons()->SetAttackFlag(false);
					break;
				}
			}

			if(FALLING && (CCD->ActorManager()->Falling(m_Actor) == GE_FALSE))
			{
				m_Falling = false;

				if(m_Crouch)
				{
					CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMCIDLE, true);
					CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMSLIDECLEFT, ANIMFALL2CRAWLTIME, ANIMFALL2CRAWL);
					CCD->ActorManager()->SetNextMotion(m_Actor, ANIMSLIDECLEFT);
				}
				else
				{
					CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMIDLE, true);

					if(!m_Run)
					{
						CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMSLIDELEFT, ANIMFALL2WALKTIME, ANIMFALL2WALK);
						CCD->ActorManager()->SetNextMotion(m_Actor, ANIMSLIDELEFT);
					}
					else
					{
						CCD->ActorManager()->SetTransitionMotion(m_Actor, ANIMRUNSLIDELEFT, ANIMFALL2WALKTIME, ANIMFALL2RUN);
						CCD->ActorManager()->SetNextMotion(m_Actor, ANIMRUNSLIDELEFT);
					}
				}

				CCD->Weapons()->SetAttackFlag(false);
				break;
			}

			if((Motion == ANIMIDLE) || (Motion == ANIMW2IDLE)
				|| (Motion == ANIMSHOOT) || (Motion == ANIMAIM)
				|| (Motion == ANIMWALK) || (Motion == ANIMI2WALK)
				|| (Motion == ANIMWALKSHOOT) || (Motion == ANIMRUNSHOOT)
				|| (Motion == ANIMRUN) || (Motion == ANIMI2RUN)
				|| (Motion == ANIMSLIDERIGHTSHOOT) || (Motion == ANIMRUNSLIDERIGHTSHOOT)
				|| (Motion == ANIMSLIDERIGHT) || (Motion == ANIMRUNSLIDERIGHT))
			{
				if(!m_Run)
				{
					if(!CCD->Weapons()->GetAttackFlag())
						CCD->ActorManager()->SetMotion(m_Actor, ANIMSLIDELEFT);
					else
					{
						CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMSLIDELEFTSHOOT, ANIMSLIDELEFTSHOOT1, CCD->CameraManager()->GetTiltPercent());
						CCD->ActorManager()->SetNextMotion(m_Actor, ANIMSLIDELEFT);
					}
				}
				else
				{
					if(!CCD->Weapons()->GetAttackFlag())
						CCD->ActorManager()->SetMotion(m_Actor, ANIMRUNSLIDELEFT);
					else
					{
						CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMRUNSLIDELEFTSHOOT, ANIMRUNSLIDELEFTSHOOT1, CCD->CameraManager()->GetTiltPercent());
						CCD->ActorManager()->SetNextMotion(m_Actor, ANIMRUNSLIDELEFT);
					}
				}

				break;
			}

			if((Motion == ANIMCIDLE) || (Motion == ANIMC2IDLE)
				|| (Motion == ANIMSLIDECRIGHT) || (Motion == ANIMSLIDECRIGHTSHOOT)
				|| (Motion == ANIMCRAWL) || (Motion == ANIMCRAWLSHOOT)
				|| (Motion == ANIMCSHOOT) || (Motion == ANIMCAIM))
			{
				if(!CCD->Weapons()->GetAttackFlag())
					CCD->ActorManager()->SetMotion(m_Actor, ANIMSLIDECLEFT);
				else
				{
					CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMSLIDECLEFTSHOOT, ANIMSLIDECLEFTSHOOT1, CCD->CameraManager()->GetTiltPercent());
					CCD->ActorManager()->SetNextMotion(m_Actor, ANIMSLIDECLEFT);
				}

				break;
			}

			if(((Motion == ANIMSLIDELEFT) || (Motion == ANIMRUNSLIDELEFT)) && m_Crouch)
			{
				if(!CCD->Weapons()->GetAttackFlag())
					CCD->ActorManager()->SetMotion(m_Actor, ANIMSLIDECLEFT);
				else
				{
					CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMSLIDECLEFTSHOOT, ANIMSLIDECLEFTSHOOT1, CCD->CameraManager()->GetTiltPercent());
					CCD->ActorManager()->SetNextMotion(m_Actor, ANIMSLIDECLEFT);
				}

				CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMCIDLE, true);
				break;
			}

			if(((Motion == ANIMSLIDECLEFT) || (Motion == ANIMSLIDECLEFTSHOOT)) && !m_Crouch)
			{
				if(CCD->ActorManager()->CheckAnimation(m_Actor, ANIMIDLE) == GE_TRUE)
				{
					if(!m_Run)
					{
						if(!CCD->Weapons()->GetAttackFlag())
							CCD->ActorManager()->SetMotion(m_Actor, ANIMSLIDELEFT);
						else
						{
							CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMSLIDELEFTSHOOT, ANIMSLIDELEFTSHOOT1, CCD->CameraManager()->GetTiltPercent());
							CCD->ActorManager()->SetNextMotion(m_Actor, ANIMSLIDELEFT);
						}
					}
					else
					{
						if(!CCD->Weapons()->GetAttackFlag())
							CCD->ActorManager()->SetMotion(m_Actor, ANIMRUNSLIDELEFT);
						else
						{
							CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMRUNSLIDELEFTSHOOT, ANIMRUNSLIDELEFTSHOOT1, CCD->CameraManager()->GetTiltPercent());
							CCD->ActorManager()->SetNextMotion(m_Actor, ANIMRUNSLIDELEFT);
						}
					}

					CCD->ActorManager()->SetAnimationHeight(m_Actor, ANIMIDLE, true);
					break;
				}
			}

			if(CCD->Weapons()->GetAttackFlag() && !m_Crouch)
			{
				if(!m_Run)
				{
					if((Motion == ANIMSLIDELEFTSHOOT))
						CCD->ActorManager()->SetBlendMot(m_Actor, ANIMSLIDELEFTSHOOT, ANIMSLIDELEFTSHOOT1, CCD->CameraManager()->GetTiltPercent());
					else
						CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMSLIDELEFTSHOOT, ANIMSLIDELEFTSHOOT1, CCD->CameraManager()->GetTiltPercent());

					CCD->ActorManager()->SetNextMotion(m_Actor, ANIMSLIDELEFT);
				}
				else
				{
					if((Motion == ANIMRUNSLIDELEFTSHOOT))
						CCD->ActorManager()->SetBlendMot(m_Actor, ANIMRUNSLIDELEFTSHOOT, ANIMRUNSLIDELEFTSHOOT1, CCD->CameraManager()->GetTiltPercent());
					else
						CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMRUNSLIDELEFTSHOOT, ANIMRUNSLIDELEFTSHOOT1, CCD->CameraManager()->GetTiltPercent());

					CCD->ActorManager()->SetNextMotion(m_Actor, ANIMRUNSLIDELEFT);
				}

				break;
			}

			if(CCD->Weapons()->GetAttackFlag() && m_Crouch)
			{
				if((Motion == ANIMSLIDECLEFTSHOOT))
					CCD->ActorManager()->SetBlendMot(m_Actor, ANIMSLIDECLEFTSHOOT, ANIMSLIDECLEFTSHOOT1, CCD->CameraManager()->GetTiltPercent());
				else
					CCD->ActorManager()->SetBlendMotion(m_Actor, ANIMSLIDECLEFTSHOOT, ANIMSLIDECLEFTSHOOT1, CCD->CameraManager()->GetTiltPercent());

				CCD->ActorManager()->SetNextMotion(m_Actor, ANIMSLIDECLEFT);
				break;
			}

			if((Motion == ANIMSLIDELEFT) && m_Run)
			{
				CCD->ActorManager()->SetMotion(m_Actor, ANIMRUNSLIDELEFT);
			}

			if((Motion == ANIMRUNSLIDELEFT) && !m_Run)
			{
				CCD->ActorManager()->SetMotion(m_Actor, ANIMSLIDELEFT);
			}

			if(!ALLSLIDELEFT && !m_Run)
			{
				CCD->ActorManager()->SetMotion(m_Actor, ANIMSLIDELEFT);
				break;
			}

			if(!ALLSLIDELEFT && m_Run)
			{
				CCD->ActorManager()->SetMotion(m_Actor, ANIMRUNSLIDELEFT);
				break;
			}

			break;
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// ViewPoint
//
// Compute the players viewpoint and return it as a transform
/* ------------------------------------------------------------------------------------ */
geXForm3d CPlayer::ViewPoint()
{
	geXForm3d theViewPoint;
	geVec3d Translation;
	geVec3d Rotation;

	CCD->ActorManager()->GetPosition(m_Actor, &Translation);
	CCD->ActorManager()->GetRotation(m_Actor, &Rotation);

	geXForm3d_SetZRotation(&theViewPoint, Rotation.Z); // Rotate then translate
	geXForm3d_RotateX(&theViewPoint, Rotation.X);
	geXForm3d_RotateY(&theViewPoint, Rotation.Y);

	geXForm3d_Translate(&theViewPoint, Translation.X, Translation.Y, Translation.Z);

	return theViewPoint;
}

/* ------------------------------------------------------------------------------------ */
// GetExtBoxWS
//
// Get the players ExtBox values (axially-aligned bounding box) in WORLD SPACE.
/* ------------------------------------------------------------------------------------ */
void CPlayer::GetExtBoxWS(geExtBox *theBox)
{
	CCD->ActorManager()->GetBoundingBox(m_Actor, theBox);
}

/* ------------------------------------------------------------------------------------ */
// GetIn
//
// Get the direction the player avatar is facing
/* ------------------------------------------------------------------------------------ */
void CPlayer::GetIn(geVec3d *In)
{
	CCD->ActorManager()->InVector(m_Actor, In);
}

/* ------------------------------------------------------------------------------------ */
// GetLeft
/* ------------------------------------------------------------------------------------ */
void CPlayer::GetLeft(geVec3d *Left)
{
	CCD->ActorManager()->LeftVector(m_Actor, Left);
}

/* ------------------------------------------------------------------------------------ */
// SaveTo
//
// Save the current player state to an open file.
/* ------------------------------------------------------------------------------------ */
int CPlayer::SaveTo(FILE *SaveFD)
{
	int i;

	fwrite(m_PlayerName.c_str(),sizeof(char),		64,	SaveFD);
	fwrite(&m_ViewPoint,		sizeof(int),		1,	SaveFD);
	geVec3d Vec = Position();
	fwrite(&Vec,				sizeof(geVec3d),	1,	SaveFD);
	CCD->ActorManager()->GetRotate(m_Actor, &Vec);
	fwrite(&Vec,				sizeof(geVec3d),	1,	SaveFD);
	fwrite(&m_JumpActive,		sizeof(bool),		1,	SaveFD);
	fwrite(&m_Jumping,			sizeof(geBoolean),	1,	SaveFD);
	fwrite(&m_Run,				sizeof(bool),		1,	SaveFD);
	fwrite(&m_Running,			sizeof(geBoolean),	1,	SaveFD);
	fwrite(&m_Crouch,			sizeof(bool),		1,	SaveFD);
	fwrite(&m_OldCrouch,		sizeof(bool),		1,	SaveFD);
	fwrite(&m_LastDirection,	sizeof(int),		1,	SaveFD);
	fwrite(&m_Falling,			sizeof(bool),		1,	SaveFD);
	fwrite(&m_OldFalling,		sizeof(bool),		1,	SaveFD);
	fwrite(&m_FallStart,		sizeof(geVec3d),	1,	SaveFD);
	fwrite(&m_LastHealth,		sizeof(int),		1,	SaveFD);
	fwrite(&m_StaminaTime,		sizeof(geFloat),	1,	SaveFD);

	for(i=0; i<20; ++i)
	{
		fwrite(&m_StaminaTime1[i],sizeof(geFloat),	1,	SaveFD);
	}

	fwrite(&m_LightOn,			sizeof(bool),		1,	SaveFD);
	fwrite(&m_CurrentLiteLife,	sizeof(geFloat),	1,	SaveFD);
	fwrite(&m_DecayLite,		sizeof(bool),		1,	SaveFD);
	fwrite(&m_CurrentLiteDecay,	sizeof(geFloat),	1,	SaveFD);
	fwrite(&m_LastMovementSpeed,sizeof(geFloat),	1,	SaveFD);
	// TODO:
	//fwrite(&m_Gravity,			sizeof(geVec3d),	1,	SaveFD);
	fwrite(&m_StepHeight,		sizeof(geFloat),	1,	SaveFD);
	fwrite(&m_InLiquid,			sizeof(int),		1,	SaveFD);
	fwrite(&m_LiquidTime,		sizeof(float),		1,	SaveFD);
	fwrite(m_UseAttribute,		sizeof(char),	64*10,	SaveFD);
	fwrite(&m_RestoreOxy,		sizeof(bool),		1,	SaveFD);

	geFloat Level, Decay;

	for(i=0; i<4; ++i)
	{
		CCD->ActorManager()->GetForce(m_Actor, i, &Vec, &Level, &Decay);
		fwrite(&Vec,			sizeof(geVec3d),	1,	SaveFD);
		fwrite(&Level,			sizeof(geFloat),	1,	SaveFD);
		fwrite(&Decay,			sizeof(geFloat),	1,	SaveFD);
	}

	//	If we have attributes, save them.
	bool bAttributes = false;

	m_Attr = CCD->ActorManager()->Inventory(m_Actor);

	if(m_Attr)
	{
		bAttributes = true;
		fwrite(&bAttributes,	sizeof(bool),		1,	SaveFD);
		m_Attr->SaveTo(SaveFD, false);
	}
	else
	{
		fwrite(&bAttributes,	sizeof(bool),		1,	SaveFD);
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// RestoreFrom
//
// Restore the player state from an open file.
/* ------------------------------------------------------------------------------------ */
int CPlayer::RestoreFrom(FILE *RestoreFD)
{
	int i;
	char buffer[64];
	fread(buffer,				sizeof(char),		64, RestoreFD);
	m_PlayerName = buffer;
	fread(&m_ViewPoint,			sizeof(int),		1,	RestoreFD);
	SwitchCamera(m_ViewPoint);
	geVec3d Vec;
	fread(&Vec,					sizeof(geVec3d),	1,	RestoreFD);
	CCD->ActorManager()->Position(m_Actor, Vec);
	fread(&Vec,					sizeof(geVec3d),	1,	RestoreFD);
	CCD->ActorManager()->Rotate(m_Actor, Vec);
	fread(&m_JumpActive,		sizeof(bool),		1,	RestoreFD);
	fread(&m_Jumping,			sizeof(geBoolean),	1,	RestoreFD);
	fread(&m_Run,				sizeof(bool),		1,	RestoreFD);
	fread(&m_Running,			sizeof(geBoolean),	1,	RestoreFD);
	fread(&m_Crouch,			sizeof(bool),		1,	RestoreFD);
	fread(&m_OldCrouch,			sizeof(bool),		1,	RestoreFD);
	fread(&m_LastDirection,		sizeof(int),		1,	RestoreFD);
	fread(&m_Falling,			sizeof(bool),		1,	RestoreFD);
	fread(&m_OldFalling,		sizeof(bool),		1,	RestoreFD);
	fread(&m_FallStart,			sizeof(geVec3d),	1,	RestoreFD);
	fread(&m_LastHealth,		sizeof(int),		1,	RestoreFD);
	fread(&m_StaminaTime,		sizeof(geFloat),	1,	RestoreFD);

	for(i=0; i<20; ++i)
	{
		fread(&m_StaminaTime1[i], sizeof(geFloat),	1,	RestoreFD);
	}

	fread(&m_LightOn,			sizeof(bool),		1,	RestoreFD);
	fread(&m_CurrentLiteLife,	sizeof(geFloat),	1,	RestoreFD);
	fread(&m_DecayLite,			sizeof(bool),		1,	RestoreFD);
	fread(&m_CurrentLiteDecay,	sizeof(geFloat),	1,	RestoreFD);
	fread(&m_LastMovementSpeed, sizeof(geFloat),	1,	RestoreFD);
	// TODO:
	//fread(&m_Gravity,			sizeof(geVec3d),	1,	RestoreFD);
	CCD->ActorManager()->SetGravity(m_Actor, CCD->Level()->GetGravity());
	fread(&m_StepHeight,		sizeof(geFloat),	1,	RestoreFD);
	CCD->ActorManager()->SetStepHeight(m_Actor, m_StepHeight);
	CCD->ActorManager()->SetAutoStepUp(m_Actor, true);
	fread(&m_InLiquid,			sizeof(int),		1,	RestoreFD);
	fread(&m_LiquidTime,		sizeof(float),		1,	RestoreFD);
	fread(m_UseAttribute,		sizeof(char),	64*10,	RestoreFD);
	fread(&m_RestoreOxy,		sizeof(bool),		1,	RestoreFD);

	geFloat Level, Decay;

	for(i=0; i<4; ++i)
	{
		fread(&Vec,				sizeof(geVec3d),	1,	RestoreFD);
		fread(&Level,			sizeof(geFloat),	1,	RestoreFD);
		fread(&Decay,			sizeof(geFloat),	1,	RestoreFD);
		CCD->ActorManager()->SetForce(m_Actor, i, Vec, Level, Decay);
	}

	bool bAttributes;
	fread(&bAttributes,			sizeof(bool),		1,	RestoreFD);

	if(bAttributes)
	{
		m_Attr = CCD->ActorManager()->Inventory(m_Actor);
		m_Attr->RestoreFrom(RestoreFD, false);
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// LookMode
//
// If in third-person mode, set flags such that (temporarily) the camera locks
// into first-person perspective, although movement is NOT permitted while in LookMode.
/* ------------------------------------------------------------------------------------ */
void CPlayer::LookMode(bool bLookOn)
{
	// First, this is effective ONLY if we're in third-person mode..
	if(m_FirstPersonView)
		return;								// Ignore the call.

	if(bLookOn == m_LookMode)
		return;								// Mode already set.

	if(bLookOn)
	{
		m_LookMode = true;
		CCD->ActorManager()->SetActorFlags(m_Actor, GE_ACTOR_COLLIDE | GE_ACTOR_RENDER_MIRRORS);
		geExtBox theBox;
		CCD->ActorManager()->GetBoundingBox(m_Actor, &theBox);
		CCD->CameraManager()->GetCameraOffset(&m_LookPosition, &m_LookRotation);
		m_LookFlags = CCD->CameraManager()->GetTrackingFlags();
		CCD->CameraManager()->SetTrackingFlags(kCameraTrackPosition);
		geVec3d theRotation = {0.0f, 0.0f, 0.0f};
		geVec3d theTranslation = {0.0f, 0.0f, 0.0f};
		theTranslation.Y = theBox.Max.Y * 0.75f;
		CCD->CameraManager()->SetCameraOffset(theTranslation, theRotation);
	}
	else
	{
		m_LookMode = false;
		CCD->ActorManager()->SetActorFlags(m_Actor,
			GE_ACTOR_RENDER_NORMAL | GE_ACTOR_COLLIDE | GE_ACTOR_RENDER_MIRRORS);
		CCD->CameraManager()->SetTrackingFlags(m_LookFlags);
		CCD->CameraManager()->SetCameraOffset(m_LookPosition, m_LookRotation);
	}
}

/* ------------------------------------------------------------------------------------ */
// SwitchToFirstPerson
//
// If the player is not in a first-person view already, switch to first-person mode.
/* ------------------------------------------------------------------------------------ */
void CPlayer::SwitchToFirstPerson()
{
	m_FirstPersonView = true;
	m_LookMode = false;
	CCD->ActorManager()->SetActorFlags(m_Actor, GE_ACTOR_COLLIDE | m_Mirror);
}

/* ------------------------------------------------------------------------------------ */
// SwitchToThirdPerson
//
// If the player is not in a third-person view already, switch to third-person mode.
/* ------------------------------------------------------------------------------------ */
void CPlayer::SwitchToThirdPerson()
{
	m_FirstPersonView = false;
	m_LookMode = false;
	CCD->ActorManager()->SetActorFlags(m_Actor,
		GE_ACTOR_RENDER_NORMAL | GE_ACTOR_COLLIDE | GE_ACTOR_RENDER_MIRRORS);
}

/* ------------------------------------------------------------------------------------ */
// SaveAttributes
//
// Save off all attributes and inventory to a file.  Mainly used to
// ..preserve player data across level changes.
/* ------------------------------------------------------------------------------------ */
int CPlayer::SaveAttributes(const std::string& saveFile)
{
	FILE *file = CFileManager::GetSingletonPtr()->OpenRFFile(kConfigFile, saveFile.c_str(), "wb");

	if(file == NULL)
	{
		CCD->Log()->Warning("File %s - Line %d: Failed to SaveAttributes to '%s'",
							__FILE__, __LINE__, saveFile.c_str());
		return RGF_FAILURE;
	}

	m_Attr = CCD->ActorManager()->Inventory(m_Actor);
	m_Attr->SaveTo(file, false);

	fclose(file);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SaveAttributesAscii
/* ------------------------------------------------------------------------------------ */
int CPlayer::SaveAttributesAscii(const std::string& saveFile)
{
	if(!CCD->SaveAttributesAsText())
		return RGF_SUCCESS;

	FILE *file = CFileManager::GetSingletonPtr()->OpenRFFile(kConfigFile, saveFile.c_str(), "wt");

	if(file == NULL)
	{
		CCD->Log()->Warning("File %s - Line %d: Failed to SaveAttributes to '%s'",
							__FILE__, __LINE__, saveFile.c_str());
		return RGF_FAILURE;
	}

	m_Attr = CCD->ActorManager()->Inventory(m_Actor);
	m_Attr->SaveAscii(file);

	fclose(file);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// LoadAttributes
//
// Load all attributes and inventory from a file.  Mainly used to
// ..restore player data after a level change.
/* ------------------------------------------------------------------------------------ */
int CPlayer::LoadAttributes(const std::string& saveFile)
{
	FILE *file = CFileManager::GetSingletonPtr()->OpenRFFile(kConfigFile, saveFile.c_str(), "rb");

	if(file == NULL)
	{
		CCD->Log()->Warning("File %s - Line %d: Failed to LoadAttributes from '%s'",
							__FILE__, __LINE__, saveFile.c_str());
		return RGF_FAILURE;
	}

	m_Attr = CCD->ActorManager()->Inventory(m_Actor);
	m_Attr->RestoreFrom(file, false);

	fclose(file);

	sxInventory::GetSingletonPtr()->Update();

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// Backtrack
//
// Move player to the previous position in the position history.
/* ------------------------------------------------------------------------------------ */
void CPlayer::Backtrack()
{
	--m_PositionHistoryPtr;

	if(m_PositionHistoryPtr < 0)
		m_PositionHistoryPtr = 49;

	m_Translation = m_PositionHistory[m_PositionHistoryPtr];
	CCD->ActorManager()->Position(m_Actor, m_Translation);
}

/* ------------------------------------------------------------------------------------ */
// AddPosition
//
// Add the players current position to the position history ring buffer
/* ------------------------------------------------------------------------------------ */
void CPlayer::AddPosition()
{
	Position();
	if(geVec3d_Compare(&m_PositionHistory[m_PositionHistoryPtr], &m_Translation, 0.0f))
		return;

	++m_PositionHistoryPtr;

	if(m_PositionHistoryPtr >= 50)
		m_PositionHistoryPtr = 0;

	m_PositionHistory[m_PositionHistoryPtr] = m_Translation;
}

/* ------------------------------------------------------------------------------------ */
// LoadAudioClip
//
// Load an audio clip and return the geSound pointer
/* ------------------------------------------------------------------------------------ */
geSound_Def* CPlayer::LoadAudioClip(const std::string& filename)
{
	if(!filename.empty())
	{
		geSound_Def *SoundPointer = SPool_Sound(filename.c_str());
		return SoundPointer;
	}

	return NULL;
}

/* ------------------------------------------------------------------------------------ */
// SetCrouch
/* ------------------------------------------------------------------------------------ */
void CPlayer::SetCrouch(bool value)
{
	m_Crouch = value;
}

/* ------------------------------------------------------------------------------------ */
// DoMovements
/* ------------------------------------------------------------------------------------ */
bool CPlayer::DoMovements()
{
	// Ok, if the player moved, handle any processing necessary to reflect
	// ..that fact in the world.
	bool bPlayerMoved = false;
	m_SpeedCoeff = 1.0f;		// Coef de course
	int theZone, OldZone;

	CCD->ActorManager()->GetActorZone(m_Actor, &theZone);
	CCD->ActorManager()->GetActorOldZone(m_Actor, &OldZone);

	if(OldZone > 0 || theZone == kLiquidZone || theZone == kInLiquidZone)
	{
		Liquid *LQ = CCD->ActorManager()->GetLiquid(m_Actor);
		if(LQ)
		{
			m_SpeedCoeff = LQ->SpeedCoeff * 0.01f;
		}
	}
	else if(theZone & kLavaZone)
	{
		m_SpeedCoeff = CCD->Level()->GetSpeedCoeffLava();
	}
	else if(theZone & kSlowMotionZone)
	{
		m_SpeedCoeff = CCD->Level()->GetSpeedCoeffSlowMotion();
	}
	else if(theZone & kFastMotionZone)
	{
		m_SpeedCoeff = CCD->Level()->GetSpeedCoeffFastMotion();
	}

	// A little run wanted ?
	if((m_Running || m_AlwaysRun) && !m_Crouch && OldZone == 0)	// Don't run when crouch
	{
		if(CCD->ActorManager()->Falling(m_Actor) != GE_TRUE)	// If we are not falling, adjust speed modifications
			m_SpeedCoeff *= m_SpeedCoeffRun;
	}

	// Want a jump ?
	if(m_Jumping)
		StartJump();

	if(GetMoving() != MOVEIDLE)
	{
		geFloat fTemp;

		switch(GetMoving())
		{
		case MOVEWALKFORWARD:
			fTemp = Speed() * CCD->LastElapsedTime_F() * 0.05f * m_SpeedCoeff;

			if(GetSlideWalk() == MOVESLIDELEFT)
				Move(RGF_K_LEFT, fTemp);
			if(GetSlideWalk() == MOVESLIDERIGHT)
				Move(RGF_K_RIGHT, fTemp);

			Move(RGF_K_FORWARD, fTemp);
			bPlayerMoved = true;
			break;
		case MOVEWALKBACKWARD:
			// Some hard coding. TODO : make configurable
			fTemp = Speed() * CCD->LastElapsedTime_F() * 0.05f * m_SpeedCoeff * m_SpeedCoeffBackward;

			if(GetSlideWalk() == MOVESLIDELEFT)
				Move(RGF_K_LEFT, fTemp);
			if(GetSlideWalk() == MOVESLIDERIGHT)
				Move(RGF_K_RIGHT, fTemp);

			Move(RGF_K_BACKWARD, fTemp);
			bPlayerMoved = true;
			break;
		case MOVESLIDELEFT:
			if(OldZone == 0)
			{
				// Some hard coding. TODO : make configurable
				fTemp = Speed() * CCD->LastElapsedTime_F() * 0.05f * m_SpeedCoeff * m_SpeedCoeffSideward;
				Move(RGF_K_LEFT, fTemp);

				if(GetSlideWalk() == MOVEWALKFORWARD)
				{
					Move(RGF_K_FORWARD, fTemp);
				}

				if(GetSlideWalk() == MOVEWALKBACKWARD)
				{
					Move(RGF_K_BACKWARD, fTemp);
				}

				bPlayerMoved = true;
			}
			else
			{
				m_Moving = MOVEIDLE;
			}
			break;
		case MOVESLIDERIGHT:
			if(OldZone == 0)
			{
				// Some hard coding. TODO : make configurable
				fTemp = Speed() * CCD->LastElapsedTime_F() * 0.05f * m_SpeedCoeff * m_SpeedCoeffSideward;
				Move(RGF_K_RIGHT, fTemp);

				if(GetSlideWalk() == MOVEWALKFORWARD)
				{
					Move(RGF_K_FORWARD, fTemp);
				}

				if(GetSlideWalk() == MOVEWALKBACKWARD)
				{
					Move(RGF_K_BACKWARD, fTemp);
				}

				bPlayerMoved = true;
			}
			else
			{
				m_Moving = MOVEIDLE;
			}
			break;
		default:
			break;
		}

		if(!CCD->Weapons()->GetAllowMoveZoom())
			CCD->CameraManager()->CancelZoom();
	}
	else
	{
		Move(RGF_K_FORWARD, 0);
	}

	return bPlayerMoved;
}

/* ------------------------------------------------------------------------------------ */
// UseItem
/* ------------------------------------------------------------------------------------ */
void CPlayer::UseItem()
{
	geVec3d theRotation;
	geVec3d thePosition;
	geXForm3d Xf;
	geExtBox theBox, pBox;
	geVec3d Direction, Front, Back;
	GE_Collision Collision;

	theBox.Min.X = theBox.Min.Y = theBox.Min.Z = -0.49f * m_BoxWidth;
	theBox.Max.X = theBox.Max.Y = theBox.Max.Z =  0.49f * m_BoxWidth;

	if(m_ViewPoint == FIRSTPERSON)
	{
		CCD->Weapons()->Use();
		CCD->CameraManager()->GetRotation(&theRotation);
		CCD->CameraManager()->GetPosition(&thePosition);

		geXForm3d_SetZRotation(&Xf, theRotation.Z);
		geXForm3d_RotateX(&Xf, theRotation.X);
		geXForm3d_RotateY(&Xf, theRotation.Y);
	}
	else
	{
		CCD->ActorManager()->GetPosition(m_Actor, &thePosition);
		CCD->ActorManager()->GetBoundingBox(m_Actor, &pBox);
		thePosition.Y += (pBox.Max.Y * 0.5f);
		CCD->ActorManager()->GetRotate(m_Actor, &theRotation);
		geVec3d CRotation, CPosition;
		CCD->CameraManager()->GetCameraOffset(&CPosition, &CRotation);

		geXForm3d_SetZRotation(&Xf, theRotation.Z);
		geXForm3d_RotateX(&Xf, theRotation.X - CCD->CameraManager()->GetTilt());
		geXForm3d_RotateY(&Xf, theRotation.Y);
	}

	geXForm3d_GetIn(&Xf, &Direction);
	geVec3d_AddScaled(&thePosition, &Direction, m_UseRange, &Back);
	geVec3d_Copy(&thePosition, &Front);

	Collision.Actor = NULL;
	Collision.Model = NULL;
	CCD->Collision()->IgnoreContents(false);

	if(CCD->Collision()->CheckForWCollision(&theBox.Min, &theBox.Max,
		Front, Back, &Collision, m_Actor))
	{
		if(Collision.Model)
		{
			if(CCD->Level()->ChangeLevels()->CheckChangeLevel(Collision.Model, true))
			{
				CCD->SetChangeLevel(true);	// We hit a change level
				return;
			}

			if(CCD->Level()->Doors()->HandleCollision(Collision.Model, false, true, m_Actor))
				return;

			// We can't just return now, there may be triggers attached to the Moving Platform that use keys also
			CCD->Level()->Platforms()->HandleCollision(Collision.Model, false, true, m_Actor);

			if(CCD->Level()->Triggers()->HandleCollision(Collision.Model, false, true, m_Actor) == RGF_SUCCESS)
				return;
		}

		if(Collision.Actor)
		{
			if(CCD->Level()->Props()->HandleCollision(Collision.Actor, m_Actor, true, true) == RGF_SUCCESS)
				return;

			if(CCD->Level()->Attributes()->HandleCollision(m_Actor, Collision.Actor, true) == GE_TRUE)
				return;

			if(CCD->Level()->Pawns()->Converse(Collision.Actor))
				return;
		}
	}
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
bool CPlayer::GetUseAttribute(const std::string& attr)
{
	int i;

	for(i=0; i<10; ++i)
	{
		if(attr == m_UseAttribute[i])
			return true;
	}

	return false;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
bool CPlayer::SetUseAttribute(const std::string& attr)
{
	int i;

	CCD->Armours()->DisableHud(attr);
	CCD->Level()->LiftBelts()->DisableHud(attr);

	for(i=0; i<10; ++i)
	{
		if(m_UseAttribute[i].empty())
		{
			m_UseAttribute[i] = attr;
			return true;
		}
	}

	return false;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
bool CPlayer::DelUseAttribute(const std::string& attr)
{
	int i;

	for(i=0; i<10; ++i)
	{
		if(attr == m_UseAttribute[i])
		{
			m_UseAttribute[i].clear();
			return true;
		}
	}

	return false;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
int CPlayer::PlaySound(const char* sound, int soundHandle /*= -1*/, geBoolean loop /*= GE_TRUE*/, float radius /*= -1.0f*/)
{
	if(!sound)
		return soundHandle;

	StopSound(soundHandle);

	Snd Sound;
	memset(&Sound, 0, sizeof(Sound));

	CCD->ActorManager()->GetPosition(m_Actor, &Sound.Pos);

	if(radius > 0.0f)
		Sound.Min = radius;
	else
		Sound.Min = CCD->Level()->GetAudibleRadius();

	Sound.Loop = loop;
	Sound.SoundDef = SPool_Sound(sound);

	int newSoundHandle = -1;
	if(Sound.SoundDef != NULL)
	{
		newSoundHandle = CCD->EffectManager()->Item_Add(EFF_SND, static_cast<void*>(&Sound));

		if(newSoundHandle != -1)
			m_SoundHandles.insert(newSoundHandle);
	}

	return newSoundHandle;
}


/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CPlayer::UpdateSounds()
{
	geVec3d position;
	CCD->ActorManager()->GetPosition(m_Actor, &position);

	for(std::set<int>::iterator iter = m_SoundHandles.begin(); iter != m_SoundHandles.end();)
	{
		// update sound position
		if(CCD->EffectManager()->Item_Alive(*iter))
		{
			Snd sound;
			memset(&sound, 0, sizeof(sound));

			sound.Pos = position;

			CCD->EffectManager()->Item_Modify(EFF_SND, *iter, static_cast<void*>(&sound), SND_POS);
			++iter;
		}
		else
		{
			m_SoundHandles.erase(iter++); // increment to next iterator then erase current iterator
		}
	}

	if(m_StreamingAudio)
	{
		if(m_StreamingAudio->IsPlaying())
		{
			position = GetSpeakBonePosition();
			m_StreamingAudio->Modify3D(&position, CCD->Level()->GetAudibleRadius());
		}
		else
		{
			m_StreamingAudio->Delete();
			SAFE_DELETE(m_StreamingAudio);
		}
	}
}


/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
void CPlayer::StopSound(int soundHandle)
{
	if(soundHandle != -1)
	{
		CCD->EffectManager()->Item_Delete(EFF_SND, soundHandle);
		m_SoundHandles.erase(soundHandle);
	}
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
geVec3d CPlayer::GetSpeakBonePosition()
{
	if(m_SpeakBone.empty())
	{
		return Position();
	}
	else
	{
		geVec3d position;
		if(CCD->ActorManager()->GetBonePosition(m_Actor, m_SpeakBone, &position) != RGF_SUCCESS)
		{
			CCD->Log()->Debug("Failed to find bone '%s' in actor '%s'", m_SpeakBone, m_ActorName);
			return Position();
		}
		return position;
	}
}

/* ------------------------------------------------------------------------------------ */
#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE CPlayer

Q_METHOD_IMPL(Speak)
{
	if(sxConversationManager::GetSingletonPtr()->IsConversing())
	{
		skRValue r_val;
		skRValueArray setIconArgs;
		setIconArgs.append(skString(m_Icon.c_str()));
		sxConversationManager::M_SetIcon(sxConversationManager::GetSingletonPtr(), setIconArgs, r_val, ScriptManager::GetContext());
		skRValue pos(new RFSX::sxVector(GetSpeakBonePosition()), true);
		args.append(pos);
		return sxConversationManager::M_Speak(sxConversationManager::GetSingletonPtr(), args, r_val, ScriptManager::GetContext());
	}
	else
	{
		// TODO display text?
		if(args.entries() > 1)
		{
			if(args[1].type() != skRValue::T_String)
				return true;

			if(args[1].str() != "")
			{
				if(m_StreamingAudio)
				{
					m_StreamingAudio->Delete();
					delete m_StreamingAudio;
				}

				m_StreamingAudio = new StreamingAudio((LPDIRECTSOUND)geSound_GetDSound());

				if(m_StreamingAudio)
				{
					char filename[_MAX_PATH];
					strcpy(filename, CCD->LanguageManager()->GetActiveLanguage()->GetDubbingDirectory().c_str());
					strcat(filename, "\\");
					strcat(filename, args[1].str().c_str());

					if(!m_StreamingAudio->Create(filename))
					{
						SAFE_DELETE(m_StreamingAudio);
					}
					else
					{
						m_StreamingAudio->Play(false);
					}
				}
			}
		}
	}

	return true;
}


bool SX_IMPL_TYPE::method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt)
{
	if(h_method(this, m_name, args, r_val, ctxt))
		return true;
	else
		return skExecutable::method(m_name, args, r_val, ctxt);
}


bool SX_IMPL_TYPE::getValue(const skString& name, const skString& att, skRValue& val)
{
	if(name == "position")
	{
		val.assignObject(new RFSX::sxVector(Position()), true);
		return true;
	}
	if(name == "screenPosition")
	{
		geVec3d pos, screenPos;

		pos = Position();
		geCamera_TransformAndProject(CCD->CameraManager()->Camera(), &pos, &screenPos);

		val.assignObject(new RFSX::sxVector2(screenPos.X, screenPos.Y), true);
		return true;
	}
	if(name == "viewPoint")
	{
		val = GetViewPoint();
		return true;
	}
	if(name == "weapon")
	{
		val = skString(CCD->Weapons()->GetCurrent().c_str());
		return true;
	}
	if(name == "animation")
	{
		val = skString(CCD->ActorManager()->GetMotion(GetActor()).c_str());
	}

	return skExecutable::getValue(name, att, val);
}

//////////////////////////////////////////////////////////////////////////////////////////
// CPlayer method functions
//////////////////////////////////////////////////////////////////////////////////////////

SX_METHOD_IMPL(GetName)
{
	r_val = skString(caller->GetPlayerName().c_str());
	return true;
}


SX_METHOD_IMPL(SetWeapon)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() == skRValue::T_Int)
	{
		int slot = args[0].intValue();
		if(slot < 0 || slot >= MAX_WEAPONS)
		{
			CCD->Weapons()->Holster();
		}
		else
		{
			CCD->Weapons()->SetWeapon(slot);
		}

		return true;
	}
	else if(args[0].type() == skRValue::T_String)
	{
		CCD->Weapons()->SetWeapon(args[0].str().c_str());
		return true;
	}

	return false;
}


SX_METHOD_IMPL(SetWeaponMatFromFlip)
{
	if(args.entries() != 6)
		return false;

	FlipBook *pEntityData = NULL;

	r_val = false;

	geActor *wActor;

	if(caller->GetViewPoint() == FIRSTPERSON)
		wActor = CCD->Weapons()->GetVActor();
	else
		wActor = CCD->Weapons()->GetPActor();

	if(wActor)
	{
		if(CCD->Level()->FlipBooks()->LocateEntity(args[0].str().c_str(), reinterpret_cast<void**>(&pEntityData)) == RGF_SUCCESS)
		{
			int matIndex = args[1].intValue();
			int flipIndex = args[2].intValue();
			float r = args[3].floatValue();
			float g = args[4].floatValue();
			float b = args[5].floatValue();
			if(geActor_SetMaterial(wActor, matIndex, pEntityData->Bitmap[flipIndex], r, g, b))
				r_val = true; // Set Actor Material
		}
	}

	return true;
}


SX_METHOD_IMPL(ChangeWeaponMaterial) // ChangeWeaponMaterial(string materialSection)
{
	if(args.entries() != 1)
		return false;

	geActor *wActor;

	if(caller->GetViewPoint() == FIRSTPERSON)
		wActor = CCD->Weapons()->GetVActor();
	else
		wActor = CCD->Weapons()->GetPActor();

	if(wActor)
	{
		CCD->ActorManager()->ChangeMaterial(wActor, args[0].str().c_str());
	}

	return true;
}


SX_METHOD_IMPL(SetUseItem)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	if(caller->SetUseAttribute(args[0].str().c_str()))
		CCD->HUD()->ActivateElement(args[0].str().c_str(), true);

	return true;
}


SX_METHOD_IMPL(ClearUseItem)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CCD->HUD()->ActivateElement(args[0].str().c_str(), false);
	caller->DelUseAttribute(args[0].str().c_str());
	return true;
}


SX_METHOD_IMPL(HasAttribute)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CPersistentAttributes *attributes = CCD->ActorManager()->Inventory(caller->GetActor());
	r_val = attributes->Has(args[0].str().c_str());
	return true;
}


SX_METHOD_IMPL(SetAttribute)
{
	skRValue::RType rtypes[] = { skRValue::T_String, skRValue::T_Int };

	if(!RFSX::CheckParams(2, 2, args, rtypes))
		return false;

	CPersistentAttributes *attributes = CCD->ActorManager()->Inventory(caller->GetActor());
	attributes->Set(args[0].str().c_str(), args[1].intValue());

	sxInventory::GetSingletonPtr()->UpdateItem(args[0].str().c_str(), true);

	return true;
}


SX_METHOD_IMPL(GetAttribute)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CPersistentAttributes *attributes = CCD->ActorManager()->Inventory(caller->GetActor());
	r_val = attributes->Value(args[0].str().c_str());
	return true;
}


SX_METHOD_IMPL(ModifyAttribute)
{
	skRValue::RType rtypes[] = { skRValue::T_String, skRValue::T_Int };

	if(!RFSX::CheckParams(2, 2, args, rtypes))
		return false;

	CPersistentAttributes *attributes = CCD->ActorManager()->Inventory(caller->GetActor());
	attributes->Modify(args[0].str().c_str(), args[1].intValue());

	sxInventory::GetSingletonPtr()->UpdateItem(args[0].str().c_str(), true);

	return true;
}


SX_METHOD_IMPL(AddAttribute)
{
	skRValue::RType rtypes[] = { skRValue::T_String, skRValue::T_Int, skRValue::T_Int };

	if(!RFSX::CheckParams(1, 3, args, rtypes))
		return false;

	CPersistentAttributes *attributes = CCD->ActorManager()->Inventory(caller->GetActor());
	attributes->Add(args[0].str().c_str());

	if(args.entries() > 2)
	{
		attributes->SetValueLimits(args[0].str().c_str(), args[1].intValue(), args[2].intValue());
	}

	sxInventory::GetSingletonPtr()->UpdateItem(args[0].str().c_str());

	return true;
}


SX_METHOD_IMPL(SetAttributeValueLimits)
{
	skRValue::RType rtypes[] = { skRValue::T_String, skRValue::T_Int, skRValue::T_Int };

	if(!RFSX::CheckParams(3, 3, args, rtypes))
		return false;

	CPersistentAttributes *attributes = CCD->ActorManager()->Inventory(caller->GetActor());
	attributes->SetValueLimits(args[0].str().c_str(), args[1].intValue(), args[2].intValue());
	return true;
}


SX_METHOD_IMPL(GetAttributeLowLimit)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CPersistentAttributes *attributes = CCD->ActorManager()->Inventory(caller->GetActor());
	r_val = attributes->Low(args[0].str().c_str());
	return true;
}


SX_METHOD_IMPL(GetAttributeHighLimit)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CPersistentAttributes *attributes = CCD->ActorManager()->Inventory(caller->GetActor());
	r_val = attributes->High(args[0].str().c_str());
	return true;
}


SX_METHOD_IMPL(PowerUp)
{
	skRValue::RType rtypes[] = { skRValue::T_String, skRValue::T_Int, skRValue::T_Bool };

	if(!RFSX::CheckParams(3, 3, args, rtypes))
		return false;

	CPersistentAttributes *attributes = CCD->ActorManager()->Inventory(caller->GetActor());
	attributes->SetHighLimit(args[0].str().c_str(), args[1].intValue());

	if(args[2].boolValue())
		attributes->Set(args[0].str().c_str(), args[1].intValue());

	sxInventory::GetSingletonPtr()->UpdateItem(args[0].str().c_str(), true);

	return true;
}


SX_METHOD_IMPL(GetPowerUpLevel)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CPersistentAttributes *attributes = CCD->ActorManager()->Inventory(caller->GetActor());
	r_val = attributes->GetPowerUpLevel(args[0].str().c_str());
	return true;
}


SX_METHOD_IMPL(SetMouseControlled)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	CCD->SetMouseControl(args[0].boolValue());
	return true;
}


SX_METHOD_IMPL(AttachBlendActor)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	geActor *slave = CCD->ActorManager()->GetByEntityName(args[0].str().c_str());
	if(!slave)
		return false;

	CCD->ActorManager()->ActorAttachBlend(slave, caller->GetActor());
	return true;
}


SX_METHOD_IMPL(DetachBlendActor)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	geActor *slave = CCD->ActorManager()->GetByEntityName(args[0].str().c_str());
	if(!slave)
		return false;

	CCD->ActorManager()->DetachBlendFromActor(caller->GetActor(), slave);
	return true;
}


SX_METHOD_IMPL(AttachAccessory)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	const std::vector<ActorPreCache> accessory = CCD->Level()->Pawns()->GetAccessoryCache();

	std::string entityName("Player");

	unsigned int keynum = accessory.size();

	for(unsigned int i=0; i<keynum; ++i)
	{
		if(accessory[i].Name == args[0].str().c_str())
		{
			geActor *slave;
			slave = CCD->ActorManager()->SpawnActor(accessory[i].ActorName,
						caller->Position(), accessory[i].Rotation, "", "", NULL);

			if(!slave)
				return false;

			entityName += args[0].str().c_str();
			CCD->ActorManager()->SetEntityName(slave, entityName);

			if(accessory[i].EnvironmentMapping)
			{
				SetEnvironmentMapping(	slave, true,
										accessory[i].AllMaterial,
										accessory[i].PercentMapping,
										accessory[i].PercentMaterial);
			}

			CCD->ActorManager()->ActorAttachBlend(slave, caller->GetActor());
			return true;
		}
	}

	return false;
}


SX_METHOD_IMPL(DetachAccessory)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	std::string entityName("Player");
	entityName += args[0].str().c_str();

	geActor *slave = CCD->ActorManager()->GetByEntityName(entityName.c_str());
	if(!slave)
		return false;

	CCD->ActorManager()->DetachBlendFromActor(caller->GetActor(), slave);
	CCD->ActorManager()->RemoveActorCheck(slave);
	geActor_Destroy(&slave);

	return true;
}


SX_METHOD_IMPL(Render)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Bool)
		return false;

	if(args[0].boolValue())
	{
		CCD->ActorManager()->SetCollide(caller->GetActor());
		CCD->Weapons()->Rendering(true);
	}
	else
	{
		CCD->ActorManager()->SetNoCollide(caller->GetActor());
		CCD->ActorManager()->SetActorFlags(caller->GetActor(), 0);
		CCD->Weapons()->Rendering(false);
	}

	return true;
}


SX_METHOD_IMPL(MatchEntityAngles)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	geActor *master = CCD->ActorManager()->GetByEntityName(args[0].str().c_str());

	if(!master)
		return false;

	geVec3d rotation;

	CCD->ActorManager()->GetRotate(master, &rotation);
	CCD->ActorManager()->Rotate(caller->GetActor(), rotation);

	return true;
}


SX_METHOD_IMPL(ChangeMaterial)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	CCD->ActorManager()->ChangeMaterial(caller->GetActor(), args[0].str().c_str());
	return true;
}


SX_METHOD_IMPL(SetLighting)
{
	if(args.entries() != 7)
		return false;

	GE_RGBA FillColor;
	GE_RGBA AmbientColor;

	FillColor.a = AmbientColor.a = 255.f;

	FillColor.r = args[0].floatValue();
	FillColor.g = args[1].floatValue();
	FillColor.b = args[2].floatValue();

	AmbientColor.r = args[3].floatValue();
	AmbientColor.g = args[4].floatValue();
	AmbientColor.b = args[5].floatValue();

	if(args[6].boolValue())
		CCD->ActorManager()->SetActorDynamicLighting(caller->GetActor(), FillColor, AmbientColor, GE_TRUE);
	else
		CCD->ActorManager()->SetActorDynamicLighting(caller->GetActor(), FillColor, AmbientColor, GE_FALSE);

	return true;
}


SX_METHOD_IMPL(SetAlpha)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_Float)
		return false;

	CCD->ActorManager()->SetAlpha(caller->GetActor(), args[0].floatValue());
	return true;
}


SX_METHOD_IMPL(GetAlpha)
{
	float alpha;

	CCD->ActorManager()->GetAlpha(caller->GetActor(), &alpha);

	r_val = alpha;
	return true;
}


SX_METHOD_IMPL(SetScale)
{
	skRValue::RType rtypes[] = { skRValue::T_Float, skRValue::T_Float, skRValue::T_Float };

	if(!RFSX::CheckParams(1, 3, args, rtypes))
		return false;

	geVec3d scale;
	scale.X = args[0].floatValue();

	if(args.entries() == 3)
	{
		scale.Y = args[1].floatValue();
		scale.Z = args[2].floatValue();

		CCD->ActorManager()->SetScaleXYZ(caller->GetActor(), scale);
		return true;
	}

	CCD->ActorManager()->SetScale(caller->GetActor(), scale.X);
	return true;
}


SX_METHOD_IMPL(SetPosition)
{
	if(args.entries() == 1)
	{
		RFSX::sxVector *vec = RFSX::IS_VECTOR(args[0]);
		if(!vec)
			return false;

		CCD->ActorManager()->Position(caller->GetActor(), vec->VectorConstRef());
		return true;
	}

	skRValue::RType rtypes[] = { skRValue::T_Float, skRValue::T_Float, skRValue::T_Float };

	if(!RFSX::CheckParams(3, 3, args, rtypes))
		return false;

	geVec3d vec = { args[0].floatValue(),
					args[1].floatValue(),
					args[2].floatValue() };

	CCD->ActorManager()->Position(caller->GetActor(), vec);
	return true;
}


SX_METHOD_IMPL(GetPosition)
{
	r_val.assignObject(new RFSX::sxVector(caller->Position()), true);
	return true;
}


SX_METHOD_IMPL(SetRotation)
{
	if(args.entries() == 1)
	{
		RFSX::sxVector *vec = RFSX::IS_VECTOR(args[0]);
		if(!vec)
			return false;

		geVec3d_Scale(vec->Vector(), GE_PIOVER180, vec->Vector());
		CCD->ActorManager()->Rotate(caller->GetActor(), vec->VectorConstRef());
		return true;
	}

	skRValue::RType rtypes[] = { skRValue::T_Float, skRValue::T_Float, skRValue::T_Float };

	if(!RFSX::CheckParams(3, 3, args, rtypes))
		return false;

	geVec3d vec = { GE_PIOVER180 * args[0].floatValue(),
					GE_PIOVER180 * args[1].floatValue(),
					GE_PIOVER180 * args[2].floatValue() };

	CCD->ActorManager()->Rotate(caller->GetActor(), vec);
	return true;
}


SX_METHOD_IMPL(GetRotation)
{
	geVec3d vec;

	CCD->ActorManager()->GetRotate(caller->GetActor(), &vec);

	r_val.assignObject(new RFSX::sxVector(vec), true);
	return true;
}


SX_METHOD_IMPL(Move)
{
	if(args.entries() == 1)
	{
		RFSX::sxVector *vec = RFSX::IS_VECTOR(args[0]);
		if(!vec)
			return false;

		CCD->ActorManager()->AddTranslation(caller->GetActor(), vec->VectorConstRef());
		return true;
	}

	skRValue::RType rtypes[] = { skRValue::T_Float, skRValue::T_Float, skRValue::T_Float };

	if(!RFSX::CheckParams(3, 3, args, rtypes))
		return false;

	geVec3d vec = { args[0].floatValue(),
					args[1].floatValue(),
					args[2].floatValue() };

	CCD->ActorManager()->AddTranslation(caller->GetActor(), vec);
	return true;
}


SX_METHOD_IMPL(Rotate)
{
	if(args.entries() == 1)
	{
		RFSX::sxVector *vec = RFSX::IS_VECTOR(args[0]);
		if(!vec)
			return false;

		geVec3d_Scale(vec->Vector(), GE_PIOVER180, vec->Vector());
		CCD->ActorManager()->AddRotation(caller->GetActor(), vec->VectorConstRef());
		return true;
	}

	skRValue::RType rtypes[] = { skRValue::T_Float, skRValue::T_Float, skRValue::T_Float };

	if(!RFSX::CheckParams(3, 3, args, rtypes))
		return false;

	geVec3d vec = { GE_PIOVER180 * args[0].floatValue(),
					GE_PIOVER180 * args[1].floatValue(),
					GE_PIOVER180 * args[2].floatValue() };

	CCD->ActorManager()->AddRotation(caller->GetActor(), vec);
	return true;
}


SX_METHOD_IMPL(GetScreenPosition)
{
	geVec3d pos, screenPos;

	pos = caller->Position();
	geCamera_TransformAndProject(CCD->CameraManager()->Camera(), &pos, &screenPos);

	r_val.assignObject(new RFSX::sxVector2(screenPos.X, screenPos.Y), true);
	return true;
}


SX_METHOD_IMPL(GetGroundTexture)
{
	char texture[256];
	geVec3d from, to;

	CCD->ActorManager()->GetPosition(caller->GetActor(), &from);

	to = from;
	to.Y -= 1000.f;

	getSingleTextureNameByTrace(CCD->World(), &from, &to, texture);

	r_val = skString(texture);

	return true;
}


SX_METHOD_IMPL(AttachWindow)
{
	// TODO
	return true;
}


SX_METHOD_IMPL(DetachWindow)
{
	// TODO
	return true;
}


SX_METHOD_IMPL(Speak)
{
	return caller->Q_Speak(0.f, args);
}


SX_METHOD_IMPL(PlaySound)
{
	if(args.entries() < 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	if(args.entries() > 1)
		caller->PlaySound(args[0].str().c_str(), -1, GE_FALSE, args[1].floatValue());
	else
		caller->PlaySound(args[0].str().c_str(), -1, GE_FALSE);

	return true;
}


SX_METHOD_IMPL(SetIcon)
{
	if(args.entries() < 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	caller->SetIcon(args[0].str().c_str());
	return true;
}


SX_METHOD_IMPL(GetIcon)
{
	r_val = skString(caller->GetIcon().c_str());
	return true;
}


SX_METHOD_IMPL(SaveAttributes)
{
	if(args.entries() != 1)
		return false;

	if(args[0].type() != skRValue::T_String)
		return false;

	caller->SaveAttributesAscii(args[0].str().c_str());
	return true;
}


RFSX::SX_INIT_MHT(SXPLAYER_METHODS);
void SX_IMPL_TYPE::InitMHT()
{
	SX_ADD_METHOD(GetName);
	SX_ADD_METHOD(SetWeapon);
	SX_ADD_METHOD(SetWeaponMatFromFlip);
	SX_ADD_METHOD(ChangeWeaponMaterial);
	SX_ADD_METHOD(SetUseItem);
	SX_ADD_METHOD(ClearUseItem);
	SX_ADD_METHOD(HasAttribute);
	SX_ADD_METHOD(SetAttribute);
	SX_ADD_METHOD(GetAttribute);
	SX_ADD_METHOD(ModifyAttribute);
	SX_ADD_METHOD(AddAttribute);
	SX_ADD_METHOD(SetAttributeValueLimits);
	SX_ADD_METHOD(GetAttributeLowLimit);
	SX_ADD_METHOD(GetAttributeHighLimit);
	SX_ADD_METHOD(PowerUp);
	SX_ADD_METHOD(GetPowerUpLevel);
	SX_ADD_METHOD(SetMouseControlled);
	SX_ADD_METHOD(AttachBlendActor);
	SX_ADD_METHOD(DetachBlendActor);
	SX_ADD_METHOD(AttachAccessory);
	SX_ADD_METHOD(DetachAccessory);
	SX_ADD_METHOD(Render);
	SX_ADD_METHOD(MatchEntityAngles);
	SX_ADD_METHOD(ChangeMaterial);
	SX_ADD_METHOD(SetLighting);
	SX_ADD_METHOD(SetAlpha);
	SX_ADD_METHOD(GetAlpha);
	SX_ADD_METHOD(SetScale);
	SX_ADD_METHOD(SetPosition);
	SX_ADD_METHOD(GetPosition);
	SX_ADD_METHOD(SetRotation);
	SX_ADD_METHOD(GetRotation);
	SX_ADD_METHOD(Move);
	SX_ADD_METHOD(Rotate);
	SX_ADD_METHOD(GetScreenPosition);
	SX_ADD_METHOD(GetGroundTexture);
	SX_ADD_METHOD(AttachWindow);
	SX_ADD_METHOD(DetachWindow);
	SX_ADD_METHOD(Speak);
	SX_ADD_METHOD(PlaySound);
	SX_ADD_METHOD(SetIcon);
	SX_ADD_METHOD(GetIcon);
	SX_ADD_METHOD(SaveAttributes);
}

#undef SX_IMPL_TYPE

/* ----------------------------------- END OF FILE ------------------------------------ */
