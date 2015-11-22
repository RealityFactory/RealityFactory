/*
CCameraManager.h:		Camera positioning, rotation, and motion manager

  This file contains the class declaration for the Camera Manager
  class.
  
	The Camera Manager is responsible for handling the motion of the
	player viewpoint camera during the game.
*/

#ifndef __CCAMERAMANAGER_H_
#define __CCAMERAMANAGER_H_

class CCameraManager: public CRGFComponent
{
public:
	CCameraManager();
	~CCameraManager();
	void Defaults();
	int Position(geVec3d thePosition);	// Set camera position
	int Rotate(geVec3d theRotation);		// Set camera rotation
	int TurnLeft(geFloat fAmount);			// Turn camera LEFT
	int TurnRight(geFloat fAmount);			// Turn camera RIGHT
	int TiltUp(geFloat fAmount);				// Tilt camera UP
	int TiltDown(geFloat fAmount);			// Tilt camera DOWN
	int TiltXUp(geFloat fAmount);				// Tilt camera UP
	int TiltXDown(geFloat fAmount);			// Tilt camera DOWN
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
	void CameraRotY(bool direction);
	void ResetCamera();
	void ChangeDistance(bool direction);
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
	void SetLook(geFloat lookup, geFloat lookdwn);
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
};

#endif
