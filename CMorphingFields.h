/****************************************************************************************/
/*																						*/
/*	CMorphingFields.h:		Morphing Fields class handler								*/
/*																						*/
/*	(c) 2001 Ralph Deane																*/
/*	All Rights Reserved																	*/
/*																						*/
/*	This file contains the class declaration for morphing fields handling.				*/
/*																						*/
/****************************************************************************************/


#include "RabidFramework.h"

#ifndef __RGF_CMORPHINGFIELDS_H_
#define __RGF_CMORPHINGFIELDS_H_

class CMorphingFields : public CRGFComponent
{
public:
	CMorphingFields();
	~CMorphingFields();

	void Tick(geFloat dwTicks);				// Do any animation
	int BindToPath(char *szName);			// Bind entity to motion path
	int SaveTo(FILE *SaveFD);
											// Save state to a file
	int RestoreFrom(FILE *RestoreFD);		// Restore state from a file
	int LocateEntity(char *szName, void **pEntityData);
	int ReSynchronize();

private:
	int m_MorphingFieldCount;				// Count of morphing fields
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
