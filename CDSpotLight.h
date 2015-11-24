// CDSpotLight.h: Schnittstelle für die Klasse CDSpotLight.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CDSPOTLIGHT_H__DB43E120_2635_11D7_BEAF_00C0DF084214__INCLUDED_)
#define AFX_CDSPOTLIGHT_H__DB43E120_2635_11D7_BEAF_00C0DF084214__INCLUDED_

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

#endif // !defined(AFX_CDSPOTLIGHT_H__DB43E120_2635_11D7_BEAF_00C0DF084214__INCLUDED_)
