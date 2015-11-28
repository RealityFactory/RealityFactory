/****************************************************************************************/
/*																						*/
/*	CPersistentAttributes.h:		Persistent attributes class							*/
/*																						*/
/*	(c) 2001 Ralph Deane																*/
/*																						*/
/*	This file contains the declaration of the CPersistentAttribute						*/
/*	class, which holds attribute data that persists for entities across					*/
/*	level boundaries, and may also be saved and restored to/from a file.				*/
/*																						*/
/****************************************************************************************/


#ifndef __RGF_CPERSISTENTATTRIBUTES_H__
#define __RGF_CPERSISTENTATTRIBUTES_H__

class CGenesisEngine;

struct PersistAttribute
{
	char				*Name;			// Name of attribute
	int					Value;			// Value of attribute
	int					ValueLowLimit;	// Value low limit
	int					ValueHighLimit;	// Value high limit
// changed RF064
	int					ModifyAmt;
// end change RF064
// changed QD 12/15/05
	int					PowerUpLevel;	// number of highlimit changes
// end change
	int					Count;			// Instance count
	int					UserDataSize;	// Size of user data
	unsigned char		*UserData;		// Pointer to user-defined data
	PersistAttribute	*pNext;			// Pointer to next entity in list
};

class CPersistentAttributes
{
public:
	CPersistentAttributes();				// Default constructor
	~CPersistentAttributes();				// Default destructor

	int Clear();														// Clear all attributes
	int SetValueLimits(char *szTag, int LowLimit, int HighLimit);		// Set value limits
// changed QD 12/15/05
	int SetHighLimit(char *szTag, int HighLimit);						// Set value highlimit (Power Up)
// end change
	int Set(char *szTag, int nValue);									// Set attribute value
	int SetIf(char *szTag, int nHow, int nCompareValue, int nValue);	// Set attribute value if test condition true
	int Modify(char *szTag, int nValue);								// Modify value of attribute
	int ModifyIf(char *szTag, int nHow, int nCompareValue, int nValue);	// Modify if test condition true
	int Add(char *szTag);												// Add an instance of an attribute
	int AddAndSet(char *szTag, int nValue);								// Add attribute with value
	int AddIf(char *szTag, int nHow, int nCompareValue, char *szWhat);	// Add instance if test condition true
	int Remove(char *szTag);											// Remove an instance of an attribute
	int RemoveIf(char *szTag, int nHow, int nCompareValue);				// Remove instance if test condition true
	int RemoveAll(char *szTag);											// Remove all instances of an attribute
	bool Has(char *szTag);												// Does attribute exist in list?
	int Value(char *szTag);												// Get attributes value
	int Low(char *szTag);
	int High(char *szTag);
	int Count(char *szTag);											// Get attribute count
	bool Compare(char *szTag, int nHow, int nCompareValue);			// Compare attribute value to a number
// changed RF064
	int GetModifyAmt(char *szTag);
// end change RF064
// changed QD 12/15/05
	int GetPowerUpLevel(char *szTag);								// Get number of highlimit changes (PowerUp Level)
// end change
	PersistAttribute *GetAttribute(PersistAttribute *pPrevious);	// Get first/next attribute in list
	int AllocateUserData(char *szTag, int nDataSize);				// Allocate some user data for attribute
	int DeleteUserData(char *szTag);								// Delete an attributes user data
	unsigned char *UserData(char *szTag);							// Get pointer to user data for attr
	int SaveTo(FILE *SaveFD, bool type);							// Save attributes to a file
	int SaveAscii(FILE *SaveFD);
	int RestoreFrom(FILE *RestoreFD, bool type);					// Restore attributes from a file
	void Dump();													// Debug list dumper

private:
	//	Member functions
	void ClampValue(PersistAttribute *pAttr);	// Clamp value to limits
	PersistAttribute *Locate(char *szTag);
	PersistAttribute *AddNew(char *szTag, int nValue);
	int Delete(char *szTag);
	bool LocalCompare(PersistAttribute *pAttr, int nHow, int nCompareValue);
private:
	//	Member variables
	PersistAttribute *theList;					// Attribute list
	int m_nCount;								// Count of unique attributes in list
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
