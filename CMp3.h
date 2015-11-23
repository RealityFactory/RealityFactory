#ifndef __CMP3_MANAGER__
#define __CMP3_MANAGER__

class CMp3Manager
{
public:
	CMp3Manager();		// Default constructor
	~CMp3Manager();	
	void OpenMediaFile(LPSTR szFile );
	void PlayMp3(long volume, geBoolean loop);
	void StopMp3();
	void Refresh();
private:
	bool Active;
	HWND m_Video;
	LONG Length;
	bool Loop;
}; 
#endif