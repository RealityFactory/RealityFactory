/************************************************************************************//**
 * @file CCountDown.h
 * @brief CCountDown class declaration
 * @author Ralph Deane
 ****************************************************************************************/

#ifndef __RGF_CCOUNT_H_
#define __RGF_CCOUNT_H_

/**
 * @brief CCountDown handles CountDown entities
 */
class CCountDown : public CRGFComponent
{
public:
	CCountDown();
	~CCountDown();

	void Tick(geFloat dwTicks);
	int SaveTo(FILE *SaveFD, bool type);
	int RestoreFrom(FILE *RestoreFD, bool type);
	int LocateEntity(const char *szName, void **pEntityData);
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
