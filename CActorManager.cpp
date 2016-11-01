/************************************************************************************//**
 * @file CActorManager.cpp
 * @brief This file contains the class implementation of the Actor Manager system
 * for Reality Factory.
 *
 * It's the Actor Manager's job to load, remove, translate, rotate, and scale
 * all the actors in the system.  The Actor Manager maintains a database of
 * "loaded actors" from which instances can be created.
 ****************************************************************************************/

#include "RabidFramework.h"
#include <Ram.h>
#include "CFileManager.h"
#include "IniFile.h"
#include "CLevel.h"
#include "CPolyShadow.h"
#include "CLiquid.h"

extern "C" void	DrawBoundBox(geWorld *World, const geVec3d *Pos, const geVec3d *Min, const geVec3d *Max);
extern geBitmap *TPool_Bitmap(const char *DefaultBmp, const char *DefaultAlpha,
							  const char *BName, const char *AName);

/* ------------------------------------------------------------------------------------ */
// Default constructor
/* ------------------------------------------------------------------------------------ */
CActorManager::CActorManager()
{
	m_InstanceCount = 0;			// No instances

	m_MaterialIniFile = new CIniFile("material.ini");
	m_ValidIniFile = true;

	if(m_MaterialIniFile == NULL || !m_MaterialIniFile->ReadFile())
		m_ValidIniFile = false;
}

/* ------------------------------------------------------------------------------------ */
// Default destructor.  Massacre everyone on the level.
/* ------------------------------------------------------------------------------------ */
CActorManager::~CActorManager()
{
	stdext::hash_map<std::string, LoadedActorList*>::iterator iter = m_LoadedActorFiles.begin();
	for(; iter!=m_LoadedActorFiles.end(); ++iter)
	{
		RemoveAllActors(iter->second);

		for(int i=0; i<4; ++i)
		{
			if(iter->second->ActorDef[i])
			{
				geActor_DefDestroy(&iter->second->ActorDef[i]);
				iter->second->ActorDef[i] = NULL;
			}

			if(iter->second->Actor[i])
			{
				geActor_Destroy(&iter->second->Actor[i]);
				iter->second->Actor[i] = NULL;
			}
		}

		if(iter->second->Bitmap)
		{
			geWorld_RemoveBitmap(CCD->World(), iter->second->Bitmap);
			geBitmap_Destroy(&iter->second->Bitmap);
			iter->second->Bitmap = NULL;
		}

		delete (iter->second);
	}

	m_LoadedActorFiles.clear();
	m_ActorLookUp.clear();

	delete m_MaterialIniFile;
}

/*
	07/15/2004 Wendell Buckner
	BUG FIX - Bone Collisions fail because we expect to hit the bone immediately after
	hitting the overall bounding box. So tag the actor as being hit at the bounding box
	level and after that check ONLY the bone bounding boxes until the whatever hit the
	overall bounding box no longer exists.
*/
/* ------------------------------------------------------------------------------------ */
// AddCollideObject
/* ------------------------------------------------------------------------------------ */
CollideObjectInformation* CActorManager::AddCollideObject(CollideObjectLevels collideObjectLevel,
														  ActorInstance* actorInstance,
														  CollideObjectInformation* collideObject)
{
	long StartCollideObjectLevel;
	long CurrCollideObjectLevel;
	long EndCollideObjectLevel;
	CollideObjectInformation *NewCollideObject = NULL;

	if(collideObjectLevel < COLActorBBox)	collideObjectLevel = COLActorBBox;
	if(collideObjectLevel > COLMaxBBox)		collideObjectLevel = COLMaxBBox;

	if(collideObjectLevel == COLMaxBBox)
	{
		StartCollideObjectLevel= COLActorBBox;
		EndCollideObjectLevel = COLMaxBBox;
	}
	else
	{
		StartCollideObjectLevel= collideObjectLevel;
		EndCollideObjectLevel = collideObjectLevel + 1;
	}


	if(collideObject)
	{
		stdext::hash_map<std::string, LoadedActorList*>::iterator iter = m_LoadedActorFiles.begin();
		for(; iter!=m_LoadedActorFiles.end(); ++iter)
		{
			ActorInstance *pInstance = NULL;

			if(actorInstance)
				pInstance = actorInstance;
			else
				pInstance = iter->second->IList;

			while(pInstance != NULL)
			{

				if(pInstance->NoCollision && !actorInstance)
				{
					pInstance = pInstance->pNext;
					continue;
				}

				for(CurrCollideObjectLevel=StartCollideObjectLevel;
					CurrCollideObjectLevel<EndCollideObjectLevel;
					++CurrCollideObjectLevel)
				{
					NewCollideObject = static_cast<CollideObjectInformation*>(geRam_Allocate(sizeof(CollideObjectInformation)));

					if(!NewCollideObject)
						break;

					NewCollideObject->CollideObject = collideObject->CollideObject;
					NewCollideObject->Normal  = collideObject->Normal;
					NewCollideObject->Percent = collideObject->Percent;
					NewCollideObject->NextCollideObject = NULL;
					NewCollideObject->PrevCollideObject = NULL;

					if(!actorInstance->CollideObjects[CurrCollideObjectLevel])
					{
						actorInstance->CollideObjects[CurrCollideObjectLevel] = NewCollideObject;
					}
					else
					{
						CollideObjectInformation *LastCollideObject = actorInstance->CollideObjects[CurrCollideObjectLevel];
						while(LastCollideObject->NextCollideObject)
						{
							LastCollideObject = LastCollideObject->NextCollideObject;
						}
						NewCollideObject->PrevCollideObject = LastCollideObject;
						LastCollideObject->NextCollideObject = NewCollideObject;
					}
				}

				if(!NewCollideObject)
					break;

				pInstance = pInstance->pNext;	// Next instance in list
			}

			if(!NewCollideObject)
				break;

			if(actorInstance)
				break;
		}
	}

	return NewCollideObject;
}

/* ------------------------------------------------------------------------------------ */
// RemoveCollideObject
/* ------------------------------------------------------------------------------------ */
geBoolean CActorManager::RemoveCollideObject(CollideObjectLevels collideObjectLevel,
											 ActorInstance* actorInstance,
											 void* collideObject)
{
	long StartCollideObjectLevel;
	long CurrCollideObjectLevel;
	long EndCollideObjectLevel;
	geBoolean CollideObjectRemoved = GE_FALSE;

	if(collideObjectLevel < COLActorBBox)	collideObjectLevel = COLActorBBox;
	if(collideObjectLevel > COLMaxBBox)		collideObjectLevel = COLMaxBBox;

	if(collideObjectLevel == COLMaxBBox)
	{
		StartCollideObjectLevel= COLActorBBox;
		EndCollideObjectLevel = COLMaxBBox;
	}
	else
	{
		StartCollideObjectLevel = collideObjectLevel;
		EndCollideObjectLevel = collideObjectLevel + 1;
	}

	stdext::hash_map<std::string, LoadedActorList*>::iterator iter = m_LoadedActorFiles.begin();
	for(; iter!=m_LoadedActorFiles.end(); ++iter)
	{
		ActorInstance *pInstance = NULL;

		if(actorInstance)
			pInstance = actorInstance;
		else
			pInstance = iter->second->IList;

		while(pInstance != NULL)
		{
			if(pInstance->NoCollision && !actorInstance)
			{
				pInstance = pInstance->pNext;
				continue;
			}

			for(CurrCollideObjectLevel=StartCollideObjectLevel;
				CurrCollideObjectLevel<EndCollideObjectLevel;
				++CurrCollideObjectLevel)
			{
				CollideObjectInformation *FoundCollideObject = NULL;
				FoundCollideObject = GetCollideObject((CollideObjectLevels)CurrCollideObjectLevel,
														pInstance, collideObject);

				if(FoundCollideObject)
				{
					if(FoundCollideObject->PrevCollideObject)
					{
						FoundCollideObject->PrevCollideObject->NextCollideObject = FoundCollideObject->NextCollideObject;
						if(FoundCollideObject->NextCollideObject)
							FoundCollideObject->NextCollideObject->PrevCollideObject = FoundCollideObject->PrevCollideObject;
					}
					else // if it's the 1st element
					{
						pInstance->CollideObjects[CurrCollideObjectLevel] = FoundCollideObject->NextCollideObject;
						if(pInstance->CollideObjects[CurrCollideObjectLevel])
							pInstance->CollideObjects[CurrCollideObjectLevel]->PrevCollideObject = NULL;
					}

					geRam_Free(FoundCollideObject);

					CollideObjectRemoved = GE_TRUE;
				}
			}

			pInstance = pInstance->pNext;	// Next instance in list
		}

		if(actorInstance)
			break;
	}

	return CollideObjectRemoved;
}

/* ------------------------------------------------------------------------------------ */
// GetCollideObject
/* ------------------------------------------------------------------------------------ */
CollideObjectInformation* CActorManager::GetCollideObject(CollideObjectLevels collideObjectLevel,
														  ActorInstance* actorInstance,
														  void* collideObject)
{
	long StartCollideObjectLevel;
	long CurrCollideObjectLevel;
	long EndCollideObjectLevel;
	CollideObjectInformation *FoundCollideObject = NULL;
	geBoolean CollideObjectFound = GE_FALSE;

	if(collideObjectLevel < COLActorBBox)	collideObjectLevel = COLActorBBox;
	if(collideObjectLevel > COLMaxBBox)		collideObjectLevel = COLMaxBBox;


	if(collideObjectLevel == COLMaxBBox)
	{
		StartCollideObjectLevel = COLActorBBox;
		EndCollideObjectLevel = COLMaxBBox;
	}
	else
	{
		StartCollideObjectLevel = collideObjectLevel;
		EndCollideObjectLevel = collideObjectLevel+1;
	}


	stdext::hash_map<std::string, LoadedActorList*>::iterator iter = m_LoadedActorFiles.begin();
	for(; iter!=m_LoadedActorFiles.end(); ++iter)
	{
		ActorInstance *pInstance = NULL;

		if(actorInstance)
			pInstance = actorInstance;
		else
			pInstance = iter->second->IList;

		while(pInstance != NULL)
		{
			if(pInstance->NoCollision && !actorInstance)
			{
				pInstance = pInstance->pNext;
				continue;
			}

			for(CurrCollideObjectLevel=StartCollideObjectLevel;
				CurrCollideObjectLevel<EndCollideObjectLevel;
				++CurrCollideObjectLevel)
			{
				for(FoundCollideObject=pInstance->CollideObjects[CurrCollideObjectLevel];
					FoundCollideObject;
					FoundCollideObject=FoundCollideObject->NextCollideObject)
				{
					CollideObjectFound = (FoundCollideObject->CollideObject == collideObject);

					if(CollideObjectFound)
						break;

					CollideObjectFound = (FoundCollideObject->CollideObject && !collideObject);

					if(CollideObjectFound)
						break;
				}

				if(CollideObjectFound)
					break;
			}

			if(CollideObjectFound)
				break;

			if(actorInstance)
				break;

			pInstance = pInstance->pNext;				// Next instance in list
		}

		if(CollideObjectFound)
			break;

		if(actorInstance)
			break;
	}

	return FoundCollideObject;
}

/* ------------------------------------------------------------------------------------ */
// LoadActor
//
// If the required actor isn't loaded, load it into memory and
// ..make a new instance list for it.  If it's already loaded,
// ..make a new instance of it.  Regardless, return a numeric
// ..handle to the actor instance.
/* ------------------------------------------------------------------------------------ */
geActor *CActorManager::LoadActor(const std::string& filename, geActor* oldActor)
{
	if(m_LoadedActorFiles.find(filename) != m_LoadedActorFiles.end())
	{
		// It's the same, add a new instance of this actor
		++m_InstanceCount;
		return AddNewInstance(m_LoadedActorFiles[filename], oldActor);
	}

	// Guess what? We don't have an entry for it! Make one.
	geVFile *actorFile;

	// Try to open up the desired .act file
	CFileManager::GetSingletonPtr()->OpenRFFile(&actorFile, kActorFile, filename.c_str(), GE_VFILE_OPEN_READONLY);

	geActor_Def *actorDef = NULL;

	if(actorFile)
	{
		// Create a definition of the actor
		actorDef = geActor_DefCreateFromFile(actorFile);
		if(!actorDef)
		{
			geVFile_Close(actorFile);				// Clean up in case of error
			CCD->Log()->Error("File %s - Line %d: Failed to create geActor_Def from file '%s'",
								__FILE__, __LINE__, filename.c_str());
			return RGF_FAILURE;
		}
	}
	else											// File didn't open, error out.
	{
		CCD->Log()->Error("File %s - Line %d: Failed to load actor '%s'",
							__FILE__, __LINE__, filename.c_str());
		return RGF_FAILURE;
	}

	// Close the input file.
	geVFile_Close(actorFile);

	m_LoadedActorFiles[filename] = new LoadedActorList;
	memset(m_LoadedActorFiles[filename], 0, sizeof(LoadedActorList));

	// Ok, set up the master entry in the table...
	m_LoadedActorFiles[filename]->ActorDef[0] = actorDef;
	m_LoadedActorFiles[filename]->Actor[0] = geActor_Create(actorDef);

	geWorld_AddActor(CCD->World(), m_LoadedActorFiles[filename]->Actor[0], 0, 0xffffffff);
	geWorld_RemoveActor(CCD->World(), m_LoadedActorFiles[filename]->Actor[0]);
	m_LoadedActorFiles[filename]->Bitmap = NULL;
	m_LoadedActorFiles[filename]->IList = NULL;

	std::string lodName, name;

	lodName = filename;
	Replace(lodName, ".act", "_LOD");

	for(int j=1; j<4; ++j)
	{
		std::ostringstream oss;
		oss << lodName << j << ".act";
		name = oss.str();

		if(CFileManager::GetSingletonPtr()->FileExist(kActorFile, name.c_str()))
		{
			CFileManager::GetSingletonPtr()->OpenRFFile(&actorFile, kActorFile, name.c_str(), GE_VFILE_OPEN_READONLY);

			if(actorFile)
			{
				actorDef = geActor_DefCreateFromFile(actorFile);

				if(!actorDef)
				{
					geVFile_Close(actorFile);							// Clean up in case of error
					CCD->Log()->Error("File %s - Line %d: Failed to create geActor_Def from file '%s'",
										__FILE__, __LINE__, name.c_str());
					return RGF_FAILURE;
				}

				geVFile_Close(actorFile);
				m_LoadedActorFiles[filename]->ActorDef[j] = actorDef;
				m_LoadedActorFiles[filename]->Actor[j] = geActor_Create(actorDef);
				geWorld_AddActor(CCD->World(), m_LoadedActorFiles[filename]->Actor[j], 0, 0xffffffff);
				geWorld_RemoveActor(CCD->World(), m_LoadedActorFiles[filename]->Actor[j]);
			}
		}
		else
		{
			m_LoadedActorFiles[filename]->ActorDef[j] = NULL;
			m_LoadedActorFiles[filename]->Actor[j] = NULL;
		}
	}

	name = lodName + ".tga";

	if(CFileManager::GetSingletonPtr()->FileExist(kActorFile, name.c_str()))
	{
		CFileManager::GetSingletonPtr()->OpenRFFile(&actorFile, kActorFile, name.c_str(), GE_VFILE_OPEN_READONLY);

		if(actorFile)
		{
			m_LoadedActorFiles[filename]->Bitmap = geBitmap_CreateFromFile(actorFile);

			if(m_LoadedActorFiles[filename]->Bitmap)
			{
				geBitmap_SetPreferredFormat(m_LoadedActorFiles[filename]->Bitmap, GE_PIXELFORMAT_32BIT_ARGB);
				geWorld_AddBitmap(CCD->World(), m_LoadedActorFiles[filename]->Bitmap);
			}

			geVFile_Close(actorFile);
		}
	}

	// Now we need to create an initial instance
	++m_InstanceCount;
	return AddNewInstance(m_LoadedActorFiles[filename], NULL);
}

/* ------------------------------------------------------------------------------------ */
// SpawnActor
/* ------------------------------------------------------------------------------------ */
geActor *CActorManager::SpawnActor(const std::string&	filename,
								   const geVec3d&		position,
								   const geVec3d&		rotation,
								   const std::string&	defaultMotion,
								   const std::string&	currentMotion,
								   geActor*				oldActor)
{
	// Try and load it...
	if(filename.empty())
	{
		CCD->Log()->Warning("File %s - Line %d: Null Actor Name, Failed To Load!",
							__FILE__, __LINE__);
		return NULL;
	}

	geActor *actor = LoadActor(filename, oldActor);

	if(actor != NULL)
	{
		// It worked!  Configure the new actor
		SetAligningRotation(actor, rotation);		// Rotate the actor
		geVec3d localRotation = {0.0f, 0.0f, 0.0f};
		Rotate(actor, localRotation);
		Position(actor, position);					// Position the actor
		SetMotion(actor, currentMotion);			// Set actor motion
		SetDefaultMotion(actor, defaultMotion);		// Set default motion
	}
	else
	{
		CCD->Log()->Warning("Failed to load Actor " + filename);
	}

	return actor;
}

/* ------------------------------------------------------------------------------------ */
// ChangeMaterial
/* ------------------------------------------------------------------------------------ */
int CActorManager::ChangeMaterial(const geActor *actor, const std::string& change)
{
	if(!m_ValidIniFile)
		return RGF_NOT_FOUND;

	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;

	std::string keyName = m_MaterialIniFile->FindFirstKey();
	std::string type, value;

	while(!keyName.empty())
	{
		if(keyName == change)
		{
			geBody		*body;
			int32		i;
			int32		materialCount;
			geBitmap	*bitmap = NULL;
			float		r=0.f, g=0.f, b=0.f;
			const char	*materialName;

			body = geActor_GetBody(pInstance->theDef);
			materialCount = geActor_GetMaterialCount(pInstance->Actor);

			type = m_MaterialIniFile->FindFirstName(keyName);
			value = m_MaterialIniFile->FindFirstValue();

			while(!type.empty())
			{
				bool found = false;

				for(i=0; i<materialCount; ++i)
				{
					geBody_GetMaterial(body, i, &materialName, &bitmap, &r, &g, &b);

					if(type == materialName)
					{
						found = true;
						break;
					}
				}

				if(!found)
					return RGF_FAILURE;

				int firstIndex = i;
				found = false;

				for(i=0; i<materialCount; ++i)
				{
					geBody_GetMaterial(body, i, &materialName, &bitmap, &r, &g, &b);

					if(value == materialName)
					{
						found = true;
						break;
					}
				}

				if(!found)
					return RGF_FAILURE;

				geActor_SetMaterial(pInstance->Actor, firstIndex, bitmap, r, g, b);

				type = m_MaterialIniFile->FindNextName();
				value = m_MaterialIniFile->FindNextValue();
			}

			return RGF_SUCCESS;
		}

		keyName = m_MaterialIniFile->FindNextKey();
	}

	return RGF_FAILURE;
}

/* ------------------------------------------------------------------------------------ */
// RemoveActor
//
// Remove the actor instance referred to by the given handle.
// ..This takes it out of the game IMMEDIATELY.
/* ------------------------------------------------------------------------------------ */
int CActorManager::RemoveActor(const geActor* actor)
{
	return RemoveInstance(actor);
}

/* ------------------------------------------------------------------------------------ */
// RemoveActorCheck
/* ------------------------------------------------------------------------------------ */
int CActorManager::RemoveActorCheck(const geActor* actor)
{
	if(CCD->Level()->ExplosionManager())
		CCD->Level()->ExplosionManager()->UnAttach(actor);
	return RemoveInstance(actor);
}

/* ------------------------------------------------------------------------------------ */
// SetAligningRotation
//
// Set the rotation necessary to align actor with world
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetAligningRotation(const geActor* actor, const geVec3d& rotation)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	if(pInstance->AttachedB)
		return RGF_SUCCESS;

	pInstance->BaseRotation = rotation;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// GetAligningRotation
//
// Get the alignment rotation for this actor
/* ------------------------------------------------------------------------------------ */
int CActorManager::GetAligningRotation(const geActor* actor, geVec3d* rotation)
{
	assert(rotation);

	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	*rotation = pInstance->BaseRotation;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetType
//
// Sets the TYPE of an actor (prop, vehicle, NPC, etc.)
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetType(const geActor* actor, int type)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	pInstance->ActorType = type;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// GetType
//
// Gets the TYPE of an actor (prop, vehicle, NPC, etc.)
/* ------------------------------------------------------------------------------------ */
int CActorManager::GetType(const geActor* actor, int* type)
{
	assert(type);

	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	*type = pInstance->ActorType;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// Rotate
//
// Assign an absolute rotation to an actor instance
/* ------------------------------------------------------------------------------------ */
int CActorManager::Rotate(const geActor* actor, const geVec3d& rotation)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	if(pInstance->AttachedB)
		return RGF_SUCCESS;

	if(pInstance->Attached)
	{
		geVec3d_Subtract(&rotation, &pInstance->MasterRotation, &pInstance->localRotation);
	}
	else
	{
		pInstance->localRotation = rotation;
	}

	UpdateActorState(pInstance);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// Position
//
// Assign an absolute position to an actor instance
/* ------------------------------------------------------------------------------------ */
int CActorManager::Position(const geActor* actor, const geVec3d& position)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	if(pInstance->AttachedB)
		return RGF_SUCCESS;

	pInstance->OldTranslation = pInstance->localTranslation;

	pInstance->localTranslation = position;

	UpdateActorState(pInstance);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// MoveAway
//
// Move an actor to a position totally outside any possible world.
/* ------------------------------------------------------------------------------------ */
int CActorManager::MoveAway(const geActor* actor)
{
	geVec3d position = {10000.0f, 10000.0f, 10000.0f};
	Position(actor, position);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// GetPosition
//
// Get the current position of a specific actor
/* ------------------------------------------------------------------------------------ */
int CActorManager::GetPosition(const geActor* actor, geVec3d* position)
{
	assert(position);

	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	*position = pInstance->localTranslation;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// GetRotation
//
// Get the current rotation for a specific actor
/* ------------------------------------------------------------------------------------ */
int CActorManager::GetRotation(const geActor* actor, geVec3d* rotation)
{
	assert(rotation);

	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	geVec3d real = pInstance->localRotation;

	if(pInstance->Attached)
		geVec3d_Add(&real, &pInstance->MasterRotation, &real);

	*rotation = real;
	rotation->X += pInstance->BaseRotation.X;
	rotation->Y += pInstance->BaseRotation.Y;
	rotation->Z += pInstance->BaseRotation.Z;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// GetRotate
/* ------------------------------------------------------------------------------------ */
int CActorManager::GetRotate(const geActor* actor, geVec3d* rotation)
{
	assert(rotation);

	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	geVec3d real;
	geVec3d_Copy(&pInstance->localRotation, &real);

	if(pInstance->Attached)
		geVec3d_Add(&real, &pInstance->MasterRotation, &real);

	geVec3d_Copy(&real, rotation);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// TurnLeft
//
// Rotate actor to turn to the LEFT
/* ------------------------------------------------------------------------------------ */
int CActorManager::TurnLeft(const geActor* actor, float amount)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	if(pInstance->AttachedB)
		return RGF_SUCCESS;

	pInstance->localRotation.Y -= amount;

	UpdateActorState(pInstance);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// TurnRight
//
// Rotate actor to turn to the RIGHT
/* ------------------------------------------------------------------------------------ */
int CActorManager::TurnRight(const geActor* actor, float amount)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	if(pInstance->AttachedB)
		return RGF_SUCCESS;

	pInstance->localRotation.Y += amount;

	UpdateActorState(pInstance);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// TiltUp
//
// Rotate actor to tilt up
/* ------------------------------------------------------------------------------------ */
int CActorManager::TiltUp(const geActor* actor, float amount)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	if(pInstance->AttachedB)
		return RGF_SUCCESS;

	if(pInstance->AllowTilt)
		pInstance->localRotation.X += amount;
	else
		pInstance->TiltX += amount;

	UpdateActorState(pInstance);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// TiltDown
//
// Rotate actor to tilt down
/* ------------------------------------------------------------------------------------ */
int CActorManager::TiltDown(const geActor* actor, float amount)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	if(pInstance->AttachedB)
		return RGF_SUCCESS;

	if(pInstance->AllowTilt)
		pInstance->localRotation.X -= amount;
	else
		pInstance->TiltX -= amount;

	UpdateActorState(pInstance);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// GetTiltX
/* ------------------------------------------------------------------------------------ */
float CActorManager::GetTiltX(const geActor* actor)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return 0.0f;					// Actor not found?!?!

	return pInstance->TiltX;
}

/* ------------------------------------------------------------------------------------ */
// ReallyFall
/* ------------------------------------------------------------------------------------ */
int CActorManager::ReallyFall(const geActor* actor)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;

	//if(pInstance->GravityTime != 0.0f)
	//	return RGF_SUCCESS;

	geVec3d startPos, endPos;
	GE_Collision collision;

	startPos = endPos = pInstance->localTranslation;
	endPos.Y -= pInstance->MaxStepHeight;

	CCD->Collision()->IgnoreContents(false);
	CCD->Collision()->CheckLevel(RGF_COLLISIONLEVEL_1); // RGF_COLLISIONLEVEL_2

	// Start off with the infamous Collision Check.
	bool result = CCD->Collision()->CheckActorCollision(startPos, endPos, pInstance->Actor, &collision);

	if(result)
		return RGF_FAILURE;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// CheckReallyFall
/* ------------------------------------------------------------------------------------ */
int CActorManager::CheckReallyFall(const geActor* actor, const geVec3d& startPos)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;

	geVec3d endPos;
	GE_Collision collision;

	endPos = startPos;
	endPos.Y -= pInstance->MaxStepHeight;

	CCD->Collision()->IgnoreContents(false);
	CCD->Collision()->CheckLevel(RGF_COLLISIONLEVEL_1); // RGF_COLLISIONLEVEL_2

	// Start off with the infamous Collision Check.
	bool result = CCD->Collision()->CheckActorCollision(startPos, endPos, pInstance->Actor, &collision);

	if(result)
		return RGF_FAILURE;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// UpVector
//
// Return the UP vector for a specified actor
/* ------------------------------------------------------------------------------------ */
int CActorManager::UpVector(const geActor* actor, geVec3d* upVector)
{
	assert(upVector);

	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	geXForm3d xForm;

	// Perform orienting rotations to properly align model
	geVec3d rotation = pInstance->localRotation;

	geXForm3d_SetZRotation(&xForm, rotation.Z);
	geXForm3d_RotateX(&xForm, rotation.X);
	geXForm3d_RotateY(&xForm, rotation.Y);

	geXForm3d_GetUp(&xForm, upVector);	// Up... yours?

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// InVector
//
// Return the IN vector for a specified actor
/* ------------------------------------------------------------------------------------ */
int CActorManager::InVector(const geActor* actor, geVec3d* inVector)
{
	assert(inVector);

	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	geXForm3d xForm;

	// Perform orienting rotations to properly align model
	geVec3d rotation = pInstance->localRotation;

	geXForm3d_SetZRotation(&xForm, rotation.Z);
	geXForm3d_RotateX(&xForm, rotation.X);
	geXForm3d_RotateY(&xForm, rotation.Y);

	geXForm3d_GetIn(&xForm, inVector);		// In & out

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// LeftVector
//
// Return the LEFT vector for a specified actor
/* ------------------------------------------------------------------------------------ */
int CActorManager::LeftVector(const geActor* actor, geVec3d* leftVector)
{
	assert(leftVector);

	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;						// Actor not found?!?!

	geXForm3d xForm;

	// Perform orienting rotations to properly align model
	geVec3d rotation = pInstance->localRotation;

	geXForm3d_SetZRotation(&xForm, rotation.Z);
	geXForm3d_RotateX(&xForm, rotation.X);
	geXForm3d_RotateY(&xForm, rotation.Y);

	geXForm3d_GetLeft(&xForm, leftVector);	// Left wing party

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// GetBonePosition
//
// Return the translation of a bone in an actor
/* ------------------------------------------------------------------------------------ */
int CActorManager::GetBonePosition(const geActor* actor, const std::string& bone, geVec3d* position)
{
	assert(position);

	geXForm3d boneXForm;

	if(geActor_GetBoneTransform(actor, bone.c_str(), &boneXForm) != GE_TRUE)
		return RGF_NOT_FOUND;	// No such bone

	*position = boneXForm.Translation;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// GetBoneRotation
//
// Return the rotation of a bone in an actor
/* ------------------------------------------------------------------------------------ */
int CActorManager::GetBoneRotation(const geActor* actor, const std::string& bone, geVec3d* rotation)
{
	assert(rotation);

	geXForm3d boneXForm;

	if(!bone.empty())
	{
		if(geActor_GetBoneTransform(actor, bone.c_str(), &boneXForm) != GE_TRUE)
			return RGF_NOT_FOUND;						// No such bone
	}
	else
	{
		if(geActor_GetBoneTransform(actor, RootBoneName(actor), &boneXForm) != GE_TRUE)
			return RGF_NOT_FOUND;						// No such bone
	}

	geXForm3d_GetEulerAngles(&boneXForm, rotation);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// AddTranslation
//
// Add translation to actors current position
/* ------------------------------------------------------------------------------------ */
int CActorManager::AddTranslation(const geActor* actor, const geVec3d& amount)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	if(pInstance->AttachedB)
		return RGF_SUCCESS;

	pInstance->OldTranslation = pInstance->localTranslation;

	pInstance->localTranslation.X += amount.X;
	pInstance->localTranslation.Y += amount.Y;
	pInstance->localTranslation.Z += amount.Z;

	UpdateActorState(pInstance);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// AddRotation
//
// Add rotation to actors current rotation
/* ------------------------------------------------------------------------------------ */
int CActorManager::AddRotation(const geActor* actor, const geVec3d& amount)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	if(pInstance->AttachedB)
		return RGF_SUCCESS;

	pInstance->localRotation.X += amount.X;
	pInstance->localRotation.Y += amount.Y;
	pInstance->localRotation.Z += amount.Z;

	UpdateActorState(pInstance);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// RotateToFacePoint
//
// Rotate the actor to face a specific point
/* ------------------------------------------------------------------------------------ */
int CActorManager::RotateToFacePoint(const geActor* actor, const geVec3d& position)
{
	geVec3d lookAt, lookFrom;
	geVec3d lookRotation;
	float length;

	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	if(pInstance->AttachedB)
		return RGF_SUCCESS;

	lookAt = position;
	lookFrom = pInstance->localTranslation;

	geVec3d_Subtract(&lookAt, &lookFrom, &lookRotation);

	length = geVec3d_Length(&lookRotation);

	// protect from Div by Zero
	if(length > 0.0f)
	{
		float x = lookRotation.X;

		lookRotation.X = GE_PIOVER2 - acos(lookRotation.Y / length);
		lookRotation.Y = atan2(x, lookRotation.Z) + GE_PI;
		lookRotation.Z = 0.0f;	// roll is zero - always!!?
	}

	pInstance->localRotation.X = lookRotation.X;
	pInstance->localRotation.Y = lookRotation.Y;
	pInstance->localRotation.Z = lookRotation.Z;

	UpdateActorState(pInstance);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetAutoStepUp
//
// Enable or disable auto step up for this actor
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetAutoStepUp(const geActor* actor, bool enable)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;		// Actor not found?!?!

	pInstance->AutoStepUp = enable;	// Set it up!

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetStepHeight
//
// Set the step-up height for this actor
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetStepHeight(const geActor* actor, float maxStep)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	pInstance->MaxStepHeight = maxStep;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// GetStepHeight
//
// Get the step-up height for this actor
/* ------------------------------------------------------------------------------------ */
int CActorManager::GetStepHeight(const geActor* actor, float* maxStep)
{
	assert(maxStep);

	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	*maxStep = pInstance->MaxStepHeight;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// MoveForward
//
// Move the actor forward at some specific velocity
/* ------------------------------------------------------------------------------------ */
int CActorManager::MoveForward(const geActor* actor, float speed)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	return Move(pInstance, RGF_K_FORWARD, speed);
}

/* ------------------------------------------------------------------------------------ */
// MoveBackward
//
// Move the actor backward at some specific velocity
/* ------------------------------------------------------------------------------------ */
int CActorManager::MoveBackward(const geActor* actor, float speed)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	return Move(pInstance, RGF_K_BACKWARD, speed);
}

/* ------------------------------------------------------------------------------------ */
// MoveLeft
//
// Move the actor left at some specific velocity
/* ------------------------------------------------------------------------------------ */
int CActorManager::MoveLeft(const geActor* actor, float speed)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	return Move(pInstance, RGF_K_LEFT, speed);
}

/* ------------------------------------------------------------------------------------ */
// MoveRight
//
// Move the actor right at some specific velocity
/* ------------------------------------------------------------------------------------ */
int CActorManager::MoveRight(const geActor* actor, float speed)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	return Move(pInstance, RGF_K_RIGHT, speed);
}

/* ------------------------------------------------------------------------------------ */
// SetAnimationSpeed
//
// Set the speed at which the actors animation will run, in percent
// (1.0 is 100% normal speed).
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetAnimationSpeed(const geActor* actor, float speed)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	pInstance->ActorAnimationSpeed = speed;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetBoxChange
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetBoxChange(const geActor* actor, bool flag)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	pInstance->BoxChange = flag;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetMotion
//
// Assign a new motion to an actor instance.  This replaces
// ..the current animation on the next time cycle.
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetMotion(const geActor* actor, const std::string& motionName, int /*channel*/)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	pInstance->MotionName = motionName;

	pInstance->AnimationTime = 0.0f;			// Clear animation timing
	pInstance->NeedsNewBB = true;
	pInstance->BlendFlag = false;
	pInstance->TransitionFlag = false;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetNextMotion
//
// Assign motion to switch to when the current animation cycle is complete.
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetNextMotion(const geActor* actor, const std::string& motionName, int /*channel*/)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	pInstance->NextMotionName = motionName;

	pInstance->BlendNextFlag = false;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetDefaultMotion
//
// Assign the "default motion" for an actor
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetDefaultMotion(const geActor* actor, const std::string& motionName, int /*channel*/)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	pInstance->DefaultMotionName = motionName;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// ClearMotionToDefault
//
// Clear all current and pending motions for an actor and set the
// ..current motion to the default.
/* ------------------------------------------------------------------------------------ */
int CActorManager::ClearMotionToDefault(const geActor* actor, int /*channel*/)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	pInstance->MotionName = pInstance->DefaultMotionName;

	pInstance->AnimationTime = 0.0f;			// Clear animation timer

	pInstance->NeedsNewBB = true;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetActorDynamicLighting
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetActorDynamicLighting(const geActor* actor,
										   const GE_RGBA& fillColor,
										   const GE_RGBA& ambientColor,
										   geBoolean ambientLightFromFloor)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	pInstance->FillColor = fillColor;
	pInstance->AmbientColor = ambientColor;
	pInstance->AmbientLightFromFloor = ambientLightFromFloor;

	geVec3d fill = {0.0f, 1.0f, 0.0f};
	geXForm3d xForm;
	geXForm3d xFormT;
	geVec3d newFillNormal;
	geActor_GetBoneTransform(actor, RootBoneName(actor), &xForm);
	geXForm3d_GetTranspose(&xForm, &xFormT);
	geXForm3d_Rotate(&xFormT, &fill, &newFillNormal);

	geActor_SetLightingOptions(pInstance->Actor, GE_TRUE, &newFillNormal, fillColor.r, fillColor.g, fillColor.b,
						ambientColor.r, ambientColor.g, ambientColor.b, ambientLightFromFloor, 6, NULL, GE_FALSE);

	for(int i=0; i<3; ++i)
	{
		if(pInstance->LODActor[i])
			geActor_SetLightingOptions(pInstance->LODActor[i], GE_TRUE, &newFillNormal, fillColor.r, fillColor.g, fillColor.b,
						ambientColor.r, ambientColor.g, ambientColor.b, ambientLightFromFloor, 6, NULL, GE_FALSE);
	}

	// also the geActor_SetStaticLightingOptions must be updated, because both - dynamic and static - AmbientLightFromFloor options
	// must be set to GE_FALSE to make the engine use the specified AmbientColor
	geActor_SetStaticLightingOptions(pInstance->Actor, ambientLightFromFloor, GE_TRUE, 6);

	for(int j=0; j<3; ++j)
	{
		if(pInstance->LODActor[j])
			geActor_SetStaticLightingOptions(pInstance->LODActor[j], ambientLightFromFloor, GE_TRUE, 6);
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// GetActorDynamicLighting
/* ------------------------------------------------------------------------------------ */
int CActorManager::GetActorDynamicLighting(const geActor* actor,
										   GE_RGBA* fillColor,
										   GE_RGBA* ambientColor,
										   geBoolean* ambientLightFromFloor)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	if(fillColor)
		*fillColor = pInstance->FillColor;

	if(ambientColor)
		*ambientColor = pInstance->AmbientColor;

	if(ambientLightFromFloor)
		*ambientLightFromFloor = pInstance->AmbientLightFromFloor;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// ResetActorDynamicLighting
/* ------------------------------------------------------------------------------------ */
int CActorManager::ResetActorDynamicLighting(const geActor* actor)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	geVec3d fill = {0.0f, 1.0f, 0.0f};
	geXForm3d xForm;
	geXForm3d xFormT;
	geVec3d newFillNormal;
	geActor_GetBoneTransform(actor, RootBoneName(actor), &xForm);
	geXForm3d_GetTranspose(&xForm, &xFormT);
	geXForm3d_Rotate(&xFormT, &fill, &newFillNormal);

	geActor_SetLightingOptions(pInstance->Actor, GE_TRUE, &newFillNormal, pInstance->FillColor.r, pInstance->FillColor.g, pInstance->FillColor.b,
						pInstance->AmbientColor.r, pInstance->AmbientColor.g, pInstance->AmbientColor.b, pInstance->AmbientLightFromFloor, 6, NULL, GE_FALSE);

	for(int i=0; i<3; ++i)
	{
		if(pInstance->LODActor[i])
			geActor_SetLightingOptions(pInstance->LODActor[i], GE_TRUE, &newFillNormal, pInstance->FillColor.r, pInstance->FillColor.g, pInstance->FillColor.b,
						pInstance->AmbientColor.r, pInstance->AmbientColor.g, pInstance->AmbientColor.b, pInstance->AmbientLightFromFloor, 6, NULL, GE_FALSE);
	}

	// also the geActor_SetStaticLightingOptions must be updated, because both - dynamic and static - AmbientLightFromFloor options
	// must be set to GE_FALSE to make the engine use the specified AmbientColor
	geActor_SetStaticLightingOptions(pInstance->Actor, pInstance->AmbientLightFromFloor, GE_TRUE, 6);

	for(int j=0; j<3; ++j)
	{
		if(pInstance->LODActor[j])
			geActor_SetStaticLightingOptions(pInstance->LODActor[j], pInstance->AmbientLightFromFloor, GE_TRUE, 6);
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetForce
//
// Assign a force to one of the four force slots to affect the actor.
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetForce(const geActor* actor, int forceNumber, const geVec3d& forceVector,
							float forceLevel, float forceDecay)
{
	if((forceNumber < 0) || (forceNumber > 3))
		return RGF_FAILURE;						// Bad argument

	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	if(pInstance->AttachedB)
		return RGF_SUCCESS;

	pInstance->ForceVector[forceNumber] = forceVector;
	pInstance->ForceLevel[forceNumber] = forceLevel;
	pInstance->ForceDecay[forceNumber] = forceDecay;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// GetForce
/* ------------------------------------------------------------------------------------ */
int CActorManager::GetForce(const geActor* actor, int forceNumber, geVec3d* forceVector,
							float* forceLevel, float* forceDecay)
{
	if((forceNumber < 0) || (forceNumber > 3))
		return RGF_FAILURE;						// Bad argument

	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	if(forceVector)
		*forceVector = pInstance->ForceVector[forceNumber];
	if(forceLevel)
		*forceLevel = pInstance->ForceLevel[forceNumber];
	if(forceDecay)
		*forceDecay = pInstance->ForceDecay[forceNumber];

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// RemoveForce
//
// Remove a force from an actor
/* ------------------------------------------------------------------------------------ */
int CActorManager::RemoveForce(const geActor* actor, int forceNumber)
{
	if((forceNumber < 0) || (forceNumber > 3))
		return RGF_FAILURE;						// Bad argument

	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	pInstance->ForceLevel[forceNumber] = 0.0f;
	pInstance->ForceDecay[forceNumber] = 0.0f;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// ForceActive
//
// Returns GE_TRUE if the given force is active, GE_FALSE otherwise.
/* ------------------------------------------------------------------------------------ */
geBoolean CActorManager::ForceActive(const geActor* actor, int forceNumber)
{
	if((forceNumber < 0) || (forceNumber > 3))
		return RGF_FAILURE;							// Bad argument

	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;						// Actor not found?!?!

	if(!(pInstance->ForceEnabled))
		return GE_FALSE;

	if(pInstance->ForceLevel[forceNumber] > 0.0f)
		return GE_TRUE;								// Force is active

	return GE_FALSE;								// Force inactive
}

/* ------------------------------------------------------------------------------------ */
// SetForceEnable
/* ------------------------------------------------------------------------------------ */
void CActorManager::SetForceEnabled(const geActor* actor, bool enable)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance)
		pInstance->ForceEnabled = enable;
}

/* ------------------------------------------------------------------------------------ */
// SetColDetLevel
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetColDetLevel(const geActor* actor, int colDetLevel)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	pInstance->CollDetLevel = colDetLevel;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// GetColDetLevel
/* ------------------------------------------------------------------------------------ */
int CActorManager::GetColDetLevel(const geActor* actor, int* colDetLevel)
{
	assert(colDetLevel);

	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	*colDetLevel = pInstance->CollDetLevel;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetBlendMot
//
// Description:  Creates a custom pose based on 2 animations,
// ..does NOT reset the animation timing
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetBlendMot(const geActor* actor,
							   const std::string& name,
							   const std::string& blendName,
							   float amount,
							   int /*channel*/)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	pInstance->MotionName = name;
	pInstance->BlendMotionName = blendName;

	pInstance->BlendAmount = amount;
	pInstance->NeedsNewBB = true;
	pInstance->BlendFlag = true;
	pInstance->TransitionFlag = false;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetBlendMotion
//
// Description:  Creates a custom pose based on 2 animations,
// ..first RESETS the animation timing
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetBlendMotion(const geActor* actor,
								  const std::string& name,
								  const std::string& blendName,
								  float amount,
								  int /*channel*/)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	pInstance->MotionName = name;
	pInstance->BlendMotionName = blendName;

	pInstance->BlendAmount = amount;
	pInstance->AnimationTime = 0.0f;			// Clear animation timing
	pInstance->NeedsNewBB = true;
	pInstance->BlendFlag = true;
	pInstance->TransitionFlag = false;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetBlendNextMotion
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetBlendNextMotion(const geActor* actor,
									  const std::string& name,
									  const std::string& blendName,
									  float amount,
									  int /*channel*/)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	pInstance->NextMotionName = name;
	pInstance->BlendNextMotionName = blendName;

	pInstance->BlendNextAmount = amount;
	pInstance->BlendNextFlag = true;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetTransitionMotion
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetTransitionMotion(const geActor* actor,
									   const std::string& name1,
									   float amount,
									   const std::string& name2,
									   int /*channel*/)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	if(amount <= 0.0f)
	{
		if(!name2.empty())
		{
			SetMotion(actor, name2);
			SetNextMotion(actor, name1);
		}
		else
		{
			SetMotion(actor, name1);
		}
	}
	else
	{
		pInstance->BlendMotionName = pInstance->MotionName;
		pInstance->MotionName = name1;

		pInstance->BlendAmount = amount;
		pInstance->StartTime = pInstance->PrevAnimationTime;
		pInstance->AnimationTime = 0.0f;		// Clear animation timing
		pInstance->NeedsNewBB = true;
		pInstance->TransitionFlag = true;
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// CheckTransitionMotion
/* ------------------------------------------------------------------------------------ */
bool CActorManager::CheckTransitionMotion(const geActor* actor, const std::string& name, int /*channel*/)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return false;							// Actor not found?!?!

	if(pInstance->TransitionFlag)
	{
		if(pInstance->BlendMotionName == name)
			return true;
	}

	return false;
}

/* ------------------------------------------------------------------------------------ */
// SetActorFlags
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetActorFlags(const geActor* actor, int flag)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return false;							// Actor not found?!?!

	pInstance->RenderFlag = flag;
	geWorld_SetActorFlags(CCD->World(), pInstance->Actor, flag);
	return true;
}

/* ------------------------------------------------------------------------------------ */
// GetGravityCollision
//
// Get GravityCollision structure
/* ------------------------------------------------------------------------------------ */
int CActorManager::GetGravityCollision(const geActor* actor, GE_Collision* collision)
{
	assert(collision);

	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	*collision = pInstance->GravityCollision;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetHideRadar
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetHideRadar(const geActor* actor, bool flag)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	pInstance->HideRadar = flag;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// GetHideRadar
/* ------------------------------------------------------------------------------------ */
int CActorManager::GetHideRadar(const geActor* actor, bool* flag)
{
	assert(flag);

	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	*flag = pInstance->HideRadar;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetGroup
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetGroup(const geActor* actor, const std::string& name)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	pInstance->Group = name;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// GetGroup
/* ------------------------------------------------------------------------------------ */
std::string CActorManager::GetGroup(const geActor* actor)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return "";								// Actor not found?!?!

	return pInstance->Group;
}

/* ------------------------------------------------------------------------------------ */
// GetByEntityName
//
// Get the actor by its EntityName
/* ------------------------------------------------------------------------------------ */
geActor* CActorManager::GetByEntityName(const char* name)
{
	stdext::hash_map<std::string, LoadedActorList*>::iterator iter = m_LoadedActorFiles.begin();
	for(; iter!=m_LoadedActorFiles.end(); ++iter)
	{
		ActorInstance *pInstance = iter->second->IList;
		while(pInstance != NULL)
		{
			if(pInstance->Actor)
			{
				if(!pInstance->szEntityName.empty())
				{
					if(pInstance->szEntityName == name)
					{
						return pInstance->Actor;
					}
				}
			}

			pInstance = pInstance->pNext;
		}
	}

	return NULL;
}

/* ------------------------------------------------------------------------------------ */
// SetEntityName
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetEntityName(const geActor* actor, const std::string& name)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	pInstance->szEntityName = name;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// GetEntityName
/* ------------------------------------------------------------------------------------ */
std::string CActorManager::GetEntityName(const geActor* actor)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return "";								// Actor not found?!?!

	return pInstance->szEntityName;
}

/* ------------------------------------------------------------------------------------ */
// SetShadow
//
// Set the shadow of an actor
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetShadow(const geActor* actor, float size)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	if(size > 0.0f)
		pInstance->ShadowSize = size;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetShadowAlpha
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetShadowAlpha(const geActor* actor, float alpha)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;

	pInstance->ShadowAlpha = alpha;
	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetShadowBitmap
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetShadowBitmap(const geActor* actor, geBitmap* bitmap)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;

	pInstance->ShadowBitmap = bitmap;
	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetProjectedShadows
//
// Activate projected shadows for this actor
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetProjectedShadows(const geActor* actor, bool flag)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;

	pInstance->ProjectedShadows = flag;
	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetStencilShadows
//
// Activate stencil shadows for this actor
/* ------------------------------------------------------------------------------------ */
geBoolean CActorManager::SetStencilShadows(const geActor* actor, geBoolean flag)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return GE_FALSE;

	return geActor_SetStencilShadow(pInstance->Actor, flag);
}

/* ------------------------------------------------------------------------------------ */
// SetScale
//
// Set the scale of an actor (x,y,z get the same value)
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetScale(const geActor* actor, float scale)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;									// Actor not found?!?!

	if(pInstance->AttachedB)
		return RGF_SUCCESS;

	geVec3d_Set(&pInstance->Scale, scale, scale, scale);

	geActor_SetScale(pInstance->Actor, scale, scale, scale);	// Scale the actor

	for(int i=0; i<3; ++i)
	{
		if(pInstance->LODActor[i])
			geActor_SetScale(pInstance->LODActor[i], scale, scale, scale);
	}

	// update attached blending actors
	std::vector<geActor*>::iterator iter = pInstance->AttachedBlendActors.begin();
	for(; iter!=pInstance->AttachedBlendActors.end(); ++iter)
	{
		ActorInstance *sInstance = LocateInstance(*iter);
		if(sInstance)
		{
			geActor_SetScale(sInstance->Actor, scale, scale, scale);
		}
	}

	UpdateActorState(pInstance);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// GetScale
//
// Return an actors current scale (max of x,y,z scale)
/* ------------------------------------------------------------------------------------ */
int CActorManager::GetScale(const geActor* actor, float* scale)
{
	assert(scale);

	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	if(pInstance->Scale.X > pInstance->Scale.Y)
	{
		if(pInstance->Scale.X > pInstance->Scale.Z)
			*scale = pInstance->Scale.X;
		else
			*scale = pInstance->Scale.Z;
	}
	else
	{
		if(pInstance->Scale.Y > pInstance->Scale.Z)
			*scale = pInstance->Scale.Y;
		else
			*scale = pInstance->Scale.Z;
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetScaleX
//
// Set the x-scale of an actor
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetScaleX(const geActor* actor, float scale)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	if(pInstance->AttachedB)
		return RGF_SUCCESS;

	pInstance->Scale.X = scale;

	// Scale the actor
	geActor_SetScale(pInstance->Actor, pInstance->Scale.X, pInstance->Scale.Y, pInstance->Scale.Z);

	for(int i=0; i<3; ++i)
	{
		if(pInstance->LODActor[i])
			geActor_SetScale(pInstance->LODActor[i], pInstance->Scale.X, pInstance->Scale.Y, pInstance->Scale.Z);
	}

	// update attached blending actors
	std::vector<geActor*>::iterator iter = pInstance->AttachedBlendActors.begin();
	for(; iter!=pInstance->AttachedBlendActors.end(); ++iter)
	{
		ActorInstance *sInstance = LocateInstance(*iter);
		if(sInstance)
		{
			geActor_SetScale(sInstance->Actor, pInstance->Scale.X, pInstance->Scale.Y, pInstance->Scale.Z);
		}
	}

	UpdateActorState(pInstance);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetScaleY
//
// Set the y-scale of an actor
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetScaleY(const geActor* actor, float scale)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	if(pInstance->AttachedB)
		return RGF_SUCCESS;

	pInstance->Scale.Y = scale;

	// Scale the actor
	geActor_SetScale(pInstance->Actor, pInstance->Scale.X, pInstance->Scale.Y, pInstance->Scale.Z);

	for(int i=0; i<3; ++i)
	{
		if(pInstance->LODActor[i])
			geActor_SetScale(pInstance->LODActor[i], pInstance->Scale.X, pInstance->Scale.Y, pInstance->Scale.Z);
	}

	// update attached blending actors
	std::vector<geActor*>::iterator iter = pInstance->AttachedBlendActors.begin();
	for(; iter!=pInstance->AttachedBlendActors.end(); ++iter)
	{
		ActorInstance *sInstance = LocateInstance(*iter);
		if(sInstance)
		{
			geActor_SetScale(sInstance->Actor, pInstance->Scale.X, pInstance->Scale.Y, pInstance->Scale.Z);
		}
	}

	UpdateActorState(pInstance);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetScaleZ
//
// Set the z-scale of an actor
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetScaleZ(const geActor* actor, float scale)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	if(pInstance->AttachedB)
		return RGF_SUCCESS;

	pInstance->Scale.Z = scale;

	// Scale the actor
	geActor_SetScale(pInstance->Actor, pInstance->Scale.X, pInstance->Scale.Y, pInstance->Scale.Z);

	for(int i=0; i<3; ++i)
	{
		if(pInstance->LODActor[i])
			geActor_SetScale(pInstance->LODActor[i], pInstance->Scale.X, pInstance->Scale.Y, pInstance->Scale.Z);
	}

	// update attached blending actors
	std::vector<geActor*>::iterator iter = pInstance->AttachedBlendActors.begin();
	for(; iter!=pInstance->AttachedBlendActors.end(); ++iter)
	{
		ActorInstance *sInstance = LocateInstance(*iter);
		if(sInstance)
		{
			geActor_SetScale(sInstance->Actor, pInstance->Scale.X, pInstance->Scale.Y, pInstance->Scale.Z);
		}
	}

	UpdateActorState(pInstance);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetScaleXYZ
//
// Set the xyz-scale of an actor
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetScaleXYZ(const geActor* actor, const geVec3d& scale)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	if(pInstance->AttachedB)
		return RGF_SUCCESS;

	geVec3d_Copy(&scale, &pInstance->Scale);

	// Scale the actor
	geActor_SetScale(pInstance->Actor, pInstance->Scale.X, pInstance->Scale.Y, pInstance->Scale.Z);

	for(int i=0; i<3; ++i)
	{
		if(pInstance->LODActor[i])
			geActor_SetScale(pInstance->LODActor[i], pInstance->Scale.X, pInstance->Scale.Y, pInstance->Scale.Z);
	}

	// update attached blending actors
	std::vector<geActor*>::iterator iter = pInstance->AttachedBlendActors.begin();
	for(; iter!=pInstance->AttachedBlendActors.end(); ++iter)
	{
		ActorInstance *sInstance = LocateInstance(*iter);
		if(sInstance)
		{
			geActor_SetScale(sInstance->Actor, pInstance->Scale.X, pInstance->Scale.Y, pInstance->Scale.Z);
		}
	}

	UpdateActorState(pInstance);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// GetScaleX
//
// Return an actors current x-scale
/* ------------------------------------------------------------------------------------ */
int CActorManager::GetScaleX(const geActor* actor, float* scale)
{
	assert(scale);

	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	*scale = pInstance->Scale.X;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// GetScaleY
//
// Return an actors current y-scale
/* ------------------------------------------------------------------------------------ */
int CActorManager::GetScaleY(const geActor* actor, float* scale)
{
	assert(scale);

	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	*scale = pInstance->Scale.Y;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// GetScaleZ
//
// Return an actors current z-scale
/* ------------------------------------------------------------------------------------ */
int CActorManager::GetScaleZ(const geActor *actor, float* scale)
{
	assert(scale);

	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	*scale = pInstance->Scale.Z;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// GetScaleXYZ
//
// Return an actors current xyz-scale
/* ------------------------------------------------------------------------------------ */
int CActorManager::GetScaleXYZ(const geActor* actor, geVec3d* scale)
{
	assert(scale);

	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	geVec3d_Copy(&pInstance->Scale, scale);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// GetLODLevel
/* ------------------------------------------------------------------------------------ */
int CActorManager::GetLODLevel(const geActor* actor, int* level)
{
	assert(level);

	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	*level = pInstance->LODLevel;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetCollision
//
// Set collision for the desired actor
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetCollide(const geActor* actor)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	if(pInstance->AttachedB)
		return RGF_SUCCESS;

	pInstance->NoCollision = false;
	pInstance->RenderFlag = GE_ACTOR_RENDER_NORMAL | GE_ACTOR_COLLIDE | GE_ACTOR_RENDER_MIRRORS;
	geWorld_SetActorFlags(CCD->World(), pInstance->Actor, pInstance->RenderFlag);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetNoCollision
//
// Set no collision for the desired actor
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetNoCollide(const geActor* actor)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	pInstance->NoCollision = true;
	pInstance->RenderFlag = GE_ACTOR_RENDER_NORMAL | GE_ACTOR_RENDER_MIRRORS;
	geWorld_SetActorFlags(CCD->World(), pInstance->Actor, pInstance->RenderFlag);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetAlpha
//
// Set the alpha blend level for the desired actor
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetAlpha(const geActor* actor, float alpha)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	if(pInstance->AttachedB)
		return RGF_SUCCESS;

	geActor_SetAlpha(pInstance->Actor, alpha);

	std::vector<geActor*>::iterator iter = pInstance->AttachedBlendActors.begin();
	for(; iter!=pInstance->AttachedBlendActors.end(); ++iter)
	{
		ActorInstance *sInstance = LocateInstance(*iter);
		if(sInstance)
		{
			geActor_SetAlpha(sInstance->Actor, alpha);
		}
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// GetAlpha
//
// Get the current alpha blend level for the desired actor
/* ------------------------------------------------------------------------------------ */
int CActorManager::GetAlpha(const geActor* actor, float* alpha)
{
	assert(alpha);

	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	*alpha = geActor_GetAlpha(pInstance->Actor);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetRoot
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetRoot(const geActor* actor, const std::string& boneName)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	geXForm3d xForm;
	geVec3d Tmp;
	geActor_GetBoneAttachment(pInstance->Actor, boneName.c_str(), &xForm);
	geXForm3d_GetEulerAngles(&xForm, &Tmp);

	geXForm3d_SetZRotation(&xForm, -Tmp.Z);
	geXForm3d_RotateX(&xForm, -Tmp.X);
	geXForm3d_RotateY(&xForm, -Tmp.Y);
	geActor_SetBoneAttachment(pInstance->Actor, boneName.c_str(), &xForm);

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// GetBoundingBox
//
// Get the current bounding box for the requested actor.
/* ------------------------------------------------------------------------------------ */
int CActorManager::GetBoundingBox(const geActor* actor, geExtBox* box)
{
	ActorInstance *pInstance = LocateInstance(actor);
	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	geActor_GetExtBox(pInstance->Actor, box);

	box->Min.X -= pInstance->localTranslation.X;
	box->Min.Y -= pInstance->localTranslation.Y;
	box->Min.Z -= pInstance->localTranslation.Z;
	box->Max.X -= pInstance->localTranslation.X;
	box->Max.Y -= pInstance->localTranslation.Y;
	box->Max.Z -= pInstance->localTranslation.Z;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// DoesBoxHitActor
//
// EVIL HACK!  This should NOT be necessary but, for some reason,
// ..the geWorld_Collision doesn't always work right for some actors
// ..even when the bounding boxes of two actors clearly intersects.
//
// This function takes an ExtBox and checks to see if it intersects
// ..the ExtBox of any managed actor, and if so, returns a pointer
// ..to that actor - otherwise NULL.  GE_TRUE is returned if there
// ..was a collision, BTW.  Note that the EXTBOX coming in is assumed
// ..to be in MODEL SPACE, we translate it into WORLD SPACE assuming
// ..that the 'thePoint' is the origin of the ExtBox.
/* ------------------------------------------------------------------------------------ */
geBoolean CActorManager::DoesBoxHitActor(const geVec3d& /*point*/, const geExtBox& box,
										 geActor** actor)
{
	geExtBox result, temp;

	*actor = NULL;

	stdext::hash_map<std::string, LoadedActorList*>::iterator iter = m_LoadedActorFiles.begin();
	for(; iter!=m_LoadedActorFiles.end(); ++iter)
	{
		ActorInstance *pInstance = iter->second->IList;

		while(pInstance != NULL)
		{
			if(pInstance->NoCollision)
			{
				pInstance = pInstance->pNext;
				continue;
			}

			// Get actor instance bounding box in MODEL SPACE
			GetBoundingBox(pInstance->Actor, &result);

			result.Min.X += pInstance->localTranslation.X;
			result.Min.Y += pInstance->localTranslation.Y;
			result.Min.Z += pInstance->localTranslation.Z;
			result.Max.X += pInstance->localTranslation.X;
			result.Max.Y += pInstance->localTranslation.Y;
			result.Max.Z += pInstance->localTranslation.Z;

			if(memcmp(&result, &box, sizeof(geExtBox)) == 0)
			{
				// We will skip identical bounding boxes since we can
				// ..assume (HEH - EVIL HACK) that they're really the
				// ..same actor.
				pInstance = pInstance->pNext;
				continue;
			}

			if(geExtBox_Intersection(&result, &box, &temp) == GE_TRUE)
			{
				// Heh, we hit someone.  Return the actor we ran into.
				*actor = pInstance->Actor;
				return GE_TRUE;
			}

			pInstance = pInstance->pNext;		//	Next instance in list
		}
		// Next master instance
	}

	// No hit, all be hunky-dory.
	return GE_FALSE;							//	Hey, no collisions!
}

/* ------------------------------------------------------------------------------------ */
// DoesBoxHitActor
//
// EVIL HACK!  This should NOT be necessary but, for some reason,
// ..the geWorld_Collision doesn't always work right for some actors
// ..even when the bounding boxes of two actors clearly intersects.
//
// This function takes an ExtBox and checks to see if it intersects
// ..the ExtBox of any managed actor, and if so, returns a pointer
// ..to that actor - otherwise NULL.  GE_TRUE is returned if there
// ..was a collision, BTW.  Note that the EXTBOX coming in is assumed
// ..to be in MODEL SPACE, we translate it into WORLD SPACE assuming
// ..that the 'thePoint' is the origin of the ExtBox.  Note that this
// ..overload takes a geActor pointer to exclude from the collision
// ..check.
/* ------------------------------------------------------------------------------------ */
geBoolean CActorManager::DoesBoxHitActor(const geVec3d& /*point*/, const geExtBox& box,
										 geActor** actor, const geActor* actorToExclude)
{
	geExtBox result, temp;

	*actor = NULL;

	stdext::hash_map<std::string, LoadedActorList*>::iterator iter = m_LoadedActorFiles.begin();
	for(; iter!=m_LoadedActorFiles.end(); ++iter)
	{
		ActorInstance *pInstance = iter->second->IList;

		while(pInstance != NULL)
		{
			if(pInstance->Actor == actorToExclude)
			{
				// We want to ignore this actor, do so.
				pInstance = pInstance->pNext;
				continue;
			}

			if(pInstance->NoCollision)
			{
				pInstance = pInstance->pNext;
				continue;
			}

			// Get actor instance bounding box in MODEL SPACE
			GetBoundingBox(pInstance->Actor, &result);

			result.Min.X += pInstance->localTranslation.X;
			result.Min.Y += pInstance->localTranslation.Y;
			result.Min.Z += pInstance->localTranslation.Z;
			result.Max.X += pInstance->localTranslation.X;
			result.Max.Y += pInstance->localTranslation.Y;
			result.Max.Z += pInstance->localTranslation.Z;

			if(memcmp(&result, &box, sizeof(geExtBox)) == 0)
			{
				// We will skip identical bounding boxes since we can
				// ..assume (HEH - EVIL HACK) that they're really the
				// ..same actor.
				pInstance = pInstance->pNext;
				continue;
			}

			if(geExtBox_Intersection(&result, &box, &temp) == GE_TRUE)
			{
				// Heh, we hit someone. Return the actor we ran into.
				*actor = pInstance->Actor;
				return GE_TRUE;
			}
			pInstance = pInstance->pNext;		//	Next instance in list
		}
		//	Next master instance
	}

	// No hit, all be hunky-dory.
	return GE_FALSE;							//	Hey, no collisions!
}

/* ------------------------------------------------------------------------------------ */
// SetHealth
//
// Set the "health level" of an actor.
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetHealth(const geActor* actor, float health)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	pInstance->Health = health;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// ModifyHealth
//
// Update the current health of an actor by a specified amount
/* ------------------------------------------------------------------------------------ */
int CActorManager::ModifyHealth(const geActor* actor, float amount)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	pInstance->Health += amount;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// GetHealth
//
// Return the current health of an actor.
/* ------------------------------------------------------------------------------------ */
int CActorManager::GetHealth(const geActor *actor, float* health)
{
	assert(health);

	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	*health = pInstance->Health;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetTilt
//
// Set the Tilt flag for this actor
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetTilt(const geActor* actor, bool flag)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	pInstance->AllowTilt = flag;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetSlide
//
// Set the Slide flag for this actor
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetSlide(const geActor* actor, bool flag)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	pInstance->slide = flag;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetGravity
//
// Set the gravity force for this actor
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetGravity(const geActor* actor, geVec3d gravity)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	pInstance->Gravity = gravity;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetGravity
//
// Set the gravity force for this actor
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetGravityTime(const geActor* actor, float gravityTime)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	pInstance->GravityTime = gravityTime;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// GetGravity
//
// Get the gravity force for this actor
/* ------------------------------------------------------------------------------------ */
int CActorManager::GetGravity(const geActor* actor, geVec3d* gravity)
{
	if(!gravity)
		return RGF_FAILURE;

	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	*gravity = pInstance->Gravity;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// IsInFrontOf
//
// Determine if one actor is "in front of" another.
/* ------------------------------------------------------------------------------------ */
bool CActorManager::IsInFrontOf(const geActor* actor, const geActor* otherActor)
{
	ActorInstance *pInstance1 = LocateInstance(actor);

	if(pInstance1 == NULL)
		return false;							// Actor not found?!?!

	ActorInstance *pInstance2 = LocateInstance(otherActor);
	if(pInstance2 == NULL)
		return false;							// Actor not found?!?!

	// Ok, let's go!
	float dotProduct;
	geVec3d temp;

	// Subtract positions to normalize
	geVec3d_Subtract(&pInstance2->localTranslation, &pInstance1->localTranslation, &temp);

	// Do dotproduct.  If it's positive, then Actor2 is in front of Actor1
	geVec3d rotation = pInstance2->BaseRotation;
	rotation.X += pInstance2->localRotation.X;
	rotation.Y += pInstance2->localRotation.Y;
	rotation.Z += pInstance2->localRotation.Z;

	dotProduct = geVec3d_DotProduct(&temp, &rotation);

	if(dotProduct > 0)
		return true;							// Actor2 in front of Actor1
	else
		return false;							// Actor2 behind Actor1
}

/* ------------------------------------------------------------------------------------ */
// DistanceFrom
//
// Determine how far apart two actors are
/* ------------------------------------------------------------------------------------ */
float CActorManager::DistanceFrom(const geActor* actor, const geActor* otherActor)
{
	ActorInstance *pInstance1 = LocateInstance(actor);

	if(pInstance1 == NULL)
		return 0.0f;							// Actor not found?!?!

	ActorInstance *pInstance2 = LocateInstance(otherActor);

	if(pInstance2 == NULL)
		return 0.0f;							// Actor not found?!?!

	return geVec3d_DistanceBetween(&pInstance1->localTranslation,
									&pInstance2->localTranslation);
}

/* ------------------------------------------------------------------------------------ */
// DistanceFrom
//
// Determine how far an actor is from a specific point
/* ------------------------------------------------------------------------------------ */
float CActorManager::DistanceFrom(const geVec3d& point, const geActor* actor)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	return geVec3d_DistanceBetween(&point, &pInstance->localTranslation);
}

/* ------------------------------------------------------------------------------------ */
// GetActorsInRange
//
// Go through the actor database and fill in a list of all actors that are
//..within a specific range of a specific point.
/* ------------------------------------------------------------------------------------ */
int CActorManager::GetActorsInRange(const geVec3d& point, float range,
									int listSize, geActor** actorList)
{
	int outputCount = 0;

	stdext::hash_map<std::string, LoadedActorList*>::iterator iter = m_LoadedActorFiles.begin();
	for(; iter!=m_LoadedActorFiles.end(); ++iter)
	{
		ActorInstance *pInstance = iter->second->IList;

		while(pInstance != NULL)
		{
			// Scan through all the actors and check 'em out.
			if(DistanceFrom(point, pInstance->Actor) <= range)
			{
				// This one is IN MODEL RANGE, return it!
				actorList[outputCount++] = pInstance->Actor;

				if(outputCount >= listSize)
					return listSize;			// Output full, bail the scan
			}

			pInstance = pInstance->pNext;		// Next one
		}
	}

	return outputCount;							// How many we ended up with
}

/* ------------------------------------------------------------------------------------ */
// GetActorZone
//
// Return the current ZONE TYPE the desired actor is in
/* ------------------------------------------------------------------------------------ */
int CActorManager::GetActorZone(const geActor* actor, int* zoneType)
{
	assert(zoneType);

	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	*zoneType = pInstance->CurrentZone;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// GetActorOldZone
//
// Return the previous ZONE TYPE the desired actor was in
/* ------------------------------------------------------------------------------------ */
int CActorManager::GetActorOldZone(const geActor* actor, int* zoneType)
{
	assert(zoneType);

	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	*zoneType = pInstance->OldZone;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetMoving
/* ------------------------------------------------------------------------------------ */
void CActorManager::SetMoving(const geActor* actor)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return;									// Actor not found?!?!

	pInstance->Moving = true;
}

/* ------------------------------------------------------------------------------------ */
// GetMoving
/* ------------------------------------------------------------------------------------ */
bool CActorManager::GetMoving(const geActor* actor)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return false;							// Actor not found?!?!

	return pInstance->Moving;
}

/* ------------------------------------------------------------------------------------ */
// GetLiquid
/* ------------------------------------------------------------------------------------ */
Liquid* CActorManager::GetLiquid(const geActor* actor)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return NULL;							// Actor not found?!?!

	return pInstance->LQ;
}

/* ------------------------------------------------------------------------------------ */
// GetMotion
//
// get the current actor animation sequence
/* ------------------------------------------------------------------------------------ */
std::string CActorManager::GetMotion(const geActor* actor)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return "";								// Actor not found?!?!

	return pInstance->MotionName;
}

/* ------------------------------------------------------------------------------------ */
// GetpMotion
/* ------------------------------------------------------------------------------------ */
geMotion* CActorManager::GetpMotion(const geActor* actor, int /*channel*/)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return NULL;							// Actor not found?!?!

	return pInstance->pMotion;
}

/* ------------------------------------------------------------------------------------ */
// GetpBMotion
/* ------------------------------------------------------------------------------------ */
geMotion *CActorManager::GetpBMotion(const geActor* actor, int /*channel*/)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return NULL;							// Actor not found?!?!

	if(pInstance->BlendFlag || pInstance->TransitionFlag)
		return pInstance->pBMotion;

	return NULL;
}

/* ------------------------------------------------------------------------------------ */
// GetBlendAmount
/* ------------------------------------------------------------------------------------ */
float CActorManager::GetBlendAmount(const geActor* actor, int /*channel*/)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return 0.0f;							// Actor not found?!?!

	if(pInstance->BlendFlag || pInstance->TransitionFlag)
		return pInstance->BlendAmount;

	return 0.0f;
}

/* ------------------------------------------------------------------------------------ */
// GetStartTime
/* ------------------------------------------------------------------------------------ */
float CActorManager::GetStartTime(const geActor* actor, int /*channel*/)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return 0.0f;							// Actor not found?!?!

	return pInstance->StartTime;
}

/* ------------------------------------------------------------------------------------ */
// GetTransitionFlag
/* ------------------------------------------------------------------------------------ */
bool CActorManager::GetTransitionFlag(const geActor* actor, int /*channel*/)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return false;							// Actor not found?!?!

	return pInstance->TransitionFlag;
}

/* ------------------------------------------------------------------------------------ */
// Falling
//
// Return GE_TRUE if the actor is falling, GE_FALSE if landed.
/* ------------------------------------------------------------------------------------ */
geBoolean CActorManager::Falling(const geActor* actor)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;					// Actor not found?!?!

	if(pInstance->OldZone > 0)
		return GE_FALSE;

	if(pInstance->GravityTime != 0.0f)
		return GE_TRUE;							// Falling!

	return GE_FALSE;							// Not falling
}

/* ------------------------------------------------------------------------------------ */
// SetVehicle
//
// Tell this actor what model he's riding on, if any
/* ------------------------------------------------------------------------------------ */
void CActorManager::SetVehicle(const geActor* actor, geActor* vehicle)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return;									// Actor not found?!?!

	pInstance->Vehicle = vehicle;
}

/* ------------------------------------------------------------------------------------ */
// ActorDetach
/* ------------------------------------------------------------------------------------ */
void CActorManager::ActorDetach(const geActor* slave)
{
	ActorInstance *pInstance = LocateInstance(slave);

	if(pInstance == NULL)
		return;

	pInstance->Attached = false;
	geVec3d_Add(&pInstance->localRotation, &pInstance->MasterRotation, &pInstance->localRotation);
}

/* ------------------------------------------------------------------------------------ */
// DetachFromActor
/* ------------------------------------------------------------------------------------ */
void CActorManager::DetachFromActor(const geActor* master, const geActor* slave)
{
	ActorInstance *sInstance = LocateInstance(slave);

	if(sInstance == NULL)
		return;

	ActorInstance *mInstance = LocateInstance(master);

	if(mInstance == NULL)
		return;

	std::vector<Attachment>::iterator iter = mInstance->AttachedActors.begin();
	for(; iter!=mInstance->AttachedActors.end(); ++iter)
	{
		if(iter->AttachedActor == sInstance->Actor)
		{
			sInstance->Attached = false;
			geVec3d_Add(&sInstance->localRotation, &sInstance->MasterRotation, &sInstance->localRotation);
			mInstance->AttachedActors.erase(iter);
			return;
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// ActorAttach
/* ------------------------------------------------------------------------------------ */
void CActorManager::ActorAttach(geActor* slave,  const char* slaveBoneName,
								const geActor* master, const char* masterBoneName,
								const geVec3d& attachOffset, const geVec3d& angle)
{
	ActorInstance *slaveInstance = LocateInstance(slave);

	if(slaveInstance == NULL)
		return;

	if(slaveInstance->Attached || slaveInstance->AttachedB)
		return;

	ActorInstance *masterInstance = LocateInstance(master);

	if(masterInstance == NULL)
		return;

	slaveInstance->Attached = true;

	Attachment newSlave;
	newSlave.AttachedActor = slave;

	strncpy(newSlave.SlaveBone, slaveBoneName, 256);
	newSlave.SlaveBone[255] = 0;

	strncpy(newSlave.MasterBone, masterBoneName, 256);
	newSlave.MasterBone[255] = 0;

	newSlave.Offset = attachOffset;

	masterInstance->AttachedActors.push_back(newSlave);

	slaveInstance->BaseRotation.X = angle.X;
	slaveInstance->BaseRotation.Y = angle.Y;
	slaveInstance->BaseRotation.Z = angle.Z;
	slaveInstance->MasterRotation = masterInstance->localRotation;
	slaveInstance->localRotation.X = slaveInstance->localRotation.Y = slaveInstance->localRotation.Z = 0.0f;

	UpdateActorState(slaveInstance);
}

/* ------------------------------------------------------------------------------------ */
//
/* ------------------------------------------------------------------------------------ */
void CActorManager::ActorAttachBlend(geActor* slave, const geActor* master)
{
	if(slave == master) // cannot attach to itself
		return;

	ActorInstance *sInstance = LocateInstance(slave);

	if(sInstance == NULL)
		return;

	if(sInstance->Attached || sInstance->AttachedB)
		return;

	ActorInstance *mInstance = LocateInstance(master);

	if(mInstance == NULL)
		return;

	if(mInstance->AttachedB)
		return;

	SetNoCollide(sInstance->Actor);
	RemoveForce(sInstance->Actor, 0);
	RemoveForce(sInstance->Actor, 1);
	RemoveForce(sInstance->Actor, 2);
	RemoveForce(sInstance->Actor, 3);
	{
		GE_RGBA fillColor;
		GE_RGBA ambientColor;
		geBoolean ambientLightFromFloor;

		GetActorDynamicLighting(mInstance->Actor, &fillColor, &ambientColor, &ambientLightFromFloor);
		SetActorDynamicLighting(sInstance->Actor, fillColor, ambientColor, ambientLightFromFloor);
	}
	SetScaleXYZ(sInstance->Actor, mInstance->Scale);
	SetAlpha(sInstance->Actor, geActor_GetAlpha(mInstance->Actor));
	sInstance->BaseRotation		= mInstance->BaseRotation;
	sInstance->localRotation	= mInstance->localRotation;
	sInstance->localTranslation	= mInstance->localTranslation;
	sInstance->MotionName		= mInstance->MotionName;
	sInstance->AnimationTime	= mInstance->AnimationTime;

	sInstance->AttachedB = true;
	geExtBox extBox;
	GetBoundingBox(mInstance->Actor, &extBox);
	geActor_SetExtBox(sInstance->Actor, &extBox, NULL);
	geActor_SetRenderHintExtBox(sInstance->Actor, &extBox, NULL);

	mInstance->AttachedBlendActors.push_back(sInstance->Actor);

	UpdateActorState(mInstance);
}

/* ------------------------------------------------------------------------------------ */
//
/* ------------------------------------------------------------------------------------ */
void CActorManager::DetachBlendFromActor(const geActor* master, const geActor* slave)
{
	if(master == slave)
		return;

	ActorInstance *sInstance = LocateInstance(slave);

	if(sInstance == NULL)
		return;

	ActorInstance *mInstance = LocateInstance(master);

	if(mInstance == NULL)
		return;

	std::vector<geActor*>::iterator iter = mInstance->AttachedBlendActors.begin();
	for(; iter!=mInstance->AttachedBlendActors.end(); ++iter)
	{
		if(*iter == sInstance->Actor)
		{
			sInstance->AttachedB = false;
			mInstance->AttachedBlendActors.erase(iter);
			return;
		}
	}

	// slave not found in master's attach-list
}

/* ------------------------------------------------------------------------------------ */
// IsActor
/* ------------------------------------------------------------------------------------ */
bool CActorManager::IsActor(const geActor* actor)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return false;					// Actor not found?!?!

	return true;
}

/* ------------------------------------------------------------------------------------ */
// GetVehicle
//
// Tell us what model the actor is on, if any
/* ------------------------------------------------------------------------------------ */
geActor* CActorManager::GetVehicle(const geActor* actor)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return NULL;					// Actor not found?!?!

	return pInstance->Vehicle;
}

/* ------------------------------------------------------------------------------------ */
// SetLODdistance
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetLODdistance(const geActor* actor, float LOD1, float LOD2,
								  float LOD3, float LOD4, float LOD5)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;

	pInstance->LODdistance[0] = LOD1;
	pInstance->LODdistance[1] = LOD2;
	pInstance->LODdistance[2] = LOD3;
	pInstance->LODdistance[3] = LOD4;
	pInstance->LODdistance[4] = LOD5;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetPassenger
//
// Given an actor to be a passenger, and an actor that's the vehicle,
// ..set the actor to be "riding" the vehicle.
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetPassenger(geActor* passenger, geActor* vehicle)
{
	ActorInstance *pInstance = LocateInstance(passenger);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;			// Actor not found?!?!

	ActorInstance *pRide = LocateInstance(vehicle);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;			// Actor not found?!?!

	int i;
	for(i=0; i<PASSENGER_LIST_SIZE; ++i)
	{
		if(pRide->Passengers[i] == passenger)
			return RGF_SUCCESS;
	}

	for(i=0; i<PASSENGER_LIST_SIZE; ++i)
	{
		if(pRide->Passengers[i] == NULL)
		{
			pRide->Passengers[i] = passenger;
			pInstance->Vehicle = vehicle;
			return RGF_SUCCESS;			// Done!
		}
	}

	return RGF_FAILURE;					// This bus is PACKED!
}

/* ------------------------------------------------------------------------------------ */
// RemovePassenger
//
// Given an actor, remove it from the vehicle it's on.
/* ------------------------------------------------------------------------------------ */
int CActorManager::RemovePassenger(const geActor* passenger)
{
	ActorInstance *pInstance = LocateInstance(passenger);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;			// Actor not found?!?!

	ActorInstance *pRide = LocateInstance(pInstance->Vehicle);

	if(pRide == NULL)
		return RGF_NOT_FOUND;			// Actor not found?!?!

	for(int i=0; i<PASSENGER_LIST_SIZE; ++i)
	{
		if(pRide->Passengers[i] == passenger)
		{
			pRide->Passengers[i] = NULL;
			pInstance->Vehicle = NULL;
			return RGF_SUCCESS;			// He's outta there
		}
	}

	return RGF_FAILURE;					// Yo, he's not in MY cab
}

/* ------------------------------------------------------------------------------------ */
// Inventory
//
// Returns a pointer to the list of persistent attributes used as the
// ..inventory/attribute list for the desired actor.
/* ------------------------------------------------------------------------------------ */
CPersistentAttributes* CActorManager::Inventory(const geActor* actor)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return NULL;					// Actor not found?!?!

	return pInstance->Inventory;
}

/* ------------------------------------------------------------------------------------ */
// Update
//
// Pass time to each instance of an actor active in the system.
/* ------------------------------------------------------------------------------------ */
void CActorManager::Update(float timeElapsed)
{
	// Move vehicles first so that we translate all the actors
	// ..that are passengers FIRST...
	stdext::hash_map<std::string, LoadedActorList*>::iterator iter = m_LoadedActorFiles.begin();
	for(; iter!=m_LoadedActorFiles.end(); ++iter)
	{
		MoveAllVehicles(iter->second, timeElapsed);
	}

	// Ok, now we handle moving the non-vehicle actors
	for(iter=m_LoadedActorFiles.begin(); iter!=m_LoadedActorFiles.end(); ++iter)
	{
		geExtBox result;

		TimeAdvanceAllInstances(iter->second, timeElapsed);
		ActorInstance *pInstance = iter->second->IList;

		while(pInstance)
		{
			ResetActorDynamicLighting(pInstance->Actor);
			GetBoundingBox(pInstance->Actor, &result);

			if(CCD->MenuManager()->GetSEBoundBox() && pInstance->Actor != CCD->Player()->GetActor())
			{
				DrawBoundBox(CCD->World(), &pInstance->localTranslation,
							&result.Min, &result.Max);
/*
				int TotalStaticBoneCount = geActor_GetBoneCount(pInstance->Actor);
				geExtBox theStaticBoneBox;
				for(int nStatic = 0; nStatic < TotalStaticBoneCount; ++nStatic)
				{
					geXForm3d theTransform;
					// The bone bounding box comes back in worldspace coordinates...
					if(geActor_GetBoneExtBoxByIndex(pInstance->Actor, nStatic, &theStaticBoneBox) != GE_TRUE)
						continue;								// Nothing here, skip it

					geXForm3d Attachment;
					int ParentBoneIndex;
					const char *BoneName;
					geBody_GetBone(geActor_GetBody(geActor_GetActorDef(pInstance->Actor)),
					nStatic, &BoneName, &Attachment, &ParentBoneIndex);

					geActor_GetBoneTransformByIndex(pInstance->Actor, nStatic, &theTransform);
					// Ok, convert from worldspace to modelspace for the bounding box
					theStaticBoneBox.Min.X -= theTransform.Translation.X;
					theStaticBoneBox.Min.Y -= theTransform.Translation.Y;
					theStaticBoneBox.Min.Z -= theTransform.Translation.Z;
					theStaticBoneBox.Max.X -= theTransform.Translation.X;
					theStaticBoneBox.Max.Y -= theTransform.Translation.Y;
					theStaticBoneBox.Max.Z -= theTransform.Translation.Z;
					DrawBoundBox(CCD->World(), &theTransform.Translation, &theStaticBoneBox.Min, &theStaticBoneBox.Max);
				}
*/
			}

			pInstance = pInstance->pNext;
		}
	}
}


/* ----------------------------- PRIVATE MEMBER FUNCTIONS ----------------------------- */


/* ------------------------------------------------------------------------------------ */
// RemoveAllActors
//
// Delete all the actors in the instance list for this loaded
// ..actor entry.
/* ------------------------------------------------------------------------------------ */
void CActorManager::RemoveAllActors(LoadedActorList* entry)
{
	if(entry == NULL)
		return;									// Empty entity, bail function

	if(entry->IList == NULL)
		return;									// Empty list, bail function

	ActorInstance *pInstance = entry->IList;
	ActorInstance *pNext = NULL;

	while(pInstance != NULL)
	{
		pNext = pInstance->pNext;

		m_ActorLookUp.erase(pInstance->Actor);
		geWorld_RemoveActor(CCD->World(), pInstance->Actor);

		for(int i=0; i<3; ++i)
		{
			if(pInstance->LODActor[i])
			{
				geWorld_RemoveActor(CCD->World(), pInstance->LODActor[i]);
				geActor_Destroy(&pInstance->LODActor[i]);
				pInstance->LODActor[i] = NULL;
			}
		}

		{
			std::vector<Attachment>::iterator iter = pInstance->AttachedActors.begin();
			for(; iter!=pInstance->AttachedActors.end(); ++iter)
			{
				ActorInstance *att = LocateInstance(iter->AttachedActor);

				if(att != NULL)
				{
					ActorDetach(att->Actor);
				}
			}
		}

		{
			std::vector<geActor*>::iterator iter = pInstance->AttachedBlendActors.begin();
			for(; iter!=pInstance->AttachedBlendActors.end(); ++iter)
			{
				ActorInstance *att = LocateInstance(*iter);

				if(att != NULL)
				{
					DetachBlendFromActor(pInstance->Actor, att->Actor);
				}
			}
			pInstance->AttachedBlendActors.clear();
		}

		pInstance->Inventory->Clear();
		delete pInstance->Inventory;

		for(int CollideLevel=COLActorBBox; CollideLevel<COLMaxBBox; ++CollideLevel)
		{
			CollideObjectInformation *ci = pInstance->CollideObjects[CollideLevel];
			CollideObjectInformation *temp = NULL;
			while(ci)
			{
				temp = ci->NextCollideObject;
				geRam_Free(ci);
				ci = temp;
			}
		}

		delete pInstance;
		pInstance = pNext;						// On to the next victim
	}

	entry->IList = NULL;						// Entire list killed
}

/* ------------------------------------------------------------------------------------ */
// AddNewInstance
//
// Adds a new instance to the instance list for the desired loaded actor entry.
/* ------------------------------------------------------------------------------------ */
geActor* CActorManager::AddNewInstance(LoadedActorList* entry, geActor* oldActor)
{
	ActorInstance *pTemp = entry->IList;
	// call compiler generated default constructor so that it zero initializes its members
	ActorInstance *newEntry = new ActorInstance;

	newEntry->NoCollision = false;
	newEntry->Health = 0.f;
	newEntry->GravityTime = 0.f;
	newEntry->CurrentZone = 0;
	newEntry->GravityCoeff = 0;
	newEntry->StartTime = 0.f;
	newEntry->BlendAmount = 0.f;
	newEntry->BlendNextAmount = 0.f;
	newEntry->BlendNextFlag = false;
	newEntry->AnimationTime = 0.f;
	newEntry->PrevAnimationTime = 0.f;
	newEntry->pMotion = NULL;
	newEntry->pBMotion = NULL;
	newEntry->Vehicle = NULL;
	memset(newEntry->ForceVector,		0, sizeof(geVec3d)     *4);
	memset(newEntry->ForceLevel,		0, sizeof(float)       *4);
	memset(newEntry->ForceDecay,		0, sizeof(float)       *4);
	memset(newEntry->CollideObjects,	0, sizeof(CollideObjectInformation*)*COLMaxBBox);
	geVec3d_Clear(&newEntry->localTranslation);
	geVec3d_Clear(&newEntry->OldTranslation);
	geVec3d_Clear(&newEntry->localRotation);
	geVec3d_Clear(&newEntry->Gravity);
	geVec3d_Clear(&newEntry->MasterRotation);
	geVec3d_Set(&newEntry->Scale, 1.f, 1.f, 1.f);

	// Ok, let's fill in the new entry
	if(oldActor)
		newEntry->Actor = oldActor;
	else
		newEntry->Actor = geActor_Create(entry->ActorDef[0]);

	newEntry->theDef = entry->ActorDef[0];

	geWorld_AddActor(CCD->World(), newEntry->Actor, GE_ACTOR_COLLIDE, 0xffffffff);
	newEntry->RenderFlag = GE_ACTOR_RENDER_NORMAL | GE_ACTOR_COLLIDE | GE_ACTOR_RENDER_MIRRORS;

	geActor_SetStaticLightingOptions(newEntry->Actor, GE_TRUE, GE_TRUE, 6);

	for(int i=0; i<3; ++i)
	{
		newEntry->theLODDef[i] = entry->ActorDef[i+1];
		newEntry->LODActor[i] = NULL;

		if(newEntry->theLODDef[i])
		{
			newEntry->LODActor[i] = geActor_Create(entry->ActorDef[i+1]);
			geWorld_AddActor(CCD->World(), newEntry->LODActor[i], 0, 0xffffffff);
			geActor_SetStaticLightingOptions(newEntry->LODActor[i], GE_TRUE, GE_TRUE, 6);
		}
	}

	for(int i=0; i<5; ++i)
	{
		newEntry->LODdistance[i] = CCD->Level()->GetLODdistance(i);
	}

	newEntry->LODLevel = 0;
	newEntry->Bitmap = entry->Bitmap;
	geVec3d_Clear(&newEntry->BaseRotation);
	newEntry->AutoStepUp = false;		// Default no auto step up
	newEntry->MaxStepHeight = 16.0f;	// Actors max step height, in world units
	newEntry->pNext = NULL;
	newEntry->AllowTilt = true;
	newEntry->slide = true;
	newEntry->ForceEnabled = true;
	newEntry->TiltX = 0.0f;

	newEntry->ShadowSize = 0.0f;
	newEntry->ShadowBitmap = NULL;
	newEntry->ShadowAlpha = 128.0f;
	newEntry->ProjectedShadows = false;

	newEntry->Moving = false;
	newEntry->OldZone = 0;
	newEntry->LQ = NULL;

	newEntry->PassengerModel = NULL;
	newEntry->TransitionFlag = false;
	newEntry->HideRadar = false;
	newEntry->Group.clear();
	newEntry->szEntityName.clear();

	newEntry->FillColor.r = newEntry->FillColor.g = newEntry->FillColor.b = 0.0f;
	newEntry->AmbientColor.r = newEntry->AmbientColor.g = newEntry->AmbientColor.b = 0.0f;
	newEntry->FillColor.a = newEntry->AmbientColor.a = 255.0f;
	newEntry->AmbientLightFromFloor = GE_TRUE;

	newEntry->BoxChange = true;
	newEntry->BlendFlag = false;
	newEntry->HoldAtEnd = false;
	newEntry->Attached = false;
	newEntry->AttachedB = false;

	newEntry->AttachedActors.clear();
	newEntry->AttachedBlendActors.clear();

	newEntry->CollDetLevel = RGF_COLLISIONLEVEL_1;
	newEntry->ActorType = ENTITY_GENERIC;			// Generic, for now
	newEntry->Inventory = new CPersistentAttributes;
	newEntry->Inventory->AddAndSet("health", 1);
	newEntry->Inventory->SetValueLimits("health", 1, 1);
	newEntry->ActorAnimationSpeed = 1.0f;

	for(int i=0; i<PASSENGER_LIST_SIZE; ++i)
		newEntry->Passengers[i] = NULL;			// No passengers

	// Get, and set, the axially-aligned bounding box for this actor
	geExtBox extBox;

	geActor_GetDynamicExtBox(newEntry->Actor, &extBox);
	extBox.Min.X -= newEntry->localTranslation.X;
	extBox.Min.Y -= newEntry->localTranslation.Y;
	extBox.Min.Z -= newEntry->localTranslation.Z;
	extBox.Max.X -= newEntry->localTranslation.X;
	extBox.Max.Y -= newEntry->localTranslation.Y;
	extBox.Max.Z -= newEntry->localTranslation.Z;

	geActor_SetExtBox(newEntry->Actor, &extBox, NULL);
	geActor_SetRenderHintExtBox(newEntry->Actor, &extBox, NULL);
	newEntry->NeedsNewBB = true;

	if(pTemp == NULL)
	{
		// Making the head of the list
		entry->IList = newEntry;
		entry->IList->pNext = NULL;
	}
	else
	{
		while(pTemp->pNext != NULL)
			pTemp = pTemp->pNext;

		pTemp->pNext = newEntry;
		pTemp->pNext->pNext = NULL;
	}

	m_ActorLookUp[newEntry->Actor] = newEntry;

	return newEntry->Actor;
}

/* ------------------------------------------------------------------------------------ */
// CountActors
/* ------------------------------------------------------------------------------------ */
int CActorManager::CountActors() const
{
	return m_InstanceCount;
}

/* ------------------------------------------------------------------------------------ */
// LocateInstance
//
// Search ALL loaded actors for this specific instance
/* ------------------------------------------------------------------------------------ */
ActorInstance* CActorManager::LocateInstance(const geActor* actor)
{
	if(m_ActorLookUp.find(actor) != m_ActorLookUp.end())
		return m_ActorLookUp[actor];

	return NULL;
}

/* ------------------------------------------------------------------------------------ */
// RemoveInstance
//
// Search ALL loaded actors for this specific instance and KILL IT DEAD!
/* ------------------------------------------------------------------------------------ */
int CActorManager::RemoveInstance(const geActor* actor)
{
	stdext::hash_map<std::string, LoadedActorList*>::iterator iter = m_LoadedActorFiles.begin();
	for(; iter!=m_LoadedActorFiles.end(); ++iter)
	{
		ActorInstance *pTemp = iter->second->IList;
		ActorInstance *pPrev = pTemp;

		while(pTemp != NULL)
		{
			if(pTemp->Actor == actor)
			{
				m_ActorLookUp.erase(pTemp->Actor);
				geWorld_RemoveActor(CCD->World(), pTemp->Actor);

				for(int i=0; i<3; ++i)
				{
					if(pTemp->LODActor[i])
					{
						geWorld_RemoveActor(CCD->World(), pTemp->LODActor[i]);
						geActor_Destroy(&pTemp->LODActor[i]);
						pTemp->LODActor[i] = NULL;
					}
				}

				{
					std::vector<Attachment>::iterator iter = pTemp->AttachedActors.begin();
					for(; iter!=pTemp->AttachedActors.end(); ++iter)
					{
						ActorInstance *pInstance = LocateInstance(iter->AttachedActor);

						if(pInstance != NULL)
						{
							ActorDetach(pInstance->Actor);
						}
					}
				}

				{
					std::vector<geActor*>::iterator iter = pTemp->AttachedBlendActors.begin();
					for(; iter!=pTemp->AttachedBlendActors.end(); ++iter)
					{

						ActorInstance *pInstance = LocateInstance(*iter);

						if(pInstance != NULL)
						{
							DetachBlendFromActor(pTemp->Actor, pInstance->Actor);
						}
					}
					pTemp->AttachedBlendActors.clear();
				}

				pTemp->Inventory->Clear();
				delete pTemp->Inventory;

				for(int CollideLevel=COLActorBBox; CollideLevel<COLMaxBBox; ++CollideLevel)
				{
					CollideObjectInformation *ci = pTemp->CollideObjects[CollideLevel];
					CollideObjectInformation *temp = NULL;
					while(ci)
					{
						temp = ci->NextCollideObject;
						geRam_Free(ci);
						ci = temp;
					}
				}

				if((iter->second->IList == pTemp))
					iter->second->IList = pTemp->pNext;

				pPrev->pNext = pTemp->pNext;		// Patch around current
				delete pTemp;						// ZAP the entry
				--m_InstanceCount;
				return RGF_SUCCESS;					// Entry is DEAD!
			}

			pPrev = pTemp;							// Save previous
			pTemp = pTemp->pNext;					// Next?
		}
	}

	return RGF_FAILURE;
}

/* ------------------------------------------------------------------------------------ */
// TimeAdvanceAllInstances
//
// Advance time for all instances of the specific list entry.
/* ------------------------------------------------------------------------------------ */
void CActorManager::TimeAdvanceAllInstances(LoadedActorList* instance,
											float timeElapsed)
{
	ActorInstance *pInstance = instance->IList;

	// advance unattached instances
	while(pInstance != NULL)
	{
		if(pInstance->ActorType != ENTITY_VEHICLE)
		{
			if(!pInstance->Attached && !pInstance->AttachedB)
			{
				pInstance->CurrentZone = GetCurrentZone(pInstance);
				if(pInstance->ForceEnabled)
					ProcessForce(pInstance, timeElapsed);		// Process any forces
				ProcessGravity(pInstance, timeElapsed);
				AdvanceInstanceTime(pInstance, timeElapsed);	// Make time move
			}
		}

		pInstance = pInstance->pNext;							// Next?
	}

	pInstance = instance->IList;

	// advance attached instances
	while(pInstance != NULL)
	{
		if(pInstance->ActorType != ENTITY_VEHICLE)
		{
			if(pInstance->Attached && !pInstance->AttachedB)
			{
				pInstance->CurrentZone = GetCurrentZone(pInstance);
				if(pInstance->ForceEnabled)
					ProcessForce(pInstance, timeElapsed);		// Process any forces
				ProcessGravity(pInstance, timeElapsed);
				AdvanceInstanceTime(pInstance, timeElapsed);	// Make time move
			}
		}

		pInstance = pInstance->pNext;							// Next?
	}
}

// start multiplayer
/* ------------------------------------------------------------------------------------ */
// SetAnimationTime
/* ------------------------------------------------------------------------------------ */
void CActorManager::SetAnimationTime(const geActor* actor, float time, int /*channel*/)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return;

	if(pInstance->AttachedB)
		return;

	pInstance->AnimationTime = time;
}
// end multiplayer

/* ------------------------------------------------------------------------------------ */
// GetAnimationTime
/* ------------------------------------------------------------------------------------ */
float CActorManager::GetAnimationTime(const geActor* actor, int /*channel*/)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return 0.0f;

	return pInstance->PrevAnimationTime;
}

/* ------------------------------------------------------------------------------------ */
// SetHoldAtEnd
/* ------------------------------------------------------------------------------------ */
void CActorManager::SetHoldAtEnd(const geActor* actor, bool State, int /*channel*/)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return;

	pInstance->HoldAtEnd = State;
}

/* ------------------------------------------------------------------------------------ */
// EndAnimation
/* ------------------------------------------------------------------------------------ */
bool CActorManager::EndAnimation(const geActor* actor, int /*channel*/)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return false;

	if(pInstance->TransitionFlag)
		return false;

	geMotion *pMotion;
	float time = (pInstance->AnimationTime * 0.001f) * pInstance->ActorAnimationSpeed;
	float tStart, tEnd;

	// Get the motion name
	pMotion = geActor_GetMotionByName(pInstance->theDef, pInstance->MotionName.c_str());

	if(!pMotion)
		return false;

	geMotion_GetTimeExtents(pMotion, &tStart, &tEnd);

	if(time >= tEnd)
		return true;

	return false;
}

/* ------------------------------------------------------------------------------------ */
// AdvanceInstanceTime
//
// Advance time for a specific instance of an actor
/* ------------------------------------------------------------------------------------ */
void CActorManager::AdvanceInstanceTime(ActorInstance *instance, float timeElapsed)
{
	geMotion *pMotion;
	geMotion *pBMotion;
	geXForm3d xForm;
	geActor_Def *theDef = instance->theDef;
	bool LODA = CCD->Level()->GetLODAnimation();

	geVec3d CamPosition;
	CCD->CameraManager()->GetPosition(&CamPosition);

	float dist = geVec3d_DistanceBetween(&CamPosition, &instance->localTranslation) / CCD->CameraManager()->AmtZoom();

	for(int k=0; k<3; ++k)
	{
		if(instance->LODActor[k])
			geWorld_SetActorFlags(CCD->World(), instance->LODActor[k], 0);
	}

	geWorld_SetActorFlags(CCD->World(), instance->Actor,
		instance->RenderFlag & GE_ACTOR_COLLIDE);

	int RenderFlag = instance->RenderFlag & (GE_ACTOR_RENDER_NORMAL | GE_ACTOR_RENDER_MIRRORS);

	geActor *RActor = instance->Actor;
	instance->LODLevel = 0;

	if(	CCD->Level()->GetLODdistance(0) == 0 &&
		CCD->Level()->GetLODdistance(1) == 0 &&
		CCD->Level()->GetLODdistance(2) == 0 &&
		CCD->Level()->GetLODdistance(3) == 0 &&
		CCD->Level()->GetLODdistance(4) == 0)
	{
		geWorld_SetActorFlags(CCD->World(), RActor, instance->RenderFlag);
	}
	else
	{
		if(CCD->Level()->GetLODdistance(0) != 0 && dist > CCD->Level()->GetLODdistance(0))
		{
			if(instance->LODActor[0])
			{
				RActor = instance->LODActor[0];
				instance->LODLevel = 1;

				if(LODA)
					theDef = instance->theLODDef[0];
			}
		}

		if(CCD->Level()->GetLODdistance(1) != 0 && dist > CCD->Level()->GetLODdistance(1))
		{
			if(instance->LODActor[1])
			{
				RActor = instance->LODActor[1];
				instance->LODLevel = 2;

				if(LODA)
					theDef = instance->theLODDef[1];
			}
		}

		if(CCD->Level()->GetLODdistance(2) != 0 && dist > CCD->Level()->GetLODdistance(2))
		{
			if(instance->LODActor[2])
			{
				RActor = instance->LODActor[2];
				instance->LODLevel = 3;

				if(LODA)
					theDef = instance->theLODDef[2];
			}
		}

		if(dist < CCD->Level()->GetLODdistance(4) || CCD->Level()->GetLODdistance(4) == 0)
		{
			if(CCD->Level()->GetLODdistance(3) != 0 && dist > CCD->Level()->GetLODdistance(3) && instance->Bitmap)
			{
				GE_LVertex	Vertex;
				Vertex.r = 255.0f;
				Vertex.g = 255.0f;
				Vertex.b = 255.0f;
				Vertex.a = 255.0f;

				if(CCD->Level()->GetLODdistance(4) != 0 && CCD->Level()->GetLODdistance(4) > CCD->Level()->GetLODdistance(3))
				{
					float diff = CCD->Level()->GetLODdistance(4) - CCD->Level()->GetLODdistance(3);
					float alpha = 1.0f - ((dist - CCD->Level()->GetLODdistance(3)) / diff);
					Vertex.a = 255.0f*alpha;
				}

				Vertex.u = 0.0f;
				Vertex.v = 0.0f;

				float halfHeight = geBitmap_Height(instance->Bitmap) * instance->Scale.Y * 0.5f;

				Vertex.X = instance->localTranslation.X;
				Vertex.Y = instance->localTranslation.Y + halfHeight;
				Vertex.Z = instance->localTranslation.Z;

				geWorld_AddPolyOnce(
					CCD->World(),
					&Vertex,
					1,
					instance->Bitmap,
					GE_TEXTURED_POINT,
					GE_RENDER_DEPTH_SORT_BF | GE_RENDER_DO_NOT_OCCLUDE_OTHERS,
					instance->Scale.Y);

				instance->LODLevel = 4;
			}
			else
			{
				if(RActor == instance->Actor)
					geWorld_SetActorFlags(CCD->World(), RActor, instance->RenderFlag);
				else
					geWorld_SetActorFlags(CCD->World(), RActor, RenderFlag);
			}
		}
		else
		{
			instance->LODLevel = 5;
		}
	}

	if(instance->MotionName.empty())
		pMotion = NULL;
	else
		pMotion = geActor_GetMotionByName(theDef, instance->MotionName.c_str());	// Get the motion name

	// TODO
	// need to cycle through each animation channel and build our pose for this frame
	// root channel first, then cycle through each sub-channel in order
	// 0	-- root / lower body
	// 1	-- upper body
	// 2	-- head
	// 3	-- user channel 1
	// 4	-- user channel 2

	// no motion?
	if(!pMotion)
	{
		//	is the animation missing from the actor?
		if(!instance->MotionName.empty() && instance->Actor == CCD->Player()->GetActor())
		{
			CCD->Log()->Warning("File %s - Line %d: Player missing animation '%s'",
								__FILE__, __LINE__, instance->MotionName.c_str());
		}

		// check to see if we have a 'next motion name', maybe we just need to play it instead
		if(!instance->NextMotionName.empty())
		{
			instance->MotionName = instance->NextMotionName;

			// do we need to blend our current pose with the 'next motion'?
			if(instance->BlendNextFlag)
			{
				instance->BlendFlag = true;
				instance->BlendAmount = instance->BlendNextAmount;

				instance->BlendMotionName = instance->BlendNextMotionName;
			}
			// no blending, just play next animation
			else
			{
				instance->BlendFlag = false;
			}

			instance->NextMotionName.clear();				// Zap next motion

			instance->NeedsNewBB = true;
		}
		// no 'next' motion, lets just default to the 'default' animation instead
		else
		{
			if(instance->DefaultMotionName.empty())
			{
				pMotion = NULL;
			}
			else
			{
				instance->MotionName = instance->DefaultMotionName;

				instance->BlendFlag = false;
				pMotion = geActor_GetMotionByName(theDef, instance->MotionName.c_str());
			}
		}
	}

	instance->pMotion = pMotion;
	instance->pBMotion = NULL;

	float time = instance->PrevAnimationTime = instance->AnimationTime * 0.001f * instance->ActorAnimationSpeed;
	float tStart = 0.f, tEnd = 0.f;

	if(pMotion)
		geMotion_GetTimeExtents(pMotion, &tStart, &tEnd);

	// Perform orienting rotations to properly world-align model
	geVec3d realRotation = instance->BaseRotation;
	geVec3d_Add(&realRotation, &instance->localRotation, &realRotation);

	if(instance->Attached)
		geVec3d_Add(&realRotation, &instance->MasterRotation, &realRotation);

	geXForm3d_SetZRotation(&xForm, realRotation.Z);
	geXForm3d_RotateX(&xForm, realRotation.X);
	geXForm3d_RotateY(&xForm, realRotation.Y);

	// Finally, translate this mother!
	geXForm3d_Translate(&xForm,
						instance->localTranslation.X,
						instance->localTranslation.Y,
						instance->localTranslation.Z);

	// Done, adjust the animation, rotation, and translation
	// ..of our unsuspecting actor
	if(pMotion)
	{
		if(instance->TransitionFlag)
		{
			tEnd = instance->BlendAmount;
			geActor_SetPose(RActor, pMotion, tEnd-time, &xForm);
			pBMotion = geActor_GetMotionByName(theDef, instance->BlendMotionName.c_str());

			if(pBMotion)
			{
				float blendAmount = (instance->BlendAmount - time) / instance->BlendAmount;

				if(blendAmount < 0.0f)
					blendAmount = 0.0f;

				instance->pBMotion = pBMotion;
				geActor_BlendPose(RActor, pBMotion, instance->StartTime, &xForm, blendAmount);
			}
		}
		else
		{
			geActor_SetPose(RActor, pMotion, time, &xForm);

			if(pMotion && instance->BlendFlag)
			{
				pBMotion = geActor_GetMotionByName(theDef, instance->BlendMotionName.c_str());

				if(pBMotion)
				{
					instance->pBMotion = pBMotion;
					geActor_BlendPose(RActor, pBMotion, time, &xForm, instance->BlendAmount);
				}
			}
		}
	}
	else
	{
		geActor_ClearPose(RActor, &xForm);
		instance->AnimationTime = 0.0f;
	}

	// advance player's weapons
	if(instance->Actor == CCD->Player()->GetActor() && !CCD->Player()->GetMonitorMode())
		CCD->Weapons()->Display();

	// advance attached blending actors
	{
		std::vector<geActor*>::iterator iter = instance->AttachedBlendActors.begin();
		for(; iter!=instance->AttachedBlendActors.end(); ++iter)
		{
			ActorInstance *sInstance = LocateInstance(*iter);
			if(sInstance)
			{
				sInstance->localRotation = instance->localRotation;
				sInstance->localTranslation = instance->localTranslation;
				geWorld_SetActorFlags(CCD->World(), sInstance->Actor, instance->RenderFlag & ~GE_ACTOR_COLLIDE);

				if(pMotion)
				{
					if(instance->TransitionFlag)
					{
						geActor_SetPose(sInstance->Actor, pMotion, instance->BlendAmount-time, &xForm);

						pBMotion = geActor_GetMotionByName(theDef, instance->BlendMotionName.c_str());
						if(pBMotion)
						{
							float blendAmount = (instance->BlendAmount - time) / instance->BlendAmount;

							if(blendAmount < 0.0f)
								blendAmount = 0.0f;

							geActor_BlendPose(sInstance->Actor, pBMotion, instance->StartTime, &xForm, blendAmount);
						}
					}
					else
					{
						geActor_SetPose(sInstance->Actor, pMotion, time, &xForm);
						if(instance->BlendFlag)
						{
							pBMotion = geActor_GetMotionByName(theDef, instance->BlendMotionName.c_str());
							if(pBMotion)
							{
								geActor_BlendPose(sInstance->Actor, pBMotion, time, &xForm, instance->BlendAmount);
							}
						}
					}
				}
				else
				{
					geActor_ClearPose(sInstance->Actor, &xForm);
				}
			}

		}
	}

	std::vector<Attachment>::iterator iter = instance->AttachedActors.begin();
	for(; iter!=instance->AttachedActors.end(); ++iter)
	{
		{
			ActorInstance *pInstance = LocateInstance(iter->AttachedActor);

			if(pInstance == NULL)
			{
			}
			else
			{
				geXForm3d Mf;

				pInstance->MasterRotation = instance->localRotation;
				geActor_GetBoneTransform(instance->Actor, iter->MasterBone, &Mf);
				pInstance->localTranslation = Mf.Translation;
				geVec3d realRotation = instance->localRotation;

				if(instance->Attached)
					geVec3d_Add(&realRotation, &instance->MasterRotation, &realRotation);

				geXForm3d_SetZRotation(&Mf, realRotation.Z);
				geXForm3d_RotateX(&Mf, realRotation.X);
				geXForm3d_RotateY(&Mf, realRotation.Y);

				geVec3d Direction;

				geXForm3d_GetIn(&Mf, &Direction);
				geVec3d_AddScaled(&(pInstance->localTranslation), &Direction,
					iter->Offset.Z, &(pInstance->localTranslation));

				geXForm3d_GetUp(&Mf, &Direction);
				geVec3d_AddScaled(&(pInstance->localTranslation), &Direction,
					iter->Offset.Y, &(pInstance->localTranslation));

				geXForm3d_GetLeft(&Mf, &Direction);
				geVec3d_AddScaled(&(pInstance->localTranslation), &Direction,
					iter->Offset.X, &(pInstance->localTranslation));
			}
		}
	}

	// Ok, let's check to see if we've hit the end of this motion,
	// ..If so, and we have a next motion, move to it - otherwise
	// ..just loop the current motion.
	if(pMotion)
	{
		if(time >= tEnd)
		{
			if(!instance->HoldAtEnd)
			{
				// Ok, the animation time has elapsed.  Do we have another queued up?
				if(!instance->NextMotionName.empty())
				{
					instance->MotionName = instance->NextMotionName;

					if(instance->BlendNextFlag)
					{
						instance->BlendFlag = true;
						instance->BlendAmount = instance->BlendNextAmount;
						instance->BlendMotionName = instance->BlendNextMotionName;
					}
					else
					{
						instance->BlendFlag = false;
					}

					instance->NextMotionName.clear();				// Zap next motion
					instance->NeedsNewBB = true;
				}

				// Clear out the animation time
				if(!instance->TransitionFlag)
					instance->AnimationTime = (time - tEnd);
			}

			if(instance->TransitionFlag)
			{
				instance->TransitionFlag = false;
				instance->AnimationTime = time;
			}
		}
		else
		{
			instance->AnimationTime += timeElapsed;						// Time moves!
		}
	}

	// Ok, adjust the actor position for the case where the animation advance
	// ..has created an intersection problem.  Don't check, of course,
	// ..if there's no animation.

	// EVIL HACK: Due to lame-o problems with sudden intersections between
	// ..dynamically computed bounding boxes and animated world models,
	// ..we're going to force a SINGLE EXTBOX FOR EACH ANIMATION, this
	// ..will change ONLY when the current animation changes.  This will
	// ..make Level 1 collision checking (overall AABB) less accurate, but
	// ..will NOT affect Level 2 (bone-based AABB) accuracy.
	if(instance->NeedsNewBB && instance->BoxChange)
	{
		// Adjust the bounding box
		geExtBox ExtBox, maxExtBox;
		geXForm3d OldPlace = xForm;

		// EVIL HACK - in order to form a perfect bounding box that will
		// ..cover every position of this animation, we have to quickly
		// ..fan through the entire timescale for the cycle, creating a
		// .."maximal" bounding box that guarantees our actor won't ever
		// ..end up intersecting world geometry, which makes the Genesis3D
		// ..engine totally blow grits.

		float tStart, tEnd;

		if(!pMotion)
		{
			// No motion?  Just use the current cleared pose OBB
			geActor_GetDynamicExtBox(instance->Actor, &ExtBox);
			// GetDynamicExtBox returns the bounding box in WORLDSPACE, so
			// ..we need to subtract the origin from it to correct for
			// ..this little "feature".  SetExtBox takes the box in MODEL
			// ..SPACE, FYI.
			ExtBox.Min.X -= instance->localTranslation.X;
			ExtBox.Min.Y -= instance->localTranslation.Y;
			ExtBox.Min.Z -= instance->localTranslation.Z;
			ExtBox.Max.X -= instance->localTranslation.X;
			ExtBox.Max.Y -= instance->localTranslation.Y;
			ExtBox.Max.Z -= instance->localTranslation.Z;
			ExtBox.Min.X -= 4.0f;
			ExtBox.Min.Y = 0.0f;
			ExtBox.Min.Z -= 4.0f;
			ExtBox.Max.X += 4.0f;
			ExtBox.Max.Z += 4.0f;
			geActor_SetExtBox(instance->Actor, &ExtBox, NULL);
			// We're going to set the render hint box to match the new
			// ..animation position to help with clipping
			geActor_SetRenderHintExtBox(instance->Actor, &ExtBox, NULL);
		}
		else
		{
			// It's a Real Live Animation, we have to go all the way.
			geMotion_GetTimeExtents(pMotion, &tStart, &tEnd);
			memset(&maxExtBox, 0, sizeof(geExtBox));
			float fTimer = 0.0f;

			while(fTimer < tEnd)
			{
				geActor_SetPose(instance->Actor, pMotion, fTimer, &xForm);
				geActor_GetDynamicExtBox(instance->Actor, &ExtBox);
				// GetDynamicExtBox returns the bounding box in WORLDSPACE, so
				// ..we need to subtract the origin from it to correct for
				// ..this little "feature".  SetExtBox takes the box in MODEL
				// ..SPACE, FYI.
				ExtBox.Min.X -= instance->localTranslation.X;
				ExtBox.Min.Y -= instance->localTranslation.Y;
				ExtBox.Min.Z -= instance->localTranslation.Z;
				ExtBox.Max.X -= instance->localTranslation.X;
				ExtBox.Max.Y -= instance->localTranslation.Y;
				ExtBox.Max.Z -= instance->localTranslation.Z;
				ExtBox.Min.X -= 4.0f;
				ExtBox.Min.Y -= 0.05f;
				ExtBox.Min.Z -= 4.0f;
				ExtBox.Max.X += 4.0f;
				ExtBox.Max.Z += 4.0f;
				geExtBox_Union(&maxExtBox, &ExtBox, &maxExtBox);
				fTimer += 0.5f;			// In half-second increments
			}

			geActor_SetPose(instance->Actor, pMotion, time, &OldPlace);
			maxExtBox.Min.Y = 0.0f;
			geActor_SetExtBox(instance->Actor, &maxExtBox, NULL);
			// We're going to set the render hint box to match the new
			// ..animation position to help with clipping
			geActor_SetRenderHintExtBox(instance->Actor, &ExtBox, NULL);
		}
	}

	// projected shadows
	// only render the bitmap shadows if:
	// 1) we are NOT rendering projected shadows, then render by default
	// 2) we ARE rendering projected shadows, and we are higher than LOD0, render bitmap shadow

	bool RenderBitmapShadow;

	if(instance->ProjectedShadows == true)
	{
		// only render projected shadows at further than closest LOD
		if(instance->LODLevel == 0)
		{
			RenderBitmapShadow = false;
			CCD->PlyShdw()->DrawShadow(instance->Actor);
		}
		else
		{
			RenderBitmapShadow = true;
		}
	}
	else
	{
		RenderBitmapShadow = true;
	}

	if(RenderBitmapShadow)
	{
		if(instance->ShadowSize > 0.0f && instance->ShadowBitmap)
		{
			geVec3d	Pos1, Pos2;
			GE_Collision Collision;
			geVec3d right, up;
			GE_LVertex	vertex[4];
			geVec3d Axis[3];
			geVec3d Impact, normal;
			geXForm3d BoneXForm;

			geActor_GetBoneTransformByIndex(instance->Actor, 0, &BoneXForm);
			Pos1 = BoneXForm.Translation;
			Pos2 = Pos1;
			Pos2.Y -= 30000.0f;

			geWorld_Collision(CCD->World(), NULL, NULL,
				&Pos1, &Pos2, GE_CONTENTS_SOLID_CLIP | GE_CONTENTS_WINDOW,
				GE_COLLIDE_MODELS, 0x0, NULL, NULL, &Collision);

			Impact = Collision.Impact;
			normal = Collision.Plane.Normal;

			float dist = geVec3d_DistanceBetween(&Pos1, &Impact);

			if(dist<(instance->ShadowSize*2.0f))
			{
				for(int i=0; i<4; ++i)
				{
					// texture coordinates
					vertex[i].u = 0.0f;
					vertex[i].v = 0.0f;
					// color
					vertex[i].r = 24.0f;
					vertex[i].g = 24.0f;
					vertex[i].b = 24.0f;
					vertex[i].a = instance->ShadowAlpha;
				}

				vertex[3].u = 1.0f;
				vertex[2].u = 1.0f;
				vertex[2].v = 1.0f;
				vertex[1].v = 1.0f;

				for(int i=0; i<3; ++i)
				{
					Axis[i].X = 0.0f;
					Axis[i].Y = 0.0f;
					Axis[i].Z = 0.0f;
				}

				Axis[0].X = 1.0f;
				Axis[1].Y = 1.0f;
				Axis[2].Z = 1.0f;

				int major = 0;

				if(fabs(normal.Y) > fabs(normal.X))
				{
					major = 1;

					if(fabs(normal.Z) > fabs(normal.Y))
						major = 2;
				}
				else
				{
					if(fabs(normal.Z) > fabs(normal.X))
						major = 2;
				}

				if(fabs(normal.X) == 1.0f || fabs(normal.Y) == 1.0f || fabs(normal.Z) == 1.0f)
				{
					if((major == 0 && normal.X > 0) || major == 1)
					{
						right.X = 0.0f;
						right.Y = 0.0f;
						right.Z = -1.0f;
					}
					else if(major == 0)
					{
						right.X = 0.0f;
						right.Y = 0.0f;
						right.Z = 1.0f;
					}
					else
					{
						right.X = normal.Z;
						right.Y = 0.0f;
						right.Z = 0.0f;
					}
				}
				else
				{
					geVec3d_CrossProduct(&Axis[major], &normal, &right);
				}

				InVector(instance->Actor, &up);
				LeftVector(instance->Actor, &right);
				geVec3d_Inverse(&right);

				geVec3d_CrossProduct(&normal, &right, &up);
				geVec3d_Normalize(&up);
				geVec3d_Normalize(&right);
				geVec3d_Scale(&right, (instance->ShadowSize - (dist * 0.5f)) * 0.5f, &right);
				geVec3d_Scale(&up, (instance->ShadowSize - (dist * 0.5f)) * 0.5f, &up);

				geVec3d_MA(&Impact, 0.4f, &normal, &Impact);

				// calculate vertices from corners
				vertex[1].X = Impact.X + (-right.X - up.X);
				vertex[1].Y = Impact.Y + (-right.Y - up.Y);
				vertex[1].Z = Impact.Z + (-right.Z - up.Z);

				vertex[2].X = Impact.X + (right.X - up.X);
				vertex[2].Y = Impact.Y + (right.Y - up.Y);
				vertex[2].Z = Impact.Z + (right.Z - up.Z);

				vertex[3].X = Impact.X + (right.X + up.X);
				vertex[3].Y = Impact.Y + (right.Y + up.Y);
				vertex[3].Z = Impact.Z + (right.Z + up.Z);

				vertex[0].X = Impact.X + (-right.X + up.X);
				vertex[0].Y = Impact.Y + (-right.Y + up.Y);
				vertex[0].Z = Impact.Z + (-right.Z + up.Z);

				geWorld_AddPolyOnce(
					CCD->World(),
					vertex,
					4,
					instance->ShadowBitmap,
					GE_TEXTURED_POLY,
					GE_RENDER_DO_NOT_OCCLUDE_OTHERS | GE_RENDER_DEPTH_SORT_BF,
					1.0f);
			}
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// UpdateActorState
//
// Updates the actors position in the world to reflect the internal
// ..object state
/* ------------------------------------------------------------------------------------ */
void CActorManager::UpdateActorState(ActorInstance *instance)
{
	if(instance->AttachedB)
		return;

	geMotion *pMotion;
	geXForm3d xForm;

	if(instance->MotionName.empty())
		pMotion = NULL;
	else
		pMotion = geActor_GetMotionByName(instance->theDef,	instance->MotionName.c_str()); // Get the motion name

	float time = instance->AnimationTime * 0.001f;
	float tStart, tEnd;

	if(pMotion)
		geMotion_GetTimeExtents(pMotion, &tStart, &tEnd);

	// Perform orienting rotations to properly world-align model
	geXForm3d_SetZRotation(&xForm, instance->BaseRotation.Z + instance->localRotation.Z);
	geXForm3d_RotateX(&xForm, instance->BaseRotation.X + instance->localRotation.X);
	geXForm3d_RotateY(&xForm, instance->BaseRotation.Y + instance->localRotation.Y);

	// Finally, translate this mother!
	geXForm3d_Translate(&xForm, instance->localTranslation.X,
		instance->localTranslation.Y, instance->localTranslation.Z);


	// Done, adjust the animation, rotation, and translation
	// ..of our unsuspecting actor
	if(pMotion)
		geActor_SetPose(instance->Actor, pMotion, time, &xForm);
	else
		geActor_ClearPose(instance->Actor, &xForm);

	// update attached blending actors
	{
		std::vector<geActor*>::iterator iter = instance->AttachedBlendActors.begin();
		for(; iter!=instance->AttachedBlendActors.end(); ++iter)
		{
			ActorInstance *sInstance = LocateInstance(*iter);
			if(sInstance)
			{
				sInstance->localRotation = instance->localRotation;
				sInstance->localTranslation = instance->localTranslation;
				if(pMotion)
					geActor_SetPose(sInstance->Actor, pMotion, time, &xForm);
				else
					geActor_ClearPose(sInstance->Actor, &xForm);
			}
		}
	}
}

/* ------------------------------------------------------------------------------------ */
// ProcessGravity
//
// Perform gravity processing for a specific actor.
/* ------------------------------------------------------------------------------------ */
void CActorManager::ProcessGravity(ActorInstance *instance, float timeElapsed)
{
	if(instance->Gravity.X == 0.0f && instance->Gravity.Y == 0.0f && instance->Gravity.Z == 0.0f)
		return;											// Don't bother, no gravity for this actor

	// Ok, we have gravity to deal with, let's do it!

	geVec3d	movespeed;
	bool	Result;
	GE_Collision Collision;
	geVec3d oldpos, newpos;

	// If in Zero-G, don't apply gravity!
	int nZoneType = instance->CurrentZone;

	if(nZoneType == kNoGravityZone || nZoneType & kClimbLaddersZone)
	{
		instance->GravityTime = 0.0f;					// Patch up the timings
		return;											// Woah, games in space...
	}

	instance->GravityTime += timeElapsed;				// Increase it...

	// If we're in water, well, there's no acceleration towards the bottom
	// ..due to water friction and it's slow, so let's simulate it.
	if(nZoneType == kLiquidZone &&
		(instance->Actor == CCD->Player()->GetActor() || instance->ActorType == ENTITY_NPC ||
		instance->ActorType == ENTITY_VEHICLE || instance->ActorType == ENTITY_PROP))
	{
		if(instance->Moving)
		{
			instance->GravityTime = 0.0f;				// Patch up the timings
			return;
		}

		geVec3d_Scale(&instance->Gravity,
				(instance->GravityTime * 0.001f) * (static_cast<float>(instance->GravityCoeff) * 0.01f),
				&movespeed); // Slow sinking to bottom
	}
	else
	{
		geVec3d_Scale(&instance->Gravity, (instance->GravityTime * 0.001f), &movespeed);
	}

	if(movespeed.X == 0.0f && movespeed.Y == 0.0f && movespeed.Z == 0.0f)
		return;

	oldpos = instance->localTranslation;
	geVec3d_AddScaled(&oldpos, &movespeed, timeElapsed * 0.001f, &newpos); // Move

	// EVIL HACK:  There is some problem with bone-box collision checking
	// ..when doing gravity processing, so for now (or until someone
	// ..finds the problem and fixes it) gravity collision detection is
	// ..at the overall bounding box stage only.

	CCD->Collision()->IgnoreContents(false);
	CCD->Collision()->CheckLevel(RGF_COLLISIONLEVEL_1);

	if(instance->PassengerModel)
	{
		CCD->ModelManager()->RemovePassenger(instance->PassengerModel, instance->Actor);
		instance->PassengerModel = NULL;
	}

	int nHitType = kNoCollision;

	Result = CCD->Collision()->CheckActorCollision(oldpos, newpos, instance->Actor,	&Collision);

	if(Result == true)					// Your new position collides with something
	{
		instance->GravityCollision = Collision;

		// Process the collision event.
		nHitType = CCD->Collision()->ProcessCollision(Collision, instance->Actor, true);
		//geEngine_Printf(CCD->Engine()->Engine(), 0, 10, "HitType = %d", nHitType);
		//geEngine_Printf(CCD->Engine()->Engine(), 0, 20, "Model = %x", Collision.Model);

		// Weapon
		if(nHitType == kNoCollision || nHitType == kCollideWeapon)
		{
			instance->localTranslation = newpos;
			UpdateActorState(instance);
			return;
		}

		// Trigger
		if(nHitType == kCollideTrigger)
		{
			UpdateActorState(instance);
			return;
		}

		// Vehicle
		if(nHitType == kCollideVehicle)
		{
			SetPassenger(instance->Actor, Collision.Actor);
			SetVehicle(instance->Actor, Collision.Actor);
			instance->GravityTime = 0.0f;
			instance->localTranslation = Collision.Impact;
			UpdateActorState(instance);
			return;
		}

		// Platform
		if(nHitType == kCollidePlatform)
		{
			CCD->ModelManager()->AddPassenger(Collision.Model, instance->Actor);
			instance->PassengerModel = Collision.Model;
			instance->GravityTime = 0.0f;
			instance->localTranslation = Collision.Impact;
			UpdateActorState(instance);
			return;
		}

		// Model, Door or Actor
		if((nHitType == kCollideWorldModel) || (nHitType == kCollideDoor) || (nHitType == kCollideActor))
		{
			// Model hit, and dealt with.
			instance->GravityTime = 0.0f;							// Same as "hitting the ground"
			instance->localTranslation = Collision.Impact;
			UpdateActorState(instance);
			return;
		}

		if(Collision.Plane.Normal.Y <= CCD->Player()->GetSlideSlope() ||
			(nZoneType & kUnclimbableZone) ||
			(nZoneType == kLiquidZone)) // non-climbable slope
		{
			float Sspeed = (1.0f - Collision.Plane.Normal.Y) * CCD->Player()->GetSlideSpeed();

			geVec3d_AddScaled(&newpos, &movespeed, Sspeed, &newpos); // Make it a faster slide
			float Slide = geVec3d_DotProduct (&newpos, &Collision.Plane.Normal) - Collision.Plane.Dist;

			newpos.X -= Collision.Plane.Normal.X * Slide;
			newpos.Y -= Collision.Plane.Normal.Y * Slide;
			newpos.Z -= Collision.Plane.Normal.Z * Slide;

			// Check to see if we can get away with sliding a bit when
			// ..we hit whatever we hit...
			if(CCD->Collision()->CheckActorCollision(oldpos, newpos, instance->Actor, &Collision) == true)
			{
				newpos = Collision.Impact;	// set new position to the point of collision.
			}

			instance->GravityTime = 0.0f;
		}
		else // We are on the ground.
		{
			instance->GravityTime = 0.0f;
			newpos = Collision.Impact;
		}
	}

	instance->OldTranslation = instance->localTranslation;
	instance->localTranslation = newpos;
	UpdateActorState(instance);
}

/* ------------------------------------------------------------------------------------ */
// ProcessForce
//
// Perform force additions for a specific actor
/* ------------------------------------------------------------------------------------ */
void CActorManager::ProcessForce(ActorInstance *instance, float timeElapsed)
{
	geVec3d newpos;

	for(int i=0; i<4; ++i)
	{
		instance->ForceLevel[i] -= instance->ForceDecay[i] * (timeElapsed / 900.0f);

		if(instance->ForceLevel[i] <= 0.0f)
		{
			instance->ForceLevel[i] = 0.0f;
			continue;
		}

		// Ok, we have a force, process it!  Forces act only on translations, we
		// ..don't have any rotational forces... right now...
		geVec3d_AddScaled(&instance->localTranslation, &instance->ForceVector[i],
			(instance->ForceLevel[i] * timeElapsed * 0.005f), &newpos);

		// We have the new position, do a collision check and all that...
		if(ValidateMotion(instance->localTranslation, newpos, instance, true, instance->slide) == GE_FALSE)
		{
			instance->ForceDecay[i] = 0.0f;
			instance->ForceLevel[i] = 0.0f;
			continue;
		}
	}

	UpdateActorState(instance);
}

/* ------------------------------------------------------------------------------------ */
// Move
//
// Move the actor forward/backward/left/right at a specific speed.
/* ------------------------------------------------------------------------------------ */
int CActorManager::Move(ActorInstance *pInstance, int nHow, float speed)
{
	if(pInstance->AttachedB)
		return RGF_SUCCESS;

	geXForm3d xForm;
	geVec3d in, newPosition, savedPosition;

	savedPosition = pInstance->localTranslation;			// Save for later.
	newPosition = savedPosition;

	// Note that the only thing we're interested in here is the
	// ..DIRECTION we're facing, so we can move forward or backward.
	// ..Ignore the X and Z rotations so we can still move forward
	// ..when we're facing UP.  **OF COURSE** This isn't true when
	// ..we're in the water or in ZeroG

	// 11/04/1999 An interesting note - be careful in checking collisions
	// ..with GE_CONTENTS_CANNOT_OCCUPY, "empty" brushes collide with
	// ..that flag.  Use GE_CONTENTS_SOLID_CLIP, empty models don't trigger
	// ..a collision then.

	if((nHow == RGF_K_BACKWARD) || (nHow == RGF_K_RIGHT))
		speed = -speed;										// Moving backward||right here

	// Ok, what kind of substance we're in affects our motion direction,
	// ..so let's deal with that.
	int nZoneType = pInstance->CurrentZone;

	//geEngine_Printf(CCD->Engine()->Engine(), 0,10,"Zone = %x",nZoneType);

	if((nZoneType & kClimbLaddersZone))
	{
		if(pInstance->Actor == CCD->Player()->GetActor() && CCD->Player()->GetViewPoint() != FIRSTPERSON)
		{
			if(nHow == RGF_K_FORWARD || nHow == RGF_K_BACKWARD)
			{
				geXForm3d_SetTranslation(
					&xForm,
					pInstance->localTranslation.X,
					pInstance->localTranslation.Y,
					pInstance->localTranslation.Z);

				geXForm3d_GetUp(&xForm, &in);
				newPosition = pInstance->localTranslation;				// From the old position...
				savedPosition = pInstance->localTranslation;			// Back this up..
				geVec3d_AddScaled(&savedPosition, &in, speed, &newPosition);

				if(nHow == RGF_K_BACKWARD)
				{
					GE_Collision Collision;

					if(CCD->Collision()->CheckActorCollision(savedPosition, newPosition, pInstance->Actor, &Collision))
					{
						geXForm3d_SetYRotation(&xForm, pInstance->localRotation.Y);

						geXForm3d_Translate(
							&xForm,
							pInstance->localTranslation.X,
							pInstance->localTranslation.Y,
							pInstance->localTranslation.Z);

						geXForm3d_GetIn(&xForm, &in);								// get forward vector
						newPosition = pInstance->localTranslation;						// From the old position...
						savedPosition = pInstance->localTranslation;					// Back this up..
						geVec3d_AddScaled(&savedPosition, &in, speed, &newPosition);
					}
				}
			}
			else
			{
				geXForm3d_SetZRotation(&xForm, pInstance->localRotation.Z);

				if(pInstance->AllowTilt)
					geXForm3d_RotateX(&xForm, pInstance->localRotation.X);
				else
					geXForm3d_RotateX(&xForm, pInstance->TiltX);

				geXForm3d_RotateY(&xForm, pInstance->localRotation.Y);
				geXForm3d_Translate(
					&xForm,
					pInstance->localTranslation.X,
					pInstance->localTranslation.Y,
					pInstance->localTranslation.Z);

				geXForm3d_GetLeft(&xForm, &in);
				newPosition = pInstance->localTranslation;				// From the old position...
				savedPosition = pInstance->localTranslation;			// Back this up..
				geVec3d_AddScaled(&savedPosition, &in, speed, &newPosition);
			}
		}
		else if(pInstance->Actor == CCD->Player()->GetActor())
		{
			if((nHow != RGF_K_FORWARD))
			{
				geXForm3d_SetZRotation(&xForm, pInstance->localRotation.Z);

				if(pInstance->AllowTilt)
					geXForm3d_RotateX(&xForm, pInstance->localRotation.X);
				else
					geXForm3d_RotateX(&xForm, pInstance->TiltX);

				geXForm3d_RotateY(&xForm, pInstance->localRotation.Y);
				geXForm3d_Translate(
					&xForm,
					pInstance->localTranslation.X,
					pInstance->localTranslation.Y,
					pInstance->localTranslation.Z);

				if((nHow == RGF_K_BACKWARD))
					geXForm3d_GetIn(&xForm, &in);						// get forward vector
				else
					geXForm3d_GetLeft(&xForm, &in);

				newPosition = pInstance->localTranslation;				// From the old position...
				savedPosition = pInstance->localTranslation;			// Back this up..
				geVec3d_AddScaled(&savedPosition, &in, speed, &newPosition);
			}
			else
			{
				float Tilt;

				geXForm3d_SetTranslation(
					&xForm,
					pInstance->localTranslation.X,
					pInstance->localTranslation.Y,
					pInstance->localTranslation.Z);

				if(pInstance->AllowTilt)
					Tilt = pInstance->localRotation.X;
				else
					Tilt = pInstance->TiltX;

				if(Tilt < 0.0f)
					speed = -speed;

				geXForm3d_GetUp(&xForm, &in);
				newPosition = pInstance->localTranslation;				// From the old position...
				savedPosition = pInstance->localTranslation;			// Back this up..
				geVec3d_AddScaled(&savedPosition, &in, speed, &newPosition);

				if(Tilt < 0.0f)
				{
					GE_Collision Collision;

					if(CCD->Collision()->CheckActorCollision(savedPosition, newPosition, pInstance->Actor, &Collision))
					{
						speed = -speed;

						geXForm3d_SetYRotation(&xForm, pInstance->localRotation.Y);

						geXForm3d_Translate(
							&xForm,
							pInstance->localTranslation.X,
							pInstance->localTranslation.Y,
							pInstance->localTranslation.Z);

						geXForm3d_GetIn(&xForm, &in);								// get forward vector
						newPosition = pInstance->localTranslation;					// From the old position...
						savedPosition = pInstance->localTranslation;				// Back this up..
						geVec3d_AddScaled(&savedPosition, &in, speed, &newPosition);
					}
				}
			}
		}
	}
	else if((nZoneType == kLiquidZone) || (nZoneType == kNoGravityZone))
	{
		// In water or zero-G, go the direction the actor is facing
		geXForm3d_SetZRotation(&xForm, pInstance->localRotation.Z);

		if(pInstance->AllowTilt)
		{
			geXForm3d_RotateX(&xForm, pInstance->localRotation.X);
		}
		else
		{
			if(pInstance->Actor == CCD->Player()->GetActor())
			{
				if(CCD->Player()->GetViewPoint() == FIRSTPERSON)
					geXForm3d_RotateX(&xForm, CCD->CameraManager()->GetTilt());
				else
					geXForm3d_RotateX(&xForm, pInstance->localRotation.X-CCD->CameraManager()->GetTilt());
			}
			else
			{
				geXForm3d_RotateX(&xForm, pInstance->TiltX);
			}
		}

		geXForm3d_RotateY(&xForm, pInstance->localRotation.Y);
		geXForm3d_Translate(
			&xForm,
			pInstance->localTranslation.X,
			pInstance->localTranslation.Y,
			pInstance->localTranslation.Z);

		if((nHow == RGF_K_FORWARD) || (nHow == RGF_K_BACKWARD))
			geXForm3d_GetIn(&xForm, &in);						// get forward vector
		else
			geXForm3d_GetLeft(&xForm, &in);

		newPosition = pInstance->localTranslation;				// From the old position...
		savedPosition = pInstance->localTranslation;			// Back this up..
		geVec3d_AddScaled(&savedPosition, &in, speed, &newPosition);
	}
	else
	{
		geXForm3d_SetYRotation(&xForm, pInstance->localRotation.Y);
		geXForm3d_Translate(
			&xForm,
			pInstance->localTranslation.X,
			pInstance->localTranslation.Y,
			pInstance->localTranslation.Z);

		if((nHow == RGF_K_FORWARD) || (nHow == RGF_K_BACKWARD))
			geXForm3d_GetIn(&xForm, &in);							// get forward vector
		else
			geXForm3d_GetLeft(&xForm, &in);

		newPosition = pInstance->localTranslation;					// From the old position...
		savedPosition = pInstance->localTranslation;				// Back this up..
		geVec3d_AddScaled(&savedPosition, &in, speed, &newPosition); // Move
	}

	// Do a fast probe ahead/behind to see if we need to go further

	GE_Collision theCollision;
	bool stepUpOK = true;

	if(CCD->Collision()->Probe(xForm, speed*10, &theCollision) == GE_TRUE)
	{
		if((theCollision.Plane.Normal.Y < 0.3f) &&
			(theCollision.Plane.Normal.Y > 0.0f))
			stepUpOK = false;				// No stepping up ahead
	}

	// Ok, we've moved.  Check to see if we just ran into something,
	// ..and if so, don't _really_ move.

	if(ValidateMotion(savedPosition, newPosition, pInstance, stepUpOK, pInstance->slide) == GE_TRUE)
	{
		pInstance->OldTranslation = savedPosition;		// Update old position
		return RGF_SUCCESS;
	}

	return RGF_FAILURE;
}

/* ------------------------------------------------------------------------------------ */
// CheckForStepUp
//
// If, during movement, we have a collision,
// ..we need to see if we're just hitting the front face of something
// ..we can step up on.  If so, we need to step up on it!
/* ------------------------------------------------------------------------------------ */
bool CActorManager::CheckForStepUp(ActorInstance *pInstance, geVec3d newPosition)
{
	float tempY = 0.0f;
	geVec3d oldPosition;
	geExtBox extBox;

	// weapon
	if(pInstance->MaxStepHeight == -1.0f)
		return false;

	// Get AABB
	GetBoundingBox(pInstance->Actor, &extBox);

	oldPosition = pInstance->localTranslation;

	while(tempY <= pInstance->MaxStepHeight)
	{
		tempY += 0.5f;
		oldPosition.Y += 0.5f;
		newPosition.Y += 0.5f;

		// See if there's a clear path now we're elevated...
		if(CCD->Collision()->CheckActorCollision(oldPosition, newPosition, pInstance->Actor) == false)
		{
			pInstance->localTranslation = newPosition;
			return true;
		}
		else
		{
			if(CCD->Collision()->CanOccupyPosition(&newPosition, &extBox, pInstance->Actor) == true)
			{
				pInstance->localTranslation = newPosition;
				return true;
			}
		}
	}

	return false;	// No step up.
}

/* ------------------------------------------------------------------------------------ */
// GetCurrentZone
//
// Get the brush contents for the actors bounding box, if any.
/* ------------------------------------------------------------------------------------ */
int CActorManager::GetCurrentZone(ActorInstance *pInstance)
{
	GE_Contents ZoneContents;
	int nZoneType;
	geExtBox theExtBox;
	float MinY, MaxY;
	Liquid *LQ;

	// Get AABB
	geActor_GetExtBox(pInstance->Actor, &theExtBox);

	theExtBox.Min.X -= pInstance->localTranslation.X;
	MinY = theExtBox.Min.Y -= pInstance->localTranslation.Y;
	theExtBox.Min.Z -= pInstance->localTranslation.Z;
	theExtBox.Max.X -= pInstance->localTranslation.X;
	theExtBox.Max.Y -= pInstance->localTranslation.Y;
	theExtBox.Max.Z -= pInstance->localTranslation.Z;

	pInstance->OldZone = 0;

	theExtBox.Min.Y += (theExtBox.Max.Y-theExtBox.Min.Y)*0.60f;

	if(CCD->Collision()->GetContentsOf(pInstance->localTranslation, &theExtBox, &ZoneContents) == RGF_SUCCESS)
	{
		//geEngine_Printf(CCD->Engine()->Engine(), 0,20,"Model = %x",ZoneContents.Model);
		LQ = CCD->Level()->Liquids()->IsLiquid(ZoneContents.Model);

		if(LQ)
		{
			pInstance->LQ = LQ;
			pInstance->OldZone = 1;
		}
	}

	theExtBox.Min.Y = MinY + (theExtBox.Max.Y-MinY)*0.80f;

	if(CCD->Collision()->GetContentsOf(pInstance->localTranslation, &theExtBox, &ZoneContents) == RGF_SUCCESS)
	{
		LQ = CCD->Level()->Liquids()->IsLiquid(ZoneContents.Model);

		if(LQ)
		{
			pInstance->LQ = LQ;
			pInstance->OldZone = 2;
		}
	}

	//geEngine_Printf(CCD->Engine()->Engine(), 0,20,"OldZone = %x",pInstance->OldZone);

	theExtBox.Min.Y = MinY;
	// Get the contents of that box!
	MaxY = theExtBox.Min.Y += (theExtBox.Max.Y-theExtBox.Min.Y)*0.70f;

	if(CCD->Collision()->GetContentsOf(pInstance->localTranslation, &theExtBox, &ZoneContents) == RGF_SUCCESS)
	{
		LQ = CCD->Level()->Liquids()->IsLiquid(ZoneContents.Model);
		//geEngine_Printf(CCD->Engine()->Engine(), 0,20,"Model = %x",ZoneContents.Model);

		if(LQ)
		{
			pInstance->LQ = LQ;
			pInstance->GravityCoeff = LQ->GravityCoeff;
			nZoneType = kLiquidZone;
			return nZoneType;
		}
	}

	nZoneType = 0;
	theExtBox.Max.Y = MaxY;
	theExtBox.Min.Y = MinY;

	if(CCD->Collision()->GetContentsOf(pInstance->localTranslation, &theExtBox, &ZoneContents) == RGF_SUCCESS)
	{
		LQ = CCD->Level()->Liquids()->IsLiquid(ZoneContents.Model);

		if(LQ)
		{
			pInstance->LQ = LQ;
			nZoneType = kInLiquidZone;
		}
	}

	geActor_GetExtBox(pInstance->Actor, &theExtBox);

	theExtBox.Min.X -= pInstance->localTranslation.X;
	theExtBox.Min.Y -= pInstance->localTranslation.Y;
	theExtBox.Min.Z -= pInstance->localTranslation.Z;
	theExtBox.Max.X -= pInstance->localTranslation.X;
	theExtBox.Max.Y -= pInstance->localTranslation.Y;
	theExtBox.Max.Z -= pInstance->localTranslation.Z;

	theExtBox.Min.Y -= 0.5f;

	if(geWorld_GetContents(CCD->World(), &pInstance->localTranslation, &theExtBox.Min,
		&theExtBox.Max, GE_COLLIDE_MODELS, 0, NULL, NULL, &ZoneContents) == GE_TRUE)
	{
		unsigned int Mask = 0x00010000;

		for(int i=0; i<16; ++i)
		{
			switch(ZoneContents.Contents & (Mask<<i))
			{
			case Water:
				nZoneType |= kWaterZone;
				break;
			case Lava:
				nZoneType |= kLavaZone;
				break;
			case ToxicGas:
				nZoneType |= kToxicGasZone;
				break;
			case ZeroG:
				nZoneType |= kNoGravityZone;
				break;
			case Frozen:
				nZoneType |= kFrozenZone;
				break;
			case Sludge:
				nZoneType |= kSludgeZone;
				break;
			case SlowMotion:
				nZoneType |= kSlowMotionZone;
				break;
			case FastMotion:
				nZoneType |= kFastMotionZone;
				break;
			case Ladder:
				nZoneType |= kClimbLaddersZone;
				break;
			case Impenetrable:
				nZoneType |= kImpassibleZone;
				break;
			case Unclimbable:
				nZoneType |= kUnclimbableZone;
				break;
			default:
				nZoneType |= kNormalZone;
				break;
			}
		}
	}
	else
	{
		// If we're standing on a HOLLOW MODEL, then contents set in the editor
		// ..ARE NOT STORED IN THE BSP!  Damn! So we make use of the MODEL ATTRIBUTES
		// ..entity and we'll check with the Model Manager and see if there are any
		// ..attributes for the model we're on...
		ModelAttributes *theAttributes = CCD->ModelManager()->GetAttributes(ZoneContents.Model);

		if(theAttributes == NULL)
		{
			nZoneType = kNormalZone;	// No contents, no problem!
		}
		else
		{
			// Return the appropriate zone for the attributes
			if(theAttributes->Frozen == GE_TRUE)
				nZoneType = kFrozenZone;
			else if(theAttributes->Lava == GE_TRUE)
				nZoneType = kLavaZone;
			else if(theAttributes->Sludge == GE_TRUE)
				nZoneType = kSludgeZone;
			else if(theAttributes->Water == GE_TRUE)
				nZoneType = kWaterZone;
			else if(theAttributes->Ladder == GE_TRUE)
				nZoneType = kClimbLaddersZone;
			else if(theAttributes->ToxicGas == GE_TRUE)
				nZoneType = kToxicGasZone;
			else if(theAttributes->ZeroG == GE_TRUE)
				nZoneType = kNoGravityZone;
			else if(theAttributes->SlowMotion == GE_TRUE)
				nZoneType = kSlowMotionZone;
			else if(theAttributes->FastMotion == GE_TRUE)
				nZoneType = kFastMotionZone;
			else if(theAttributes->Impassible == GE_TRUE)
				nZoneType = kImpassibleZone;
			else if(theAttributes->UnClimbable == GE_TRUE)
				nZoneType = kUnclimbableZone;

			delete theAttributes;
		}
	}

	return nZoneType;											// Type to caller
}

/* ------------------------------------------------------------------------------------ */
// ValidateMove
/* ------------------------------------------------------------------------------------ */
geBoolean CActorManager::ValidateMove(const geVec3d& startPos, const geVec3d &endPos,
									  const geActor *actor, bool slide)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;  // Actor not found?!?!

	geVec3d tempPos = pInstance->localTranslation;
	bool result = ValidateMotion(startPos, endPos, pInstance, true, slide);

	if(result == GE_TRUE)
		pInstance->OldTranslation = tempPos;

	UpdateActorState(pInstance);
	return result;
}

/* ------------------------------------------------------------------------------------ */
// ValidateMotion
//
// Given a START and END position, check to see if the motion is valid,
// ..that is, if no collision occurs.  If a collision does happen, then
// ..process the collision event.
/* ------------------------------------------------------------------------------------ */
geBoolean CActorManager::ValidateMotion(geVec3d startPos, geVec3d endPos,
										ActorInstance *pInstance, bool stepUpOK, bool slide)
{
	GE_Collision collision;
	bool result, stepUp;

	CCD->Collision()->IgnoreContents(false);
	CCD->Collision()->CheckLevel(RGF_COLLISIONLEVEL_1); // RGF_COLLISIONLEVEL_2

	// Start off with the infamous Collision Check.
	{
		std::vector<Attachment>::iterator iter = pInstance->AttachedActors.begin();
		for(; iter!=pInstance->AttachedActors.end(); ++iter)
		{
			ActorInstance *mInstance = LocateInstance(iter->AttachedActor);

			if(mInstance)
			{
				iter->CollFlag = mInstance->NoCollision;
				mInstance->NoCollision = true;
				geWorld_SetActorFlags(CCD->World(), mInstance->Actor, GE_ACTOR_RENDER_NORMAL | GE_ACTOR_RENDER_MIRRORS);
				iter->RenderFlag = mInstance->RenderFlag;
			}
		}
	}


	int hitType = kNoCollision;
	geBoolean eFlag;

	do
	{
		result = CCD->Collision()->CheckActorCollision(startPos, endPos, pInstance->Actor, &collision);

		if(result == true)
		{
			// Argh, we hit something!  Check to see if its a world model.
			hitType = CCD->Collision()->ProcessCollision(collision, pInstance->Actor, false);

			if(hitType == kNoCollision || hitType == kCollideWeapon)
			{
				pInstance->localTranslation = endPos;
				eFlag = GE_TRUE;
				goto exitcoll;
			}

			if(hitType == kCollideDoor)
			{
				eFlag = GE_FALSE;					// If we hit an door, exit.
				goto exitcoll;
			}

			if(pInstance->ActorType == ENTITY_VEHICLE)
			{
				if(pInstance->Passengers && hitType == kCollideActor)
				{
					pInstance->localTranslation = endPos;
					eFlag = GE_TRUE;
					goto exitcoll;
				}
			}

			if(hitType == kCollideTrigger)
			{
				eFlag = GE_FALSE;
				goto exitcoll;
			}

			if(hitType == kCollideVehicle)
				SetVehicle(pInstance->Actor, collision.Actor);

			if(!pInstance->AutoStepUp)
			{
				eFlag = GE_FALSE;
				goto exitcoll;
			}

			int nZoneType = pInstance->CurrentZone;

			if(nZoneType & kUnclimbableZone)
			{
				eFlag = GE_FALSE;
				goto exitcoll;
			}

			if(stepUpOK)
				stepUp = CheckForStepUp(pInstance, endPos);
			else
				stepUp = false;

			if(stepUp == false)
			{
				if(!(CCD->MenuManager()->GetNoClip()))
				{
					// don't do any calculations/collision-tests if we don't want to slide at all
					if(!slide)
					{
						// No good move, back to where we came from
						pInstance->localTranslation = startPos;
						eFlag = GE_FALSE;
						goto exitcoll;
					}

					float Slide = 1.0f;
					// the ground doesn't allow normal sliding if Collision.Plane.Normal.Y < 0.0f

					// division by 0 if Y == -1 !
					if(collision.Plane.Normal.Y < 0.0f && collision.Plane.Normal.Y > -1.0f)
					{
						Slide = geVec3d_DotProduct(&endPos, &collision.Plane.Normal) - collision.Plane.Dist;
						endPos.X -= collision.Plane.Normal.X * Slide;
						endPos.Y = collision.Impact.Y;
						endPos.Z -= collision.Plane.Normal.Z * Slide;

						Slide = (collision.Plane.Dist - geVec3d_DotProduct(&endPos, &(collision.Plane.Normal)))/
									(collision.Plane.Normal.X * collision.Plane.Normal.X +
									 collision.Plane.Normal.Z * collision.Plane.Normal.Z);

						endPos.X += collision.Plane.Normal.X * Slide;
						endPos.Z += collision.Plane.Normal.Z * Slide;
					}
					else
					{
						Slide = geVec3d_DotProduct(&endPos, &collision.Plane.Normal) - collision.Plane.Dist;
						endPos.X -= collision.Plane.Normal.X * Slide;
						endPos.Y -= collision.Plane.Normal.Y * Slide;
						endPos.Z -= collision.Plane.Normal.Z * Slide;
					}

					CCD->Collision()->IgnoreContents(false);
					CCD->Collision()->CheckLevel(RGF_COLLISIONLEVEL_1); // RGF_COLLISIONLEVEL_2

					if(CCD->Collision()->CheckActorCollision(startPos, endPos, pInstance->Actor, &collision) == true)
					{
						// No good move, back to where we came from
						pInstance->localTranslation = startPos;
						eFlag = GE_FALSE;
						goto exitcoll;
					}
					else
					{
						// Motion OK, go for it
						pInstance->localTranslation = endPos;
					}
				}
				else
				{
					pInstance->localTranslation = endPos;
				}
			}
		}
		else
		{
			// Movement is OK, no collision in motion direction.
			pInstance->localTranslation = endPos;						// Movement is OK with me.
		}
	}while(hitType == kCollideRecheck);

	eFlag = GE_TRUE;

exitcoll:

	{
		std::vector<Attachment>::iterator iter = pInstance->AttachedActors.begin();
		for(; iter!=pInstance->AttachedActors.end(); ++iter)
		{
			ActorInstance *mInstance = LocateInstance(iter->AttachedActor);

			if(mInstance)
			{
				mInstance->NoCollision = iter->CollFlag;
				geWorld_SetActorFlags(CCD->World(), mInstance->Actor, iter->RenderFlag);
				mInstance->RenderFlag = iter->RenderFlag;
			}
		}
	}

	return eFlag;											// Motion fine and was done.
}

/* ------------------------------------------------------------------------------------ */
// MoveAllVehicles
//
// We need to move the vehicle actors before any other class of
// ..actor gets moved.  This function goes through and takes
// ..care of moving any vehicle-type actors...
/* ------------------------------------------------------------------------------------ */
void CActorManager::MoveAllVehicles(LoadedActorList* entry, float timeElapsed)
{
	ActorInstance *pInstance = entry->IList;

	while(pInstance != NULL)
	{
		if(pInstance->ActorType == ENTITY_VEHICLE)
		{
			if(pInstance->ForceEnabled)
				ProcessForce(pInstance, timeElapsed);		// Process any forces
			ProcessGravity(pInstance, timeElapsed);			// Process any gravity
			AdvanceInstanceTime(pInstance, timeElapsed);	// Make time move
			TranslateAllPassengers(pInstance);				// Move any passengers
		}

		pInstance = pInstance->pNext;						// Next?
	}
}

/* ------------------------------------------------------------------------------------ */
// TranslateAllPassengers
//
// This function translates all actors that are marked as
// ..passengers of this vehicle.  As each passenger is translated,
// ..it's removed from the list as gravity and/or other processing
// ..will reset a passenger elsewhere.
/* ------------------------------------------------------------------------------------ */
int CActorManager::TranslateAllPassengers(ActorInstance* entry)
{
	geVec3d delta = entry->localTranslation;

	delta.X -= entry->OldTranslation.X;
	delta.Y = 0.0f;
	delta.Z -= entry->OldTranslation.Z;

	for(int i=0; i<PASSENGER_LIST_SIZE; ++i)
	{
		if(entry->Passengers[i] == NULL)
			continue;											// Skip blanks (doh)

		AddTranslation(entry->Passengers[i], delta);	// Move it!
		entry->Passengers[i] = NULL;	// Been there, done that
	}

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// CheckAnimation
//
// check to see if our new animation collides with something
/* ------------------------------------------------------------------------------------ */
int CActorManager::CheckAnimation(const geActor* actor, const std::string& animation, int /*channel*/)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;

	float height;
	geExtBox box;

	if(!GetAnimationHeight(actor, animation, &height))
		return RGF_NOT_FOUND;

	GetBoundingBox(actor, &box);
	box.Max.Y = height;
	box.Min.Y = 1.0f;

	return CCD->Collision()->CanOccupyPosition(&pInstance->localTranslation, &box, pInstance->Actor);
}

/* ------------------------------------------------------------------------------------ */
// SetBBox
/* ------------------------------------------------------------------------------------ */
void CActorManager::SetBBox(geActor* actor, float sizeX, float sizeY, float sizeZ)
{
	geExtBox extBox;
	extBox.Max.X = sizeX * 0.5f;
	extBox.Max.Y = fabs(sizeY * 0.5f);
	extBox.Max.Z = sizeZ * 0.5f;

	extBox.Min.X = -extBox.Max.X;

	if(sizeY >= 0.0f)
		extBox.Min.Y = -extBox.Max.Y;
	else
		extBox.Min.Y = 0.0f;

	extBox.Min.Z = -extBox.Max.Z;

	geActor_SetExtBox(actor, &extBox, NULL);
	geActor_SetRenderHintExtBox(actor, &extBox, NULL);
}

/* ------------------------------------------------------------------------------------ */
// SetBoundingBox
/* ------------------------------------------------------------------------------------ */
void CActorManager::SetBoundingBox(const geActor* actor, const std::string& animation)
{
	geExtBox extBox, maxExtBox;
	float timer = 0.0f;
	float tStart, tEnd;
	geXForm3d xForm;

	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return;

	// Get the motion name
	geMotion *pMotion = geActor_GetMotionByName(pInstance->theDef, animation.c_str());

	if(!pMotion)
	{
		// No motion?  Just use the current cleared pose OBB
		geActor_GetDynamicExtBox(pInstance->Actor, &extBox);
		extBox.Min.X -= pInstance->localTranslation.X;
		extBox.Min.Y -= pInstance->localTranslation.Y;
		extBox.Min.Z -= pInstance->localTranslation.Z;
		extBox.Max.X -= pInstance->localTranslation.X;
		extBox.Max.Y -= pInstance->localTranslation.Y;
		extBox.Max.Z -= pInstance->localTranslation.Z;
		extBox.Min.X -= 4.0f;
		extBox.Min.Y = 0.0f;
		extBox.Min.Z -= 4.0f;
		extBox.Max.X += 4.0f;
		extBox.Max.Z += 4.0f;
		geActor_SetExtBox(pInstance->Actor, &extBox, NULL);
		geActor_SetRenderHintExtBox(pInstance->Actor, &extBox, NULL);
		return;
	}

	geXForm3d_SetZRotation(&xForm, pInstance->BaseRotation.Z + pInstance->localRotation.Z);
	geXForm3d_RotateX(&xForm, pInstance->BaseRotation.X + pInstance->localRotation.X);
	geXForm3d_RotateY(&xForm, pInstance->BaseRotation.Y + pInstance->localRotation.Y);
	geXForm3d_Translate(&xForm, pInstance->localTranslation.X, pInstance->localTranslation.Y, pInstance->localTranslation.Z);

	geMotion_GetTimeExtents(pMotion, &tStart, &tEnd);
	memset(&maxExtBox, 0, sizeof(geExtBox));

	while(timer < tEnd)
	{
		geActor_SetPose(pInstance->Actor, pMotion, timer, &xForm);
		geActor_GetDynamicExtBox(pInstance->Actor, &extBox);
		extBox.Min.X -= pInstance->localTranslation.X;
		extBox.Min.Y -= pInstance->localTranslation.Y;
		extBox.Min.Z -= pInstance->localTranslation.Z;
		extBox.Max.X -= pInstance->localTranslation.X;
		extBox.Max.Y -= pInstance->localTranslation.Y;
		extBox.Max.Z -= pInstance->localTranslation.Z;
		extBox.Min.X -= 4.0f;
		extBox.Min.Y -= 0.05f;
		extBox.Min.Z -= 4.0f;
		extBox.Max.X += 4.0f;
		extBox.Max.Z += 4.0f;
		geExtBox_Union(&maxExtBox, &extBox, &maxExtBox);
		timer += 0.5f;			// In half-second increments
	}

	if(maxExtBox.Min.X < maxExtBox.Min.Z)
		maxExtBox.Min.X = maxExtBox.Min.Z;
	else
		maxExtBox.Min.Z = maxExtBox.Min.X;

	if(maxExtBox.Max.X < maxExtBox.Max.Z)
		maxExtBox.Max.X = maxExtBox.Max.Z;
	else
		maxExtBox.Max.Z = maxExtBox.Max.X;

	maxExtBox.Min.Y = 0.0f;
	geActor_SetExtBox(pInstance->Actor, &maxExtBox, NULL);
	geActor_SetRenderHintExtBox(pInstance->Actor, &maxExtBox, NULL);
}

/* ------------------------------------------------------------------------------------ */
// GetAnimationHeight
/* ------------------------------------------------------------------------------------ */
int CActorManager::GetAnimationHeight(const geActor* actor, const std::string& animation, float* height)
{
	assert(height);
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;

	// Get the motion name
	geMotion *pMotion = geActor_GetMotionByName(pInstance->theDef, animation.c_str());

	if(!pMotion)
		return RGF_NOT_FOUND;

	geExtBox extBox, maxExtBox;
	float timer = 0.0f;
	float tStart, tEnd;
	geXForm3d xForm;

	geXForm3d_SetZRotation(&xForm, pInstance->BaseRotation.Z + pInstance->localRotation.Z);
	geXForm3d_RotateX(&xForm, pInstance->BaseRotation.X + pInstance->localRotation.X);
	geXForm3d_RotateY(&xForm, pInstance->BaseRotation.Y + pInstance->localRotation.Y);
	geXForm3d_Translate(&xForm, pInstance->localTranslation.X, pInstance->localTranslation.Y, pInstance->localTranslation.Z);

	geMotion_GetTimeExtents(pMotion, &tStart, &tEnd);
	memset(&maxExtBox, 0, sizeof(geExtBox));

	while(timer < tEnd)
	{
		geActor_SetPose(pInstance->Actor, pMotion, timer, &xForm);
		geActor_GetDynamicExtBox(pInstance->Actor, &extBox);
		extBox.Min.X -= pInstance->localTranslation.X;
		extBox.Min.Y -= pInstance->localTranslation.Y;
		extBox.Min.Z -= pInstance->localTranslation.Z;
		extBox.Max.X -= pInstance->localTranslation.X;
		extBox.Max.Y -= pInstance->localTranslation.Y;
		extBox.Max.Z -= pInstance->localTranslation.Z;
		extBox.Min.X -= 4.0f;
		extBox.Min.Y -= 0.05f;
		extBox.Min.Z -= 4.0f;
		extBox.Max.X += 4.0f;
		extBox.Max.Z += 4.0f;
		geExtBox_Union(&maxExtBox, &extBox, &maxExtBox);
		timer += 0.5f;			// In half-second increments
	}

	pMotion = geActor_GetMotionByName(pInstance->theDef, pInstance->MotionName.c_str());

	float time = pInstance->AnimationTime * 0.001f * pInstance->ActorAnimationSpeed;

	geActor_SetPose(pInstance->Actor, pMotion, time, &xForm);
	*height = maxExtBox.Max.Y;

	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
// SetAnimationHeight
/* ------------------------------------------------------------------------------------ */
int CActorManager::SetAnimationHeight(const geActor* actor, const std::string& animation, bool camera)
{
	ActorInstance *pInstance = LocateInstance(actor);

	if(pInstance == NULL)
		return RGF_NOT_FOUND;

	// Get the motion name
	geMotion *pMotion = geActor_GetMotionByName(pInstance->theDef, animation.c_str());

	if(!pMotion)
		return RGF_NOT_FOUND;

	geExtBox extBox, maxExtBox;
	float timer = 0.0f;
	float tStart, tEnd;
	geXForm3d xForm;

	geXForm3d_SetZRotation(&xForm, pInstance->BaseRotation.Z + pInstance->localRotation.Z);
	geXForm3d_RotateX(&xForm, pInstance->BaseRotation.X + pInstance->localRotation.X);
	geXForm3d_RotateY(&xForm, pInstance->BaseRotation.Y + pInstance->localRotation.Y);
	geXForm3d_Translate(&xForm, pInstance->localTranslation.X, pInstance->localTranslation.Y, pInstance->localTranslation.Z);

	geMotion_GetTimeExtents(pMotion, &tStart, &tEnd);
	memset(&maxExtBox, 0, sizeof(geExtBox));

	while(timer < tEnd)
	{
		geActor_SetPose(pInstance->Actor, pMotion, timer, &xForm);
		geActor_GetDynamicExtBox(pInstance->Actor, &extBox);
		extBox.Min.X -= pInstance->localTranslation.X;
		extBox.Min.Y -= pInstance->localTranslation.Y;
		extBox.Min.Z -= pInstance->localTranslation.Z;
		extBox.Max.X -= pInstance->localTranslation.X;
		extBox.Max.Y -= pInstance->localTranslation.Y;
		extBox.Max.Z -= pInstance->localTranslation.Z;
		extBox.Min.X -= 4.0f;
		extBox.Min.Y -= 0.05f;
		extBox.Min.Z -= 4.0f;
		extBox.Max.X += 4.0f;
		extBox.Max.Z += 4.0f;
		geExtBox_Union(&maxExtBox, &extBox, &maxExtBox);
		timer += 0.5f;			// In half-second increments
	}

	pMotion = geActor_GetMotionByName(pInstance->theDef, pInstance->MotionName.c_str());

	float time = pInstance->AnimationTime * 0.001f * pInstance->ActorAnimationSpeed;

	geActor_SetPose(pInstance->Actor, pMotion, time, &xForm);

	GetBoundingBox(pInstance->Actor, &extBox);
	extBox.Max.Y = maxExtBox.Max.Y;
	geActor_SetExtBox(pInstance->Actor, &extBox, NULL);
	geActor_SetRenderHintExtBox(pInstance->Actor, &extBox, NULL);

	if(camera)
	{
		float DesiredHeight;

		switch(CCD->Player()->GetViewPoint())
		{
		case FIRSTPERSON:
			if(CCD->CameraManager()->GetViewHeight() != -1)
			{
				DesiredHeight = (extBox.Max.Y * CCD->CameraManager()->GetViewHeight()) / CCD->Player()->GetHeight();
				DesiredHeight *= CCD->Player()->GetScale();
			}
			else
			{
				DesiredHeight = extBox.Max.Y;
			}
			break;
		case THIRDPERSON:
			if(CCD->CameraManager()->GetPlayerHeight() != -1)
			{
				DesiredHeight = (extBox.Max.Y * CCD->CameraManager()->GetPlayerHeight()) / CCD->Player()->GetHeight();
				DesiredHeight *= CCD->Player()->GetScale();
			}
			else
			{
				DesiredHeight = extBox.Max.Y;
			}
			break;
		case ISOMETRIC:
			if(CCD->CameraManager()->GetIsoHeight() != -1)
			{
				DesiredHeight = (extBox.Max.Y * CCD->CameraManager()->GetIsoHeight()) / CCD->Player()->GetHeight();
				DesiredHeight *= CCD->Player()->GetScale();
			}
			else
			{
				DesiredHeight = extBox.Max.Y;
			}
			break;
		default:
			return RGF_SUCCESS;
		}

		CCD->CameraManager()->SetDesiredHeight(DesiredHeight);
		CCD->CameraManager()->TrackMotion();
	}

	return RGF_SUCCESS;
}

/*
	We probably do not hit the bone immediately after hitting the overall bounding box.
	So tag the actor as being hit at the bounding box level and after that check ONLY the
	bone bounding boxes until the whatever hit the overall bounding box no longer exists.
*/
/* ------------------------------------------------------------------------------------ */
// DoesRayHitActor
/* ------------------------------------------------------------------------------------ */
geBoolean CActorManager::DoesRayHitActor(const geVec3d& oldPosition,
										 const geVec3d& newPosition,
										 geActor**		actor,
										 const geActor* actorToExclude,
										 float*			percent,
										 geVec3d*		normal,
										 void*			collisionObject)
{
	geExtBox result;
	float t;
	geVec3d norm;

	*actor = NULL;

	stdext::hash_map<std::string, LoadedActorList*>::iterator iter = m_LoadedActorFiles.begin();
	for(; iter!=m_LoadedActorFiles.end(); ++iter)
	{
		ActorInstance *pInstance = iter->second->IList;

		while(pInstance != NULL)
		{
			if(pInstance->Actor == actorToExclude)
			{
				// We want to ignore this actor, do so.
				pInstance = pInstance->pNext;
				continue;
			}

			if(pInstance->NoCollision)
			{
				pInstance = pInstance->pNext;
				continue;
			}

			// Get actor instance bounding box in MODEL SPACE
			GetBoundingBox(pInstance->Actor, &result);
			result.Min.X += pInstance->localTranslation.X;
			result.Min.Y += pInstance->localTranslation.Y;
			result.Min.Z += pInstance->localTranslation.Z;
			result.Max.X += pInstance->localTranslation.X;
			result.Max.Y += pInstance->localTranslation.Y;
			result.Max.Z += pInstance->localTranslation.Z;

			if(geExtBox_RayCollision(&result, &oldPosition, &newPosition, &t, &norm)==GE_TRUE)
			{
				// Heh, we hit someone.  Return the actor we ran into.
				*actor = pInstance->Actor;
				*percent = t;
				*normal = norm;

				CollideObjectInformation ci;

				ci.CollideObject = collisionObject;
				ci.Normal = *normal;
				ci.Percent = *percent;

				AddCollideObject(COLActorBBox, pInstance, &ci);

				return GE_TRUE;
			}

			pInstance = pInstance->pNext;				// Next instance in list
		}
		// Next master instance
	}

	// No hit, all be hunky-dory.
	return GE_FALSE;							// Hey, no collisions!
}

/* ------------------------------------------------------------------------------------ */
// DidRayHitActor
/* ------------------------------------------------------------------------------------ */
geBoolean CActorManager::DidRayHitActor(const geVec3d&	/*oldPosition*/,
										const geVec3d&	/*newPosition*/,
										geActor**		actor,
										const geActor*	actorToExclude,
										float*			percent,
										geVec3d*		normal,
										void*			collisionObj)
{
	*actor = NULL;

	stdext::hash_map<std::string, LoadedActorList*>::iterator iter = m_LoadedActorFiles.begin();
	for(; iter!=m_LoadedActorFiles.end(); ++iter)
	{
		ActorInstance *pInstance = iter->second->IList;

		while(pInstance != NULL)
		{
			if(pInstance->Actor == actorToExclude)
			{
				// We want to ignore this actor, do so.
				pInstance = pInstance->pNext;
				continue;
			}

			if(pInstance->NoCollision)
			{
				pInstance = pInstance->pNext;
				continue;
			}

			CollideObjectInformation *ci;

			ci = GetCollideObject(COLActorBBox, pInstance, collisionObj);

			if(ci)
			{
				// Heh, we hit someone.  Return the actor we ran into.
				*actor = pInstance->Actor;
				*percent = ci->Percent;
				*normal = ci->Normal;

				return GE_TRUE;
			}

			pInstance = pInstance->pNext;				// Next instance in list
		}
		// Next master instance
	}

	// No hit, all be hunky-dory.
	return GE_FALSE;							// Hey, no collisions!
}

/* ------------------------------------------------------------------------------------ */
// SetLastBoneHit
/* ------------------------------------------------------------------------------------ */
geBoolean CActorManager::SetLastBoneHit(const geActor* actor, const std::string& lastBoneHit)
{
	ActorInstance *pInstance = LocateInstance(actor);

	geBoolean setHitBoneName = (pInstance && !lastBoneHit.empty());

	if(setHitBoneName)
	{
		pInstance->LastBoneHit = lastBoneHit;
	}

	return setHitBoneName;
}

/* ------------------------------------------------------------------------------------ */
// GetLastBoneHit
/* ------------------------------------------------------------------------------------ */
std::string CActorManager::GetLastBoneHit(const geActor* actor)
{
	ActorInstance *pInstance = LocateInstance(actor);

	geBoolean getHitBoneName = (pInstance != NULL);

	std::string lastBoneHit;

	if(getHitBoneName)
		lastBoneHit = pInstance->LastBoneHit;

	return lastBoneHit;
}

/* ------------------------------------------------------------------------------------ */
// DoesRayHitActor
/* ------------------------------------------------------------------------------------ */
geBoolean CActorManager::DoesRayHitActor(const geVec3d&	oldPosition,
										 const geVec3d&	newPosition,
										 geActor**		actor,
										 const geActor* actorToExclude,
										 float*			percent,
										 geVec3d*		normal)
{
	geExtBox result;
	float t;
	geVec3d norm;

	*actor = NULL;

	stdext::hash_map<std::string, LoadedActorList*>::iterator iter = m_LoadedActorFiles.begin();
	for(; iter!=m_LoadedActorFiles.end(); ++iter)
	{
		ActorInstance *pInstance = iter->second->IList;

		while(pInstance != NULL)
		{
			if(pInstance->Actor == actorToExclude)
			{
				// We want to ignore this actor, do so.
				pInstance = pInstance->pNext;
				continue;
			}

			if(pInstance->NoCollision)
			{
				pInstance = pInstance->pNext;
				continue;
			}

			// Get actor instance bounding box in MODEL SPACE
			GetBoundingBox(pInstance->Actor, &result);
			result.Min.X += pInstance->localTranslation.X;
			result.Min.Y += pInstance->localTranslation.Y;
			result.Min.Z += pInstance->localTranslation.Z;
			result.Max.X += pInstance->localTranslation.X;
			result.Max.Y += pInstance->localTranslation.Y;
			result.Max.Z += pInstance->localTranslation.Z;

			if(geExtBox_RayCollision(&result, &oldPosition, &newPosition, &t, &norm) == GE_TRUE)
			{
				// Heh, we hit someone.  Return the actor we ran into.
				*actor = pInstance->Actor;
				*percent = t;
				*normal = norm;
				return GE_TRUE;
			}

			pInstance = pInstance->pNext;				// Next instance in list
		}
		// Next master instance
	}

	// No hit, all be hunky-dory.
	return GE_FALSE;							// Hey, no collisions!
}


/* ----------------------------------- END OF FILE ------------------------------------ */
