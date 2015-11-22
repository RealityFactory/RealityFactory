/*
	CInventory.cpp:		Inventory class implementation

	(c) 2000 Ralph Deane
	All Rights Reserved

	This file contains the class implementation for Inventory
handling.
*/

//	Include the One True Header

#include "RabidFramework.h"

extern geSound_Def *SPool_Sound(char *SName);

//	Constructor
//

CInventory::CInventory()
{
	InvItem *item;
	background = NULL;
	highlight = NULL;
	Bottom = NULL;
	ZBottom = NULL;
	active = false;
	Selected = 0;
	MaxItems = 0;

	CIniFile AttrFile("Inventory.ini");
	if(!AttrFile.ReadFile())
		return;

	CString KeyName = AttrFile.FindFirstKey();
	char szName[128], szAlpha[128];
	CString Tname, Talpha;
	geBitmap_Info	BmpInfo;
	bool flag;

	while(KeyName != "")
	{
		if(KeyName=="Inventory")
		{
			Tname = AttrFile.GetValue(KeyName, "background");
			if(Tname!="")
			{
				Tname = "inventory\\"+Tname;
				strcpy(szName,Tname);
				background = CreateFromFileName(szName);
				if(background)
					flag = true;
			}
			geBitmap_GetInfo(background, &BmpInfo, NULL);
			backgroundx = (CCD->Engine()->Width() - BmpInfo.Width) / 2;
			backgroundy = (CCD->Engine()->Height() - BmpInfo.Height) / 2;
			Tname = AttrFile.GetValue(KeyName, "highlight");
			if(Tname!="" && flag)
			{
				Talpha = AttrFile.GetValue(KeyName, "highlightalpha");
				if(Talpha=="")
					Talpha = Tname;
				Tname = "inventory\\"+Tname;
				Talpha = "inventory\\"+Talpha;
				strcpy(szName,Tname);
				strcpy(szAlpha,Talpha);
				highlight = CreateFromFileAndAlphaNames(szName, szAlpha);
				if(highlight)
					flag = true;
			}
			if(!flag)
			{
				CCD->ReportError("Missing Data in Inventory", false);
				CCD->ShutdownLevel();
				delete CCD;
				MessageBox(NULL, "Missing Data in Inventory","Inventory Setup", MB_OK);
				exit(-333);
			}
			leftx = AttrFile.GetValueI(KeyName, "leftx");
			lefty = AttrFile.GetValueI(KeyName, "lefty");
			middlex = AttrFile.GetValueI(KeyName, "middlex");
			middley = AttrFile.GetValueI(KeyName, "middley");
			rightx = AttrFile.GetValueI(KeyName, "rightx");
			righty = AttrFile.GetValueI(KeyName, "righty");
			font = AttrFile.GetValueI(KeyName, "font");
			txtfont = AttrFile.GetValueI(KeyName, "textfont");
			keyclick = NULL;
			Tname = AttrFile.GetValue(KeyName, "keysound");
			if(Tname!="")
			{
				strcpy(szName,Tname);
				keyclick=SPool_Sound(szName);
			}
			geEngine_AddBitmap(CCD->Engine()->Engine(), background);
			geEngine_AddBitmap(CCD->Engine()->Engine(), highlight);

		}
		else
		{
			CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
			strcpy(szName, KeyName);
			int catg = -1;
			Tname = AttrFile.GetValue(KeyName, "catagory");
			if(Tname=="weapon")
				catg = INVWEAPON;
			if(Tname=="use")
				catg = INVUSE;
			if(Tname=="modify")
				catg = INVMODIFY;
			if(Tname=="load")
				catg = INVLOAD;
			if(Tname=="save")
				catg = INVSAVE;
			if(Tname=="static")
				catg = INVSTATIC;
			if(theInv->Has(szName) || catg==INVLOAD || catg==INVSAVE)
			{
				item = new InvItem;
				item->catagory = catg;
				strcpy(item->Attribute, KeyName);
				if(item->catagory==-1)
					delete item;
				else
				{
					if(item->catagory == INVMODIFY)
					{
						item->Modify[0] = '\0';
						Tname = AttrFile.GetValue(KeyName, "modifiedattribute");
						if(Tname!="")
						{
							strcpy(item->Modify, Tname);
							item->Amount = AttrFile.GetValueI(KeyName, "modifiedamount");
						}
					}
					if(item->catagory == INVUSE)
					{
						item->Decrease = false;
						Tname = AttrFile.GetValue(KeyName, "usedecrease");
						if(Tname="true")
							item->Decrease = true;
					}
					item->Image = NULL;
					Tname = AttrFile.GetValue(KeyName, "image");
					if(Tname!="")
					{
						Talpha = AttrFile.GetValue(KeyName, "imagealpha");
						if(Talpha=="")
							Talpha = Tname;
						Tname = "inventory\\"+Tname;
						Talpha = "inventory\\"+Talpha;
						strcpy(szName,Tname);
						strcpy(szAlpha,Talpha);
						item->Image = CreateFromFileAndAlphaNames(szName, szAlpha);
					}
					if(!item->Image)
						delete item;
					else
					{
						item->text[0] = '\0';
						Tname = AttrFile.GetValue(KeyName, "text");
						if(Tname!="")
							strcpy(item->text,Tname);

						geEngine_AddBitmap(CCD->Engine()->Engine(), item->Image);
						MaxItems += 1;
						item->next = NULL;
						item->prev = Bottom;
						if(Bottom != NULL) Bottom->next = item;
						Bottom = item;
					}
				}
			}
		}

		KeyName = AttrFile.FindNextKey();
	}
	active = true;
}


//	Destructor
//
//	Clean up.

CInventory::~CInventory()
{
	if(background != NULL)
		geBitmap_Destroy(&background);
	if(highlight != NULL)
		geBitmap_Destroy(&highlight);

	InvItem *item, *next;
	item = Bottom;
	while(item != NULL)
	{
		next = item->prev;
		geBitmap_Destroy(&item->Image);
		delete item;
		item = next;
	}
	Bottom = NULL;
}


void CInventory::Display()
{
	if(!active)
		return;
	geRect 	 M_CameraRect;
	geCamera 	 *M_Camera;
	M_CameraRect.Left = 0;
	M_CameraRect.Right = CCD->Engine()->Width() - 1;
	M_CameraRect.Top = 0;
	M_CameraRect.Bottom = CCD->Engine()->Height() - 1;
	M_Camera = geCamera_Create(2.0f, &M_CameraRect);
	GenerateList();

	while(1)
	{
		geEngine_BeginFrame(CCD->Engine()->Engine(), M_Camera, GE_TRUE);
		Blit();
		geEngine_EndFrame(CCD->Engine()->Engine());
		int key = CCD->Input()->GetKeyboardInput();
		if(key==KEY_RETURN)
		{
			int cat = GetCatagory(Selected);
			CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
			if(cat==INVUSE)
			{
				if(theInv->Value(GetAttribute(Selected))>0)
				{
					if(keyclick)
						geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
					if(!EffectC_IsStringNull(CCD->Player()->GetUseAttribute()))
						CCD->HUD()->ActivateElement(CCD->Player()->GetUseAttribute(), false);
					CCD->Player()->SetUseAttribute(GetAttribute(Selected));
					CCD->HUD()->ActivateElement(GetAttribute(Selected), true);
					if(GetDecrease(Selected))
						theInv->Modify(GetAttribute(Selected), -1);
				}
			}
			if(cat==INVMODIFY)
			{
				if(theInv->Value(GetAttribute(Selected))>theInv->Low(GetAttribute(Selected)))
				{
					if(theInv->Has(GetModified(Selected)))
					{
						if(!strcmp(GetModified(Selected), "light"))
						{
							if(theInv->Value("light")>0)
							{
								if(keyclick)
									geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
								theInv->Modify(GetAttribute(Selected), -1);
								CCD->Player()->ModifyLight(GetModifiedAmount(Selected));
								GenerateList();
							}
						}
						else
						{
							if(keyclick)
								geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
							theInv->Modify(GetAttribute(Selected), -1);
							theInv->Modify(GetModified(Selected), GetModifiedAmount(Selected));
							GenerateList();
						}
					}
				}
			}
			if(cat==INVWEAPON)
			{
				if(theInv->Value(GetAttribute(Selected))>0)
				{
					if(keyclick)
						geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
					CCD->Weapons()->ChangeWeapon(GetAttribute(Selected));
				}
			}
			if(cat==INVLOAD)
			{
			}
			if(cat==INVSAVE)
			{
			}
		}
		if(key==KEY_ESC)
			break;
		if(key==KEY_RBRACKET)
		{
			if(keyclick)
				geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
			Selected-=1;
			if(Selected<0)
				Selected = ZMaxItems-1;

		}
		if(key==KEY_LBRACKET)
		{
			if(keyclick)
				geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
			Selected+=1;
			if(Selected>(ZMaxItems-1))
				Selected = 0;
		}
	}
	InvItem *item, *next;

	item = ZBottom;
	while(item != NULL)
	{
		next = item->prev;
		delete item;
		item = next;
	}

	ZBottom = NULL;
	geCamera_Destroy(&M_Camera);
	CCD->ResetClock();
	CCD->Engine()->BeginFrame();
	CCD->Engine()->EndFrame();
	CCD->ResetClock();
}

void CInventory::Blit()
{
	geBitmap_Info	BmpInfo;
	geBitmap *Bmp;
	char szText[64];
	int right = -1;
	int middle = -1;
	int left = -1;

	geEngine_DrawBitmap(CCD->Engine()->Engine(), background, NULL, backgroundx, backgroundy );

	if(ZMaxItems>0)
	{
		if(Selected>=ZMaxItems)
			Selected = ZMaxItems-1;
		if(ZMaxItems==1)
		{
			middle = 0;
		}
		else if(ZMaxItems==2)
		{
			if(Selected==0)
			{
				middle = 0;
				right = 1;
			}
			else
			{
				middle = 1;
				left = 0;
			}
		}
		else
		{
			if(Selected==0)
			{
				left = ZMaxItems-1;
				middle = Selected;
				right = Selected+1;
			}
			else
			{
				left = Selected-1;
				middle = Selected;
				if(Selected<(ZMaxItems-1))
					right = Selected+1;
				else
					right = 0;
			}
		}
		if(left!=-1)
		{
			Bmp = GetImage(left);
			geBitmap_GetInfo(Bmp, &BmpInfo, NULL);
			geEngine_DrawBitmap(CCD->Engine()->Engine(), Bmp, NULL, backgroundx+leftx, backgroundy+lefty);
		}
		if(middle!=-1)
		{
			Bmp = GetImage(middle);
			geBitmap_GetInfo(Bmp, &BmpInfo, NULL);
			geEngine_DrawBitmap(CCD->Engine()->Engine(), Bmp, NULL, backgroundx+middlex, backgroundy+middley);
			if(GetAmount(middle)>0)
			{
				sprintf(szText, "%d", GetAmount(middle));
				CCD->MenuManager()->FontRect(szText, font,
					backgroundx+middlex + (BmpInfo.Width/2) - (CCD->MenuManager()->FontWidth(font, szText)/2),
					backgroundy+middley+BmpInfo.Height+2);
			}
			if(!EffectC_IsStringNull(GetText(middle)))
				{
					CCD->MenuManager()->FontRect(GetText(middle), txtfont,
						backgroundx+middlex + (BmpInfo.Width/2) - (CCD->MenuManager()->FontWidth(txtfont, GetText(middle))/2),
						backgroundy+middley-CCD->MenuManager()->FontHeight(txtfont)-2);
				}
		}
		if(right!=-1)
		{
			Bmp = GetImage(right);
			geBitmap_GetInfo(Bmp, &BmpInfo, NULL);
			geEngine_DrawBitmap(CCD->Engine()->Engine(), Bmp, NULL, backgroundx+rightx, backgroundy+righty);
		}
	}
	geEngine_DrawBitmap(CCD->Engine()->Engine(), highlight, NULL, backgroundx+middlex, backgroundy+middley);
}

geBitmap *CInventory::GetImage(int index)
{
	InvItem *item, *next;
	int count = ZMaxItems-1;

	item = ZBottom;
	while(item != NULL)
	{
		next = item->prev;
		if(count==index)
		{
			return item->Image;
		}
		count-=1;
		item = next;
	}
	return NULL;
}


int CInventory::GetAmount(int index)
{
	InvItem *item, *next;
	int count = ZMaxItems-1;
	
	CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());

	item = ZBottom;
	while(item != NULL)
	{
		next = item->prev;
		if(count==index)
		{
			if(item->catagory==INVLOAD || item->catagory==INVSAVE)
				return 0;
			if(theInv->Value(item->Attribute)==1 && theInv->High(item->Attribute)==1)
				return 0;
			return theInv->Value(item->Attribute);
		}
		count-=1;
		item = next;
	}
	return 0;
}

int CInventory::GetCatagory(int index)
{
	InvItem *item, *next;
	int count = ZMaxItems-1;

	item = ZBottom;
	while(item != NULL)
	{
		next = item->prev;
		if(count==index)
		{
			return item->catagory;
		}
		count-=1;
		item = next;
	}
	return 0;
}

char *CInventory::GetAttribute(int index)
{
	InvItem *item, *next;
	int count = ZMaxItems-1;

	item = ZBottom;
	while(item != NULL)
	{
		next = item->prev;
		if(count==index)
		{
			return item->Attribute;
		}
		count-=1;
		item = next;
	}
	return "";
}

int CInventory::GetModifiedAmount(int index)
{
	InvItem *item, *next;
	int count = ZMaxItems-1;

	item = ZBottom;
	while(item != NULL)
	{
		next = item->prev;
		if(count==index)
		{
			return item->Amount;
		}
		count-=1;
		item = next;
	}
	return 0;
}

bool CInventory::GetDecrease(int index)
{
	InvItem *item, *next;
	int count = ZMaxItems-1;

	item = ZBottom;
	while(item != NULL)
	{
		next = item->prev;
		if(count==index)
		{
			return item->Decrease;
		}
		count-=1;
		item = next;
	}
	return false;
}

char *CInventory::GetModified(int index)
{
	InvItem *item, *next;
	int count = ZMaxItems-1;

	item = ZBottom;
	while(item != NULL)
	{
		next = item->prev;
		if(count==index)
		{
			return item->Modify;
		}
		count-=1;
		item = next;
	}
	return "";
}

char *CInventory::GetText(int index)
{
	InvItem *item, *next;
	int count = ZMaxItems-1;

	item = ZBottom;
	while(item != NULL)
	{
		next = item->prev;
		if(count==index)
		{
			return item->text;
		}
		count-=1;
		item = next;
	}
	return "";
}

void CInventory::GenerateList()
{
	InvItem *item, *zitem, *next;
	
	CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());

	item = ZBottom;
	while(item != NULL)
	{
		next = item->prev;
		delete item;
		item = next;
	}

	ZBottom = NULL;
	ZMaxItems = 0;

	item = Bottom;
	while(item->prev != NULL)
	{
		item = item->prev;
	}

	while(item!=NULL)
	{
		if(theInv->Value(item->Attribute)>0 || item->catagory==INVLOAD || item->catagory==INVSAVE)
		{
			zitem = new InvItem;
			memcpy(zitem, item, sizeof(InvItem));
			ZMaxItems += 1;
			zitem->next = NULL;
			zitem->prev = ZBottom;
			if(ZBottom != NULL) ZBottom->next = zitem;
			ZBottom = zitem;
		}
		item = item->next;
	}
}
