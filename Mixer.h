/************************************************************************************//**
 * @file Mixer.h
 * @brief Interface for the CMixer class.
 * @author Alexander Fedorov
 *//*
 * Copyright (c) 1999 Alexander Fedorov; All rights reserved.
 * You may do whatever you want with this code, as long as you include this
 * copyright notice in your implementation files.
 ****************************************************************************************/

#ifndef __RGF_MIXER_H_
#define __RGF_MIXER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Thanks to Langis Pitre
#define NO_SOURCE ((MIXERLINE_COMPONENTTYPE_SRC_LAST + 1))

/**
 * @brief CMixer - simple mixer control wrapper
 */
class CMixer
{
public:
	CMixer(DWORD DstType, DWORD SrcType, DWORD ControlType);
	CMixer(HWND hwnd, DWORD DstType, DWORD SrcType, DWORD ControlType);
	virtual ~CMixer();

	BOOL IsOk() { return m_bSuccess; }

	BOOL On();

	BOOL Off();

	DWORD GetControlValue();

	BOOL SetControlValue(DWORD dw);

protected:
	HMIXER m_HMixer;
	INT m_iMixerControlID;
	MMRESULT mmr;
	DWORD m_dwChannels;
	BOOL m_bSuccess;

};

#endif // !defined(__RGF_MIXER_H_)

/* ----------------------------------- END OF FILE ------------------------------------ */
