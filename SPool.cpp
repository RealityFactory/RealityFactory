/************************************************************************************//**
 * @file SPool.cpp
 * @brief Sound pool
 ****************************************************************************************/

#include <memory>
#include "RabidFramework.h"
#include <Ram.h>


typedef struct	SPool
{
	SPool		*next;
	SPool		*prev;
	geSound_Def	*SoundDef;
	char		*Name;

} SPool;

SPool *Bottom;


/* ------------------------------------------------------------------------------------ */
// Spool_Initialize
/* ------------------------------------------------------------------------------------ */
void SPool_Initalize()
{
	Bottom = NULL;
}

/* ------------------------------------------------------------------------------------ */
// SPool_Sound
/* ------------------------------------------------------------------------------------ */
geSound_Def *SPool_Sound(const char *SName)
{
	SPool *pool;
	geVFile *MainFS;

	if(EffectC_IsStringNull(SName) == GE_TRUE)
	{
		char szError[256];
		sprintf(szError, "[WARNING] File %s - Line %d: Missing Required Field from Entity - Sound File: %s\n",
				__FILE__, __LINE__, SName);
		CCD->ReportError(szError, false);
		return NULL;
	}

	pool = Bottom;

	while(pool != NULL)
	{
		if(!stricmp(SName, pool->Name))
			return pool->SoundDef;

		pool = pool->next;
	}

	CCD->OpenRFFile(&MainFS, kAudioFile, SName, GE_VFILE_OPEN_READONLY);

	if(!MainFS)
		return NULL;

	pool = GE_RAM_ALLOCATE_STRUCT(SPool);
	memset(pool, 0, sizeof(SPool));

	pool->Name = strdup(SName);
	pool->SoundDef = geSound_LoadSoundDef(CCD->Engine()->AudioSystem(), MainFS);
	geVFile_Close(MainFS);

	if(!pool->SoundDef)
	{
		char szError[256];
		sprintf(szError, "[WARNING] File %s - Line %d: Unable to load file, invalid path or filename: %s",
				__FILE__, __LINE__, SName);
		CCD->ReportError(szError, false);
		free(pool->Name);
		geRam_Free(pool);
		return NULL;
	}

	pool->next = Bottom;
	Bottom = pool;

	if(pool->next)
		pool->next->prev = pool;

	return pool->SoundDef;
}

/* ------------------------------------------------------------------------------------ */
// SPool_Delete
/* ------------------------------------------------------------------------------------ */
void SPool_Delete()
{
	SPool *pool, *temp;

	pool = Bottom;

	while(pool!= NULL)
	{
		temp = pool->next;

		if(pool->SoundDef)
			geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), pool->SoundDef);

		free(pool->Name);
		geRam_Free(pool);
		pool = temp;
	}
}

/* ----------------------------------- END OF FILE ------------------------------------ */
