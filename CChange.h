/****************************************************************************************/
/*																						*/
/*	CChange.h:		ChangeAttribute class handler										*/
/*																						*/
/*	(c) 2002 Ralph Deane																*/
/*	All Rights Reserved																	*/
/*																						*/
/*	This file contains the class declaration for ChangeAttribute						*/
/*	handling.																			*/
/*																						*/
/****************************************************************************************/


#ifndef __RGF_CCHANGE_H_
#define __RGF_CCHANGE_H_

class CChangeAttribute : public CRGFComponent
{
public:
	CChangeAttribute();
	~CChangeAttribute();

	void Tick(float dwTicks);
	int SaveTo(FILE *SaveFD, bool type);
	int RestoreFrom(FILE *RestoreFD, bool type);
	bool GetActive()  { return Active; }

private:

	bool Active;
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
