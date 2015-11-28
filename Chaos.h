/****************************************************************************************/
/*																						*/
/*	Chaos.h:		Chaos handler														*/
/*																						*/
/*	(c) 1999 Ralph Deane																*/
/*																						*/
/*	This file contains the class declaration for the Chaos								*/
/*	class that encapsulates chaaos based special effects for							*/
/*	RGF-based games.																	*/
/*																						*/
/****************************************************************************************/


#ifndef __RGF_CHAOS_H_
#define __RGF_CHAOS_H_

#pragma warning( disable : 4068 )

#include "genesis.h"

// changed QD 12/15/05
// #define GE_TWOPI		(GE_PI*2.0f)
#define CHAOS_FORMAT	GE_PIXELFORMAT_32BIT_ARGB

// class declaration for Chaos
class Chaos : public CRGFComponent
{
public:
	Chaos();
	~Chaos();

	void Tick(geFloat dwTick);

private:
};


#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
