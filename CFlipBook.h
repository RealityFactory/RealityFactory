/************************************************************************************//**
 * @file CFlipBook.h
 * @brief CFlipBook class declaration
 ****************************************************************************************/

#ifndef __RGF_CFLIP_H_
#define __RGF_CFLIP_H_

/**
 * @brief Class declaration for CFlipBook
 */
class CFlipBook : public CRGFComponent
{
public:
	CFlipBook();
	~CFlipBook();

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
	int CreateSprite(FlipBook *S);	///< Create Sprite effect
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
