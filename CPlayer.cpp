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
#include "IniFile.h"
#include "CArmour.h"
#include "CHeadsUpDisplay.h"
#include "CCDAudio.h"
#include "CMIDIAudio.h"

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

extern "C" void	DrawBoundBox(geWorld *World, const geVec3d *Pos, const geVec3d *Min, const geVec3d *Max);
extern geSound_Def *SPool_Sound(const char *SName);
extern geBitmap *TPool_Bitmap(const char *DefaultBmp, const char *DefaultAlpha,
							  const char *BName, const char *AName);

/* ------------------------------------------------------------------------------------ */
// CPlayer
//
// Default constructor
/* ------------------------------------------------------------------------------------ */
CPlayer::CPlayer()
{
	// Initialize all state variables.  Note that the player avatar is not
	// ..scaled up at all!  Why?  This means the designer can make a level
	// ..that is, effectively, _immense_ in the level editor without running
	// ..up against the G3D 1.0 limit of 4096x4096x4096 maximum size.  The
	// ..player CAN be scaled when the environment is loaded, though.

	m_Speed = m_CurrentSpeed = 3.0f;
	m_JumpSpeed				= 60.0f;
	m_JumpActive			= false;
	m_LastMovementSpeed		= 0.0f;
	m_StepHeight			= 16.0f;
	m_FirstPersonView		= true;
	m_CoeffSpeed			= 1.0f;
	m_LookMode				= false;
	m_PositionHistoryPtr	= 0;
	m_FogActive				= false;
	m_ClipActive			= false;
	m_PlayerViewPoint		= FIRSTPERSON;
	m_Gravity.X				= 0.0f;
	m_Gravity.Y				= -4.6f;
	m_Gravity.Z				= 0.0f;
	m_Wind.X				= 0.0f;
	m_Wind.Y				= 0.0f;
	m_Wind.Z				= 0.0f;
	m_InitialWind.X			= 0.0f;
	m_InitialWind.Y			= 0.0f;
	m_InitialWind.Z			= 0.0f;
	alwaysrun = m_Running = m_run = false;
	monitorstate = monitormode = false;
	deathspace				= false;
	m_Scale					= 1.0f;
	m_lastdirection			= 0;
	Alive					= true;
	firstframe				= true;

	m_oldcrouch = m_crouch	= GE_FALSE;

	m_Moving				= MOVEIDLE;
	m_SlideWalk				= MOVEIDLE;

	lighton					= true;
	lightactive				= false;
	lighteffect				= -1;
	LiteDecayTime			= 0.0f;
	LiteTime				= 0.0f;
	CurrentLiteLife			= -1.0f;
	StaminaDelay			= 1.0f;
	StaminaTime				= 0.0f;

	for(int sn=0; sn<20; sn++)
	{
		StaminaDelay1[sn] = 1.0f;
		StaminaTime1[sn] = 0.0f;
	}

	Allow3rdLook			= true;
	LastHealth				= -1;
	Injured					= false;
	Dying					= false;
	ContinueAfterDeath		= true;
	OldFalling = Falling = FallInjure = false;
	MinFallDist = MaxFallDist = FallDamage = 0.0f;

	for(int j=0;j<10;j++)
		UseAttribute[j][0] = '\0';

	LockView				= false;
	restoreoxy				= true;

	RealJumping				= false;
	LiquidTime				= 0.0f;
	InLiquid				= -1;
	SwimSound				= -1;
	SurfaceSound			= -1;
	InLiquidSound			= -1;
	slideslope				= 0.8f;
	slidespeed				= 40.0f;
	OnLadder				= false;
	m_PlayerName[0]			= '\0';
	DefaultMotion[0]		= NULL;
	DefaultMotion[1]		= NULL;
	DefaultMotion[2]		= NULL;
	szCDTrack[0]			= '\0';
	szMIDIFile[0]			= '\0';
	szStreamingAudio[0]		= '\0';

	for(int nTemp=0; nTemp<16; nTemp++)
		for(int j=0; j<3; j++)
			Contents[nTemp][j] = NULL;

	std::string pSetup = "playersetup.ini";

	if(CCD->MenuManager()->GetUseSelect())
	{
		if(!EffectC_IsStringNull(CCD->MenuManager()->GetCurrentpSetup()))
			pSetup = CCD->MenuManager()->GetCurrentpSetup();
	}

	CIniFile AttrFile(pSetup);

	if(!AttrFile.ReadFile())
	{
		CCD->ReportError("[ERROR] Failed to open playersetup initialization file\n", false);
		return;
	}

	std::string KeyName = AttrFile.FindFirstKey();
	std::string Type, Vector;
	char szName[64];

	while(KeyName != "")
	{
		if(KeyName == "Animations")
		{
			Type = AttrFile.GetValue(KeyName, "idle");
			strcpy(Animations[IDLE], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "walk");
			strcpy(Animations[WALK], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "run");
			strcpy(Animations[RUN], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "jump");
			strcpy(Animations[JUMP], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "starttojump");
			strcpy(Animations[STARTJUMP], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "fall");
			strcpy(Animations[FALL], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "land");
			strcpy(Animations[LAND], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "climbidle");
			strcpy(Animations[CLIMBIDLE], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "climbdown");
			strcpy(Animations[CLIMBDOWN], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "climbup");
			strcpy(Animations[CLIMBUP], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "slidetoleft");
			strcpy(Animations[SLIDELEFT], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "slideruntoleft");
			strcpy(Animations[RUNSLIDELEFT], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "slidetoright");
			strcpy(Animations[SLIDERIGHT], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "slideruntoright");
			strcpy(Animations[RUNSLIDERIGHT], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "crawl");
			strcpy(Animations[CRAWL], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "crouchidle");
			strcpy(Animations[CIDLE], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "crouchstarttojump");
			strcpy(Animations[CSTARTJUMP], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "crouchland");
			strcpy(Animations[CLAND], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "crawlslidetoleft");
			strcpy(Animations[SLIDECLEFT], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "crawlslidetoright");
			strcpy(Animations[SLIDECRIGHT], Type.c_str());

			Type = AttrFile.GetValue(KeyName, "shootup");
			strcpy(Animations[SHOOT1], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "shootdwn");
			strcpy(Animations[SHOOT], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "aimup");
			strcpy(Animations[AIM1], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "aimdwn");
			strcpy(Animations[AIM], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "walkshootup");
			strcpy(Animations[WALKSHOOT1], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "walkshootdwn");
			strcpy(Animations[WALKSHOOT], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "runshootup");
			strcpy(Animations[RUNSHOOT1], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "runshootdwn");
			strcpy(Animations[RUNSHOOT], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "slidetoleftshootup");
			strcpy(Animations[SLIDELEFTSHOOT1], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "slidetoleftshootdwn");
			strcpy(Animations[SLIDELEFTSHOOT], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "slideruntoleftshootup");
			strcpy(Animations[RUNSLIDELEFTSHOOT1], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "slideruntoleftshootdwn");
			strcpy(Animations[RUNSLIDELEFTSHOOT], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "slidetorightshootup");
			strcpy(Animations[SLIDERIGHTSHOOT1], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "slidetorightshootdwn");
			strcpy(Animations[SLIDERIGHTSHOOT], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "slideruntorightshootup");
			strcpy(Animations[RUNSLIDERIGHTSHOOT1], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "slideruntorightshootdwn");
			strcpy(Animations[RUNSLIDERIGHTSHOOT], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "jumpshootup");
			strcpy(Animations[JUMPSHOOT1], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "jumpshootdwn");
			strcpy(Animations[JUMPSHOOT], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "fallshootup");
			strcpy(Animations[FALLSHOOT1], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "fallshootdwn");
			strcpy(Animations[FALLSHOOT], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "crouchaimup");
			strcpy(Animations[CAIM1], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "crouchaimdwn");
			strcpy(Animations[CAIM], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "crouchshootup");
			strcpy(Animations[CSHOOT1], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "crouchshootdwn");
			strcpy(Animations[CSHOOT], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "crawlshootup");
			strcpy(Animations[CRAWLSHOOT1], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "crawlshootdwn");
			strcpy(Animations[CRAWLSHOOT], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "crawlslidetoleftshootup");
			strcpy(Animations[SLIDECLEFTSHOOT1], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "crawlslidetoleftshootdwn");
			strcpy(Animations[SLIDECLEFTSHOOT], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "crawlslidetorightshootup");
			strcpy(Animations[SLIDECRIGHTSHOOT1], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "crawlslidetorightshootdwn");
			strcpy(Animations[SLIDECRIGHTSHOOT], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "swim");
			strcpy(Animations[SWIM], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "treadwater");
			strcpy(Animations[TREADWATER], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "idletowalk");
			strcpy(Animations[I2WALK], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "idletorun");
			strcpy(Animations[I2RUN], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "walktoidle");
			strcpy(Animations[W2IDLE], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "crawltoidle");
			strcpy(Animations[C2IDLE], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "crouchtoidle");
			strcpy(Animations[CROUCH2IDLE], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "idletocrouch");
			strcpy(Animations[IDLE2CROUCH], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "swimtotread");
			strcpy(Animations[SWIM2TREAD], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "treadtoswim");
			strcpy(Animations[TREAD2SWIM], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "idletotread");
			strcpy(Animations[IDLE2TREAD], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "swimtowalk");
			strcpy(Animations[SWIM2WALK], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "walktoswim");
			strcpy(Animations[WALK2SWIM], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "treadtoidle");
			strcpy(Animations[TREAD2IDLE], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "jumptofall");
			strcpy(Animations[JUMP2FALL], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "jumptotread");
			strcpy(Animations[JUMP2TREAD], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "falltotread");
			strcpy(Animations[FALL2TREAD], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "falltocrawl");
			strcpy(Animations[FALL2CRAWL], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "falltowalk");
			strcpy(Animations[FALL2WALK], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "falltojump");
			strcpy(Animations[FALL2JUMP], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "walktojump");
			strcpy(Animations[WALK2JUMP], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "walktocrawl");
			strcpy(Animations[WALK2CRAWL], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "crawltowalk");
			strcpy(Animations[CRAWL2WALK], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "idletocrawl");
			strcpy(Animations[IDLE2CRAWL], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "aimtocrouch");
			strcpy(Animations[AIM2CROUCH], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "crouchtoaim");
			strcpy(Animations[CROUCH2AIM], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "walktotread");
			strcpy(Animations[W2TREAD], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "falltorun");
			strcpy(Animations[FALL2RUN], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "crawltorun");
			strcpy(Animations[CRAWL2RUN], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "walkback");
			strcpy(Animations[WALKBACK], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "runback");
			strcpy(Animations[RUNBACK], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "crawlback");
			strcpy(Animations[CRAWLBACK], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "walkshootupback");
			strcpy(Animations[WALKSHOOT1BACK], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "walkshootdwnback");
			strcpy(Animations[WALKSHOOTBACK], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "runshootupback");
			strcpy(Animations[RUNSHOOT1BACK], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "runshootdwnback");
			strcpy(Animations[RUNSHOOTBACK], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "crawlshootupback");
			strcpy(Animations[CRAWLSHOOT1BACK], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "crawlshootdwnback");
			strcpy(Animations[CRAWLSHOOTBACK], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "swimback");
			strcpy(Animations[SWIMBACK], Type.c_str());
			Type = AttrFile.GetValue(KeyName, "die");
			char strip[256], *temp;
			int i = 0;
			DieAnimAmt = 0;
			if(Type != "")
			{
				strcpy(strip, Type.c_str());
				temp = strtok(strip," \n");

				while(temp)
				{
					strcpy(DieAnim[i],temp);
					i += 1;
					DieAnimAmt = i;

					if(i == 5)
						break;

					temp = strtok(NULL," \n");
				}
			}
			Type = AttrFile.GetValue(KeyName, "injury");
			i = 0;
			InjuryAnimAmt = 0;
			if(Type != "")
			{
				strcpy(strip, Type.c_str());
				temp = strtok(strip," \n");

				while(temp)
				{
					strcpy(InjuryAnim[i],temp);
					i += 1;
					InjuryAnimAmt = i;

					if(i == 5)
						break;
					temp = strtok(NULL," \n");
				}
			}
			TranTime[I2WALKTIME]		= (float)AttrFile.GetValueF(KeyName, "idletowalktime");
			TranTime[I2RUNTIME]			= (float)AttrFile.GetValueF(KeyName, "idletoruntime");
			TranTime[W2IDLETIME]		= (float)AttrFile.GetValueF(KeyName, "walktoidletime");
			TranTime[C2IDLETIME]		= (float)AttrFile.GetValueF(KeyName, "crawltoidletime");
			TranTime[CROUCH2IDLETIME]	= (float)AttrFile.GetValueF(KeyName, "crouchtoidletime");
			TranTime[IDLE2CROUCHTIME]	= (float)AttrFile.GetValueF(KeyName, "idletocrouchtime");
			TranTime[SWIM2TREADTIME]	= (float)AttrFile.GetValueF(KeyName, "swimtotreadtime");
			TranTime[TREAD2SWIMTIME]	= (float)AttrFile.GetValueF(KeyName, "treadtoswimtime");
			TranTime[IDLE2TREADTIME]	= (float)AttrFile.GetValueF(KeyName, "idletotreadtime");
			TranTime[TREAD2IDLETIME]	= (float)AttrFile.GetValueF(KeyName, "treadtoidletime");
			TranTime[SWIM2WALKTIME]		= (float)AttrFile.GetValueF(KeyName, "swimtowalktime");
			TranTime[WALK2SWIMTIME]		= (float)AttrFile.GetValueF(KeyName, "walktoswimtime");
			TranTime[JUMP2FALLTIME]		= (float)AttrFile.GetValueF(KeyName, "jumptofalltime");
			TranTime[JUMP2TREADTIME]	= (float)AttrFile.GetValueF(KeyName, "jumptotreadtime");
			TranTime[FALL2TREADTIME]	= (float)AttrFile.GetValueF(KeyName, "falltotreadtime");
			TranTime[SLIDE2CROUCHTIME]	= (float)AttrFile.GetValueF(KeyName, "slidetocrouchtime");
			TranTime[SLIDE2IDLETIME]	= (float)AttrFile.GetValueF(KeyName, "slidetoidletime");
			TranTime[FALL2CRAWLTIME]	= (float)AttrFile.GetValueF(KeyName, "falltocrawltime");
			TranTime[FALL2WALKTIME]		= (float)AttrFile.GetValueF(KeyName, "falltowalktime");
			TranTime[FALL2JUMPTIME]		= (float)AttrFile.GetValueF(KeyName, "falltojumptime");
			TranTime[WALK2JUMPTIME]		= (float)AttrFile.GetValueF(KeyName, "walktojumptime");
			TranTime[WALK2CRAWLTIME]	= (float)AttrFile.GetValueF(KeyName, "walktocrawltime");
			TranTime[CRAWL2WALKTIME]	= (float)AttrFile.GetValueF(KeyName, "crawltowalktime");
			TranTime[IDLE2CRAWLTIME]	= (float)AttrFile.GetValueF(KeyName, "idletocrawltime");

		}

		if(KeyName == "Sounds")
		{
			Type = AttrFile.GetValue(KeyName, "die");
			char strip[256], *temp;
			int i = 0;
			DieSoundAmt = 0;
			strcpy(strip, Type.c_str());
			temp = strtok(strip," \n");
			while(temp)
			{
				DieSound[i] = SPool_Sound(temp);
				i += 1;
				DieSoundAmt = i;
				if(i == 5)
					break;
				temp = strtok(NULL," \n");
			}
			Type = AttrFile.GetValue(KeyName, "injury");
			i = 0;
			InjurySoundAmt = 0;
			strcpy(strip, Type.c_str());
			temp = strtok(strip," \n");
			while(temp)
			{
				InjurySound[i] = SPool_Sound(temp);
				i += 1;
				InjurySoundAmt = i;
				if(i == 5)
					break;
				temp = strtok(NULL," \n");
			}
			Type = AttrFile.GetValue(KeyName, "land");
			i = 0;
			LandSoundAmt = 0;
			strcpy(strip, Type.c_str());
			temp = strtok(strip," \n");
			while(temp)
			{
				LandSound[i] = SPool_Sound(temp);
				i+=1;
				LandSoundAmt = i;
				if(i==5)
					break;
				temp = strtok(NULL," \n");
			}
		}

		if(KeyName == "Light")
		{
			geVec3d convert;
			LiteIntensity	= (float)AttrFile.GetValueF(KeyName, "intensity");
			LiteRadiusMin	= (float)AttrFile.GetValueF(KeyName, "minimumradius");
			LiteRadiusMax	= (float)AttrFile.GetValueF(KeyName, "maximumradius");
			LiteLife		= (float)AttrFile.GetValueF(KeyName, "lifetime");
			LiteDecay		= (float)AttrFile.GetValueF(KeyName, "decaytime");
			Vector = AttrFile.GetValue(KeyName, "minimumcolor");
			if(Vector != "")
			{
				strcpy(szName, Vector.c_str());
				convert = Extract(szName);
				LiteColorMin.r = convert.X;
				LiteColorMin.g = convert.Y;
				LiteColorMin.b = convert.Z;
			}
			Vector = AttrFile.GetValue(KeyName, "maximumcolor");
			if(Vector != "")
			{
				strcpy(szName, Vector.c_str());
				convert = Extract(szName);
				LiteColorMax.r = convert.X;
				LiteColorMax.g = convert.Y;
				LiteColorMax.b = convert.Z;
			}
			Vector = AttrFile.GetValue(KeyName, "offsetangles");
			if(Vector != "")
			{
				strcpy(szName, Vector.c_str());
				LiteOffset = Extract(szName);
			}
			geVec3d_Scale(&LiteOffset, 0.0174532925199433f, &LiteOffset);
			LiteSpot = false;
			Type = AttrFile.GetValue(KeyName, "spotlight");
			if(Type == "true")
				LiteSpot = true;
			LiteArc = (float)AttrFile.GetValueF(KeyName, "arc");
			LiteStyle = AttrFile.GetValueI(KeyName, "style");
			LiteBone[0] = '\0';
			Type = AttrFile.GetValue(KeyName, "attachtobone");
			if(Type != "")
				strcpy(LiteBone, Type.c_str());
		}

		if(KeyName == "Misc")
		{
			strcpy(StaminaName,"stamina");
			Type = AttrFile.GetValue(KeyName, "attributename");
			if(Type != "")
				strcpy(StaminaName, Type.c_str());
			char szSName[64];
			for(int sn=0; sn<20; sn++)
			{
				StaminaName1[sn][0] = '\0';
				sprintf(szSName, "attributename%d",sn+1);
				Type = AttrFile.GetValue(KeyName, szSName);
				if(Type != "")
					strcpy(StaminaName1[sn], Type.c_str());
				sprintf(szSName, "recoverytime%d", sn+1);
				StaminaDelay1[sn] = (float)AttrFile.GetValueF(KeyName, szSName);
			}
			strcpy(FallDamageAttr,"health");
			Type = AttrFile.GetValue(KeyName, "falldamageattribute");
			if(Type != "")
				strcpy(FallDamageAttr, Type.c_str());
			StaminaDelay	= (float)AttrFile.GetValueF(KeyName, "recoverytime");
			MinFallDist		= (float)AttrFile.GetValueF(KeyName, "minimumfalldistance");
			MaxFallDist		= (float)AttrFile.GetValueF(KeyName, "maximumfalldistance");
			FallDamage		= (float)AttrFile.GetValueF(KeyName, "falldamage");
			UseRange = (float)AttrFile.GetValueF(KeyName, "userange");
			BoxWidth = (float)AttrFile.GetValueF(KeyName, "boxsize");
			alwaysrun = false;
			Type = AttrFile.GetValue(KeyName, "alwaysrun");
			if(Type == "true")
				alwaysrun = true;
			nocrouchjump = false;
			Type = AttrFile.GetValue(KeyName, "allowcrouchjump");
			if(Type == "false")
				nocrouchjump = true;
			Type = AttrFile.GetValue(KeyName, "restartafterdeath");
			if(Type == "false")
				ContinueAfterDeath = false;
			strcpy(JumpOnDamageAttr,"health");
			Type = AttrFile.GetValue(KeyName, "jumpondamageattribute");
			if(Type != "")
				strcpy(JumpOnDamageAttr, Type.c_str());
			MinJumpOnDist	= (float)AttrFile.GetValueF(KeyName, "minimumjumpondistance");
			MaxJumpOnDist	= (float)AttrFile.GetValueF(KeyName, "maximumjumpondistance");
			JumpOnDamage	= (float)AttrFile.GetValueF(KeyName, "jumpondamage");
			mirror = 0;
			Type = AttrFile.GetValue(KeyName, "mirror1st");
			if(Type == "true")
				mirror = GE_ACTOR_RENDER_MIRRORS;
			alterkey = false;
			Type = AttrFile.GetValue(KeyName, "alternatekey");
			if(Type == "true")
				alterkey = true;
			ChangeMaterial[0] = '\0';
			Type = AttrFile.GetValue(KeyName, "changematerial");
			if(Type != "")
				strcpy(ChangeMaterial, Type.c_str());
			geVec3d TFillColor = {0.0f, 0.0f, 0.0f};
			geVec3d TAmbientColor = {0.0f, 0.0f, 0.0f};
			Vector = AttrFile.GetValue(KeyName, "fillcolor");
			if(Vector != "")
			{
				strcpy(szName, Vector.c_str());
				TFillColor = Extract(szName);
			}
			Vector = AttrFile.GetValue(KeyName, "ambientcolor");
			if(Vector != "")
			{
				strcpy(szName, Vector.c_str());
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
			AmbientLightFromFloor = true;
			Vector = AttrFile.GetValue(KeyName, "ambientlightfromfloor");
			if(Vector == "false")
				AmbientLightFromFloor = false;
			EnvironmentMapping = false;
			Vector = AttrFile.GetValue(KeyName, "environmentmapping");
			if(Vector == "true")
			{
				EnvironmentMapping = true;
				AllMaterial = false;
				Vector = AttrFile.GetValue(KeyName, "allmaterial");
				if(Vector == "true")
					AllMaterial = true;
				PercentMapping = (float)AttrFile.GetValueF(KeyName, "percentmapping");
				PercentMaterial = (float)AttrFile.GetValueF(KeyName, "percentmaterial");
			}
		}

		KeyName = AttrFile.FindNextKey();
	}

    Actor = NULL;

}

/* ------------------------------------------------------------------------------------ */
// ~CPlayer
//
// Default destructor
/* ------------------------------------------------------------------------------------ */
CPlayer::~CPlayer()
{
	// Release all the environmental audio sounds

	if(DefaultMotion[0] != NULL)
		geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), DefaultMotion[0]);

	if(DefaultMotion[1] != NULL)
		geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), DefaultMotion[1]);

	if(DefaultMotion[2] != NULL)
		geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), DefaultMotion[2]);

	for(int nTemp=0; nTemp<16; ++nTemp)
	{
		for(int j=0; j<3; ++j)
		{
			if(Contents[nTemp][j] != NULL)
				geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), Contents[nTemp][j]);
		}
	}

	if(Actor)
	{
		geActor_Destroy(&Actor);
		Actor = NULL;
	}

}

/* ------------------------------------------------------------------------------------ */
// LoadAvatar
//
// Load the actor file (.ACT) to be used as the players avatar.
// ..In normal, first-person use this won't actually be seen except in mirrors.
/* ------------------------------------------------------------------------------------ */
int CPlayer::LoadAvatar(const char *szFile, const char *Name)
{
	if(CCD->MenuManager()->GetUseSelect())
	{
		strcpy(ActorName, CCD->MenuManager()->GetCurrentActor());
		Actor = CCD->ActorManager()->LoadActor(ActorName, NULL);

		SetPlayerName(CCD->MenuManager()->GetCurrentName());

		if(!Actor)
		{
			char szError[256];
			sprintf(szError, "[ERROR] File %s - Line %d: Missing Character Actor '%s'\n",
					__FILE__, __LINE__, ActorName); // changed QD 12/15/05
			CCD->ReportError(szError, false);
			CCD->ShutdownLevel();
			delete CCD;
			CCD = NULL;
			MessageBox(NULL, szError,"Player", MB_OK);
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
			char szError[256];
			sprintf(szError, "[ERROR] File %s - Line %d: Missing PlayerSetup\n", __FILE__, __LINE__);
			CCD->ReportError(szError, false);
			CCD->ShutdownLevel();
			delete CCD;
			CCD = NULL;
			MessageBox(NULL, szError,"Fatal Error", MB_OK);
			exit(-333);
		}

		pEntity = geEntity_EntitySetGetNextEntity(pSet, NULL);
		PlayerSetup *pSetup = (PlayerSetup*)geEntity_GetUserData(pEntity);

		if(EffectC_IsStringNull(pSetup->ActorName))
		{
			strcpy(ActorName, szFile);
			Actor = CCD->ActorManager()->LoadActor(szFile, NULL);
		}
		else
		{
			strcpy(ActorName, pSetup->ActorName);
			Actor = CCD->ActorManager()->LoadActor(pSetup->ActorName, NULL);
		}

		if(!Actor)
		{
			char szError[256];

			if(EffectC_IsStringNull(pSetup->ActorName))
				sprintf(szError, "[ERROR] File %s - Line %d: Missing Player Actor '%s'\n",
						__FILE__, __LINE__, szFile);
			else
				sprintf(szError, "[ERROR] File %s - Line %d: Missing Player Actor '%s'\n",
						__FILE__, __LINE__, pSetup->ActorName);

			CCD->ReportError(szError, false);
			CCD->ShutdownLevel();
			delete CCD;
			CCD = NULL;
			MessageBox(NULL, szError, "Player", MB_OK);
			exit(-333);
		}

		SetPlayerName(Name);
	}

	if(CCD->MenuManager()->GetUseNameSelect())
		SetPlayerName(CCD->MenuManager()->GetSelectedName());

	if(!EffectC_IsStringNull(ChangeMaterial))
		CCD->ActorManager()->ChangeMaterial(Actor, ChangeMaterial);

	if(EnvironmentMapping)
		SetEnvironmentMapping(Actor, true, AllMaterial, PercentMapping, PercentMaterial);

	CCD->CameraManager()->BindToActor(Actor);							// Bind the camera

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	SetPlayerName
/* ------------------------------------------------------------------------------------ */
void CPlayer::SetPlayerName(const char *Name)
{
	strncpy(m_PlayerName, Name, 63);
	m_PlayerName[63] = '\0';
}

/* ------------------------------------------------------------------------------------ */
// LoadConfiguration
//
// Loads the attribute configuration from the player configuration file
// ..defined in the PlayerSetup entity.
/* ------------------------------------------------------------------------------------ */
int CPlayer::LoadConfiguration()
{
	CCD->ReportError("Loading Attributes and Player Configuration from PlayerSetup.ini", false);

	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	// Load environmental audio
	CCD->ReportError("Loading Environmental Audio", false);
	LoadEnvironmentalAudio();

	CCD->ReportError("Parsing PlayerSetup Entity", false);

	pSet = geWorld_GetEntitySet(CCD->World(), "PlayerSetup");

	if(!pSet)
	{
		char szError[256];
		sprintf(szError, "[ERROR] File %s - Line %d: Missing PlayerSetup", __FILE__, __LINE__);
		CCD->ReportError(szError, false);
		CCD->ShutdownLevel();
		delete CCD;
		CCD = NULL;
		MessageBox(NULL, szError,"Fatal Error", MB_OK);
		exit(-333);
	}

	// Ok, get the setup information. There should only be one, so
	// we'll just take the first one we run into.
	pEntity = geEntity_EntitySetGetNextEntity(pSet, NULL);
	PlayerSetup *pSetup = static_cast<PlayerSetup*>(geEntity_GetUserData(pEntity));


	m_PlayerViewPoint = pSetup->LevelViewPoint;
	LockView = pSetup->LockView;
	monitormode = pSetup->MovieMode;

	if(monitormode)
		CCD->SetKeyPaused(true);

	geVec3d m_Rotation;
	float ShadowSize;
	float ShadowAlpha;
	char *ShadowBitmap;
	char *ShadowAlphamap;
	geBoolean UseProjectedShadows, UseStencilShadows;

	CCD->ReportError("Initialize Player Data", false);

	if(CCD->MenuManager()->GetUseSelect())
	{
		m_Scale = CCD->MenuManager()->GetCurrentScale();
		m_Rotation = CCD->MenuManager()->GetCurrentRotation();
		ShadowSize = CCD->MenuManager()->GetCurrentShadow();
		FillColor = CCD->MenuManager()->GetCurrentFillColor();
		AmbientColor = CCD->MenuManager()->GetCurrentAmbientColor();
		AmbientLightFromFloor = CCD->MenuManager()->GetCurrentAmbientLightFromFloor();
		CCD->ActorManager()->SetAnimationSpeed(Actor, CCD->MenuManager()->GetCurrentSpeed());
		ShadowAlpha = CCD->MenuManager()->GetCurrentShadowAlpha();
		ShadowBitmap = CCD->MenuManager()->GetCurrentShadowBitmap();
		ShadowAlphamap = CCD->MenuManager()->GetCurrentShadowAlphamap();
		UseProjectedShadows = CCD->MenuManager()->GetCurrentPShadows();
		UseStencilShadows = CCD->MenuManager()->GetCurrentSShadows();
	}
	else
	{
		m_Scale = pSetup->PlayerScaleFactor;			// Get scale factor
		m_Rotation.X = 0.0174532925199433f*pSetup->ActorRotation.X;
		m_Rotation.Y = 0.0174532925199433f*pSetup->ActorRotation.Y;
		m_Rotation.Z = 0.0174532925199433f*pSetup->ActorRotation.Z;
		ShadowSize = pSetup->ShadowSize;
		ShadowAlpha = pSetup->ShadowAlpha;
		ShadowBitmap = pSetup->ShadowBitmap;
		ShadowAlphamap = pSetup->ShadowAlphamap;
		UseProjectedShadows = pSetup->UseProjectedShadows;
		UseStencilShadows = pSetup->UseStencilShadows;
	}

	float actscale = pSetup->ActorScaleFactor;

	CCD->ActorManager()->SetAligningRotation(Actor, m_Rotation);
	CCD->ActorManager()->SetTilt(Actor, false);
	CCD->ActorManager()->SetBoxChange(Actor, false);
	CCD->ActorManager()->SetDefaultMotion(Actor, Animations[IDLE]);
	CCD->ActorManager()->SetMotion(Actor, Animations[IDLE]);
	CCD->ActorManager()->SetScale(Actor, m_Scale*actscale);
	CCD->ActorManager()->SetBoundingBox(Actor, Animations[IDLE]);
	CCD->ActorManager()->SetType(Actor, ENTITY_ACTOR);
	CCD->ActorManager()->SetActorDynamicLighting(Actor, FillColor, AmbientColor, AmbientLightFromFloor);
	CCD->ActorManager()->SetShadow(Actor, ShadowSize);

	if(pSetup->ShadowAlpha > 0.0f)
		CCD->ActorManager()->SetShadowAlpha(Actor, ShadowAlpha);

	if(!EffectC_IsStringNull(pSetup->ShadowBitmap))
		CCD->ActorManager()->SetShadowBitmap(Actor, TPool_Bitmap(ShadowBitmap, ShadowAlphamap, NULL, NULL));


	geExtBox theBox;
	CCD->ActorManager()->GetBoundingBox(Actor, &theBox);
	m_CurrentHeight = theBox.Max.Y;
	if(BoxWidth>0.0f)
		CCD->ActorManager()->SetBBox(Actor, BoxWidth, -m_CurrentHeight*2.0f, BoxWidth);

	int nFlags = 0;
	geVec3d theRotation = {0.0f, 0.0f, 0.0f};
	CCD->ActorManager()->Rotate(Actor, theRotation);
	geVec3d theTranslation = {0.0f, 0.0f, 0.0f};

	switch(m_PlayerViewPoint)
	{
	case FIRSTPERSON:
	default:
		nFlags = kCameraTrackPosition + kCameraTrackRotation;
		theTranslation.Y = m_CurrentHeight;
		if(CCD->CameraManager()->GetViewHeight() != -1.0f)
			theTranslation.Y = CCD->CameraManager()->GetViewHeight() * m_Scale;
		m_Rotation.X = m_Rotation.Z = 0.0f;
		// Set offset
		CCD->CameraManager()->SetCameraOffset(theTranslation, m_Rotation); //theRotation);
		// no projected shadows in 1st person
		CCD->ActorManager()->SetProjectedShadows(Actor, false);
		CCD->ActorManager()->SetStencilShadows(Actor, GE_FALSE);
		SwitchToFirstPerson();
		break;
	case THIRDPERSON:
		nFlags = kCameraTrackThirdPerson | kCameraTrackFree;
		/*theRotation.X*/m_Rotation.X = CCD->CameraManager()->GetPlayerAngleUp();
		theTranslation.Y = m_CurrentHeight;
		if(CCD->CameraManager()->GetPlayerHeight() != -1.0f)
			theTranslation.Y = CCD->CameraManager()->GetPlayerHeight()*m_Scale;
		theTranslation.Z = CCD->CameraManager()->GetPlayerDistance();
		// Set offset
		CCD->CameraManager()->SetCameraOffset(theTranslation, m_Rotation); //theRotation);
		CCD->ActorManager()->SetProjectedShadows(Actor, UseProjectedShadows);
		CCD->ActorManager()->SetStencilShadows(Actor, UseStencilShadows);
		Allow3rdLook = CCD->CameraManager()->GetPlayerAllowLook();
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
		CCD->ActorManager()->SetProjectedShadows(Actor, UseProjectedShadows);
		CCD->ActorManager()->SetStencilShadows(Actor, UseStencilShadows);
		SwitchToThirdPerson();
		CCD->CameraManager()->ResetCamera();
		break;
	case FIXEDCAMERA:
		if(CCD->FixedCameras()->GetNumber() == 0)
		{
			char szError[256];
			sprintf(szError, "[WARNING] File %s - Line %d: No Fixed Cameras in Level",
					__FILE__, __LINE__);
			CCD->ReportError(szError, false);
			m_PlayerViewPoint = THIRDPERSON;
			nFlags = kCameraTrackThirdPerson | kCameraTrackFree;
			theTranslation.Y = m_CurrentHeight;
			if(CCD->CameraManager()->GetPlayerHeight() != -1.0f)
				theTranslation.Y = CCD->CameraManager()->GetPlayerHeight() * m_Scale;
			theTranslation.Z = CCD->CameraManager()->GetPlayerDistance();
			// Set offset
			CCD->CameraManager()->SetCameraOffset(theTranslation, m_Rotation); //theRotation);
			Allow3rdLook = CCD->CameraManager()->GetPlayerAllowLook();
		}
		else
		{
			nFlags = kCameraTrackFixed;
			CCD->CameraManager()->Unbind();
		}
		CCD->ActorManager()->SetProjectedShadows(Actor, UseProjectedShadows);
		CCD->ActorManager()->SetStencilShadows(Actor, UseStencilShadows);

		SwitchToThirdPerson();
		CCD->CameraManager()->ResetCamera();
		break;
	}

	// Head bob setup
	m_HeadBobSpeed = pSetup->HeadBobSpeed;
	m_HeadBobLimit = pSetup->HeadBobLimit;
	HeadBobbing = pSetup->HeadBobbing;

	if(pSetup->HeadBobbing == GE_TRUE)
		CCD->CameraManager()->EnableHeadBob(true);
	else
		CCD->CameraManager()->EnableHeadBob(false);

	CCD->CameraManager()->SetTrackingFlags(nFlags);		// Set tracking flags
	CCD->ActorManager()->SetAutoStepUp(Actor, true);	// Allow stepping up
	CCD->ActorManager()->SetStepHeight(Actor, 32.0f);	// Set step height

	m_Speed = m_CurrentSpeed *= m_Scale;
	m_JumpSpeed *= m_Scale;

	if(monitormode)
	{
		CCD->ActorManager()->SetShadow(Actor, 0.0f);
		CCD->ActorManager()->SetNoCollide(Actor);
		CCD->ActorManager()->SetActorFlags(Actor, 0);
		CCD->HUD()->Deactivate();
	}

	geEntity_EntitySet *lEntitySet;
	geEntity *lEntity;

	CCD->ReportError("Parsing EnvironmentSetup Entity", false);

	lEntitySet = geWorld_GetEntitySet(CCD->World(), "EnvironmentSetup");

	if(lEntitySet != NULL)
	{
		lEntity = geEntity_EntitySetGetNextEntity(lEntitySet, NULL);

		if(lEntity)
		{
			EnvironmentSetup *theState = (EnvironmentSetup*)geEntity_GetUserData(lEntity);
			EnvSetup = theState;
			RealJumping = theState->RealJumping;

			if(theState->Gravity.X != 0.0f || theState->Gravity.Y != 0.0f || theState->Gravity.Z != 0.0f)
				m_Gravity = theState->Gravity;				// Over-ride default gravity

			if(theState->WindSpeed.X != 0.0f || theState->Gravity.Y != 0.0f || theState->Gravity.Z != 0.0f)
			{
				m_Wind = theState->WindSpeed;
				m_InitialWind = theState->WindSpeed;
			}

			if(theState->JumpSpeed != 0.0f)
				m_JumpSpeed = theState->JumpSpeed;			// Over-ride default jump speed

			if(theState->Speed != 0.0f)
				m_Speed = m_CurrentSpeed = theState->Speed;	// Over-ride default speed

			if(theState->StepHeight != 0.0f)
				m_StepHeight = theState->StepHeight;		// Over-ride default step-up height

			if(theState->AudibleRadius != 0.0f)
				CCD->SetAudibleRadius(theState->AudibleRadius);

			if(theState->SlideSlope != 0.0f)
				slideslope = theState->SlideSlope;

			if(theState->SlideSpeed != 0.0f)
				slidespeed = theState->SlideSpeed;

			float detailevel = (float)CCD->MenuManager()->GetDetail();

			if(theState->EnableDistanceFog == GE_TRUE)
			{
				float start = theState->FogStartDistLow + ((theState->FogStartDistHigh - theState->FogStartDistLow)*(detailevel/100));
				float end = theState->TotalFogDistLow + ((theState->TotalFogDistHigh - theState->TotalFogDistLow)*(detailevel/100));
				CCD->Engine()->SetFogParameters(theState->DistanceFogColor,	start, end);
				m_FogActive = true;
			}

			if(theState->UseFarClipPlane == GE_TRUE)
			{
				float dist = theState->FarClipPlaneDistLow + ((theState->FarClipPlaneDistHigh - theState->FarClipPlaneDistLow)*(detailevel/100));
				CCD->CameraManager()->SetFarClipPlane(dist);
				m_ClipActive = true;
			}

			CCD->CameraManager()->SetShakeMin(theState->MinShakeDist);

			if(CCD->MenuManager()->GetStencilShadows())
				geEngine_SetStencilShadowsEnable(CCD->Engine()->Engine(), GE_TRUE, theState->SShadowsMaxLightToUse,
					theState->SShadowsColor.r, theState->SShadowsColor.g, theState->SShadowsColor.b, theState->SShadowsAlpha);

		}
	}

	CCD->ActorManager()->SetStepHeight(Actor, m_StepHeight);
	CCD->ActorManager()->SetGravity(Actor, m_Gravity);

	char *AttrInfo = pSetup->AttributeInfoFile;

	if(CCD->MenuManager()->GetUseSelect())
	{
		if(!EffectC_IsStringNull(CCD->MenuManager()->GetCurrentAttr()))
			AttrInfo = CCD->MenuManager()->GetCurrentAttr();

		if(CCD->MenuManager()->GetCurrentMSpeed() != -1.0f)
			m_Speed = m_CurrentSpeed = CCD->MenuManager()->GetCurrentMSpeed();

		if(CCD->MenuManager()->GetCurrentJumpSpeed() != -1.0f)
			m_JumpSpeed = CCD->MenuManager()->GetCurrentJumpSpeed();

		if(CCD->MenuManager()->GetCurrentStep() != -1.0f)
		{
			m_StepHeight = CCD->MenuManager()->GetCurrentStep();
			CCD->ActorManager()->SetStepHeight(Actor, m_StepHeight);
		}

		if(CCD->MenuManager()->GetCurrentSlopeSlide() != -1.0f)
			slideslope = CCD->MenuManager()->GetCurrentSlopeSlide();

		if(CCD->MenuManager()->GetCurrentSlopeSpeed() != -1.0f)
			slidespeed = CCD->MenuManager()->GetCurrentSlopeSpeed();
	}

	CIniFile AttrFile(AttrInfo);

	if(!AttrFile.ReadFile())
	{
		char szAttrError[256];
		sprintf(szAttrError, "[WARNING] File %s - Line %d: Failed to open Attribute Info file '%s'",
				__FILE__, __LINE__, AttrInfo);
		CCD->ReportError(szAttrError, false);
		return RGF_FAILURE;
	}

	m_Attr = CCD->ActorManager()->Inventory(Actor);
	std::string KeyName = AttrFile.FindFirstKey();
	char szAttr[64];
	int InitialValue, LowValue, HighValue;

	while(KeyName != "")
	{
		strcpy(szAttr, KeyName.c_str());
		InitialValue = AttrFile.GetValueI(KeyName, "initial");
		m_Attr->AddAndSet(szAttr, InitialValue);
		LowValue = AttrFile.GetValueI(KeyName, "low");
		HighValue = AttrFile.GetValueI(KeyName, "high");
		m_Attr->SetValueLimits(szAttr, LowValue, HighValue);
		KeyName = AttrFile.FindNextKey();
	}

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
	DefaultMotion[0] = NULL;
	DefaultMotion[1] = NULL;
	DefaultMotion[2] = NULL;
	DefaultMotionHandle = NULL;

	for(int nTemp=0; nTemp<16; ++nTemp)
	{
		for(int j=0; j<3; ++j)
			Contents[nTemp][j] = NULL;

		ContentsHandles[nTemp] = NULL;
	}

	char envir[64];
	strcpy(envir, "environment.ini");

	if(CCD->MenuManager()->GetUseSelect())
	{
		if(!EffectC_IsStringNull(CCD->MenuManager()->GetCurrentEnv()))
			strcpy(envir, CCD->MenuManager()->GetCurrentEnv());
	}

	CIniFile AttrFile;
	AttrFile.SetPath(envir);

	if(AttrFile.ReadFile())
	{
		std::string KeyName = AttrFile.FindFirstKey();
		std::string Type, Value;
		char szName[64];
		int count;

		while(KeyName != "")
		{
			if(!strcmp(KeyName.c_str(), "Default"))
			{
				count = 0;
				Type = AttrFile.FindFirstName(KeyName);
				Value = AttrFile.FindFirstValue();

				while(Type != "" && count < 3)
				{
					strcpy(szName,  Type.c_str());
					DefaultMotion[count] = LoadAudioClip(szName);
					count += 1;
					Type = AttrFile.FindNextName();
					Value = AttrFile.FindNextValue();
				}
			}
			else if(!strcmp(KeyName.c_str(), "Water"))
			{
				count = 0;
				Type = AttrFile.FindFirstName(KeyName);
				Value = AttrFile.FindFirstValue();

				while(Type != "" && count < 3)
				{
					strcpy(szName, Type.c_str());
					Contents[0][count] = LoadAudioClip(szName);
					count += 1;
					Type = AttrFile.FindNextName();
					Value = AttrFile.FindNextValue();
				}
			}
			else if(!strcmp(KeyName.c_str(), "Lava"))
			{
				count = 0;
				Type = AttrFile.FindFirstName(KeyName);
				Value = AttrFile.FindFirstValue();

				while(Type != "" && count < 3)
				{
					strcpy(szName, Type.c_str());
					Contents[1][count] = LoadAudioClip(szName);
					count += 1;
					Type = AttrFile.FindNextName();
					Value = AttrFile.FindNextValue();
				}
			}
			else if(!strcmp(KeyName.c_str(), "ToxicGas"))
			{
				count = 0;
				Type = AttrFile.FindFirstName(KeyName);
				Value = AttrFile.FindFirstValue();

				while(Type != "" && count < 3)
				{
					strcpy(szName, Type.c_str());
					Contents[2][count] = LoadAudioClip(szName);
					count += 1;
					Type = AttrFile.FindNextName();
					Value = AttrFile.FindNextValue();
				}
			}
			else if(!strcmp(KeyName.c_str(), "ZeroG"))
			{
				count = 0;
				Type = AttrFile.FindFirstName(KeyName);
				Value = AttrFile.FindFirstValue();

				while(Type != "" && count < 3)
				{
					strcpy(szName, Type.c_str());
					Contents[3][count] = LoadAudioClip(szName);
					count += 1;
					Type = AttrFile.FindNextName();
					Value = AttrFile.FindNextValue();
				}
			}
			else if(!strcmp(KeyName.c_str(), "Frozen"))
			{
				count = 0;
				Type = AttrFile.FindFirstName(KeyName);
				Value = AttrFile.FindFirstValue();

				while(Type != "" && count < 3)
				{
					strcpy(szName, Type.c_str());
					Contents[4][count] = LoadAudioClip(szName);
					count += 1;
					Type = AttrFile.FindNextName();
					Value = AttrFile.FindNextValue();
				}
			}
			else if(!strcmp(KeyName.c_str(), "Sludge"))
			{
				count = 0;
				Type = AttrFile.FindFirstName(KeyName);
				Value = AttrFile.FindFirstValue();

				while(Type != "" && count < 3)
				{
					strcpy(szName, Type.c_str());
					Contents[5][count] = LoadAudioClip(szName);
					count += 1;
					Type = AttrFile.FindNextName();
					Value = AttrFile.FindNextValue();
				}
			}
			else if(!strcmp(KeyName.c_str(), "SlowMotion"))
			{
				count = 0;
				Type = AttrFile.FindFirstName(KeyName);
				Value = AttrFile.FindFirstValue();

				while(Type != "" && count < 3)
				{
					strcpy(szName, Type.c_str());
					Contents[6][count] = LoadAudioClip(szName);
					count += 1;
					Type = AttrFile.FindNextName();
					Value = AttrFile.FindNextValue();
				}
			}
			else if(!strcmp(KeyName.c_str(), "FastMotion"))
			{
				count = 0;
				Type = AttrFile.FindFirstName(KeyName);
				Value = AttrFile.FindFirstValue();

				while(Type != "" && count < 3)
				{
					strcpy(szName, Type.c_str());
					Contents[7][count] = LoadAudioClip(szName);
					count += 1;
					Type = AttrFile.FindNextName();
					Value = AttrFile.FindNextValue();
				}
			}
			else if(!strcmp(KeyName.c_str(), "Ladders"))
			{
				count = 0;
				Type = AttrFile.FindFirstName(KeyName);
				Value = AttrFile.FindFirstValue();

				while(Type != "" && count < 3)
				{
					strcpy(szName, Type.c_str());
					Contents[8][count] = LoadAudioClip(szName);
					count += 1;
					Type = AttrFile.FindNextName();
					Value = AttrFile.FindNextValue();
				}
			}
			else if(!strcmp(KeyName.c_str(), "Unclimbable"))
			{
				count = 0;
				Type = AttrFile.FindFirstName(KeyName);
				Value = AttrFile.FindFirstValue();

				while(Type != "" && count < 3)
				{
					strcpy(szName, Type.c_str());
					Contents[9][count] = LoadAudioClip(szName);
					count += 1;
					Type = AttrFile.FindNextName();
					Value = AttrFile.FindNextValue();
				}
			}

			KeyName = AttrFile.FindNextKey();
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
		char szError[256];
		sprintf(szError, "[ERROR] File %s - Line %d: Missing PlayerStart", __FILE__, __LINE__);
		CCD->ReportError(szError, false);
		CCD->ShutdownLevel();
		delete CCD;
		CCD = NULL;
		MessageBox(NULL, szError, "Fatal Error", MB_OK);
		exit(-333);
	}

	bool flag = false;

	for(lEntity=geEntity_EntitySetGetNextEntity(lEntitySet, NULL); lEntity;
		lEntity=geEntity_EntitySetGetNextEntity(lEntitySet, lEntity))
	{
		theStart = (PlayerStart*)geEntity_GetUserData(lEntity);

		if(CCD->ChangeLevel() && !EffectC_IsStringNull(CCD->StartName()))
		{
			if(!stricmp(CCD->StartName(), theStart->szEntityName))
			{
				flag = true;
				break;
			}
		}
		else if(CCD->MenuManager()->GetUseSelect())
		{
			if(!EffectC_IsStringNull(CCD->MenuManager()->GetCurrentPS()))
			{
				if(!stricmp(CCD->MenuManager()->GetCurrentPS(),theStart->szEntityName))
				{
					flag = true;
					break;
				}
			}
			else
			{
				if(EffectC_IsStringNull(theStart->szEntityName))
				{
					flag = true;
					break;
				}
			}
		}
		else
		{
			if(EffectC_IsStringNull(theStart->szEntityName))
			{
				flag = true;
				break;
			}
		}
	}

	if(!flag)
	{
		for(lEntity=geEntity_EntitySetGetNextEntity(lEntitySet, NULL); lEntity;
			lEntity=geEntity_EntitySetGetNextEntity(lEntitySet, lEntity))
		{
			theStart = (PlayerStart*)geEntity_GetUserData(lEntity);

			if(EffectC_IsStringNull(theStart->szEntityName))
			{
				flag = true;
				break;
			}
		}
	}

	if(flag)
	{
		theStart = (PlayerStart*)geEntity_GetUserData(lEntity);

		m_Translation.X = theStart->origin.X;
		m_Translation.Y = theStart->origin.Y;
		m_Translation.Z = theStart->origin.Z;

		geVec3d theRotation;
		theRotation.X = 0.0f;
		theRotation.Y = 0.0174532925199433f*(theStart->Angle.Y-90.0f);
		theRotation.Z = 0.0f;

		CCD->ActorManager()->Rotate(Actor, theRotation);

		if(m_PlayerViewPoint == FIRSTPERSON)
		{
			CCD->CameraManager()->Rotate(theRotation);
		}

		CCD->ActorManager()->Position(Actor, m_Translation);

		if(m_PlayerViewPoint == FIXEDCAMERA)
		{
			if(!CCD->FixedCameras()->GetFirstCamera())
			{
				char szError[256];
				sprintf(szError, "[ERROR] File %s - Line %d: No active Fixed Camera can see Player"
						__FILE__, __LINE__);
				CCD->ReportError(szError, false);
				CCD->ShutdownLevel();
				delete CCD;
				CCD = NULL;
				MessageBox(NULL, szError, "Fixed Camera", MB_OK);
				exit(-333);
			}
		}

		CCD->CameraManager()->TrackMotion();

		szCDTrack[0] = '\0';
		szMIDIFile[0] = '\0';
		szStreamingAudio[0] = '\0';

		if(!EffectC_IsStringNull(theStart->szCDTrack))
			strcpy(szCDTrack, theStart->szCDTrack);

		if(!EffectC_IsStringNull(theStart->szMIDIFile))
			strcpy(szMIDIFile, theStart->szMIDIFile);

		if(!EffectC_IsStringNull(theStart->szStreamingAudio))
			strcpy(szStreamingAudio, theStart->szStreamingAudio);

		bSoundtrackLoops = theStart->bSoundtrackLoops;
	}
	else
	{
		char szError[256];
		sprintf(szError, "[ERROR] File %s - Line %d: Missing PlayerStart", __FILE__, __LINE__);
		CCD->ReportError(szError, false);
		CCD->ShutdownLevel();
		delete CCD;
		CCD = NULL;
		MessageBox(NULL, szError,"Fatal Error", MB_OK);
		exit(-333);
	}

	// Fill the position history with the new location
	for(int nCtr=0; nCtr<50; nCtr++)
		m_PositionHistory[nCtr] = m_Translation;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// CheckKeyLook
//
// Adjust the various rotation angles depending on the current key rotation.
/* ------------------------------------------------------------------------------------ */
void CPlayer::CheckKeyLook(int keyrotate)
{

	geFloat TURN_SPEED = 40.0f * CCD->MenuManager()->GetMouseSen();
	geFloat UPDOWN_SPEED = 40.0f * CCD->MenuManager()->GetMouseSen();

	if(RealJumping && (CCD->ActorManager()->ForceActive(Actor, 0)
		|| CCD->ActorManager()->Falling(Actor) == GE_TRUE))
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

		if((keyrotate & 8) && Allow3rdLook) // is it to the bottom?
		{
			if(Allow3rdLook)
				CCD->CameraManager()->TiltXUp(UPDOWN_SPEED);

			CCD->ActorManager()->TiltUp(Actor, UPDOWN_SPEED);
		}
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
// CheckMouseLook
//
// Adjust the various rotation angles depending on the current position of the mouse.
/* ------------------------------------------------------------------------------------ */
void CPlayer::CheckMouseLook()
{
	POINT temppos, tempfilter;
	RECT client;
	geFloat TURN_SPEED;				// speed camera will move if turning left/right
	geFloat UPDOWN_SPEED;			// speed camera will move if looking up/down
	POINT pos;

	GetCursorPos(&temppos);			// get the mouse position in SCREEN coordinates

	if(CCD->MenuManager()->GetMouseFilter())
	{
		tempfilter = CCD->MenuManager()->GetFilter();
		CCD->MenuManager()->SetFilter(temppos);

		if(tempfilter.x == -1)
			tempfilter = temppos;

		temppos.x = (temppos.x + tempfilter.x)/2;
		temppos.y = (temppos.y + tempfilter.y)/2;
	}

	if(CCD->Engine()->FullScreen())
    {
		pos.x = CCD->Engine()->Width()/2;			// calculate the center of the screen
		pos.y = CCD->Engine()->Height()/2;			// calculate the center of the screen
		SetCursorPos(pos.x, pos.y);					// set the cursor in the center of the screen
		TURN_SPEED   = abs(temppos.x-pos.x) * CCD->MenuManager()->GetMouseSen();	// calculate the turning speed
		UPDOWN_SPEED = abs(temppos.y-pos.y) * CCD->MenuManager()->GetMouseSen();	// calculate the up/down speed
    }
	else
    {
		GetClientRect(CCD->Engine()->WindowHandle(),&client);	// get the client area of the window
		pos.x = client.right/2;									// calculate the center of the client area
		pos.y = client.bottom/2;								// calculate the center of the client area
		ClientToScreen(CCD->Engine()->WindowHandle(),&pos);		// convert to SCREEN coordinates
		SetCursorPos(pos.x,pos.y);								// put the cursor in the middle of the window
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
			if(temppos.y > pos.y)								// is it to the top?
				CCD->CameraManager()->TiltDown(UPDOWN_SPEED);
			else if (temppos.y < pos.y)							// is it to the bottom?
				CCD->CameraManager()->TiltUp(UPDOWN_SPEED);

			if(temppos.x > pos.x)								// is it to the left?
				CCD->CameraManager()->TurnLeft(TURN_SPEED);
			else if (temppos.x < pos.x)							// is it to the right?
				CCD->CameraManager()->TurnRight(TURN_SPEED);

			return;
		}

		if(m_FirstPersonView)
		{
			if(!CCD->CameraManager()->GetAllowMouse1st())
				return;

			// First person we run both the camera AND the actor
			if(temppos.y > pos.y)							// is it to the top?
			{
				if(CCD->ActorManager()->TiltDown(Actor, UPDOWN_SPEED) == RGF_SUCCESS)
					CCD->CameraManager()->TiltDown(UPDOWN_SPEED);
			}
			else if(temppos.y < pos.y)						// is it to the bottom?
			{
				if(CCD->ActorManager()->TiltUp(Actor, UPDOWN_SPEED) == RGF_SUCCESS)
					CCD->CameraManager()->TiltUp(UPDOWN_SPEED);
			}
			if(temppos.x > pos.x)							// is it to the left?
			{
				if(CCD->ActorManager()->TurnLeft(Actor, TURN_SPEED) == RGF_SUCCESS)
					CCD->CameraManager()->TurnLeft(TURN_SPEED);
			}
			else if(temppos.x < pos.x)						// is it to the right?
			{
				if(CCD->ActorManager()->TurnRight(Actor, TURN_SPEED) == RGF_SUCCESS)
					CCD->CameraManager()->TurnRight(TURN_SPEED);
			}
		}
		else	// Third-person mode
		{
			if(m_PlayerViewPoint == THIRDPERSON)
			{
				if(!CCD->CameraManager()->GetAllowMouse3rd())
					return;

				if(CCD->CameraManager()->GetPlayerMouseRotation())
				{
					if(temppos.x > pos.x)				// is it to the left?
						CCD->CameraManager()->CameraRotY(true, TURN_SPEED*57.0f);
					else if (temppos.x < pos.x)			// is it to the right?
						CCD->CameraManager()->CameraRotY(false, TURN_SPEED*57.0f);

					if(temppos.y < pos.y)				// is it to the top?
						CCD->CameraManager()->ChangeDistance(false, UPDOWN_SPEED*57.0f);
					else if (temppos.y > pos.y)			// is it to the bottom?
						CCD->CameraManager()->ChangeDistance(true, UPDOWN_SPEED*57.0f);
				}
				else
				{
					// In third-person the camera handles all the hard work for us.
					if(temppos.x > pos.x)				// is it to the left?
						CCD->ActorManager()->TurnLeft(Actor, TURN_SPEED);
					else if (temppos.x < pos.x)			// is it to the right?
						CCD->ActorManager()->TurnRight(Actor, TURN_SPEED);

					if(temppos.y < pos.y)				// is it to the top?
					{
						if(Allow3rdLook)
							CCD->CameraManager()->TiltXDown(UPDOWN_SPEED);

						CCD->ActorManager()->TiltDown(Actor, UPDOWN_SPEED);
					}
					else if (temppos.y > pos.y)			// is it to the bottom?
					{
						if(Allow3rdLook)
							CCD->CameraManager()->TiltXUp(UPDOWN_SPEED);

						CCD->ActorManager()->TiltUp(Actor, UPDOWN_SPEED);
					}
				}
			}
			else if(m_PlayerViewPoint == ISOMETRIC)
			{
				if(!CCD->CameraManager()->GetAllowMouseIso())
					return;

				// In third-person the camera handles all the hard work for us.
				if(temppos.x > pos.x)					// is it to the left?
					CCD->ActorManager()->TurnLeft(Actor, TURN_SPEED);
				else if (temppos.x < pos.x)				// is it to the right?
					CCD->ActorManager()->TurnRight(Actor, TURN_SPEED);

				if(temppos.y < pos.y)					// is it to the top?
				{
					CCD->CameraManager()->TiltXDown(UPDOWN_SPEED);
					CCD->ActorManager()->TiltDown(Actor, UPDOWN_SPEED);
				}
				else if (temppos.y > pos.y)				// is it to the bottom?
				{
					CCD->CameraManager()->TiltXUp(UPDOWN_SPEED);
					CCD->ActorManager()->TiltUp(Actor, UPDOWN_SPEED);
				}
			}
			else if(m_PlayerViewPoint == FIXEDCAMERA)
			{
				// In third-person the camera handles all the hard work for us.
				if(temppos.x > pos.x)					// is it to the left?
					CCD->ActorManager()->TurnLeft(Actor, TURN_SPEED);
				else if (temppos.x < pos.x)				// is it to the right?
					CCD->ActorManager()->TurnRight(Actor, TURN_SPEED);

				if(temppos.y < pos.y)					// is it to the top?
					CCD->ActorManager()->TiltDown(Actor, UPDOWN_SPEED);
				else if (temppos.y > pos.y)				// is it to the bottom?
					CCD->ActorManager()->TiltUp(Actor, UPDOWN_SPEED);
			}
		}
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
// ProcessMove
//
// Check the contents of the current area to see if we need to play Something Special.
/* ------------------------------------------------------------------------------------ */
int CPlayer::ProcessMove(bool bPlayerMoved)
{
	// head bob control
	static bool BobUp = true;

	float nBobValue = CCD->CameraManager()->GetHeadBobOffset();

	if(!bPlayerMoved || CCD->ActorManager()->ForceActive(Actor, 0) || CCD->ActorManager()->Falling(Actor))
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

	CCD->ActorManager()->GetActorZone(Actor, &ZoneType);
	CCD->ActorManager()->GetActorOldZone(Actor, &OldZone);

	if(OldZone == 1 && bPlayerMoved)
	{
		if(SurfaceSound == -1)
		{
			Liquid *LQ = CCD->ActorManager()->GetLiquid(Actor);

			if(!EffectC_IsStringNull(LQ->SurfaceSound))
			{
				Snd Sound;
				geVec3d Origin = Position();
				memset(&Sound, 0, sizeof(Sound));
				geVec3d_Copy(&Origin, &(Sound.Pos));
				Sound.Min = CCD->GetAudibleRadius();
				Sound.Loop = GE_TRUE;
				Sound.SoundDef = SPool_Sound(LQ->SurfaceSound);

				if(Sound.SoundDef)
					SurfaceSound = CCD->EffectManager()->Item_Add(EFF_SND, (void*)&Sound);
			}
		}

		liq = true;
	}
	else
	{
		if(SurfaceSound != -1)
		{
			CCD->EffectManager()->Item_Delete(EFF_SND, SurfaceSound);
			SurfaceSound = -1;
		}
	}

	if(OldZone == 2 && bPlayerMoved)
	{
		if(SwimSound == -1)
		{
			Liquid *LQ = CCD->ActorManager()->GetLiquid(Actor);

			if(!EffectC_IsStringNull(LQ->SwimSound))
			{
				Snd Sound;
				geVec3d Origin = Position();
				memset(&Sound, 0, sizeof(Sound));
				geVec3d_Copy(&Origin, &(Sound.Pos));
				Sound.Min = CCD->GetAudibleRadius();
				Sound.Loop = GE_TRUE;
				Sound.SoundDef = SPool_Sound(LQ->SwimSound);

				if(Sound.SoundDef)
					SwimSound = CCD->EffectManager()->Item_Add(EFF_SND, (void*)&Sound);
			}
		}

		liq = true;
	}
	else
	{
		if(SwimSound != -1)
		{
			CCD->EffectManager()->Item_Delete(EFF_SND, SwimSound);
			SwimSound = -1;
		}
	}

	if(OldZone == 0 && (ZoneType & kInLiquidZone) && bPlayerMoved)
	{
		if(InLiquidSound == -1)
		{
			Liquid *LQ = CCD->ActorManager()->GetLiquid(Actor);

			if(!EffectC_IsStringNull(LQ->InLiquidSound))
			{
				Snd Sound;
				geVec3d Origin = Position();
				memset(&Sound, 0, sizeof(Sound));
				geVec3d_Copy(&Origin, &(Sound.Pos));
				Sound.Min = CCD->GetAudibleRadius();
				Sound.Loop = GE_TRUE;
				Sound.SoundDef = SPool_Sound(LQ->InLiquidSound);

				if(Sound.SoundDef)
					InLiquidSound = CCD->EffectManager()->Item_Add(EFF_SND, (void*)&Sound);
			}
		}

		liq = true;
	}
	else
	{
		if(InLiquidSound != -1)
		{
			CCD->EffectManager()->Item_Delete(EFF_SND, InLiquidSound);
			InLiquidSound = -1;
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

	if(!(CCD->ActorManager()->ForceActive(Actor, 0) || CCD->ActorManager()->ReallyFall(Actor)))
	{
		if(ConType == -1)
		{
			if(geSound_SoundIsPlaying(CCD->Engine()->AudioSystem(),
				DefaultMotionHandle) != GE_TRUE && bPlayerMoved)
			{
				if(DefaultMotion[0])
				{
					int i = rand()%3;
					while(DefaultMotion[i] == NULL)
					{
						i = rand()%3;
					}

					DefaultMotionHandle = geSound_PlaySoundDef(
						CCD->Engine()->AudioSystem(), DefaultMotion[i],
						1.0f, 0.0f, 1.0f, GE_FALSE);
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

					while(Contents[ConType][i] == NULL)
					{
						i = rand()%3;
					}

					ContentsHandles[ConType] = geSound_PlaySoundDef(
						CCD->Engine()->AudioSystem(), Contents[ConType][i],
						1.0f, 0.0f, 1.0f, GE_FALSE);
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
		if(ConType == -1)
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

/* ------------------------------------------------------------------------------------ */
// Move
//
// Move the player forward/backward/left/right at a specific speed.
/* ------------------------------------------------------------------------------------ */
int CPlayer::Move(int nHow, geFloat fSpeed)
{
	if(m_LookMode)
		return RGF_SUCCESS;

	if(Injured)
		return RGF_SUCCESS;

	if(CCD->ActorManager()->CheckTransitionMotion(Actor, ANIMSWIM))
		return RGF_SUCCESS;

	m_LastMovementSpeed = fSpeed;
	m_lastdirection = nHow;


	if(RealJumping && CCD->ActorManager()->ForceActive(Actor, 0))
		return RGF_SUCCESS;

	if(RealJumping && CCD->ActorManager()->Falling(Actor) == GE_TRUE)
	{
		geVec3d theDir;

		if(m_LastMovementSpeed != 0.0f && !CCD->ActorManager()->ForceActive(Actor, 1))
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

			CCD->ActorManager()->SetForce(Actor, 1, theDir, m_JumpSpeed, m_JumpSpeed*0.5f);// /2.0f);
		}

		return RGF_SUCCESS;
	}

	if(fSpeed == 0.0f)
		return RGF_SUCCESS;

	m_run = (m_Running || alwaysrun);

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

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// StartJump
//
// Initiates a jump of the player.
/* ------------------------------------------------------------------------------------ */
void CPlayer::StartJump()
{
	int theZone;

	if(m_LookMode || m_JumpActive)
		return;

	CCD->ActorManager()->GetActorZone(Actor, &theZone);

	// You shouldn't be able to jump in certain
	// ..environments, like water, lava, or zero-G.
	int OldZone;
	CCD->ActorManager()->GetActorOldZone(Actor, &OldZone);

	if(((OldZone>0) && CCD->ActorManager()->GetMoving(Actor)) || (theZone & kLavaZone)
		|| (theZone & kNoGravityZone) || (theZone & kUnclimbableZone)
		|| (m_crouch && nocrouchjump))
		return;					// Sorry, no jumping allowed

	m_JumpActive = true;
	return;
}

/* ------------------------------------------------------------------------------------ */
// SetJump
/* ------------------------------------------------------------------------------------ */
void CPlayer::SetJump()
{
	// Jumping removes us as a passenger from any vehicle we're on.
	geActor *theVehicle = CCD->ActorManager()->GetVehicle(Actor);

	if(theVehicle != NULL)
		CCD->ActorManager()->RemovePassenger(Actor);

	if(CCD->ActorManager()->Falling(Actor) == GE_TRUE)
		return;						// No jumping in mid-air, heh heh heh

	int theZone;
	CCD->ActorManager()->GetActorZone(Actor, &theZone);

	int OldZone;
	CCD->ActorManager()->GetActorOldZone(Actor, &OldZone);

	if(((OldZone>0) && CCD->ActorManager()->GetMoving(Actor)) || (theZone & kLavaZone)
		|| (theZone & kNoGravityZone) || (theZone & kUnclimbableZone)
		|| (m_crouch && nocrouchjump))
		return;

	// Add a force to the player actor, aiming upwards, of our jump speed,
	// ..decaying 10 units over time.
	geVec3d theUp, theDir;
	CCD->ActorManager()->UpVector(Actor, &theUp);

	if(RealJumping && m_LastMovementSpeed != 0.0f)
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

	if(!FIRSTPERSON && OnLadder)
	{
		// jump backwards if on ladder
		geVec3d Back;
		CCD->ActorManager()->InVector(Actor, &Back);
		geVec3d_Inverse(&Back);
		CCD->ActorManager()->SetForce(Actor, 2, Back, m_JumpSpeed*0.3f, m_JumpSpeed*0.2f);
	}
	else
	{
		CCD->ActorManager()->SetForce(Actor, 0, theUp, m_JumpSpeed, m_JumpSpeed);
	}
}

/* ------------------------------------------------------------------------------------ */
// Position
/* ------------------------------------------------------------------------------------ */
geVec3d CPlayer::Position()
{
	CCD->ActorManager()->GetPosition(Actor, &m_Translation);
	return m_Translation;
}

/* ------------------------------------------------------------------------------------ */
// SwitchCamera
/* ------------------------------------------------------------------------------------ */
void CPlayer::SwitchCamera(int mode)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	if(LockView)
		return;

	int FixedView = CCD->Weapons()->GetFixedView();

	if(FixedView != -1 && !monitormode)
	{
		if(mode != FixedView)
			return;
	}

	pSet = geWorld_GetEntitySet(CCD->World(), "PlayerSetup");
	pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL);

	int nFlags = 0;
	PlayerSetup *pSetup = static_cast<PlayerSetup*>(geEntity_GetUserData(pEntity));

	// Mode
	geVec3d theRotation, theAlignRotation;
	CCD->ActorManager()->GetAligningRotation(Actor, &theAlignRotation);
	CCD->ActorManager()->GetRotate(Actor, &theRotation);
	CCD->CameraManager()->Rotate(theRotation);
	theAlignRotation.X = 0.0f;
	theAlignRotation.Z = 0.0f;
	geVec3d theTranslation = {0.0f, 0.0f, 0.0f};

	switch(mode)
	{
	default:
		mode = FIRSTPERSON;
	case FIRSTPERSON:
		CCD->CameraManager()->BindToActor(Actor);
		nFlags = kCameraTrackPosition + kCameraTrackRotation;

		theTranslation.Y = m_CurrentHeight;
		if(CCD->CameraManager()->GetViewHeight()!=-1.0f)
			theTranslation.Y = CCD->CameraManager()->GetViewHeight()*m_Scale;

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

		CCD->Weapons()->SetView(0);
		break;
	case THIRDPERSON:
		CCD->CameraManager()->BindToActor(Actor);
		nFlags = kCameraTrackThirdPerson | kCameraTrackFree;
		theAlignRotation.X = CCD->CameraManager()->GetPlayerAngleUp();

		theTranslation.Y = m_CurrentHeight;
		if(CCD->CameraManager()->GetPlayerHeight() != -1.0f)
			theTranslation.Y = CCD->CameraManager()->GetPlayerHeight()*m_Scale;

		theTranslation.Z = CCD->CameraManager()->GetPlayerDistance();
		CCD->CameraManager()->SetCameraOffset(theTranslation, theAlignRotation);
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
		if(CCD->CameraManager()->GetIsoHeight() != -1.0f)
			theTranslation.Y = CCD->CameraManager()->GetIsoHeight()*m_Scale;

		theTranslation.Z = CCD->CameraManager()->GetIsoDistance();
		CCD->CameraManager()->SetCameraOffset(theTranslation, theRotation);
		SwitchToThirdPerson();
		CCD->CameraManager()->ResetCamera();
		CCD->Weapons()->SetView(1);
		break;
	case FIXEDCAMERA:
		if(CCD->FixedCameras()->GetNumber() == 0)
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

/* ------------------------------------------------------------------------------------ */
// ModifyLight
/* ------------------------------------------------------------------------------------ */
void CPlayer::ModifyLight(int amount)
{
	if(DecayLite)
		CurrentLiteLife = (float)amount;
	else
		CurrentLiteLife += (float)amount;

	if(CurrentLiteLife>LiteLife)
		CurrentLiteLife = LiteLife;

	DecayLite = false;
}

/* ------------------------------------------------------------------------------------ */
// GetDieAnim
/* ------------------------------------------------------------------------------------ */
char *CPlayer::GetDieAnim()
{
	char *DA = CCD->Weapons()->DieAnim();

	if(DA)
		return DA;

	return DieAnim[rand()%DieAnimAmt];
}

/* ------------------------------------------------------------------------------------ */
// GetInjuryAnim
/* ------------------------------------------------------------------------------------ */
char *CPlayer::GetInjuryAnim()
{
	char *IA = CCD->Weapons()->InjuryAnim();

	if(IA)
		return IA;

	return InjuryAnim[rand()%InjuryAnimAmt];
}

/* ------------------------------------------------------------------------------------ */
// Tick
//
// Process actor animation changes
/* ------------------------------------------------------------------------------------ */
void CPlayer::Tick(geFloat dwTicks)
{
	if(monitormode)
	{
		CCD->HUD()->Deactivate();
		if(CCD->Input()->GetKeyboardInputNoWait() == KEY_ESC)
		{
			monitorstate = true;
		}
	}

	if(firstframe)
	{
		if(!EffectC_IsStringNull(szCDTrack))
		{
			// Start CD player running, if we have one
			if(CCD->CDPlayer())
			{
				CCD->CDPlayer()->SetCdOn(true);
				CCD->CDPlayer()->Play(atoi(szCDTrack), bSoundtrackLoops);
			}
		}

		if(!EffectC_IsStringNull(szMIDIFile))
		{
			// Start MIDI file playing
			if(CCD->MIDIPlayer())
				CCD->MIDIPlayer()->Play(szMIDIFile, bSoundtrackLoops);
		}

		if(!EffectC_IsStringNull(szStreamingAudio))
		{
			// Start streaming audio file playing
			if(CCD->AudioStreams())
				CCD->AudioStreams()->Play(szStreamingAudio, bSoundtrackLoops, true);
		}

		firstframe = false;
	}

	AddPosition();

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
			if(CCD->Input()->GetKeyboardInputNoWait() == KEY_ESC)
			{
				if(!ContinueAfterDeath)
					monitorstate = true;
				else
				{
					Alive = false;
					Dying = false;
				}

				while(CCD->Input()->GetKeyboardInputNoWait() == KEY_ESC)
				{
				}
			}
			else if(CCD->Input()->GetKeyboardInputNoWait() == KEY_SPACE)
			{
				deathspace = true;

				while(CCD->Input()->GetKeyboardInputNoWait() == KEY_SPACE)
				{
				}
			}
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
	if(theInv->Value("health") <= 0)
	{
		bool Switch = true;
		char *DA = CCD->Weapons()->DieAnim();

		if(!DA)
		{
			if(DieAnimAmt == 0)
				Switch = false;
		}

		if(!CCD->CameraManager()->GetSwitchAllowed() || !CCD->CameraManager()->GetSwitch3rd())
			Switch = false;

		if(m_PlayerViewPoint == FIRSTPERSON && Switch)
			SwitchCamera(THIRDPERSON);

		if(!Switch)
		{
			Alive = false;
		}
		else
		{
			if(DieSoundAmt > 0)
			{
				Snd Sound;
				memset(&Sound, 0, sizeof(Sound));
				CCD->ActorManager()->GetPosition(Actor, &(Sound.Pos));
				Sound.Min = CCD->GetAudibleRadius();
				Sound.Loop = GE_FALSE;
				Sound.SoundDef = DieSound[rand()%DieSoundAmt];
				CCD->EffectManager()->Item_Add(EFF_SND, (void*)&Sound);
			}

			Dying = true;
			CCD->ActorManager()->SetNoCollide(Actor);
			CCD->ActorManager()->SetMotion(Actor, ANIMDIE);
			CCD->ActorManager()->SetHoldAtEnd(Actor, true);
		}

		return;
	}

	// check health for injury
	if(LastHealth == -1)
	{
		LastHealth = theInv->Value("health");
	}
	else
	{
		if((LastHealth > theInv->Value("health") && !Injured)
			&& (!m_JumpActive && CCD->ActorManager()->Falling(Actor) != GE_TRUE)
			&& !CCD->ChangeAttributes()->GetActive())
		{
			if(InjurySoundAmt > 0 && OldZone == 0)
			{
				Snd Sound;
				memset(&Sound, 0, sizeof(Sound));
				CCD->ActorManager()->GetPosition(Actor, &(Sound.Pos));
				Sound.Min = CCD->GetAudibleRadius();
				Sound.Loop = GE_FALSE;
				Sound.SoundDef = InjurySound[rand()%InjurySoundAmt];
				CCD->EffectManager()->Item_Add(EFF_SND, (void*)&Sound);
			}

			if(m_PlayerViewPoint != FIRSTPERSON && !FallInjure)
			{
				if(OldZone == 0)
				{
					bool Switch = true;
					char *IA = CCD->Weapons()->InjuryAnim();

					if(!IA)
					{
						if(InjuryAnimAmt == 0)
							Switch = false;
					}

					if(Switch && (rand()%10) > 7)
					{
						strcpy(LastMotion, CCD->ActorManager()->GetMotion(Actor));
						CCD->ActorManager()->SetMotion(Actor, ANIMINJURY);
						CCD->ActorManager()->SetHoldAtEnd(Actor, true);
						Injured = true;
					}
				}
			}

			FallInjure = false;
		}

		LastHealth = theInv->Value("health");
	}

	// check for fall damage
	if(!Falling && OldFalling)
	{
		CCD->ActorManager()->GetPosition(Actor, &FallEnd);
		distance = (float)fabs(FallEnd.Y-FallStart.Y);

		if(Zone != kLiquidZone)
		{
			if(LandSoundAmt > 0 && distance > MinFallDist)
			{
				Snd Sound;
				memset(&Sound, 0, sizeof(Sound));
				CCD->ActorManager()->GetPosition(Actor, &(Sound.Pos));
				Sound.Min = CCD->GetAudibleRadius();
				Sound.Loop = GE_FALSE;
				Sound.SoundDef = LandSound[rand()%LandSoundAmt];
				CCD->EffectManager()->Item_Add(EFF_SND, static_cast<void*>(&Sound));
			}

			if(FallDamage > 0.0f && distance >= MinFallDist)
			{
				if(distance > MaxFallDist)
					distance = MaxFallDist;

				float damage = ((distance-MinFallDist)/(MaxFallDist-MinFallDist))*FallDamage;
				CCD->Damage()->DamageActor(Actor, damage, FallDamageAttr, damage, FallDamageAttr, "Fall");
				FallInjure = true;
			}

			if(JumpOnDamage > 0.0f && distance >= MinJumpOnDist)
			{
				if(distance > MaxJumpOnDist)
					distance = MaxJumpOnDist;

				float damage = ((distance-MinJumpOnDist)/(MaxJumpOnDist-MinJumpOnDist))*JumpOnDamage;
				GE_Collision Collision;
				CCD->ActorManager()->GetGravityCollision(Actor, &Collision);

				if(Collision.Actor)
					CCD->Damage()->DamageActor(Collision.Actor, damage, JumpOnDamageAttr, damage, JumpOnDamageAttr, "JumpOn");

				if(Collision.Model)
					CCD->Damage()->DamageModel(Collision.Model, damage, JumpOnDamageAttr, damage, JumpOnDamageAttr);
			}
		}
	}

	OldFalling = Falling;

	// check auto increase attribute
	if(!strcmp(StaminaName, "LightValue"))
	{

		StaminaTime += dwTicks*0.001f;

		if(StaminaTime >= StaminaDelay)
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
			if(StaminaTime >= StaminaDelay)
			{
				StaminaTime = 0.0f;
				theInv->Modify(StaminaName, 1);
			}
		}
	}

	for(int sn=0; sn<20; ++sn)
	{
		if(!strcmp(StaminaName1[sn], "LightValue"))
		{

			StaminaTime1[sn] += dwTicks*0.001f;

			if(StaminaTime1[sn] >= StaminaDelay1[sn])
			{
				StaminaTime1[sn] = 0.0f;
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
			if(theInv->Has(StaminaName1[sn]))
			{
				StaminaTime1[sn] += dwTicks*0.001f;
				if(StaminaTime1[sn] >= StaminaDelay1[sn])
				{
					StaminaTime1[sn] = 0.0f;
					theInv->Modify(StaminaName1[sn], 1);
				}
			}
		}
	}

	// restore oxygen if out of liquid
	if(theInv->Has("oxygen") && restoreoxy)
	{
		if(OldZone < 2)
			theInv->Modify("oxygen", theInv->High("oxygen"));
	}
	restoreoxy = true;

	// check for damage in liquid
	if(OldZone > 0 || Zone & kLiquidZone || Zone & kInLiquidZone)
	{
		Liquid *LQ = CCD->ActorManager()->GetLiquid(Actor);

		if(LQ)
		{
			if(LiquidTime == 0.0f || InLiquid == -1)
			{
				if(LQ->DamageIn || OldZone > 1)
				{
					if(!EffectC_IsStringNull(LQ->DamageAttr))
					{
						if(theInv->Has(LQ->DamageAttr) && theInv->Value(LQ->DamageAttr) > theInv->Low(LQ->DamageAttr))
						{
							InLiquid = 1;
							theInv->Modify(LQ->DamageAttr, -LQ->DamageAmt);
						}
					}

					if(InLiquid == -1)
					{
						if(!EffectC_IsStringNull(LQ->DamageAltAttr))
						{
							if(theInv->Has(LQ->DamageAltAttr) && theInv->Value(LQ->DamageAltAttr) > theInv->Low(LQ->DamageAltAttr))
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

			if(InLiquid > 0)
			{
				if(InLiquid == 1)
				{
					if(LiquidTime >= LQ->DamageDelay)
					{
						LiquidTime = 0.0f;
						InLiquid = -1;
					}
				}
				else
				{
					if(LiquidTime >= LQ->DamageAltDelay)
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

	// TODO: rotate player to face the ladder
	if(Zone & kClimbLaddersZone)
		OnLadder = true;
	else
		OnLadder = false;

	if(theInv->Has("light"))
	{
		if(theInv->Value("light") != 0 && lighton)
		{
			if(lightactive == false)
			{
				Glow Gl;
				geExtBox theBox;
				geXForm3d thePosition;
				geVec3d Pos = Position();
				CCD->ActorManager()->GetBoundingBox(Actor, &theBox);
				Pos.Y += theBox.Max.Y;

				if(LiteBone[0] != '\0')
				{
					if(geActor_GetBoneTransform(Actor, LiteBone, &(thePosition)))
						Pos = thePosition.Translation;
				}
				else
					geActor_GetBoneTransform(Actor, RootBoneName(Actor), &thePosition);

				memset(&Gl, 0, sizeof(Gl));
				geVec3d_Copy(&Pos, &(Gl.Pos));
				Gl.Spot = LiteSpot;
				Gl.Arc = LiteArc;
				Gl.Style = LiteStyle;

				geXForm3d_RotateY(&thePosition, LiteOffset.Y);
				geXForm3d_GetEulerAngles(&thePosition, &Gl.Direction);
				Gl.Direction.Z += LiteOffset.Z;
				geVec3d_Scale(&(Gl.Direction), GE_180OVERPI, &(Gl.Direction));
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
				lightactive = true;
				Gl.Intensity = LiteIntensity;

				if(CurrentLiteLife == -1.0f)
				{
					CurrentLiteLife = LiteLife;
					DecayLite = false;
				}
				else if(DecayLite)
					Gl.Intensity = LiteIntensity*(CurrentLiteDecay/LiteDecay);

				lighteffect = CCD->EffectManager()->Item_Add(EFF_LIGHT, (void*)&Gl);
			}
		}
		else
		{
			if(lightactive)
			{
				if(lighteffect!=-1)
					CCD->EffectManager()->Item_Delete(EFF_LIGHT, lighteffect);
				lighteffect = -1;
				lightactive = false;
			}
		}
	}

	if(lightactive && lighteffect != -1)
	{
		Glow Gl;
		geExtBox theBox;
		geXForm3d thePosition;
		geVec3d Pos = Position();
		CCD->ActorManager()->GetBoundingBox(Actor, &theBox);
		Pos.Y += theBox.Max.Y;

		if(LiteBone[0] != '\0')
		{
			if(geActor_GetBoneTransform(Actor, LiteBone, &thePosition))
				Pos = thePosition.Translation;
		}
		else
		{
			geActor_GetBoneTransform(Actor, RootBoneName(Actor), &thePosition);
		}

		geVec3d_Copy(&Pos, &(Gl.Pos));
		geXForm3d_RotateY(&thePosition, LiteOffset.Y);
		geXForm3d_GetEulerAngles(&thePosition, &(Gl.Direction));
		// move the light up/down when looking up/down
		Gl.Direction.Z += LiteOffset.Z;
		geVec3d_Scale(&(Gl.Direction), 57.3f, &(Gl.Direction));

		if(!DecayLite)
		{
			CurrentLiteLife -= dwTicks*0.001f;

			if(CurrentLiteLife <= 0.0f)
			{
				DecayLite = true;
				CurrentLiteDecay = LiteDecay;
			}

			CCD->EffectManager()->Item_Modify(EFF_LIGHT, lighteffect, (void*)&Gl, GLOW_POS);
		}
		else
		{
			CurrentLiteDecay -= dwTicks*0.001f;

			if(CurrentLiteDecay <= 0.0f)
			{
				CCD->EffectManager()->Item_Delete(EFF_LIGHT, lighteffect);
				lighteffect = -1;
				lightactive = false;
				CurrentLiteLife = -1.0f;
				lighton = false;
				theInv->Modify("light", -1);
			}
			else
			{
				Gl.Intensity = LiteIntensity*(CurrentLiteDecay/LiteDecay);
				CCD->EffectManager()->Item_Modify(EFF_LIGHT, lighteffect, (void*)&Gl, GLOW_POS | GLOW_INTENSITY);
			}
		}
	}

#define CLIMBING (!strcmp(Motion, ANIMCLIMBUP) || !strcmp(Motion, ANIMCLIMBDOWN))
#define JUMPING (!strcmp(Motion, ANIMJUMP) || !strcmp(Motion, ANIMJUMPSHOOT))
#define FALLING (!strcmp(Motion, ANIMFALL) || !strcmp(Motion, ANIMFALLSHOOT))
#define STARTLAND (!strcmp(Motion, ANIMSTARTJUMP) || !strcmp(Motion, ANIMLAND))
#define CSTARTLAND (!strcmp(Motion, ANIMCSTARTJUMP) || !strcmp(Motion, ANIMCLAND))
#define TREADING (!strcmp(Motion, ANIMTREADWATER))
#define SWIMING (!strcmp(Motion, ANIMSWIM) || !strcmp(Motion, ANIMSWIMBACK))
#define IDLING (!strcmp(Motion, ANIMIDLE) || !strcmp(Motion, ANIMAIM))
#define CIDLING (!strcmp(Motion, ANIMCIDLE) || !strcmp(Motion, ANIMCAIM))
#define TRANS1 (!strcmp(Motion, ANIMW2IDLE) || !strcmp(Motion, ANIMC2IDLE) || !strcmp(Motion, ANIMCROUCH2IDLE) || !strcmp(Motion, ANIMIDLE2CROUCH) || !strcmp(Motion, ANIMSWIM2TREAD) || !strcmp(Motion, ANIMIDLE2TREAD) || !strcmp(Motion, ANIMTREAD2IDLE))
#define TRANS2 (!strcmp(Motion, ANIMI2WALK) || !strcmp(Motion, ANIMI2RUN) || !strcmp(Motion, ANIMTREAD2SWIM) || !strcmp(Motion, ANIMSWIM2WALK) || !strcmp(Motion, ANIMWALK2SWIM) || !strcmp(Motion, ANIMJUMP2FALL) || !strcmp(Motion, ANIMJUMP2TREAD))
#define TRANS3 (!strcmp(Motion, ANIMFALL2TREAD) || !strcmp(Motion, ANIMFALL2CRAWL) || !strcmp(Motion, ANIMFALL2WALK) || !strcmp(Motion, ANIMFALL2JUMP) || !strcmp(Motion, ANIMWALK2JUMP) || !strcmp(Motion, ANIMWALK2CRAWL) || !strcmp(Motion, ANIMCRAWL2WALK))
#define TRANS4 (!strcmp(Motion, ANIMIDLE2CRAWL) || !strcmp(Motion, ANIMAIM2CROUCH) || !strcmp(Motion, ANIMCROUCH2AIM) || !strcmp(Motion, ANIMW2TREAD) || !strcmp(Motion, ANIMFALL2RUN) || !strcmp(Motion, ANIMCRAWL2RUN))
#define ALLBACK ( !strcmp(Motion, ANIMWALKBACK) || !strcmp(Motion, ANIMRUNBACK) || !strcmp(Motion, ANIMCRAWLBACK) || !strcmp(Motion, ANIMWALKSHOOTBACK) || !strcmp(Motion, ANIMRUNSHOOTBACK) || !strcmp(Motion, ANIMCRAWLSHOOTBACK))
#define ALLTRANS (TRANS1 || TRANS2 || TRANS3 || TRANS4)
#define ALLIDLE (ALLTRANS || JUMPING || FALLING || STARTLAND || CSTARTLAND || TREADING || IDLING || CIDLING || !strcmp(Motion, ANIMCLIMB) || !strcmp(Motion, ANIMSHOOT) || !strcmp(Motion, ANIMCSHOOT))
#define ALLWALK (CLIMBING || SWIMING || ALLTRANS || TREADING || JUMPING || FALLING || ALLBACK || !strcmp(Motion, ANIMWALK) || !strcmp(Motion, ANIMRUN) || !strcmp(Motion, ANIMCRAWL) || !strcmp(Motion, ANIMWALKSHOOT) || !strcmp(Motion, ANIMRUNSHOOT) || !strcmp(Motion, ANIMCRAWLSHOOT))
#define ALLSLIDERIGHT (ALLTRANS || JUMPING || FALLING || !strcmp(Motion, ANIMSLIDERIGHT) || !strcmp(Motion, ANIMRUNSLIDERIGHT) || !strcmp(Motion, ANIMSLIDECRIGHT) || !strcmp(Motion, ANIMSLIDERIGHTSHOOT) || !strcmp(Motion, ANIMRUNSLIDERIGHTSHOOT) || !strcmp(Motion, ANIMSLIDECRIGHTSHOOT))
#define ALLSLIDELEFT (ALLTRANS || JUMPING || FALLING || !strcmp(Motion, ANIMSLIDELEFT) || !strcmp(Motion, ANIMRUNSLIDELEFT) || !strcmp(Motion, ANIMSLIDECLEFT) || !strcmp(Motion, ANIMSLIDELEFTSHOOT) || !strcmp(Motion, ANIMRUNSLIDELEFTSHOOT) || !strcmp(Motion, ANIMSLIDECLEFTSHOOT))

	char *Motion;

	Motion = CCD->ActorManager()->GetMotion(Actor);

	//geEngine_Printf(CCD->Engine()->Engine(), 0,310,"Motion = %s",Motion);
	//geEngine_Printf(CCD->Engine()->Engine(), 0,20,"OldZone = %d",OldZone);
	//geEngine_Printf(CCD->Engine()->Engine(), 0,30,"Zone = %x",Zone);

	switch(m_Moving)
	{
	case MOVEIDLE:
		{
			// jump at idle
			if(m_JumpActive && !STARTLAND && !JUMPING && !CSTARTLAND && !FALLING)
			{
				if(CCD->ActorManager()->CheckAnimation(Actor, ANIMJUMP) == GE_TRUE)
				{
					CCD->ActorManager()->SetAnimationHeight(Actor, ANIMJUMP, false);

					if(m_PlayerViewPoint != FIRSTPERSON)
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

			if(STARTLAND || CSTARTLAND)
			{
				CCD->Weapons()->SetAttackFlag(false);
			}

			if(!strcmp(Motion, ANIMJUMP) && (CCD->ActorManager()->ForceActive(Actor, 0) == GE_FALSE) && m_JumpActive)
			{
				SetJump();
				m_JumpActive = false;
				break;
			}

			if(JUMPING && (CCD->ActorManager()->ForceActive(Actor, 0) == GE_FALSE) && !m_JumpActive)
			{
				if(!Falling)
				{
					CCD->ActorManager()->GetPosition(Actor, &(FallStart));
					Falling = true;
				}

				if(OldZone > 0)
				{
					CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
					CCD->ActorManager()->SetTransitionMotion(Actor, ANIMTREADWATER, ANIMJUMP2TREADTIME, ANIMJUMP2TREAD);
					CCD->ActorManager()->SetNextMotion(Actor, ANIMTREADWATER);
				}
				else
				{
					CCD->ActorManager()->SetTransitionMotion(Actor, ANIMFALL, ANIMJUMP2FALLTIME, ANIMJUMP2FALL);
					CCD->ActorManager()->SetNextMotion(Actor, ANIMFALL);
				}

				break;
			}

			if(SWIMING)
			{
				if(CCD->ActorManager()->CheckAnimation(Actor, ANIMIDLE) == GE_TRUE)
				{
					CCD->ActorManager()->SetTransitionMotion(Actor, ANIMTREADWATER, ANIMSWIM2TREADTIME, ANIMSWIM2TREAD);
					CCD->ActorManager()->SetNextMotion(Actor, ANIMTREADWATER);
					CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
				}

				break;
			}

			if(CCD->ActorManager()->Falling(Actor) == GE_TRUE)
			{
				if(CCD->ActorManager()->ReallyFall(Actor) == RGF_SUCCESS || !strcmp(Motion, ANIMJUMP))
				{
					if(!Falling)
					{
						CCD->ActorManager()->GetPosition(Actor, &FallStart);
						Falling = true;
					}

					if(FALLING || TREADING)
						break;

					if(OldZone > 0 || Zone & kLiquidZone)
					{
						CCD->ActorManager()->SetTransitionMotion(Actor, ANIMTREADWATER, ANIMFALL2TREADTIME, ANIMFALL2TREAD);
						CCD->ActorManager()->SetNextMotion(Actor, ANIMTREADWATER);
						CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
					}
					else
					{
						CCD->ActorManager()->SetTransitionMotion(Actor, ANIMFALL, ANIMJUMP2FALLTIME, ANIMJUMP2FALL);
						CCD->ActorManager()->SetNextMotion(Actor, ANIMFALL);
						CCD->ActorManager()->SetAnimationHeight(Actor, ANIMFALL, false);
					}

					break;
				}
			}

			if(FALLING && (CCD->ActorManager()->Falling(Actor) == GE_FALSE))
			{
				Falling = false;

				m_JumpActive = false;

				if(OnLadder)
				{
					CCD->ActorManager()->SetMotion(Actor, ANIMCLIMB);
					CCD->ActorManager()->SetNextMotion(Actor, ANIMCLIMB);
					CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
				}
				else
				{
					if(m_crouch)
					{
						CCD->ActorManager()->SetMotion(Actor, ANIMCLAND);
						CCD->ActorManager()->SetNextMotion(Actor, ANIMCIDLE);
						CCD->ActorManager()->SetAnimationHeight(Actor, ANIMCIDLE, true);
					}
					else
					{
						CCD->ActorManager()->SetMotion(Actor, ANIMLAND);

						if(OldZone > 0)
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
						if(CCD->ActorManager()->CheckAnimation(Actor, ANIMIDLE) == GE_TRUE)
						{
							CCD->ActorManager()->SetTransitionMotion(Actor, ANIMIDLE, ANIMCROUCH2IDLETIME, ANIMCROUCH2IDLE);
							CCD->ActorManager()->SetNextMotion(Actor, ANIMIDLE);
							CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
							m_crouch = false;
						}
					}
					else if(!TREADING)
					{
						CCD->ActorManager()->SetTransitionMotion(Actor, ANIMTREADWATER, ANIMIDLE2TREADTIME, ANIMIDLE2TREAD);
						CCD->ActorManager()->SetNextMotion(Actor, ANIMTREADWATER);
						CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
						m_crouch = false;
						break;
					}
				}

				if(OldZone == 0)
				{
					if(TREADING)
					{
						CCD->ActorManager()->SetTransitionMotion(Actor, ANIMIDLE, ANIMTREAD2IDLETIME, ANIMTREAD2IDLE);
						CCD->ActorManager()->SetMotion(Actor, ANIMIDLE);
						CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
						break;
					}
				}
			}

			// crouch at idle
			if(!strcmp(Motion, ANIMIDLE) && m_crouch && OldZone == 0)
			{
				CCD->ActorManager()->SetTransitionMotion(Actor, ANIMCIDLE, ANIMIDLE2CROUCHTIME, ANIMIDLE2CROUCH);
				CCD->ActorManager()->SetNextMotion(Actor, ANIMCIDLE);
				CCD->ActorManager()->SetAnimationHeight(Actor, ANIMCIDLE, true);
			}

			if(!strcmp(Motion, ANIMSHOOT) && m_crouch && OldZone == 0)
			{
				CCD->ActorManager()->SetBlendMotion(Actor, ANIMCSHOOT, ANIMCSHOOT1, CCD->CameraManager()->GetTiltPercent());
				CCD->ActorManager()->SetBlendNextMotion(Actor, ANIMCAIM, ANIMCAIM1, CCD->CameraManager()->GetTiltPercent());
				CCD->ActorManager()->SetAnimationHeight(Actor, ANIMCIDLE, true);
			}

			if(!strcmp(Motion, ANIMAIM) && m_crouch && OldZone == 0)
			{
				CCD->ActorManager()->SetTransitionMotion(Actor, ANIMCAIM, ANIMIDLE2CROUCHTIME, ANIMAIM2CROUCH);
				CCD->ActorManager()->SetNextMotion(Actor, ANIMCAIM);
				CCD->ActorManager()->SetAnimationHeight(Actor, ANIMCIDLE, true);
			}

			// uncrouch at idle
			if(!strcmp(Motion, ANIMCIDLE) && !m_crouch)
			{
				if(CCD->ActorManager()->CheckAnimation(Actor, ANIMIDLE) == GE_TRUE)
				{
					CCD->ActorManager()->SetTransitionMotion(Actor, ANIMIDLE, ANIMCROUCH2IDLETIME, ANIMCROUCH2IDLE);
					CCD->ActorManager()->SetNextMotion(Actor, ANIMIDLE);
					CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
				}
			}

			if(!strcmp(Motion, ANIMCSHOOT) && !m_crouch)
			{
				if(CCD->ActorManager()->CheckAnimation(Actor, ANIMIDLE) == GE_TRUE)
				{
					CCD->ActorManager()->SetBlendMotion(Actor, ANIMSHOOT, ANIMSHOOT1, CCD->CameraManager()->GetTiltPercent());
					CCD->ActorManager()->SetBlendNextMotion(Actor, ANIMAIM, ANIMAIM1, CCD->CameraManager()->GetTiltPercent());
					CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
				}
			}

			if(!strcmp(Motion, ANIMCAIM) && !m_crouch)
			{
				if(CCD->ActorManager()->CheckAnimation(Actor, ANIMIDLE) == GE_TRUE)
				{
					CCD->ActorManager()->SetTransitionMotion(Actor, ANIMAIM, ANIMCROUCH2IDLETIME, ANIMCROUCH2AIM);
					CCD->ActorManager()->SetNextMotion(Actor, ANIMAIM);
					CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
				}
			}

			// attack
			if(CCD->Weapons()->GetAttackFlag())
			{
				if(IDLING)
				{
					CCD->ActorManager()->SetBlendMotion(Actor, ANIMSHOOT, ANIMSHOOT1, CCD->CameraManager()->GetTiltPercent());
					CCD->ActorManager()->SetBlendNextMotion(Actor, ANIMAIM, ANIMAIM1, CCD->CameraManager()->GetTiltPercent());
				}

				if(CIDLING)
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
				|| !strcmp(Motion, ANIMWALKBACK)|| !strcmp(Motion, ANIMRUNBACK)
				|| !strcmp(Motion, ANIMRUNSHOOTBACK) || !strcmp(Motion, ANIMWALKSHOOTBACK))
			{
				if(OnLadder && !JUMPING)
				{
					CCD->ActorManager()->SetMotion(Actor, ANIMCLIMB);
				}
				else
				{
					if(OldZone > 0)
					{
						CCD->ActorManager()->SetTransitionMotion(Actor, ANIMTREADWATER, ANIMW2IDLETIME, ANIMW2TREAD);
						CCD->ActorManager()->SetMotion(Actor, ANIMTREADWATER);
						CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
					}
					else
					{
						CCD->ActorManager()->SetTransitionMotion(Actor, ANIMIDLE, ANIMW2IDLETIME, ANIMW2IDLE);
						CCD->ActorManager()->SetNextMotion(Actor, ANIMIDLE);
						CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
					}
				}

				break;
			}

			if(!strcmp(Motion, ANIMCRAWL) || !strcmp(Motion, ANIMCRAWLSHOOT)
				|| !strcmp(Motion, ANIMCRAWLBACK) || !strcmp(Motion, ANIMCRAWLSHOOTBACK))
			{
				CCD->ActorManager()->SetTransitionMotion(Actor, ANIMCIDLE, ANIMC2IDLETIME, ANIMC2IDLE);
				CCD->ActorManager()->SetNextMotion(Actor, ANIMCIDLE);
				break;
			}

			if(!strcmp(Motion, ANIMSLIDECLEFT) || !strcmp(Motion, ANIMSLIDECRIGHT)
				|| !strcmp(Motion, ANIMSLIDECLEFTSHOOT) || !strcmp(Motion, ANIMSLIDECRIGHTSHOOT))
			{
				CCD->ActorManager()->SetTransitionMotion(Actor, ANIMCIDLE, ANIMSLIDE2CROUCHTIME, NULL);
				CCD->ActorManager()->SetNextMotion(Actor, ANIMCIDLE);
				break;
			}

			if(!strcmp(Motion, ANIMSLIDELEFT) || !strcmp(Motion, ANIMRUNSLIDELEFT)
				|| !strcmp(Motion, ANIMSLIDERIGHT) || !strcmp(Motion, ANIMRUNSLIDERIGHT)
				|| !strcmp(Motion, ANIMSLIDELEFTSHOOT) || !strcmp(Motion, ANIMRUNSLIDELEFTSHOOT)
				|| !strcmp(Motion, ANIMSLIDERIGHTSHOOT) || !strcmp(Motion, ANIMRUNSLIDERIGHTSHOOT))
			{
				if(OnLadder)
				{
					CCD->ActorManager()->SetMotion(Actor, ANIMCLIMB);
					break;
				}
				else
				{
					if(OldZone > 0)
					{
						CCD->ActorManager()->SetTransitionMotion(Actor, ANIMTREADWATER, ANIMSLIDE2IDLETIME, NULL);
						CCD->ActorManager()->SetNextMotion(Actor, ANIMTREADWATER);
					}
					else
					{
						CCD->ActorManager()->SetTransitionMotion(Actor, ANIMIDLE, ANIMSLIDE2IDLETIME, NULL);
						CCD->ActorManager()->SetNextMotion(Actor, ANIMIDLE);
					}
				}

				CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, false);

				break;
			}

			if(OnLadder && CLIMBING)
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMCLIMB);
				break;
			}

			if(!ALLIDLE)
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMIDLE);
			}

			break;
		}
	case MOVEWALKFORWARD:
		{
			if(strcmp(Motion, ANIMFALL) && (CCD->ActorManager()->Falling(Actor) == GE_TRUE))
			{
				if(CCD->ActorManager()->ReallyFall(Actor) == RGF_SUCCESS || !strcmp(Motion, ANIMJUMP))
				{
					if(!Falling)
					{
						CCD->ActorManager()->GetPosition(Actor, &FallStart);
						Falling = true;
					}

					if(!(Zone & kInLiquidZone || Zone & kLiquidZone) || !strcmp(Motion, ANIMJUMP))
					{
						CCD->ActorManager()->SetTransitionMotion(Actor, ANIMFALL, ANIMJUMP2FALLTIME, ANIMJUMP2FALL);
						CCD->ActorManager()->SetNextMotion(Actor, ANIMFALL);
						CCD->ActorManager()->SetAnimationHeight(Actor, ANIMFALL, false);
					}

					CCD->Weapons()->SetAttackFlag(false);
					break;
				}
			}

			if(FALLING && (CCD->ActorManager()->Falling(Actor) == GE_FALSE))
			{
				Falling = false;

				if(!m_JumpActive)
				{
					if(OnLadder)
					{
						CCD->ActorManager()->SetMotion(Actor, ANIMCLIMBUP);
						CCD->ActorManager()->SetNextMotion(Actor, ANIMCLIMBUP);
					}
					else
					{
						if(m_crouch)
						{
							CCD->ActorManager()->SetAnimationHeight(Actor, ANIMCIDLE, true);
							CCD->ActorManager()->SetTransitionMotion(Actor, ANIMCRAWL, ANIMFALL2CRAWLTIME, ANIMFALL2CRAWL);
							CCD->ActorManager()->SetNextMotion(Actor, ANIMCRAWL);
						}
						else
						{
							CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
							if(!m_run)
							{
								CCD->ActorManager()->SetTransitionMotion(Actor, ANIMWALK, ANIMFALL2WALKTIME, ANIMFALL2WALK);
								CCD->ActorManager()->SetNextMotion(Actor, ANIMWALK);
							}
							else
							{
								CCD->ActorManager()->SetTransitionMotion(Actor, ANIMRUN, ANIMFALL2WALKTIME, ANIMFALL2RUN);
								CCD->ActorManager()->SetNextMotion(Actor, ANIMRUN);
							}
						}
					}
				}
				else
				{
					if(CCD->ActorManager()->CheckAnimation(Actor, ANIMJUMP) == GE_TRUE)
					{
						CCD->ActorManager()->SetAnimationHeight(Actor, ANIMJUMP, false);
						CCD->ActorManager()->SetTransitionMotion(Actor, ANIMJUMP, ANIMFALL2JUMPTIME, ANIMFALL2JUMP);
						CCD->ActorManager()->SetNextMotion(Actor, ANIMJUMP);
						SetJump();
					}

					m_JumpActive = false;
				}

				CCD->Weapons()->SetAttackFlag(false);
				break;
			}

			if(m_JumpActive && strcmp(Motion, ANIMJUMP) && strcmp(Motion, ANIMFALL)
				&& (CCD->ActorManager()->ForceActive(Actor, 0) == GE_FALSE))
			{
				m_JumpActive = false;

				if(!SWIMING)
				{
					if(CCD->ActorManager()->CheckAnimation(Actor, ANIMJUMP) == GE_TRUE)
					{
						CCD->ActorManager()->SetAnimationHeight(Actor, ANIMJUMP, false);
						CCD->ActorManager()->SetTransitionMotion(Actor, ANIMJUMP, ANIMWALK2JUMPTIME, ANIMWALK2JUMP);
						CCD->ActorManager()->SetNextMotion(Actor, ANIMJUMP);
						SetJump();
						CCD->Weapons()->SetAttackFlag(false);
						break;
					}
				}
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

			if(OldZone > 0)
			{
				if(!SWIMING)
				{
					if(!strcmp(Motion, ANIMTREADWATER))
						CCD->ActorManager()->SetTransitionMotion(Actor, ANIMSWIM, ANIMTREAD2SWIMTIME, ANIMTREAD2SWIM);
					else
						CCD->ActorManager()->SetTransitionMotion(Actor, ANIMSWIM, ANIMWALK2SWIMTIME, ANIMWALK2SWIM);
					CCD->ActorManager()->SetNextMotion(Actor, ANIMSWIM);
					CCD->ActorManager()->SetAnimationHeight(Actor, ANIMSWIM, true);
				}
			}
			else
			{
				if(SWIMING)
				{
					CCD->ActorManager()->SetTransitionMotion(Actor, ANIMWALK, ANIMSWIM2WALKTIME, ANIMSWIM2WALK);
					CCD->ActorManager()->SetNextMotion(Actor, ANIMWALK);
					CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
				}
			}
			// walk from idle
			if(!strcmp(Motion, ANIMCLIMB) || !strcmp(Motion, ANIMIDLE) || !strcmp(Motion, ANIMW2IDLE) || !strcmp(Motion, ANIMCROUCH2IDLE))
			{
				if(OnLadder)
				{
					CCD->ActorManager()->SetMotion(Actor, ANIMCLIMBUP);
				}
				else
				{
					if(!m_run)
					{
						if(!CCD->Weapons()->GetAttackFlag())
						{
							CCD->ActorManager()->SetTransitionMotion(Actor, ANIMWALK, ANIMI2WALKTIME, ANIMI2WALK);
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
							CCD->ActorManager()->SetTransitionMotion(Actor, ANIMRUN, ANIMI2RUNTIME, ANIMI2RUN);
							CCD->ActorManager()->SetNextMotion(Actor, ANIMRUN);
						}
						else
						{
							CCD->ActorManager()->SetBlendMotion(Actor, ANIMRUNSHOOT, ANIMRUNSHOOT1, CCD->CameraManager()->GetTiltPercent());
							CCD->ActorManager()->SetNextMotion(Actor, ANIMRUN);
						}
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
				&& m_crouch && OldZone == 0)
			{
				CCD->ActorManager()->SetAnimationHeight(Actor, ANIMCIDLE, true);
				CCD->ActorManager()->SetTransitionMotion(Actor, ANIMCRAWL, ANIMWALK2CRAWLTIME, ANIMWALK2CRAWL);
				CCD->ActorManager()->SetNextMotion(Actor, ANIMCRAWL);
				break;
			}

			// crawl
			if(!strcmp(Motion, ANIMCIDLE) || !strcmp(Motion, ANIMC2IDLE)
				|| !strcmp(Motion, ANIMCSHOOT) || !strcmp(Motion, ANIMCAIM) || !strcmp(Motion, ANIMIDLE2CROUCH))
			{
				CCD->ActorManager()->SetTransitionMotion(Actor, ANIMCRAWL, ANIMIDLE2CRAWLTIME, ANIMIDLE2CRAWL);
				CCD->ActorManager()->SetNextMotion(Actor, ANIMCRAWL);
				break;
			}

			// change to walk from crawl
			if((!strcmp(Motion, ANIMCRAWL) || !strcmp(Motion, ANIMCRAWLSHOOT)) && !m_crouch)
			{
				if(CCD->ActorManager()->CheckAnimation(Actor, ANIMIDLE) == GE_TRUE)
				{
					CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);

					if(!m_run)
					{
						if(!CCD->Weapons()->GetAttackFlag())
						{
							CCD->ActorManager()->SetTransitionMotion(Actor, ANIMWALK, ANIMCRAWL2WALKTIME, ANIMCRAWL2WALK);
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
							CCD->ActorManager()->SetTransitionMotion(Actor, ANIMRUN, ANIMCRAWL2WALKTIME, ANIMCRAWL2RUN);
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
			}

			if(CCD->Weapons()->GetAttackFlag() && !m_crouch)
			{
				if(!m_run)
				{
					if(OldZone == 0)
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
				CCD->ActorManager()->SetNextMotion(Actor, ANIMCRAWL);
				break;
			}

			if(!strcmp(Motion, ANIMSLIDELEFT) || !strcmp(Motion, ANIMRUNSLIDELEFT)
				|| !strcmp(Motion, ANIMSLIDERIGHT) || !strcmp(Motion, ANIMRUNSLIDERIGHT)
				|| !strcmp(Motion, ANIMSLIDELEFTSHOOT) || !strcmp(Motion, ANIMRUNSLIDELEFTSHOOT)
				|| !strcmp(Motion, ANIMSLIDERIGHTSHOOT) || !strcmp(Motion, ANIMRUNSLIDERIGHTSHOOT))
			{
				if(OnLadder)
				{
					CCD->ActorManager()->SetMotion(Actor, ANIMCLIMBUP);
					CCD->ActorManager()->SetNextMotion(Actor, ANIMCLIMBUP);
				}
				else
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
				}

				break;
			}

			if(!strcmp(Motion, ANIMSLIDECLEFT) || !strcmp(Motion, ANIMSLIDECRIGHT)
				|| !strcmp(Motion, ANIMSLIDECLEFTSHOOT) || !strcmp(Motion, ANIMSLIDECRIGHTSHOOT))
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMCRAWL);
				break;
			}

			if(!strcmp(Motion, ANIMCLIMB))
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMCLIMBUP);
				break;
			}

			if((!strcmp(Motion, ANIMWALK) || !strcmp(Motion, ANIMRUN) || !strcmp(Motion, ANIMCRAWL)) && OnLadder)
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMCLIMBUP);
				break;
			}

			if(CLIMBING && !OnLadder)
				CCD->ActorManager()->SetMotion(Actor, ANIMWALK);

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

			if(!strcmp(Motion, ANIMWALKBACK))
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMWALK);
				break;
			}

			if(!strcmp(Motion, ANIMRUNBACK))
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMRUN);
				break;
			}

			if(!strcmp(Motion, ANIMCRAWLBACK))
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMCRAWL);
				break;
			}

			if(!strcmp(Motion, ANIMSWIMBACK))
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMSWIM);
				break;
			}

			if(!ALLWALK && !m_run)
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMWALK);
				break;
			}

			if(!ALLWALK && m_run)
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMRUN);
				break;
			}

			break;
		}
	case MOVEWALKBACKWARD:
		{
			if(strcmp(Motion, ANIMFALL) && (CCD->ActorManager()->Falling(Actor) == GE_TRUE))
			{
				if(CCD->ActorManager()->ReallyFall(Actor) == RGF_SUCCESS || !strcmp(Motion, ANIMJUMP))
				{
					if(!Falling)
					{
						CCD->ActorManager()->GetPosition(Actor, &FallStart);
						Falling = true;
					}

					if(!(Zone & kInLiquidZone || Zone & kLiquidZone) || !strcmp(Motion, ANIMJUMP))
					{
						CCD->ActorManager()->SetTransitionMotion(Actor, ANIMFALL, ANIMJUMP2FALLTIME, ANIMJUMP2FALL);
						CCD->ActorManager()->SetNextMotion(Actor, ANIMFALL);
						CCD->ActorManager()->SetAnimationHeight(Actor, ANIMFALL, false);
					}

					CCD->Weapons()->SetAttackFlag(false);
					break;
				}
			}

			if(FALLING && (CCD->ActorManager()->Falling(Actor) == GE_FALSE))
			{
				Falling = false;

				if(!m_JumpActive)
				{
					if(OnLadder)
					{
						CCD->ActorManager()->SetMotion(Actor, ANIMCLIMBDOWN);
					}
					else
					{
						if(m_crouch)
						{
							CCD->ActorManager()->SetAnimationHeight(Actor, ANIMCIDLE, true);
							CCD->ActorManager()->SetTransitionMotion(Actor, ANIMCRAWLBACK, ANIMFALL2CRAWLTIME, ANIMFALL2CRAWL);
							CCD->ActorManager()->SetNextMotion(Actor, ANIMCRAWLBACK);
						}
						else
						{
							CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
							if(!m_run)
							{
								CCD->ActorManager()->SetTransitionMotion(Actor, ANIMWALKBACK, ANIMFALL2WALKTIME, ANIMFALL2WALK);
								CCD->ActorManager()->SetNextMotion(Actor, ANIMWALKBACK);
							}
							else
							{
								CCD->ActorManager()->SetTransitionMotion(Actor, ANIMRUNBACK, ANIMFALL2WALKTIME, ANIMFALL2RUN);
								CCD->ActorManager()->SetNextMotion(Actor, ANIMRUNBACK);
							}
						}
					}
				}
				else
				{
					if(CCD->ActorManager()->CheckAnimation(Actor, ANIMJUMP) == GE_TRUE)
					{
						CCD->ActorManager()->SetAnimationHeight(Actor, ANIMJUMP, false);
						CCD->ActorManager()->SetTransitionMotion(Actor, ANIMJUMP, ANIMFALL2JUMPTIME, ANIMFALL2JUMP);
						CCD->ActorManager()->SetNextMotion(Actor, ANIMJUMP);
						SetJump();
					}

					m_JumpActive = false;
				}

				CCD->Weapons()->SetAttackFlag(false);
				break;
			}

			if(m_JumpActive && strcmp(Motion, ANIMJUMP) && strcmp(Motion, ANIMFALL)
				&& (CCD->ActorManager()->ForceActive(Actor, 0) == GE_FALSE))
			{
				m_JumpActive = false;

				if(!SWIMING)
				{
					if(CCD->ActorManager()->CheckAnimation(Actor, ANIMJUMP) == GE_TRUE)
					{
						CCD->ActorManager()->SetAnimationHeight(Actor, ANIMJUMP, false);
						CCD->ActorManager()->SetTransitionMotion(Actor, ANIMJUMP, ANIMWALK2JUMPTIME, ANIMWALK2JUMP);
						CCD->ActorManager()->SetNextMotion(Actor, ANIMJUMP);
						SetJump();
						CCD->Weapons()->SetAttackFlag(false);
						break;
					}
				}
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

			if(OldZone > 0)
			{
				if(!SWIMING)
				{
					if(!strcmp(Motion, ANIMTREADWATER))
						CCD->ActorManager()->SetTransitionMotion(Actor, ANIMSWIMBACK, ANIMTREAD2SWIMTIME, ANIMTREAD2SWIM);
					else
						CCD->ActorManager()->SetTransitionMotion(Actor, ANIMSWIMBACK, ANIMWALK2SWIMTIME, ANIMWALK2SWIM);
					CCD->ActorManager()->SetNextMotion(Actor, ANIMSWIMBACK);
					CCD->ActorManager()->SetAnimationHeight(Actor, ANIMSWIMBACK, true);
				}
			}
			else
			{
				if(SWIMING)
				{
					CCD->ActorManager()->SetTransitionMotion(Actor, ANIMWALKBACK, ANIMSWIM2WALKTIME, ANIMSWIM2WALK);
					CCD->ActorManager()->SetNextMotion(Actor, ANIMWALKBACK);
					CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);
				}
			}

			// walk from idle
			if(!strcmp(Motion, ANIMCLIMB) || !strcmp(Motion, ANIMIDLE) || !strcmp(Motion, ANIMW2IDLE) || !strcmp(Motion, ANIMCROUCH2IDLE))
			{
				if(OnLadder)
				{
					CCD->ActorManager()->SetMotion(Actor, ANIMCLIMBDOWN);
					CCD->ActorManager()->SetNextMotion(Actor, ANIMCLIMBDOWN);
				}
				else
				{
					if(!m_run)
					{
						if(!CCD->Weapons()->GetAttackFlag())
						{
							CCD->ActorManager()->SetTransitionMotion(Actor, ANIMWALKBACK, ANIMI2WALKTIME, ANIMI2WALK);
							CCD->ActorManager()->SetNextMotion(Actor, ANIMWALKBACK);
						}
						else
						{
							CCD->ActorManager()->SetBlendMotion(Actor, ANIMWALKSHOOTBACK, ANIMWALKSHOOT1BACK, CCD->CameraManager()->GetTiltPercent());
							CCD->ActorManager()->SetNextMotion(Actor, ANIMWALKBACK);
						}
					}
					else
					{
						if(!CCD->Weapons()->GetAttackFlag())
						{
							CCD->ActorManager()->SetTransitionMotion(Actor, ANIMRUNBACK, ANIMI2RUNTIME, ANIMI2RUN);
							CCD->ActorManager()->SetNextMotion(Actor, ANIMRUNBACK);
						}
						else
						{
							CCD->ActorManager()->SetBlendMotion(Actor, ANIMRUNSHOOTBACK, ANIMRUNSHOOT1BACK, CCD->CameraManager()->GetTiltPercent());
							CCD->ActorManager()->SetNextMotion(Actor, ANIMRUNBACK);
						}
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
						CCD->ActorManager()->SetMotion(Actor, ANIMWALKBACK);
					else
					{
						CCD->ActorManager()->SetBlendMotion(Actor, ANIMWALKSHOOTBACK, ANIMWALKSHOOT1BACK, CCD->CameraManager()->GetTiltPercent());
						CCD->ActorManager()->SetNextMotion(Actor, ANIMWALKBACK);
					}
				}
				else
				{
					if(!CCD->Weapons()->GetAttackFlag())
						CCD->ActorManager()->SetMotion(Actor, ANIMRUNBACK);
					else
					{
						CCD->ActorManager()->SetBlendMotion(Actor, ANIMRUNSHOOTBACK, ANIMRUNSHOOT1BACK, CCD->CameraManager()->GetTiltPercent());
						CCD->ActorManager()->SetNextMotion(Actor, ANIMRUNBACK);
					}
				}
				break;
			}

			// change to crawl
			if((!strcmp(Motion, ANIMI2WALK) || !strcmp(Motion, ANIMWALKBACK)
				|| !strcmp(Motion, ANIMI2RUN) || !strcmp(Motion, ANIMRUNBACK)
				|| !strcmp(Motion, ANIMWALKSHOOTBACK) || !strcmp(Motion, ANIMRUNSHOOTBACK))
				&& m_crouch && OldZone == 0)
			{
				CCD->ActorManager()->SetAnimationHeight(Actor, ANIMCIDLE, true);
				CCD->ActorManager()->SetTransitionMotion(Actor, ANIMCRAWLBACK, ANIMWALK2CRAWLTIME, ANIMWALK2CRAWL);
				CCD->ActorManager()->SetNextMotion(Actor, ANIMCRAWLBACK);
				break;
			}

			// crawl
			if(!strcmp(Motion, ANIMCIDLE) || !strcmp(Motion, ANIMC2IDLE)
				|| !strcmp(Motion, ANIMCSHOOT) || !strcmp(Motion, ANIMCAIM) || !strcmp(Motion, ANIMIDLE2CROUCH))
			{
				CCD->ActorManager()->SetTransitionMotion(Actor, ANIMCRAWLBACK, ANIMIDLE2CRAWLTIME, ANIMIDLE2CRAWL);
				CCD->ActorManager()->SetNextMotion(Actor, ANIMCRAWLBACK);
				break;
			}

			// change to walk from crawl
			if((!strcmp(Motion, ANIMCRAWLBACK) || !strcmp(Motion, ANIMCRAWLSHOOTBACK)) && !m_crouch)
			{
				if(CCD->ActorManager()->CheckAnimation(Actor, ANIMIDLE) == GE_TRUE)
				{
					CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);

					if(!m_run)
					{
						if(!CCD->Weapons()->GetAttackFlag())
						{
							CCD->ActorManager()->SetTransitionMotion(Actor, ANIMWALKBACK, ANIMCRAWL2WALKTIME, ANIMCRAWL2WALK);
							CCD->ActorManager()->SetNextMotion(Actor, ANIMWALKBACK);
						}
						else
						{
							CCD->ActorManager()->SetBlendMotion(Actor, ANIMWALKSHOOTBACK, ANIMWALKSHOOT1BACK, CCD->CameraManager()->GetTiltPercent());
							CCD->ActorManager()->SetNextMotion(Actor, ANIMWALKBACK);
						}
					}
					else
					{
						if(!CCD->Weapons()->GetAttackFlag())
						{
							CCD->ActorManager()->SetTransitionMotion(Actor, ANIMRUNBACK, ANIMCRAWL2WALKTIME, ANIMCRAWL2RUN);
							CCD->ActorManager()->SetNextMotion(Actor, ANIMRUNBACK);
						}
						else
						{
							CCD->ActorManager()->SetBlendMotion(Actor, ANIMRUNSHOOTBACK, ANIMRUNSHOOT1BACK, CCD->CameraManager()->GetTiltPercent());
							CCD->ActorManager()->SetNextMotion(Actor, ANIMRUNBACK);
						}
					}
					break;
				}
			}

			if(CCD->Weapons()->GetAttackFlag() && !m_crouch)
			{
				if(!m_run)
				{
					if(OldZone == 0)
					{
						if(!strcmp(Motion, ANIMWALKSHOOTBACK))
							CCD->ActorManager()->SetBlendMot(Actor, ANIMWALKSHOOTBACK, ANIMWALKSHOOT1BACK, CCD->CameraManager()->GetTiltPercent());
						else
							CCD->ActorManager()->SetBlendMotion(Actor, ANIMWALKSHOOTBACK, ANIMWALKSHOOT1BACK, CCD->CameraManager()->GetTiltPercent());

						CCD->ActorManager()->SetNextMotion(Actor, ANIMWALKBACK);
					}
				}
				else
				{
					if(!strcmp(Motion, ANIMRUNSHOOTBACK))
						CCD->ActorManager()->SetBlendMot(Actor, ANIMRUNSHOOTBACK, ANIMRUNSHOOT1BACK, CCD->CameraManager()->GetTiltPercent());
					else
						CCD->ActorManager()->SetBlendMotion(Actor, ANIMRUNSHOOTBACK, ANIMRUNSHOOT1BACK, CCD->CameraManager()->GetTiltPercent());

					CCD->ActorManager()->SetNextMotion(Actor, ANIMRUNBACK);
				}

				break;
			}

			if(CCD->Weapons()->GetAttackFlag() && m_crouch)
			{
				if(!strcmp(Motion, ANIMCRAWLSHOOTBACK))
					CCD->ActorManager()->SetBlendMot(Actor, ANIMCRAWLSHOOTBACK, ANIMCRAWLSHOOT1BACK, CCD->CameraManager()->GetTiltPercent());
				else
					CCD->ActorManager()->SetBlendMotion(Actor, ANIMCRAWLSHOOTBACK, ANIMCRAWLSHOOT1BACK, CCD->CameraManager()->GetTiltPercent());

				CCD->ActorManager()->SetNextMotion(Actor, ANIMCRAWLBACK);
				break;
			}

			if(!strcmp(Motion, ANIMSLIDELEFT) || !strcmp(Motion, ANIMRUNSLIDELEFT)
				|| !strcmp(Motion, ANIMSLIDERIGHT) || !strcmp(Motion, ANIMRUNSLIDERIGHT)
				|| !strcmp(Motion, ANIMSLIDELEFTSHOOT) || !strcmp(Motion, ANIMRUNSLIDELEFTSHOOT)
				|| !strcmp(Motion, ANIMSLIDERIGHTSHOOT) || !strcmp(Motion, ANIMRUNSLIDERIGHTSHOOT))
			{
				if(OnLadder)
				{
					CCD->ActorManager()->SetMotion(Actor, ANIMCLIMBUP);
					CCD->ActorManager()->SetNextMotion(Actor, ANIMCLIMBUP);
				}
				else
				{
					if(!m_run)
					{
						if(!CCD->Weapons()->GetAttackFlag())
							CCD->ActorManager()->SetMotion(Actor, ANIMWALKBACK);
						else
						{
							CCD->ActorManager()->SetBlendMotion(Actor, ANIMWALKSHOOTBACK, ANIMWALKSHOOT1BACK, CCD->CameraManager()->GetTiltPercent());
							CCD->ActorManager()->SetNextMotion(Actor, ANIMWALKBACK);
						}
					}
					else
					{
						if(!CCD->Weapons()->GetAttackFlag())
							CCD->ActorManager()->SetMotion(Actor, ANIMRUNBACK);
						else
						{
							CCD->ActorManager()->SetBlendMotion(Actor, ANIMRUNSHOOTBACK, ANIMRUNSHOOT1BACK, CCD->CameraManager()->GetTiltPercent());
							CCD->ActorManager()->SetNextMotion(Actor, ANIMRUNBACK);
						}
					}
				}

				break;
			}

			if(!strcmp(Motion, ANIMSLIDECLEFT) || !strcmp(Motion, ANIMSLIDECRIGHT)
				|| !strcmp(Motion, ANIMSLIDECLEFTSHOOT) || !strcmp(Motion, ANIMSLIDECRIGHTSHOOT))
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMCRAWLBACK);
				break;
			}

			if(!strcmp(Motion, ANIMCLIMB))
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMCLIMBDOWN);
				break;
			}

			if((!strcmp(Motion, ANIMWALKBACK) || !strcmp(Motion, ANIMRUNBACK) || !strcmp(Motion, ANIMCRAWLBACK)) && OnLadder)
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMCLIMBDOWN);
				break;
			}

			if(CLIMBING && !OnLadder)
				CCD->ActorManager()->SetMotion(Actor, ANIMWALKBACK);

			if(!strcmp(Motion, ANIMWALKBACK) && m_run)
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMRUNBACK);
				break;
			}

			if(!strcmp(Motion, ANIMRUNBACK) && !m_run)
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMWALKBACK);
				break;
			}

			if(!strcmp(Motion, ANIMWALK))
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMWALKBACK);
				break;
			}

			if(!strcmp(Motion, ANIMRUN))
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMRUNBACK);
				break;
			}

			if(!strcmp(Motion, ANIMCRAWL))
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMCRAWLBACK);
				break;
			}

			if(!strcmp(Motion, ANIMSWIM))
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMSWIMBACK);
				break;
			}

			if(!ALLWALK && !m_run)
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMWALKBACK);
				break;
			}

			if(!ALLWALK && m_run)
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMRUNBACK);
				break;
			}

			break;
		}
	case MOVESLIDERIGHT:
		{
			m_JumpActive = false;

			if(strcmp(Motion, ANIMFALL) && (CCD->ActorManager()->Falling(Actor) == GE_TRUE))
			{
				if(CCD->ActorManager()->ReallyFall(Actor) == RGF_SUCCESS || !strcmp(Motion, ANIMJUMP))
				{
					if(!Falling)
					{
						CCD->ActorManager()->GetPosition(Actor, &(FallStart));
						Falling = true;
					}

					if(!(Zone & kInLiquidZone || Zone & kLiquidZone) || !strcmp(Motion, ANIMJUMP))
					{
						CCD->ActorManager()->SetTransitionMotion(Actor, ANIMFALL, ANIMJUMP2FALLTIME, ANIMJUMP2FALL);
						CCD->ActorManager()->SetNextMotion(Actor, ANIMFALL);
						CCD->ActorManager()->SetAnimationHeight(Actor, ANIMFALL, false);
					}

					CCD->Weapons()->SetAttackFlag(false);
					break;
				}
			}

			if(FALLING && (CCD->ActorManager()->Falling(Actor) == GE_FALSE))
			{
				Falling = false;

				if(m_crouch)
				{
					CCD->ActorManager()->SetAnimationHeight(Actor, ANIMCIDLE, true);
					CCD->ActorManager()->SetTransitionMotion(Actor, ANIMSLIDECRIGHT, ANIMFALL2CRAWLTIME, ANIMFALL2CRAWL);
					CCD->ActorManager()->SetNextMotion(Actor, ANIMSLIDECRIGHT);
				}
				else
				{
					CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);

					if(!m_run)
					{
						CCD->ActorManager()->SetTransitionMotion(Actor, ANIMSLIDERIGHT, ANIMFALL2WALKTIME, ANIMFALL2WALK);
						CCD->ActorManager()->SetNextMotion(Actor, ANIMSLIDERIGHT);
					}
					else
					{
						CCD->ActorManager()->SetTransitionMotion(Actor, ANIMRUNSLIDERIGHT, ANIMFALL2WALKTIME, ANIMFALL2RUN);
						CCD->ActorManager()->SetNextMotion(Actor, ANIMRUNSLIDERIGHT);
					}
				}

				CCD->Weapons()->SetAttackFlag(false);
				break;
			}

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
				if(CCD->ActorManager()->CheckAnimation(Actor, ANIMIDLE) == GE_TRUE)
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

			if(!ALLSLIDERIGHT && !m_run)
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMSLIDERIGHT);
				break;
			}

			if(!ALLSLIDERIGHT && m_run)
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMRUNSLIDERIGHT);
				break;
			}

			break;
		}
	case MOVESLIDELEFT:
		{
			m_JumpActive = false;

			if(strcmp(Motion, ANIMFALL) && (CCD->ActorManager()->Falling(Actor) == GE_TRUE))
			{
				if(CCD->ActorManager()->ReallyFall(Actor) == RGF_SUCCESS || !strcmp(Motion, ANIMJUMP))
				{
					if(!Falling)
					{
						CCD->ActorManager()->GetPosition(Actor, &(FallStart));
						Falling = true;
					}

					if(!(Zone & kInLiquidZone || Zone & kLiquidZone) || !strcmp(Motion, ANIMJUMP))
					{
						CCD->ActorManager()->SetTransitionMotion(Actor, ANIMFALL, ANIMJUMP2FALLTIME, ANIMJUMP2FALL);
						CCD->ActorManager()->SetNextMotion(Actor, ANIMFALL);
						CCD->ActorManager()->SetAnimationHeight(Actor, ANIMFALL, false);
					}

					CCD->Weapons()->SetAttackFlag(false);
					break;
				}
			}

			if(FALLING && (CCD->ActorManager()->Falling(Actor) == GE_FALSE))
			{
				Falling = false;

				if(m_crouch)
				{
					CCD->ActorManager()->SetAnimationHeight(Actor, ANIMCIDLE, true);
					CCD->ActorManager()->SetTransitionMotion(Actor, ANIMSLIDECLEFT, ANIMFALL2CRAWLTIME, ANIMFALL2CRAWL);
					CCD->ActorManager()->SetNextMotion(Actor, ANIMSLIDECLEFT);
				}
				else
				{
					CCD->ActorManager()->SetAnimationHeight(Actor, ANIMIDLE, true);

					if(!m_run)
					{
						CCD->ActorManager()->SetTransitionMotion(Actor, ANIMSLIDELEFT, ANIMFALL2WALKTIME, ANIMFALL2WALK);
						CCD->ActorManager()->SetNextMotion(Actor, ANIMSLIDELEFT);
					}
					else
					{
						CCD->ActorManager()->SetTransitionMotion(Actor, ANIMRUNSLIDELEFT, ANIMFALL2WALKTIME, ANIMFALL2RUN);
						CCD->ActorManager()->SetNextMotion(Actor, ANIMRUNSLIDELEFT);
					}
				}

				CCD->Weapons()->SetAttackFlag(false);
				break;
			}

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
				if(CCD->ActorManager()->CheckAnimation(Actor, ANIMIDLE) == GE_TRUE)
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

			if(!ALLSLIDELEFT && !m_run)
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMSLIDELEFT);
				break;
			}

			if(!ALLSLIDELEFT && m_run)
			{
				CCD->ActorManager()->SetMotion(Actor, ANIMRUNSLIDELEFT);
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

	CCD->ActorManager()->GetPosition(Actor, &Translation);
	CCD->ActorManager()->GetRotation(Actor, &Rotation);

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
	CCD->ActorManager()->GetBoundingBox(Actor, theBox);
}

/* ------------------------------------------------------------------------------------ */
// GetIn
//
// Get the direction the player avatar is facing
/* ------------------------------------------------------------------------------------ */
void CPlayer::GetIn(geVec3d *In)
{
	CCD->ActorManager()->InVector(Actor, In);
}

/* ------------------------------------------------------------------------------------ */
// GetLeft
/* ------------------------------------------------------------------------------------ */
void CPlayer::GetLeft(geVec3d *Left)
{
	CCD->ActorManager()->LeftVector(Actor, Left);
}

/* ------------------------------------------------------------------------------------ */
// SaveTo
//
// Save the current player state to an open file.
/* ------------------------------------------------------------------------------------ */
int CPlayer::SaveTo(FILE *SaveFD)
{
	int i;

	fwrite(m_PlayerName, sizeof(char), 64, SaveFD);
	fwrite(&m_PlayerViewPoint, sizeof(int), 1, SaveFD);
	geVec3d position = Position();
	fwrite(&position, sizeof(geVec3d), 1, SaveFD);
	CCD->ActorManager()->GetRotate(Actor, &position);
	fwrite(&position, sizeof(geVec3d), 1, SaveFD);
	fwrite(&m_JumpActive, sizeof(bool), 1, SaveFD);
	fwrite(&m_Jumping, sizeof(geBoolean), 1, SaveFD);
	fwrite(&m_run, sizeof(bool), 1, SaveFD);
	fwrite(&m_Running, sizeof(geBoolean), 1, SaveFD);
	fwrite(&m_crouch, sizeof(bool), 1, SaveFD);
	fwrite(&m_oldcrouch, sizeof(bool), 1, SaveFD);
	fwrite(&m_lastdirection, sizeof(int), 1, SaveFD);
	fwrite(&Falling, sizeof(bool), 1, SaveFD);
	fwrite(&OldFalling, sizeof(bool), 1, SaveFD);
	fwrite(&FallStart, sizeof(geVec3d), 1, SaveFD);
	fwrite(&LastHealth, sizeof(int), 1, SaveFD);
	fwrite(&StaminaTime, sizeof(geFloat), 1, SaveFD);

	for(i=0; i<20; i++)
	{
		fwrite(&StaminaTime1[i], sizeof(geFloat), 1, SaveFD);
	}

	fwrite(&lighton, sizeof(bool), 1, SaveFD);
	fwrite(&CurrentLiteLife, sizeof(geFloat), 1, SaveFD);
	fwrite(&DecayLite, sizeof(bool), 1, SaveFD);
	fwrite(&CurrentLiteDecay, sizeof(geFloat), 1, SaveFD);
	fwrite(&m_LastMovementSpeed, sizeof(geFloat), 1, SaveFD);
	fwrite(&m_Gravity, sizeof(geFloat), 1, SaveFD);
	fwrite(&m_StepHeight, sizeof(geFloat), 1, SaveFD);
	fwrite(&InLiquid, sizeof(int), 1, SaveFD);
	fwrite(&LiquidTime, sizeof(float), 1, SaveFD);
	fwrite(UseAttribute, sizeof(char), 64*10, SaveFD);
	fwrite(&restoreoxy, sizeof(bool), 1, SaveFD);
	geFloat Level, Decay;

	for(i=0; i<4; ++i)
	{
		CCD->ActorManager()->GetForce(Actor, i, &position, &Level, &Decay);
		fwrite(&position, sizeof(geVec3d), 1, SaveFD);
		fwrite(&Level, sizeof(geFloat), 1, SaveFD);
		fwrite(&Decay, sizeof(geFloat), 1, SaveFD);
	}

	//	If we have attributes, save them.
	bool bAttributes = false;

	m_Attr = CCD->ActorManager()->Inventory(Actor);

	if(m_Attr)
	{
		bAttributes = true;
		fwrite(&bAttributes, sizeof(bool), 1, SaveFD);
		m_Attr->SaveTo(SaveFD, false);
	}
	else
	{
		fwrite(&bAttributes, sizeof(bool), 1, SaveFD);
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
	fread(m_PlayerName, sizeof(char), 64, RestoreFD);
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
	fread(&m_Running, sizeof(geBoolean), 1, RestoreFD);
	fread(&m_crouch, sizeof(bool), 1, RestoreFD);
	fread(&m_oldcrouch, sizeof(bool), 1, RestoreFD);
	fread(&m_lastdirection, sizeof(int), 1, RestoreFD);
	fread(&Falling, sizeof(bool), 1, RestoreFD);
	fread(&OldFalling, sizeof(bool), 1, RestoreFD);
	fread(&FallStart, sizeof(geVec3d), 1, RestoreFD);
	fread(&LastHealth, sizeof(int), 1, RestoreFD);
	fread(&StaminaTime, sizeof(geFloat), 1, RestoreFD);

	for(i=0;i<20;i++)
	{
		fread(&StaminaTime1[i], sizeof(geFloat), 1, RestoreFD);
	}

	fread(&lighton, sizeof(bool), 1, RestoreFD);
	fread(&CurrentLiteLife, sizeof(geFloat), 1, RestoreFD);
	fread(&DecayLite, sizeof(bool), 1, RestoreFD);
	fread(&CurrentLiteDecay, sizeof(geFloat), 1, RestoreFD);
	fread(&m_LastMovementSpeed, sizeof(geFloat), 1, RestoreFD);
	fread(&m_Gravity, sizeof(geFloat), 1, RestoreFD);
	CCD->ActorManager()->SetGravity(Actor, m_Gravity);
	fread(&m_StepHeight, sizeof(geFloat), 1, RestoreFD);
	CCD->ActorManager()->SetStepHeight(Actor, m_StepHeight);
	CCD->ActorManager()->SetAutoStepUp(Actor, true);
	fread(&InLiquid, sizeof(int), 1, RestoreFD);
	fread(&LiquidTime, sizeof(float), 1, RestoreFD);
	fread(UseAttribute, sizeof(char), 64*10, RestoreFD);
	fread(&restoreoxy, sizeof(bool), 1, RestoreFD);
	geFloat Level, Decay;

	for(i=0; i<4; ++i)
	{
		fread(&position, sizeof(geVec3d), 1, RestoreFD);
		fread(&Level, sizeof(geFloat), 1, RestoreFD);
		fread(&Decay, sizeof(geFloat), 1, RestoreFD);
		CCD->ActorManager()->SetForce(Actor, i, position, Level, Decay);
	}

	bool bAttributes;
	fread(&bAttributes, sizeof(bool), 1, RestoreFD);

	if(bAttributes)
	{
		m_Attr = CCD->ActorManager()->Inventory(Actor);
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
		CCD->ActorManager()->SetActorFlags(Actor, GE_ACTOR_COLLIDE | GE_ACTOR_RENDER_MIRRORS);
		geExtBox theBox;
		CCD->ActorManager()->GetBoundingBox(Actor, &theBox);
		CCD->CameraManager()->GetCameraOffset(&LookPosition, &LookRotation);
		LookFlags = CCD->CameraManager()->GetTrackingFlags();
		CCD->CameraManager()->SetTrackingFlags(kCameraTrackPosition);
		geVec3d theRotation = {0.0f, 0.0f, 0.0f};
		geVec3d theTranslation = {0.0f, 0.0f, 0.0f};
		theTranslation.Y = theBox.Max.Y * 0.75f;
		CCD->CameraManager()->SetCameraOffset(theTranslation, theRotation);
	}
	else
	{
		m_LookMode = false;
		CCD->ActorManager()->SetActorFlags(Actor,
			GE_ACTOR_RENDER_NORMAL | GE_ACTOR_COLLIDE | GE_ACTOR_RENDER_MIRRORS);
		CCD->CameraManager()->SetTrackingFlags(LookFlags);
		CCD->CameraManager()->SetCameraOffset(LookPosition, LookRotation);
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
	CCD->ActorManager()->SetActorFlags(Actor, GE_ACTOR_COLLIDE | mirror);
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
	CCD->ActorManager()->SetActorFlags(Actor,
		GE_ACTOR_RENDER_NORMAL | GE_ACTOR_COLLIDE | GE_ACTOR_RENDER_MIRRORS);
}

/* ------------------------------------------------------------------------------------ */
// SaveAttributes
//
// Save off all attributes and inventory to a file.  Mainly used to
// ..preserve player data across level changes.
/* ------------------------------------------------------------------------------------ */
int CPlayer::SaveAttributes(const char *szSaveFile)
{
	FILE *theFile = CCD->OpenRFFile(kInstallFile, szSaveFile, "wb");

	if(theFile == NULL)
	{
		char szError[256];
		sprintf(szError, "[WARNING] File %s - Line %d: Failed to SaveAttributes to '%s'",
				__FILE__, __LINE__, szSaveFile);
		CCD->ReportError(szError, false);
		return RGF_FAILURE;
	}

	m_Attr = CCD->ActorManager()->Inventory(Actor);
	m_Attr->SaveTo(theFile, false);

	fclose(theFile);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SaveAttributesAscii
/* ------------------------------------------------------------------------------------ */
int CPlayer::SaveAttributesAscii(const char *szSaveFile)
{
	FILE *theFile = CCD->OpenRFFile(kTempFile, szSaveFile, "wt");

	if(theFile == NULL)
	{
		char szError[256];
		sprintf(szError, "[WARNING] File %s - Line %d: Failed to SaveAttributes to '%s'",
				__FILE__, __LINE__, szSaveFile);
		CCD->ReportError(szError, false);
		return RGF_FAILURE;
	}

	m_Attr = CCD->ActorManager()->Inventory(Actor);
	m_Attr->SaveAscii(theFile);

	fclose(theFile);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// LoadAttributes
//
// Load all attributes and inventory from a file.  Mainly used to
// ..restore player data after a level change.
/* ------------------------------------------------------------------------------------ */
int CPlayer::LoadAttributes(const char *szSaveFile)
{
	FILE *theFile = CCD->OpenRFFile(kInstallFile, szSaveFile, "rb");

	if(theFile == NULL)
	{
		char szError[256];
		sprintf(szError, "[WARNING] File %s - Line %d: Failed to LoadAttributes from '%s'",
				__FILE__, __LINE__, szSaveFile);
		CCD->ReportError(szError, false);
		return RGF_FAILURE;
	}

	m_Attr = CCD->ActorManager()->Inventory(Actor);
	m_Attr->RestoreFrom(theFile, false);

	fclose(theFile);

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
	CCD->ActorManager()->Position(Actor, m_Translation);
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
geSound_Def *CPlayer::LoadAudioClip(const char *szFilename)
{
	if(!EffectC_IsStringNull(szFilename))
	{
		geSound_Def *SoundPointer = SPool_Sound(szFilename);
		return SoundPointer;
	}

	return NULL;
}

/* ------------------------------------------------------------------------------------ */
//	ShowFog
//
//	If fogging is enabled for this player, activate it.
/* ------------------------------------------------------------------------------------ */
void CPlayer::ShowFog()
{
	CCD->Engine()->EnableFog(m_FogActive);
}

/* ------------------------------------------------------------------------------------ */
//	DisableFog
//
//	Unconditionally disable fog for this player.
/* ------------------------------------------------------------------------------------ */
void CPlayer::DisableFog()
{
	CCD->Engine()->EnableFog(GE_FALSE);
}

/* ------------------------------------------------------------------------------------ */
//	ActivateClipPlane
//
//	Activate the far clip plane, if used.
/* ------------------------------------------------------------------------------------ */
void CPlayer::ActivateClipPlane()
{
	CCD->CameraManager()->EnableFarClipPlane(m_ClipActive);
}

/* ------------------------------------------------------------------------------------ */
//	DisableClipPlane
//
//	Unconditionally disable the far clip plane.
/* ------------------------------------------------------------------------------------ */
void CPlayer::DisableClipPlane()
{
	CCD->CameraManager()->EnableFarClipPlane(GE_FALSE);
}

/* ------------------------------------------------------------------------------------ */
// SetCrouch
/* ------------------------------------------------------------------------------------ */
void CPlayer::SetCrouch(bool value)
{
	m_crouch = value;

}

/* ------------------------------------------------------------------------------------ */
// DoMovements
/* ------------------------------------------------------------------------------------ */
bool CPlayer::DoMovements()
{
	// Ok, if the player moved, handle any processing necessary to reflect
	// ..that fact in the world.
	bool bPlayerMoved = false;
	m_CoeffSpeed = 1.0f;		//Coef de course
	int theZone, OldZone;
	geFloat fTemp;

	CCD->ActorManager()->GetActorZone(Actor, &theZone);
	CCD->ActorManager()->GetActorOldZone(Actor, &OldZone);

	if(OldZone > 0 || theZone == kLiquidZone || theZone == kInLiquidZone)
	{
		Liquid *LQ = CCD->ActorManager()->GetLiquid(Actor);
		if(LQ)
		{
			m_CoeffSpeed = (float)LQ->SpeedCoeff*0.01f;// /100.0f;
		}
	}
	else if(theZone & kLavaZone)
	{
		m_CoeffSpeed = 0.55f;
	}
	else if(theZone & kSlowMotionZone)
	{
		m_CoeffSpeed = 0.25f;
	}
	else if(theZone & kFastMotionZone)
	{
		m_CoeffSpeed = 2.0f;
	}

	// A little run wanted ?
	if((m_Running || alwaysrun) && !GetCrouch() && OldZone == 0)	//Don't run when crouch
	{
		if(CCD->ActorManager()->Falling(Actor) != GE_TRUE)	//If we are not falling, adjust speed modifications
			m_CoeffSpeed *= 2.0f;
	}

	// Want a jump ?
	if (m_Jumping)
		StartJump();

	if(GetMoving() != MOVEIDLE)
	{
		switch(GetMoving())
		{
		case MOVEWALKFORWARD:
			fTemp = Speed() * ((CCD->LastElapsedTime_F()*0.05f) * m_CoeffSpeed);
			if(GetSlideWalk() == MOVESLIDELEFT)
				Move(RGF_K_LEFT, fTemp);
			if(GetSlideWalk() == MOVESLIDERIGHT)
				Move(RGF_K_RIGHT, fTemp);
			Move(RGF_K_FORWARD, fTemp);
			bPlayerMoved = true;
			break;
		case MOVEWALKBACKWARD:
			fTemp = Speed() * ((CCD->LastElapsedTime_F()*0.05f) * (m_CoeffSpeed-0.33f));	//Some hard coding. todo : make configurable
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
				fTemp = Speed() * ((CCD->LastElapsedTime_F()*0.05f) * (m_CoeffSpeed-0.25f));	//Some hard coding. todo : make configurable
				Move(RGF_K_LEFT, fTemp);
				if(GetSlideWalk() == MOVEWALKFORWARD)
				{
					Move(RGF_K_FORWARD, fTemp);
					Moving(MOVEWALKFORWARD);
				}
				if(GetSlideWalk() == MOVEWALKBACKWARD)
				{
					Move(RGF_K_BACKWARD, fTemp);
					Moving(MOVEWALKBACKWARD);
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
				fTemp = Speed() * ((CCD->LastElapsedTime_F()*0.05f) * (m_CoeffSpeed-0.25f));	//Some hard coding. todo : make configurable
				Move(RGF_K_RIGHT, fTemp);
				if(GetSlideWalk() == MOVEWALKFORWARD)
				{
					Move(RGF_K_FORWARD, fTemp);
					Moving(MOVEWALKFORWARD);
				}
				if(GetSlideWalk() == MOVEWALKBACKWARD)
				{
					Move(RGF_K_BACKWARD, fTemp);
					Moving(MOVEWALKBACKWARD);
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

	theBox.Min.X = theBox.Min.Y = theBox.Min.Z = -10.0f * m_Scale;
	theBox.Max.X = theBox.Max.Y = theBox.Max.Z = 10.0f * m_Scale;

	if(m_PlayerViewPoint == FIRSTPERSON)
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
		CCD->ActorManager()->GetPosition(Actor, &thePosition);
		CCD->ActorManager()->GetBoundingBox(Actor, &pBox);
		thePosition.Y += (pBox.Max.Y*0.5f);
		CCD->ActorManager()->GetRotate(Actor, &theRotation);
		geVec3d CRotation, CPosition;
		CCD->CameraManager()->GetCameraOffset(&CPosition, &CRotation);

		geXForm3d_SetZRotation(&Xf, theRotation.Z);
		geXForm3d_RotateX(&Xf, theRotation.X - CCD->CameraManager()->GetTilt());
		geXForm3d_RotateY(&Xf, theRotation.Y);
	}

	geXForm3d_GetIn(&Xf, &Direction);
	geVec3d_AddScaled(&thePosition, &Direction, UseRange, &Back);
	geVec3d_Copy(&thePosition, &Front);

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

// We can't just return now, there may be triggers attached to the Moving Platform that use keys also
			CCD->Platforms()->HandleCollision(Collision.Model, false, true, Actor);

			if(CCD->Triggers()->HandleCollision(Collision.Model, false, true, Actor) == RGF_SUCCESS)
				return;
		}

		if(Collision.Actor)
		{
			if(CCD->Props()->HandleCollision(Collision.Actor, Actor, true, true) == RGF_SUCCESS)
				return;

			if(CCD->Attributes()->HandleCollision(Actor, Collision.Actor, true) == GE_TRUE)
				return;

			if(CCD->Pawns()->Converse(Collision.Actor))
				return;
		}
	}
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
bool CPlayer::GetUseAttribute(const char *Attr)
{
	int i;

	for(i=0; i<10; ++i)
	{
		if(!strcmp(Attr, UseAttribute[i]))
			return true;
	}

	return false;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
bool CPlayer::SetUseAttribute(const char *Attr)
{
	int i;

	CCD->Armours()->DisableHud(Attr);
	CCD->LiftBelts()->DisableHud(Attr);

	for(i=0; i<10; ++i)
	{
		if(EffectC_IsStringNull(UseAttribute[i]))
		{
			strcpy(UseAttribute[i], Attr);
			return true;
		}
	}

	return false;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
bool CPlayer::DelUseAttribute(const char *Attr)
{
	int i;

	for(i=0; i<10; ++i)
	{
		if(!strcmp(Attr, UseAttribute[i]))
		{
			UseAttribute[i][0] = '\0';
			return true;
		}
	}

	return false;
}

void CPlayer::SetGravity(const geVec3d *vec)
{
	geVec3d_Copy(vec, &m_Gravity);
}

void CPlayer::SetWind(const geVec3d *vec)
{
	geVec3d_Copy(vec, &m_Wind);
}

void CPlayer::ModifyWind(const geVec3d *vec)
{
	geVec3d_Add(vec, &m_Wind, &m_Wind);
}

void CPlayer::SetInitialWind(const geVec3d *vec)
{
	geVec3d_Copy(vec, &m_InitialWind);
}

/* ----------------------------------- END OF FILE ------------------------------------ */
