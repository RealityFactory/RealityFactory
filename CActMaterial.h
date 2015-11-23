
#ifndef __RGF_ACTMATERIAL_H_
#define __RGF_ACTMATERIAL_H_


typedef struct MaterialList
{
	MaterialList *next;
	MaterialList *prev;
	char Entity[64];
	char Material[64];
	bool ChangeLighting;
	GE_RGBA FillColor;
	GE_RGBA AmbientColor;
} MaterialList;

class CActMaterial : public CRGFComponent  
{
public:
	CActMaterial();
	~CActMaterial();
	void Tick(float dwTicks);
	int SaveTo(FILE *SaveFD, bool type);
	int RestoreFrom(FILE *RestoreFD, bool type);
private:
	MaterialList *Bottom;
	MaterialList *Top;
};

#endif