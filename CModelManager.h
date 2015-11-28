/************************************************************************************//**
 * @file CModelManager.h
 * @brief Animated World Model Manager
 *
 * This file contains the class declaration of the Model Manager system for
 * Reality Factory.
 ****************************************************************************************/

#ifndef __CMODEL_MANAGER__
#define __CMODEL_MANAGER__

#define	MODEL_LIST_SIZE	512	// changed QD 12/15/05 Max # of models per level
#define TIME_LIST_MAX	20  // MOD010122 - Added for new RunFromList option - max length of list
#define MAX_PASSENGERS	64
/**
 * @brief This struct holds information for each instance of a model
 */
struct ModelInstanceList
{
	geWorld_Model	*Model;						///< The model itself
	int				ModelType;					///< The model type (door, platform, etc.)
	geFloat			ModelTime;					///< Animation time for model
	geFloat			OldModelTime;				///< Previous time count for model
	// Start Aug2003DCS
    geXForm3d		OldModelXForm;				///< Last transform - in case we have to back up
	// End Aug2003DCS
	geFloat			ModelAnimationSpeed;		///< Speed of models motion
	geVec3d			Translation;				///< Model translation
	geVec3d			Rotation;					///< Model rotation
	geVec3d			Mins;
	geVec3d			Maxs;
	geXForm3d		Xf;
	// Added by Pickles to RF071A
	bool			bRideable;
	bool			bForward;					///< Forward/backward flag
	bool			bMoving;					///< Is model moving?
	bool			bLooping;					///< Animation automatically loops?
	// See the history comment in the header area.
	bool			bOneShot;					///< OneShot or multiple times flag
	bool			bHasMoved;					///< Has model already moved?
	bool			bRunTimed;					///< Animation runs specified time each trigger
	geFloat			TimeEachTrig;				///< Time to animate each trigger if RunTimed is true
	bool			bRunFromList;				///< Animation runs to next time in list each trigger
	geFloat			TimeList[TIME_LIST_MAX];	///< List of animation time targets
	int				TLIndex;					///< Current index into TimeList
	geFloat			AnimStartTime;				///< Last animation start time
	bool			ListWrapAround;				///< Indicates when the list index is wrapping from max to 0 or 0 to max
	// Start Aug2003DCS
    geFloat			TargetTime;					///< External controller animation time command
    geWorld_Model	*ParentModel;				///< This model is attached to another model
    geWorld_Model	*ChildModel;				///< Model attached to this model
    geWorld_Model	*SiblingModel;				///< Model that has same parent as this model
    geVec3d			OriginOffset;				///< Offset to attached model
    geVec3d			origin;						///< Origin of this model
    bool			bModelInCollision;			///< Indicates if the model is currently colliding with anything
	// End Aug2003DCS
	bool			bReverse;
	bool			bAllowInside;
	bool			bRotating;
	int				SoundEffectHandle;			///< Sound effect to play while moving
	bool			bLoopAudioWhileMoving;		///< TRUE loops the audio effect while moving
	geExtBox		ModelBBox;					///< Model bounding box
	bool			bReverseOnCollision;		///< TRUE if model should reverse on collision
	geActor			*Passengers[MAX_PASSENGERS];///< Up to 64 passengers on this model
};

/**
 * @brief Animated World Model Manager
 *
 * The Model Manager handles starting, stopping, and moving all of the various
 * types of Animated World Models in the level.
 */
class CModelManager : public CRGFComponent
{
public:
	CModelManager();					///< Default constructor
	~CModelManager();					///< Default destructor

	int AddModel(geWorld_Model *theModel, int nModelType);			///< Add model to database
	int RemoveModel(const geWorld_Model *theModel);					///< Remove model from database
	int Start(const geWorld_Model *theModel);						///< Start model moving
	int Stop(const geWorld_Model *theModel);						///< Stop model moving
	int Reset(const geWorld_Model *theModel);						///< Reset to initial conditions
	bool IsRunning(const geWorld_Model *theModel);					///< Is model animating?
	bool IsModel(const geWorld_Model *theModel);					///< Is model a world model?
	int SetLooping(const geWorld_Model *theModel, bool bLoopIt);	///< Set model loop flag
	// MOD010122 - Added next 8 functions for new KINDERWARE,
	//            INC. options.  See the history comment in
	//            the header area.
	int ReStart(const geWorld_Model *theModel);						///< Restart model moving
	int SetOneShot(const geWorld_Model *theModel, bool bOneShot);	///< Set model OneShot flag
	bool HasMoved(const geWorld_Model *theModel);					///< Has model already moved?
	int SetRunTimed(const geWorld_Model *theModel, bool bRT);
	int SetTimeEachTrig(const geWorld_Model *theModel, geFloat fTET);
	int SetRunFromList(const geWorld_Model *theModel, bool bRFL);
	int SetTimeList(const geWorld_Model *theModel, const geFloat *fTL);

	// Start Aug2003DCS
	int SetParentModel(geWorld_Model *theModel, geWorld_Model *ParentModel);
	int SetModelOriginOffset(const geWorld_Model *theModel, const geVec3d *OriginOffset);
	int SetModelOrigin(const geWorld_Model *theModel, const geVec3d *Origin);
	int SetTargetTime(const geWorld_Model *theModel, geFloat TT);	///< Script controller animation time command
	// changed Nout 12/15/05
    int SetToTargetTime(const geWorld_Model *theModel, geFloat TT);
	// end change
	int GetTargetTime(const geWorld_Model *theModel, geFloat *TT);
	int GetModelCurrentTime(const geWorld_Model *theModel, geFloat *time);
	int ModelInCollision(const geWorld_Model *theModel, bool *Colliding);
	// End Aug2003DCS

	// Start Added by pickles to RF071A
	int SetRideable(const geWorld_Model *theModel, bool bRideable);
	// End Added by pickles to rf071a

	int SetReverseOnCollide(const geWorld_Model *theModel, bool bReverseIt);
	int SetReverse(const geWorld_Model *theModel, bool bReverseIt);
	int SetAllowInside(const geWorld_Model *theModel, bool bAllowInside);
	int SetRotating(const geWorld_Model *theModel, bool bRotating);
	int SetAnimationSpeed(const geWorld_Model *theModel, geFloat fSpeed);	///< Set animation speed
	void Tick(geFloat dwTicks);												///< Process passage of time
	int GetPosition(const geWorld_Model *theModel, geVec3d *thePosition);
	// changed RF064
	int SetPosition(const geWorld_Model *theModel, const geVec3d &thePosition);
	geBoolean ContentModel(const geVec3d &thePoint, geExtBox theBox, geWorld_Model **theModel);
	// end change RF064
	int GetRotation(const geWorld_Model *theModel, geVec3d *theRotation);
	int GetModelTime(const geWorld_Model *theModel, geFloat *time);

	int SetBoundingBox(const geWorld_Model *theModel, const geExtBox &theBBox);
	int GetBoundingBox(const geWorld_Model *theModel, geExtBox *theBBox);

	int AddPassenger(const geWorld_Model *theModel, geActor *theActor);
	int RemovePassenger(const geWorld_Model *theModel, const geActor *theActor);
	geBoolean IsAPassenger(const geWorld_Model *theModel, const geActor *theActor);

	int EmptyContent(const geWorld_Model *Model);
	geBoolean DoesBoxHitModel(const geVec3d &thePoint, geExtBox theBox, geWorld_Model **theModel);
	int HandleCollision(const geWorld_Model *theModel, const geActor *theActor);	///< Check for collision processing

	ModelStateModifier *GetModifier(const geWorld_Model *theModel);
	ModelAttributes *GetAttributes(const geWorld_Model *theModel);

	// start multiplayer
	int SaveTo(FILE *SaveFD, bool type);
	int RestoreFrom(FILE *RestoreFD, bool type);
	// end multiplayer

private:
	//	Private member functions
	int AddNewModelToList(geWorld_Model *theModel, int nModelType);
	int RemoveModelFromList(const geWorld_Model *theModel);
	ModelInstanceList *FindModel(const geWorld_Model *theModel);
	int MoveModel(ModelInstanceList *theEntry, const gePath *pPath);
	// Start Nov2003DCS
	void GetEulerAngles(const geXForm3d *M, geVec3d *Angles);
	// End Nov2003DCS
	// Start Aug2003DCS
	int ModelIndex(const geWorld_Model *theModel);
	int ProcessModelTick(int nEntry, geFloat dwTicks);
	int ProcessChildModelTick(int nEntry, geFloat dwTicks);
	// End Aug2003DCS
private:
	//	Private member variables
	ModelInstanceList *MainList[MODEL_LIST_SIZE];	///< Database of managed models
	//Start Aug2003DCS
    int               ManagedModels;	///< Always set to the number of managed models. We can use this to only loop
										///< thru the models that are defined, not all 512 every time (saves time).
	//End Aug2003DCS
};

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
