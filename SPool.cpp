
#include <memory.h>
//#include <malloc.h>
#include <string.h>
#include "RabidFramework.h"
#include <Ram.h>

typedef struct	SPool
{
  SPool    *next;
  SPool    *prev;
  geSound_Def	*SoundDef;
  char     *Name;
} SPool;

SPool *Bottom;


void SPool_Initalize()
{
  Bottom=(SPool *)NULL;
}

geSound_Def *SPool_Sound(char *SName)
{
  SPool *pool;
  geVFile *MainFS;

  if(EffectC_IsStringNull(SName)==GE_TRUE)
  {
    char szError[256];
	sprintf(szError,"Missing Required Field from Entity - Sound File: %s", SName);
	CCD->ReportError(szError, false);
	CCD->ShutdownLevel();
	delete CCD;
	CCD = NULL;
	MessageBox(NULL, szError,"Sound Manager", MB_OK);
	exit(-333);
  }

  pool=Bottom;
  while ( pool != NULL )
  {
    if(!stricmp(SName, pool->Name))
      return pool->SoundDef;
    pool = pool->next;
  } 

  CCD->OpenRFFile(&MainFS, kAudioFile, SName, GE_VFILE_OPEN_READONLY);
  if(!MainFS)
  {
    return (geSound_Def *)NULL;
  }
  pool = GE_RAM_ALLOCATE_STRUCT(SPool);
  memset(pool, 0, sizeof(SPool));
  pool->next = Bottom;
  Bottom = pool;
  if(pool->next)
    pool->next->prev = pool;
  pool->Name=strdup(SName);
  pool->SoundDef = geSound_LoadSoundDef(CCD->Engine()->AudioSystem(), MainFS);
  geVFile_Close(MainFS);

  if(!pool->SoundDef)
  {
	char szError[256];
	sprintf(szError,"Unable to load file, invalid path or filename: Sound File %s", SName);
	CCD->ReportError(szError, false);
	CCD->ShutdownLevel();
	delete CCD;
	CCD = NULL;
	MessageBox(NULL, szError,"Sound Manager", MB_OK);
	exit(-333);
  }

  return pool->SoundDef;
}

void SPool_Delete()
{
  SPool *pool, *temp;

  pool = Bottom;
  while	(pool!= NULL)
  {
	temp = pool->next;

	if(pool->SoundDef)
        geSound_FreeSoundDef(CCD->Engine()->AudioSystem(), pool->SoundDef );
	free(pool->Name);
  geRam_Free(pool);

	pool = temp;
  }
}
