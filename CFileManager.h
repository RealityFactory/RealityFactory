/************************************************************************************//**
 * @file CFileManager.h
 * @brief FileManager class
 *
 * This file contains the class declaration for the FileManager class.
 * @author Daniel Queteschiner
 * @date June 2011
 *//*
 * Copyright (c) 2011 Daniel Queteschiner; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_FILEMANAGER_H_
#define __RGF_FILEMANAGER_H_

#include <stdio.h>
#include <windows.h>
#include <genesis.h>

class CFileManager
{
public:
	~CFileManager();

	/**
	 * @brief Open RF file
	 */
	FILE*		OpenRFFile(int fileType, LPCTSTR filename, LPCTSTR mode);

	/**
	 * @brief Use geVFile for RF file opening
	 */
	bool		OpenRFFile(geVFile **fp, int fileType, LPCTSTR filename, int mode);

	/**
	 * @brief Check if the given file exists
	 */
	bool		FileExist(int fileType, LPCTSTR filename);

	/**
	 * @brief Delete RF file
	 */
	bool		DeleteRFFile(int fileType, LPCTSTR filename);

	LPCTSTR		GetDirectory(int directoryType);	///< Get configured directory
	geVFile*	GetVFS() { return m_VFS; }
	void SetLevelDirectory(LPCTSTR dir)
						{ if(dir)	strcpy(m_LevelDirectory, dir); }

	static CFileManager* GetSingletonPtr();
	static void Destroy();
	void Create(LPTSTR vFilename);

private:
	int CreateDataDirectory(int csidl);
	int CreateDataSubDirectory(int csidl, LPCTSTR subDir, LPTSTR finalPath);

private:
	CFileManager();
	// The FileManager cannot be copied, so the copy constructor is private
	CFileManager(const CFileManager&) { }
	// The FileManager cannot be copied, so the assignment operator is private
	CFileManager& operator=(const CFileManager&);

private:
	static CFileManager*	m_FileManager;

	// Configuration data
	TCHAR		m_ActorDirectory[MAX_PATH];			///< Actor directory
	TCHAR		m_BitmapDirectory[MAX_PATH];		///< Bitmap directory
	TCHAR		m_LevelDirectory[MAX_PATH];			///< Level directory
	TCHAR		m_AudioDirectory[MAX_PATH];			///< Audio directory
	TCHAR		m_VideoDirectory[MAX_PATH];			///< Video directory
	TCHAR		m_AudioStreamDirectory[MAX_PATH];	///< Audio stream directory
	TCHAR		m_MIDIDirectory[MAX_PATH];			///< MIDI file directory
	TCHAR		m_SavegameDirectory[MAX_PATH];
	TCHAR		m_ScreenshotDirectory[MAX_PATH];
	TCHAR		m_ConfigDirectory[MAX_PATH];
	TCHAR		m_LogDirectory[MAX_PATH];
	TCHAR		m_GameTitle[80];
	geVFile*	m_VFS;
};

#endif // __RGF_FILEMANAGER_H_

/* ----------------------------------- END OF FILE ------------------------------------ */
