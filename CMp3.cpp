/************************************************************************************//**
 * @file CMp3.cpp
 * @brief CMp3Manager class implementation
 ****************************************************************************************/

#include "RabidFramework.h"
#include "CMp3.h"

/* ------------------------------------------------------------------------------------ */
// Constructor
/* ------------------------------------------------------------------------------------ */
CMp3Manager::CMp3Manager() :
	m_Active(false),
	m_Video(NULL),
	m_Length(0),
	m_Loop(false)
{
}

/* ------------------------------------------------------------------------------------ */
// Destructor
/* ------------------------------------------------------------------------------------ */
CMp3Manager::~CMp3Manager()
{
	if(m_Video != NULL)
		MCIWndDestroy(m_Video);
}

/* ------------------------------------------------------------------------------------ */
// OpenMediaFile
//
// This function opens and renders the specified media file.
// File..Open has been selected
/* ------------------------------------------------------------------------------------ */
void CMp3Manager::OpenMediaFile(LPCSTR szFile)
{
	m_Video = MCIWndCreate(CCD->Engine()->WindowHandle(),
							NULL,
							WS_CHILD | MCIWNDF_NOMENU,
							szFile);

	if(m_Video)
	{
		MCIWndUseTime(m_Video);
		m_Length = MCIWndGetLength(m_Video);
	}
}

/* ------------------------------------------------------------------------------------ */
// Rewind
/* ------------------------------------------------------------------------------------ */
void CMp3Manager::Rewind()
{
	if(m_Video)
	{
		MCIWndHome(m_Video);
	}
}

/* ------------------------------------------------------------------------------------ */
// Refresh
/* ------------------------------------------------------------------------------------ */
void CMp3Manager::Refresh()
{
	if(m_Active)
	{
		if(m_Loop)
		{
			LONG Pos = MCIWndGetPosition(m_Video);

			if(Pos > 0)
			{
				if((Pos+5) >= m_Length)
				{
					MCIWndHome(m_Video);
					MCIWndPlay(m_Video);
				}
			}
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// PlayMp3
/* ------------------------------------------------------------------------------------ */
void CMp3Manager::PlayMp3(long volume, geBoolean loop)
{
	if(m_Video)
	{
		MCIWndPlay(m_Video);
		m_Loop = loop;
		m_Active = true;
		SetVolume(volume);
	}
}

/* ------------------------------------------------------------------------------------ */
// StopMp3
/* ------------------------------------------------------------------------------------ */
void CMp3Manager::StopMp3()
{
	if(m_Active)
	{
		MCIWndStop(m_Video);
		m_Active = false;
	}

	return ;
}

/* ------------------------------------------------------------------------------------ */
// IsPlaying
/* ------------------------------------------------------------------------------------ */
bool CMp3Manager::IsPlaying()
{
	if(m_Active == false)
		return false;

	if(!m_Loop)
	{
		LONG Pos = MCIWndGetPosition(m_Video);

		if(Pos > 0)
		{
			if((Pos+5) >= m_Length)
			{
				StopMp3();
				return false;
			}
		}
	}

	return true;
}

/* ------------------------------------------------------------------------------------ */
// SetVolume
/* ------------------------------------------------------------------------------------ */
void CMp3Manager::SetVolume(long volume)
{
	if(m_Video)
	{
		MCIWndSetVolume(m_Video, volume);
	}
}


/* ----------------------------------- END OF FILE ------------------------------------ */
