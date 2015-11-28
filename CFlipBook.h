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
	int LocateEntity(const char *szName, void **pEntityData);
	int ReSynchronize();

private:
	int CreateS(FlipBook *S);	///< Create Sprite effect
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
