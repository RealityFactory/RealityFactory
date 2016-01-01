/************************************************************************************//**
 * @file ScriptManager.cpp
 * @brief ScriptManager class
 *
 * This file contains the implementation of the ScriptManager class.
 * @author Daniel Queteschiner
 * @date March 2010
 *//*
 * Copyright (c) 2010 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "Simkin\\skInterpreter.h"
#include "RGFScriptMethods.h"
#include "CScriptManager.h"
#include "CLog.h"


skInterpreter ScriptManager::Interpreter;
skTSHashTable<long> ScriptManager::h_method(RGF_SM_MAXMETHODS);


skExecutableContext ScriptManager::GetContext()
{
	return skExecutableContext(&Interpreter);
}


void ScriptManager::AddGlobalVariable(const std::string& name, skRValue value)
{
	Interpreter.addGlobalVariable(name.c_str(), value);
}


void ScriptManager::RemoveGlobalVariable(const std::string& name)
{
	Interpreter.removeGlobalVariable(name.c_str());
}


void ScriptManager::ParmCheck(int entries, int desired, const std::string& order,
					  const std::string& script, const skString &method)
{
	if(entries >= desired)
		return;

	sxLog::GetSingletonPtr()->Critical(
		"Incorrect # of parameters in method '%s' in script '%s' order '%s'",
		method.c_str(), script.c_str(), order.c_str());
}


long ScriptManager::GetHashMethod(const skString& name)
{
	long *value = h_method.value(name);

	if(value)
		return *value;

	return 0;
}


void ScriptManager::InitMHT()
{
	if(h_method.entries())
		return;

	// low methods
	h_method.insertKeyAndValue("HighLevel",				new long(RGF_SM_HIGHLEVEL));
	h_method.insertKeyAndValue("Animate",				new long(RGF_SM_ANIMATE));
	h_method.insertKeyAndValue("Gravity",				new long(RGF_SM_GRAVITY));
	h_method.insertKeyAndValue("PlaySound",				new long(RGF_SM_PLAYSOUND));
	h_method.insertKeyAndValue("Speak",					new long(RGF_SM_SPEAK));
	h_method.insertKeyAndValue("EnemyExist",			new long(RGF_SM_ENEMYEXIST));
	h_method.insertKeyAndValue("GetEventState",			new long(RGF_SM_GETEVENTSTATE));
	h_method.insertKeyAndValue("Integer",				new long(RGF_SM_INTEGER));
	h_method.insertKeyAndValue("GetAttribute",			new long(RGF_SM_GETATTRIBUTE));
	h_method.insertKeyAndValue("GetCurFlipBook",		new long(RGF_SM_GETCURFLIPBOOK));
	h_method.insertKeyAndValue("ModifyAttribute",		new long(RGF_SM_MODIFYATTRIBUTE));
	h_method.insertKeyAndValue("SetAttribute",			new long(RGF_SM_SETATTRIBUTE));
	h_method.insertKeyAndValue("SetPlayerWeapon",		new long(RGF_SM_SETPLAYERWEAPON));
	h_method.insertKeyAndValue("SetUseItem",			new long(RGF_SM_SETUSEITEM));
	h_method.insertKeyAndValue("ClearUseItem",			new long(RGF_SM_CLEARUSEITEM));
	h_method.insertKeyAndValue("StringCopy",			new long(RGF_SM_STRINGCOPY));
	h_method.insertKeyAndValue("LeftCopy",				new long(RGF_SM_LEFTCOPY));
	h_method.insertKeyAndValue("IsEntityVsible",		new long(RGF_SM_ISENTITYVSIBLE));
	h_method.insertKeyAndValue("DamageEntity",			new long(RGF_SM_DAMAGEENTITY));
	h_method.insertKeyAndValue("AnimateEntity",			new long(RGF_SM_ANIMATEENTITY));
	h_method.insertKeyAndValue("AnimateHold",			new long(RGF_SM_ANIMATEHOLD));
	h_method.insertKeyAndValue("AnimateTarget",			new long(RGF_SM_ANIMATETARGET));
	h_method.insertKeyAndValue("GetEntityX",			new long(RGF_SM_GETENTITYX));
	h_method.insertKeyAndValue("GetEntityY",			new long(RGF_SM_GETENTITYY));
	h_method.insertKeyAndValue("GetEntityZ",			new long(RGF_SM_GETENTITYZ));
	h_method.insertKeyAndValue("IsKeyDown",				new long(RGF_SM_ISKEYDOWN));
	h_method.insertKeyAndValue("GetEntityYaw",			new long(RGF_SM_GETENTITYYAW));
	h_method.insertKeyAndValue("MatchEntityAngles",		new long(RGF_SM_MATCHENTITYANGLES));
	h_method.insertKeyAndValue("FacePoint",				new long(RGF_SM_FACEPOINT));
	h_method.insertKeyAndValue("NewPoint",				new long(RGF_SM_NEWPOINT));
	h_method.insertKeyAndValue("GetPointYaw",			new long(RGF_SM_GETPOINTYAW));
	h_method.insertKeyAndValue("NextPoint",				new long(RGF_SM_NEXTPOINT));
	h_method.insertKeyAndValue("SetTarget",				new long(RGF_SM_SETTARGET));
	h_method.insertKeyAndValue("GetDistanceTo",			new long(RGF_SM_GETDISTANCETO));
	h_method.insertKeyAndValue("TeleportEntity",		new long(RGF_SM_TELEPORTENTITY));
	h_method.insertKeyAndValue("SaveAttributes",		new long(RGF_SM_SAVEATTRIBUTES));
	h_method.insertKeyAndValue("TraceToActor",			new long(RGF_SM_TRACETOACTOR));
	h_method.insertKeyAndValue("AnimateBlend",			new long(RGF_SM_ANIMATEBLEND));
	h_method.insertKeyAndValue("AnimationSpeed",		new long(RGF_SM_ANIMATIONSPEED));
	h_method.insertKeyAndValue("SetCollision",			new long(RGF_SM_SETCOLLISION));
	h_method.insertKeyAndValue("SetNoCollision",		new long(RGF_SM_SETNOCOLLISION));
	h_method.insertKeyAndValue("DamageArea",			new long(RGF_SM_DAMAGEAREA));
	h_method.insertKeyAndValue("PlayerMatchAngles",		new long(RGF_SM_PLAYERMATCHANGLES));
	h_method.insertKeyAndValue("ConvertDegrees",		new long(RGF_SM_CONVERTDEGREES));
	h_method.insertKeyAndValue("AttachCamera",			new long(RGF_SM_ATTACHCAMERA));
	h_method.insertKeyAndValue("AttachCameraToBone",	new long(RGF_SM_ATTACHCAMERATOBONE));
	h_method.insertKeyAndValue("AttachCameraToEntity",	new long(RGF_SM_ATTACHCAMERATOENTITY));
	h_method.insertKeyAndValue("DetachCamera",			new long(RGF_SM_DETACHCAMERA));
	h_method.insertKeyAndValue("TiltCamera",			new long(RGF_SM_TILTCAMERA));
	h_method.insertKeyAndValue("PositionToPlatform",	new long(RGF_SM_POSITIONTOPLATFORM));
	h_method.insertKeyAndValue("ActivateTrigger",		new long(RGF_SM_ACTIVATETRIGGER));
	h_method.insertKeyAndValue("UpdateEnemyVis",		new long(RGF_SM_UPDATEENEMYVIS));
	h_method.insertKeyAndValue("TargetPlayer",			new long(RGF_SM_TARGETPLAYER));
	h_method.insertKeyAndValue("FireProjectileBlind",	new long(RGF_SM_FIREPROJECTILEBLIND));
	h_method.insertKeyAndValue("SetTargetPoint",		new long(RGF_SM_SETTARGETPOINT));
	h_method.insertKeyAndValue("GetBoneX",				new long(RGF_SM_GETBONEX));
	h_method.insertKeyAndValue("GetBoneY",				new long(RGF_SM_GETBONEY));
	h_method.insertKeyAndValue("GetBoneZ",				new long(RGF_SM_GETBONEZ));
	h_method.insertKeyAndValue("GetBoneYaw",			new long(RGF_SM_GETBONEYAW));
	h_method.insertKeyAndValue("SetPosition",			new long(RGF_SM_SETPOSITION));
	h_method.insertKeyAndValue("IsButtonDown",			new long(RGF_SM_ISBUTTONDOWN));
	h_method.insertKeyAndValue("GetJoyAxisX",			new long(RGF_SM_GETJOYAXISX));
	h_method.insertKeyAndValue("GetJoyAxisY",			new long(RGF_SM_GETJOYAXISY));
	h_method.insertKeyAndValue("GetJoyAxisZ",			new long(RGF_SM_GETJOYAXISZ));
	h_method.insertKeyAndValue("GetNumJoysticks",		new long(RGF_SM_GETNUMJOYSTICKS));
	h_method.insertKeyAndValue("SetBoundingBox",		new long(RGF_SM_SETBOUNDINGBOX));
	h_method.insertKeyAndValue("GetBoneToBone",			new long(RGF_SM_GETBONETOBONE));
	h_method.insertKeyAndValue("SwitchView",			new long(RGF_SM_SWITCHVIEW));
	h_method.insertKeyAndValue("ForceEntityUp",			new long(RGF_SM_FORCEENTITYUP));
	h_method.insertKeyAndValue("ForceEntityDown",		new long(RGF_SM_FORCEENTITYDOWN));
	h_method.insertKeyAndValue("ForceEntityRight",		new long(RGF_SM_FORCEENTITYRIGHT));
	h_method.insertKeyAndValue("ForceEntityLeft",		new long(RGF_SM_FORCEENTITYLEFT));
	h_method.insertKeyAndValue("ForceEntityForward",	new long(RGF_SM_FORCEENTITYFORWARD));
	h_method.insertKeyAndValue("ForceEntityBackward",	new long(RGF_SM_FORCEENTITYBACKWARD));
	h_method.insertKeyAndValue("GetGroundTexture",		new long(RGF_SM_GETGROUNDTEXTURE));
	h_method.insertKeyAndValue("GetPlayerGroundTexture",new long(RGF_SM_GETPLAYERGROUNDTEXTURE));
	h_method.insertKeyAndValue("PositionToBone",		new long(RGF_SM_POSITIONTOBONE));
	h_method.insertKeyAndValue("SetWeaponMatFromFlip",	new long(RGF_SM_SETWEAPONMATFROMFLIP));
	h_method.insertKeyAndValue("SetShadowFromFlip",		new long(RGF_SM_SETSHADOWFROMFLIP));
	h_method.insertKeyAndValue("GetCollideDistance",	new long(RGF_SM_GETCOLLIDEDISTANCE));
	h_method.insertKeyAndValue("ResetAnimate",			new long(RGF_SM_RESETANIMATE));
	h_method.insertKeyAndValue("WhereIsPlayer",			new long(RGF_SM_WHEREISPLAYER));
	h_method.insertKeyAndValue("WhereIsEntity",			new long(RGF_SM_WHEREISENTITY));
	h_method.insertKeyAndValue("InsertEvent",			new long(RGF_SM_INSERTEVENT));
	h_method.insertKeyAndValue("CheckForEvent",			new long(RGF_SM_CHECKFOREVENT));
	h_method.insertKeyAndValue("PlayEventSound",		new long(RGF_SM_PLAYEVENTSOUND));
	h_method.insertKeyAndValue("LoadAnimation",			new long(RGF_SM_LOADANIMATION));
	h_method.insertKeyAndValue("StartSoundTrack",		new long(RGF_SM_STARTSOUNDTRACK));
	h_method.insertKeyAndValue("StopAllAudioStreams",	new long(RGF_SM_STOPALLAUDIOSTREAMS));
	h_method.insertKeyAndValue("ChangeYaw",				new long(RGF_SM_CHANGEYAW));
	h_method.insertKeyAndValue("ChangePitch",			new long(RGF_SM_CHANGEPITCH));
	h_method.insertKeyAndValue("random",				new long(RGF_SM_RANDOM));
	h_method.insertKeyAndValue("walkmove",				new long(RGF_SM_WALKMOVE));
	h_method.insertKeyAndValue("flymove",				new long(RGF_SM_FLYMOVE));
	h_method.insertKeyAndValue("Damage",				new long(RGF_SM_DAMAGE));
	h_method.insertKeyAndValue("DamagePlayer",			new long(RGF_SM_DAMAGEPLAYER));
	h_method.insertKeyAndValue("PositionToPlayer",		new long(RGF_SM_POSITIONTOPLAYER));
	h_method.insertKeyAndValue("PlayerToPosition",		new long(RGF_SM_PLAYERTOPOSITION));
	h_method.insertKeyAndValue("PositionToPawn",		new long(RGF_SM_POSITIONTOPAWN));
	h_method.insertKeyAndValue("SetKeyPause",			new long(RGF_SM_SETKEYPAUSE));
	h_method.insertKeyAndValue("PlayerRender",			new long(RGF_SM_PLAYERRENDER));
	h_method.insertKeyAndValue("PawnRender",			new long(RGF_SM_PAWNRENDER));
	h_method.insertKeyAndValue("Render",				new long(RGF_SM_RENDER));
	h_method.insertKeyAndValue("ChangeMaterial",		new long(RGF_SM_CHANGEMATERIAL));
	h_method.insertKeyAndValue("SetHoldAtEnd",			new long(RGF_SM_SETHOLDATEND));
	h_method.insertKeyAndValue("ForceUp",				new long(RGF_SM_FORCEUP));
	h_method.insertKeyAndValue("ForceDown",				new long(RGF_SM_FORCEDOWN));
	h_method.insertKeyAndValue("ForceRight",			new long(RGF_SM_FORCERIGHT));
	h_method.insertKeyAndValue("ForceLeft",				new long(RGF_SM_FORCELEFT));
	h_method.insertKeyAndValue("ForceForward",			new long(RGF_SM_FORCEFORWARD));
	h_method.insertKeyAndValue("ForceBackward",			new long(RGF_SM_FORCEBACKWARD));
	h_method.insertKeyAndValue("UpdateTarget",			new long(RGF_SM_UPDATETARGET));
	h_method.insertKeyAndValue("FireProjectile",		new long(RGF_SM_FIREPROJECTILE));
	h_method.insertKeyAndValue("AddExplosion",			new long(RGF_SM_ADDEXPLOSION));
	h_method.insertKeyAndValue("GetLastBoneHit",		new long(RGF_SM_GETLASTBONEHIT));
	h_method.insertKeyAndValue("debug",					new long(RGF_SM_DEBUG));
	h_method.insertKeyAndValue("SetEventState",			new long(RGF_SM_SETEVENTSTATE));
	h_method.insertKeyAndValue("GetStringLength",		new long(RGF_SM_GETSTRINGLENGTH));
	h_method.insertKeyAndValue("DrawText",				new long(RGF_SM_DRAWTEXT));
	//h_method.insertKeyAndValue("DrawFlipBookImage",	new long(RGF_SM_DRAWFLIPBOOKIMAGE));
	h_method.insertKeyAndValue("DrawPolyShadow",		new long(RGF_SM_DRAWPOLYSHADOW));
	h_method.insertKeyAndValue("MatchPlayerAngles",		new long(RGF_SM_MATCHPLAYERANGLES));
	h_method.insertKeyAndValue("DamageAtBone",			new long(RGF_SM_DAMAGEATBONE));
	h_method.insertKeyAndValue("SaveActor",				new long(RGF_SM_SAVEACTOR));
	h_method.insertKeyAndValue("LookAtPawn",			new long(RGF_SM_LOOKATPAWN));
	h_method.insertKeyAndValue("AutoWalk",				new long(RGF_SM_AUTOWALK));
	h_method.insertKeyAndValue("FastDistance",			new long(RGF_SM_FASTDISTANCE));
	h_method.insertKeyAndValue("StepHeight",			new long(RGF_SM_STEPHEIGHT));
	h_method.insertKeyAndValue("DrawVPoly",				new long(RGF_SM_DRAWVPOLY));
	h_method.insertKeyAndValue("DrawHPoly",				new long(RGF_SM_DRAWHPOLY));
	h_method.insertKeyAndValue("GetPitchToPoint",		new long(RGF_SM_GETPITCHTOPOINT));
	h_method.insertKeyAndValue("GetYawToPoint",			new long(RGF_SM_GETYAWTOPOINT));
	h_method.insertKeyAndValue("FastPointCheck",		new long(RGF_SM_FASTPOINTCHECK));
	h_method.insertKeyAndValue("SetCameraWindow",		new long(RGF_SM_SETCAMERAWINDOW));
	h_method.insertKeyAndValue("SetFixedCameraPosition",new long(RGF_SM_SETFIXEDCAMERAPOSITION));
	h_method.insertKeyAndValue("SetFixedCameraRotation",new long(RGF_SM_SETFIXEDCAMERAROTATION));
	h_method.insertKeyAndValue("SetFixedCameraFOV",		new long(RGF_SM_SETFIXEDCAMERAFOV));
	h_method.insertKeyAndValue("MoveFixedCamera",		new long(RGF_SM_MOVEFIXEDCAMERA));
	h_method.insertKeyAndValue("RotateFixedCamera",		new long(RGF_SM_ROTATEFIXEDCAMERA));
	h_method.insertKeyAndValue("DistanceBetweenEntities",new long(RGF_SM_DISTANCEBETWEENENTITIES));
	h_method.insertKeyAndValue("SetEntityProperties",	new long(RGF_SM_SETENTITYPROPERTIES));
	h_method.insertKeyAndValue("SetEntityLighting",		new long(RGF_SM_SETENTITYLIGHTING));
	h_method.insertKeyAndValue("UpdateScriptPoint",		new long(RGF_SM_UPDATESCRIPTPOINT));
	h_method.insertKeyAndValue("GetEntityScreenX",		new long(RGF_SM_GETENTITYSCREENX));
	h_method.insertKeyAndValue("GetEntityScreenY",		new long(RGF_SM_GETENTITYSCREENY));
	h_method.insertKeyAndValue("GetScreenWidth",		new long(RGF_SM_GETSCREENWIDTH));
	h_method.insertKeyAndValue("GetScreenHeight",		new long(RGF_SM_GETSCREENHEIGHT));
	h_method.insertKeyAndValue("MouseSelect",			new long(RGF_SM_MOUSESELECT));
	h_method.insertKeyAndValue("MouseControlledPlayer",	new long(RGF_SM_MOUSECONTROLLEDPLAYER));
	h_method.insertKeyAndValue("ShowMouse",				new long(RGF_SM_SHOWMOUSE));
	h_method.insertKeyAndValue("GetMousePosX",			new long(RGF_SM_GETMOUSEPOSX));
	h_method.insertKeyAndValue("GetMousePosY",			new long(RGF_SM_GETMOUSEPOSY));
	h_method.insertKeyAndValue("SetMousePos",			new long(RGF_SM_SETMOUSEPOS));
	h_method.insertKeyAndValue("SetGamma",				new long(RGF_SM_SETGAMMA));
	h_method.insertKeyAndValue("GetGamma",				new long(RGF_SM_GETGAMMA));
	h_method.insertKeyAndValue("FillScreenArea",		new long(RGF_SM_FILLSCREENAREA));
	h_method.insertKeyAndValue("RemoveScreenArea",		new long(RGF_SM_REMOVESCREENAREA));
	h_method.insertKeyAndValue("ShowText",				new long(RGF_SM_SHOWTEXT));
	h_method.insertKeyAndValue("RemoveText",			new long(RGF_SM_REMOVETEXT));
	h_method.insertKeyAndValue("ShowHudPicture",		new long(RGF_SM_SHOWHUDPICTURE));
	h_method.insertKeyAndValue("SetHudDraw",			new long(RGF_SM_SETHUDDRAW));
	h_method.insertKeyAndValue("GetHudDraw",			new long(RGF_SM_GETHUDDRAW));
	h_method.insertKeyAndValue("SetAlpha",				new long(RGF_SM_SETALPHA));
	h_method.insertKeyAndValue("GetAlpha",				new long(RGF_SM_GETALPHA));
	h_method.insertKeyAndValue("SetEntityAlpha",		new long(RGF_SM_SETENTITYALPHA));
	h_method.insertKeyAndValue("GetEntityAlpha",		new long(RGF_SM_GETENTITYALPHA));
	h_method.insertKeyAndValue("SetScale",				new long(RGF_SM_SETSCALE));
	h_method.insertKeyAndValue("SetEntityScale",		new long(RGF_SM_SETENTITYSCALE));
	h_method.insertKeyAndValue("CheckArea",				new long(RGF_SM_CHECKAREA));
	h_method.insertKeyAndValue("SetFlag",				new long(RGF_SM_SETFLAG));
	h_method.insertKeyAndValue("GetFlag",				new long(RGF_SM_GETFLAG));
	h_method.insertKeyAndValue("PowerUp",				new long(RGF_SM_POWERUP));
	h_method.insertKeyAndValue("GetPowerUpLevel",		new long(RGF_SM_GETPOWERUPLEVEL));
	h_method.insertKeyAndValue("ActivateHudElement",	new long(RGF_SM_ACTIVATEHUDELEMENT));
	h_method.insertKeyAndValue("MoveEntity",			new long(RGF_SM_MOVEENTITY));
	h_method.insertKeyAndValue("RotateEntity",			new long(RGF_SM_ROTATEENTITY));
	h_method.insertKeyAndValue("SetEntityPosition",		new long(RGF_SM_SETENTITYPOSITION));
	h_method.insertKeyAndValue("SetEntityRotation",		new long(RGF_SM_SETENTITYROTATION));
	h_method.insertKeyAndValue("AddAttribute",			new long(RGF_SM_ADDATTRIBUTE));
	h_method.insertKeyAndValue("SetAttributeValueLimits",new long(RGF_SM_SETATTRIBUTEVALUELIMITS));
	h_method.insertKeyAndValue("RightCopy",				new long(RGF_SM_RIGHTCOPY));
	h_method.insertKeyAndValue("NearestPoint",			new long(RGF_SM_NEARESTPOINT));
	h_method.insertKeyAndValue("SetFOV",				new long(RGF_SM_SETFOV));
	h_method.insertKeyAndValue("sin",					new long(RGF_SM_SIN));
	h_method.insertKeyAndValue("cos",					new long(RGF_SM_COS));
	h_method.insertKeyAndValue("tan",					new long(RGF_SM_TAN));
	h_method.insertKeyAndValue("asin",					new long(RGF_SM_ASIN));
	h_method.insertKeyAndValue("acos",					new long(RGF_SM_ACOS));
	h_method.insertKeyAndValue("atan",					new long(RGF_SM_ATAN));
	h_method.insertKeyAndValue("SetGravity",			new long(RGF_SM_SETGRAVITY));
	h_method.insertKeyAndValue("GetGravityX",			new long(RGF_SM_GETGRAVITYX));
	h_method.insertKeyAndValue("GetGravityY",			new long(RGF_SM_GETGRAVITYY));
	h_method.insertKeyAndValue("GetGravityZ",			new long(RGF_SM_GETGRAVITYZ));
	h_method.insertKeyAndValue("SetWind",				new long(RGF_SM_SETWIND));
	h_method.insertKeyAndValue("GetWindX",				new long(RGF_SM_GETWINDX));
	h_method.insertKeyAndValue("GetWindY",				new long(RGF_SM_GETWINDY));
	h_method.insertKeyAndValue("GetWindZ",				new long(RGF_SM_GETWINDZ));
	h_method.insertKeyAndValue("SetWindBase",			new long(RGF_SM_SETWINDBASE));
	h_method.insertKeyAndValue("GetWindBaseX",			new long(RGF_SM_GETWINDBASEX));
	h_method.insertKeyAndValue("GetWindBaseY",			new long(RGF_SM_GETWINDBASEY));
	h_method.insertKeyAndValue("GetWindBaseZ",			new long(RGF_SM_GETWINDBASEZ));
	h_method.insertKeyAndValue("SetWindGenerator",		new long(RGF_SM_SETWINDGENERATOR));
	h_method.insertKeyAndValue("SetForceEnabled",		new long(RGF_SM_SETFORCEENABLED));
	h_method.insertKeyAndValue("IsInLiquid",			new long(RGF_SM_ISINLIQUID));
	h_method.insertKeyAndValue("GetLiquid",				new long(RGF_SM_GETLIQUID));
	h_method.insertKeyAndValue("PauseScript",			new long(RGF_SM_PAUSESCRIPT));
	h_method.insertKeyAndValue("ResumeScript",			new long(RGF_SM_RESUMESCRIPT));
	h_method.insertKeyAndValue("EndScript",				new long(RGF_SM_ENDSCRIPT));
	//h_method.insertKeyAndValue("SetSlowMotion",		new long(RGF_SM_SETSLOWMOTION));

	// high methods
	h_method.insertKeyAndValue("MoveToPoint",			new long(RGF_SM_MOVETOPOINT));
	h_method.insertKeyAndValue("RotateToPoint",			new long(RGF_SM_ROTATETOPOINT));
	h_method.insertKeyAndValue("NewOrder",				new long(RGF_SM_NEWORDER));
	h_method.insertKeyAndValue("NextOrder",				new long(RGF_SM_NEXTORDER));
	h_method.insertKeyAndValue("RotateToAlign",			new long(RGF_SM_ROTATETOALIGN));
	//h_method.insertKeyAndValue("NextPoint",			new long(RGF_SM_NEXTPOINT));				// same as low method
	h_method.insertKeyAndValue("Delay",					new long(RGF_SM_DELAY));
	h_method.insertKeyAndValue("PlayAnimation",			new long(RGF_SM_PLAYANIMATION));
	h_method.insertKeyAndValue("BlendToAnimation",		new long(RGF_SM_BLENDTOANIMATION));
	h_method.insertKeyAndValue("LoopAnimation",			new long(RGF_SM_LOOPANIMATION));
	h_method.insertKeyAndValue("Rotate",				new long(RGF_SM_ROTATE));
	h_method.insertKeyAndValue("RotateMoveToPoint",		new long(RGF_SM_ROTATEMOVETOPOINT));
	h_method.insertKeyAndValue("RotateMove",			new long(RGF_SM_ROTATEMOVE));
	h_method.insertKeyAndValue("NewPath",				new long(RGF_SM_NEWPATH));
	h_method.insertKeyAndValue("RestartOrder",			new long(RGF_SM_RESTARTORDER));
	h_method.insertKeyAndValue("PlayerDistOrder",		new long(RGF_SM_PLAYERDISTORDER));
	h_method.insertKeyAndValue("Console",				new long(RGF_SM_CONSOLE));
	h_method.insertKeyAndValue("AudibleRadius",			new long(RGF_SM_AUDIBLERADIUS));
	h_method.insertKeyAndValue("AddPainOrder",			new long(RGF_SM_ADDPAINORDER));
	h_method.insertKeyAndValue("FindTargetOrder",		new long(RGF_SM_FINDTARGETORDER));
	h_method.insertKeyAndValue("FindPointOrder",		new long(RGF_SM_FINDPOINTORDER));
	//h_method.insertKeyAndValue("NewPoint",			new long(RGF_SM_NEWPOINT));					// same as low method
	h_method.insertKeyAndValue("MoveForward",			new long(RGF_SM_MOVEFORWARD));
	h_method.insertKeyAndValue("MoveBackward",			new long(RGF_SM_MOVEBACKWARD));
	h_method.insertKeyAndValue("MoveLeft",				new long(RGF_SM_MOVELEFT));
	h_method.insertKeyAndValue("MoveRight",				new long(RGF_SM_MOVERIGHT));
	h_method.insertKeyAndValue("Move",					new long(RGF_SM_MOVE));
	h_method.insertKeyAndValue("AvoidOrder",			new long(RGF_SM_AVOIDORDER));
	h_method.insertKeyAndValue("Return",				new long(RGF_SM_RETURN));
	h_method.insertKeyAndValue("Align",					new long(RGF_SM_ALIGN));
	h_method.insertKeyAndValue("Jump",					new long(RGF_SM_JUMP));
	h_method.insertKeyAndValue("AddTriggerOrder",		new long(RGF_SM_ADDTRIGGERORDER));
	h_method.insertKeyAndValue("DelTriggerOrder",		new long(RGF_SM_DELTRIGGERORDER));
	//h_method.insertKeyAndValue("SetEventState",		new long(RGF_SM_SETEVENTSTATE));			// same as low method
	h_method.insertKeyAndValue("FacePlayer",			new long(RGF_SM_FACEPLAYER));
	h_method.insertKeyAndValue("RotateToPlayer",		new long(RGF_SM_ROTATETOPLAYER));
	h_method.insertKeyAndValue("RotateAroundPointLeft",	new long(RGF_SM_ROTATEAROUNDPOINTLEFT));
	h_method.insertKeyAndValue("RotateAroundPointRight",new long(RGF_SM_ROTATEAROUNDPOINTRIGHT));
	h_method.insertKeyAndValue("TeleportToPoint",		new long(RGF_SM_TELEPORTTOPOINT));
	//h_method.insertKeyAndValue("AnimationSpeed",		new long(RGF_SM_ANIMATIONSPEED));			// same as low method
	//h_method.insertKeyAndValue("SetFlag",				new long(RGF_SM_SETFLAG));					// same as low method
	h_method.insertKeyAndValue("AddFlagOrder",			new long(RGF_SM_ADDFLAGORDER));
	h_method.insertKeyAndValue("DelFlagOrder",			new long(RGF_SM_DELFLAGORDER));
	//h_method.insertKeyAndValue("ChangeMaterial",		new long(RGF_SM_CHANGEMATERIAL));			// same as low method
	h_method.insertKeyAndValue("AddTimerOrder",			new long(RGF_SM_ADDTIMERORDER));
	h_method.insertKeyAndValue("DelTimerOrder",			new long(RGF_SM_DELTIMERORDER));
	h_method.insertKeyAndValue("AddRandomSound",		new long(RGF_SM_ADDRANDOMSOUND));
	h_method.insertKeyAndValue("DelRandomSound",		new long(RGF_SM_DELRANDOMSOUND));
	h_method.insertKeyAndValue("AddDistanceOrder",		new long(RGF_SM_ADDDISTANCEORDER));
	h_method.insertKeyAndValue("DelDistanceOrder",		new long(RGF_SM_DELDISTANCEORDER));
	h_method.insertKeyAndValue("AddCollisionOrder",		new long(RGF_SM_ADDCOLLISIONORDER));
	h_method.insertKeyAndValue("DelCollisionOrder",		new long(RGF_SM_DELCOLLISIONORDER));
	h_method.insertKeyAndValue("AnimateStop",			new long(RGF_SM_ANIMATESTOP));
	h_method.insertKeyAndValue("AttributeOrder",		new long(RGF_SM_ATTRIBUTEORDER));
	h_method.insertKeyAndValue("Remove",				new long(RGF_SM_REMOVE));
	//h_method.insertKeyAndValue("SetKeyPause",			new long(RGF_SM_SETKEYPAUSE));				// same as low method
	//h_method.insertKeyAndValue("SetNoCollision",		new long(RGF_SM_SETNOCOLLISION));			// same as low method
	//h_method.insertKeyAndValue("SetCollision",		new long(RGF_SM_SETCOLLISION));				// same as low method
	h_method.insertKeyAndValue("AllowUseKey",			new long(RGF_SM_ALLOWUSEKEY));
	//h_method.insertKeyAndValue("SetHudDraw",			new long(RGF_SM_SETHUDDRAW));				// same as low method
	h_method.insertKeyAndValue("HideFromRadar",			new long(RGF_SM_HIDEFROMRADAR));
	h_method.insertKeyAndValue("Conversation",			new long(RGF_SM_CONVERSATION));
	h_method.insertKeyAndValue("FadeIn",				new long(RGF_SM_FADEIN));
	h_method.insertKeyAndValue("FadeOut",				new long(RGF_SM_FADEOUT));
	//h_method.insertKeyAndValue("SetFOV",				new long(RGF_SM_SETFOV));					// same as low method
	//h_method.insertKeyAndValue("StepHeight",			new long(RGF_SM_STEPHEIGHT));				// same as low method
	h_method.insertKeyAndValue("SetGroup",				new long(RGF_SM_SETGROUP));
	h_method.insertKeyAndValue("HostilePlayer",			new long(RGF_SM_HOSTILEPLAYER));
	h_method.insertKeyAndValue("HostileDifferent",		new long(RGF_SM_HOSTILEDIFFERENT));
	h_method.insertKeyAndValue("HostileSame",			new long(RGF_SM_HOSTILESAME));
	//h_method.insertKeyAndValue("Gravity",				new long(RGF_SM_GRAVITY));					// same as low method
	h_method.insertKeyAndValue("SoundLoop",				new long(RGF_SM_SOUNDLOOP));
	h_method.insertKeyAndValue("IsPushable",			new long(RGF_SM_ISPUSHABLE));
	h_method.insertKeyAndValue("IsVehicle",				new long(RGF_SM_ISVEHICLE));
	h_method.insertKeyAndValue("MoveToTarget",			new long(RGF_SM_MOVETOTARGET));
	h_method.insertKeyAndValue("RotateToTarget",		new long(RGF_SM_ROTATETOTARGET));
	h_method.insertKeyAndValue("RotateMoveToTarget",	new long(RGF_SM_ROTATEMOVETOTARGET));
	h_method.insertKeyAndValue("LowLevel",				new long(RGF_SM_LOWLEVEL));
	h_method.insertKeyAndValue("BoxWidth",				new long(RGF_SM_BOXWIDTH));
	h_method.insertKeyAndValue("BoxHeight",				new long(RGF_SM_BOXHEIGHT));
	h_method.insertKeyAndValue("Scale",					new long(RGF_SM_SCALE));
	//h_method.insertKeyAndValue("SetScale",			new long(RGF_SM_SETSCALE));					// same as low method
	//h_method.insertKeyAndValue("FireProjectile",		new long(RGF_SM_FIREPROJECTILE));			// same as low method
	//h_method.insertKeyAndValue("AddExplosion",		new long(RGF_SM_ADDEXPLOSION));				// same as low method
	h_method.insertKeyAndValue("TargetGroup",			new long(RGF_SM_TARGETGROUP));
	h_method.insertKeyAndValue("TestDamageOrder",		new long(RGF_SM_TESTDAMAGEORDER));
	h_method.insertKeyAndValue("SetLODDistance",		new long(RGF_SM_SETLODDISTANCE));
	h_method.insertKeyAndValue("AttachToActor",			new long(RGF_SM_ATTACHTOACTOR));
	h_method.insertKeyAndValue("DetachFromActor",		new long(RGF_SM_DETACHFROMACTOR));
	h_method.insertKeyAndValue("AttachBlendActor",		new long(RGF_SM_ATTACHBLENDACTOR));
	h_method.insertKeyAndValue("DetachBlendActor",		new long(RGF_SM_DETACHBLENDACTOR));
	h_method.insertKeyAndValue("AttachAccessory",		new long(RGF_SM_ATTACHACCESSORY));
	h_method.insertKeyAndValue("DetachAccessory",		new long(RGF_SM_DETACHACCESSORY));
	h_method.insertKeyAndValue("SetWeapon",				new long(RGF_SM_SETWEAPON));
	h_method.insertKeyAndValue("RemoveWeapon",			new long(RGF_SM_REMOVEWEAPON));
	//h_method.insertKeyAndValue("random",				new long(RGF_SM_RANDOM));					// same as low method
	//h_method.insertKeyAndValue("debug",				new long(RGF_SM_DEBUG));					// same as low method
	h_method.insertKeyAndValue("ShowTextDelay",			new long(RGF_SM_SHOWTEXTDELAY));
	//h_method.insertKeyAndValue("ShowText",			new long(RGF_SM_SHOWTEXT));					// same as low method
	//h_method.insertKeyAndValue("RemoveText",			new long(RGF_SM_REMOVETEXT));				// same as low method
	h_method.insertKeyAndValue("GetConvReplyNr",		new long(RGF_SM_GETCONVREPLYNR));
	h_method.insertKeyAndValue("Concat",				new long(RGF_SM_CONCAT));
	//h_method.insertKeyAndValue("GetAttribute",		new long(RGF_SM_GETATTRIBUTE));				// same as low method
	//h_method.insertKeyAndValue("ModifyAttribute",		new long(RGF_SM_MODIFYATTRIBUTE));			// same as low method
	//h_method.insertKeyAndValue("SetAttribute",		new long(RGF_SM_SETATTRIBUTE));				// same as low method
	//h_method.insertKeyAndValue("AddAttribute",		new long(RGF_SM_ADDATTRIBUTE));				// same as low method
	//h_method.insertKeyAndValue("SetAttributeValueLimits",new long(RGF_SM_SETATTRIBUTEVALUELIMITS));	// same as low method
	//h_method.insertKeyAndValue("GetFlag",				new long(RGF_SM_GETFLAG));					// same as low method
	//h_method.insertKeyAndValue("MouseControlledPlayer",new long(RGF_SM_MOUSECONTROLLEDPLAYER));	// same as low method
	//h_method.insertKeyAndValue("GetEventState",		new long(RGF_SM_GETEVENTSTATE));			// same as low method
	//h_method.insertKeyAndValue("EndScript",			new long(RGF_SM_ENDSCRIPT));				// same as low method

	// readable variables
	h_method.insertKeyAndValue("time",					new long(RGF_SM_TIME));
	h_method.insertKeyAndValue("ThinkTime",				new long(RGF_SM_THINKTIME));
	h_method.insertKeyAndValue("DifficultyLevel",		new long(RGF_SM_DIFFICULTYLEVEL));
	h_method.insertKeyAndValue("EntityName",			new long(RGF_SM_ENTITYNAME));
	h_method.insertKeyAndValue("Icon",					new long(RGF_SM_ICON));
	h_method.insertKeyAndValue("health",				new long(RGF_SM_HEALTH));
	h_method.insertKeyAndValue("attack_finished",		new long(RGF_SM_ATTACK_FINISHED));
	h_method.insertKeyAndValue("attack_state",			new long(RGF_SM_ATTACK_STATE));
	h_method.insertKeyAndValue("enemy_vis",				new long(RGF_SM_ENEMY_VIS));
	h_method.insertKeyAndValue("enemy_infront",			new long(RGF_SM_ENEMY_INFRONT));
	h_method.insertKeyAndValue("enemy_range",			new long(RGF_SM_ENEMY_RANGE));
	h_method.insertKeyAndValue("player_range",			new long(RGF_SM_PLAYER_RANGE));
	h_method.insertKeyAndValue("enemy_yaw",				new long(RGF_SM_ENEMY_YAW));
	h_method.insertKeyAndValue("last_enemy_yaw",		new long(RGF_SM_LAST_ENEMY_YAW));
	h_method.insertKeyAndValue("enemy_pitch",			new long(RGF_SM_ENEMY_PITCH));
	h_method.insertKeyAndValue("last_enemy_pitch",		new long(RGF_SM_LAST_ENEMY_PITCH));
	h_method.insertKeyAndValue("last_enemy_range",		new long(RGF_SM_LAST_ENEMY_RANGE));
	h_method.insertKeyAndValue("current_yaw",			new long(RGF_SM_CURRENT_YAW));
	h_method.insertKeyAndValue("yaw_speed",				new long(RGF_SM_YAW_SPEED));
	h_method.insertKeyAndValue("ideal_yaw",				new long(RGF_SM_IDEAL_YAW));
	h_method.insertKeyAndValue("current_pitch",			new long(RGF_SM_CURRENT_PITCH));
	h_method.insertKeyAndValue("pitch_speed",			new long(RGF_SM_PITCH_SPEED));
	h_method.insertKeyAndValue("ideal_pitch",			new long(RGF_SM_IDEAL_PITCH));
	h_method.insertKeyAndValue("in_pain",				new long(RGF_SM_IN_PAIN));
	h_method.insertKeyAndValue("animate_at_end",		new long(RGF_SM_ANIMATE_AT_END));
	h_method.insertKeyAndValue("IsFalling",				new long(RGF_SM_ISFALLING));
	h_method.insertKeyAndValue("current_X",				new long(RGF_SM_CURRENT_X));
	h_method.insertKeyAndValue("current_Y",				new long(RGF_SM_CURRENT_Y));
	h_method.insertKeyAndValue("current_Z",				new long(RGF_SM_CURRENT_Z));
	h_method.insertKeyAndValue("player_X",				new long(RGF_SM_PLAYER_X));
	h_method.insertKeyAndValue("player_Y",				new long(RGF_SM_PLAYER_Y));
	h_method.insertKeyAndValue("player_Z",				new long(RGF_SM_PLAYER_Z));
	h_method.insertKeyAndValue("distancetopoint",		new long(RGF_SM_DISTANCETOPOINT));
	h_method.insertKeyAndValue("playertopoint",			new long(RGF_SM_PLAYERTOPOINT));
	h_method.insertKeyAndValue("key_pressed",			new long(RGF_SM_KEY_PRESSED));
	h_method.insertKeyAndValue("player_vis",			new long(RGF_SM_PLAYER_VIS));
	h_method.insertKeyAndValue("target_name",			new long(RGF_SM_TARGET_NAME));
	h_method.insertKeyAndValue("enemy_X",				new long(RGF_SM_ENEMY_X));
	h_method.insertKeyAndValue("enemy_Y",				new long(RGF_SM_ENEMY_Y));
	h_method.insertKeyAndValue("enemy_Z",				new long(RGF_SM_ENEMY_Z));
	h_method.insertKeyAndValue("player_yaw",			new long(RGF_SM_PLAYER_YAW));
	h_method.insertKeyAndValue("point_vis",				new long(RGF_SM_POINT_VIS));
	h_method.insertKeyAndValue("player_weapon",			new long(RGF_SM_PLAYER_WEAPON));
	h_method.insertKeyAndValue("point_name",			new long(RGF_SM_POINT_NAME));
	h_method.insertKeyAndValue("camera_pitch",			new long(RGF_SM_CAMERA_PITCH));
	h_method.insertKeyAndValue("LODLevel",				new long(RGF_SM_LODLEVEL));
	h_method.insertKeyAndValue("current_screen_X",		new long(RGF_SM_CURRENT_SCREEN_X));
	h_method.insertKeyAndValue("current_screen_Y",		new long(RGF_SM_CURRENT_SCREEN_Y));
	h_method.insertKeyAndValue("player_screen_X",		new long(RGF_SM_PLAYER_SCREEN_X));
	h_method.insertKeyAndValue("player_screen_Y",		new long(RGF_SM_PLAYER_SCREEN_Y));
	h_method.insertKeyAndValue("lbutton_pressed",		new long(RGF_SM_LBUTTON_PRESSED));
	h_method.insertKeyAndValue("rbutton_pressed",		new long(RGF_SM_RBUTTON_PRESSED));
	h_method.insertKeyAndValue("mbutton_pressed",		new long(RGF_SM_MBUTTON_PRESSED));
	h_method.insertKeyAndValue("player_animation",		new long(RGF_SM_PLAYER_ANIMATION));
	h_method.insertKeyAndValue("player_viewpoint",		new long(RGF_SM_PLAYER_VIEWPOINT));
	h_method.insertKeyAndValue("ExecutionLevel",		new long(RGF_SM_EXECUTIONLEVEL));

	// writeable variables
	h_method.insertKeyAndValue("lowTime",				new long(RGF_SM_LOWTIME));
	//h_method.insertKeyAndValue("ThinkTime",			new long(RGF_SM_THINKTIME));			// same as readable
	h_method.insertKeyAndValue("think",					new long(RGF_SM_THINK));
	//h_method.insertKeyAndValue("attack_finished",		new long(RGF_SM_ATTACK_FINISHED));		// same as readable
	//h_method.insertKeyAndValue("attack_state",		new long(RGF_SM_ATTACK_STATE));			// same as readable
	//h_method.insertKeyAndValue("yaw_speed",			new long(RGF_SM_YAW_SPEED));			// same as readable
	//h_method.insertKeyAndValue("ideal_yaw",			new long(RGF_SM_IDEAL_YAW));			// same as readable
	//h_method.insertKeyAndValue("pitch_speed",			new long(RGF_SM_PITCH_SPEED));			// same as readable
	//h_method.insertKeyAndValue("ideal_pitch",			new long(RGF_SM_IDEAL_PITCH));			// same as readable
}
