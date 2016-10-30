/************************************************************************************//**
 * @file CInputAction.cpp
 * @brief Implementation for RFSX Input Action object.
 * @author Daniel Queteschiner
 * @date January 2014
 *//*
 * Copyright (c) 2014 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "RGFStatus.h"
#include "CInputAction.h"

#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE sxAction

namespace RFSX
{

//////////////////////////////////////////////////////////////////////////////////////////
// sxAction implementation
//////////////////////////////////////////////////////////////////////////////////////////

SX_IMPL_TYPE::SX_IMPL_TYPE()
{
	InitActionHT();
}


SX_IMPL_TYPE::~SX_IMPL_TYPE()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxAction fields
//////////////////////////////////////////////////////////////////////////////////////////

bool SX_IMPL_TYPE::getValue(const skString& name, const skString& /*att*/, skRValue& val)
{
	int *intValue = h_Action.value(name);
	if(!intValue)
		return false;

	val = *intValue;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// sxAction initialization
//////////////////////////////////////////////////////////////////////////////////////////

void SX_IMPL_TYPE::InitActionHT()
{
	h_Action.insertKeyAndValue("NoAction",		new int(RGF_K_NOACTION));
	h_Action.insertKeyAndValue("MoveForward",	new int(RGF_K_FORWARD));
	h_Action.insertKeyAndValue("MoveBackward",	new int(RGF_K_BACKWARD));
	h_Action.insertKeyAndValue("TurnLeft",		new int(RGF_K_TURN_LEFT));
	h_Action.insertKeyAndValue("TurnRight",		new int(RGF_K_TURN_RIGHT));
	h_Action.insertKeyAndValue("StrafeLeft",	new int(RGF_K_LEFT));
	h_Action.insertKeyAndValue("StrafeRight",	new int(RGF_K_RIGHT));
	h_Action.insertKeyAndValue("Jump",			new int(RGF_K_JUMP));
	h_Action.insertKeyAndValue("Crouch",		new int(RGF_K_CROUCH));
	h_Action.insertKeyAndValue("LookUp",		new int(RGF_K_LOOKUP));
	h_Action.insertKeyAndValue("LookDown",		new int(RGF_K_LOOKDOWN));
	h_Action.insertKeyAndValue("LookStraight",	new int(RGF_K_LOOKSTRAIGHT));
	h_Action.insertKeyAndValue("Attack",		new int(RGF_K_FIRE));
	h_Action.insertKeyAndValue("AltAttack",		new int(RGF_K_ALTFIRE));
	h_Action.insertKeyAndValue("Weapon0",		new int(RGF_K_WEAPON_0));
	h_Action.insertKeyAndValue("Weapon1",		new int(RGF_K_WEAPON_1));
	h_Action.insertKeyAndValue("Weapon2",		new int(RGF_K_WEAPON_2));
	h_Action.insertKeyAndValue("Weapon3",		new int(RGF_K_WEAPON_3));
	h_Action.insertKeyAndValue("Weapon4",		new int(RGF_K_WEAPON_4));
	h_Action.insertKeyAndValue("Weapon5",		new int(RGF_K_WEAPON_5));
	h_Action.insertKeyAndValue("Weapon6",		new int(RGF_K_WEAPON_6));
	h_Action.insertKeyAndValue("Weapon7",		new int(RGF_K_WEAPON_7));
	h_Action.insertKeyAndValue("Weapon8",		new int(RGF_K_WEAPON_8));
	h_Action.insertKeyAndValue("Weapon9",		new int(RGF_K_WEAPON_9));
	h_Action.insertKeyAndValue("Run",			new int(RGF_K_RUN));
	h_Action.insertKeyAndValue("CameraMode",	new int(RGF_K_CAMERA));
	h_Action.insertKeyAndValue("ZoomIn",		new int(RGF_K_ZOOM_IN));
	h_Action.insertKeyAndValue("ZoomOut",		new int(RGF_K_ZOOM_OUT));
	h_Action.insertKeyAndValue("CameraReset",	new int(RGF_K_CAMERA_RESET));
	h_Action.insertKeyAndValue("Exit",			new int(RGF_K_EXIT));
	h_Action.insertKeyAndValue("QuickSave",		new int(RGF_K_QUICKSAVE));
	h_Action.insertKeyAndValue("QuickLoad",		new int(RGF_K_QUICKLOAD));
	h_Action.insertKeyAndValue("1stPerson",		new int(RGF_K_FIRST_PERSON_VIEW));
	h_Action.insertKeyAndValue("3rdPerson",		new int(RGF_K_THIRD_PERSON_VIEW));
	h_Action.insertKeyAndValue("Isometric",		new int(RGF_K_ISO_VIEW));
	h_Action.insertKeyAndValue("Skip",			new int(RGF_K_SKIP));
	h_Action.insertKeyAndValue("Help",			new int(RGF_K_HELP));
	h_Action.insertKeyAndValue("HUD",			new int(RGF_K_HUD));
	h_Action.insertKeyAndValue("LookMode",		new int(RGF_K_LOOKMODE));
	h_Action.insertKeyAndValue("Screenshot",	new int(RGF_K_SCRNSHOT));
	h_Action.insertKeyAndValue("ZoomWeapon",	new int(RGF_K_ZOOM_WEAPON));
	h_Action.insertKeyAndValue("HolsterWeapon",	new int(RGF_K_HOLSTER_WEAPON));
	h_Action.insertKeyAndValue("Light",			new int(RGF_K_LIGHT));
	h_Action.insertKeyAndValue("UseItem",		new int(RGF_K_USE));
	h_Action.insertKeyAndValue("Inventory",		new int(RGF_K_INVENTORY));
	h_Action.insertKeyAndValue("Console",		new int(RGF_K_CONSOLE));
	h_Action.insertKeyAndValue("DropWeapon",	new int(RGF_K_DROP));
	h_Action.insertKeyAndValue("ReloadWeapon",	new int(RGF_K_RELOAD));
	h_Action.insertKeyAndValue("PowerUp",		new int(RGF_K_POWERUP));
	h_Action.insertKeyAndValue("PowerDown",		new int(RGF_K_POWERDWN));
}

} // namespace RFSX

#undef SX_IMPL_TYPE // And that's it in a nutshell
