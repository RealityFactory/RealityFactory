/************************************************************************************//**
 * @file CMorph.h
 * @brief CMorph class declaration
 * @author Peter Siamidis
 * @author Daniel Queteschiner
 ****************************************************************************************/

#ifndef __RGF_CMORPH_H_
#define __RGF_CMORPH_H_

/**
 * @brief CMorph handles EM_Morph entities
 */
class CMorph : public CRGFComponent
{
public:
	CMorph();
	~CMorph();

	void Tick(geFloat dwTicks);
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
