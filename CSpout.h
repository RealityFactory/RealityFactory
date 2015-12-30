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

	void Tick(geFloat dwTicks);

	/**
	 * @brief Given a name, locate the desired entity in the currently loaded
	 * level and return its user data.
	 */
	int LocateEntity(const char *szName, void **pEntityData);

	/**
	 * @brief Correct internal timing to match current time, to make up for time
	 * lost when outside the game loop (typically in "menu mode").
	 */
	int ReSynchronize();

private:
	int CreateSpray(Spout *S);
};


/**
 * @brief Class declaration for CActorSpout
 */
class CActorSpout : public CRGFComponent
{
public:
	CActorSpout();
	~CActorSpout();

	void Tick(geFloat dwTicks);

	/**
	 * @brief Given a name, locate the desired entity in the currently loaded
	 * level and return its user data.
	 */
	int LocateEntity(const char *szName, void **pEntityData);

	/**
	 * @brief Correct internal timing to match current time, to make up for time
	 * lost when outside the game loop (typically in "menu mode").
	 */
	int ReSynchronize();

private:
	int CreateActorSpray(ActorSpout *S);
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
