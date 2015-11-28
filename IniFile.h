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

using namespace std;


class CIniFile
{
	//all private variables
private:

	//stores pathname of ini file to read/write
	string path;

	unsigned int numkey;
// changed RF064
	unsigned int entrykey;
	unsigned int keyindex;
// end change RF064

	//all keys are of this time
	struct key
	{
		//list of values in key
		vector<string> values;

		//corresponding list of value names
		vector<string> names;
	};

	//list of keys in ini
	vector<key> keys;

	//corresponding list of keynames
	vector<string> names;


	//all private functions
private:
	//overloaded to take CString

	//returns index of specified value, in the specified key, or -1 if not found
	int FindValue(int keynum, string valuename);

	//returns index of specified key, or -1 if not found
	int FindKey(string keyname);

	//public variables
public:

	//will contain error info if one occurs
	//ended up not using much, just in ReadFile and GetValue
	string error;


	//public functions
public:
	//default constructor
	CIniFile();

	//constructor, can specify pathname here instead of using SetPath later
	CIniFile(string inipath);

	//default destructor
	virtual ~CIniFile();

	//sets path of ini file to read and write from
	void SetPath(string newpath);

	//reads ini file specified using CIniFile::SetPath()
	//returns true if successful, false otherwise
	bool ReadFile();

	//writes data stored in class to ini file
	void WriteFile();

	//deletes all stored ini data
	void Reset();

	//returns number of keys currently in the ini
	int GetNumKeys();

	//returns number of values stored for specified key
	int GetNumValues(string keyname);

	//gets value of [keyname] valuename =
	//overloaded to return CString, int, and double,
	//returns "", or 0 if key/value not found.  Sets error member to show problem
	string GetValue(string keyname, string valuename);
	int GetValueI(string keyname, string valuename);
	double GetValueF(string keyname, string valuename);

	//sets value of [keyname] valuename =.
	//specify the optional paramter as false (0) if you do not want it to create
	//the key if it doesn't exist. Returns true if data entered, false otherwise
	//overloaded to accept CString, int, and double
	bool SetValue(string key, string valuename, string value, bool create = 1);
	bool SetValueI(string key, string valuename, int value, bool create = 1);
	bool SetValueF(string key, string valuename, double value, bool create = 1);

	//deletes specified value
	//returns true if value existed and deleted, false otherwise
	bool DeleteValue(string keyname, string valuename);

	//deletes specified key and all values contained within
	//returns true if key existed and deleted, false otherwise
	bool DeleteKey(string keyname);

// changed RF064
// end change RF064
	string FindFirstKey();
	string FindNextKey();
	string FindFirstName(string keyname);
	string FindFirstValue();
	string FindNextName();
	string FindNextValue();
};

#endif // !defined(AFX_INIFILE_H__D6BE0D97_13A8_11D4_A5D2_002078B03530__INCLUDED_)

/* ----------------------------------- END OF FILE ------------------------------------ */
