/*
CCameraManager.cpp:		Camera positioning, rotation, and motion manager

  This file contains the class declaration for the Camera Manager
  class.
  
	The Camera Manager is responsible for handling the motion of the
	player viewpoint camera during the game.
*/

#include "RabidFramework.h"				// The One True Include File

//	Default constructor

CCameraManager::CCameraManager()
{
	Defaults();
	memset(&CameraExtBox, 0, sizeof(geExtBox));
	CameraExtBox.Min.X = CameraExtBox.Min.Y = CameraExtBox.Min.Z = -4.0f;
	CameraExtBox.Max.X = CameraExtBox.Max.Y = CameraExtBox.Max.Z = 4.0f;
	
	// Set up our camera viewing rectangle
	
	theCameraRect.Left = 0;
	theCameraRect.Right = CCD->Engine()->Width() - 1;
	theCameraRect.Top = 0;
	theCameraRect.Bottom = CCD->Engine()->Height() - 1;
	
	// Create the Genesis3D camera
	
	EngineCamera = geCamera_Create(2.0f, &theCameraRect);
	
	if(!EngineCamera) 
	{	
		CCD->ReportError("ge_Camera_Created failure", false);
		return;
	}
	geCamera_SetZScale(EngineCamera, 1.0f);
	return;
}

void CCameraManager::Defaults()
{
	memset(&Translation, 0, sizeof(geVec3d));
	memset(&Rotation, 0, sizeof(geVec3d));
	bBindToActor = false;
	theActor = NULL;
	szActorBone[0] = 0;
	memset(&CameraOffsetTranslation, 0, sizeof(geVec3d));
	memset(&CameraOffsetRotation, 0, sizeof(geVec3d));
	bUseExtBox = false;
	theModel = NULL;
	bBindToModel = NULL;
	FarClipPlaneDistance = 500.0f;
	TrackingFlags = kCameraTrackPosition;
	// eaa3 12/18/2000 head bob setup
	m_HeadBobOffset = 0.0f;					// No offset
	m_HeadBob = false;						// Default to no bobbing
	// eaa3 12/18/2000 end
	m_LookUp = 1.0f;
	m_LookDwn = 1.0f;
}
//	Default destructor

CCameraManager::~CCameraManager()
{
	if(EngineCamera != NULL)
		geCamera_Destroy(&EngineCamera);
	
	EngineCamera = NULL;
	
	return;
}

//	Position
//
//	Set the current camera position

int CCameraManager::Position(geVec3d thePosition)
{
	Translation = thePosition;
	
	return RGF_SUCCESS;
}

//	Rotate
//
//	Set the camera rotation

int CCameraManager::Rotate(geVec3d theRotation)
{
	Rotation = theRotation;
	
	return RGF_SUCCESS;
}

//	TurnLeft
//
//	Rotate the camera to look LEFT

int CCameraManager::TurnLeft(geFloat fAmount)
{
	Rotation.Y -= fAmount;
	
	return RGF_SUCCESS;
}

//	TurnRight
//
//	Rotate the camera to look RIGHT

int CCameraManager::TurnRight(geFloat fAmount)
{
	Rotation.Y += fAmount;
	
	return RGF_SUCCESS;
}

//	TiltUp
//
//	Rotate the camera to look UP

int CCameraManager::TiltUp(geFloat fAmount)
{
	Rotation.X += fAmount;
// Mode
	if (Rotation.X >m_LookUp)  Rotation.X = m_LookUp;
	return RGF_SUCCESS;
}

//	TiltDown
//
//	Rotate the camera to look DOWN

int CCameraManager::TiltDown(geFloat fAmount)
{
	Rotation.X -= fAmount;
// Mode
	if (Rotation.X <-m_LookDwn) Rotation.X = -m_LookDwn;
	return RGF_SUCCESS;
}

//	Center
//
//	Reset the camera rotations to more or less straighten it up

int CCameraManager::Center()
{
	Rotation.X = 0.0f;
	Rotation.Y = 0.0f;
	
	return RGF_SUCCESS;
}


//	TiltXUp
//
//	Rotate the camera to look UP

int CCameraManager::TiltXUp(geFloat fAmount)
{
	if((TrackingFlags & kCameraTrackFree) != 0)
	{
		CameraOffsetRotation.X += fAmount;
		if (CameraOffsetRotation.X >m_LookUp)
			CameraOffsetRotation.X = m_LookUp;
	}
	return RGF_SUCCESS;
}

//	TiltXDown
//
//	Rotate the camera to look DOWN

int CCameraManager::TiltXDown(geFloat fAmount)
{
	if((TrackingFlags & kCameraTrackFree) != 0)
	{
		CameraOffsetRotation.X -= fAmount;
		if (CameraOffsetRotation.X <-m_LookDwn)
			CameraOffsetRotation.X = -m_LookDwn;
	}
	return RGF_SUCCESS;
}


void CCameraManager::SetLook(geFloat lookup, geFloat lookdwn)
{
	if(lookup>0.0f)
		m_LookUp = lookup;
	if(lookdwn>0.0f)
		m_LookDwn = lookdwn;
}

//	GetPosition
//
//	Return the cameras current position

int CCameraManager::GetPosition(geVec3d *thePosition)
{
	*thePosition = Translation;
	
	return RGF_SUCCESS;
}

//	GetRotation
//
//	Return the cameras current rotation

int CCameraManager::GetRotation(geVec3d *theRotation)
{
	*theRotation = Rotation;
	
	return RGF_SUCCESS;
}

//	BindToActor
//
//	Bind the camera to an actor in the level.  This will force the camera
//	..to track the position and rotation of the actor as long as the
//	..binding is in force.

int CCameraManager::BindToActor(geActor *ActorToBindTo)
{
	bBindToActor = true;
	theActor = ActorToBindTo;
	
	return RGF_SUCCESS;
}

//	SetCameraOffset
//
//	Defines an offset to be used whenever the camera is bound to an actor
//	..or a world model.  This offset will be added to the translation of
//	..the bound-to entity to offset the camera from the thing itself.

int CCameraManager::SetCameraOffset(geVec3d theTranslation, geVec3d theRotation)
{
	CameraOffsetTranslation = theTranslation;
	CameraOffsetRotation = theRotation;
	
	return RGF_SUCCESS;
}

int CCameraManager::GetCameraOffset(geVec3d *theTranslation, geVec3d *theRotation)
{
	*theTranslation = CameraOffsetTranslation;
	*theRotation = CameraOffsetRotation;
	m_oldrotationx = CameraOffsetRotation.X;
	
	return RGF_SUCCESS;
}

//	BindToActorBone
//
//	Binds the camera to a specific bone on a specific actor, other than that,
//	..is just like BindToActor().  The camera offset is used here.

int CCameraManager::BindToActorBone(geActor *theBindActor, char *szBoneName)
{
	theActor = theBindActor;
	strcpy(szActorBone, szBoneName);
	bBindToActor = true;
	
	return RGF_SUCCESS;
}

//	BindToWorldModel
//
//	Binds the camera to track the position and rotation of the given world
//	..model.  The camera offset is used here.

int CCameraManager::BindToWorldModel(geWorld_Model *theBindModel)
{
	bBindToModel = true;
	theModel = theBindModel;
	
	return RGF_SUCCESS;
}

//	SetBoundingBox
//
//	Defines a bounding box for use by the camera so camera collisions can
//	..be tracked.

int CCameraManager::SetBoundingBox(geExtBox theBox)
{
	CameraExtBox = theBox;
	bUseExtBox = true;
	
	return RGF_SUCCESS;
}

//	ClearBoundingBox
//
//	Removes the camera bounding box

int CCameraManager::ClearBoundingBox()
{
	bUseExtBox = false;
	
	return RGF_SUCCESS;
}

//	Unbind
//
//	Removes ALL CAMERA BINDINGS.

int CCameraManager::Unbind()
{
	theActor = NULL;
	szActorBone[0] = 0;
	bBindToActor = false;
	bBindToModel = false;
	theModel = NULL;
	
	return RGF_SUCCESS;
}

//	TrackMotion
//
//	This routine forces the camera rotation and position to track whatever
//	..it is bound to, if anything.

int CCameraManager::TrackMotion()
{
	if(bBindToActor)
	{
		// Get the actors position and rotation
		if((TrackingFlags & kCameraTrackPosition) != 0)
		{
			if(szActorBone[0] != 0)
				CCD->ActorManager()->GetBonePosition(theActor, szActorBone, &Translation);
			else
				CCD->ActorManager()->GetPosition(theActor, &Translation);
			// Ok, add in the offset to line us up the way we want
			Translation.X += CameraOffsetTranslation.X;
			Translation.Y += CameraOffsetTranslation.Y;
			Translation.Z += CameraOffsetTranslation.Z;
		}
		if((TrackingFlags & kCameraTrackRotation) != 0)
		{
			if(szActorBone[0] != 0)
				CCD->ActorManager()->GetBoneRotation(theActor,szActorBone, &Rotation);
			else
				CCD->ActorManager()->GetRotation(theActor, &Rotation);
		}

		Rotation.X += CameraOffsetRotation.X;
		Rotation.Y += CameraOffsetRotation.Y;
		Rotation.Z += CameraOffsetRotation.Z;

		// eaa3 12/18/200 Start head bob code
		if((m_HeadBob) && (m_HeadBobOffset != 0.0f))
			Translation.Y += m_HeadBobOffset;			// Adjust for 'head bob'
		// eaa3 12/18/2000 End head bob code

		if((TrackingFlags & kCameraTrackThirdPerson) != 0)
			DoThirdPersonTracking();
		if((TrackingFlags & kCameraTrackIso) != 0)
			DoIsoTracking();
	}
	else if(bBindToModel)
	{
		// Get the actors position and rotation
		if((TrackingFlags & kCameraTrackPosition) != 0)
		{
			CCD->ModelManager()->GetPosition(theModel, &Translation);
			// Ok, add in the offset to line us up the way we want
			Translation.X += CameraOffsetTranslation.X;
			Translation.Y += CameraOffsetTranslation.Y;
			Translation.Z += CameraOffsetTranslation.Z;
		}
		if((TrackingFlags & kCameraTrackRotation) != 0)
		{
			CCD->ModelManager()->GetRotation(theModel, &Rotation);
			// Ok, add in the offset to line us up the way we want
			Rotation.X += CameraOffsetRotation.X;
			Rotation.Y += CameraOffsetRotation.Y;
			Rotation.Z += CameraOffsetRotation.Z;
		}
	}
	
	return RGF_SUCCESS;
}

//	RenderView
//
//	Render the view from this camera into the current window.

int CCameraManager::RenderView()
{
	geXForm3d theViewPoint;
	
	geXForm3d_SetIdentity(&theViewPoint); // Clear the matrix
	
	geXForm3d_RotateZ(&theViewPoint, Rotation.Z); // Rotate then translate
	geXForm3d_RotateX(&theViewPoint, Rotation.X);
	geXForm3d_RotateY(&theViewPoint, Rotation.Y);
	
	geXForm3d_Translate(&theViewPoint, Translation.X, Translation.Y,
		Translation.Z);
	
	//	Set up camera attributes just prior to rendering the world.
	
	geRect Rect;
	geCamera_SetWorldSpaceXForm(EngineCamera, &theViewPoint);
	geCamera_GetClippingRect(EngineCamera, &Rect);
	geCamera_SetAttributes(EngineCamera, 2.0, &Rect);
	//geCamera_SetAttributes(EngineCamera, 2.0, &theCameraRect);
	
	return RGF_SUCCESS;
}

// ViewPoint
//
// return the viewpoint of the camera

geXForm3d CCameraManager::ViewPoint()
{
	geXForm3d theViewPoint;
	
	TrackMotion();
	geXForm3d_SetIdentity(&theViewPoint); // Clear the matrix
	geXForm3d_RotateZ(&theViewPoint, Rotation.Z); // Rotate then translate
	geXForm3d_RotateX(&theViewPoint, Rotation.X);
	geXForm3d_RotateY(&theViewPoint, Rotation.Y);
	geXForm3d_Translate(&theViewPoint, Translation.X, Translation.Y,
		Translation.Z);

	return theViewPoint;
}

//	InCollision
//
//	Returns TRUE if the camera bounding box is in collision with anything.
//	..returns FALSE otherwise, or if no bounding box is set for use.

bool CCameraManager::InCollision()
{
	return false;
}

//	SetFarClipPlane
//
//	Set the far clip plane distance, in other words, nothing further
//	..than this from the camera will get rendered.

int CCameraManager::SetFarClipPlane(geFloat fDistance)
{
	FarClipPlaneDistance = fDistance;
	
	return RGF_SUCCESS;
}

//	EnableFarClipPlane
//
//	Turn the far clip plane on and off.

void CCameraManager::EnableFarClipPlane(geBoolean ClipOn)
{
	geCamera_SetFarClipPlane(EngineCamera, ClipOn, FarClipPlaneDistance);
	
	return;
}

void CCameraManager::CameraRotX(bool direction)
{
	if((TrackingFlags & kCameraTrackFree) != 0)
	{
		if(direction)
		{
			m_cameraX+=2.0f;
			if(m_cameraX>(89.99f-(CameraOffsetRotation.X/0.0174532925199433f)))
				m_cameraX = (89.99f-(CameraOffsetRotation.X/0.0174532925199433f));
		}
		else
		{
			m_cameraX-=2.0f;
			if(m_cameraX<0.0f)
				m_cameraX = 0.0f;
		}
	}
}

void CCameraManager::CameraRotY(bool direction)
{
	if((TrackingFlags & kCameraTrackFree) != 0)
	{
		if(direction)
		{
			m_cameraY+=5.0f;
			if(m_cameraY>=360.0f)
				m_cameraY = 0.0f;
		}
		else
		{
			m_cameraY-=5.0f;
			if(m_cameraY<0.0f)
				m_cameraY = 360.0f-m_cameraY;
		}
	}
}

void CCameraManager::ResetCamera()
{
	m_currentdistance = m_defaultdistance = CameraOffsetTranslation.Z;
	m_cameraX = 0.0f;
	m_cameraY = 0.0f;
	Rotation.X = m_oldrotationx;
}

void CCameraManager::ChangeDistance(bool direction)
{
	if((TrackingFlags & kCameraTrackFree) != 0)
	{
		if(m_defaultdistance>m_currentdistance)
			m_defaultdistance = m_currentdistance;
		
		if(direction)
		{
			m_defaultdistance+=5.0f;
			if(m_defaultdistance>(CameraOffsetTranslation.Z * 2.0f))
				m_defaultdistance = CameraOffsetTranslation.Z * 2.0f;
		}
		else
		{
			m_defaultdistance-=5.0f;
			if(m_defaultdistance<5.0f)
				m_defaultdistance = 5.0f;
		}
	}
}

//	********* PRIVATE MEMBER FUNCTIONS ************

//	DoThirdPersonTracking
//
//	We have to do some fancy footwork if we want to have decent third-person
//	..view handling when tracking an actor - so this does the job.

void CCameraManager::DoThirdPersonTracking()
{
	geXForm3d theViewPoint;
	geVec3d Pos;
	GE_Collision Collision;
	geVec3d Front, Back;
	geVec3d Direction;
	geFloat CurrentDistance;
	geExtBox ActorExtBox;
	geFloat ActorScale;
	geVec3d Orient;
	geFloat x;
	
	CCD->ActorManager()->GetScale(theActor, &ActorScale);		// Get actor scale
	CCD->ActorManager()->GetBoundingBox(theActor, &ActorExtBox);
	CurrentDistance = m_defaultdistance * ActorScale;
	
	geVec3d ActorPosition, ActorRotation;
	CCD->ActorManager()->GetPosition(theActor, &ActorPosition);
	ActorPosition.Y += CameraOffsetTranslation.Y;
	CCD->ActorManager()->GetRotation(theActor, &ActorRotation);
	ActorRotation.X = 0.0f;
	geXForm3d_SetIdentity(&theViewPoint);
	geXForm3d_RotateZ(&theViewPoint, ActorRotation.Z+CameraOffsetRotation.Z);
	geXForm3d_RotateX(&theViewPoint, ActorRotation.X+CameraOffsetRotation.X+0.0174532925199433f*(m_cameraX));
	geXForm3d_RotateY(&theViewPoint, ActorRotation.Y+CameraOffsetRotation.Y+0.0174532925199433f*(m_cameraY));
	geXForm3d_Translate(&theViewPoint, ActorPosition.X, ActorPosition.Y,
		ActorPosition.Z);
	
	Pos = theViewPoint.Translation;
	geXForm3d_GetIn(&theViewPoint, &Direction);
	geVec3d_AddScaled (&Pos, &Direction, CurrentDistance, &Back);
	geVec3d_AddScaled (&Pos, &Direction, 0.0f, &Front);
	
	if(geWorld_Collision(CCD->World(), &CameraExtBox.Min, &CameraExtBox.Max, &Front, 
		&Back, GE_VISIBLE_CONTENTS, GE_COLLIDE_ALL, 0, NULL, NULL, &Collision))
    {
		CurrentDistance = (geFloat)fabs(geVec3d_DistanceBetween(&Collision.Impact, &Front));
		if(CurrentDistance < 0.0f)
			CurrentDistance = 0.0f;
		if(CurrentDistance > (m_defaultdistance*ActorScale))
			CurrentDistance = m_defaultdistance*ActorScale;
		geVec3d_AddScaled (&Pos, &Direction, CurrentDistance, &Back);
	}
	m_currentdistance = CurrentDistance/ActorScale;

	// Ok, here's the implementation of Ralph Deane's too-cool
	// ..third-person Actor Fading code. As the camera approaches
	// ..the player avatar, the AVATAR IS FADED OUT until finally
	// ..it's rendering is shut down. This is one Way Cool Effect.
	// 03/22/2000 eaa3 Added Ralph Deane's scaling fixes.
	
	geFloat fAlpha = 255.0f;
	
	if(CurrentDistance < (40.0f*ActorScale))
	{
		fAlpha = (10.0f*((CurrentDistance-((geFloat)fabs(ActorExtBox.Min.Z)+1.0f))/ActorScale))+30.0f;
		if(fAlpha < (15.0f*ActorScale))
			fAlpha = 0.0f;
		if(fAlpha > 255.0f)
			fAlpha = 255.0f;
	}
	
	CCD->ActorManager()->SetAlpha(theActor, fAlpha);			// Adjust actor alpha
	
	geVec3d_Subtract( &Pos, &Back, &Orient );
	
	// protect from Div by Zero
	
	if(CurrentDistance > 0.0f) 
	{
		x = Orient.X;
		Orient.X = (geFloat)( GE_PI*0.5 ) - (geFloat)acos(Orient.Y / CurrentDistance);
		Orient.Y = (geFloat)atan2( x , Orient.Z ) + GE_PI;
		// roll is zero - always!!?
		Orient.Z = 0.0;
	}
	
	Rotation = Orient;										// Set camera orientation
	Translation = Back;										// Set camera translation
	
	return;
}


//	DoIsoTracking
//
//	We have to do some fancy footwork if we want to have decent isometric
//	..view handling when tracking an actor - so this does the job.

void CCameraManager::DoIsoTracking()
{
	geXForm3d theViewPoint;
	geVec3d Pos;
	geVec3d Front, Back;
	geVec3d Direction;
	geFloat CurrentDistance;
	geExtBox ActorExtBox;
	geFloat ActorScale;
	geVec3d Orient;
	geFloat x;
	
	CCD->ActorManager()->GetScale(theActor, &ActorScale);		// Get actor scale
	CCD->ActorManager()->GetBoundingBox(theActor, &ActorExtBox);
	CurrentDistance = m_defaultdistance * ActorScale;
	
	geVec3d ActorPosition, ActorRotation;
	CCD->ActorManager()->GetPosition(theActor, &ActorPosition);
	ActorPosition.Y += CameraOffsetTranslation.Y;
	CCD->ActorManager()->GetRotation(theActor, &ActorRotation);
	ActorRotation.X = 0.0f;
	geXForm3d_SetIdentity(&theViewPoint);
	geXForm3d_RotateZ(&theViewPoint, CameraOffsetRotation.Z);
	geXForm3d_RotateX(&theViewPoint, CameraOffsetRotation.X+0.0174532925199433f*(m_cameraX));
	geXForm3d_RotateY(&theViewPoint, CameraOffsetRotation.Y+0.0174532925199433f*(m_cameraY));
	geXForm3d_Translate(&theViewPoint, ActorPosition.X, ActorPosition.Y,
		ActorPosition.Z);
	
	Pos = theViewPoint.Translation;
	geXForm3d_GetIn(&theViewPoint, &Direction);
	geVec3d_AddScaled (&Pos, &Direction, CurrentDistance, &Back);
	geVec3d_AddScaled (&Pos, &Direction, 0.0f, &Front);
	
	CurrentDistance = m_defaultdistance*ActorScale;
	m_currentdistance = CurrentDistance/ActorScale;

	geVec3d_Subtract( &Pos, &Back, &Orient );
	
	// protect from Div by Zero
	
	if(CurrentDistance > 0.0f) 
	{
		x = Orient.X;
		Orient.X = (geFloat)( GE_PI*0.5 ) - (geFloat)acos(Orient.Y / CurrentDistance);
		Orient.Y = (geFloat)atan2( x , Orient.Z ) + GE_PI;
		// roll is zero - always!!?
		Orient.Z = 0.0;
	}
	
	Rotation = Orient;										// Set camera orientation
	Translation = Back;										// Set camera translation
	
	return;
}

void CCameraManager::Get3rdData(float *distance, float *camerax, float *cameray)
{
	*distance = m_defaultdistance;
	*camerax = m_cameraX;
	*cameray = m_cameraY;
}

void CCameraManager::Set3rdData(float distance, float camerax, float cameray)
{
	m_defaultdistance = distance;
	m_cameraX = camerax;
	m_cameraY = cameray;
}
