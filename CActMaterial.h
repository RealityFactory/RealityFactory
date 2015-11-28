/************************************************************************************//**
 * @file CActMaterial.h
 * @author Ralph Deane
 * @author Daniel Queteschiner
 * @brief CActMaterial handler
 *
 * This file contains the class implementation for the
 * CActMaterial enitity  for RGF-based games.
 *//*
 * Copyright (c) 2002 Ralph Deane; All rights reserved.
 ****************************************************************************************/

#ifndef __RGF_ACTMATERIAL_H_
#define __RGF_ACTMATERIAL_H_

// changed QD 12/15/05
/*
typedef struct MaterialList
{
	MaterialList *next;
	MaterialList *prev;
	char Entity[64];
	char Material[64];
	bool ChangeLighting;
	GE_RGBA FillColor;
	GE_RGBA AmbientColor;
// changed QD 07/21/04
	geBoolean AmbientLightFromFloor;
// end change
} MaterialList;
*/
// end change

class CActMaterial : public CRGFComponent
{
public:
	CActMaterial();
	~CActMaterial();

	void Tick(float dwTicks);
	int SaveTo(FILE *SaveFD, bool type);
	int RestoreFrom(FILE *RestoreFD, bool type);

// changed QD 12/15/05
/*
private:
	MaterialList *Bottom;
	MaterialList *Top;
*/
// end change
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
