
#ifndef QX_USER_H
#define QX_USER_H

#include "..\\RabidFramework.h"

#define RADIANS(x) 0.0174532925199433f*x
#define RANDOM_ONE_TO_ZERO    (((FLOAT)rand()-(FLOAT)rand())/RAND_MAX)
#define MAX_USER_VERTS				4
#define PRIORITIES 512
#define ONE_OVER_THREE		0.33333333333333333333333333333333f;
#define ONE_OVER_255		0.00392156862745098039215686274509804f
#define fractRand(v) randnum (-v, v)
#define MAX_DISTANCE_DETAIL 16
			

struct geWorld_Leaf;

typedef struct	gePoly
{
#ifdef _DEBUG
	struct gePoly	*Self1;
#endif

	geWorld			*World;
	GE_LVertex		Verts[MAX_USER_VERTS];	// Copy of user verts					

	geWorld_Leaf	*LeafData;
	geFloat			ZOrder;

	int32			NumVerts;

	geBitmap		*Bitmap;

	gePoly_Type		Type;					// Type of poly
	int32			RenderFlags;			// Fx of poly

	geFloat			Scale;					// Currently only used for TexturedPoint

	struct gePoly	*Prev;
	struct gePoly	*Next;

	struct gePoly	*AddOnceNext;

#ifdef _DEBUG
	struct gePoly	*Self2;
#endif
} gePoly;

enum eDirection
{
	
	DIRECTION_N,
	DIRECTION_NE,
	DIRECTION_E,
	DIRECTION_SE,
	DIRECTION_S,
	DIRECTION_SW,
	DIRECTION_W,
	DIRECTION_NW

};

static geVec3d ANGLE_N =	{ 0.0f, 90.0f, 0.0f };
static geVec3d ANGLE_NE  =	{ 0.0f, 45.0f, 0.0f };
static geVec3d ANGLE_E  =	{ 0.0f, 0.0f, 0.0f };
static geVec3d ANGLE_SE  =	{ 0.0f, 315.0f, 0.0f };
static geVec3d ANGLE_S =	{ 0.0f, 270.0f, 0.0f };
static geVec3d ANGLE_SW  = { 0.0f, 225.0f, 0.0f };
static geVec3d ANGLE_W  = { 0.0f, 180.0f, 0.0f };
static geVec3d ANGLE_NW  = { 0.0f, 135.0f, 0.0f };
static geVec3d ANGLE_UP	=	{ 90.0f, 90.0f, 0.0f };
static geVec3d ANGLE_DOWN	=	{ 90.0f, -90.0f, 0.0f };

static void* g_VoidNull = 0;

void ColorInit(GE_RGBA* p);
geBoolean IsColorGood(GE_RGBA	*Color );
float RandFloat();
float FastScreenDistance( geVec3d* pVec );
void GetViewVector( geVec3d* pVect );
float randnum (float min, float max);

#endif