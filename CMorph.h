// CMorph.h: Schnittstelle für die Klasse CMorph.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CMORPH_H__1538EAC0_A2AA_11D6_BEAF_00C0DF084214__INCLUDED_)
#define AFX_CMORPH_H__1538EAC0_A2AA_11D6_BEAF_00C0DF084214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CMorph : public CRGFComponent  
{
public:
	CMorph();
	~CMorph();
	void Tick(float dwTicks);

};

#endif // !defined(AFX_CMORPH_H__1538EAC0_A2AA_11D6_BEAF_00C0DF084214__INCLUDED_)
