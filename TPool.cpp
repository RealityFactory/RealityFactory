/************************************************************************************//**
 * @file TPool.cpp
 * @brief Texture pool
 ****************************************************************************************/

#include <memory>
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


/* ------------------------------------------------------------------------------------ */
//	TPool_Initalize
/* ------------------------------------------------------------------------------------ */
void TPool_Initalize()
{
	Bottom = NULL;
}

/* ------------------------------------------------------------------------------------ */
//	TPool_Bitmap
/* ------------------------------------------------------------------------------------ */
geBitmap *TPool_Bitmap(const char *DefaultBmp, const char *DefaultAlpha,
					   const char *BName, const char *AName)
{
	TPool *pool;
	const char *TBName, *TAName;

	if(EffectC_IsStringNull(BName) == GE_TRUE)
	{
		TBName = DefaultBmp;

		if(EffectC_IsStringNull(DefaultAlpha))
			TAName = DefaultBmp;
		else
			TAName = DefaultAlpha;
	}
	else
	{
		if(EffectC_IsStringNull(AName ) == GE_TRUE)
		{
			TBName = BName;
			TAName = BName;
		}
		else
		{
			TBName = BName;
			TAName = AName;
		}
	}

	pool = Bottom;

	while(pool != NULL)
	{
		if((!stricmp(TBName, pool->BmpName)) && (!stricmp(TAName, pool->AlphaName)))
			return pool->Bitmap;

		pool = pool->next;
	}

	pool = GE_RAM_ALLOCATE_STRUCT(TPool);
	memset(pool, 0, sizeof(TPool));

// changed QD 07/15/06
	pool->BmpName = strdup(TBName);
	pool->AlphaName = strdup(TAName);
	pool->Bitmap = CreateFromFileAndAlphaNames(TBName, TAName);

	if(!pool->Bitmap)
	{
		char szError[256];
		sprintf(szError, "[WARNING] File %s - Line %d: Error in Bitmap %s or Alphamap '%s'\n",
				__FILE__, __LINE__, TBName, TAName);
		CCD->ReportError(szError, false);
		free(pool->BmpName);
		free(pool->AlphaName);
		geRam_Free(pool);
		return NULL;
	}

	if(!geWorld_AddBitmap(CCD->World(), pool->Bitmap))
	{
		geBitmap_Destroy(&pool->Bitmap);
		free(pool->BmpName);
		free(pool->AlphaName);
		geRam_Free(pool);
		return NULL;
	}

	pool->next = Bottom;
	Bottom = pool;

	if(pool->next)
		pool->next->prev = pool;
// end change

	return pool->Bitmap;
}

/* ------------------------------------------------------------------------------------ */
//	TPool_Delete
/* ------------------------------------------------------------------------------------ */
void TPool_Delete()
{
	TPool *pool, *temp;

	pool = Bottom;

	while(pool!= NULL)
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

/* ----------------------------------- END OF FILE ------------------------------------ */
