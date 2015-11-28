/****************************************************************************************/
/*																						*/
/*	CProcedural.h:		Procedural handler												*/
/*																						*/
/*	(c) 2001 Ralph Deane																*/
/*																						*/
/*	This file contains the class declaration for the CProcedural						*/
/*	class that encapsulates Procedural handling in the RGF.								*/
/*																						*/
/****************************************************************************************/


#include "RabidFramework.h"

#ifndef __RGF_CProcedural_H__
#define __RGF_CProcedural_H__


class CProcedural : public CRGFComponent
{
public:
	CProcedural();				// Constructor
	~CProcedural();				// Destructor

	void Tick(geFloat dwTicks);	// Update video textures
	int ReSynchronize();
private:
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
