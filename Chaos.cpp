

//	Include the One True Header

#include "RabidFramework.h"

Chaos::Chaos()
{
  geBitmap_Info	AttachInfo;
  geEntity_EntitySet *pSet;
  geEntity *pEntity;

  pSet = geWorld_GetEntitySet(CCD->World(), "EChaos");

  if(!pSet) 
    return;	// Not on this level.

//	Ok, we have procedurals somewhere.  Dig through 'em all.

  for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
      pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
  {
    EChaos *pTex = (EChaos*)geEntity_GetUserData(pEntity);

    pTex->SegmentSize = 1;
// changed RF064
// changes by QuestofDreams
     pTex->Actor = NULL;
       if(!EffectC_IsStringNull(pTex->EntityName))
       {
               pTex->Actor = GetEntityActor(pTex->EntityName);
               if(pTex->Actor == NULL)
               {
                       char szError[256];
                       sprintf(szError,"EChaos : Error can't find actor");
                       CCD->ReportError(szError, false);
                       CCD->ShutdownLevel();
                       delete CCD;
                       MessageBox(NULL, szError,"EChaos", MB_OK);
                       exit(-333);// or continue
               }
       }

	   pTex->CAttachBmp = NULL;

       // get the output bitmap as an actor bitmap...
       if (pTex->Actor != NULL)
       {
               // locals
               geActor_Def *ActorDef;
               geBody *Body;
               int MaterialCount;
               int i;
               const char *MaterialName;
               float R, G, B;
               int Length;

               // get actor material count
               ActorDef = geActor_GetActorDef(pTex->Actor);
               if (ActorDef == NULL)
               continue;
               Body = geActor_GetBody(ActorDef);
               if (Body == NULL)
                       continue;
               MaterialCount = geActor_GetMaterialCount(pTex->Actor);

               // get bitmap pointer
               Length = strlen(pTex->AttachBmp);
               for(i = 0;i < MaterialCount;i++)
               {

                       if(geBody_GetMaterial(Body, i, &MaterialName, &(pTex->CAttachBmp), &R, &G, &B) == GE_FALSE)
                       {
                               continue;
                       }
                       if(strnicmp(pTex->AttachBmp, MaterialName, Length) == 0)
                       {
                               break;
                       }
               }
               if(i == MaterialCount)
               {
				   pTex->CAttachBmp = NULL;
                   /*    char szError[256];
                       sprintf(szError,"EChaos : Error can't find ActorMaterial");
                       CCD->ReportError(szError, false);
                       CCD->ShutdownLevel();
                       delete CCD;
                       MessageBox(NULL, szError,"EChaos", MB_OK);
                       exit(-333); //or continue */
               }
       }

       // ...or a world bitmap
       else
       {
               pTex->CAttachBmp = geWorld_GetBitmapByName(CCD->World(), pTex->AttachBmp);
        /*       if(pTex->CAttachBmp == NULL)
               {
                       char szError[256];
                       sprintf(szError,"EChaos : Error can't find bitmap in level");
                       CCD->ReportError(szError, false);
                       CCD->ShutdownLevel();
                       delete CCD;
                       MessageBox(NULL, szError,"EChaos", MB_OK);
                       exit(-333); //or continue
               } */
       }
// end change QuestOfDreams 300802
// end change RF064

    if ( pTex->CAttachBmp == NULL )
		continue;
    geBitmap_SetFormatMin( pTex->CAttachBmp, CHAOS_FORMAT );
    geBitmap_ClearMips( pTex->CAttachBmp );
    if ( geBitmap_GetInfo( pTex->CAttachBmp, &AttachInfo, NULL ) == GE_FALSE )
		continue;
    pTex->SegmentCount = AttachInfo.Width / pTex->SegmentSize;
    // fail if the sway amount is bigger than the texture size
    if ( (pTex->MaxXSway >= AttachInfo.Width ) || (pTex->MaxYSway >= AttachInfo.Height ) )
		continue;
    pTex->OriginalBmp = geBitmap_Create( AttachInfo.Width, AttachInfo.Height, 1, CHAOS_FORMAT );
    if (pTex->OriginalBmp == NULL )
		continue;
    geBitmap_ClearMips( pTex->OriginalBmp );
    geBitmap_BlitBitmap( pTex->CAttachBmp, pTex->OriginalBmp );
    pTex->WorkBmp = geBitmap_Create( AttachInfo.Width, AttachInfo.Height, 1, CHAOS_FORMAT );
    if (pTex->WorkBmp != NULL )
		geBitmap_ClearMips(pTex->WorkBmp );
  }

  // all done
  return;
}

Chaos::~Chaos()
{
  geEntity_EntitySet *pSet;
  geEntity *pEntity;

  pSet = geWorld_GetEntitySet(CCD->World(), "EChaos");

  if(!pSet) 
    return;	// Not on this level.

//	Ok, we have procedurals somewhere.  Dig through 'em all.

  for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
      pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
  {
    EChaos *pTex = (EChaos*)geEntity_GetUserData(pEntity);
    if ( (pTex->OriginalBmp != NULL ) && ( pTex->CAttachBmp != NULL ) )
	geBitmap_BlitBitmap(pTex->OriginalBmp, pTex->CAttachBmp );
    if (pTex->OriginalBmp != NULL )
	geBitmap_Destroy( &(pTex->OriginalBmp ) );
    if (pTex->WorkBmp != NULL )
	geBitmap_Destroy( &(pTex->WorkBmp ) );
  }
}

void Chaos::Tick(geFloat dwTicksIn)
{
  geBitmap_Info	AttachInfo, OriginalInfo;
  int		Row, Col;
  int		XPos, YPos;
  geFloat		CosStep;
  geFloat		CurYOffset;
  geFloat		CurXOffset;

  geEntity_EntitySet *pSet;
  geEntity *pEntity;

  geFloat dwTicks = (geFloat)(dwTicksIn);

  pSet = geWorld_GetEntitySet(CCD->World(), "EChaos");

  if(!pSet) 
    return;	// Not on this level.

//	Ok, we have procedurals somewhere.  Dig through 'em all.

  for(pEntity= geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
      pEntity= geEntity_EntitySetGetNextEntity(pSet, pEntity)) 
  {
    EChaos *pTex = (EChaos*)geEntity_GetUserData(pEntity);
	if(!pTex->CAttachBmp)
		continue;
// changed RF064
    // changed QuestOfDreams 300802
       // do nothing further if the texture is not visible
	if(pTex->Actor && !CCD->ActorManager()->IsActor(pTex->Actor))
		continue;

       if(pTex->Actor == NULL)
       {
               if(geWorld_BitmapIsVisible(CCD->World(), pTex->CAttachBmp) == GE_FALSE)
                       continue;
       }
       else
       {
               if(geWorld_IsActorPotentiallyVisible(CCD->World(), pTex->Actor, CCD->CameraManager()->Camera()) == GE_FALSE)
                       continue;
       }
// end change QuestOfDreams 300802
// end change RF064
    if ( geBitmap_GetInfo( pTex->CAttachBmp, &AttachInfo, NULL ) == GE_FALSE )
		continue;
    if ( geBitmap_GetInfo(pTex->OriginalBmp, &OriginalInfo, NULL ) == GE_FALSE )
		continue;

    // compute vertical offset
    pTex->YOffset += (geFloat)pTex->YStep * (dwTicks*0.001f);
    if (pTex->YOffset > GE_TWOPI )
	pTex->YOffset = 0.0f;
    CosStep = GE_TWOPI / (geFloat)pTex->SegmentCount;
    CurYOffset = pTex->YOffset;

    // adjust vertically
    for ( Col = 0; Col < pTex->SegmentCount; Col++ )
    {
      // adjust offset
      CurYOffset += CosStep;
      if ( CurYOffset > GE_PI )
      {
	CosStep = -CosStep;
	CurYOffset = GE_PI - ( CurYOffset - GE_PI );
      }
      else if ( CurYOffset < 0.0 )
	   {
		CosStep = -CosStep;
		CurYOffset = -CurYOffset;
	   }

     // compute positions
     XPos = Col * pTex->SegmentSize;
     YPos = (int)( ( (geFloat)cos( CurYOffset ) + 1.0f ) * ( (geFloat)pTex->MaxYSway / 2.0f ) );

     // adjust bitmap
     geBitmap_Blit(pTex->OriginalBmp, XPos, 0, pTex->WorkBmp, XPos, YPos, pTex->SegmentSize, AttachInfo.Height - YPos );
     geBitmap_Blit(pTex->OriginalBmp, XPos, AttachInfo.Height - YPos, pTex->WorkBmp, XPos, 0, pTex->SegmentSize, YPos );
    }

    // compute horizontal offset
    pTex->XOffset += (geFloat)pTex->XStep * (dwTicks*0.001f);
    if (pTex->XOffset > GE_TWOPI )
	pTex->XOffset = 0.0f;
    CosStep = GE_TWOPI / (geFloat)pTex->SegmentCount;
    CurXOffset = pTex->XOffset;
	
    // adjust horizontally
    for ( Row = 0; Row < pTex->SegmentCount; Row++ )
    {
	// adjust offset
	CurXOffset += CosStep;
	if ( CurXOffset > GE_PI )
	{
		CosStep = -CosStep;
		CurXOffset = GE_PI - ( CurXOffset - GE_PI );
	}
	else if ( CurXOffset < 0.0 )
  	     {
		CosStep = -CosStep;
		CurXOffset = -CurXOffset;
	     }

      // compute positions
      XPos = (int)( ( (geFloat)cos( CurXOffset ) + 1.0f ) * ( (geFloat)pTex->MaxXSway / 2.0f ) );
      YPos = Row * pTex->SegmentSize;

      // adjust bitmap
      geBitmap_Blit( pTex->WorkBmp, 0, YPos, pTex->CAttachBmp, XPos, YPos, AttachInfo.Width - XPos, pTex->SegmentSize );
      geBitmap_Blit( pTex->WorkBmp, AttachInfo.Width - XPos, YPos, pTex->CAttachBmp, 0, YPos, XPos, pTex->SegmentSize );
    }
  }

  // all done
  return;
}
