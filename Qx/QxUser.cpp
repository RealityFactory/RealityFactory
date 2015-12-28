
#include "..\\RabidFramework.h"
#include "QxUser.h"


void ColorInit(GE_RGBA* p)
{
	p->r = 255.0;
	p->g = 255.0;
	p->b = 255.0;
	p->a = 255.0;
}

////////////////////////////////////////////////////////////////////////////////////////
//
//	IsColorGood()
//
//	Checks if a color struct contains valid data.
//
////////////////////////////////////////////////////////////////////////////////////////
geBoolean IsColorGood(GE_RGBA *Color)	// color to check
{
	// ensure valid data
	QXASSERT( Color != NULL );

	// fail if any color values are out of range
	if ( ( Color->a < 0.0f ) || ( Color->a > 255.0f ) )
	{
		return GE_FALSE;
	}
	if ( ( Color->r < 0.0f ) || ( Color->r > 255.0f ) )
	{
		return GE_FALSE;
	}
	if ( ( Color->g < 0.0f ) || ( Color->g > 255.0f ) )
	{
		return GE_FALSE;
	}
	if ( ( Color->b < 0.0f ) || ( Color->b > 255.0f ) )
	{
		return GE_FALSE;
	}

	// if we got to here then the color is good
	return GE_TRUE;

} // IsColorGood()


float RandFloat()
{
	// pick a random float from whithin the range
	return ( (float)( ( ( rand() % 1000 ) + 1 ) ) ) / 1000.0f * -1;

} // RandFloat()


float FastScreenDistance( geVec3d* pVec )
{
	const geXForm3d* pCXF =
		geCamera_GetCameraSpaceXForm(CCD->CameraManager()->Camera());

	return (float)(
					( pVec->X * pCXF->CX)
				+	( pVec->Y * pCXF->CY)
				+	( pVec->Z * pCXF->CZ)
				+	pCXF->Translation.Z);
}


void GetViewVector( geVec3d* pVect )
{
	geXForm3d* p = ( geXForm3d*)geCamera_GetWorldSpaceXForm(CCD->CameraManager()->Camera());
	geXForm3d_GetIn( p, pVect ) ;
}


float randnum (float min, float max)
{
	int r;
    float x;

	r = rand();
    x = (float)(r & 0x7fff) /
		(float)0x7fff;

	return (x * (max - min) + min);
}
