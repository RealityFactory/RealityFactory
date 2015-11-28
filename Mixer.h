/****************************************************************************************/
/*																						*/
/*	Mixer.h: interface for the CMixer class.											*/
/*																						*/
/*	CMixer - simple mixer control wrapper												*/
/*	Copyright (C) Alexander Fedorov 1999												*/
/*	You may do whatever you want with this code, as long as you include this			*/
/*	copyright notice in your implementation files.										*/
/*																						*/
/****************************************************************************************/

#if !defined(AFX_ALEXFMIXER_H__4FC2F321_07F3_11D3_9864_CB4A97E7D6CA__INCLUDED_)
#define AFX_ALEXFMIXER_H__4FC2F321_07F3_11D3_9864_CB4A97E7D6CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <mmsystem.h>

// Thanks to Langis Pitre
#define NO_SOURCE ((MIXERLINE_COMPONENTTYPE_SRC_LAST + 1))

class CMixer
{
protected:
	HMIXER m_HMixer;
	INT m_iMixerControlID;
	MMRESULT mmr;
	DWORD m_dwChannels;
	BOOL m_bSuccess;
	void ZeroAll();

public:
	BOOL IsOk() {return m_bSuccess;};
	BOOL On();
	BOOL Off();
	DWORD GetControlValue();
	BOOL SetControlValue(DWORD dw);

	CMixer(DWORD DstType, DWORD SrcType, DWORD ControlType);
	CMixer(HWND hwnd, DWORD DstType, DWORD SrcType, DWORD ControlType);
	virtual ~CMixer();
};

#endif // !defined(AFX_ALEXFMIXER_H__4FC2F321_07F3_11D3_9864_CB4A97E7D6CA__INCLUDED_)

/* ----------------------------------- END OF FILE ------------------------------------ */
