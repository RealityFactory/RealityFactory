/*
	Utilities.h:		Utility functions

	(c) 2001 Ralph Deane
	All Rights Reserved

	This file contains the headers for utility functions

*/

#ifndef __RGF_UTILITY_H_
#define __RGF_UTILITY_H_

bool SetOriginOffset(char *EntityName, char *BoneName, geWorld_Model *Model, geVec3d *OriginOffset);
bool SetAngle(char *EntityName, char *BoneName, geVec3d *Angle);
bool GetTriggerState(char *TriggerName);
bool GetCallBackState(char *CallBackName);
geBoolean EffectC_IsStringNull(char *String );
geBoolean EffectC_IsPointVisible(geWorld *World, geCamera *Camera, geVec3d *Target, int32 Leaf,	uint32 ClipStyle );
float EffectC_Frand(float Low, float High );
void EffectC_XFormFromVector(geVec3d *Source, geVec3d *Target, geXForm3d *Out );
geBitmap *CreateFromFileAndAlphaNames(char * BmName,char *AlphaName);
geBitmap * CreateFromFileName(char * BmName);
long DLL_CALLCONV VFS_Tell(fi_handle handle);
int DLL_CALLCONV VFS_Seek(fi_handle handle, long offset, int origin);
unsigned DLL_CALLCONV VFS_Read(void *buffer, unsigned size, unsigned count, fi_handle handle);
// Weapon
void CollisionCalcRatio(const GE_Collision& a_Collision, const geVec3d& a_OldPos,
                               const geVec3d& a_NewPos, float& a_Ratio);
void CollisionCalcImpactPoint(const GE_Collision& a_Collision, const geVec3d& a_OldPos,
                                     const geVec3d& a_NewPos, float a_MinDistanceFromPlane,
                                     float a_Ratio, geVec3d& a_Impact);
void CollisionCalcVelocityImpact(const GE_Collision& a_Collision, const geVec3d& a_OldVelocity, 
                                        float a_Elasticity, float a_Friction, 
                                        geVec3d& a_NewVelocity);
geVec3d Extract(char *Vector);

//===========================
// Bit manipulation
//===========================

#define TEST(flags,mask) ((flags) & (mask))
#define SET(flags,mask) ((flags) |= (mask))
#define RESET(flags,mask) ((flags) &= ~(mask))
#define FLIP(flags,mask) ((flags) ^= (mask))

// mask definitions
#define BIT(shift)     (1<<(shift))

#define SET_BITARR(bitarr, bitnum) (bitarr[(bitnum)>>3] |= (1<<((bitnum)&7)))
#define RESET_BITARR(bitarr, bitnum) (bitarr[(bitnum)>>3] &= ~(1<<((bitnum)&7)))
#define TEST_BITARR(bitarr, bitnum) (bitarr[(bitnum)>>3] & (1<<((bitnum)&7)))

#define M_PI (3.14159f)
#define PI_2 (M_PI*2.0f)
#define M_PI2 (PI_2)

void Ang2Vec(float ang, geVec3d *vec);
geBoolean CanSeePointToPoint(geVec3d *Pos1, geVec3d *Pos2);
geBoolean CanSeeActorToPoint(geActor *Actor, geVec3d *Pos2);
geBoolean CanSeePointToActor(geVec3d *Pos2, geActor *Actor);
geBoolean CanSeeActorToActor(geActor *Actor1, geActor *Actor2);
// changed RF064
geActor * GetEntityActor(char *EntityName);
bool geVec3d_IsZero( geVec3d* pVect );
geFloat Length(geVec3d & vec);
void SetEnvironmentMapping(geActor *Actor, bool Enable, bool AllMaterial, float Percent, float PercentMaterial);
// end change RF064
const char *RootBoneName(geActor *Actor);
#endif
