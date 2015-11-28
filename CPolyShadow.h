#pragma warning( disable : 4068 )

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