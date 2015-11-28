/************************************************************************************//**
 * @file AutoSelect.c
 * @brief Attempts to automatically choose a good video mode
 * @author Mike Sandige
 * @author Dee (Combine with ModeList and slight mods)
 *//*
 * Copyright (c) 1999 WildTangent, Inc.; All rights reserved.
 *
 * See the accompanying file LICENSE.TXT for terms on the use of this library.
 * This library is distributed in the hope that it will be useful but WITHOUT
 * ANY WARRANTY OF ANY KIND and without any implied warranty of MERCHANTABILITY
 * or FITNESS FOR ANY PURPOSE. Refer to LICENSE.TXT for more details.
 ****************************************************************************************/

#define MODELIST_MAX_NAME (1000)
#define MODELIST_ALLOCATION_BLOCK_SIZE (10)

static char *ModeList_640WindowString="640x480 Window";
static char *ModeList_320WindowString="320x240 Window";
static char *ModeList_800WindowString="800x600 Window";


#include <stdlib.h>  //	qsort
#include <assert.h>
#include <Windows.h>


#include "AutoSelect.h"

#include "ErrorLog.h"
#include "Ram.h"

#include "resource.h"


/* ------------------------------------------------------------------------------------ */
//	AutoSelect_Compare
/* ------------------------------------------------------------------------------------ */
static int AutoSelect_Compare( const void *arg1, const void *arg2 )
{
	/*	used for quicksort comparison.
		returns <0 if arg1 is 'smaller than' arg2
		returns >0 if arg1 is 'greater than' arg2
		returns =0 if arg1 is 'the same as'  arg2
		   since the list is sorted by most desirable mode/resolution first, the better mode is 'smaller than'
	*/
	#define A1_BETTER  (-1)
	#define A2_BETTER  ( 1)
	#define TIE        ( 0)

	int Compare = 0;
	ModeList *A1,*A2;
	assert(arg1);
	assert(arg2);
	A1 = (ModeList *)arg1;
	A2 = (ModeList *)arg2;

	//	sort by DriverType		 (smaller enum value first)
	//	then by windowed
	//	then by Width             (larger width first)
	//	then by Height            (larger height first)

	if		( A1->DriverType < A2->DriverType )
		return A1_BETTER;
	else if ( A2->DriverType < A1->DriverType )
		return A2_BETTER;

	if		( !A1->InAWindow && A2->InAWindow )
		return A1_BETTER;

	if		( A1->InAWindow && !A2->InAWindow )
		return A2_BETTER;

	if      ( A1->Width > A2->Width )
		return A1_BETTER;
	else if ( A2->Width > A1->Width )
		return A2_BETTER;

	if      ( A1->Height > A2->Height )
		return A1_BETTER;
	else if ( A2->Height > A1->Height )
		return A2_BETTER;

	return TIE;
}

/* ------------------------------------------------------------------------------------ */
//	AutoSelect_SortDriverList
/* ------------------------------------------------------------------------------------ */
void AutoSelect_SortDriverList(ModeList *DriverList, int ListLength)
{
	qsort( DriverList, ListLength, sizeof(DriverList[0]), AutoSelect_Compare);
}

/* ------------------------------------------------------------------------------------ */
//	AutoSelect_PickDriver
/* ------------------------------------------------------------------------------------ */
geBoolean AutoSelect_PickDriver(HWND hWnd, geEngine *Engine,ModeList *DriverList,
								int ListLength, int *Selection)
{
	int i;

	assert(Engine      != NULL);
	assert(DriverList  != NULL);
	assert(Selection   != NULL);

	for(i=0; i<ListLength; i++)
	{
		if (DriverList[i].Evaluation == MODELIST_EVALUATED_OK)
		{
			*Selection = i;
			return GE_TRUE;
		}
	}

	return GE_FALSE;
}

/* ------------------------------------------------------------------------------------ */
//	This part does the driver find and list modes portion
/* ------------------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------------------ */
//	ModeList_Destroy
/* ------------------------------------------------------------------------------------ */
void ModeList_Destroy(ModeList *List)
{
	assert(List);
	geRam_Free(List);
}


/* ------------------------------------------------------------------------------------ */
//	ModeList_Create
/* ------------------------------------------------------------------------------------ */
ModeList *ModeList_Create(geEngine *Engine,int *ListLength, geDriver_System *m_DrvSys,
						  geDriver *m_Driver, geDriver_Mode *m_Mode)
{
	ModeList	*DriverList=NULL;
	ModeList	*NewDriverList;
	int			Allocated = 0;
	int			Needed    = 0;
	const char	*DriverNamePtr;
	const char	*ModeNamePtr;
	char		 DriverName[MODELIST_MAX_NAME];
	char		 ModeName[MODELIST_MAX_NAME];

	*ListLength = 0;

	m_Driver = geDriver_SystemGetNextDriver(m_DrvSys, NULL);

	while(m_Driver != NULL)
	{
		ModeList *	dinfo;
		ModeList_DriverType DriverType;

		if(geDriver_GetName(m_Driver, &DriverNamePtr) == GE_FALSE)
		{
			geErrorLog_AddString(-1, "AutoSelect: Failed to get driver name", NULL);
			goto ModeList_Exit;
		}

		strncpy(DriverName,DriverNamePtr, MODELIST_MAX_NAME);
		DriverName[MODELIST_MAX_NAME-1] = 0;		// just in case
		_strupr(DriverName);

		if(strstr(DriverName, "D3D") != NULL)
		{
			{
				if(strstr(DriverName, "PRIMARY") != NULL)
					DriverType = MODELIST_TYPE_D3D_PRIMARY;
				else
					DriverType = MODELIST_TYPE_D3D_SECONDARY;
			}
		}
		else
		{
			DriverType = MODELIST_TYPE_UNKNOWN;
		}


		m_Mode = geDriver_GetNextMode(m_Driver, NULL);

		while(m_Mode != NULL)
		{
			if(geDriver_ModeGetName(m_Mode, &ModeNamePtr) == GE_FALSE)
			{
				geErrorLog_AddString(-1, "AutoSelect: Failed to get mode name", NULL);
				goto ModeList_Exit;
			}

			Needed++;

			if(Allocated<Needed)
			{
				Allocated += MODELIST_ALLOCATION_BLOCK_SIZE;
				NewDriverList = geRam_Realloc(DriverList,sizeof(ModeList) * Allocated);

				if(NewDriverList == NULL)
				{
					geErrorLog_AddString(-1, "AutoSelect: Memory allocation failed", NULL);
					goto ModeList_Exit;
				}

				DriverList = NewDriverList;
			}

			dinfo = &(DriverList[Needed-1]);
			dinfo->DriverNamePtr = DriverNamePtr;
			dinfo->ModeNamePtr   = ModeNamePtr;
			dinfo->Driver = m_Driver;
			dinfo->Mode = m_Mode;
			dinfo->DriverType = DriverType;

			if(!geDriver_ModeGetWidthHeight(m_Mode, &(dinfo->Width), &(dinfo->Height)))
			{
				geErrorLog_AddString(-1, "AutoSelect: Failed to get mode width & height.  (recovering)", ModeNamePtr);
				dinfo->Evaluation = MODELIST_EVALUATED_TRIED_FAILED;
			}

			if((dinfo->Width <= 1024) && (dinfo->Height <= 768) && (dinfo->Width > 320) && (dinfo->Height > 240))
			{
				dinfo->Evaluation = MODELIST_EVALUATED_OK;
			}
			else
			{
				dinfo->Evaluation = MODELIST_EVALUATED_UNDESIRABLE;
			}

			if(dinfo->DriverType == MODELIST_TYPE_D3D_3DFX)
			{
				dinfo->Evaluation = MODELIST_EVALUATED_UNDESIRABLE;
			}

			if((dinfo->Width == -1) && (dinfo->Height == -1))
			{
				//	add some 'virtual modes'  for software window preselected resolutions

				dinfo->InAWindow = GE_TRUE;
				dinfo->ModeNamePtr   = ModeList_800WindowString;
				dinfo->Width = 800;
				dinfo->Height = 600;
				dinfo->Evaluation = MODELIST_EVALUATED_UNDESIRABLE;

				Needed++;

				if(Allocated < Needed)
				{
					Allocated += MODELIST_ALLOCATION_BLOCK_SIZE;
					NewDriverList = geRam_Realloc(DriverList, sizeof(ModeList) * Allocated);

					if(NewDriverList == NULL)
					{
						geErrorLog_AddString(-1, "AutoSelect: Memory allocation failed", NULL);
						goto ModeList_Exit;
					}

					DriverList = NewDriverList;

				}

				dinfo = &(DriverList[Needed-1]);
				dinfo->InAWindow = GE_TRUE;
				dinfo->DriverNamePtr = DriverNamePtr;
				dinfo->ModeNamePtr   = ModeList_640WindowString;
				dinfo->Driver = m_Driver;
				dinfo->Mode = m_Mode;
				dinfo->DriverType = DriverType;
				dinfo->Width = 640;
				dinfo->Height = 480;
				dinfo->Evaluation = MODELIST_EVALUATED_OK;

				Needed++;

				if(Allocated<Needed)
				{
					Allocated += MODELIST_ALLOCATION_BLOCK_SIZE;
					NewDriverList = geRam_Realloc(DriverList, sizeof(ModeList) * Allocated);

					if(NewDriverList == NULL)
					{
						geErrorLog_AddString(-1, "AutoSelect: Memory allocation failed", NULL);
						goto ModeList_Exit;
					}

					DriverList = NewDriverList;

				}

				dinfo = &(DriverList[Needed-1]);
				dinfo->InAWindow = GE_TRUE;
				dinfo->DriverNamePtr = DriverNamePtr;
				dinfo->ModeNamePtr   = ModeList_320WindowString;
				dinfo->Driver = m_Driver;
				dinfo->Mode = m_Mode;
				dinfo->DriverType = DriverType;
				dinfo->Width = 320;
				dinfo->Height = 240;
				dinfo->Evaluation = MODELIST_EVALUATED_OK;
			}
			else
			{
				strncpy(ModeName,ModeNamePtr, MODELIST_MAX_NAME);
				ModeName[MODELIST_MAX_NAME-1] = 0;		// just in case
				_strupr(ModeName);

				if(strstr(ModeName, "WIN") != NULL)
				{
					dinfo->InAWindow = GE_TRUE;
				}
				else
				{
					dinfo->InAWindow = GE_FALSE;
				}
			}

			m_Mode = geDriver_GetNextMode(m_Driver, m_Mode);
		}

		m_Driver = geDriver_SystemGetNextDriver(m_DrvSys, m_Driver);
	}

	*ListLength = Needed;
	return DriverList;

ModeList_Exit:

	if(DriverList != NULL)
	{
		ModeList_Destroy(DriverList);
	}

	return NULL;
}


/* ------------------------------------------------------------------------------------ */
//	This part does the Driver Dialog and Pick Routines
/* ------------------------------------------------------------------------------------ */

#define DRVLIST_OK		1
#define DRVLIST_CANCEL	2

#define DRVLIST_MAX_DISPLAY_MODES (40)

typedef struct
{
	ModeList *ModeList;
	int		  ModeListLength;
	int		  ModeListDriverCount;
	geDriver *IndexTable[DRVLIST_MAX_DISPLAY_MODES];
	int		  Selection;

} DrvList_LocalStruct;


static DrvList_LocalStruct DrvList_Locals;

/* ------------------------------------------------------------------------------------ */
//	DrvList_FillModeList
/* ------------------------------------------------------------------------------------ */
static geBoolean DrvList_FillModeList(HWND hwndDlg,int DriverNumber,DrvList_LocalStruct *DL)
{
	int		i;
	int		MaxCX = 0;
	SIZE	extents;
	HWND	ListBox;
	HDC		hDC;

	ListBox = GetDlgItem(hwndDlg, IDC_DRIVERLIST);
	if(ListBox == NULL)
		return GE_FALSE;

	hDC = GetDC(ListBox);
	if(hDC == NULL)
		return GE_FALSE;

	SendDlgItemMessage(hwndDlg, IDC_DRIVERLIST2, LB_RESETCONTENT, (WPARAM)0, (LPARAM)0);

	for(i=0; i<DL->ModeListLength; i++)
	{
		if(DL->IndexTable[DriverNumber] == DL->ModeList[i].Driver)
		{
			int index;
			index = SendDlgItemMessage(hwndDlg, IDC_DRIVERLIST2, LB_ADDSTRING, (WPARAM)0,
										(LPARAM)(DL->ModeList[i].ModeNamePtr));

			SendDlgItemMessage(hwndDlg, IDC_DRIVERLIST2, LB_SETITEMDATA, (WPARAM)index,(LPARAM)i);
			GetTextExtentPoint32(hDC, DL->ModeList[i].ModeNamePtr, strlen(DL->ModeList[i].ModeNamePtr), &extents);

			if(extents.cx > MaxCX)
				MaxCX = extents.cx;
		}
	}

	SendDlgItemMessage(hwndDlg, IDC_DRIVERLIST2, LB_SETCURSEL, (WPARAM)0, (LPARAM)0);
	SendDlgItemMessage(hwndDlg, IDC_DRIVERLIST2, LB_SETHORIZONTALEXTENT, (WPARAM)MaxCX, (LPARAM)0);
	ReleaseDC(ListBox, hDC);

	return GE_TRUE;
}

/* ------------------------------------------------------------------------------------ */
//	DlgProc
/* ------------------------------------------------------------------------------------ */
static BOOL	CALLBACK DlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DrvList_LocalStruct *DL = &DrvList_Locals;

	switch	(uMsg)
	{
	case WM_INITDIALOG:
		{
			HWND	DriverListBox;
			HDC		hDC;
			int		MaxCX;
			SIZE	extents;
			int		DriverNumber;
			int		i,j;

			DriverListBox = GetDlgItem(hwndDlg, IDC_DRIVERLIST);
			hDC = GetDC(DriverListBox);

			MaxCX = 0;

			DriverNumber = 0;

			for(i=0; i<DL->ModeListLength; i++)
			{
				int AlreadyAdded=0;

				for(j=0; j<i; j++)
				{
					if(DL->ModeList[j].Driver == DL->ModeList[i].Driver)	// only add one entry for each driver.
						AlreadyAdded = 1;
				}

				if(!AlreadyAdded)
				{
					int index;

					index = SendDlgItemMessage(hwndDlg, IDC_DRIVERLIST, LB_ADDSTRING, (WPARAM)0, (LPARAM)(DL->ModeList[i].DriverNamePtr));

					SendDlgItemMessage(hwndDlg, IDC_DRIVERLIST, LB_SETITEMDATA, (WPARAM)index,(LPARAM)DriverNumber);
					GetTextExtentPoint32(hDC, DL->ModeList[i].DriverNamePtr, strlen(DL->ModeList[i].DriverNamePtr), &extents);

					if(extents.cx > MaxCX)
						MaxCX = extents.cx;

					DL->IndexTable[DriverNumber] = DL->ModeList[i].Driver;
					DriverNumber ++;

				}

				if(DriverNumber >= DRVLIST_MAX_DISPLAY_MODES)
					break;
			}

			DL->ModeListDriverCount = DriverNumber;

			SendDlgItemMessage(hwndDlg, IDC_DRIVERLIST, LB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			SendDlgItemMessage(hwndDlg, IDC_DRIVERLIST, LB_SETHORIZONTALEXTENT, (WPARAM)MaxCX, (LPARAM)0);

			ReleaseDC(DriverListBox, hDC);

			DrvList_FillModeList(hwndDlg,0,DL);

			// position the dialog in the center of the desktop and set it as topmost window
			{
				RECT	ScreenRect, DlgRect;
				GetWindowRect(GetDesktopWindow(), &ScreenRect);
				GetWindowRect(hwndDlg, &DlgRect);

				SetWindowPos
				(
					hwndDlg, HWND_TOPMOST,
					(ScreenRect.right+ScreenRect.left)/2 - (DlgRect.right - DlgRect.left)/2,
					(ScreenRect.bottom+ScreenRect.top)/2 - (DlgRect.bottom - DlgRect.top)/2,
					1, 1, SWP_NOSIZE
				);
				UpdateWindow(hwndDlg);
			}

			return TRUE;
		}
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_DRIVERLIST:
			if(HIWORD(wParam) == LBN_SELCHANGE)
			{
				int Driver;

				Driver = SendDlgItemMessage(hwndDlg, IDC_DRIVERLIST, LB_GETCURSEL, (WPARAM)0, (LPARAM)0);

				if(Driver >= 0)
					DrvList_FillModeList(hwndDlg, Driver, DL);
			}
			break;
		case IDC_DRIVERLIST2:
			if(HIWORD(wParam) == LBN_DBLCLK)
			{
				int	index;

				index = SendDlgItemMessage(hwndDlg, IDC_DRIVERLIST2, LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
				if(index >= 0)
				{
					DL->Selection = SendDlgItemMessage(hwndDlg, IDC_DRIVERLIST2, LB_GETITEMDATA, index, 0);
					EndDialog(hwndDlg, DRVLIST_OK);
				}
			}
			break;
		case IDOK:
			{
				int	index;

				index = SendDlgItemMessage(hwndDlg, IDC_DRIVERLIST2, LB_GETCURSEL, (WPARAM)0, (LPARAM)0);

				if(index >= 0)
				{
					DL->Selection = SendDlgItemMessage(hwndDlg, IDC_DRIVERLIST2, LB_GETITEMDATA, index, 0);
					EndDialog(hwndDlg, DRVLIST_OK);
				}
			}
			break;
		case IDCANCEL:
			EndDialog(hwndDlg, DRVLIST_CANCEL);
			break;
		}
		break;
	}

	return 0;
}

/* ------------------------------------------------------------------------------------ */
//	DrvList_PickDriver
/* ------------------------------------------------------------------------------------ */
geBoolean DrvList_PickDriver(HANDLE hInstance, HWND hwndParent,
							 ModeList *List, int ListLength, int *ListSelection)
{
	int	res;
	DrvList_LocalStruct *DL = &DrvList_Locals;

	assert(hInstance != 0);
	assert(List  != NULL);
	assert(ListLength >= 0);
	assert(ListSelection != NULL);


	DL->ModeList			= List;
	DL->ModeListLength		= ListLength;
	DL->ModeListDriverCount = 0;
	DL->Selection			= -1;

	res = DialogBoxParam(hInstance,
						 MAKEINTRESOURCE(IDD_DRIVERDIALOG),
						 hwndParent,
						 DlgProc,
						 (LPARAM)0);

	*ListSelection = DL->Selection;

	if(res == DRVLIST_OK)
	{
		return GE_TRUE;
	}

	return GE_FALSE;
}

/* ------------------------------------------------------------------------------------ */
//	ResetMainWindow
/* ------------------------------------------------------------------------------------ */
// Dee 12-07-00
// Add to support driver and mode changes on the fly
void ResetMainWindow(HWND hWnd, int32 Width, int32 Height)
{
	RECT ScreenRect;

	ShowWindow(hWnd, SW_SHOWNORMAL);

	GetWindowRect(GetDesktopWindow(), &ScreenRect);

	{
		RECT ClientRect;
		int Style = GetWindowLong (hWnd, GWL_STYLE);

		ClientRect.left = 0;
		ClientRect.top = 0;
		ClientRect.right = Width - 1;
		ClientRect.bottom = Height - 1;
		AdjustWindowRect (&ClientRect, Style, FALSE);	// FALSE == No menu

		{
			int WindowWidth = ClientRect.right - ClientRect.left + 1;
			int WindowHeight = ClientRect.bottom - ClientRect.top + 1;

			SetWindowPos
			(
				hWnd, HWND_TOP,
				(ScreenRect.right-ScreenRect.left)/2 - WindowWidth/2,
				(ScreenRect.bottom-ScreenRect.top)/2 - WindowHeight/2,
				WindowWidth, WindowHeight,
				SWP_NOCOPYBITS | SWP_NOZORDER
			);
		}
	}

	UpdateWindow(hWnd);
	SetFocus(hWnd);
}
// End Dee

/* ----------------------------------- END OF FILE ------------------------------------ */
