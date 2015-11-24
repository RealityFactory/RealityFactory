/*
CInventory.cpp:		Inventory class implementation

  (c) 2000 Ralph Deane
  All Rights Reserved
  
	This file contains the class implementation for Inventory
	handling.

  Modified by QuestOfDreams for better layout
*/

//	Include the One True Header

#include "RabidFramework.h"

extern geSound_Def *SPool_Sound(char *SName);

//	Constructor
//

CInventory::CInventory()
{
	////////////////////////////////////// change 
	for(int nTemp1 = 0; nTemp1 < NUMOFSUB; nTemp1++)
		for(int nTemp2 = 0; nTemp2 < MAXENTRIES; nTemp2++)
			SubInv[nTemp1][nTemp2] = NULL;
		
		for(nTemp1 = 0; nTemp1 < 4; nTemp1++)
			Main[nTemp1] = NULL;
		
		background = NULL;
		highlight = NULL;
		active = false;
		Selected = 1;
		MaxItems = 0;
		MaxWeapons = 0;
		MaxMain = 0;
		page=0;
		isactive = false;
		weaponactive= false;
		itemactive = false;
		keyreturn = false;
		keyesc = false;
		keyup = false;
		keydown = false;
		keyleft = false;
		keyright = false;
		////////////////////////////////////// end change
		
		CIniFile AttrFile("Inventory.ini");
		if(!AttrFile.ReadFile())
			return;
		
		CString KeyName = AttrFile.FindFirstKey();
		char szName[128], szAlpha[128];
		CString Tname, Talpha;
		geBitmap_Info	BmpInfo;
		bool flag = false;
		
		while(KeyName != "")
		{
			if(KeyName=="Inventory")
			{
				Tname = AttrFile.GetValue(KeyName, "background");
				if(Tname!="")
				{
					Talpha = AttrFile.GetValue(KeyName, "backgroundalpha");
					if(Talpha=="")
						Talpha = Tname;
					Tname = "inventory\\"+Tname;
					Talpha = "inventory\\"+Talpha;
					strcpy(szName,Tname);
					strcpy(szAlpha,Talpha);
					background = CreateFromFileAndAlphaNames(szName, szAlpha);
					
					if(background)
					{
						flag = true;
						geBitmap_GetInfo(background, &BmpInfo, NULL);
						backgroundx = (CCD->Engine()->Width() - BmpInfo.Width) / 2;
						backgroundy = (CCD->Engine()->Height() - BmpInfo.Height) / 2;
					}
				}

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
					////////////////////////////////////////// change
					if(!highlight)
						flag = false;
				}
				else
					flag = false;

				Tname = AttrFile.GetValue(KeyName, "arrowr");
				if(Tname!="" && flag)
				{
					Talpha = AttrFile.GetValue(KeyName, "arrowralpha");
					if(Talpha=="")
						Talpha = Tname;
					Tname = "inventory\\"+Tname;
					Talpha = "inventory\\"+Talpha;
					strcpy(szName,Tname);
					strcpy(szAlpha,Talpha);
					arrowr = CreateFromFileAndAlphaNames(szName, szAlpha);
					if(!arrowr)
						flag = false;
				}
				else
					flag = false;

				Tname = AttrFile.GetValue(KeyName, "arrowl");
				if(Tname!="" && flag)
				{
					Talpha = AttrFile.GetValue(KeyName, "arrowlalpha");
					if(Talpha=="")
						Talpha = Tname;
					Tname = "inventory\\"+Tname;
					Talpha = "inventory\\"+Talpha;
					strcpy(szName,Tname);
					strcpy(szAlpha,Talpha);
					arrowl = CreateFromFileAndAlphaNames(szName, szAlpha);
					if(!arrowl)
						flag = false;
				}
				else
					flag = false;

				///////////////////////////////// end change
				if(!flag)
				{
					CCD->ReportError("Missing Data in Inventory", false);
					CCD->ShutdownLevel();
					delete CCD;
					MessageBox(NULL, "Missing Data in Inventory","Inventory Setup", MB_OK);
					exit(-333);
				}
				
				/////////////////////////// change
				//to give each Item independent x,y positions add them here and change the Blit function
				leftx = AttrFile.GetValueI(KeyName, "leftx");
				middlex = AttrFile.GetValueI(KeyName, "middlex");
				rightx = AttrFile.GetValueI(KeyName, "rightx");
				
				topy = AttrFile.GetValueI(KeyName, "topy");
				middley = AttrFile.GetValueI(KeyName, "middley");
				bottomy = AttrFile.GetValueI(KeyName, "bottomy");
				
				arrowrx = AttrFile.GetValueI(KeyName, "arrowrx");
				arrowlx = AttrFile.GetValueI(KeyName, "arrowlx");
				/////////////////////////// end change			
				font = AttrFile.GetValueI(KeyName, "font");
				txtfont = AttrFile.GetValueI(KeyName, "textfont");
				
				stoptime = true;
				Tname = AttrFile.GetValue(KeyName, "stoptime");
				if(Tname=="false")
					stoptime = false;
				
				keyclick = NULL;
				Tname = AttrFile.GetValue(KeyName, "keysound");
				if(Tname!="")
				{
					strcpy(szName,Tname);
					keyclick=SPool_Sound(szName);
				}
				
				geEngine_AddBitmap(CCD->Engine()->Engine(), background);
				geEngine_AddBitmap(CCD->Engine()->Engine(), highlight);
				/////////////////////////// change
				geEngine_AddBitmap(CCD->Engine()->Engine(), arrowr);
				geEngine_AddBitmap(CCD->Engine()->Engine(), arrowl);
				/////////////////////////// end change
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
			if(Tname=="graphic")
				catg = INVGRAPHIC;
			/////////////////////////// change
			if(Tname=="weaponinv")
				catg = WEAPONINV;
			if(Tname=="iteminv")
				catg = ITEMINV;
			if(catg == -1)
				continue;
			if(theInv->Has(szName) || catg==INVLOAD || catg==INVSAVE || catg==WEAPONINV || catg==ITEMINV)
			{
				
				if(catg==INVLOAD || catg==INVSAVE || catg==WEAPONINV || catg==ITEMINV)
				{
					if(MaxMain>3)
						continue;
					Main[MaxMain] = new InvItem;
					Main[MaxMain]->catagory = catg;
					strcpy(Main[MaxMain]->Attribute, KeyName);
					if(catg==INVLOAD)
						Main[MaxMain]->index = 5;
					if(catg==INVSAVE)
						Main[MaxMain]->index = 3;
					if(catg==WEAPONINV)
						Main[MaxMain]->index = 7;
					if(catg==ITEMINV)
						Main[MaxMain]->index = 1;
					Main[MaxMain]->Graphic = NULL;
					Main[MaxMain]->Image = NULL;
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
						Main[MaxMain]->Image = CreateFromFileAndAlphaNames(szName, szAlpha);
					}
					if(!Main[MaxMain]->Image)
					{
						delete Main[MaxMain];
						Main[MaxMain]=NULL;
						continue;
					}
					else
					{
						Main[MaxMain]->text[0] = '\0';
						Tname = AttrFile.GetValue(KeyName, "text");
						if(Tname!="")
							strcpy(Main[MaxMain]->text,Tname);
						
						geEngine_AddBitmap(CCD->Engine()->Engine(), Main[MaxMain]->Image);
						
					}
					
					MaxMain++;
				}
				
				else if(catg== INVWEAPON)
				{
					if(MaxWeapons>=MAXENTRIES)
						continue;
					SubInv[WEAPONINV][MaxWeapons] = new InvItem;
					SubInv[WEAPONINV][MaxWeapons]->Graphic = NULL;
					SubInv[WEAPONINV][MaxWeapons]->catagory = catg;
					strcpy(SubInv[WEAPONINV][MaxWeapons]->Attribute, KeyName);
					SubInv[WEAPONINV][MaxWeapons]->index = MaxWeapons;
					SubInv[WEAPONINV][MaxWeapons]->Image = NULL;
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
						SubInv[WEAPONINV][MaxWeapons]->Image = CreateFromFileAndAlphaNames(szName, szAlpha);
					}
					if(!SubInv[WEAPONINV][MaxWeapons]->Image)
					{
						delete SubInv[WEAPONINV][MaxWeapons];
						SubInv[WEAPONINV][MaxWeapons]=NULL;
						continue;
					}
					else
					{
						SubInv[WEAPONINV][MaxWeapons]->text[0] = '\0';
						Tname = AttrFile.GetValue(KeyName, "text");
						if(Tname!="")
							strcpy(SubInv[WEAPONINV][MaxWeapons]->text,Tname);
						
						geEngine_AddBitmap(CCD->Engine()->Engine(), SubInv[WEAPONINV][MaxWeapons]->Image);
						
					}
					
					MaxWeapons++;
				}
				
				else
				{
					if(MaxItems>=MAXENTRIES)
						continue;
					SubInv[ITEMINV][MaxItems] = new InvItem;
					SubInv[ITEMINV][MaxItems]->Graphic = NULL;
					SubInv[ITEMINV][MaxItems]->catagory = catg;
					strcpy(SubInv[ITEMINV][MaxItems]->Attribute, KeyName);
					SubInv[ITEMINV][MaxItems]->index = MaxItems;
					
					if(SubInv[ITEMINV][MaxItems]->catagory == INVMODIFY)
					{
						SubInv[ITEMINV][MaxItems]->Modify[0] = '\0';
						Tname = AttrFile.GetValue(KeyName, "modifiedattribute");
						if(Tname!="")
						{
							strcpy(SubInv[ITEMINV][MaxItems]->Modify, Tname);
							SubInv[ITEMINV][MaxItems]->Amount = AttrFile.GetValueI(KeyName, "modifiedamount");
						}
					}
					if(SubInv[ITEMINV][MaxItems]->catagory == INVUSE)
					{
						SubInv[ITEMINV][MaxItems]->Decrease = false;
						Tname = AttrFile.GetValue(KeyName, "usedecrease");
						
						if(Tname=="true")
							SubInv[ITEMINV][MaxItems]->Decrease = true;
					}
					SubInv[ITEMINV][MaxItems]->Graphic = NULL;
					if(SubInv[ITEMINV][MaxItems]->catagory == INVGRAPHIC)
					{
						Tname = AttrFile.GetValue(KeyName, "graphic");
						if(Tname!="")
						{
							Talpha = AttrFile.GetValue(KeyName, "graphicalpha");
							if(Talpha=="")
								Talpha = Tname;
							Tname = "inventory\\"+Tname;
							Talpha = "inventory\\"+Talpha;
							strcpy(szName,Tname);
							strcpy(szAlpha,Talpha);
							SubInv[ITEMINV][MaxItems]->Graphic = CreateFromFileAndAlphaNames(szName, szAlpha);
						}
					}

					SubInv[ITEMINV][MaxItems]->Image = NULL;
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
						SubInv[ITEMINV][MaxItems]->Image = CreateFromFileAndAlphaNames(szName, szAlpha);
					}
					if(!SubInv[ITEMINV][MaxItems]->Image)
					{
						delete SubInv[ITEMINV][MaxItems];
						SubInv[ITEMINV][MaxItems]=NULL;
						continue;
					}
					else
					{
						SubInv[ITEMINV][MaxItems]->text[0] = '\0';
						Tname = AttrFile.GetValue(KeyName, "text");
						if(Tname!="")
							strcpy(SubInv[ITEMINV][MaxItems]->text,Tname);
						
						geEngine_AddBitmap(CCD->Engine()->Engine(), SubInv[ITEMINV][MaxItems]->Image);
						if(SubInv[ITEMINV][MaxItems]->Graphic)
							geEngine_AddBitmap(CCD->Engine()->Engine(), SubInv[ITEMINV][MaxItems]->Graphic);
						
					}
					
					MaxItems++;
				}
			}
		}
		/////////////////////////// end change
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
	/////////////////////////// change
	if(arrowr != NULL)
		geBitmap_Destroy(&arrowr);
	if(arrowl != NULL)
		geBitmap_Destroy(&arrowl);
	
	for(int index1 = 0; index1 < 4; index1++)
	{
		if(Main[index1]!=NULL)
		{
			geBitmap_Destroy(&(Main[index1]->Image));
			if(Main[index1]->Graphic)
				geBitmap_Destroy(&Main[index1]->Graphic);
		}
		delete Main[index1];
		Main[index1] = NULL;
	}
	for(index1 = 0; index1 < NUMOFSUB; index1++)
		for(int index2 = 0; index2 < MAXENTRIES; index2++)
		{
			if(SubInv[index1][index2]!=NULL)
			{
				geBitmap_Destroy(&(SubInv[index1][index2]->Image));
				if(SubInv[index1][index2]->Graphic)
					geBitmap_Destroy(&(SubInv[index1][index2]->Graphic));
			}
			delete SubInv[index1][index2];
			SubInv[index1][index2] = NULL;
		}
		/////////////////////////// end change
		
}


void CInventory::SetActive(bool flag)
{
	isactive = true;
	/////////////////////////// change
	GenerateList(WEAPONINV);
	GenerateList(ITEMINV);
	/////////////////////////// end change
}


void CInventory::Display()
{
	// changed RF064
	bool ret = false;
	bool esc = false;
	/////////////////////////// change
	bool up =false;
	bool down = false;
	bool right = false;
	bool left = false;
	/////////////////////////// end change
	if(!isactive)
	{
		CCD->Weapons()->DisplayZoom();
		CCD->HUD()->Render();
		return;
	}
	
	CCD->HUD()->Deactivate();
	
	/////////////////////////// change
	if(weaponactive)
	{
		DisplaySubInv(WEAPONINV);
		return;
	}
	else if(itemactive)
	{
		DisplaySubInv(ITEMINV);
		return;
	}
	
	Blit(MAININV);
	
	/////////////////////////// end change
	
	int key = CCD->Input()->GetKeyboardInputNoWait();
	if(key==KEY_RETURN)
	{
		ret = true;
		if(!keyreturn)
		{
			keyreturn = true;
			int cat = GetCatagory(Selected, MAININV);
			
			/////////////////////////// change			
			if(cat==WEAPONINV)
			{
				if(keyclick)
					geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
				weaponactive = true;
				Selected = 0;
				page = 0;
			}
			
			if(cat==ITEMINV)
			{
				if(keyclick)
					geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
				itemactive = true;
				Selected = 0;
				page = 0;
			}
			/////////////////////////// end change
			if(cat==INVLOAD)
			{
			}
			if(cat==INVSAVE)
			{
			}
		}
	}
	if(key==KEY_ESC)
	{
		esc = true;
		if(!keyesc)
		{
			keyesc = true;
		}
	}
	/////////////////////////// change	
	if(key==KEY_UP && (GetCatagory(3,MAININV) != -1))
	{
		up = true;
		if(!keyup)
		{
			keyup = true;
			if(keyclick)
				geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
			Selected = 3;
		}
	}
	if(key==KEY_DOWN && (GetCatagory(5,MAININV) != -1))
	{
		down = true;
		if(!keydown)
		{
			keydown = true;
			if(keyclick)
				geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
			Selected = 5;
		}
	}
	if(key==KEY_RIGHT)
	{
		right = true;
		if(!keyright)
		{
			keyright = true;
			if(keyclick)
				geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
			Selected = 7;
		}
	}
	if(key==KEY_LEFT)
	{
		left = true;
		if(!keyleft)
		{
			keyleft = true;
			if(keyclick)
				geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
			Selected = 1;
		}
	}
	
	if(!ret)
		keyreturn = false;
	if(!esc)
	{
		if(keyesc)
		{
			isactive = false;
			// changed RF064
			CCD->HUD()->Activate();
			// end change RF064
			CCD->SetKeyPaused(false);
		}
		keyesc = false;
	}
	/////////////////////////// end change
	if(!up)
		keyup = false;
	if(!down)
		keydown = false;
	if(!right)
		keyright = false;
	if(!left)
		keyleft = false;
	
	// end change RF064
}

/////////////////////////// change
void CInventory::Blit(int menu)
{
	geBitmap_Info	BmpInfo;
	geBitmap *Bmp;
	char szText[64];
	int x;
	int y;
	int count;
	
	geEngine_DrawBitmap(CCD->Engine()->Engine(), background, NULL, backgroundx, backgroundy );
	
	// Draw items/weapons
	for(count=0;count<9;count++)
	{
		Bmp = GetImage(count+page*9, menu);
		if(!Bmp)
			continue;
		geBitmap_GetInfo(Bmp, &BmpInfo, NULL);
		
		if(count < 3)
			x = backgroundx+leftx;
		else if(count > 5)
			x = backgroundx+rightx;
		else
			x = backgroundx+middlex;
		
		if(count == 0 || count == 3 || count == 6)
			y = backgroundy+topy;
		else if(count == 1 || count == 4 || count == 7)
			y = backgroundy+middley;
		else
			y = backgroundy+bottomy;
		
		
		if(count == Selected)
		{			
			if(GetAmount(Selected+page*9, menu)>0)
			{
				sprintf(szText, "%d", GetAmount(Selected+page*9, menu));
				CCD->MenuManager()->FontRect(szText, font,
					x + (BmpInfo.Width/2) - (CCD->MenuManager()->FontWidth(font, szText)/2),
					y+BmpInfo.Height+2);
			}
			
			if(CCD->Player()->GetUseAttribute(GetAttribute(Selected+page*9, menu)))
			{
				CCD->MenuManager()->FontRect("Selected for Use", txtfont,
					x + (BmpInfo.Width/2) - (CCD->MenuManager()->FontWidth(txtfont, "Selected for Use")/2),
					y+BmpInfo.Height+2+CCD->MenuManager()->FontHeight(txtfont)+2);
			}
			int cat = GetCatagory(Selected+page*9, menu);
			if(cat==INVWEAPON)
			{
				if(!strcmp(GetAttribute(Selected+page*9, menu), CCD->Weapons()->GetWeaponName()))
				{
					CCD->MenuManager()->FontRect("Current Selected Weapon", txtfont,
						x + (BmpInfo.Width/2) - (CCD->MenuManager()->FontWidth(txtfont, "Current Selected Weapon")/2),
						y+BmpInfo.Height+2+CCD->MenuManager()->FontHeight(txtfont)+2); 
				}
			} 
			
			
			if(!EffectC_IsStringNull(GetText(Selected+page*9, menu)))
			{
				CCD->MenuManager()->FontRect(GetText(Selected+page*9, menu), txtfont,
					x + (BmpInfo.Width/2) - (CCD->MenuManager()->FontWidth(txtfont, GetText(Selected+page*9, menu))/2),
					y-CCD->MenuManager()->FontHeight(txtfont)-2);
			}
		}
		geEngine_DrawBitmap(CCD->Engine()->Engine(), Bmp, NULL, x, y );		
	}
	
	// Draw highlight
	if(Selected < 3)
		x = backgroundx+leftx;
	else if(Selected > 5)
		x = backgroundx+rightx;
	else
		x = backgroundx+middlex;
	
	if(Selected == 0 || Selected == 3 || Selected == 6)
		y = backgroundy+topy;
	else if(Selected == -1 || Selected == 1 || Selected == 4 || Selected == 7 || Selected== 9)
		y = backgroundy+middley;
	else
		y = backgroundy+bottomy;
	
	if(Selected == -1)
		x = backgroundx+arrowlx;
	else if(Selected == 9)
		x = backgroundx+arrowrx;
	
	geEngine_DrawBitmap(CCD->Engine()->Engine(), highlight, NULL, x, y);
	
	int cat = GetCatagory(Selected+page*9, menu);
	if(cat==INVGRAPHIC && keyreturn)
	{
		Bmp = GetGraphic(Selected+page*9, menu);
		if(Bmp)
		{
			geBitmap_GetInfo(Bmp, &BmpInfo, NULL);
			geEngine_DrawBitmap(CCD->Engine()->Engine(), Bmp, NULL, (CCD->Engine()->Width()/2)-(BmpInfo.Width/2), (CCD->Engine()->Height()/2)-(BmpInfo.Height/2));
		}
	}
	
	// Draw arrows, only if needed
	if((menu == WEAPONINV && MaxWeapons > 9) || (menu == ITEMINV && MaxItems > 9))
	{
		geEngine_DrawBitmap(CCD->Engine()->Engine(), arrowr, NULL, backgroundx+arrowrx, backgroundy+middley);
		geEngine_DrawBitmap(CCD->Engine()->Engine(), arrowl, NULL, backgroundx+arrowlx, backgroundy+middley);
	}
}


void CInventory::DisplaySubInv(int menu)
{
	bool ret = false;
	bool esc = false;
	bool up =false;
	bool down = false;
	bool right = false;
	bool left = false;
	int  MaxEntries;
	
	if(menu == WEAPONINV)
		MaxEntries = MaxWeapons;
	else if(menu == ITEMINV)
		MaxEntries = MaxItems;
	
	
	Blit(menu);
	int key = CCD->Input()->GetKeyboardInputNoWait();
	
	if(key==KEY_RETURN)
	{
		if(Selected!=-1 && Selected !=9)
		{
			
			ret = true;
			if(!keyreturn)
			{
				keyreturn = true;
				
				int cat = GetCatagory(Selected+page*9, menu);
				CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
				
				if(cat==INVUSE)
				{
					if(theInv->Value(GetAttribute(Selected+page*9, ITEMINV))>0)
					{
						if(keyclick)
							geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
						if(CCD->Player()->GetUseAttribute(GetAttribute(Selected+page*9, ITEMINV)))
						{
							CCD->HUD()->ActivateElement(GetAttribute(Selected+page*9, ITEMINV), false);
							CCD->Player()->DelUseAttribute(GetAttribute(Selected+page*9, ITEMINV));
						}
						else
						{
							if(CCD->Player()->SetUseAttribute(GetAttribute(Selected+page*9, ITEMINV)))
							{
								CCD->HUD()->ActivateElement(GetAttribute(Selected+page*9, ITEMINV), true);
								if(GetDecrease(Selected+page*9, ITEMINV))
								{
									theInv->Modify(GetAttribute(Selected+page*9, ITEMINV), -1);
									if(theInv->Value(GetAttribute(Selected, ITEMINV))==0)
										GenerateList(ITEMINV);
								}
							}
						}
					}
				}
				if(cat==INVMODIFY)
				{	
					if(theInv->Value(GetAttribute(Selected+page*9, ITEMINV))>theInv->Low(GetAttribute(Selected+page*9, ITEMINV)))
					{
						if(theInv->Has(GetModified(Selected+page*9, ITEMINV)))
						{
							if(!strcmp(GetModified(Selected+page*9, ITEMINV), "light"))
							{
								if(theInv->Value("light")>0)
								{
									if(keyclick)
										geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
									theInv->Modify(GetAttribute(Selected+page*9, ITEMINV), -1);
									CCD->Player()->ModifyLight(GetModifiedAmount(Selected+page*9, ITEMINV));
									GenerateList(ITEMINV);
								}
							}
							else
							{
								if(keyclick)
									geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
								theInv->Modify(GetAttribute(Selected+page*9, ITEMINV), -1);
								theInv->Modify(GetModified(Selected+page*9, ITEMINV), GetModifiedAmount(Selected+page*9, ITEMINV));
								GenerateList(ITEMINV);
							}
						}
					}
				}
				
				
				if(cat==INVWEAPON)
				{
					if(theInv->Value(GetAttribute(Selected+page*9, WEAPONINV))>0)
					{
						if(keyclick)
							geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
						CCD->Weapons()->ChangeWeapon(GetAttribute(Selected+page*9, WEAPONINV));
					}
				}
			}		
		}
	}
	if(key==KEY_ESC)
	{
		esc = true;
		if(!keyesc)
		{
			keyesc = true;
		}
	}
	if(key==KEY_UP)
	{
		up = true;
		if(!keyup)
		{
			keyup = true;
			if(Selected != -1 && Selected != 0 && Selected != 3 && Selected != 6 && Selected != 9)
			{
				Selected--;
				if(keyclick)
					geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
			}
			else if(Selected == -1)
			{
				Selected = 0;
				if(keyclick)
					geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
			}
			else if(Selected == 9)
			{
				Selected = 6;
				if(keyclick)
					geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
			}
		}
	}
	if(key==KEY_DOWN)
	{
		down = true;
		if(!keydown)
		{
			keydown = true;
			if(Selected != -1 && Selected != 2 && Selected != 5 && Selected != 8 && Selected != 9)
			{
				Selected ++;
				if(keyclick)
					geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
			}
			else if(Selected == -1)
			{
				Selected = 2;
				if(keyclick)
					geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
			}
			else if(Selected == 9)
			{
				Selected = 8;
				if(keyclick)
					geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
			}
		}
	}
	if(key==KEY_RIGHT)
	{
		right = true;
		if(!keyright)
		{
			keyright = true;
			if(keyclick)
				geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
			if((Selected == 6 || Selected == 7 || Selected == 8) && MaxEntries > 9)
				Selected = 9;
			else if(Selected == -1)
				Selected = 1;
			else if(Selected == 9)
			{
				if((page+1)*9<(MaxEntries))
					page++;
				else
					page = 0;
				Selected = -1;
			}
			else 
			{
				Selected += 3;
				if(Selected > 8)
				{
					if(MaxEntries > 9)
						Selected=9;
					else
						Selected-= 3;
				}
			}
			
		}
	}
	if(key==KEY_LEFT)
	{
		left = true;
		if(!keyleft)
		{
			keyleft = true;
			if(keyclick)
				geSound_PlaySoundDef(CCD->Engine()->AudioSystem(), keyclick, 0.99f, 0.5f, 1.0f, false);
			
			if((Selected == 0|| Selected == 1|| Selected ==2) && MaxEntries > 9)
				Selected = -1;
			else if(Selected == 9)
				Selected = 7;
			else if(Selected == -1)
			{	
				page--;
				if(page<0)
					while((page+1)*9<(MaxEntries))
						page++;
					Selected = 9;
			}
			else
			{
				Selected -= 3;
				if(Selected < 0)
				{	
					if(MaxEntries > 9)
						Selected = -1;
					else
						Selected+= 3;
				}
			}
		}
	}
	
	if(!ret)
		keyreturn = false;
	if(!esc)
	{
		if(keyesc)
		{
			itemactive  = false;
			weaponactive = false;
			Selected = 1;
			page=0;
		}
		keyesc = false;
	}
	if(!up)
		keyup = false;
	if(!down)
		keydown = false;
	if(!right)
		keyright = false;
	if(!left)
		keyleft = false;
	
}


geBitmap *CInventory::GetImage(int index, int menu)
{
	int count;
	
	if(menu == MAININV)
	{
		for(count=0;count<4; count++)
		{	
			if(Main[count]!=NULL)
				if(index==Main[count]->index)
					return Main[count]->Image;
		}
	}
	else
	{
		for(count=0;count<MAXENTRIES; count++)
		{	
			if(SubInv[menu][count]!=NULL)
				if(index==SubInv[menu][count]->index)
					return SubInv[menu][count]->Image;
		}	
	}
	
	return NULL;
}

geBitmap *CInventory::GetGraphic(int index, int menu)
{
		int count;
	
	if(menu == MAININV)
	{
		for(count=0;count<4; count++)
		{	
			if(Main[count]!=NULL)
				if(index==Main[count]->index)
					return Main[count]->Graphic;
		}
	}
	else
	{
		for(count=0;count<MAXENTRIES; count++)
		{	
			if(SubInv[menu][count]!=NULL)
				if(index==SubInv[menu][count]->index)
					return SubInv[menu][count]->Graphic;
		}	
	}
	
	return NULL;
}


int CInventory::GetAmount(int index, int menu)
{
	if(menu != MAININV)
	{	
		int count;
		
		CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
		
		for(count=0;count<MAXENTRIES; count++)
		{	
			if(SubInv[menu][count]!=NULL)
			{
				if(index==SubInv[menu][count]->index)
				{
					if(theInv->Value(SubInv[menu][count]->Attribute)==1 && theInv->High(SubInv[menu][count]->Attribute)==1)
						return 0;
					return theInv->Value(SubInv[menu][count]->Attribute);
				}
			}
		}
	}	
	
	return 0;
}

int CInventory::GetCatagory(int index, int menu)
{
	int count;
	
	if(menu == MAININV)
	{
		for(count=0;count<4; count++)
		{	
			if(Main[count]!=NULL)
				if(index==Main[count]->index)
					return Main[count]->catagory;
		}
	}
	else
	{
		for(count=0;count<MAXENTRIES; count++)
		{	
			if(SubInv[menu][count]!=NULL)
				if(index==SubInv[menu][count]->index)
					return SubInv[menu][count]->catagory;
		}	
	}
	
	return -1;
}

char *CInventory::GetAttribute(int index, int menu)
{
	int count;
	
	if(menu == MAININV)
	{
		for(count=0;count<4; count++)
		{	
			if(Main[count]!=NULL)
				if(index==Main[count]->index)
					return Main[count]->Attribute;
		}
	}
	else
	{
		for(count=0;count<MAXENTRIES; count++)
		{	
			if(SubInv[menu][count]!=NULL)
				if(index==SubInv[menu][count]->index)
					return SubInv[menu][count]->Attribute;
		}	
	}
	
	return "";
}

int CInventory::GetModifiedAmount(int index, int menu)
{
	int count;
	
	if(menu == MAININV)
	{
		for(count=0;count<4; count++)
		{	
			if(Main[count]!=NULL)
				if(index==Main[count]->index)
					return Main[count]->Amount;
		}
	}
	else
	{
		for(count=0;count<MAXENTRIES; count++)
		{	
			if(SubInv[menu][count]!=NULL)
				if(index==SubInv[menu][count]->index)
					return SubInv[menu][count]->Amount;
		}	
	}
	
	return 0;
}

bool CInventory::GetDecrease(int index, int menu)
{
	int count;
	
	if(menu == MAININV)
	{
		for(count=0;count<4; count++)
		{	
			if(Main[count]!=NULL)
				if(index==Main[count]->index)
					return Main[count]->Decrease;
		}
	}
	else
	{
		for(count=0;count<MAXENTRIES; count++)
		{	
			if(SubInv[menu][count]!=NULL)
				if(index==SubInv[menu][count]->index)
					return SubInv[menu][count]->Decrease;
		}	
	}
	
	return false;
}

char *CInventory::GetModified(int index, int menu)
{
	int count;
	
	if(menu == MAININV)
	{
		for(count=0;count<4; count++)
		{	
			if(Main[count]!=NULL)
				if(index==Main[count]->index)
					return Main[count]->Modify;
		}
	}
	else
	{
		for(count=0;count<MAXENTRIES; count++)
		{	
			if(SubInv[menu][count]!=NULL)
				if(index==SubInv[menu][count]->index)
					return SubInv[menu][count]->Modify;
		}	
	}
	
	return "";
}

char *CInventory::GetText(int index, int menu)
{
	int count;
	
	if(menu == MAININV)
	{
		for(count=0;count<4; count++)
		{	
			if(Main[count]!=NULL)
				if(index==Main[count]->index)
					return Main[count]->text;
		}
	}
	else
	{
		for(count=0;count<MAXENTRIES; count++)
		{	
			if(SubInv[menu][count]!=NULL)
				if(index==SubInv[menu][count]->index)
					return SubInv[menu][count]->text;
		}	
	}
	
	return "";
}


void CInventory::GenerateList(int menu)
{
	
	CPersistentAttributes *theInv = CCD->ActorManager()->Inventory(CCD->Player()->GetActor());
	
	int count, MaxEntries;
	
	for(count=0;count<MAXENTRIES;count++)
	{
		if(SubInv[menu][count]==NULL)
			break;
		if(theInv->Value(SubInv[menu][count]->Attribute)>0)
			SubInv[menu][count]->index=0;
		else
			SubInv[menu][count]->index=-1;
		
	}
	MaxEntries=0;
	for(count=0; count<MAXENTRIES; count++)
	{
		if(SubInv[menu][count]==NULL)
			break;
		if(SubInv[menu][count]->index!=-1)
		{
			SubInv[menu][count]->index=MaxEntries;
			MaxEntries++;
		}
	}
	
	if(menu == WEAPONINV)
		MaxWeapons = MaxEntries;
	else if(menu == ITEMINV)
		MaxItems = MaxEntries; 
	
}
/////////////////////////// end change