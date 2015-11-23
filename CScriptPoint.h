#ifndef	CSCRIPTPOINT_H
#define	CSCRIPTPOINT_H


typedef struct	SPOrigin
{
	SPOrigin    *next;
	SPOrigin    *prev;
	char     *Name;
	geVec3d	origin;
} SPOrigin;

class CScriptPoint
{
public:
	CScriptPoint();
	~CScriptPoint();
	int LocateEntity(char *szName, void **pEntityData);
	void Render();
private:
	void DrawLine3d(const geVec3d *p1, const geVec3d *p2, int r, int g, int b, int r1, int g1, int b1);
	void SetOrigin();
	void FreeOrigin();
	geVec3d GetOrigin(char *Name);
	geBitmap *Texture;
	SPOrigin *Bottom;
};

#endif