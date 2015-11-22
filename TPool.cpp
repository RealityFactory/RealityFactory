
#include <memory.h>
//#include <malloc.h>
#include <string.h>
#include "RabidFramework.h"
#include <Ram.h>

typedef struct	TPool
{
  TPool    *next;
  TPool    *prev;
  geBitmap *Bitmap;
  char     *BmpName;
  char     *AlphaName;
} TPool;

TPool *Bottom;


void TPool_Initalize()
{
  Bottom=(TPool *)NULL;
}

geBitmap *TPool_Bitmap(char *DefaultBmp, char *DefaultAlpha, char *BName, char *AName)
{
  TPool *pool;
  char *TBName, *TAName;

  if(EffectC_IsStringNull(BName ) == GE_TRUE)
  {
	  TBName=DefaultBmp;
	  TAName=DefaultAlpha;
  }
  else
  {
	   if(EffectC_IsStringNull(AName ) == GE_TRUE)
	   {
	     TBName=BName;
		 TAName=BName;
	   }
	   else
	   {
	     TBName=BName;
		 TAName=AName;
	   }
  }

  pool=Bottom;
  while ( pool != NULL )
  {
    if((!stricmp(TBName, pool->BmpName)) && (!stricmp(TAName, pool->AlphaName)))
      return pool->Bitmap;
    pool = pool->next;
  } 

  pool = GE_RAM_ALLOCATE_STRUCT(TPool);
  memset(pool, 0, sizeof(TPool));
  pool->next = Bottom;
  Bottom = pool;
  if(pool->next)
    pool->next->prev = pool;
  pool->BmpName=strdup(TBName);
  pool->AlphaName=strdup(TAName);
  pool->Bitmap = CreateFromFileAndAlphaNames(TBName, TAName);

  if(!pool->Bitmap)
  {
	char szError[256];
	sprintf(szError,"Error in Bitmap %s or Alphamap '%s'", TBName, TAName);
	CCD->ReportError(szError, false);
	CCD->ShutdownLevel();
	delete CCD;
	MessageBox(NULL, szError,"Bitmap Manager", MB_OK);
	exit(-333);
  }

  if (!geWorld_AddBitmap(CCD->World(), pool->Bitmap))
  {
    geBitmap_Destroy(&pool->Bitmap);
    return (geBitmap *)NULL;
  }

  return pool->Bitmap;
}

void TPool_Delete()
{
  TPool *pool, *temp;

  pool = Bottom;
  while	(pool!= NULL)
  {
	temp = pool->next;

	if(pool->Bitmap)
	{
		geWorld_RemoveBitmap(CCD->World(), pool->Bitmap);
		geBitmap_Destroy(&pool->Bitmap);
	}
	free(pool->BmpName);
	free(pool->AlphaName);
  geRam_Free(pool);

	pool = temp;
  }
}
