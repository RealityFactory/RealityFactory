/************************************************************************************//**
 * @file Utilities.h
 * @brief Utility functions
 *
 * This file contains the headers for utility functions
 * @author Ralph Deane
 *//*
 ****************************************************************************************/

#ifndef __RGF_UTILITY_H_
#define __RGF_UTILITY_H_

// changed QD 02/01/07
// utility functions for std::string replacing CString functionality
inline void Replace(std::string &source, const std::string &strold, const std::string &strnew)
{
	if(strold == strnew) return;
	for(int found = source.find(strold); found != -1; found = source.find(strold, found+strnew.length()))
		source.replace(found, strold.length(), strnew);
}

inline void MakeLower(std::string &source)
{ for(unsigned int i=source.size(); i--!=0; source[i] = tolower(source[i])); }

inline void TrimRight(std::string &source)
{ source.erase(source.find_last_not_of(" \r\t\n")+1); }

inline void TrimRight(std::string &source, const std::string &t)
{ source.erase(source.find_last_not_of(t)+1); }

inline void TrimLeft(std::string &source)
{ source.erase(0, source.find_first_not_of(" \r\t\n"));}

inline void TrimLeft(std::string &source, const std::string &t)
{ source.erase(0, source.find_first_not_of(t)); }
// end change

bool SetOriginOffset(const char *EntityName, const char *BoneName,
					 const geWorld_Model *Model, geVec3d *OriginOffset);
bool SetAngle(const char *EntityName, const char *BoneName, geVec3d *Angle);

bool GetTriggerState(const char *TriggerName);
bool GetCallBackState(const char *CallBackName);

geBoolean EffectC_IsStringNull(const char *String);
geBoolean EffectC_IsPointVisible(geWorld *World, const geCamera *Camera, const geVec3d *Target,
								 int32 Leaf, uint32 ClipStyle);

// changed QD 01/2004
geBoolean EffectC_IsBoxVisible(geWorld *World, const geCamera *Camera, const geExtBox* TestBox);
// end change
int EffectC_rand(int Low, int High);
float EffectC_Frand(float Low, float High);
void EffectC_XFormFromVector(const geVec3d *Source, const geVec3d *Target, geXForm3d *Out);

geBitmap *CreateFromFileAndAlphaNames(const char *BmName, const char *AlphaName);
geBitmap *CreateFromFileName(const char *BmName);

long DLL_CALLCONV VFS_Tell(fi_handle handle);
int DLL_CALLCONV VFS_Seek(fi_handle handle, long offset, int origin);
unsigned DLL_CALLCONV VFS_Read(void *buffer, unsigned size, unsigned count, fi_handle handle);

// Weapon
void CollisionCalcRatio(const GE_Collision &a_Collision, const geVec3d &a_OldPos,
						const geVec3d &a_NewPos, float *a_Ratio);
void CollisionCalcImpactPoint(const GE_Collision &a_Collision, const geVec3d &a_OldPos,
								const geVec3d &a_NewPos, float a_MinDistanceFromPlane,
								float a_Ratio, geVec3d *a_Impact);
void CollisionCalcVelocityImpact(const GE_Collision &a_Collision, const geVec3d &a_OldVelocity,
									float a_Elasticity, float a_Friction, geVec3d *a_NewVelocity);
geVec3d Extract(char *Vector);

//===========================
// Bit manipulation
//===========================

#define TEST(flags,mask)	((flags) & (mask))
#define SET(flags,mask)		((flags) |= (mask))
#define RESET(flags,mask)	((flags) &= ~(mask))
#define FLIP(flags,mask)	((flags) ^= (mask))

// mask definitions
#define BIT(shift)     (1<<(shift))

#define SET_BITARR(bitarr, bitnum)		(bitarr[(bitnum)>>3] |= (1<<((bitnum)&7)))
#define RESET_BITARR(bitarr, bitnum)	(bitarr[(bitnum)>>3] &= ~(1<<((bitnum)&7)))
#define TEST_BITARR(bitarr, bitnum)		(bitarr[(bitnum)>>3] & (1<<((bitnum)&7)))

void Ang2Vec(float ang, geVec3d *vec);

geBoolean CanSeePointToPoint(const geVec3d *Pos1, const geVec3d *Pos2);
geBoolean CanSeeActorToPoint(const geActor *Actor, const geVec3d *Pos2);
geBoolean CanSeePointToActor(const geVec3d *Pos2, const geActor *Actor);
geBoolean CanSeeActorToActor(const geActor *Actor1, const geActor *Actor2);

// changed RF064
geActor *GetEntityActor(const char *EntityName);
bool geVec3d_IsZero(geVec3d* pVect);
geFloat Length(geVec3d &vec);
void SetEnvironmentMapping(geActor *Actor, bool Enable, bool AllMaterial, float Percent, float PercentMaterial);
// end change RF064
const char *RootBoneName(const geActor *Actor);

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
