/************************************************************************************//**
 * @file CMorph.h
 * @brief CMorph class declaration
 * @author Peter Siamidis
 * @author Daniel Queteschiner
 ****************************************************************************************/

#ifndef __RGF_CMORPH_H_
#define __RGF_CMORPH_H_

class CMorph : public CRGFComponent
{
public:
	CMorph();
	~CMorph();

	void Tick(float dwTicks);
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
