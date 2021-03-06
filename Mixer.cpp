/************************************************************************************//**
 * @file Mixer.cpp
 * @brief Implementation of the CMixer class.
 *
 * CMixer - simple mixer control wrapper.
 * @author Alexander Fedorov
 *//*
 * Copyright (C) Alexander Fedorov 1999
 *
 * You may do whatever you want with this code, as long as you include this
 * copyright notice in your implementation files.
 * If you wish to add new classes to this collection, feel free to do so.
 * But please send me your code so that I can update the collection.
 * Comments and bug reports: lamer2000@usa.net
 ****************************************************************************************/
#include <windows.h>
#include <mmsystem.h>
#include "Mixer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


/* ------------------------------------------------------------------------------------ */
//	Construction/Destruction
/* ------------------------------------------------------------------------------------ */
CMixer::CMixer(DWORD DstType, DWORD SrcType, DWORD ControlType) :
	m_HMixer(NULL),
	m_iMixerControlID(0),
	mmr(MMSYSERR_NOERROR),
	m_dwChannels(0),
	m_bSuccess(FALSE)
{
	if(mixerGetNumDevs() < 1)
		return;

	mmr = mixerOpen(&m_HMixer, 0, 0, 0L, CALLBACK_NULL);

	if(mmr != MMSYSERR_NOERROR)
		return;

	// get dwLineID
	MIXERLINE mxl;
	mxl.cbStruct = sizeof(MIXERLINE);

	// DstType
	mxl.dwComponentType = DstType;

	if(mixerGetLineInfo(reinterpret_cast<HMIXEROBJ>(m_HMixer), &mxl, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR)
		return;

	// SrcType
	if(SrcType != NO_SOURCE)
	{
		UINT nconn = mxl.cConnections;
		DWORD DstIndex = mxl.dwDestination;

		for(UINT j=0; j<nconn; ++j)
		{
			mxl.cbStruct = sizeof(MIXERLINE);
			mxl.dwSource = j;
			mxl.dwDestination = DstIndex;

			if(mixerGetLineInfo(reinterpret_cast<HMIXEROBJ>(m_HMixer), &mxl, MIXER_GETLINEINFOF_SOURCE) != MMSYSERR_NOERROR)
				return;

			if(mxl.dwComponentType == SrcType)
				break;
		}
	}

	// get dwControlID
	MIXERCONTROL mxc;
	MIXERLINECONTROLS mxlc;

	mxlc.cbStruct		= sizeof(MIXERLINECONTROLS);
	mxlc.dwLineID		= mxl.dwLineID;
	mxlc.dwControlType	= ControlType;
	mxlc.cControls		= 1;
	mxlc.cbmxctrl		= sizeof(MIXERCONTROL);
	mxlc.pamxctrl		= &mxc;

	if(mixerGetLineControls(reinterpret_cast<HMIXEROBJ>(m_HMixer), &mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE) != MMSYSERR_NOERROR)
		return;

	m_iMixerControlID = mxc.dwControlID;
	m_dwChannels = mxl.cChannels;

	if(MIXERCONTROL_CONTROLF_UNIFORM & mxc.fdwControl)
		m_dwChannels = 1;

	m_bSuccess = TRUE;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
CMixer::CMixer(HWND hwnd, DWORD DstType, DWORD SrcType, DWORD ControlType) :
	m_HMixer(NULL),
	m_iMixerControlID(0),
	mmr(MMSYSERR_NOERROR),
	m_dwChannels(0),
	m_bSuccess(FALSE)
{
	if(mixerGetNumDevs() < 1)
		return;

	mmr = mixerOpen(&m_HMixer, 0, reinterpret_cast<DWORD>(hwnd), 0L, CALLBACK_WINDOW);

	if(mmr != MMSYSERR_NOERROR)
		return;

	// get dwLineID
	MIXERLINE mxl;
	mxl.cbStruct = sizeof(MIXERLINE);

	// DstType
	mxl.dwComponentType = DstType;

	if(mixerGetLineInfo(reinterpret_cast<HMIXEROBJ>(m_HMixer), &mxl, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR)
		return;

	// SrcType
	if(SrcType != NO_SOURCE)
	{
		UINT nconn = mxl.cConnections;
		DWORD DstIndex = mxl.dwDestination;

		for(UINT j=0; j<nconn; ++j)
		{
			mxl.cbStruct = sizeof(MIXERLINE);
			mxl.dwSource = j;
			mxl.dwDestination = DstIndex;

			if(mixerGetLineInfo(reinterpret_cast<HMIXEROBJ>(m_HMixer), &mxl, MIXER_GETLINEINFOF_SOURCE) != MMSYSERR_NOERROR)
				return;

			if(mxl.dwComponentType == SrcType)
				break;
		}
	}

	// get dwControlID
	MIXERCONTROL mxc;
	MIXERLINECONTROLS mxlc;

	mxlc.cbStruct		= sizeof(MIXERLINECONTROLS);
	mxlc.dwLineID		= mxl.dwLineID;
	mxlc.dwControlType	= ControlType;
	mxlc.cControls		= 1;
	mxlc.cbmxctrl		= sizeof(MIXERCONTROL);
	mxlc.pamxctrl		= &mxc;

	if(mixerGetLineControls(reinterpret_cast<HMIXEROBJ>(m_HMixer), &mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE) != MMSYSERR_NOERROR)
		return;

	m_iMixerControlID = mxc.dwControlID;
	m_dwChannels = mxl.cChannels;

	if(MIXERCONTROL_CONTROLF_UNIFORM & mxc.fdwControl)
		m_dwChannels = 1;

	m_bSuccess = TRUE;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
CMixer::~CMixer()
{
	if(m_HMixer)
		mixerClose(m_HMixer);
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
DWORD CMixer::GetControlValue()
{
	if(!m_bSuccess)
		return 0;

	m_bSuccess = FALSE;

	MIXERCONTROLDETAILS mxcd;
	MIXERCONTROLDETAILS_UNSIGNED mxcd_u;

	mxcd.cbStruct		= sizeof(mxcd);
	mxcd.dwControlID	= m_iMixerControlID;
	mxcd.cChannels		= m_dwChannels;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails		= sizeof(mxcd_u);
	mxcd.paDetails		= &mxcd_u;

	mmr = mixerGetControlDetails(reinterpret_cast<HMIXEROBJ>(m_HMixer), &mxcd, 0L);

	if(MMSYSERR_NOERROR != mmr)
		return 0;

	m_bSuccess = TRUE;
	return mxcd_u.dwValue;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
BOOL CMixer::SetControlValue(DWORD dw)
{
	if(!m_bSuccess)
		return m_bSuccess;

	m_bSuccess = FALSE;

	MIXERCONTROLDETAILS mxcd;
	MIXERCONTROLDETAILS_UNSIGNED mxcd_u;

	mxcd.cbStruct		= sizeof(mxcd);
	mxcd.dwControlID	= m_iMixerControlID;
	mxcd.cChannels		= m_dwChannels;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails		= sizeof(mxcd_u);
	mxcd.paDetails		= &mxcd_u;

	mmr = mixerGetControlDetails(reinterpret_cast<HMIXEROBJ>(m_HMixer), &mxcd, 0L);

	if(MMSYSERR_NOERROR != mmr) return m_bSuccess;

	mxcd_u.dwValue  = dw;
	mmr = mixerSetControlDetails(reinterpret_cast<HMIXEROBJ>(m_HMixer), &mxcd, 0L);

	if(MMSYSERR_NOERROR != mmr)
		return m_bSuccess;

	m_bSuccess = TRUE;
	return m_bSuccess;
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
BOOL CMixer::On()
{
	return SetControlValue(0);
}

/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */
BOOL CMixer::Off()
{
	return SetControlValue(1);
}


/* ----------------------------------- END OF FILE ------------------------------------ */
