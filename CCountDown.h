/****************************************************************************************/
/*																						*/
/*	CCountDown.h																		*/
/*																						*/
/****************************************************************************************/

#ifndef __RGF_CCOUNT_H_
#define __RGF_CCOUNT_H_

class CCountDown : public CRGFComponent
{
public:
	CCountDown();
	~CCountDown();

	void Tick(float dwTicks);
	int SaveTo(FILE *SaveFD, bool type);
	int RestoreFrom(FILE *RestoreFD, bool type);
	int LocateEntity(char *szName, void **pEntityData);
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
