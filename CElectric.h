/*
	CElectric.h:		Electric Bolt handler

	(c) 1999 Ralph Deane

	This file contains the class declarations for the CCorona
class that encapsulates all corona special effects for
RGF-based games.
*/

#ifndef __RGF_CELECTRIC_H_
#define __RGF_CELECTRIC_H_


#define	ELECTRIC_BOLT_REDDOMINANT	0
#define	ELECTRIC_BOLT_GREENDOMINANT	1
#define	ELECTRIC_BOLT_BLUEDOMINANT	2

#define	LIGHTNINGSTROKEDURATION	0.05f

typedef struct	Electric_BoltEffect
{
	int			beInitialized;
	int			beNumPoints;
	geFloat		beWildness;

	/* For rendering */
	geVec3d		beStart;
	geVec3d		beEnd;

	/* For generating the geometry */
	geVec3d	*	beCenterPoints;
	geVec3d *	beCurrentPoint;

	geFloat		beBaseColors[3];
	geFloat		beCurrentColors[3];
	geFloat		beBaseBlue;
	int			beDecayRate;
	int			beDominantColor;

	int			beWidth;

	geBitmap	*beBitmap;

}	Electric_BoltEffect;

// class declaration for CElectric
class CElectric : public CRGFComponent
{
public:
  CElectric();	// Default constructor
  ~CElectric();	
  int Create(geVec3d Origin, ElectricBolt *pBolt);
  geBoolean Tick(float dwTicks);
  void CheckCollision(ElectricBolt *Bolt);
	int LocateEntity(char *szName, void **pEntityData);
	int ReSynchronize();
// start multiplayer
	int SaveTo(FILE *SaveFD, bool type);
  int RestoreFrom(FILE *RestoreFD, bool type);
// end multiplayer
private:

};


#endif

// --------------------------------------------------------------------------
