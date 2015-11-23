

int FindFont(char *szArg)
{
	int index;
	if(!stricmp(szArg,"FONT1"))
		index = FONT1;
	else if(!stricmp(szArg,"FONT2"))
		  index = FONT2;
	else if(!stricmp(szArg,"FONT3"))
		  index = FONT3;
	else if(!stricmp(szArg,"FONT4"))
		  index = FONT4;
	else if(!stricmp(szArg,"FONT5"))
		  index = FONT5;
	else if(!stricmp(szArg,"FONT6"))
		  index = FONT6;
	else if(!stricmp(szArg,"FONT7"))
		  index = FONT7;
	else if(!stricmp(szArg,"FONT8"))
		  index = FONT8;
	else if(!stricmp(szArg,"FONT9"))
		  index = FONT9;
	else if(!stricmp(szArg,"FONT10"))
		  index = FONT10;
	else if(!stricmp(szArg,"FONT11"))
		  index = FONT11;
	else if(!stricmp(szArg,"FONT12"))
		  index = FONT12;
	else if(!stricmp(szArg,"FONT13"))
		  index = FONT13;
	else if(!stricmp(szArg,"FONT14"))
		  index = FONT14;
	else if(!stricmp(szArg,"FONT15"))
		  index = FONT15;
	else if(!stricmp(szArg,"FONT16"))
		  index = FONT16;
	else if(!stricmp(szArg,"FONT17"))
		  index = FONT17;
	else if(!stricmp(szArg,"FONT18"))
		  index = FONT18;
	else if(!stricmp(szArg,"FONT19"))
		  index = FONT19;
	else if(!stricmp(szArg,"FONT20"))
		  index = FONT20;
	else if(!stricmp(szArg,"FONT21"))
		  index = FONT21;
	else if(!stricmp(szArg,"FONT22"))
		  index = FONT22;
	else if(!stricmp(szArg,"FONT23"))
		  index = FONT23;
	else if(!stricmp(szArg,"FONT24"))
		  index = FONT24;
	else if(!stricmp(szArg,"FONT25"))
		  index = FONT25;
	else if(!stricmp(szArg,"FONT26"))
		  index = FONT26;
	else if(!stricmp(szArg,"FONT27"))
		  index = FONT27;
	else if(!stricmp(szArg,"FONT28"))
		  index = FONT28;
	else if(!stricmp(szArg,"FONT29"))
		  index = FONT29;
	else
	{
		char szBug[256];
		sprintf(szBug, "Bad font size name %s", szArg);
		CCD->ReportError(szBug, false);
		exit(-100);
	}
	return index;
}




	CIniFile AttrFile(CCD->MenuIni();
	if(!AttrFile.ReadFile())
	{
		CCD->ReportError("Can't open menu initialization file", false);
		exit(-1);
	}
	CString KeyName = AttrFile.FindFirstKey();
	CString Type;
	while(KeyName != "")
	{
		if(KeyName=="Background")  
		{
			for(int i=0;i<NUM_BACKGROUNDS; i++)
			{
				char szText[256];
				sprintf(szText, 'background%d",i);
				Type = AttrFile.GetValue(KeyName, szText);
				if(Type!="")
				{
					strcpy(menuline,"menu\\");
					strcat(menuline, Type);
					geBitmap *TB = CreateFromFileName(menuline);
					if(TB == (geBitmap *)NULL)
					{
						char szBug[256];
						sprintf(szBug, "Bad file name %s", menuline);
						CCD->ReportError(szBug, false);
						exit(-100);
					}
					geBitmap_GetInfo(TB, &BmpInfo, NULL);
					int TBw = BmpInfo.Width;
					int TBh = BmpInfo.Height;
					int TBx = (BmpInfo.Width - CCD->Engine()->Width()) / 2;
					if(TBx<0)
						TBx = 0;
					else
						TBw = CCD->Engine()->Width();
					int TBy = (BmpInfo.Height - CCD->Engine()->Height()) / 2;
					if(TBy<0)
						TBy = 0;
					else
						TBh = CCD->Engine()->Height();
					Backgrounds[i] = geBitmap_Create(TBw, TBh, BmpInfo.MaximumMip+1, BmpInfo.Format);
					geBitmap_Blit(TB, TBx, TBy, Backgrounds[index], 0, 0, TBw, TBh); 
					geBitmap_Destroy(&TB);
					if(Backgrounds[i] == (geBitmap *)NULL)
					{
						char szBug[256];
						sprintf(szBug, "Bad file name %s", menuline);
						CCD->ReportError(szBug, false);
						exit(-100);
					}
					geEngine_AddBitmap(CCD->Engine()->Engine(), Backgrounds[i]);
				}
			}
		}
		if(KeyName=="Image")  
		{
			for(int i=0;i<NUM_IMAGES; i++)
			{
				char szText[256];
				sprintf(szText, 'image%d",i);
				Type = AttrFile.GetValue(KeyName, szText);
				if(Type!="")
				{
					char strip[256], *temp;
					strcpy(strip,Type);
					temp = strtok(strip," \n");
					strcpy(menuline,"menu\\");
					strcat(menuline,temp);
					temp = strtok(NULL," \n");
					strcpy(menuline2,"menu\\");
					strcat(menuline2,temp);
					Images[i] = CreateFromFileAndAlphaNames(menuline, menuline2);
					if(Images[i] == (geBitmap *)NULL)
					{
						char szBug[256];
						sprintf(szBug, "Bad file name %s", menuline);
						CCD->ReportError(szBug, false);
						exit(-100);
					}
					geEngine_AddBitmap(CCD->Engine()->Engine(), Images[i]);
				}
			}
		}
		if(KeyName=="Title")  
		{
			for(int i=0;i<NUM_TITLES; i++)
			{
				char szText[256];
				sprintf(szText, 'title%d",i);
				Type = AttrFile.GetValue(KeyName, szText);
				if(Type!="")
				{
					char strip[256], *temp;
					strcpy(strip,Type);
					temp = strtok(strip," \n");
					strcpy(menuline,"menu\\");
					strcat(menuline,temp);
					temp = strtok(NULL," \n");
					strcpy(menuline2,"menu\\");
					strcat(menuline2,temp);
					Titles[i] = CreateFromFileAndAlphaNames(menuline, menuline2);
					if(Titles[i] == (geBitmap *)NULL)
					{
						char szBug[256];
						sprintf(szBug, "Bad file name %s", menuline);
						CCD->ReportError(szBug, false);
						exit(-100);
					}
					geEngine_AddBitmap(CCD->Engine()->Engine(), Titles[i]);
				}
			}
		}
		if(KeyName=="Font")  
		{
			for(int i=1;i<=NUM_FONTS; i++)
			{
				char szText[256];
				sprintf(szText, 'FONT%d",i);
				Type = AttrFile.GetValue(KeyName, szText);
				if(Type!="")
				{
					char fontline[132];
					strcpy(fontline,Type);
					strcpy(menuline,"fonts\\");
					strcat(menuline,fontline);
					strcat(menuline,".bmp");
					strcpy(menuline2,"fonts\\a_");
					strcat(menuline2,fontline);
					strcat(menuline2,".bmp");
					MenuFont[i-1].Bitmap = CreateFromFileAndAlphaNames(menuline, menuline2);
					if(MenuFont[i-1].Bitmap == (geBitmap *)NULL)
					{
						char szBug[256];
						sprintf(szBug, "Bad file name %s", fontline);
						CCD->ReportError(szBug, false);
						exit(-100);
					}
					geEngine_AddBitmap(CCD->Engine()->Engine(), MenuFont[i-1].Bitmap);
					
					strcpy(menuline,"fonts\\");
					strcat(menuline,fontline);
					strcat(menuline,".dat");
					geVFile *datFile;
					CCD->OpenRFFile(&datFile, kBitmapFile, menuline, GE_VFILE_OPEN_READONLY);
					geVFile_Read(datFile,&MenuFont[i-1].font_height,sizeof(int));
					for(int c=0;c<96;c++)
					{
						geVFile_Read(datFile,&MenuFont[i-1].dat[c].width,sizeof(int));
						geVFile_Read(datFile,&MenuFont[i-1].dat[c].x,sizeof(int));
						geVFile_Read(datFile,&MenuFont[i-1].dat[c].y,sizeof(int));
					}
					geVFile_Close(datFile);;
				}
			}
		}
		if(KeyName=="Animation")  
		{
			for(int i=1;i<=NUM_ANIM; i++)
			{
				char szText[256];
				sprintf(szText, 'animation%d",i);
				Type = AttrFile.GetValue(KeyName, szText);
				if(Type!="")
				{
					strcpy(menuline,"menu\\");
					strcat(menuline, Type);
					Animation[i] = new CAnimGif(menuline, kVideoFile);
				}
			}
		}
		if(KeyName=="Cursor")  
		{
			Type = AttrFile.GetValue(KeyName, "bitmap");
			strcpy(menuline,"menu\\");
			strcat(menuline,Type);
			Type = AttrFile.GetValue(KeyName, "alphamap");
			strcpy(menuline2,"menu\\");
			strcat(menuline2,Type);
			Cursor = CreateFromFileAndAlphaNames(menuline, menuline2);
			if(Cursor == (geBitmap *)NULL)
			{
				char szBug[256];
				sprintf(szBug, "Bad file name %s", menuline);
				CCD->ReportError(szBug, false);
				exit(-100);
			}
			geEngine_AddBitmap(CCD->Engine()->Engine(), Cursor);
			AnimCursor = AttrFile.GetValueI(KeyName, "animation");
		}
		if(KeyName=="Crosshair")
		{
			Type = AttrFile.GetValue(KeyName, "bitmap");
			strcpy(menuline,"menu\\");
			strcat(menuline,Type);
			Type = AttrFile.GetValue(KeyName, "alphamap");
			strcpy(menuline2,"menu\\");
			strcat(menuline2,Type);
			Crosshair = CreateFromFileAndAlphaNames(menuline, menuline2);
			if(Crosshair == (geBitmap *)NULL)
			{
				char szBug[256];
				sprintf(szBug, "Bad file name %s", menuline);
				CCD->ReportError(szBug, false);
				exit(-100);
			}
			FCrosshair = CreateFromFileAndAlphaNames(menuline, menuline2);
			geEngine_AddBitmap(CCD->Engine()->Engine(), FCrosshair);
		}
		if(KeyName=="Sound")
		{
			Type = AttrFile.GetValue(KeyName, "mouseclick");
			char file[256] = "menu\\";
			strcat(file, Type);
			CCD->OpenRFFile(&MainFS, kAudioFile, file, GE_VFILE_OPEN_READONLY);
			mouseclick = geSound_LoadSoundDef(CCD->Engine()->AudioSystem(), MainFS);
			geVFile_Close(MainFS);
			Type = AttrFile.GetValue(KeyName, "keyclick");
			strcat(file, Type);
			CCD->OpenRFFile(&MainFS, kAudioFile, file, GE_VFILE_OPEN_READONLY);
			keyclick = geSound_LoadSoundDef(CCD->Engine()->AudioSystem(), MainFS);
			geVFile_Close(MainFS);
			Type = AttrFile.GetValue(KeyName, "slideclick");
			strcat(file, Type);
			CCD->OpenRFFile(&MainFS, kAudioFile, file, GE_VFILE_OPEN_READONLY);
			slideclick = geSound_LoadSoundDef(CCD->Engine()->AudioSystem(), MainFS);
			geVFile_Close(MainFS);
			Type = AttrFile.GetValue(KeyName, "music");
			char musicname[256];
			musictype = -1;
			strcpy(musicname, Type);
			int len = strlen(musicname)-4;
			if(stricmp((musicname+len),".mid")==0)
			{
				strcpy(music, musicname);
				if(CCD->MIDIPlayer())
				{
					musictype = 1;
				}
			}
			else
			{
				strcpy(music, CCD->GetDirectory(kAudioStreamFile));
				strcat(music, "\\");
				strcat(music, musicname);
				m_dsPtr = (LPDIRECTSOUND)geSound_GetDSound();
				m_Streams = new StreamingAudio(m_dsPtr);
				if(m_Streams)
				{
					if(m_Streams->Create(music))
					{
						musictype = 0;
					}
				}
			}
		}
		if(KeyName=="LoadMsg")
		{
			Type = AttrFile.GetValue(KeyName, "font");
			strcpy(menuline,Type);
			LoadFont = FindFont(menuline);
			Type = AttrFile.GetValue(KeyName, "text");
			if(Type!="")
				strcpy(Loadmsg, Type);
		}
		if(KeyName=="SaveMsg")
		{
			Type = AttrFile.GetValue(KeyName, "font");
			strcpy(menuline,Type);
			SaveFont = FindFont(menuline);
			Type = AttrFile.GetValue(KeyName, "text");
			if(Type!="")
				strcpy(Savemsg, Type);
			SavingTime = (float)AttrFile.GetValueF(KeyName, "time");
		}
		if(KeyName=="MainTitle")
		{
			int index =0;
			MTitles[index].Image_Number = AttrFile.GetValueI(KeyName, "titleimage");
			MTitles[index].X = AttrFile.GetValueI(KeyName, "screenx");
			MTitles[index].Y = AttrFile.GetValueI(KeyName, "screeny");
			MTitles[index].Width = AttrFile.GetValueI(KeyName, "width");
			MTitles[index].Height = AttrFile.GetValueI(KeyName, "height");
			MTitles[index].Image_X = AttrFile.GetValueI(KeyName, "imagex");
			MTitles[index].Image_Y = AttrFile.GetValueI(KeyName, "imagey");
			MTitles[index].Animation = AttrFile.GetValueI(KeyName, "animation");
		}
		if(KeyName=="MultiPlayerTitle")
		{
			int index =1;
			MTitles[index].Image_Number = AttrFile.GetValueI(KeyName, "titleimage");
			MTitles[index].X = AttrFile.GetValueI(KeyName, "screenx");
			MTitles[index].Y = AttrFile.GetValueI(KeyName, "screeny");
			MTitles[index].Width = AttrFile.GetValueI(KeyName, "width");
			MTitles[index].Height = AttrFile.GetValueI(KeyName, "height");
			MTitles[index].Image_X = AttrFile.GetValueI(KeyName, "imagex");
			MTitles[index].Image_Y = AttrFile.GetValueI(KeyName, "imagey");
			MTitles[index].Animation = AttrFile.GetValueI(KeyName, "animation");
		}
		if(KeyName=="LoadGameTitle")
		{
			int index =2;
			MTitles[index].Image_Number = AttrFile.GetValueI(KeyName, "titleimage");
			MTitles[index].X = AttrFile.GetValueI(KeyName, "screenx");
			MTitles[index].Y = AttrFile.GetValueI(KeyName, "screeny");
			MTitles[index].Width = AttrFile.GetValueI(KeyName, "width");
			MTitles[index].Height = AttrFile.GetValueI(KeyName, "height");
			MTitles[index].Image_X = AttrFile.GetValueI(KeyName, "imagex");
			MTitles[index].Image_Y = AttrFile.GetValueI(KeyName, "imagey");
			MTitles[index].Animation = AttrFile.GetValueI(KeyName, "animation");
		}
		if(KeyName=="SaveGameTitle")
		{
			int index =3;
			MTitles[index].Image_Number = AttrFile.GetValueI(KeyName, "titleimage");
			MTitles[index].X = AttrFile.GetValueI(KeyName, "screenx");
			MTitles[index].Y = AttrFile.GetValueI(KeyName, "screeny");
			MTitles[index].Width = AttrFile.GetValueI(KeyName, "width");
			MTitles[index].Height = AttrFile.GetValueI(KeyName, "height");
			MTitles[index].Image_X = AttrFile.GetValueI(KeyName, "imagex");
			MTitles[index].Image_Y = AttrFile.GetValueI(KeyName, "imagey");
			MTitles[index].Animation = AttrFile.GetValueI(KeyName, "animation");
		}
		if(KeyName=="OptionsTitle")
		{
			int index =4;
			MTitles[index].Image_Number = AttrFile.GetValueI(KeyName, "titleimage");
			MTitles[index].X = AttrFile.GetValueI(KeyName, "screenx");
			MTitles[index].Y = AttrFile.GetValueI(KeyName, "screeny");
			MTitles[index].Width = AttrFile.GetValueI(KeyName, "width");
			MTitles[index].Height = AttrFile.GetValueI(KeyName, "height");
			MTitles[index].Image_X = AttrFile.GetValueI(KeyName, "imagex");
			MTitles[index].Image_Y = AttrFile.GetValueI(KeyName, "imagey");
			MTitles[index].Animation = AttrFile.GetValueI(KeyName, "animation");
		}
		if(KeyName=="CreditTitle")
		{
			int index =5;
			MTitles[index].Image_Number = AttrFile.GetValueI(KeyName, "titleimage");
			MTitles[index].X = AttrFile.GetValueI(KeyName, "screenx");
			MTitles[index].Y = AttrFile.GetValueI(KeyName, "screeny");
			MTitles[index].Width = AttrFile.GetValueI(KeyName, "width");
			MTitles[index].Height = AttrFile.GetValueI(KeyName, "height");
			MTitles[index].Image_X = AttrFile.GetValueI(KeyName, "imagex");
			MTitles[index].Image_Y = AttrFile.GetValueI(KeyName, "imagey");
			MTitles[index].Animation = AttrFile.GetValueI(KeyName, "animation");
		}
		if(KeyName=="AudioTitle")
		{
			int index =6;
			MTitles[index].Image_Number = AttrFile.GetValueI(KeyName, "titleimage");
			MTitles[index].X = AttrFile.GetValueI(KeyName, "screenx");
			MTitles[index].Y = AttrFile.GetValueI(KeyName, "screeny");
			MTitles[index].Width = AttrFile.GetValueI(KeyName, "width");
			MTitles[index].Height = AttrFile.GetValueI(KeyName, "height");
			MTitles[index].Image_X = AttrFile.GetValueI(KeyName, "imagex");
			MTitles[index].Image_Y = AttrFile.GetValueI(KeyName, "imagey");
			MTitles[index].Animation = AttrFile.GetValueI(KeyName, "animation");
		}
		if(KeyName=="VideoTitle")
		{
			int index =7;
			MTitles[index].Image_Number = AttrFile.GetValueI(KeyName, "titleimage");
			MTitles[index].X = AttrFile.GetValueI(KeyName, "screenx");
			MTitles[index].Y = AttrFile.GetValueI(KeyName, "screeny");
			MTitles[index].Width = AttrFile.GetValueI(KeyName, "width");
			MTitles[index].Height = AttrFile.GetValueI(KeyName, "height");
			MTitles[index].Image_X = AttrFile.GetValueI(KeyName, "imagex");
			MTitles[index].Image_Y = AttrFile.GetValueI(KeyName, "imagey");
			MTitles[index].Animation = AttrFile.GetValueI(KeyName, "animation");
		}
		if(KeyName=="ControlTitle")
		{
			int index =8;
			MTitles[index].Image_Number = AttrFile.GetValueI(KeyName, "titleimage");
			MTitles[index].X = AttrFile.GetValueI(KeyName, "screenx");
			MTitles[index].Y = AttrFile.GetValueI(KeyName, "screeny");
			MTitles[index].Width = AttrFile.GetValueI(KeyName, "width");
			MTitles[index].Height = AttrFile.GetValueI(KeyName, "height");
			MTitles[index].Image_X = AttrFile.GetValueI(KeyName, "imagex");
			MTitles[index].Image_Y = AttrFile.GetValueI(KeyName, "imagey");
			MTitles[index].Animation = AttrFile.GetValueI(KeyName, "animation");
		}
		if(KeyName=="DebugTitle")
		{
			int index =9;
			MTitles[index].Image_Number = AttrFile.GetValueI(KeyName, "titleimage");
			MTitles[index].X = AttrFile.GetValueI(KeyName, "screenx");
			MTitles[index].Y = AttrFile.GetValueI(KeyName, "screeny");
			MTitles[index].Width = AttrFile.GetValueI(KeyName, "width");
			MTitles[index].Height = AttrFile.GetValueI(KeyName, "height");
			MTitles[index].Image_X = AttrFile.GetValueI(KeyName, "imagex");
			MTitles[index].Image_Y = AttrFile.GetValueI(KeyName, "imagey");
			MTitles[index].Animation = AttrFile.GetValueI(KeyName, "animation");
		}
		if(KeyName=="AdvancedTitle")
		{
			int index =10;
			MTitles[index].Image_Number = AttrFile.GetValueI(KeyName, "titleimage");
			MTitles[index].X = AttrFile.GetValueI(KeyName, "screenx");
			MTitles[index].Y = AttrFile.GetValueI(KeyName, "screeny");
			MTitles[index].Width = AttrFile.GetValueI(KeyName, "width");
			MTitles[index].Height = AttrFile.GetValueI(KeyName, "height");
			MTitles[index].Image_X = AttrFile.GetValueI(KeyName, "imagex");
			MTitles[index].Image_Y = AttrFile.GetValueI(KeyName, "imagey");
			MTitles[index].Animation = AttrFile.GetValueI(KeyName, "animation");
		}
		if(KeyName=="SelectionTitle")
		{
			int index =11;
			MTitles[index].Image_Number = AttrFile.GetValueI(KeyName, "titleimage");
			MTitles[index].X = AttrFile.GetValueI(KeyName, "screenx");
			MTitles[index].Y = AttrFile.GetValueI(KeyName, "screeny");
			MTitles[index].Width = AttrFile.GetValueI(KeyName, "width");
			MTitles[index].Height = AttrFile.GetValueI(KeyName, "height");
			MTitles[index].Image_X = AttrFile.GetValueI(KeyName, "imagex");
			MTitles[index].Image_Y = AttrFile.GetValueI(KeyName, "imagey");
			MTitles[index].Animation = AttrFile.GetValueI(KeyName, "animation");
		}
		if(KeyName=="Main")
		{
			MainBack = AttrFile.GetValueI(KeyName, "background");
			MainTitle = 0;
		}
		if(KeyName=="NewGame")
		{
			MainMenu[0].X = AttrFile.GetValueI(KeyName, "screenx");
			MainMenu[0].Y = AttrFile.GetValueI(KeyName, "screeny");
			NewGame.Image_Number = AttrFile.GetValueI(KeyName, "buttonimage");
			NewGame.Width = AttrFile.GetValueI(KeyName, "width");
			NewGame.Height = AttrFile.GetValueI(KeyName, "height");
			NewGame.Image_X = AttrFile.GetValueI(KeyName, "imagex");
			NewGame.Image_Y = AttrFile.GetValueI(KeyName, "imagey");
			NewGame.MImage_Number = AttrFile.GetValueI(KeyName, "mouseoverimage");
			NewGame.Mover_X = AttrFile.GetValueI(KeyName, "mouseoverx");
			NewGame.Mover_Y = AttrFile.GetValueI(KeyName, "mouseovery");
			NewGame.Animation = AttrFile.GetValueI(KeyName, "normalanimation");
			NewGame.AnimationOver = AttrFile.GetValueI(KeyName, "mouseoveranimation");
		}
			
		KeyName = AttrFile.FindNextKey(); 
		
	}
