/************************************************************************************//**
 * @file CFlame.h
 * @brief CFlame class declaration
 * @author Ralph Deane
 ****************************************************************************************/

#ifndef __RGF_CFLAME_H_
#define __RGF_CFLAME_H_

/**
 * @brief Class declaration for CFlame
 */
class CFlame : public CRGFComponent
{
public:
	CFlame();
	~CFlame();

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
	int CreateS(Flame *S);	///< Create Spray effect
	int CreateG(Flame *S);	///< Create Glow effect
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
