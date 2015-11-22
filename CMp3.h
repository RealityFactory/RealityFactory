#ifndef __CMP3_MANAGER__
#define __CMP3_MANAGER__

typedef enum Mp3State 
{
	Uninitialized, 
		Stopped, 
		Paused, 
		Playing 
} Mp3State;

typedef struct Mp3Media
{
    Mp3State			state;
    IGraphBuilder		*pGraph;
    HANDLE				hGraphNotifyEvent;
} Mp3Media;

class CMp3Manager
{
public:
	CMp3Manager();		// Default constructor
	~CMp3Manager();	
	void OpenMediaFile(LPSTR szFile );
	void PlayMp3(long volume, geBoolean loop);
	void StopMp3();
private:
	void ChangeStateTo(Mp3State newState);
	bool IsInitialized();
	bool CreateFilterGraph();
	bool CanPlay();
	void DeleteContentsMp3();
	bool RenderFile( LPSTR szFileName );

	Mp3Media media;
	bool Active;
};
#endif