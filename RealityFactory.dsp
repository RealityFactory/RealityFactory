# Microsoft Developer Studio Project File - Name="RGF" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=RGF - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "RealityFactory.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "RealityFactory.mak" CFG="RGF - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RGF - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "RGF - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "RGF - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Op /Ob2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 genesis.lib strmbase.lib winmm.lib vfw32.lib dxguid.lib quartz.lib user32.lib gdi32.lib ole32.lib comdlg32.lib decrypt.lib wsock32.lib kernel32.lib winspool.lib advapi32.lib shell32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libcmt" /out:"../RealityFactory.exe"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "RGF - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 genesisd.lib strmbasd.lib winmm.lib vfw32.lib dxguid.lib quartz.lib user32.lib gdi32.lib ole32.lib comdlg32.lib decrypt.lib wsock32.lib kernel32.lib winspool.lib advapi32.lib shell32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcmtd" /out:"../RealityFactory.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "RGF - Win32 Release"
# Name "RGF - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Simkin"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Simkin\skAlist.cpp
# End Source File
# Begin Source File

SOURCE=.\Simkin\skAlist.h
# End Source File
# Begin Source File

SOURCE=.\Simkin\skAlist.inl
# End Source File
# Begin Source File

SOURCE=.\Simkin\skBoundsException.h
# End Source File
# Begin Source File

SOURCE=.\Simkin\skExecutable.cpp
# End Source File
# Begin Source File

SOURCE=.\Simkin\skExecutable.h
# End Source File
# Begin Source File

SOURCE=.\Simkin\skExecutableIterator.cpp
# End Source File
# Begin Source File

SOURCE=.\Simkin\skExecutableIterator.h
# End Source File
# Begin Source File

SOURCE=.\Simkin\skGeneral.h
# End Source File
# Begin Source File

SOURCE=.\Simkin\skHashTable.cpp
# End Source File
# Begin Source File

SOURCE=.\Simkin\skHashTable.h
# End Source File
# Begin Source File

SOURCE=.\Simkin\skHashTable.inl
# End Source File
# Begin Source File

SOURCE=.\Simkin\skiExecutable.h
# End Source File
# Begin Source File

SOURCE=.\Simkin\skInterpreter.cpp
# End Source File
# Begin Source File

SOURCE=.\Simkin\skInterpreter.h
# End Source File
# Begin Source File

SOURCE=.\Simkin\skInterpreterp.h
# End Source File
# Begin Source File

SOURCE=.\Simkin\skLang_tab.cpp
# End Source File
# Begin Source File

SOURCE=.\Simkin\skLang_tab.h
# End Source File
# Begin Source File

SOURCE=.\Simkin\skMethodTable.h
# End Source File
# Begin Source File

SOURCE=.\Simkin\skNull.h
# End Source File
# Begin Source File

SOURCE=.\Simkin\skParseException.h
# End Source File
# Begin Source File

SOURCE=.\Simkin\skParseNode.h
# End Source File
# Begin Source File

SOURCE=.\Simkin\skParser.cpp
# End Source File
# Begin Source File

SOURCE=.\Simkin\skParser.h
# End Source File
# Begin Source File

SOURCE=.\Simkin\skRuntimeException.h
# End Source File
# Begin Source File

SOURCE=.\Simkin\skRValue.cpp
# End Source File
# Begin Source File

SOURCE=.\Simkin\skRValue.h
# End Source File
# Begin Source File

SOURCE=.\Simkin\skRValue.inl
# End Source File
# Begin Source File

SOURCE=.\Simkin\skRValueArray.h
# End Source File
# Begin Source File

SOURCE=.\Simkin\skRValueTable.h
# End Source File
# Begin Source File

SOURCE=.\Simkin\skScriptedExecutable.cpp
# End Source File
# Begin Source File

SOURCE=.\Simkin\skScriptedExecutable.h
# End Source File
# Begin Source File

SOURCE=.\Simkin\skStatementStepper.h
# End Source File
# Begin Source File

SOURCE=.\Simkin\skString.cpp
# End Source File
# Begin Source File

SOURCE=.\Simkin\skString.h
# End Source File
# Begin Source File

SOURCE=.\Simkin\skString.inl
# End Source File
# Begin Source File

SOURCE=.\Simkin\skStringList.h
# End Source File
# Begin Source File

SOURCE=.\Simkin\skStringTokenizer.cpp
# End Source File
# Begin Source File

SOURCE=.\Simkin\skStringTokenizer.h
# End Source File
# Begin Source File

SOURCE=.\Simkin\skTraceCallback.h
# End Source File
# Begin Source File

SOURCE=.\Simkin\skTracer.cpp
# End Source File
# Begin Source File

SOURCE=.\Simkin\skTracer.h
# End Source File
# Begin Source File

SOURCE=.\Simkin\skTreeNode.cpp
# End Source File
# Begin Source File

SOURCE=.\Simkin\skTreeNode.h
# End Source File
# Begin Source File

SOURCE=.\Simkin\skTreeNodeObject.cpp
# End Source File
# Begin Source File

SOURCE=.\Simkin\skTreeNodeObject.h
# End Source File
# Begin Source File

SOURCE=.\Simkin\skTreeNodeObjectEnumerator.cpp
# End Source File
# Begin Source File

SOURCE=.\Simkin\skTreeNodeObjectEnumerator.h
# End Source File
# Begin Source File

SOURCE=.\Simkin\skTreeNodp.h
# End Source File
# Begin Source File

SOURCE=.\Simkin\skValist.h
# End Source File
# End Group
# Begin Group "HawkNL"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\HawkNL\crc.c
# End Source File
# Begin Source File

SOURCE=.\HawkNL\err.c
# End Source File
# Begin Source File

SOURCE=.\HawkNL\errorstr.c
# End Source File
# Begin Source File

SOURCE=.\HawkNL\group.c
# End Source File
# Begin Source File

SOURCE=.\HawkNL\ipx.c
# End Source File
# Begin Source File

SOURCE=.\HawkNL\ipx.h
# End Source File
# Begin Source File

SOURCE=.\HawkNL\loopback.c
# End Source File
# Begin Source File

SOURCE=.\HawkNL\loopback.h
# End Source File
# Begin Source File

SOURCE=.\HawkNL\mutex.c
# End Source File
# Begin Source File

SOURCE=.\HawkNL\mutex.h
# End Source File
# Begin Source File

SOURCE=.\HawkNL\nl.c
# End Source File
# Begin Source File

SOURCE=.\HawkNL\nl.h
# End Source File
# Begin Source File

SOURCE=.\HawkNL\nlinternal.h
# End Source File
# Begin Source File

SOURCE=.\HawkNL\parallel.h
# End Source File
# Begin Source File

SOURCE=.\HawkNL\serial.h
# End Source File
# Begin Source File

SOURCE=.\HawkNL\sock.c
# End Source File
# Begin Source File

SOURCE=.\HawkNL\sock.h
# End Source File
# Begin Source File

SOURCE=.\HawkNL\thread.c
# End Source File
# Begin Source File

SOURCE=.\HawkNL\thread.h
# End Source File
# Begin Source File

SOURCE=.\HawkNL\wsock.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\AutoSelect.c
# End Source File
# Begin Source File

SOURCE=.\C3DAudioSource.cpp
# End Source File
# Begin Source File

SOURCE=.\CActorManager.cpp
# End Source File
# Begin Source File

SOURCE=.\CAnimGif.cpp
# End Source File
# Begin Source File

SOURCE=.\CAttribute.cpp
# End Source File
# Begin Source File

SOURCE=.\CAudioManager.cpp
# End Source File
# Begin Source File

SOURCE=.\CAudioStream.cpp
# End Source File
# Begin Source File

SOURCE=.\CAutoDoors.cpp
# End Source File
# Begin Source File

SOURCE=.\CAVIPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\CCameraManager.cpp
# End Source File
# Begin Source File

SOURCE=.\CCDAudio.cpp
# End Source File
# Begin Source File

SOURCE=.\CChange.cpp
# End Source File
# Begin Source File

SOURCE=.\CChangeLevel.cpp
# End Source File
# Begin Source File

SOURCE=.\CCommonData.cpp
# End Source File
# Begin Source File

SOURCE=.\CCorona.cpp
# End Source File
# Begin Source File

SOURCE=.\CCountDown.cpp
# End Source File
# Begin Source File

SOURCE=.\CDamage.cpp
# End Source File
# Begin Source File

SOURCE=.\CDecal.cpp
# End Source File
# Begin Source File

SOURCE=.\CDynalite.cpp
# End Source File
# Begin Source File

SOURCE=.\CEffects.cpp
# End Source File
# Begin Source File

SOURCE=.\CElectric.cpp
# End Source File
# Begin Source File

SOURCE=.\CEnemy.cpp
# End Source File
# Begin Source File

SOURCE=.\CEntityRegistry.cpp
# End Source File
# Begin Source File

SOURCE=.\CExplosion.cpp
# End Source File
# Begin Source File

SOURCE=.\CFirePoint.cpp
# End Source File
# Begin Source File

SOURCE=.\CFixedCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\CFlame.cpp
# End Source File
# Begin Source File

SOURCE=.\CFlipBook.cpp
# End Source File
# Begin Source File

SOURCE=.\CFloating.cpp
# End Source File
# Begin Source File

SOURCE=.\CGenesisEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\Chaos.cpp
# End Source File
# Begin Source File

SOURCE=.\CHeadsUpDisplay.cpp
# End Source File
# Begin Source File

SOURCE=.\CInput.cpp
# End Source File
# Begin Source File

SOURCE=.\CInventory.cpp
# End Source File
# Begin Source File

SOURCE=.\CLiquid.cpp
# End Source File
# Begin Source File

SOURCE=.\CLogic.cpp
# End Source File
# Begin Source File

SOURCE=.\CMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\CMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\CMIDIAudio.cpp
# End Source File
# Begin Source File

SOURCE=.\CModelManager.cpp
# End Source File
# Begin Source File

SOURCE=.\CMorph.cpp
# End Source File
# Begin Source File

SOURCE=.\CMorphingFields.cpp
# End Source File
# Begin Source File

SOURCE=.\CMovingPlatforms.cpp
# End Source File
# Begin Source File

SOURCE=.\CMp3.cpp
# End Source File
# Begin Source File

SOURCE=.\CNetBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\CNetwork.cpp
# End Source File
# Begin Source File

SOURCE=.\CNPC.cpp
# End Source File
# Begin Source File

SOURCE=.\CNPCPathPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\Collider.cpp
# End Source File
# Begin Source File

SOURCE=.\CParticleSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\CPathDatabase.cpp
# End Source File
# Begin Source File

SOURCE=.\CPathFollower.cpp
# End Source File
# Begin Source File

SOURCE=.\CPawn.cpp
# End Source File
# Begin Source File

SOURCE=.\CPawnCon.cpp
# End Source File
# Begin Source File

SOURCE=.\CPawnHigh.cpp
# End Source File
# Begin Source File

SOURCE=.\CPawnLow.cpp
# End Source File
# Begin Source File

SOURCE=.\CPersistentAttributes.cpp
# End Source File
# Begin Source File

SOURCE=.\CPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\CProcedural.cpp
# End Source File
# Begin Source File

SOURCE=.\CRain.cpp
# End Source File
# Begin Source File

SOURCE=.\CRGFComponent.cpp
# End Source File
# Begin Source File

SOURCE=.\CScriptPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\CShake.cpp
# End Source File
# Begin Source File

SOURCE=.\CSoundtrackToggle.cpp
# End Source File
# Begin Source File

SOURCE=.\CSpout.cpp
# End Source File
# Begin Source File

SOURCE=.\CStaticEntity.cpp
# End Source File
# Begin Source File

SOURCE=.\CTeleporter.cpp
# End Source File
# Begin Source File

SOURCE=.\CTriggers.cpp
# End Source File
# Begin Source File

SOURCE=.\CVideoTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\CViewSwitch.cpp
# End Source File
# Begin Source File

SOURCE=.\CWallDecal.cpp
# End Source File
# Begin Source File

SOURCE=.\CWeapon.cpp
# End Source File
# Begin Source File

SOURCE=.\drawbbox.c
# End Source File
# Begin Source File

SOURCE=.\EffManger.cpp
# End Source File
# Begin Source File

SOURCE=.\EffParticle.cpp
# End Source File
# Begin Source File

SOURCE=.\IniFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Mixer.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcBumpmap.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcElectricFx.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcFire.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcFlow.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcParticles.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcPlasma.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcSmoke.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcWater.cpp
# End Source File
# Begin Source File

SOURCE=.\RabidFrameworkMain.cpp
# End Source File
# Begin Source File

SOURCE=.\RealityFactory.rc
# End Source File
# Begin Source File

SOURCE=.\SPool.cpp
# End Source File
# Begin Source File

SOURCE=.\StreamingAudio.cpp
# End Source File
# Begin Source File

SOURCE=.\TPool.cpp
# End Source File
# Begin Source File

SOURCE=.\Track.cpp
# End Source File
# Begin Source File

SOURCE=.\Utilities.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AutoSelect.h
# End Source File
# Begin Source File

SOURCE=.\C3DAudioSource.h
# End Source File
# Begin Source File

SOURCE=.\CActorManager.h
# End Source File
# Begin Source File

SOURCE=.\CAnimGif.h
# End Source File
# Begin Source File

SOURCE=.\CAttribute.h
# End Source File
# Begin Source File

SOURCE=.\CAudioManager.h
# End Source File
# Begin Source File

SOURCE=.\CAudioStream.h
# End Source File
# Begin Source File

SOURCE=.\CAutoDoors.h
# End Source File
# Begin Source File

SOURCE=.\CAVIPlayer.h
# End Source File
# Begin Source File

SOURCE=.\CCameraManager.h
# End Source File
# Begin Source File

SOURCE=.\CCDAudio.h
# End Source File
# Begin Source File

SOURCE=.\CChange.h
# End Source File
# Begin Source File

SOURCE=.\CChangeLevel.h
# End Source File
# Begin Source File

SOURCE=.\CCommonData.h
# End Source File
# Begin Source File

SOURCE=.\CCorona.h
# End Source File
# Begin Source File

SOURCE=.\CCountDown.h
# End Source File
# Begin Source File

SOURCE=.\CDamage.h
# End Source File
# Begin Source File

SOURCE=.\CDecal.h
# End Source File
# Begin Source File

SOURCE=.\CDynalite.h
# End Source File
# Begin Source File

SOURCE=.\CEffects.h
# End Source File
# Begin Source File

SOURCE=.\CElectric.h
# End Source File
# Begin Source File

SOURCE=.\CEnemy.h
# End Source File
# Begin Source File

SOURCE=.\CEntityRegistry.h
# End Source File
# Begin Source File

SOURCE=.\CExplosion.h
# End Source File
# Begin Source File

SOURCE=.\CFirePoint.h
# End Source File
# Begin Source File

SOURCE=.\CFixedCamera.h
# End Source File
# Begin Source File

SOURCE=.\CFlame.h
# End Source File
# Begin Source File

SOURCE=.\CFlipBook.h
# End Source File
# Begin Source File

SOURCE=.\CFloating.h
# End Source File
# Begin Source File

SOURCE=.\CGenesisEngine.h
# End Source File
# Begin Source File

SOURCE=.\Chaos.h
# End Source File
# Begin Source File

SOURCE=.\CHeadsUpDisplay.h
# End Source File
# Begin Source File

SOURCE=.\CInput.h
# End Source File
# Begin Source File

SOURCE=.\CInventory.h
# End Source File
# Begin Source File

SOURCE=.\CLiquid.h
# End Source File
# Begin Source File

SOURCE=.\Clogic.h
# End Source File
# Begin Source File

SOURCE=.\CMenu.h
# End Source File
# Begin Source File

SOURCE=.\CMessage.h
# End Source File
# Begin Source File

SOURCE=.\CMIDIAudio.h
# End Source File
# Begin Source File

SOURCE=.\CModelManager.h
# End Source File
# Begin Source File

SOURCE=.\CMorph.h
# End Source File
# Begin Source File

SOURCE=.\CMorphingFields.h
# End Source File
# Begin Source File

SOURCE=.\CMovingPlatforms.h
# End Source File
# Begin Source File

SOURCE=.\CMp3.h
# End Source File
# Begin Source File

SOURCE=.\CNetBuffer.h
# End Source File
# Begin Source File

SOURCE=.\CNetwork.h
# End Source File
# Begin Source File

SOURCE=.\CNPC.h
# End Source File
# Begin Source File

SOURCE=.\CNPCPathPoint.h
# End Source File
# Begin Source File

SOURCE=.\Collider.h
# End Source File
# Begin Source File

SOURCE=.\CParticleSystem.h
# End Source File
# Begin Source File

SOURCE=.\CPathDatabase.h
# End Source File
# Begin Source File

SOURCE=.\CPathFollower.h
# End Source File
# Begin Source File

SOURCE=.\CPawn.h
# End Source File
# Begin Source File

SOURCE=.\CPersistentAttributes.h
# End Source File
# Begin Source File

SOURCE=.\Cplayer.h
# End Source File
# Begin Source File

SOURCE=.\CProcedural.h
# End Source File
# Begin Source File

SOURCE=.\CRain.h
# End Source File
# Begin Source File

SOURCE=.\CRGFComponent.h
# End Source File
# Begin Source File

SOURCE=.\CScriptPoint.h
# End Source File
# Begin Source File

SOURCE=.\CShake.h
# End Source File
# Begin Source File

SOURCE=.\CSoundtrackToggle.h
# End Source File
# Begin Source File

SOURCE=.\CSpout.h
# End Source File
# Begin Source File

SOURCE=.\CStaticEntity.h
# End Source File
# Begin Source File

SOURCE=.\CTeleporter.h
# End Source File
# Begin Source File

SOURCE=.\CTriggers.h
# End Source File
# Begin Source File

SOURCE=.\CVideoTexture.h
# End Source File
# Begin Source File

SOURCE=.\CViewSwitch.h
# End Source File
# Begin Source File

SOURCE=.\CWallDecal.h
# End Source File
# Begin Source File

SOURCE=.\Cweapon.h
# End Source File
# Begin Source File

SOURCE=.\EffManager.h
# End Source File
# Begin Source File

SOURCE=.\GameEntityDataTypes.h
# End Source File
# Begin Source File

SOURCE=.\IniFile.h
# End Source File
# Begin Source File

SOURCE=.\Mixer.h
# End Source File
# Begin Source File

SOURCE=.\ProcUtil.h
# End Source File
# Begin Source File

SOURCE=.\RabidFramework.h
# End Source File
# Begin Source File

SOURCE=.\RGFEvents.h
# End Source File
# Begin Source File

SOURCE=.\RGFStatus.h
# End Source File
# Begin Source File

SOURCE=.\RGFTypes.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\StreamingAudio.h
# End Source File
# Begin Source File

SOURCE=.\track.h
# End Source File
# Begin Source File

SOURCE=.\Utilities.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\mainfram.ico
# End Source File
# Begin Source File

SOURCE=.\rficon.ico
# End Source File
# End Group
# End Target
# End Project
