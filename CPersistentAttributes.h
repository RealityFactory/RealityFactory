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

#include <hash_map>

/**
 * @brief Persistent Attribute
 */
struct PersistAttribute;

/**
 * @brief CPersistentAttributes handles persistent attributes
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

	int Clear();																///< Clear all attributes
	int SetValueLimits(const std::string& tag, int lowLimit, int highLimit);	///< Set value limits
	int SetHighLimit(const std::string& tag, int highLimit);					///< Set value high limit (Power Up)
	int Set(const std::string& tag, int value);									///< Set attribute value
	int SetIf(const std::string& tag, int how, int compareValue, int value);	///< Set attribute value if test condition true
	int Modify(const std::string& tag, int value);								///< Modify value of attribute
	int ModifyIf(const std::string& tag, int how, int compareValue, int value);	///< Modify if test condition true
	int Add(const std::string& tag);											///< Add an instance of an attribute
	int AddAndSet(const std::string& tag, int value);							///< Add attribute with value
	int AddIf(const std::string& tag, int how, int compareValue, const std::string& what);	///< Add instance if test condition true
	int Remove(const std::string& tag);											///< Remove an instance of an attribute
	int RemoveIf(const std::string& tag, int how, int compareValue);			///< Remove instance if test condition true
	int RemoveAll(const std::string& tag);										///< Remove all instances of an attribute
	bool Has(const std::string& tag);											///< Does attribute exist in list?
	int Value(const std::string& tag);											///< Get attributes value
	int Low(const std::string& tag);											///< Get value low limit
	int High(const std::string& tag);											///< Get value high limit
	int Count(const std::string& tag);											///< Get attribute count
	bool Compare(const std::string& tag, int how, int compareValue);			///< Compare attribute value to a number
	int GetModifyAmt(const std::string& tag);
	int GetPowerUpLevel(const std::string& tag);					///< Get number of highlimit changes (PowerUp Level)

	int AllocateUserData(const std::string& tag, int dataSize);		///< Allocate some user data for attribute
	int DeleteUserData(const std::string& tag);						///< Delete an attributes user data
	unsigned char* UserData(const std::string& tag);				///< Get pointer to user data for attr

	int SaveTo(FILE* saveFD, bool type);							///< Save attributes to a file
	int SaveAscii(FILE* saveFD);
	int RestoreFrom(FILE* restoreFD, bool type);					///< Restore attributes from a file
	void Dump();													///< Debug list dumper

private:
	void ClampValue(PersistAttribute* attr);						///< Clamp value to limits
	inline PersistAttribute* Locate(const std::string& tag);		///< Find attribute in the list
	PersistAttribute* AddNew(const std::string& tag, int value);	///< Add a new attribute to the list
	int Delete(const std::string& tag);								///< Remove an attribute from the list and delete it
	bool LocalCompare(PersistAttribute* attr, int how, int compareValue);

private:
	stdext::hash_map<std::string, PersistAttribute*> m_List;		///< Attribute list
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
