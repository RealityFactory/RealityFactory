/************************************************************************************//**
 * @file IniFile.cpp
 * @brief Implementation of the CIniFile class.
 * @author Adam Clauss
 *//*
 * Copyright (c) Adam Clauss
 * Email: pandcc3@comwerx.net
 * You may use this class/code as you wish in your programs. Feel free to
 * distribute it, and email suggested changes to me.
 ****************************************************************************************/

#include "RabidFramework.h"
#include <fstream>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/* ------------------------------------------------------------------------------------ */
//	Construction/Destruction
//	default constructor
/* ------------------------------------------------------------------------------------ */
CIniFile::CIniFile()
{
}

/* ------------------------------------------------------------------------------------ */
//	constructor, can specify pathname here instead of using SetPath later
/* ------------------------------------------------------------------------------------ */
CIniFile::CIniFile(std::string inipath)
{
	path = inipath;
}

/* ------------------------------------------------------------------------------------ */
//	default destructor
/* ------------------------------------------------------------------------------------ */
CIniFile::~CIniFile()
{

}

/* ------------------------------------------------------------------------------------ */
// Public Functions
/* ------------------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------------------ */
//	sets path of ini file to read and write from
/* ------------------------------------------------------------------------------------ */
void CIniFile::SetPath(std::string newpath)
{
	path = newpath;
}


/* ------------------------------------------------------------------------------------ */
//	reads ini file specified using CIniFile::SetPath()
//	returns true if successful, false otherwise
/* ------------------------------------------------------------------------------------ */
bool CIniFile::ReadFile()
{
// changed RF063
	std::string readinfo;
	int curkey = -1, curval = -1;
	std::string keyname, valuename, value;
	std::string temp;
	geVFile *MainFS;
	char szPath[132];
	char szInputLine[132];

	strcpy(szPath, path.c_str());

	if(!CCD->OpenRFFile(&MainFS, kInstallFile, szPath, GE_VFILE_OPEN_READONLY))
		return 0;

	while(geVFile_GetS(MainFS, szInputLine, 132) == GE_TRUE)
	{
		if(strlen(szInputLine) <= 1)
			readinfo = "";
		else
			readinfo = szInputLine;

		TrimRight(readinfo);

		if(readinfo != "")
		{
			//if a section heading
			if(readinfo[0] == '[' && readinfo[readinfo.length()-1] == ']')
			{
				keyname = readinfo;
				TrimLeft(keyname, "[");
				TrimRight(keyname, "]");
			}
			else
			{
// changed RF064
				if(readinfo[0] != ';')
				{
					if(readinfo.find("=") != -1)
					{
						valuename = readinfo.substr(0, readinfo.find("="));
						if(readinfo.size() > readinfo.find("=")+1)
							value = readinfo.substr(readinfo.find("=")+1);
						else
							value = "";
					}
					else
					{
						valuename = readinfo;
						value = "";
					}
					TrimLeft(valuename);
					TrimRight(valuename);
					TrimLeft(value);
					TrimRight(value);
// end change RF064
					SetValue(keyname,valuename,value);
				}
			}
		}
	}

	geVFile_Close(MainFS);

	return 1;
// end change RF063
}

/* ------------------------------------------------------------------------------------ */
//	writes data stored in class to ini file
/* ------------------------------------------------------------------------------------ */
void CIniFile::WriteFile()
{
	std::ofstream inifile;
	inifile.open(path.c_str());

	for(unsigned int keynum=0; keynum<names.size(); keynum++)
	{
		if(keys[keynum].names.size() != 0)
		{
			inifile << '[' << names[keynum] << ']' << std::endl;

			for(unsigned int valuenum=0; valuenum<keys[keynum].names.size(); valuenum++)
			{
				inifile << keys[keynum].names[valuenum] << "=" << keys[keynum].values[valuenum];

				if(valuenum != keys[keynum].names.size()-1)
					inifile << std::endl;
				else if(keynum < names.size())
					inifile << std::endl;
			}

			if(keynum < names.size())
				inifile << std::endl;
		}
	}

	inifile.close();
}

/* ------------------------------------------------------------------------------------ */
//	deletes all stored ini data
/* ------------------------------------------------------------------------------------ */
void CIniFile::Reset()
{
	keys.resize(0);
	names.resize(0);
}

/* ------------------------------------------------------------------------------------ */
//	returns number of keys currently in the ini
/* ------------------------------------------------------------------------------------ */
int CIniFile::GetNumKeys()
{
	return keys.size();
}

/* ------------------------------------------------------------------------------------ */
//	returns number of values stored for specified key, or -1 if key found
/* ------------------------------------------------------------------------------------ */
int CIniFile::GetNumValues(std::string keyname)
{
	int keynum = FindKey(keyname);

	if(keynum == -1)
		return -1;
	else
		return keys[keynum].names.size();
}

/* ------------------------------------------------------------------------------------ */
//	gets value of [keyname] valuename =
//	overloaded to return CString, int, and double
/* ------------------------------------------------------------------------------------ */
std::string CIniFile::GetValue(std::string keyname, std::string valuename)
{
	int keynum = FindKey(keyname), valuenum = FindValue(keynum, valuename);

	if(keynum == -1)
	{
		error = "Unable to locate specified key.";
		return "";
	}

	if(valuenum == -1)
	{
		error = "Unable to locate specified value.";
		return "";
	}

	return keys[keynum].values[valuenum];
}

/* ------------------------------------------------------------------------------------ */
//	gets value of [keyname] valuename =
//	overloaded to return CString, int, and double
/* ------------------------------------------------------------------------------------ */
int CIniFile::GetValueI(std::string keyname, std::string valuename)
{
	return atoi(GetValue(keyname, valuename).c_str());
}

/* ------------------------------------------------------------------------------------ */
//	gets value of [keyname] valuename =
//	overloaded to return CString, int, and double
/* ------------------------------------------------------------------------------------ */
double CIniFile::GetValueF(std::string keyname, std::string valuename)
{
	return atof(GetValue(keyname, valuename).c_str());
}

/* ------------------------------------------------------------------------------------ */
//	sets value of [keyname] valuename =.
//	specify the optional paramter as false (0) if you do not want it to create
//	the key if it doesn't exist. Returns true if data entered, false otherwise
//	overloaded to accept CString, int, and double
/* ------------------------------------------------------------------------------------ */
bool CIniFile::SetValue(std::string keyname, std::string valuename, std::string value, bool create)
{
	int keynum = FindKey(keyname), valuenum = 0;

	//find key
	if(keynum == -1)	//if key doesn't exist
	{
		if(!create)		//and user does not want to create it,
			return 0;	//stop entering this key

		names.resize(names.size()+1);
		keys.resize(keys.size()+1);
		keynum = names.size()-1;
		names[keynum] = keyname;
	}

	//find value
	valuenum = FindValue(keynum,valuename);
	if(valuenum == -1)
	{
		if (!create)
			return 0;

		keys[keynum].names.resize(keys[keynum].names.size()+1);
		keys[keynum].values.resize(keys[keynum].names.size()+1);
		valuenum = keys[keynum].names.size()-1;
		keys[keynum].names[valuenum] = valuename;
	}

	keys[keynum].values[valuenum] = value;
	return 1;
}

/* ------------------------------------------------------------------------------------ */
//	sets value of [keyname] valuename =.
//	specify the optional paramter as false (0) if you do not want it to create
//	the key if it doesn't exist. Returns true if data entered, false otherwise
//	overloaded to accept CString, int, and double
/* ------------------------------------------------------------------------------------ */
bool CIniFile::SetValueI(std::string keyname, std::string valuename, int value, bool create)
{
	std::string temp;
	std::ostringstream oss;
	oss << value;
	temp = oss.str();

	return SetValue(keyname, valuename, temp, create);
}

/* ------------------------------------------------------------------------------------ */
//	sets value of [keyname] valuename =.
//	specify the optional paramter as false (0) if you do not want it to create
//	the key if it doesn't exist. Returns true if data entered, false otherwise
//	overloaded to accept CString, int, and double
/* ------------------------------------------------------------------------------------ */
bool CIniFile::SetValueF(std::string keyname, std::string valuename, double value, bool create)
{
	std::string temp;
	std::ostringstream oss;
	oss << value;
	temp = oss.str();

	return SetValue(keyname, valuename, temp, create);
}

/* ------------------------------------------------------------------------------------ */
//	deletes specified value
//	returns true if value existed and deleted, false otherwise
/* ------------------------------------------------------------------------------------ */
bool CIniFile::DeleteValue(std::string keyname, std::string valuename)
{
	int keynum = FindKey(keyname), valuenum = FindValue(keynum, valuename);

	if(keynum == -1 || valuenum == -1)
		return 0;

	keys[keynum].names.erase(keys[keynum].names.begin() + valuenum);
	keys[keynum].values.erase(keys[keynum].values.begin() + valuenum);
	return 1;
}

/* ------------------------------------------------------------------------------------ */
//	deletes specified key and all values contained within
//	returns true if key existed and deleted, false otherwise
/* ------------------------------------------------------------------------------------ */
bool CIniFile::DeleteKey(std::string keyname)
{
	int keynum = FindKey(keyname);

	if(keynum == -1)
		return 0;

	keys.erase(keys.begin() + keynum);
	names.erase(names.begin() + keynum);
	return 1;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
std::string CIniFile::FindFirstKey()
{
	numkey = 0;

	if(numkey < keys.size())
		return names[numkey];
	else
		return "";
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
std::string CIniFile::FindNextKey()
{
	numkey +=1;

	if(numkey < keys.size())
		return names[numkey];
	else
		return "";
}

// changed RF064
/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
std::string CIniFile::FindFirstName(std::string keyname)
{
	entrykey = 0;
	keyindex = FindKey(keyname);

	if(keyindex == -1)
		return "";

	if(entrykey < keys[keyindex].names.size())
		return keys[keyindex].names[entrykey];

	return "";
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
std::string CIniFile::FindFirstValue()
{
	if(keyindex == -1)
		return "";

	if(entrykey < keys[keyindex].values.size())
		return keys[keyindex].values[entrykey];

	return "";
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
std::string CIniFile::FindNextName()
{
	if(keyindex == -1)
		return "";

	entrykey += 1;

	if(entrykey < keys[keyindex].names.size())
		return keys[keyindex].names[entrykey];

	return "";
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
std::string CIniFile::FindNextValue()
{
	if(keyindex == -1)
		return "";

	if(entrykey < keys[keyindex].values.size())
		return keys[keyindex].values[entrykey];

	return "";
}
// end change RF064


/* ------------------------------------------------------------------------------------ */
// Private Functions
/* ------------------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------------------ */
//	returns index of specified key, or -1 if not found
/* ------------------------------------------------------------------------------------ */
int CIniFile::FindKey(std::string keyname)
{
	unsigned int keynum = 0;

	while(keynum < keys.size() && names[keynum] != keyname)
		keynum++;

	if(keynum == keys.size())
		return -1;

	return keynum;
}

/* ------------------------------------------------------------------------------------ */
//	returns index of specified value, in the specified key, or -1 if not found
/* ------------------------------------------------------------------------------------ */
int CIniFile::FindValue(int keynum, std::string valuename)
{
	if(keynum == -1)
		return -1;

	unsigned int valuenum = 0;

	while(valuenum < keys[keynum].names.size() && keys[keynum].names[valuenum] != valuename)
		valuenum++;

	if(valuenum == keys[keynum].names.size())
		return -1;

	return valuenum;
}

/* ------------------------------------------------------------------------------------ */
//	overloaded from original getline to take CString
/* ------------------------------------------------------------------------------------ */
/*istream& CIniFile:: getline(istream &is, CString &str)
{
    char buf[2048];
    is.getline(buf, 2048);
    str = buf;
    return is;
}*/

/* ----------------------------------- END OF FILE ------------------------------------ */
