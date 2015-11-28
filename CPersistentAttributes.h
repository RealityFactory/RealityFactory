/************************************************************************************//**
 * @file CPersistentAttributes.h
 * @brief Persistent attributes class
 *
 * This file contains the declaration of the CPersistentAttribute class,
 * which holds attribute data that persists for entities across level
 * boundaries, and may also be saved and restored to/from a file.
 * @author Ralph Deane
 *//*
 * Copyright (c) 2001 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_CPERSISTENTATTRIBUTES_H__
#define __RGF_CPERSISTENTATTRIBUTES_H__

class CGenesisEngine;

/**
 * @brief Persistent Attribute
 */
struct PersistAttribute
{
	char				*Name;			///< Name of attribute
	int					Value;			///< Value of attribute
	int					ValueLowLimit;	///< Value low limit
	int					ValueHighLimit;	///< Value high limit
// changed RF064
	int					ModifyAmt;
// end change RF064
// changed QD 12/15/05
	int					PowerUpLevel;	///< number of highlimit changes
// end change
	int					Count;			///< Instance count
	int					UserDataSize;	///< Size of user data
	unsigned char		*UserData;		///< Pointer to user-defined data
	PersistAttribute	*pNext;			///< Pointer to next entity in list
};

/**
 * @brief CPersistentAttributes handles persitent attributes
 *
 * The concept here is that attribute lists can be associate with ANYTHING,
 * and maintained at a higher level than the individual	object in case things
 * have to persist (like player attributes and inventory).
 */
class CPersistentAttributes
{
public:
	CPersistentAttributes();				///< Default constructor
	~CPersistentAttributes();				///< Default destructor

	int Clear();															///< Clear all attributes
	int SetValueLimits(const char *szTag, int LowLimit, int HighLimit);		///< Set value limits
// changed QD 12/15/05
	int SetHighLimit(const char *szTag, int HighLimit);						///< Set value highlimit (Power Up)
// end change
	int Set(const char *szTag, int nValue);									///< Set attribute value
	int SetIf(const char *szTag, int nHow, int nCompareValue, int nValue);	///< Set attribute value if test condition true
	int Modify(const char *szTag, int nValue);								///< Modify value of attribute
	int ModifyIf(const char *szTag, int nHow, int nCompareValue, int nValue);	///< Modify if test condition true
	int Add(const char *szTag);												///< Add an instance of an attribute
	int AddAndSet(const char *szTag, int nValue);							///< Add attribute with value
	int AddIf(const char *szTag, int nHow, int nCompareValue, const char *szWhat);	///< Add instance if test condition true
	int Remove(const char *szTag);											///< Remove an instance of an attribute
	int RemoveIf(const char *szTag, int nHow, int nCompareValue);			///< Remove instance if test condition true
	int RemoveAll(const char *szTag);										///< Remove all instances of an attribute
	bool Has(const char *szTag);											///< Does attribute exist in list?
	int Value(const char *szTag);											///< Get attributes value
	int Low(const char *szTag);
	int High(const char *szTag);
	int Count(const char *szTag);											///< Get attribute count
	bool Compare(const char *szTag, int nHow, int nCompareValue);			///< Compare attribute value to a number
// changed RF064
	int GetModifyAmt(const char *szTag);
// end change RF064
// changed QD 12/15/05
	int GetPowerUpLevel(const char *szTag);							///< Get number of highlimit changes (PowerUp Level)
// end change
	PersistAttribute *GetAttribute(PersistAttribute *pPrevious);	///< Get first/next attribute in list
	int AllocateUserData(const char *szTag, int nDataSize);			///< Allocate some user data for attribute
	int DeleteUserData(const char *szTag);							///< Delete an attributes user data
	unsigned char *UserData(const char *szTag);						///< Get pointer to user data for attr
	int SaveTo(FILE *SaveFD, bool type);							///< Save attributes to a file
	int SaveAscii(FILE *SaveFD);
	int RestoreFrom(FILE *RestoreFD, bool type);					///< Restore attributes from a file
	void Dump();													///< Debug list dumper

private:
	//	Member functions
	void ClampValue(PersistAttribute *pAttr);	///< Clamp value to limits
	PersistAttribute *Locate(const char *szTag);
	PersistAttribute *AddNew(const char *szTag, int nValue);
	int Delete(const char *szTag);
	bool LocalCompare(PersistAttribute *pAttr, int nHow, int nCompareValue);
private:
	//	Member variables
	PersistAttribute *theList;					///< Attribute list
	int m_nCount;								///< Count of unique attributes in list
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
