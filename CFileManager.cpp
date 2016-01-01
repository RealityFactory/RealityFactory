/************************************************************************************//**
 * @file CFileManager.cpp
 * @brief FileManager class
 *
 * This file contains the class implementation for the FileManager class.
 * @author Daniel Queteschiner
 * @date June 2011
 *//*
 * Copyright (c) 2011 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#include "CLog.h"
#include <tchar.h>
#include <shlobj.h>
#include "RGFConstants.h"
#include "RGFStatus.h"
#include "CFileManager.h"
#include "IniFile.h"

extern geVFile *PassWord(char *vFile, bool encrypt);
extern void CloseFile();

CFileManager* CFileManager::m_FileManager = NULL;

CFileManager::CFileManager(void)
{
	// Set up directory defaults
	_tcscpy(m_LevelDirectory,		TEXT("levels")		);
	_tcscpy(m_ActorDirectory,		TEXT("actors")		);
	_tcscpy(m_BitmapDirectory,		TEXT("bitmaps")		);
	_tcscpy(m_AudioDirectory,		TEXT("audio")		);
	_tcscpy(m_VideoDirectory,		TEXT("video")		);
	_tcscpy(m_AudioStreamDirectory,	TEXT("audio")		);
	_tcscpy(m_MIDIDirectory,		TEXT("midi")		);
	_tcscpy(m_SavegameDirectory,	TEXT("saves")		);
	_tcscpy(m_ScreenshotDirectory,	TEXT("screenshots")	);
	_tcscpy(m_ConfigDirectory,		TEXT("configs")		);
	_tcscpy(m_LogDirectory,			TEXT("logs")		);
	_tcscpy(m_GameTitle,			TEXT("Game")		);

	m_VFS = NULL;
}


CFileManager::~CFileManager(void)
{
	geVFile_CloseAPI();

	if(m_VFS != NULL)
	{
		CloseFile();
	}
}


CFileManager* CFileManager::GetSingletonPtr()
{
	if(m_FileManager == 0)
	{
		m_FileManager = new CFileManager();
	}

	return m_FileManager;
}


void CFileManager::Create(LPTSTR vFilename)
{
	FILE *fdInput = NULL;

	if((fdInput = fopen(vFilename, "rt")) == NULL)
	{
		m_VFS = NULL;
		sxLog::GetSingletonPtr()->Info("No VFS detected - Reading from Concrete File System...");
	}
	else
	{
		char szInputString[16];

		fread(szInputString, 4, 1, fdInput);
		fclose(fdInput);

		if(memcmp(szInputString, "CF00", 4) == 0)
		{
			m_VFS = PassWord(vFilename, true);
			sxLog::GetSingletonPtr()->Info("VFS detected (encrypted)...");
		}
		else
		{
			m_VFS = PassWord(vFilename, false);
			sxLog::GetSingletonPtr()->Info("VFS detected (not encrypted)...");
		}
	}

	CIniFile iniFile("internal_config.ini");

	if(!iniFile.ReadFile())
	{
		sxLog::GetSingletonPtr()->Critical("File %s - Line %d: Failed to open internal_config.ini file!", __FILE__, __LINE__);
	}
	else
	{
		std::string	value;

		value = iniFile.GetValue("Directories", "LevelDirectory");
		if(!value.empty())	strcpy(m_LevelDirectory, value.c_str());

		value = iniFile.GetValue("Directories", "ActorDirectory");
		if(!value.empty())	strcpy(m_ActorDirectory, value.c_str());

		value = iniFile.GetValue("Directories", "BitmapDirectory");
		if(!value.empty())	strcpy(m_BitmapDirectory, value.c_str());

		value = iniFile.GetValue("Directories", "AudioDirectory");
		if(!value.empty())	strcpy(m_AudioDirectory, value.c_str());

		value = iniFile.GetValue("Directories", "AudioStreamDirectory");
		if(!value.empty())	strcpy(m_AudioStreamDirectory, value.c_str());

		value = iniFile.GetValue("Directories", "VideoDirectory");
		if(!value.empty())	strcpy(m_VideoDirectory, value.c_str());

		value = iniFile.GetValue("Directories", "MIDIDirectory");
		if(!value.empty())	strcpy(m_MIDIDirectory, value.c_str());

		value = iniFile.GetValue("Directories", "SavegameDirectory");
		if(!value.empty())	strcpy(m_SavegameDirectory, value.c_str());

		value = iniFile.GetValue("Directories", "ScreenshotDirectory");
		if(!value.empty())	strcpy(m_ScreenshotDirectory, value.c_str());

		value = iniFile.GetValue("Directories", "ConfigDirectory");
		if(!value.empty())	strcpy(m_ConfigDirectory, value.c_str());

		value = iniFile.GetValue("Directories", "LogDirectory");
		if(!value.empty())	strcpy(m_LogDirectory, value.c_str());

		value = iniFile.GetValue("General", "GameName");
		if(!value.empty())	strcpy(m_GameTitle, value.c_str());
	}

	// create writeable directories
	CreateDataDirectory(CSIDL_APPDATA);
	CreateDataDirectory(CSIDL_PERSONAL);
	CreateDataSubDirectory(CSIDL_APPDATA, m_ConfigDirectory, m_ConfigDirectory);
	CreateDataSubDirectory(CSIDL_APPDATA, m_LogDirectory, m_LogDirectory);
	CreateDataSubDirectory(CSIDL_PERSONAL, m_SavegameDirectory, m_SavegameDirectory);
	CreateDataSubDirectory(CSIDL_PERSONAL, m_ScreenshotDirectory, m_ScreenshotDirectory);
}


void CFileManager::Destroy()
{
	delete m_FileManager;
	m_FileManager = NULL;
}


void AppendPath(LPTSTR path, LPCTSTR more)
{
	if(path && more)
	{
		_tcscat(path, TEXT("\\"));
		_tcscat(path, more);
	}
}

//SHGetFolderPath(); // on XP
//SHGetKnownFolderPath(FOLDERID_DocumentsLibrary, 0, NULL, pSzFolderPath); // on Vista

int CFileManager::CreateDataDirectory(int csidl)
{
	TCHAR path[_MAX_PATH];
	WIN32_FIND_DATA findFileData;
	HANDLE hFindFile = INVALID_HANDLE_VALUE;

	if(FAILED(SHGetFolderPath(	NULL,
								csidl | CSIDL_FLAG_CREATE,
								NULL,
								0,
								path)))
	{
		sxLog::GetSingletonPtr()->Error("Failed to get known folder path (%x).", csidl);
	}
	else
	{
		AppendPath(path, m_GameTitle);
	}

	hFindFile = FindFirstFile(path, &findFileData);

	if(hFindFile == INVALID_HANDLE_VALUE)
	{
		if(!CreateDirectory(path, NULL))
		{
			sxLog::GetSingletonPtr()->Warning("Failed to create directory %s", path);
			return RGF_FAILURE;
		}
	}
	else
	{
		FindClose(hFindFile);
	}

	return RGF_SUCCESS;
}


int CFileManager::CreateDataSubDirectory(int csidl, LPCTSTR subDir, LPTSTR finalPath)
{
	TCHAR path[_MAX_PATH];
	WIN32_FIND_DATA findFileData;
	HANDLE hFindFile = INVALID_HANDLE_VALUE;

	if(FAILED(SHGetFolderPath(	NULL,
								csidl | CSIDL_FLAG_CREATE,
								NULL,
								0,
								path)))
	{
		sxLog::GetSingletonPtr()->Error("Failed to get known folder path (%x).", csidl);
		// try to create directory in current working directory
		_tcscpy(finalPath, subDir);
	}
	else
	{
		AppendPath(path, m_GameTitle);
		AppendPath(path, subDir);
		_tcscpy(finalPath, path);
	}

	hFindFile = FindFirstFile(finalPath, &findFileData);

	if(hFindFile == INVALID_HANDLE_VALUE)
	{
		if(!CreateDirectory(finalPath, NULL))
		{
			sxLog::GetSingletonPtr()->Warning("Failed to create directory %s", finalPath);
			return RGF_FAILURE;
		}
	}
	else
	{
		FindClose(hFindFile);
	}

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// OpenRFFile
//
// This version of OpenRFFile opens a file of a specified type via the
// .."normal" fopen() calls.  This is NOT to be used to open geVFile
// ..entities, there is an overload that handles this kind of file open
// ..as well.
//
// This makes it easy to insure that you're opening files in the right
// ..directory for your installation as all the directory names are
// ..loaded from the RealityFactory.ini file in the same directory as
// ..the game runner.
/* ------------------------------------------------------------------------------------ */
FILE* CFileManager::OpenRFFile(int fileType, LPCTSTR filename, LPCTSTR mode)
{
	TCHAR szTemp[_MAX_PATH];

	// Ok, based on the TYPE of file we're looking for, build a "proper"
	// ..filename (fully qualified, more or less).

	switch(fileType)
	{
	case kActorFile:
		_tcscpy(szTemp, m_ActorDirectory);
		AppendPath(szTemp, filename);
		break;
	case kAudioFile:
		_tcscpy(szTemp, m_AudioDirectory);
		AppendPath(szTemp, filename);
		break;
	case kVideoFile:
		_tcscpy(szTemp, m_VideoDirectory);
		AppendPath(szTemp, filename);
		break;
	case kMIDIFile:
		_tcscpy(szTemp, m_MIDIDirectory);
		AppendPath(szTemp, filename);
		break;
	case kLevelFile:
		_tcscpy(szTemp, m_LevelDirectory);
		AppendPath(szTemp, filename);
		break;
	case kAudioStreamFile:
		_tcscpy(szTemp, m_AudioStreamDirectory);
		AppendPath(szTemp, filename);
		break;
	case kBitmapFile:
		_tcscpy(szTemp, m_BitmapDirectory);
		AppendPath(szTemp, filename);
		break;
	case kSavegameFile:
		_tcscpy(szTemp, m_SavegameDirectory);
		AppendPath(szTemp, filename);
		break;
	case kTempFile:
		_tcscpy(szTemp, TEXT(".\\"));
		strcat(szTemp, filename);
		break;
	case kInstallFile:
		_tcscpy(szTemp, TEXT("install"));
		AppendPath(szTemp, filename);
		break;
	case kScriptFile:
		_tcscpy(szTemp, TEXT(".\\"));
		strcat(szTemp, filename);
		break;
	case kConfigFile:
		_tcscpy(szTemp, m_ConfigDirectory);
		AppendPath(szTemp, filename);
		break;
	case kLogFile:
		_tcscpy(szTemp, m_LogDirectory);
		AppendPath(szTemp, filename);
		break;
	case kScreenshotFile:
		_tcscpy(szTemp, m_ScreenshotDirectory);
		AppendPath(szTemp, filename);
		break;
	case kRawFile:
		_tcscpy(szTemp, filename);
		break;
	default:
		sxLog::GetSingletonPtr()->Error("OpenRFFile: Bad type '%d' for '%s'", fileType, filename);
		return NULL;
	}

	FILE *pTemp = _tfopen(szTemp, mode);			// Open the damn thing.

	// Debug tracing time
	if(!pTemp)
	{
		sxLog::GetSingletonPtr()->Error("File open attempt failed on type '%d', file '%s'", fileType, szTemp);
	}

	return pTemp;									// Back to the caller with it.
}


/* ------------------------------------------------------------------------------------ */
// OpenRFFile
//
// This version of OpenRFFile operates identically to the 'fopen()' based
// ..version, but uses the geVFile system to open up virtual files.  This
// ..is provided since some of the G3D API relies on geVFiles.
/* ------------------------------------------------------------------------------------ */
bool CFileManager::OpenRFFile(geVFile **fp, int fileType, LPCTSTR filename, int mode)
{
	TCHAR szTemp[_MAX_PATH];

	*fp = NULL;

	// Ok, based on the TYPE of file we're looking for, build a "proper"
	// ..filename (fully qualified, more or less).
	switch(fileType)
	{
	case kActorFile:
		_tcscpy(szTemp, m_ActorDirectory);
		AppendPath(szTemp, filename);
		break;
	case kAudioFile:
		_tcscpy(szTemp, m_AudioDirectory);
		AppendPath(szTemp, filename);
		break;
	case kVideoFile:
		_tcscpy(szTemp, m_VideoDirectory);
		AppendPath(szTemp, filename);
		break;
	case kMIDIFile:
		_tcscpy(szTemp, m_MIDIDirectory);
		AppendPath(szTemp, filename);
		break;
	case kLevelFile:
		_tcscpy(szTemp, m_LevelDirectory);
		AppendPath(szTemp, filename);
		break;
	case kAudioStreamFile:
		_tcscpy(szTemp, m_AudioStreamDirectory);
		AppendPath(szTemp, filename);
		break;
	case kBitmapFile:
		_tcscpy(szTemp, m_BitmapDirectory);
		AppendPath(szTemp, filename);
		break;
	case kSavegameFile:
		_tcscpy(szTemp, m_SavegameDirectory);
		AppendPath(szTemp, filename);
		break;
	case kTempFile:
		_tcscpy(szTemp, TEXT(".\\"));
		_tcscat(szTemp, filename);
		break;
	case kInstallFile:
		_tcscpy(szTemp, TEXT("install\\"));
		_tcscat(szTemp, filename);
		break;
	case kScriptFile:
		_tcscpy(szTemp, TEXT(".\\"));
		_tcscat(szTemp, filename);
		break;
	case kConfigFile:
		_tcscpy(szTemp, m_ConfigDirectory);
		AppendPath(szTemp, filename);
		break;
	case kLogFile:
		_tcscpy(szTemp, m_LogDirectory);
		AppendPath(szTemp, filename);
		break;
	case kScreenshotFile:
		_tcscpy(szTemp, m_ScreenshotDirectory);
		AppendPath(szTemp, filename);
		break;
	case kRawFile:
		_tcscpy(szTemp, filename);
		break;
	default:
		sxLog::GetSingletonPtr()->Error("OpenRFFile(V): bad type '%d' for '%s'", fileType, filename);
		return false;
	}

	// Ok, open the file up.
	*fp = geVFile_OpenNewSystem(NULL, GE_VFILE_TYPE_DOS, szTemp, NULL, mode);

	if(*fp == NULL && m_VFS)
	{
		sxLog::GetSingletonPtr()->Debug("File '%s' not found in Real File System, searching in VFS", szTemp);
		*fp = geVFile_Open(m_VFS, szTemp, mode);
	}

	// If it worked, return true, otherwise return false.
	if(*fp != NULL)
	{
		sxLog::GetSingletonPtr()->Debug("Opened file '%s'", szTemp);
		return true;
	}
	else
	{
		// Debug tracing time
		sxLog::GetSingletonPtr()->Error("OpenRFFile: File open attempt failed on type '%d', file '%s'", fileType, szTemp);
		return false;
	}
}


/* ------------------------------------------------------------------------------------ */
// FileExist
/* ------------------------------------------------------------------------------------ */
bool CFileManager::FileExist(int fileType, LPCTSTR filename)
{
	TCHAR szTemp[_MAX_PATH];
	geVFile *pVFile = NULL;

	// Ok, based on the TYPE of file we're looking for, build a "proper"
	// ..filename (fully qualified, more or less).
	switch(fileType)
	{
	case kActorFile:
		_tcscpy(szTemp, m_ActorDirectory);
		AppendPath(szTemp, filename);
		break;
	case kAudioFile:
		_tcscpy(szTemp, m_AudioDirectory);
		AppendPath(szTemp, filename);
		break;
	case kVideoFile:
		_tcscpy(szTemp, m_VideoDirectory);
		AppendPath(szTemp, filename);
		break;
	case kMIDIFile:
		_tcscpy(szTemp, m_MIDIDirectory);
		AppendPath(szTemp, filename);
		break;
	case kLevelFile:
		_tcscpy(szTemp, m_LevelDirectory);
		AppendPath(szTemp, filename);
		break;
	case kAudioStreamFile:
		_tcscpy(szTemp, m_AudioStreamDirectory);
		AppendPath(szTemp, filename);
		break;
	case kBitmapFile:
		_tcscpy(szTemp, m_BitmapDirectory);
		AppendPath(szTemp, filename);
		break;
	case kSavegameFile:
		_tcscpy(szTemp, m_SavegameDirectory);
		AppendPath(szTemp, filename);
		break;
	case kTempFile:
		_tcscpy(szTemp, TEXT(".\\"));
		_tcscat(szTemp, filename);
		break;
	case kInstallFile:
		_tcscpy(szTemp, TEXT("install\\"));
		_tcscat(szTemp, filename);
		break;
	case kScriptFile:
		_tcscpy(szTemp, TEXT(".\\"));
		_tcscat(szTemp, filename);
		break;
	case kConfigFile:
		_tcscpy(szTemp, m_ConfigDirectory);
		AppendPath(szTemp, filename);
		break;
	case kLogFile:
		_tcscpy(szTemp, m_LogDirectory);
		AppendPath(szTemp, filename);
		break;
	case kScreenshotFile:
		_tcscpy(szTemp, m_ScreenshotDirectory);
		AppendPath(szTemp, filename);
		break;
	case kRawFile:
		_tcscpy(szTemp, filename);
		break;
	default:
		sxLog::GetSingletonPtr()->Error("FileExist: bad type '%d' for '%s'", fileType, filename);
		return false;
	}

	// Ok, open the file up.
	pVFile = geVFile_OpenNewSystem(NULL, GE_VFILE_TYPE_DOS, szTemp, NULL, GE_VFILE_OPEN_READONLY);

	if(pVFile == NULL && m_VFS)
	{
		// File does not exist in Real File System, search in VFS
		pVFile = geVFile_Open(m_VFS, szTemp, GE_VFILE_OPEN_READONLY);
	}

	if(pVFile != NULL)
	{
		geVFile_Close(pVFile);
		return true;
	}

	return false;
}


bool CFileManager::DeleteRFFile(int fileType, LPCTSTR filename)
{
	TCHAR directory[_MAX_PATH];
	_tcscpy(directory, GetDirectory(fileType));

	// Ok, open the file system.
	geVFile *pVFile = geVFile_OpenNewSystem(NULL, GE_VFILE_TYPE_DOS, directory, NULL, GE_VFILE_OPEN_DIRECTORY);
	if(pVFile)
	{
		if(geVFile_DeleteFile(pVFile, filename) == GE_TRUE)
		{
			sxLog::GetSingletonPtr()->Info("RemoveRFFile: File deleted type '%d', file '%s'", fileType, filename);
			geVFile_Close(pVFile);
			return true;
		}
		geVFile_Close(pVFile);
	}

	if(m_VFS)
	{
		AppendPath(directory, filename);
		if(geVFile_FileExists(m_VFS, directory))
		{
			sxLog::GetSingletonPtr()->Info("RemoveRFFile: geVFile_DeleteFile not implemented for VFS: type '%d', file '%s'", fileType, filename);
		}
	}

	sxLog::GetSingletonPtr()->Notice("RemoveRFFile: File delete attempt failed on type '%d', file '%s'", fileType, filename);
	return false;
}


/* ------------------------------------------------------------------------------------ */
// GetDirectory
//
// Given the type of directory desired, return a pointer to the appropriate
// ..configured directory.
/* ------------------------------------------------------------------------------------ */
LPCTSTR CFileManager::GetDirectory(int type)
{
	LPTSTR pDir = NULL;

	switch(type)
	{
	case kActorFile:
		pDir = m_ActorDirectory;
		break;
	case kAudioFile:
		pDir = m_AudioDirectory;
		break;
	case kVideoFile:
		pDir = m_VideoDirectory;
		break;
	case kMIDIFile:
		pDir = m_MIDIDirectory;
		break;
	case kLevelFile:
		pDir = m_LevelDirectory;
		break;
	case kAudioStreamFile:
		pDir = m_AudioStreamDirectory;
		break;
	case kBitmapFile:
		pDir = m_BitmapDirectory;
		break;
	case kSavegameFile:
		pDir = m_SavegameDirectory;
		break;
	case kTempFile:
		pDir = TEXT(".");
		break;
	case kInstallFile:
		pDir = TEXT("install");
		break;
	case kScriptFile:
		pDir = TEXT(".");
		break;
	case kConfigFile:
		pDir = m_ConfigDirectory;
		break;
	case kLogFile:
		pDir = m_LogDirectory;
		break;
	case kScreenshotFile:
		pDir = m_ScreenshotDirectory;
		break;
	case kRawFile:
		pDir = TEXT(".");
		break;
	default:
		sxLog::GetSingletonPtr()->Error("GetDirectory: bad directory type '%d'", type);
		return "";
	}

	return pDir;				// Back to caller with ... whatever.
}
