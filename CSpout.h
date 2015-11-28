/************************************************************************************//**
 * @file CSpout.h
 * @brief CSpout and CActorSpout class
 ****************************************************************************************/

#ifndef __RGF_CSPOUT_H_
#define __RGF_CSPOUT_H_

/**
 * @brief Class declaration for CSpout
 */
class CSpout : public CRGFComponent
{
public:
	CSpout();
	~CSpout();

	int Create(Spout *S);
	void Tick(geFloat dwTicks);
	int LocateEntity(const char *szName, void **pEntityData);
	int ReSynchronize();
private:
};


/**
 * @brief Class declaration for CActorSpout
 */
class CActorSpout : public CRGFComponent
{
public:
	CActorSpout();
	~CActorSpout();

	int Create(ActorSpout *S);
	void Tick(geFloat dwTicks);
	int LocateEntity(const char *szName, void **pEntityData);
	int ReSynchronize();
private:
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
