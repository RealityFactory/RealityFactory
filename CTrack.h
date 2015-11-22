#ifdef RF063

#ifndef	CTRACK_H
#define	CTRACK_H

class CTrackPoint
{
public:
	CTrackPoint();
	~CTrackPoint();
	int LocateEntity(char *szName, void **pEntityData);
};

class CTrackStart
{
public:
	CTrackStart();
	~CTrackStart();
	int LocateEntity(char *szName, void **pEntityData);
	void Render();
private:
	void DrawLine3d(const geVec3d *p1, const geVec3d *p2, int r, int g, int b, int r1, int g1, int b1);
	geBitmap *TrackStartTexture;
};

#endif

#endif