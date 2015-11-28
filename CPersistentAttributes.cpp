/****************************************************************************************/
/*																						*/
/*	CPersistentAttributes.cpp:		Persistent attributes class							*/
/*																						*/
/*	(c) 2001 Ralph Deane																*/
/*																						*/
/*	This file contains the implementation of the CPersistentAttribute					*/
/*	class, which holds attribute data that persists for entities across					*/
/*	level boundaries, and may also be saved and restored to/from a file.				*/
/*																						*/
/*	The concept here is that attribute lists can be associate with						*/
/*	ANYTHING, and maintained at a higher level than the individual						*/
/*	object in case things have to persist (like player attributes and					*/
/*	inventory).																			*/
/*																						*/
/*	Edit History:																		*/
/*	=============																		*/
/*	07/15/06 QD:	- BUG FIX															*/
/*	12/15/05 QD:	- Add PowerUp Level													*/
/*																						*/
/****************************************************************************************/

//	The One True Include File
#include "RabidFramework.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/* ------------------------------------------------------------------------------------ */
//	Default constructor
/* ------------------------------------------------------------------------------------ */
CPersistentAttributes::CPersistentAttributes()
{
	theList = NULL;					// Fast & easy.
	m_nCount = 0;					// Nothing here.

	return;
}

/* ------------------------------------------------------------------------------------ */
//	Default destructor
/* ------------------------------------------------------------------------------------ */
CPersistentAttributes::~CPersistentAttributes()
{
	Clear();

	return;
}

/* ------------------------------------------------------------------------------------ */
//	Clear
//
//	Delete all attributes in this object
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::Clear()
{
	if(theList == NULL)
		return RGF_SUCCESS;					// Already empty, cool!

	PersistAttribute *pTemp = theList;
	PersistAttribute *pNextOne = NULL;

	while(pTemp != NULL)
	{
		pNextOne = pTemp->pNext;			// Save off pointer to next
		delete pTemp->Name;					// Gun the name

		if(pTemp->UserData)
			delete pTemp->UserData;

		delete pTemp;						// ZAP!  It's dead, Jim.
		pTemp = pNextOne;					// On to the next victim
	}

	theList = NULL;							// Gone forever.
	m_nCount = 0;							// List out.

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	SetValueLimits
//
//	Set LOW and HIGH limits for attribute values
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::SetValueLimits(char *szTag, int Low, int High)
{
	PersistAttribute *pAttr = Locate(szTag);

	if(pAttr == NULL)
		return RGF_NOT_FOUND;				// Not found

	pAttr->ValueLowLimit = Low;
	pAttr->ValueHighLimit = High;

	return RGF_SUCCESS;						// Limits set
}

// changed QD 12/15/05
/* ------------------------------------------------------------------------------------ */
//	SetValueLimits
//
//	Set HIGH limit for attribute values
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::SetHighLimit(char *szTag, int HighLimit)
{
	PersistAttribute *pAttr = Locate(szTag);

	if(pAttr == NULL)
		return RGF_NOT_FOUND;				// Not found

	pAttr->ValueHighLimit = HighLimit;
	pAttr->PowerUpLevel++;

	return RGF_SUCCESS;						// Limits set
}
// end change

/* ------------------------------------------------------------------------------------ */
//	Set
//
//	Locate the attribute identified by the tag and set its value to the desired value.
//	If the attribute doesn't exist, create it and set the value.
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::Set(char *szTag, int nValue)
{
	PersistAttribute *pAttr = Locate(szTag);

	if(pAttr == NULL)
		return RGF_NOT_FOUND;				// Not found

	pAttr->Value = nValue;					// Set it, and forget it

	ClampValue(pAttr);						// Clamp the values to limits

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	SetIf
//
//	Locate the attribute identified by the tag.  If it doesn't exist, return
//	..an eror.  If it DOES exist, perform the test indicated by 'nHow'
//	..and if the result is TRUE, set the attributes value to the desired value.
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::SetIf(char *szTag, int nHow, int nCompareValue, int nValue)
{
	PersistAttribute *pAttr = Locate(szTag);

	if(pAttr == NULL)
		return RGF_NOT_FOUND;				// Not found

	if(LocalCompare(pAttr, nHow, nCompareValue))
		pAttr->Value = nValue;				// Set it, and forget it

	ClampValue(pAttr);						// Clamp the values to limits

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	Modify
//
//	Locate the attribute identified by the tag.  If it doesn't exist, return
//	..an error.  If it DOES exist, add 'nValue' to the attribute value.
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::Modify(char *szTag, int nValue)
{
	PersistAttribute *pAttr = Locate(szTag);

	if(pAttr == NULL)
		return RGF_NOT_FOUND;			// No such attribute

	pAttr->Value += nValue;				// MAke the mod

// changed RF064
	pAttr->ModifyAmt = nValue;
// end change RF064

    ClampValue(pAttr);					// Clamp the values to limits

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	ModifyIf
//
//	Locate the attribute identifided by the tag.  If it doesn't exist, return
//	..an error.  If it does exist, perform the comparison indicated by
//	..'nHow' and if true modify the value by adding 'nValue' to the attribute value.
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::ModifyIf(char *szTag, int nHow, int nCompareValue, int nValue)
{
	PersistAttribute *pAttr = Locate(szTag);

	if(pAttr == NULL)
		return RGF_NOT_FOUND;			// No such attribute

	if(LocalCompare(pAttr, nHow, nCompareValue))
		pAttr->Value += nValue;

	ClampValue(pAttr);					// Clamp the values to limits

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	Add
//
//	Add a new attribute to the attribute list.  If the attribute already exists,
//	..don't return an error but don't perform the add.
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::Add(char *szTag)
{
	PersistAttribute *pAttr = Locate(szTag);

	if(pAttr != NULL)
	{
		pAttr->Count++;						// Another one of these...
		return RGF_SUCCESS;					// Already there, ignore call
	}

	AddNew(szTag, 0);						// Add new attribute

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	AddAndSet
//
//	Add a new attribute to the attribute list.  If the attribute already exists,
//	..don't return an error but don't perform the add.
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::AddAndSet(char *szTag, int nValue)
{
	PersistAttribute *pAttr = Locate(szTag);

	if(pAttr != NULL)
	{
		pAttr->Value = nValue;
		pAttr->Count++;						// Another one of these...
		return RGF_SUCCESS;					// Already there, ignore call
	}

	AddNew(szTag, nValue);					// Add new attribute with value

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	AddIf
//
//	Add a new attribute to the list if the comparison indicated by 'nHow'
//	..with the value of attribute 'szWhat' and 'nCompareValue' is true.
//	..If the attribute already exists, take no action but return no error.
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::AddIf(char *szTag, int nHow, int nCompareValue, char *szWhat)
{
	PersistAttribute *pAttr = Locate(szWhat);

	if(pAttr == NULL)
		return RGF_NOT_FOUND;				// Attribute not found

	if(LocalCompare(pAttr, nHow, nCompareValue))
	{
		pAttr = Locate(szTag);				// Is it there?

		if(pAttr != NULL)
			return RGF_SUCCESS;				// Already exists

		AddNew(szTag, 0);					// Add a new attribute
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	Remove
//
//	Remove an instance of the attribute 'szTag' from the list.  If the last
//	..instance is removed, delete the attribute from the list.
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::Remove(char *szTag)
{
	PersistAttribute *pAttr = Locate(szTag);

	if(pAttr == NULL)
		return RGF_NOT_FOUND;				// No such attribute

	pAttr->Count--;							// Decrement instance count

	if(pAttr->Count <= 0)
		Delete(szTag);						// None left, wipe it out.

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	RemoveIf
//
//	Remove an instance of the attribute 'szTag' if the compare indicated
//	..by 'nHow' and 'nCompareValue' is true.  If the last instance is
//	..removed, delete the attribute from the attribute list.
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::RemoveIf(char *szTag, int nHow, int nCompareValue)
{
	PersistAttribute *pAttr = Locate(szTag);

	if(pAttr == NULL)
		return RGF_NOT_FOUND;				// Not found.

	if(LocalCompare(pAttr, nHow, nCompareValue))
	{
		pAttr->Count--;						// Decrement instance count

		if(pAttr->Count <= 0)
			Delete(szTag);					// Down to zero, drop it.
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	RemoveAll
//
//	Delete all instances of the indicated attribute from the list.
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::RemoveAll(char *szTag)
{
	return Delete(szTag);
}

/* ------------------------------------------------------------------------------------ */
//	Has
//
//	Return true if the list has at least one instance of the attribute.
/* ------------------------------------------------------------------------------------ */
bool CPersistentAttributes::Has(char *szTag)
{
	if(Locate(szTag) != NULL)
		return true;
	else
		return false;
}

/* ------------------------------------------------------------------------------------ */
//	Value
//
//	Return the value of an attribute, if it exists, otherwise 0.
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::Value(char *szTag)
{
	PersistAttribute *pAttr = Locate(szTag);

	if(pAttr == NULL)
		return 0;
	else
		return pAttr->Value;
}

// changed RF064
/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::GetModifyAmt(char *szTag)
{
	PersistAttribute *pAttr = Locate(szTag);

	if(pAttr == NULL)
		return 0;
	else
		return pAttr->ModifyAmt;
}
// end change RF064

// changed QD 12/15/05
/* ------------------------------------------------------------------------------------ */
//	GetPowerUpLevel
//
//	Get number of highlimit changes (PowerUp Level)
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::GetPowerUpLevel(char *szTag)
{
	PersistAttribute *pAttr = Locate(szTag);

	if(pAttr == NULL)
		return 0;
	else
		return pAttr->PowerUpLevel;
}
// end change

/* ------------------------------------------------------------------------------------ */
//	Low
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::Low(char *szTag)
{
	PersistAttribute *pAttr = Locate(szTag);

	if(pAttr == NULL)
		return 0;
	else
		return pAttr->ValueLowLimit;
}

/* ------------------------------------------------------------------------------------ */
//	High
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::High(char *szTag)
{
	PersistAttribute *pAttr = Locate(szTag);

	if(pAttr == NULL)
		return 0;
	else
		return pAttr->ValueHighLimit;
}

/* ------------------------------------------------------------------------------------ */
//	Count
//
//	Return the count of instance of an attribute, otherwise 0.
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::Count(char *szTag)
{
	PersistAttribute *pAttr = Locate(szTag);

	if(pAttr == NULL)
		return 0;
	else
		return pAttr->Count;
}

/* ------------------------------------------------------------------------------------ */
//	Compare
//
//	Perform a comparison between the value of the attribute 'szTag' and
//	..'nCompareValue' of the type indicated by 'nHow'.  Return the status
//	..of the compare, true or false.
/* ------------------------------------------------------------------------------------ */
bool CPersistentAttributes::Compare(char *szTag, int nHow, int nCompareValue)
{
	bool bResult = false;
	PersistAttribute *pAttr = Locate(szTag);

	if(pAttr == NULL)
		return false;								// Attribute not found

	return LocalCompare(pAttr, nHow, nCompareValue);
}

/* ------------------------------------------------------------------------------------ */
//	GetAttribute
//
//	Iterate through the attribute list.  If pPrevious is NULL, then we get the
//	..list head, otherwise we get the pNext element.
/* ------------------------------------------------------------------------------------ */
PersistAttribute *CPersistentAttributes::GetAttribute(PersistAttribute *pPrevious)
{
	if(pPrevious == NULL)
		return theList;
	else
		return pPrevious->pNext;
}

/* ------------------------------------------------------------------------------------ */
//	AllocateUserData
//
//	Allocate user data to be associated with an attribute
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::AllocateUserData(char *szTag, int nDataSize)
{
	PersistAttribute *pAttr = Locate(szTag);

	if(pAttr == NULL)
		return RGF_FAILURE;								// Attribute not found

	if(pAttr->UserData)
		delete pAttr->UserData;

	pAttr->UserData = new unsigned char[nDataSize];
	pAttr->UserDataSize = nDataSize;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	DeleteUserData
//
//	Delete the user data associated with an attribute.
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::DeleteUserData(char *szTag)
{
	PersistAttribute *pAttr = Locate(szTag);

	if(pAttr == NULL)
		return RGF_FAILURE;								// Attribute not found

	delete pAttr->UserData;
	pAttr->UserData = NULL;
	pAttr->UserDataSize = 0;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	UserData
//
//	Return a pointer to the user data associated with an attribute
/* ------------------------------------------------------------------------------------ */
unsigned char *CPersistentAttributes::UserData(char *szTag)
{
	PersistAttribute *pAttr = Locate(szTag);

	if(pAttr == NULL)
		return NULL;								// Attribute not found

	return pAttr->UserData;
}

/* ------------------------------------------------------------------------------------ */
//	SaveTo
//
//	Save all attributes in this object to the supplied file.
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::SaveTo(FILE *SaveFD, bool type)
{
	int nLen = 0;
	PersistAttribute *pAttr = theList;

	fwrite(&m_nCount, sizeof(int), 1, SaveFD);

	while(pAttr != NULL)
	{
		nLen = strlen(pAttr->Name) + 1;

		WRITEDATA(&nLen,					sizeof(int), 1, SaveFD);
		WRITEDATA(pAttr->Name,				1,		  nLen, SaveFD);
		WRITEDATA(&(pAttr->Count),			sizeof(int), 1, SaveFD);
		WRITEDATA(&(pAttr->Value),			sizeof(int), 1, SaveFD);
		WRITEDATA(&(pAttr->ValueLowLimit),	sizeof(int), 1, SaveFD);
		WRITEDATA(&(pAttr->ValueHighLimit),	sizeof(int), 1, SaveFD);
// changed QD 12/15/05
		WRITEDATA(&(pAttr->PowerUpLevel),		sizeof(int), 1, SaveFD);
// end change
		WRITEDATA(&(pAttr->UserDataSize),		sizeof(int), 1, SaveFD);

		if(pAttr->UserDataSize != 0)
			WRITEDATA(&(pAttr->UserData), 1, pAttr->UserDataSize, SaveFD);

		pAttr = pAttr->pNext;				// Next item
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	SaveAscii
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::SaveAscii(FILE *SaveFD)
{
	PersistAttribute *pAttr = theList;
	char szTemp[64];

	while(pAttr != NULL)
	{
		fputs(pAttr->Name, SaveFD); fputs("\n", SaveFD);

		sprintf(szTemp, "%d", pAttr->Value);

		fputs(szTemp, SaveFD); fputs("\n", SaveFD);

		sprintf(szTemp, "%d", pAttr->ValueLowLimit);

		fputs(szTemp, SaveFD); fputs("\n", SaveFD);

		sprintf(szTemp, "%d", pAttr->ValueHighLimit);

		fputs(szTemp, SaveFD); fputs("\n", SaveFD);

		pAttr = pAttr->pNext;				// Next item
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	RestoreFrom
//
//	Restore the attributes in this object from the supplied file.
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::RestoreFrom(FILE *RestoreFD, bool type)
{
	char szTempTag[256];
	int nValue, nCount, nLen, nUserDataSize, Low, High, PowerUpLevel;
	int nInFile;
	unsigned char *theUserData;
	PersistAttribute *pTemp = NULL;

	Clear();														// Zap everything first

	fread(&nInFile, sizeof(int), 1, RestoreFD);

	for(int nTemp = 0; nTemp < nInFile; nTemp++)
	{
		READDATA(&nLen,				sizeof(int), 1, RestoreFD);		// Tag size
		READDATA(&szTempTag,		1,		  nLen, RestoreFD);
		READDATA(&nCount,			sizeof(int), 1, RestoreFD);
		READDATA(&nValue,			sizeof(int), 1, RestoreFD);
		READDATA(&Low,				sizeof(int), 1, RestoreFD);
		READDATA(&High,				sizeof(int), 1, RestoreFD);
// changed QD 12/15/05
		READDATA(&PowerUpLevel,		sizeof(int), 1, RestoreFD);
// end change
		READDATA(&nUserDataSize,	sizeof(int), 1, RestoreFD);

		if(nUserDataSize != 0)
		{
			theUserData = new unsigned char[nUserDataSize];
			READDATA(&theUserData, 1, nUserDataSize, RestoreFD);
		}
		else
			theUserData = NULL;

		pTemp = AddNew(szTempTag, nValue);
		pTemp->ValueLowLimit = Low;
		pTemp->ValueHighLimit = High;
// changed QD 12/15/05
		pTemp->PowerUpLevel = PowerUpLevel;
// end change
		pTemp->Count = nCount;										// Adjust count
		pTemp->UserDataSize = nUserDataSize;
		pTemp->UserData = theUserData;
	}

	return RGF_SUCCESS;
}

//	************ PRIVATE MEMBER FUNCTIONS ***********

/* ------------------------------------------------------------------------------------ */
//	ClampValue
//
//	Clamp the value of an attribute to its limits
/* ------------------------------------------------------------------------------------ */
void CPersistentAttributes::ClampValue(PersistAttribute *pAttr)
{
	if(pAttr->Value < pAttr->ValueLowLimit)
		pAttr->Value = pAttr->ValueLowLimit;			// Clamp LOW limit

	if(pAttr->Value > pAttr->ValueHighLimit)
		pAttr->Value = pAttr->ValueHighLimit;			// Clamp HIGH limit

	return;
}

/* ------------------------------------------------------------------------------------ */
//	Locate
//
//	Search our internal list of a specific attribute, and return a
//	..pointer to it if it exists.
/* ------------------------------------------------------------------------------------ */
PersistAttribute *CPersistentAttributes::Locate(char *szTag)
{
	PersistAttribute *pTemp = theList;

	while(pTemp != NULL)
	{
		if(!strcmp(szTag, pTemp->Name))
			return pTemp;						// This is the one!

		pTemp = pTemp->pNext;					// Keep looking
	}

	return NULL;								// Couldn't find the attribute in our list.
}

/* ------------------------------------------------------------------------------------ */
//	AddNew
//
//	Add a new attribute on to the end of the list
/* ------------------------------------------------------------------------------------ */
PersistAttribute *CPersistentAttributes::AddNew(char *szTag, int nValue)
{
	PersistAttribute *pTemp = theList;

	if(pTemp != NULL)
	{
		while(pTemp->pNext != NULL)
			pTemp = pTemp->pNext;				// Slew to end of list
	}

	//	Construct the new attribute
	PersistAttribute *pNew = new PersistAttribute;
	pNew->Name = new char[strlen(szTag)+1];
	strcpy(pNew->Name, szTag);
	pNew->pNext = NULL;
	pNew->Count = 1;
	pNew->Value = nValue;
// changed RF064
	pNew->ModifyAmt = 0;
// end change RF064
// changed QD 12/15/05
	pNew->PowerUpLevel = 0;
// end change
// changed QD 07/15/05 - BUGFIX
	pNew->ValueLowLimit = 0;
	pNew->ValueHighLimit = 100;
// end change
	pNew->UserData = NULL;
	pNew->UserDataSize = 0;

	//	Check to see if this is the FIRST attribute
	if(pTemp == NULL)
		theList = pNew;							// This is the list head
	else
		pTemp->pNext = pNew;					// Add to the end of the list

	m_nCount++;									// List got bigger.

	return pNew;
}

/* ------------------------------------------------------------------------------------ */
//	Delete
//
//	Locate an attribute and delete it from the list.
/* ------------------------------------------------------------------------------------ */
int CPersistentAttributes::Delete(char *szTag)
{
	PersistAttribute *pTemp = theList;
	PersistAttribute *pPrev = NULL;

	while(pTemp != NULL)
	{
		if(!strcmp(szTag, pTemp->Name))
		{
			if(pPrev == NULL)
			{
				// Gunning the list head
				delete pTemp->Name;
				delete pTemp;
				theList = NULL;						// Attribute list now EMPTY
			}
			else
			{
				// Gunning Just Another Entry
				pPrev->pNext = pTemp->pNext;		// Patch around entry to be deleted
				delete pTemp->Name;
				delete pTemp;						// Attribute is DOA
			}

			m_nCount--;								// List got smaller
			return RGF_SUCCESS;						// Zapped.
		}

		pPrev = pTemp;								// Save previous
		pTemp = pTemp->pNext;						// Keep looking
	}

	return RGF_FAILURE;								// Couldn't find it
}

/* ------------------------------------------------------------------------------------ */
//	LocalCompare
//
//	Perform a compare with the passed-in PersistAttribute
/* ------------------------------------------------------------------------------------ */
bool CPersistentAttributes::LocalCompare(PersistAttribute *pAttr, int nHow,
										 int nCompareValue)
{
	bool bResult = false;

	switch(nHow)
	{
	case RGF_ATTR_EQUAL:
		if(pAttr->Value == nCompareValue)
			bResult = true;
		break;
	case RGF_ATTR_LESS:
		if(pAttr->Value < nCompareValue)
			bResult = true;
		break;
	case RGF_ATTR_GREATER:
		if(pAttr->Value > nCompareValue)
			bResult = true;
		break;
	case RGF_ATTR_NOTEQUAL:
		if(pAttr->Value != nCompareValue)
			bResult = true;
		break;
	case RGF_ATTR_LESSEQ:
		if(pAttr->Value <= nCompareValue)
			bResult = true;
		break;
	case RGF_ATTR_GREATEREQ:
		if(pAttr->Value >= nCompareValue)
			bResult = true;
		break;
	}

	return bResult;
}

/* ------------------------------------------------------------------------------------ */
//	Dump
//
//	Dump the contents of the attribute list to the debug window and the debug file.
//	Typically used for debugging.
/* ------------------------------------------------------------------------------------ */
void CPersistentAttributes::Dump()
{
	PersistAttribute *pTemp = theList;
	char szDump[256];

	while(pTemp != NULL)
	{
		sprintf(szDump, "%s count %d value %d", pTemp->Name, pTemp->Count, pTemp->Value);
		CCD->ReportError(szDump, false);
		pTemp = pTemp->pNext;
	}

	return;
}


/* ----------------------------------- END OF FILE ------------------------------------ */
