/****************************************************************************************/
/*																						*/
/*	CDynalite.h:		Dynamic Light handler											*/
/*																						*/
/*	(c) 2001 Ralph Deane																*/
/*																						*/
/*	This file contains the class declarations for the CDynalite							*/
/*	class that encapsulates all dynamic light effects for								*/
/*	RGF-based games.																	*/
/*																						*/
/****************************************************************************************/


#ifndef __RGF_CDYNALITE_H_
#define __RGF_CDYNALITE_H_

// class declaration for CDynalite
class CDynalite : public CRGFComponent
{
public:
	CDynalite();	// Default constructor
	~CDynalite();

	geBoolean Tick(float dwTicks);
	int LocateEntity(char *szName, void **pEntityData);
	int ReSynchronize();
private:

};


#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
