/****************************************************************************************/
/*																						*/
/*	CMessage.h:		Message class implementation										*/
/*																						*/
/*	(c) 2001 Ralph Deane																*/
/*	All Rights Reserved																	*/
/*																						*/
/*	This file contains the class declaration for Message								*/
/*	implementation.																		*/
/*																						*/
/*	Edit History:																		*/
/*	=============																		*/
/*	02/01/07 QD:	- replaced MFC, VC++ 2005 compatibility								*/
/*																						*/
/****************************************************************************************/

//	Include the One True Header
#include "RabidFramework.h"
#include <Ram.h>

extern geBitmap *TPool_Bitmap(char *DefaultBmp, char *DefaultAlpha, char *BName, char *AName);

/* ------------------------------------------------------------------------------------ */
//	Constructor
/* ------------------------------------------------------------------------------------ */
CMessage::CMessage()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	//	Ok, check to see if there are Messages in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "Message");

	if(!pSet)
		return;

	active = true;

	LoadText(CCD->MenuManager()->GetMessagetxts());

	AttrFile.SetPath("message.ini");
	if(!AttrFile.ReadFile())
	{
		active = false;
		return;
	}

	//	Ok, we have Messages somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Message *pSource = (Message*)geEntity_GetUserData(pEntity);

		if(EffectC_IsStringNull(pSource->szEntityName))
		{
			char szName[128];
			geEntity_GetName(pEntity, szName, 128);
			pSource->szEntityName = szName;
		}

		// Ok, put this entity into the Global Entity Registry
		CCD->EntityRegistry()->AddEntity(pSource->szEntityName, "Message");
		pSource->Data = NULL;

		if(!EffectC_IsStringNull(pSource->DisplayType))
		{
			MessageData *Data = new MessageData;

			// changed QD 07/15/06
			if(Data)
				memset(Data,0,sizeof(MessageData));
			else
				continue;
			// end change

			pSource->Data = (void*)Data;
			string KeyName = AttrFile.FindFirstKey();
			string Type;

			char szName[64], szAlpha[64];

			while(KeyName != "")
			{
				if(!strcmp(KeyName.c_str(), pSource->DisplayType))
				{
					Type = AttrFile.GetValue(KeyName, "type");

					if(Type == "")
					{
						delete pSource->Data;
						pSource->Data = NULL;
						break;
					}

					if(Type == "static")
					{
						Data->type = 0;
						Data->centerx = false;

						Type = AttrFile.GetValue(KeyName, "positionx");

						if(Type == "center")
							Data->centerx = true;

						Data->posx = AttrFile.GetValueI(KeyName, "positionx");
						Data->centery = false;

						Type = AttrFile.GetValue(KeyName, "positiony");

						if(Type == "center")
							Data->centery = true;

						Data->posy = AttrFile.GetValueI(KeyName, "positiony");
						Data->time = (float)AttrFile.GetValueF(KeyName, "displaytime");
						Data->fadeintime = (float)AttrFile.GetValueF(KeyName, "fadeintime");
						Data->fadeouttime = (float)AttrFile.GetValueF(KeyName, "fadeouttime");
						Data->font = AttrFile.GetValueI(KeyName, "fontsize");
						Data->graphic = NULL;

						Type = AttrFile.GetValue(KeyName, "graphic");

						if(Type != "")
						{
							Data->graphiccenterx = false;

							Type = AttrFile.GetValue(KeyName, "graphicpositionx");

							if(Type == "center")
								Data->graphiccenterx = true;

							Data->graphicposx = AttrFile.GetValueI(KeyName, "graphicpositionx");
							Data->graphiccentery = false;

							Type = AttrFile.GetValue(KeyName, "graphicpositiony");

							if(Type == "center")
								Data->graphiccentery = true;

							Data->graphicposy = AttrFile.GetValueI(KeyName, "graphicpositiony");
							Data->graphicstyle = AttrFile.GetValueI(KeyName, "graphicstyle");
							Data->graphicframes = AttrFile.GetValueI(KeyName, "graphicframes");

							if(Data->graphicframes == 0)
								Data->graphicframes = 1;

							Data->graphicspeed = AttrFile.GetValueI(KeyName, "graphicspeed");

							if(Data->graphicspeed == 0)
								Data->graphicspeed = 1;

							Data->graphicfadeintime = (float)AttrFile.GetValueF(KeyName, "graphicfadeintime");
							Data->graphicfadeouttime = (float)AttrFile.GetValueF(KeyName, "graphicfadeouttime");
							//Data->graphic = (geBitmap**)geRam_AllocateClear(sizeof(*(Data->graphic)) * Data->graphicframes);
							Data->graphic = (geBitmap**)geRam_AllocateClear(sizeof(geBitmap*)*(Data->graphicframes));

							if(Data->graphic)
							{
								Type = AttrFile.GetValue(KeyName, "graphic");

								strcpy(szName,Type.c_str());

								Type = AttrFile.GetValue(KeyName, "graphicalpha");

								if(Type == "")
									Type = AttrFile.GetValue(KeyName, "graphic");

								strcpy(szAlpha, Type.c_str());

								if(Data->graphicstyle == 0)
								{
									Data->graphic[0] = TPool_Bitmap(szName, szAlpha, NULL, NULL);
								}
								else
								{
									for(int i=0; i<Data->graphicframes; i++)
									{
										char BmpName[256];
										char AlphaName[256];
										// build bmp and alpha names
										sprintf(BmpName, "%s%d%s", szName, i, ".bmp");
										sprintf(AlphaName, "%s%d%s", szAlpha, i, ".bmp");
										Data->graphic[i] = TPool_Bitmap(BmpName, AlphaName, NULL, NULL);
									}
								}
							}
						}

						Data->active = false;
					}

					break;
				}

				KeyName = AttrFile.FindNextKey();
			}
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//	Destructor
/* ------------------------------------------------------------------------------------ */
CMessage::~CMessage()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	//	Ok, check to see if there are Messages in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "Message");

	if(!pSet)
		return;

	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Message *pSource = (Message*)geEntity_GetUserData(pEntity);

		if(pSource->Data)
		{
			MessageData *Data = (MessageData*)(pSource->Data);

			if(Data->graphic)
			{
				geRam_Free(Data->graphic);
				Data->graphic = NULL;
			}

			delete Data;
			pSource->Data = NULL;
		}
	}
}

/* ------------------------------------------------------------------------------------ */
//  Tick
/* ------------------------------------------------------------------------------------ */
void CMessage::Tick(float dwTicks)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	//	Ok, check to see if there are Messages in this world
	if(!active)
		return;

	pSet = geWorld_GetEntitySet(CCD->World(), "Message");

	if(!pSet)
		return;

	//	Ok, we have Messages somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Message *pSource = (Message*)geEntity_GetUserData(pEntity);
		MessageData *Data = (MessageData*)pSource->Data;

		if(!EffectC_IsStringNull(pSource->TriggerName))
		{
			if(GetTriggerState(pSource->TriggerName))
			{
				if(!(Data->active))
				{
					Data->ticks = 0.0f;
					Data->graphicticks = 0.0f;
					Data->active = true;
					Data->fadetime = Data->fadeintime;
					Data->fadein = 0;
					Data->alpha = 255.0f;

					if(Data->fadetime > 0.0f)
					{
						Data->fadein = 1;
						Data->alpha = 0.0f;
						Data->alphastep = 255.0f/Data->fadetime;
					}
					else
					{
						Data->fadetime = Data->fadeouttime;
						if(Data->fadetime > 0.0f)
						{
							Data->fadein = 2;
							Data->alpha = 255.0f;
							Data->alphastep = 255.0f/Data->fadetime;
						}
					}

					if(Data->graphic)
					{
						Data->graphiccur = 0;
						Data->graphicdir = 1;
						Data->graphicfadetime = Data->graphicfadeintime;
						Data->graphicfadein = 0;
						Data->graphicalpha = 255.0f;

						if(Data->graphicfadetime > 0.0f)
						{
							Data->graphicfadein = 1;
							Data->graphicalpha = 0.0f;
							Data->graphicalphastep = 255.0f/Data->graphicfadetime;
						}
						else
						{
							Data->graphicfadetime = Data->graphicfadeouttime;

							if(Data->graphicfadetime > 0.0f)
							{
								Data->graphicfadein = 2;
								Data->graphicalpha = 255.0f;
								Data->graphicalphastep = 255.0f/Data->graphicfadetime;
							}
						}
					}
				}
			}
		}

		if(Data->active)
		{
			Data->ticks += dwTicks;

			if((Data->ticks*0.001f) >= Data->time)
			{
				Data->active = false;
			}
			else
			{
				// fade in/out
				if(Data->fadein == 1)
				{
					if((Data->ticks*0.001f) <= Data->fadetime)
					{
						Data->alpha += (Data->alphastep*(dwTicks*0.001f));

						if(Data->alpha > 255.0f)
							Data->alpha = 255.0f;
					}
					else
					{
						Data->fadetime = Data->fadeouttime;
						Data->fadein = 0;
						Data->alpha = 255.0f;

						if(Data->fadetime > 0.0f)
						{
							Data->fadein = 2;
							Data->alpha = 255.0f;
							Data->alphastep = 255.0f/Data->fadetime;
						}
					}
				}
				else if(Data->fadein == 2)
				{
					if((Data->time-(Data->ticks*0.001f)) <= Data->fadetime)
					{
						Data->alpha -= (Data->alphastep*(dwTicks*0.001f));

						if(Data->alpha < 0.0f)
							Data->alpha = 0.0f;
					}
				}

				if(Data->graphic)
				{
					Data->graphicticks += dwTicks;

					if(Data->graphicfadein == 1)
					{
						if((Data->ticks*0.001f) <= Data->graphicfadetime)
						{
							Data->graphicalpha += (Data->graphicalphastep*(dwTicks*0.001f));

							if(Data->graphicalpha > 255.0f)
								Data->graphicalpha = 255.0f;
						}
						else
						{
							Data->graphicfadetime = Data->graphicfadeouttime;
							Data->graphicfadein = 0;
							Data->graphicalpha = 255.0f;

							if(Data->graphicfadetime > 0.0f)
							{
								Data->graphicfadein = 2;
								Data->graphicalpha = 255.0f;
								Data->graphicalphastep = 255.0f/Data->graphicfadetime;
							}
						}
					}
					else if(Data->graphicfadein == 2)
					{
						if((Data->time-(Data->ticks*0.001f)) <= Data->graphicfadetime)
						{
							Data->graphicalpha -= (Data->graphicalphastep*(dwTicks*0.001f));

							if(Data->graphicalpha < 0.0f)
								Data->graphicalpha = 0.0f;
						}
					}

					if(Data->graphicstyle != 0)
					{
						if(Data->graphicticks >= (1000.0f/Data->graphicspeed))
						{
							Data->graphicticks = 0.0f;

							switch(Data->graphicstyle)
							{
							case 1:
								Data->graphiccur += 1;
								if(Data->graphiccur >= Data->graphicframes)
									Data->graphiccur = 0;
								break;
							case 2:
								Data->graphiccur += Data->graphicdir;
								if(Data->graphiccur >= Data->graphicframes || Data->graphiccur < 0)
								{
									Data->graphicdir = -Data->graphicdir;
									Data->graphiccur += Data->graphicdir;
								}
								break;
							case 3:
								Data->graphiccur = (rand() % Data->graphicframes);
								break;
							case 4:
								Data->graphiccur +=1;
								if(Data->graphiccur >= Data->graphicframes)
									Data->graphiccur = Data->graphicframes-1;
								break;
							}
						}
					}
				}
			}
		}
	}

	return;
}

/* ------------------------------------------------------------------------------------ */
//  Display
/* ------------------------------------------------------------------------------------ */
void CMessage::Display()
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	if(!active)
		return;

	//	Ok, check to see if there are Messages in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "Message");

	if(!pSet)
		return;

	//	Ok, we have Messages somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Message *pSource = (Message*)geEntity_GetUserData(pEntity);
		MessageData *Data = (MessageData *)pSource->Data;

		if(Data->active)
		{
			if(!EffectC_IsStringNull(pSource->TextName))
			{
				string Textt = GetText(pSource->TextName);

				// static type message
				if(Data->type == 0)
				{
					int posx, posy;

					if(Data->graphic)
					{
						posx = Data->graphicposx;
						posy = Data->graphicposy;

						if(posx < 0)
							posx = CCD->Engine()->Width() + posx;

						if(posy < 0)
							posy = CCD->Engine()->Height() + posy;

						if(Data->graphiccenterx)
							posx = (CCD->Engine()->Width() - geBitmap_Width(Data->graphic[Data->graphiccur]))/2;

						if(Data->graphiccentery)
							posy = (CCD->Engine()->Height() - geBitmap_Height(Data->graphic[Data->graphiccur]))/2;

						CCD->Engine()->DrawBitmap(Data->graphic[Data->graphiccur], NULL, posx, posy, Data->graphicalpha, 1.0f);
					}

					// changed Nout/QD 12/15/05
					//char szText[256];
					//strcpy(szText, Textt);
					char *szText = NULL;
					// end change

					posx = Data->posx;
					posy = Data->posy;

					if(posx < 0)
						posx = CCD->Engine()->Width() + posx;

					if(posy < 0)
						posy = CCD->Engine()->Height() + posy;

					// changed Nout/QD 12/15/05
					if(Data->centery)
						posy = (CCD->Engine()->Height() - CCD->MenuManager()->FontHeight(Data->font))/2;

					while(1)
					{
						int Index = Textt.find((char)1);

						if(Index == -1) // no newline indicator ( <CR> ) found
						{
							szText = new char[Textt.length()+1];
							strcpy(szText, Textt.c_str());

							if(Data->centerx)
								posx = (CCD->Engine()->Width() - CCD->MenuManager()->FontWidth(Data->font, szText))/2;

							CCD->MenuManager()->WorldFontRect(szText, Data->font, posx, posy, Data->alpha);

							delete szText;
							break;
						}
						else
						{
							szText = new char[Textt.length()+1];
							strcpy(szText, Textt.substr(0, Index).c_str());

							if(Data->centerx)
								posx = (CCD->Engine()->Width() - CCD->MenuManager()->FontWidth(Data->font, szText))/2;

							CCD->MenuManager()->WorldFontRect(szText, Data->font, posx, posy, Data->alpha);
							delete szText;
							posy += CCD->MenuManager()->FontHeight(Data->font) + 2;
							Textt = Textt.substr(Index+1);
						}
					}
					// end change
				}
			}
		}
	}

	return;
}


//	******************** CRGF Overrides ********************

/* ------------------------------------------------------------------------------------ */
//	LocateEntity
//
//	Given a name, locate the desired item in the currently loaded level
//	..and return it's user data.
/* ------------------------------------------------------------------------------------ */
int CMessage::LocateEntity(char *szName, void **pEntityData)
{
	geEntity_EntitySet *pSet;
	geEntity *pEntity;

	//	Ok, check to see if there are Messages in this world
	pSet = geWorld_GetEntitySet(CCD->World(), "Message");

	if(!pSet)
		return RGF_NOT_FOUND;									// No 3D audio sources

	//	Ok, we have Messages somewhere.  Dig through 'em all.
	for(pEntity=geEntity_EntitySetGetNextEntity(pSet, NULL); pEntity;
		pEntity=geEntity_EntitySetGetNextEntity(pSet, pEntity))
	{
		Message *pSource = (Message*)geEntity_GetUserData(pEntity);

		if(!strcmp(pSource->szEntityName, szName))
		{
			*pEntityData = (void*)pSource;
			return RGF_SUCCESS;
		}
	}

	return RGF_NOT_FOUND;								// Sorry, no such entity here
}

/* ------------------------------------------------------------------------------------ */
//	ReSynchronize
//
//	Correct internal timing to match current time, to make up for time lost
//	..when outside the game loop (typically in "menu mode").
/* ------------------------------------------------------------------------------------ */
int CMessage::ReSynchronize()
{
	return RGF_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
//	LoadText
/* ------------------------------------------------------------------------------------ */
// changed QD Language Menu
void CMessage::LoadText(char *messagetxt)
{
	geVFile *MainFS;
	char szInputLine[256];
	string readinfo, keyname, text;

	Text.resize(0);

// changed QD Language Menu
	if(!CCD->OpenRFFile(&MainFS, kInstallFile, messagetxt, GE_VFILE_OPEN_READONLY))
		return;

	keyname = "";
	text = "";

	while(geVFile_GetS(MainFS, szInputLine, 256) == GE_TRUE)
	{
		if(strlen(szInputLine) <= 1)
			readinfo = "";
		else
			readinfo = szInputLine;

		if(readinfo != "" && readinfo[0] != ';')
		{
			TrimRight(readinfo);

// changed QD 07/15/06 - if trim operation results in empty string...
			if(readinfo.length()<=1)
				continue;
// end change QD 07/15/06

			if(readinfo[0] == '[' && readinfo[readinfo.length()-1] == ']')
			{
				if(keyname != "" && text != "")
				{
					Text.resize(Text.size()+1);
					int keynum = Text.size()-1;
					Text[keynum].Name = keyname;
					Text[keynum].Text = text;
// changed QD 12/15/05
					Replace(Text[keynum].Text, "<Player>", CCD->Player()->GetPlayerName());
// end change
				}

				keyname = readinfo;
				TrimLeft(keyname, "[");
				TrimRight(keyname, "]");
				text = "";
			}
			else
			{
				if(readinfo == "<CR>")
				{
					text += " ";
					text[text.length()-1] = (char)1;
				}
				else
				{
					if(text != "" && text[text.length()-1] != 1)
						text += " ";

					text += readinfo;
				}
			}
		}
	}

	if(keyname != "" && text != "")
	{
		Text.resize(Text.size()+1);
		int keynum = Text.size()-1;
		Text[keynum].Name = keyname;
		Text[keynum].Text = text;
// changed QD 12/15/05
		Replace(Text[keynum].Text, "<Player>", CCD->Player()->GetPlayerName());
// end change
	}

	geVFile_Close(MainFS);
}

/* ------------------------------------------------------------------------------------ */
//	GetText
/* ------------------------------------------------------------------------------------ */
string CMessage::GetText(char *Name)
{
	int size = Text.size();

	if(size < 1)
		return "";

	for(int i=0; i<size; i++)
	{
		if(Text[i].Name == Name)
			return Text[i].Text;
	}

	return "";
}


/* ----------------------------------- END OF FILE ------------------------------------ */
