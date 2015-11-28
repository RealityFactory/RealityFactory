/************************************************************************************//**
 * @file CMp3.h
 * @brief CMp3Manager class declaration
 ****************************************************************************************/

#ifndef __CMP3_MANAGER__
#define __CMP3_MANAGER__

/**
 * @brief CMp3Manager handles playback of MP3 files
 */
class CMp3Manager
{
public:
	CMp3Manager();		///< Default constructor
	~CMp3Manager();

	/**
	 * @brief This function opens and renders the specified media file.
	 */
	void OpenMediaFile(LPSTR szFile);
	void PlayMp3(long volume, geBoolean loop);
	void StopMp3();
	void Refresh();
	void SetVolume(long volume);

private:
	bool Active;
	HWND m_Video;
	LONG Length;
	bool Loop;
};
#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
