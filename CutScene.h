/****************************************************************************************/
/*																						*/
/*	CutScene.h:			CCutScene class													*/
/*																						*/
/****************************************************************************************/

#ifndef __RGF_CUTSCENE_H_
#define __RGF_CUTSCENE_H_

class CCutScene : public CRGFComponent
{
public:
	CCutScene();
	~CCutScene();

	void Tick(float dwTicks);
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
