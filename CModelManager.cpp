/************************************************************************************//**
 * @file CModelManager.cpp
 * @brief Animated World Model Manager
 *
 * This file contains the class implementation of the Model Manager system for
 * Reality Factory.
 *//*
 ****************************************************************************************/

#include "RabidFramework.h"		// The One True Include File
#include "CAutoDoors.h"
#include "CDamage.h"
#include "CTriggers.h"

extern "C" void	DrawBoundBox(geWorld *World, const geVec3d *Pos, const geVec3d *Min, const geVec3d *Max);
extern geSound_Def *SPool_Sound(const char *SName);

// Modular data for the Tick routines
int ModelState[MODEL_LIST_SIZE];

#define NOT_PROCESSED	0
#define NO_MOVEMENT		1
#define MOVED_IT		2
#define CANT_MOVE		3

/* ------------------------------------------------------------------------------------ */
// Default constructor
/* ------------------------------------------------------------------------------------ */
CModelManager::CModelManager()
{
	for(int nTemp=0; nTemp<MODEL_LIST_SIZE; ++nTemp)
		MainList[nTemp] = NULL;

	ManagedModels = 0;


	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "ModelStateModifier");

	if(!pSet)
		return;

	geEntity *pEntity;

	// Once more we scan the door list.  Does this get old, or what?
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		// Get the door data so we can compare models
		ModelStateModifier *pMod = static_cast<ModelStateModifier*>(geEntity_GetUserData(pEntity));

		pMod->DoForce	= false;
		pMod->DoDamage	= false;
		pMod->DTime		= 0.0f;
		pMod->FTime		= 0.0f;
		pMod->theFSound = NULL;
		pMod->theDSound = NULL;

		if(!EffectC_IsStringNull(pMod->ForceAudioEffect))
			pMod->theFSound = SPool_Sound(pMod->ForceAudioEffect);

		if(!EffectC_IsStringNull(pMod->DamageAudioEffect))
			pMod->theDSound = SPool_Sound(pMod->DamageAudioEffect);
	}
}

/* ------------------------------------------------------------------------------------ */
// Default destructor.
// Clean out our in-memory model database
/* ------------------------------------------------------------------------------------ */
CModelManager::~CModelManager()
{
	for(int nTemp=0; nTemp<MODEL_LIST_SIZE; ++nTemp)
	{
		if(MainList[nTemp] == NULL)
			continue;					// Empty slot, ignore

		delete MainList[nTemp];
		MainList[nTemp] = NULL;
	}
}

/* ------------------------------------------------------------------------------------ */
// AddModel
//
// Add a model to be managed to the in-memory database
/* ------------------------------------------------------------------------------------ */
int CModelManager::AddModel(geWorld_Model *theModel, int nModelType)
{
	return AddNewModelToList(theModel, nModelType);
}

/* ------------------------------------------------------------------------------------ */
// RemoveModel
//
// Remove a model from automatic management
/* ------------------------------------------------------------------------------------ */
int CModelManager::RemoveModel(const geWorld_Model *theModel)
{
	return RemoveModelFromList(theModel);
}

/* ------------------------------------------------------------------------------------ */
// Start
//
// Start animation for a specific model
/* ------------------------------------------------------------------------------------ */
int CModelManager::Start(const geWorld_Model *theModel)
{
	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
	{
		OutputDebugString("START for unmanaged model?\n");
		return RGF_NOT_FOUND;					// Model not managed by us...
	}

	pEntry->bForward		= true;				// Running forwards
	pEntry->bMoving			= true;				// Model will move on next tick
	pEntry->bHasMoved		= true;				// Model has been moved
	pEntry->ModelTime		= 0.0f;				// Reset the timer
	pEntry->AnimStartTime	= pEntry->ModelTime;// Time animation was started

	geWorld_OpenModel(CCD->World(), pEntry->Model, GE_TRUE);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// ReStart
//
// ReStart animation for a specific model
/* ------------------------------------------------------------------------------------ */
int CModelManager::ReStart(const geWorld_Model *theModel)
{
	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
	{
		OutputDebugString("RESTART for unmanaged model?\n");
		return RGF_NOT_FOUND;				// Model not managed by us...
	}

	pEntry->bMoving = true;					// Model will move on next tick
	pEntry->AnimStartTime = pEntry->ModelTime;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// Stop
//
// Stop animation for a specific model
/* ------------------------------------------------------------------------------------ */
int CModelManager::Stop(const geWorld_Model *theModel)
{
	geXForm3d Xf;
	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
		return RGF_NOT_FOUND;				// Model not managed by us...

	pEntry->bMoving = false;				// Model stops moving now

	geWorld_GetModelXForm(CCD->World(), pEntry->Model, &Xf);

	if(memcmp(&Xf, &pEntry->Xf, sizeof(geXForm3d)) == 0)
	{
		geWorld_OpenModel(CCD->World(), pEntry->Model, GE_FALSE);
	}
	else
	{
		geWorld_OpenModel(CCD->World(), pEntry->Model, GE_TRUE);
	}

	// if (pEntry->bOneShot)				// Close model if OneShot is selected
	// geWorld_OpenModel(CCD->World(), pEntry->Model, GE_FALSE);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetParentModel
/* ------------------------------------------------------------------------------------ */
int CModelManager::SetParentModel(geWorld_Model *theModel, geWorld_Model *ParentModel)
{
	ModelInstanceList *pEntry;

	// Find the Model and set its parent pointer
	pEntry = FindModel(theModel);

	if(pEntry == NULL)
	{
		OutputDebugString("SetParentModel for unmanaged model?\n");
		return RGF_NOT_FOUND;					// Model not managed by us...
	}

	pEntry->ParentModel = ParentModel;

	// Find the Parent Model and set its child pointer,
	// or if a child has already been set then add to the child's sibling pointer list
	pEntry = FindModel(ParentModel);

	if(pEntry == NULL)
	{
		OutputDebugString("SetParentModel: Parent model not found.\n");
		return RGF_NOT_FOUND;					// Model not managed by us...
	}

	if(!pEntry->ChildModel)
	{
		pEntry->ChildModel = theModel;
	}
	else
	{
		pEntry = FindModel(pEntry->ChildModel);

		if(pEntry == NULL)
		{
			OutputDebugString("SetParentModel: Parent's child model not found.\n");
			return RGF_NOT_FOUND;					// Model not managed by us...
		}

		if(!pEntry->SiblingModel)
		{
			pEntry->SiblingModel = theModel;
		}
		else
		{
			do
			{
				pEntry = FindModel(pEntry->SiblingModel);

				if(pEntry == NULL)
				{
					OutputDebugString("SetParentModel: Parent's child's sibling model not found.\n");
					return RGF_NOT_FOUND;					// Model not managed by us...
				}

				if(!pEntry->SiblingModel)
				{
					pEntry->SiblingModel = theModel;
					break;
				}
			} while(pEntry != NULL);
		}
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetModelOriginOffset
/* ------------------------------------------------------------------------------------ */
int CModelManager::SetModelOriginOffset(const geWorld_Model *theModel, const geVec3d *OriginOffset)
{
	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
	{
		OutputDebugString("SetOriginOffset for unmanaged model?\n");
		return RGF_NOT_FOUND;					// Model not managed by us...
	}

	geVec3d_Copy(OriginOffset, &(pEntry->OriginOffset));
	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetModelOrigin
/* ------------------------------------------------------------------------------------ */
int CModelManager::SetModelOrigin(const geWorld_Model *theModel, const geVec3d *Origin)
{
	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
	{
		OutputDebugString("SetOrigin for unmanaged model?\n");
		return RGF_NOT_FOUND;					// Model not managed by us...
	}

	geVec3d_Copy(Origin, &(pEntry->origin));
	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetTargetTime
/* ------------------------------------------------------------------------------------ */
int CModelManager::SetTargetTime(const geWorld_Model *theModel, geFloat TT)
{
	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
	{
		OutputDebugString("SetTargetTime for unmanaged model?\n");
		return RGF_NOT_FOUND;					// Model not managed by us...
	}

	if(TT < 0.0f)
	{
		// Stop
		pEntry->bMoving = false;				// Model stops moving now
		pEntry->TargetTime = TT;
	}
	else
	{
		pEntry->TargetTime = TT * 1000.0f;

		if(pEntry->bHasMoved)
		{
			// Restart
			if(pEntry->TargetTime < pEntry->ModelTime)
				pEntry->bForward = false;
			else
				pEntry->bForward = true;

			pEntry->bMoving = true;						// Model will move on next tick
			pEntry->AnimStartTime = pEntry->ModelTime;	// Time animation was started
		}
		else
		{
			// Start
			pEntry->bForward		= true;				// Running forwards
			pEntry->bMoving			= true;				// Model will move on next tick
			pEntry->bHasMoved		= true;				// Model has been moved
			pEntry->ModelTime		= 0.0f;				// Reset the timer
			pEntry->AnimStartTime	= pEntry->ModelTime;// Time animation was started

			geWorld_OpenModel(CCD->World(), pEntry->Model, GE_TRUE);
		}
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetToTargetTime
/* ------------------------------------------------------------------------------------ */
int CModelManager::SetToTargetTime(const geWorld_Model *theModel, geFloat TT)
{
	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
	{
		OutputDebugString("SetToTargetTime for unmanaged model?\n");
		return RGF_NOT_FOUND;					// Model not managed by us...
	}

	if(TT < 0.0f)
	{
		// Stop
		pEntry->bMoving = false;				// Model stops moving now
		pEntry->TargetTime = TT;
	}
	else
	{
		pEntry->TargetTime = TT * 1000.0f;

		if(pEntry->bHasMoved)
		{
			// Restart
			if(pEntry->TargetTime < pEntry->ModelTime)
				pEntry->bForward = false;
			else
				pEntry->bForward = true;

			pEntry->bMoving = true;						// Model will move on next tick
			pEntry->AnimStartTime = pEntry->ModelTime;
		}
		else
		{
			// Start
			pEntry->bForward		= true;
			pEntry->bMoving			= true;
			pEntry->bHasMoved		= true;
			pEntry->ModelTime		= 0.0f;
			pEntry->AnimStartTime	= pEntry->ModelTime;
			geWorld_OpenModel(CCD->World(), pEntry->Model, GE_TRUE);
		}

		pEntry->ModelTime = pEntry->TargetTime;
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// GetModelTargetTime
/* ------------------------------------------------------------------------------------ */
int CModelManager::GetTargetTime(const geWorld_Model *theModel, geFloat *TT)
{
	assert(TT);

	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
		return RGF_NOT_FOUND;					// Model not managed by us...

	if(!pEntry->bMoving)
		return RGF_FAILURE;

	*TT = pEntry->TargetTime*0.001f;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// GetModelCurrentTime
/* ------------------------------------------------------------------------------------ */
int CModelManager::GetModelCurrentTime(const geWorld_Model *theModel, geFloat *time)
{
	assert(time);

	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
		return RGF_NOT_FOUND;					// Model not managed by us...

	*time = pEntry->ModelTime*0.001f;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// ModelInCollision
//
// Return true if the model is currently colliding with something
/* ------------------------------------------------------------------------------------ */
int CModelManager::ModelInCollision(const geWorld_Model *theModel, bool *Colliding)
{
	assert(Colliding);

	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
		return RGF_NOT_FOUND;					// Model not managed by us...

	*Colliding = pEntry->bModelInCollision;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// Reset
//
// Reset the state of an animated model to its defaults
/* ------------------------------------------------------------------------------------ */
int CModelManager::Reset(const geWorld_Model *theModel)
{
	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
		return RGF_NOT_FOUND;					// Model not managed by us...

	// Ok, reset everything about this model to it's initial values
	pEntry->bForward			= true;
	pEntry->bMoving				= false;
	pEntry->ModelAnimationSpeed = 1.0f;
	pEntry->ModelTime			= 0.0f;
	pEntry->bLooping			= false;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// IsRunning
//
// Return true if the model is animating, otherwise false
/* ------------------------------------------------------------------------------------ */
bool CModelManager::IsRunning(const geWorld_Model *theModel)
{
	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
		return false;					// Model not managed by us...

	return pEntry->bMoving;
}

/* ------------------------------------------------------------------------------------ */
// HasMoved
//
// Return true if the model has moved, otherwise false
/* ------------------------------------------------------------------------------------ */
bool CModelManager::HasMoved(const geWorld_Model *theModel)
{
	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
		return false;				// Model not managed by us...

	return pEntry->bHasMoved;
}

/* ------------------------------------------------------------------------------------ */
// IsModel
//
// Is the model pointer passed in a managed world model?
/* ------------------------------------------------------------------------------------ */
bool CModelManager::IsModel(const geWorld_Model *theModel)
{
	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
		return false;		// Model not managed by us...

	return true;			// Sure is.
}

/* ------------------------------------------------------------------------------------ */
// SetLooping
//
// Set the automatic looping flag for a model
/* ------------------------------------------------------------------------------------ */
int CModelManager::SetLooping(const geWorld_Model *theModel, bool bLoopIt)
{
	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
		return RGF_NOT_FOUND;			// Model not managed by us...

	pEntry->bLooping = bLoopIt;			// Set loopy flag

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetOneShot
//
// Set the OneShot flag for a model
/* ------------------------------------------------------------------------------------ */
int CModelManager::SetOneShot(const geWorld_Model *theModel, bool bOS)
{
	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
		return RGF_NOT_FOUND;		// Model not managed by us...

	pEntry->bOneShot = bOS;			// Set OneShot flag

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetReverseOnCollide
//
// Sets the desired models 'reverse direction on collide' flag.  This
// ..is useful for doors when you don't want them trapping an actor.
/* ------------------------------------------------------------------------------------ */
int CModelManager::SetReverseOnCollide(const geWorld_Model *theModel, bool bReverseIt)
{
	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
		return RGF_NOT_FOUND;					// Model not managed by us...

	pEntry->bReverseOnCollision = bReverseIt;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetRideable
/* ------------------------------------------------------------------------------------ */
int CModelManager::SetRideable(const geWorld_Model *theModel, bool bRideable)
{
	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
		return RGF_NOT_FOUND;					// Model not managed by us...

	pEntry->bRideable = bRideable;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetReverse
/* ------------------------------------------------------------------------------------ */
int CModelManager::SetReverse(const geWorld_Model *theModel, bool bReverseIt)
{
	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
		return RGF_NOT_FOUND;					// Model not managed by us...

	pEntry->bReverse = bReverseIt;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetAllowInside
/* ------------------------------------------------------------------------------------ */
int CModelManager::SetAllowInside(const geWorld_Model *theModel, bool bAllowInside)
{
	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
		return RGF_NOT_FOUND;					// Model not managed by us...

	pEntry->bAllowInside = bAllowInside;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetRotating
/* ------------------------------------------------------------------------------------ */
int CModelManager::SetRotating(const geWorld_Model *theModel, bool bRotating)
{
	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
		return RGF_NOT_FOUND;					// Model not managed by us...

	pEntry->bRotating = bRotating;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetRunTimed
//
// Set the run timed flag for a model
/* ------------------------------------------------------------------------------------ */
int CModelManager::SetRunTimed(const geWorld_Model *theModel, bool bRT)
{
	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
		return RGF_NOT_FOUND;					// Model not managed by us...

	pEntry->bRunTimed = bRT;					// Set run timed flag

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetTimeEachTrig
//
// Set the time each trigger value for a model
/* ------------------------------------------------------------------------------------ */
int CModelManager::SetTimeEachTrig(const geWorld_Model *theModel, geFloat fTET)
{
	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
		return RGF_NOT_FOUND;					// Model not managed by us...

	pEntry->TimeEachTrig = fTET * 1000.0f;		// Set TimeEachTrig value

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetRunFromList
//
// Set the run from list flag for a model
/* ------------------------------------------------------------------------------------ */
int CModelManager::SetRunFromList(const geWorld_Model *theModel, bool bRFL)
{
	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
		return RGF_NOT_FOUND;					// Model not managed by us...

	pEntry->bRunFromList = bRFL;				// Set run from list flag

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetTimeList
//
// Set the time list for a model
//  Note: Only 20 list items are supported
/* ------------------------------------------------------------------------------------ */
int CModelManager::SetTimeList(const geWorld_Model *theModel, const geFloat *fTL)
{
	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
		return RGF_NOT_FOUND;					// Model not managed by us...

	memcpy(pEntry->TimeList, fTL, sizeof(pEntry->TimeList)); // Set TimeList values

	for(int i=0; i<(sizeof(pEntry->TimeList)/sizeof(pEntry->TimeList[0])); ++i)
	{
		pEntry->TimeList[i] *= 1000.0f;			// Scale values for model manager
	}

	pEntry->TLIndex = 0;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetAnimationSpeed
//
// Set the animation speed percentage for the specified world model.
/* ------------------------------------------------------------------------------------ */
int CModelManager::SetAnimationSpeed(const geWorld_Model *theModel, geFloat fSpeed)
{
	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
		return RGF_NOT_FOUND;					// Model not managed by us...

	pEntry->ModelAnimationSpeed = fSpeed;		// Adjust animation speed

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SaveTo
/* ------------------------------------------------------------------------------------ */
int CModelManager::SaveTo(FILE *SaveFD, bool type)
{
	for(int nEntry=0; nEntry<MODEL_LIST_SIZE; ++nEntry)
	{
		if(MainList[nEntry] != NULL)
		{
			WRITEDATA(type, &MainList[nEntry]->ModelTime,		sizeof(geFloat),	1, SaveFD);
			WRITEDATA(type, &MainList[nEntry]->OldModelTime,	sizeof(geFloat),	1, SaveFD);
			WRITEDATA(type, &MainList[nEntry]->Translation,		sizeof(geVec3d),	1, SaveFD);
			WRITEDATA(type, &MainList[nEntry]->Rotation,		sizeof(geVec3d),	1, SaveFD);
			WRITEDATA(type, &MainList[nEntry]->bForward,		sizeof(bool),		1, SaveFD);
			WRITEDATA(type, &MainList[nEntry]->bMoving,			sizeof(bool),		1, SaveFD);
			WRITEDATA(type, &MainList[nEntry]->bLooping,		sizeof(bool),		1, SaveFD);
			WRITEDATA(type, &MainList[nEntry]->bOneShot,		sizeof(bool),		1, SaveFD);
			WRITEDATA(type, &MainList[nEntry]->bHasMoved,		sizeof(bool),		1, SaveFD);
			WRITEDATA(type, &MainList[nEntry]->TLIndex,			sizeof(int),		1, SaveFD);
			WRITEDATA(type, &MainList[nEntry]->AnimStartTime,	sizeof(geFloat),	1, SaveFD);
			WRITEDATA(type, &MainList[nEntry]->ListWrapAround,	sizeof(bool),		1, SaveFD);
		}
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// RestoreFrom
/* ------------------------------------------------------------------------------------ */
int CModelManager::RestoreFrom(FILE *RestoreFD, bool type)
{
	for(int nEntry=0; nEntry<MODEL_LIST_SIZE; ++nEntry)
	{
		if(MainList[nEntry] != NULL)
		{
			READDATA(type, &MainList[nEntry]->ModelTime,		sizeof(geFloat),	1, RestoreFD);
			READDATA(type, &MainList[nEntry]->OldModelTime,		sizeof(geFloat),	1, RestoreFD);
			READDATA(type, &MainList[nEntry]->Translation,		sizeof(geVec3d),	1, RestoreFD);
			READDATA(type, &MainList[nEntry]->Rotation,			sizeof(geVec3d),	1, RestoreFD);
			READDATA(type, &MainList[nEntry]->bForward,			sizeof(bool),		1, RestoreFD);
			READDATA(type, &MainList[nEntry]->bMoving,			sizeof(bool),		1, RestoreFD);
			READDATA(type, &MainList[nEntry]->bLooping,			sizeof(bool),		1, RestoreFD);
			READDATA(type, &MainList[nEntry]->bOneShot,			sizeof(bool),		1, RestoreFD);
			READDATA(type, &MainList[nEntry]->bHasMoved,		sizeof(bool),		1, RestoreFD);
			READDATA(type, &MainList[nEntry]->TLIndex,			sizeof(int),		1, RestoreFD);
			READDATA(type, &MainList[nEntry]->AnimStartTime,	sizeof(geFloat),	1, RestoreFD);
			READDATA(type, &MainList[nEntry]->ListWrapAround,	sizeof(bool),		1, RestoreFD);
		}
	}

	return RGF_SUCCESS;
}


// Data required to undo actor motion caused by models
int UndoIndex;
geActor *UndoActor[512];
geVec3d UndoActorPos[512];
geVec3d UndoActorRot[512];

/* ------------------------------------------------------------------------------------ */
// Tick
//
// Dispatch time to all animating models, let them move, and make any
// ..adjustments to actors etc. in the vicinity if necessary.
/* ------------------------------------------------------------------------------------ */
void CModelManager::Tick(geFloat dwTicks)
{
	memset(&ModelState[0], NOT_PROCESSED, MODEL_LIST_SIZE * sizeof(int));

	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "ModelStateModifier");

	if(pSet)
	{
		geEntity *pEntity;
		// Once more we scan the entity list.  Does this get old, or what?
		for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
			pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
		{
			// Get the entity data so we can compare models
			ModelStateModifier *pMod = (ModelStateModifier*)geEntity_GetUserData(pEntity);

			if(pMod->DoForce)
			{
				pMod->FTime += (dwTicks*0.001f);

				if(pMod->FTime >= pMod->ForceTimeDelay)
					pMod->DoForce = false;
			}

			if(pMod->DoDamage)
			{
				pMod->DTime += (dwTicks*0.001f);

				if(pMod->DTime >= pMod->DamageTimeDelay)
					pMod->DoDamage = false;
			}
		}
	}

	// Call ProcessModelTick and ProcessChildModelTick.
	// ProcessChildModelTick recursively follows the model hierarchy to move the linked models together.
	for(int nEntry=0; nEntry<ManagedModels; nEntry++)
	{
		if((MainList[nEntry] != NULL) && !MainList[nEntry]->ParentModel)
		{
			UndoIndex = -1;

			if(ProcessModelTick(nEntry, dwTicks) == RGF_SUCCESS)
			{
				if(MainList[nEntry]->ChildModel)
				{
					if(ProcessChildModelTick(ModelIndex(MainList[nEntry]->ChildModel), dwTicks) == RGF_FAILURE)
					{
						if(ModelState[nEntry] == MOVED_IT)
						{
							// Model moved, since its child can't move then neither can the parent. Undo parent motion.
							MainList[nEntry]->ModelTime = MainList[nEntry]->OldModelTime;
							geWorld_SetModelXForm(CCD->World(), MainList[nEntry]->Model, &MainList[nEntry]->OldModelXForm);
							ModelState[nEntry] = NO_MOVEMENT;
						}

						// This model family group can't move, undo any actor motion induced
						for(int i=0; i<=UndoIndex; ++i)
						{
							UndoActorPos[i].X = -UndoActorPos[i].X;
							UndoActorPos[i].Y = -UndoActorPos[i].Y;
							UndoActorPos[i].Z = -UndoActorPos[i].Z;

							UndoActorRot[i].X = -UndoActorRot[i].X;
							UndoActorRot[i].Y = -UndoActorRot[i].Y;
							UndoActorRot[i].Z = -UndoActorRot[i].Z;

							CCD->ActorManager()->AddTranslation(UndoActor[i], UndoActorPos[i]);
							CCD->ActorManager()->AddRotation(UndoActor[i], UndoActorRot[i]);
						}
					}
				}
			}
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// ProcessChildModelTick
/* ------------------------------------------------------------------------------------ */
int CModelManager::ProcessChildModelTick(int nEntry, geFloat dwTicks)
{
	if(ProcessModelTick(nEntry, dwTicks) == RGF_FAILURE)
	{
		MainList[nEntry]->ModelTime = MainList[nEntry]->OldModelTime;
		ModelState[nEntry] = NO_MOVEMENT;
		return RGF_FAILURE;
	}
	else
	{
		if(MainList[nEntry]->ChildModel)
		{
			if(ProcessChildModelTick(ModelIndex(MainList[nEntry]->ChildModel), dwTicks) == RGF_FAILURE)
			{
				MainList[nEntry]->ModelTime = MainList[nEntry]->OldModelTime;
				geWorld_SetModelXForm(CCD->World(), MainList[nEntry]->Model, &MainList[nEntry]->OldModelXForm);
				ModelState[nEntry] = NO_MOVEMENT;
				return RGF_FAILURE;
			}
		}

		if(MainList[nEntry]->SiblingModel)
		{
			if(ProcessChildModelTick(ModelIndex(MainList[nEntry]->SiblingModel), dwTicks) == RGF_FAILURE)
			{
				MainList[nEntry]->ModelTime = MainList[nEntry]->OldModelTime;
				geWorld_SetModelXForm(CCD->World(), MainList[nEntry]->Model, &MainList[nEntry]->OldModelXForm);
				ModelState[nEntry] = NO_MOVEMENT;
				return RGF_FAILURE;
			}
		}
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// ProcessModelTick
/* ------------------------------------------------------------------------------------ */
int CModelManager::ProcessModelTick(int nEntry, geFloat dwTicks)
{
	// Return Value:  RGF_SUCCESS = model moved everything ok
	//                RGF_FAILURE = model can't move because of collision

	if(MainList[nEntry] != NULL)
	{
		geMotion *pMotion;
		gePath *pPath;
		geFloat tStart, tEnd, theTime;
		geFloat TempTimeDelta, NextTime, TargetTime;
		const char *Eventstring;

		// Is this model moving?
		if(MainList[nEntry]->bMoving)
		{
			ModelState[nEntry] = MOVED_IT;

			if (CCD->MenuManager()->GetSEBoundBox())
			{
				geExtBox Result;
				geVec3d Verts[8];
				geXForm3d Xf;
				int	i;
				geFloat dx, dy, dz;

				for(i=0; i<8; ++i)
					Verts[i] = MainList[nEntry]->Mins;

				dx = MainList[nEntry]->Maxs.X - MainList[nEntry]->Mins.X;
				dy = MainList[nEntry]->Maxs.Y - MainList[nEntry]->Mins.Y;
				dz = MainList[nEntry]->Maxs.Z - MainList[nEntry]->Mins.Z;

				Verts[0].Y += dy;
				Verts[3].Y += dy;
				Verts[3].X += dx;
				Verts[7].X += dx;

				Verts[1].Y += dy;
				Verts[1].Z += dz;
				Verts[5].Z += dz;
				Verts[6].Z += dz;
				Verts[6].X += dx;

				Verts[2].X += dx;
				Verts[2].Y += dy;
				Verts[2].Z += dz;

				geWorld_GetModelXForm(CCD->World(), MainList[nEntry]->Model, &Xf);
				geXForm3d_TransformArray(&Xf, Verts, Verts, 8);
				Result.Min = Result.Max = Verts[0];

				for(i=1; i<8; ++i)
				{
					if(Verts[i].X < Result.Min.X)
						Result.Min.X = Verts[i].X;
					else if(Verts[i].X > Result.Max.X)
						Result.Max.X = Verts[i].X;

					if(Verts[i].Y < Result.Min.Y)
						Result.Min.Y = Verts[i].Y;
					else if(Verts[i].Y > Result.Max.Y)
						Result.Max.Y = Verts[i].Y;

					if(Verts[i].Z < Result.Min.Z)
						Result.Min.Z = Verts[i].Z;
					else if(Verts[i].Z > Result.Max.Z)
						Result.Max.Z = Verts[i].Z;
				}

				DrawBoundBox(CCD->World(), &MainList[nEntry]->Translation, &Result.Min, &Result.Max);
			}

			// Make sure we have animation data for the model
			pMotion= geWorld_ModelGetMotion(MainList[nEntry]->Model);

			if(!pMotion)					// No motion data?
			{
				MainList[nEntry]->bMoving = false;
				MainList[nEntry]->bHasMoved = false;
				return RGF_SUCCESS;
			}

			pPath = geMotion_GetPath(pMotion, 0);

			if(!pPath)						// No path data?
			{
				MainList[nEntry]->bMoving = false;
				MainList[nEntry]->bHasMoved = false;
				return RGF_SUCCESS;
			}

			// Compute animation time and find our position
			theTime = MainList[nEntry]->ModelTime *0.001f; // / 1000.0f;
			geMotion_GetTimeExtents(pMotion, &tStart, &tEnd);

			// Save off the previous animation time in case we need to back up
			MainList[nEntry]->OldModelTime = MainList[nEntry]->ModelTime;

			geWorld_GetModelXForm(CCD->World(), MainList[nEntry]->Model, &MainList[nEntry]->OldModelXForm);

			// Now, check the direction
			if(MainList[nEntry]->bForward)
			{
				// Model is animating FORWARDS along timeline
				MainList[nEntry]->ModelTime += (dwTicks * MainList[nEntry]->ModelAnimationSpeed);
				if(MainList[nEntry]->TargetTime >= 0.0f)
				{
					if(MainList[nEntry]->ModelTime > MainList[nEntry]->TargetTime)
					{
						MainList[nEntry]->ModelTime = MainList[nEntry]->TargetTime;
						MainList[nEntry]->bMoving = false;		// Kill the motion
					}

					TargetTime = MainList[nEntry]->ModelTime;
				}
				else
				{
					if(MainList[nEntry]->bRunTimed)
					{
						TempTimeDelta = MainList[nEntry]->ModelTime - MainList[nEntry]->AnimStartTime;

						if(TempTimeDelta >= MainList[nEntry]->TimeEachTrig)
						{
							MainList[nEntry]->ModelTime = MainList[nEntry]->AnimStartTime +
															MainList[nEntry]->TimeEachTrig;
							MainList[nEntry]->bMoving = false;		// Kill the motion
						}

						TargetTime = MainList[nEntry]->ModelTime;
					}

					if(MainList[nEntry]->bRunFromList)
					{
						if(MainList[nEntry]->ListWrapAround && (MainList[nEntry]->ModelTime >= MainList[nEntry]->TimeList[MainList[nEntry]->TLIndex]))
						{
							if(MainList[nEntry]->ModelTime >= (tEnd * 1000.0f))
							{
								MainList[nEntry]->ListWrapAround = false;
								MainList[nEntry]->ModelTime = (MainList[nEntry]->ModelTime - (tEnd * 1000.0f)) + (tStart * 1000.0f);
							}
						}

						if(!MainList[nEntry]->ListWrapAround && (MainList[nEntry]->ModelTime >= MainList[nEntry]->TimeList[MainList[nEntry]->TLIndex]))
						{
							MainList[nEntry]->ModelTime = MainList[nEntry]->TimeList[MainList[nEntry]->TLIndex];
							TargetTime = MainList[nEntry]->ModelTime;

							if(++MainList[nEntry]->TLIndex >= TIME_LIST_MAX)
								NextTime = -1000.f;
							else
								NextTime = MainList[nEntry]->TimeList[MainList[nEntry]->TLIndex];

							if(NextTime == -1000.f)
							{
								if(MainList[nEntry]->bLooping)
								{
									MainList[nEntry]->TLIndex -= 2;

									if(MainList[nEntry]->TLIndex < 0)
										MainList[nEntry]->TLIndex = 0;

									if(MainList[nEntry]->bReverse)
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
									// Do nothing, leave model where it is
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
						if((MainList[nEntry]->ModelTime * 0.001f) >= tEnd)
						{
							TargetTime = tEnd * 1000.0f;

							if(MainList[nEntry]->bReverse)
							{
								MainList[nEntry]->bForward = false;	// Reverse direction
								MainList[nEntry]->ModelTime = tEnd * 1000.0f;
								MainList[nEntry]->AnimStartTime = MainList[nEntry]->ModelTime + TempTimeDelta;
							}
							else if(MainList[nEntry]->bLooping) // else
							{
								MainList[nEntry]->ModelTime = tStart * 1000.0f;
								MainList[nEntry]->AnimStartTime = MainList[nEntry]->ModelTime - TempTimeDelta;
							}
							else // not looping and not reverse
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
			}
			else
			{
				// Model is animating BACKWARDS along timeline
				MainList[nEntry]->ModelTime -= (dwTicks * MainList[nEntry]->ModelAnimationSpeed);
				if(MainList[nEntry]->TargetTime >= 0.0f)
				{
					if(MainList[nEntry]->ModelTime < MainList[nEntry]->TargetTime)
					{
						MainList[nEntry]->ModelTime = MainList[nEntry]->TargetTime;
						MainList[nEntry]->bMoving = false;		// Kill the motion
					}

					TargetTime = MainList[nEntry]->ModelTime;
				}
				else
				{
					if(MainList[nEntry]->bRunTimed)
					{
						TempTimeDelta = MainList[nEntry]->AnimStartTime - MainList[nEntry]->ModelTime;

						if(TempTimeDelta >= MainList[nEntry]->TimeEachTrig)
						{
							MainList[nEntry]->ModelTime = MainList[nEntry]->AnimStartTime -
															MainList[nEntry]->TimeEachTrig;
							MainList[nEntry]->bMoving = false;		// Kill the motion
						}

						TargetTime = MainList[nEntry]->ModelTime;
					}

					if(MainList[nEntry]->bRunFromList)
					{
						if(MainList[nEntry]->ListWrapAround && (MainList[nEntry]->ModelTime <= MainList[nEntry]->TimeList[MainList[nEntry]->TLIndex]))
						{
							if(MainList[nEntry]->ModelTime <= (tStart * 1000.0f))
							{
								MainList[nEntry]->ListWrapAround = false;
								MainList[nEntry]->ModelTime = (tEnd * 1000.0f) - ((tStart * 1000.0f) - MainList[nEntry]->ModelTime);
							}
						}

						if(!MainList[nEntry]->ListWrapAround && (MainList[nEntry]->ModelTime <= MainList[nEntry]->TimeList[MainList[nEntry]->TLIndex]))
						{
							MainList[nEntry]->ModelTime = MainList[nEntry]->TimeList[MainList[nEntry]->TLIndex];
							TargetTime = MainList[nEntry]->ModelTime;

							if(--MainList[nEntry]->TLIndex < 0)
								NextTime = -1000.f;
							else
								NextTime = MainList[nEntry]->TimeList[MainList[nEntry]->TLIndex];

							if(NextTime == -1000.f)
							{
								if(MainList[nEntry]->bLooping)
								{
									MainList[nEntry]->TLIndex += 2;

									if(MainList[nEntry]->bReverse)
									{
										MainList[nEntry]->bForward = true;	// Reverse direction
									}
									else
									{
										for(int i=TIME_LIST_MAX-1; i>=0; i--)
										{
											if(MainList[nEntry]->TimeList[i] != -1)
											{
												MainList[nEntry]->TLIndex = i;
												MainList[nEntry]->ListWrapAround = TRUE;
												break;
											}

											if(i == 0)
											{
												MainList[nEntry]->bMoving = false;		// Kill the motion
											}
										}

										if(MainList[nEntry]->TLIndex < 0)
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
						if((MainList[nEntry]->ModelTime * 0.001f) <= tStart)
						{
							TargetTime = tStart * 1000.0f;

							if(MainList[nEntry]->bLooping)
							{
								if(MainList[nEntry]->bReverse)
								{
									MainList[nEntry]->bForward = true;	// Reverse direction
									MainList[nEntry]->ModelTime = tStart * 1000.0f;
									MainList[nEntry]->AnimStartTime = MainList[nEntry]->ModelTime - TempTimeDelta;
								}
								else
								{
									MainList[nEntry]->ModelTime = tEnd * 1000.0f;
									MainList[nEntry]->AnimStartTime = MainList[nEntry]->ModelTime + TempTimeDelta;
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
			}

			// Time updated, let's adjust the models position...
			if(MoveModel(MainList[nEntry], pPath) == GE_TRUE)
			{
				// Process Events in the motion
				if(MainList[nEntry]->bForward)
				{
					geMotion_SetupEventIterator(pMotion, (MainList[nEntry]->OldModelTime+1.0f)*0.001f, (TargetTime+1.0f)*0.001f);
				}
				else
				{
					geMotion_SetupEventIterator(pMotion, TargetTime*0.001f, (MainList[nEntry]->OldModelTime)*0.001f);
				}

				while(geMotion_GetNextEvent(pMotion, &tStart, &Eventstring))
				{
					char Arg[6][128];

					memset(Arg, 0, sizeof(Arg));
					sscanf(Eventstring, "%s %s %s %s %s %s", &Arg[0], &Arg[1], &Arg[2], &Arg[3], &Arg[4], &Arg[5]);

					switch(Arg[0][0])
					{
					case 'S':   // Play sound, (.wav file, non-looping)
						{
							if(!EffectC_IsStringNull(Arg[1]))
							{
								geSound_Def *theSound;

								theSound = SPool_Sound(Arg[1]);

								if(theSound)
								{
									// We have some sound, play the sucker.
									geFloat Volume, Pan, Frequency;
									geXForm3d xfmPlayer = CCD->Player()->ViewPoint();
									geSound3D_GetConfig(CCD->World(), &xfmPlayer, &MainList[nEntry]->Translation,
														CCD->GetAudibleRadius(), 2.0f, &Volume, &Pan, &Frequency);
									geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), theSound,
															Volume, Pan, Frequency, GE_FALSE);
								}
							}

							break;
						}
					case 'T':	// Set trigger
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

				ModelState[nEntry] = MOVED_IT;
				// Model moved, everything ok
				MainList[nEntry]->bModelInCollision = false;
				return RGF_SUCCESS;
			}
			else
			{
				// Model could not move due to collision
				MainList[nEntry]->bModelInCollision = true;
				return RGF_FAILURE;
			}
		}
		else
		{
			// This model is not moving on its own, but we must check if its parent moved
			if(MainList[nEntry]->ParentModel && (ModelState[ModelIndex(MainList[nEntry]->ParentModel)] == MOVED_IT))
			{
				// Parent moved, so the child must too!  Whether it likes it or not.
				if(CCD->MenuManager()->GetSEBoundBox())
				{
					geExtBox  Result;
					geVec3d   Verts[8];
					geXForm3d Xf;
					int		 i;
					geFloat	 dx, dy, dz;

					for(i=0; i<8; ++i)
						Verts[i] = MainList[nEntry]->Mins;

					dx = MainList[nEntry]->Maxs.X - MainList[nEntry]->Mins.X;
					dy = MainList[nEntry]->Maxs.Y - MainList[nEntry]->Mins.Y;
					dz = MainList[nEntry]->Maxs.Z - MainList[nEntry]->Mins.Z;

					Verts[0].Y += dy;
					Verts[3].Y += dy;
					Verts[3].X += dx;
					Verts[7].X += dx;

					Verts[1].Y += dy;
					Verts[1].Z += dz;
					Verts[5].Z += dz;
					Verts[6].Z += dz;
					Verts[6].X += dx;

					Verts[2].X += dx;
					Verts[2].Y += dy;
					Verts[2].Z += dz;

					geWorld_GetModelXForm(CCD->World(), MainList[nEntry]->Model, &Xf);
					geXForm3d_TransformArray(&Xf, Verts, Verts, 8);
					Result.Min = Result.Max = Verts[0];

					for(i=1; i<8; ++i)
					{
						if(Verts[i].X < Result.Min.X)
							Result.Min.X = Verts[i].X;
						else if(Verts[i].X > Result.Max.X)
							Result.Max.X = Verts[i].X;

						if(Verts[i].Y < Result.Min.Y)
							Result.Min.Y = Verts[i].Y;
						else if(Verts[i].Y > Result.Max.Y)
							Result.Max.Y = Verts[i].Y;

						if(Verts[i].Z < Result.Min.Z)
							Result.Min.Z = Verts[i].Z;
						else if(Verts[i].Z > Result.Max.Z)
							Result.Max.Z = Verts[i].Z;
					}

					DrawBoundBox(CCD->World(), &MainList[nEntry]->Translation, &Result.Min, &Result.Max);
				}

				// Make sure we have animation data for the model
				pMotion= geWorld_ModelGetMotion(MainList[nEntry]->Model);

				if(pMotion)
					pPath= geMotion_GetPath(pMotion, 0);
				else
					pPath = NULL;

				if(MoveModel(MainList[nEntry], pPath) == GE_TRUE)
				{
					ModelState[nEntry] = MOVED_IT;
					// Model moved, everything ok
					return RGF_SUCCESS;
				}
				else
				{
					// Model could not move due to collision
					//return RGF_FAILURE;

					MainList[nEntry]->ModelTime = MainList[nEntry]->OldModelTime;
					MainList[ModelIndex(MainList[nEntry]->ParentModel)]->ModelTime = MainList[ModelIndex(MainList[nEntry]->ParentModel)]->OldModelTime;
					geWorld_SetModelXForm(CCD->World(), MainList[nEntry]->ParentModel, &MainList[ModelIndex(MainList[nEntry]->ParentModel)]->OldModelXForm);
					ModelState[ModelIndex(MainList[nEntry]->ParentModel)] = NO_MOVEMENT;
					ModelState[nEntry] = NO_MOVEMENT;

					return RGF_FAILURE;
				}
			}
			else
			{
				ModelState[nEntry] = NO_MOVEMENT;
				// Model didn't have to move at all
				return RGF_SUCCESS;
			}
		}
	}

	return RGF_SUCCESS;
}


/* ------------------------------------------------------------------------------------ */
// GetPosition
//
// Get the current position of the desired world model.
/* ------------------------------------------------------------------------------------ */
int CModelManager::GetPosition(const geWorld_Model *theModel, geVec3d *thePosition)
{
	assert(thePosition);

	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
		return RGF_NOT_FOUND;					// Model not managed by us...

	*thePosition = pEntry->Translation;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetPosition
/* ------------------------------------------------------------------------------------ */
int CModelManager::SetPosition(const geWorld_Model *theModel, const geVec3d &thePosition)
{
	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
		return RGF_NOT_FOUND;					// Model not managed by us...

	pEntry->Translation = thePosition;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// GetRotation
//
// Get the current rotation of the desired world model.
/* ------------------------------------------------------------------------------------ */
int CModelManager::GetRotation(const geWorld_Model *theModel, geVec3d *theRotation)
{
	assert(theRotation);

	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
		return RGF_NOT_FOUND;					// Model not managed by us...

	*theRotation = pEntry->Rotation;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// GetModelTime
/* ------------------------------------------------------------------------------------ */
int CModelManager::GetModelTime(const geWorld_Model *theModel, geFloat *time)
{
	assert(time);

	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
		return RGF_NOT_FOUND;					// Model not managed by us...

	if(!pEntry->bMoving)
		return RGF_FAILURE;

	*time = pEntry->ModelTime;

	return RGF_SUCCESS;
}

// EVIL HACK ALERT
//
// Due to problems in getting proper bounding box information out
// ..of the G3D API, the following two set-and-get bounding box
// ..calls have been added.  The manually-set bounding box will be
// ..used elsewhere to perform collision detection.

/* ------------------------------------------------------------------------------------ */
// SetBoundingBox
//
// Define the OBB for a specific managed model.
/* ------------------------------------------------------------------------------------ */
int CModelManager::SetBoundingBox(const geWorld_Model *theModel, const geExtBox &theBBox)
{
	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
		return RGF_NOT_FOUND;					// Model not managed by us...

	pEntry->ModelBBox = theBBox;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// GetBoundingBox
//
// Retrieve the previously set OBB for a specific managed model.
/* ------------------------------------------------------------------------------------ */
int CModelManager::GetBoundingBox(const geWorld_Model *theModel, geExtBox *theBBox)
{
	assert(theBBox);

	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
		return RGF_NOT_FOUND;					// Model not managed by us...

	*theBBox = pEntry->ModelBBox;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// AddPassenger
//
// Set a specified model to have a specified actor as a passenger on it.
// ..This lasts one animation cycle, it is assumed actor gravity
// ..processing, etc. will re-add the passenger as long as required.
/* ------------------------------------------------------------------------------------ */
int CModelManager::AddPassenger(const geWorld_Model *theModel, geActor *theActor)
{
	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
		return RGF_NOT_FOUND;					// Model not managed by us...

	for(int nX=0; nX<MAX_PASSENGERS; ++nX)
	{
		if(pEntry->Passengers[nX] == NULL)
		{
			pEntry->Passengers[nX] = theActor;
			return RGF_SUCCESS;
		}
	}

	return RGF_FAILURE;							// Full bus?
}

/* ------------------------------------------------------------------------------------ */
// RemovePassenger
//
// Remove an actor using a model as a vehicle.
/* ------------------------------------------------------------------------------------ */
int CModelManager::RemovePassenger(const geWorld_Model *theModel, const geActor *theActor)
{
	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
		return RGF_NOT_FOUND;					// Model not managed by us...

	for(int nX=0; nX<MAX_PASSENGERS; ++nX)
	{
		if(pEntry->Passengers[nX] == theActor)
		{
			pEntry->Passengers[nX] = NULL;
			return RGF_SUCCESS;
		}
	}

	return RGF_FAILURE;							// Full bus?
}

/* ------------------------------------------------------------------------------------ */
// IsAPassenger
//
// Checks to see if a specific actor is a passenger on a model.  Returns
// ..GE_TRUE if so, otherwise GE_FALSE.
/* ------------------------------------------------------------------------------------ */
geBoolean CModelManager::IsAPassenger(const geWorld_Model *theModel, const geActor *theActor)
{
	ModelInstanceList *pEntry = FindModel(theModel);

	if(pEntry == NULL)
		return GE_FALSE;					// Model not managed by us...

	for(int nX=0; nX<MAX_PASSENGERS; ++nX)
	{
		if(pEntry->Passengers[nX] == theActor)
			return GE_TRUE;
	}

	return GE_FALSE;							// Not there
}

/* ------------------------------------------------------------------------------------ */
// EmptyContent
/* ------------------------------------------------------------------------------------ */
int CModelManager::EmptyContent(const geWorld_Model *Model)
{
	ModelInstanceList *pEntry = FindModel(Model);

	if(pEntry == NULL)
		return RGF_NOT_FOUND;				// Model not managed by us...

	GE_Contents Contents;

	memset(&Contents, 0, sizeof(GE_Contents));

	int Result1 = geWorld_GetContents(CCD->World(), &pEntry->Translation, &pEntry->Mins, &pEntry->Maxs,
										GE_COLLIDE_MODELS, 0xffffffff, NULL, NULL, &Contents);

	if(Result1 == GE_TRUE)
	{
		if((Contents.Contents & (GE_CONTENTS_EMPTY)) == GE_CONTENTS_EMPTY)
			return RGF_SUCCESS;
	}

	return RGF_FAILURE;
}

/* ------------------------------------------------------------------------------------ */
// DoesBoxHitModel
//
// EVIL HACK!  This should NOT be necessary but the freaking G3D API
// ..won't return a proper bounding box for an animated world model,
// ..so we have to use manually-set ones.
//
// This function takes an ExtBox and checks to see if it intersects
// ..the ExtBox of any managed model, and if so, returns a pointer
// ..to that model - otherwise NULL.  GE_TRUE is returned if there
// ..was a collision, BTW.  Note that the EXTBOX coming in is assumed
// ..to be in MODEL SPACE, we translate it into WORLD SPACE assuming
// ..that the 'thePoint' is the origin of the ExtBox.
/* ------------------------------------------------------------------------------------ */
geBoolean CModelManager::DoesBoxHitModel(const geVec3d &/*thePoint*/, geExtBox theBox,
                                         geWorld_Model **theModel)
{
	geExtBox Result, Temp;
	geVec3d Verts[8];
	geXForm3d Xf;
	int	i;
	geFloat	dx, dy, dz;

	*theModel = NULL;

	for(int nTemp=0; nTemp<ManagedModels; nTemp++)
	{
		if(MainList[nTemp] == NULL)
			continue;				// Empty slot

		if(MainList[nTemp]->bAllowInside)
			continue;

		for(i=0; i<8; ++i)
			Verts[i] = MainList[nTemp]->Mins;

		dx = MainList[nTemp]->Maxs.X - MainList[nTemp]->Mins.X;
		dy = MainList[nTemp]->Maxs.Y - MainList[nTemp]->Mins.Y;
		dz = MainList[nTemp]->Maxs.Z - MainList[nTemp]->Mins.Z;

		Verts[0].Y += dy;
		Verts[3].Y += dy;
		Verts[3].X += dx;
		Verts[7].X += dx;

		Verts[1].Y += dy;
		Verts[1].Z += dz;
		Verts[5].Z += dz;
		Verts[6].Z += dz;
		Verts[6].X += dx;

		Verts[2].X += dx;
		Verts[2].Y += dy;
		Verts[2].Z += dz;

		geWorld_GetModelXForm(CCD->World(), MainList[nTemp]->Model, &Xf);
		geXForm3d_TransformArray(&Xf, Verts, Verts, 8);
		Result.Min = Result.Max = Verts[0];

		for(i=1; i<8; ++i)
		{
			if(Verts[i].X < Result.Min.X)
				Result.Min.X = Verts[i].X;
			else if(Verts[i].X > Result.Max.X)
				Result.Max.X = Verts[i].X;

			if(Verts[i].Y < Result.Min.Y)
				Result.Min.Y = Verts[i].Y;
			else if(Verts[i].Y > Result.Max.Y)
				Result.Max.Y = Verts[i].Y;

			if(Verts[i].Z < Result.Min.Z)
				Result.Min.Z = Verts[i].Z;
			else if(Verts[i].Z > Result.Max.Z)
				Result.Max.Z = Verts[i].Z;
		}

		//if(CCD->MenuManager()->GetSEBoundBox())
		//DrawBoundBox(CCD->World(), &MainList[nTemp]->Translation, &Result.Min, &Result.Max);

		if(CCD->Doors()->IsADoor(MainList[nTemp]->Model)
			&& IsRunning(MainList[nTemp]->Model)
			&& MainList[nTemp]->bRotating)
			continue;

		if(MainList[nTemp]->ModelType == ENTITY_LIQUID || MainList[nTemp]->ModelType == ENTITY_OVERLAY)
			continue;

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

	// No hit, all be hunky-dory.

	return GE_FALSE;						// Hey, no collisions!
}

/* ------------------------------------------------------------------------------------ */
// ContentModel
/* ------------------------------------------------------------------------------------ */
geBoolean CModelManager::ContentModel(const geVec3d &/*thePoint*/, geExtBox theBox,
									  geWorld_Model **theModel)
{
	geExtBox Result, Temp;

	*theModel = NULL;

	for(int nTemp=0; nTemp<ManagedModels; nTemp++)
	{
		if(MainList[nTemp] == NULL)
			continue;				// Empty slot

		geVec3d Forward, Up, Left, Pos;
		geXForm3d Xf;
		geWorld_GetModelXForm(CCD->World(), MainList[nTemp]->Model, &Xf);

		geXForm3d_GetUp(&Xf, &Up);
		geVec3d_Normalize(&Up);

		geXForm3d_GetIn(&Xf, &Forward);
		geVec3d_Normalize(&Forward);

		geXForm3d_GetLeft(&Xf, &Left);
		geVec3d_Normalize(&Left);

		geWorld_GetModelRotationalCenter(CCD->World(), MainList[nTemp]->Model, &Pos);

		geVec3d_AddScaled(&Pos, &Forward, -MainList[nTemp]->Mins.Z, &Result.Min);
		geVec3d_AddScaled(&Result.Min, &Left, -MainList[nTemp]->Mins.X, &Result.Min);
		geVec3d_AddScaled(&Result.Min, &Up, MainList[nTemp]->Mins.Y, &Result.Min);

		geVec3d_AddScaled(&Pos, &Forward, -MainList[nTemp]->Maxs.Z, &Result.Max);
		geVec3d_AddScaled(&Result.Max, &Left, -MainList[nTemp]->Maxs.X, &Result.Max);
		geVec3d_AddScaled(&Result.Max, &Up, MainList[nTemp]->Maxs.Y, &Result.Max);

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

	// No hit, all be hunky-dory.

	return GE_FALSE;						// Hey, no collisions!
}

/* ------------------------------------------------------------------------------------ */
// HandleCollision
//
// Check to see if this model needs to do anything when collided with.
/* ------------------------------------------------------------------------------------ */
int CModelManager::HandleCollision(const geWorld_Model *theModel, const geActor *theActor)
{
	geEntity *pEntity;
	geVec3d Direction;

	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "ModelStateModifier");

	if(!pSet)
		return RGF_FAILURE;

	// Once more we scan the door list.  Does this get old, or what?
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		// Get the door data so we can compare models
		ModelStateModifier *pMod = static_cast<ModelStateModifier*>(geEntity_GetUserData(pEntity));

		if(pMod->theModel != theModel)
			continue;								// Not this one

		// Ok, the passed-in model has a MODIFIER.  Let's see what it's supposed to do.

		if(pMod->ForceImparter == GE_TRUE && !pMod->DoForce)
		{
			// Need to add force to the actor that hit us...
			if(CCD->ActorManager()->ForceActive(theActor, 2) == false)
			{
				Direction.Z = GE_PIOVER180*(pMod->ForceVector.Z);
				Direction.X = GE_PIOVER180*(pMod->ForceVector.X);
				Direction.Y = GE_PIOVER180*(pMod->ForceVector.Y);

				geVec3d_Normalize(&Direction);
				CCD->ActorManager()->SetForce(theActor, 2, Direction, pMod->ForceAmount, pMod->ForceDecay);

				pMod->DoForce = true;
				pMod->FTime = 0.0f;

				if(pMod->theFSound != NULL)
				{
					// Check to see if the sound is already playing,
					// ..and if so, DON'T REPLAY IT!!!
					if(geSound_SoundIsPlaying(CCD->Engine()->AudioSystem(), pMod->FSoundHandle) != GE_TRUE)
					{
						// We have some sound, play the sucker.
						geFloat Volume, Pan, Frequency;
						geXForm3d xfmPlayer = CCD->Player()->ViewPoint();

						geSound3D_GetConfig(CCD->World(), &xfmPlayer, &pMod->origin,
                           CCD->GetAudibleRadius(), 2.0f, &Volume, &Pan, &Frequency);

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
					CCD->Damage()->DamageActor(theActor, pMod->DamageAmount, "health", pMod->DamageAltAmount, pMod->DamageAltAttribute, "Model");
				}
				else
				{
					CCD->Damage()->DamageActor(theActor, pMod->DamageAmount, pMod->DamageAttribute, pMod->DamageAltAmount, pMod->DamageAltAttribute, "Model");
				}
			}

			pMod->DoDamage = true;
			pMod->DTime = 0.0f;

			if(pMod->theDSound != NULL)
			{
				// Check to see if the sound is already playing,
				// ..and if so, DON'T REPLAY IT!!!
				if(geSound_SoundIsPlaying(CCD->Engine()->AudioSystem(), pMod->DSoundHandle) != GE_TRUE)
				{
					// We have some sound, play the sucker.
					geFloat Volume, Pan, Frequency;
					geXForm3d xfmPlayer = CCD->Player()->ViewPoint();

					geSound3D_GetConfig(CCD->World(), &xfmPlayer, &pMod->origin,
                        CCD->GetAudibleRadius(), 2.0f, &Volume, &Pan, &Frequency);

					pMod->DSoundHandle = geSound_PlaySoundDef(CCD->Engine()->AudioSystem(),
						pMod->theDSound, Volume, Pan, Frequency, GE_FALSE);
				}
				// End of sound checking
			}
		}

		if(EmptyContent(pMod->theModel) == RGF_SUCCESS)
			return RGF_EMPTY;

		return RGF_SUCCESS;							// We handled it.
	}

	return RGF_FAILURE;								// Nope, didn't process collision
}

/* ------------------------------------------------------------------------------------ */
// GetModifier
//
// Get any modifiers for the passed-in model.  Note that the caller is
// ..expected to delete the returned struct.
/* ------------------------------------------------------------------------------------ */
ModelStateModifier *CModelManager::GetModifier(const geWorld_Model *theModel)
{
	geEntity *pEntity;

	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "ModelStateModifier");

	if(!pSet)
		return false;

	// Once more we scan the door list.  Does this get old, or what?
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		// Get the door data so we can compare models
		ModelStateModifier *pMod = static_cast<ModelStateModifier*>(geEntity_GetUserData(pEntity));

		if(pMod->theModel != theModel)
			continue;							// Not this one

		ModelStateModifier *ModPerm = new ModelStateModifier;
		memcpy(ModPerm, pMod, sizeof(ModelStateModifier));
		return ModPerm;							// Users problem to delete struct now...
	}

	return NULL;								// No modifiers for the desired model
}

/* ------------------------------------------------------------------------------------ */
// GetAttributes
//
// Check to see if a specified model has any attached attributes, and if
// ..so, return a pointer to the attributes structure.  NOTE THAT IT IS
// ..THE CALLERS RESPONSIBILITY TO DELETE THE RETURNED POINTER!
/* ------------------------------------------------------------------------------------ */
ModelAttributes *CModelManager::GetAttributes(const geWorld_Model *theModel)
{
	geEntity *pEntity;

	geEntity_EntitySet *pSet = geWorld_GetEntitySet(CCD->World(), "ModelAttributes");

	if(!pSet)
		return NULL;

	// Once more we scan the door list.  Does this get old, or what?
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		// Get the door data so we can compare models
		ModelAttributes *pMod = static_cast<ModelAttributes*>(geEntity_GetUserData(pEntity));

		if(pMod->theModel != theModel)
			continue;							// Not this one

		ModelAttributes *ModPerm = new ModelAttributes;
		memcpy(ModPerm, pMod, sizeof(ModelAttributes));
		return ModPerm;							// Users problem to delete struct now...
	}

	return NULL;								// No attributes for the desired model
}

// ******** PRIVATE MEMBER FUNCTIONS **********

/* ------------------------------------------------------------------------------------ */
// AddNewModelToList
//
// Add a model to be managed to the in-memory database
/* ------------------------------------------------------------------------------------ */
int CModelManager::AddNewModelToList(geWorld_Model *theModel, int nModelType)
{
	int i;

	for(int nTemp=0; nTemp<MODEL_LIST_SIZE; ++nTemp)
	{
		// Let's find a free entry
		if(MainList[nTemp] == NULL)
		{
			MainList[nTemp] = new ModelInstanceList;
			memset(MainList[nTemp], 0, sizeof(ModelInstanceList));

			MainList[nTemp]->Model					= theModel;
			MainList[nTemp]->bForward				= true;
			MainList[nTemp]->bMoving				= false;
			MainList[nTemp]->ModelAnimationSpeed	= 1.0f;		// 100%
			MainList[nTemp]->ModelTime				= 0.0f;
			MainList[nTemp]->TargetTime				= -1.0f;
			MainList[nTemp]->ModelType				= nModelType;
			MainList[nTemp]->bLooping				= false;
			MainList[nTemp]->bRotating				= false;
			MainList[nTemp]->bReverse				= false; //true; // changed QD 12/15/05
			MainList[nTemp]->bAllowInside			= false;
			MainList[nTemp]->bRunTimed				= false;
			MainList[nTemp]->TimeEachTrig			= 1.0f;
			MainList[nTemp]->bRunFromList			= false;

			for(i=0; i<TIME_LIST_MAX; ++i)
			{
				MainList[nTemp]->TimeList[i] = 0.0f;
			}

			MainList[nTemp]->TLIndex				= 0;
			MainList[nTemp]->AnimStartTime			= 0.0f;
			MainList[nTemp]->ListWrapAround			= false;

			MainList[nTemp]->bReverseOnCollision	= false;
			MainList[nTemp]->bRideable				= true;

			geWorld_ModelGetBBox(CCD->World(), MainList[nTemp]->Model, &MainList[nTemp]->Mins, &MainList[nTemp]->Maxs);

			MainList[nTemp]->Mins.X += 127.0f;
			MainList[nTemp]->Mins.Y += 127.0f;
			MainList[nTemp]->Mins.Z += 127.0f;
			MainList[nTemp]->Maxs.X -= 127.0f;
			MainList[nTemp]->Maxs.Y -= 127.0f;
			MainList[nTemp]->Maxs.Z -= 127.0f;

			geWorld_GetModelRotationalCenter(CCD->World(), theModel, &MainList[nTemp]->Translation);
			geWorld_GetModelXForm(CCD->World(), theModel, &MainList[nTemp]->Xf);
			geWorld_ModelSetFlags(theModel, GE_MODEL_RENDER_NORMAL | GE_MODEL_RENDER_MIRRORS | GE_MODEL_COLLIDE);

			for(int iPassenger=0; iPassenger<MAX_PASSENGERS; ++iPassenger)
				MainList[nTemp]->Passengers[iPassenger] = NULL;

			if(nTemp >= ManagedModels)
				ManagedModels = nTemp + 1;

			return RGF_SUCCESS;
		}
	}

	return RGF_NO_FREE_SPACE;					// No room for more world models
}

/* ------------------------------------------------------------------------------------ */
// RemoveModelFromList
//
// Remove a model from the in-memory database
/* ------------------------------------------------------------------------------------ */
int CModelManager::RemoveModelFromList(const geWorld_Model *theModel)
{
	for(int nTemp=0; nTemp<ManagedModels; nTemp++)
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

/* ------------------------------------------------------------------------------------ */
// FindModel
//
// Locate a models entry in the database, return a pointer to it.
/* ------------------------------------------------------------------------------------ */
ModelInstanceList *CModelManager::FindModel(const geWorld_Model *theModel)
{
	if(theModel)
	{
		for(int nTemp=0; nTemp<ManagedModels; nTemp++)
		{
			// Let's find the right
			if((MainList[nTemp] != NULL) && (MainList[nTemp]->Model == theModel))
				return MainList[nTemp];		// Entry to caller
		}
	}

	return NULL;							// Heh, not found, sucker!
}

/* ------------------------------------------------------------------------------------ */
// ModelIndex
//
// Return the index of the model in the 'm_MainList'.
/* ------------------------------------------------------------------------------------ */
int CModelManager::ModelIndex(const geWorld_Model *theModel)
{
	for(int nTemp=0; nTemp<ManagedModels; nTemp++)
	{
		// Let's find the right
		if((MainList[nTemp] != NULL) && (MainList[nTemp]->Model == theModel))
			return nTemp;					// Index to caller
	}

	return NULL;							// Heh, not found, sucker!
}

/* ------------------------------------------------------------------------------------ */
// MoveModel
//
// Given a model instance, move the dratted thing.  Note that this will
// ..perform motion checking to make sure that all actors within the
// ..models bounding box will remain unaffected by its motion...
/* ------------------------------------------------------------------------------------ */
int CModelManager::MoveModel(ModelInstanceList *theEntry, const gePath *pPath)
{
	geActor *ActorsInRange[512];
	geVec3d ActorsInRangeDeltaTranslation[512];
	geVec3d ActorsInRangeDeltaRotation[512];
	geVec3d Center, OldRotation;
	geFloat ModelRange = 500.0f;			// Reasonable default *HACK*
	geXForm3d xfmDest, xfmOldPosition;
	geFloat theTime;
	geXForm3d ParentXfm, ResultXfm;
	int i;

	// Get the old transform for this model. (Note: this includes any motion due to a parent)
	geWorld_GetModelXForm(CCD->World(), theEntry->Model, &xfmOldPosition);

	// Compute the new animation time
	theTime = theEntry->ModelTime * 0.001f;

	// Get the new transform from the path (Note: this does not include any parent movement)
	gePath_Sample(pPath, theTime, &xfmDest);

	// Include the translation and rotation of the parent model, if any
	if(theEntry->ParentModel)
	{
		// Get the parent's transform
		geWorld_GetModelXForm(CCD->World(), theEntry->ParentModel, &ParentXfm);

		// Move model with its parent model
		geVec3d RotatedOriginOffset, OriginOffsetOffset;
		geXForm3d_Rotate(&ParentXfm, &(theEntry->OriginOffset), &RotatedOriginOffset);
		geVec3d_Subtract(&RotatedOriginOffset, &(theEntry->OriginOffset), &OriginOffsetOffset);
		geXForm3d_Multiply(&ParentXfm, &xfmDest, &ResultXfm);
		geVec3d_Add(&OriginOffsetOffset, &(ResultXfm.Translation), &(ResultXfm.Translation));
	}
	else
	{
		// If no parent then just copy model's new transform into ResultXfm for later use
		geXForm3d_Copy(&xfmDest, &ResultXfm);
	}

	// Find the current center of rotation
	geWorld_GetModelRotationalCenter(CCD->World(), theEntry->Model, &Center);

	// At this point, we need to check to see if we have any actors within a
	// ..reasonable range of our platform, and if so, we'll need to check to see
	// ..if moving the platform will end up forcing any of the actors in this
	// ..platforms range to move.

	geVec3d tmp;
	geVec3d_Add(&Center, &(ResultXfm.Translation), &tmp);
	int nActorCount = CCD->ActorManager()->GetActorsInRange(tmp, ModelRange, 512, &ActorsInRange[0]);

	if(!(theEntry->bRideable) && !(theEntry->bReverseOnCollision))
		nActorCount = 0;

	if(nActorCount == 0)
	{
		// No actors to test collisions with, LET'S ROCK ON!
		geWorld_SetModelXForm(CCD->World(), theEntry->Model, &ResultXfm);
		theEntry->Translation = Center;						// Update position
		GetEulerAngles(&ResultXfm, &theEntry->Rotation);	// Update rotation
		return RGF_SUCCESS;
	}

	// Ugh, we have to check EACH and EVERY ACTOR that was returned for a possible
	// ..collision. Is this a pain, or what?

	geVec3d ActorPosition, NewActorPosition, DeltaRotation, Delta1;
	bool	InitDataForPassengers = true;
	int		nTemp;

	for(nTemp=0; nTemp<nActorCount; ++nTemp)
	{
		ActorsInRangeDeltaTranslation[nTemp].X = 0.0f;
		ActorsInRangeDeltaTranslation[nTemp].Y = 0.0f;
		ActorsInRangeDeltaTranslation[nTemp].Z = 0.0f;

		ActorsInRangeDeltaRotation[nTemp].X = 0.0f;
		ActorsInRangeDeltaRotation[nTemp].Y = 0.0f;
		ActorsInRangeDeltaRotation[nTemp].Z = 0.0f;

		// Note that, if at ANY point we can't adjust an actors position to allow
		// ..the model to move we abort moving the model AT ALL and set the current
		// ..time back to the previous time, effectively "freezing time" for the
		// ..model.
		CCD->ActorManager()->GetPosition(ActorsInRange[nTemp], &ActorPosition);
		bool fModelMoveOK = CCD->Collision()->CheckModelMotionActor(theEntry->Model, &ResultXfm,
									ActorsInRange[nTemp], &ActorPosition, &NewActorPosition);

		if(fModelMoveOK == false)
		{
			HandleCollision(theEntry->Model, ActorsInRange[nTemp]);

			// Ok, since we did collide, if this model is flagged as reverse
			// on collision, we need to flip this models motion direction
			if(theEntry->bReverseOnCollision == true)
				theEntry->bForward = !theEntry->bForward;		// Switch direction

			return RGF_FAILURE;					// Couldn't move the model
		}

		if(IsAPassenger(theEntry->Model, ActorsInRange[nTemp]) && theEntry->bRideable)
		{
			geXForm3d DeltaXfm;
			geVec3d Offset, NewOffset;

			if(InitDataForPassengers)
			{
				Delta1 = ResultXfm.Translation;
				Delta1.X = Delta1.X - xfmOldPosition.Translation.X;
				Delta1.Y = Delta1.Y - xfmOldPosition.Translation.Y;
				Delta1.Z = Delta1.Z - xfmOldPosition.Translation.Z;

				GetEulerAngles(&xfmOldPosition, &OldRotation);
				GetEulerAngles(&ResultXfm, &theEntry->Rotation);
				geVec3d_Subtract(&theEntry->Rotation, &OldRotation, &DeltaRotation);
				InitDataForPassengers = false;
			}

			CCD->ActorManager()->GetPosition(ActorsInRange[nTemp], &Offset);

			geVec3d_Subtract(&Offset, &Center, &Offset);

			geXForm3d_SetZRotation(&DeltaXfm, DeltaRotation.Z);
			geXForm3d_RotateX(&DeltaXfm, DeltaRotation.X);
			geXForm3d_RotateY(&DeltaXfm, DeltaRotation.Y);

			geXForm3d_Rotate(&DeltaXfm, &Offset, &NewOffset);
			geVec3d_Subtract(&NewOffset, &Offset, &Offset);
			geVec3d_Add(&Offset, &Delta1, &ActorsInRangeDeltaTranslation[nTemp]);
			ActorsInRangeDeltaRotation[nTemp] = DeltaRotation;
		}
		else
		{
			geVec3d_Subtract(&NewActorPosition, &ActorPosition, &ActorsInRangeDeltaTranslation[nTemp]);
		}

		// Ok, if we DID collide (ActorPosition != NewActorPosition) and this
		// ..model is flagged as reverse on collision, we need to flip this
		// ..models motion direction
		if((memcmp(&ActorPosition, &NewActorPosition, sizeof(geVec3d)) != 0) &&
			(theEntry->bReverseOnCollision == true))
			theEntry->bForward = !theEntry->bForward;		// Switch direction

		if(memcmp(&ActorPosition, &NewActorPosition, sizeof(geVec3d)) != 0)
			i = 0;
	}

	// If we made it this far the model CAN move so let's move the affected actors
	for(nTemp=0; nTemp<nActorCount; ++nTemp)
	{
		CCD->ActorManager()->AddTranslation(ActorsInRange[nTemp], ActorsInRangeDeltaTranslation[nTemp]);
		CCD->ActorManager()->AddRotation(ActorsInRange[nTemp], ActorsInRangeDeltaRotation[nTemp]);
		++UndoIndex;

		if(UndoIndex < 512)
		{
			UndoActor[UndoIndex]    = ActorsInRange[nTemp];
			UndoActorPos[UndoIndex] = ActorsInRangeDeltaTranslation[nTemp];
			UndoActorRot[UndoIndex] = ActorsInRangeDeltaRotation[nTemp];
		}
		else
		{
			UndoIndex--;
		}
	}

	// Ok, let's move the model
	geWorld_SetModelXForm(CCD->World(), theEntry->Model, &ResultXfm);
	theEntry->Translation = Center;						// Update position
	GetEulerAngles(&ResultXfm, &theEntry->Rotation);	// Update rotation

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// This function is a rewrite of the Genesis function called geXForm3d_GetEulerAngles.
// The Genesis version assumes the matrix was created in the rotation order of z,y,x. Why?
// We need this function to assume the order of z,x,y.  In addition, the Genesis
// version doesn't evaluate the Y angle beyond doing an inverse sin.  The inverse sin
// only gives a range of +-90 degrees.  We need +-180 degrees.  In our version the
// X angle has this problem, but we recalculate X using the atan2 function once we
// know the value of the Y angle.
/* ------------------------------------------------------------------------------------ */
void CModelManager::GetEulerAngles(const geXForm3d *M, geVec3d *Angles)
// order of angles z,x,y
//         Z rotation                  X rotation                 Y rotation
//  /  cos(Z) -sin(Z) 0 0 \     / 1     0      0    0 \     /  cos(Y) 0  sin(Y) 0 \
//  |  sin(Z)  cos(Z) 0 0 |  *  | 0  cos(X) -sin(X) 0 |  *  |     0   1     0   0 |  =
//  |     0       0   1 0 |     | 0  sin(X)  cos(X) 0 |     | -sin(Y) 0  cos(Y) 0 |
//  \     0       0   0 1 /     \ 0     0      0    1 /     \     0   0     0   1 /
//
//  / cos(Y)*cos(Z)-sin(X)*sin(Y)*sin(Z)  -cos(X)*sin(Z)  sin(X)*cos(Y)*sin(Z)+sin(Y)*cos(Z)  0 \
//  | sin(X)*sin(Y)*cos(Z)+cos(Y)*sin(Z)   cos(X)*cos(Z)  sin(Y)*sin(Z)-sin(X)*cos(Y)*cos(Z)  0 |
//  |          -cos(X)*sin(Y)                  sin(X)               cos(X)*cos(Y)             0 |
//  \                 0                           0                       0                   1 /
{
	geFloat BZ;
	assert(M      != NULL);
	assert(Angles != NULL);

	assert(geXForm3d_IsOrthonormal(M) == GE_TRUE);

	//ack.  due to floating point error, the value can drift away from 1.0 a bit
	//   this will clamp it.  The _IsOrthonormal test will pass because it allows
	//   for a tolerance.
	BZ = M->BZ;

	if(BZ > 1.0f)
		BZ = 1.0f;

	if(BZ < -1.0f)
		BZ = -1.0f;

	Angles->X = -asin(BZ);

	if(cos(Angles->X) != 0)
	{
		Angles->Y = -atan2(-M->AZ, M->CZ);
		Angles->Z = -atan2(-M->BX, M->BY);
	}
	else
	{
		Angles->Y = -atan2(M->BX, -M->BY / sin(Angles->X));
		Angles->Z = 0.0f;
	}

	if(cos(Angles->Y) != 0.0f)
	{
		Angles->X = -atan2(BZ, M->CZ / cos(Angles->Y));
	}
	else
	{
		Angles->Y = -atan2(BZ, -M->AZ / sin(Angles->Y));
	}

	assert(geVec3d_IsValid(Angles) != GE_FALSE);
}


/* ----------------------------------- END OF FILE ------------------------------------ */
