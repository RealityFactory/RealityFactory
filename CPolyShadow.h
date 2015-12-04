/************************************************************************************//**
 * @file CPolyShadow.h
 * @brief CPolyShadow class declaration
 * @author Dan Valeo
 *//*
 * Copyright (c) 2004 Dan Valeo; All rights reserved.
 ****************************************************************************************/

#ifndef	_PLYSHDW_H_
#define	_PLYSHDW_H_

/**
 * @brief CPolyShadow projects an actor onto a surfaces and renders its polys
 */
class CPolyShadow
{
public:
	CPolyShadow();
	~CPolyShadow();

	bool DrawShadow(const geActor *Actor);

private:
	GE_LVertex m_Vertex[3];
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
