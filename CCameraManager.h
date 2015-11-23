/*
CCameraManager.h:		Camera positioning, rotation, and motion manager

  This file contains the class declaration for the Camera Manager
  class.
  
	The Camera Manager is responsible for handling the motion of the
	player viewpoint camera during the game.
*/

#ifndef __CCAMERAMANAGER_H_
#define __CCAMERAMANAGER_H_

#define DEFAULTFOV 2.0f

class CCameraManager: public CRGFComponent
{
public:
	CCameraManager();
	~CCameraManager();
	void Defaults();
// changed RF063
	int SaveTo(FILE *SaveFD);
	int RestoreFrom(FILE *RestoreFD);
	void SaveToS();
	void RestoreFromS();
	void SetJerk(float MaxAmount, geFloat Decay);
// end change RF063
	void Tick(float dwTicks);
	void SetZoom(bool flag);
	void SetShake(float MaxAmount, geFloat Decay, geVec3d Pos);
	void GetShake(float *x, float *y);
	int Position(geVec3d thePosition);	// Set camera position
	int Rotate(geVec3d theRotation);		// Set camera rotation
	int TurnLeft(geFloat fAmount);			// Turn camera LEFT
	int TurnRight(geFloat fAmount);			// Turn camera RIGHT
	int TiltUp(geFloat fAmount);				// Tilt camera UP
	int TiltDown(geFloat fAmount);			// Tilt camera DOWN
	int TiltXUp(geFloat fAmount);				// Tilt camera UP
	int TiltXDown(geFloat fAmount);			// Tilt camera DOWN
	float GetTiltPercent();
	float GetTilt();
	int Center();												// Center camera view
	int GetPosition(geVec3d *thePosition);	// Get camera position
	int GetRotation(geVec3d *theRotation);		// Get camera rotation
	int BindToActor(geActor *theActor);			// Bind camera to actor
	int SetCameraOffset(geVec3d theTranslation, geVec3d theRotation);	// Set camera XYZ/Rot offset
	int GetCameraOffset(geVec3d *theTranslation, geVec3d *theRotation);
	int BindToActorBone(geActor *theActor, char *szBoneName);	// Bind to actor bone
	int BindToWorldModel(geWorld_Model *theModel);		// Bind camera to world model
	int SetBoundingBox(geExtBox theBox);		// Set camera collision box
	int ClearBoundingBox();									// Remove bounding box
	int Unbind();												// Remove camera bindings
	int TrackMotion();									// Track motion, if required
	int RenderView();										// Render the view from the camera
	bool InCollision();									// Check for camera in collision
	int SetFarClipPlane(geFloat fDistance);	// Set camera clip plane distance
	void EnableFarClipPlane(geBoolean ClipOn);	// Enable/disable clip plane
	inline void SetTrackingFlags(int nFlags)			// Set camera tracking flags
				{ TrackingFlags = nFlags;}
	inline int GetTrackingFlags()			// Get camera tracking flags
				{ return TrackingFlags;}
	inline geCamera *Camera() { return EngineCamera;}
	void CameraRotX(bool direction);
	void CameraRotY(bool direction, float Speed);
	void ResetCamera();
	void ChangeDistance(bool direction, float Speed);
	geXForm3d ViewPoint();
	// eaa3 12/18/2000 Head bob functions
	void SetHeadBobOffset(geFloat fOffset)
	  { m_HeadBobOffset = fOffset;}
	geFloat GetHeadBobOffset() { return m_HeadBobOffset;}
	bool EnableHeadBob(bool fEnable)
	  { bool xtmp = m_HeadBob; m_HeadBob = fEnable; return xtmp;}
	// eaa3 12/18/2000 end
	void Get3rdData(float *distance, float *camerax, float *cameray);
	void Set3rdData(float distance, float camerax, float cameray);
	void SetFOV(geFloat Fov)
	{ FOV = Fov; }
	geFloat GetFOV()
	{ return FOV; }
	void SetShakeMin(float Min)
	{ shakemin = Min; }

	float GetViewHeight()
	{ return viewheight; }
	float GetPlayerHeight()
	{ return playerheight; }
	float GetPlayerAngleUp()
	{ return playerangleup; }
	float GetPlayerDistance()
	{ return playerdistance; }
	float GetPlayerMinDistance()
	{ return playermindistance; }
	float GetPlayerMaxDistance()
	{ return playermaxdistance; }
	bool GetPlayerZoom()
	{ return playerzoom; }
	bool GetPlayerAllowLook()
	{ return playerallowlook; }
	bool GetPlayerMouseRotation()
	{ return playermouserotation; }
	float GetIsoHeight()
	{ return isoheight; }
	float GetIsoAngleUp()
	{ return isoangleup; }
	float GetIsoAngleAround()
	{ return isoanglearound; }
	float GetIsoDistance()
	{ return isodistance; }
	bool GetIsoFlag()
	{ return IsoCollFlag; }
	bool GetSwitchAllowed()
	{ return switchallowed; }
	bool GetSwitch1st()
	{ return switch1st; }
	bool GetSwitch3rd()
	{ return switch3rd; }
	bool GetSwitchIso()
	{ return switchiso; }
// changed RF063
	bool GetZooming()
	{ return zoommode; }
	float GetJerk()
	{ return jerkamt; }
// end change RF063
// changed RF064
	void SetDesiredHeight(float height)
	{ DesiredHeight = height; }
	float GetOverlayDist()
	{ return OverlayDist; }
	bool	GetViewChanged()
	{ return m_bViewChanged;	}
	bool	GetPositionMoved()
	{ return m_bPositionMoved;	} 
	geVec3d* GetMoveDif()
	{ return &m_vMoveDif;		}
	float	GetDistanceMoved()
	{ return geVec3d_Length( &m_vMoveDif ); }
// end change RF064
private:
	//	Member functions
	void DoThirdPersonTracking();				// Handle actor-locked third person view
	void DoIsoTracking();
	//	Member data
	geVec3d Translation;								// Camera translation
	geVec3d Rotation;										// Camera rotation
	bool bBindToActor;									// TRUE if camera bound to actor
	geActor *theActor;									// Actor camera bound to, if any
	char szActorBone[128];							// Bone to bind to on actor, if any
	geVec3d CameraOffsetTranslation;		// Camera offset from actor, TRANSLATION
	geVec3d CameraOffsetRotation;				// Camera offset from actor, ROTATION
	geExtBox CameraExtBox;							// Camera bounding box, if used
	bool bUseExtBox;										// TRUE if camera collision checking used
	geWorld_Model *theModel;						// World model to bind to
	bool bBindToModel;									// TRUE if camera bound to world model
	geCamera *EngineCamera;							// Camera used for G3D rendering
	geRect theCameraRect;								// G3D camera rectangle
	geFloat FarClipPlaneDistance;				// Far clip plane distance
	int TrackingFlags;									// Camera tracking control flags
	geFloat m_defaultdistance;
	geFloat m_currentdistance;
	geFloat m_cameraX;
	geFloat m_cameraY;
	geFloat m_oldrotationx;
	// eaa3 12/18/2000 Head bobbing vars
	geFloat m_HeadBobOffset;					// For head bobbing
	bool m_HeadBob;								// Head bob enable flag
	// eaa3 12/20/2000 end
	geFloat m_LookUp;
	geFloat m_LookDwn;
	bool IsoCollFlag;
	geFloat FOV;
	bool zooming;
	bool zoommode;
	bool shake;
	float shakeamt;
	float shakedecay;
	float shakex, shakey;
	float shakemin;
	float viewheight;
	float playerheight;
	float playerangleup;
	float playerdistance;
	float playermindistance;
	float playermaxdistance;
	bool playerzoom;
	bool playerallowlook;
	bool playermouserotation;
	float isoheight;
	float isoangleup;
	float isoanglearound;
	float isodistance;
	bool switchallowed;
	bool switch1st;
	bool switch3rd;
	bool switchiso;
// changed RF063
	bool jerk;
	float jerkamt;
	float jerkdecay;
	geVec3d sTranslation;
	geVec3d sRotation;
	geVec3d sCameraOffsetTranslation;
	geVec3d sCameraOffsetRotation;
	geFloat sm_defaultdistance;
	geFloat sm_currentdistance;
	geFloat sm_cameraX;
	geFloat sm_cameraY;
	geFloat sm_oldrotationx;
	geFloat sFOV;
// end change RF063
// changed RF064
	geFloat DesiredHeight;
	float HeightSpeed;
	float OverlayDist;
	geXForm3d	m_OldXForm;
	geVec3d		m_vMoveDif;
	bool		m_bViewChanged;
	bool		m_bPositionMoved;
// end change RF064
};

#endif
