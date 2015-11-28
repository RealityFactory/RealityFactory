/************************************************************************************//**
 * @file IniFile.h
 * @brief Interface for the CIniFile class.
 * @author Adam Clauss
 *//*
 * Copyright (c) Adam Clauss
 * Email: pandcc3@comwerx.net
 * You may use this class/code as you wish in your programs. Feel free to
 * distribute it, and email suggested changes to me.
 ****************************************************************************************/

#ifndef __RGF_INIFILE_H_
#define __RGF_INIFILE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

/**
 * @brief CIniFile class for parsing INI files
 */
class CIniFile
{
	// all private variables
private:

	std::string path;	///< stores pathname of ini file to read/write

	unsigned int numkey;
// changed RF064
	unsigned int entrykey;
	unsigned int keyindex;
// end change RF064

	/**
	 * @brief all keys are of this time
	 */
	struct key
	{
		std::vector<std::string> values;	///< list of values in key
		std::vector<std::string> names;		///< corresponding list of value names
	};

	std::vector<key> keys;			///< list of keys in ini
	std::vector<std::string> names;	///< corresponding list of keynames


	// all private functions
private:
	/**
	 * returns index of specified value, in the specified key, or -1 if not found
	 */
	int FindValue(int keynum, std::string valuename);

	/**
	 * returns index of specified key, or -1 if not found
	 */
	int FindKey(std::string keyname);

	// public variables
public:
	/**
	 * will contain error info if one occurs
	 * ended up not using much, just in ReadFile and GetValue
	 */
	std::string error;

	// public functions
public:
	/**
	 * default constructor
	 */
	CIniFile();

	/**
	 * constructor, can specify pathname here instead of using SetPath later
	 */
	CIniFile(std::string inipath);

	/**
	 * default destructor
	 */
	virtual ~CIniFile();

	/**
	 * sets path of ini file to read and write from
	 */
	void SetPath(std::string newpath);

	/**
	 * reads ini file specified using CIniFile::SetPath()
	 * returns true if successful, false otherwise
	 */
	bool ReadFile();

	/**
	 * writes data stored in class to ini file
	 */
	void WriteFile();

	/**
	 * deletes all stored ini data
	 */
	void Reset();

	/**
	 * returns number of keys currently in the ini
	 */
	int GetNumKeys();

	/**
	 * returns number of values stored for specified key
	 */
	int GetNumValues(std::string keyname);

	/**
	 * gets value of [keyname] valuename =
	 * overloaded to return std::string, int, and double,
	 * returns "", or 0 if key/value not found. Sets error member to show problem
	 */
	std::string GetValue(std::string keyname, std::string valuename);
	int GetValueI(std::string keyname, std::string valuename);
	double GetValueF(std::string keyname, std::string valuename);

	/**
	 * sets value of [keyname] valuename =.
	 * specify the optional paramter as false (0) if you do not want it to create
	 * the key if it doesn't exist. Returns true if data entered, false otherwise
	 * overloaded to accept CString, int, and double
	 */
	bool SetValue(std::string key, std::string valuename, std::string value, bool create = 1);
	bool SetValueI(std::string key, std::string valuename, int value, bool create = 1);
	bool SetValueF(std::string key, std::string valuename, double value, bool create = 1);

	/**
	 * deletes specified value
	 * returns true if value existed and deleted, false otherwise
	 */
	bool DeleteValue(std::string keyname, std::string valuename);

	/**
	 * deletes specified key and all values contained within
	 * returns true if key existed and deleted, false otherwise
	 */
	bool DeleteKey(std::string keyname);

	std::string FindFirstKey();
	std::string FindNextKey();
	std::string FindFirstName(std::string keyname);
	std::string FindFirstValue();
	std::string FindNextName();
	std::string FindNextValue();
};

#endif // !defined(__RGF_INIFILE_H_)

/* ----------------------------------- END OF FILE ------------------------------------ */
