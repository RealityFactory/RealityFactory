/*
CModelManager.h:		Animated World Model Manager

  This file contains the class declaration of the Model Manager
  system for the Reality Factory.
  
	The Model Manager handles starting, stopping, and moving all of
	the various types of Animated World Models in the level.
*/

#ifndef __CMODEL_MANAGER__
#define __CMODEL_MANAGER__

#define TIME_LIST_MAX 20  //MOD010122 - Added for new RunFromList option - max length of list

//	This struct holds information for each instance of a model

struct ModelInstanceList
{
	geWorld_Model *Model;					// The model itself
	int ModelType;								// The model type (door, platform, etc.)
	geFloat ModelTime;						// Animation time for model
	geFloat OldModelTime;					// Previous time count for model
	geFloat ModelAnimationSpeed;	// Speed of models motion
	geVec3d Translation;					// Model translation
	geVec3d Rotation;							// Model rotation
	geVec3d Mins;
	geVec3d Maxs;
	bool bForward;								// Forward/backward flag
	bool bMoving;									// Is model moving?
	bool bLooping;								// Animation automatically loops?
	//            See the history comment in the header area.
	bool bOneShot;                      // OneShot or multiple times flag
	bool bHasMoved;                     // Has model already moved?
	bool bRunTimed;                     // Animation runs specified time each trigger
	geFloat TimeEachTrig;               // Time to animate each trigger if RunTimed is true
	bool bRunFromList;                  // Animation runs to next time in list each trigger
	geFloat TimeList[TIME_LIST_MAX];    // List of animation time targets
	int  TLIndex;                       // Current index into TimeList
	geFloat AnimStartTime;              // Last animation start time
	bool ListWrapAround;                // Indicates when the list index is wrapping from max to 0 or 0 to max
	
	bool bReverse;
	bool bAllowInside;
	bool bRotating;
	int SoundEffectHandle;				// Sound effect to play while moving
	bool bLoopAudioWhileMoving;		// TRUE loops the audio effect while moving
	geExtBox ModelBBox;						// Model bounding box
	bool bReverseOnCollision;			// TRUE if model should reverse on collision
	geActor *Passengers[64];			// Up to 64 passengers on this model
};

class CModelManager : public CRGFComponent
{
public:
	CModelManager();					// Default constructor
	~CModelManager();					// Default destructor
	int AddModel(geWorld_Model *theModel, int nModelType);		// Add model to database
	int RemoveModel(geWorld_Model *theModel);	// Remove model from database
	int Start(geWorld_Model *theModel);				// Start model moving
	int Stop(geWorld_Model *theModel);				// Stop model moving
	int Reset(geWorld_Model *theModel);				// Reset to initial conditions
	bool IsRunning(geWorld_Model *theModel);	// Is model animating?
	bool IsModel(geWorld_Model *theModel);		// Is model a world model?
	int SetLooping(geWorld_Model *theModel, bool bLoopIt);	// Set model loop flag
	//MOD010122 - Added next 8 functions for new KINDERWARE,
	//            INC. options.  See the history comment in 
	//            the header area.
	int ReStart(geWorld_Model *theModel);           // Restart model moving
	int SetOneShot(geWorld_Model *theModel, bool bOneShot);	// Set model OneShot flag
	bool HasMoved(geWorld_Model *theModel);     // Has model already moved?
	int SetRunTimed(geWorld_Model *theModel, bool bRT);
	int SetTimeEachTrig(geWorld_Model *theModel, geFloat fTET);
	int SetRunFromList(geWorld_Model *theModel, bool bRFL);
	int SetTimeList(geWorld_Model *theModel, geFloat *fTL);
	
	int SetReverseOnCollide(geWorld_Model *theModel, bool bReverseIt);
	int SetReverse(geWorld_Model *theModel, bool bReverseIt);
	int SetAllowInside(geWorld_Model *theModel, bool bAllowInside);
	int SetRotating(geWorld_Model *theModel, bool bRotating);
	int SetAnimationSpeed(geWorld_Model *theModel, geFloat fSpeed);	// Set anim. speed
	void Tick(geFloat dwTicks);			// Process passage of time
	int GetPosition(geWorld_Model *theModel, geVec3d *thePosition);
	int GetRotation(geWorld_Model *theModel, geVec3d *theRotation);
	int GetModelTime(geWorld_Model *theModel, geFloat *time);
	int SetBoundingBox(geWorld_Model *theModel, geExtBox theBBox);
	int GetBoundingBox(geWorld_Model *theModel, geExtBox *theBBox);
	int AddPassenger(geWorld_Model *theModel, geActor *theActor);
	int RemovePassenger(geWorld_Model *theModel, geActor *theActor);
	geBoolean IsAPassenger(geWorld_Model *theModel, geActor *theActor);
	int EmptyContent(geWorld_Model *Model);
	geBoolean DoesBoxHitModel(geVec3d thePoint, geExtBox theBox, geWorld_Model **theModel);
	int HandleCollision(geWorld_Model *theModel, geActor *theActor);		// Check for collision processing
	ModelStateModifier *GetModifier(geWorld_Model *theModel);
	ModelAttributes *GetAttributes(geWorld_Model *theModel);
private:
	//	Private member functions
	int AddNewModelToList(geWorld_Model *theModel, int nModelType);
	int RemoveModelFromList(geWorld_Model *theModel);
	ModelInstanceList *FindModel(geWorld_Model *theModel);
	int MoveModel(ModelInstanceList *theEntry, gePath *pPath);
	//	Private member variables
	ModelInstanceList *MainList[512];	// Database of managed models
};

#endif
