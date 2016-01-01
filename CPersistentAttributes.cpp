/************************************************************************************//**
 * @file CPersistentAttributes.cpp
 * @brief Persistent attributes class
 *
 * This file contains the implementation of the CPersistentAttribute class,
 * which holds attribute data that persists for entities across level
 * boundaries, and may also be saved and restored to/from a file.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#include "RabidFramework.h"

struct PersistAttribute
{
	int				Value;			///< Value of attribute
	int				ValueLowLimit;	///< Value low limit
	int				ValueHighLimit;	///< Value high limit
	int				ModifyAmt;
	int				PowerUpLevel;	///< Number of high limit changes
	int				Count;			///< Instance count
	int				UserDataSize;	///< Size of user data
	unsigned char*	UserData;		///< Pointer to user-defined data
};


/* ------------------------------------------------------------------------------------ */
// Default constructor
/* ------------------------------------------------------------------------------------ */
CPersistentAttributes::CPersistentAttributes()
{
}


/* ------------------------------------------------------------------------------------ */
// Default destructor
/* ------------------------------------------------------------------------------------ */
CPersistentAttributes::~CPersistentAttributes()
{
	Clear();
}


/* ------------------------------------------------------------------------------------ */
// Clear
//
// Delete all attributes in this object
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::Clear()
{
	stdext::hash_map<std::string, PersistAttribute*>::iterator iter = m_List.begin();
	for(; iter!=m_List.end(); ++iter)
	{
		delete[] (iter->second->UserData);
		delete (iter->second);
	}

	m_List.clear();

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// SetValueLimits
//
// Set LOW and HIGH limits for attribute values
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::SetValueLimits(const std::string& tag, int low, int high)
{
	PersistAttribute *pAttr = Locate(tag);

	if(pAttr == NULL)
		return RGF_NOT_FOUND;				// Not found

	pAttr->ValueLowLimit = low;
	pAttr->ValueHighLimit = high;

	return RGF_SUCCESS;						// Limits set
}


/* ------------------------------------------------------------------------------------ */
// SetValueLimits
//
// Set HIGH limit for attribute values
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::SetHighLimit(const std::string& tag, int highLimit)
{
	PersistAttribute *pAttr = Locate(tag);

	if(pAttr == NULL)
		return RGF_NOT_FOUND;				// Not found

	pAttr->ValueHighLimit = highLimit;
	pAttr->PowerUpLevel++;

	return RGF_SUCCESS;						// Limits set
}


/* ------------------------------------------------------------------------------------ */
// Set
//
// Locate the attribute identified by the tag and set its value to the desired value.
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::Set(const std::string& tag, int value)
{
	PersistAttribute *pAttr = Locate(tag);

	if(pAttr == NULL)
		return RGF_NOT_FOUND;				// Not found

	pAttr->Value = value;					// Set it, and forget it

	ClampValue(pAttr);						// Clamp the values to limits

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// SetIf
//
// Locate the attribute identified by the tag.  If it doesn't exist, return
// ..an error.  If it DOES exist, perform the test indicated by 'how'
// ..and if the result is TRUE, set the attributes value to the desired value.
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::SetIf(const std::string& tag, int how, int compareValue, int value)
{
	PersistAttribute *pAttr = Locate(tag);

	if(pAttr == NULL)
		return RGF_NOT_FOUND;				// Not found

	if(LocalCompare(pAttr, how, compareValue))
		pAttr->Value = value;				// Set it, and forget it

	ClampValue(pAttr);						// Clamp the values to limits

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// Modify
//
// Locate the attribute identified by the tag.  If it doesn't exist, return
// ..an error.  If it DOES exist, add 'value' to the attribute value.
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::Modify(const std::string& tag, int value)
{
	PersistAttribute *pAttr = Locate(tag);

	if(pAttr == NULL)
		return RGF_NOT_FOUND;			// No such attribute

	pAttr->Value += value;				// Make the mod

	pAttr->ModifyAmt = value;

	ClampValue(pAttr);					// Clamp the values to limits

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// ModifyIf
//
// Locate the attribute identifided by the tag.  If it doesn't exist, return
// ..an error.  If it does exist, perform the comparison indicated by
// ..'how' and if true modify the value by adding 'value' to the attribute value.
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::ModifyIf(const std::string& tag, int how, int compareValue, int value)
{
	PersistAttribute *pAttr = Locate(tag);

	if(pAttr == NULL)
		return RGF_NOT_FOUND;			// No such attribute

	if(LocalCompare(pAttr, how, compareValue))
		pAttr->Value += value;

	ClampValue(pAttr);					// Clamp the values to limits

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// Add
//
// Add a new attribute to the attribute list.  If the attribute already exists,
// ..don't return an error but don't perform the add.
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::Add(const std::string& tag)
{
	PersistAttribute *pAttr = Locate(tag);

	if(pAttr != NULL)
	{
		pAttr->Count++;						// Another one of these...
		return RGF_SUCCESS;					// Already there, ignore call
	}

	AddNew(tag, 0);							// Add new attribute

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// AddAndSet
//
// Add a new attribute to the attribute list.  If the attribute already exists,
// ..don't return an error but don't perform the add.
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::AddAndSet(const std::string& tag, int value)
{
	PersistAttribute *pAttr = Locate(tag);

	if(pAttr != NULL)
	{
		pAttr->Value = value;
		pAttr->Count++;						// Another one of these...
		return RGF_SUCCESS;					// Already there, ignore call
	}

	AddNew(tag, value);						// Add new attribute with value

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// AddIf
//
// Add a new attribute to the list if the comparison indicated by 'how'
// ..with the value of attribute 'what' and 'compareValue' is true.
// ..If the attribute already exists, take no action but return no error.
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::AddIf(const std::string& tag, int how, int compareValue, const std::string& what)
{
	PersistAttribute *pAttr = Locate(what);

	if(pAttr == NULL)
		return RGF_NOT_FOUND;				// Attribute not found

	if(LocalCompare(pAttr, how, compareValue))
	{
		pAttr = Locate(tag);				// Is it there?

		if(pAttr != NULL)
			return RGF_SUCCESS;				// Already exists

		AddNew(tag, 0);						// Add a new attribute
	}

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// Remove
//
// Remove an instance of the attribute 'tag' from the list.  If the last
// ..instance is removed, delete the attribute from the list.
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::Remove(const std::string& tag)
{
	PersistAttribute *pAttr = Locate(tag);

	if(pAttr == NULL)
		return RGF_NOT_FOUND;				// No such attribute

	pAttr->Count--;							// Decrement instance count

	if(pAttr->Count <= 0)
		Delete(tag);						// None left, wipe it out.

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// RemoveIf
//
// Remove an instance of the attribute 'tag' if the compare indicated
// ..by 'how' and 'compareValue' is true.  If the last instance is
// ..removed, delete the attribute from the attribute list.
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::RemoveIf(const std::string& tag, int how, int compareValue)
{
	PersistAttribute *pAttr = Locate(tag);

	if(pAttr == NULL)
		return RGF_NOT_FOUND;				// Not found.

	if(LocalCompare(pAttr, how, compareValue))
	{
		pAttr->Count--;						// Decrement instance count

		if(pAttr->Count <= 0)
			Delete(tag);					// Down to zero, drop it.
	}

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// RemoveAll
//
// Delete all instances of the indicated attribute from the list.
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::RemoveAll(const std::string& tag)
{
	return Delete(tag);
}


/* ------------------------------------------------------------------------------------ */
// Has
//
// Return true if the list has at least one instance of the attribute.
/* ------------------------------------------------------------------------------------ */
bool CPersistentAttributes::Has(const std::string& tag)
{
	if(Locate(tag) != NULL)
		return true;
	else
		return false;
}


/* ------------------------------------------------------------------------------------ */
// Value
//
// Return the value of an attribute, if it exists, otherwise 0.
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::Value(const std::string& tag)
{
	PersistAttribute *pAttr = Locate(tag);

	if(pAttr == NULL)
		return 0;
	else
		return pAttr->Value;
}


/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::GetModifyAmt(const std::string& tag)
{
	PersistAttribute *pAttr = Locate(tag);

	if(pAttr == NULL)
		return 0;
	else
		return pAttr->ModifyAmt;
}


/* ------------------------------------------------------------------------------------ */
// GetPowerUpLevel
//
// Get number of highlimit changes (PowerUp Level)
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::GetPowerUpLevel(const std::string& tag)
{
	PersistAttribute *pAttr = Locate(tag);

	if(pAttr == NULL)
		return 0;
	else
		return pAttr->PowerUpLevel;
}


/* ------------------------------------------------------------------------------------ */
// Low
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::Low(const std::string& tag)
{
	PersistAttribute *pAttr = Locate(tag);

	if(pAttr == NULL)
		return 0;
	else
		return pAttr->ValueLowLimit;
}


/* ------------------------------------------------------------------------------------ */
// High
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::High(const std::string& tag)
{
	PersistAttribute *pAttr = Locate(tag);

	if(pAttr == NULL)
		return 0;
	else
		return pAttr->ValueHighLimit;
}


/* ------------------------------------------------------------------------------------ */
// Count
//
// Return the count of instance of an attribute, otherwise 0.
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::Count(const std::string& tag)
{
	PersistAttribute *pAttr = Locate(tag);

	if(pAttr == NULL)
		return 0;
	else
		return pAttr->Count;
}


/* ------------------------------------------------------------------------------------ */
// Compare
//
// Perform a comparison between the value of the attribute 'tag' and
// ..'compareValue' of the type indicated by 'how'.  Return the status
// ..of the compare, true or false.
/* ------------------------------------------------------------------------------------ */
bool CPersistentAttributes::Compare(const std::string& tag, int how, int compareValue)
{
	PersistAttribute *pAttr = Locate(tag);

	if(pAttr == NULL)
		return false;								// Attribute not found

	return LocalCompare(pAttr, how, compareValue);
}


/* ------------------------------------------------------------------------------------ */
// AllocateUserData
//
// Allocate user data to be associated with an attribute
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::AllocateUserData(const std::string& tag, int dataSize)
{
	PersistAttribute *pAttr = Locate(tag);

	if(pAttr == NULL)
		return RGF_FAILURE;								// Attribute not found

	delete[] pAttr->UserData;

	pAttr->UserData = new unsigned char[dataSize];
	pAttr->UserDataSize = dataSize;

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// DeleteUserData
//
// Delete the user data associated with an attribute.
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::DeleteUserData(const std::string& tag)
{
	PersistAttribute *pAttr = Locate(tag);

	if(pAttr == NULL)
		return RGF_FAILURE;								// Attribute not found

	SAFE_DELETE_A(pAttr->UserData);
	pAttr->UserDataSize = 0;

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// UserData
//
// Return a pointer to the user data associated with an attribute
/* ------------------------------------------------------------------------------------ */
unsigned char* CPersistentAttributes::UserData(const std::string& tag)
{
	PersistAttribute *pAttr = Locate(tag);

	if(pAttr == NULL)
		return NULL;								// Attribute not found

	return pAttr->UserData;
}


/* ------------------------------------------------------------------------------------ */
// SaveTo
//
// Save all attributes in this object to the supplied file.
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::SaveTo(FILE* saveFD, bool type)
{
	int count = m_List.size();
	fwrite(&count, sizeof(int), 1, saveFD);

	stdext::hash_map<std::string, PersistAttribute*>::iterator iter = m_List.begin();
	for(; iter!=m_List.end(); ++iter)
	{
		int length = iter->first.length() + 1;

		WRITEDATA(type, &length,						sizeof(int), 1, saveFD);
		WRITEDATA(type, (void*)iter->first.c_str(),		length,      1, saveFD);
		WRITEDATA(type, &iter->second->Count,			sizeof(int), 1, saveFD);
		WRITEDATA(type, &iter->second->Value,			sizeof(int), 1, saveFD);
		WRITEDATA(type, &iter->second->ValueLowLimit,	sizeof(int), 1, saveFD);
		WRITEDATA(type, &iter->second->ValueHighLimit,	sizeof(int), 1, saveFD);
		WRITEDATA(type, &iter->second->PowerUpLevel,	sizeof(int), 1, saveFD);
		WRITEDATA(type, &iter->second->UserDataSize,	sizeof(int), 1, saveFD);

		if(iter->second->UserDataSize != 0)
			WRITEDATA(type, &iter->second->UserData, sizeof(unsigned char), iter->second->UserDataSize, saveFD);
	}

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// SaveAscii
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::SaveAscii(FILE* saveFD)
{
	char szTemp[128];

	stdext::hash_map<std::string, PersistAttribute*>::iterator iter = m_List.begin();
	for(; iter!=m_List.end(); ++iter)
	{
		fputs(iter->first.c_str(), saveFD); fputs("\n", saveFD);

		sprintf(szTemp, "%d", iter->second->Value);
		fputs(szTemp, saveFD); fputs("\n", saveFD);

		sprintf(szTemp, "%d", iter->second->ValueLowLimit);
		fputs(szTemp, saveFD); fputs("\n", saveFD);

		sprintf(szTemp, "%d", iter->second->ValueHighLimit);
		fputs(szTemp, saveFD); fputs("\n", saveFD);
	}

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// RestoreFrom
//
// Restore the attributes in this object from the supplied file.
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::RestoreFrom(FILE* restoreFD, bool type)
{
	Clear();														// Zap everything first

	int inFile;

	fread(&inFile, sizeof(int), 1, restoreFD);

	for(int i=0; i<inFile; ++i)
	{
		int length, count, value, low, high, powerUpLevel, userDataSize;

		READDATA(type, &length,			sizeof(int), 1, restoreFD);		// Tag size
		char *szTag = new char[length];
		READDATA(type, &szTag,			length,      1, restoreFD);
		READDATA(type, &count,			sizeof(int), 1, restoreFD);
		READDATA(type, &value,			sizeof(int), 1, restoreFD);
		READDATA(type, &low,			sizeof(int), 1, restoreFD);
		READDATA(type, &high,			sizeof(int), 1, restoreFD);
		READDATA(type, &powerUpLevel,	sizeof(int), 1, restoreFD);
		READDATA(type, &userDataSize,	sizeof(int), 1, restoreFD);

		unsigned char* userData;

		if(userDataSize != 0)
		{
			userData = new unsigned char[userDataSize];
			READDATA(type, &userData, 1, userDataSize, restoreFD);
		}
		else
		{
			userData = NULL;
		}

		PersistAttribute *pTemp = AddNew(szTag, value);
		pTemp->ValueLowLimit = low;
		pTemp->ValueHighLimit = high;
		pTemp->PowerUpLevel = powerUpLevel;
		pTemp->Count = count;										// Adjust count
		pTemp->UserDataSize = userDataSize;
		pTemp->UserData = userData;
		delete[] szTag;
	}

	return RGF_SUCCESS;
}


// ************ PRIVATE MEMBER FUNCTIONS ***********

/* ------------------------------------------------------------------------------------ */
// ClampValue
//
// Clamp the value of an attribute to its limits
/* ------------------------------------------------------------------------------------ */
void CPersistentAttributes::ClampValue(PersistAttribute *attr)
{
	if(attr->Value < attr->ValueLowLimit)
		attr->Value = attr->ValueLowLimit;			// Clamp LOW limit

	if(attr->Value > attr->ValueHighLimit)
		attr->Value = attr->ValueHighLimit;			// Clamp HIGH limit
}


/* ------------------------------------------------------------------------------------ */
// Locate
//
// Search our internal list of a specific attribute, and return a
// ..pointer to it if it exists.
/* ------------------------------------------------------------------------------------ */
PersistAttribute *CPersistentAttributes::Locate(const std::string& tag)
{
	if(m_List.find(tag) != m_List.end())
		return m_List[tag];

	return NULL;								// Couldn't find the attribute in our list.
}

/* ------------------------------------------------------------------------------------ */
// AddNew
//
// Add a new attribute to the list
/* ------------------------------------------------------------------------------------ */
PersistAttribute *CPersistentAttributes::AddNew(const std::string& tag, int value)
{
	// Construct the new attribute
	m_List[tag] = new PersistAttribute;
	m_List[tag]->Count = 1;
	m_List[tag]->Value = value;
	m_List[tag]->ModifyAmt = 0;
	m_List[tag]->PowerUpLevel = 0;
	m_List[tag]->ValueLowLimit = 0;
	m_List[tag]->ValueHighLimit = 100;
	m_List[tag]->UserData = NULL;
	m_List[tag]->UserDataSize = 0;

	return m_List[tag];
}


/* ------------------------------------------------------------------------------------ */
// Delete
//
// Locate an attribute and delete it from the list.
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::Delete(const std::string& tag)
{
	if(m_List.find(tag) != m_List.end())
	{
		delete[] m_List[tag]->UserData;
		delete m_List[tag];
		m_List.erase(tag);
		return RGF_SUCCESS;							// Zapped.
	}

	return RGF_FAILURE;								// Couldn't find it
}


/* ------------------------------------------------------------------------------------ */
// LocalCompare
//
// Perform a compare with the passed-in PersistAttribute
/* ------------------------------------------------------------------------------------ */
bool CPersistentAttributes::LocalCompare(PersistAttribute* attr, int how, int compareValue)
{
	switch(how)
	{
	case RGF_ATTR_EQUAL:
		return (attr->Value == compareValue);
	case RGF_ATTR_LESS:
		return (attr->Value < compareValue);
	case RGF_ATTR_GREATER:
		return (attr->Value > compareValue);
	case RGF_ATTR_NOTEQUAL:
		return (attr->Value != compareValue);
	case RGF_ATTR_LESSEQ:
		return (attr->Value <= compareValue);
	case RGF_ATTR_GREATEREQ:
		return (attr->Value >= compareValue);
	default:
		return false;
	}
}


/* ------------------------------------------------------------------------------------ */
// Dump
//
// Dump the contents of the attribute list to the debug window and the debug file.
// Typically used for debugging.
/* ------------------------------------------------------------------------------------ */
void CPersistentAttributes::Dump()
{
	stdext::hash_map<std::string, PersistAttribute*>::iterator iter = m_List.begin();
	for(; iter!=m_List.end(); ++iter)
	{
		sxLog::GetSingletonPtr()->Print("%s count %d value %d",
			iter->first.c_str(),
			iter->second->Count,
			iter->second->Value);
	}
}


/* ----------------------------------- END OF FILE ------------------------------------ */
