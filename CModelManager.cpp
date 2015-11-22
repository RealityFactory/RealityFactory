/*
CModelManager.cpp:		Animated World Model Manager

  This file contains the class implementation of the Model Manager
  system for the Rabid Game Framework.
*/

#include "RabidFramework.h"		// The One True Include File

extern "C" void	DrawBoundBox(geWorld *World, const geVec3d *Pos, const geVec3d *Min, const geVec3d *Max);
extern geSound_Def *SPool_Sound(char *SName);  //MOD010122 - Added to handle SOUND events

//	Default constructor

CModelManager::CModelManager()
{
	for(int nTemp = 0; nTemp < 512; nTemp++)
		MainList[nTemp] = NULL;
	
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	pSet = geWorld_GetEntitySet(CCD->World(), "ModelStateModifier");
	if(!pSet)
		return;
	
	//	Once more we scan the door list.  Does this get old, or what?
	
	for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
	pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
	{
		// Get the door data so we can compare models
		ModelStateModifier *pMod = (ModelStateModifier*)geEntity_GetUserData(pEntity);
		AddModel(pMod->theModel, ENTITY_ATTRIBUTE_MOD);
		pMod->DoForce = false;
		pMod->DoDamage = false;
		pMod->DTime = 0.0f;
		pMod->FTime = 0.0f;
		pMod->theFSound = NULL;
		pMod->theDSound = NULL;
		if(!EffectC_IsStringNull(pMod->ForceAudioEffect))
			pMod->theFSound = SPool_Sound(pMod->ForceAudioEffect);
		if(!EffectC_IsStringNull(pMod->DamageAudioEffect))
			pMod->theDSound = SPool_Sound(pMod->DamageAudioEffect);
	}
	
	return;
}

//	Default destructor.  Clean out our in-memory model database

CModelManager::~CModelManager()
{
	for(int nTemp = 0; nTemp < 512; nTemp++)
	{
		if(MainList[nTemp] == NULL)
			continue;					// Empty slot, ignore
		delete MainList[nTemp];
		MainList[nTemp] = NULL;
	}
	
	return;
}

//	AddModel
//
//	Add a model to be managed to the in-memory database

int CModelManager::AddModel(geWorld_Model *theModel, int nModelType)
{
	return AddNewModelToList(theModel, nModelType);
}

//	RemoveModel
//
//	Remove a model from automatic management

int CModelManager::RemoveModel(geWorld_Model *theModel)
{
	return RemoveModelFromList(theModel);
}

//	Start
//
//	Start animation for a specific model

int CModelManager::Start(geWorld_Model *theModel)
{
	ModelInstanceList *pEntry = FindModel(theModel);
	if(pEntry == NULL)
	{
		OutputDebugString("START for unmanaged model?\n");
		return RGF_NOT_FOUND;					// Model not managed by us...
	}
	
	pEntry->bForward = true;				// Running forwards
	pEntry->bMoving = true;					// Model will move on next tick
	pEntry->bHasMoved = true;                               //MOD010122 - Added - Model has been moved
    pEntry->ModelTime = 0.0f;				// Reset the timer
    pEntry->AnimStartTime = pEntry->ModelTime;              //MOD010122 - Added - Time animation was started
	
	geWorld_OpenModel(CCD->World(), pEntry->Model, GE_TRUE);
	
	return RGF_SUCCESS;
}
//MOD010122 - Added ReStart function
//	ReStart
//
//	ReStart animation for a specific model

int CModelManager::ReStart(geWorld_Model *theModel)
{
	ModelInstanceList *pEntry = FindModel(theModel);
	if(pEntry == NULL)
	{
		OutputDebugString("RESTART for unmanaged model?\n");
		return RGF_NOT_FOUND;					// Model not managed by us...
	}
	
	pEntry->bMoving = true;					// Model will move on next tick
	pEntry->AnimStartTime = pEntry->ModelTime;
	
	return RGF_SUCCESS;
}

//	Stop
//
//	Stop animation for a specific model

int CModelManager::Stop(geWorld_Model *theModel)
{
	ModelInstanceList *pEntry = FindModel(theModel);
	if(pEntry == NULL)
		return RGF_NOT_FOUND;					// Model not managed by us...
	
	pEntry->bMoving = false;				// Model stops moving now
	
	if (pEntry->bOneShot)                                          //MOD010122 - Close model if OneShot is selected
		geWorld_OpenModel(CCD->World(), pEntry->Model, GE_FALSE);
	
	return RGF_SUCCESS;
}

//	Reset
//
//	Reset the state of an animated model to its defaults

int CModelManager::Reset(geWorld_Model *theModel)
{
	ModelInstanceList *pEntry = FindModel(theModel);
	if(pEntry == NULL)
		return RGF_NOT_FOUND;					// Model not managed by us...
	
	//	Ok, reset everything about this model to it's initial values
	
	pEntry->bForward = true;
	pEntry->bMoving = false;
	pEntry->ModelAnimationSpeed = 1.0f;
	pEntry->ModelTime = 0.0f;
	pEntry->bLooping = false;
	
	return RGF_SUCCESS;
}

//	IsRunning
//
//	Return true if the model is animating, otherwise false

bool CModelManager::IsRunning(geWorld_Model *theModel)
{
	ModelInstanceList *pEntry = FindModel(theModel);
	if(pEntry == NULL)
		return false;					// Model not managed by us...
	
	return pEntry->bMoving;
}

//MOD010122 - Added HasMoved function
//	HasMoved
//
//	Return true if the model has moved, otherwise false

bool CModelManager::HasMoved(geWorld_Model *theModel)
{
	ModelInstanceList *pEntry = FindModel(theModel);
	if(pEntry == NULL)
		return false;					// Model not managed by us...
	
	return pEntry->bHasMoved;
}

//	IsModel
//
//	Is the model pointer passed in a managed world model?

bool CModelManager::IsModel(geWorld_Model *theModel)
{
	ModelInstanceList *pEntry = FindModel(theModel);
	if(pEntry == NULL)
		return false;					// Model not managed by us...
	
	return true;			// Sure is.
}

//	SetLooping
//
//	Set the automatic looping flag for a model

int CModelManager::SetLooping(geWorld_Model *theModel, bool bLoopIt)
{
	ModelInstanceList *pEntry = FindModel(theModel);
	if(pEntry == NULL)
		return RGF_NOT_FOUND;					// Model not managed by us...
	
	pEntry->bLooping = bLoopIt;			// Set loopy flag
	
	return RGF_SUCCESS;
}

//MOD010122 - Added SetOneShot function
//	SetOneShot
//
//	Set the OneShot flag for a model

int CModelManager::SetOneShot(geWorld_Model *theModel, bool bOS)
{
	ModelInstanceList *pEntry = FindModel(theModel);
	if(pEntry == NULL)
		
		pEntry->bOneShot = bOS;			// Set OneShot flag
	
	return RGF_SUCCESS;
}

//	SetReverseOnCollide
//
//	Sets the desired models 'reverse direction on collide' flag.  This
//	..is useful for doors when you don't want them trapping an actor.

int CModelManager::SetReverseOnCollide(geWorld_Model *theModel, bool bReverseIt)
{
	ModelInstanceList *pEntry = FindModel(theModel);
	if(pEntry == NULL)
		return RGF_NOT_FOUND;					// Model not managed by us...
	
	pEntry->bReverseOnCollision = bReverseIt;
	
	return RGF_SUCCESS;
}

int CModelManager::SetReverse(geWorld_Model *theModel, bool bReverseIt)
{
	ModelInstanceList *pEntry = FindModel(theModel);
	if(pEntry == NULL)
		return RGF_NOT_FOUND;					// Model not managed by us...
	
	pEntry->bReverse = bReverseIt;
	
	return RGF_SUCCESS;
}

int CModelManager::SetRotating(geWorld_Model *theModel, bool bRotating)
{
	ModelInstanceList *pEntry = FindModel(theModel);
	if(pEntry == NULL)
		return RGF_NOT_FOUND;					// Model not managed by us...
	
	pEntry->bRotating = bRotating;
	
	return RGF_SUCCESS;
}
//MOD010122 - Added SetRunTimed function
//	SetRunTimed
//
//	Set the run timed flag for a model

int CModelManager::SetRunTimed(geWorld_Model *theModel, bool bRT)
{
	ModelInstanceList *pEntry = FindModel(theModel);
	if(pEntry == NULL)
		return RGF_NOT_FOUND;					// Model not managed by us...
	
	pEntry->bRunTimed = bRT;			// Set run timed flag
	
	return RGF_SUCCESS;
}

//MOD010122 - Added SetTimeEachTrig function
//	SetTimeEachTrig
//
//	Set the time each trigger value for a model

int CModelManager::SetTimeEachTrig(geWorld_Model *theModel, geFloat fTET)
{
	ModelInstanceList *pEntry = FindModel(theModel);
	if(pEntry == NULL)
		return RGF_NOT_FOUND;					// Model not managed by us...
	
	pEntry->TimeEachTrig = fTET * 1000.0f;	 		    // Set TimeEachTrig value
	
	return RGF_SUCCESS;
}

//MOD010122 - Added SetRunFromList function
//	SetRunFromList
//
//	Set the run from list flag for a model

int CModelManager::SetRunFromList(geWorld_Model *theModel, bool bRFL)
{
	ModelInstanceList *pEntry = FindModel(theModel);
	if(pEntry == NULL)
		return RGF_NOT_FOUND;					// Model not managed by us...
	
   	pEntry->bRunFromList = bRFL;			// Set run from list flag
	
	return RGF_SUCCESS;
}

//MOD010122 - Added SetTimeList function
//	SetTimeList
//
//	Set the time list for a model
//  Note: Only 20 list items are supported

int CModelManager::SetTimeList(geWorld_Model *theModel, geFloat *fTL)
{
	ModelInstanceList *pEntry = FindModel(theModel);
	if(pEntry == NULL)
		return RGF_NOT_FOUND;					// Model not managed by us...
	
	memcpy(pEntry->TimeList, fTL, sizeof(pEntry->TimeList)); // Set TimeList values
	for (int i = 0; i < (sizeof(pEntry->TimeList)/sizeof(pEntry->TimeList[0])); i++)
	{
		pEntry->TimeList[i] *= 1000.0f;  // Scale values for model manager
	}
	pEntry->TLIndex = 0;
	
	return RGF_SUCCESS;
}

//	SetAnimationSpeed
//
//	Set the animation speed percentage for the specified world model.

int CModelManager::SetAnimationSpeed(geWorld_Model *theModel, geFloat fSpeed)
{
	ModelInstanceList *pEntry = FindModel(theModel);
	if(pEntry == NULL)
		return RGF_NOT_FOUND;					// Model not managed by us...
	
	pEntry->ModelAnimationSpeed = fSpeed;		// Adjust animation speed
	
	return RGF_SUCCESS;
}

//	Tick
//
//	Dispatch time to all animating models, let them move, and make any
//	..adjustments to actors etc. in the vicinity if necessary.

void CModelManager::Tick(geFloat dwTicks)
{
	geMotion *pMotion;
	gePath *pPath;
	geFloat tStart, tEnd, theTime;
    //MOD010122 - Added variable declarations in the next two lines
    geFloat    TempTimeDelta, NextTime, TargetTime;
    const char *Eventstring;
	
	geEntity_EntitySet *pSet;
	geEntity *pEntity;
	
	pSet = geWorld_GetEntitySet(CCD->World(), "ModelStateModifier");
	if(pSet)
	{
		
		//	Once more we scan the door list.  Does this get old, or what?
		
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
			// Get the door data so we can compare models
			ModelStateModifier *pMod = (ModelStateModifier*)geEntity_GetUserData(pEntity);
			if(pMod->DoForce)
			{
				pMod->FTime+=(dwTicks*0.001f);
				if(pMod->FTime>=pMod->ForceTimeDelay)
					pMod->DoForce = false;
			}
			if(pMod->DoDamage)
			{
				pMod->DTime+=(dwTicks*0.001f);
				if(pMod->DTime>=pMod->DamageTimeDelay)
					pMod->DoDamage = false;
			}
		}
	}

	for(int nEntry = 0; nEntry < 512; nEntry++)
	{
		if(MainList[nEntry] != NULL)
		{
			// Is this model moving?
			if(MainList[nEntry]->bMoving == false)
				continue;						// Not moving, ignore it.
			// Make sure we have animation data for the model
			pMotion= geWorld_ModelGetMotion(MainList[nEntry]->Model);
			if(!pMotion)					// No motion data?
			{
				//CCD->ReportError("No motion for model", false);
				//MOD010122 - Added next two lines
				MainList[nEntry]->bMoving = false;
				MainList[nEntry]->bHasMoved = false;
				continue;
			}
			pPath= geMotion_GetPath(pMotion, 0);
			if(!pPath)						// No path data?
			{
				//CCD->ReportError("No path for model animation", false);
				//MOD010122 - Added next two lines
				MainList[nEntry]->bMoving = false;
				MainList[nEntry]->bHasMoved = false;
				continue;
			}
			// Compute animation time and find our position
			theTime = MainList[nEntry]->ModelTime / 1000.0f;
			geMotion_GetTimeExtents(pMotion, &tStart, &tEnd);
			// Save off the previous animation time in case we need to back up
			MainList[nEntry]->OldModelTime = MainList[nEntry]->ModelTime;
			// Now, check the direction
			if(MainList[nEntry]->bForward)
			{
				// Model is animating FORWARDS along timeline
				MainList[nEntry]->ModelTime += (dwTicks * MainList[nEntry]->ModelAnimationSpeed);
				//MOD010122 - The next section of code has been extensively modified.
				//            Another marker such as this will signal the end of the changed area.
				
				if (MainList[nEntry]->bRunTimed)
				{
					TempTimeDelta = MainList[nEntry]->ModelTime - MainList[nEntry]->AnimStartTime;
					if ( TempTimeDelta >= MainList[nEntry]->TimeEachTrig )
					{
						MainList[nEntry]->ModelTime = MainList[nEntry]->AnimStartTime + 
							MainList[nEntry]->TimeEachTrig;
						MainList[nEntry]->bMoving = false;		// Kill the motion
					}
					TargetTime = MainList[nEntry]->ModelTime;
				}
				if (MainList[nEntry]->bRunFromList)
				{
					if (MainList[nEntry]->ListWrapAround && (MainList[nEntry]->ModelTime >= MainList[nEntry]->TimeList[MainList[nEntry]->TLIndex]))
					{
						if (MainList[nEntry]->ModelTime >= (tEnd * 1000.0f))
						{
							MainList[nEntry]->ListWrapAround = false;
							MainList[nEntry]->ModelTime = (MainList[nEntry]->ModelTime - (tEnd * 1000.0f)) + (tStart * 1000.0f);
						}
					}
					if (!MainList[nEntry]->ListWrapAround && (MainList[nEntry]->ModelTime >= MainList[nEntry]->TimeList[MainList[nEntry]->TLIndex]))
					{
						MainList[nEntry]->ModelTime = MainList[nEntry]->TimeList[MainList[nEntry]->TLIndex];
						TargetTime = MainList[nEntry]->ModelTime;
						if (++MainList[nEntry]->TLIndex >= TIME_LIST_MAX)
						{
							NextTime = -1000.0;
						}
						else
							NextTime = MainList[nEntry]->TimeList[MainList[nEntry]->TLIndex];
						if (NextTime == -1000.0)
						{
							if(MainList[nEntry]->bLooping)
							{
								MainList[nEntry]->TLIndex -= 2;
								if (MainList[nEntry]->TLIndex < 0)
									MainList[nEntry]->TLIndex = 0;
								if (MainList[nEntry]->bReverse)
								{
									MainList[nEntry]->bForward = false;	// Reverse direction
								}
								else
								{
									MainList[nEntry]->TLIndex = 0;
									MainList[nEntry]->ListWrapAround = TRUE;
								}
							}
							else
							{
								//Do nothing, leave model where it is                                
							}
						}
						MainList[nEntry]->bMoving = false;		// Kill the motion
					}
					else
					{
						TargetTime = MainList[nEntry]->ModelTime;
					}
				}
				else
				{
					if((MainList[nEntry]->ModelTime / 1000.0f) >= tEnd)
					{
						TargetTime = tEnd * 1000.0f;
						if(MainList[nEntry]->bLooping)
						{
							if (MainList[nEntry]->bReverse)
							{
								MainList[nEntry]->bForward = false;	// Reverse direction
								MainList[nEntry]->ModelTime = tEnd * 1000.0f;
								MainList[nEntry]->AnimStartTime = MainList[nEntry]->ModelTime +
									TempTimeDelta;
							}
							else
							{
								MainList[nEntry]->ModelTime = tStart * 1000.0f;
								MainList[nEntry]->AnimStartTime = MainList[nEntry]->ModelTime -
									TempTimeDelta;
							}
						}
						else
						{
							MainList[nEntry]->bMoving = false;		// Kill the motion
							MainList[nEntry]->ModelTime = tEnd * 1000.0f;
						}
					}
					else
					{
						TargetTime = MainList[nEntry]->ModelTime;
					}
				}
			}
			else
			{
				// Model is animating BACKWARDS along timeline
				MainList[nEntry]->ModelTime -= (dwTicks * MainList[nEntry]->ModelAnimationSpeed);
				if (MainList[nEntry]->bRunTimed)
				{
					TempTimeDelta = MainList[nEntry]->AnimStartTime - MainList[nEntry]->ModelTime;
					if ( TempTimeDelta >= MainList[nEntry]->TimeEachTrig )
					{
						MainList[nEntry]->ModelTime = MainList[nEntry]->AnimStartTime - 
							MainList[nEntry]->TimeEachTrig;
						MainList[nEntry]->bMoving = false;		// Kill the motion
					}
					TargetTime = MainList[nEntry]->ModelTime;
				}
				if (MainList[nEntry]->bRunFromList)
				{
					if (MainList[nEntry]->ListWrapAround && (MainList[nEntry]->ModelTime <= MainList[nEntry]->TimeList[MainList[nEntry]->TLIndex]))
					{
						if (MainList[nEntry]->ModelTime <= (tStart * 1000.0f))
						{
							MainList[nEntry]->ListWrapAround = false;
							MainList[nEntry]->ModelTime = (tEnd * 1000.0f) - ((tStart * 1000.0f) - MainList[nEntry]->ModelTime);
						}
					}
					if (!MainList[nEntry]->ListWrapAround && (MainList[nEntry]->ModelTime <= MainList[nEntry]->TimeList[MainList[nEntry]->TLIndex]))
					{
						MainList[nEntry]->ModelTime = MainList[nEntry]->TimeList[MainList[nEntry]->TLIndex];
						TargetTime = MainList[nEntry]->ModelTime;
						if (--MainList[nEntry]->TLIndex < 0)
						{
							NextTime = -1000.0;
						}
						else
							NextTime = MainList[nEntry]->TimeList[MainList[nEntry]->TLIndex];
						if (NextTime == -1000.0)
						{
							if(MainList[nEntry]->bLooping)
							{
								MainList[nEntry]->TLIndex += 2;
								if (MainList[nEntry]->bReverse)
								{
									MainList[nEntry]->bForward = true;	// Reverse direction
								}
								else
								{
									for(int i = TIME_LIST_MAX - 1; i >= 0; i--)
									{
										if (MainList[nEntry]->TimeList[i] != -1)
										{
											MainList[nEntry]->TLIndex = i;
											MainList[nEntry]->ListWrapAround = TRUE;
											break;
										}
										if (i == 0)
										{
											MainList[nEntry]->bMoving = false;		// Kill the motion
										}
									}
									if (MainList[nEntry]->TLIndex < 0)
										MainList[nEntry]->TLIndex = 0;
									MainList[nEntry]->ModelTime = tEnd * 1000.0f;
								}
							}
							else
							{
								//Do nothing, leave model where it is
							}
						}
						MainList[nEntry]->bMoving = false;		// Kill the motion
					}
					else
					{
						TargetTime = MainList[nEntry]->ModelTime;
					}
				}
				else
				{
					if((MainList[nEntry]->ModelTime / 1000.0f) <= tStart)
					{
						TargetTime = tStart * 1000.0f;
						if(MainList[nEntry]->bLooping)
						{
							if(MainList[nEntry]->bReverse)
							{
								MainList[nEntry]->bForward = true;	// Reverse direction
								MainList[nEntry]->ModelTime = tStart * 1000.0f;
								MainList[nEntry]->AnimStartTime = MainList[nEntry]->ModelTime -
									TempTimeDelta;
							}
							else
							{
								MainList[nEntry]->ModelTime = tEnd * 1000.0f;
								MainList[nEntry]->AnimStartTime = MainList[nEntry]->ModelTime +
									TempTimeDelta;
							}
						}
						else
						{
							MainList[nEntry]->bMoving = false;		// Kill the motion
							MainList[nEntry]->ModelTime = tStart * 1000.0f;
						}
					}
					else
					{
						TargetTime = MainList[nEntry]->ModelTime;
					}
				}
                }
                // Time updated, let's adjust the models position...
                if(MoveModel(MainList[nEntry], pPath) == GE_TRUE)
                {
                    // Process Events in the motion
                    if (MainList[nEntry]->bForward)
                    {
                        geMotion_SetupEventIterator(pMotion,
                            (MainList[nEntry]->OldModelTime + 1.0f) / 1000.0f,
                            (TargetTime + 1.0f) / 1000.0f);
                    }
                    else
                    {
                        geMotion_SetupEventIterator(pMotion,
                            TargetTime / 1000.0f,
                            (MainList[nEntry]->OldModelTime) / 1000.0f);
                    }
                    while (geMotion_GetNextEvent(pMotion, &tStart, &Eventstring))
                    {
                        char Arg[6][128];
                        
                        memset(Arg, 0, sizeof(Arg));
                        sscanf(Eventstring, 
                            "%s %s %s %s %s %s",
                            &Arg[0], &Arg[1], &Arg[2], &Arg[3], &Arg[4], &Arg[5]);
                        switch (Arg[0][0])
                        {
                        case 'S':   // Play sound, (.wav file, non-looping)
                            {
                                if(!EffectC_IsStringNull(Arg[1]))
                                {
                                    geSound_Def *theSound;
                                    
                                    theSound=SPool_Sound(Arg[1]);
                                    if(theSound)
                                    {
                                        // We have some sound, play the sucker.
                                        geFloat Volume, Pan, Frequency;
                                        geXForm3d xfmPlayer = CCD->Player()->ViewPoint();
                                        geSound3D_GetConfig(CCD->World(), &xfmPlayer,                                                                                 &MainList[nEntry]->Translation,
                                            kAudibleRadius, 2.0f, &Volume, &Pan, &Frequency);
                                        geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), theSound, 
                                            Volume, Pan, Frequency, GE_FALSE);
                                    }
                                }
                                break;
                            }
                        case 'T':   // Set trigger
                            {
                                if(!EffectC_IsStringNull(Arg[1]))
                                {
                                    CCD->Triggers()->HandleTriggerEvent(Arg[1]);
                                }
                                break;
                            }
                        default:
                            break;
                        }
                    }
                }
                else
                {
                    MainList[nEntry]->ModelTime = MainList[nEntry]->OldModelTime;
                }
        }
        // Done processing this model, on to the next one!
        }
        
        return;
    }
    
    //MOD010122 - End of extensive modification area
	
	//	GetPosition
	//
	//	Get the current position of the desired world model.
	
	int CModelManager::GetPosition(geWorld_Model *theModel, geVec3d *thePosition)
	{
		ModelInstanceList *pEntry = FindModel(theModel);
		if(pEntry == NULL)
			return RGF_NOT_FOUND;					// Model not managed by us...
		
		*thePosition = pEntry->Translation;
		
		return RGF_SUCCESS;
	}
	
	//	GetRotation
	//
	//	Get the current rotation of the desired world model.
	
	int CModelManager::GetRotation(geWorld_Model *theModel, geVec3d *theRotation)
	{
		ModelInstanceList *pEntry = FindModel(theModel);
		if(pEntry == NULL)
			return RGF_NOT_FOUND;					// Model not managed by us...
		
		*theRotation = pEntry->Rotation;
		
		return RGF_SUCCESS;
	}
	
	int CModelManager::GetModelTime(geWorld_Model *theModel, geFloat *time)
	{
		ModelInstanceList *pEntry = FindModel(theModel);
		if(pEntry == NULL)
			return RGF_NOT_FOUND;					// Model not managed by us...
		
		if(!pEntry->bMoving)
			return RGF_FAILURE;
		
		*time = pEntry->ModelTime;
		
		return RGF_SUCCESS;
	}
	
	//	EVIL HACK ALERT
	//
	//	Due to problems in getting proper bounding box information out
	//	..of the G3D API, the following two set-and-get bounding box
	//	..calls have been added.  The manually-set bounding box will be
	//	..used elsewhere to perform collision detection.
	
	//	SetBoundingBox
	//
	//	Define the OBB for a specific managed model.
	
	int CModelManager::SetBoundingBox(geWorld_Model *theModel, geExtBox theBBox)
	{
		ModelInstanceList *pEntry = FindModel(theModel);
		if(pEntry == NULL)
			return RGF_NOT_FOUND;					// Model not managed by us...
		
		pEntry->ModelBBox = theBBox;
		
		return RGF_SUCCESS;
	}
	
	//	GetBoundingBox
	//
	//	Retrieve the previously set OBB for a specific managed model.
	
	int CModelManager::GetBoundingBox(geWorld_Model *theModel, geExtBox *theBBox)
	{
		ModelInstanceList *pEntry = FindModel(theModel);
		if(pEntry == NULL)
			return RGF_NOT_FOUND;					// Model not managed by us...
		
		*theBBox = pEntry->ModelBBox;
		
		return RGF_SUCCESS;
	}
	
	//	AddPassenger
	//
	//	Set a specified model to have a specified actor as a passenger on it.
	//	..This lasts one animation cycle, it is assumed actor gravity
	//	..processing, etc. will re-add the passenger as long as required.
	
	int CModelManager::AddPassenger(geWorld_Model *theModel, geActor *theActor)
	{
		ModelInstanceList *pEntry = FindModel(theModel);
		if(pEntry == NULL)
			return RGF_NOT_FOUND;					// Model not managed by us...
		
		for(int nX = 0; nX < 64; nX++)
		{
			if(pEntry->Passengers[nX] == NULL)
			{
				pEntry->Passengers[nX] = theActor;
				return RGF_SUCCESS;
			}
		}
		
		return RGF_FAILURE;							// Full bus?
	}
	
	//	RemovePassenger
	//
	//	Remove an actor using a model as a vehicle.
	
	int CModelManager::RemovePassenger(geWorld_Model *theModel, geActor *theActor)
	{
		ModelInstanceList *pEntry = FindModel(theModel);
		if(pEntry == NULL)
			return RGF_NOT_FOUND;					// Model not managed by us...
		
		for(int nX = 0; nX < 64; nX++)
		{
			if(pEntry->Passengers[nX] == theActor)
			{
				pEntry->Passengers[nX] = NULL;
				return RGF_SUCCESS;
			}
		}
		
		return RGF_FAILURE;							// Full bus?
	}
	
	//	IsAPassenger
	//
	//	Checks to see if a specific actor is a passenger on a model.  Returns
	//	..GE_TRUE if so, otherwise GE_FALSE.
	
	geBoolean CModelManager::IsAPassenger(geWorld_Model *theModel, geActor *theActor)
	{
		ModelInstanceList *pEntry = FindModel(theModel);
		if(pEntry == NULL)
			return GE_FALSE;					// Model not managed by us...
		
		for(int nX = 0; nX < 64; nX++)
		{
			if(pEntry->Passengers[nX] == theActor)
				return GE_TRUE;
		}
		
		return GE_FALSE;							// Not there
	}
	
int CModelManager::EmptyContent(geWorld_Model *Model)
{
		ModelInstanceList *pEntry = FindModel(Model);
		if(pEntry == NULL)
			return false;					// Model not managed by us...

		GE_Contents Contents;

		memset(&Contents, 0, sizeof(GE_Contents));
		int Result1 = geWorld_GetContents(CCD->World(), 
			&pEntry->Translation, &pEntry->Mins, &pEntry->Maxs, GE_COLLIDE_MODELS, 
			0xffffffff, NULL, NULL, &Contents);
		if(Result1 == GE_TRUE)
		{
			if((Contents.Contents & (GE_CONTENTS_EMPTY)) == GE_CONTENTS_EMPTY)
				return true;
		}
		return false;
	}

	//	DoesBoxHitModel
	//
	//	EVIL HACK!  This should NOT be necessary but the freaking G3D API
	//	..won't return a proper bounding box for an animated world model,
	//	..so we have to use manually-set ones.
	//
	//	This function takes an ExtBox and checks to see if it intersects
	//	..the ExtBox of any managed model, and if so, returns a pointer
	//	..to that model - otherwise NULL.  GE_TRUE is returned if there
	//	..was a collision, BTW.  Note that the EXTBOX coming in is assumed
	//	..to be in MODEL SPACE, we translate it into WORLD SPACE assuming
	//	..that the 'thePoint' is the origin of the ExtBox.
	
	geBoolean CModelManager::DoesBoxHitModel(geVec3d thePoint, geExtBox theBox, 
		geWorld_Model **theModel)
	{
		geExtBox Result, Temp;
		
		*theModel = NULL;
		
		for(int nTemp = 0; nTemp < 512; nTemp++)
		{
			if(MainList[nTemp] == NULL)
				continue;				// Empty slot
			
			geVec3d Forward, Up, Left, Pos;
			geXForm3d Xf;
			geWorld_GetModelXForm(CCD->World(), MainList[nTemp]->Model, &Xf); 
			geXForm3d_GetUp (&Xf, &Up);
			geVec3d_Normalize(&Up);
			geXForm3d_GetIn (&Xf, &Forward);
			geVec3d_Normalize(&Forward);
			geXForm3d_GetLeft (&Xf, &Left);
			geVec3d_Normalize(&Left);
			geWorld_GetModelRotationalCenter(CCD->World(), MainList[nTemp]->Model, &Pos);
			geVec3d_AddScaled (&Pos, &Forward, -MainList[nTemp]->Mins.Z, &Result.Min);
			geVec3d_AddScaled (&Result.Min, &Left, -MainList[nTemp]->Mins.X, &Result.Min);
			geVec3d_AddScaled (&Result.Min, &Up, MainList[nTemp]->Mins.Y, &Result.Min);
			geVec3d_AddScaled (&Pos, &Forward, -MainList[nTemp]->Maxs.Z, &Result.Max);
			geVec3d_AddScaled (&Result.Max, &Left, -MainList[nTemp]->Maxs.X, &Result.Max);
			geVec3d_AddScaled (&Result.Max, &Up, MainList[nTemp]->Maxs.Y, &Result.Max);
			
			Result.Min.X -= Pos.X;
			Result.Min.Y -= Pos.Y;
			Result.Min.Z -= Pos.Z;
			Result.Max.X -= Pos.X;
			Result.Max.Y -= Pos.Y;
			Result.Max.Z -= Pos.Z;
			float Mtemp;
			if(Result.Min.X > Result.Max.X)
			{
				Mtemp = Result.Min.X;
				Result.Min.X = Result.Max.X;
				Result.Max.X = Mtemp;
			}
			if(Result.Min.Y > Result.Max.Y)
			{
				Mtemp = Result.Min.Y;
				Result.Min.Y = Result.Max.Y;
				Result.Max.Y = Mtemp;
			}
			if(Result.Min.Z > Result.Max.Z)
			{
				Mtemp = Result.Min.Z;
				Result.Min.Z = Result.Max.Z;
				Result.Max.Z = Mtemp;
			}
			
			if(CCD->MenuManager()->GetSEBoundBox())
				DrawBoundBox(CCD->World(), &MainList[nTemp]->Translation,
				&Result.Min, &Result.Max);
			
			if(CCD->Doors()->IsADoor(MainList[nTemp]->Model) && IsRunning(MainList[nTemp]->Model) && MainList[nTemp]->bRotating)
				return GE_FALSE;
			
			Result.Min.X += MainList[nTemp]->Translation.X;
			Result.Min.Y += MainList[nTemp]->Translation.Y;
			Result.Min.Z += MainList[nTemp]->Translation.Z;
			Result.Max.X += MainList[nTemp]->Translation.X;
			Result.Max.Y += MainList[nTemp]->Translation.Y;
			Result.Max.Z += MainList[nTemp]->Translation.Z;
			if(geExtBox_Intersection(&Result, &theBox, &Temp) == GE_TRUE)
			{
				// Heh, we hit someone.  Return the model we ran into.
				*theModel = MainList[nTemp]->Model;
				return GE_TRUE;
			}
			
		}
		
		//	No hit, all be hunky-dory.
		
		return GE_FALSE;						// Hey, no collisions!
	}
	
	//	HandleCollision
	//
	//	Check to see if this model needs to do anything when collided with.
	
	int CModelManager::HandleCollision(geWorld_Model *theModel, geActor *theActor)
	{
		geEntity_EntitySet *pSet;
		geEntity *pEntity;
		static geVec3d Directions[6] = {
			{0.0f, -1.0f, 0.0f},			// DOWN
			{0.0f, 1.0f, 0.0f},				// UP
			{-1.0f, 0.0f, 0.0f},			// LEFT
			{1.0f, 0.0f, 0.0f},				// RIGHT
			{0.0f, 0.0f, 1.0f},				// FORWARD
			{0.0f, 0.0f, -1.0f}};			// BACKWARD
			
			pSet = geWorld_GetEntitySet(CCD->World(), "ModelStateModifier");
			if(!pSet)
				return RGF_FAILURE;
			
			//	Once more we scan the door list.  Does this get old, or what?
			
			for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
			pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
			{
				// Get the door data so we can compare models
				ModelStateModifier *pMod = (ModelStateModifier*)geEntity_GetUserData(pEntity);
				if(pMod->theModel != theModel)
					continue;								// Not this one
				// Ok, the passed-in model has a MODIFIER.  Let's see what it's supposed
				// ..to do.
				if(pMod->ForceImparter == GE_TRUE && !pMod->DoForce)
				{
					// Need to add force to the actor that hit us...
					if(CCD->ActorManager()->ForceActive(theActor, 2) == false)
					{
						CCD->ActorManager()->SetForce(theActor, 2, Directions[pMod->ForceVector], 
							pMod->ForceAmount, pMod->ForceDecay);
						pMod->DoForce = true;
						pMod->FTime = 0.0f;
						if(pMod->theFSound != NULL)
						{
							// eaa3 03/28/2000 Check to see if the sound is already playing,
							// ..and if so, DON'T REPLAY IT!!!
							if(geSound_SoundIsPlaying(CCD->Engine()->AudioSystem(), pMod->FSoundHandle) != GE_TRUE)
							{
								// We have some sound, play the sucker.
								geFloat Volume, Pan, Frequency;
								geXForm3d xfmPlayer = CCD->Player()->ViewPoint();
								geSound3D_GetConfig(CCD->World(),	&xfmPlayer,	&pMod->origin,
									kAudibleRadius, 2.0f,	&Volume, &Pan,	&Frequency);
								pMod->FSoundHandle = geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), 
									pMod->theFSound, Volume, Pan, Frequency, GE_FALSE);
							}
							// End of sound checking
						}
					}
				}
				if(pMod->DamageGenerator == GE_TRUE && !pMod->DoDamage)
				{
					// Need to damage the actor that hit us...
					if(theActor)
					{
						if(EffectC_IsStringNull(pMod->DamageAttribute))
						{
							CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(theActor);
							if(theInv->Value("health")>0)
								CCD->Damage()->DamageActor(theActor, pMod->DamageAmount, "health");
							else
							{
								if(EffectC_IsStringNull(pMod->DamageAltAttribute))
									CCD->Damage()->DamageActor(theActor, pMod->DamageAmount, pMod->DamageAltAttribute);
							}
						}
						else
						{
							CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(theActor);
							if(theInv->Value(pMod->DamageAttribute)>0)
								CCD->Damage()->DamageActor(theActor, pMod->DamageAmount, pMod->DamageAttribute);
							else
							{
								if(EffectC_IsStringNull(pMod->DamageAltAttribute))
									CCD->Damage()->DamageActor(theActor, pMod->DamageAmount, pMod->DamageAltAttribute);
							}
						}
					}
					pMod->DoDamage = true;
					pMod->DTime = 0.0f;
					if(pMod->theDSound != NULL)
					{
						// eaa3 03/28/2000 Check to see if the sound is already playing,
						// ..and if so, DON'T REPLAY IT!!!
						if(geSound_SoundIsPlaying(CCD->Engine()->AudioSystem(), pMod->DSoundHandle) != GE_TRUE)
						{
							// We have some sound, play the sucker.
							geFloat Volume, Pan, Frequency;
							geXForm3d xfmPlayer = CCD->Player()->ViewPoint();
							geSound3D_GetConfig(CCD->World(),	&xfmPlayer,	&pMod->origin,
								kAudibleRadius, 2.0f,	&Volume, &Pan,	&Frequency);
							pMod->DSoundHandle = geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), 
								pMod->theDSound, Volume, Pan, Frequency, GE_FALSE);
						}
						// End of sound checking
					}
				}
				if(EmptyContent(pMod->theModel))
					return RGF_EMPTY;
				return RGF_SUCCESS;							// We handled it.
			}
			
			return RGF_FAILURE;								// Nope, didn't process collision
	}
	
	//	GetModifier
	//
	//	Get any modifiers for the passed-in model.  Note that the caller is
	//	..expected to delete the returned struct.
	
	ModelStateModifier *CModelManager::GetModifier(geWorld_Model *theModel)
	{
		geEntity_EntitySet *pSet;
		geEntity *pEntity;
		
		pSet = geWorld_GetEntitySet(CCD->World(), "ModelStateModifier");
		if(!pSet)
			return false;
		
		//	Once more we scan the door list.  Does this get old, or what?
		
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
			// Get the door data so we can compare models
			ModelStateModifier *pMod = (ModelStateModifier*)geEntity_GetUserData(pEntity);
			if(pMod->theModel != theModel)
				continue;								// Not this one
			ModelStateModifier *ModPerm = new ModelStateModifier;
			memcpy(ModPerm, pMod, sizeof(ModelStateModifier));
			return ModPerm;						// Users problem to delete struct now...
		}
		
		return NULL;								// No modifiers for the desired model
	}
	
	//	GetAttributes
	//
	//	Check to see if a specified model has any attached attributes, and if
	//	..so, return a pointer to the attributes structure.  NOTE THAT IT IS
	//	..THE CALLERS RESPONSIBILITY TO DELETE THE RETURNED POINTER!
	
	ModelAttributes *CModelManager::GetAttributes(geWorld_Model *theModel)
	{
		geEntity_EntitySet *pSet;
		geEntity *pEntity;
		
		pSet = geWorld_GetEntitySet(CCD->World(), "ModelAttributes");
		if(!pSet)
			return NULL;
		
		//	Once more we scan the door list.  Does this get old, or what?
		
		for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
		{
			// Get the door data so we can compare models
			ModelAttributes *pMod = (ModelAttributes*)geEntity_GetUserData(pEntity);
			if(pMod->theModel != theModel)
				continue;								// Not this one
			ModelAttributes *ModPerm = new ModelAttributes;
			memcpy(ModPerm, pMod, sizeof(ModelAttributes));
			return ModPerm;						// Users problem to delete struct now...
		}
		
		return NULL;								// No attributes for the desired model
	}
	
	//	******** PRIVATE MEMBER FUNCTIONS **********
	
	//	AddNewModelToList
	//
	//	Add a model to be managed to the in-memory database
	
	int CModelManager::AddNewModelToList(geWorld_Model *theModel, int nModelType)
	{
		int i;  //MOD010122 - Used for index
		
		for(int nTemp = 0; nTemp < 512; nTemp++)
		{
			// Let's find a free entry
			if(MainList[nTemp] == NULL)
			{
				MainList[nTemp] = new ModelInstanceList;
				memset(MainList[nTemp], 0, sizeof(ModelInstanceList));
				MainList[nTemp]->Model = theModel;
				MainList[nTemp]->bForward = true;
				MainList[nTemp]->bMoving = false;
				MainList[nTemp]->ModelAnimationSpeed = 1.0f;		// 100%
				MainList[nTemp]->ModelTime = 0.0f;
				MainList[nTemp]->ModelType = nModelType;
				MainList[nTemp]->bLooping = false;
				MainList[nTemp]->bRotating = false;
				MainList[nTemp]->bReverse = true;
				//MOD010122 - Added next seven variable initializations
                MainList[nTemp]->bRunTimed = false;
                MainList[nTemp]->TimeEachTrig = 1.0f;
                MainList[nTemp]->bRunFromList = false;
                for (i=0; i<TIME_LIST_MAX; i++)
                {
                    MainList[nTemp]->TimeList[i] = 0.0f;
                }
                MainList[nTemp]->TLIndex = 0;
                MainList[nTemp]->AnimStartTime = 0.0f;
                MainList[nTemp]->ListWrapAround = false;
                //MOD010122 - End of variable init additions
                
				MainList[nTemp]->bReverseOnCollision = false;
				geWorld_ModelGetBBox(CCD->World(), MainList[nTemp]->Model,
					&MainList[nTemp]->Mins, &MainList[nTemp]->Maxs);
				MainList[nTemp]->Mins.X += 127.0f;
				MainList[nTemp]->Mins.Y += 127.0f;
				MainList[nTemp]->Mins.Z += 127.0f;
				MainList[nTemp]->Maxs.X -= 127.0f;
				MainList[nTemp]->Maxs.Y -= 127.0f;
				MainList[nTemp]->Maxs.Z -= 127.0f;
				geWorld_GetModelRotationalCenter(CCD->World(), theModel, 
					&MainList[nTemp]->Translation);
				geWorld_ModelSetFlags(theModel, GE_MODEL_RENDER_NORMAL |
					GE_MODEL_RENDER_MIRRORS | GE_MODEL_COLLIDE);
				for(int nFoo = 0; nFoo < 64; nFoo++)
					MainList[nTemp]->Passengers[nFoo] = NULL;
				return RGF_SUCCESS;
			}
		}
		
		return RGF_NO_FREE_SPACE;					// No room for more world models
	}
	
	//	RemoveModelFromList
	//
	//	Remove a model from the in-memory database
	
	int CModelManager::RemoveModelFromList(geWorld_Model *theModel)
	{
		for(int nTemp = 0; nTemp < 512; nTemp++)
		{
			// Let's find the right
			if((MainList[nTemp] != NULL) && (MainList[nTemp]->Model == theModel))
			{
				delete MainList[nTemp];
				MainList[nTemp] = NULL;
				return RGF_SUCCESS;
			}
		}
		
		return RGF_NOT_FOUND;						// Couldn't find it!
	}
	
	//	FindModel
	//
	//	Locate a models entry in the database, return a pointer to it.
	
	ModelInstanceList *CModelManager::FindModel(geWorld_Model *theModel)
	{
		for(int nTemp = 0; nTemp < 512; nTemp++)
		{
			// Let's find the right
			if((MainList[nTemp] != NULL) && (MainList[nTemp]->Model == theModel))
				return MainList[nTemp];			// Entry to caller
		}
		
		return NULL;										// Heh, not found, sucker!
	}
	
	//	MoveModel
	//
	//	Given a model instance, move the dratted thing.  Note that this will
	//	..perform motion checking to make sure that all actors within the
	//	..models bounding box will remain unaffected by its motion...
	
	int CModelManager::MoveModel(ModelInstanceList *theEntry, gePath *pPath)
	{
		geActor *ActorsInRange[512];
		geVec3d ModelCenter;
		geFloat ModelRange = 500.0f;			// Reasonable default *HACK*
		geXForm3d xfmDest, xfmOldPosition;
		geFloat theTime;
		
		// Compute animation time and find our position
		
		theTime = theEntry->ModelTime / 1000.0f;
		
		//	Get the transform to be added to the origin for the platform model.  Also
		//	..grab the old position for later use...
		
		gePath_Sample(pPath, theTime, &xfmDest);
		gePath_Sample(pPath, (theEntry->OldModelTime / 1000.0f), &xfmOldPosition);
		geWorld_GetModelRotationalCenter(CCD->World(), theEntry->Model, &ModelCenter);
		
		ModelCenter.X += xfmDest.Translation.X;
		ModelCenter.Y += xfmDest.Translation.Y;
		ModelCenter.Z += xfmDest.Translation.Z;
		
		// Get the Euler angles out of the transform for our rotation
		
		geXForm3d_GetEulerAngles(&xfmDest, &theEntry->Rotation);
		
		//	At this point, we need to check to see if we have any actors within a
		//	..reasonable range of our platform, and if so, we'll need to check to see
		//	..if moving the platform will end up forcing any of the actors in this
		//	..platforms range to move.
		
		int nActorCount = CCD->ActorManager()->GetActorsInRange(ModelCenter, 
			ModelRange, 512, &ActorsInRange[0]);
		
		if(nActorCount == 0)
		{
			// No actors to test collisions with, LET'S ROCK ON!
			geWorld_SetModelXForm(CCD->World(), theEntry->Model, &xfmDest);
			theEntry->Translation = ModelCenter;			// Update position
			return RGF_SUCCESS;
		}
		
		//	Ugh, we have to check EACH and EVERY ACTOR that was returned for a possible
		//	..collision.  Is this a pain, or what?
		
		geVec3d ActorPosition, NewActorPosition;
		bool fModelMoveOK;
		
		for(int nTemp = 0; nTemp < nActorCount; nTemp++)
		{
			// Note that, if at ANY point we can't adjust an actors position to allow
			// ..the model to move we abort moving the model AT ALL and set the current
			// ..time back to the previous time, effectively "freezing time" for the
			// ..model.
			CCD->ActorManager()->GetPosition(ActorsInRange[nTemp], &ActorPosition);
			fModelMoveOK = CCD->Collision()->CheckModelMotionActor(theEntry->Model, &xfmDest,
				ActorsInRange[nTemp], &ActorPosition, &NewActorPosition);
			if(fModelMoveOK == false)
				return RGF_FAILURE;					// Couldn't move the model
			geVec3d Delta1 = xfmDest.Translation;
			Delta1.X = Delta1.X - xfmOldPosition.Translation.X;
			Delta1.Y = Delta1.Y - xfmOldPosition.Translation.Y;
			Delta1.Z = Delta1.Z - xfmOldPosition.Translation.Z;
			if(IsAPassenger(theEntry->Model, ActorsInRange[nTemp]))
			{
				CCD->ActorManager()->AddTranslation(ActorsInRange[nTemp], Delta1);
				RemovePassenger(theEntry->Model, ActorsInRange[nTemp]);
			}
			else
				CCD->ActorManager()->Position(ActorsInRange[nTemp], NewActorPosition);
			// Ok, if we DID collide (ActorPosition != NewActorPosition) and this
			// ..model is flagged as reverse on collision, we need to flip this
			// ..models motion direction
			if((memcmp(&ActorPosition, &NewActorPosition, sizeof(geVec3d)) != 0) &&
				(theEntry->bReverseOnCollision == true))
				theEntry->bForward = !theEntry->bForward;		// Switch direction
		}
		
		//	Ok, at this point we _know_ no actors need be moved to compensate for the
		//	..motion of this model, so let's Just Move It.
		
		geWorld_SetModelXForm(CCD->World(), theEntry->Model, &xfmDest);
		
		theEntry->Translation = ModelCenter;			// Update position
		
		return RGF_SUCCESS;
	}
