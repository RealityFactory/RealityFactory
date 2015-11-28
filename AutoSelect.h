/****************************************************************************************/
/*  AutoSelect.h                                                                        */
/*                                                                                      */
/*  Original Author: Mike Sandige                                                       */
/*  Combine with ModeList and slight mods: Dee                                          */
/*  Description:    Attempts to automatically choose a good video mode                  */
/*                                                                                      */
/*  Copyright (c) 1999 WildTangent, Inc.; All rights reserved.                          */
/*                                                                                      */
/*  See the accompanying file LICENSE.TXT for terms on the use of this library.         */
/*  This library is distributed in the hope that it will be useful but WITHOUT          */
/*  ANY WARRANTY OF ANY KIND and without any implied warranty of MERCHANTABILITY        */
/*  or FITNESS FOR ANY PURPOSE.  Refer to LICENSE.TXT for more details.                 */
/*                                                                                      */
/****************************************************************************************/
#ifndef AUTOSELECT_H
#define AUTOSELECT_H

#include "genesis.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ModeList_DriverType
{
	MODELIST_TYPE_D3D_PRIMARY,
	MODELIST_TYPE_D3D_SECONDARY,
	MODELIST_TYPE_UNKNOWN,
	MODELIST_TYPE_D3D_3DFX,

} ModeList_DriverType;


typedef enum ModeList_Evaluation
{
	MODELIST_EVALUATED_OK,
	MODELIST_EVALUATED_UNDESIRABLE,
	MODELIST_EVALUATED_TRIED_FAILED,

} ModeList_Evaluation;


typedef	struct	ModeList
{
	geDriver				*Driver;
	geDriver_Mode			*Mode;
	const char				*DriverNamePtr;
	const char				*ModeNamePtr;
	ModeList_DriverType		DriverType;
	int						Width;
	int						Height;
	ModeList_Evaluation		Evaluation;
	geBoolean				InAWindow;

} ModeList;


void      ModeList_Destroy(ModeList *List);
ModeList *ModeList_Create(geEngine *Engine,int *ListLength, geDriver_System *m_DrvSys,
						  geDriver *m_Driver, geDriver_Mode *m_Mode);

void AutoSelect_SortDriverList(ModeList *DriverList, int ListLength);
geBoolean AutoSelect_PickDriver(HWND hWnd, geEngine *Engine, ModeList *List, int ModeListLength, int *Selection);

geBoolean DrvList_PickDriver(HANDLE hInstance, HWND hwndParent,
		ModeList *List, int ListLength, int *ListSelection);

void ResetMainWindow(HWND hWnd, int32 Width, int32 Height);


#ifdef __cplusplus
}
#endif

#endif

/* ----------------------------------- END OF FILE ------------------------------------ */
