
#include "RabidFramework.h"				// The One True Header

CMp3Manager::CMp3Manager()
{
	if(SUCCEEDED(CoInitialize(NULL)))
	{
		ChangeStateTo(Uninitialized);
		media.hGraphNotifyEvent = NULL;
		media.pGraph = NULL;
		Active = true;
		return;
	}
	
    Active = false;
}

CMp3Manager::~CMp3Manager()
{
	if(Active)
	{
		CoUninitialize( );
	}
}

//
// ChangeStateTo
//
void CMp3Manager::ChangeStateTo(Mp3State newState)
{
    media.state = newState;
}

//
// CanPlay
//
// Return true if we can go to a playing state from our current state
//
bool CMp3Manager::CanPlay()
{
    return (media.state == Stopped || media.state == Paused);
}

//
// IsInitialized
//
// Return true if we have loaded and initialized a multimedia file
//
bool CMp3Manager::IsInitialized()
{
    return (media.state != Uninitialized);
}

//
// CreateFilterGraph
//
bool CMp3Manager::CreateFilterGraph()
{
    IMediaEvent *pME;					//	Event Pointer
    HRESULT hr;							//	for communicating with the filter graph
	
    ASSERT(media.pGraph == NULL);		//	make sure we start clean
	
    hr = CoCreateInstance(CLSID_FilterGraph,           // CLSID of object
		NULL,                         // Outer unknown
		CLSCTX_INPROC_SERVER,         // Type of server
		IID_IGraphBuilder,           // Interface wanted
		(void **) &media.pGraph);     // Returned object
    if (FAILED(hr))
	{
		media.pGraph = NULL;
		return false;
    }
	
    // We use this to find out events sent by the filtergraph
	
    hr = media.pGraph->QueryInterface(IID_IMediaEvent, 
		(void **) &pME);				//	open lines of communication
    if (FAILED(hr)) 
	{
		DeleteContentsMp3();
		return false;
    }
	//	establish event handle
    hr = pME->GetEventHandle((OAEVENT*) &media.hGraphNotifyEvent);
    pME->Release();
	
    if (FAILED(hr)) 
	{
		DeleteContentsMp3();
		return false;
    }
	
    return true;
	
} // CreateFilterGraph


// Destruction
//
// DeleteContents
//
void CMp3Manager::DeleteContentsMp3()
{
    if (media.pGraph != NULL) 
	{
		media.pGraph->Release();
		media.pGraph = NULL;
    }
	
    // this event is owned by the filter graph and is thus invalid
    media.hGraphNotifyEvent = NULL;
	
    ChangeStateTo( Uninitialized );
	
}	//	Delete Contents

//
// RenderFile
// Process the file through the appropriate filter path. This function is called by
// OpenMediaFile()
//
bool CMp3Manager::RenderFile( LPSTR szFileName )
{
    HRESULT hr;
    WCHAR wPath[MAX_PATH];
	
    DeleteContentsMp3();
	
    if ( !CreateFilterGraph() ) 
	{
		return false; 
    } 
	
    MultiByteToWideChar( CP_ACP, 0, szFileName, -1, wPath, MAX_PATH );
	
    hr = media.pGraph->RenderFile(wPath, NULL);
	
    if (FAILED( hr )) 
	{
		return false;
    } 
    return true;
	
} // RenderFile

//
// OpenMediaFile
// This function opens and renders the specified media file.
// File..Open has been selected
//
void CMp3Manager::OpenMediaFile(LPSTR szFile )
{
	if(Active)
	{
		if( szFile != NULL && RenderFile( szFile ))	//	this calls the filter graph
			ChangeStateTo( Stopped );
	}
} // OpenMediaFile

//
// PlayMp3
//

void CMp3Manager::PlayMp3(long volume, geBoolean loop)
{
	if(Active)
	{
		if( CanPlay() )
		{
			HRESULT hr;
			IMediaControl *pMC;
			IBasicAudio		*pMA;
			IMediaPosition	*pMP;
			
			// Obtain the interface to our filter graph
			hr = media.pGraph->QueryInterface(IID_IMediaControl, 
				(void **) &pMC);
			
			if( SUCCEEDED(hr) )
			{
			/*	In order to loop sounds, we will check with Media
			Player to see when the sound is almost over. When
			it's within 0.05 sec of ending, we'll stop the sound,
			rewind it and declare that the sound is ready to play
			again. The next time around your game's main loop,
			PlayMp3 will start it up again. And so it goes...
				*/
				
				//	Set volume level
				hr = media.pGraph->QueryInterface(IID_IBasicAudio,
					(void**) &pMA);
				
				if (SUCCEEDED(hr)) 
				{							/*	Set volume. 
					-10000 is silence,	0 is full volume*/
					hr = pMA->put_Volume(volume);
					pMA->Release();	//	release the interface
				}			
				
				
				if (loop == GE_TRUE)
				{			
					hr = media.pGraph->QueryInterface(IID_IMediaPosition,
						(void**) &pMP);
					if (SUCCEEDED(hr)) 
					{
						// start from last position, but rewind if near the end
						REFTIME tCurrent, tLength;
						hr = pMP->get_Duration(&tLength);
						if (SUCCEEDED(hr)) 
						{
							hr = pMP->get_CurrentPosition(&tCurrent);
							if (SUCCEEDED(hr)) 
							{
								// within 0.05 sec of end? (or past end?)
								if ((tLength - tCurrent) < 0.05) 
								{
									//	Rewind it	
									pMP->put_CurrentPosition(0);
									CanPlay();		//	It's ready to play again
								}						//	If not, you can't loop.
							}
						}
						pMP->Release();
					}
					
					
					// Ask the filter graph to play and release the interface
					hr = pMC->Run();
					pMC->Release();
					
					if( SUCCEEDED(hr) )
					{
						return;
					}
				}
				else
					// Ask the filter graph to play and release the interface
					hr = pMC->Run();
				pMC->Release();
				
				if( SUCCEEDED(hr) )
				{
					return;
				}	
			}
			
		}
	}	
} // PlayMp3

//
// StopMp3
//
//

void CMp3Manager::StopMp3()
{
	HRESULT hr;
	IMediaControl *pMC;
	
	if(Active)
	{
		// Obtain the interface to our filter graph
		hr = media.pGraph->QueryInterface(IID_IMediaControl, 
			(void **) &pMC);
		if( SUCCEEDED(hr) )
		{
			pMC->StopWhenReady();	//	stop it!
			pMC->Release();
			ChangeStateTo( Stopped );
		}
	}
	return ;
} //StopMp3