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
	int LocateEntity(const char *szName, void **pEntityData);
	int ReSynchronize();

private:
	int CreateS(Flame *S);	///< Create Spray effect
	int CreateG(Flame *S);	///< Create Glow effect
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
