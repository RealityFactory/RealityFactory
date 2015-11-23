
#ifndef __RGF_ACTMATERIAL_H_
#define __RGF_ACTMATERIAL_H_

class CActMaterial : public CRGFComponent  
{
public:
	CActMaterial();
	~CActMaterial();
	void Tick(float dwTicks);
};

#endif