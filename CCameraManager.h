/************************************************************************************//**
 * @file CCameraManager.h
 * @brief Camera positioning, rotation, and motion manager
 *
 * This file contains the class declaration for the Camera Manager class.
 * The Camera Manager is responsible for handling the motion of the player
 * viewpoint camera during the game.
 ****************************************************************************************/

#ifndef __CCAMERAMANAGER_H_
#define __CCAMERAMANAGER_H_

#include "RFSX\\RFSX.h"
#include <queue>


#ifdef SX_IMPL_TYPE
#undef SX_IMPL_TYPE
#endif
#define SX_IMPL_TYPE CCameraManager

#ifdef SX_METHOD_ARGS
#undef SX_METHOD_ARGS
#endif
#define SX_METHOD_ARGS sxCameraMethodArgs

class SX_METHOD_ARGS;

#define SXCAMERAMANAGER_METHODS 19

/**
 * @brief Camera Manager class
 *
 * The Camera Manager is responsible for handling the motion of the
 * player viewpoint camera during the game.
 */
class CCameraManager: public CRGFComponent, skExecutable
{
public:
	~CCameraManager();
	static void InitMHT();

	virtual bool method(const skString& m_name, skRValueArray& args, skRValue& r_val, skExecutableContext& ctxt);
	virtual int executableType() { return RFSXU_CAMERA; }

	void Defaults();

	int SaveTo(FILE* saveFD);
	int RestoreFrom(FILE* restoreFD);

	void SaveToS();
	void RestoreFromS();

	void SetJerk(float maxAmount, float decay);

	void Update(float timeElapsed);

	void Enqueue(SX_METHOD_ARGS* ma);
	void Dequeue();
	void DequeueAll();

	void SetZoom(bool flag);
	void CancelZoom();

	void SetShake(float maxAmount, float decay, const geVec3d& pos);
	void GetShake(float* x, float* y) const;

	int Center();								///< Center camera view
	int SetPosition(const geVec3d& position);	///< Set camera position
	int GetPosition(geVec3d* position) const;	///< Get camera position

	int SetRotation(const geVec3d& rotation);	///< Set camera rotation
	int GetRotation(geVec3d* rotation) const;	///< Get camera rotation

	int TurnLeft(float amount);					///< Turn camera LEFT
	int TurnRight(float amount);				///< Turn camera RIGHT

	int TiltUp(float amount);					///< Tilt camera UP
	int TiltDown(float amount);					///< Tilt camera DOWN
	int TiltXUp(float amount);					///< Tilt camera UP
	int TiltXDown(float amount);				///< Tilt camera DOWN
	float GetTiltPercent() const;
	float GetTilt() const;

	int BindToActor(geActor* actor);								///< Bind camera to actor
	int BindToActorBone(geActor* actor, const char* szBoneName);	///< Bind to actor bone
	int BindToWorldModel(geWorld_Model* model);						///< Bind camera to world model
	int Unbind();													///< Remove camera bindings

	// Set camera XYZ/Rot offset
	int SetCameraOffset(const geVec3d& translation, const geVec3d& rotation);
	int GetCameraOffset(geVec3d* translation, geVec3d* rotation);

	int SetBoundingBox(const geExtBox& box);	///< Set camera collision box
	int ClearBoundingBox();						///< Remove bounding box
	int RenderView();							///< Render the view from the camera
	bool InCollision();							///< Check for camera in collision

	int		SetFarClipPlane(float distance);		///< Set camera clip plane distance
	void	EnableFarClipPlane(geBoolean clipOn);	///< Enable/disable clip plane
	float	GetFarClipPlane() const		{ return m_FarClipPlaneDistance;}
	bool	GetClipEnable() const		{ return m_ClipEnable;			}

	int TrackMotion();							///< Track motion, if required
	inline void SetTrackingFlags(int flags)		///< Set camera tracking flags
				{ m_TrackingFlags = flags; }
	inline int GetTrackingFlags() const			///< Get camera tracking flags
				{ return m_TrackingFlags; }

	inline geCamera* Camera() { return m_EngineCamera;}

	void	CameraRotX(bool direction);
	void	CameraRotY(bool direction, float speed);
	void	ResetCamera();
	void	ChangeDistance(bool direction, float speed);
	geXForm3d ViewPoint();

	// Head bob functions
	void	SetHeadBobOffset(float offset)	{ m_HeadBobOffset = offset; }
	float	GetHeadBobOffset() const		{ return m_HeadBobOffset; }
	bool	EnableHeadBob(bool enable)		{ bool xtmp = m_HeadBob; m_HeadBob = enable; return xtmp; }

	void	Set3rdData(float distance, float cameraX, float cameraY);
	void	Get3rdData(float* distance, float* cameraX, float* cameraY);

	void	SetFOV(float fov)				{ m_DefaultFOV = m_FOV = fov;	}
	float	GetFOV() const					{ return m_FOV;					}

	float	AmtZoom() const					{ return m_DefaultFOV/m_FOV;	}
	void	SetShakeMin(float min)			{ m_ShakeMin = min;				}

	float	GetViewHeight() const			{ return m_ViewHeight;			}
	float	GetPlayerHeight() const			{ return m_PlayerHeight;		}
	float	GetPlayerAngleUp() const		{ return m_PlayerAngleUp;		}
	float	GetPlayerDistance() const		{ return m_PlayerDistance;		}
	float	GetPlayerMinDistance() const	{ return m_PlayerMinDistance;	}
	float	GetPlayerMaxDistance() const	{ return m_PlayerMaxDistance;	}
	bool	GetPlayerZoom() const			{ return m_PlayerZoom;			}
	bool	GetPlayerAllowLook() const		{ return m_PlayerAllowLook;		}
	bool	GetPlayerMouseRotation() const	{ return m_PlayerMouseRotation;	}
	float	GetIsoHeight() const			{ return m_IsoHeight;			}
	float	GetIsoAngleUp() const			{ return m_IsoAngleUp;			}
	float	GetIsoAngleAround() const		{ return m_IsoAngleAround;		}
	float	GetIsoDistance() const			{ return m_IsoDistance;			}
	bool	GetIsoFlag() const				{ return m_IsoCollFlag;			}
	bool	GetSwitchAllowed() const		{ return m_SwitchAllowed;		}
	bool	GetSwitch1st() const			{ return m_Switch1st;			}
	bool	GetSwitch3rd() const			{ return m_Switch3rd;			}
	bool	GetSwitchIso() const			{ return m_SwitchIso;			}

	bool	GetZooming() const				{ return m_ZoomMode;			}
	float	GetJerk() const					{ return m_JerkAmount;			}

	float	GetOverlayDist() const			{ return m_OverlayDist;			}
	bool	GetViewChanged() const			{ return m_bViewChanged;		}
	bool	GetPositionMoved() const		{ return m_bPositionMoved;		}
	float	GetDistanceMoved() const		{ return geVec3d_Length(&m_vMoveDif); }
	bool	GetAllowMouse1st() const		{ return m_AllowMouse1st;		}
	bool	GetAllowMouse3rd() const		{ return m_AllowMouse3rd;		}
	bool	GetAllowMouseIso() const		{ return m_AllowMouseIso;		}
	const geVec3d* GetMoveDif() const		{ return &m_vMoveDif;			}
	void SetDesiredHeight(float height)		{ m_DesiredHeight = height;		}

	static CCameraManager* GetSingletonPtr();

	Q_METHOD_DECL(SetFOV);
	Q_METHOD_DECL(SetClippingRect);
	Q_METHOD_DECL(SetPosition);
	Q_METHOD_DECL(SetFarClipPlane);
	Q_METHOD_DECL(EnableFarClipPlane);
	Q_METHOD_DECL(DisableFarClipPlane);
	Q_METHOD_DECL(Move);
	Q_METHOD_DECL(Rotate);
	Q_METHOD_DECL(LookAt);
	Q_METHOD_DECL(SyncPoint);

	void InsertScriptSyncPoint();
	void ReachScriptSyncPoint() { ++m_SyncPoints; }
private:
	int m_SyncPoints;

private:
	// Member functions
	void DoThirdPersonTracking();				///< Handle actor-locked third person view
	void DoIsoTracking();

private:
	// Member data
	geVec3d			m_Translation;				///< Camera translation
	geVec3d			m_Rotation;					///< Camera rotation (rad)
	bool			m_bBindToActor;				///< TRUE if camera bound to actor
	geActor*		m_Actor;					///< Actor camera bound to, if any
	char			m_szActorBone[128];			///< Bone to bind to on actor, if any
	geVec3d			m_CameraOffsetTranslation;	///< Camera offset from actor, TRANSLATION
	geVec3d			m_CameraOffsetRotation;		///< Camera offset from actor, ROTATION
	geExtBox		m_CameraExtBox;				///< Camera bounding box, if used
	bool			m_bUseExtBox;				///< TRUE if camera collision checking used
	geWorld_Model*	m_Model;					///< World model to bind to
	bool			m_bBindToModel;				///< TRUE if camera bound to world model
	geCamera*		m_EngineCamera;				///< Camera used for G3D rendering
	geRect			m_CameraRect;				///< Genesis3D camera rectangle
	float			m_FarClipPlaneDistance;		///< Far clip plane distance
	bool			m_ClipEnable;
	int				m_TrackingFlags;			///< Camera tracking control flags

	float			m_DefaultDistance;
	float			m_CurrentDistance;

	float			m_CameraX; // in rad
	float			m_CameraY; // in rad
	float			m_OldRotationX;

	// Head bobbing vars
	float			m_HeadBobOffset;			///< Head bobbing offset
	bool			m_HeadBob;					///< Head bob enable flag

	float			m_LookUp;
	float			m_LookDwn;

	float			m_DefaultFOV;
	float			m_FOV;

	bool			m_Zooming;
	bool			m_ZoomMode;

	bool			m_Shake;
	float			m_ShakeAmount;
	float			m_ShakeDecay;
	float			m_ShakeX, m_ShakeY;
	float			m_ShakeMin;

	float			m_ViewHeight;

	float			m_PlayerHeight;
	float			m_PlayerAngleUp;
	float			m_PlayerDistance;
	float			m_PlayerMinDistance;
	float			m_PlayerMaxDistance;
	bool			m_PlayerZoom;
	bool			m_PlayerAllowLook;
	bool			m_PlayerMouseRotation;

	float			m_IsoHeight;
	float			m_IsoAngleUp;
	float			m_IsoAngleAround;
	float			m_IsoDistance;
	bool			m_IsoCollFlag;

	bool			m_SwitchAllowed;
	bool			m_Switch1st;
	bool			m_Switch3rd;
	bool			m_SwitchIso;

	bool			m_Jerk;
	float			m_JerkAmount;
	float			m_JerkDecay;

	// save camera settings to temp variables
	geVec3d			m_sTranslation;
	geVec3d			m_sRotation;
	geVec3d			m_sCameraOffsetTranslation;
	geVec3d			m_sCameraOffsetRotation;

	float			m_sDefaultDistance;
	float			m_sCurrentDistance;
	float			m_sCameraX;
	float			m_sCameraY;
	float			m_sOldRotationX;
	float			m_sFOV;

	float			m_DesiredHeight;
	float			m_HeightSpeed;
	float			m_OverlayDist;
	geXForm3d		m_OldXForm;
	geVec3d			m_vMoveDif;
	bool			m_bViewChanged;
	bool			m_bPositionMoved;

	bool			m_AllowMouse1st;
	bool			m_AllowMouse3rd;
	bool			m_AllowMouseIso;
	bool			m_AllowTilt3rd;

protected:
	SX_METHOD_DECL(SetFOV);
	SX_METHOD_DECL(SetClippingRect);
	SX_METHOD_DECL(SetPosition);
	SX_METHOD_DECL(SetFarClipPlane);
	SX_METHOD_DECL(EnableFarClipPlane);
	SX_METHOD_DECL(DisableFarClipPlane);
	SX_METHOD_DECL(Move);
	SX_METHOD_DECL(MoveForward);
	SX_METHOD_DECL(MoveBackward);
	SX_METHOD_DECL(MoveLeft);
	SX_METHOD_DECL(MoveRight);
	SX_METHOD_DECL(MoveUp);
	SX_METHOD_DECL(MoveDown);
	SX_METHOD_DECL(Rotate);
	SX_METHOD_DECL(LookAt);

	// low level
	SX_METHOD_DECL(GetFOV);
	SX_METHOD_DECL(GetClippingRect);
	SX_METHOD_DECL(GetPosition);
	SX_METHOD_DECL(GetFarClipPlane);

private:
	SX_IMPL_TYPE();
	SX_IMPL_TYPE(const SX_IMPL_TYPE&) {}
	SX_IMPL_TYPE& operator = (const SX_IMPL_TYPE&);

	bool					m_Skip;

	std::queue<SX_METHOD_ARGS*>	m_MethodQueue;

private:
	static RFSX::sxMHT<SX_IMPL_TYPE>	h_method;
	static SX_IMPL_TYPE*				m_CameraManager;
};

#undef SX_METHOD_ARGS
#undef SX_IMPL_TYPE

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
