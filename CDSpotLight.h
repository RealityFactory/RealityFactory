/****************************************************************************************/
/*																						*/
/*	CDSpotLight.h:		Dynamic Spotlight handler										*/
/*																						*/
/*																						*/
/****************************************************************************************/

#ifndef __RGF_CDSPOTLIGHT_H_
#define __RGF_CDSPOTLIGHT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CRGFComponent.h"

class CDSpotLight : public CRGFComponent
{
public:
	CDSpotLight();
	virtual ~CDSpotLight();

	geBoolean Tick(float dwTicks);
	int LocateEntity(char *szName, void **pEntityData);
	int ReSynchronize();
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
