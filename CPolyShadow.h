#pragma warning( disable : 4068 )
/************************************************************************************//**
 * @file CPolyShadow.h
 * @brief CPolyShadow class declaration
 * @author Dan Valeo
 *//*
 * Copyright (c) 2004 Dan Valeo; All rights reserved.
 ****************************************************************************************/

#ifndef	_PLYSHDW_H_
#define	_PLYSHDW_H_

#include "genesis.h"

class CPolyShadow
{
public:
	CPolyShadow();
	~CPolyShadow();

	bool DrawShadow(geActor *Actor);
	bool DrawPolyCel(geActor *Actor);
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
