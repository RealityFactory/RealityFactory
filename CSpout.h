/************************************************************************************//**
 * @file CSpout.h
 * @brief CSpout and CActorSpout class
 ****************************************************************************************/

#ifndef __RGF_CSPOUT_H_
#define __RGF_CSPOUT_H_

#pragma warning( disable : 4068 )

#include "genesis.h"

// class declaration for CSpout
class CSpout : public CRGFComponent
{
public:
	CSpout();
	~CSpout();

	int Create(Spout *S);
	void Tick(float dwTicks);
	int LocateEntity(char *szName, void **pEntityData);
	int ReSynchronize();
private:
};


// class declaration for CActorSpout
class CActorSpout : public CRGFComponent
{
public:
	CActorSpout();
	~CActorSpout();

	int Create(ActorSpout *S);
	void Tick(float dwTicks);
	int LocateEntity(char *szName, void **pEntityData);
	int ReSynchronize();
private:
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
