/************************************************************************************//**
 * @file CPlayState.cpp
 * @brief CPlayState class
 *
 * This file contains the implementation of the Play State class.
 * @author Daniel Queteschiner
 * @date December 2009
 *//*
 * Copyright (c) 2009 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"
#include "CFileManager.h"
#include "CPlayState.h"
#include "CMenuState.h"

#include "CGUIManager.h"
#include "CHeadsUpDisplay.h"
#include "CInventory.h"
#include "CAudioManager.h"
#include "CMIDIAudio.h"
#include "CCDAudio.h"
// start multiplayer
#include "HawkNL\\nl.h"
// end multiplayer

#include "CLevel.h"
#include "CPawnManager.h" // get rid of this
#include "Qx\\qxTerrainMgr.h"

#include "CActMaterial.h"
#include "CAttribute.h"
#include "CAutoDoors.h"
#include "CChange.h"
#include "CChangeLevel.h"
#include "CCountDown.h"
#include "CDamage.h"
#include "CElectric.h"
#include "CLiftBelt.h"
#include "CLogic.h"
#include "CMorphingFields.h"
#include "CMovingPlatforms.h"
#include "CStaticEntity.h"
#include "CStaticMesh.h"
#include "CTeleporter.h"
#include "CTriggers.h"

#include "CutScene.h"
#include "CMessage.h"


CPlayState* CPlayState::m_PlayState = NULL;


CPlayState::CPlayState()
	: m_ShowCursor(false),
	m_CameraMode(false),
	m_KeyRotation(0),
	m_Zooming(0),
	m_Attacking(0)
{
	// create a default window and set it as root
	m_DefaultWindow = (CEGUI::DefaultWindow*)CCD->GUIManager()->CreateDefaultWindow("Play/Root");
}


void CPlayState::Enter()
{
	CCD->Log()->Debug("Enter PlayState");

	if(!m_ShowCursor)
	{
		CCD->Input()->CenterMouse();
		CCD->GUIManager()->MousePosition(0.5f * CCD->Engine()->Width(), 0.5f * CCD->Engine()->Height());
	}

	CCD->GUIManager()->SetSheet(m_DefaultWindow);
	CCD->GUIManager()->SetCursorVisible(m_ShowCursor);

	m_CameraMode = false;

	m_KeyRotation = 0;
	m_Zooming = 0;
	m_Attacking = 0;

	if(!CCD->Player()->FirstPersonView())
		CCD->Player()->LookMode(false);

	CCD->Player()->Moving(MOVEIDLE);
	CCD->Player()->SetSlideWalk(MOVEIDLE);
	CCD->Player()->SetJumping(GE_FALSE);

	CCD->Level()->ShowFog();
	CCD->Engine()->ResetSystem();

	if(CCD->MenuManager()->IsFrameRateEnabled())
		CCD->Engine()->ShowFrameRate(true);
}


void CPlayState::Exit()
{
	CCD->Log()->Debug("Exit PlayState");

	// save the screen just before switching to the menu
	if(CCD->GUIManager()->IsWindowPresent("Menu/LoadGame/SavegameImage")
		|| CCD->GUIManager()->IsWindowPresent("Menu/SaveGame/SavegameImage"))
	{
		std::string filename = CFileManager::GetSingletonPtr()->GetDirectory(kSavegameFile);
		filename += "\\savegame.bmp";
		geEngine_ScreenShot(CCD->Engine()->Engine(), filename.c_str());
	}

	if(CCD->Level()->AudioStreams())
		CCD->Level()->AudioStreams()->PauseAll();// pause streaming audio

	if(CCD->CDPlayer())
		CCD->CDPlayer()->Stop();		// stop CD music

	if(CCD->MIDIPlayer())
		CCD->MIDIPlayer()->Stop();		// stop midi music

	CCD->Engine()->ShowFrameRate(false);
	CCD->Level()->DisableFog();

	CCD->MenuManager()->ClearVolume();
}


void CPlayState::Pause()
{
	CCD->Log()->Debug("Pause PlayState");
}


void CPlayState::Resume()
{
	CCD->Log()->Debug("Resume PlayState");

	if(!m_ShowCursor)
	{
		CCD->Input()->CenterMouse();
		CCD->GUIManager()->MousePosition(0.5f * CCD->Engine()->Width(), 0.5f * CCD->Engine()->Height());
	}
	CCD->GUIManager()->SetSheet(m_DefaultWindow);
	CCD->GUIManager()->SetCursorVisible(m_ShowCursor);
}


void CPlayState::Update(float timeElapsed)
{
	if(	CCD->GetPaused() || CCD->GetKeyPaused() ||
		CCD->Player()->GetDying() || !CCD->Player()->GetAlive())
	{
		m_CameraMode = false;
		m_KeyRotation = 0;
		m_Zooming = 0;
		m_Attacking = 0;

		if(!CCD->Player()->FirstPersonView())
			CCD->Player()->LookMode(false);

		CCD->Player()->Moving(MOVEIDLE);
		CCD->Player()->SetSlideWalk(MOVEIDLE);
		CCD->Player()->SetJumping(GE_FALSE);
	}

	if(m_CameraMode)
	{
		if(m_KeyRotation & 1)
			CCD->CameraManager()->CameraRotX(false);
		if(m_KeyRotation & 2)
			CCD->CameraManager()->CameraRotY(false, timeElapsed * CCD->MenuManager()->GetMouseSen());
		if(m_KeyRotation & 4)
			CCD->CameraManager()->CameraRotY(true,  timeElapsed * CCD->MenuManager()->GetMouseSen());
		if(m_KeyRotation & 8)
			CCD->CameraManager()->CameraRotX(true);
	}
	else
	{
		CCD->Player()->CheckKeyLook(m_KeyRotation, timeElapsed);
	}

	if(m_Zooming < 0)
		CCD->CameraManager()->ChangeDistance(false, 5.0f);
	else if(m_Zooming > 0)
		CCD->CameraManager()->ChangeDistance(true, 5.0f);

	if(m_Attacking > 0)
		CCD->Weapons()->Attack(false);
	else if(m_Attacking < 0)
		CCD->Weapons()->Attack(true);

	bool player_moved = CCD->Player()->DoMovements();	// Manage player "wanted" movements

	if(!CCD->Player()->GetDying() && CCD->Player()->GetAlive())
	{
#ifdef _DEBUG
		CCD->Player()->CheckMouseLook();
#else
		if(CCD->GetMouseControl())
			CCD->Player()->CheckMouseLook();
#endif
	}

	CCD->Player()->ProcessMove(player_moved);

	CCD->Level()->Update(timeElapsed);

	CCD->CameraManager()->Update(timeElapsed);

	CCD->Level()->ActorManager()->Update(timeElapsed);
	CCD->Level()->HUD()->Tick(timeElapsed);
	CCD->Level()->Messages()->Update(timeElapsed);
	CCD->Level()->CutScenes()->Tick(timeElapsed);

	CCD->NetPlayerManager()->Tick(timeElapsed);
	CCD->AudioManager()->Tick(timeElapsed);			// Audio updates
	CCD->GUIManager()->Update(timeElapsed);

	CCD->CheckMediaPlayers();						// Poll media players

	if(!CCD->Player()->GetAlive())
	{
		geEntity_EntitySet *pSet;
		geEntity *pEntity;
		pSet = geWorld_GetEntitySet(CCD->World(), "PlayerSetup");
		pEntity = geEntity_EntitySetGetNextEntity(pSet, NULL);
		PlayerSetup *pSetup = static_cast<PlayerSetup*>(geEntity_GetUserData(pEntity));

		CCD->Level()->DisableFog();			// Fogging OFF
		CCD->Level()->DisableClipPlane();	// Clip plane OFF

		if(!pSetup->KeepAttrAtDeath)
		{
			CCD->Player()->LoadAttributes("pdoa.bin");
		}
		else
		{
			CCD->Player()->SaveAttributes("temp.bin");
			CCD->Player()->LoadAttributes("pdoa.bin");
			CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
			int health = theInv->Value("health");
			CCD->Player()->LoadAttributes("temp.bin");
			theInv->Set("health", health);
		}

		CCD->Weapons()->ClearWeapon();
		CCD->ProcessLevelChange();
		CCD->Level()->ShowFog();			// Fog on, if any there
		CCD->Level()->ActivateClipPlane();	// Clip plane on, if any there
		CCD->Player()->SetAlive(true);
		CCD->Player()->SetJumping(GE_FALSE);

		//m_SaveTime = 0.0f;
		CCD->Engine()->ResetSystem();
		//framecount = 0;

	}
	else
	{
		// The player has moved, frame updated and rendered.  What we do now is
		// ..check to see if, during the course of all this, the player hit a
		// ..changelevel entity.  If so, we need to flush the current level
		// ..and load the new one.
		if(CCD->ChangeLevel())
		{
			CCD->Log()->Debug("Begin Changelevel Process...");

			CCD->Level()->DisableFog();		// Fogging OFF
			CCD->Level()->DisableClipPlane();	// Clip plane OFF
			CCD->GUIManager()->SetCursorVisible(m_ShowCursor);

			if(EffectC_IsStringNull(CCD->NextLevel()))
			{
				CCD->Level()->AudioStreams()->StopAll(); // stop old streaming audio that might be paused
				CCD->Level()->Shutdown();
				CCD->MenuManager()->DeleteSound();

				if(CCD->CDPlayer())
					CCD->CDPlayer()->Stop();

				if(CCD->MIDIPlayer())
					CCD->MIDIPlayer()->Stop();

				if(CCD->MenuManager()->GetMusicType() != -1)
				{
					if(CCD->MenuManager()->GetMusicType() == 1)
						CCD->MenuManager()->MIDIPlayer()->Play(CCD->MenuManager()->GetMusic(), true);
					else
						CCD->MenuManager()->GetStreamingAudio()->Play(true);
				}

				CCD->MenuManager()->SetmMusicVol(CCD->MenuManager()->GetmVolLevel());

				CCD->MenuManager()->SetInGame(0);
				CCD->GUIManager()->DisableWindow("Menu/Main/SaveGame");
				CCD->SetChangeLevel(false);
				this->PopState();
				return;
			}

			CCD->Weapons()->ClearWeapon();
			CCD->ProcessLevelChange();			// Do the level change
			CCD->Level()->ShowFog();			// Fog on, if any there
			CCD->Level()->ActivateClipPlane();	// Clip plane on, if any there
			CCD->Player()->SaveAttributes("pdoa.bin");
			CCD->Player()->SaveAttributesAscii("attributes.txt");
			CCD->Engine()->ResetSystem();
		}
	}
}


void CPlayState::Render()
{
	CCD->Engine()->BeginFrame();				// Start up rendering for this frame

	CCD->Level()->Render();						// Render the RGF components

	CCD->Engine()->RenderWorld();				// Render the world

	// :TODO: get rid of this
	// Fills a rectangle on the screen with color / alpha (can be used for color/alpha fading)
	for(int i=0; i<MAXFILLAREA; ++i)
		CCD->Level()->Pawns()->FillScreenArea(i);

	bool cflag = false;

	if(CCD->MenuManager()->IsCrosshairsEnabled() &&
		CCD->Player()->GetViewPoint() == FIRSTPERSON &&
		CCD->Weapons()->CrossHair())	// display crosshair
	{
		if(!CCD->Weapons()->CrossHairFixed() && CCD->Weapons()->GetCrossHair())
		{
			CCD->MenuManager()->DisplayCrossHair();
			cflag = true;
		}
	}

	if(CCD->MenuManager()->IsCrosshairsEnabled() &&
		(CCD->Player()->GetViewPoint() == FIRSTPERSON || CCD->Player()->GetViewPoint() == THIRDPERSON) &&
		!CCD->Player()->GetMonitorMode())
	{
		if(CCD->Weapons()->CrossHair() && CCD->Weapons()->CrossHairFixed() && CCD->Weapons()->GetCrossHair())
		{
			CCD->Weapons()->DisplayCrossHair();
		}
		else
		{
			if(!cflag)
			{
				geBitmap *theBmp = CCD->MenuManager()->GetFCrossHair();
				geBitmap_Info BmpInfo;

				if(geBitmap_GetInfo(theBmp, &BmpInfo, NULL) == GE_TRUE)
				{
					int x, y;

					if(CCD->Player()->GetViewPoint() == THIRDPERSON
						&& !CCD->Weapons()->GetCurrent().empty())
					{
						geVec3d ProjectedPoint = CCD->Weapons()->GetProjectedPoint();
						x = (CCD->Engine()->Width() - BmpInfo.Width) / 2;
						y = static_cast<int>(ProjectedPoint.Y) - (BmpInfo.Height) / 2;
					}
					else
					{
						x = (CCD->Engine()->Width() - BmpInfo.Width) / 2;
						y = (CCD->Engine()->Height() - BmpInfo.Height) / 2;
					}

					geEngine_DrawBitmap(CCD->Engine()->Engine(), theBmp, NULL, x, y);
				}
			}
		}
	}

	if(CCD->MenuManager()->IsDebugInfoEnabled())
	{
		CCD->Weapons()->WeaponData();

		char szInfo[256];
		geVec3d Pos = CCD->Player()->Position();
		sprintf(szInfo, "Player Position:  X = %.2f  Y = %.2f  Z = %.2f, Actors: %d", Pos.X, Pos.Y, Pos.Z, CCD->ActorManager()->CountActors());
		CEGUI::Rect draw_area(4.f,
								29.f * CCD->Engine()->Height() / 30.f,
								CCD->Engine()->Width() - 4.f,
								(float)CCD->Engine()->Height());
		CCD->GUIManager()->DrawText(szInfo, "", draw_area, 1.0f);
	}

	/*if(CCD->GetSaving())
	{
		m_SaveTime = m_SavingTime;
		CCD->SetSaving(false);
	}

	if(m_SaveTime > 0.0f)
	{
		m_SaveTime -= (CCD->LastElapsedTime_F() * 0.001f);
		int width	= FontWidth(m_SaveFont, m_Savemsg);
		int height	= FontHeight(m_SaveFont);
		int xoffset = (CCD->Engine()->Width() - width) / 2;
		int yoffset = (CCD->Engine()->Height() - height) / 2;
		FontRect(m_Savemsg, m_SaveFont, xoffset, yoffset);
	}
	else
	{
		m_SaveTime = 0.0f;
	}*/

	CCD->Level()->HUD()->Render();

	CCD->GUIManager()->Render();

	// Displays a text on the screen that tracks position with an Entity
	for(int ii=0; ii<MAXTEXT; ++ii)
		CCD->Level()->Pawns()->ShowText(ii);

	CCD->Level()->Teleporters()->DoFade();

	// Everything rendered, now end the frame.
	CCD->Engine()->EndFrame();					// All done, do the necessary flip

	// TODO
	if(CCD->GetPaused())
	{
		if(CCD->GetUseEffect())
		{
			geFog *theFog;
			int nTemp;
			geVec3d Pos;
			CCD->CameraManager()->TrackMotion();
			CCD->CameraManager()->GetPosition(&Pos);
			GE_RGBA cColor = CCD->GetFogColor();
			cColor.a = 255.0f;
			theFog = geWorld_AddFog(CCD->World());
			geFog_SetAttributes(theFog, &Pos, &cColor, 0.0f, 1500.0f, 240.0f);

			for(nTemp=1000; nTemp<6000; nTemp+=200)
			{
				CCD->Engine()->BeginFrame();
				geFog_SetAttributes(theFog, &Pos, &cColor, 0.0f,
					static_cast<float>(nTemp) + 500.0f,
					240.0f + static_cast<float>(nTemp / 400));
				CCD->Engine()->RenderWorld();
				CCD->Engine()->EndFrame();
			}

			for(nTemp=6000; nTemp>0; nTemp-=200)
			{
				CCD->Engine()->BeginFrame();
				geFog_SetAttributes(theFog, &Pos, &cColor, 0.0f,
					static_cast<float>(nTemp) + 500.0f,
					240.0f + static_cast<float>(nTemp / 400));
				CCD->Engine()->RenderWorld();
				CCD->Engine()->EndFrame();
			}

			geWorld_RemoveFog(CCD->World(), theFog);
			//m_SaveTime = 0.0f;
			CCD->Engine()->ResetSystem();
			//framecount = 0;
		}

		CCD->SetPaused(false);
	}
	// END TODO

	if(!CCD->Player()->GetAlive())
	{
		geEntity_EntitySet *pSet;
		geEntity *pEntity;
		pSet = geWorld_GetEntitySet(CCD->World(), "PlayerSetup");
		pEntity = geEntity_EntitySetGetNextEntity(pSet, NULL);
		PlayerSetup *pSetup = static_cast<PlayerSetup*>(geEntity_GetUserData(pEntity));

		if(pSetup->UseDeathFog)
		{
			geFog *theFog;
			int nTemp;
			geVec3d Pos;
			CCD->CameraManager()->TrackMotion();
			CCD->CameraManager()->GetPosition(&Pos);
			GE_RGBA cColor = pSetup->DeathFogColor;
			cColor.a = 255.0f;
			theFog = geWorld_AddFog(CCD->World());
			geFog_SetAttributes(theFog, &Pos, &cColor, 0.0f, 1500.0f, 50.0f);

			for(nTemp=1000; nTemp<6000; nTemp+=200)
			{
				CCD->Engine()->BeginFrame();
				geFog_SetAttributes(theFog, &Pos, &cColor, 0.0f,
					static_cast<float>(nTemp) + 500.0f,
					50.0f + static_cast<float>(nTemp / 200));
				CCD->Engine()->RenderWorld();
				CCD->Engine()->EndFrame();
			}

			for(nTemp=6000; nTemp>=0; nTemp-=200)
			{
				CCD->Engine()->BeginFrame();
				geFog_SetAttributes(theFog, &Pos, &cColor, 0.0f,
					static_cast<float>(nTemp) + 500.0f,
					50.0f + static_cast<float>(nTemp / 200));
				CCD->Engine()->RenderWorld();
				CCD->Engine()->EndFrame();
			}

			geWorld_RemoveFog(CCD->World(), theFog);
		}
	}
}


void CPlayState::ProcessPressedAction(int action)
{
	if(CCD->GetPaused() || CCD->GetKeyPaused())
		return;
	if(CCD->Player()->GetDying() || !CCD->Player()->GetAlive())
		return;

	switch(action)
	{
	case RGF_K_EXIT:
		{
			geExtBox CBox;
			geVec3d CPosition;

			CCD->ActorManager()->GetBoundingBox(CCD->Player()->GetActor(), &CBox);

			for(int ch=0; ch<50; ++ch)
			{
				CCD->ActorManager()->GetPosition(CCD->Player()->GetActor(), &CPosition);

				if(CCD->Collision()->CheckSolid(&CPosition, &CBox, CCD->Player()->GetActor()))
				{
					CCD->Player()->AddPosition();
					break;
				}

				CCD->Player()->Backtrack();
			}

			this->PopState(); // Exit game

			break;
		}
	case RGF_K_FORWARD:
		{
			CCD->Player()->SetSlideWalk(CCD->Player()->GetMoving());
			CCD->Player()->Moving(MOVEWALKFORWARD);
			break;
		}
	case RGF_K_BACKWARD:
		{
			CCD->Player()->SetSlideWalk(CCD->Player()->GetMoving());
			CCD->Player()->Moving(MOVEWALKBACKWARD);
			break;
		}
	case RGF_K_RUN:
		{
			CCD->Player()->FlipRunning();
			break;
		}
	case RGF_K_CAMERA:
		{
			m_CameraMode = true;
			break;
		}
	case RGF_K_LOOKMODE:
		{
			if(!CCD->Player()->FirstPersonView())
				CCD->Player()->LookMode(true);

			break;
		}
	case RGF_K_USE:
		{
			CCD->Player()->UseItem();
			break;
		}
	case RGF_K_HUD:
		{
			if(CCD->Level()->HUD()->GetActive())
				CCD->Level()->HUD()->Deactivate();
			else
				CCD->Level()->HUD()->Activate();

			break;
		}
	case RGF_K_JUMP:
		{
			CCD->Player()->SetJumping(GE_TRUE);	// Start player jumping
			break;
		}
	case RGF_K_LEFT:
		{
			if(CCD->Player()->GetMoving() == MOVEWALKFORWARD || CCD->Player()->GetMoving() == MOVEWALKBACKWARD)
			{
				CCD->Player()->SetSlideWalk(MOVESLIDELEFT);
			}
			else
			{
				CCD->Player()->SetSlideWalk(CCD->Player()->GetMoving());
				CCD->Player()->Moving(MOVESLIDELEFT);
			}

			break;
		}
	case RGF_K_RIGHT:
		{
			if(CCD->Player()->GetMoving() == MOVEWALKFORWARD || CCD->Player()->GetMoving() == MOVEWALKBACKWARD)
			{
				CCD->Player()->SetSlideWalk(MOVESLIDERIGHT);
			}
			else
			{
				CCD->Player()->SetSlideWalk(CCD->Player()->GetMoving());
				CCD->Player()->Moving(MOVESLIDERIGHT);
			}

			break;
		}
	case RGF_K_LOOKUP:
		{
			m_KeyRotation |= 1;
			break;
		}
	case RGF_K_TURN_LEFT:
		{
			m_KeyRotation |= 2;
			break;
		}
	case RGF_K_TURN_RIGHT:
		{
			m_KeyRotation |= 4;
			break;
		}
	case RGF_K_LOOKDOWN:
		{
			m_KeyRotation |= 8;
			break;
		}
	case RGF_K_ZOOM_WEAPON:
		{
			CCD->CameraManager()->SetZoom(true);
			break;
		}
	case RGF_K_DROP:
		{
			CCD->Weapons()->DropWeapon();
			break;
		}
	case RGF_K_RELOAD:
		{
			CCD->Weapons()->KeyReload();
			break;
		}
	case RGF_K_POWERUP:
		{
			CCD->Level()->LiftBelts()->ChangeLift(true);
			break;
		}
	case RGF_K_POWERDWN:
		{
			CCD->Level()->LiftBelts()->ChangeLift(false);
			break;
		}
	case RGF_K_HOLSTER_WEAPON:
		{
			CCD->Weapons()->Holster();
			break;
		}
	case RGF_K_WEAPON_1:
		{
			CCD->Weapons()->SetWeapon(0);
			break;
		}
	case RGF_K_WEAPON_2:
		{
			CCD->Weapons()->SetWeapon(1);
			break;
		}
	case RGF_K_WEAPON_3:
		{
			CCD->Weapons()->SetWeapon(2);
			break;
		}
	case RGF_K_WEAPON_4:
		{
			CCD->Weapons()->SetWeapon(3);
			break;
		}
	case RGF_K_WEAPON_5:
		{
			CCD->Weapons()->SetWeapon(4);
			break;
		}
	case RGF_K_WEAPON_6:
		{
			CCD->Weapons()->SetWeapon(5);
			break;
		}
	case RGF_K_WEAPON_7:
		{
			CCD->Weapons()->SetWeapon(6);
			break;
		}
	case RGF_K_WEAPON_8:
		{
			CCD->Weapons()->SetWeapon(7);
			break;
		}
	case RGF_K_WEAPON_9:
		{
			CCD->Weapons()->SetWeapon(8);
			break;
		}
	case RGF_K_WEAPON_0:
		{
			CCD->Weapons()->SetWeapon(9);
			break;
		}
	case RGF_K_FIRE:
		{
			m_Attacking = 1;
			break;
		}
	case RGF_K_ALTFIRE:
		{
			m_Attacking = -1;
			break;
		}
	case RGF_K_FIRST_PERSON_VIEW:
		{
			if(CCD->CameraManager()->GetSwitchAllowed() && CCD->CameraManager()->GetSwitch1st())
				CCD->Player()->SwitchCamera(0);

			break;
		}
	case RGF_K_THIRD_PERSON_VIEW:
		{
			if(CCD->CameraManager()->GetSwitchAllowed() && CCD->CameraManager()->GetSwitch3rd())
				CCD->Player()->SwitchCamera(1);

			break;
		}
	case RGF_K_ISO_VIEW:
		{
			if(CCD->CameraManager()->GetSwitchAllowed() && CCD->CameraManager()->GetSwitchIso())
				CCD->Player()->SwitchCamera(2);

			break;
		}
	case RGF_K_SCRNSHOT:
		{
			CCD->MenuManager()->ScreenShot();
			break;
		}
	case RGF_K_CAMERA_RESET:
		{
			CCD->CameraManager()->ResetCamera();
			break;
		}
	case RGF_K_ZOOM_OUT:
		{
			m_Zooming = 1;
			break;
		}
	case RGF_K_ZOOM_IN:
		{
			m_Zooming = -1;
			break;
		}
	case RGF_K_CROUCH:
		{
			CCD->Player()->FlipCrouch();
			break;
		}
	case RGF_K_LIGHT:
		{
			CCD->Player()->FlipLight();
			break;
		}
	case RGF_K_QUICKSAVE:
		{
			FILE *outFD = CFileManager::GetSingletonPtr()->OpenRFFile(kSavegameFile, "savegame.rgf", "wb");
			if(outFD == NULL)
			{
				CCD->Log()->Error("Failed to create savegame file!");
				break;
			}

			// :TODO: show Save Game message (something like CCD->GUIManager()->ShowWindow(...))

			CCD->Engine()->SaveTo(outFD);
			CCD->MenuManager()->SaveTo(outFD, false);
			CCD->Player()->SaveTo(outFD);
			CCD->Level()->Doors()->SaveTo(outFD, false);
			CCD->Level()->Platforms()->SaveTo(outFD, false);
			CCD->Level()->Props()->SaveTo(outFD, false);
			CCD->Level()->Meshes()->SaveTo(outFD, false);
			CCD->Level()->Teleporters()->SaveTo(outFD, false);
			CCD->Level()->MorphingFields()->SaveTo(outFD);
			CCD->MIDIPlayer()->SaveTo(outFD);
			CCD->CDPlayer()->SaveTo(outFD);
			CCD->Level()->Triggers()->SaveTo(outFD, false);
			CCD->Level()->LogicGates()->SaveTo(outFD, false);
			CCD->Level()->Attributes()->SaveTo(outFD, false);
			CCD->Level()->Damage()->SaveTo(outFD, false);
			CCD->CameraManager()->SaveTo(outFD);
			CCD->Level()->WeaponManager()->SaveTo(outFD);
			CCD->Level()->ElectricBolts()->SaveTo(outFD, false);
			CCD->Level()->CountDownTimers()->SaveTo(outFD, false);
			CCD->Level()->ChangeAttributes()->SaveTo(outFD, false);
			CCD->Level()->ChangeLevels()->SaveTo(outFD, false);
			CCD->Level()->ActorMaterials()->SaveTo(outFD, false);
			CCD->Level()->ModelManager()->SaveTo(outFD, false);
			CCD->SaveTo(outFD);
			fclose(outFD);
			CCD->SetSaving(true);
			break;
		}
	case RGF_K_QUICKLOAD:
		{
			FILE *inFD = CFileManager::GetSingletonPtr()->OpenRFFile(kSavegameFile, "savegame.rgf", "rb");
			if(inFD == NULL)
			{
				CCD->Log()->Error("No savegame.rgf file to restore");
				break;
			}

			// :TODO: show Load Game message (something like CCD->GUIManager()->ShowWindow(...))

			/*geRect 	 cameraRect;
			geCamera *camera;

			cameraRect.Left = 0;
			cameraRect.Right = CCD->Engine()->Width() - 1;
			cameraRect.Top = 0;
			cameraRect.Bottom = CCD->Engine()->Height() - 1;
			camera = geCamera_Create(2.0f, &cameraRect);

			if(CCD->GetHasFocus())
			{
				geEngine_BeginFrame(CCD->Engine()->Engine(), camera, GE_FALSE);

				int width = CCD->MenuManager()->FontWidth(CCD->MenuManager()->GetLoadFont(), CCD->MenuManager()->GetLoadmsg());
				int height = CCD->MenuManager()->FontHeight(CCD->MenuManager()->GetLoadFont());
				int xoffset = (CCD->Engine()->Width() - width) / 2;
				int yoffset = (CCD->Engine()->Height() - height) / 2;

				CCD->MenuManager()->FontRect(CCD->MenuManager()->GetLoadmsg(), CCD->MenuManager()->GetLoadFont(), xoffset, yoffset);

				geEngine_EndFrame(CCD->Engine()->Engine());

				//geEngine_BeginFrame(CCD->Engine()->Engine(), camera, GE_FALSE);
				//CCD->MenuManager()->FontRect(CCD->MenuManager()->GetLoadmsg(), CCD->MenuManager()->GetLoadFont(), xoffset, yoffset);
				//geEngine_EndFrame(CCD->Engine()->Engine());
			}

			geCamera_Destroy(&camera);
			*/

			// Restoring will do a level change to the level that
			// ..is in the savegame file
			CCD->Level()->Shutdown();

			if(CCD->CDPlayer())
				CCD->CDPlayer()->Stop();

			if(CCD->MIDIPlayer())
				CCD->MIDIPlayer()->Stop();

			CCD->Engine()->RestoreFrom(inFD);
			CCD->MenuManager()->RestoreFrom(inFD, false);
			CCD->Level()->Initialize(CCD->Engine()->LevelName());
			CCD->TerrainManager()->Init();
			CCD->Player()->RestoreFrom(inFD);
			CCD->TerrainManager()->Frame();
			CCD->Level()->Doors()->RestoreFrom(inFD, false);
			CCD->Level()->Platforms()->RestoreFrom(inFD, false);
			CCD->Level()->Props()->RestoreFrom(inFD, false);
			CCD->Level()->Meshes()->RestoreFrom(inFD, false);
			CCD->Level()->Teleporters()->RestoreFrom(inFD, false);
			CCD->Level()->MorphingFields()->RestoreFrom(inFD);
			CCD->MIDIPlayer()->RestoreFrom(inFD);
			CCD->CDPlayer()->RestoreFrom(inFD);
			CCD->Level()->Triggers()->RestoreFrom(inFD, false);
			CCD->Level()->LogicGates()->RestoreFrom(inFD, false);
			CCD->Level()->Attributes()->RestoreFrom(inFD, false);
			CCD->Level()->Damage()->RestoreFrom(inFD, false);
			CCD->CameraManager()->RestoreFrom(inFD);
			CCD->Level()->WeaponManager()->RestoreFrom(inFD);
			CCD->Level()->ElectricBolts()->RestoreFrom(inFD, false);
			CCD->Level()->CountDownTimers()->RestoreFrom(inFD, false);
			CCD->Level()->ChangeAttributes()->RestoreFrom(inFD, false);
			CCD->Level()->ChangeLevels()->RestoreFrom(inFD, false);
			CCD->Level()->ActorMaterials()->RestoreFrom(inFD, false);
			CCD->Level()->ModelManager()->RestoreFrom(inFD, false);
			CCD->RestoreFrom(inFD);

			fclose(inFD);

			break;
		}
	}
}


void CPlayState::ProcessReleasedAction(int action)
{
	if(CCD->GetPaused() || CCD->GetKeyPaused())
		return;

	if(CCD->Player()->GetDying() || !CCD->Player()->GetAlive())
		return;

	switch(action)
	{
	case RGF_K_FORWARD:
		{
			if(CCD->Player()->GetMoving() == MOVEWALKFORWARD)
			{
				CCD->Player()->Moving(CCD->Player()->GetSlideWalk());
				CCD->Player()->SetSlideWalk(MOVEIDLE);
			}

			if(CCD->Player()->GetSlideWalk() == MOVEWALKFORWARD)
			{
				CCD->Player()->SetSlideWalk(MOVEIDLE);
			}

			break;
		}
	case RGF_K_BACKWARD:
		{
			if(CCD->Player()->GetMoving() == MOVEWALKBACKWARD)
			{
				CCD->Player()->Moving(CCD->Player()->GetSlideWalk());
				CCD->Player()->SetSlideWalk(MOVEIDLE);
			}

			if(CCD->Player()->GetSlideWalk() == MOVEWALKBACKWARD)
			{
				CCD->Player()->SetSlideWalk(MOVEIDLE);
			}

			break;
		}
	case RGF_K_CAMERA:
		{
			m_CameraMode = false;
			break;
		}
	case RGF_K_LOOKMODE:
		{
			if(!CCD->Player()->FirstPersonView())
				CCD->Player()->LookMode(false);
			break;
		}
	case RGF_K_INVENTORY:
		{
			sxInventory::GetSingletonPtr()->Show();
			break;
		}
	case RGF_K_JUMP:
		{
			CCD->Player()->SetJumping(GE_FALSE);
			break;
		}
	case RGF_K_LEFT:
		{
			if(CCD->Player()->GetMoving() == MOVESLIDELEFT)
			{
				CCD->Player()->Moving(CCD->Player()->GetSlideWalk());
				CCD->Player()->SetSlideWalk(MOVEIDLE);
			}

			if(CCD->Player()->GetSlideWalk() == MOVESLIDELEFT)
			{
				CCD->Player()->SetSlideWalk(MOVEIDLE);
			}

			break;
		}
	case RGF_K_RIGHT:
		{
			if(CCD->Player()->GetMoving() == MOVESLIDERIGHT)
			{
				CCD->Player()->Moving(CCD->Player()->GetSlideWalk());
				CCD->Player()->SetSlideWalk(MOVEIDLE);
			}

			if(CCD->Player()->GetSlideWalk() == MOVESLIDERIGHT)
			{
				CCD->Player()->SetSlideWalk(MOVEIDLE);
			}

			break;
		}
	case RGF_K_LOOKUP:
		{
			m_KeyRotation &= ~1;
			break;
		}
	case RGF_K_TURN_LEFT:
		{
			m_KeyRotation &= ~2;
			break;
		}
	case RGF_K_TURN_RIGHT:
		{
			m_KeyRotation &= ~4;
			break;
		}
	case RGF_K_LOOKDOWN:
		{
			m_KeyRotation &= ~8;
			break;
		}
	case RGF_K_ZOOM_WEAPON:
		{
			CCD->CameraManager()->SetZoom(false);
			break;
		}
	// :TODO:
	/*case RGF_K_POWERUP:
		theLiftBelt->ChangeLift(true);
		break;
	case RGF_K_POWERDWN:
		theLiftBelt->ChangeLift(false);
		break;
		*/
	case RGF_K_FIRE:
	case RGF_K_ALTFIRE:
		{
			m_Attacking = 0;
			break;
		}
	case RGF_K_ZOOM_OUT:
	case RGF_K_ZOOM_IN:
		{
			m_Zooming = 0;
			break;
		}
	case RGF_K_QUICKSAVE:
		{
			/*save = true;
			if(!m_SaveKey)
			{
				FILE *outFD = CFileManager::GetSingletonPtr()->OpenRFFile(kSavegameFile, "savegame.rgf", "wb");
				if(outFD == NULL)
				{
					Log()->Error("Failed to create savegame file!");
					break;
				}
				m_GameEngine->SaveTo(outFD);
				theMenu->SaveTo(outFD, false);
				thePlayer->SaveTo(outFD);
				theAutoDoors->SaveTo(outFD, false);
				thePlatforms->SaveTo(outFD, false);
				theProps->SaveTo(outFD, false);
				theMeshes->SaveTo(outFD, false);
				theTeleports->SaveTo(outFD, false);
				theFields->SaveTo(outFD);
				theMIDIPlayer->SaveTo(outFD);
				theCDPlayer->SaveTo(outFD);
				theTriggers->SaveTo(outFD, false);
				theLogic->SaveTo(outFD, false);
				theAttribute->SaveTo(outFD, false);
				theDamage->SaveTo(outFD, false);
				theCameraManager->SaveTo(outFD);
				theWeapon->SaveTo(outFD);
				theElectric->SaveTo(outFD, false);
				theCountDownTimer->SaveTo(outFD, false);
				theChangeAttribute->SaveTo(outFD, false);
				theChangeLevel->SaveTo(outFD, false);
				theActMaterial->SaveTo(outFD, false);
				theModelManager->SaveTo(outFD, false);
				SaveTo(outFD);
				fclose(outFD);
				m_SaveKey = true;
				m_Saving = true;
			}*/
			//m_SaveKey = false;
			break;
		}
	case RGF_K_QUICKLOAD:
		{
			/*load = true;
			if(!m_LoadKey)
			{
				FILE *inFD = CFileManager::GetSingletonPtr()->OpenRFFile(kSavegameFile, "savegame.rgf", "rb");
				if(inFD == NULL)
				{
					Log()->Error("No savegame.rgf file to restore");
					break;
				}

				geRect 	 M_CameraRect;
				geCamera *M_Camera;

				M_CameraRect.Left = 0;
				M_CameraRect.Right = CCD->Engine()->Width() - 1;
				M_CameraRect.Top = 0;
				M_CameraRect.Bottom = CCD->Engine()->Height() - 1;
				M_Camera = geCamera_Create(2.0f, &M_CameraRect);

				if(m_HasFocus)
				{
					geEngine_BeginFrame(CCD->Engine()->Engine(), M_Camera, GE_FALSE);
					int width = CCD->MenuManager()->FontWidth(CCD->MenuManager()->GetLoadFont(), CCD->MenuManager()->GetLoadmsg());
					int height = CCD->MenuManager()->FontHeight(CCD->MenuManager()->GetLoadFont());
					int xoffset = (CCD->Engine()->Width()-width)/2;
					int yoffset = (CCD->Engine()->Height()-height)/2;
					CCD->MenuManager()->FontRect(CCD->MenuManager()->GetLoadmsg(), CCD->MenuManager()->GetLoadFont(), xoffset, yoffset);
					geEngine_EndFrame(CCD->Engine()->Engine());
					geEngine_BeginFrame(CCD->Engine()->Engine(), M_Camera, GE_FALSE);
					CCD->MenuManager()->FontRect(CCD->MenuManager()->GetLoadmsg(), CCD->MenuManager()->GetLoadFont(), xoffset, yoffset);
					geEngine_EndFrame(CCD->Engine()->Engine());
				}

				geCamera_Destroy(&M_Camera);
				// Restoring will do a level change to the level that
				// ..is in the savegame file
				CCD->Level()->Shutdown();

				if(theCDPlayer)
					theCDPlayer->Stop();

				if(theMIDIPlayer)
					theMIDIPlayer->Stop();

				m_GameEngine->RestoreFrom(inFD);
				theMenu->RestoreFrom(inFD, false);
				InitializeLevel(m_GameEngine->LevelName());
				TerrainMgr()->Init();
				thePlayer->RestoreFrom(inFD);
				theTerrainMgr->Frame();
				theAutoDoors->RestoreFrom(inFD, false);
				thePlatforms->RestoreFrom(inFD, false);
				theProps->RestoreFrom(inFD, false);
				theMeshes->RestoreFrom(inFD, false);
				theTeleports->RestoreFrom(inFD, false);
				theFields->RestoreFrom(inFD);
				theMIDIPlayer->RestoreFrom(inFD);
				theCDPlayer->RestoreFrom(inFD);
				theTriggers->RestoreFrom(inFD, false);
				theLogic->RestoreFrom(inFD, false);
				theAttribute->RestoreFrom(inFD, false);
				theDamage->RestoreFrom(inFD, false);
				theCameraManager->RestoreFrom(inFD);
				theWeapon->RestoreFrom(inFD);
				theElectric->RestoreFrom(inFD, false);
				theCountDownTimer->RestoreFrom(inFD, false);
				theChangeAttribute->RestoreFrom(inFD, false);
				theChangeLevel->RestoreFrom(inFD, false);
				theActMaterial->RestoreFrom(inFD, false);
				theModelManager->RestoreFrom(inFD, false);
				RestoreFrom(inFD);
				fclose(inFD);
				m_LoadKey = true;
			}*/
			//m_LoadKey = false;
			break;
		}
	}
}


void CPlayState::keyPressed(const OIS::KeyEvent &e)
{
	if(CCD->Player()->GetDying())
	{
		if(CCD->ActorManager()->EndAnimation(CCD->Player()->GetActor()))
		{
			if(e.key == OIS::KC_ESCAPE)
			{
				if(!CCD->Player()->GetContinue())
				{
					CCD->Player()->SetMonitorState(true);
				}
				else
				{
					CCD->Player()->SetAlive(false);
					CCD->Player()->SetDying(false);
				}
			}
			else if(e.key == OIS::KC_SPACE)
			{
				CCD->Player()->SetDeathSpace(true);
			}
		}
		return;
	}

	ProcessPressedAction(CCD->Input()->GetKeyAction(e.key));
}


void CPlayState::keyReleased(const OIS::KeyEvent &e)
{
	ProcessReleasedAction(CCD->Input()->GetKeyAction(e.key));
}


void CPlayState::mouseMoved(const OIS::MouseEvent &e)
{
	if(m_ShowCursor)
	{
		POINT tempPos;
		CCD->Input()->GetMousePos(&tempPos.x, &tempPos.y);
		CCD->GUIManager()->MousePosition(static_cast<float>(tempPos.x), static_cast<float>(tempPos.y));

		// update only if position has change, otherwise hovering of thumbs (slider, scrollbar) won't work
		if(e.state.Z.rel != 0)
			CCD->GUIManager()->MouseWheelChange(static_cast<float>(e.state.Z.rel)/120.f);
	}
}


void CPlayState::mousePressed(const OIS::MouseEvent &/*e*/, OIS::MouseButtonID id)
{
	if(!CCD->GUIManager()->MouseButtonDown(id))
	{
		ProcessPressedAction(CCD->Input()->GetMouseAction(id));
	}
}


void CPlayState::mouseReleased(const OIS::MouseEvent &/*e*/, OIS::MouseButtonID id)
{
	if(!CCD->GUIManager()->MouseButtonUp(id))
	{
		ProcessReleasedAction(CCD->Input()->GetMouseAction(id));
	}
}


CPlayState* CPlayState::GetSingletonPtr()
{
	if(!m_PlayState)
	{
		m_PlayState = new CPlayState();
	}

	return m_PlayState;
}
