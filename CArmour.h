
#ifndef __RGF_CARMOUR_H_
#define __RGF_CARMOUR_H_

#define MAXARMOUR 20
#define MAXDAMAGEBY 10

typedef struct ArmourType
{
	char *Name;
	char *Attribute;
	char *DamageBy[MAXDAMAGEBY];
	int Protection[MAXDAMAGEBY];
	int DamageTo[MAXDAMAGEBY];
} ArmourType;

class CArmour : public CRGFComponent
{
public:
  CArmour();
  ~CArmour();
  void DisableHud(char *Attr);
  int AdjustDamage(int Amount, char *name, char *Attr);
private:
	ArmourType List[MAXARMOUR];
	int ListPtr;
};



class CLiftBelt : public CRGFComponent
{
public:
  CLiftBelt();
  ~CLiftBelt();
  void Tick(float dwTicks);
  void ChangeLift(bool increase);
private:
  bool Change;
  bool Increase;
};

#endif
